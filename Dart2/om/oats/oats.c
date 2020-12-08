
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

#include "oats.h"
#include "message_queue.h"
#include "parser_func.h"
#include "dart_constants.h"
#include "config_loader.h"
#include "ex_config_helper.h"
#include "rom_handler.h"
#include "connection_interface.h"
#include "debug_logger.h"
#include "jigglypuff.h"
#include "gatekeeper.h"
#include "debug_logger.h"
#include "malloc.h"
#include "mailer.h"

struct oats {
    hashmap_t destinations;
    hashmap_t clients;
    con_interface *oat_ci;
    pthread_spinlock_t oats_mux;
};


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
void destroy_oats(oats_t* oat)
{
    printf("Destroying oats!!\n");
    clean_oats_map(oat->destinations);
    clean_oats_map(oat->clients);
    free(oat);
}

oats_t *create_oats_struct(con_interface * oci)
{
    struct oats *oat = malloc(sizeof(struct oats));
    oat->destinations = create_map(128);
    oat->clients = create_map(128);
    oat->oat_ci = oci;
    pthread_spin_init(&oat->oats_mux, PTHREAD_PROCESS_PRIVATE);
    return oat;
}

static void oats_raw_send_wrapper(databuf_t * buff, con_interface * ci)
{
    if (ci && ci->connected) {
        ci_async_send("OATS", 4, buff, ci);
        buff->rd_ptr = buff->buffer;
    }
}

void parse_oats(int *cut_con, struct async_parse_args *pc);
int on_oats_enter_order(con_interface * ci, dart_order_obj * doj,
        struct async_parse_args *ap)
{
    return 0;
}

int on_oats_cancel_order(con_interface * ci, dart_order_obj * doj)
{
    return 1;
}

void on_oats_replace_order(con_interface * ci, dart_order_obj * doj)
{
}

int build_oats_offset(struct async_parse_args *apa)
{
    return 0;
}

void on_oats_connect(struct connection_interface *cb, int is_connected)
{
    switch (is_connected) {
        case 0:
            cb->can_send_orders = 0;
            cb->connected = 0;
            reload_config(cb->connection_conf);
            reload_init_args(cb->session_conf);
            cb->ap->port = cb->connection_conf->fix_port;
            cb->ap->ip = (char *) cb->connection_conf->fix_ip;
            cb->ap->ip_len = cb->connection_conf->fix_ip_len;
            if(cb->recon_wrapper == 0) {
                cb->recon_wrapper = add_timer(cb->ass, 0,
                        0, 30, 0, cb->ap,async_reconnect_t);
            } else {
                cb->recon_wrapper->stuff = cb->ap;
                cb->recon_wrapper->call_back_func = async_reconnect_t;
                add_back_recon_timer(cb->ass, cb->recon_wrapper);
            }

            //get_current_dests(cb->ap);
            break;
        case 1:
            cb->can_send_orders = 1;
            cb->connected = 1;
            
            //get_current_dests(cb->ap);
            break;
        case 2:
            cb->can_send_orders = 0;
            cb->connected = 1;
            break;
    }
}

void init_oats_seq(async_parse_args* pa)
{
    sequence_req *sr;
    sr = create_sequence_request(pa->in_args->name, pa->in_args->name_len,
                                 pa->in_args,
                                 build_offsets,
                                 create_rom_resend_pad, pa);
    get_sequence_numbers(sr);
    /* Add the sequences into the connection object */
    pa->out_seq = dsr_get_outgoing(sr);
    destroy_sequence_req(sr);
}

con_interface *create_oats_connection(char *filename, int name_len,
        gatekeeper * gk)
{

    ex_config *ex = init_config(filename, name_len);
    struct init_args *fix_args = load_fix_config(filename, name_len);
    con_interface* con = create_empty_interface();
    con->connect = init_async_rom_logon;
    con->build_offset = build_oats_offset;
    con->on_enter_order = on_oats_enter_order;
    con->on_cancel_order = on_oats_cancel_order;
    con->on_replace_order = on_oats_replace_order;
    con->cl_ord_id_map = 0;
    con->module_name = calloc(ex->mod_name_len + 1, 1);
    memcpy(con->module_name, ex->mod_name, ex->mod_name_len);
    con->module_name_len = ex->mod_name_len;
    con->fix_con_name = calloc(fix_args->tc_len + 1, 1);
    memcpy(con->fix_con_name, fix_args->target_comp, fix_args->tc_len);
    con->fix_con_len = fix_args->tc_len;
    con->ass = NULL;
    con->session_conf = NULL;
    con->can_send_orders = 0;
    con->is_blocked = 0;
    con->pendingblock = 0;
    con->adminblockoverride = 0;
    con->cleansing_rinse = ci_cleansing_rinse;
    con->seq = 0;
    con->last_sent = 0;
    con->last_recv = 0;
    con->ip_str = calloc(64, 1);
    con->ci_iovec_sender = ci_async_iovec_sender;
    con->dest_id = NULL;
    con->dest_len = 0;
    con->cancel_on_dis = 0;
    con->do_oats_balance = 1;
    con->should_buffer = 0;
    con->client_mess_parse = parse_oats;
    con->connection_conf = ex;
    con->can_send_orders = 0;
    con->is_blocked = 0;
    con->session_conf = fix_args;
    con->on_connect = on_oats_connect;
    return con;
}


void add_client_connection(oats_t * oat, char *client_name, int name_len,
        databuf_t * buff)
{
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
}

void add_dest_connection(oats_t * oat, char *dest_name, int name_len,
        databuf_t * buff)
{
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
}

void remove_dest_connection(oats_t * oat, char *dest_name, int name_len,
        databuf_t * buff)
{
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

void remove_client_connection(oats_t * oat, char *dest_name, int name_len,
        databuf_t * buff)
{
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

void send_oats_update(async_parse_args * ap, int is_connected, char *name,
        int name_len, int client_or_serv, con_interface * ci)
{
    databuf_t *buff = databuf_alloc(1024);
    char l_name[100];
    memset(l_name, '\0', 100);
    int mod_len = 0;
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
    if (client_or_serv) {
        memcpy(l_name, name, name_len);
    }
    databuf_write(buff, "J,,%d,%s,,%d|%d|%d|1,%s,,,,,,,,,,,,,,,,,,,,", type,
            l_name, is_connected, logged_on, blocked,
            get_mod_name(ap->gk, &mod_len));
    if (is_connected) {
        struct sockaddr_in sa;
        memset(&sa, 0, sizeof(sa));
        socklen_t sa_len = sizeof(sa);
        getpeername(ap->sock, (struct sockaddr *) &sa, &sa_len);
        short p = ntohs(sa.sin_port);
        databuf_write(buff, "%s|%d", inet_ntoa(sa.sin_addr), p);
        if (ci) {
            if (ci->ip_str == NULL) {
                ci->ip_str = calloc(50, 1);
            }
            int len =
                sprintf(ci->ip_str, "%s|%d", inet_ntoa(sa.sin_addr), p);
            if (len >= 50) {
                memset(ci->ip_str + 40, '\0', 10);
            }
        }
    } else {
        if (ci->ip_str == NULL) {
            ci->ip_str = calloc(50, 1);
            databuf_write(buff, "%s", ci->ip_str);
        }
    }
    oats_t *oat = (oats_t *) get_compliance_con(ap->gk);
    if (is_connected && client_or_serv && oat) {
        databuf_memcpy(buff, ",,,,,,,,,,,,,,\n", 15);
        add_client_connection(oat, name, name_len, buff);
    } else if (client_or_serv && oat) {
        databuf_memcpy(buff, ",,,,,,,,,,,,,,\n", 15);
        remove_client_connection(oat, name, name_len, buff);
        databuf_destroy(buff);
    } else if (!client_or_serv && is_connected && oat) {
        databuf_write(buff, ",,,,,,,,,,,,,,,%s,\n",
                ci->connection_conf->dest_id);
        add_dest_connection(oat, name, name_len, buff);
    } else {
        if (oat) {
            databuf_write(buff, ",,,,,,,,,,,,,,,%s,\n", ci->connection_conf->dest_id);
            remove_dest_connection(oat, name, name_len, buff);
            databuf_destroy(buff);
        }
    }
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

static con_interface *get_interface_for_name(gatekeeper * gk, char *name,
        int name_len)
{
    con_interface *con = 0;
    int ret = get_obj_by_client_id(gk, name, name_len, (void *) &con);
    if (ret) {
        return con;
    } else {
        return 0;
    }
}


static void block_dest(const char *message, unsigned long len,
        struct async_parse_args *pc)
{
    int trader_len = 0;
    int is_blocked = 0;
    char *trader = get_rom_val(message, len, &trader_len, ORDER_DEST);
    char *action = get_rom_val(message, len, &is_blocked, ACTION);
    con_interface *ci = 0;
    int found = get_destination(pc->gk, trader, trader_len, (void *) &ci);
    if (found && trader_len > 0 && is_blocked > 0) {
        int sessionup = should_session_up(ci);
        is_blocked = strtol(action, NULL, 10);
        ci->is_blocked = is_blocked;
        ci->adminblockoverride = !ci->adminblockoverride;
        
        if (0 == sessionup &&
            0 == ci->is_blocked && 
            0 == ci->connected) {
            //in case there is no longer pending reconnect event
            //force reconnect!
            if (ci->recon_wrapper == 0) {
                ci->recon_wrapper = add_timer(ci->ass, 0,
                            0, 30, 0, ci->ap,async_reconnect_t);
            } else {
                ci->recon_wrapper->stuff = ci->ap;
                ci->recon_wrapper->call_back_func = async_reconnect_t;
                add_back_recon_timer(ci->ass, ci->recon_wrapper);
            }
        }
        
        send_oats_update(ci->ap, ci->connected, trader,
                trader_len, ci->ap->is_server, ci);
    }
}

static void update_route(const char *message, unsigned long len,
        struct async_parse_args *pc)
{
    int dest_len;
    int dest_rout_len;
    char *dest = get_rom_val(message, len, &dest_len, 13);
    char *dest_route = get_rom_val(message, len, &dest_rout_len, 41);
    if (dest_len > 0 && dest_rout_len > 0) {
        int change = reset_dest_routes(pc->gk, dest, dest_len, dest_route,
                dest_rout_len);
        databuf_t *buff = databuf_alloc(1024);
        int mod_len = 0;
        if (change) {
            databuf_memcpy(buff, "J", 1);
            databuf_memcpy(buff, message + 1, len - 1);
        } else {
            databuf_write(buff, "J,,11,,,,%s,,,,,,,",
                    get_mod_name(pc->gk, &mod_len));
            databuf_memcpy(buff, dest, dest_len);
            databuf_write(buff, ",,,,,,,,,,,,,INVALID,,,,,,,,,,,,,,,");
            databuf_memcpy(buff, dest_route, dest_rout_len);
            databuf_write(buff, ",,,,\n");
        }
        con_interface *ci = (con_interface *) pc->ds_book;
        oats_raw_send_wrapper(buff, ci);
    }
}

static void block_user(const char *message, unsigned long len,
        struct async_parse_args *pc)
{
    int trader_len = 0;
    int is_blocked = 0;
    char *trader = get_rom_val(message, len, &trader_len, TRADER);
    char *action = get_rom_val(message, len, &is_blocked, ACTION);
    if (trader_len > 0 && is_blocked > 0) {
        is_blocked = strtol(action, NULL, 10);
        jp_limits_checker *jlc = get_balance_struct(pc->gk);
        jp_block_trader(jlc, trader, trader_len, is_blocked);
        con_interface *ci =
            get_interface_for_name(pc->gk, trader, trader_len);
        if (ci) {
            ci->is_blocked = is_blocked;
            send_oats_update(ci->ap, ci->connected, trader,
                    trader_len, ci->ap->is_server, ci);
        }
    }
}

static void build_limits_message(struct async_parse_args *pc,
        struct jp_limits *limits, char type,
        con_interface * ci, char *trader,
        int trader_len, int num)
{
    databuf_t *buff = databuf_alloc(1000);
    databuf_write(buff, "J,,%d,", num);
    databuf_memcpy(buff, trader, trader_len);
    int mod_len = 0;
    databuf_write(buff, ",,,%s", get_mod_name(pc->gk, &mod_len));
    
    if (!limits)
    {
        databuf_write(buff, ",,,,,,,,,,,,,,,,,,,,limits not found,,,,,,,,,%c,,,,,,,,,,,,\n", type);
    }
    else
    {
        long mult = ('E' != type)? 100000000 : 10000;

        //int mod_len = 0;
        double mbp = (double) limits->bp / mult;
        double mdol = (double) limits->max_doll / mult;
        unsigned long lcb = labs(limits->cur_balance_buy);
        double cdol = (double) lcb / mult;
        double cdols = (double) (limits->cur_balance_sell / mult);
        //databuf_t *buff = databuf_alloc(1000);
        //databuf_write(buff, "J,,%d,", num);
        //databuf_memcpy(buff, trader, trader_len);
        databuf_write(buff,
                ",,,,,,,%ld,%ld,%ld,%ld,%f,%f|%f,%ld,%f,,,,,,,,,,,,,,,%c,,,,,,,,,,,,\n",
                limits->max_ord,
                limits->open_ords, limits->max_share,
                limits->open_shares, mbp, cdol, cdols, limits->max_shares_ord,
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


static void send_back_billinggrplimitinfo(struct async_parse_args *pc, char *message,
        int len)
{
    //the key value is actually the billing group id
    int key_len = 0;
    char *key = get_rom_val(message, len, &key_len, TRADER);
    int stlen = 0;
    char* char_type = get_rom_val(message, len, &stlen, 35);
    if(stlen > 0) {
        char sec_type = char_type[0];
        
        jp_limits_checker *jlc = get_balance_struct(pc->gk);
        con_interface *ci = (con_interface *) pc->ds_book;
        jp_limits *limits = 0;
            
        if (stlen > 0) {
            switch(sec_type) {
                case 'O':
                    sec_type = 'O';
                    limits = get_equity_option_limits_for_billinggrp(jlc, key, key_len);
                    break;
                case 'M':
                    sec_type = 'M';
                    limits = get_fut_opt_limits_for_billinggrp(jlc, key, key_len);
                    break;
                case 'F':
                    sec_type = 'F';
                    limits = get_future_limits_for_billinggrp(jlc, key, key_len);
                    break;
                default:
                    limits = get_equity_limits_for_billinggrp(jlc, key, key_len);
                    break;
            }
            build_limits_message(pc, limits, sec_type, ci, key, key_len, BILLINGGRP_LIMIT_STATUS);
        }
        else
        {
            limits = get_equity_limits_for_billinggrp(jlc, key, key_len);
            build_limits_message(pc, limits, 'E', ci, key, key_len, BILLINGGRP_LIMIT_STATUS);
            
            if (limits)
            {
                limits = get_equity_option_limits_for_billinggrp(jlc, key, key_len);
                if (limits)
                {
                    build_limits_message(pc, limits, 'O', ci, key, key_len, BILLINGGRP_LIMIT_STATUS);
                }
                limits = get_future_limits_for_billinggrp(jlc, key, key_len);
                if (limits)
                {
                    build_limits_message(pc, limits, 'F', ci, key, key_len, BILLINGGRP_LIMIT_STATUS);
                }
                limits = get_fut_opt_limits_for_billinggrp(jlc, key, key_len);
                if (limits)
                {
                    build_limits_message(pc, limits, 'M', ci, key, key_len, BILLINGGRP_LIMIT_STATUS);
                }
            }
        }
    }
}

static void get_oat_fut(jp_limits_checker* jlc, char* message, int len,
                        char* trader, int trader_len,
                        struct async_parse_args* pc)
{
    con_interface *ci = (con_interface *) pc->ds_book;
    int slen = 0;
    char* sym = get_rom_val(message, len, &slen, 5);
    databuf_t* db = databuf_alloc(2048);
    get_future_limits_for_trader(jlc, trader, trader_len, db, pc->gk,
                                  sym, slen);
    if(databuf_unread(db) > 0) {
        oats_raw_send_wrapper(db, ci);
    }
    databuf_destroy(db);
}



static void send_back_status(struct async_parse_args *pc, char *message,
        int len)
{
    int trader_len;
    char *trader = get_rom_val(message, len, &trader_len, TRADER);
    int stlen = 0;
    char* sec_type = get_rom_val(message, len, &stlen, 35);
    if (trader_len > 0) {
        jp_limits_checker *jlc = get_balance_struct(pc->gk);
        con_interface *ci = (con_interface *) pc->ds_book;
        if(stlen > 0) {
            struct jp_limits* eq = 0;
            switch(sec_type[0]) {
                case 'M':
                    eq = get_of_limits_for_trader(jlc, trader, trader_len);
                    build_limits_message(pc, eq, 'M', ci, trader, trader_len, 24);
                    break;
                case 'E':
                    eq = get_equity_limits_for_trader(jlc, trader, trader_len);
                    build_limits_message(pc, eq, 'E', ci, trader, trader_len, 24);
                    break;
                case 'O':
                    eq = get_option_limits_for_trader(jlc, trader, trader_len);
                    build_limits_message(pc, eq, 'O', ci, trader, trader_len, 24);
                    break;
                case 'F':
                    get_oat_fut(jlc, message, len, trader, trader_len, pc);
                    break;
            }
        } else {
            struct jp_limits *eq =
                get_equity_limits_for_trader(jlc, trader, trader_len);
            build_limits_message(pc, eq, 'E', ci, trader, trader_len, 24);
            if (eq) 
            {
                eq = get_option_limits_for_trader(jlc, trader, trader_len);
                if (eq) {
                    build_limits_message(pc, eq, 'O', ci, trader, trader_len, 24);
                }
                get_oat_fut(jlc, message, len, trader, trader_len, pc);
                eq = get_of_limits_for_trader(jlc, trader, trader_len);
                if (eq) {
                    build_limits_message(pc, eq, 'M', ci, trader, trader_len, 24);
                }
            }
        }
    }
}

static void disconnect_user(const char *message, unsigned long len,
        struct async_parse_args *pc)
{
    int trader_len = 0;
    char *trader = get_rom_val(message, len, &trader_len, TRADER);
    con_interface *ci = get_interface_for_name(pc->gk, trader, trader_len);
    if (ci) {
        ci->connected = 0;
        ci->ap->disconnect = 1;
        send_oats_update(ci->ap, ci->connected, trader,
                trader_len, ci->ap->is_server, ci);
    }
    disconnect_by_name(pc->ass, trader, trader_len);
}

static void exempt_user(const char *message, unsigned long len,
        struct async_parse_args *pc)
{
    int trader_len = 0;
    int is_exempt = 0;
    char *trader = get_rom_val(message, len, &trader_len, TRADER);
    char *action = get_rom_val(message, len, &is_exempt, ACTION);
    if (trader_len > 0 && is_exempt > 0) {
        is_exempt = strtol(action, NULL, 10);
        jp_limits_checker *jlc = get_balance_struct(pc->gk);
        jp_exempt_trader(jlc, trader, trader_len, is_exempt);
    }
}
static void exempt_group(const char* message, unsigned long len,
        struct async_parse_args* pc)
{
    int trader_len = 0;
    int is_exempt = 0;
    char *trader = get_rom_val(message, len, &trader_len, TRADER);
    char *action = get_rom_val(message, len, &is_exempt, ACTION);
    if(trader_len > 0 && is_exempt > 0) {
        is_exempt = strtol(action, NULL, 10);
        jp_limits_checker *jlc = get_balance_struct(pc->gk);
        jp_exempt_billing_group(jlc,trader, trader_len,is_exempt);
    }
}

static void reload_dest_ipport(const char *message, unsigned long len,
        struct async_parse_args *pc)
{
    int trader_len = 0;
    char *trader = get_rom_val(message, len, &trader_len, ORDER_DEST);
    con_interface *con_int = 0;
    int found = get_destination(pc->gk, trader,
            trader_len, (void **) &con_int);
    if (found) {
        ci_reload_ip_port(con_int);
    }
}

static void disconnect_dest(const char *message, unsigned long len,
        struct async_parse_args *pc)
{
    int trader_len = 0;
    char *trader = get_rom_val(message, len, &trader_len, ORDER_DEST);
    disconnect_by_name(pc->ass, trader, trader_len);
}

static void get_current_routes(async_parse_args* pc)
{
    databuf_t *buff = databuf_alloc(2000);
    int mod_len = 0;
    databuf_write(buff, "J,,12,,,,%s,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,",
            get_mod_name(pc->gk, &mod_len));
    print_dest_routes(pc->gk, buff);
    databuf_write(buff, ",,,,,,,,\n", 9);
    con_interface *ci = (con_interface *) pc->ds_book;
    oats_raw_send_wrapper(buff, ci);
    databuf_destroy(buff);
}

static void get_current_dests(async_parse_args * pc)
{
    oats_t *oat = (oats_t *) get_compliance_con(pc->gk);
    hashmap_iterator_t hi = hashmap_iterator_begin(oat->destinations);
    databuf_t *buff = 0;
    char *key = 0;
    int key_len = 0;
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                (void *) &buff);
    while (valid) {
        oats_raw_send_wrapper(buff, oat->oat_ci);
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

static void get_current_users(async_parse_args * pc)
{
    oats_t *oat = (oats_t *) get_compliance_con(pc->gk);
    hashmap_iterator_t hi = hashmap_iterator_begin(oat->clients);
    databuf_t *buff = 0;
    char *key = 0;
    int key_len = 0;
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                (void *) &buff);
    while (valid) {
        oats_raw_send_wrapper(buff, oat->oat_ci);
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

static void reload_traders(async_parse_args * pc, const char* trader, int trader_len)
{
    jp_limits_checker *jlc = get_balance_struct(pc->gk);

    char* ptrader = NULL;
    char ltrader[128];

    if (trader_len)
    {
        memcpy(ltrader, trader, trader_len);
        ltrader[trader_len] = '\0';
        ptrader = ltrader;
    }
    load_trader_profiles(jlc, ptrader);

    databuf_t *buff = databuf_alloc(400);
    int mod_len = 0;

    if (ptrader)
    {
        databuf_write(buff,
                "J,,4,,,,%s,,,,,,,,,,,,,,,,,,,,Done loading trader: %s,,,,,,,,,,,,,,,\n",
                get_mod_name(pc->gk, &mod_len), ptrader);
    }
    else
    {
        databuf_write(buff,
                "J,,4,,,,%s,,,,,,,,,,,,,,,,,,,,Done loading traders,,,,,,,,,,,,,,,\n",
                get_mod_name(pc->gk, &mod_len));
    }
    con_interface *ci = (con_interface *) pc->ds_book;
    oats_raw_send_wrapper(buff, ci);
    databuf_destroy(buff);
}

static void equity_sym_update(async_parse_args* pc,char* message, int len)
{
    jp_limits_checker *jlc = get_balance_struct(pc->gk);
    databuf_t *buff = databuf_alloc(400);
    int mod_len = 0;
    int l_len = 0;
    int sym_len = 0;
    char *action = get_rom_val(message, len, &l_len, ACTION);
    char* symbol = get_rom_val(message, len, &sym_len,ROM_TRADE_FOR);
    int a = strtol(action, NULL, 10);
    if (sym_len > 0) {
        jp_exempt_sym(jlc, symbol, sym_len, a);
        char* s[32];
        memset(s, '\0', 32);
        memcpy(s, symbol, sym_len);
        databuf_write(buff,
                "J,,36,,,,%s,,,,,,,,,,,,,,,,,,,,Symbol: %s should we check? %d,,,,,,,,,,,,,,,\n",
                get_mod_name(pc->gk, &mod_len), s, a);
    } else {
        short update = a == 1 ? 0 : 1;
        jp_start_stop_equity_check(jlc, update);
        databuf_write(buff,
                "J,,36,,,,%s,,,,,,,,,,,,,,,,,,,,Equity price checking %d,,,,,,,,,,,,,,,\n",
                get_mod_name(pc->gk, &mod_len), a);
    }
    con_interface *ci = (con_interface *) pc->ds_book;
    oats_raw_send_wrapper(buff, ci);
    databuf_destroy(buff);
}
static void reload_billinggrp_limits(async_parse_args *pc, const char* key, int key_len)
{
    jp_limits_checker *jlc = get_balance_struct(pc->gk);

    char* pkey = NULL;
    char lkey[128];

    if (key_len > 0)
    {
        memcpy(lkey, key, key_len);
        lkey[key_len] = '\0';
        pkey = lkey;
    }

    load_billinggrp_limit(jlc, pkey);

    databuf_t *buff = databuf_alloc(400);
    int mod_len = 0;
    if (pkey)
    {
        databuf_write(buff,
                "J,,26,,,,%s,,,,,,,,,,,,,,,,,,,,Done loading billing group limits(billing group ID=%s),,,,,,,,\n",
                get_mod_name(pc->gk, &mod_len), pkey);
    }
    else
    {
        databuf_write(buff,
                "J,,26,,,,%s,,,,,,,,,,,,,,,,,,,,Done loading billing group limits,,,,,,,,,,,,,,,\n",
                get_mod_name(pc->gk, &mod_len));
    }
    con_interface *ci = (con_interface *) pc->ds_book;
    oats_raw_send_wrapper(buff, ci);
    databuf_destroy(buff);
}

void call_from_user_updates(dart_order_obj * doj, int cancel_all,
        struct async_parse_args *pc)
{
    con_interface *ci = 0;
    int found = 0;
    int own_len = getplen(doj, ROM_OWNER);
    char *owner = getpval(doj, ROM_OWNER);
    if (own_len > 0) {
        found = get_obj_by_client_id(pc->gk, owner, own_len,
                (void **) &ci);
    } else {
        found = get_destination(pc->gk,
                (char *) doj->positions[ROM_DEST].iov_base,
                doj->positions[ROM_DEST].iov_len,
                (void **) &ci);
    }
    if (found) {
        if (cancel_all) {
            con_interface *di = (con_interface *) pc->ds_book;
            doj->sender = di;
            if(own_len > 0) {
                set_rom_field(doj, ROM_SENDER, owner, own_len);
            }
            di->on_cancel_all(ci, doj);
        } else {
            con_interface *di = (con_interface *) pc->ds_book;
            doj->sender = di;
            if(own_len > 0) {
                set_rom_field(doj, ROM_SENDER, owner, own_len);
            }
            di->on_cancel_order(ci, doj);
        }
    } else {
        found =
            get_destination(pc->gk, doj->positions[ROM_DESTID].iov_base,
                    doj->positions[ROM_DESTID].iov_len,
                    (void **) &ci);
        if (found) {
            if (cancel_all) {
                con_interface *di = (con_interface *) pc->ds_book;
                if(own_len > 0) {
                    set_rom_field(doj, ROM_SENDER, owner, own_len);
                }
                di->on_cancel_all(ci, doj);
            } else {
                con_interface *di = (con_interface *) pc->ds_book;
                if(own_len > 0) {
                    set_rom_field(doj, ROM_SENDER, owner, own_len);
                }
                di->on_cancel_order(ci, doj);
            }
        }
    }
}

static void process_zap_or_fill(con_interface* ci, dart_order_obj* doj,
                                int zapped, con_interface* di)
{
    dart_order_obj *orig = 0;
    pthread_spin_lock(&ci->mutex);
    int found = get_map_entry(ci->cl_ord_id_map,
                              doj->positions[ROM_CLIENT_ID].iov_base,
                              doj->positions[ROM_CLIENT_ID].iov_len,
                              (void **) &orig);
    pthread_spin_unlock(&ci->mutex);
    if(found) {
        reset_rom_field(orig, 0, "S", 1);
        if(zapped) {
            reset_rom_field(orig, ROM_TEXT, "Zapped", 6);
            reset_rom_field(orig, ROM_STATUS, "4", 1);
            di->on_out(ci, orig);
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
            di->on_fill(ci, orig);
        }
    }
}

void call_to_user_updates(dart_order_obj * doj, int zapped,
        struct async_parse_args *pc)
{
    con_interface *ci = 0;
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
        con_interface *di = (con_interface *) pc->ds_book;
        process_zap_or_fill(ci, doj, zapped, di);
    }
}

void process_config_reset(dart_order_obj* doj, struct async_parse_args* pc)
{
    con_interface *ci = 0;
    int found = 0;
    int own_len = getplen(doj, ROM_DEST);
    char *owner = getpval(doj, ROM_DEST);
    if (own_len > 0) {
        found = get_destination(pc->gk, owner, own_len, (void *) &ci);
    } else {
        send_debug_message("Could not find this owner for config: %s \n", owner);
        return;
    }
    send_debug_message("Did we find exchange? %d if not must be reset seq \n", found);
    if (found) {
        if(ci && ci->reset_seq) {
        send_debug_message("Found this owner reloading seq: %s \n", owner);
            ci->reset_seq(ci, 0,0);
        } else {
        send_debug_message("Is owner Null? %lu if not must be reset seq \n", ci);
        }
    }
}

static unsigned long read_oats_commands(const char *message,
        unsigned long len,
        struct async_parse_args *pc)
{
    unsigned long bytes_used = 0;
    char *start = (char *) message;
    unsigned long len_left = len;
    unsigned long mess_len = 0;
    char *end = memchr(start, '\n', len);
    while (end) {
        mess_len = (end - start) + 1;
        if (start[0] == 'N') {
            int v = 0;
            char *d_off = get_rom_val(start, len, &v, 2);
            char *error = 0;
            int cfc = strtol(d_off, &error, 10);
            if (error && error[0] == ',' && error - d_off > 0) {
                switch (cfc) {
                    case BLOCK_USER:
                        block_user(start, len, pc);
                        break;
                    case DISCONNECT_USER:
                        disconnect_user(start, len, pc);
                        break;
                    case EXEMPT_USER:
                        exempt_user(start, len, pc);
                        break;
                    case EXEMPT_BILLINGGRP:
                        exempt_group(start, len, pc);
                        break;
                    case RELOAD_DEST_IPPORT:
                        reload_dest_ipport(start, len, pc);
                        break;
                    case RESET_SEQ:
                        break;
                    case QUERY_DESTS:
                        get_current_dests(pc);
                        break;
                    case QUERY_USERS:
                        get_current_users(pc);
                        break;
                    case DISCONNECT_DEST:
                        disconnect_dest(start, len, pc);
                        break;
                    case MOD_DEST_ACCESS:
                        block_dest(start, len, pc);
                        break;
                    case DESTROUTE_UPDATE:
                        update_route(start, len, pc);
                        break;
                    case RELOAD_TRADERS:
                        d_off = get_rom_val(start, len, &v, 3);
                        reload_traders(pc, d_off, v);
                        break;
                    //case RELOAD_CLEARINGACT_LIMITS:
                    case RELOAD_BILLINGGRP_LIMITS:
                        d_off = get_rom_val(start, len, &v, 3);
                        reload_billinggrp_limits(pc, d_off, v);
                        break;
                    case USER_INFO:
                        send_back_status(pc, start, len);
                        break;
                    //case CLEARINGACT_SEC_LIMITS_STATUS:
                    case BILLINGGRP_LIMIT_STATUS:
                        send_back_billinggrplimitinfo(pc, start, len);
                        break;
                    case EQUITY_SYM_CHECK:
                        equity_sym_update(pc, start, len);
                        break;
                    default:
                        break;
                }
            }
        } else {
            char command = start[0];
            dart_order_obj *cur_doj = 0;
            switch (command) {
                case 'C':
                    cur_doj = (dart_order_obj *) get_gk_obj(pc->gk);
                    databuf_memcpy(cur_doj->current->data, start, mess_len);
                    ph_build_iovec(cur_doj);
                    call_from_user_updates(cur_doj, 0, pc);
                    break;
                case 'c':
                    cur_doj = (dart_order_obj *) get_gk_obj(pc->gk);
                    databuf_memcpy(cur_doj->current->data, start, mess_len);
                    {
                        /**
                         * DEBUG REMOVE
                         */
                        char deb[mess_len+1];
                        memset(deb, '\0', mess_len + 1);
                        memcpy(deb, start, mess_len);
                        send_debug_message("Got this Cancel all from Oats: %s \n", deb);
                    }
                    ph_build_iovec(cur_doj);
                    call_from_user_updates(cur_doj, 1, pc);
                    break;
                case 'Z':
                    cur_doj = (dart_order_obj *) get_gk_obj(pc->gk);
                    databuf_memcpy(cur_doj->current->data, start, mess_len);
                    ph_build_iovec(cur_doj);
                    call_to_user_updates(cur_doj, 1, pc);
                    break;
                case 'S':
                    cur_doj = (dart_order_obj *) get_gk_obj(pc->gk);
                    databuf_memcpy(cur_doj->current->data, start, mess_len);
                    ph_build_iovec(cur_doj);
                    call_to_user_updates(cur_doj, 0, pc);
                    break;
                case 'P':
                    cur_doj = (dart_order_obj *) get_gk_obj(pc->gk);
                    databuf_memcpy(cur_doj->current->data, start, mess_len);
                    ph_build_iovec(cur_doj);
                    {
                        char deb[mess_len+1];
                        memset(deb, '\0', mess_len + 1);
                        memcpy(deb, start, mess_len);
                    send_debug_message("Got this reload_config from Oats: %s \n", deb);
                    }
                    process_config_reset(cur_doj, pc);                                         
                    break;
                default:
                    break;
            }
        }
        len_left -= mess_len;
        bytes_used += mess_len;
        start = end + 1;
        end = memchr(start, '\n', len_left);
    }
    return bytes_used;
}

static void push_oats_crap(struct async_parse_args *pc)
{
    get_current_routes(pc);
    get_current_dests(pc);
    get_current_users(pc);
}

void parse_oats(int *cut_con, struct async_parse_args *pc)
{
    char* message = pc->con->rd_ptr;
    long len = pc->con->wr_ptr - pc->con->rd_ptr;
    unsigned long bytes_used = 0;
    if (message[0] == 'L') {
        char *end = memchr(message, '\n', len);
        if (end != NULL && (end - message) <= len) {
            unsigned long log_len = (end - message) + 1;
            handle_async_csv_logon('L', message, log_len, cut_con, pc);
            push_oats_crap(pc);
            if (log_len < len) {
                bytes_used += log_len;
                bytes_used +=
                    read_oats_commands((message + log_len),
                            (len - log_len), pc);
            } else {
                bytes_used = log_len;
            }
        }
    } else {
        bytes_used = read_oats_commands(message, len, pc);
    }
    pc->con->rd_ptr += bytes_used;
}

void oats_rom_helper_func(dart_order_obj * rph, void *b)
{
}
