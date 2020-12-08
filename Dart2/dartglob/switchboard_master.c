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
#include "oats.h"
#include "connection_interface.h"
#include "balance_checker.h"
#include "dart_handler.h"
#include "rex_constants.h"


void glob_log_to_oats(con_int* ab, int is_connected)
{ 
    if(ab->mod_name_len > 0) {
        send_debug_message(ab->o, "Connection: %s, is connected: %d ?\n", ab->mod_name,
                           is_connected);
        set_obj_by_client_id(ab->gk, ab->mod_name, ab->mod_name_len, ab);
    }
    con_int* oats = get_compliance_con(ab->gk);
    if(oats) {
        send_oats_update(oats, is_connected, ab->mod_name, ab->mod_name_len, 1, ab);
    }
}
void glob_log_to_oats_dest(con_int* ab, int is_connected)
{
    if(ab->mod_name_len > 0) {
        send_debug_message(ab->o, "Connection: %s,%s is connected: %d ?\n", ab->mod_name,
                           ab->ex->dest_id, is_connected);
        set_obj_by_client_id(ab->gk, ab->ex->dest_id, ab->ex->dest_id_len, ab);
    }
    con_int* oats = get_compliance_con(ab->gk);
    if(oats) {
        send_oats_update(oats, is_connected, ab->ex->dest_id, ab->ex->dest_id_len, 0, ab);
    }
}
static void reject_to_rom(dart_order_obj * doj, con_int * ap,
                          char *status, int status_len)
{
    reset_rom_field(doj, ROM_STATUS, status, status_len);
    reset_rom_field(doj, 0, ROM_ZERO_STATUS, 1);
    if (doj->sender != NULL) {
        ((con_int*)doj->sender)->send_to_con(doj->sender, &doj->risk);
        record_dart_message(ap->o, doj);
    }
}

static void new_order_init(con_int* ap, dart_order_obj * doj)
{
    gatekeeper *gk = ap->gk;
    con_int*ci = 0;
    int found = get_destination(gk, doj->positions[ROM_DESTID].iov_base,
                                doj->positions[ROM_DESTID].iov_len,
                                (void **) &ci);
    if (found && ci->can_send_orders && !ci->is_blocked) {
        doj->dest = ci;
        doj->sender = ap;
        int allowed = 1;
        if (ap->logon->do_balance) {
            pthread_spin_lock(&ap->mutex);
            allowed = unique_no_copy_insert(ap->cl_ord_id_map,
                                            doj->positions[ROM_CLIENT_ID].
                                            iov_base,
                                            doj->positions[ROM_CLIENT_ID].
                                            iov_len, doj);
            pthread_spin_unlock(&ap->mutex);
            if (allowed == 0) {
                set_rom_field(doj, ROM_TEXT, "Order ID currently active", 25);
                reject_to_rom(doj, ap, "8", 1);
                reset_doj(((dartfact*)doj->dalloc), doj);
                return;
            }
#ifdef _SPREAD_N_READY_
            set_spread_dom_tag(doj, gk);
#else
            set_dom_tag(doj, gk);
#endif
            /*jp_limits_checker *jlc =
                (jp_limits_checker *) get_balance_struct(ap->gk);
            TODO allowed = update_balance_new(jlc, doj);*/
            if (allowed == 0) {
                con_int* oat = get_compliance_con(ap->gk);
                if(oat) {
                    send_oats_message(oat, &doj->risk);
                }
                reject_to_rom(doj, ap, "8", 1);
                increment_ex_tag(doj);
                if(oat) {
                    send_oats_message(oat, &doj->risk);
                }
                pthread_spin_lock(&ap->mutex);
                remove_from_map(ap->cl_ord_id_map,
                        doj->positions[ROM_CLIENT_ID].iov_base,
                        doj->positions[ROM_CLIENT_ID].iov_len);
                pthread_spin_unlock(&ap->mutex);
                reset_doj(((dartfact*)doj->dalloc), doj);
                return;
            }
        
        } else {
            pthread_spin_lock(&ap->mutex);
            allowed = unique_no_copy_insert(ap->cl_ord_id_map,
                                            doj->positions[ROM_TAG].
                                            iov_base,
                                            doj->positions[ROM_TAG].
                                            iov_len, doj);
            pthread_spin_unlock(&ap->mutex);
        }
        if (allowed == 0) {
            set_rom_field(doj, ROM_TEXT, "Order ID currently active", 25);
            reject_to_rom(doj, ap, "8", 1);
            reset_doj(((dartfact*)doj->dalloc), doj);
            return;
        }
        
        set_rom_field(doj, ROM_OWNER, doj->positions[ROM_SENDER].iov_base,
                      doj->positions[ROM_SENDER].iov_len);
        set_rom_field(doj, ROM_DEST, ci->ex->dest_id, ci->ex->dest_id_len);
        if (ap->logon->do_balance) {
            con_int* oat = get_compliance_con(ap->gk);
            if(oat) {
                send_oats_message(oat, &doj->risk);
            }
        }
        ci->on_enter_order(ci, &doj->risk);
    } else {
        if(found) {
            set_rom_field(doj, ROM_TEXT, "Blocked", 7);
        } else {
            set_rom_field(doj, ROM_TEXT, "Not connected", 13);
        }
        if (ap->logon->do_balance) {
            con_int* oat = get_compliance_con(ap->gk);
            set_dom_tag(doj, ap->gk);
            if(oat) {
                send_oats_message(oat, &doj->risk);
            }
            increment_ex_tag(doj);
            reset_rom_field(doj, ROM_STATUS, "8", 1);
            reset_rom_field(doj, 0, ROM_ZERO_STATUS, 1);
            if(oat) {
                send_oats_message(oat, &doj->risk);
            }
        }
        reject_to_rom(doj, ap, "8", 1);
        reset_doj(((dartfact*)doj->dalloc), doj);
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


void cancel_all_orders(con_int* ci, rcntr * risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
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
            con_int* ct = 0;
            if (current->dest != NULL) {
                ct = (con_int*) current->dest;
                reset_rom_field(current, 0, "S", 1);
                reset_rom_field(current, ROM_STATUS, "26", 2);
                if(doj && current->sender && doj->sender &&
                        current->sender != doj->sender) {
                    reset_rom_field(current, ROM_OWN_CNL, "1", 1);
                }
                if(current->sender && ((con_int*)current->sender)->logon->do_balance) {
                    increment_ex_tag(current);
                    con_int* oat = get_compliance_con(ci->gk);
                    if(oat) {
                        send_oats_message(oat, &current->risk);
                    }
                }
                ct->on_cancel_order(ct, &current->risk);
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

int cancel_single_order(con_int* ci, rcntr * risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    int ret = 0;
    dart_order_obj *orig = 0;
    pthread_spin_lock(&ci->mutex);
    int check_tag = ci->logon->do_balance ? ROM_CLIENT_ID : ROM_TAG;
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
            con_int* ct = 0;
            if (orig->dest == 0) {
                found =
                    get_destination(ci->gk,
                                    orig->positions[ROM_DESTID].iov_base,
                                    orig->positions[ROM_DESTID].iov_len,
                                    (void **) &ct);
            } else {
                found = 1;
                ct = (con_int*) orig->dest;
            }
            if (found && ct && ct->can_send_orders) {
                reset_rom_field(orig, ROM_STATUS, "26", 2);
                reset_rom_field(orig, 0, "S", 1);
                if (ci->logon->do_balance) {
                    increment_ex_tag(orig);
                    con_int* oat = get_compliance_con(ci->gk);
                    if(oat) {
                        send_oats_message(oat, &orig->risk);
                    }
                }
                ct->on_cancel_order(ct, &orig->risk);
                ret = 1;
            }
        }
        unlock(orig);
    }
    return ret;
}

static int cancel_order_init(con_int* ci, dart_order_obj * doj)
{
    int was_sent = cancel_single_order(ci, &doj->risk);
    if (!was_sent) {
        set_rom_field(doj, ROM_TEXT, "Too late to cancel", 18);
        reject_to_rom(doj, ci, "14", 2);
    }
    reset_doj(((dartfact*)doj->dalloc), doj);
    return was_sent; 
}

static int replace_order_init(con_int* ci, dart_order_obj * doj)
{
    dart_order_obj *orig = 0;
    pthread_spin_lock(&ci->mutex);
    int check_tag = ci->logon->do_balance ? ROM_CLIENT_ID : ROM_TAG;
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
            if (ci->logon->do_balance) {
                /* TODO jp_limits_checker *jlc =
                    (jp_limits_checker *) get_balance_struct(ci->gk);*/
                int allowed = 1; /*TODO update_balance_replace(jlc, orig);*/
                if (allowed == 0) {
                    orig->rep = 0;
                    reset_rom_field(doj, ROM_LEAVES,
                                    orig->positions[ROM_LEAVES].iov_base,
                                    orig->positions[ROM_LEAVES].iov_len);
                    reset_rom_field(doj, ROM_CUM,
                                    orig->positions[ROM_CUM].iov_base,
                                    orig->positions[ROM_CUM].iov_len);
                    reject_to_rom(doj, ci, "20", 2);
                    reset_doj(((dartfact*)doj->dalloc), doj);
                    unlock(orig);
                    return allowed;
                }
            }
            con_int* ct = 0;
            if (orig->dest == 0) {
                found =
                    get_destination(ci->gk,
                                    orig->positions[ROM_DESTID].iov_base,
                                    orig->positions[ROM_DESTID].iov_len,
                                    (void **) &ct);
            } else {
                found = 1;
                ct = (con_int*) orig->dest;
            }
            if (found && ct->can_send_orders && !ct->is_blocked) {
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
                    fill_in_rom_field(orig, ROM_NEW_ID,
                                      getpval(doj, ROM_NEW_ID),
                                      getplen(doj, ROM_NEW_ID));
                } else if(getplen(orig, ROM_NEW_ID) <= 0) {
                    set_rom_field(orig, ROM_NEW_ID,
                                  getpval(doj, ROM_CLIENT_ID),
                                  getplen(doj, ROM_CLIENT_ID));
                }
                if (ci->logon->do_balance) {
                    increment_ex_tag(orig);
                    fill_in_rom_field(doj, ROM_EXECUTION_TAG,
                              orig->positions[ROM_EXECUTION_TAG].iov_base,
                              orig->positions[ROM_EXECUTION_TAG].iov_len);
                    con_int* oat = get_compliance_con(ci->gk);
                    if(oat) {
                        send_oats_message(oat, &doj->risk);
                    }
                }
                ct->on_replace_order(ct, &orig->risk);
            } else {
                orig->rep = 0;
                set_rom_field(doj, ROM_TEXT, "Not connected", 13);
                set_rom_field(doj, ROM_TAG, getpval(orig, ROM_TAG),
                              getplen(orig, ROM_TAG));
                reject_to_rom(doj, ci, "20", 2);
                reset_doj(((dartfact*)doj->dalloc), doj);
            }
        }
        unlock(orig);
    } else {
        set_rom_field(doj, ROM_TEXT, "Too late to replace", 19);
        reject_to_rom(doj, ci, "20", 2);
        reset_doj(((dartfact*)doj->dalloc), doj);
    }
    return 1;
}

void on_reject(con_int* ci, rcntr * risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    con_int* ct = (con_int*) doj->sender;
    char *status = doj->positions[ROM_STATUS].iov_base;
    if (status && status[0] == '8') {
        if (ct->logon->do_balance) {
            /*TODO jp_limits_checker *jlc =
                (jp_limits_checker *) get_balance_struct(ct->gk);*/
#ifdef _SPREAD_N_READY_
        char* cmplx = getpval(doj, ROM_CPLX_TYPE);
        int cmplx_len = getplen(doj, ROM_CPLX_TYPE);
        if(cmplx_len > 0 && (cmplx[0] == '6')) {
            //do nothing;
        } else {
            /*TODO update_balance(jlc, doj);*/
        }
#else
            /* TODO update_balance(jlc, doj);*/
#endif
        }
        pthread_spin_lock(&ct->mutex);
        int check_tag = ct->logon->do_balance ? ROM_CLIENT_ID : ROM_TAG;
        remove_from_map(ct->cl_ord_id_map,
                        doj->positions[check_tag].iov_base,
                        doj->positions[check_tag].iov_len);
        pthread_spin_unlock(&ct->mutex);
    } else if (status && strncmp(status, "20", 2) == 0 &&
               ct->logon->do_balance) {
        /* TODO jp_limits_checker *jlc =
            (jp_limits_checker *) get_balance_struct(ct->gk);*/
#ifdef _SPREAD_N_READY_
        char* cmplx = getpval(doj, ROM_CPLX_TYPE);
        int cmplx_len = getplen(doj, ROM_CPLX_TYPE);
        if(cmplx_len > 0 && (cmplx[0] == '6')) {
            //do nothing;
        } else {
            /* TODO update_balance_rep_rej(jlc, doj);*/
        }
#else
        /* TODO update_balance_rep_rej(jlc, doj);*/
#endif
    }
    ct->send_to_con(ct, &doj->risk);
    if (ct->logon->do_balance) {
        con_int* oat = get_compliance_con(ci->gk);
        increment_ex_tag(doj);
        if(oat) {
            send_oats_message(oat, &doj->risk);
        }
    }
    record_dart_message(ct->o, doj);
}

void on_out(con_int* ci, rcntr * risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    con_int* ct = (con_int*) doj->sender;
    if (ct->logon->do_balance) {
        /* TODO jp_limits_checker *jlc =
            (jp_limits_checker *) get_balance_struct(ct->gk);*/
#ifdef _SPREAD_N_READY_
        char* cmplx = getpval(doj, ROM_CPLX_TYPE);
        int cmplx_len = getplen(doj, ROM_CPLX_TYPE);
        if(cmplx_len > 0 && (cmplx[0] == '6')) {
            //do nothing;
        } else {
            /* TODO update_balance(jlc, doj);*/
        }
#else
        /* TODO update_balance(jlc, doj);*/
#endif
        increment_ex_tag(doj);
    }
    pthread_spin_lock(&ct->mutex);
    int check_tag = ct->logon->do_balance ? ROM_CLIENT_ID : ROM_TAG;
    remove_from_map(ct->cl_ord_id_map,
                    doj->positions[check_tag].iov_base,
                    doj->positions[check_tag].iov_len);
    pthread_spin_unlock(&ct->mutex);
    ct->send_to_con(ct, &doj->risk);
    if (ct->logon->do_balance) {
        con_int* oat = get_compliance_con(ci->gk);
        if(oat) {
            send_oats_message(oat, &doj->risk);
        }
    }
    record_dart_message(ct->o, doj);
}

void on_replaced(con_int* ci, rcntr * risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    con_int* ct = (con_int*) doj->sender;
    ct->send_to_con(ct, &doj->risk);
    fill_in_rom_field(doj, ROM_CLIENT_ID, getpval(doj, ROM_NEW_ID), getplen(doj, ROM_NEW_ID));
    if (ct->logon->do_balance) {
        increment_ex_tag(doj);
        con_int* oat = get_compliance_con(ci->gk);
        if(oat) {
            send_oats_message(oat, &doj->risk);
        }
    }
    record_dart_message(ci->o, doj);
}

void on_fill(con_int* ci, rcntr * risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    con_int* ct = (con_int*) doj->sender;
    char *status = doj->positions[ROM_STATUS].iov_base;
    if (ct->logon->do_balance) {
        /* TODO jp_limits_checker *jlc =
            (jp_limits_checker *) get_balance_struct(ct->gk);*/
        increment_ex_tag(doj);
#ifdef _SPREAD_N_READY_
        char* cmplx = getpval(doj, ROM_CPLX_TYPE);
        int cmplx_len = getplen(doj, ROM_CPLX_TYPE);
        if(cmplx_len > 0 && (cmplx[0] == '6')) {
            //do nothing;
        } else {
            /* TODO update_balance_fill(jlc, doj);*/
        }
#else
        /* TODO update_balance_fill(jlc, doj);*/
#endif
    }
    if (status && status[0] == '2') {
        pthread_spin_lock(&ct->mutex);
        int check_tag = ct->logon->do_balance ? ROM_CLIENT_ID : ROM_TAG;
        remove_from_map(ct->cl_ord_id_map,
                        doj->positions[check_tag].iov_base,
                        doj->positions[check_tag].iov_len);
        pthread_spin_unlock(&ct->mutex);
    }
    ct->send_to_con(ct, &doj->risk);
    if (ct->logon->do_balance) {
        con_int* oat = get_compliance_con(ci->gk);
        if(oat) {
            send_oats_message(oat, &doj->risk);
        }
    }
    record_dart_message(ci->o, doj);
}

void return_to_sender(con_int* ci, rcntr * risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    if (doj->sender != NULL) {
        con_int* sender = (con_int*)doj->sender;
        sender->send_to_con(sender, &doj->risk);
        if (sender->logon->do_balance) {
            con_int* oat = get_compliance_con(ci->gk);
            increment_ex_tag(doj);
            if(oat) {
                send_oats_message(oat, &doj->risk);
            }
        }
        record_dart_message(ci->o, doj);

    } else {
        con_int* oat = get_compliance_con(ci->gk);
        if(oat) {
            send_oats_message(oat, &doj->risk);
        }
    }
}

void switchboard(void* a, dart_order_obj * doj)
{
    con_int *ap = (con_int *) a;
    if (doj && ap) {
        doj->sender = ap;
        char *rtype = doj->positions[0].iov_base;
        switch (rtype[0]) {
        case 'E':
            new_order_init(ap, doj);
            break;
        case 'C':
            cancel_order_init(ap, doj);
            break;
        case 'c':
            cancel_all_orders(ap, &doj->risk);
            break;
        case 'R':
            replace_order_init(ap, doj);
            break;
        default:
            {
                long addr = (long)doj;
                send_debug_message(ap->o, "Unknown order Address: %d\n", addr);
            }
            break;
        }
    }
}
