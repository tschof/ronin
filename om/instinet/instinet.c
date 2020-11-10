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

#include "instinet.h"
#include "message_queue.h"
#include "parser_func.h"
#include "dart_constants.h"
#include "config_loader.h"
#include "ex_config_helper.h"
#include "fix_handler.h"
#include "fix_trans.h"
#include "connection_interface.h"
#include "mailer.h"
#include "debug_logger.h"
#include "rom_fix_trans_funcs.h"


void from_instinet(dart_order_obj * doj, void *a);

int build_instinet_offsets(struct async_parse_args *apa)
{
    //con_interface* con = (con_interface*)apa->ds_book;
    return 0;
}
static void trans_instinet_cap(trans_t* t, ofp* fix_obj, char* cap,
        char* mm_home, int mmlen)
{
    switch(cap[0]) {
        case 'B':
            set_fix_val(t, fix_obj, 204, "7", 1);
            break;
        case 'X':
            set_fix_val(t, fix_obj, 204, "8", 1);
            break;
        case 'S':
            set_fix_val(t, fix_obj, 204, "5", 1);
            break;
	case 'A':
            set_fix_val(t, fix_obj, 47, "A", 1);
	    break;
    }
}

static void set_ex_dest(trans_t* t, ofp* fix_obj, char* instr, int instrlen)
{
	set_fix_val(t, fix_obj, 57, instr, instrlen);
}

void instinet_handle_parent_fill(dart_order_obj* orig,
        dart_order_obj* parent,con_interface* ci,
        void (*next_step) (struct connection_interface *,
            dart_order_obj * doj), char* status, int status_len)
{
        char copier[32];
        memset(copier, '\0', 32);
        memcpy(&copier, orig->positions[ROM_LAST_SHARES].iov_base, orig->positions[ROM_LAST_SHARES].iov_len);
        long clast = strtol(copier, NULL, 10);
        memset(copier, '\0', 32);
        memcpy(&copier, parent->positions[ROM_SHARES].iov_base, parent->positions[ROM_SHARES].iov_len);
        long parent_size =strtol(copier, NULL, 10);
        long cshares = 0;
        memcpy(&cshares, orig->positions[ROM_BIN_QTY].iov_base,
                orig->positions[ROM_BIN_QTY].iov_len);
        long ratio = 0;
        memcpy(&ratio, parent->positions[ROM_BIN_RATIO].iov_base,
                parent->positions[ROM_BIN_RATIO].iov_len);
        long delta = 0;
        if(parent->positions[ROM_BIN_SHARES_DELTA].iov_len > 0) {
            memcpy(&delta, parent->positions[ROM_BIN_SHARES_DELTA].iov_base,
                    parent->positions[ROM_BIN_SHARES_DELTA].iov_len);
        }
        clast += delta;
        long mod = clast % cshares;
        if(status[0] == '1' && mod) {
            fill_in_rom_field(parent, ROM_BIN_SHARES_DELTA , &clast, sizeof(long));
            unlock(parent);
            return; 
        } else {
            long zero = 0;
            fill_in_rom_field(parent, ROM_BIN_SHARES_DELTA , &zero, sizeof(long));
            ratio -= clast;
            fill_in_rom_field(parent, ROM_BIN_RATIO, &ratio, sizeof(long));
            databuf_t* db = get_databuf(ci->gk);
            dart_itoa(parent, (ratio/cshares), ROM_LEAVES, db);
            databuf_reset(db);
            dart_itoa(parent, (parent_size - (ratio/cshares)), ROM_CUM, db);
            databuf_reset(db);
            dart_itoa(parent, ((clast/cshares)), ROM_LAST_SHARES, db);
            destroy_gk_db(ci->gk, db);
        }
        next_step(ci, parent);
        unlock(parent);
}

void instinet_calc_leg_ratio(dart_order_obj* child, dart_order_obj* parent)
{
    char copier[32];
    memset(copier, '\0', 32);
    memcpy(&copier, parent->positions[ROM_SHARES].iov_base, parent->positions[ROM_SHARES].iov_len);
    long parent_size =strtol(copier, NULL, 10);
    memset(copier, '\0', 32);
    long parent_cum = 0;
    if(parent->positions[ROM_CUM].iov_len > 0) {
        memcpy(&copier, parent->positions[ROM_CUM].iov_base, parent->positions[ROM_CUM].iov_len);
        parent_cum =strtol(copier, NULL, 10);
    }
    parent_size -= parent_cum;
    memset(copier, '\0', 32);
    memcpy(&copier, child->positions[ROM_ORIG_SHARES].iov_base, child->positions[ROM_ORIG_SHARES].iov_len);
    long child_size = strtol(copier, NULL, 10);
    fill_in_rom_field(child, ROM_BIN_QTY, &child_size, sizeof(long));
    send_debug_message("Child Size: %ld, Parent: %ld \n", child_size, parent_size);
    parent_size *= child_size;
    fill_in_rom_field(parent, ROM_BIN_RATIO, &parent_size, sizeof(long));
}
void instinet_reset_shares_on_replace(dart_order_obj* doj)
{
    children* lch = doj->childs;
    dart_order_obj* ldoj = (dart_order_obj*)lch->payload;
    instinet_calc_leg_ratio(ldoj, doj);
}
static void instinet_reset_fields_for_parent(dart_order_obj* orig,
        dart_order_obj* doj,con_interface* ci,
        void (*next_step) (struct connection_interface *,
            dart_order_obj * doj))
{
    char* mark = getpval(orig, ROM_LEG_MARK);
    if(getplen(orig, ROM_LEG_MARK) > 0 && mark[0] == '1') {
        dart_order_obj* parent = 0;
        int found = get_obj_by_id(ci, doj->positions[FIX_CLORDID].iov_base,
                doj->positions[FIX_CLORDID].iov_len, &parent);
        if(found == 0) {
            found = get_obj_by_id(ci, doj->positions[FIX_ORIG_ID].iov_base,
                    doj->positions[FIX_ORIG_ID].iov_len, &parent);
        }
        if(found && lock_n_check(parent)) {
            set_rom_field(parent, 0, ROM_ZERO_STATUS, 1);
            set_rom_field(parent, ROM_STATUS, getpval(orig, ROM_STATUS), getplen(orig, ROM_STATUS));
            char* status = getpval(orig, ROM_STATUS);
            int status_len = getplen(orig, ROM_STATUS);
            if(status[0] == '5') {
                dart_order_obj *rep = (dart_order_obj *) parent->rep;
                if(rep != 0) {
                    set_rep_fields(parent, rep, ci);
                    instinet_reset_shares_on_replace(parent);
                }
            }
            if(status_len == 1 && (status[0] == '1' || status[0] == '2')) {
                instinet_handle_parent_fill(orig, parent, ci, next_step, status, status_len);
            } else {
                next_step(ci, parent);
                unlock(parent);
            }
        }
    }
}
static void gs_set_cfi_code(trans_t* t, dart_order_obj* obj, ofp* fix_spread)
{
    char* sectype = getpval(obj, ROM_SEC_TYPE);
    if(getplen(obj, ROM_SEC_TYPE) > 0 && sectype[0] == 'O') {
        char* cp = getpval(obj, ROM_P_C);
        if(getplen(obj, ROM_P_C) > 0 && cp[0] == 'P') {
            set_fix_val(t, fix_spread, 608, "OP", 2);
        } else {
            set_fix_val(t, fix_spread, 608, "OC", 2);
        }
    } else {
        set_fix_val(t, fix_spread, 608, "ES", 2);
    }
}
static void gs_set_leg_mat_date(trans_t* t, dart_order_obj* obj, ofp* fix_spread)
{
    char* matmy = getpval(obj, ROM_EXPIRE_DATE);
    char* mday = getpval(obj, ROM_EXPIRE_DAY);
    if(getplen(obj, ROM_EXPIRE_DATE) > 0 && getplen(obj, ROM_EXPIRE_DAY) > 0) {
        char lmd[8];
        memcpy(lmd, matmy, 6);
        memcpy(lmd + 6, mday, 2);
        set_fix_val(t, fix_spread, 611, lmd, 8);
    }
}
static void gs_set_leg_strike(trans_t* t, dart_order_obj* obj, ofp* fix_spread)
{
    if(getplen(obj, ROM_STRIKE) > 0) {
        set_fix_val(t, fix_spread, 612, getpval(obj, ROM_STRIKE),
                    getplen(obj, ROM_STRIKE));
    }
}
static void gs_set_leg_pos_effect(trans_t* t, dart_order_obj* obj, ofp* fix_spread)
{
    char* sectype = getpval(obj, ROM_SEC_TYPE);
    if(getplen(obj, ROM_SEC_TYPE) > 0 && getplen(obj, ROM_O_C) > 0 
               && sectype[0] == 'O') {
        char* oc = getpval(obj, ROM_O_C);
        if(oc[0] == '1') {
            set_fix_val(t, fix_spread, 564, "O", 1);
        } else {
            set_fix_val(t, fix_spread, 564, "C", 1);
        }
    }
}

void gs_apply_spread_fields(trans_t* t, dart_order_obj* obj, ofp* fix_spread)
{
    set_fix_val(t, fix_spread, 600, getpval(obj, ROM_SYM), getplen(obj, ROM_SYM));
    gs_set_cfi_code(t, obj, fix_spread);
    set_fix_val(t, fix_spread, 610, getpval(obj, ROM_EXPIRE_DATE), getplen(obj, ROM_EXPIRE_DATE));
    gs_set_leg_mat_date(t, obj, fix_spread);
    gs_set_leg_strike(t, obj, fix_spread);
    gs_set_leg_pos_effect(t, obj, fix_spread);
    set_fix_val(t, fix_spread, 624, getpval(obj, ROM_SIDE), getplen(obj, ROM_SIDE));
    set_fix_val(t, fix_spread, 623, getpval(obj, ROM_ORIG_SHARES), getplen(obj, ROM_ORIG_SHARES));
}


static void add_spread_legs(trans_t* t, dart_order_obj* doj, 
        ofp* fix_obj, con_interface* ci, int new)
{
    {
          children* lch = doj->childs;
          dart_order_obj* ldoj = (dart_order_obj*)lch->payload;
          set_rom_field(ldoj, ROM_SYM, getpval(ldoj, ROM_SYM), getplen(ldoj, ROM_SYM));
          set_fix_val(t, fix_obj, 55, getpval(ldoj, ROM_SYM), getplen(ldoj, ROM_SYM));
    } 
      children* lch = doj->childs;
    add_num_val(t, fix_obj, 555, doj->num_childs);
    int check = 0;
    while(lch != NULL) {
        dart_order_obj* ldoj = (dart_order_obj*)lch->payload;
        if(new) {
            char clordid[32];
            memset(clordid, '\0', 32);
            int cur_size = create_in_place_ord_token(ci->tg,
                    clordid, 12);
            fill_in_rom_field(ldoj, ROM_CLORDID, clordid,
                    cur_size);
            pthread_spin_lock(&ci->mutex);
            no_copy_insert(ci->cl_ord_id_map, clordid, cur_size,
                    ldoj);
            pthread_spin_unlock(&ci->mutex);
            set_fix_val(t, fix_obj, 654, getpval(ldoj, ROM_CLORDID),
                    getplen(ldoj, ROM_CLORDID));
            ldoj->dest = ci;
            ldoj->sender = doj->sender;
            set_rom_field(ldoj, ROM_ORIG_SHARES, getpval(ldoj, ROM_SHARES), getplen(ldoj, ROM_SHARES));
            char copier[32];
            memset(copier, '\0', 32);
            memcpy(&copier, ldoj->positions[ROM_ORIG_SHARES].iov_base, ldoj->positions[ROM_ORIG_SHARES].iov_len);
            long child_size = strtol(copier, NULL, 10);
            fill_in_rom_field(ldoj, ROM_BIN_QTY, &child_size, sizeof(long));
        } else {
            set_fix_val(t, fix_obj, 654, getpval(ldoj, ROM_CLORDID),
                    getplen(ldoj, ROM_CLORDID));
        }
        gs_apply_spread_fields(t, ldoj, fix_obj);
        if(check == 0) {
            set_rom_field(ldoj, ROM_SYM, getpval(ldoj, ROM_SYM), getplen(ldoj, ROM_SYM)); 
            set_rom_field(ldoj, ROM_LEG_MARK, "1", 1); 
            if(new) {
                instinet_calc_leg_ratio(ldoj, doj);
            }
        }
        ++check;
        lch = lch->next;
    }
}


int on_instinet_enter_order(con_interface * ci, dart_order_obj * doj,
        struct async_parse_args *ap)
{
    trans_t* t = (trans_t*)ci->parser;
    ofp* fix_obj = 0;
    if(doj->childs == 0) {
        fix_obj = get_fixy_message(t,0x44);
    } else {
        fix_obj = get_fixy_message(t,0x4241);
    }
    if(getplen(doj, ROM_INSTR) > 0) {
        set_rom_field(doj, ROM_COPY_INSTR,
                getpval(doj, ROM_INSTR), getplen(doj, ROM_INSTR));
    }
    int clr_len = 0;
    char* flipper = get_mpid_for_clr_acct(ci->sbm,
            getpval(doj, ROM_CLR_ACC),
            getplen(doj, ROM_CLR_ACC),
            &clr_len);
    if(clr_len > 0) {
        set_fix_val(t, fix_obj, 115, flipper, clr_len);
    } else {
		set_fix_val(t, fix_obj, 115, "DART", 4);
    }
	set_fix_val(t, fix_obj, 116, getpval(doj,ROM_CLR_ACC), getplen(doj,ROM_CLR_ACC));
    /*Begin*/
    char clordid[32];
    memset(clordid, '\0', 32);
    int cur_size = create_multiday_in_place_ord_token(ci->tg,
            clordid, 12);
    fill_in_rom_field(doj, ROM_CLORDID, clordid, cur_size);
    pthread_spin_lock(&ci->mutex);
    no_copy_insert(ci->cl_ord_id_map, doj->positions[ROM_CLORDID].iov_base,
            doj->positions[ROM_CLORDID].iov_len, doj);
    pthread_spin_unlock(&ci->mutex);
    set_fix_val(t, fix_obj, 11, doj->positions[ROM_CLORDID].iov_base,
            doj->positions[ROM_CLORDID].iov_len);
    /*End*/
    if(getplen(doj, ROM_SYM) > 0) {
        set_fix_val(t, fix_obj, 55, getpval(doj, ROM_SYM), getplen(doj, ROM_SYM));
    }

    set_ex_dest(t, fix_obj, getpval(doj,ROM_COPY_INSTR),
            getplen(doj, ROM_COPY_INSTR));
    char* sectype = getpval(doj, ROM_SEC_TYPE);
    if(getplen(doj, ROM_SEC_TYPE) > 0 && sectype[0] == 'E') {
        set_fix_val(t, fix_obj, 167, "CS", 2);
    } else if(doj->childs == 0) {
        set_rom_field(doj, ROM_EXEC_SEC_TYPE, "OPT", 3);
        set_fix_val(t, fix_obj, 167, "OPT", 3);
        if(getplen(doj, ROM_EXPIRE_DATE) > 0) {
            set_fix_val(t, fix_obj, 200, getpval(doj,ROM_EXPIRE_DATE), getplen(doj,ROM_EXPIRE_DATE));
        }
        if(getplen(doj, ROM_P_C) > 0) {
            char* pc = (char*)getpval(doj, ROM_P_C);
            add_single_char_val(t, fix_obj, "201", 3, translate_put_call(pc[0]));
        }
        if(getplen(doj, ROM_STRIKE) > 0) {
            set_fix_val(t, fix_obj, 202, getpval(doj,ROM_STRIKE), getplen(doj,ROM_STRIKE));
        }
        if(getplen(doj, ROM_EXPIRE_DAY) > 0) {
            set_fix_val(t, fix_obj, 205, getpval(doj,ROM_EXPIRE_DAY), getplen(doj,ROM_EXPIRE_DAY));
        }
    } else {
        set_fix_val(t, fix_obj, 167, "MLEG", 4);
        set_rom_field(doj, ROM_EXEC_SEC_TYPE, "MLEG", 4);
    }
    set_fix_val(t, fix_obj, 54, getpval(doj, ROM_SIDE), getplen(doj, ROM_SIDE));
    if(is_spread(doj)) {
        set_rom_field(doj, ROM_STASH_SIDE, getpval(doj, ROM_SIDE), getplen(doj, ROM_SIDE));
    }
    set_exec_time(t, fix_obj);
    set_fix_val(t, fix_obj, 38, getpval(doj, ROM_SHARES), getplen(doj, ROM_SHARES));
    if(getplen(doj, ROM_ORIG_SHARES) < 0) {
        set_rom_field(doj, ROM_ORIG_SHARES, getpval(doj, ROM_SHARES), getplen(doj, ROM_SHARES));
    } 
    if(getplen(doj, ROM_MAX_FLOOR) > 0) {
        set_fix_val(t, fix_obj, 111, getpval(doj,ROM_MAX_FLOOR), getplen(doj,ROM_MAX_FLOOR));
    }
    if(getplen(doj, ROM_MIN_QTY) > 0) {
        set_fix_val(t, fix_obj, 110, getpval(doj,ROM_MIN_QTY), getplen(doj,ROM_MIN_QTY));
    }
    set_fix_val(t, fix_obj, 40, getpval(doj,ROM_TYPE), getplen(doj,ROM_TYPE));
    if(getplen(doj, ROM_PRICE) > 0) {
        set_fix_val(t, fix_obj, 44, getpval(doj,ROM_PRICE), getplen(doj,ROM_PRICE));
    }
    if(getplen(doj,ROM_STOP) > 0) {
        set_fix_val(t, fix_obj, 99, getpval(doj,ROM_STOP), getplen(doj,ROM_STOP));
    }
    set_fix_val(t, fix_obj, 59, getpval(doj,ROM_TIF), getplen(doj,ROM_TIF));
    set_fix_val(t, fix_obj, 1, getpval(doj,ROM_CLR_ACC), getplen(doj,ROM_CLR_ACC));
    set_fix_val(t, fix_obj, 207, "N", 1);
    if(getplen(doj, ROM_O_C) > 0) {
        char* oc_c = (char*)getpval(doj, ROM_O_C);
        add_single_char_val(t, fix_obj, "77",2, translate_open_close(oc_c[0]));
    }
    if(getplen(doj, ROM_CAP) > 0) {
        char* mmhome = getpval(doj, ROM_MM_HOME);
        int mmlen = getplen(doj, ROM_MM_HOME);
        trans_instinet_cap(t, fix_obj, getpval(doj, ROM_CAP), mmhome, mmlen);
    }
    if(getplen(doj, ROM_EXPIRE_TIME) > 0) { 
        set_fix_val(t, fix_obj, 432, getpval(doj,ROM_EXPIRE_TIME),
                getplen(doj,ROM_EXPIRE_TIME));
    }
    if(getplen(doj, ROM_CMTA) > 0) {
        set_fix_val(t, fix_obj, 439, getpval(doj,ROM_CMTA), getplen(doj,ROM_CMTA));
    }
    if(getplen(doj, ROM_EXEC_INST) >= 1) {
        set_fix_val(t, fix_obj, 18, getpval(doj,ROM_EXEC_INST), getplen(doj,ROM_EXEC_INST));
    } 
    if(getplen(doj, ROM_SHORT_LEND) > 0&& sectype[0] == 'E') {
        set_fix_val(t, fix_obj, 114, "N", 1);
    }
    if(doj->childs != 0) {
        add_spread_legs(t, doj, fix_obj, ci, 1);
    }
    dart_send_fix_message(fix_obj, ci);
    destroy_fixy_message(t, fix_obj);
    return 1;
}
int on_instinet_cancel_order(con_interface * ci, dart_order_obj * doj)
{
    trans_t* t = (trans_t*)ci->parser;
    ofp* fix_obj = get_fixy_message(t,0x46);
    check_and_resize(doj, 12);
    int clr_len = 0;
    char* flipper = get_mpid_for_clr_acct(ci->sbm,
            getpval(doj, ROM_CLR_ACC),
            getplen(doj, ROM_CLR_ACC),
            &clr_len);
    if(clr_len > 0) {
        set_fix_val(t, fix_obj, 115, flipper, clr_len);
    } else {
		set_fix_val(t, fix_obj, 115, "DART", 4);
    }
	set_fix_val(t, fix_obj, 116, getpval(doj,ROM_CLR_ACC), getplen(doj,ROM_CLR_ACC));
    /*Begin*/
    char clordid[32];
    memset(clordid, '\0', 32);
    int cur_size = create_multiday_in_place_ord_token(ci->tg,
            clordid, 12);
    fill_in_rom_field(doj, ROM_PENDING_ID, clordid, cur_size);
    set_fix_val(t, fix_obj, 11, doj->positions[ROM_PENDING_ID].iov_base,
            doj->positions[ROM_PENDING_ID].iov_len);
    set_ex_dest(t, fix_obj, getpval(doj,ROM_COPY_INSTR),
            getplen(doj, ROM_COPY_INSTR));
    /*End*/
    set_fix_val(t, fix_obj, 41, getpval(doj, ROM_CLORDID), getplen(doj, ROM_CLORDID));
    set_fix_val(t, fix_obj, 38, getpval(doj, ROM_SHARES), getplen(doj, ROM_SHARES));
	char* sectype = getpval(doj, ROM_SEC_TYPE);
    if(getplen(doj, ROM_SEC_TYPE) > 0 && sectype[0] == 'E') {
        set_fix_val(t, fix_obj, 167, "CS", 2);
        if(getplen(doj, ROM_SYM) > 0) {
            set_fix_val(t, fix_obj, 55, getpval(doj, ROM_SYM), getplen(doj, ROM_SYM));
        }
    } else if(doj->childs == 0) {
        if(getplen(doj, ROM_SYM) > 0) {
            set_fix_val(t, fix_obj, 55, getpval(doj, ROM_SYM), getplen(doj, ROM_SYM));
        }
        if(getplen(doj, ROM_EXPIRE_DATE) > 0) {
            set_fix_val(t, fix_obj, 200, getpval(doj,ROM_EXPIRE_DATE), getplen(doj,ROM_EXPIRE_DATE));
        }
        if(getplen(doj, ROM_P_C) > 0) {
            char* pc = (char*)getpval(doj, ROM_P_C);
            add_single_char_val(t, fix_obj, "201", 3, translate_put_call(pc[0]));
        }
        if(getplen(doj, ROM_STRIKE) > 0) {
            set_fix_val(t, fix_obj, 202, getpval(doj,ROM_STRIKE), getplen(doj,ROM_STRIKE));
        }
        if(getplen(doj, ROM_EXPIRE_DAY) > 0) {
            set_fix_val(t, fix_obj, 205, getpval(doj,ROM_EXPIRE_DAY), getplen(doj,ROM_EXPIRE_DAY));
        }
        set_rom_field(doj, ROM_EXEC_SEC_TYPE, "OPT", 2);
        set_fix_val(t, fix_obj, 167, "OPT", 3);
    } else {
        set_fix_val(t, fix_obj, 167, "MLEG", 4);
    }
    if(is_spread(doj)) {
        if(getplen(doj, ROM_STASH_SIDE) > 0) {
            set_fix_val(t, fix_obj, 54, getpval(doj, ROM_STASH_SIDE), getplen(doj, ROM_STASH_SIDE));
        } else {
            set_rom_field(doj, ROM_STASH_SIDE, getpval(doj, ROM_SIDE), getplen(doj, ROM_SIDE));
            set_fix_val(t, fix_obj, 54, getpval(doj, ROM_STASH_SIDE), getplen(doj, ROM_STASH_SIDE));
        }
    } else {
        set_fix_val(t, fix_obj, 54, getpval(doj, ROM_SIDE), getplen(doj, ROM_SIDE));
    }
    if(getplen(doj, ROM_SHORT_LEND) > 0&& sectype[0] == 'E') {
        set_fix_val(t, fix_obj, 114, "N", 1);
    }
    set_exec_time(t, fix_obj);
    if(doj->childs != 0) {
        add_spread_legs(t, doj, fix_obj, ci, 0);
    }
    dart_send_fix_message(fix_obj, ci);
    destroy_fixy_message(t, fix_obj);
    return 1;
}
void on_instinet_replace_order(con_interface * ci, dart_order_obj * doj)
{
    trans_t* t = (trans_t*)ci->parser;
    ofp* fix_obj = 0;
    if(doj->childs == 0) {
        fix_obj = get_fixy_message(t,0x47);
    } else { 
        fix_obj = get_fixy_message(t,0x4341);
    } 
    check_and_resize(doj, 12);
    int clr_len = 0;
    char* flipper = get_mpid_for_clr_acct(ci->sbm,
            getpval(doj, ROM_CLR_ACC),
            getplen(doj, ROM_CLR_ACC),
            &clr_len);
    if(clr_len > 0) {
        set_fix_val(t, fix_obj, 115, flipper, clr_len);
    } else {
		set_fix_val(t, fix_obj, 115, "DART", 4);
    }
	set_fix_val(t, fix_obj, 116, getpval(doj,ROM_CLR_ACC), getplen(doj,ROM_CLR_ACC));
    /*Begin*/
    char clordid[32];
    memset(clordid, '\0', 32);
    int cur_size = create_multiday_in_place_ord_token(ci->tg,
            clordid, 12);
    fill_in_rom_field(doj, ROM_PENDING_ID, clordid, cur_size);
    set_fix_val(t, fix_obj, 11, doj->positions[ROM_PENDING_ID].iov_base,
            doj->positions[ROM_PENDING_ID].iov_len);
    /*End*/
    set_fix_val(t, fix_obj, 41, getpval(doj, ROM_CLORDID), getplen(doj, ROM_CLORDID));
    if(getplen(doj, ROM_SYM) > 0) {
        set_fix_val(t, fix_obj, 55, getpval(doj, ROM_SYM), getplen(doj, ROM_SYM));
    }
    set_ex_dest(t, fix_obj, getpval(doj,ROM_COPY_INSTR),
            getplen(doj, ROM_COPY_INSTR));
    if(getplen(doj, ROM_CAP) > 0) {
        char* mmhome = getpval(doj, ROM_MM_HOME);
        int mmlen = getplen(doj, ROM_MM_HOME);
        trans_instinet_cap(t, fix_obj, getpval(doj, ROM_CAP), mmhome, mmlen);
    }
	char* sectype = getpval(doj, ROM_SEC_TYPE);
    if(getplen(doj, ROM_SEC_TYPE) > 0 && sectype[0] == 'E') {
        set_fix_val(t, fix_obj, 167, "CS", 2);
    } else if(doj->childs == 0) {
        if(getplen(doj, ROM_EXPIRE_DATE) > 0) {
            set_fix_val(t, fix_obj, 200, getpval(doj,ROM_EXPIRE_DATE), getplen(doj,ROM_EXPIRE_DATE));
        }
        if(getplen(doj, ROM_P_C) > 0) {
            char* pc = (char*)getpval(doj, ROM_P_C);
            add_single_char_val(t, fix_obj, "201", 3, translate_put_call(pc[0]));
        }
        if(getplen(doj, ROM_STRIKE) > 0) {
            set_fix_val(t, fix_obj, 202, getpval(doj,ROM_STRIKE), getplen(doj,ROM_STRIKE));
        }
        if(getplen(doj, ROM_EXPIRE_DAY) > 0) {
            set_fix_val(t, fix_obj, 205, getpval(doj,ROM_EXPIRE_DAY), getplen(doj,ROM_EXPIRE_DAY));
        }
        set_rom_field(doj, ROM_EXEC_SEC_TYPE,"OPT", 3);
        set_fix_val(t, fix_obj, 167, "OPT", 3);
    } else {
        set_fix_val(t, fix_obj, 167, "MLEG", 4);
    }
    if(is_spread(doj)) {
        if(getplen(doj, ROM_STASH_SIDE) > 0) {
            set_fix_val(t, fix_obj, 54, getpval(doj, ROM_STASH_SIDE), getplen(doj, ROM_STASH_SIDE));
        } else {
            set_rom_field(doj, ROM_STASH_SIDE, getpval(doj, ROM_SIDE), getplen(doj, ROM_SIDE));
            set_fix_val(t, fix_obj, 54, getpval(doj, ROM_STASH_SIDE), getplen(doj, ROM_STASH_SIDE));
        }
    } else {
        set_fix_val(t, fix_obj, 54, getpval(doj, ROM_SIDE), getplen(doj, ROM_SIDE));
    }
    set_fix_val(t, fix_obj, 1, getpval(doj,ROM_CLR_ACC), getplen(doj,ROM_CLR_ACC));
    set_exec_time(t, fix_obj);
    dart_order_obj *rph = (dart_order_obj *) doj->rep;
    if(getplen(rph, ROM_SHARES) > 0) {
        set_fix_val(t, fix_obj, 38, getpval(rph, ROM_SHARES), getplen(rph, ROM_SHARES));
    } else {
        set_fix_val(t, fix_obj, 38, getpval(doj, ROM_SHARES), getplen(doj, ROM_SHARES));
    }
    if(getplen(rph, ROM_MAX_FLOOR) > 0) {
        set_fix_val(t, fix_obj, 111, getpval(rph,ROM_MAX_FLOOR), getplen(rph,ROM_MAX_FLOOR));
    }
    if(getplen(doj, ROM_MIN_QTY) > 0) {
        set_fix_val(t, fix_obj, 110, getpval(doj,ROM_MIN_QTY), getplen(doj,ROM_MIN_QTY));
    }
    if(getplen(rph, ROM_TYPE) > 0) {
        set_fix_val(t, fix_obj, 40, getpval(rph,ROM_TYPE), getplen(rph,ROM_TYPE));
    } else {
        set_fix_val(t, fix_obj, 40, getpval(doj,ROM_TYPE), getplen(doj,ROM_TYPE));
    }
    if(getplen(rph, ROM_PRICE) > 0) {
        set_fix_val(t, fix_obj, 44, getpval(rph,ROM_PRICE), getplen(rph,ROM_PRICE));
    } else if(doj->positions[ROM_PRICE].iov_len > 0){
        set_fix_val(t, fix_obj, 44, getpval(doj,ROM_PRICE), getplen(doj,ROM_PRICE));
    }
    if(getplen(rph, ROM_STOP) > 0) {
        set_fix_val(t, fix_obj, 99, getpval(rph,ROM_STOP), getplen(rph,ROM_STOP));
    }
    if(getplen(rph, ROM_TIF) > 0) {
        set_fix_val(t, fix_obj, 59, getpval(rph,ROM_TIF), getplen(rph,ROM_TIF));
        char* tif = getpval(rph, ROM_TIF);
        if(tif[0] == '1') {
            set_fix_val(t, fix_obj, 18, "H", 1);
        }
    } else {
        char* tif = getpval(doj, ROM_TIF);
        if(tif[0] == '1') {
            set_fix_val(t, fix_obj, 18, "H", 1);
        }
    }
    if(getplen(doj, ROM_O_C) > 0) {
        char* oc_c = (char*)getpval(doj, ROM_O_C);
        add_single_char_val(t, fix_obj, "77",2, translate_open_close(oc_c[0]));
    }
    if(getplen(rph, ROM_EXPIRE_TIME) > 0) {
        set_fix_val(t, fix_obj, 432, getpval(rph,ROM_EXPIRE_TIME),
                getplen(rph,ROM_EXPIRE_TIME));
    } else if(getplen(doj, ROM_EXPIRE_TIME) > 0) { 
        set_fix_val(t, fix_obj, 432, getpval(doj,ROM_EXPIRE_TIME),
                getplen(doj,ROM_EXPIRE_TIME));
    }
    if(getplen(doj, ROM_CMTA) > 0) {
        set_fix_val(t, fix_obj, 439, getpval(doj,ROM_CMTA), getplen(doj,ROM_CMTA));
    }
    if(getplen(doj, ROM_EXEC_INST) >= 1) {
        set_fix_val(t, fix_obj, 18, getpval(doj,ROM_EXEC_INST), getplen(doj,ROM_EXEC_INST));
    }
    if(getplen(doj, ROM_SHORT_LEND) > 0&& sectype[0] == 'E') {
        set_fix_val(t, fix_obj, 114, "N", 1);
    }
    if(doj->childs != 0) {
        add_spread_legs(t, doj, fix_obj, ci, 0);
    }
    dart_send_fix_message(fix_obj, ci);
    destroy_fixy_message(t, fix_obj);
}
static ff_desc* create_instinet_ff_out_array(int* len, char* dir, int d_len)
{
    char* full_path = calloc(1, d_len + 9);
    ff_desc* ffd = calloc(16, sizeof(struct fix_file_desc));
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "NewOrder", 8);
    ffd[0].filename = full_path;
    full_path = calloc(1, d_len + 7);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "Cancel", 6);
    ffd[1].filename = full_path;
    full_path = calloc(1, d_len + 8);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "Replace", 7);
    ffd[2].filename = full_path;
    full_path = calloc(1, d_len + 8);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "GapFill", 7);
    ffd[3].filename = full_path;
    full_path = calloc(1, d_len + 6);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "Logon", 5);
    ffd[4].filename = full_path;
    full_path = calloc(1, d_len + 8);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "Logout", 6);
    ffd[5].filename = full_path;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "ResendReq", 9);
    ffd[6].filename = full_path;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "MLegNew", 7);
    ffd[7].filename = full_path;
    full_path = calloc(1, d_len + 8);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "TestReq", 7);
    ffd[8].filename = full_path;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "MLegReplace", 11);
    ffd[9].filename = full_path;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "Heartbeat", 9);
    ffd[10].filename = full_path;
    *len = 11;
    return ffd;
}

static ff_desc* create_instinet_ff_in_array(int* len, char* dir, int d_len)
{
    char* full_path = calloc(1, d_len + 16);
    ff_desc* ffd = calloc(10, sizeof(struct fix_file_desc));
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomExe", 11);
    ffd[0].filename = full_path;
    ffd[0].fix_type = 0x38;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomCnlRej", 14);
    ffd[1].filename = full_path;
    ffd[1].fix_type = 0x39;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomNORej", 13);
    ffd[2].filename = full_path;
    ffd[2].fix_type = 0x44;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomLogon", 13);
    ffd[3].filename = full_path;
    ffd[3].fix_type = 0x41;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomSecDef", 14);
    ffd[4].filename = full_path;
    ffd[4].fix_type = 0x64;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomMail", 12);
    ffd[5].filename = full_path;
    ffd[5].fix_type = 0x42;
    //New
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomBizRej", 14);
    ffd[6].filename = full_path;
    ffd[6].fix_type = 0x6A;
    //
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomSesRej", 14);
    ffd[7].filename = full_path;
    ffd[7].fix_type = 0x33;
    //done
    *len = 8;
    return ffd;
}
void create_connection(con_interface * con, char *filename, int name_len,
        gatekeeper * gk)
{
    ex_config *ex = init_config(filename, name_len);
    struct init_args *fix_args = load_fix_config(filename, name_len);
    reset_con_interface(con, 8192,
            (char *) ex->mod_name, ex->mod_name_len,
            fix_args->target_comp, fix_args->tc_len,
            send_fix_logon, build_instinet_offsets,
            on_instinet_enter_order, on_instinet_cancel_order,
            on_instinet_replace_order, gk, (char *) ex->dest_id,
            ex->dest_id_len);
    con->client_mess_parse = parse_fix_message;
    con->ci_parse_v = 0;
    con->on_connect = on_connect;
    con->ci_obj_callback= from_instinet;
    con->connection_conf = ex;
    con->can_send_orders = 0;
    con->session_conf = fix_args;
    add_int_orderids(con);
    int ret_len = 0;
    char* directory = get_val_for_tag(ex->dc, "FIX SERVER", 10,
            "directory", 9, &ret_len);
    int out_len = 0;
    int in_len = 0;
    ff_desc* out_types = create_instinet_ff_out_array(&out_len,directory, ret_len);
    ff_desc* in_types = create_instinet_ff_in_array(&in_len,directory, ret_len);
    trans_t* t = create_translator(out_types,out_len,
            in_types, in_len);
    con->parser = t;
    con->tg = token_creation_func(con->dest_id, con->dest_len);
    build_children(con);
    char* flipper = get_val_for_tag(ex->dc, "FIX SERVER", 10,
            "MPID.map", 9, &ret_len);
    if(ret_len <=0 ) {
        con->sbm = create_service_bureau_matcher("MPID.map");
    } else {
        con->sbm = create_service_bureau_matcher(flipper);
    }
}
static void set_time(dart_order_obj* orig, dart_order_obj* doj)
{
    if(getplen(doj, FIX_EXEC_TIME) > 0) {
        fill_in_rom_field(orig, EXEC_TIME, getpval(doj, FIX_EXEC_TIME),
                getplen(doj, FIX_EXEC_TIME));
    }
}
static void handle_ex_report(dart_order_obj* doj, con_interface* ci)
{
    dart_order_obj *orig = 0;
    int found = 0;
    if(doj->positions[FIX_LEG_TAG].iov_len <= 1) {
        found = get_obj_by_id(ci, doj->positions[FIX_CLORDID].iov_base,
                doj->positions[FIX_CLORDID].iov_len, &orig);
        if(found == 0) {
            found = get_obj_by_id(ci, doj->positions[FIX_ORIG_ID].iov_base,
                    doj->positions[FIX_ORIG_ID].iov_len, &orig);
        }
    } else {
        /*This is the first clue we have a spread leg*/
        found = get_obj_by_id(ci, doj->positions[FIX_LEG_TAG].iov_base,
                doj->positions[FIX_LEG_TAG].iov_len, &orig);
        if(found == 0) {
            found = get_obj_by_id(ci, doj->positions[FIX_CLORDID].iov_base,
                    doj->positions[FIX_CLORDID].iov_len, &orig);
        }
    }
    if (found && lock_n_check(orig)) {
        //short status = 0;
        set_time(orig, doj);
        int destroy_orig = 0;
        char *type = doj->positions[FIX_STATUS].iov_base;
        switch(type[0]) {
            case '0':
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "13", 2);
                fill_in_rom_field(orig, ROM_LEAVES,
                        getpval(orig, ROM_SHARES), 
                        getplen(orig, ROM_SHARES));
                if(getplen(doj, FIX_PRICE) > 0) {
                    fill_in_rom_field(doj, ROM_PRICE, getpval(doj, FIX_PRICE),
                            getplen(doj, FIX_PRICE));
                }
                if(getplen(doj, FIX_EXCHANGE_TAG) > 0) {
                    fill_in_rom_field(doj, ROM_EX_TAG, getpval(doj, FIX_EXCHANGE_TAG),
                            getplen(doj, FIX_EXCHANGE_TAG));
                }
                if(is_spread(orig)) {
                    instinet_reset_fields_for_parent(orig, doj, ci, fix_return);
                }
                fix_return(ci, orig);
                break;
            case '1':
            case '2':
                {
                    set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                    set_rom_field(orig, ROM_STATUS, type, 1);
                    int leaves = reset_fields_for_fill(orig, doj, ci, ci->on_fill);
                    if (!is_spread(orig) && leaves == 0) {
                        remove_from_maps(ci, getpval(orig, ROM_CLORDID), 
                                getplen(orig, ROM_CLORDID)); 
                        if(getplen(orig, ROM_PENDING_ID) > 0) {
                            remove_from_maps(ci, getpval(orig, ROM_PENDING_ID), 
                                    getplen(orig, ROM_PENDING_ID));
                        } 
                        destroy_orig = 1;
                    }
                    if(is_spread(orig)) {
                        instinet_reset_fields_for_parent(orig, doj, ci, ci->on_fill);
                    }
                    ci->on_fill(ci, orig);
                }
                break;
            case '3':
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "3", 1);
                if(is_spread(orig)) {
                    instinet_reset_fields_for_parent(orig, doj, ci, fix_return);
                }
                fix_return(ci, orig);
                //done for day
                break;
            case '4':
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "4", 1);
                remove_from_maps(ci, getpval(orig, ROM_CLORDID),
                        getplen(orig, ROM_CLORDID));
                if(getplen(orig, ROM_PENDING_ID) > 0) {
                    remove_from_maps(ci, getpval(orig, ROM_PENDING_ID), 
                            getplen(orig, ROM_PENDING_ID));
                } 
                if(is_spread(orig)) {
                    instinet_reset_fields_for_parent(orig, doj, ci, ci->on_out);
                }
                ci->on_out(ci, orig);
                if(!is_spread(orig)) {
                    destroy_orig = 1;
                }
                break;
            case '5':
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "5", 1);
                if (orig->rep != NULL) {
                    dart_order_obj *rep = (dart_order_obj *) orig->rep;
                    set_rep_fields(orig, rep, ci);
                } else if(orig->rep == NULL && is_spread(orig)) {
                    set_spread_rep_fields(orig, doj, ci);
                }
                if(is_spread(orig)) {
                    instinet_reset_fields_for_parent(orig, doj, ci, ci->on_replaced);
                }
                ci->on_replaced(ci, orig);
                break;
            case '6':
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "6", 1);
                fix_return(ci, orig);
                break;
            case '7':
                //stopped
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "7", 1);
                if(is_spread(orig)) {
                    instinet_reset_fields_for_parent(orig, doj, ci, fix_return);
                }
                fix_return(ci, orig);
                break;
            case '8':
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "8", 1);
                remove_from_maps(ci, getpval(orig, ROM_CLORDID),
                        getplen(orig, ROM_CLORDID)); 
                databuf_t* db = get_databuf(ci->gk);
                clean_text(orig, db, ROM_TEXT, getpval(doj, FIX_TEXT),
                        getplen(doj, FIX_TEXT));
                destroy_gk_db(ci->gk, db);
                if(is_spread(orig)) {
                    instinet_reset_fields_for_parent(orig, doj, ci, ci->on_reject);
                }
                ci->on_reject(ci, orig);
                if(is_spread(orig)) {
                    send_leg_rej_updates(orig, ci);
                }
                destroy_orig = 1;
                break;
            case '9':
                //suspended
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "9", 1);
                if(is_spread(orig)) {
                    instinet_reset_fields_for_parent(orig, doj, ci, fix_return);
                }
                fix_return(ci, orig);
                break;
            case 'A':
                //pending new
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "0", 1);
                if(is_spread(orig)) {
                    instinet_reset_fields_for_parent(orig, doj, ci, fix_return);
                }
                fix_return(ci, orig);
                break;
            case 'B':
                //calculated
                break;
            case 'C':
                //Expired
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "4", 1);
                remove_from_maps(ci, getpval(orig, ROM_CLORDID), 
                        getplen(orig, ROM_CLORDID)); 
                if(is_spread(orig)) {
                    instinet_reset_fields_for_parent(orig, doj, ci, ci->on_out);
                }
                ci->on_out(ci, orig);
                if(!is_spread(orig)) {
                    destroy_orig = 1;
                }
                break;
            case 'D':
                //restarted
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "13", 2);
                if(is_spread(orig)) {
                    instinet_reset_fields_for_parent(orig, doj, ci, fix_return);
                }
                fix_return(ci, orig);
                break;
            case 'E':
                //pending replace
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "21", 2);
                if(is_spread(orig)) {
                    instinet_reset_fields_for_parent(orig, doj, ci, fix_return);
                }
                fix_return(ci, orig);
                break;
        }
        if (orig != NULL) {
            unlock(orig);
            if (destroy_orig) {
                destroy_gk_obj(ci->gk, orig);
            }
        }
    } else {
        if(getplen(doj, FIX_CLORDID) > 0 && getplen(doj, FIX_STATUS)) {
            //jsend_debug_message("Could not find orig order for: %s, status: %s \n",
              //      (char*)getpval(doj, FIX_CLORDID), (char*) getpval(doj, FIX_STATUS));

        }
    }
}

void from_instinet(dart_order_obj * doj, void *a)
{
    async_parse_args *apa = (async_parse_args*) a;
    if (doj && apa) {
        doj->dest = apa->ds_book;
        con_interface *ci = (con_interface *) apa->ds_book;
        switch(doj->type) {
            case 0x38:
                handle_ex_report(doj, ci);
                //execution report
                break;
            case 0x39:
                handle_cnl_rpl_rej(doj, ci);
                //cnl_rep_rej
                break;
            case 0x4A43:
                ci->can_send_orders = 1;
                break;
            default:
                break;
        }
    }
}
