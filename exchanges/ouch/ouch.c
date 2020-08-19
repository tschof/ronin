#include "server_objects/message_queue.h"
#include "server_objects/rom_handler.h"
#include "server_objects/dart_constants.h"
#include "server_objects/dart_orderbook.h"
#include "server_objects/debug_logger.h"
#include "server_objects/tm_to_str.h"

#include "ex_common/ex_common.h"
#include "ex_common/ex_config_helper.h"

#include "ex_db/db_layer.h"

#include "smtp/mailer.h"

#include "ouch_exchange_interface.h"
#include "exchange_interface.h"
#include "soupbin.h"
#include "conf_util.h"
#include "ouch_util.h"
#include "ouch41.h"
#include "dump_data.h"
#include "macros.h"
#include "dart_to_ouch.h"
#include "dstring.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <limits.h>

void ouch__on_order_not_found(void *ouch_message,
                              size_t ouch_message_length,
                              struct databuf *output)
{
    databuf_write(output, "Cannot find the order this message references "
                  "on our book:\n");
    ouch41__dump_message(ouch_message, ouch_message_length, output);
    const char *title = "Cannot find order";
    send_tech_message(output->rd_ptr, databuf_unread(output), title,
                      strlen(title));
    send_debug_message("%.*s\n", (int) databuf_unread(output),
                       output->rd_ptr);
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

static void ouch_database_init(struct database_conf *dbconf,
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
static int ouch__send_message(pthread_mutex_t * mutex, char *p, size_t n,
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

static int get_obj_by_rom_tag(orderbook * ob, dart_order_obj * rph,
                              dart_order_obj ** obj)
{
    int ret = 0;
    char *rom_id = rph->data->buffer + rph->positions[ROM_TAG].beg;
    int rom_id_len =
        rph->positions[ROM_TAG].end - rph->positions[ROM_TAG].beg;
    pthread_mutex_lock(&ob->mutex);
    ret =
        get_map_entry(ob->rom_ord_id_map, rom_id, rom_id_len,
                      (void **) obj);
    pthread_mutex_unlock(&ob->mutex);
    return ret;
}

static const char log_prefix[] = "Order ID ";

static int ouch__cancel_order(struct exchange_interface *ei,
                              struct message *message,
                              struct parse_con *connparser,
                              void *token, struct databuf *msgbuf,
                              const char **errmsg)
{
    int retval = -1;
    dart_order_obj *book_order = 0;
    /* Find the order on our book. */
    if (get_obj_by_rom_tag(ei->book, token, &book_order)) {
        int order_token_length;
        char *order_token = get_rom_field(book_order, ROM_CLORDID,
                                          &order_token_length);
        struct ouch41__cancel_order cancel;
        ouch41__cancel_order__init(&cancel);
        memcpy(cancel.order_token, order_token, order_token_length);
        retval = ouch41__serialize(&cancel, sizeof cancel, msgbuf);
    }
    return retval;
}

static dart_order_obj *ouch__lookup_book_order(struct exchange_interface
                                               *ei, void *parser_helper)
{
    dart_order_obj *book_order = 0;
    if (!get_obj_by_rom_tag(ei->book, parser_helper, &book_order))
        book_order = 0;
    return book_order;
}

static int ouch__replace_order(struct exchange_interface *ei,
                               struct message *message,
                               struct parse_con *connparser,
                               void *parser_helper,
                               struct databuf *msgbuf, const char **errmsg)
{
    int retval = -1;
    dart_order_obj *book_order =
        ouch__lookup_book_order(ei, parser_helper);
    jumpassert(book_order != 0, done);
    if (book_order->rep != 0) {
        *errmsg = strdup("Replace already pending");
        goto done;
    }
    int current_order_token_length;
    char *current_order_token = get_rom_field(book_order, ROM_CLORDID,
                                              &current_order_token_length);
    jump_on_fail(current_order_token_length > 0, done, *errmsg,
                 "cannot find order");
    struct ouch41__replace_order replace;
    ouch41__replace_order__init(&replace);
    jumpassert(dart_csv__to__ouch41__replace_order(parser_helper,
                                                   book_order,
                                                   &replace,
                                                   errmsg) == 0, done);
    memcpy(&replace.existing_order_token, current_order_token,
           current_order_token_length);
    struct dstring id;
    dstring_attach(&id, (char *) &replace.replacement_order_token,
                   sizeof replace.replacement_order_token - 1, 0);
    retval = ei->id_generator->generate_id(ei->id_generator, &id);
    jump_on_fail(retval > 0, done, *errmsg, "error generating unique id");
    set_rom_field(parser_helper, ROM_CLORDID, id.p, retval);
    id.p[retval] = ' ';
    no_copy_insert(ei->book->cl_ord_id_map, id.p, id.nlen, book_order);
    retval = ouch41__serialize(&replace, sizeof replace, msgbuf);
  done:
    if (retval != -1 && book_order != 0)
        book_order->rep = parser_helper;
    return retval;
}

/* Here we hand the on_enter_order function callback the
 * exchange_interface, the incoming message, the connection object, a
 * void pointer that can point to a state or helper object for the
 * incoming stream, the outgoing message buffer, and an error message
 * that, upon returning -1 indicating the message has been rejected,
 * should contain something descriptive to send back to the user.
 * This function simply responds with whether the translation passed
 * or failed and, if it passed, the caller sends the contents of the
 * message buffer to the destination. */
static int ouch__enter_order(struct exchange_interface *ei,
                             struct message *message,
                             struct parse_con *connparser, void *token,
                             struct databuf *msgbuf, const char **errmsg)
{
    int retval = -1;
    struct dart_order_obj *csv = token;
    set_rom_field(csv, ROM_SENDER, message->name, message->name_len);
    struct ouch41__enter_order order;

    /* Generate an ID for this order. */
    struct dstring id;
    dstring_attach(&id, order.order_token,
                   sizeof order.order_token - 1, 0);
    int nidchars;
    if (ouch41__csv_to_ouch_enter_order(csv, &order, errmsg) != -1
        && (nidchars =
            ei->id_generator->generate_id(ei->id_generator, &id)) > -1) {
        /* Since the ID generator null-terminates the string and OUCH
         * requires space-padded text fields we must overwrite the
         * null with a space. */
        id.p[nidchars] = ' ';
        set_rom_field(csv, ROM_CLORDID, order.order_token, nidchars);
        retval = ouch41__serialize(&order, sizeof order, msgbuf);
        if (pthread_mutex_lock(&ei->book->mutex) == 0) {
            no_copy_insert(ei->book->cl_ord_id_map, order.order_token,
                           nidchars, csv);
            int romtag_length;
            char *romtag = get_rom_field(csv, ROM_TAG, &romtag_length);
            if (romtag_length > 0)
                no_copy_insert(ei->book->rom_ord_id_map, romtag,
                               romtag_length, csv);
            pthread_mutex_unlock(&ei->book->mutex);
        }
    }
    return retval;
}

char *memdup_nullterminate(const char *p, size_t n)
{
    char *retval = malloc(n + 1);
    memcpy(retval, p, n);
    retval[n] = '\0';
    return retval;
}

struct message *make_message(const char *name,
                             size_t name_length,
                             const char *data, size_t data_length)
{
    struct message *retval = malloc(sizeof *retval);
    retval->name = memdup_nullterminate(name, name_length);
    retval->name_len = name_length;
    retval->data = memdup_nullterminate(data, data_length);
    retval->len = data_length;
    return retval;
}

struct message *dup_message(struct message *message)
{
    return make_message(message->name, message->name_len, message->data,
                        message->len);
}

void record_incoming(struct message *message)
{
    struct incoming_message *incoming_message =
        malloc(sizeof *incoming_message);
    incoming_message->name =
        memdup_nullterminate(message->name, message->name_len);
    incoming_message->name_len = message->name_len;
    incoming_message->num = 1;
    record_inbound_message(incoming_message);
}

void record_incoming2(struct parse_con *pc)
{
    struct incoming_message *incoming_message =
        malloc(sizeof *incoming_message);
    incoming_message->name = (char *) get_name(pc->co);
    incoming_message->name_len = get_name_len(pc->co);
    incoming_message->num = 1;
    record_inbound_message(incoming_message);
}

static void ouch__on_message_using_parser_helper(void *csvwrapper, struct message
                                                 *dart_message, void *b)
{
    struct parse_con *connection = b;
    struct ob *orderbook = connection->book;

    /* A message buffer for the message to be sent, whether to NASDAQ
     * or as a response back to ROM. */
    struct databuf *message_buffer = get_obj(orderbook->databufs);
    int convert_result = -1;
    databuf_reset(message_buffer);

    /* By the time we reach send_message_and_log, this pointer will
     * point to the log data. For rejects back to ROM it is the same
     * as the ROM message. For OUCH messages another databuf is
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

    /* Make room for the SoupBin TCP header. */
    struct soupbin_header header_init = {
        .packet_length = 1,
        .packet_type = SOUPBIN_PACKET_UNSEQUENCED_DATA,
    };
    databuf_memcpy(message_buffer, &header_init, sizeof header_init);
    message_buffer->rd_ptr = message_buffer->wr_ptr;

    const char *error_text = 0;

    switch (dart_message->data[0]) {
    case 'E':
        set_rom_field(csvwrapper, ROM_SENDER, dart_message->name,
                      dart_message->name_len);
        convert_result = ouch__enter_order(orderbook->ei, dart_message,
                                           connection, csvwrapper,
                                           message_buffer, &error_text);
        dispose_of_dart_order_object = 0;
        break;
    case 'C':
        convert_result =
            ouch__cancel_order(orderbook->ei, dart_message,
                               connection, csvwrapper,
                               message_buffer, &error_text);
        break;
    case 'R':
        convert_result =
            ouch__replace_order(orderbook->ei, dart_message,
                                connection, csvwrapper,
                                message_buffer, &error_text);
        dispose_of_dart_order_object = 0;
        break;
    }

    if (convert_result != -1) { /* Sending translated message to
                                 * OUCH. */
        /* Fixup header */
        struct soupbin_header *headerp =
            (struct soupbin_header *) message_buffer->buffer;
        headerp->packet_length = databuf_unread(message_buffer) + 1;
        headerp->packet_length = htons(headerp->packet_length);
        message_buffer->rd_ptr = message_buffer->buffer;

        /* Create human readable log */
        log_buffer = get_obj(orderbook->databufs);
        if (soupbin__dump_data
            (message_buffer->rd_ptr, databuf_unread(message_buffer),
             log_buffer, ouch41__dump_message) == -1) {
            send_debug_message("%s:%d Error dumping SoupBin TCP message",
                               __FILE__, __LINE__);
        }

        /* Now that it is logged, fixup the message type. */
        message_buffer->rd_ptr += sizeof *headerp;
        if (message_buffer->rd_ptr[0] == 'R')
            message_buffer->rd_ptr[0] = 'U';
        message_buffer->rd_ptr = message_buffer->buffer;
    } else {
        databuf_reset(message_buffer);
        if (create_rom_reject(dart_message->data, message_buffer,
                              error_text)) {
            outgoing_message.name = dart_message->name;
            outgoing_message.name_len = dart_message->name_len;
        } else {
            send_debug_message("%s:%d Error creating ROM reject", __FILE__,
                               __LINE__);
            return;
        }
    }

  send_message_and_log:
    outgoing_message.data = message_buffer->rd_ptr;
    outgoing_message.len = databuf_unread(message_buffer);

    if (outgoing_message.name != 0 && outgoing_message.name_len > 0 &&
        outgoing_message.data != 0 && outgoing_message.len > 0) {
        ouch__send_message(&orderbook->send_mutex, outgoing_message.data,
                           outgoing_message.len, outgoing_message.name,
                           outgoing_message.name_len);
    } else {
        send_debug_message("%s:%d No message to send", __FILE__, __LINE__);
    }

    struct message log_message = {
        .name = outgoing_message.name,
        .name_len = outgoing_message.name_len,
        .data = log_buffer->rd_ptr,
        .len = databuf_unread(log_buffer),
    };
    dart_rolling_log_going(&log_message, orderbook->module_name,
                           orderbook->module_name_len);
    if (log_buffer != message_buffer) {
        destroy_elem(orderbook->databufs, log_buffer);
    } else {
        record_outbound_message(dup_message(&outgoing_message));
    }
    destroy_elem(orderbook->databufs, message_buffer);
    if (convert_result == -1 || dispose_of_dart_order_object != 0)
        destroy_elem(orderbook->ord_obj, csvwrapper);
}

enum ouch_session_state {
    disconnected,
    socket_connection_attempted,
    login_sent,
    session_established,
};

static enum ouch_session_state session_state = disconnected;

static struct init_args *load_ouch_config(const char *filename,
                                          int file_len)
{
    struct init_args *in_args = malloc(sizeof(struct init_args));
    in_args->dc = initialize_config(filename, file_len);
    char *reset_hour, *reset_minute, *incoming, *outgoing, *last_run_date;
    struct conf_map mappings[] = {
        {"OUCH", "account_name", &(in_args->name), &(in_args->name_len)},
        {0, 0, &(in_args->username), &(in_args->un_len)},
        {"OUCH", "password", &(in_args->password), &(in_args->p_len)},
        {0, 0, &(in_args->target_comp), &(in_args->tc_len)},
        {0, 0, &(in_args->sender_comp), &(in_args->sc_len)},
        {0, 0, &(in_args->target_sub), &(in_args->ts_len)},
        {0, 0, &(in_args->sender_sub), &(in_args->ss_len)},
        {0, 0, &(in_args->fix_version), &(in_args->fv_len)},
        {"OUCH", "Reset Incoming", &incoming, 0},
        {"OUCH", "Reset Outgoing", &outgoing, 0},
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

#if 0
static void *databuf_create(void *arg)
{
    return databuf_alloc((size_t) arg);
}

static void create_n(queue_t q, int n, void *(*create) (void *), void *arg)
{
    int i;
    for (i = 0; i < n; ++i)
        enqueue(q, create(arg));
}

static void create_shitload_of_databufs(void)
{
    queue_t q = create_queue();
    create_n(q, 10000, databuf_create, (void *) 512);
}
#endif

static void create_databuffs(queue_t glob, queue_t q, int num)
{
    int i = 0;
    for (; i < num; ++i) {
        databuf_t *buff = databuf_alloc(500);
        enqueue(q, buff);
    }
}

static orderbook *ouch_book_init(struct exchange_interface *ei)
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

static int ouch_configure(struct exchange_interface *ei,
                          const char *filename)
{
    on_entry;
    int retval = -1;
    ei->exchange_conf = get_config(filename, strlen(filename), "OUCH", 4);
    if (ei->exchange_conf != 0
        && (ei->session_conf =
            load_ouch_config(filename, strlen(filename)))
        && (ei->rom_conf = create_queue()) != 0) {
        load_rom_config(ei->exchange_conf->dc, ei->rom_conf);
        /* Configure OUCH specific stuff */
        struct ouch_exchange_interface *ouch =
            (struct ouch_exchange_interface *) ei;
        if ((retval =
             soupbin_login_init(&ouch->login, ei->session_conf->name,
                                ei->session_conf->password)) == 0) {
            retval = soupbin_heartbeat_init(&ouch->heartbeat);
        }
        ei->book = ouch_book_init(ei);
        ei->id_generator = make_dart_standard_id_generator();

        /* Load orders from the database. */
        ouch_database_init(&ei->exchange_conf->db,
                           ei->exchange_conf->dest_id, ei->book);
    }
    return retval;
}

static int ouch_run(struct exchange_interface *exch)
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

static int ouch_send_heartbeat(struct exchange_interface *ei)
{
    struct ouch_exchange_interface *oei =
        (struct ouch_exchange_interface *) ei;
    if (oei == 0)
        return -1;
    if (ei->book->can_send_orders != 0) {
        return ouch__send_message(&(ei->book->send_mutex),
                                  (char *) &(oei->heartbeat),
                                  sizeof oei->heartbeat,
                                  ei->book->fix_con_name,
                                  ei->book->fix_con_len);
    }
    return 0;
}

static int ouch_connect(struct exchange_interface *ei)
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

int ouch_connect_flag = 0;
pthread_mutex_t ouch_connect_mutex = PTHREAD_MUTEX_INITIALIZER;

static void ouch_set_connection_flag(int connect_flag)
{
    printf("%s\n", __FUNCTION__);
    int retval;
    if ((retval = pthread_mutex_lock(&ouch_connect_mutex)) == 0) {
        ouch_connect_flag = connect_flag;
        if ((retval = pthread_mutex_unlock(&ouch_connect_mutex)) != 0) {
            errno = retval;
            perror("pthread_mutex_unlock");
        }
    } else {
        errno = retval;
        perror("pthread_mutex_lock");
    }
}

static void ouch_on_connection_update(con_obj * connection,
                                      int is_connected, char *log, int len)
{
    printf("%s(%p, %d, %p, %d)\n", __FUNCTION__, connection, is_connected,
           log, len);
    if (is_connected == 0)
        session_state = disconnected;
    ouch_set_connection_flag(is_connected);
}

static int ouch_is_connected(struct exchange_interface *ei)
{
    if (session_state == disconnected)
        return 0;
    else
        return 1;
}

size_t ouch__find_order_token_length(const char *p,
                                     size_t text_field_length)
{
    const char *first_space = memchr(p, ' ', text_field_length);
    if (first_space != 0)
        return first_space - p;
    else
        return text_field_length;
}

static int exchange_interface__get_order_object_by_id(orderbook * book, const char
                                                      *client_order_id,
                                                      size_t length,
                                                      dart_order_obj **
                                                      order_object)
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

static int send_iovec(orderbook * ob, struct iovec *iov, int iovcnt,
                      const char *name, int name_len)
{
    int err = 0;
    int vec_sent = 0;
    int sock = get_sock_for_con(name, name_len);
    if (pthread_mutex_lock(&ob->send_mutex) == 0) {
        if (sock >= 0) {
            while (err == 0) {
                send_bunches(iov, iovcnt, sock, &err, &vec_sent);
            }
            if (err < 0) {
                perror("send_iovec");
                kick_off_connection((char *) name, name_len);
            }
        }
        pthread_mutex_unlock(&ob->send_mutex);
    }
    return err;
}

static void ouch__iovec__record_sent_message(struct databuf *buf,
                                             struct iovec *iov,
                                             size_t iovcnt,
                                             const char *name,
                                             size_t name_len)
{
    databuf_reset(buf);
    size_t i;
    for (i = 0; i < iovcnt; ++i)
        databuf_memcpy(buf, iov[i].iov_base, iov[i].iov_len);
    record_outbound_message(make_message(name, name_len,
                                         buf->rd_ptr,
                                         databuf_unread(buf)));
}

static void iovec_send_wrapper(orderbook * ob, databuf_t * buff,
                               dart_order_obj * doj,
                               const char *name, int name_len)
{
    struct iovec iov[2];
    iov[0].iov_base = "S,,,,,,,,,,,,,,";
    iov[0].iov_len = 15;
    iov[1].iov_base = buff->rd_ptr;
    iov[1].iov_len = databuf_unread(buff);
    send_iovec(ob, iov, 2, name, name_len);
    struct databuf *tmpbuf = get_obj(ob->databufs);
    ouch__iovec__record_sent_message(tmpbuf, iov, 2, name, name_len);
    destroy_elem(ob->databufs, tmpbuf);
    dart_rolling_log_going_iovec(iov, 2, name, name_len, ob->module_name,
                                 ob->module_name_len);
}

static void iovec_send_wrapper2(orderbook * ob, databuf_t * buff,
                               dart_order_obj * doj,
                               const char *name, int name_len)
{
    struct iovec iov[2];
    iov[0].iov_base = "S,,,,,,,";
    iov[0].iov_len = 8;
    iov[1].iov_base = buff->rd_ptr;
    iov[1].iov_len = databuf_unread(buff);
    send_iovec(ob, iov, 2, name, name_len);
    struct databuf *tmpbuf = get_obj(ob->databufs);
    ouch__iovec__record_sent_message(tmpbuf, iov, 2, name, name_len);
    destroy_elem(ob->databufs, tmpbuf);
    dart_rolling_log_going_iovec(iov, 2, name, name_len, ob->module_name,
                                 ob->module_name_len);
}

static int ouch__on_accepted(struct ouch_exchange_interface *ei,
                             struct ouch41__accepted *accepted)
{
    int retval = -1;
    struct ob *book = ei->ei.book;
    struct databuf *status_message = get_obj(book->databufs);
    size_t order_token_length =
        ouch__find_order_token_length(accepted->order_token,
                                      sizeof accepted->order_token);
    if (order_token_length > INT_MAX) {
        goto done;
    }
    dart_order_obj *order_object = 0;
    if (!exchange_interface__get_order_object_by_id
        (book, accepted->order_token, (int) order_token_length,
         &order_object)) {
        ouch__on_order_not_found(accepted, sizeof *accepted,
                                 status_message);
        goto done;
    }
    int sender_name_length;
    char *sender_name =
        get_rom_field(order_object, ROM_SENDER, &sender_name_length);
    if (sender_name_length == 0) {
        goto done;
    }

    ouch_price_int_to_string(accepted->price, status_message);
    databuf_write(status_message, ",,,,,,,");
    /* Field 14 (Status) = 13 (Order Accepted) */
    /* The Accepted message in OUCH can signal that an order has been
     * accepted and is currently open, and it can also signal that an
     * order has been accepted but is now dead. The latter scenario
     * would happen with IOC orders that cannot be filled. */
    if (accepted->order_state == 'L') {
        databuf_memcpy(status_message, "13,", 3);
    } else if (accepted->order_state == 'D') {
        databuf_memcpy(status_message, "4,", 2);
    }

    /* Field 15 (Client ID/ClOrdID) */
    databuf_memcpy(status_message,
                   order_object->data->buffer +
                   order_object->positions[ROM_CLORDID].beg,
                   order_object->positions[ROM_CLORDID].val_len);

    /* Send OUCH Order Reference Number back in DART CSV field 16. */
    databuf_write(status_message, ",%lu,",
                  accepted->order_reference_number);

    /* Add field 17, ROM tag. */
    databuf_memcpy(status_message,
                   order_object->data->buffer +
                   order_object->positions[ROM_TAG].beg,
                   order_object->positions[ROM_TAG].val_len);

    databuf_write(status_message, ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,");
    char timestamp[TIMESTAMP_LEN + 1];
    if (ouch__timestamp_to_string(accepted->timestamp, timestamp,
                                  sizeof timestamp + 1) == 0) {
        databuf_memcpy(status_message, timestamp, TIMESTAMP_LEN);
    }
    databuf_write(status_message, ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n");

    iovec_send_wrapper2(book, status_message, order_object, sender_name,
                       sender_name_length);
    retval = 0;

  done:
    destroy_elem(book->databufs, status_message);
    return retval;
}

void dart_order_obj__copy_if_present(dart_order_obj * dest,
                                     dart_order_obj * src, int fieldnum)
{
    int n;
    char *p = get_rom_field(src, fieldnum, &n);
    if (n > 0) {
        set_rom_field(dest, fieldnum, p, n);
    }
}

/* We keeep extra fields above and beyond the regular DART CSV fields
   that are present. The list of additional fields are: ROM_SENDER,
   ROM_CLORDID, ROM_BIN_CUM. */
static void ouch__apply_replaced_fields(dart_order_obj * order,
                                        dart_order_obj * replace)
{
    /* The general idea here is that we want to take any replacement
       fields from the replace message and apply them to the original
       order. When the replacement was going out we sent the replacement
       fields, back-filling missing replacement fields with those from
       the original order. Here we are just making it official. */
    dart_order_obj__copy_if_present(order, replace, ROM_CLORDID);
    dart_order_obj__copy_if_present(order, replace, ROM_BIN_QTY);
    dart_order_obj__copy_if_present(order, replace, ROM_PRICE);
    dart_order_obj__copy_if_present(order, replace, ROM_TIF);
    /* Display Instructions? */
    dart_order_obj__copy_if_present(order, replace, ROM_MIN_QTY);

    /* Take care of applying changed shares, if any.  This code allows
     * for the rare case that a replace message didn't specify shares
     * thereby not changing them.  In that case the original order's
     * shares are kept and not touched. */
    int n;
    char *p = get_rom_field(replace, ROM_SHARES, &n);
    const char *errmsg = 0;
    if (n > 0) {
        /* Set the shares from the replace in the original order. */
        set_rom_field(order, ROM_SHARES, p, n);

        /* Take care of the binary shares, used to determine when the
         * order is filled. */
        uint32_t binqty;
        if (dart_to_ouch__convert_shares(p, n, &binqty, &errmsg) == 0) {
            set_rom_field(order, ROM_BIN_QTY, &binqty, sizeof binqty);
        } else {
            send_debug_message("%s:%d %s shares='%.*s'", __FILE__,
                               __LINE__,
                               errmsg !=
                               0 ? errmsg :
                               "Cannot convert replacement shares to "
                               "integer", n, p);
            free((char *) errmsg);
        }
    }
}

static int ouch__on_order_canceled(struct ouch_exchange_interface *ei,
                                   struct ouch41__canceled *canceled)
{
    int retval = -1;
    struct ob *book = ei->ei.book;
    struct databuf *status_message = get_obj(book->databufs);
    size_t order_token_length =
        ouch__find_order_token_length(canceled->order_token,
                                      sizeof canceled->order_token);
    if (order_token_length > INT_MAX) {
        goto done;
    }
    dart_order_obj *order_object = 0;
    if (!exchange_interface__get_order_object_by_id
        (book, canceled->order_token, (int) order_token_length,
         &order_object) || order_object == 0) {
        ouch__on_order_not_found(canceled, sizeof *canceled,
                                 status_message);
        goto done;
    }
    int sender_name_length;
    char *sender_name = get_rom_field(order_object, ROM_SENDER,
                                      &sender_name_length);
    if (sender_name_length == 0) {
        send_debug_message("No sender for canceled message on %.*s",
                           (int) order_token_length,
                           canceled->order_token);
        goto done;
    }

    /* Field 14 (Status) = 8 (Reject) */
    databuf_memcpy(status_message, "4,", 2);

    /* Get the order off of our book. */
    if (pthread_mutex_lock(&book->mutex) == 0) {
        remove_from_map(book->cl_ord_id_map,
                        order_object->data->buffer +
                        order_object->positions[ROM_CLORDID].beg,
                        order_object->positions[ROM_CLORDID].val_len);
        remove_from_map(book->rom_ord_id_map,
                        order_object->data->buffer +
                        order_object->positions[ROM_TAG].beg,
                        order_object->positions[ROM_TAG].val_len);
        pthread_mutex_unlock(&book->mutex);
    }

    /* Field 15 (Client ID/ClOrdID) */
    databuf_memcpy(status_message,
                   order_object->data->buffer +
                   order_object->positions[ROM_CLORDID].beg,
                   order_object->positions[ROM_CLORDID].val_len);

    /* Terminate field 15 and add an empty field 16 */
    databuf_memcpy(status_message, ",,", 2);

    /* Add field 17, ROM tag. */
    databuf_memcpy(status_message,
                   order_object->data->buffer +
                   order_object->positions[ROM_TAG].beg,
                   order_object->positions[ROM_TAG].val_len);

    /* Terminate 17 and add empty fields up till 26. */
    databuf_memcpy(status_message, ",,,,,,,,,", 9);

    /* Add a reason code to field 26. */
    ouch41__cancel_reason_code_to_string(status_message, canceled->reason);

    databuf_write(status_message, ",,,,,,,,,,,,,,,,,,,,,,,,");
    char timestamp[TIMESTAMP_LEN + 1];
    if (ouch__timestamp_to_string(canceled->timestamp, timestamp,
                                  sizeof timestamp) == 0)
        databuf_memcpy(status_message, timestamp, TIMESTAMP_LEN);

    databuf_write(status_message, ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n");

    iovec_send_wrapper(book, status_message, order_object, sender_name,
                       sender_name_length);
    retval = 0;

  done:
    destroy_elem(book->databufs, status_message);
    return retval;
}

/* The rep variable of the dart_order_obj points to a dart_order_obj
 * representing the replace Dart message.  If the replaced message
 * indicates the replacement order is dead, we hand the rep variable
 * back to the pool from whence it came, build a replace reject Dart
 * CSV message and send it to ROM.  If the replacement order is live,
 * thus indicating the replacement was successful, we must replace the
 * fields in the original CSV object with the replaced fields in the
 * replacement order, taking into account the fact that some fields
 * may not have been specified in the replacement order and, in that
 * case, should keep the original order's values for those fields.
 * The list of replaceable fields are: order token, shares, price,
 * time in force, display, and minimum quantity.  The binary
 * representation of the shares of the order must be updated as it is
 * used in execution processing to determine if the order is now
 * partially or completely filled. */
static int ouch__on_order_replaced(struct ouch_exchange_interface *ei,
                                   struct ouch41__replaced *replaced)
{
    int retval = -1;
    struct ob *book = ei->ei.book;
    struct databuf *status_message = get_obj(book->databufs);
    size_t previous_order_token_length =
        ouch__find_order_token_length(replaced->previous_order_token,
                                      sizeof
                                      replaced->previous_order_token);
    if (previous_order_token_length > INT_MAX) {
        goto done;
    }
    dart_order_obj *previous_order = 0;
    if (!exchange_interface__get_order_object_by_id
        (book, replaced->previous_order_token,
         (int) previous_order_token_length, &previous_order)) {
        ouch__on_order_not_found(replaced, sizeof *replaced,
                                 status_message);
        goto done;
    }

    if (previous_order->rep == 0) {
        /* The main place this is expected to happen is when the
           process goes down in between sending a replace on the order
           but before the replaced message is received and processed
           by the exchange interface.  There is no way to tell from
           the order object loaded from the database whether there was
           an in-flight replace outstanding.  The best we can do is to
           use the replaced message as the source of the replacement
           fields and move on. */
        /* TODO: Get the fields from the replaced message and move on.
           Also possible: always use the replaced message to figure
           out what's been replaced. */
        send_debug_message
            ("%s:%d %s Replaced message received, do not have a "
             "record of the replace going out", __FILE__, __LINE__,
             __FUNCTION__);
        retval = 0;
        goto done;
    }
    dart_order_obj *replace = previous_order->rep;

    int sender_name_length;
    char *sender_name =
        get_rom_field(previous_order, ROM_SENDER, &sender_name_length);
    if (sender_name_length == 0) {
        send_debug_message("%s:%d Cannot find sender for replaced message "
                           "for order %.*s", __FILE__, __LINE__,
                           (int) previous_order_token_length,
                           replaced->previous_order_token);
        goto done;
    }

    ouch_price_int_to_string(replaced->price, status_message);
    databuf_write(status_message, ",,,,,,,");
    databuf_memcpy(status_message, "5,", 2);    /* Replaced */
    ouch__apply_replaced_fields(previous_order, replace);
    destroy_elem(book->ord_obj, replace);
    previous_order->rep = 0;

    /* Take care of the order token mappings. */
    if (pthread_mutex_lock(&book->mutex) == 0) {
        remove_from_map(book->cl_ord_id_map,
                        replaced->previous_order_token,
                        previous_order_token_length);
        int new_order_token_length =
            ouch__find_order_token_length(replaced->order_token,
                                          sizeof replaced->order_token);
        no_copy_insert(book->cl_ord_id_map, replaced->order_token,
                       new_order_token_length, previous_order);
        pthread_mutex_unlock(&book->mutex);
    }

    /* Field 15 (Client ID/ClOrdID) */
    int n;
    char *p = get_rom_field(previous_order, ROM_CLORDID, &n);
    databuf_memcpy(status_message, p, n);

    /* Terminate field 15 and add an empty field 16 */
    databuf_write(status_message, ",%lu,",
                  replaced->order_reference_number);

    /* Add field 17, ROM tag. */
    p = get_rom_field(previous_order, ROM_TAG, &n);
    databuf_memcpy(status_message, p, n);

    /* Adding OMExecTime  */
    databuf_write(status_message, ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,");
    databuf_reserve(status_message, TIMESTAMP_LEN);
    ouch__timestamp_to_string(replaced->timestamp, status_message->wr_ptr,
                              databuf_space_left(status_message));
    status_message->wr_ptr += TIMESTAMP_LEN;

    /* Finish the message. */
    databuf_write(status_message, ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n");

    iovec_send_wrapper2(book, status_message, previous_order, sender_name,
                       sender_name_length);

    if (replaced->order_state == 'D') {
        /* Under all circumstances, receiving a Replaced message truly
           indicates that the order has been replaced.  However, if the
           Order State field is now 'D' for Dead, the order will have been
           replaced but is now dead.  To reuse the code we've already
           written for handling canceled orders, and since a canceled
           message is so easy to construct, we create a fake one and pass
           it into the on_canceled handler. */
        struct ouch41__canceled canceled = {
            .type = 'C',
            .timestamp = replaced->timestamp,
            .decrement_shares = replaced->shares,
            .reason = 'U',
        };
        memcpy(canceled.order_token, replaced->order_token,
               sizeof canceled.order_token);
        ouch__on_order_canceled(ei, &canceled);
    }

    retval = 0;

  done:
    destroy_elem(book->databufs, status_message);
    return retval;
}

/* A rejected message can be sent in response to either an order or a
 * replace message. */
int ouch__on_order_reject(struct ouch_exchange_interface *ei,
                          struct ouch41__rejected *rejected)
{
    int retval = -1;
    struct ob *book = ei->ei.book;
    struct databuf *status_message = get_obj(book->databufs);
    size_t order_token_length =
        ouch__find_order_token_length(rejected->order_token,
                                      sizeof rejected->order_token);
    if (order_token_length > INT_MAX) {
        goto done;
    }
    dart_order_obj *order_object = 0;
    if (!exchange_interface__get_order_object_by_id
        (book, rejected->order_token, (int) order_token_length,
         &order_object)) {
        ouch__on_order_not_found(rejected, sizeof *rejected,
                                 status_message);
        goto done;
    }

    /* Determine whether this is a reject of an order or a reject of a
     * replace. */
    int is_order_reject = 1;
    if (order_object->rep != 0) {
        int replace_id_len;
        char *replace_id = get_rom_field(order_object->rep, ROM_CLORDID,
                                         &replace_id_len);
        if (replace_id != 0 && order_token_length == replace_id_len
            && strncmp(replace_id, rejected->order_token,
                       order_token_length) == 0)
            is_order_reject = 0;
    }

    int sender_name_length;
    char *sender_name = get_rom_field(order_object, ROM_SENDER,
                                      &sender_name_length);
    if (sender_name_length == 0) {
        send_debug_message("%s:%d (%s) No sender for reject on %.*s",
                           __FILE__, __LINE__, __FUNCTION__,
                           (int) order_token_length,
                           rejected->order_token);
        goto done;
    }

    /* Field 14 (Status) = 8 (Reject) */
    if (is_order_reject)
        databuf_memcpy(status_message, "8,", 2);
    else
        databuf_memcpy(status_message, "20,", 3);

    char *p;
    int n;

    /* Get the order off of our book. */
    if (is_order_reject) {
        if (pthread_mutex_lock(&book->mutex) == 0) {
            p = get_rom_field(order_object, ROM_CLORDID, &n);
            remove_from_map(book->cl_ord_id_map, p, n);
            p = get_rom_field(order_object, ROM_TAG, &n);
            remove_from_map(book->rom_ord_id_map, p, n);
            pthread_mutex_unlock(&book->mutex);
        }
    } else {
        destroy_elem(book->databufs, order_object->rep);
        order_object->rep = 0;
    }

    /* Field 15 (Client ID/ClOrdID) */
    p = get_rom_field(order_object, ROM_CLORDID, &n);
    databuf_memcpy(status_message, p, n);

    /* Terminate field 15 and add an empty field 16 */
    databuf_memcpy(status_message, ",,", 2);

    /* Add field 17, ROM tag. */
    p = get_rom_field(order_object, ROM_TAG, &n);
    databuf_memcpy(status_message, p, n);

    /* Terminate 17 and add empty fields up till 26. */
    databuf_memcpy(status_message, ",,,,,,,,,", 9);

    /* Add a reason code to field 26. */
    ouch41__reject_reason_code_to_string(status_message, rejected->reason);

    /* Add the execution time. */
    databuf_write(status_message, ",,,,,,,,,,,,,,,,,,,,,,,,");
    databuf_reserve(status_message, TIMESTAMP_LEN);
    ouch__timestamp_to_string(rejected->timestamp, status_message->wr_ptr,
                              databuf_space_left(status_message));
    status_message->wr_ptr += TIMESTAMP_LEN;

    /* Finish the status message. */
    databuf_memcpy(status_message, ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,"
                   ",,,,,,,,,,,,,,,,,,,,\n", 55);

    iovec_send_wrapper(book, status_message, order_object, sender_name,
                       sender_name_length);

    retval = 0;

  done:
    destroy_elem(book->databufs, status_message);
    return retval;
}

static void ouch__summarize_execution(struct ouch41__executed *executed,
                                      struct databuf *buffer)
{
    databuf_write(buffer, "OUCH Execution occurred at ");
    char timestamp[TIMESTAMP_LEN + 1];
    if (ouch__timestamp_to_string(executed->timestamp, timestamp,
                                  sizeof timestamp) == 0)
        databuf_memcpy(buffer, timestamp, TIMESTAMP_LEN);
    else
        databuf_write(buffer, "(invalid timestamp)");
    databuf_write(buffer, "\nOrder ID when sent to NASDAQ: %.*s\n",
                  (int) sizeof executed->order_token,
                  executed->order_token);
    databuf_write(buffer, "Side and symbol not provided by NASDAQ, %u@",
                  executed->executed_shares);
    ouch_price_int_to_string(executed->execution_price, buffer);
    databuf_write(buffer, "\nLiquidity code %c, Execution ID %llu\n",
                  executed->liquidity_flag, executed->match_number);
}

static int ouch__on_order_executed(struct ouch_exchange_interface *ei,
                                   struct ouch41__executed *executed)
{
    int retval = -1;
    struct ob *book = ei->ei.book;
    size_t order_token_length =
        ouch__find_order_token_length(executed->order_token,
                                      sizeof executed->order_token);
    if (order_token_length > INT_MAX)
        return -1;
    dart_order_obj *book_order = 0;
    if (!exchange_interface__get_order_object_by_id
        (book, executed->order_token, (int) order_token_length,
         &book_order)) {
        send_debug_message
            ("%s:%d (%s): No order on book for order executed message %.*s",
             __FILE__, __LINE__, __FUNCTION__, (int) order_token_length,
             executed->order_token);
        struct databuf *mail_message = get_obj(book->databufs);
        ouch__summarize_execution(executed, mail_message);
        const char *title = "Unmatched fill from NASDAQ OUCH";
        send_biz_message(mail_message->rd_ptr,
                         databuf_unread(mail_message), title,
                         strlen(title));
        destroy_elem(book->databufs, mail_message);
        goto done;
    }
    jumpassert(book_order != 0, done);
    struct databuf *status_message = get_obj(book->databufs);
    int sender_name_length;
    char *sender_name = get_rom_field(book_order, ROM_SENDER,
                                      &sender_name_length);
    if (sender_name_length == 0) {
        send_debug_message
            ("Could not find sender name in order object for order ID %.*s",
             (int) order_token_length, executed->order_token);
    }

    /* Determine if this execution has completely filled the order. */
    const char *ordstatus = "1,";
    uint32_t *ord_qty = (uint32_t *) csvptr(book_order, ROM_BIN_QTY);
    uint32_t *cum_qty = (uint32_t *) csvptr(book_order, ROM_BIN_CUM);
    send_debug_message("%.*s: cum_qty=%u ord_qty=%d last_qty=%u",
                       (int) order_token_length, executed->order_token,
                       *cum_qty, *ord_qty, executed->executed_shares);
    *cum_qty += executed->executed_shares;
    if (*cum_qty >= *ord_qty) {
        ordstatus = "2,";

        /* The order is filled, get it off of our book. */
        if (pthread_mutex_lock(&book->mutex) == 0) {
            remove_from_map(book->cl_ord_id_map,
                            csvptr(book_order, ROM_CLORDID),
                            csvlen(book_order, ROM_CLORDID));
            remove_from_map(book->rom_ord_id_map,
                            csvptr(book_order, ROM_TAG), csvlen(book_order,
                                                                ROM_TAG));
            pthread_mutex_unlock(&book->mutex);
        }
    }

    /* Field 14 (Status), Partially Filled (1) or Filled (2) */
    databuf_memcpy(status_message, ordstatus, 2);

    /* Field 15 (Client ID/ClOrdID) */
    databuf_memcpy(status_message, csvptr(book_order, ROM_CLORDID),
                   csvlen(book_order, ROM_CLORDID));

    /* Terminate field 15 and add an empty field 16 */
    databuf_memcpy(status_message, ",,", 2);

    /* Add field 17, ROM tag. */
    databuf_memcpy(status_message, csvptr(book_order, ROM_TAG),
                   csvlen(book_order, ROM_TAG));

    /* 42: executed shares */
    databuf_write(status_message, ",,,,,,,,,,,,,,,,,,,,,,,,,%u,,",
                  executed->executed_shares);

    /* 44: execution price */
    ouch_price_int_to_string(executed->execution_price, status_message);

    /* 50: Execution time */
    databuf_write(status_message, ",,,,,,");
    char timestamp[TIMESTAMP_LEN + 1];
    if (ouch__timestamp_to_string(executed->timestamp, timestamp,
                                  sizeof timestamp) == 0) {
        databuf_memcpy(status_message, timestamp, TIMESTAMP_LEN);
    }

    /* 54: execution tag */
    databuf_write(status_message, ",,,,%lu", executed->match_number);

    /* 69: billing/liquidity code */
    databuf_write(status_message, ",,,,,,,,,,,,,,,%c,,,,,,,,,,,\n",
                  executed->liquidity_flag);

    iovec_send_wrapper(book, status_message, book_order, sender_name,
                       sender_name_length);
    retval = 0;

  done:
    destroy_elem(book->databufs, status_message);
    return retval;
}

static int ouch__on_price_correction(struct ouch_exchange_interface *oei, struct ouch41__price_correction
                                     *correction)
{
    /* Price corrections are reported via email */
    struct databuf *msg = get_obj(oei->ei.book->databufs);
    const char *title = "Price correction from NASDAQ OUCH";
    databuf_write(msg, "OUCH Price Correction occurred at ");
    char timestamp[TIMESTAMP_LEN + 1];
    if (ouch__timestamp_to_string(correction->timestamp, timestamp,
                                  sizeof timestamp) == 0)
        databuf_memcpy(msg, timestamp, TIMESTAMP_LEN);
    else
        databuf_write(msg, "(invalid timestamp)");
    databuf_write(msg, "\nOrder ID when sent to NASDAQ: %.*s\n",
                  (int) sizeof correction->order_token,
                  correction->order_token);
    databuf_write(msg, "Match number: %llu\n", correction->match_number);
    databuf_write(msg, "New execution price: ");
    ouch_price_int_to_string(correction->new_execution_price, msg);
    const char *reason = "unknown";
    switch (correction->reason) {
    case 'E':
        reason = "Erroneous: The trade was deemed clearly erroneous.";
        break;
    case 'C':
        reason = "Consent: The two parties mutually agreed to correct the "
            "price.";
        break;
    case 'S':
        reason =
            "Supervisory: The trade was manually price-corrected by a "
            "NASDAQ supervisory terminal.";
        break;
    case 'X':
        reason = "External: The trade was price-corrected by an external "
            "third party.";
    }
    databuf_write(msg, "Reason: (%c) %s\n", correction->reason, reason);
    send_biz_message(msg->rd_ptr, databuf_unread(msg), title,
                     strlen(title));
    destroy_elem(oei->ei.book->databufs, msg);
    return 0;
}

static int ouch__on_broken_trade(struct ouch_exchange_interface *oei,
                                 struct ouch41__broken_trade *bust)
{
    /* Broken trades are reported via email. */
    struct databuf *msg = get_obj(oei->ei.book->databufs);
    const char *title = "Trade bust from NASDAQ OUCH";
    databuf_write(msg, "OUCH Trade Bust occurred at ");
    char timestamp[TIMESTAMP_LEN + 1];
    if (ouch__timestamp_to_string(bust->timestamp, timestamp,
                                  sizeof timestamp) == 0)
        databuf_memcpy(msg, timestamp, TIMESTAMP_LEN);
    else
        databuf_write(msg, "(invalid timestamp)");
    databuf_write(msg, "\nOrder ID when sent to NASDAQ: %.*s\n",
                  (int) sizeof bust->order_token, bust->order_token);
    databuf_write(msg, "Match number: %llu\n", bust->match_number);
    const char *reason = "unknown";
    switch (bust->reason) {
    case 'E':
        reason = "Erroneous: The trade was deemed clearly erroneous.";
        break;
    case 'C':
        reason = "Consent: The two parties mutually agreed to break the "
            "trade";
        break;
    case 'S':
        reason = "Supervisory: The trade was manually broken by a NASDAQ "
            "supervisory terminal.";
        break;
    case 'X':
        reason = "External: The trade was broken by an external third "
            "party.";
    }
    databuf_write(msg, "Reason: (%c) %s\n", bust->reason, reason);
    send_biz_message(msg->rd_ptr, databuf_unread(msg), title,
                     strlen(title));
    destroy_elem(oei->ei.book->databufs, msg);
    return 0;
}

/* The socket is connected, a login request sent and a login accepted
 * message received. */
static int ouch_connected_state(struct ouch_exchange_interface *oei,
                                const struct soupbin_header *header,
                                int *cut_connection,
                                struct parse_con *connection_parse_info)
{
    int retval = -1;
    const uint16_t nouchbytes = ntohs(header->packet_length) - 1;
    if (header->packet_type == SOUPBIN_PACKET_SEQUENCED_DATA) {
        /* Log this incoming message */
        record_incoming2(connection_parse_info);

        void *p = (void *) header;
        p += sizeof *header;
        retval = ouch41__deserialize(p, nouchbytes);
        char ouchtype = ((char *) p)[0];
        switch (ouchtype) {
        case 'J':              /* Rejected */
            {
                struct ouch41__rejected *rejected = p;
                retval = ouch__on_order_reject(oei, rejected);
            }
            break;
        case 'A':
            {
                struct ouch41__accepted *accepted = p;
                retval = ouch__on_accepted(oei, accepted);
            }
            break;
        case 'C':
            {
                struct ouch41__canceled *canceled = p;
                retval = ouch__on_order_canceled(oei, canceled);
            }
            break;
        case 'E':
            {
                struct ouch41__executed *executed = p;
                retval = ouch__on_order_executed(oei, executed);
            }
            break;
        case 'U':
            {
                struct ouch41__replaced *replaced = p;
                retval = ouch__on_order_replaced(oei, replaced);
            }
            break;
        case 'K':
            {
                struct ouch41__price_correction *price_correction = p;
                retval = ouch__on_price_correction(oei, price_correction);
            }
            break;
        case 'B':
            {
                struct ouch41__broken_trade *bust = p;
                retval = ouch__on_broken_trade(oei, bust);
            }
            break;
        case 'S':
            {
                struct ouch41__system_event *sysevent = p;
                const char *typestr = "Unknown";
                if (sysevent->event_code == 'S') {
                    typestr = "Start of Day";
                } else if (sysevent->event_code == 'E') {
                    typestr = "End of Day";
                }
                char timestampbuffer[TIMESTAMP_LEN + 1];
                if (ouch__timestamp_to_string
                    (sysevent->timestamp, timestampbuffer,
                     sizeof timestampbuffer) != -1) {
                    timestampbuffer[sizeof timestampbuffer - 1] = '\0';
                    send_debug_message("%s OUCH System Event %c: %s",
                                       timestampbuffer,
                                       sysevent->event_code, typestr);
                }
            }
        }
    } else {
        /* Ignore the rest for now */
    }
    return retval;
}

/* The OUCH session is currently disconnected. A socket connection has
 * been made and a login request has been sent. */
static int ouch_disconnected_state(struct ouch_exchange_interface *oei,
                                   const struct soupbin_header *header,
                                   int *cut_connection,
                                   struct parse_con *connection_parse_info)
{
    int retval = -1;
    if (header->packet_type == SOUPBIN_PACKET_LOGIN_ACCEPTED) {
        oei->evaluate = ouch_connected_state;
        session_state = session_established;
        oei->ei.book->can_send_orders = 1;
        retval = 0;
    } else {
        *cut_connection = 1;
        oei->ei.book->can_send_orders = 0;
    }
    return retval;
}

static unsigned long ouch_frame(const char *data, unsigned long nbytes,
                                int *cut_connection,
                                struct parse_con *connection_parse_info)
{
    const char *p = data;
    const char *end = data + nbytes;
    while (p < end) {
        struct soupbin_header *header = (struct soupbin_header *) p;
        if (nbytes < sizeof *header) {
            break;
        }
        uint16_t packet_size =
            ntohs(header->packet_length) + sizeof header->packet_length;
        if (nbytes < packet_size) {
            break;
        }
        struct ouch_exchange_interface *oei = connection_parse_info->book;
        if (header->packet_type != 'H') {
            struct databuf *log_buffer = get_obj(oei->ei.book->databufs);
            if (soupbin__dump_data
                ((void *) p, packet_size, log_buffer,
                 ouch41__dump_message) == -1) {
                databuf_write(log_buffer, "%s:%d Error dumping packet",
                              __FILE__, __LINE__);
            }
            struct message log_message = {
                .name = (char *) get_name(connection_parse_info->co),
                .name_len = get_name_len(connection_parse_info->co),
                .data = log_buffer->rd_ptr,
                .len = databuf_unread(log_buffer),
            };
            dart_rolling_log_coming(&log_message,
                                    oei->ei.book->module_name,
                                    oei->ei.book->module_name_len);
            free(log_message.name);
            destroy_elem(oei->ei.book->databufs, log_buffer);
        }
        oei->evaluate(oei, header, cut_connection, connection_parse_info);
        p += packet_size;
    }
    return p - data;
}

static void ouch_on_message(struct message *message, void *token)
{
    printf("%s\n", __FUNCTION__);
}

static void ouch_on_idle(con_obj * connection)
{
    printf("%s\n", __FUNCTION__);
}

static unsigned long ouch_parse_sequence(char *p, unsigned long n,
                                         unsigned long j, unsigned long *k)
{
    printf("%s\n", __FUNCTION__);
    return 0;
}

static char *ouch_reset_pad(unsigned long i, char *p, size_t * n)
{
    printf("%s\n", __FUNCTION__);
    return 0;
}

static int ouch_on_connection_established(struct parse_args *parse_info)
{
    printf("%s\n", __FUNCTION__);

    struct parse_con *connectionparser = parse_info->con;
    size_t namelength = get_name_len(connectionparser->co);
    const char *connectionname = get_name(connectionparser->co);
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

    struct ouch_exchange_interface *ei =
        (struct ouch_exchange_interface *) parse_info->con->book;
    ei->ei.client_connection.connection = parse_info->con->co;
    struct soupbin_login *login = &(ei->login);

    /* Set the sequence number */
    if (soupbin__login__set_sequence_number(login, in_seqno) == -1) {
        send_debug_message("%s:%d ", __FILE__, __LINE__);
        return -1;
    }

    databuf_t *buffer = get_obj(ei->ei.book->databufs);
    databuf_memcpy(buffer, login, sizeof *login);
    char *connection_name = (char *) get_name(parse_info->con->co);
    const size_t connection_name_length =
        get_name_len(parse_info->con->co);
    ouch__send_message(&ei->ei.book->send_mutex, buffer->rd_ptr,
                       databuf_unread(buffer), connection_name,
                       connection_name_length);
    struct databuf *log_buffer = get_obj(ei->ei.book->databufs);
    if (soupbin__dump_data(buffer->rd_ptr, databuf_unread(buffer),
                           log_buffer, ouch41__dump_message) != -1) {
        struct message log_message = {
            .name = connection_name,
            .name_len = connection_name_length,
            .data = log_buffer->rd_ptr,
            .len = databuf_unread(log_buffer),
        };
        dart_rolling_log_going(&log_message, ei->ei.book->module_name,
                               ei->ei.book->module_name_len);
    }
    destroy_elem(ei->ei.book->databufs, log_buffer);
    free(connection_name);
    session_state = login_sent;
    destroy_elem(ei->ei.book->databufs, buffer);
    return 0;
}

static struct ouch_exchange_interface *ouch_exchange_interface_create(void)
{
    printf("%s\n", __FUNCTION__);
    struct ouch_exchange_interface *ouch = malloc(sizeof *ouch);
    exchange_interface__init(&(ouch->ei));
    ouch->ei.configure = ouch_configure;
    ouch->ei.listen = exchange_interface_listen;
    exchange_interface_rom_connection_init(&(ouch->ei.server_connection));
    ouch->ei.server_connection.on_message_using_stream_state =
        ouch__on_message_using_parser_helper;
    ouch->ei.run = ouch_run;
    ouch->ei.get_stop_time = exchange_interface_get_stop_time;
    ouch->ei.is_connected = ouch_is_connected;
    ouch->ei.send_heartbeat = ouch_send_heartbeat;
    ouch->ei.connect = ouch_connect;
    ouch->evaluate = ouch_disconnected_state;
    ouch->ei.on_enter_order = ouch__enter_order;
    struct exchange_interface_connection *connection =
        &(ouch->ei.client_connection);
    connection->on_connection_update = ouch_on_connection_update;
    connection->frame_message = ouch_frame;
    connection->on_message = ouch_on_message;
    connection->on_idle = ouch_on_idle;
    connection->parse_sequence = ouch_parse_sequence;
    connection->create_reset_pad = ouch_reset_pad;
    connection->on_connection_established = ouch_on_connection_established;
    connection->check_idle_flag = 0;
    connection->heartbeat_interval = 1;
    return ouch;
}

#define handle_errorcode(errorcode, msg)  \
    do { errno = errorcode; perror(msg); exit(EXIT_FAILURE); } while (0)

/* Returns 0 => success, 1 => bad arguments, 2 => runtime error */
int main(int argc, char *argv[])
{
    int retval = 0;
    if (argc > 1) {
        struct exchange_interface *exch =
            (struct exchange_interface *) ouch_exchange_interface_create();
        if ((retval = exch->configure(exch, argv[1])) == 0
            && (retval = exch->listen(exch)) == 0) {
            retval = exch->run(exch);
        }
    } else {
        fprintf(stderr, "usage: ouch conf-file\n");
        retval = 1;
    }
    if (retval == -1) {
        retval = 2;
    }
    return retval;
}
