/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "switchboard_master.h"
#include "gatekeeper.h"
#include "jigglypuff.h"
#include "oats.h"
#include "connection_interface.h"
#include "transaction_recorder.h"
#include "debug_logger.h"
#include "dart_constants.h"

//void cancel_all_orders(con_interface * ci, dart_order_obj * doj);

static char *get_single_rom_val(const char *message, unsigned long len,
                                int *val_len, int rom_val)
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

void glob_on_connect(async_parse_args* ap, int is_connected,
                     char *log, int len)
{ 
    char *name = ap->in_args->name;
    int name_len = ap->in_args->name_len;
    send_debug_message("Connection: %s, is connected: %d ?\n", name,
                       is_connected);
    if (ap->ds_book == NULL && is_connected == 2) {
        con_interface *ci = 0;
        int found =
            get_obj_by_client_id(ap->gk, name, name_len, (void **) &ci);
        if (found <= 0) {
            ap->ds_book = init_client_con(name, name_len,
                                          1, ap->gk,
                                          ap->ass, ap->sock,
                                          ap, 16384);
            set_obj_by_client_id(ap->gk, name, name_len, ap->ds_book);
            ((con_interface *) ap->ds_book)->can_send_orders = 1;
            ((con_interface *) ap->ds_book)->ass = ap->ass;
            ((con_interface *) ap->ds_book)->on_cancel_all =
                cancel_all_orders;
            ((con_interface *) ap->ds_book)->do_oats_balance =
                ap->in_args->do_balance;
        } else {
            ci->ap = ap;
            ap->ds_book = ci;
            ci->sock = ap->sock;
            ci->ass = ap->ass;
            ci->can_send_orders = 1;
        }
    } else if (ap->ds_book != NULL) {
        con_interface *ci = (con_interface *) ap->ds_book;
        if (ci->ap == NULL) {
            ci->ap = ap;
        }
        if (contains_obj(ap->gk, name, name_len)) {
            // Do nothing
        } else {
            set_obj_by_client_id(ap->gk, name, name_len, ap->ds_book);
        }
    }
    con_interface *ci = (con_interface *) ap->ds_book;
    if (ci) {
        if (ci->ap == NULL) {
            ci->ap = ap;
        }
        ci->on_connect(ci, is_connected);
        ci->on_cancel_all = cancel_all_orders;
        ci->do_oats_balance = ap->in_args->do_balance;
        if (is_connected) {
            ci->sock = ap->read_wrapper->sock;
            add_connection(ci->ass, name,
                           name_len, (async_parse_args *) ci->ap);
        } else {
            ci->sock = -1;
            name = ap->in_args->name;
            name_len = ap->in_args->name_len;
        }
        if (is_connected == 2 && len > 0 && log != NULL) {
            int llen = 0;
            char *cancel_choice = get_single_rom_val(log, len, &llen, 3);
            if (llen > 0 && cancel_choice != NULL &&
                ((strncmp("2", cancel_choice, 1) == 0)
                 || (strncmp("1", cancel_choice, 1) == 0))) {
                ci->cancel_on_dis = 1;
            }
            llen = 0;
            cancel_choice = get_single_rom_val(log, len, &llen, 72);
            if (llen > 0 && cancel_choice != NULL ){
                ci->should_buffer = 0;
                send_debug_message("We found a sequence for: %s, length: %d, %s\n",
                        name, llen, cancel_choice);
                databuf_reset(ci->mess_to_send);
            } else {
                send_debug_message("We did not found a sequence for: %s\n",
                        name);
                ci->should_buffer = 1;
            }
        }
    }
    send_oats_update(ap, is_connected, name, name_len, ap->is_server, ci);
}

static void reject_to_rom(dart_order_obj * doj, async_parse_args * ap,
                          char *status, int status_len)
{
    set_rom_field(doj, ROM_STATUS, status, status_len);
    set_rom_field(doj, 0, ROM_ZERO_STATUS, 1);
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
        record_client_outbound(doj);
    }
}


static void new_order_init(dart_order_obj * doj, async_parse_args * ap)
{
    gatekeeper *gk = ap->gk;
    con_interface *con_int = 0;
       
    int found = get_destination(gk, doj->positions[ROM_DESTID].iov_base,
                                doj->positions[ROM_DESTID].iov_len,
                                (void **) &con_int);
    if (found && con_int->can_send_orders && !con_int->is_blocked) {
        doj->dest = con_int;
        doj->sender = ap->ds_book;
        con_interface *sender = (con_interface *) ap->ds_book;
        int allowed = 1;
        if (sender->do_oats_balance) {
            pthread_spin_lock(&sender->mutex);
            allowed = unique_no_copy_insert(sender->cl_ord_id_map,
                                            doj->positions[ROM_CLIENT_ID].
                                            iov_base,
                                            doj->positions[ROM_CLIENT_ID].
                                            iov_len, doj);
            pthread_spin_unlock(&sender->mutex);
            if (allowed == 0) {
                set_rom_field(doj, ROM_TEXT, "Order ID currently active", 25);
                reject_to_rom(doj, ap, "8", 1);
                destroy_gk_obj(ap->gk, doj);
                return;
            }
            
            char* sec_type = "E";
            if (doj->positions[ROM_SEC_TYPE].iov_len > 0) {
                sec_type = doj->positions[ROM_SEC_TYPE].iov_base;
            }
            double multiplier = ('O' == sec_type[0])? 100 : 1;
            //in order received from client ROM_EXECUTION_TAG is multiplier!!!
            if (doj->positions[ROM_EXECUTION_TAG].iov_len > 0)
            {
                multiplier = strtod((char*)(doj->positions[ROM_EXECUTION_TAG].iov_base), NULL);
            }
            doj->positions[ROM_BIN_MULT].iov_len = sizeof(double);
            set_rom_field(doj, ROM_BIN_MULT, &multiplier, sizeof(multiplier));
            
#ifdef _SPREAD_N_READY_
            set_spread_dom_tag(doj, gk);
#else
            set_dom_tag(doj, gk);
#endif
            jp_limits_checker *jlc =
                (jp_limits_checker *) get_balance_struct(ap->gk);
             
            allowed = update_balance_new(jlc, doj, 1);
            if (allowed == 0) {
                set_rom_field(doj, ROM_STATUS, "0", 1);
                send_oats_message(doj);
                reject_to_rom(doj, ap, "8", 1);
                increment_ex_tag(doj);
                send_oats_message(doj);
                pthread_spin_lock(&sender->mutex);
                remove_from_map(sender->cl_ord_id_map,
                        doj->positions[ROM_CLIENT_ID].iov_base,
                        doj->positions[ROM_CLIENT_ID].iov_len);
                pthread_spin_unlock(&sender->mutex);
                destroy_gk_obj(ap->gk, doj);
                return;
            }
        
        } else {
            pthread_spin_lock(&sender->mutex);
            allowed = unique_no_copy_insert(sender->cl_ord_id_map,
                                            doj->positions[ROM_TAG].
                                            iov_base,
                                            doj->positions[ROM_TAG].
                                            iov_len, doj);
            pthread_spin_unlock(&sender->mutex);
        }
        if (allowed == 0) {
            set_rom_field(doj, ROM_TEXT, "Order ID currently active", 25);
            reject_to_rom(doj, ap, "8", 1);
            destroy_gk_obj(ap->gk, doj);
            return;
        }
        
        set_rom_field(doj, ROM_OWNER, doj->positions[ROM_SENDER].iov_base,
                      doj->positions[ROM_SENDER].iov_len);
        set_rom_field(doj, ROM_DEST, con_int->dest_id, con_int->dest_len);
        if (sender->do_oats_balance) {
            set_rom_field(doj, ROM_STATUS, "0", 1);
            send_oats_message(doj);
        }
        con_int->on_enter_order(con_int, doj, ap);
    } else {
        doj->sender = ap->ds_book;
        databuf_t *buff = get_databuf(gk);
        
        if(found) {
            databuf_memcpy(buff, con_int->ds.dest_name, con_int->ds.dest_name_len);
            if (con_int->is_blocked)
            {
                databuf_memcpy(buff, " is Blocked", 11);
            }
            else if (!con_int->connected)
            {
                databuf_memcpy(buff, " is Not connected", 17);
            }
            else
            {
                databuf_memcpy(buff, " is Not logged on", 17);
            }
            
            set_rom_field(doj, ROM_TEXT, buff->buffer, databuf_unread(buff));
        } else {
            databuf_memcpy(buff,  " Dest not found", 15);
        }
        
        set_rom_field(doj, ROM_TEXT, buff->buffer, databuf_unread(buff));
        destroy_gk_db(gk, buff);
        
        if (((con_interface*)doj->sender)->do_oats_balance) {
            set_dom_tag(doj, gk);
            set_rom_field(doj, ROM_STATUS, "0", 1);
            send_oats_message(doj);
            increment_ex_tag(doj);
            set_rom_field(doj, ROM_STATUS, "8", 1);
            set_rom_field(doj, 0, ROM_ZERO_STATUS, 1);
            send_oats_message(doj);
        }
        reject_to_rom(doj, ap, "8", 1);
        destroy_gk_obj(ap->gk, doj);
    }
}

static int filter_cancel_all(dart_order_obj * cancel_all,
                             dart_order_obj * cur_ord)
{
    if (cancel_all == NULL) {
        return 1;
    }
    if (cancel_all->positions[ROM_SEC_TYPE].iov_len > 0 &&
        strncmp(cancel_all->positions[ROM_SEC_TYPE].iov_base,
                cur_ord->positions[ROM_SEC_TYPE].iov_base,
                cur_ord->positions[ROM_SEC_TYPE].iov_len) != 0) {
        return 0;
    }
    if (cancel_all->positions[ROM_DESTID].iov_len > 0 &&
        strncmp(cancel_all->positions[ROM_DESTID].iov_base,
                cur_ord->positions[ROM_DESTID].iov_base,
                cur_ord->positions[ROM_DESTID].iov_len) != 0) {
        return 0;
    }
    if (cancel_all->positions[ROM_TRADER].iov_len > 0 &&
        strncmp(cancel_all->positions[ROM_TRADER].iov_base,
                cur_ord->positions[ROM_TRADER].iov_base,
                cur_ord->positions[ROM_TRADER].iov_len) != 0) {
        return 0;
    }
    if (cancel_all->positions[ROM_SYM].iov_len > 0 &&
        strncmp(cancel_all->positions[ROM_SYM].iov_base,
                cur_ord->positions[ROM_SYM].iov_base,
                cur_ord->positions[ROM_SYM].iov_len) != 0) {
        return 0;
    }
    if (cancel_all->positions[ROM_SIDE].iov_len > 0 &&
        strncmp(cancel_all->positions[ROM_SIDE].iov_base,
                cur_ord->positions[ROM_SIDE].iov_base,
                cur_ord->positions[ROM_SIDE].iov_len) != 0) {
        return 0;
    }
    if (cancel_all->positions[ROM_TYPE].iov_len > 0 &&
        strncmp(cancel_all->positions[ROM_TYPE].iov_base,
                cur_ord->positions[ROM_TYPE].iov_base,
                cur_ord->positions[ROM_TYPE].iov_len) != 0) {
        return 0;
    }
    if (cancel_all->positions[ROM_PRICE].iov_len > 0 &&
        strncmp(cancel_all->positions[ROM_PRICE].iov_base,
                cur_ord->positions[ROM_PRICE].iov_base,
                cur_ord->positions[ROM_PRICE].iov_len) != 0) {
        return 0;
    }
    if (cancel_all->positions[ROM_TIF].iov_len > 0 &&
        strncmp(cancel_all->positions[ROM_TIF].iov_base,
                cur_ord->positions[ROM_TIF].iov_base,
                cur_ord->positions[ROM_TIF].iov_len) != 0) {
        return 0;
    }
    if (cancel_all->positions[ROM_CLR_ACC].iov_len > 0 &&
        strncmp(cancel_all->positions[ROM_CLR_ACC].iov_base,
                cur_ord->positions[ROM_CLR_ACC].iov_base,
                cur_ord->positions[ROM_CLR_ACC].iov_len) != 0) {
        return 0;
    }
    if (cancel_all->positions[ROM_P_C].iov_len > 0 &&
        strncmp(cancel_all->positions[ROM_P_C].iov_base,
                cur_ord->positions[ROM_P_C].iov_base,
                cur_ord->positions[ROM_P_C].iov_len) != 0) {
        return 0;
    }
    if (cancel_all->positions[ROM_STRIKE].iov_len > 0 &&
        strncmp(cancel_all->positions[ROM_STRIKE].iov_base,
                cur_ord->positions[ROM_STRIKE].iov_base,
                cur_ord->positions[ROM_STRIKE].iov_len) != 0) {
        return 0;
    }

    return 1;
}


void cancel_all_orders(con_interface * ci, dart_order_obj * doj)
{
    pthread_spin_lock(&ci->mutex);
    hashmap_iterator_t hi = hashmap_iterator_begin(ci->cl_ord_id_map);
    dart_order_obj *current = 0;
    char *key = 0;
    int key_len = 0;
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                                     (void *) &current);
    while (valid && current) {
        if (lock_n_check(current) && filter_cancel_all(doj, current)) {
            con_interface *con_int = 0;
            
            if (NULL == current->dest)
            {
                int found = get_destination(ci->gk,
                                    current->positions[ROM_DESTID].iov_base,
                                    current->positions[ROM_DESTID].iov_len,
                                    (void **) &con_int);
                if (found)
                {
                    current->dest = con_int;
                }
            }
            
            if (current->dest != NULL) {
                con_int = (con_interface *) current->dest;
                set_rom_field(current, 0, "S", 1);
                set_rom_field(current, ROM_STATUS, "26", 2);
                if(doj && current->sender && doj->sender &&
                        current->sender != doj->sender) {
                    set_rom_field(current, ROM_OWN_CNL, "1", 1);
                }
                if(current->sender && ((con_interface*)current->sender)->do_oats_balance) {
                    increment_ex_tag(current);
                    send_oats_message(current);
                }
                con_int->on_cancel_order(con_int, current);
            }
            unlock(current);
        }
        valid = hashmap_iterator_next(ci->cl_ord_id_map, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &current);
    }
    pthread_spin_unlock(&ci->mutex);
    hashmap_iterator_destroy(hi);
}

int cancel_single_order(con_interface * ci, dart_order_obj * doj)
{
    int ret = 0;
    dart_order_obj *orig = 0;
    pthread_spin_lock(&ci->mutex);
    int check_tag = ci->do_oats_balance ? ROM_CLIENT_ID : ROM_TAG;
    int found = get_map_entry(ci->cl_ord_id_map,
                              doj->positions[check_tag].iov_base,
                              doj->positions[check_tag].iov_len,
                              (void **) &orig);
    pthread_spin_unlock(&ci->mutex);
    if (found && lock_n_check(orig)) {
        if (check_vals
            (orig, doj->positions[ROM_SENDER].iov_base,
             doj->positions[ROM_SENDER].iov_len,
             doj->positions[ROM_CLIENT_ID].iov_base,
             doj->positions[ROM_CLIENT_ID].iov_len)) {
            con_interface *con_int = 0;
            if (orig->dest == 0) {
                found =
                    get_destination(ci->gk,
                                    orig->positions[ROM_DESTID].iov_base,
                                    orig->positions[ROM_DESTID].iov_len,
                                    (void **) &con_int);
            } else {
                found = 1;
                con_int = (con_interface *) orig->dest;
            }
            if (found && con_int && con_int->can_send_orders) {
                set_rom_field(orig, ROM_STATUS, "26", 2);
                set_rom_field(orig, 0, "S", 1);
                if (ci->do_oats_balance) {
                    increment_ex_tag(orig);
                    send_oats_message(orig);
                }
                con_int->on_cancel_order(con_int, orig);
                ret = 1;
            }
        }
        unlock(orig);
    }
    return ret;
}

static void cancel_order_init(dart_order_obj * doj, async_parse_args * ap)
{
    con_interface *ci = (con_interface *) ap->ds_book;
    int was_sent = cancel_single_order(ci, doj);
    if (!was_sent) {
        set_rom_field(doj, ROM_TEXT, "Too late to cancel", 18);
        reject_to_rom(doj, ap, "14", 2);
    }
    destroy_gk_obj(ci->gk, doj);
}

static void replace_order_init(dart_order_obj * doj, async_parse_args * ap)
{
    con_interface *ci = (con_interface *) ap->ds_book;
    dart_order_obj *orig = 0;
    pthread_spin_lock(&ci->mutex);
    int check_tag = ci->do_oats_balance ? ROM_CLIENT_ID : ROM_TAG;
    int found = get_map_entry(ci->cl_ord_id_map,
                              doj->positions[check_tag].iov_base,
                              doj->positions[check_tag].iov_len,
                              (void **) &orig);
    pthread_spin_unlock(&ci->mutex);
    if (found && lock_n_check(orig)) {
        if (check_vals
            (orig, doj->positions[ROM_SENDER].iov_base,
             doj->positions[ROM_SENDER].iov_len,
             doj->positions[ROM_CLIENT_ID].iov_base,
             doj->positions[ROM_CLIENT_ID].iov_len)) {
            orig->rep = doj;
            if (ci->do_oats_balance) {
                jp_limits_checker *jlc =
                    (jp_limits_checker *) get_balance_struct(ap->gk);
                int allowed = update_balance_replace(jlc, orig);
                if (allowed == 0) {
                    orig->rep = 0;
                    set_rom_field(doj, ROM_TEXT, 
                                    orig->positions[ROM_TEXT].iov_base,
                                    orig->positions[ROM_TEXT].iov_len);
                    set_rom_field(doj, ROM_LEAVES,
                                    orig->positions[ROM_LEAVES].iov_base,
                                    orig->positions[ROM_LEAVES].iov_len);
                    set_rom_field(doj, ROM_CUM,
                                    orig->positions[ROM_CUM].iov_base,
                                    orig->positions[ROM_CUM].iov_len);
                    
                    set_rom_field(doj, ROM_STATUS, "20", 2);
                    increment_ex_tag(orig);
                    fill_in_rom_field(doj, ROM_EXECUTION_TAG,
                              orig->positions[ROM_EXECUTION_TAG].iov_base,
                              orig->positions[ROM_EXECUTION_TAG].iov_len);
                    fill_in_rom_field(doj, ROM_TAG,
                              orig->positions[ROM_TAG].iov_base,
                              orig->positions[ROM_TAG].iov_len);
                    
                    send_oats_message(doj);
                    
                    reject_to_rom(doj, ap, "20", 2);
                                        
                    destroy_gk_obj(ap->gk, doj);
                    unlock(orig);
                    return;
                }
            }
            con_interface *con_int = 0;
            if (orig->dest == 0) {
                found =
                    get_destination(ci->gk,
                                    orig->positions[ROM_DESTID].iov_base,
                                    orig->positions[ROM_DESTID].iov_len,
                                    (void **) &con_int);
            } else {
                found = 1;
                con_int = (con_interface *) orig->dest;
            }
            int pending_new = 0;
            int status_len = getplen(orig, ROM_STATUS);
            if(status_len > 0) {
                char* status = getpval(orig, ROM_STATUS);
                if(status[0] == '0') {
                    pending_new = 1;
                }
            } else if(status_len <= 0) {
                    pending_new = 1;
            }
            if (found && con_int->can_send_orders && !con_int->is_blocked && (pending_new == 0)) {
                orig->rep = doj;
                fill_in_rom_field(doj, ROM_TAG,
                              orig->positions[ROM_TAG].iov_base,
                              orig->positions[ROM_TAG].iov_len);
                fill_in_rom_field(doj, ROM_DESTID,
                              orig->positions[ROM_DESTID].iov_base,
                              orig->positions[ROM_DESTID].iov_len);
                fill_in_rom_field(doj, ROM_DEST,
                              orig->positions[ROM_DEST].iov_base,
                              orig->positions[ROM_DEST].iov_len);
                if(getplen(doj, ROM_NEW_ID) > 0) {
                    fill_in_rom_field(orig, ROM_NEW_ID, getpval(doj, ROM_NEW_ID), getplen(doj, ROM_NEW_ID));
                } else if(getplen(orig, ROM_NEW_ID) <= 0) {
                    set_rom_field(orig, ROM_NEW_ID, getpval(doj, ROM_CLIENT_ID), getplen(doj, ROM_CLIENT_ID));
                }
                if (ci->do_oats_balance) {
                    increment_ex_tag(orig);
                    fill_in_rom_field(doj, ROM_EXECUTION_TAG,
                              orig->positions[ROM_EXECUTION_TAG].iov_base,
                              orig->positions[ROM_EXECUTION_TAG].iov_len);
                    set_rom_field(doj, ROM_STATUS, "27", 2);
                    int tiff_len = getplen(doj,ROM_TIF);
                    if(tiff_len <= 0) {
                        int o_tiff_len = getplen(orig, ROM_TIF);
                        if(o_tiff_len > 0) {
                        fill_in_rom_field(doj, ROM_TIF,
                                  orig->positions[ROM_TIF].iov_base,
                                  orig->positions[ROM_TIF].iov_len);
                        }
                    }
                    int type_len = getplen(doj,ROM_TYPE);
                    if(type_len <= 0) {
                        int o_type_len = getplen(orig, ROM_TYPE);
                        if(o_type_len > 0) {
                        fill_in_rom_field(doj, ROM_TYPE,
                                  orig->positions[ROM_TYPE].iov_base,
                                  orig->positions[ROM_TYPE].iov_len);
                        }
                    }
                    send_oats_message(doj);
                }
                con_int->on_replace_order(con_int, orig);
            } else {
                orig->rep = 0;
                if(pending_new) {
                set_rom_field(doj, ROM_TEXT, "Replace before open", 19);
                } else {
                set_rom_field(doj, ROM_TEXT, "Not connected", 13);
                }
                set_rom_field(doj, ROM_TAG, getpval(orig, ROM_TAG), getplen(orig, ROM_TAG));
                
                if (ci->do_oats_balance) 
                {
                    set_rom_field(doj, ROM_STATUS, "20", 2);
                    increment_ex_tag(orig);
                    fill_in_rom_field(doj, ROM_EXECUTION_TAG,
                              orig->positions[ROM_EXECUTION_TAG].iov_base,
                              orig->positions[ROM_EXECUTION_TAG].iov_len);
                    fill_in_rom_field(doj, ROM_TAG,
                              orig->positions[ROM_TAG].iov_base,
                              orig->positions[ROM_TAG].iov_len);
                    send_oats_message(doj);
                }
                
                reject_to_rom(doj, ap, "20", 2);
                destroy_gk_obj(ap->gk, doj);
            }
        }
        unlock(orig);
    } else {
        set_rom_field(doj, ROM_TEXT, "Too late to replace", 19);
        reject_to_rom(doj, ap, "20", 2);
        destroy_gk_obj(ap->gk, doj);
    }
}

void on_reject(con_interface * ci, dart_order_obj * doj)
{
    con_interface *con_int = (con_interface *) doj->sender;
    char *status = doj->positions[ROM_STATUS].iov_base;
    if (status && status[0] == '8') {
        if (con_int->do_oats_balance) {
            jp_limits_checker *jlc =
                (jp_limits_checker *) get_balance_struct(con_int->gk);
#ifdef _SPREAD_N_READY_
        char* cmplx = getpval(doj, ROM_CPLX_TYPE);
        int cmplx_len = getplen(doj, ROM_CPLX_TYPE);
        if(cmplx_len > 0 && (cmplx[0] == '6')) {
            //do nothing
        } else {
            update_balance(jlc, doj);
        } 
#else
            update_balance(jlc, doj);
#endif
        }
        pthread_spin_lock(&con_int->mutex);
        int check_tag = con_int->do_oats_balance ? ROM_CLIENT_ID : ROM_TAG;
        remove_from_map(con_int->cl_ord_id_map,
                        doj->positions[check_tag].iov_base,
                        doj->positions[check_tag].iov_len);
        pthread_spin_unlock(&con_int->mutex);
    } else if (status && strncmp(status, "20", 2) == 0 && con_int->do_oats_balance) {
        jp_limits_checker *jlc =
            (jp_limits_checker *) get_balance_struct(con_int->gk);
#ifdef _SPREAD_N_READY_
        char* cmplx = getpval(doj, ROM_CPLX_TYPE);
        int cmplx_len = getplen(doj, ROM_CPLX_TYPE);
        if(cmplx_len > 0 && (cmplx[0] == '6')) {
            //do nothing
        } else {
            update_balance_rep_rej(jlc, doj);
        }
#else
        update_balance_rep_rej(jlc, doj);
#endif
    }
    con_int->ci_iovec_sender(doj->positions, DART_SEND_LEN,
                             doj->positions[ROM_SENDER].iov_base,
                             doj->positions[ROM_SENDER].iov_len, con_int,
                             con_int->sock);
    if (con_int->do_oats_balance) {
        increment_ex_tag(doj);
        send_oats_message(doj);
    }
    record_client_outbound(doj);
}

void on_out(con_interface * ci, dart_order_obj * doj)
{
    con_interface *con_int = (con_interface *) doj->sender;
    if(con_int) {
    if (con_int->do_oats_balance) {
        increment_ex_tag(doj);
        jp_limits_checker *jlc =
            (jp_limits_checker *) get_balance_struct(con_int->gk);
#ifdef _SPREAD_N_READY_
        char* cmplx = getpval(doj, ROM_CPLX_TYPE);
        int cmplx_len = getplen(doj, ROM_CPLX_TYPE);
        if(cmplx_len > 0 && (cmplx[0] == '6')) {
            //do nothing
        } else {
            update_balance(jlc, doj);
        }
#else
        update_balance(jlc, doj);
#endif
    }
    pthread_spin_lock(&con_int->mutex);
    int check_tag = con_int->do_oats_balance ? ROM_CLIENT_ID : ROM_TAG;
    remove_from_map(con_int->cl_ord_id_map,
                    doj->positions[check_tag].iov_base,
                    doj->positions[check_tag].iov_len);
    pthread_spin_unlock(&con_int->mutex);
    con_int->ci_iovec_sender(doj->positions, DART_SEND_LEN,
                             doj->positions[ROM_SENDER].iov_base,
                             doj->positions[ROM_SENDER].iov_len, con_int,
                             con_int->sock);
    if (con_int->do_oats_balance) {
        send_oats_message(doj);
    }
    } else {
        char* rom_tag = getpval(doj, ROM_TAG);
        int s_len = doj->positions[ROM_SENDER].iov_len;
        send_debug_message("Sender WAS null on_out: %s in the message: %d \n", rom_tag, s_len);
    }
    record_client_outbound(doj);
}

void on_replaced(con_interface * ci, dart_order_obj * doj)
{
    con_interface *con_int = (con_interface *) doj->sender;
    if(con_int) { 
        if (con_int->do_oats_balance) {   //update client tag
            pthread_spin_lock(&con_int->mutex);

            remove_from_map(con_int->cl_ord_id_map, 
                            doj->positions[ROM_CLIENT_ID].iov_base, 
                            doj->positions[ROM_CLIENT_ID].iov_len);
            unique_no_copy_insert(con_int->cl_ord_id_map,
                                doj->positions[ROM_NEW_ID].iov_base,
                                doj->positions[ROM_NEW_ID].iov_len, doj);

            pthread_spin_unlock(&con_int->mutex);
        }
        
        fill_in_rom_field(doj, ROM_CLIENT_ID, doj->positions[ROM_NEW_ID].iov_base,
                          doj->positions[ROM_NEW_ID].iov_len);
        con_int->ci_iovec_sender(doj->positions, DART_SEND_LEN,
                                 doj->positions[ROM_SENDER].iov_base,
                                 doj->positions[ROM_SENDER].iov_len, con_int,
                                 con_int->sock);
        if (con_int->do_oats_balance) {
            increment_ex_tag(doj);
            send_oats_message(doj);
        }
    }
    record_client_outbound(doj);
}

void on_fill(con_interface * ci, dart_order_obj * doj)
{
    con_interface *con_int = (con_interface *) doj->sender;
    char *status = doj->positions[ROM_STATUS].iov_base;
    if (con_int->do_oats_balance) {
        jp_limits_checker *jlc =
            (jp_limits_checker *) get_balance_struct(con_int->gk);
        increment_ex_tag(doj);
#ifdef _SPREAD_N_READY_
        char* cmplx = getpval(doj, ROM_CPLX_TYPE);
        int cmplx_len = getplen(doj, ROM_CPLX_TYPE);
        if(cmplx_len > 0 && (cmplx[0] == '6')) {
            //do nothing
        } else {
            update_balance_fill(jlc, doj);
        }
#else
        update_balance_fill(jlc, doj);
#endif
    }
    if (status && status[0] == '2') {
        pthread_spin_lock(&con_int->mutex);
        int check_tag = con_int->do_oats_balance ? ROM_CLIENT_ID : ROM_TAG;
        remove_from_map(con_int->cl_ord_id_map,
                        doj->positions[check_tag].iov_base,
                        doj->positions[check_tag].iov_len);
        pthread_spin_unlock(&con_int->mutex);
    }
    con_int->ci_iovec_sender(doj->positions, DART_SEND_LEN,
                             doj->positions[ROM_SENDER].iov_base,
                             doj->positions[ROM_SENDER].iov_len, con_int,
                             con_int->sock);
    if (con_int->do_oats_balance) {
        send_oats_message(doj);
    }
    record_client_outbound(doj);
}

void return_to_sender(dart_order_obj * doj, void *a)
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
        if (((con_interface *) doj->sender)->do_oats_balance) {
            increment_ex_tag(doj);
            send_oats_message(doj);
        }
        record_client_outbound(doj);

    } else {
        increment_ex_tag(doj);
        send_oats_message(doj);
    }
}

void switchboard(dart_order_obj * doj, void *a)
{
    async_parse_args *ap = (async_parse_args *) a;
    if (doj && ap) {
        doj->sender = ap->ds_book;
        char *rtype = doj->positions[0].iov_base;
        switch (rtype[0]) {
        case 'E':
            new_order_init(doj, ap);
            break;
        case 'C':
            cancel_order_init(doj, ap);
            break;
        case 'c':
            if(ap->ds_book != 0x0) {
                cancel_all_orders((con_interface *) ap->ds_book, doj);
            }
            break;
        case 'R':
            replace_order_init(doj, ap);
            break;
        case 'H':
            destroy_gk_obj(ap->gk, doj);
            break;
        case 'P':
            {
                send_debug_message("P message received\n");
                break;
            }
        default:
            {
                long addr = (long)doj;
                send_debug_message("Unknown order Address: %d\n", addr);
                destroy_gk_obj(ap->gk, doj);
            }
            break;
        }
    }
}
