
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
#include "fix_handler.h"
#include "fix_trans.h"
#include "connection_interface.h"
#include "mailer.h"
#include "debug_logger.h"
#include "rom_fix_trans_funcs.h"
#include "globex_trans.h"
#include "globex.h"


void from_globex(dart_order_obj * doj, void *a);


int build_globex_offsets(struct async_parse_args *apa)
{
    //con_interface* con = (con_interface*)apa->ds_book;
    return 0;
}

static int set_firm_id(trans_t* t, dart_order_obj* obj, ofp* fix_obj,
        g_trans_t* gt)
{
    int rej = 0;
    int len = getplen(obj, ROM_CLR_ID);
    char* clr = getpval(obj, ROM_CLR_ID);
    int valid = valid_cl_ord_id(gt,clr, len);
    if(len > 0 && valid) {
        overwrite_fix_val(t, fix_obj,49, clr,len, 3);
    } else {
        rej = 1;
    }
    return rej;
}

void set_rom_to_cme_price(dart_order_obj* doj, int romfield, int rval,databuf_t* fix)
{
    char* p = getpval(doj, romfield);
    int len = getplen(doj, romfield);
    char *dec = memchr(p, '.', len);
    int len_left = 0;
    if (dec) {
        databuf_memcpy(fix, p, dec - p);
        dec += 1;
        len_left = len - ((dec - p));
    } else {
        databuf_memcpy(fix, p, len);
    }
    while (rval > 1) {
        if (len_left > 0) {
            databuf_memcpy(fix, dec, 1);
        } else {
            databuf_memcpy(fix, "0", 1);
        }
        rval = rval / 10;
        if (dec) {
            dec += 1;
            --len_left;
        }
    }
    if (len_left > 0) {
        databuf_memcpy(fix, ".", 1);
        while (len_left > 0) {
            databuf_memcpy(fix, dec, 1);
            dec += 1;
            --len_left;
        }
    }
}
void set_cme_to_rom(dart_order_obj* doj, int romfield, int rval,databuf_t* fix)
{
    char* val = getpval(doj, romfield);
    int len = getplen(doj, romfield);
    char* end = val + (len -1);
    char* dec = memchr(val, '.', len);
    int len_left = len;
    if(dec) {
        do {
            databuf_memcpy(fix, end, 1);
            --end;
            --len_left;
        } while(end - dec > 0);
        --end;
        --len_left;
    }
    while(rval > 1) {
        if(len_left > 0 && *end != '-') {
            databuf_memcpy(fix, end, 1);
            --len_left;
            --end;
        } else {
            databuf_memcpy(fix, "0", 1);
        }
        rval /= 10;
    }
    databuf_memcpy(fix, ".", 1); 
    if(len_left > 0) {
        do {
            databuf_memcpy(fix, end, 1);
            --end;
            --len_left;
        } while (len_left > 0);
    } 
    char *p, *q;
    char c;
    for (p = fix->rd_ptr, q = p + (databuf_unread(fix) - 1); p < q; ++p, --q) {
        c = *p;
        *p = *q;
        *q = c;
    }
}

static void set_globex_fields(con_interface* ci, struct async_parse_args* ap, 
                              ofp* fix_obj, dart_order_obj* doj)
{
    trans_t* t = (trans_t*)ci->parser;
    char* location = get_sender_location(ci->sym_trans,doj->positions[ROM_TRADER].iov_base,
           doj->positions[ROM_TRADER].iov_len);
    if(location != 0x0) {
        overwrite_fix_val(t, fix_obj, 142, location, 5, 0);
    } else {
        overwrite_fix_val(t, fix_obj, 142, "US,IL", 5, 0);
    }
    set_num_val(t, fix_obj, 369, ap->in_seq);
    int cof_len = 0;
    char* cof  =  get_cust_or_firm(ci->sym_trans, &cof_len);
    if(cof_len > 0) {
        set_fix_val(t, fix_obj, 204, cof, cof_len);
    }
    int cti_len = 0;
    char* cti  =  get_cust_type(ci->sym_trans, &cti_len);
    if(cti_len > 0) {
        set_fix_val(t, fix_obj, 9702, cti, cti_len);
    }
}

int on_globex_enter_order(con_interface * ci, dart_order_obj * doj,
        struct async_parse_args *ap)
{
    trans_t* t = (trans_t*)ci->parser;
    ofp* fix_obj = get_fixy_message(t,0x44);
    int should_rej = set_firm_id(t, doj, fix_obj, ci->sym_trans);
    if(should_rej) {
        set_rom_field(doj, 0, ROM_ZERO_STATUS, 1);
        set_rom_field(doj, ROM_STATUS, "8", 1);
        set_rom_field(doj, ROM_TEXT, "Invalid CLRID", 13);
        ci->on_reject(ci, doj);
        return -1;
    }
    set_fix_val(t, fix_obj, 50, getpval(doj,ROM_TRADER), getplen(doj,ROM_TRADER));
    char clorid_db[32];
    memset(clorid_db, '\0', 32);
    int cur_size = create_multiday_all_num_token(ci->tg, ci->seq,
            clorid_db, 12, 9);
    set_rom_field(doj, ROM_CLORDID, clorid_db,
                    cur_size);
    pthread_spin_lock(&ci->mutex);
    no_copy_insert(ci->cl_ord_id_map, doj->positions[ROM_CLORDID].iov_base, 
                   doj->positions[ROM_CLORDID].iov_len,
                   doj);
    pthread_spin_unlock(&ci->mutex);
    set_fix_val(t, fix_obj, 11, clorid_db, cur_size);
    set_rom_field(doj, ROM_ORIG_ID , doj->positions[ROM_CLORDID].iov_base,
            doj->positions[ROM_CLORDID].iov_len);
    set_fix_val(t, fix_obj, 1, getpval(doj,ROM_CLR_ACC), getplen(doj,ROM_CLR_ACC));
    set_fix_val(t, fix_obj, 40, getpval(doj,ROM_TYPE), getplen(doj,ROM_TYPE));
    set_fix_val(t, fix_obj, 55, getpval(doj, ROM_SYM), getplen(doj, ROM_SYM));
    set_fix_val(t, fix_obj, 107, getpval(doj, ROM_SEC_DEF), getplen(doj, ROM_SEC_DEF));
    set_rom_field(doj, ROM_EXCHANGE_SYM, doj->positions[ROM_SEC_DEF].iov_base,
            doj->positions[ROM_SEC_DEF].iov_len);
    fill_in_rom_field(doj, ROM_STASH_SYM, getpval(doj, ROM_SYM), getplen(doj, ROM_SYM));
    if(is_spread(doj)) {
        set_rom_field(doj, ROM_STASH_SIDE, getpval(doj, ROM_SIDE), getplen(doj, ROM_SIDE));
    }
    memset(clorid_db, '\0', cur_size);
    cur_size = trans_sec_type(getpval(doj, ROM_SEC_TYPE), getplen(doj, ROM_SEC_TYPE),
                              clorid_db);
    fill_in_rom_field(doj, ROM_EXEC_SEC_TYPE, clorid_db, cur_size);
    set_fix_val(t, fix_obj, 167, clorid_db, cur_size);
    set_fix_val(t, fix_obj, 54, getpval(doj, ROM_SIDE), getplen(doj, ROM_SIDE));
    set_exec_time(t, fix_obj);
    set_fix_val(t, fix_obj, 38, getpval(doj, ROM_SHARES), getplen(doj, ROM_SHARES));
    if(getplen(doj, ROM_MAX_FLOOR) > 0) {
        set_fix_val(t, fix_obj, 210, getpval(doj,ROM_MAX_FLOOR), getplen(doj,ROM_MAX_FLOOR));
    }
    if(getplen(doj, ROM_MIN_QTY) > 0) {
        set_fix_val(t, fix_obj, 110, getpval(doj,ROM_MIN_QTY), getplen(doj,ROM_MIN_QTY));
    }
    databuf_t *itoa_buff = get_databuf(ci->gk);
    int rval = get_display_factor(ci->sym_trans, getpval(doj, ROM_SEC_DEF),
                                      getplen(doj, ROM_SEC_DEF));
    if(getplen(doj, ROM_PRICE) > 0) {
        set_rom_to_cme_price(doj, ROM_PRICE, rval,itoa_buff);
        set_fix_val(t, fix_obj, 44, itoa_buff->rd_ptr, databuf_unread(itoa_buff));
    }
    databuf_reset(itoa_buff);
    if(getplen(doj,ROM_STOP) > 0) {
        set_rom_to_cme_price(doj, ROM_STOP, rval,itoa_buff);
        set_fix_val(t, fix_obj, 99, itoa_buff->rd_ptr, databuf_unread(itoa_buff));
    }
    if(getplen(doj, ROM_MANUAL) > 0) {
        char* v = getpval(doj, ROM_MANUAL);
        if(v[0] == 'Y') {
            set_fix_val(t, fix_obj, 1028, "N", 1);
        } else {
            set_fix_val(t, fix_obj, 1028, "Y", 1);
        }
    } else {
        set_fix_val(t, fix_obj, 1028, "N", 1);
    }
    if(getplen(doj, ROM_DESC) > 0) {
        set_fix_val(t, fix_obj, 7928, getpval(doj, ROM_DESC),
                    getplen(doj, ROM_DESC));
    }
    set_fix_val(t, fix_obj, 59, getpval(doj,ROM_TIF), getplen(doj,ROM_TIF));
    set_globex_fields(ci, ap, fix_obj, doj);
    dart_send_fix_message(fix_obj, ci);
    destroy_fixy_message(t, fix_obj);
    destroy_gk_db(ci->gk, itoa_buff);
    return 1;
}

int on_globex_cancel_order(con_interface * ci, dart_order_obj * doj)
{
    trans_t* t = (trans_t*)ci->parser;
    ofp* fix_obj = get_fixy_message(t,0x46);
    char clorid_db[32];
    memset(clorid_db, '\0', 32);
    int cur_size = create_multiday_all_num_token(ci->tg, ci->seq,
            clorid_db, 12, 9);
    set_rom_field(doj, ROM_PENDING_ID, clorid_db,
                    cur_size);
    set_fix_val(t, fix_obj, 11, clorid_db, cur_size);
    memset(clorid_db, '\0',cur_size);
    set_firm_id(t, doj, fix_obj, ci->sym_trans);
    set_fix_val(t, fix_obj, 50, getpval(doj,ROM_TRADER), getplen(doj,ROM_TRADER));
    set_fix_val(t, fix_obj, 41, getpval(doj, ROM_CLORDID), getplen(doj, ROM_CLORDID));
    if(doj->positions[ROM_EXCHANGE_SYM].iov_len > 0) {
        set_fix_val(t, fix_obj, 107, getpval(doj, ROM_EXCHANGE_SYM), getplen(doj, ROM_EXCHANGE_SYM));
    } else {
        set_fix_val(t, fix_obj, 107, getpval(doj, ROM_SEC_DEF), getplen(doj, ROM_SEC_DEF));
    }
    if(doj->positions[ROM_EXEC_SEC_TYPE].iov_len > 0) {
        set_fix_val(t, fix_obj, 167, getpval(doj, ROM_EXEC_SEC_TYPE), getplen(doj, ROM_EXEC_SEC_TYPE));
    } else {
        cur_size = trans_sec_type(getpval(doj, ROM_SEC_TYPE), getplen(doj, ROM_SEC_TYPE),
                                  clorid_db);
        set_rom_field(doj, ROM_EXEC_SEC_TYPE, clorid_db, cur_size);
        set_fix_val(t, fix_obj, 167, clorid_db, cur_size);
        memset(clorid_db, '\0', cur_size);
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
    set_fix_val(t, fix_obj, 37, getpval(doj,ROM_EX_TAG), getplen(doj,ROM_EX_TAG));
    set_fix_val(t, fix_obj, 9717, getpval(doj, ROM_ORIG_ID), getplen(doj, ROM_ORIG_ID));
    set_exec_time(t, fix_obj);
    if(getplen(doj, ROM_MANUAL) > 0) {
        char* v = getpval(doj, ROM_MANUAL);
        if(v[0] == 'Y') {
            set_fix_val(t, fix_obj, 1028, "N", 1);
        } else {
            set_fix_val(t, fix_obj, 1028, "Y", 1);
        }
    } else {
        set_fix_val(t, fix_obj, 1028, "N", 1);
    }
    set_globex_fields(ci, ci->ap, fix_obj, doj);
    dart_send_fix_message(fix_obj, ci);
    destroy_fixy_message(t, fix_obj);
    return 1;
}

void on_globex_replace_order(con_interface * ci, dart_order_obj * doj)
{
    trans_t* t = (trans_t*)ci->parser;
    ofp* fix_obj = get_fixy_message(t,0x47);
    char clorid_db[32];
    memset(clorid_db, '\0', 32);
    int cur_size = create_multiday_all_num_token(ci->tg, ci->seq,
            clorid_db, 12, 9);
    set_rom_field(doj, ROM_PENDING_ID, clorid_db,
                    cur_size);
    set_fix_val(t, fix_obj, 11, clorid_db, cur_size);
    set_firm_id(t, doj, fix_obj, ci->sym_trans);
    set_fix_val(t, fix_obj, 41, getpval(doj, ROM_CLORDID), getplen(doj, ROM_CLORDID));
    if(doj->positions[ROM_SYM].iov_len > 0) {
        set_fix_val(t, fix_obj, 55, getpval(doj, ROM_SYM), getplen(doj, ROM_SYM));
    } else if(doj->positions[ROM_SEC_DEF].iov_len > 0) {
        set_fix_val(t, fix_obj, 55, getpval(doj, ROM_SEC_DEF), getplen(doj, ROM_SEC_DEF));
    } else if(doj->positions[ROM_EXCHANGE_SYM].iov_len > 0) {
        set_fix_val(t, fix_obj, 55, getpval(doj, ROM_EXCHANGE_SYM), getplen(doj, ROM_EXCHANGE_SYM));
    }
    set_fix_val(t, fix_obj, 1, getpval(doj,ROM_CLR_ACC), getplen(doj,ROM_CLR_ACC));
    if(doj->positions[ROM_EXCHANGE_SYM].iov_len > 0) {
        set_fix_val(t, fix_obj, 107, getpval(doj, ROM_EXCHANGE_SYM), getplen(doj, ROM_EXCHANGE_SYM));
    } else {
        set_fix_val(t, fix_obj, 107, getpval(doj, ROM_SEC_DEF), getplen(doj, ROM_SEC_DEF));
    }
    if(doj->positions[ROM_EXEC_SEC_TYPE].iov_len > 0) {
        set_fix_val(t, fix_obj, 167, getpval(doj, ROM_EXEC_SEC_TYPE), getplen(doj, ROM_EXEC_SEC_TYPE));
    } else {
        memset(clorid_db, '\0', cur_size);
        cur_size = trans_sec_type(getpval(doj, ROM_SEC_TYPE), getplen(doj, ROM_SEC_TYPE),
                                  clorid_db);
        set_rom_field(doj, ROM_EXEC_SEC_TYPE, clorid_db, cur_size);
        set_fix_val(t, fix_obj, 167, clorid_db, cur_size);
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
    set_fix_val(t, fix_obj, 50, getpval(doj,ROM_TRADER), getplen(doj,ROM_TRADER));
    set_fix_val(t, fix_obj, 37, getpval(doj,ROM_EX_TAG), getplen(doj,ROM_EX_TAG));
    set_fix_val(t, fix_obj, 9717, getpval(doj, ROM_ORIG_ID), getplen(doj, ROM_ORIG_ID));
    set_exec_time(t, fix_obj);
    dart_order_obj *rph = (dart_order_obj *) doj->rep;
    if(getplen(rph, ROM_SHARES) > 0) {
        set_fix_val(t, fix_obj, 38, getpval(rph, ROM_SHARES), getplen(rph, ROM_SHARES));
    } else {
        set_fix_val(t, fix_obj, 38, getpval(doj, ROM_SHARES), getplen(doj, ROM_SHARES));
    }
    if(getplen(rph, ROM_MAX_FLOOR) > 0) {
        set_fix_val(t, fix_obj, 210, getpval(rph,ROM_MAX_FLOOR), getplen(rph,ROM_MAX_FLOOR));
    }
    if(getplen(rph, ROM_TYPE) > 0) {
        set_fix_val(t, fix_obj, 40, getpval(rph,ROM_TYPE), getplen(rph,ROM_TYPE));
    } else {
        set_fix_val(t, fix_obj, 40, getpval(doj,ROM_TYPE), getplen(doj,ROM_TYPE));
    }
    databuf_t *itoa_buff = get_databuf(ci->gk);
    int rval = get_display_factor(ci->sym_trans, getpval(doj, ROM_SEC_DEF),
                                      getplen(doj, ROM_SEC_DEF));
    if(getplen(rph, ROM_PRICE) > 0) {
        set_rom_to_cme_price(rph, ROM_PRICE, rval,itoa_buff);
        set_fix_val(t, fix_obj, 44, itoa_buff->rd_ptr, databuf_unread(itoa_buff));
    } else {
        set_rom_to_cme_price(doj, ROM_PRICE, rval,itoa_buff);
        set_fix_val(t, fix_obj, 44, itoa_buff->rd_ptr, databuf_unread(itoa_buff));
    }
    databuf_reset(itoa_buff);
    if(getplen(rph, ROM_STOP) > 0) {
        set_rom_to_cme_price(rph, ROM_STOP, rval,itoa_buff);
        set_fix_val(t, fix_obj, 99, itoa_buff->rd_ptr, databuf_unread(itoa_buff));
    }
    if(getplen(rph, ROM_TIF) > 0) {
        set_fix_val(t, fix_obj, 59, getpval(rph,ROM_TIF), getplen(rph,ROM_TIF));
    }
    if(getplen(rph, ROM_MAX_FLOOR) > 0) {
        set_fix_val(t, fix_obj, 210, getpval(rph,ROM_MAX_FLOOR), getplen(rph,ROM_MAX_FLOOR));
    }
    if(getplen(rph, ROM_MIN_QTY) > 0) {
        set_fix_val(t, fix_obj, 110, getpval(rph,ROM_MIN_QTY), getplen(rph,ROM_MIN_QTY));
    }
    if(getplen(doj, ROM_MANUAL) > 0) {
        char* v = getpval(doj, ROM_MANUAL);
        if(v[0] == 'Y') {
            set_fix_val(t, fix_obj, 1028, "N", 1);
        } else {
            set_fix_val(t, fix_obj, 1028, "Y", 1);
        }
    } else {
        set_fix_val(t, fix_obj, 1028, "N", 1);
    }
    if(getplen(doj, ROM_DESC) > 0) {
        set_fix_val(t, fix_obj, 7928, getpval(doj, ROM_DESC),
                    getplen(doj, ROM_DESC));
    }
    set_globex_fields(ci, ci->ap, fix_obj, doj);
    dart_send_fix_message(fix_obj, ci);
    destroy_fixy_message(t, fix_obj);
}

static void globex_on_connect(struct connection_interface *ci, int is_connected)
{
    on_connect(ci, is_connected);
}
static ff_desc* create_globex_ff_out_array(int* len, char* dir, int d_len)
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
    memcpy(full_path + d_len, "SecDefReq", 9);
    ffd[7].filename = full_path;
    full_path = calloc(1, d_len + 8);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "TestReq", 7);
    ffd[8].filename = full_path;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "TraderLogon", 11);
    ffd[9].filename = full_path;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "Heartbeat", 9);
    ffd[10].filename = full_path;
    *len = 11;
    return ffd;
}

static ff_desc* create_globex_ff_in_array(int* len, char* dir, int d_len)
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

void glbx_reset_n_reload(struct connection_interface *con, int out, int in)
{
    send_debug_message("Reload called in Globex: out: %d, in: %d \n",out, in);
    if(out == 0 && in == 0) {
        reload_display_factors(con->sym_trans);
    }
}

void create_connection(con_interface * con, char *filename, int name_len,
        gatekeeper * gk)
{

    ex_config *ex = init_config(filename, name_len);
    struct init_args *fix_args = load_fix_config(filename, name_len);
    reset_con_interface(con, 8192,
            (char *) ex->mod_name, ex->mod_name_len,
            fix_args->target_comp, fix_args->tc_len,
            send_fix_logon, build_globex_offsets,
            on_globex_enter_order, on_globex_cancel_order,
            on_globex_replace_order, gk, (char *) ex->dest_id,
            ex->dest_id_len);
    con->client_mess_parse = parse_fix_message;
    con->ci_parse_v = 0;
    con->on_connect = globex_on_connect;
    con->ci_obj_callback= from_globex;
    con->connection_conf = ex;
    con->can_send_orders = 0;
    con->session_conf = fix_args;
    con->reset_seq = glbx_reset_n_reload;
    add_int_orderids(con);
    int ret_len = 0;
    char* directory = get_val_for_tag(ex->dc, "FIX SERVER", 10,
            "directory", 9, &ret_len);
    int out_len = 0;
    int in_len = 0;
    ff_desc* out_types = create_globex_ff_out_array(&out_len,directory, ret_len);
    ff_desc* in_types = create_globex_ff_in_array(&in_len,directory, ret_len);

    trans_t* t = create_translator(out_types,out_len,
            in_types, in_len);
    con->parser = t;
    con->tg = generic_creation_func();
    con->sym_trans = create_globex_trans(directory, ret_len, ex);
}

void build_fill_not_found_mail(con_interface * ob, 
        dart_order_obj* obj)
{
    char* title = calloc(1,50);
    int t_len = 0;
    if(getplen(obj, FIX_EXE_TYPE) > 0) {
        char* t = (char*)getpval(obj, FIX_EXE_TYPE);
        if(t[0] == '1') {
            memcpy(title, "Bust from Globex", 13);
            t_len = 13;
        } else {
            memcpy(title, "Correction from Globex", 19);
            t_len = 19;
        }
    } else {
        memcpy(title, "Unmatched Fill from Globex", 23);
        t_len = 23;
    }
    databuf_t* cur_off = databuf_alloc(200);
    if(getplen(obj, FIX_CLORDID) > 0) {
        databuf_memcpy(cur_off, "CLORDID: ", 9); 
        databuf_memcpy(cur_off, getpval(obj, FIX_CLORDID),
                getplen(obj, FIX_CLORDID));
        databuf_memcpy(cur_off, "\n", 1);
    }
    if(getplen(obj, FIX_LAST_SHARES) > 0) {
        databuf_memcpy(cur_off, "Last Shares: ", 13);
        databuf_memcpy(cur_off, getpval(obj, FIX_LAST_SHARES), 
                        getplen(obj, FIX_LAST_SHARES));
        databuf_memcpy(cur_off, "\n", 1);
    }
   
    if(getplen(obj, FIX_EXEC_PRICE) > 0) {
        databuf_memcpy(cur_off, "Last Price: ", 12);
        databuf_memcpy(cur_off, getpval(obj, FIX_EXEC_PRICE),
                        getplen(obj, FIX_EXEC_PRICE));
        databuf_memcpy(cur_off, "\n", 1);
    }
    send_biz_message(cur_off->buffer, databuf_unread(cur_off), title, t_len);
    databuf_destroy(cur_off);
}

static int globex_reset_fields_for_fill(dart_order_obj * orig,
        dart_order_obj * doj, con_interface* ob)
{
    
    fill_in_rom_field(orig, ROM_LAST_SHARES,
            doj->positions[FIX_LAST_SHARES].iov_base,
            doj->positions[FIX_LAST_SHARES].iov_len);
    fill_in_rom_field(orig, ROM_EXEC_PRICE,
            doj->positions[FIX_EXEC_PRICE].iov_base,
            doj->positions[FIX_EXEC_PRICE].iov_len);
    if(getplen(doj, FIX_EXEC_TAG) > 0) {
        fill_in_rom_field(orig, EXEC_ID,
                doj->positions[FIX_EXEC_TAG].iov_base,
                doj->positions[FIX_EXEC_TAG].iov_len);
    }
    if(getplen(doj, FIX_EXEC_TIME) > 0) {
        fill_in_rom_field(orig, EXEC_TIME,
                doj->positions[FIX_EXEC_TIME].iov_base,
                doj->positions[FIX_EXEC_TIME].iov_len);
    }
    if(getplen(doj, FIX_LIQ_FLAG) > 0) {
        fill_in_rom_field(orig, ROM_LIQ,
                doj->positions[FIX_LIQ_FLAG].iov_base,
                doj->positions[FIX_LIQ_FLAG].iov_len);
    }
    
    databuf_t *itoa_buff = get_databuf(ob->gk);
    int rval = get_display_factor(ob->sym_trans, getpval(orig, ROM_SEC_DEF),
                                      getplen(orig, ROM_SEC_DEF));
    set_cme_to_rom(doj, FIX_EXEC_PRICE, rval,itoa_buff);
    fill_in_rom_field(orig, ROM_EXEC_PRICE,itoa_buff->rd_ptr,
            databuf_unread(itoa_buff));
    int leaves = 0;
    /**
     * Right now the ICE only sends 442 on leg partials and fills
     */
    if(is_spread(orig)) {
        
        fill_in_rom_field(orig, ROM_SEC_DEF,doj->positions[FIX_SEC_DEF].iov_base,
                doj->positions[FIX_SEC_DEF].iov_len);
        fill_in_rom_field(orig, ROM_SYM,doj->positions[FIX_SEC_DEF].iov_base, 
                doj->positions[FIX_SEC_DEF].iov_len);
        if(doj->positions[FIX_SEC_TYPE].iov_len > 0) {
            databuf_t *itoa_buff = get_databuf(ob->gk);
            int slen = trans_sec_type(doj->positions[FIX_SEC_TYPE].iov_base,
                    doj->positions[FIX_SEC_TYPE].iov_len, itoa_buff->wr_ptr);
            fill_in_rom_field(orig, ROM_SEC_TYPE,itoa_buff->rd_ptr, slen);
            destroy_gk_db(ob->gk, itoa_buff);
        }
        if(getplen(doj, FIX_SIDE) > 0) {
            fill_in_rom_field(orig, ROM_SIDE, getpval(doj, FIX_SIDE), getplen(doj, FIX_SIDE));
        }
        char* lt = getpval(doj, FIX_LEG_TYPE);
        if(getplen(doj, FIX_LEG_TYPE) > 0 && lt[0] == '2') {
            set_rom_field(orig, ROM_SPREAD, "6", 1);
            reset_rom_field(orig, ROM_EXPIRE_DATE, "", 0);
            fill_in_rom_field(orig, ROM_CUM,
                    doj->positions[FIX_CUM].iov_base,
                    doj->positions[FIX_CUM].iov_len);
        } else {
            int qty = 0;
            int ls = strtol(doj->positions[FIX_LAST_SHARES].iov_base, NULL, 10);
            qty = strtol(orig->positions[ROM_SHARES].iov_base, NULL, 10);
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
            set_rom_field(orig, ROM_SPREAD, "5", 1);
            leaves = qty - cum;
            databuf_t *itoa_buff = get_databuf(ob->gk);
            dart_itoa(orig, leaves, ROM_LEAVES, itoa_buff);
            dart_itoa(orig, cum, ROM_CUM, itoa_buff);
            destroy_gk_db(ob->gk, itoa_buff);
            if(orig->positions[ROM_EXCHANGE_SYM].iov_len <= 0) {
                fill_in_rom_field(orig, ROM_EXCHANGE_SYM,doj->positions[FIX_SEC_DEF].iov_base,
                doj->positions[FIX_SEC_DEF].iov_len);
            }
            if(orig->positions[ROM_STASH_SIDE].iov_len <= 0) {
                fill_in_rom_field(orig, ROM_STASH_SIDE,doj->positions[FIX_SIDE].iov_base,
                doj->positions[FIX_SIDE].iov_len);
            }
        }
    } else {
        int qty = 0;
        int ls = strtol(doj->positions[FIX_LAST_SHARES].iov_base, NULL, 10);
        qty = strtol(orig->positions[ROM_SHARES].iov_base, NULL, 10);
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
        leaves = qty - cum;
        databuf_t *itoa_buff = get_databuf(ob->gk);
        dart_itoa(orig, leaves, ROM_LEAVES, itoa_buff);
        dart_itoa(orig, cum, ROM_CUM, itoa_buff);
        destroy_gk_db(ob->gk, itoa_buff);
    }
    return leaves;
}

static void reset_fields_for_parent(dart_order_obj* orig)
{
    fill_in_rom_field(orig, ROM_SYM, getpval(orig, ROM_STASH_SYM),
                      getplen(orig, ROM_STASH_SYM));
    set_rom_field(orig, ROM_SPREAD, "5", 1);
}

static void handle_ex_report(dart_order_obj* doj, con_interface* ci)
{
    dart_order_obj *orig = 0;
    int found = get_obj_by_id(ci, doj->positions[FIX_CLORDID].iov_base,
            doj->positions[FIX_CLORDID].iov_len, &orig);
    if(found == 0) {
        found = get_obj_by_id(ci, doj->positions[FIX_ORIG_ID].iov_base,
                              doj->positions[FIX_ORIG_ID].iov_len, &orig);
    }
    if (found && lock_n_check(orig)) {
        if(doj->positions[FIX_EXEC_TIME].iov_len > 0) {
            set_rom_field(orig, EXEC_TIME, getpval(doj, FIX_EXEC_TIME),
                    getplen(doj, FIX_EXEC_TIME));
        }
        //short status = 0;
        int destroy_orig = 0;
        char *type = doj->positions[FIX_STATUS].iov_base;
        switch(type[0]) {
            case '0':
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "13", 2);
                set_rom_field(orig, ROM_LEAVES,
                        getpval(orig, ROM_SHARES), 
                        getplen(orig, ROM_SHARES));
                if(getplen(doj, FIX_EXCHANGE_TAG) > 0) {
                    fill_in_rom_field(orig, ROM_EX_TAG, getpval(doj, FIX_EXCHANGE_TAG),
                            getplen(doj, FIX_EXCHANGE_TAG));
                }
                fix_return(ci, orig);
                break;
            case '1':
            case '2':
                {
                    set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                    set_rom_field(orig, ROM_STATUS, type, 1);
                    int leaves = globex_reset_fields_for_fill(orig, doj, ci);
                    if (!is_spread(orig) && leaves == 0) {
                        remove_from_maps(ci, getpval(orig, ROM_CLORDID), 
                                getplen(orig, ROM_CLORDID)); 
                        if(getplen(orig, ROM_PENDING_ID) > 0) {
                            remove_from_maps(ci, getpval(orig, ROM_PENDING_ID), 
                                    getplen(orig, ROM_PENDING_ID));
                        } 
                        destroy_orig = 1;
                    }
                    ci->on_fill(ci, orig);
                }
                break;
            case '3':
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "3", 1);
                if(is_spread(orig)) {
                    reset_fields_for_parent(orig);
                }
                fix_return(ci, orig);
                //done for day
                break;
            case '4':
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "4", 1);
                if(is_spread(orig)) {
                    reset_fields_for_parent(orig);
                }
                remove_from_maps(ci, getpval(orig, ROM_CLORDID),
                        getplen(orig, ROM_CLORDID));
                if(getplen(orig, ROM_PENDING_ID) > 0) {
                    remove_from_maps(ci, getpval(orig, ROM_PENDING_ID), 
                            getplen(orig, ROM_PENDING_ID));
                } 
                ci->on_out(ci, orig);
                destroy_orig = 1;
                break;
            case '5':
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "5", 1);
                if(is_spread(orig)) {
                    reset_fields_for_parent(orig);
                }
                if (orig->rep != NULL) {
                    dart_order_obj *rep = (dart_order_obj *) orig->rep;
                    set_rep_fields(orig, rep, ci);
                } 
                ci->on_replaced(ci, orig);
                break;
            case '6':
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "6", 1);
                if(is_spread(orig)) {
                    reset_fields_for_parent(orig);
                }
                fix_return(ci, orig);
                break;
            case '7':
                //stopped
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "7", 1);
                if(is_spread(orig)) {
                    reset_fields_for_parent(orig);
                }
                fix_return(ci, orig);
                break;
            case '8':
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "8", 1);
                remove_from_maps(ci, getpval(orig, ROM_CLORDID),
                        getplen(orig, ROM_CLORDID)); 
                {
                    databuf_t *out_mess = get_databuf(ci->gk);
                    clean_text(orig, out_mess, 
                                ROM_TEXT, getpval(doj, FIX_TEXT),
                                getplen(doj, FIX_TEXT));
                    destroy_gk_db(ci->gk, out_mess);
                }
                ci->on_reject(ci, orig);
                destroy_orig = 1;
                break;
            case '9':
                //suspended
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "9", 1);
                if(is_spread(orig)) {
                    reset_fields_for_parent(orig);
                }
                fix_return(ci, orig);
                break;
            case 'A':
                //pending new
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "0", 1);
                fix_return(ci, orig);
                break;
            case 'B':
                //calculated
                break;
            case 'C':
                //Expired
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "4", 1);
                if(is_spread(orig)) {
                    reset_fields_for_parent(orig);
                }
                remove_from_maps(ci, getpval(orig, ROM_CLORDID), 
                        getplen(orig, ROM_CLORDID)); 
                ci->on_out(ci, orig);
                destroy_orig = 1;
                break;
            case 'D':
                //restarted
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "13", 2);
                fix_return(ci, orig);
                break;
            case 'E':
                //pending replace
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "21", 2);
                if(is_spread(orig)) {
                    reset_fields_for_parent(orig);
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
            send_debug_message("Could not find orig order for: %s, status: %s \n",
                    (char*)getpval(doj, FIX_CLORDID), (char*) getpval(doj, FIX_STATUS));
        
        }
        char* status = getpval(doj, FIX_STATUS);
        if(status[0] == '1' || status[0] == '2') {
            build_fill_not_found_mail(ci, doj);
        }
    }
}

static void handle_email_req(dart_order_obj* doj, con_interface* con)
{
    send_biz_message(getpval(doj, 26), getplen(doj, 26), "News Flash from GSET!",
                     24);
}

void from_globex(dart_order_obj * doj, void *a)
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
            case 0x64:
                //sec def response
                break;
            case 0x42:
                handle_email_req(doj, ci);
                break;
            default:
                break;
        }
    }
}
