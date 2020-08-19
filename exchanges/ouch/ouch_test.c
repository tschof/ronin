/* #include "util_macros.h" */
#include "ouch_util.h"

#include "databuf.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <assert.h>

#define jump_assert(cond, label)                \
    if (!(cond)) { goto label; }


#pragma pack(push, 1)           /* Required because OUCH breaks
                                 * alignment starting at
                                 * minimum_quantity. */
struct ouch_enter_order_message {
    char type;
    char order_token[14];
    char buy_sell_indicator;
    uint32_t shares;
    char stock[8];
    uint32_t price;
    uint32_t time_in_force;
    char firm[4];
    char display;
    char capacity;
    char intermarket_sweep_eligibility;
    uint32_t minimum_quantity;
    char cross_type;
};
#pragma pack(pop)

void ouch_enter_order_message_initialize(struct ouch_enter_order_message
                                         *order)
{
    memset(order, ' ', sizeof *order);
    order->type = 'O';
    order->shares = 0;
    order->price = 0x7fffffff;  /* Default to market order. */
    order->time_in_force = 0;   /* Default to IOC. */
    order->minimum_quantity = 0;
}

void ouch_enter_order_to_text(struct ouch_enter_order_message *order,
                              FILE * fd)
{
    fprintf(fd, "              Type  %c\n", order->type);
    fprintf(fd, "       Order Token  '%.*s'\n",
            (int) sizeof order->order_token, order->order_token);
    fprintf(fd, "Buy/Sell Indicator  %c\n", order->buy_sell_indicator);
    fprintf(fd, "            Shares  %u\n", order->shares);
    fprintf(fd, "             Stock  '%.*s'\n", (int) sizeof order->stock,
            order->stock);
    fprintf(fd, "             Price  %u\n", order->price);
    fprintf(fd, "     Time in Force  %u\n", order->time_in_force);
    fprintf(fd, "              Firm  '%.*s'\n", (int) sizeof order->firm,
            order->firm);
    fprintf(fd, "           Display  %c\n", order->display);
    fprintf(fd, "          Capacity  %c\n", order->capacity);
    fprintf(fd, "   ISO Eligibility  %c\n",
            order->intermarket_sweep_eligibility);
    fprintf(fd, "  Minimum Quantity  %u\n", order->minimum_quantity);
    fprintf(fd, "        Cross Type  %c\n", order->cross_type);
}

void ouch_enter_order_to_rom_text(struct ouch_enter_order_message *order,
                                  FILE * fd)
{
    fprintf(fd, "              Type  %c\n", order->type);
    fprintf(fd, "       Order Token  '%.*s'\n",
            (int) sizeof order->order_token, order->order_token);
    fprintf(fd, "Buy/Sell Indicator  %c\n", order->buy_sell_indicator);
    fprintf(fd, "            Shares  %u\n", order->shares);
    fprintf(fd, "             Stock  '%.*s'\n", (int) sizeof order->stock,
            order->stock);
    fprintf(fd, "             Price  %u\n", order->price);
    fprintf(fd, "     Time in Force  %u\n", order->time_in_force);
    fprintf(fd, "              Firm  '%.*s'\n", (int) sizeof order->firm,
            order->firm);
    fprintf(fd, "           Display  %c\n", order->display);
    fprintf(fd, "          Capacity  %c\n", order->capacity);
    fprintf(fd, "   ISO Eligibility  %c\n",
            order->intermarket_sweep_eligibility);
    fprintf(fd, "  Minimum Quantity  %u\n", order->minimum_quantity);
    fprintf(fd, "        Cross Type  %c\n", order->cross_type);
}

void ouch_enter_order_message_serialize(struct ouch_enter_order_message
                                        *order, void **buffer,
                                        size_t nbytes)
{
    struct ouch_enter_order_message *serialized_message =
        malloc(sizeof *serialized_message);
    order->shares = htonl(order->shares);
    order->price = htonl(order->price);
    order->time_in_force = htonl(order->time_in_force);
    order->minimum_quantity = htonl(order->minimum_quantity);
}

int ouch_generate_order_token(char *tokendest, size_t tokenlen)
{
    memcpy(tokendest, "foobar", 6);
    return 0;
}

/* Given a pointer to a _valid_ ROM CSV buffer, a delimiter pointer,
 * and an integer to keep the current field index, advance to the
 * desired field index as specified in the last integer argument. The
 * desired index _must_ be equal to or greater than the current index
 * otherwise the behavior is undefined and guaranteed to be rather
 * icky. The delimiter pointer _must_ be check after this to see if
 * the end of message or buffer has been reached. */
#define csv_adv_to_field(csvptr, delimptr, fieldvar, goto_field) \
    for (delimptr = strpbrk(csvptr, ",\n");                 \
         delim != 0 && *delim != '\n' && fieldvar < goto_field;  \
         csvptr = delimptr + 1, ++fieldvar,                 \
             delimptr = strpbrk(csvptr, ",\n"))

#define csv_field_len(csvptr)                   \
    ({ char *p = strpbrk(csvptr, ",\n"); p != 0 ? p - csvptr : 0; })

/* Using our standards of end of message ('\n') and end of buffer
 * ('\0') goto the label specified if either of these ends have been
 * reached. */
#define delim_jump_on_msg_end(delimptr, label)      \
    if ((delimptr) == 0 || *(delimptr) == '\n') {   \
        goto label;                                 \
    } else {                                        \
        ++(delimptr);                               \
    }

#define max_share_chars 7

#define OUCH_TIF_IOC 0
#define OUCH_TIF_MARKET_HOURS 99998
#define OUCH_TIF_SYSTEM_HOURS 99999

int ouch_enter_order(const char *romcsv, size_t csvlen,
                     struct ouch_enter_order_message *order)
{
    int result = -1;
    ouch_enter_order_message_initialize(order);
    if (ouch_generate_order_token
        (order->order_token, sizeof order->order_token) == -1) {
        goto done;
    }
    size_t nfield = 0, len;
    char *delim;

    /* Side */
    csv_adv_to_field(romcsv, delim, nfield, 4);
    delim_jump_on_msg_end(delim, done);
    jump_assert((len = csv_field_len(romcsv)) == 1, done);
    switch (*romcsv) {
    case '1':
        order->buy_sell_indicator = 'B';
        break;
    case '2':
        order->buy_sell_indicator = 'S';
        break;
    case '5':
        order->buy_sell_indicator = 'T';
        break;
    case '6':
        order->buy_sell_indicator = 'E';
        break;
    default:
        goto done;
    }

    /* Symbol */
    csv_adv_to_field(romcsv, delim, nfield, 5);
    delim_jump_on_msg_end(delim, done);
    jump_assert((len =
                 csv_field_len(romcsv)) <= sizeof order->stock, done);
    memcpy(order->stock, romcsv, len);

    /* Shares */
    csv_adv_to_field(romcsv, delim, nfield, 6);
    delim_jump_on_msg_end(delim, done);
    char *endptr;
    order->shares = strtol(romcsv, &endptr, 10);
    jump_assert(endptr > romcsv, done);
    jump_assert(order->shares > 0, done);

    /* Price */
    csv_adv_to_field(romcsv, delim, nfield, 7);
    delim_jump_on_msg_end(delim, done);
    /* jump_assert(ouch_price_string_to_int(romcsv, &(order->price)) != -1, */
    /*             done); */

    /* Order Type */
    csv_adv_to_field(romcsv, delim, nfield, 8);
    delim_jump_on_msg_end(delim, done);
    jump_assert(csv_field_len(romcsv) == 1
                && (romcsv[0] == '1' || romcsv[0] == '2'), done);
    /* No need to handle the limit price based on the order type. We
     * only accept market or limit. If a limit price is specified we
     * fill that in the limit price field, otherwise it defaults to
     * market. */

    /* Time in Force */
    csv_adv_to_field(romcsv, delim, nfield, 9);
    delim_jump_on_msg_end(delim, done);
    jump_assert(csv_field_len(romcsv) == 1, done);
    if (romcsv[0] == '1') {
        order->time_in_force = OUCH_TIF_MARKET_HOURS;
    } else if (romcsv[0] != '3') {
        goto done;
    }

  done:
    return result;
}

int main(int argc, char *argv[])
{
    uint32_t priceint;
    assert(ouch_price_string_to_int("0.56", &priceint) != -1);
    assert(ouch_price_string_to_int("1", &priceint) != -1);
    assert(ouch_price_string_to_int("1234.56,", &priceint) != -1);
    assert(priceint == 12345600);
    /* const char *romcsv = */
    /*     "E,095206,SIMPG,SIMPGTEST5,1,JPG,100,13.21,2,0,A,501,AC8E1209,7,,,,SIMPG2-180120322-2,,,N,,SIMPG,GSEC,SIMPG,,,SIMPG2,SIMPG2-180120322-2-0,,,,,,,E,,,,,,7,,,,,,,,,,,,,,JPG,,,,,,,,,,,,,,,,1,,,,,,"; */
    /* struct ouch_enter_order_message enter_order; */
    /* assert(ouch_enter_order(romcsv, strlen(romcsv), &enter_order) != -1); */
    databuf_t *dbuff = databuf_alloc(256);
    assert(ouch_price_int_to_string(priceint, dbuff) != -1);
    return 0;
}
