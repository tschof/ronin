
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

#include "message_queue.h"
#include "parser_func.h"
#include "dart_constants.h"
#include "config_loader.h"
#include "ex_config_helper.h"
#include "rom_handler.h"
#include "connection_interface.h"
#include "mailer.h"
#include "debug_logger.h"

#define MICRO_HOUR 3600000000
#define MICRO_MIN 60000000
#define MICRO_SEC 1000000


void dom_con(dart_order_obj * doj, void *a);


int send_dc_logon(struct async_parse_args *pa)
{
    return 0;
}


void dom_con_return(dart_order_obj * doj, void *a)
{
    if (doj->sender != NULL) {
        ((con_interface *) doj->sender)->ci_iovec_sender(doj->positions,
                                                         DART_SEND_LEN,
                                                         doj->positions
                                                         [ROM_SENDER].iov_base,
                                                         doj->positions
                                                         [ROM_SENDER].iov_len,
                                                         ((con_interface *)
                                                          doj->sender),
                                                         ((con_interface *)
                                                          doj->
                                                          sender)->sock);
        if(((con_interface*)doj->sender)->do_oats_balance) {
            increment_ex_tag(doj);
            send_oats_message(doj);
        }
        record_client_outbound(doj);
    }
}

int get_obj_by_id(con_interface * ob, void *clordid, int len,
                  dart_order_obj ** obj)
{
    int ret = 0;
    pthread_spin_lock(&ob->mutex);
    ret = get_map_entry(ob->cl_ord_id_map, clordid, len, (void **) obj);
    pthread_spin_unlock(&ob->mutex);
    return ret;
}

int build_dc_offset(struct async_parse_args *apa)
{
    return 0;
}

int on_dc_enter_order(con_interface * ci, dart_order_obj * doj,
                      struct async_parse_args *ap)
{
    pthread_spin_lock(&ci->mutex);
    no_copy_insert(ci->cl_ord_id_map,
                   doj->positions[ROM_TAG].iov_base,
                   doj->positions[ROM_TAG].iov_len, doj);
    pthread_spin_unlock(&ci->mutex);
    ci->ci_iovec_sender(doj->positions, DART_SEND_LEN,
                        ci->fix_con_name, ci->fix_con_len, ci, ci->sock);
    //record_client_outbound(doj);
    return 1;
}

int on_dc_cancel_order(con_interface * ci, dart_order_obj * doj)
{
    reset_rom_field(doj, 0, "C", 1);
    ci->ci_iovec_sender(doj->positions, DART_SEND_LEN,
                        ci->fix_con_name, ci->fix_con_len, ci, ci->sock);
    //record_client_outbound(doj);
    return 1;
}

void on_dc_replace_order(con_interface * ci, dart_order_obj * doj)
{
    dart_order_obj *rep = (dart_order_obj *) (doj->rep);
    reset_rom_field(rep, ROM_TAG, doj->positions[ROM_TAG].iov_base,
                    doj->positions[ROM_TAG].iov_len);
    if (rep->positions[ROM_SHARES].iov_len <= 0) {
        reset_rom_field(rep, ROM_ORIG_SHARES,
                        doj->positions[ROM_SHARES].iov_base,
                        doj->positions[ROM_SHARES].iov_len);
    } else {
        reset_rom_field(rep, ROM_ORIG_SHARES,
                        rep->positions[ROM_SHARES].iov_base,
                        rep->positions[ROM_SHARES].iov_len);
    }
    if (rep->positions[ROM_TYPE].iov_len <= 0) {
        reset_rom_field(rep, ROM_TYPE, doj->positions[ROM_TYPE].iov_base,
                        doj->positions[ROM_TYPE].iov_len);
    }
    ci->ci_iovec_sender(rep->positions, DART_SEND_LEN,
                        ci->fix_con_name, ci->fix_con_len, ci, ci->sock);
    //record_client_outbound(rep);
}


void create_connection(con_interface * con, char *filename, int name_len,
                       gatekeeper * gk)
{

    ex_config *ex = init_config(filename, name_len);
    struct init_args *fix_args = load_fix_config(filename, name_len);
    reset_con_interface(con, 30000,
                        (char *) ex->mod_name, ex->mod_name_len,
                        fix_args->target_comp, fix_args->tc_len,
                        init_async_rom_logon, build_dc_offset,
                        on_dc_enter_order, on_dc_cancel_order,
                        on_dc_replace_order, gk, (char *) ex->dest_id,
                        ex->dest_id_len);
    con->client_mess_parse = ph_parse_rom_message;
    con->ci_parse_v = ph_build_non_fix_rom_pos;
    con->ci_obj_callback = dom_con;
    con->connection_conf = ex;
    con->can_send_orders = 0;
    con->session_conf = fix_args;
    add_int_orderids(con);
}

void build_fill_not_found_mail(con_interface * ob, char *d_off,
                               databuf_t * buff)
{
    int last_shares = 0;
    int last_price = 0;
    double rom_price = 0;
    char scale = '4';
    memcpy(&last_shares, d_off + 32, 4);
    last_shares = ntohl(last_shares);
    memcpy(&last_price, d_off + 36, 4);
    last_price = ntohl(last_price);
    memcpy(&scale, d_off + 40, 1);
    rom_price = 0;
    char *title = "Unmatched fill from ARCA DIRECT";
    int t_len = 35;
    char *side = 0;
    if (d_off[30] == '1') {
        side = "Buy";
    } else {
        side = "Sell";
    }
    databuf_write(buff, "%s of %d shares @ %f.\n", side, last_shares,
                  rom_price);
    unsigned long order_id = 0;
    memcpy(&order_id, d_off + 16, 8);
    unsigned long exec_id = 0;
    memcpy(&exec_id, d_off + 24, 8);
    exec_id = ntohl(exec_id);
    int clordid = 0;
    memcpy(&clordid, d_off + 12, 4);
    clordid = ntohl(clordid);
    databuf_write(buff, "\nExec ID:%lu \nCL Ord ID: %d\nArca ID: %lu\n",
                  exec_id, clordid, order_id);
    databuf_write(buff, "Billing indicator: %c\n", d_off[33]);
    send_biz_message(buff->buffer, databuf_unread(buff), title, t_len);
}

static void remove_from_maps(con_interface * ci, char *id, int id_len)
{
    pthread_spin_lock(&ci->mutex);
    remove_from_map(ci->cl_ord_id_map, id, id_len);
    pthread_spin_unlock(&ci->mutex);
}

static int is_spread(dart_order_obj * doj)
{
    return doj->positions[ROM_SPREAD].iov_len > 0;
}

static void reset_fields_for_fill(dart_order_obj * orig,
                                  dart_order_obj * doj, con_interface* ob)
{
    fill_in_rom_field(orig, ROM_LAST_SHARES,
                      doj->positions[ROM_LAST_SHARES].iov_base,
                      doj->positions[ROM_LAST_SHARES].iov_len);
    fill_in_rom_field(orig, ROM_EXEC_PRICE,
                      doj->positions[ROM_EXEC_PRICE].iov_base,
                      doj->positions[ROM_EXEC_PRICE].iov_len);
    
    double d = strtod(doj->positions[ROM_EXEC_PRICE].iov_base, NULL);
    d = d * 10000;
    d += .0000001;
    long rblp = (long) d;
    fill_in_rom_field(orig, ROM_BIN_LAST_PRICE, &rblp, sizeof(long));
    int ls = strtol(doj->positions[ROM_LAST_SHARES].iov_base, NULL, 10);
    int qty = strtol(doj->positions[ROM_SHARES].iov_base, NULL, 10);
    fill_in_rom_field(orig, ROM_BIN_LAST_SHARES, &ls, sizeof(int));
    int cum = 0;
    if (orig->positions[ROM_BIN_CUM].iov_len > 0) {
        memcpy(&cum, orig->positions[ROM_BIN_CUM].iov_base, 4);
        cum += ls;
        fill_in_rom_field(orig, ROM_BIN_CUM, &cum, sizeof(int));

    } else {
        fill_in_rom_field(orig, ROM_BIN_CUM, &ls, sizeof(int));
        cum = ls;
    }
    if(getplen(doj, ROM_LIQ) > 0) {
        fill_in_rom_field(orig, ROM_LIQ, getpval(doj, ROM_LIQ),
                          getplen(doj, ROM_LIQ));
    }
    int leaves = qty - cum;
    databuf_t *itoa_buff = get_databuf(ob->gk);
    dart_itoa(orig, leaves, ROM_LEAVES, itoa_buff);
    dart_itoa(orig, cum, ROM_CUM, itoa_buff);
    destroy_gk_db(ob->gk, itoa_buff);
}

static void set_rep_fields(dart_order_obj * ute, dart_order_obj * rep, 
                           con_interface* ob)
{
    char *off = rep->positions[ROM_SHARES].iov_base;
    fill_in_rom_field(ute, ROM_SHARES, off,
                      rep->positions[ROM_SHARES].iov_len);
    int l_q = strtol(off, NULL, 10);
    //memcpy(ute->positions[ROM_BIN_QTY].iov_base, &l_q, 4);
    fill_in_rom_field(ute, ROM_BIN_QTY, &l_q, 4);
    if (rep->positions[ROM_BIN_PRICE].iov_base != NULL) {
        memcpy(ute->positions[ROM_BIN_PRICE].iov_base,
               rep->positions[ROM_BIN_PRICE].iov_base, 4);
    }
    fill_in_rom_field(ute, ROM_PRICE, rep->positions[ROM_PRICE].iov_base,
                      rep->positions[ROM_PRICE].iov_len);
    // Setting leaves
    int l_cum = 0;
    if(ute->positions[ROM_BIN_CUM].iov_base != NULL) {
        memcpy(&l_cum, ute->positions[ROM_BIN_CUM].iov_base, 4);
    }
    int leaves = l_q - l_cum;
    databuf_t *itoa_buff = get_databuf(ob->gk);
    dart_itoa(ute, leaves, ROM_LEAVES, itoa_buff);
    destroy_gk_db(ob->gk, itoa_buff);
    // End leaves
}

void dom_con(dart_order_obj * doj, void *a)
{
    async_parse_args *apa = (async_parse_args*) a;
    if (doj && apa) {
        doj->dest = apa->ds_book;
        con_interface *ci = (con_interface *) apa->ds_book;
        char *rtype = doj->positions[ROM_STATUS].iov_base;
        dart_order_obj *orig = 0;
        int destroy_orig = 0;
        int found = get_obj_by_id(ci, doj->positions[ROM_TAG].iov_base,
                                  doj->positions[ROM_TAG].iov_len, &orig);
        if (found && lock_n_check(orig)) {
            if(getplen(doj, EXEC_TIME) > 0) {
                fill_in_rom_field(orig, EXEC_TIME, getpval(doj, EXEC_TIME),
                                  getplen(doj, EXEC_TIME));
            }
            if(getplen(doj, ROM_EXDEST) > 0) {
                fill_in_rom_field(orig, ROM_EXDEST, getpval(doj, ROM_EXDEST),
                                  getplen(doj, ROM_EXDEST));
            }
           if(getplen(doj,  EXEC_ID) > 0) {
                fill_in_rom_field(orig, EXEC_ID, getpval(doj, EXEC_ID),
                                  getplen(doj, EXEC_ID));
           }
           if(getplen(doj, ROM_EX_TAG) > 0) {
                fill_in_rom_field(orig, ROM_EX_TAG, getpval(doj, ROM_EX_TAG),
                                  getplen(doj, ROM_EX_TAG));
           }
            if (rtype && strncmp(rtype, "13", 2) == 0) {
                reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                reset_rom_field(orig, ROM_STATUS, "13", 2);
                fill_in_rom_field(orig, ROM_LEAVES,
                                  orig->positions[ROM_SHARES].iov_base,
                                  orig->positions[ROM_SHARES].iov_len);
                fill_in_rom_field(orig, ROM_CLORDID,
                                  doj->positions[ROM_CLORDID].iov_base,
                                  doj->positions[ROM_CLORDID].iov_len);
                dom_con_return(orig, ci);
            } else if (rtype && strncmp(rtype, "4", 1) == 0) {
                reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                reset_rom_field(orig, ROM_STATUS, "4", 1);
                remove_from_maps(ci, doj->positions[ROM_TAG].iov_base,
                                 doj->positions[ROM_TAG].iov_len);
                ci->on_out(ci, orig);
                destroy_orig = 1;
            } else if (rtype && strncmp(rtype, "5", 1) == 0) {
                reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                reset_rom_field(orig, ROM_STATUS, "5", 1);
                if (orig->rep != NULL) {
                    dart_order_obj *rep = (dart_order_obj *) orig->rep;
                    set_rep_fields(orig, rep, ci);
                } else {
                    set_rep_fields(orig, doj, ci);
                }
                fill_in_rom_field(orig, ROM_CLORDID,
                                  doj->positions[ROM_CLORDID].iov_base,
                                  doj->positions[ROM_CLORDID].iov_len);
                ci->on_replaced(ci, orig);
            } else if (rtype && strncmp(rtype, "8", 1) == 0) {
                reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                reset_rom_field(orig, ROM_STATUS, "8", 1);
                if(getplen(doj, ROM_TEXT) > 0) {
                    fill_in_rom_field(orig, ROM_TEXT,
                                      doj->positions[ROM_TEXT].iov_base,
                                      doj->positions[ROM_TEXT].iov_len);
                }
                remove_from_maps(ci, doj->positions[ROM_TAG].iov_base,
                                 doj->positions[ROM_TAG].iov_len);
                ci->on_reject(ci, orig);
                destroy_orig = 1;
            } else if (rtype && strncmp(rtype, "6", 1) == 0) {
                reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                reset_rom_field(orig, ROM_STATUS, "6", 1);
                dom_con_return(orig, ci);
            } else if (rtype && strncmp(rtype, "20", 2) == 0) {
                reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                reset_rom_field(orig, ROM_STATUS, "20", 2);
                if(getplen(doj, ROM_TEXT) > 0) {
                    fill_in_rom_field(orig, ROM_TEXT,
                                      doj->positions[ROM_TEXT].iov_base,
                                      doj->positions[ROM_TEXT].iov_len);
                }
                ci->on_reject(ci, orig);
            } else if (rtype && doj->positions[ROM_STATUS].iov_len == 1 &&
                       strncmp(rtype, "2", 1) == 0) {
                reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                reset_rom_field(orig, ROM_STATUS, "2", 1);
                reset_fields_for_fill(orig, doj, ci);
                if (!is_spread(orig)) {
                    remove_from_maps(ci, doj->positions[ROM_TAG].iov_base,
                                     doj->positions[ROM_TAG].iov_len);
                }
                ci->on_fill(ci, orig);
                destroy_orig = 1;
            } else if (rtype && strncmp(rtype, "12", 2) == 0) {
                reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                reset_rom_field(orig, ROM_STATUS, "4", 1);
                remove_from_maps(ci, doj->positions[ROM_TAG].iov_base,
                                 doj->positions[ROM_TAG].iov_len);
                ci->on_out(ci, orig);
                destroy_orig = 1;
            } else if (rtype && strncmp(rtype, "14", 2) == 0) {
                reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                reset_rom_field(orig, ROM_STATUS, "14", 2);
                if(getplen(doj, ROM_TEXT) > 0) {
                    fill_in_rom_field(orig, ROM_TEXT,
                                      doj->positions[ROM_TEXT].iov_base,
                                      doj->positions[ROM_TEXT].iov_len);
                }
                ci->on_reject(ci, orig);
            } else if (rtype && doj->positions[ROM_STATUS].iov_len == 1
                       && strncmp(rtype, "1", 1) == 0) {
                reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                reset_rom_field(orig, ROM_STATUS, "1", 1);
                reset_fields_for_fill(orig, doj, ci);
                ci->on_fill(ci, orig);
            } else if (rtype && strncmp(rtype, "3", 1) == 0) {
                reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                reset_rom_field(orig, ROM_STATUS, "3", 1);
                dom_con_return(orig, ci);
            }
        }
        destroy_gk_obj(ci->gk, doj);
        if (orig != NULL) {
            unlock(orig);
            if (destroy_orig) {
                destroy_gk_obj(ci->gk, orig);
            }
        }
    }
}
