
/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/timeb.h>

#include "oats.h"
#include "message_queue.h"
#include "parser_func.h"
#include "rex_constants.h"
#include "config_loader.h"
#include "ex_config_helper.h"
#include "dart_handler.h"
#include "rex_seq_request.h"
#include "connection_interface.h"
#include "connection_factory.h"
#include "gatekeeper.h"
#include "dom_con.h"
#include "tm_to_str.h"
#include "balance_checker.h"
#include "polling_server.h"

struct oats {
    hashmap_t destinations;
    hashmap_t clients;
    con_int *oat_ci;
    pthread_spinlock_t oats_mux;
};

void oats_update(void* p, dart_order_obj* doj);
void on_oats_rc_connect(void* ap, int is_connected,
                     dart_order_obj *log);
void on_oats_connect(dartp* ap, int is_connected,
                     dart_order_obj *log);

static void clean_oats_map(hashmap_t map)
{
    databuf_t* rlf;
    char *key = 0;
    int key_len = 0;
    hashmap_iterator_t hi = hashmap_iterator_begin(map);
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                                     (void *) &rlf);
    while (valid) {
        databuf_destroy(rlf);
        rlf = 0;
        valid = hashmap_iterator_next(map, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &rlf);
    }
    hashmap_iterator_destroy(hi);
    delete_map(map);
}
void oat_cleansing_rinse(con_int* ci)
{
    oats_t* oat = (oats_t*)ci->ph;
    printf("Destroying oats!!\n");
    clean_oats_map(oat->destinations);
    clean_oats_map(oat->clients);
    dartp* oatp = (dartp*)ci->cparser;
    clean_dart_parser(oatp);
    free(oat);
    free(ci->mod_name);
    free(ci);
}

static struct oats* create_oats_struct(con_int * oci)
{
    struct oats *oat = malloc(sizeof(struct oats));
    oat->destinations = create_map(128);
    oat->clients = create_map(128);
    oat->oat_ci = oci;
    oci->ph = oat;
    pthread_spin_init(&oat->oats_mux, PTHREAD_PROCESS_PRIVATE);
    return oat;
}

static void oats_raw_send_wrapper(databuf_t * buff, con_int * ci)
{
    if (ci && ci->connected) {
        send_raw_csv(((dartp*)ci->cparser), buff->rd_ptr, databuf_unread(buff));
        buff->rd_ptr = buff->buffer;
    }
}
void send_oats_logon(void* p)
{
    dartp* dp = (dartp*)p;
    send_dart_logon_for_user(dp, "OATS", 4);
}

void create_oats_connection(con_int* ci, char* filename, int name_len,
        gatekeeper* gk)
{
    ci->gk = gk;
    ci->cparser = 0;
    ci->ph = 0;
    if(ci->mod_name_len <= 0) {
        ci->mod_name = calloc(name_len + 1, 1);
        memcpy(ci->mod_name, ci->ex->mod_name, ci->ex->mod_name_len);
        ci->mod_name_len = ci->ex->mod_name_len;
    }
    ci->should_buffer = 0;
    ci->connected= 0;
    ci->can_send_orders = 0;
    ci->is_blocked = 0;
    ci->sbm = 0;
    ci->tg = 0;
    ci->cl_ord_id_map = 0;
    ci->on_enter_order = 0;
    ci->on_replace_order = 0;
    ci->on_cancel_order = 0;
    ci->on_rej = 0;
    ci->on_replaced = 0;
    ci->on_out = 0;
    ci->on_fill = 0;
    ci->on_update = send_oats_message;
    ci->reset_seq = 0;
    ci->send_to_con = dc_send;
    ci->reset_seq = dc_reset_seq;
    ci->cleansing_rinse = oat_cleansing_rinse;
    ci->can_send_orders = 0;
    create_oats_struct(ci);
    //ci->from_con = oats_update;
    dartp* dp = create_dart_parser(ci, on_oats_connect,oats_update,get_gk_fact(gk),
                               ci->o,ci->ex->order_allocation,
                                1, 0);
    login_info* li = build_logon_info_from_config(ci->ex);
    rex_con* rc = create_rex_con(ci->p, ci->ex, dp, dp,li,
                              dart_write_callback, 
                              on_oats_rc_connect,
                              ph_parse_rom_message, NULL);
    rc->connect = send_oats_logon;
    rc->parser = dp;
    ci->cparser = dp;
    ci->rc = rc;
    dart_set_rex_con(dp, rc);
    set_compliance_con(gk, ci);
    if(li->create_new) {
        reset_seq(rc->o, "OATS", 4);
    } else {
        sequence_req* sr = create_sequence_request("OATS", 4,
                                 rc->logon,rc->conf->version,
                                 build_offsets,
                                 create_rom_resend_pad, rc);
        get_sequence_numbers(sr, rc->o);
        destroy_sequence_req(sr);
    }
    rex_connect(ci->p, rc);
}
void create_connection(con_int* ci, char *filename, int name_len,
        gatekeeper * gk)
{
    create_oats_connection(ci, filename, name_len, gk);
}


void add_client_connection(con_int* ci, char *client_name, int name_len,
        databuf_t * buff)
{
    oats_t * oat = (oats_t*)ci->ph;
    pthread_spin_lock(&oat->oats_mux);
    databuf_t *b = NULL;
    int found =
        get_map_entry(oat->clients, client_name, name_len, (void **) &b);
    if (found) {
        remove_from_map(oat->clients, client_name, name_len);
        databuf_destroy(b);
    }
    no_copy_insert(oat->clients, client_name, name_len, (void **) buff);
    pthread_spin_unlock(&oat->oats_mux);
    oats_raw_send_wrapper(buff, oat->oat_ci);
    send_debug_message(ci->o, "To Oats Client: %s \n", buff->rd_ptr);
}

void add_dest_connection(con_int * ci, char *dest_name, int name_len,
        databuf_t * buff)
{
    oats_t * oat = (oats_t*)ci->ph;
    if (strncmp(dest_name, "OATS", name_len) == 0) {
        databuf_destroy(buff);
        return;
    }
    pthread_spin_lock(&oat->oats_mux);
    databuf_t *b = NULL;
    int found = get_map_entry(oat->destinations, dest_name, name_len,
            (void **) &b);
    if (found) {
        remove_from_map(oat->destinations, dest_name, name_len);
        databuf_destroy(b);
    }
    no_copy_insert(oat->destinations, dest_name, name_len, (void **) buff);
    pthread_spin_unlock(&oat->oats_mux);
    oats_raw_send_wrapper(buff, oat->oat_ci);
    send_debug_message(ci->o, "To Oats Server: %s \n", buff->rd_ptr);
}

void remove_dest_connection(con_int * ci, char *dest_name, int name_len,
        databuf_t * buff)
{
    oats_t * oat = (oats_t*)ci->ph;
    if (strncmp(dest_name, "OATS", name_len) == 0) {
        return;
    }
    pthread_spin_lock(&oat->oats_mux);
    databuf_t *b = NULL;
    int found = get_map_entry(oat->destinations, dest_name, name_len,
            (void **) &b);
    remove_from_map(oat->destinations, dest_name, name_len);
    pthread_spin_unlock(&oat->oats_mux);
    if (found) {
        databuf_destroy(b);
    }
    oats_raw_send_wrapper(buff, oat->oat_ci);
}

void remove_client_connection(con_int * ci, char *dest_name, int name_len,
        databuf_t * buff)
{
    oats_t * oat = (oats_t*)ci->ph;
    pthread_spin_lock(&oat->oats_mux);
    databuf_t *b = NULL;
    int found =
        get_map_entry(oat->clients, dest_name, name_len, (void **) &b);
    remove_from_map(oat->clients, dest_name, name_len);
    pthread_spin_unlock(&oat->oats_mux);
    if (found) {
        databuf_destroy(b);
    }
    oats_raw_send_wrapper(buff, oat->oat_ci);
}

void send_oats_update(con_int * ap, int is_connected, char *name,
        int name_len, int client_or_serv, con_int * ci)
{
    oats_t * oat = (oats_t*)ap->ph;
    databuf_t *buff = databuf_alloc(1024);
    char l_name[100];
    memset(l_name, '\0', 100);
    int type = client_or_serv == 0 ? 17 : 19;
    int logged_on = 0;
    int blocked = 0;
    if (is_connected && ci != NULL) {
        logged_on = ci->can_send_orders;
        blocked = ci->is_blocked;
    } else if(ci == NULL) {
        databuf_destroy(buff);
        return;
    }
    if (name_len > 0) {
        memcpy(l_name, name, name_len);
    }
    int mod_len = 0;
    databuf_write(buff, "J,,%d,%s,,%d|%d|%d|1,%s,,,,,,,,,,,,,,,,,,,,", type,
            l_name, is_connected, logged_on, blocked,get_mod_name(ap->gk, &mod_len));
    if (is_connected) {
        if(ci->sock > 0) {
            struct sockaddr_in sa;
            memset(&sa, 0, sizeof(sa));
            socklen_t sa_len = sizeof(sa);
            getpeername((ci->sock), (struct sockaddr *) &sa, &sa_len);
            unsigned short p = ntohs(sa.sin_port);
            databuf_write(buff, "%s|%lu", inet_ntoa(sa.sin_addr), p);
        } else if(ci->ex && ci->ex->ip_len > 0) {
            databuf_write(buff, "%s|%lu", ci->ex->ip, ci->ex->port);
        }
    }
    if (is_connected && client_or_serv && oat) {
        databuf_memcpy(buff, ",,,,,,,,,,,,,,\n", 15);
        add_client_connection(ap, name, name_len, buff);
    } else if (client_or_serv && oat) {
        databuf_memcpy(buff, ",,,,,,,,,,,,,,\n", 15);
        remove_client_connection(ap, name, name_len, buff);
        databuf_destroy(buff);
    } else if (!client_or_serv && is_connected && oat) {
        databuf_write(buff, ",,,,,,,,,,,,,,,%s,\n",
                l_name);
        add_dest_connection(ap, name, name_len, buff);
    } else {
        if (oat) {
            databuf_write(buff, ",,,,,,,,,,,,,,,%s,\n", l_name);
            remove_dest_connection(ap, name, name_len, buff);
            databuf_destroy(buff);
        }
    }
}

static int set_time_stamp(char* mblk)
{
    int len = 0;
    struct timeb rawtime;
    ftime(&rawtime);
    struct tm lm;
    if(gmtime_r(&rawtime.time, &lm) != 0
            && tm_to_str_millis(&lm, rawtime.millitm, mblk,
                TIMESTAMP_LEN_WITH_MILLIS) == 0) {
        len = TIMESTAMP_LEN_WITH_MILLIS;
    }
    return len;
}

void send_oats_message(con_int* oats_con, rcntr* risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    if(doj->positions[ROM_TIME].iov_len <= 0) {
        int len = set_time_stamp(doj->current->data->wr_ptr);
        set_rom_field(doj, ROM_TIME, doj->current->data->wr_ptr,
                len);
        doj->current->data->wr_ptr += len;
    } else {
        set_time_stamp((char*)doj->positions[ROM_TIME].iov_base);
    }
    dartp* dp = (dartp*)oats_con->cparser;
    record_dart_message_for_sender(oats_con->o, doj,"OATS", 4, dp);
}

char *get_rom_val(const char *message, unsigned long len, int *val_len,
        int rom_val)
{
    int i = 0;
    char *d_off = (char *) message;
    char *ret_val = 0;
    unsigned int ret_code = 0;
    unsigned long s_len = len;
    for (; i < rom_val; ++i) {
        ret_val = memchr(d_off, ',', s_len);
        if (ret_val) {
            ret_code = (ret_val - d_off);
            d_off += (ret_code + 1);
            s_len -= ret_code + 1;
            ret_val += 1;
        } else {
            *val_len = 0;
            return 0;
        }
    }
    char *end = memchr(d_off, ',', s_len);
    if (end && (end - ret_val) < len) {
        *val_len = end - ret_val;
    } else {
        *val_len = (len - 1) - (ret_val - message);
    }
    return ret_val;
}

static con_int *get_interface_for_name(gatekeeper * gk, char *name,
        int name_len)
{
    con_int *con = 0;
    int ret = get_obj_by_client_id(gk, name, name_len, (void *) &con);
    if (ret) {
        return con;
    } else {
        return 0;
    }
}


static void send_dest_logout(dart_order_obj* doj,
        con_int *pc)
{
    int trader_len = getplen(doj, ORDER_DEST);
    char *trader = getpval(doj, ORDER_DEST);
    con_int *ci = 0;
    int found = get_destination(pc->gk, trader, trader_len, (void *) &ci);
    if (found && trader_len > 0) {
        ci->cleansing_rinse(ci);
    }
}
static void block_dest(dart_order_obj* doj,
        con_int *pc)
{
    int trader_len = getplen(doj, ORDER_DEST);
    int is_blocked = getplen(doj, ACTION);;
    char *trader = getpval(doj, ORDER_DEST);
    char *action = getpval(doj, ACTION);
    con_int *ci = 0;
    int found = get_destination(pc->gk, trader, trader_len, (void **) &ci);
    if (found && trader_len > 0 && is_blocked > 0) {
        is_blocked = strtol(action, NULL, 10);
        ci->is_blocked = is_blocked;
        send_oats_update(pc, ci->connected, trader,
                trader_len, ci->is_server, ci);
    }
}

static void update_route(dart_order_obj* doj,
        con_int *pc)
{
    int dest_len = getplen(doj, 13);
    int dest_rout_len = getplen(doj, 41);
    char *dest = getpval(doj, 13);
    char *dest_route = getpval(doj, 41);
    if (dest_len > 0 && dest_rout_len > 0) {
        int change = reset_dest_routes(pc->gk, dest, dest_len, dest_route,
                dest_rout_len);
        databuf_t *buff = databuf_alloc(1024);
        int mod_len = 0;
        if (change) {
            set_rom_field(doj, 0, "J", 1);
            pc->send_to_con(pc, &doj->risk);
        } else {
            databuf_write(buff, "J,,11,,,,%s,,,,,,,",
                    get_mod_name(pc->gk, &mod_len));
            databuf_memcpy(buff, dest, dest_len);
            databuf_write(buff, ",,,,,,,,,,,,,INVALID,,,,,,,,,,,,,,,");
            databuf_memcpy(buff, dest_route, dest_rout_len);
            databuf_write(buff, ",,,,\n");
            oats_raw_send_wrapper(buff, pc);
        }
    }
}

static void block_user(dart_order_obj* doj, 
        con_int *pc)
{
    char *trader = getpval(doj, TRADER);
    char *action = getpval(doj, ACTION);
    int trader_len = getplen(doj, TRADER);
    if (trader_len > 0 && getplen(doj, ACTION) > 0) {
        int is_blocked = strtol(action, NULL, 10);
        jp_limits_checker *jlc = get_balance_struct(pc->gk);
        jp_block_trader(jlc, trader, trader_len, is_blocked);
        con_int *ci =
            get_interface_for_name(pc->gk, trader, getplen(doj, TRADER));
        if (ci) {
            ci->is_blocked = is_blocked;
            send_oats_update(pc, ci->connected, trader,
                    trader_len, ci->is_server, ci);
        }
    }
}

static void build_limits_message(struct jp_limits *limits, char type,
        con_int * ci, char *trader,
        int trader_len, int num)
{
    int mod_len = 0;
    databuf_t *buff = databuf_alloc(1000);
    databuf_write(buff, "J,,%d,", num);
    databuf_memcpy(buff, trader, trader_len);
    if(limits == 0x0) {
        char ltrader[64];
        memset(ltrader, '\0', 64);
        memcpy(ltrader, trader, trader_len);
        databuf_write(buff, ",,,%s,,,,,,,,,,,,,,,,,Unable to find limits for: %s,,,,,,,,,%c,,,,,,,,,,,,\n",
               get_mod_name(ci->gk, &mod_len), ltrader, type);
    } else {
        double deno = EQ_MULT;
        if(type == 'F') {
            deno = FUT_MULT;
        }
        double mbp = (double) limits->bp / deno;
        double mdol = (double) limits->max_doll / deno;
        unsigned long lcb = labs(limits->cur_balance_buy);
        double cdol = (double) lcb / deno;
        double lcbs = (double) limits->cur_balance_sell/deno;
        databuf_write(buff,
                ",,,%s,,,,,,,%ld,%ld,%ld,%ld,%f,%f|%f,%ld,%f,,,,,,,,,,,,,,,%c,,,,,,,,,,,,\n",
                get_mod_name(ci->gk, &mod_len), limits->max_ord,
                limits->open_ords, limits->max_share,
                limits->open_shares, mbp, cdol, lcbs, limits->max_shares_ord,
                mdol, type);
    }
    oats_raw_send_wrapper(buff, ci);
    databuf_destroy(buff);
}

#define FIND_SEC_SLASH(off, end)                                   \
    do {                                                     \
        ++off;                                               \
        --end;                                               \
    } while(*off != '/' && end > 0);             \


static void send_back_clrlimitinfo(con_int *ci, dart_order_obj* doj)
{
    int key_len = getplen(doj, TRADER);
    char *key = getpval(doj, TRADER);
    if (key_len > 0) {
        char* sec = key;
        int len = key_len;
        FIND_SEC_SLASH(sec,len);
        if(len > 0) {
            jp_limits_checker *jlc = get_balance_struct(ci->gk);
            int len = sec - key;
            char sec_type = 'E';
            jp_limits *limits = 0;
            switch(sec[1]) {
                case 'O':
                    sec_type = 'O';
                    limits = get_equity_option_limits_for_clraccnt(jlc, key, len);
                    break;
                case 'M':
                    sec_type = 'M';
                    limits = get_fut_opt_limits_for_clraccnt(jlc, key, len);
                    break;
                case 'F':
                    sec_type = 'F';
                    limits = get_future_limits_for_clraccnt(jlc, key, len);
                    break;
                default:
                    limits = get_equity_limits_for_clraccnt(jlc, key, len);
                    break;
            }
            if (limits)
            {
                build_limits_message(limits, sec_type, ci, key, key_len, 25);
            }
        }
    }
}
static void send_back_billinginfo(con_int* ci, dart_order_obj* doj)
{
    //the key value is actually the billing group id
    int key_len = getplen(doj, TRADER);
    char *key = getpval(doj, TRADER);
    int ikey = 0;
    char* char_type = getpval(doj, 35);
    int stlen = getplen(doj, 35);
    if(key_len > 0) {
        ikey = atoi(key);
    } else {
        return;
    }
    if(stlen > 0) {
        char sec_type = char_type[0];
        
        jp_limits_checker *jlc = get_balance_struct(ci->gk);
        jp_limits *limits = 0;
            
        if (stlen > 0) {
            switch(sec_type) {
                case 'O':
                    sec_type = 'O';
                    limits = get_equity_option_limits_for_bill(jlc, ikey);
                    break;
                case 'M':
                    sec_type = 'M';
                    limits = get_fut_opt_limits_for_bill(jlc, ikey);
                    break;
                case 'F':
                    sec_type = 'F';
                    limits = get_future_limits_for_bill(jlc, ikey);
                    break;
                default:
                    limits = get_equity_limits_for_bill(jlc, ikey);
                    break;
            }
            if(limits) {
                build_limits_message(limits, sec_type, ci, key, key_len, BILLINGGRP_LIMIT_STATUS);
            } else {
                build_limits_message(0x0, sec_type, ci, key, key_len, BILLINGGRP_LIMIT_STATUS);
            }
        }
        else
        {
            limits = get_equity_limits_for_bill(jlc, ikey);
            if (limits) {
                build_limits_message(limits, 'E', ci, key, key_len, BILLINGGRP_LIMIT_STATUS);
                limits = get_equity_option_limits_for_bill(jlc, ikey);
                build_limits_message(limits, 'O', ci, key, key_len, BILLINGGRP_LIMIT_STATUS);
                limits = get_future_limits_for_bill(jlc, ikey);
                build_limits_message(limits, 'F', ci, key, key_len, BILLINGGRP_LIMIT_STATUS);
                limits = get_fut_opt_limits_for_bill(jlc, ikey);
                build_limits_message(limits, 'M', ci, key, key_len, BILLINGGRP_LIMIT_STATUS);
            } else {
                build_limits_message(0x0, sec_type, ci, key, key_len, BILLINGGRP_LIMIT_STATUS);
            }
        }
    }
}
static void get_oat_fut(jp_limits_checker* jlc, dart_order_obj* doj,
                        char* trader, int trader_len,
                        con_int* ci)
{
    int slen = getplen(doj, 10);
    char* sym = getpval(doj, 10);
    databuf_t* db = databuf_alloc(2048);
    get_future_limits_for_trader(jlc, trader, trader_len, db, ci->gk,
                                  sym, slen);
    if(databuf_unread(db) > 0) {
        oats_raw_send_wrapper(db, ci);
    }
    databuf_destroy(db);
}

static void send_back_status(con_int *ci, dart_order_obj* doj)
{
    int stlen = getplen(doj, 70);
    char* sec_type = getpval(doj, 70);
    int trader_len = getplen(doj, TRADER);
    char *trader = getpval(doj, TRADER);
    if (trader_len > 0) {
        jp_limits_checker *jlc = get_balance_struct(ci->gk);
        if(stlen > 0) {
            struct jp_limits* eq = 0;
            switch(sec_type[0]) {
                case 'M':
                     eq = get_of_limits_for_trader(jlc, trader, trader_len);
                    if (eq) {
                        build_limits_message(eq, 'M', ci, trader, trader_len, 24);
                    }
                    break;
                case 'E':
                    eq =
                        get_equity_limits_for_trader(jlc, trader, trader_len);
                    if (eq) {
                        build_limits_message(eq, 'E', ci, trader, trader_len, 24);
                    }
                    break;
                case 'O':
                    eq = get_option_limits_for_trader(jlc, trader, trader_len);
                    if (eq) {
                        build_limits_message(eq, 'O', ci, trader, trader_len, 24);
                    }
                    break;
                case 'F':
                    get_oat_fut(jlc, doj, trader, trader_len, ci);
                    break;
            }
        } else {
            struct jp_limits *eq =
                get_equity_limits_for_trader(jlc, trader, trader_len);
            if (eq) {
                build_limits_message(eq, 'E', ci, trader, trader_len, 24);
            }
            eq = get_option_limits_for_trader(jlc, trader, trader_len);
            if (eq) {
                build_limits_message(eq, 'O', ci, trader, trader_len, 24);
            }
            get_oat_fut(jlc, doj, trader, trader_len, ci);
            eq = get_of_limits_for_trader(jlc, trader, trader_len);
            if (eq) {
                build_limits_message(eq, 'M', ci, trader, trader_len, 24);
            }
        }
    }
}

static void disconnect_user(dart_order_obj* doj,
        con_int *pc)
{
    char *trader = getpval(doj, TRADER);
    int trader_len = getplen(doj, TRADER);
    con_int *ci = get_interface_for_name(pc->gk, trader, trader_len);
    if (ci) {
        ci->connected = 0;
        ci->is_blocked = 1;
        send_oats_update(pc, ci->connected, trader,
                getplen(doj, TRADER), 1, ci);
    }
    gk_disconnect_by_name(pc->gk, trader, trader_len);
}

static void exempt_user(dart_order_obj* doj,
        con_int *pc)
{
    int trader_len = getplen(doj, TRADER);
    int is_exempt = getplen(doj, ACTION);
    char *trader = getpval(doj, TRADER);
    char *action = getpval(doj, ACTION);
    if (trader_len > 0 && is_exempt > 0) {
        is_exempt = strtol(action, NULL, 10);
        jp_limits_checker *jlc = get_balance_struct(pc->gk);
        jp_exempt_trader(jlc, trader, trader_len, is_exempt);
    }
}

static void exempt_group(dart_order_obj* doj, con_int*pc)
{
    int trader_len = getplen(doj, TRADER);
    int is_exempt = getplen(doj, ACTION);
    char *trader = getpval(doj, TRADER);
    char *action = getpval(doj, ACTION);
    if(trader_len > 0 && is_exempt > 0) {
        is_exempt = strtol(action, NULL, 10);
        trader_len = strtol(trader, NULL, 10);
        jp_limits_checker *jlc = get_balance_struct(pc->gk);
        jp_exempt_billing_group(jlc,trader_len,is_exempt);
    }
}

static void reload_dest_ipport(dart_order_obj* doj,
        con_int *pc)
{
    int trader_len = getplen(doj, ORDER_DEST);
    char *trader = getpval(doj, ORDER_DEST);
    con_int *con_int = 0;
    int found = get_destination(pc->gk, trader,
            trader_len, (void **) &con_int);
    if (found) {
        con_int->reload_ip_port(con_int, doj);
    }
}

static void disconnect_dest(dart_order_obj* doj, con_int *pc)
{
    int trader_len = getplen(doj, TRADER);
    char *trader = getpval(doj, TRADER);
    gk_disconnect_dest(pc->gk, trader, trader_len);
}

static void get_current_routes(con_int* pc)
{
    databuf_t *buff = databuf_alloc(2000);
    int mod_len = 0;
    databuf_write(buff, "J,,12,,,,%s,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,",
            get_mod_name(pc->gk, &mod_len));
    print_dest_routes(pc->gk, buff);
    databuf_write(buff, ",,,,,,,,\n", 9);
    oats_raw_send_wrapper(buff, pc);
    databuf_destroy(buff);
}

static void get_current_dests(con_int * pc)
{
    oats_t *oat = (oats_t *) (pc->ph);
    hashmap_iterator_t hi = hashmap_iterator_begin(oat->destinations);
    databuf_t *buff = 0;
    char *key = 0;
    int key_len = 0;
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                (void *) &buff);
    while (valid) {
        oats_raw_send_wrapper(buff, pc);
        valid = hashmap_iterator_next(oat->destinations, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                    &key_len, (void *) &buff);
    }
    hashmap_iterator_destroy(hi);
}

static void get_current_users(con_int * pc)
{
    oats_t *oat = (oats_t *) (pc->ph);
    hashmap_iterator_t hi = hashmap_iterator_begin(oat->clients);
    databuf_t *buff = 0;
    char *key = 0;
    int key_len = 0;
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                (void *) &buff);
    while (valid) {
        oats_raw_send_wrapper(buff, pc);
        valid = hashmap_iterator_next(oat->clients, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key, &key_len,
                    (void *) &buff);
    }
    hashmap_iterator_destroy(hi);
}

static void reload_traders(con_int * pc, dart_order_obj* doj)
{
    jp_limits_checker *jlc = get_balance_struct(pc->gk);

    char ltrader[128];
    int tlen = getplen(doj, TRADER); 
    if (tlen > 0) {
        memset(ltrader, '\0', 128);
        memcpy(ltrader, getpval(doj, TRADER), getplen(doj, TRADER));
        load_trader_profiles(jlc, ltrader);
    } else {
        load_trader_profiles(jlc, 0x0);
    }

    databuf_t *buff = databuf_alloc(400);
    int mod_len = 0;

    if (tlen > 0) {
        databuf_write(buff,
                "J,,4,,,,%s,,,,,,,,,,,,,,,,,,,,Done loading trader: %s,,,,,,,,,,,,,,,\n",
                get_mod_name(pc->gk, &mod_len), ltrader);
    } else {
        databuf_write(buff,
                "J,,4,,,,%s,,,,,,,,,,,,,,,,,,,,Done loading traders,,,,,,,,,,,,,,,\n",
                get_mod_name(pc->gk, &mod_len));
    }
    oats_raw_send_wrapper(buff, pc);
    databuf_destroy(buff);
}
static void message_rate_update(con_int* ci, dart_order_obj* doj)
{
    //jp_limits_checker *jlc = get_balance_struct(ci->gk);
    int mod_len = 0;
    char* ptrader = getpval(doj, TRADER);
    int plen = getplen(doj, TRADER);
    if (plen > 0) {
        //char* action = getpval(doj,ROM_CAP);
        //int a = strtol(action, NULL, 10);
            char* s[32];
            memset(s, '\0', 33* sizeof(char));
            memcpy(s, ptrader, plen);
        databuf_t *buff = databuf_alloc(400);
        databuf_write(buff,
                "J,,35,,,,%s,,,,,,,,,,,,,,,,,,,,Done Changing Msg Rate trader: %s,,,,,,,,,,,,,,,\n",
                get_mod_name(ci->gk, &mod_len), s);
        oats_raw_send_wrapper(buff, ci);
        databuf_destroy(buff);
    } 
}
/**
 *
void jp_start_stop_equity_check(jp_limits_checker* jpc, int should_check);
void jp_exempt_sym(jp_limits_checker* jpc, char* sym, int len, int exempt);
 */
static void equity_sym_update(con_int* ci,dart_order_obj* doj)
{
    jp_limits_checker *jlc = get_balance_struct(ci->gk);
    databuf_t *buff = databuf_alloc(400);
    int mod_len = 0;
    char* sec_type = getpval(doj, ROM_SEC_TYPE);
    int sec_len = getplen(doj, ROM_SEC_TYPE);
    if(sec_len > 0 && sec_type[0] == 'E') {
        char* action = getpval(doj,ROM_CAP);
        int a = strtol(action, NULL, 10);
        char* symbol = getpval(doj, ROM_TRADE_FOR);
        int sym_len = getplen(doj, ROM_TRADE_FOR);
        if (sym_len > 0) {
            jp_exempt_sym(jlc, symbol, sym_len, a);
            char* s[32];
            memset(s, '\0', 33*sizeof(char));
            memcpy(s, symbol, sym_len);
            databuf_write(buff,
                    "J,,36,,,,%s,,,,,,,,,,,,,,,,,,,,Symbol: %s should we check? %d,,,,,,,,,,,,,,,\n",
                    get_mod_name(ci->gk, &mod_len), s, a);
        } else {
            short update = a == 1 ? 0 : 1;
            jp_start_stop_equity_check(jlc, update);
            databuf_write(buff,
                    "J,,36,,,,%s,,,,,,,,,,,,,,,,,,,,Equity price checking %d,,,,,,,,,,,,,,,\n",
                    get_mod_name(ci->gk, &mod_len), a);
        }
        oats_raw_send_wrapper(buff, ci);
        databuf_destroy(buff);
    }
}
static void reload_clraccnt_limits(con_int *pc, dart_order_obj* doj)
{
    jp_limits_checker *jlc = get_balance_struct(pc->gk);

    char* pkey = NULL;
    char lkey[128];

    if (getplen(doj, TRADER) > 0)
    {
        memcpy(lkey, getpval(doj, TRADER), getplen(doj, TRADER));
        lkey[getplen(doj, TRADER)] = '\0';
        pkey = lkey;
    }

    load_clraccnt_limit(jlc, pkey);

    databuf_t *buff = databuf_alloc(400);
    int mod_len = 0;
    if (pkey)
    {
        databuf_write(buff,
                "J,,26,,,,%s,,,,,,,,,,,,,,,,,,,,Done loading clearing account limits(%s),,,,,,,,\n",
                get_mod_name(pc->gk, &mod_len), pkey);
    }
    else
    {
        databuf_write(buff,
                "J,,26,,,,%s,,,,,,,,,,,,,,,,,,,,Done loading clearing account limits,,,,,,,,,,,,,,,\n",
                get_mod_name(pc->gk, &mod_len));
    }
    oats_raw_send_wrapper(buff, pc);
    databuf_destroy(buff);
}
static void reload_billinggrp_limits(con_int* pc, dart_order_obj* doj)
{
    jp_limits_checker *jlc = get_balance_struct(pc->gk);

    char lkey[128];
    if (getplen(doj, TRADER) > 0) {
        memset(lkey, '\0', 128);
        memcpy(lkey, getpval(doj, TRADER), getplen(doj, TRADER));
    }


    load_billing_group(jlc, lkey);

    databuf_t *buff = databuf_alloc(400);
    int mod_len = 0;
    if (getplen(doj, TRADER) > 0)
    {
        databuf_write(buff,
                "J,,26,,,,%s,,,,,,,,,,,,,,,,,,,,Done loading billing group limits(billing group ID=%s),,,,,,,,\n",
                get_mod_name(pc->gk, &mod_len), lkey);
    }
    else
    {
        databuf_write(buff,
                "J,,26,,,,%s,,,,,,,,,,,,,,,,,,,,Done loading billing group limits,,,,,,,,,,,,,,,\n",
                get_mod_name(pc->gk, &mod_len));
    }
    oats_raw_send_wrapper(buff, pc);
    databuf_destroy(buff);
}

void call_from_user_updates(dart_order_obj * doj, int cancel_all,
        con_int *pc)
{
    con_int *ci = 0;
    int found = 0;
    int own_len = getplen(doj, ROM_OWNER);
    char *owner = getpval(doj, ROM_OWNER);
    if (own_len > 0) {
        found = get_obj_by_client_id(pc->gk, owner, own_len,
                (void **) &ci);
        if(found && ci->connected) {
            //do nothing we will send cancel
        } else {
            found = get_destination(pc->gk,
                    (char *) doj->positions[ROM_DEST].iov_base,
                    doj->positions[ROM_DEST].iov_len,
                    (void **) &ci);
            if(!found) {
                found =
                    get_destination(pc->gk, doj->positions[ROM_DESTID].iov_base,
                            doj->positions[ROM_DESTID].iov_len,
                            (void **) &ci);
            }
        }
    } else {
        found = get_destination(pc->gk,
                (char *) doj->positions[ROM_DEST].iov_base,
                doj->positions[ROM_DEST].iov_len,
                (void **) &ci);
        if(!found) {
            found =
                get_destination(pc->gk, doj->positions[ROM_DESTID].iov_base,
                        doj->positions[ROM_DESTID].iov_len,
                        (void **) &ci);
        }
    }
    if (found) {
        if (cancel_all) {
            doj->sender = pc;
            rcntr lcnt;
            memset(&lcnt, '\0', sizeof(rcntr));
            lcnt.raw = doj;
            lcnt.rlen = sizeof(dart_order_obj);
            doj->risk = lcnt;
            if(own_len > 0) {
                set_rom_field(doj, ROM_SENDER, owner, own_len);
                memcpy(&doj->risk.owner, owner, own_len);
                doj->risk.olen = own_len;
            }
            if(pc->on_cancel_all) {
                pc->on_cancel_all(ci);
            }
        } else {
            int check_tag = ROM_CLIENT_ID;
            if(ci->logon == 0x0) {
                check_tag = ROM_CLORDID;
            } else {
                check_tag = ci->logon->do_balance ? ROM_CLIENT_ID : ROM_TAG;
            }
            rcntr* cnt = 0;
            found = get_map_entry(ci->cl_ord_id_map, doj->positions[check_tag].iov_base,
                                      doj->positions[check_tag].iov_len, (void**)&cnt);
            if(found && cnt->rlen == sizeof(dart_order_obj)) {
                memcpy(&doj->risk, cnt, sizeof(rcntr));
                doj->sender = pc;
                if(own_len > 0) {
                    set_rom_field(doj, ROM_SENDER, owner, own_len);
                }
                set_rom_field(doj, ROM_STATUS, "26", 2);
                ci->on_cancel_order(ci, &doj->risk);
            }
        }
    } 
}

static void remove_zap_from_dest(con_int* ci, dart_order_obj* doj)
{
    if(doj->positions[ROM_CLORDID].iov_len > 0) {
        pthread_spin_lock(&ci->mutex);
	remove_from_map(ci->cl_ord_id_map, doj->positions[ROM_CLORDID].iov_base,
                        doj->positions[ROM_CLORDID].iov_len);
        pthread_spin_unlock(&ci->mutex);
    }
}

static void process_zap_or_fill(con_int* ci, dart_order_obj* doj,
                                int zapped, con_int* di)
{
    dart_order_obj *orig = 0;
    rcntr* cnt = 0;
    pthread_spin_lock(&ci->mutex);
    int found = get_map_entry(ci->cl_ord_id_map,
                              doj->positions[ROM_CLIENT_ID].iov_base,
                              doj->positions[ROM_CLIENT_ID].iov_len,
                              (void **) &cnt);
    pthread_spin_unlock(&ci->mutex);
    if(found && cnt->rlen == sizeof(dart_order_obj)) {
        orig = cnt->raw;
        memcpy(&orig->risk, cnt, sizeof(rcntr));
        set_rom_field(orig, 0, "S", 1);
        if(zapped) {
	    if(orig->dest != 0x0) {
                remove_zap_from_dest(((con_int*)orig->dest),orig); 
            } else {
                con_int *dest = 0;
                int found = 0;
                int own_len = getplen(doj, ROM_CLORDID);
                char *owner = getpval(doj, ROM_CLORDID);
                if (own_len > 0) {
                    found = get_obj_by_client_id(ci->gk, owner, own_len,
                                                (void **) &dest);
                }
                if(found) {
                   remove_zap_from_dest(dest,orig); 
                } 
            }
            set_rom_field(orig, ROM_TEXT, "Zapped", 6);
            set_rom_field(orig, ROM_STATUS, "4", 1);
            if(ci->on_out) {
                ci->on_out(ci, &orig->risk);
            } else {
                send_oats_message(di, &orig->risk);
            }
        } else {
            set_rom_field(orig, ROM_STATUS, getpval(doj, ROM_STATUS),
                          getplen(doj, ROM_STATUS));
            set_rom_field(orig, ROM_EXEC_PRICE, getpval(doj, ROM_EXEC_PRICE),
                          getplen(doj, ROM_EXEC_PRICE));
            set_rom_field(orig, ROM_LAST_SHARES, getpval(doj, ROM_LAST_SHARES),
                          getplen(doj, ROM_LAST_SHARES));
            char cb_array [16];
            memset(cb_array, '\0', 16);
            memcpy(cb_array, getpval(doj, ROM_LAST_SHARES), getplen(doj, ROM_LAST_SHARES));
            int bls = atoi(cb_array);
            set_rom_field(orig, ROM_BIN_LAST_SHARES, &bls, 4);
            if(ci->on_fill) {
                ci->on_fill(ci, &orig->risk);
            } else {
                send_oats_message(di, &orig->risk);
            }
        }
    }
}

void call_to_user_updates(dart_order_obj * doj, int zapped,
        con_int *pc)
{
    con_int *ci = 0;
    int found = 0;
    int own_len = getplen(doj, ROM_OWNER);
    char *owner = getpval(doj, ROM_OWNER);
    if (own_len > 0) {
        found = get_obj_by_client_id(pc->gk, owner, own_len,
                (void **) &ci);
    } else {
        return;
    }
    if (found) {
        process_zap_or_fill(ci, doj, zapped, pc);
    }
}
void oats_update(void* p, dart_order_obj* doj)
{
    con_int* ci = (con_int*)p;
    char* type = (char*)getpval(doj, 0);
        if (type[0] == 'N') {
            if(getplen(doj, 4) > 0) {
                int cfe = atoi(getpval(doj, 4));
                switch (cfe) {
                    case BLOCK_USER:
                        block_user(doj, ci);
                        break;
                    case DISCONNECT_USER:
                        disconnect_user(doj, ci);
                        break;
                    case EXEMPT_USER:
                        exempt_user(doj, ci);
                        break;
                    case EXEMPT_BILLINGGRP:
                        exempt_group(doj, ci);
                        break;
                    case RELOAD_DEST_IPPORT:
                        reload_dest_ipport(doj, ci);
                        break;
                    case RESET_SEQ:
                        break;
                    case QUERY_DESTS:
                        get_current_dests(ci);
                        break;
                    case QUERY_USERS:
                        get_current_users(ci);
                        break;
                    case DISCONNECT_DEST:
                        send_dest_logout(doj, ci);
                        disconnect_dest(doj, ci);
                        break;
                    case MOD_DEST_ACCESS:
                        block_dest(doj, ci);
                        break;
                    case DESTROUTE_UPDATE:
                        update_route(doj, ci);
                        break;
                    case RELOAD_TRADERS:
                        reload_traders(ci, doj);
                        break;
                    case RELOAD_CLEARINGACT_LIMITS:
                        reload_clraccnt_limits(ci, doj);
                        break;
                    case USER_INFO:
                        send_back_status(ci, doj);
                    case CLEARINGACT_SEC_LIMITS_STATUS:
                        send_back_clrlimitinfo(ci, doj);
                        break;
                    case BILLINGGRP_LIMIT_STATUS:
                        send_back_billinginfo(ci, doj);
                        break;
                    case RELOAD_BILLINGGRP_LIMITS:
                        reload_billinggrp_limits(ci, doj);
                        break;
                    case MSG_RATE_LIMIT:
                        message_rate_update(ci, doj);
                        break;
                    case EQUITY_SYM_CHECK:
                        equity_sym_update(ci, doj);
                        break;
                    default:
                        break;
                }
            }
        } else {
            switch (type[0]) {
                case 'C':
                    call_from_user_updates(doj, 0, ci);
                    break;
                case 'c':
                    call_from_user_updates(doj, 1, ci);
                    break;
                case 'Z':
                    call_to_user_updates(doj, 1, ci);
                    break;
                case 'S':
                    call_to_user_updates(doj, 0, ci);
                    break;
                default:
                    break;
            }
        }
}

static void push_oats_crap(con_int *pc)
{
    get_current_routes(pc);
    get_current_dests(pc);
    get_current_users(pc);
}

void on_oats_rc_connect(void* ap, int is_connected,
                     dart_order_obj *log)
{
    on_oats_connect((dartp*)ap, is_connected, log);
}


void on_oats_connect(dartp* dp, int is_connected,
                     dart_order_obj *log)
{
    con_int* cb = (con_int*)get_pass_back_point(dp);
    cb->connected = is_connected;
    switch (is_connected) {
        case 0:
            cb->can_send_orders = 0;
            cb->connected = 0;
            reload_dartp_config(dp);
            dart_reconnect(dp);
            break;
        case 1:
            cb->can_send_orders = 1;
            cb->connected = 1;
            push_oats_crap(cb);
            break;
        case 2:
            cb->can_send_orders = 0;
            cb->connected = 1;
            break;
    }
}

