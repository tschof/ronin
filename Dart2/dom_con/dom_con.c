
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
#include "rex_constants.h"
#include "config_loader.h"
#include "ex_config_helper.h"
#include "dart_handler.h"
#include "balance_checker.h"
#include "connection_interface.h"
#include "connection_factory.h"
#include "rom_fix_trans_funcs.h"
#include "mailer.h"
#include "oats.h"
#include "dart_order_fact.h"

#define MICRO_HOUR 3600000000
#define MICRO_MIN 60000000
#define MICRO_SEC 1000000

static uint32_t trans_shares(void* share,int slen)
{
    char nstr[32];
    memset(nstr, '\0', 32);
    memcpy(nstr,share, slen); 
    return atoi(nstr);
}
static int is_cmplx(dart_order_obj* doj)
{
    return doj->positions[ROM_CPLX_TYPE].iov_len > 0;
}
void ex_dom_con(void *a, dart_order_obj * doj);
void dc_switchboard(void* a, dart_order_obj * doj);


void dc_send(con_int* ci, rcntr* risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    if(ci->cparser != 0x0) {
        send_dart_message((dartp*)ci->cparser, doj);
    }
}
static void dc_remove_from_maps(con_int * ci, rcntr* cnt)
{
    dart_order_obj* doj = (dart_order_obj*)cnt->raw;
    int check_tag = ci->logon->do_balance ? ROM_CLIENT_ID : ROM_TAG;
    pthread_spin_lock(&ci->mutex);
    remove_from_map(ci->cl_ord_id_map, doj->positions[check_tag].iov_base,
                              doj->positions[check_tag].iov_len);
    pthread_spin_unlock(&ci->mutex);
    {
        char clordid [32];
        memset(&clordid, '\0', 32);
        memcpy(&clordid, doj->positions[check_tag].iov_base, 
                doj->positions[check_tag].iov_len);
    }
    if(!is_cmplx(doj)) {
      dp_return_doj((dartp*)ci->cparser, doj);
      return_cnt(ci->gk, cnt);
    }
}

void send_reject(con_int* ci, dart_order_obj* rej_mess,
        char* error, int err_len, char* status, int slen)
{
    set_rom_field(rej_mess, ROM_STATUS, status, slen);
    set_rom_field(rej_mess, 0, "S", 1);
    set_rom_field(rej_mess, ROM_TEXT, error, err_len);
    send_dart_message((dartp*)ci->cparser, rej_mess);
}

int dc_enter_order(con_int * ci, rcntr* cnt)
{
    int ret = 1;
    con_int* dest = 0;
    dart_order_obj* doj = (dart_order_obj*)cnt->raw;
    get_destination(ci->gk, doj->positions[ROM_DESTID].iov_base, 
            doj->positions[ROM_DESTID].iov_len, (void **) &dest);
    if(ci->logon && ci->logon->do_balance) {
        set_dom_tag(doj, ci->gk);
        set_ex_tag(doj, 0);
    }
    if (dest && !dest->is_blocked && dest->connected) {
        set_rom_field(doj, ROM_DEST, dest->ex->dest_id,
                dest->ex->dest_id_len);
        //set_rom_field(doj, ROM_DESTID, dest->ex->dest_id,
         //       dest->ex->dest_id_len);
        int allowed = 1;
        jp_limits_checker *jlc =
            (jp_limits_checker *) get_balance_struct(ci->gk);
        if(ci->logon && ci->logon->do_balance) {
            allowed = update_balance_new(jlc, cnt);
        }
        if(allowed) {
            int check_tag = ci->logon->do_balance ? ROM_CLIENT_ID : ROM_TAG;
            pthread_spin_lock(&ci->mutex);
            allowed = unique_no_copy_insert(ci->cl_ord_id_map,
                              doj->positions[check_tag].iov_base,
                              doj->positions[check_tag].iov_len, cnt);
            pthread_spin_unlock(&ci->mutex);
            if(allowed) {
                doj->dest = dest;
                ret = dest->on_enter_order(dest, cnt);
                if(ret < 0) {
                    set_rom_field(doj, ROM_STATUS, "8", 1);
                    if(ci->logon && ci->logon->do_balance) {
                        update_balance(jlc, cnt);
                    }
                }
            } else {
                ret =  -1;
            }
        } else {
            ret = -1;
        }
    } else {
        char* err_str = 0;
        int err_len = 0;
        if(dest && dest->is_blocked) {
            err_str = "Blocked";
            err_len = 7;
        } else if(dest && !dest->connected) {
            err_str = "Not Connected";
            err_len = 13;
        } else {
            err_str = "Invalid Dest";
            err_len = 12;
        }
        memcpy(cnt->errStr, err_str, err_len);
        cnt->err_len = err_len;
        ret =  -1;
    }
    return ret;
}
int dc_cancel_order(con_int * ci, rcntr * risk)
{
    con_int* dest = 0;
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    if(doj->dest) {
        dest = doj->dest;
    } else {
        get_destination(ci->gk, doj->positions[ROM_DESTID].iov_base, 
                doj->positions[ROM_DESTID].iov_len, (void **) &dest);
    }
    if(dest && dest->connected) {
        set_rom_field(doj, 0, "S", 1);
        set_rom_field(doj, ROM_STATUS, "26", 2);
        if(ci->logon && ci->logon->do_balance) {
            increment_ex_tag(doj);
        }
        return dest->on_cancel_order(dest, risk);
    } else {
        return -1;
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
void cancel_all_orders(con_int* ci, dart_order_obj* doj)
{
    pthread_spin_lock(&ci->mutex);
    hashmap_iterator_t hi = hashmap_iterator_begin(ci->cl_ord_id_map);
    rcntr *current = 0;
    char *key = 0;
    int key_len = 0;
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                                     (void *) &current);
    while (valid && current) {
        int is_doj = current->rlen == sizeof(dart_order_obj); 
        dart_order_obj* cur_doj = (dart_order_obj*)current->raw;
        if (is_doj && filter_cancel_all(doj, cur_doj)) {
            con_int* ct = 0;
            if (cur_doj->dest != NULL) {
                ct = (con_int*) cur_doj->dest;
                set_rom_field(cur_doj, 0, "S", 1);
                set_rom_field(cur_doj, ROM_STATUS, "26", 2);
                if(doj && cur_doj->sender && doj->sender &&
                        cur_doj->sender != doj->sender) {
                    set_rom_field(cur_doj, ROM_OWN_CNL, "1", 1);
                }
                if(cur_doj->sender && ((con_int*)cur_doj->sender)->logon->do_balance) {
                    increment_ex_tag(cur_doj);
                    con_int* oat = get_compliance_con(ci->gk);
                    if(oat) {
                        send_oats_message(oat, current);
                    }
                }
                ct->on_cancel_order(ct, current);
            }
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
int dc_replace_order(con_int * ci, rcntr * risk)
{
    con_int* dest = 0;
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    if(doj->dest) {
        dest = doj->dest;
    } else {
        get_destination(ci->gk, doj->positions[ROM_DESTID].iov_base, 
                doj->positions[ROM_DESTID].iov_len, (void **) &dest);
    }
    if(dest && !dest->is_blocked && dest->connected) {
        dart_order_obj *rep = (dart_order_obj *) (doj->rep);
        set_rom_field(doj, 0, "S", 1);
        set_rom_field(doj, ROM_STATUS, "27", 2);
        memcpy(&rep->risk, risk, sizeof(rcntr));
        set_rom_field(rep, ROM_TAG, doj->positions[ROM_TAG].iov_base,
                        doj->positions[ROM_TAG].iov_len);
        if (rep->positions[ROM_SHARES].iov_len <= 0) {
            set_rom_field(rep, ROM_ORIG_SHARES,
                            doj->positions[ROM_SHARES].iov_base,
                            doj->positions[ROM_SHARES].iov_len);
            rep->risk.size = risk->size;
        } else {
            set_rom_field(rep, ROM_ORIG_SHARES,
                            rep->positions[ROM_SHARES].iov_base,
                            rep->positions[ROM_SHARES].iov_len);
            rep->risk.size = trans_shares(rep->positions[ROM_SHARES].iov_base,
                    rep->positions[ROM_SHARES].iov_len);
        }
        if (rep->positions[ROM_TYPE].iov_len <= 0) {
            set_rom_field(rep, ROM_TYPE, doj->positions[ROM_TYPE].iov_base,
                            doj->positions[ROM_TYPE].iov_len);
        }
        if(rep->positions[ROM_PRICE].iov_len > 0) {
            rcntr* rep_risk = &rep->risk;
            jiggly_price_converter(rep_risk,
                      (jp_limits_checker *) get_balance_struct(ci->gk),
                      rep, rep_risk->sec_type);
        }

        int allowed = 1;
        jp_limits_checker *jlc =
            (jp_limits_checker *) get_balance_struct(ci->gk);
        if(ci->logon && ci->logon->do_balance) {
            increment_ex_tag(doj);
            set_rom_field(rep, ROM_EX_TAG, doj->positions[ROM_EX_TAG].iov_base,
                    doj->positions[ROM_EX_TAG].iov_len);
             allowed = update_balance_replace(jlc, risk, &rep->risk);
        }
        if(allowed) {
            int ret = dest->on_replace_order(dest, risk);
            if(ret < 0) {
                if(ci->logon && ci->logon->do_balance) {
                    update_balance_rep_rej(jlc,risk, &rep->risk);
                }
            }
            return ret;
        } else {
            return -1;
        }
    } else {
        return -1;
    }
}
int on_dc_enter_order(con_int * ci, rcntr* risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    pthread_spin_lock(&ci->mutex);
    no_copy_insert(ci->cl_ord_id_map,
                   doj->positions[ROM_TAG].iov_base,
                   doj->positions[ROM_TAG].iov_len, doj);
    pthread_spin_unlock(&ci->mutex);
    send_dart_message((dartp*)ci->cparser, doj);
    record_dart_message(ci->o, doj);
    return 1;
}

int on_dc_cancel_order(con_int * ci, rcntr * risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    set_rom_field(doj, 0, "C", 1);
    send_dart_message((dartp*)ci->cparser, doj);
    record_dart_message(ci->o, doj);
    return 1;
}

int on_dc_replace_order(con_int * ci, rcntr * risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    dart_order_obj *rep = (dart_order_obj *) (doj->rep);
    set_rom_field(rep, ROM_TAG, doj->positions[ROM_TAG].iov_base,
                    doj->positions[ROM_TAG].iov_len);
    if (rep->positions[ROM_SHARES].iov_len <= 0) {
        set_rom_field(rep, ROM_ORIG_SHARES,
                        doj->positions[ROM_SHARES].iov_base,
                        doj->positions[ROM_SHARES].iov_len);
    } else {
        set_rom_field(rep, ROM_ORIG_SHARES,
                        rep->positions[ROM_SHARES].iov_base,
                        rep->positions[ROM_SHARES].iov_len);
    }
    if (rep->positions[ROM_TYPE].iov_len <= 0) {
        set_rom_field(rep, ROM_TYPE, doj->positions[ROM_TYPE].iov_base,
                        doj->positions[ROM_TYPE].iov_len);
    }
    send_dart_message((dartp*)ci->cparser, rep);
    record_dart_message(ci->o, rep);
    return 1;
}

void dc_cleansing_rinse(con_int* ci)
{
    if(ci) {
        remove_obj_by_client_id(ci->gk, ci->mod_name, ci->mod_name_len);
        if(ci->cparser) {
            dartp* dp = (dartp*)ci->cparser;
            clean_dart_parser(dp);
        }
        free(ci->mod_name);
        free(ci);
    }
}

static void dc_send_oats_message(con_int* ci, rcntr* cnt)
{
    con_int* oat = get_compliance_con(ci->gk);
    if(oat && ci->logon && ci->logon->do_balance) {
        send_oats_message(oat, cnt);
    }
}
void dc_on_update(con_int* ci, rcntr* cnt)
{
    dart_order_obj* doj = (dart_order_obj*)cnt->raw;
    increment_ex_tag(doj);
    ci->send_to_con(ci, cnt);
    if(doj->sender == 0x0) {
        doj->sender = ci;
    }
    record_dart_message(ci->o, doj);
    dc_send_oats_message(ci,cnt);
}

static int is_cmplx_leg(dart_order_obj* doj)
{
  int ret = 0;
  if(doj->positions[ROM_CPLX_TYPE].iov_len > 0) {
    char* type = doj->positions[ROM_CPLX_TYPE].iov_base;
    ret = type[0] == '6';
  }
  return ret;
}

static void dc_on_replaced(con_int* ci, rcntr* cnt)
{
    dart_order_obj* doj = (dart_order_obj*)cnt->raw;
    increment_ex_tag(doj);
    ci->send_to_con(ci, cnt);
    doj->sender = ci;
    record_dart_message(ci->o, doj);
    dc_send_oats_message(ci,cnt);
    if(doj->rep != 0x0) {
        dp_return_doj((dartp*)ci->cparser, doj->rep);
        doj->rep = 0x0;
    }
}

static void dc_on_fill(con_int* ci, rcntr* cnt)
{
    dart_order_obj* doj = (dart_order_obj*)cnt->raw;
    if(ci->logon && ci->logon->do_balance && !is_cmplx_leg(doj)) {
        jp_limits_checker *jlc =
            (jp_limits_checker *) get_balance_struct(ci->gk);
        update_balance_fill(jlc, cnt);
        increment_ex_tag(doj);
    }
    ci->send_to_con(ci, cnt);
    if(doj->sender == 0x0) {
        doj->sender = ci;
    }
    record_dart_message(ci->o, doj);
    dc_send_oats_message(ci,cnt);
    if(cnt->cum >= cnt->size && !is_cmplx(doj)) {
        dc_remove_from_maps(ci,cnt);
    }
}
static void dc_on_rej(con_int* ci, rcntr* cnt)
{
    uint8_t kill = 0;
    dart_order_obj* doj = (dart_order_obj*)cnt->raw;
    jp_limits_checker *jlc =
        (jp_limits_checker *) get_balance_struct(ci->gk);
    if(doj->positions[ROM_STATUS].iov_len == 2 && 
            strncmp("20", doj->positions[ROM_STATUS].iov_base, 2) == 0) {
        dart_order_obj* rep = (dart_order_obj*)doj->rep;
        if(rep) {
            rcntr rep_risk;
            memcpy(&rep_risk, cnt, sizeof(rcntr));
            if(rep->positions[ROM_PRICE].iov_len > 0) {
                jiggly_price_converter(&rep_risk,
                          (jp_limits_checker *) get_balance_struct(ci->gk),
                          rep, rep_risk.sec_type);
            }
            if(rep->positions[ROM_SHARES].iov_len > 0) {
                rep_risk.size = trans_shares(rep->positions[ROM_SHARES].iov_base,
                        rep->positions[ROM_SHARES].iov_len);
                rep_risk.size = (rep_risk.size - cnt->size);
                rep_risk.size = (rep_risk.size == 0) ? cnt->size : rep_risk.size;
            }
            if(ci->logon && ci->logon->do_balance) {
                update_balance_rep_rej(jlc,cnt, &rep_risk);
            }
        }
    } else if(doj->positions[ROM_STATUS].iov_len == 1) {
        if(ci->logon && ci->logon->do_balance && !is_cmplx_leg(doj)) {
            update_balance(jlc, cnt);
        }
        kill = 1;
    }
    increment_ex_tag(doj);
    ci->send_to_con(ci, cnt);
    if(doj->sender == 0x0) {
        doj->sender =ci;
    }
    record_dart_message(ci->o, doj);
    dc_send_oats_message(ci,cnt);
    if(kill && !is_cmplx(doj)) {
        dc_remove_from_maps(ci,cnt);
    }
}
static void dc_on_out(con_int* ci, rcntr* cnt)
{
    dart_order_obj* doj = (dart_order_obj*)cnt->raw;
    if(ci->logon && ci->logon->do_balance && !is_cmplx_leg(doj)) {
        jp_limits_checker *jlc =
            (jp_limits_checker *) get_balance_struct(ci->gk);
        update_balance(jlc, cnt);
    }
    increment_ex_tag(doj);
    ci->send_to_con(ci, cnt);
    if(doj->sender == 0x0) {
        doj->sender = ci;
    }
    record_dart_message(ci->o, doj);
    dc_send_oats_message(ci,cnt);
    if(!is_spread(doj)) {
        dc_remove_from_maps(ci,cnt);
    }
}
void dc_reset_seq(con_int* ci, int out, int in)
{
    /*TODO Add reset to dart_parser*/
}

static void dc_listener_connect(dartp* db, int is_connected,
                           dart_order_obj* log)
{
    con_int* ab = (con_int*)get_pass_back_point(db);
    if (is_connected == 1 && log != NULL) {
        /*copy con_int we need a local version for this connection*/
        con_int* ci = clone_interface(ab, log);
        ci->cparser = db;
        ci->sock = get_sock(db);
        ci->is_server = 1;
        reset_pass_back_point(db, ci);
        rex_con* rc = get_rex_con(db);
        ci->rc = rc;
        ci->logon = rc->logon;
        //rc->con_pass_back = db;
        int llen = getplen(log, 3);
        char *cancel_choice = getpval(log, 3);
        if (llen > 0 && cancel_choice != NULL &&
            ((strncmp("2", cancel_choice, 1) == 0)
             || (strncmp("1", cancel_choice, 1) == 0))) {
            ci->logon->cancel_on_dis = 1;
        }
        ci->should_buffer = 1;
        ci->can_send_orders = 1;
        ci->connected = 1;
        ci->update_oats(ci, is_connected);
    } else if(is_connected == 0) {
        ab->update_oats(ab, is_connected);
    }
}

static void dc_rc_server_con(void* pass_back, int is_connected,
                             dart_order_obj* log)
{
    con_int* ab = (con_int*)pass_back;
    if(is_connected == 0) {
        ab->update_oats(ab, is_connected);
        if(ab->rc->is_server ) {
            dartp* dp = (dartp*)ab->cparser;
            if(dp && get_logged_on(dp)) {
                clean_dart_parser(dp);
            }
        }
    }
}

static void dc_rc_connect(void* pass_back, int is_connected,
                          dart_order_obj* log)
{
    con_int* ab = (con_int*)pass_back;
    if(is_connected == 0) {
        ab->update_oats(ab, is_connected);
    }
}

static void dc_connect(dartp* db, int is_connected,
                        dart_order_obj* log)
{
    con_int* ci = get_pass_back_point(db);
    ci->update_oats(ci, is_connected);
    switch(is_connected) {
        case 0:
            ci->can_send_orders = 0;
            ci->connected = 0;
            reload_config(ci->ex);
            dart_reconnect(db);
            break;
        case 1:
            ci->can_send_orders = 1;
            ci->connected = 1;
            break;
        case 2:
            ci->can_send_orders = 0;
            ci->connected = 1;
            break;
    }
}

static void reset_owner(con_int* ci, rcntr* cnt)
{
    dart_order_obj* doj = (dart_order_obj*)cnt->raw;
    if(ci->logon) {
        memcpy(cnt->owner, ci->logon->name, ci->logon->nlen);
        cnt->olen = ci->logon->nlen;
        set_rom_field(doj, ROM_OWNER, cnt->owner, cnt->olen); 
    }
    doj->sender = ci;
}
void add_dom_sender(con_int* con)
{
    hashmap_iterator_t hi = hashmap_iterator_begin(con->cl_ord_id_map);
    rcntr* cnt = 0;
    char *key = 0;
    int key_len = 0;
    int valid = hashmap_iterator_dereference(hi, &key,
                                             &key_len,
                                             (void *) &cnt);
    while (valid) {
        reset_owner(con, cnt);
        valid = hashmap_iterator_next(con->cl_ord_id_map, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &cnt);
    }
    hashmap_iterator_destroy(hi);

}

void create_connection(con_int * ci, char *filename, int name_len,
                       gatekeeper * gk)
{

    ci->cl_ord_id_map = get_initial_order_map(gk, ci->mod_name,
                                              ci->mod_name_len);
    ci->sbm = 0;
    ci->tg = 0;
    ci->send_to_con = dc_send;
    ci->reset_seq = dc_reset_seq;
    ci->cleansing_rinse = dc_cleansing_rinse;
    ci->can_send_orders = 0;
    add_dom_sender(ci);
    //
    if(ci->is_server) {
        ci->on_update = dc_on_update;
        ci->on_enter_order = dc_enter_order;
        ci->on_replace_order = dc_replace_order;
        ci->on_cancel_order = dc_cancel_order;
        ci->on_fill = dc_on_fill;
        ci->on_replaced = dc_on_replaced;
        ci->on_rej = dc_on_rej;
        ci->on_out = dc_on_out;
        dartp* dp = create_dart_parser(ci, dc_listener_connect,dc_switchboard,get_gk_fact(gk),
                                       ci->o,ci->ex->order_allocation,
                                       1, 1);
        rex_con* rc = add_listener(ci->p, ci->ex, NULL, ci,
                                     //ci->users,
                                     dart_write_callback, 
                                     dc_rc_server_con,
                                     ph_parse_rom_message,
                                     validate_logon);
        rc->parser = dp;
        ci->rc = rc;
        ci->cparser = 0;
    } else {
        ci->on_enter_order = on_dc_enter_order;
        ci->on_replace_order = on_dc_replace_order;
        ci->on_cancel_order = on_dc_cancel_order;
        //ci->from_con = ex_dom_con;
        dartp* dp = create_dart_parser(ci, dc_connect,ex_dom_con,get_gk_fact(gk),
                                   ci->o,ci->ex->order_allocation,
                                    1, 0);
        login_info* li = build_logon_info_from_config(ci->ex);
        rex_con* rc = create_rex_con(ci->p, ci->ex, dp, ci,li,
                                  dart_write_callback, 
                                  dc_rc_connect,
                                  ph_parse_rom_message, NULL);
        rc->connect = send_dart_logon;
        rc->parser = dp;
        ci->cparser = dp;
        ci->rc = rc;
        dart_set_rex_con(dp, rc);
        rex_connect(ci->p, rc);
    }
}

static void reset_fields_for_fill(dart_order_obj * orig,
                                  dart_order_obj * doj, con_int* ob)
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

static void dc_set_rep_fields(dart_order_obj * ute, dart_order_obj * rep, 
                           con_int* ob)
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

void ex_dom_con(void* a, dart_order_obj * doj)
{
    con_int *ci = (con_int*) a;
    if (doj && ci) {
        doj->dest = ci;
        char *rtype = doj->positions[ROM_STATUS].iov_base;
        rcntr* cnt = 0;
        dart_order_obj *orig = 0;
        int destroy_orig = 0;
        int found = get_obj_by_id(ci, doj->positions[ROM_TAG].iov_base,
                                  doj->positions[ROM_TAG].iov_len, &cnt);
        if(found) {
            orig = (dart_order_obj*)cnt->raw;
        } 
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
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "13", 2);
                fill_in_rom_field(orig, ROM_LEAVES,
                                  orig->positions[ROM_SHARES].iov_base,
                                  orig->positions[ROM_SHARES].iov_len);
                fill_in_rom_field(orig, ROM_CLORDID,
                                  doj->positions[ROM_CLORDID].iov_base,
                                  doj->positions[ROM_CLORDID].iov_len);
                ci->on_update(ci, &orig->risk);
            } else if (rtype && strncmp(rtype, "4", 1) == 0) {
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "4", 1);
                remove_from_maps(ci, doj->positions[ROM_TAG].iov_base,
                                 doj->positions[ROM_TAG].iov_len);
                ci->on_out(ci, &orig->risk);
                destroy_orig = 1;
            } else if (rtype && strncmp(rtype, "5", 1) == 0) {
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "5", 1);
                if (orig->rep != NULL) {
                    dart_order_obj *rep = (dart_order_obj *) orig->rep;
                    dc_set_rep_fields(orig, rep, ci);
                } else {
                    dc_set_rep_fields(orig, doj, ci);
                }
                fill_in_rom_field(orig, ROM_CLORDID,
                                  doj->positions[ROM_CLORDID].iov_base,
                                  doj->positions[ROM_CLORDID].iov_len);
                ci->on_replaced(ci, &orig->risk);
            } else if (rtype && strncmp(rtype, "8", 1) == 0) {
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "8", 1);
                remove_from_maps(ci, doj->positions[ROM_TAG].iov_base,
                                 doj->positions[ROM_TAG].iov_len);
                ci->on_rej(ci, &orig->risk);
                destroy_orig = 1;
            } else if (rtype && strncmp(rtype, "6", 1) == 0) {
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "6", 1);
                ci->on_update(ci, &orig->risk);
            } else if (rtype && strncmp(rtype, "20", 2) == 0) {
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "20", 2);
                ci->on_rej(ci, &orig->risk);
            } else if (rtype && doj->positions[ROM_STATUS].iov_len == 1 &&
                       strncmp(rtype, "2", 1) == 0) {
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "2", 1);
                reset_fields_for_fill(orig, doj, ci);
                if (!is_spread(orig)) {
                    remove_from_maps(ci, doj->positions[ROM_TAG].iov_base,
                                     doj->positions[ROM_TAG].iov_len);
                }
                ci->on_fill(ci, &orig->risk);
                destroy_orig = 1;
            } else if (rtype && strncmp(rtype, "12", 2) == 0) {
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "4", 1);
                remove_from_maps(ci, doj->positions[ROM_TAG].iov_base,
                                 doj->positions[ROM_TAG].iov_len);
                ci->on_out(ci, &orig->risk);
                destroy_orig = 1;
            } else if (rtype && strncmp(rtype, "14", 2) == 0) {
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "14", 2);
                ci->on_rej(ci, &orig->risk);
            } else if (rtype && doj->positions[ROM_STATUS].iov_len == 1
                       && strncmp(rtype, "1", 1) == 0) {
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "1", 1);
                reset_fields_for_fill(orig, doj, ci);
                ci->on_fill(ci, &orig->risk);
            } else if (rtype && strncmp(rtype, "3", 1) == 0) {
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "3", 1);
                ci->on_update(ci, &orig->risk);
            }
        }
        reset_doj(((dartfact*)doj->dalloc), doj);
        //destroy_gk_obj(ci->gk, doj);
        if (orig != NULL) {
            unlock(orig);
            if (destroy_orig) {
                reset_doj(((dartfact*)orig->dalloc), orig);
                //destroy_gk_obj(ci->gk, orig);
            }
        }
    }
}
rcntr* create_cnt_for_doj(con_int* ci, dart_order_obj* doj)
{
    rcntr* cnt = get_cnt(ci->gk);
    cnt->raw = doj;
    cnt->rlen = sizeof(dart_order_obj);

    cnt->cum = 0;
    cnt->last = 0;
    if(doj->positions[ROM_SHARES].iov_len > 0) {
        cnt->size = trans_shares(doj->positions[ROM_SHARES].iov_base, 
                doj->positions[ROM_SHARES].iov_len);
    }
    if(doj->positions[ROM_SEC_TYPE].iov_len > 0) {
        cnt->sec_type = (((char*)doj->positions[ROM_SEC_TYPE].iov_base)[0]);
    } else {
        cnt->sec_type = 'E';
    }
    cnt->side = (((char*)doj->positions[ROM_SIDE].iov_base)[0]);
    
    memcpy(cnt->owner, ci->logon->name, ci->logon->nlen);
    cnt->olen = ci->logon->nlen;
    set_rom_field(doj, ROM_OWNER, cnt->owner, cnt->olen); 

    if(doj->positions[ROM_SYM].iov_len > 0) {
        cnt->sym = doj->positions[ROM_SYM].iov_base;
        cnt->slen = doj->positions[ROM_SYM].iov_len;
    } else if(!is_spread(doj)) {
        return_cnt(ci->gk,cnt);
        return NULL;
    }
    if(doj->positions[ROM_CLR_ACC].iov_len > 0) {
        cnt->clr_acc = doj->positions[ROM_CLR_ACC].iov_base;
        cnt->clr_len = doj->positions[ROM_CLR_ACC].iov_len;
    }
    
    if(doj->positions[ROM_PRICE].iov_len > 0) {
        jiggly_price_converter(cnt,
                          (jp_limits_checker *) get_balance_struct(ci->gk),
                          doj, cnt->sec_type);
    }
    return cnt;
}


void dc_switchboard(void* a, dart_order_obj * doj)
{
    con_int *ap = (con_int *) a;
    if (doj && ap) {
        doj->sender = ap;
        char *rtype = doj->positions[0].iov_base;
        switch (rtype[0]) {
        case 'E':
            {
                rcntr* cnt = create_cnt_for_doj(ap,doj);
                if(cnt != NULL) {
                    int rej = dc_enter_order(ap, cnt);
                    dc_send_oats_message(ap, cnt);
                    if(rej < 0) {
                        increment_ex_tag(doj);
                        send_reject(ap, doj, cnt->errStr, cnt->err_len, "8", 1);
                        dc_send_oats_message(ap, cnt);
                        record_dart_message(ap->o, doj);
                        return_cnt(ap->gk, cnt);
                        dp_return_doj((dartp*)ap->cparser, doj);
                    }
                } else {
                    set_rom_field(doj, ROM_SENDER, ap->logon->name, ap->logon->nlen);
                    send_reject(ap, doj, "Invalid Order", 13, "8", 1);
                    record_dart_message(ap->o, doj);
                    dp_return_doj((dartp*)ap->cparser, doj);
                }
            }
            break;
        case 'C':
            {
                int check_tag = ap->logon->do_balance ? ROM_CLIENT_ID : ROM_TAG;
                rcntr* cnt = 0;
                int found = get_obj_by_id(ap, doj->positions[check_tag].iov_base,
                                      doj->positions[check_tag].iov_len, &cnt);
                if(found) {
                    int ret = dc_cancel_order(ap, cnt);
                    dc_send_oats_message(ap, cnt);
                    if(ret < 0) {
                        increment_ex_tag(doj);
                        send_reject(ap, doj, cnt->errStr, cnt->err_len, "14", 2);
                        dc_send_oats_message(ap, cnt);
                        record_dart_message(ap->o, doj);
                    }
                } else {
                    send_reject(ap, doj,
                            "Order not found too late to cancel", 34,
                            "14", 2);
                    set_rom_field(doj, ROM_SENDER, ap->logon->name, ap->logon->nlen);
                    record_dart_message(ap->o, doj);
                    dp_return_doj((dartp*)ap->cparser, doj);
                }
            }
            break;
        case 'c':
            {
                cancel_all_orders(ap, doj);
            }
            break;
        case 'R':
            {
                int check_tag = ap->logon->do_balance ? ROM_CLIENT_ID : ROM_TAG;
                rcntr* cnt = 0;
                int found = get_obj_by_id(ap, doj->positions[check_tag].iov_base,
                                      doj->positions[check_tag].iov_len, &cnt);
                if(found) {
                    dart_order_obj* orig = (dart_order_obj*)cnt->raw;
                    orig->rep = doj;
                    int ret = dc_replace_order(ap, cnt);
                    dc_send_oats_message(ap, cnt);
                    if(ret < 0) {
                        increment_ex_tag(doj);
                        send_reject(ap, doj, cnt->errStr, cnt->err_len, "20", 2);
                        dc_send_oats_message(ap, cnt);
                        record_dart_message(ap->o, doj);
                    }
                } else {
                    send_reject(ap, doj,
                            "Order not found too late to replace", 35,
                            "20", 2);
                    set_rom_field(doj, ROM_SENDER, ap->logon->name, ap->logon->nlen);
                    record_dart_message(ap->o, doj);
                    dp_return_doj((dartp*)ap->cparser, doj);
                }
            }
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
