
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
#include "fix_handler.h"
#include "fix_parser.h"
#include "connection_interface.h"
#include "connection_factory.h"
#include "fix_parser_factory.h"
#include "rom_fix_trans_funcs.h"
#include "mailer.h"


void fs_send(con_int* ci, rcntr* risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    int status_len = doj->positions[ROM_STATUS].iov_len;
    char* status = doj->positions[ROM_STATUS].iov_base;
    fixp* fp = (fixp*)ci->cparser;
    trans_t* t = get_trans(fp);
    ofp* fix_obj = 0;
    if(status_len == 2 && (strncmp(status, "20", 2) == 0 
            || strncmp(status, "14", 2) == 0)) {
        fix_obj = get_fixy_message(t,0x39);
        set_fix_val(fix_obj, 434, getpval(doj, FIX_CNL_TYPE),
                    getplen(doj, FIX_CNL_TYPE));
        set_fix_val(fix_obj, 39, "0",1);
        set_fix_val(fix_obj, 37, "NONE",4);
    } else {
        fix_obj = get_fixy_message(t,0x38);
        if(status_len > 0) {
            set_fix_val(fix_obj, 39, getpval(doj, ROM_STATUS),
                        getplen(doj, ROM_STATUS));
            set_fix_val(fix_obj, 150, getpval(doj, ROM_STATUS),
                        getplen(doj, ROM_STATUS));
        }
    }
    if(doj->positions[ROM_CLORDID].iov_len > 0) {
        set_fix_val(fix_obj, 11, getpval(doj, ROM_CLORDID),
                    getplen(doj, ROM_CLORDID));
    }
    if(doj->positions[ROM_NEW_ID].iov_len > 0) {
        set_fix_val(fix_obj, 41, getpval(doj, ROM_NEW_ID),
                    getplen(doj, ROM_NEW_ID));
    }
    if(doj->positions[ROM_SIDE].iov_len > 0) {
        set_fix_val(fix_obj, 54, getpval(doj,ROM_SIDE),
                    getplen(doj,ROM_SIDE));
    }
    if(doj->positions[ROM_TEXT].iov_len > 0) {
        set_fix_val(fix_obj, 58, getpval(doj,ROM_TEXT),
                    getplen(doj,ROM_TEXT));
    }
    if(getplen(doj, ROM_SYM) > 0) {
        set_fix_val(fix_obj, 55, getpval(doj, ROM_SYM),
                    getplen(doj, ROM_SYM));
    }
    if(getplen(doj, ROM_PRICE) > 0) {
        set_fix_val(fix_obj, 44, getpval(doj,ROM_PRICE),
                    getplen(doj,ROM_PRICE));
    }
    set_exec_time(t, fix_obj);
    fp_send_fix_message(fix_obj, fp);
    destroy_fixy_message(t, fix_obj);
}

static void fs_on_reject(con_int* ci, rcntr* risk) 
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    char* status = doj->positions[ROM_STATUS].iov_base;
    if(status[0] == '8') {
        fixp* fp = (fixp*)ci->cparser;
        trans_t* t = get_trans(fp);
        ofp* fix_obj = get_fixy_message(t,0x38);
        get_cp_len_off(doj, fix_obj);
        fp_send_fix_message(fix_obj, fp);
        destroy_fixy_message(t, fix_obj);
    } else {
        fixp* fp = (fixp*)ci->cparser;
        trans_t* t = get_trans(fp);
        ofp* fix_obj = get_fixy_message(t,0x39);
        get_cp_len_off(doj, fix_obj);
        fp_send_fix_message(fix_obj, fp);
        destroy_fixy_message(t, fix_obj);
    }
}

static void fs_on_update(con_int* ci, rcntr* risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    fixp* fp = (fixp*)ci->cparser;
    trans_t* t = get_trans(fp);
    ofp* fix_obj = get_fixy_message(t,0x38);
    get_cp_len_off(doj, fix_obj);
    fp_send_fix_message(fix_obj, fp);
    destroy_fixy_message(t, fix_obj);
}

void fs_cleansing_rinse(con_int* ci)
{
    if(ci) {
        remove_obj_by_client_id(ci->gk, ci->mod_name, ci->mod_name_len);
        free(ci->mod_name);
        free(ci);
    }
}

void fs_reset_seq(con_int* ci, int out, int in)
{
    /*TODO Add reset to dart_parser*/
}

static void fs_listener_connect(void* pass_back, int is_connected,
                           dart_order_obj* log)
{
    fixp* db = (fixp*)pass_back;
    set_fix_parser_con(db, is_connected);
    con_int* ab = (con_int*)get_fs_pass_back_pointer(db);
    if (is_connected == 1 && log != NULL) {
        /*copy con_int we need a local version for this connection*/
        con_int* ci = clone_interface(ab, log);
        rex_con* rc = get_fs_rex_con(db);
        ci->cparser = db;
        ci->sock = rc->sock;
        ci->is_server = 1;
        set_fs_pass_back_pointer(db, ci);
        ci->rc = rc;
        ci->logon = rc->logon;
        rc->con_pass_back = db;
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
        ci->mod_name = ab->mod_name;
        ci->mod_name_len =ab->mod_name_len;
        ci->update_oats(ci, is_connected);
    } else if(is_connected == 0) {
        ab->update_oats(ab, is_connected);
    }
}
static void fs_parse_oats(void* p, char* d, int dlen, dart_order_obj* doj)
{
    con_int* ci = (con_int*)p;
    fixp* fp = (fixp*)ci->cparser;
    con_int* oat = get_compliance_con(ci->gk);
    dartp* dp = (dartp*)oat->cparser;
    doj->dest = dp;
    no_create_copy_trans(fp, d, dlen, doj,0x44);
    if(doj->positions[148].iov_len > 0) {
        char* echo = doj->positions[148].iov_base;
        int elen = doj->positions[148].iov_len;
        char* status = translate_pending_status(echo[0]);
        set_rom_field(doj, 0, status, 1);
        int ret_len = 0;
        char* rs = get_rom_status_for_fix(echo,elen, &ret_len);
        set_rom_field(doj, ROM_STATUS, rs, ret_len);
    }
    set_defaults_for_dma(doj, ci->ex);
}

static void fs_from_client(void* d, dart_order_obj* doj)
{
    con_int* ci = (con_int*)d;
    fixp* fp = (fixp*)ci->cparser;
    trans_t* t = get_trans(fp);
    ofp* fix_obj = get_fixy_message(t,0x38);
    get_cp_len_off(doj, fix_obj);
    fp_send_fix_message(fix_obj, fp);
    destroy_fixy_message(t, fix_obj);
}

static void fix_checker_callback(void* pbp, dart_order_obj* doj)
{
    con_int* ci = (con_int*)pbp;
    set_rom_field(doj, ROM_TRADER, ci->mod_name, ci->mod_name_len);
    char* r_off = getpval(doj, ROM_ECHO);
    r_off != 0 ? r_off -= 3 : r_off;
    switch(doj->type) {
        case 0x44:
            set_rom_field(doj, 0, "E", 1);
            ci->on_enter_order(ci, &doj->risk);
            break;
        case 0x47:
            set_rom_field(doj, 0, "S", 1);
            ci->on_replace_order(ci, &doj->risk);
            break;
        case 0x46:
            set_rom_field(doj, 0, "S", 1);
            ci->on_cancel_order(ci, &doj->risk);
            break;
        default:
            set_rom_field(doj, 0, "S", 1);
            fs_from_client(ci, doj);
            break;
    }
#ifdef _REDEEMS__
    /*Do nothing Redemtions are not sent to OATS.*/
#else
    record_dma_message(ci->o, "OATS", 4,  
                       r_off, doj->ex_mess,
                       fs_parse_oats,
                       ci);
#endif 
    int rstatuslen = getplen(doj, ROM_STATUS);
    char* rstatus = getpval(doj, ROM_STATUS);
    if(rstatuslen > 0 && (rstatus[0] == '8' ||
                strncmp(rstatus, "20", 2) == 0 ||
                strncmp(rstatus, "14", 2) == 0)) {
        fixp* fp = (fixp*)ci->cparser;
        no_create_trans(fp, r_off,
                            doj->ex_mess,doj, 0x44);
        if(strncmp(rstatus, "14", 2) == 0) {
            set_rom_field(doj, FIX_CNL_TYPE, "1", 1);
        } else {
            set_rom_field(doj, FIX_CNL_TYPE, "2", 1);
        }
        set_rom_field(doj, ROM_STATUS, rstatus, rstatuslen);
        if(doj->positions[ROM_TEXT].iov_len > 0) {
            set_rom_field(doj, ROM_TEXT, doj->positions[ROM_TEXT].iov_base,
                          doj->positions[ROM_TEXT].iov_len);
        }
        fs_send(ci, &doj->risk);
#ifndef _REDEEMS__
        con_int* oat = get_compliance_con(ci->gk);
        set_rom_field(doj, 0, "S", 1);
        set_defaults_for_dma(doj, ci->ex);
        oat->on_update(oat, &doj->risk);
#endif
    }
}

void create_connection(con_int * ci, char *filename, int name_len,
                       gatekeeper * gk)
{

    ci->cl_ord_id_map = get_initial_order_map(gk, ci->mod_name,
                                              ci->mod_name_len);
    ci->sbm = 0;
    ci->tg = 0;
    ci->gk = gk;
    ci->send_to_con = fs_send;
    ci->reset_seq = fs_reset_seq;
    ci->cleansing_rinse = fs_cleansing_rinse;
    ci->can_send_orders = 0;
    ci->on_rej = fs_on_reject;
    ci->on_replaced = fs_on_update;
    ci->on_out = fs_on_update;
    ci->on_fill = fs_on_update;
    ci->on_update = fs_on_update;
    //ci->from_con = fs_from_client;
    add_int_orderids(ci);
    otl* o = get_logger(ci->p);
    fixp* fp = create_fix_parse_block(ci, fix_checker_callback, 
                                      o,ci->ex->directory, ci->ex->dir_len,
                                      ci->ex->order_allocation);
    rex_con* rc = add_listener(ci->p, ci->ex, fp, fp,
                                     fp_write_callback, 
                                     fs_listener_connect,
                                     parse_fix_message,
                                     validate_logon);
    ci->cparser = fp;
    ci->rc = rc;
}

