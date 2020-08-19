/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <arpa/inet.h>

#include "cx_object.h"
#include "dart_constants.h"
#include "databuf.h"
#include "rb_tree.h"
#include "hashmap.h"
#include "mailer.h"

#define MILLI_HOUR 3600000
#define MILLI_MIN  60000
#define MILLI_SEC  1000


struct cx_con_obj {
    char *userid;
    char *password;
    int sessionid;
    int sequence;
    int cl_ordid;
    pthread_mutex_t cx_seq_blocker;
    pthread_mutex_t cx_map_blocker;
    pthread_mutex_t cx_cond_mux;
    pthread_cond_t cx_cond;
    size_t end_hour;
    size_t end_min;
    int disconnect;
    int is_connected;
    time_t last_mess_sec;
    con_obj *connection;
    dart_obj_manager *mgr;
    hashmap_t rom_map;
    hashmap_t symbols;
    rb_tree_t *ouch_tree;
    char *mod_name;
    int mod_name_len;
    void (*send_to_rom) (char *message, int len, char *rom, int rom_len);
};

int is_cx_connected(cx_obj * c)
{
    return c->is_connected;
}

char *cx_get_mod_name(cx_obj * c, int *mod_len)
{
    *mod_len = c->mod_name_len;
    return c->mod_name;
}

void set_cx_con_obj(cx_obj * cx, con_obj * co)
{
    cx->connection = co;
}

static char *get_currenex_cnl_code(char *off)
{
    short ec = 0;
    memcpy(&ec, off, 2);
    ec = ntohs(ec);
    switch (ec) {
    case 0:
        return "User cancel";
    case 1:
        return "System cancel";
    case 2:
        return "Below minimum cancel";
    default:
        return "Unknown";
    };
}

static char *get_currenex_ec(char *off)
{
    short ec = 0;
    memcpy(&ec, off, 2);
    ec = ntohs(ec);
    switch (ec) {
    case 0:
        return "Confirmed";
    case 1:
        return "Invalid Instrument";
    case 2:
        return "Invalid Side";
    case 3:
        return "Invalid Price";
    case 4:
        return "Invalid Expiry";
    case 5:
        return "Invalid Amount";
    case 6:
        return "Invalid Show Amount";
    case 7:
        return "Invalid Permission";
    case 8:
        return "Invalid Order";
    case 9:
        return "Invalid Order Type";
    case 10:
        return "Invalid Client Order Id";
    case 11:
        return "Invalid Credit";
    case 12:
        return "Invalid New Client Order Id";
    case 14:
        return "Order Not Active";
    case 15:
        return "Invalid Specified Amount";
    case 16:
        return "Maximum Number of Active Orders Exceeded";
    case 17:
        return "Rate precision error";
    case 18:
        return "Invalid Error";
    default:
        return "Unknown Error";

    }
}

static void send_from_rolling_log(cx_obj * obj, const char *data, int len,
                                  char type, int cl_ord)
{
    struct message mess;
    databuf_t *out = databuf_alloc(200);
    switch (type) {
    case 'R':
        databuf_write(out, "CX sent order cancelled: %d", cl_ord);
        break;
    case 'M':
        databuf_write(out, "CX sent order open: %d", cl_ord);
        break;
    case 'Q':
        databuf_write(out, "CX send order replaced: %d", cl_ord);
        break;
    case 'A':
        databuf_write(out, "Logged on to CX OUCH.");
        break;
    case 'D':
        databuf_write(out, "Instrument info from CX OUCH: ");
        databuf_memcpy(out, data, len);
        break;
    default:
        databuf_memcpy(out, data, len);
        break;
    }
    mess.data = out->buffer;
    mess.len = databuf_unread(out);
    mess.name = "CXOUCH";
    mess.name_len = 6;
    dart_rolling_log_coming(&mess, obj->mod_name, obj->mod_name_len);
    databuf_destroy(out);
}

static void send_to_rolling_log(cx_obj * obj, const char *data, int len,
                                char type, int cl_ord)
{
    struct message mess;
    databuf_t *out = databuf_alloc(200);
    switch (type) {
    case 'R':
        databuf_write(out, "CX sent order cancelled: %d", cl_ord);
        break;
    case 'M':
        databuf_write(out, "CX sent order open: %d", cl_ord);
        break;
    case 'Q':
        databuf_write(out, "CX send order replaced: %d", cl_ord);
        break;
    case 'A':
        databuf_write(out, "Logged on to CX OUCH.");
        break;
    case 'D':
        databuf_write(out, "Instrument info from CX OUCH: ");
        databuf_memcpy(out, data, len);
        break;
    default:
        break;
    }
    mess.data = out->buffer;
    mess.len = databuf_unread(out);
    mess.name = "CXOUCH";
    mess.name_len = 6;
    dart_rolling_log_going(&mess, obj->mod_name, obj->mod_name_len);
    databuf_destroy(out);
}

static void rom_reject_helper(cx_obj * obj, const char *message,
                              const char *ret_status,
                              dart_ex_order_obj * ord)
{
    size_t sender_len = 0;
    char *sender = doa_get_sender_name(ord, &sender_len);
    size_t rom_len = 0;
    char *rom = doa_get_rom_id(ord, &rom_len);
    databuf_t *output = databuf_alloc(100);
    databuf_write(output, "S,,,,,,,,,,,,,,%s,,,", ret_status);
    databuf_memcpy(output, rom, rom_len);
    databuf_write(output, ",,,,,,,,,%s,,,,,,,,,,,,,,,,,,,,\n", message);
    obj->send_to_rom(output->buffer, databuf_unread(output), sender,
                     sender_len);
    databuf_destroy(output);
    if (sender) {
        free(sender);
        sender = 0;
    }
    if (rom) {
        free(rom);
        rom = 0;
    }
}

void add_order_to_maps(cx_obj * obj, dart_ex_order_obj * ord,
                       int cl_ord_id)
{
    pthread_mutex_lock(&obj->cx_map_blocker);
    int *key = malloc(sizeof(int));
    size_t rom_len = 0;
    char *rom = NULL;
    *key = cl_ord_id;
    rb_tree_insert(obj->ouch_tree, key, ord);
    rom = doa_get_rom_id(ord, &rom_len);
    no_copy_insert(obj->rom_map, rom, rom_len, ord);
    free(rom);
    pthread_mutex_unlock(&obj->cx_map_blocker);
}

void add_new_id_to_tree(cx_obj * obj, dart_ex_order_obj * ord,
                        int cl_ord_id)
{
    pthread_mutex_lock(&obj->cx_map_blocker);
    int *key = malloc(sizeof(int));
    *key = cl_ord_id;
    rb_tree_no_delete_insert(obj->ouch_tree, key, ord);
    pthread_mutex_unlock(&obj->cx_map_blocker);
}

dart_ex_order_obj *get_ord_from_tree(cx_obj * obj, int cl_ord)
{
    dart_ex_order_obj *ord = 0;
    pthread_mutex_lock(&obj->cx_map_blocker);
    ord = rb_tree_get_val(obj->ouch_tree, &cl_ord);
    pthread_mutex_unlock(&obj->cx_map_blocker);
    return ord;
}

void remove_from_tree(cx_obj * obj, int cl_ord)
{
    pthread_mutex_lock(&obj->cx_map_blocker);
    rb_tree_delete(obj->ouch_tree, &cl_ord);
    pthread_mutex_unlock(&obj->cx_map_blocker);
}

static void remove_from_cx_map(cx_obj * obj, char *rom, size_t rom_len)
{
    pthread_mutex_lock(&obj->cx_map_blocker);
    remove_from_map(obj->rom_map, rom, rom_len);
    pthread_mutex_unlock(&obj->cx_map_blocker);
}

int cx_get_obj_w_rom(cx_obj * c, char *rom_tag, size_t len, void **obj)
{
    int ret_code = 0;
    pthread_mutex_lock(&c->cx_map_blocker);
    ret_code = get_map_entry(c->rom_map, rom_tag, len, obj);
    pthread_mutex_unlock(&c->cx_map_blocker);
    return ret_code;
}

void *cx_get_dart_ord_obj(cx_obj * c)
{
    return get_empty_dart_order_obj(c->mgr);
}

void cx_int_destroy(void *a)
{
    free((int *) a);
}

int cx_int_compare(const void *a, const void *b)
{
    if (*(int *) a > *(int *) b) {
        return 1;
    }
    if (*(int *) a < *(int *) b) {
        return -1;
    }
    return 0;
}

void cx_int_print(const void *a)
{
    printf("%d", *(int *) a);
}

void cx_info_print(const void *a)
{
    printf("Printing the object");
}

void cx_info_destroy(void *a)
{
    destroy_dart_order((dart_ex_order_obj *) a);
}

cx_obj *create_cx_obj(const char *userid, int user_len,
                      const char *password, int pass_len,
                      int session_id, size_t end_hour,
                      size_t end_min, int disconnect, char *mod_name,
                      int mod_name_len,
                      con_obj * obj, void (*send_to_rom) (char *message,
                                                          int len,
                                                          char *rom,
                                                          int rom_len))
{
    cx_obj *cx = malloc(sizeof(struct cx_con_obj));
    if (cx != NULL) {
        time_t rawtime;
        time(&rawtime);
        cx->userid = calloc(user_len + 1, 1);
        memcpy(cx->userid, userid, user_len);

        cx->mgr = startup_allocator(50000);
        cx->password = calloc(pass_len + 1, 1);
        memcpy(cx->password, password, pass_len);
        cx->cl_ordid = 1;
        cx->sequence = 1;
        cx->sessionid = session_id;
        cx->end_hour = end_hour;
        cx->end_min = end_min;
        cx->disconnect = disconnect;
        cx->is_connected = 0;
        cx->connection = obj;
        cx->send_to_rom = send_to_rom;
        cx->last_mess_sec = rawtime;
        cx->rom_map = create_map(20000);
        cx->symbols = create_map(100);
        cx->mod_name = mod_name;
        cx->mod_name_len = mod_name_len;
        cx->ouch_tree =
            create_rb_tree(cx_int_compare, cx_int_destroy, cx_info_destroy,
                           cx_int_print, cx_info_print);
        if (pthread_mutex_init(&cx->cx_seq_blocker, NULL) != 0) {
            printf("Failed to init fix_seq_blocker.\n");
        }
        if (pthread_mutex_init(&cx->cx_map_blocker, NULL) != 0) {
            printf("Failed to init fix_map_blocker.\n");
        }
        if (pthread_mutex_init(&cx->cx_cond_mux, NULL) != 0) {
            printf("Failed to init cx_cond_mux.\n");
        }
        if (pthread_cond_init(&cx->cx_cond, NULL) != 0) {
            printf("Failed to init conditional.\n");
        }
    }
    return cx;
}

void destroy_cx_obj(cx_obj * obj)
{
    free(obj->userid);
    free(obj->password);
    rb_tree_destroy(obj->ouch_tree);
    delete_map(obj->rom_map);
    delete_map(obj->symbols);
    clean_dart_oo_pool(obj->mgr);
}

void *run_cx_loop(void *vobj)
{
    cx_obj *obj = (cx_obj *) vobj;
    time_t now = 0;
    struct timespec timeout;
    struct tm *newtime;
    int status;
    int keep_running;
    size_t l_timeout_val = 5;
    keep_running = 1;
    while (keep_running) {
        time_t timmy;
        time(&timmy);
        timeout.tv_sec = timmy + l_timeout_val;
        timeout.tv_nsec = 0;
        status = pthread_mutex_lock(&obj->cx_cond_mux);
        if (status != 0) {
            printf("Unable to get the cond lock!\n");
        }
        if (obj->disconnect) {
            keep_running = 0;
            pthread_mutex_unlock(&obj->cx_cond_mux);
        } else {
            status =
                pthread_cond_timedwait(&obj->cx_cond, &obj->cx_cond_mux,
                                       &timeout);
            pthread_mutex_unlock(&obj->cx_cond_mux);
            if (status == ETIMEDOUT) {
                time(&now);
                struct tm lm;
                newtime = localtime_r(&now, &lm);
                newtime = &lm;
                if (newtime->tm_hour == obj->end_hour &&
                    newtime->tm_min >= obj->end_min) {
                    keep_running = 0;
                } else {
                    send_seq_cx_message(obj, 'C', NULL);
                }
            } else if (obj->disconnect) {
                keep_running = 0;
            }
        }
    }
    pthread_exit(NULL);
    return NULL;
}

void run_cx_admin_loop(cx_obj * obj)
{
    pthread_attr_t thread_attr;
    pthread_t cx_admin_thread;
    int status = 0;
    status = pthread_attr_init(&thread_attr);
    if (status != 0) {
        printf("Pthread attr init failed! \n");
    }
    status =
        pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    if (status != 0) {
        printf("Thread set detached failed!\n");
    }
    pthread_create(&cx_admin_thread, &thread_attr, &run_cx_loop, obj);
}

static int get_sequence(cx_obj * obj)
{
    int ret_seq = 0;
    ret_seq = obj->sequence;
    ++obj->sequence;
    return ret_seq;
}

static void roll_back_sequence(cx_obj * obj)
{
    --obj->sequence;
}

static int get_cl_ord_id(cx_obj * obj)
{
    int ret_id = obj->cl_ordid;
    ++obj->cl_ordid;
    return ret_id;
}

static int generate_cx_timestamp(cx_obj * obj)
{
    time_t rawtime;
    struct tm *ti;
    int cx_millis = 0;
    time(&rawtime);
    struct tm lm;
    ti = gmtime_r(&rawtime, &lm);
    ti = &lm;
    obj->last_mess_sec = rawtime;
    cx_millis = (ti->tm_hour * MILLI_HOUR) + (ti->tm_min * MILLI_MIN) +
        (ti->tm_sec * MILLI_SEC);
    return cx_millis;
}

void rom_to_cx_amount(databuf_t * buff, double qty)
{
    long new_qty = (qty * 100) + .00001;
    new_qty = htonll(new_qty);
    databuf_memcpy(buff, (char *) &new_qty, 8);
}

void cx_double_to_char_amount(databuf_t * buff, double amt)
{
    databuf_write(buff, "%.02f", amt);
}

void cx_double_to_char_price(databuf_t * buff, double price)
{
    databuf_write(buff, "%.06f", price);
}

double cx_to_rom_amount(char *data)
{
    long amount = 0;
    double d = 0;
    memcpy(&amount, data, 8);
    amount = ntohll(amount);
    d = (((double) amount) / 100) + .00001;
    return d;
}

void rom_to_cx_price(databuf_t * buff, char *price, size_t len)
{
    double d = atof(price);
    d = d * 100000;
    d += .000000001;
    int i = htonl((int) d);
    databuf_memcpy(buff, (char *) &i, 4);
}

double cx_to_rom_price(char *data)
{
    int a = 0;
    double d = 0;
    memcpy(&a, data, 4);
    a = ntohl(a);
    d = ((double) a / 100000) + .0000000001;
    return d;
}

static void set_cx_header(cx_obj * obj, databuf_t * buff, char type)
{
    if (type == 'A') {
        obj->sequence = 1;
    }
    int s = htonl(get_sequence(obj));
    int t = htonl(generate_cx_timestamp(obj));
    databuf_write(buff, "%c", FIX_FIELD_DELIM);
    databuf_memcpy(buff, (char *) &s, 4);
    databuf_memcpy(buff, (char *) &t, 4);
    databuf_write(buff, "%c", type);
}

static int create_cx_heart_beat(cx_obj * obj, databuf_t * buff)
{
    int s = htonl(obj->sessionid);
    databuf_memcpy(buff, (char *) &s, 4);
    return 1;
}

static int create_logout(cx_obj * obj, databuf_t * buff)
{
    int s = htonl(obj->sessionid);
    databuf_write(buff, "%-20s", obj->userid);
    databuf_memcpy(buff, (char *) &s, 4);
    databuf_write(buff, "BYE");
    return 1;
}

static int create_logon(cx_obj * obj, databuf_t * buff)
{
    int s = htonl(obj->sessionid);
    databuf_write(buff, "%-20s%-20s", obj->userid, obj->password);
    databuf_memcpy(buff, (char *) &s, 4);
    return 1;
}

static int create_info_req(cx_obj * obj, databuf_t * buff)
{
    int s = htonl(obj->sessionid);
    databuf_memcpy(buff, (char *) &s, 4);
    return 1;
}

int get_instr_idx(cx_obj * obj, char *symbol, size_t len, databuf_t * buff)
{
    char *idx = NULL;
    if (find_n_fill(obj->symbols, symbol, len, (void *) &idx)) {
        databuf_memcpy(buff, idx, 2);
        return 1;
    } else {
        return 0;
    }
}

static int create_replace(cx_obj * obj, databuf_t * buff,
                          dart_ex_order_obj * ord)
{
    int ret_code = 1;
    int cl_ord_id = get_cl_ord_id(obj);
    int h_clordid = htonl(cl_ord_id);
    size_t cl_len = 0;
    int orig_id = 0;
    char *temp = doa_get_cl_ord_id(ord, &cl_len);
    memcpy(&orig_id, temp, 4);
    doa_set_rep_ord_id(ord, &cl_ord_id, 4);
    add_new_id_to_tree(obj, ord, cl_ord_id);
    int h_orig = htonl(orig_id);
    size_t sym_len = 0;
    char *sym = doa_get_symbol(ord, &sym_len);
    size_t price_len = 0;
    char *price = 0;
    databuf_memcpy(buff, (char *) &h_clordid, 4);
    databuf_memcpy(buff, (char *) &h_orig, 4);
    rom_to_cx_amount(buff, doa_get_order_qty(ord));
    price = doa_get_price(ord, &price_len);
    rom_to_cx_price(buff, price, price_len);
    if (!get_instr_idx(obj, sym, sym_len, buff)) {
        roll_back_sequence(obj);
        ret_code = 0;
        rom_reject_helper(obj, "Unable to find Instrument Code.", "20",
                          ord);
    }
    if (temp) {
        free(temp);
    }
    if (sym) {
        free(sym);
    }
    if (price) {
        free(price);
    }
    return ret_code;
}

static int create_cancel(cx_obj * obj, databuf_t * buff,
                         dart_ex_order_obj * ord)
{
    int ret_code = 1;
    int cl_ord_id = get_cl_ord_id(obj);
    int h_clordid = htonl(cl_ord_id);
    size_t cl_len = 0;
    int orig_id = 0;
    char *temp = doa_get_cl_ord_id(ord, &cl_len);
    memcpy(&orig_id, temp, 4);
    doa_set_rep_ord_id(ord, &cl_ord_id, 4);
    add_new_id_to_tree(obj, ord, cl_ord_id);
    int h_orig = htonl(orig_id);
    size_t sym_len = 0;
    char *sym = doa_get_symbol(ord, &sym_len);
    databuf_memcpy(buff, (char *) &h_clordid, 4);
    databuf_memcpy(buff, (char *) &h_orig, 4);
    if (!get_instr_idx(obj, sym, sym_len, buff)) {
        ret_code = 0;
        roll_back_sequence(obj);
        rom_reject_helper(obj, "Unable to find Instrument Code.", "14",
                          ord);
    }
    free(temp);
    free(sym);
    return ret_code;
}

static int create_new_order(cx_obj * obj, databuf_t * buff,
                            dart_ex_order_obj * ord)
{
    int ret_code = 1;
    size_t sym_len = 0;
    char *sym = 0;
    char *side = 0;
    size_t side_len = 0;
    size_t price_len = 0;
    char *price = 0;
    size_t tif_len = 0;
    char *tif = 0;
    int cl_ord_id = get_cl_ord_id(obj);
    int h_clordid = htonl(cl_ord_id);
    doa_set_cl_ord_id(ord, &cl_ord_id, 4);
    databuf_memcpy(buff, (char *) &h_clordid, 4);
    if (doa_get_show_amt(ord) > -1) {
        databuf_write(buff, "Z");
    } else {
        databuf_write(buff, "F");
    }
    sym = doa_get_symbol(ord, &sym_len);
    if (!get_instr_idx(obj, sym, sym_len, buff)) {
        ret_code = 0;
        roll_back_sequence(obj);
        rom_reject_helper(obj, "Unable to find Instrument code.", "8",
                          ord);
    }
    side = doa_get_side(ord, &side_len);
    if (side_len > 0 && strncmp(side, "1", 1) == 0) {
        databuf_write(buff, "%c", 'B');
    } else if (side_len > 0 && strncmp(side, "2", 1) == 0) {
        databuf_write(buff, "%c", 'S');
    } else {
        ret_code = 0;
        roll_back_sequence(obj);
        rom_reject_helper(obj, "Non Compatible Side.", "8", ord);
    }
    rom_to_cx_amount(buff, doa_get_order_qty(ord));
    rom_to_cx_amount(buff, doa_get_min_qty(ord));
    price = doa_get_price(ord, &price_len);
    rom_to_cx_price(buff, price, price_len);
    double show_amt = doa_get_show_amt(ord);
    if (show_amt < 0) {
        long b = 0x7FFFFFFF;
        b = htonll(b);
        databuf_memcpy(buff, &b, 8);
    } else {
        rom_to_cx_amount(buff, show_amt);
    }
    tif = doa_get_tif(ord, &tif_len);
    if (tif_len > 0 && strncmp(tif, "3", 1) == 0) {
        databuf_memcpy(buff, "I", 1);
    } else {
        databuf_memcpy(buff, "G", 1);
    }
    add_order_to_maps(obj, ord, cl_ord_id);
    if (sym) {
        free(sym);
    }
    if (side) {
        free(side);
    }
    if (tif_len > 0) {
        free(tif);
    }
    if (price) {
        free(price);
    }
    return ret_code;
}

static int cx_internal_send(con_obj * co, char *data, int len)
{
    int ret_code = 0;
    int len_sent = 0;
    long bytes_to_send = len;
    char *s_off = data;
    int failed_send = 0;
    int sock = get_sock(co);
    if (sock >= 0) {
        while (len_sent < len) {
            ret_code = dart_send_message(co, s_off, bytes_to_send);
            if (ret_code >= 0) {
                len_sent += ret_code;
            } else {
                ++failed_send;
                len_sent += 0;
                if (failed_send >= 1) {
                    printf
                        ("Giving up sending fix message: total_bytes: %d bytes_sent %d \n",
                         len, len_sent);
                    return ret_code;
                }
                ret_code = 0;
            }
            if (len_sent < len) {
                bytes_to_send -= ret_code;
                s_off += ret_code;
            }
        }
    }
    return ret_code;
}

/*
 * All sends must go through this function in order to be properly sequenced.
 */
void send_seq_cx_message(cx_obj * obj, char type, dart_ex_order_obj * ord)
{
    databuf_t *buff = databuf_alloc(200);
    int ret_code = 0;
    pthread_mutex_lock(&obj->cx_seq_blocker);
    set_cx_header(obj, buff, type);
    switch (type) {
    case 'L':
        ret_code = create_new_order(obj, buff, ord);
        break;
    case 'N':
        ret_code = create_cancel(obj, buff, ord);
        break;
    case 'P':
        ret_code = create_replace(obj, buff, ord);
        break;
    case 'C':
        ret_code = create_cx_heart_beat(obj, buff);
        break;
    case 'E':
        ret_code = create_info_req(obj, buff);
        break;
    case 'A':
        ret_code = create_logon(obj, buff);
        break;
    case 'B':
        ret_code = create_logout(obj, buff);
        break;
    default:
        databuf_destroy(buff);
        pthread_mutex_unlock(&obj->cx_seq_blocker);
        return;
    }
    if (ret_code) {
        databuf_write(buff, "%c", ETX);
        cx_internal_send(obj->connection, buff->buffer,
                         databuf_unread(buff));
    }
    databuf_destroy(buff);
    pthread_mutex_unlock(&obj->cx_seq_blocker);
}

void set_is_connected(cx_obj * obj, int is_connected)
{
    obj->is_connected = is_connected;
    if (!is_connected) {
        obj->sequence = 1;
    }
}

void kill_cx_connection(cx_obj * obj)
{
    obj->disconnect = 1;
}

void handle_cx_logon(cx_obj * obj, char *data, unsigned long len)
{
    char *d_off = data;
    d_off += 40;
    memcpy(&obj->sessionid, d_off, 4);
    obj->sessionid = ntohl(obj->sessionid);
    obj->is_connected = 1;
    send_seq_cx_message(obj, 'E', 0);
    send_from_rolling_log(obj, "Logged on to CX OUCH.", 21, 'A', 0);
}

void handle_cx_logout(cx_obj * obj, char *data, unsigned long len)
{
    obj->is_connected = 0;
    printf("GOT A LOGOUT FROM CURRENEX.\n");
    obj->sequence = 1;
    send_from_rolling_log(obj, "Logged out of CX OUCH.", 22, 'F', 0);
}

void handle_cx_instr_info(cx_obj * obj, char *data, unsigned long len)
{
    char *d_off = data;
    char *i_off = d_off + 4;
    d_off += 7;
    insert_map(obj->symbols, d_off, 7, i_off, 2);
    send_from_rolling_log(obj, d_off, 7, 'D', 0);
}

static void send_mail_for_trade(cx_obj * obj, char *data,
                                unsigned long len, int cl_ord)
{
    char *d_off = data;
    long order_id = 0;
    time_t tt = 0;
    struct tm lm;
    struct tm *t;
    double fill_amt = 0;
    double fill_rate = 0;
    double leaves_amt = 0;
    char side;
    char liq;
    char ex_type;
    char *ex_off = 0;
    char *ret_status = 0;
    long raw_leaves = 0;
    d_off += 4;
    memcpy(&order_id, d_off, 8);
    order_id = ntohll(order_id);
    d_off += 10;
    side = d_off[0];
    d_off += 1;
    fill_amt = cx_to_rom_amount(d_off);
    d_off += 8;
    fill_rate = cx_to_rom_price(d_off);
    d_off += 8;
    ex_off = d_off;
    d_off += 20;
    ex_type = d_off[0];
    d_off += 17;
    memcpy(&tt, d_off, 8);
    tt = ntohll(tt);
    tt = tt / 1000;
    t = gmtime_r(&tt, &lm);
    t = &lm;
    d_off += 8;
    leaves_amt = cx_to_rom_amount(d_off);
    memcpy(&raw_leaves, d_off, 8);
    ret_status = (raw_leaves > 0) ? "1" : "2";
    databuf_t *output = databuf_alloc(1000);
    databuf_t *title = databuf_alloc(100);
    databuf_write(title, "Unmatched order from CX OUCH.");
    databuf_write(output, "Status:%s, Order ID: %d, CX Order ID: %lu\n",
                  ret_status, cl_ord, order_id);
    databuf_write(output, "Last Shares: ");
    cx_double_to_char_amount(output, fill_amt);
    databuf_write(output, "\nLast Price: ");
    cx_double_to_char_price(output, fill_rate);
    databuf_write(output, "\nLeaves Amount: ");
    cx_double_to_char_amount(output, leaves_amt);
    if (t != 0) {
        databuf_write(output, "\n%04d%02d%02d-%02d:%02d:%02d\n",
                      (t->tm_year + 1900), (t->tm_mon + 1), t->tm_mday,
                      t->tm_hour, t->tm_min, t->tm_sec);
    } else {
        databuf_write(output, "\n");
    }
    databuf_memcpy(output, ex_off, 20);
    d_off += 8;
    liq = d_off[0];
    databuf_write(output, "\nLIQ: %c\n", liq);
    send_tech_message(output->buffer, databuf_unread(output),
                      title->buffer, databuf_unread(title));
    databuf_destroy(output);
    databuf_destroy(title);
}

void handle_cx_trade(cx_obj * obj, char *data, unsigned long len)
{
    int cl_ord = 0;
    dart_ex_order_obj *doj = 0;
    char *d_off = data;
    memcpy(&cl_ord, d_off, 4);
    cl_ord = ntohl(cl_ord);
    doj = get_ord_from_tree(obj, cl_ord);
    if (doj != 0) {
        long order_id = 0;
        time_t tt = 0;
        struct tm lm;
        struct tm *t;
        double fill_amt = 0;
        double fill_rate = 0;
        double leaves_amt = 0;
        char side;
        char liq;
        char ex_type;
        char *ex_off = 0;
        char *ret_status = 0;
        size_t sender_len = 0;
        char *sender = doa_get_sender_name(doj, &sender_len);
        size_t rom_len = 0;
        char *rom = doa_get_rom_id(doj, &rom_len);
        long raw_leaves = 0;
        d_off += 4;
        memcpy(&order_id, d_off, 8);
        order_id = ntohll(order_id);
        d_off += 10;
        side = d_off[0];
        d_off += 1;
        fill_amt = cx_to_rom_amount(d_off);
        d_off += 8;
        fill_rate = cx_to_rom_price(d_off);
        d_off += 8;
        ex_off = d_off;
        d_off += 20;
        ex_type = d_off[0];
        d_off += 17;
        memcpy(&tt, d_off, 8);
        tt = ntohll(tt);
        tt = tt / 1000;
        t = gmtime_r(&tt, &lm);
        t = &lm;
        d_off += 8;
        leaves_amt = cx_to_rom_amount(d_off);
        memcpy(&raw_leaves, d_off, 8);
        raw_leaves = ntohll(raw_leaves);
        ret_status = (raw_leaves > 0) ? "1" : "2";
        databuf_t *output = databuf_alloc(300);
        databuf_write(output, "S,,,,,,,,,,,,,,%s,%d,%lu,", ret_status,
                      cl_ord, order_id);
        databuf_memcpy(output, rom, rom_len);
        databuf_write(output, ",,,,,,,,,,,,,,,,,,,,,,,,,");
        cx_double_to_char_amount(output, fill_amt);
        databuf_memcpy(output, ",,", 2);
        cx_double_to_char_price(output, fill_rate);
        databuf_memcpy(output, ",,,,,", 5);
        cx_double_to_char_amount(output, leaves_amt);
        if (t != 0) {
            databuf_write(output, ",%04d%02d%02d-%02d:%02d:%02d,,,,",
                          (t->tm_year + 1900), (t->tm_mon + 1), t->tm_mday,
                          t->tm_hour, t->tm_min, t->tm_sec);
        } else {
            databuf_write(output, ",,,,,");
        }
        databuf_memcpy(output, ex_off, 20);
        d_off += 8;
        liq = d_off[0];
        databuf_write(output, ",,,,,,,,,,,,,,,%c,,,,,,,,,\n", liq);
        obj->send_to_rom(output->buffer, databuf_unread(output), sender,
                         sender_len);
        databuf_destroy(output);
        if (sender) {
            free(sender);
        }
        if (rom) {
            free(rom);
        }
    } else {
        printf("Unable to find fill for: %d \n", cl_ord);
        send_mail_for_trade(obj, data, len, cl_ord);
    }
}

void handle_cx_expired(cx_obj * obj, char *data, unsigned long len)
{
    int cl_ord = 0;
    dart_ex_order_obj *doj = 0;
    memcpy(&cl_ord, data, 4);
    cl_ord = ntohl(cl_ord);
    doj = get_ord_from_tree(obj, cl_ord);
    if (doj != 0) {
        size_t sender_len = 0;
        char *sender = doa_get_sender_name(doj, &sender_len);
        size_t rom_len = 0;
        char *rom = doa_get_rom_id(doj, &rom_len);
        long ex_ord_id = 0;
        char *d_off = data;
        char *ret_status = "4";
        char *message = 0;
        size_t cl_ord_id_len = 0;
        int orig_id = 0;
        char *ocl = doa_get_cl_ord_id(doj, &cl_ord_id_len);
        if (cl_ord_id_len > 0) {
            memcpy(&orig_id, ocl, 4);
        }
        d_off += 4;
        memcpy(&ex_ord_id, d_off, 8);
        ex_ord_id = ntohll(ex_ord_id);
        d_off += 9;
        message = get_currenex_cnl_code(d_off);
        databuf_t *output = databuf_alloc(300);
        databuf_write(output, "S,,,,,,,,,,,,,,%s,%d,%lu,", ret_status,
                      cl_ord, ex_ord_id);
        databuf_memcpy(output, rom, rom_len);
        databuf_write(output, ",,,,,,,,,%s,,,,,,,,,,,,,,,,,,,,\n",
                      message);
        obj->send_to_rom(output->buffer, databuf_unread(output), sender,
                         sender_len);
        remove_from_tree(obj, cl_ord);
        remove_from_tree(obj, orig_id);
        remove_from_cx_map(obj, rom, rom_len);
        databuf_destroy(output);
        if (ocl) {
            free(ocl);
        }
        if (rom) {
            free(rom);
        }
        if (sender) {
            free(sender);
        }
    } else {
        printf("Expired Unable to find order in tree: %d \n", cl_ord);
        send_to_rolling_log(obj, data, len, 'R', cl_ord);
    }
}

void handle_cx_replaced(cx_obj * obj, char *data, unsigned long len)
{
    int cl_ord = 0;
    int prev_ord = 0;
    char *d_off = data;
    dart_ex_order_obj *doj = 0;
    memcpy(&cl_ord, data, 4);
    cl_ord = ntohl(cl_ord);
    d_off += 4;
    memcpy(&prev_ord, d_off, 4);
    prev_ord = ntohl(prev_ord);
    doj = get_ord_from_tree(obj, prev_ord);
    if (doj == 0) {
        doj = get_ord_from_tree(obj, cl_ord);
    }
    if (doj != 0) {
        size_t sender_len = 0;
        char *sender = doa_get_sender_name(doj, &sender_len);
        size_t rom_len = 0;
        char *rom = doa_get_rom_id(doj, &rom_len);
        char *ret_status = 0;
        char *message = 0;
        d_off += 4;
        if (d_off[0] == 'P') {
            ret_status = "5";
            remove_from_tree(obj, prev_ord);
            doa_set_cl_ord_id(doj, &cl_ord, 4);
        } else if (d_off[0] == 'C') {
            ret_status = "4";
            remove_from_cx_map(obj, rom, rom_len);
            remove_from_tree(obj, cl_ord);
            remove_from_tree(obj, prev_ord);
        } else {
            ret_status = "20";
            remove_from_tree(obj, cl_ord);
            cl_ord = prev_ord;
        }
        d_off += 1;
        message = get_currenex_ec(d_off);
        databuf_t *output = databuf_alloc(400);
        databuf_write(output, "S,,,,,,,,,,,,,,%s,%d,,", ret_status,
                      cl_ord);
        databuf_memcpy(output, rom, rom_len);
        databuf_write(output, ",,,,,,,,,%s,,,,,,,,,,,,,,,,,,,,\n",
                      message);
        obj->send_to_rom(output->buffer, databuf_unread(output), sender,
                         sender_len);
        databuf_destroy(output);
        if (sender) {
            free(sender);
            sender = 0;
        }
        if (rom) {
            free(rom);
            rom = 0;
        }
    } else {
        printf("Replace Unable to find order in tree: %d \n", cl_ord);
        send_to_rolling_log(obj, data, len, 'Q', cl_ord);
    }
}

void handle_cx_cancel_rej(cx_obj * obj, char *data, unsigned long len)
{
    int cl_ord = 0;
    int prev_ord = 0;
    char *d_off = data;
    dart_ex_order_obj *doj = 0;
    memcpy(&cl_ord, data, 4);
    cl_ord = ntohl(cl_ord);
    d_off += 4;
    memcpy(&prev_ord, d_off, 4);
    prev_ord = ntohl(prev_ord);
    doj = get_ord_from_tree(obj, prev_ord);
    if (doj != 0) {
        size_t sender_len = 0;
        char *sender = doa_get_sender_name(doj, &sender_len);
        size_t rom_len = 0;
        char *rom = doa_get_rom_id(doj, &rom_len);
        char *message = 0;
        d_off += 4;
        message = get_currenex_ec(d_off);
        databuf_t *output = databuf_alloc(400);
        databuf_write(output, "S,,,,,,,,,,,,,,14,%d,,", prev_ord);
        databuf_memcpy(output, rom, rom_len);
        databuf_write(output, ",,,,,,,,,%s,,,,,,,,,,,,,,,,,,,,\n",
                      message);
        obj->send_to_rom(output->buffer, databuf_unread(output), sender,
                         sender_len);
        databuf_destroy(output);
        if (sender) {
            free(sender);
            sender = 0;
        }
        if (rom) {
            free(rom);
            rom = 0;
        }
    } else {
        printf("Unable to find order in tree: %d \n", prev_ord);
        send_to_rolling_log(obj, data, len, 'O', cl_ord);
    }
}

void handle_cx_ack(cx_obj * obj, char *data, unsigned long len)
{
    int cl_ord = 0;
    dart_ex_order_obj *doj = 0;
    memcpy(&cl_ord, data, 4);
    cl_ord = ntohl(cl_ord);
    doj = get_ord_from_tree(obj, cl_ord);
    if (doj != 0) {
        size_t sender_len = 0;
        char *sender = doa_get_sender_name(doj, &sender_len);
        size_t rom_len = 0;
        char *rom = doa_get_rom_id(doj, &rom_len);
        long ex_ord_id = 0;
        char *d_off = data;
        char *ret_status = 0;
        char *message = 0;
        d_off += 4;
        memcpy(&ex_ord_id, d_off, 8);
        ex_ord_id = ntohll(ex_ord_id);
        d_off += 8;
        ret_status = d_off[0] == 'R' ? "8" : "13";
        d_off += 1;
        message = get_currenex_ec(d_off);
        databuf_t *output = databuf_alloc(400);
        databuf_write(output, "S,,,,,,,,,,,,,,%s,%d,%lu,", ret_status,
                      cl_ord, ex_ord_id);
        databuf_memcpy(output, rom, rom_len);
        databuf_write(output, ",,,,,,,,,%s,,,,,,,,,,,,,,,,,,,,\n",
                      message);
        obj->send_to_rom(output->buffer, databuf_unread(output), sender,
                         sender_len);
        databuf_destroy(output);
        if (sender) {
            free(sender);
            sender = 0;
        }
        if (rom) {
            free(rom);
            rom = 0;
        }
    } else {
        printf("Unable to find order in tree: %d \n", cl_ord);
        send_to_rolling_log(obj, data, len, 'M', cl_ord);
    }
}

static int determine_cx_type(cx_obj * obj, int *cut_con,
                             struct parse_con *pc, char *soh_off,
                             unsigned long len)
{
    if (len > 8) {
        char *d_off = soh_off;
        d_off += 9;
        switch (d_off[0]) {
        case 'A':
            if (len >= 55) {
                ++d_off;
                handle_cx_logon(obj, d_off, len);
                return 55;
            } else {
                return -1;
            }
        case 'B':
            if (len >= 38) {
                ++d_off;
                handle_cx_logout(obj, d_off, len);
                return 38;
            } else {
                return -1;
            }
        case 'C':
            if (len >= 15) {
                return 15;
            } else {
                return -1;
            }
        case 'D':
            if (len >= 46) {
                ++d_off;
                handle_cx_instr_info(obj, d_off, len);
                return 46;
            } else {
                return -1;
            }
        case 'M':
            if (len >= 26) {
                ++d_off;
                handle_cx_ack(obj, d_off, len);
                return 26;
            } else {
                return -1;
            }
        case 'O':
            if (len >= 21) {
                ++d_off;
                handle_cx_cancel_rej(obj, d_off, len);
                return 21;
            } else {
                return -1;
            }
        case 'Q':
            if (len >= 22) {
                ++d_off;
                handle_cx_replaced(obj, d_off, len);
                return 22;
            } else {
                return -1;
            }
        case 'R':
            if (len >= 26) {
                ++d_off;
                handle_cx_expired(obj, d_off, len);
                return 26;
            } else {
                return -1;
            }
        case 'T':
            if (len >= 96) {
                ++d_off;
                handle_cx_trade(obj, d_off, len);
                return 96;
            } else {
                return -1;
            }
        default:
            {
                printf("Unknown order type: %c \n", d_off[0]);
            }
            return -1;
        }
    } else {
        char *hit_n_quit = calloc(len + 1, 1);
        memcpy(hit_n_quit, soh_off, len);
        printf("VALUE = [%s] \n", hit_n_quit);
        return -1;
    }
}

unsigned long frame_cx_mess(cx_obj * obj, const char *mess,
                            unsigned long len, int *cut_con,
                            struct parse_con *pc)
{
    unsigned long t_bytes_read = 0;
    int bytes_read = 0;
    char *soh_off = (char *) mess;
    while (t_bytes_read < len) {
        bytes_read =
            determine_cx_type(obj, cut_con, pc, soh_off,
                              (len - t_bytes_read));
        if (bytes_read >= 0) {
            soh_off += bytes_read;
            t_bytes_read += (unsigned long) bytes_read;
        } else {
            return t_bytes_read;
        }
    }
    return t_bytes_read;
}
