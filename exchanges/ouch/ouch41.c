#include "ouch41.h"
#include "soupbin.h"
#include "ouch_util.h"
#include "macros.h"
#include "dart_to_ouch.h"
#include "server_objects/dart_constants.h"

#include "server_objects/dart_order_obj.h"
#include "server_objects/dart_constants.h"
#include "server_objects/tm_to_str.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>
#include <endian.h>
#include <time.h>

static const uint64_t ns_per_second = 1000000000llu;

struct ouch__timestamp_converter {
    struct tm today, tomorrow;
    int seconds_from_utc;
};

int ouch__timestamp_converter__init(struct ouch__timestamp_converter
                                    *converter)
{
    time_t t = time(0);
    if (gmtime_r(&t, &converter->today) == 0)
        return -1;
    t += 24 * 60 * 60;          /* Set it to this time tomorrow. */
    if (gmtime_r(&t, &converter->tomorrow) == 0)
        return -1;
    time_t local;
    time(&local);
    struct tm lm;
    localtime_r(&local, &lm);
    if (lm.tm_isdst)
        converter->seconds_from_utc = 4 * 60 * 60;
    else
        converter->seconds_from_utc = 5 * 60 * 60;
    return 0;
}

/* OUCH sends timestamps as a binary integer representing the number
 * of nanoseconds since midnight, US Eastern Time.  To send back to
 * the DART system we need a timestamp string representing the time in
 * UTC.  Please note that this uses static variables as well as
 * modifying this static state and therefore can *only* be used from a
 * single thread.  For OUCH we only process timestamps from OUCH and
 * there is only one thread processing OUCH messages at a time so it
 * is okay, but only in this exact scenario. */
int ouch__timestamp_to_string(uint64_t ouch_timestamp, char *buffer,
                              size_t buffer_length)
{
    if (buffer_length < TIMESTAMP_LEN)
        return -1;
    time_t seconds_from_midnight = ouch_timestamp / ns_per_second;
    static struct ouch__timestamp_converter time_converter;
    static int is_initialized = 0;
    if (is_initialized == 0) {
        if (ouch__timestamp_converter__init(&time_converter) == -1)
            return -1;
        is_initialized = 1;
    }
    seconds_from_midnight += time_converter.seconds_from_utc;
    time_converter.today.tm_hour = seconds_from_midnight / 3600;
    seconds_from_midnight -= (time_converter.today.tm_hour * 3600);
    struct tm *ptm = &time_converter.today;
    if (ptm->tm_hour > 23) {
        time_converter.tomorrow.tm_hour = ptm->tm_hour % 24;
        ptm = &time_converter.tomorrow;
    }
    ptm->tm_min = seconds_from_midnight / 60;
    seconds_from_midnight %= 60;
    ptm->tm_sec = seconds_from_midnight;
    return tm_to_str(ptm, buffer, buffer_length);
}

int ouch__convert_minqty(struct dart_order_obj *csv, uint32_t * minqty)
{
    int n, retval = 0;
    char *p = get_rom_field(csv, ROM_MIN_QTY, &n);
    if (n > 0) {
        char *endptr;
        errno = 0;
        long int tmp = strtol(p, &endptr, 10);
        if (errno == 0 && endptr > p) {
            *minqty = tmp;
        } else {
            retval = -1;
        }
    }
    return retval;
}

int ouch__translate_cross_type(struct dart_order_obj *csv,
                               char *cross_type)
{
    int retval = 0, n;
    /* We will represent the cross type in the instructions field */
    char *p = get_rom_field(csv, ROM_INSTR, &n);
    if (n == 1 && p != 0) {
        switch (p[0]) {
        case 'O':
        case 'C':
        case 'N':
        case 'H':
        case 'S':
            *cross_type = p[0];
        }
    } else if(n == 6 && p != 0) {
      switch (p[5]) {
          case 'O':
          case 'C':
              *cross_type = p[5];
      } 
    }
    return retval;
}

int ouch41__csv_to_ouch_enter_order(struct dart_order_obj *csv,
                                    struct ouch41__enter_order *order,
                                    const char **errmsg)
{
    int retval = -1, n;
    *errmsg = "";
    ouch41__enter_order__init(order);
    char *p = get_rom_field(csv, ROM_SIDE, &n);
    jump_on_fail(dart_to_ouch__convert_side
                 (p, n, &order->buy_sell_indicator)
                 == 0, done, *errmsg, "invalid side");
    p = get_rom_field(csv, ROM_SYM, &n);
    jump_on_fail(n > 0, done, *errmsg, "no symbol");
    memcpy(&order->stock, p, n);
    p = get_rom_field(csv, ROM_SHARES, &n);
    jump_on_fail(n > 0, done, *errmsg, "no shares");
    jumpassert(dart_to_ouch__convert_shares(p, n, &order->shares, errmsg)
               == 0, done);
    p = get_rom_field(csv, ROM_MIN_QTY, &n);
    if (n > 0) {
        jumpassert(dart_to_ouch__convert_shares
                   (p, n, &order->minimum_quantity, errmsg) == 0, done);
    }
    /* Add bin qty to the DART CSV message */
    set_rom_field(csv, ROM_BIN_QTY, &order->shares, sizeof order->shares);
    uint32_t cum_qty = 0;
    set_rom_field(csv, ROM_BIN_CUM, &cum_qty, sizeof cum_qty);
    p = get_rom_field(csv, ROM_PRICE, &n);
    jump_on_fail(ouch_price_string_to_int(p, &order->price) != -1, done,
                 *errmsg, "bad price");
    p = get_rom_field(csv, ROM_TYPE, &n);
    jump_on_fail(n >= 1 && (p[0] == '1' || p[0] == '2' || p[0] == '5'), done, *errmsg,
                 "market or limit orders only");
    /* OUCH has no order type field, just a special value for the
     * price to indicate a market order. */
    if (n == 1 && (p[0] == '1' || p[0] == '5'))
        order->price = OUCH_PRICE_MARKET;
    p = get_rom_field(csv, ROM_TIF, &n);
    jump_on_fail(n == 1 && (p[0] == '0' || p[0] == '2' || p[0] == '3'),
                 done, *errmsg, "unsupported tif");
    if (p[0] == '0')            /* DAY */
        order->time_in_force = OUCH_TIF_DAY;
    else if (p[0] == '2') {     /* OPG */
        order->time_in_force = OUCH_TIF_MARKET_HOURS;
        order->cross_type = 'O';        /* Opening cross */
    } else if (p[0] == '3') {   /* IOC */
        order->time_in_force = 0;
    }
    p = get_rom_field(csv, ROM_DISPLAY_INSTRUCTIONS, &n);
    if (n > 0) {
        jump_on_fail(n == 1, done, *errmsg,
                     "invalid display instructions");
        order->display = p[0];
    } else {
        /* Defaulting to Anonymous-Price to Comply */
        order->display = 'Y';
    }

    /* Non-displayed orders can be either specified by setting the 'N'
       into the ROM Display Instructions field or by setting MaxFloor
       to 0. */
    p = get_rom_field(csv, ROM_MAX_FLOOR, &n);
    if (n > 0) {
        char *endptr;
        errno = 0;
        if (strtol(p, &endptr, 10) == 0 && errno == 0 && endptr > p) {
            order->display = 'N';
        }
    }

    p = get_rom_field(csv, ROM_CAP, &n);
    jump_on_fail(n != 0, done, *errmsg, "Capacity not specified");
    jump_on_fail(n == 1, done, *errmsg, "Invalid capacity");
    order->capacity = p[0];
    order->intermarket_sweep_eligibility = 'N';

    order->cross_type = 'N';    /* No cross eligibile */

    /* We will represent the cross type in the instructions field */
    jump_on_fail(ouch__translate_cross_type(csv, &order->cross_type) == 0,
                 done, *errmsg, "invalid cross type");
    jump_on_fail(ouch__convert_minqty(csv, &order->minimum_quantity) == 0,
                 done, *errmsg, "invalid minqty");
    retval = 0;
  done:
    return retval;
}

const char *get_replaceable(struct dart_order_obj
                            *incoming_csv, struct dart_order_obj
                            *book_order, int dart_csv_field, int *length)
{
    char *p = get_rom_field(incoming_csv, dart_csv_field, length);
    if (*length == 0)
        p = get_rom_field(book_order, dart_csv_field, length);
    return p;
}

int text_to_uint32(const char *p, size_t n, uint32_t * num)
{
    int retval = -1;
    char *endptr;
    errno = 0;
    *num = strtol(p, &endptr, 10);
    if (endptr > p && errno != 0)
        retval = 0;
    return retval;
}

int dart_csv__to__ouch41__replace_order(struct dart_order_obj
                                        *incoming_csv,
                                        struct dart_order_obj *book_order,
                                        struct ouch41__replace_order
                                        *replace, const char **errmsg)
{
    int retval = -1, n;
    const char *p =
        get_replaceable(incoming_csv, book_order, ROM_SHARES, &n);
    jump_on_fail(n > 0, done, *errmsg, "Replace shares missing");
    jumpassert(dart_to_ouch__convert_shares(p, n, &replace->shares, errmsg)
               == 0, done);
    p = get_replaceable(incoming_csv, book_order, ROM_PRICE, &n);
    jump_on_fail(ouch_price_string_to_int(p, &replace->price) != -1, done,
                 *errmsg, "Replace price missing");
    p = get_replaceable(incoming_csv, book_order, ROM_TIF, &n);
    jump_on_fail(dart_to_ouch__convert_tif(p[0], &replace->time_in_force)
                 == 0, done, *errmsg, "Invalid TIF");
    replace->display = 'N';     /* Non-display */
    replace->intermarket_sweep_eligibility = 'N';
    p = get_replaceable(incoming_csv, book_order, ROM_MIN_QTY, &n);
    if (n == 0) {
        replace->minimum_quantity = 0;
    } else {
        jump_on_fail(text_to_uint32(p, n, &replace->minimum_quantity) == 0,
                     done, *errmsg, "invalid minimum quantity");
    }
    retval = 0;
  done:
    return retval;
}

int ouch41__reject_reason_code_to_string(struct databuf *str, char c)
{
    static const char *rejectcodes[] = {
        "A:Unknown",
        "B:Unknown",
        "C:NASDAQ is closed",
        "D:Invalid display type",
        "E:Unknown",
        "F:Unknown",
        "G:Unknown",
        "H:Trading is halted",
        "I:Unknown",
        "J:Unknown",
        "K:Unknown",
        "L:Firm not authorized for clearing type",
        "M:Outside of permitted clearing times",
        "N:Invalid minimum quantity",
        "O:Other",
        "P:Unknown",
        "Q:Unknown",
        "R:Not allowed in this type of cross",
        "S:Invalid stock",
        "T:Test Mode",
        "U:Unknown",
        "V:Unknown",
        "W:Unknown",
        "X:Invalid price",
        "Y:Unknown",
        "Z:Shares exceed safety threshold",
        "[:Unknown",
        "\\:Unknown",
        "]:Unknown",
        "^:Unknown",
        "_:Unknown",
        "`:Unknown",
        "a:Reject all enabled",
        "b:Easy to borrow reject",
        "c:Restricted symbol list reject",
        "d:ISO order restriction",
        "e:Odd lot order restriction",
        "f:Mid-point order restriction",
        "g:Pre-market order restriction",
        "h:Post market order restriction",
        "i:Short sale order restriction",
        "j:On open order restriction",
        "k:On close order restriction",
        "l:Two-sided quote reject",
        "m:Exceeded shares limit",
        "n:Exceeded dollar value limit",
    };
    if (isprint(c) == 0)
        return databuf_write(str, "%x", (int) c);
    else if ((c < 'A')
             || (c > ('A' + (sizeof rejectcodes / sizeof rejectcodes[0]))))
        return databuf_write(str, "%c", c);
    else
        return databuf_write(str, "%s", rejectcodes[c - 'A']);
}

int ouch41__cancel_reason_code_to_string(struct databuf *str, char c)
{
    static const char *rejectcodes[] = {
        "A:Unknown",
        "B:Unknown",
        "C:Unknown",
        "D:Order cannot be canceled because of a regulatory restriction",
        "E:Unknown",
        "F:Unknown",
        "G:Unknown",
        "H:Unknown",
        "I:Immediate or cancel order",
        "J:Unknown",
        "K:Unknown",
        "L:Unknown",
        "M:Unknown",
        "N:Unknown",
        "O:Unknown",
        "P:Unknown",
        "Q:Canceled due to self match prevention",
        "R:Unknown",
        "S:Supervisory cancel",
        "T:Order timed out",
        "U:Canceled by user request",
        "V:Unknown",
        "W:Unknown",
        "X:Unknown",
        "Y:Unknown",
        "Z:Unknown",
    };
    if (isprint(c) == 0)
        return databuf_write(str, "Unknown cancel reason: '%x'", (int) c);
    else if ((c < 'A')
             || (c > ('A' + (sizeof rejectcodes / sizeof rejectcodes[0]))))
        return databuf_write(str, "Cancel reason: '%c'", c);
    else
        return databuf_write(str, "%s", rejectcodes[c - 'A']);
}

/* OUCH 4.1 Serialization Functions */


/* Serialize */

#define ouch41__message_begin(structure, typecode)                      \
    int ouch41__##structure##__serialize(struct ouch41__##structure *message, struct databuf *buffer) \
    {
#define ouch41__msg_element_char(field)
#define ouch41__msg_element_uint32(field)       \
    message->field = htobe32(message->field);
#define ouch41__msg_element_uint64(field)       \
    message->field = htobe64(message->field);
#define ouch41__msg_element_char_array(field, arity)
#define ouch41__message_end                                     \
    return databuf_memcpy(buffer, message, sizeof *message);    \
    }
#include "ouch41__message_definitions.h"
#undef ouch41__message_begin
#undef ouch41__msg_element_char
#undef ouch41__msg_element_uint32
#undef ouch41__msg_element_uint64
#undef ouch41__msg_element_char_array
#undef ouch41__message_end


/* Deserialize */

#define ouch41__message_begin(structure, typecode)  \
    void ouch41__##structure##__deserialize(struct ouch41__##structure *message) \
    {
#define ouch41__msg_element_char(field)
#define ouch41__msg_element_uint32(field) message->field = be32toh(message->field);
#define ouch41__msg_element_uint64(field) message->field = be64toh(message->field);
#define ouch41__msg_element_char_array(field, arity)
#define ouch41__message_end                     \
    }
#include "ouch41__message_definitions.h"
#undef ouch41__message_begin
#undef ouch41__msg_element_char
#undef ouch41__msg_element_uint32
#undef ouch41__msg_element_uint64
#undef ouch41__msg_element_char_array
#undef ouch41__message_end


#define ouch41__message_begin(structure, typecode)              \
    case typecode:                                              \
    jumpassert(n == sizeof(struct ouch41__##structure), done);  \
    ouch41__##structure##__deserialize                          \
    ((struct ouch41__##structure*) p);                          \
    retval = 0;                                                 \
    break;
#define ouch41__msg_element_char(field)
#define ouch41__msg_element_uint32(field)
#define ouch41__msg_element_uint64(field)
#define ouch41__msg_element_char_array(field, arity)
#define ouch41__message_end

int ouch41__deserialize(void *p, size_t n)
{
    int retval = -1;
    /* The first byte of a valid OUCH 4.1 message is the type. */
    if (n == 0)
        return retval;
    char type = ((char *) p)[0];
    switch (type) {
#include "ouch41__message_definitions.h"
    }
  done:
    return retval;
}

#undef ouch41__message_begin
#undef ouch41__msg_element_char
#undef ouch41__msg_element_uint32
#undef ouch41__msg_element_uint64
#undef ouch41__msg_element_char_array
#undef ouch41__message_end


#define ouch41__message_begin(structure, typecode)              \
    case typecode:                                              \
    jumpassert(n == sizeof(struct ouch41__##structure), done);  \
    retval = ouch41__##structure##__serialize                   \
        ((struct ouch41__##structure*) p, buf);                 \
    break;
#define ouch41__msg_element_char(field)
#define ouch41__msg_element_uint32(field)
#define ouch41__msg_element_uint64(field)
#define ouch41__msg_element_char_array(field, arity)
#define ouch41__message_end

int ouch41__serialize(void *p, size_t n, struct databuf *buf)
{
    int retval = -1;
    /* The first byte of a valid OUCH 4.1 message is the type. */
    if (n == 0)
        return retval;
    char type = ((char *) p)[0];
    switch (type) {
#include "ouch41__message_definitions.h"
    }
  done:
    return retval;
}

#undef ouch41__message_begin
#undef ouch41__msg_element_char
#undef ouch41__msg_element_uint32
#undef ouch41__msg_element_uint64
#undef ouch41__msg_element_char_array
#undef ouch41__message_end


#define ouch41__message_begin(name, typecode)                           \
    int ouch41__##name##__to_string(struct ouch41__##name* obj, struct databuf *output) \
    {                                                                   \
    struct ouch41__##name temp = *obj;                                  \
    ouch41__##name##__deserialize(&temp);
#define ouch41__msg_element_char(name)                          \
    if (databuf_write(output, #name"=%c;", temp.name) == -1) {  \
        return -1;                                              \
    }
#define ouch41__msg_element_uint32(name)                        \
    if (databuf_write(output, #name"=%u;", temp.name) == -1) {  \
        return -1;                                              \
    }
#define ouch41__msg_element_uint64(name)                        \
    if (databuf_write(output, #name"=%lu;", temp.name) == -1) { \
        return -1;                                              \
    }
#define ouch41__msg_element_char_array(name, arity)                     \
    if (databuf_write(output, #name"=%.*s;", (int) sizeof temp.name,    \
                      temp.name) == -1) {                               \
        return -1;                                                      \
    }
#define ouch41__message_end                     \
    return 0;                                   \
    }
#include "ouch41__message_definitions.h"
#undef ouch41__message_begin
#undef ouch41__msg_element_char
#undef ouch41__msg_element_uint32
#undef ouch41__msg_element_uint64
#undef ouch41__msg_element_char_array
#undef ouch41__message_end

#define ouch41__message_begin(structure, typecode)                      \
    case typecode:                                                      \
    jumpassert(n >= sizeof(struct ouch41__##structure), done);          \
    retval = ouch41__##structure##__to_string                           \
        ((struct ouch41__##structure*) p, buffer);                      \
    break;
#define ouch41__msg_element_char(name)
#define ouch41__msg_element_uint32(name)
#define ouch41__msg_element_uint64(name)
#define ouch41__msg_element_char_array(name, arity)
#define ouch41__message_end


int ouch41__dump_message(void *p, size_t n, struct databuf *buffer)
{
    int retval = -1;
    jumpassert(n > 0, done);
    char type = ((char *) p)[0];
    switch (type) {
#include "ouch41__message_definitions.h"
    }
  done:
    return retval;
}

#undef ouch41__message_begin
#undef ouch41__msg_element_char
#undef ouch41__msg_element_uint32
#undef ouch41__msg_element_uint64
#undef ouch41__msg_element_char_array
#undef ouch41__message_end

#define ouch41__message_begin(structure, typecode)                      \
    void ouch41__##structure##__init(struct ouch41__##structure *message) \
    {                                                                   \
    char type = typecode;
#define ouch41__msg_element_char(field)         \
    message->field = ' ';
#define ouch41__msg_element_uint32(field)       \
    message->field = 0;
#define ouch41__msg_element_uint64(field)       \
    message->field = 0;
#define ouch41__msg_element_char_array(field, arity)            \
    memset(&message->field, ' ', arity);
#define ouch41__message_end                     \
    message->type = type;                       \
    }
#include "ouch41__message_definitions.h"
#undef ouch41__message_begin
#undef ouch41__msg_element_char
#undef ouch41__msg_element_uint32
#undef ouch41__msg_element_uint64
#undef ouch41__msg_element_char_array
#undef ouch41__message_end
