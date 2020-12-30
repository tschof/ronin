
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
#include "mailer.h"
#include "parser_func.h"
#include "rex_constants.h"
#include "config_loader.h"
#include "ex_config_helper.h"
#include "balance_checker.h"
#include "fix_handler.h"
#include "fix_trans.h"
#include "fix_parser.h"
#include "fix_parser_factory.h"
#include "polling_server.h"
#include "rom_fix_trans_funcs.h"
#include "connection_factory.h"


void from_fixdma(void* a, dart_order_obj * doj);


static void fixdma_parse_oats(void* p, char* d, int dlen, dart_order_obj* doj)
{
    con_int* ci = (con_int*)p;
    fixp* fp = (fixp*)ci->cparser;
    con_int* oat = get_compliance_con(ci->gk);
    dartp* dp = (dartp*)oat->cparser;
    doj->dest = dp;
    no_create_copy_trans(fp, d, dlen, doj,0x38);
    if(doj->positions[0].iov_len == 0 && 
            doj->positions[148].iov_len > 0) {
        char* echo = doj->positions[148].iov_base;
        int elen = doj->positions[148].iov_len;
        if(elen > 0) {
            char* status = translate_pending_status(echo[0]);
            set_rom_field(doj, 0, status, 1);
            char* fstat = doj->positions[28].iov_base;
            int flen = doj->positions[28].iov_len;
            int ret_len = 0;
            if(flen > 0) {
                char* rstat = get_rom_status_for_fix(fstat, flen, &ret_len);
                set_rom_field(doj, ROM_STATUS, rstat, ret_len);
            }
        }
    }
    set_defaults_for_dma(doj, ci->ex);
}

int on_fixdma_enter_order(con_int * ci, rcntr * risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    fixp* fp = (fixp*)ci->cparser;
    trans_t* t = get_trans(fp);
    ofp* fix_obj = 0;
    fix_obj = get_message_for_type(fp,0x44);
    get_cp_len_off(doj, fix_obj);
    fp_send_fix_message(fix_obj, fp);
    destroy_fixy_message(t, fix_obj);
    return 1;
}

int on_fixdma_cancel_order(con_int * ci, rcntr * risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    fixp* fp = (fixp*)ci->cparser;
    trans_t* t = get_trans(fp);
    ofp* fix_obj = get_fixy_message(t,0x46);
    get_cp_len_off(doj, fix_obj);
    fp_send_fix_message(fix_obj, fp);
    destroy_fixy_message(t, fix_obj);
    return 1;
}

int on_fixdma_replace_order(con_int * ci, rcntr * risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    fixp* fp = (fixp*)ci->cparser;
    trans_t* t = get_trans(fp);
    ofp* fix_obj = 0;
    fix_obj = get_fixy_message(t,0x47);
    get_cp_len_off(doj, fix_obj);
    fp_send_fix_message(fix_obj, fp);
    destroy_fixy_message(t, fix_obj);
    return 1;
}

static void fixdma_logout(con_int* ci)
{
    fixp* fp = (fixp*)ci->cparser;
    send_logout(fp, "BYE", 3);
    ci->rc->disconnect = 1;
    ci->can_send_orders = 0;
    ci->connected = 0;
}

static void fixdma_on_connect(void *v, int is_connected, dart_order_obj* doj)
{
    fixp* fp = (fixp*)v;
    con_int* ci = get_fs_pass_back_pointer(fp);
    set_fix_parser_con(fp, is_connected);
    switch(is_connected) {
        case 0:
            ci->can_send_orders = 0;
            ci->connected = 0;
            reload_config(ci->ex);
            ci->rc->disconnect = 0;
            fix_reconnect((fixp*)ci->cparser);
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
    ci->update_oats(ci, is_connected);
}
static ff_desc* create_fixdma_ff_out_array(int* len, char* dir, int d_len)
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

static ff_desc* create_fixdma_ff_in_array(int* len, char* dir, int d_len)
{
    char* full_path = calloc(1, d_len + 16);
    ff_desc* ffd = calloc(16, sizeof(struct fix_file_desc));
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
    ffd[2].fix_type = 0x14;
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
    //done
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomSesRej", 14);
    ffd[7].filename = full_path;
    ffd[7].fix_type = 0x33;
    //New
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomNew", 11);
    ffd[8].filename = full_path;
    ffd[8].fix_type = 0x44;
    //done
    //Cancel
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomCnl", 11);
    ffd[9].filename = full_path;
    ffd[9].fix_type = 0x46;
    //done
    //Replace
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomRpl", 11);
    ffd[10].filename = full_path;
    ffd[10].fix_type = 0x47;
    //done
    *len = 11;
    return ffd;
}
static void fix_on_fill(con_int* ci, rcntr* risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    int tlen = 0;
    char* trader = get_config_val(ci->ex, "SERVER", 6, 
                              "Trader", 6, &tlen);
    set_rom_field(doj, ROM_TRADER, trader, tlen);
    jp_limits_checker *jlc =
        (jp_limits_checker *) get_balance_struct(ci->gk);
    update_balance_fill(jlc, risk);
    ci->on_update(ci, &doj->risk);
}
static void fix_on_rej(con_int* ci, rcntr* risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    int tlen = 0;
    char* trader = get_config_val(ci->ex, "SERVER", 6, 
                              "Trader", 6, &tlen);
    set_rom_field(doj, ROM_TRADER, trader, tlen);
    jp_limits_checker *jlc =
        (jp_limits_checker *) get_balance_struct(ci->gk);
    update_balance(jlc, risk);
    ci->on_rej(ci, &doj->risk);
}
static void fix_on_out(con_int* ci, rcntr* risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    int tlen = 0;
    char* trader = get_config_val(ci->ex, "SERVER", 6, 
                              "Trader", 6, &tlen);
    set_rom_field(doj, ROM_TRADER, trader, tlen);
    jp_limits_checker *jlc =
        (jp_limits_checker *) get_balance_struct(ci->gk);
    update_balance(jlc, risk);
    ci->on_update(ci, &doj->risk);
}

static void fixdma_handle_cnl_rej(con_int* ci, rcntr* risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    int tlen = 0;
    char* trader = get_config_val(ci->ex, "SERVER", 6, 
                              "Trader", 6, &tlen);
    set_rom_field(doj, ROM_TRADER, trader, tlen);
    con_int* sender = 0;
    get_obj_by_client_id(ci->gk, NULL,0, (void**)&sender);        
    if(getplen(doj, FIX_CNL_TYPE) > 0 &&
            ((char*)getpval(doj,FIX_CNL_TYPE))[0] == '2') {
        jp_limits_checker *jlc =
            (jp_limits_checker *) get_balance_struct(ci->gk);
        update_balance(jlc, risk);
    }
    sender->on_rej(sender, &doj->risk); 
    char* r_off = getpval(doj, ROM_ECHO);
    r_off != 0 ? r_off -= 3 : r_off;
    record_dma_message(ci->o, "OATS", 4,  
                       r_off,doj->ex_mess, 
                       fixdma_parse_oats,ci);
}
void create_connection(con_int * con, char *filename, int name_len,
        gatekeeper * gk)
{
    con->cl_ord_id_map = get_initial_order_map(gk, con->mod_name,
                                              con->mod_name_len);
    con->sbm = 0;
    con->on_enter_order = on_fixdma_enter_order;
    con->on_replace_order = on_fixdma_replace_order;
    con->on_cancel_order = on_fixdma_cancel_order;
    con->on_fill = fix_on_fill;
    con->on_rej = fix_on_rej;
    con->on_replaced = 0x0;
    con->on_out = fix_on_out;
    con->can_send_orders = 0;
    con->connected = 0;
    add_int_orderids(con);
    login_info* li = build_logon_info_from_config(con->ex);

    int out_len = 0;
    int in_len = 0;
    ff_desc* out_types = create_fixdma_ff_out_array(&out_len,con->ex->directory,
                                                    con->ex->dir_len);
    ff_desc* in_types = create_fixdma_ff_in_array(&in_len,con->ex->directory,
                                                  con->ex->dir_len);
    trans_t* t = create_translator(out_types,out_len,
                                    in_types, in_len);
    dartfact* d = create_dart_order_factory(con->ex->order_allocation);
    fixp* fp = create_fix_parser(con,
                                 from_fixdma,d, con->o, t,
                                 con->ex->order_allocation);
    set_fp_header_len(fp, 12);
    set_log_info(fp, li);
    rex_con* rc = create_rex_con(con->p, con->ex, fp, fp,
                                 li,fp_write_callback, 
                                 fixdma_on_connect, parse_fix_message, NULL);
    set_rex_con(fp, rc);
    con->cleansing_rinse = fixdma_logout;
    rc->parser = fp;
    con->cparser = fp;
    rc->connect = send_fix_logon;
    con->rc = rc;
    rex_connect(con->p, rc);
}
static void handle_biznes(dart_order_obj* doj, con_int* ci)
{
    set_defaults_for_dma(doj, ci->ex);
    con_int* sender = 0;
    get_obj_by_client_id(ci->gk, NULL,0, (void**)&sender);        
    if(sender) {
        sender->send_to_con(sender, &doj->risk);
    }
    con_int* oat = get_compliance_con(ci->gk);
    oat->on_update(oat, &doj->risk);
}

static void handle_ex_report(dart_order_obj* doj, con_int* ci)
{
        //short status = 0;
        char *type = doj->positions[ROM_STATUS].iov_base;
        con_int* sender = 0;
        get_obj_by_client_id(ci->gk, NULL,0, (void**)&sender);        
        char* r_off = getpval(doj, ROM_ECHO);
        r_off != 0 ? r_off -= 3 : r_off;
//        validate_offsets(doj, r_off, doj->ex_mess);
        if(sender) {
            switch(type[0]) {
                default:
                    sender->on_update(sender, &doj->risk);
                    break;
                case '1':
                case '2':
                case 'F':
                    ci->on_fill(sender, &doj->risk);
                    break;
                case 'C':
                case '4':
                    ci->on_out(sender, &doj->risk);
                    break;
                case '8':
                    ci->on_rej(sender, &doj->risk);
                    break;
            }
        }
        record_dma_message(ci->o, "OATS", 4,  
                       r_off, doj->ex_mess,
                       fixdma_parse_oats,
                       ci);
}
static void handle_email_req(dart_order_obj* doj, con_int* con)
{
    send_biz_message(getpval(doj, 26), getplen(doj, 26), "News Flash from the PHASER!",
                     24);
}

void from_fixdma(void* a, dart_order_obj * doj)
{
    con_int *ci = (con_int*) a;
    if (doj && ci) {
        doj->dest = ci;
        set_rom_field(doj, 0, "S", 1);
        switch(doj->type) {
            case 0x38:
                handle_ex_report(doj, ci);
                //execution report
                break;
            case 0x39:
                fixdma_handle_cnl_rej(ci, &doj->risk);
                break;
            case 0x42:
                handle_email_req(doj, ci);
                break;
            case 0x4A43:
                //Logon Response Positive
                ci->can_send_orders = 1;
                ci->connected = 1;
                break;
            case 0x3:
                handle_biznes(doj, ci);
                break;
            default:
                break;
        }
    }
}
