#include "server_objects/rom_handler.h"
#include "server_objects/dart_constants.h"
#include "server_objects/dart_orderbook.h"
#include "server_objects/debug_logger.h"
#include "ex_common/ex_common.h"
#include "ex_common/ex_config_helper.h"
#include "ex_db/db_layer.h"
#include "smtp/mailer.h"
#include "rash_exchange_interface.h"
#include "conf_util.h"
#include "transaction_recorder_tools.h"
#include "rashmsg.h"
#include "soupmsg.h"
#include <string.h>
#include <errno.h>
#include <ctype.h>

struct error_report {
    char *message;
    int free;
};

#define declare_error_report(name) struct error_report name = { 0, 0 }
#define destroy_error_report(name) do { if (name.free != 0) free(name.message); } while (0);

static const char sides[] = { 'B', 'S', 0, 0, 'T', 'E' };

static int nbcvtsidefd(const char *dartval, size_t dartlen, char *nbval,
                       size_t nblen)
{
    int result = -1;
    if (dartlen == 1 && nblen == 1) {
        int c = dartval[0];
        if (dartval[0] > '0' && dartval[0] < '7') {
            c -= '1';
            if (sides[c] != 0) {
                nbval[0] = sides[c];
                result = 0;
            }
        }
    }
    return result;
}

#define RASH_PRICE_INITIALIZER {'0','0','0','0','0','0','0','0','0','0'}
#define RASH_NUMERIC_INITIALIZER {'0','0','0','0','0','0'}

static const struct rash_EnterOrder enter_order_defaults = {
    .type = 'Q',
    .order_token =
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
         ' '},
    .side = ' ',
    .shares = {'0', '0', '0', '0', '0', '0'},
    .symbol = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    .price = RASH_PRICE_INITIALIZER,
    .time_in_force = {'0', '0', '0', '0', '0'},
    .firm = {' ', ' ', ' ', ' '},
    .display = 'Y',
    .min_qty = RASH_NUMERIC_INITIALIZER,
    .max_floor = RASH_NUMERIC_INITIALIZER,
    .peg_type = 'N',
    .peg_difference_sign = '+',
    .peg_difference = RASH_PRICE_INITIALIZER,
    .discretion_price = RASH_PRICE_INITIALIZER,
    .discretion_peg_type = 'N',
    .discretion_peg_difference_sign = '+',
    .discretion_peg_difference = RASH_PRICE_INITIALIZER,
    .capacity = ' ',
    .random_reserve = RASH_NUMERIC_INITIALIZER,
    .route_dest = {' ', ' ', ' ', ' '},
    .cust =
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
         ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
         ' ', ' ', ' ', ' ', ' ', ' '},
    .inter_market_sweep_eligibility = 'N',
    .cross_type = 'N',
};

static const struct rash_CancelOrder cancelOrderDefaults = {
    .type = 'X',
    .orderToken =
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
         ' '},
    .shares = {'0', '0', '0', '0', '0', '0'},
};

static int setOrdTypeMarket(struct dart_order_obj *csvmsg,
                            struct rash_EnterOrder *eo,
                            struct error_report *err)
{
    eo->peg_type = 'P';
    memset(&eo->price, '0', sizeof eo->price);
    int tifLength;
    char *tif = get_rom_field(csvmsg, ROM_TIF, &tifLength);
    if (tifLength == 0) {
        err->message = "Time in force is required";
        return -1;
    }
    eo->cross_type = 'N';
    if (tifLength == 1 && tif[0] == '2') {      /* MOO */
        memset(&eo->time_in_force, '0', sizeof eo->time_in_force);
        eo->cross_type = 'O';
    } else if (tifLength == 1 && tif[0] == '3') {       /* IOC */
        memset(&eo->time_in_force, '0', sizeof eo->time_in_force);
    } else if (tifLength == 1 && tif[0] == '0') {       /* Day */
        memset(&eo->time_in_force, '9', sizeof eo->time_in_force);
    } else {
        err->message = "Invalid time in force";
        return -1;
    }
    return 0;
}

static int rash_setLimitPrice(struct dart_order_obj *csvmsg, char *p,
                              struct error_report *err)
{
    int dartPriceLength;
    char *dartPrice = get_rom_field(csvmsg, ROM_PRICE, &dartPriceLength);
    if (dartPriceLength == 0) {
        err->message = "No limit price";
        return -1;
    }
    if (rash_put_price(dartPrice, dartPriceLength, p) == -1) {
        err->message = "Error processing limit price";
        return -1;
    }
    return 0;
}

static int setOrdTypeLimit(struct dart_order_obj *csvmsg,
                           struct rash_EnterOrder *eo,
                           struct error_report *err)
{
    err->free = 0;
    eo->peg_type = 'N';
    eo->cross_type = 'N';

    /* Handle our price */
    if (rash_setLimitPrice(csvmsg, (char *) &eo->price, err) == -1) {
        return -1;
    }

    /* Set the TIF */
    int tifLength;
    char *tif = get_rom_field(csvmsg, ROM_TIF, &tifLength);
    if (tifLength == 0) {
        err->message = "No Time in Force specified";
        return -1;
    }
    eo->cross_type = 'N';
    if (tifLength == 1 && tif[0] == '2') {      /* MOO */
        memset(&eo->time_in_force, '0', sizeof eo->time_in_force);
        eo->cross_type = 'O';
    } else if (tifLength == 1 && tif[0] == '3') {       /* IOC */
        memset(&eo->time_in_force, '0', sizeof eo->time_in_force);
    } else if (tifLength == 1 && tif[0] == '0') {       /* Day */
        memset(&eo->time_in_force, '9', sizeof eo->time_in_force);
    } else {
        err->message = "Invalid time in force";
        return -1;
    }
    return 0;
}

static int setOrdTypeMOC(struct dart_order_obj *csvmsg,
                         struct rash_EnterOrder *eo,
                         struct error_report *err)
{
    memset(&eo->price, '0', sizeof eo->price);
    memset(&eo->time_in_force, '0', sizeof eo->time_in_force);
    eo->peg_type = 'P';
    eo->cross_type = 'C';
    int tifLength;
    char *tif = get_rom_field(csvmsg, ROM_TIF, &tifLength);
    if (tifLength != 1 || (tif[0] != '3' && tif[0] != '0')) {
        /* Limit on close should be IOC orders. */
        err->message = "Invalid TIF for MOC order";
        return -1;
    }
    return 0;
}

static int setOrdTypeLOC(struct dart_order_obj *csvmsg,
                         struct rash_EnterOrder *eo,
                         struct error_report *err)
{
    eo->peg_type = 'N';
    eo->cross_type = 'C';
    if (rash_setLimitPrice(csvmsg, (char *) &eo->price, err) == -1) {
        return -1;
    }
    int tifLength;
    char *tif = get_rom_field(csvmsg, ROM_TIF, &tifLength);
    if (tifLength != 1 || (tif[0] != '3' && tif[0] != '0')) {
        /* Limit on close should be IOC orders. */
        err->message = "Invalid TIF for LOC order";
        return -1;
    }
    return 0;
}

static int setOrdTypePeg(struct dart_order_obj *csvmsg, char pegType,
                         struct rash_EnterOrder *eo,
                         struct error_report *err)
{
    if (pegType != 'P' && pegType != 'R' && pegType != 'M') {
        err->message = "Unsupported execution instruction";
        return -1;
    }
    eo->peg_type = pegType;
    if (pegType == 'M') {
        eo->display = 'N';
    }
    /* Check the offset */
    int offsetLength;
    char *offset = get_rom_field(csvmsg, ROM_PEGPRICE_OFFSET,
                                 &offsetLength);
    if (offsetLength > 0) {
        /* Determine the sign */
        char *end;
        for (end = offset + offsetLength;
             offset != end && isdigit(*offset) == 0; ++offset) {
            if (*offset == '-') {
                eo->peg_difference_sign = '-';
            } else if (*offset == '+') {
                eo->peg_difference_sign = '+';
            } else if (isspace(*offset) == 0) {
                err->message = "Bad peg offset";
                return -1;
            }
        }
        if (rash_put_price(offset, end - offset,
                           (char *) &eo->peg_difference) == -1) {
            err->message = "Bad peg offset";
            return -1;
        }
    }

    /* Set the TIF */
    int tifLength;
    char *tif = get_rom_field(csvmsg, ROM_TIF, &tifLength);
    if (tifLength == 0) {
        err->message = "No Time in Force specified";
        return -1;
    }
    eo->cross_type = 'N';
    if (tifLength == 1 && tif[0] == '0') {
        memset(&eo->time_in_force, '9', sizeof eo->time_in_force);
    } else if (tifLength == 1 && tif[0] == '3') {
        memset(&eo->time_in_force, '0', sizeof eo->time_in_force);
    } else {
        err->message = "Invalid time in force";
        return -1;
    }

    return 0;
}

static void dart_order_obj__init(dart_order_obj * o)
{
    set_rom_field(o, 0, "E", 1);
    size_t i;
    for (i = 0; i < ROM_LEN; ++i) {
        o->positions[i].beg = o->positions[i].end =
            o->positions[i].val_len = 0;
        o->positions[i].name = 0;
        o->positions[i].len = 0;
    }
    o->rep = 0;
}

static void rash_databaseInit(struct database_conf *dbconf,
                              const char *destID, orderbook * ob)
{
    int res = 0;
    element_t e, temp;
    queue_t q = create_queue();
    e = NULL;
    temp = NULL;
    res = get_db_objects(dbconf, q, destID);
    e = dequeue(q);
    while (e != NULL) {
        struct dart_ex_result_set *ders = 0;
        dart_order_obj *obj = (dart_order_obj *) get_obj(ob->ord_obj);
        dart_order_obj__init(obj);
        ders = (struct dart_ex_result_set *) e->d;
        if (ders != NULL) {
            set_rom_field(obj, ROM_SHARES, &ders->qty, 4);
            int l_len = strlen(ders->orderTag);
            set_rom_field(obj, ROM_CLORDID, ders->orderTag, l_len);
            no_copy_insert(ob->cl_ord_id_map, ders->orderTag, l_len, obj);
            l_len = strlen(ders->omTag);
            set_rom_field(obj, ROM_TAG, ders->omTag, l_len);
            no_copy_insert(ob->rom_ord_id_map, ders->omTag, l_len, obj);
            size_t rom_inst_len = 0;
            char *rom_inst = build_rom_name(ders->omTag, 32,
                                            &rom_inst_len);
            if (rom_inst != NULL) {
                set_rom_field(obj, ROM_SENDER, rom_inst, rom_inst_len);
                free(rom_inst);
            }
            set_rom_field(obj, ROM_BIN_CUM, &ders->cumQty,
                          sizeof ders->qty);
            set_rom_field(obj, ROM_BIN_QTY, &ders->qty, sizeof ders->qty);

            switch (ders->side) {
            case 1:
                set_rom_field(obj, ROM_SIDE, "1", 1);
                break;
            case 2:
                set_rom_field(obj, ROM_SIDE, "2", 1);
                break;
            case 5:
                set_rom_field(obj, ROM_SIDE, "5", 1);
                break;
            }
            int sym_len = strlen(ders->symbol);
            set_rom_field(obj, ROM_SYM, ders->symbol, sym_len);
            int account_len = strlen(ders->clearingAccount);
            set_rom_field(obj, ROM_CLR_ACC, ders->clearingAccount,
                          account_len);
            double d = ders->price;
            d = d * 10000;
            d += .0000001;
            int setter = (int) d;
            set_rom_field(obj, ROM_BIN_PRICE, &setter, sizeof(int));
            switch (ders->orderType) {
            case 1:
                set_rom_field(obj, ROM_TYPE, "1", 1);
                break;
            case 2:
                set_rom_field(obj, ROM_TYPE, "2", 1);
                break;
            case 3:
                set_rom_field(obj, ROM_TYPE, "3", 1);
                break;
            case 5:
                set_rom_field(obj, ROM_TYPE, "5", 1);
                break;
            case 11:
                set_rom_field(obj, ROM_TYPE, "B", 1);
                break;
            }
        }
        temp = e;
        e = temp->next;
        if (ders != NULL) {
            free(ders);
        }
        temp->d = NULL;
        free(temp);
        temp = NULL;
    }
    destroy_queue(q);
}

#define MAXSENDFAIL 40

/* Returns non-zero if all bytes are sent, zero if not */
static int rash_sendMessage(pthread_mutex_t * mutex, char *p, size_t n,
                            char *name, int name_len)
{
    char *end = p + n;
    int nfail = 0, nwritten;
    if (pthread_mutex_lock(mutex) == 0) {
        do {
            if ((nwritten =
                 linux_only_dart_send(name, name_len, p, end - p)) > 0)
                p += nwritten;
            else
                ++nfail;
        } while (p < end && nfail < MAXSENDFAIL);
        pthread_mutex_unlock(mutex);
    } else {
        send_debug_message("%s:%d Cannot lock mutex", __FILE__, __LINE__);
    }
    return p == end;
}

static const char log_prefix[] = "Order ID ";

static int rash_cancelOrder(struct exchange_interface *ei,
                            struct message *message,
                            struct parse_con *connparser,
                            void *token, struct databuf *rashmsg,
                            struct error_report *err)
{
    err->free = 0;
    if (databuf_reserve(rashmsg, sizeof(struct rash_CancelOrder)) == -1) {
        err->message = "interval error: out of memory";
        return -1;
    }
    struct dart_order_obj *csvCancelMsg = token;
    int fieldLength;
    char *field = get_rom_field(csvCancelMsg, ROM_TAG, &fieldLength);
    if (fieldLength == 0) {
        err->message = "No ROM tag in cancel message";
        return -1;
    }
    /* Find the existing order on our order book */
    void *val;
    if (get_map_entry(ei->book->rom_ord_id_map, field, fieldLength, &val)
        == 0) {
        err->message = "Unknown order";
        return -1;
    }
    struct dart_order_obj *bookOrder = val;
    field = get_rom_field(bookOrder, ROM_CLORDID, &fieldLength);
    if (fieldLength == 0) {
        err->message = "Cannot find RASH order token for order";
        /* Delete order from book? */
        return -1;
    }
    struct rash_CancelOrder *cancel =
        (struct rash_CancelOrder *) rashmsg->wr_ptr;
    memcpy(cancel, &cancelOrderDefaults, sizeof *cancel);
    memcpy(&cancel->orderToken, field, fieldLength);
    rashmsg->wr_ptr += sizeof *cancel;
    return 0;
}

typedef int (*fieldfunc) (struct dart_order_obj *,
                          struct rash_EnterOrder *, struct error_report *);
#define not_allowed (fieldfunc) -1
#define allowed_no_action_needed 0
fieldfunc ordertypeTranslations[] = {
    not_allowed,                /* Simply invalid */
    setOrdTypeMarket,           /* Market */
    setOrdTypeLimit,            /* Limit */
    not_allowed,                /* Stop market */
    not_allowed,                /* Stop limit */
    setOrdTypeMOC,              /* Market on close */
    not_allowed,                /* With or without */
    not_allowed,                /* Limit or better */
    not_allowed,                /* Limit with or without */
    not_allowed,                /* On basis */
    not_allowed,                /* On close */
    setOrdTypeLOC               /* Limit on close */
};

static const unsigned int validDisplayInstructions[] =
    { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1,
    0, 1, 0
};

static int rash_placeOrder(struct exchange_interface *ei,
                           struct message *message,
                           struct parse_con *connparser, void *token,
                           struct databuf *rashmsg,
                           struct error_report *err)
{
    err->free = 0;
    struct dart_order_obj *csvmsg = token;
    set_rom_field(csvmsg, ROM_SENDER, message->name, message->name_len);
    /* Assuming that the rashmsg databuf has already had room
     * allocated for the header. */
    if (databuf_reserve(rashmsg, sizeof(struct rash_EnterOrder)) == -1) {
        err->message = "internal error: out of memory";
        return -1;
    }
    struct rash_EnterOrder *order =
        (struct rash_EnterOrder *) rashmsg->wr_ptr;
    memcpy(order, &enter_order_defaults, sizeof *order);
    int length;
    char *field;
    field = get_rom_field(csvmsg, ROM_SIDE, &length);
    if (nbcvtsidefd
        (field, length, (char *) &order->side, sizeof order->side) == -1) {
        err->message = "invalid side";
        return -1;
    }
    field = get_rom_field(csvmsg, ROM_SHARES, &length);
    if (length == 0
        || rash_set_numeric(field, length, order->shares,
                            sizeof order->shares) == -1) {
        err->message = "invalid shares";
        return -1;
    }
    field = get_rom_field(csvmsg, ROM_SYM, &length);
    if (length == 0 || length > sizeof order->symbol) {
        err->message = "symbol exceeds length limit";
        return -1;
    }
    memcpy(order->symbol, field, length);

    /* Capacity */
    field = get_rom_field(csvmsg, ROM_CAP, &length);
    if (length != 0) {
        if (length != 1) {
            err->message = "error processing capacity";
            return -1;
        }
        order->capacity = field[0];
    }

    /* Check if this is a peg order */
    field = get_rom_field(csvmsg, ROM_EXEC_INST, &length);
    int pegResult = 0;
    if (length == 1) {
        char execInst = field[0];
        pegResult = setOrdTypePeg(csvmsg, execInst, order, err);
        if (pegResult == -1) {
            if (err->message == 0) {
                err->message = "Error processing peg order";
            }
            return -1;
        }
    }

    if (pegResult == 0) {
        field = get_rom_field(csvmsg, ROM_TYPE, &length);
        if (length == 0) {
            err->message = "missing order type";
            return -1;
        }
        long int ordertype = strtol(field, 0, 10);
        if (ordertype < 0 || ordertype >= sizeof ordertypeTranslations
            || (long int) ordertypeTranslations[ordertype] == -1) {
            if (err->message == 0) {
                err->message = "invalid order type";
            }
            return -1;
        }
        if (ordertypeTranslations[ordertype] != 0
            && ordertypeTranslations[ordertype] (csvmsg, order,
                                                 err) == -1) {
            if (err->message == 0) {
                err->message = "error processing order type";
            }
            return -1;
        }
    }

    /* Display Instructions */
    field = get_rom_field(csvmsg, ROM_DISPLAY_INSTRUCTIONS, &length);
    if (length == 1) {
        if (order->display < 'A' || order->display > 'Z'
            || !validDisplayInstructions[order->display - 'A']) {
            err->message = "Invalid display instruction";
            return -1;
        }
        order->display = field[0];
    } else if (length > 1) {
        err->message = "Invalid display instruction";
        return -1;
    }

    /* MinQty */
    field = get_rom_field(csvmsg, ROM_MIN_QTY, &length);
    if (length > 0 && rash_set_numeric(field, length,
                                       (char *) &order->min_qty,
                                       sizeof order->min_qty) == -1) {
        err->message = "Bad MinQty";
        return -1;
    }

    /* MaxFloor */
    field = get_rom_field(csvmsg, ROM_MAX_FLOOR, &length);
    if (length > 0 && rash_set_numeric(field, length,
                                       (char *) &order->max_floor,
                                       sizeof order->max_floor) == -1) {
        err->message = "Bad MinQty";
        return -1;
    }

    /* Route Dest/Exec Broker */
    field = get_rom_field(csvmsg, ROM_INSTR, &length);
    if (length > 0) {
        char *end = field + length;
        char *dash = memchr(field, '-', length);
        if (dash != 0) {
            ++dash;
            if ((end - dash) != 1) {
                err->message = "Bad routing instruction auction indicator";
                return -1;
            }
            if (*dash != 'O' && *dash != 'C' && *dash != 'I') {
                err->message =
                    "Invalid crossing session specified in instructions";
                return -1;
            }
            order->cross_type = *dash;
            end = dash - 1;
        }
        if ((end - field) != 4) {
            err->message = "Bad routing instruction";
            return -1;
        }
        memcpy(order->route_dest, field, 4);
    }


    /* Now that all of our translation and message verification is
     * done we can generate the next order ID for this message. */
    struct dstring id;
    dstring_attach(&id, order->order_token, sizeof order->order_token - 1,
                   0);
    int nidchars = ei->id_generator->generate_id(ei->id_generator, &id);
    if (nidchars == -1) {
        err->message = "error generating order ID";
        return -1;
    }
    id.p[nidchars] = ' ';       /* The ID generator puts a null-terminator
                                 * on the string. RASH requires the order
                                 * token field to be ASCII alpha-numeric
                                 * characters only, left-justified,
                                 * space-filled. */
    set_rom_field(csvmsg, ROM_CLORDID, order->order_token, nidchars);
    if (pthread_mutex_lock(&ei->book->mutex) == 0) {
        no_copy_insert(ei->book->cl_ord_id_map, order->order_token,
                       nidchars, csvmsg);
        int romtag_length;
        char *romtag = get_rom_field(csvmsg, ROM_TAG, &romtag_length);
        if (romtag_length > 0)
            no_copy_insert(ei->book->rom_ord_id_map, romtag, romtag_length,
                           csvmsg);
        pthread_mutex_unlock(&ei->book->mutex);
    }
    rashmsg->wr_ptr += sizeof(struct rash_EnterOrder);
    return 0;
}

static void record_incoming(struct message *message)
{
    struct incoming_message *incoming_message =
        malloc(sizeof *incoming_message);
    incoming_message->name =
        memdup_nullterminate(message->name, message->name_len);
    incoming_message->name_len = message->name_len;
    incoming_message->num = 1;
    record_inbound_message(incoming_message);
}

static void onDartCsvMessage(void *csvwrapper, struct message
                             *dart_message, void *b)
{
    struct parse_con *connection = b;
    struct ob *orderbook = connection->book;

    /* A message buffer for the message to be sent, whether to NASDAQ
     * or as a response back to ROM. */
    struct databuf *message_buffer = get_obj(orderbook->databufs);
    int convert_result = -1;
    databuf_reset(message_buffer);
    databuf_reserve(message_buffer, 256);

    /* By the time we reach send_message_and_log, this pointer will
     * point to the log data. For rejects back to ROM it is the same
     * as the ROM message. For RASH messages another databuf is
     * allocated and a string representation of the message is put
     * into the buffer. */
    struct databuf *log_buffer = message_buffer;

    struct message outgoing_message = {
        .name = orderbook->fix_con_name,
        .name_len = orderbook->fix_con_len,
        .data = 0,
        .len = 0,
    };

    record_incoming(dart_message);
    dart_rolling_log_coming(dart_message, orderbook->module_name,
                            orderbook->module_name_len);

    int dispose_of_dart_order_object = 1;

    if (orderbook->can_send_orders == 0) {
        if (create_rom_reject(dart_message->data, message_buffer,
                              "Not connected to exchange")) {
            outgoing_message.name = dart_message->name;
            outgoing_message.name_len = dart_message->name_len;
            goto send_message_and_log;
        } else {
            send_debug_message("%s:%d Error creating ROM reject", __FILE__,
                               __LINE__);
            return;
        }
    }

    /* Make room for the Soup TCP header. */
    databuf_memcpy(message_buffer, "U", 1);
    declare_error_report(err);
    switch (dart_message->data[0]) {
    case 'E':
        set_rom_field(csvwrapper, ROM_SENDER, dart_message->name,
                      dart_message->name_len);
        convert_result = rash_placeOrder(orderbook->ei, dart_message,
                                         connection, csvwrapper,
                                         message_buffer, &err);
        dispose_of_dart_order_object = 0;
        break;
    case 'C':
        convert_result =
            rash_cancelOrder(orderbook->ei, dart_message,
                             connection, csvwrapper, message_buffer, &err);
        break;
    case 'R':
        convert_result = -1;
        err.message = "cancel/replace not allowed on this destination";
        dispose_of_dart_order_object = 0;
        break;
    }

    if (convert_result != -1) { /* Sending translated message to
                                 * RASH. */
        /* Framing for Soup TCP is simply adding a terminating line
         * feed character to the end of the message. */
        databuf_memcpy(message_buffer, "\n", 1);
    } else {
        databuf_reset(message_buffer);
        if (create_rom_reject
            (dart_message->data, message_buffer, err.message)) {
            outgoing_message.name = dart_message->name;
            outgoing_message.name_len = dart_message->name_len;
            destroy_error_report(err);
        } else {
            send_debug_message("%s:%d Error creating ROM reject", __FILE__,
                               __LINE__);
            destroy_error_report(err);
            return;
        }
    }

  send_message_and_log:
    outgoing_message.data = message_buffer->rd_ptr;
    outgoing_message.len = databuf_unread(message_buffer);
    if (outgoing_message.name != 0 && outgoing_message.name_len > 0 &&
        outgoing_message.data != 0 && outgoing_message.len > 0) {
        rash_sendMessage(&orderbook->send_mutex, outgoing_message.data,
                         outgoing_message.len, outgoing_message.name,
                         outgoing_message.name_len);
    } else {
        send_debug_message("%s:%d No message to send", __FILE__, __LINE__);
    }
    unsigned long int bufferLength = databuf_unread(log_buffer);
    struct message log_message = {
        .name = outgoing_message.name,
        .name_len = outgoing_message.name_len,
        .data = log_buffer->rd_ptr,
        .len = bufferLength,
    };
    dart_rolling_log_going(&log_message, orderbook->module_name,
                           orderbook->module_name_len);
    if (log_buffer != message_buffer) {
        destroy_elem(orderbook->databufs, log_buffer);
    } else {
        record_outbound_message(dup_message(&outgoing_message));
    }
    destroy_elem(orderbook->databufs, message_buffer);
    if (convert_result == -1 || dispose_of_dart_order_object != 0) {
        destroy_elem(orderbook->ord_obj, csvwrapper);
    }
}

enum rash_session_state {
    disconnected,
    socket_connection_attempted,
    login_sent,
    session_established,
};

static enum rash_session_state session_state = disconnected;

static struct init_args *load_rash_config(const char *filename,
                                          int file_len)
{
    struct init_args *in_args = malloc(sizeof(struct init_args));
    in_args->dc = initialize_config(filename, file_len);
    char *reset_hour, *reset_minute, *incoming, *outgoing, *last_run_date;
    struct conf_map mappings[] = {
        {"RASH", "account_name", &(in_args->name), &(in_args->name_len)},
        {0, 0, &(in_args->username), &(in_args->un_len)},
        {"RASH", "password", &(in_args->password), &(in_args->p_len)},
        {0, 0, &(in_args->target_comp), &(in_args->tc_len)},
        {0, 0, &(in_args->sender_comp), &(in_args->sc_len)},
        {0, 0, &(in_args->target_sub), &(in_args->ts_len)},
        {0, 0, &(in_args->sender_sub), &(in_args->ss_len)},
        {0, 0, &(in_args->fix_version), &(in_args->fv_len)},
        {"RASH", "Reset Incoming", &incoming, 0},
        {"RASH", "Reset Outgoing", &outgoing, 0},
        {"TIMES", "Reset Hour", &reset_hour, 0},
        {"TIMES", "Reset Min", &reset_minute, 0},
        {"TIMES", "Last Run Day", &last_run_date, 0},
    };
    conf_map_to_vars(in_args->dc, mappings,
                     sizeof mappings / sizeof mappings[0]);
    struct seq_reset_time *reset_time = malloc(sizeof *reset_time);
    reset_time->reset_hour = reset_hour != 0 ? atoi(reset_hour) : 0;
    free(reset_hour);
    reset_time->reset_min = reset_minute != 0 ? atoi(reset_minute) : 0;
    free(reset_minute);
    reset_time->new_incoming = incoming != 0 ? atoi(incoming) : 0;
    free(incoming);
    reset_time->new_outgoing = outgoing != 0 ? atoi(outgoing) : 0;

    char current_run_date[9] = { '\0' };
    struct tm local;
    int retval =
        make_current_date_string(current_run_date, sizeof current_run_date,
                                 &local);
    reset_time->create_new = 1;
    if (retval != -1 && retval <= sizeof current_run_date
        && last_run_date != 0
        && strncmp(current_run_date, last_run_date, 8) == 0) {
        reset_time->create_new =
            should_reset(&local, reset_time->reset_hour,
                         reset_time->reset_min, 5);
    }
    in_args->reset_time = reset_time;
    set_val_for_tag(in_args->dc, "TIMES", 5, "Last Run Day", 12,
                    current_run_date, 8);
    free(last_run_date);
    return in_args;
}

#define on_entry printf("%s\n", __FUNCTION__)

static int exchange_interface_is_connected(void)
{
    /* TODO: Implement */
    return 0;
}

static void create_databuffs(queue_t glob, queue_t q, int num)
{
    int i = 0;
    for (; i < num; ++i) {
        databuf_t *buff = databuf_alloc(500);
        enqueue(q, buff);
    }
}

static orderbook *rash_book_init(struct exchange_interface *ei)
{
    char *(*generate_unique_token) (size_t *) = 0;
    orderbook *book = no_order_mng_book(generate_unique_token,
                                        exchange_interface_is_connected, 0,
                                        0, 0,
                                        ei->exchange_conf->
                                        order_allocation,
                                        ei->exchange_conf->mod_name,
                                        ei->exchange_conf->mod_name_len,
                                        ei->session_conf->name,
                                        ei->session_conf->name_len,
                                        0, 0);
    book->ei = ei;
    book->ord_obj =
        create_dart_obj_fact(ei->exchange_conf->order_allocation,
                             destroy_doo, create_dart_obj_func, clean_doo);
    book->databufs =
        create_dart_obj_fact(100, databuf_clear, create_databuffs,
                             databuf_clear);

    /* Outgoing are not sequenced so there's no point in logging
     * them. */
    ei->session_conf->log_outgoing = 0;

    /* TODO: Check on the effect of this flag. Seems to be for the ROM
     * handler resend functionality to restrict resending only
     * cancels. Since we shouldn't be sending cancels to the ROM and
     * since we want all status messages to be reported back to ROM, I
     * am assuming this should be false. */
    ei->session_conf->cancel_only = 0;

    book->in_args = copy_init_args(ei->session_conf);
    /* TODO: DB initialization here. */
    return book;
}

static int rash_configure(struct exchange_interface *ei,
                          const char *filename)
{
    on_entry;
    int retval = 0;
    ei->exchange_conf = get_config(filename, strlen(filename), "RASH", 4);
    if (ei->exchange_conf != 0
        && (ei->session_conf =
            load_rash_config(filename, strlen(filename)))
        && (ei->rom_conf = create_queue()) != 0) {
        load_rom_config(ei->exchange_conf->dc, ei->rom_conf);
        /* Configure RASH specific stuff */
        struct rash_exchange_interface *rash =
            (struct rash_exchange_interface *) ei;
        ei->book = rash_book_init(ei);
        ei->id_generator = make_dart_standard_id_generator();

        /* Load orders from the database. */
        rash_databaseInit(&ei->exchange_conf->db,
                          ei->exchange_conf->dest_id, ei->book);

        /* Create a timestamp converter */
        if ((rash->tsconverter = create_rash_timestamp_converter()) == 0)
            retval = -1;
    }
    return retval;
}

static int rash_run(struct exchange_interface *exch)
{
    printf("%s\n", __FUNCTION__);
    int retval = -1;
    time_t now, stop = exch->get_stop_time(exch);
    for (time(&now); now < stop; time(&now)) {
        if (exch->is_connected(exch) != 0) {
            sleep(1);
            exch->send_heartbeat(exch);
        } else {
            session_state = socket_connection_attempted;
            exch->connect(exch);
            if (exch->is_connected(exch) == 0)
                sleep(30);      /* Reconnect interval of 30 seconds. */
        }
    }
    return retval;
}

static const char soup_heartbeat_message[2] = { 'R', '\n' };

static int rash_send_heartbeat(struct exchange_interface *ei)
{
    struct rash_exchange_interface *oei =
        (struct rash_exchange_interface *) ei;
    if (oei == 0)
        return -1;
    if (ei->book->can_send_orders != 0) {
        return rash_sendMessage(&(ei->book->send_mutex),
                                (char *) soup_heartbeat_message,
                                sizeof soup_heartbeat_message,
                                ei->book->fix_con_name,
                                ei->book->fix_con_len);
    }
    return 0;
}

static int rash_connect(struct exchange_interface *ei)
{
    printf("%s\n", __FUNCTION__);
    struct exchange_interface_connection *orderdest =
        &(ei->client_connection);
    struct ex_con *conf = ei->exchange_conf;
    return dart_connect(orderdest->on_message, conf->mod_name,
                        conf->mod_name_len, conf->fix_port, conf->fix_ip,
                        conf->fix_ip_len, conf->end_hour, conf->end_min,
                        conf->end_day,
                        ei->client_connection.heartbeat_interval, ei,
                        orderdest->on_connection_update,
                        orderdest->frame_message, ei->session_conf,
                        orderdest->on_connection_established,
                        orderdest->parse_sequence, orderdest->on_idle,
                        orderdest->check_idle_flag);
}

int rash_connect_flag = 0;
pthread_mutex_t rash_connect_mutex = PTHREAD_MUTEX_INITIALIZER;

static void rash_set_connection_flag(int connect_flag)
{
    printf("%s\n", __FUNCTION__);
    int retval;
    if ((retval = pthread_mutex_lock(&rash_connect_mutex)) == 0) {
        rash_connect_flag = connect_flag;
        if ((retval = pthread_mutex_unlock(&rash_connect_mutex)) != 0) {
            errno = retval;
            perror("pthread_mutex_unlock");
        }
    } else {
        errno = retval;
        perror("pthread_mutex_lock");
    }
}

static void rash_on_connection_update(con_obj * connection,
                                      int is_connected, char *log, int len)
{
    printf("%s(%p, %d, %p, %d)\n", __FUNCTION__, connection, is_connected,
           log, len);
    if (is_connected == 0)
        session_state = disconnected;
    rash_set_connection_flag(is_connected);
}

static int rash_is_connected(struct exchange_interface *ei)
{
    if (session_state == disconnected)
        return 0;
    else
        return 1;
}

size_t rash__find_order_token_length(const char *p,
                                     size_t text_field_length)
{
    const char *first_space = memchr(p, ' ', text_field_length);
    if (first_space != 0)
        return first_space - p;
    else
        return text_field_length;
}

static int destadaptor_getOrderObjectByID(orderbook * book,
                                          const char *client_order_id,
                                          size_t length,
                                          dart_order_obj ** order_object)
{
    int retval = -1;
    if (pthread_mutex_lock(&book->mutex) == 0) {
        retval =
            get_map_entry(book->cl_ord_id_map, client_order_id, length,
                          (void **) order_object);
        pthread_mutex_unlock(&book->mutex);
    }
    return retval;
}

struct StatusFields;
typedef void (*cleanup_callback) (struct StatusFields *);
struct StatusFields {
    rdonlystr_t sender;
    rdonlystr_t dartOrderID;
    rdonlystr_t status;
    rdonlystr_t clientOrderID;
    rdonlystr_t destOrderID;
    rdonlystr_t timestamp;
    rdonlystr_t execPrice;
    rdonlystr_t execShares;
    rdonlystr_t execID;
    rdonlystr_t liqCode;
    rdonlystr_t desc;
    cleanup_callback cb;
    void *privateData1;
    void *privateData2;
};

static void sendMessageAndLog(pthread_mutex_t * sendMutex, char *p,
                              size_t n, const char *name,
                              size_t nameLength, const char *modName,
                              size_t modNameLength)
{
    char *end = p + n, *logstart = p;
    int nfail = 0, nwritten;
    if (pthread_mutex_lock(sendMutex) == 0) {
        do {
            if ((nwritten =
                 linux_only_dart_send(name, nameLength, p, end - p)) > 0) {
                p += nwritten;
            } else {
                ++nfail;
            }
        } while (p < end && nfail < MAXSENDFAIL);
        pthread_mutex_unlock(sendMutex);
    } else {
        send_debug_message("%s:%d Cannot lock mutex", __FILE__, __LINE__);
    }
    struct message logMsg = { (char *) name, nameLength, logstart, n };
    dart_rolling_log_going(&logMsg, modName, modNameLength);
    record_outbound_message(dup_message(&logMsg));
}

void releaseDatabuf(struct StatusFields *sf)
{
    destroy_elem((dof_t *) sf->privateData2, sf->privateData1);
}

static struct databuf *createCsvStatusMessage(struct StatusFields *sf,
                                              struct ob *book)
{
    struct databuf *msg = get_obj(book->databufs);
    databuf_memcpy(msg, "S,,,,,,,,,,,,,,", 15);
    databuf_memcpy(msg, sf->status.p, sf->status.n);    /* 14 */
    databuf_memcpy(msg, ",", 1);
    databuf_memcpy(msg, sf->clientOrderID.p, sf->clientOrderID.n);      /* 15 */
    databuf_memcpy(msg, ",,", 2);
    databuf_memcpy(msg, sf->dartOrderID.p, sf->dartOrderID.n);  /* 17 */
    databuf_memcpy(msg, ",,,,,,,,,", 9);
    databuf_memcpy(msg, sf->desc.p, sf->desc.n);
    databuf_memcpy(msg, ",,", 2);
    databuf_memcpy(msg, sf->execID.p, sf->execID.n);
    databuf_memcpy(msg, ",,,,,,,,,,,,,,", 14);
    databuf_memcpy(msg, sf->execShares.p, sf->execShares.n);
    databuf_memcpy(msg, ",,", 2);
    databuf_memcpy(msg, sf->execPrice.p, sf->execPrice.n);
    databuf_memcpy(msg, ",,,,,,", 6);
    databuf_memcpy(msg, sf->timestamp.p, sf->timestamp.n);
    databuf_memcpy(msg, ",,,,,,,,,,,,,,,,,,,", 19);
    databuf_memcpy(msg, sf->liqCode.p, sf->liqCode.n);
    databuf_memcpy(msg, ",,,,,,,,,,,\n", 12);
    sf->cb = releaseDatabuf;
    sf->privateData1 = msg;
    sf->privateData2 = book->databufs;
    return msg;
}

/* Not much to do but on accept but to set the exchange order ID and
 * the status. */
static int rash_onExtOrdAccept(struct exchange_interface *ei,
                               struct dart_order_obj *orderobj, void *p,
                               size_t n, struct StatusFields *sf,
                               int *removeOrderFromBook)
{
    sf->status.p = "13";
    sf->status.n = 2;
    struct rash_ExtOrdAccept *accept = p;
    sf->destOrderID.p = accept->orderReferenceNumber;
    sf->destOrderID.n = sizeof accept->orderReferenceNumber;
    return 0;
}

typedef int (*onRashMessage) (struct exchange_interface *,
                              struct dart_order_obj *, void *p, size_t n,
                              struct StatusFields *,
                              int *removeOrderFromBook);

static const rdonlystr_t cancelReasons[] = {
    {0, 0}, {0, 0}, {0, 0},
    {"Regulator restriction", 21},      /* D */
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {"Immediate or cancel order", 25},  /* I */
    {0, 0},
    {"Can't execute because of market collars", 39},    /* K */
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {"Self match prevention", 21},      /* Q */
    {0, 0},
    {"Supervisory cancel", 18}, /* S */
    {"Timeout", 7},             /* T */
    {"User requested cancel", 21},      /* "U" */
    {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}
};

static int rash_onOrderCanceled(struct exchange_interface *ei,
                                struct dart_order_obj *bookOrder, void *p,
                                size_t n, struct StatusFields *sf,
                                int *removeOrderFromBook)
{
    sf->status.p = "4";
    sf->status.n = 1;
    struct rash_OrderIsCanceled *cancel = p;
    if (cancel->reason >= 'A' && cancel->reason <= 'Z') {
        sf->desc = cancelReasons[cancel->reason - 'A'];
    }
    *removeOrderFromBook = 1;
    return 0;
}

static const rdonlystr_t rejectReasons[] = {
    {"Advance features not allowed", 28},
    {"Quote not available for pegged order", 36},       /* B */
    {"NASDAQ is closed", 16},   /* C */
    {"Invalid display value", 21},      /* D */
    {"Invalid peg", 11},        /* E */
    {0, 0},
    {"Destination not available", 25},  /* G */
    {"Security is halted", 18}, /* H */
    {"Invalid order side", 18}, /* I */
    {"Processing error", 16},   /* J */
    {"Invalid minimum quantity", 24},   /* K */
    {"Invalid firm", 12},       /* L */
    {"Outside of permitted times for clearing destination", 51},        /* M */
    {"Invalid routing instructions", 28},       /* N */
    {"Other", 5},               /* O */
    {"Pegging not allowed", 19},        /* P */
    {"Invalid order quantity", 22},     /* Q */
    {"Routing not allowed", 19},        /* R */
    {"Invalid symbol", 14},     /* S */
    {"Test mode", 9},           /* T */
    {"Possible duplicate order", 24},   /* U */
    {"Invalid order type", 18}, /* V */
    {"Invalid destination", 19},        /* W */
    {"Invalid price", 13},      /* X */
    {"No shares found for routing", 27},        /* Y */
    {"Quantity exceeds threshold", 26}, /* Z */
};

static int rash_onOrderRejected(struct exchange_interface *ei,
                                struct dart_order_obj *bookOrder, void *p,
                                size_t n, struct StatusFields *sf,
                                int *removeOrderFromBook)
{
    sf->status.p = "8";
    sf->status.n = 1;
    struct rash_OrderIsRejected *reject = p;
    if (n != sizeof *reject) {
        return -1;
    }
    if (reject->reason >= 'A' && reject->reason <= 'Z') {
        sf->desc = rejectReasons[reject->reason - 'A'];
    }
    *removeOrderFromBook = 1;
    return 0;
}

#define rash_assert(cond)                                               \
    if (!(cond)) {                                                      \
        send_debug_message("%s:%d %s Assertion failed: '%s'", __FILE__, __LINE__, __FUNCTION__, #cond); \
    }

static int rash_onBrokenTrade(struct exchange_interface *ei,
                              struct dart_order_obj *bookOrder,
                              void *p, size_t n, struct StatusFields *sf,
                              int *removeOrderFromBook)
{
    struct rash_BrokenTrade *brokenTrade = p;
    if (n != sizeof *brokenTrade) {
        send_debug_message
            ("Incorrect size of broken trade message: '%.*s'", (int) n, p);
        return -1;
    }
    const char title[] = "RASH Broken Trade Alert";
    struct databuf *body = get_obj(ei->book->databufs);
    databuf_reserve(body, TIMESTAMP_LEN_WITH_MILLIS + 1);
    rash_assert(rash_millisFromMidnightToDartTimestamp
                (((struct rash_exchange_interface *) ei)->tsconverter,
                 (const char *) &brokenTrade->timeStamp,
                 sizeof brokenTrade->timeStamp, body->wr_ptr,
                 TIMESTAMP_LEN_WITH_MILLIS + 1) != -1);
    body->wr_ptr += TIMESTAMP_LEN_WITH_MILLIS;
    const char *reasonStr = "Unknown";
    switch (brokenTrade->reason) {
    case 'E':
        reasonStr = "Erroneous. The trade was deemed clearly erroneous";
        break;
    case 'C':
        reasonStr =
            "Consent. The two parties mutually agreed to break the trade.";
        break;
    case 'S':
        reasonStr =
            "Supervisory. The trade was manually broken by a NASDAQ supervisory terminal.";
        break;
    case 'X':
        reasonStr =
            "External. The trade was broken by an external third party.";
        break;
    }
    databuf_write(body,
                  ": Order Token = '%.*s', Match Number = '%.*s'\nReason (%c): %s\n",
                  (int) sizeof brokenTrade->orderToken,
                  brokenTrade->orderToken,
                  (int) sizeof brokenTrade->matchNumber,
                  brokenTrade->matchNumber, brokenTrade->reason,
                  reasonStr);
    send_biz_message(body->rd_ptr, databuf_unread(body), title,
                     sizeof title - 1);
    return 0;
}

typedef uint32_t shareqty_t;

static int rash_onOrderExecution(struct exchange_interface *ei,
                                 struct dart_order_obj *bookOrder,
                                 void *p, size_t n,
                                 struct StatusFields *sf,
                                 int *removeOrderFromBook)
{
    struct rash_OrderExecution *exec = p;
    if (n != sizeof *exec) {
        return -1;
    }
    sf->status.p = "1";         /* Default to partially filled, will check to
                                   see if completely filled later. */
    sf->status.n = 1;

    /* Handle the shares executed, removing the leading zeros from the
     * quantity string. */
    for (sf->execShares.p = exec->execShares, sf->execShares.n =
         sizeof exec->execShares;
         sf->execShares.n != 0 && *(sf->execShares.p) == '0';
         ++(sf->execShares.p), --(sf->execShares.n));

    struct databuf *scratchBuffer = get_obj(ei->book->databufs);
    if (scratchBuffer == 0) {
        send_debug_message("%s:%d %s Could not get scratch buffer",
                           __FILE__, __LINE__, __FUNCTION__);
        return -1;
    }
    sf->privateData1 = scratchBuffer;
    sf->privateData2 = ei->book->databufs;
    databuf_reset(scratchBuffer);
    if (rash_get_price(exec->execPrice, scratchBuffer) == -1) {
        send_debug_message("%s:%d %s Error converting RASH price",
                           __FILE__, __LINE__, __FUNCTION__);
        return -1;
    }
    sf->execPrice.p = scratchBuffer->rd_ptr;
    sf->execPrice.n = databuf_unread(scratchBuffer);
    sf->cb = releaseDatabuf;

    sf->execID.p = exec->matchNumber;
    sf->execID.n = sizeof exec->matchNumber;
    sf->liqCode.p = &exec->liquidityCode;
    sf->liqCode.n = sizeof exec->liquidityCode;
    int sharesLength;
    shareqty_t *pCumShares =
        (shareqty_t *) get_rom_field(bookOrder, ROM_BIN_CUM,
                                     &sharesLength);
    if (sharesLength != sizeof *pCumShares || pCumShares == 0) {
        shareqty_t cumShares = 0;
        set_rom_field(bookOrder, ROM_BIN_CUM, &cumShares,
                      sizeof cumShares);
        pCumShares =
            (shareqty_t *) get_rom_field(bookOrder, ROM_BIN_CUM,
                                         &sharesLength);
        if (sharesLength != sizeof *pCumShares || pCumShares == 0) {
            send_debug_message
                ("%s:%d %s error retrieving binary cum shares", __FILE__,
                 __LINE__, __FUNCTION__);
            return -1;
        }
    }
    char localExecShares[sizeof exec->execShares + 1];
    memcpy(localExecShares, exec->execShares, sizeof exec->execShares);
    localExecShares[sizeof localExecShares - 1] = '\0';
    char *endptr;
    unsigned long int execShares = strtol(localExecShares, &endptr, 10);
    if (endptr != &localExecShares[sizeof localExecShares - 1]) {
        send_debug_message
            ("%s:%d %s Might not have properly consumed executed shares",
             __FILE__, __LINE__, __FUNCTION__);
    }
    *pCumShares += execShares;

    /* Get the original order's shares */
    shareqty_t *pOrderShares =
        (shareqty_t *) get_rom_field(bookOrder, ROM_BIN_QTY,
                                     &sharesLength);
    if (sharesLength != sizeof *pOrderShares || pOrderShares == 0) {
        char *strOrderShares =
            get_rom_field(bookOrder, ROM_SHARES, &sharesLength);
        char *endptr;
        errno = 0;
        shareqty_t localOrderShares = strtol(strOrderShares, &endptr, 10);
        if (*endptr != ',' || localOrderShares == 0 || errno != 0) {
            send_debug_message
                ("%s:%d %s Warning, possible error converting ROM "
                 "shares fields to an integer: %s\n", __FILE__, __LINE__,
                 __FUNCTION__,
                 errno != 0 ? strerror(errno) : "no errno problem");
        }
        set_rom_field(bookOrder, ROM_BIN_QTY, &localOrderShares,
                      sizeof localOrderShares);
        pOrderShares =
            (shareqty_t *) get_rom_field(bookOrder, ROM_BIN_QTY,
                                         &sharesLength);
    }
    if (pOrderShares != 0 && sharesLength == sizeof *pOrderShares) {
        if (*pCumShares >= *pOrderShares) {
            sf->status.p = "2";
            sf->status.n = 1;
            *removeOrderFromBook = 1;
        }
    }
    return 0;
}

static onRashMessage rash_message_handlers[] = {
    0,
    rash_onBrokenTrade,         /* B */
    rash_onOrderCanceled,       /* C */
    0,
    rash_onOrderExecution,      /* E */
    0, 0, 0, 0,
    rash_onOrderRejected,       /* J */
    0, 0, 0, 0, 0, 0, 0,
    rash_onExtOrdAccept,        /* R */
    0, 0, 0, 0, 0, 0, 0, 0,
};

static size_t getTextLengthWithoutTrailingWhitespace(const char *p,
                                                     size_t n)
{
    const char *firstSpace = memchr(p, ' ', n);
    if (firstSpace != 0 && firstSpace > p)
        n = firstSpace - p;
    return n;
}

/* The RASH order IDs are space-padded on the right but we only store
 * them without the spaces. This function strips the spaces and looks
 * up the order object by the standard exchange interface method. */
static int rash_getOrderByToken(struct ob *book, const char *p, size_t n,
                                dart_order_obj ** orderobj)
{
    n = getTextLengthWithoutTrailingWhitespace(p, n);
    return destadaptor_getOrderObjectByID(book, p, n, orderobj);
}

static void recordIncoming(struct parse_con *pc)
{
    struct incoming_message *inmsg = malloc(sizeof *inmsg);
    inmsg->name = (char *) get_name(pc->co);
    inmsg->name_len = get_name_len(pc->co);
    inmsg->num = 1;
    record_inbound_message(inmsg);
}

static int rash_connectedState(struct rash_exchange_interface *rei,
                               const char *p, size_t n,
                               int *dropconnection, struct parse_con *pc)
{
    int result = -1;
    if (n < sizeof(struct rash_soup_header)) {
        send_debug_message
            ("Bad packet received (too small for SOUP/RASH header): '%.*s'",
             (int) n, p);
        return -1;
    }
    struct rash_soup_header *hdr = (struct rash_soup_header *) p;
    if (hdr->packet_type != 'S') {
        send_debug_message
            ("Expecting only Sequenced Data Packets but received this: '%.*s'",
             (int) n, p);
        return -1;
    }
    recordIncoming(pc);
    int msgtype = hdr->message_type;
    if (msgtype < 'A' || msgtype > 'Z') {
        send_debug_message
            ("Message type outside of expected range ('%c'): %.*s",
             msgtype, (int) n, p);
        return -1;
    }
    msgtype -= 'A';
    onRashMessage handlefunc = rash_message_handlers[msgtype];
    if (handlefunc == 0) {
        send_debug_message("Unsupported message type '%c': %.*s", msgtype,
                           (int) n, p);
        return -1;
    }
    /* Get the order object to which this pertains. It just so happens
     * that all of the messages that come from the destination have
     * the same first three fields, the last of which is the order
     * token. */
    dart_order_obj *orderobj;
    if (rash_getOrderByToken
        (rei->ei.book, hdr->order_token, sizeof hdr->order_token,
         &orderobj) == 0) {
        send_debug_message
            ("Could not find order on book for this order: '%.*s'",
             (int) n, p);
        return -1;
    }
    struct StatusFields sf;
    memset(&sf, 0, sizeof sf);
    int len;
    char *field = get_rom_field(orderobj, ROM_SENDER, &len);
    if (len == 0) {
        send_debug_message
            ("No sender for message, order token '%.*s': %.*s",
             (int) sizeof hdr->order_token, hdr->order_token, (int) n, p);
        return -1;
    }
    sf.sender.p = field;
    sf.sender.n = len;
    field = get_rom_field(orderobj, ROM_TAG, &len);
    if (len == 0) {
        send_debug_message
            ("No ROM tag for message, order token '%.*s': %.*s",
             (int) sizeof hdr->order_token, hdr->order_token, (int) n, p);
        return -1;
    }
    sf.dartOrderID.p = field;
    sf.dartOrderID.n = len;
    sf.clientOrderID.p = hdr->order_token;
    sf.clientOrderID.n =
        getTextLengthWithoutTrailingWhitespace(hdr->order_token,
                                               sizeof hdr->order_token);
    char ts[TIMESTAMP_LEN_WITH_MILLIS + 1];
    if (rash_millisFromMidnightToDartTimestamp
        (rei->tsconverter, hdr->timestamp, sizeof hdr->timestamp, ts,
         sizeof ts) != -1) {
        sf.timestamp.p = ts;
        sf.timestamp.n = TIMESTAMP_LEN_WITH_MILLIS;
    } else {
        send_debug_message("error converting timestamp: '%.*s'",
                           (int) sizeof hdr->timestamp, hdr->timestamp);
    }

    int removeOrderFromBook = 0;

    /* Make the status message */
    /* Note: when passing the pointers and size to the handling
     * functions, we are advancing the pointer by one byte past the
     * Soup packet type and reducing the size by 2, 1 for the the Soup
     * packet byte we skipped, and 1 for the trailing newline
     * character the ASCII Soup framing requires. The pointer and size
     * passed to the handler function will be only the RASH message
     * with none of the Soup framing. */
    if (handlefunc
        ((struct exchange_interface *) rei, orderobj, (void *) (p + 1),
         n - 2, &sf, &removeOrderFromBook) == -1)
        return -1;

    struct ob *book = rei->ei.book;

    /* Todo: Remove the order from the book */
    if (removeOrderFromBook != 0 && pthread_mutex_lock(&book->mutex) == 0) {
        field = get_rom_field(orderobj, ROM_CLORDID, &len);
        if (len > 0) {
            remove_from_map(book->cl_ord_id_map, field, len);
        }
        field = get_rom_field(orderobj, ROM_TAG, &len);
        if (len > 0) {
            remove_from_map(book->rom_ord_id_map, field, len);
        }
        destroy_elem(book->ord_obj, orderobj);
        pthread_mutex_unlock(&book->mutex);
    }

    struct databuf *sendBuf = createCsvStatusMessage(&sf, book);
    sendMessageAndLog(&book->send_mutex, sendBuf->rd_ptr,
                      databuf_unread(sendBuf), sf.sender.p, sf.sender.n,
                      book->module_name, book->module_name_len);

    /* Call the cleanup callback */
    if (sf.cb != 0)
        sf.cb(&sf);

    return result;
}

/* The RASH session is currently disconnected. A socket connection has
 * been made and a login request has been sent. */
static int rash_disconnectedState(struct rash_exchange_interface *rei,
                                  const char *p, size_t n,
                                  int *dropconnection,
                                  struct parse_con *pc)
{
    if (n == 0)
        return -1;
    char type = p[0];
    if (type == 'A') {          /* Login accepted */
        if (n != sizeof(struct soup_login_accepted)) {
            send_debug_message
                ("Bad A message length (login accepted): '%.*s'", (int) n,
                 p);
            return -1;
        }
        send_debug_message("Login accepted: %.*s", (int) n, p);
        rei->evaluate = rash_connectedState;
        session_state = session_established;
        rei->ei.book->can_send_orders = 1;
        return 0;
    } else if (type == 'J') {   /* Login rejected */
        if (n != 3) {
            send_debug_message
                ("Bad J message size (login rejected): '%.*s'", (int) n,
                 p);
            return -1;
        }
        char reject_reason = p[1];
        const char *reject_text = "unknown";
        if (reject_reason == 'A')
            reject_text = "Not authorized";
        else if (reject_reason == 'S')
            reject_text = "Session not available";
        send_debug_message("Login rejected; reason (%c) %s\n",
                           reject_reason, reject_text);
        return 0;
    } else {
        send_debug_message
            ("Unexpected message before login handshake complete: '%.*s'",
             (int) n, p);
        return -1;
    }
}

static unsigned long rash_frame(const char *data, unsigned long nbytes,
                                int *cut_connection,
                                struct parse_con *connection_parse_info)
{
    struct rash_exchange_interface *rei = connection_parse_info->book;
    const char *p, *end, *eom, *name;
    for (p = data, end = data + nbytes;
         p < end && (eom = memchr(p, '\n', end - p)) != 0; p = eom) {
        const size_t mlen = ++eom - p;
        if (mlen == 0) {
            fprintf(stderr, "ERROR %s(%d)(%s): 0 length framed message\n",
                    __FILE__, __LINE__, __FUNCTION__);
            goto done;
        }
        char type = p[0];
        if (type == 'H') {
            continue;
        } else if (type == '+') {
            send_debug_message("SoupTCP Debug Packet received: '%.*s'",
                               (int) (eom - (p + 1)), p + 1);
        } else {
            /* Log it */
            name = get_name(connection_parse_info->co);
            struct message logmsg = {
                .name = (char *) name,
                .name_len = get_name_len(connection_parse_info->co),
                .data = (char *) p,
                .len = eom - p,
            };
            dart_rolling_log_coming(&logmsg, rei->ei.book->module_name,
                                    rei->ei.book->module_name_len);
            free((char *) name);
            rei->evaluate(rei, p, mlen, cut_connection,
                          connection_parse_info);
        }
    }
  done:
    return p - data;            /* Number of bytes consumed */
}

static void rash_on_message(struct message *message, void *token)
{
    printf("%s\n", __FUNCTION__);
}

static void rash_on_idle(con_obj * connection)
{
    printf("%s\n", __FUNCTION__);
}

static unsigned long rash_parse_sequence(char *p, unsigned long n,
                                         unsigned long j, unsigned long *k)
{
    printf("%s\n", __FUNCTION__);
    return 0;
}

static char *rash_reset_pad(unsigned long i, char *p, size_t * n)
{
    printf("%s\n", __FUNCTION__);
    return 0;
}

struct soup_LoginRequest {
    char type;
    char username[6];
    char password[10];
    char requestedSession[10];
    char requestedSeqNo[10];
    char terminator;
};

static int soup_initLoginRequest(struct soup_LoginRequest *login,
                                 const char *puser, const char *ppass,
                                 unsigned long int seqno)
{
    login->type = 'L';
    memset(&login->username, ' ', sizeof login->username);
    size_t nuser = strlen(puser);
    if (nuser > sizeof login->username)
        return -1;
    memcpy(&login->username, puser, nuser);
    memset(&login->password, ' ', sizeof login->password);
    size_t npass = strlen(ppass);
    if (npass > sizeof login->password)
        return -1;
    memcpy(&login->password, ppass, npass);
    memset(&login->requestedSession, ' ', sizeof login->requestedSession);
    int nwritten = sprintf((char *) &login->requestedSeqNo,
                           "%*ld",
                           (int) sizeof login->requestedSeqNo,
                           seqno);
    if (nwritten != sizeof login->requestedSeqNo)
        return -1;
    login->terminator = '\n';
    return 0;
}

static int rash_onConnectionEstablished(struct parse_args *parse_info)
{
    printf("%s\n", __FUNCTION__);
    struct parse_con *connectionparser = parse_info->con;
    size_t namelength = get_name_len(connectionparser->co);
    char *connectionname = (char *) get_name(connectionparser->co);
    sequence_req *request =
        create_sequence_request(connectionname, namelength,
                                connectionparser->in_args
                                , build_offsets,
                                create_rom_resend_pad);
    get_sequence_numbers(request);
    set_incoming_seq(connectionparser->co, dsr_get_incoming(request));
    unsigned long int in_seqno = get_incoming_seq(connectionparser->co);
    send_debug_message("Retrieved incoming sequence number: %lu",
                       in_seqno);
    ++in_seqno;
    destroy_sequence_req(request);
    connectionparser->init_con(connectionparser->co);
    struct rash_exchange_interface *ei =
        (struct rash_exchange_interface *) parse_info->con->book;
    ei->ei.client_connection.connection = parse_info->con->co;

    struct soup_LoginRequest login_request;
    if (soup_initLoginRequest
        (&login_request, ei->ei.session_conf->name,
         ei->ei.session_conf->password, in_seqno) == -1)
        return -1;
    rash_sendMessage(&ei->ei.book->send_mutex, (char *) &login_request,
                     sizeof login_request, connectionname, namelength);
    struct message logmsg = {
        .name = connectionname,
        .name_len = namelength,
        .data = (char *) &login_request,
        .len = sizeof login_request,
    };
    dart_rolling_log_going(&logmsg, ei->ei.book->module_name,
                           ei->ei.book->module_name_len);
    free(connectionname);
    session_state = login_sent;
    return 0;
}

static struct rash_exchange_interface *rash_exchange_interface_create(void)
{
    printf("%s\n", __FUNCTION__);
    struct rash_exchange_interface *rash = malloc(sizeof *rash);
    exchange_interface__init(&(rash->ei));
    rash->ei.configure = rash_configure;
    rash->ei.listen = exchange_interface_listen;
    exchange_interface_rom_connection_init(&(rash->ei.server_connection));
    rash->ei.server_connection.on_message_using_stream_state =
        onDartCsvMessage;
    rash->ei.run = rash_run;
    rash->ei.get_stop_time = exchange_interface_get_stop_time;
    rash->ei.is_connected = rash_is_connected;
    rash->ei.send_heartbeat = rash_send_heartbeat;
    rash->ei.connect = rash_connect;
    rash->evaluate = rash_disconnectedState;
    rash->ei.on_enter_order = rash_placeOrder;
    struct exchange_interface_connection *connection =
        &(rash->ei.client_connection);
    connection->on_connection_update = rash_on_connection_update;
    connection->frame_message = rash_frame;
    connection->on_message = rash_on_message;
    connection->on_idle = rash_on_idle;
    connection->parse_sequence = rash_parse_sequence;
    connection->create_reset_pad = rash_reset_pad;
    connection->on_connection_established = rash_onConnectionEstablished;
    connection->check_idle_flag = 0;
    connection->heartbeat_interval = 1;
    return rash;
}

#define handle_errorcode(errorcode, msg)  \
    do { errno = errorcode; perror(msg); exit(EXIT_FAILURE); } while (0)

/* Returns 0 => success, 1 => bad arguments, 2 => runtime error */
int main(int argc, char *argv[])
{
    int retval = 0;
    if (argc > 1) {
        struct exchange_interface *exch =
            (struct exchange_interface *) rash_exchange_interface_create();
        if ((retval = exch->configure(exch, argv[1])) == 0
            && (retval = exch->listen(exch)) == 0) {
            retval = exch->run(exch);
        }
    } else {
        fprintf(stderr, "usage: rash conf-file\n");
        retval = 1;
    }
    if (retval == -1) {
        retval = 2;
    }
    return retval;
}
