
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
#include "ice_sym_trans.h"
#include "rom_fix_trans_funcs.h"


static void send_sec_def_reqs(async_parse_args* apa, con_interface* con);
void from_ice(dart_order_obj * doj, void *a);


int build_ice_offsets(struct async_parse_args *apa)
{
    //con_interface* con = (con_interface*)apa->ds_book;
    return 0;
}


static void create_on_behalf(trans_t* t, dart_order_obj* obj, ofp* fix_obj)
{
    int len = getplen(obj, ROM_TRADE_FOR);
    if(len > 0) {
        char on_b_sub[len+len+3];
        memset(on_b_sub, '\0', len+len+3);
        memcpy(on_b_sub, getpval(obj, ROM_TRADE_FOR), len);
        memcpy(on_b_sub + len, "|", 1);
        memcpy(on_b_sub + len+1, getpval(obj, ROM_TRADE_FOR), len);
        fill_in_rom_field(obj, ROM_ON_BEHALF , on_b_sub, len+len+1);
        set_fix_val(t, fix_obj,116, getpval(obj,ROM_ON_BEHALF), getplen(obj,ROM_ON_BEHALF));
    }
}

int on_ice_enter_order(con_interface * ci, dart_order_obj * doj,
        struct async_parse_args *ap)
{
    
    trans_t* t = (trans_t*)ci->parser;
    ofp* fix_obj = get_fixy_message(t,0x44);
    create_on_behalf(t, doj, fix_obj);
    set_fix_val(t, fix_obj, 50, getpval(doj,ROM_TRADE_FOR), getplen(doj,ROM_TRADE_FOR));
        int cur_size = create_multiday_all_num_token(ci->tg, ci->seq,
                doj->current->data->wr_ptr, 12, 9);
    reset_rom_field(doj, ROM_CLORDID, doj->current->data->wr_ptr,
                    cur_size);
    pthread_spin_lock(&ci->mutex);
    no_copy_insert(ci->cl_ord_id_map, doj->current->data->wr_ptr, cur_size,
                   doj);
    pthread_spin_unlock(&ci->mutex);
    set_fix_val(t, fix_obj, 11, doj->current->data->wr_ptr, cur_size);
    doj->current->data->wr_ptr += cur_size;
    ice_sym_t* ist = (ice_sym_t*)ci->sym_trans;
    fill_in_rom_field(doj, ROM_STASH_SYM, getpval(doj, ROM_SYM), getplen(doj, ROM_SYM));
    char* ice_sym = get_ice_sym(ist, getpval(doj, ROM_SEC_DEF), getplen(doj, ROM_SEC_DEF), 
                                  &cur_size);
    if(cur_size > 0) {
        fill_in_rom_field(doj, ROM_EXCHANGE_SYM, ice_sym, cur_size);
        set_fix_val(t, fix_obj, 55, ice_sym, cur_size);
    } else {
        fill_in_rom_field(doj, ROM_EXCHANGE_SYM, getpval(doj, ROM_SEC_DEF),
                          getplen(doj, ROM_SEC_DEF));
        set_fix_val(t, fix_obj, 55, getpval(doj, ROM_SEC_DEF), getplen(doj, ROM_SEC_DEF));
    }
    cur_size = trans_sec_type(getpval(doj, ROM_SEC_TYPE), getplen(doj, ROM_SEC_TYPE),
                              doj->current->data->wr_ptr);
    reset_rom_field(doj, ROM_EXEC_SEC_TYPE, doj->current->data->wr_ptr, cur_size);
    set_fix_val(t, fix_obj, 167, doj->current->data->wr_ptr, cur_size);
    doj->current->data->wr_ptr += cur_size;
    set_fix_val(t, fix_obj, 54, getpval(doj, ROM_SIDE), getplen(doj, ROM_SIDE));
    if(is_spread(doj)) {
        set_rom_field(doj, ROM_STASH_SIDE, getpval(doj, ROM_SIDE), getplen(doj, ROM_SIDE));
    }
    set_mifid_fields(ist, doj, fix_obj,t);
    set_exec_time(t, fix_obj);
    set_fix_val(t, fix_obj, 38, getpval(doj, ROM_SHARES), getplen(doj, ROM_SHARES));
    if(getplen(doj, ROM_MAX_FLOOR) > 0) {
        set_fix_val(t, fix_obj, 210, getpval(doj,ROM_MAX_FLOOR), getplen(doj,ROM_MAX_FLOOR));
    }
    set_fix_val(t, fix_obj, 40, getpval(doj,ROM_TYPE), getplen(doj,ROM_TYPE));
    if(getplen(doj, ROM_PRICE) > 0) {
        set_fix_val(t, fix_obj, 44, getpval(doj,ROM_PRICE), getplen(doj,ROM_PRICE));
    }
    if(getplen(doj,ROM_STOP) > 0) {
        set_fix_val(t, fix_obj, 99, getpval(doj,ROM_STOP), getplen(doj,ROM_STOP));
    }
    set_fix_val(t, fix_obj, 59, getpval(doj,ROM_TIF), getplen(doj,ROM_TIF));
    set_fix_val(t, fix_obj, 440, getpval(doj,ROM_CLR_ACC), getplen(doj,ROM_CLR_ACC));
    set_fix_val(t, fix_obj, 1, getpval(doj,ROM_CLR_ACC), getplen(doj,ROM_CLR_ACC));
    set_fix_val(t, fix_obj, 439, getpval(doj,ROM_CLR_ID), getplen(doj,ROM_CLR_ID));
    /*if(get_is_nybot(ist, getpval(doj, ROM_SEC_DEF), getplen(doj, ROM_SEC_DEF))) {
        set_fix_val(t, fix_obj, 9195,"C",1); 
        set_fix_val(t, fix_obj, 9208,"2",1); 
        set_fix_val(t, fix_obj, 9207, getpval(doj,ROM_CLR_ACC), getplen(doj,ROM_CLR_ACC));
    } */
    dart_send_fix_message(fix_obj, ci);
    destroy_fixy_message(t, fix_obj);
    return 1;
}

int on_ice_cancel_order(con_interface * ci, dart_order_obj * doj)
{
    trans_t* t = (trans_t*)ci->parser;
    ofp* fix_obj = get_fixy_message(t,0x46);
    check_and_resize(doj, 12);
        int cur_size = create_multiday_all_num_token(ci->tg, ci->seq,
                doj->current->data->wr_ptr, 12, 9);

    reset_rom_field(doj, ROM_PENDING_ID, doj->current->data->wr_ptr,
                    cur_size);
    set_fix_val(t, fix_obj, 11, doj->current->data->wr_ptr, cur_size);
    doj->current->data->wr_ptr += cur_size;
    set_fix_val(t, fix_obj, 41, getpval(doj, ROM_CLORDID), getplen(doj, ROM_CLORDID));
    if(getplen(doj, ROM_EXCHANGE_SYM) > 0) {
        set_fix_val(t, fix_obj, 55, getpval(doj, ROM_EXCHANGE_SYM), getplen(doj, ROM_EXCHANGE_SYM));
    } else {
        ice_sym_t* ist = (ice_sym_t*)ci->sym_trans;
        int cur_size = 0;
        fill_in_rom_field(doj, ROM_STASH_SYM, getpval(doj, ROM_SYM), getplen(doj, ROM_SYM));
        char* ice_sym = get_ice_sym(ist, getpval(doj, ROM_SEC_DEF), getplen(doj, ROM_SEC_DEF), 
                                      &cur_size);
        if(cur_size > 0) {
            fill_in_rom_field(doj, ROM_EXCHANGE_SYM, ice_sym, cur_size);
            set_fix_val(t, fix_obj, 55, ice_sym, cur_size);
        } else {
            set_fix_val(t, fix_obj, 55, getpval(doj, ROM_SEC_DEF), getplen(doj, ROM_SEC_DEF));
        }
    }
    ice_sym_t* ist = (ice_sym_t*)ci->sym_trans;
    set_mifid_fields(ist, doj, fix_obj,t);
    if(getplen(doj, ROM_EXEC_SEC_TYPE) > 0) {
        set_fix_val(t, fix_obj, 167, getpval(doj, ROM_EXEC_SEC_TYPE), getplen(doj, ROM_EXEC_SEC_TYPE));
    } else {
        cur_size = trans_sec_type(getpval(doj, ROM_SEC_TYPE), getplen(doj, ROM_SEC_TYPE),
                                  doj->current->data->wr_ptr);
        if(cur_size > 0) {
            reset_rom_field(doj, ROM_EXEC_SEC_TYPE, doj->current->data->wr_ptr, cur_size);
            set_fix_val(t, fix_obj, 167, doj->current->data->wr_ptr, cur_size);
            doj->current->data->wr_ptr += cur_size;
        }
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
    dart_send_fix_message(fix_obj, ci);
    destroy_fixy_message(t, fix_obj);
    return 1;
}

void on_ice_replace_order(con_interface * ci, dart_order_obj * doj)
{
    trans_t* t = (trans_t*)ci->parser;
    ofp* fix_obj = get_fixy_message(t,0x47);
    set_fix_val(t, fix_obj, 50, getpval(doj,ROM_TRADE_FOR), getplen(doj,ROM_TRADE_FOR));
    if(getplen(doj, ROM_ON_BEHALF) > 0) {
        set_fix_val(t, fix_obj, 116, getpval(doj,ROM_ON_BEHALF), getplen(doj,ROM_ON_BEHALF));
    } else {
        create_on_behalf(t, doj, fix_obj);
    }
    check_and_resize(doj, 12);
    int  cur_size = create_multiday_all_num_token(ci->tg, ci->seq,
                doj->current->data->wr_ptr, 12, 9);

    reset_rom_field(doj, ROM_PENDING_ID, doj->current->data->wr_ptr,
                    cur_size);
    set_fix_val(t, fix_obj, 11, doj->current->data->wr_ptr, cur_size);
    doj->current->data->wr_ptr += cur_size;
    set_fix_val(t, fix_obj, 41, getpval(doj, ROM_CLORDID), getplen(doj, ROM_CLORDID));
    ice_sym_t* ist = (ice_sym_t*)ci->sym_trans;
    set_mifid_fields(ist, doj, fix_obj,t);
    if(getplen(doj, ROM_EXCHANGE_SYM) > 0) {
        set_fix_val(t, fix_obj, 55, getpval(doj, ROM_EXCHANGE_SYM), getplen(doj, ROM_EXCHANGE_SYM));
    } else {
        ice_sym_t* ist = (ice_sym_t*)ci->sym_trans;
        int cur_size = 0;
        fill_in_rom_field(doj, ROM_STASH_SYM, getpval(doj, ROM_SYM), getplen(doj, ROM_SYM));
        char* ice_sym = get_ice_sym(ist, getpval(doj, ROM_SEC_DEF), getplen(doj, ROM_SEC_DEF), 
                                      &cur_size);
        if(cur_size > 0) {
            fill_in_rom_field(doj, ROM_EXCHANGE_SYM, ice_sym, cur_size);
            set_fix_val(t, fix_obj, 55, ice_sym, cur_size);
        } else {
            set_fix_val(t, fix_obj, 55, getpval(doj, ROM_SEC_DEF), getplen(doj, ROM_SEC_DEF));
        }
    }
    if(getplen(doj, ROM_EXEC_SEC_TYPE) > 0) {
        set_fix_val(t, fix_obj, 167, getpval(doj, ROM_EXEC_SEC_TYPE), getplen(doj, ROM_EXEC_SEC_TYPE));
    } else {
        cur_size = trans_sec_type(getpval(doj, ROM_SEC_TYPE), getplen(doj, ROM_SEC_TYPE),
                                  doj->current->data->wr_ptr);
        if(cur_size > 0) {
            reset_rom_field(doj, ROM_EXEC_SEC_TYPE, doj->current->data->wr_ptr, cur_size);
            set_fix_val(t, fix_obj, 167, doj->current->data->wr_ptr, cur_size);
            doj->current->data->wr_ptr += cur_size;
        }
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
    set_fix_val(t, fix_obj, 440, getpval(doj,ROM_CLR_ACC), getplen(doj,ROM_CLR_ACC));
    set_fix_val(t, fix_obj, 1, getpval(doj,ROM_CLR_ACC), getplen(doj,ROM_CLR_ACC));
    set_fix_val(t, fix_obj, 439, getpval(doj,ROM_CLR_ID), getplen(doj,ROM_CLR_ID));
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
    }
    /*ice_sym_t* ist = (ice_sym_t*)ci->sym_trans;
    if(get_is_nybot(ist, getpval(doj, ROM_SEC_DEF), getplen(doj, ROM_SEC_DEF))) {
        set_fix_val(t, fix_obj, 9195,"C",1); 
        set_fix_val(t, fix_obj, 9208,"2",1); 
        set_fix_val(t, fix_obj, 9207, getpval(doj,ROM_CLR_ACC), getplen(doj,ROM_CLR_ACC));
        //set_fix_val(t, fix_obj, 9207,"123456789012",12); 
    } */
    dart_send_fix_message(fix_obj, ci);
    destroy_fixy_message(t, fix_obj);
}

static void ice_send_trader_logon(con_interface* ci)
{
    trans_t* t = (trans_t*)ci->parser;
    ofp* tl = get_fixy_message(t, 0x4743);
    dart_send_fix_message(tl, ci);
    destroy_fixy_message(t, tl);
}

static void ice_on_connect(struct connection_interface *ci, int is_connected)
{
    on_connect(ci, is_connected);
    if(is_connected) {
        ice_send_trader_logon(ci);
        send_sec_def_reqs(ci->ap, ci);
    }
}
static ff_desc* create_ice_ff_out_array(int* len, char* dir, int d_len)
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

static ff_desc* create_ice_ff_in_array(int* len, char* dir, int d_len)
{
    char* full_path = calloc(1, d_len + 16);
    ff_desc* ffd = calloc(12, sizeof(struct fix_file_desc));
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
    //UDS
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomUDS", 11);
    ffd[8].filename = full_path;
    ffd[8].fix_type = 0x4455;
    //done
    *len = 9;
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
            send_fix_logon, build_ice_offsets,
            on_ice_enter_order, on_ice_cancel_order,
            on_ice_replace_order, gk, (char *) ex->dest_id,
            ex->dest_id_len);
    con->client_mess_parse = parse_fix_message;
    con->ci_parse_v = 0;
    con->on_connect = ice_on_connect;
    con->ci_obj_callback= from_ice;
    con->connection_conf = ex;
    con->can_send_orders = 0;
    con->session_conf = fix_args;
    add_int_orderids(con);
    int ret_len = 0;
    char* directory = get_val_for_tag(ex->dc, "FIX SERVER", 10,
            "directory", 9, &ret_len);
    int out_len = 0;
    int in_len = 0;
    ff_desc* out_types = create_ice_ff_out_array(&out_len,directory, ret_len);
    ff_desc* in_types = create_ice_ff_in_array(&in_len,directory, ret_len);
    trans_t* t = create_translator(out_types,out_len,
            in_types, in_len);
    con->parser = t;
    con->tg = token_creation_func(con->dest_id, con->dest_len);
    char* mifid = get_val_for_tag(ex->dc, "FIX SERVER", 10,
            "MiFID_vals", 10, &ret_len);
    if(ret_len <= 0) {
        mifid = "MiFIDIDs";
    }
    con->sym_trans = init_sym_trans(mifid);
}

void build_fill_not_found_mail(con_interface * ob, 
        dart_order_obj* obj)
{
    char* title = calloc(1,50);
    int t_len = 0;
    if(getplen(obj, FIX_EXE_TYPE) > 0) {
        char* t = (char*)getpval(obj, FIX_EXE_TYPE);
        if(t[0] == '1') {
            memcpy(title, "Bust from ICE", 13);
            t_len = 13;
        } else {
            memcpy(title, "Correction from ICE", 19);
            t_len = 19;
        }
    } else {
        memcpy(title, "Unmatched Fill from ICE", 23);
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



static int ice_reset_fields_for_fill(dart_order_obj * orig,
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
    

    double d = strtod(doj->positions[FIX_EXEC_PRICE].iov_base, NULL);
    d = d * 10000;
    d += .0000001;
    long rblp = (long) d;
    fill_in_rom_field(orig, ROM_BIN_LAST_PRICE, &rblp, sizeof(long));
    int ls = strtol(doj->positions[FIX_LAST_SHARES].iov_base, NULL, 10);
    int qty = strtol(doj->positions[FIX_SHARES].iov_base, NULL, 10);
    fill_in_rom_field(orig, ROM_BIN_LAST_SHARES, &ls, sizeof(int));
    int cum = 0;
    int leaves = 0;
    /**
     * Right now the ICE only sends 442 on leg partials and fills
     */
    if(is_spread(orig)) {
        int sym_len = 0;
        char* sym = get_dart_sym(ob->sym_trans, getpval(doj, FIX_SYM),
                                 getplen(doj,FIX_SYM),
                                 &sym_len);
        fill_in_rom_field(orig, ROM_SEC_DEF, sym, sym_len);
        if(getplen(doj, FIX_SIDE) > 0) {
            fill_in_rom_field(orig, ROM_SIDE, getpval(doj, FIX_SIDE), getplen(doj, FIX_SIDE));
        }
        ice_sec_def* isd  = get_sec_def(ob->sym_trans, getpval(doj, FIX_SYM),
                                        getplen(doj, FIX_SYM));
        if(isd != 0) {
            fill_in_rom_field(orig, ROM_SEC_TYPE, &isd->sec_type, 1);
            if(isd->sec_type == 'O') {
                fill_in_rom_field(orig, ROM_P_C, &isd->p_or_c, 1);
            }
            fill_in_rom_field(orig, ROM_EXPIRE_DATE, isd->mat_mon_year, 6);
            fill_in_rom_field(orig, ROM_EXPIRE_DAY, isd->mat_day, isd->m_day_len);
            if(isd->r_sym_len > 0 && getplen(doj, FIX_LEG_TYPE) > 0) {
                fill_in_rom_field(orig, ROM_SYM, isd->ronin_sym, isd->r_sym_len);
            } else {
                fill_in_rom_field(orig, ROM_SYM, getpval(orig, ROM_STASH_SYM),
                                  getplen(orig, ROM_STASH_SYM));
            }
            if(isd->strike_len > 0) {
                fill_in_rom_field(orig, ROM_STRIKE, isd->strike, isd->strike_len);
            }
        } else {
            fill_in_rom_field(orig, ROM_SYM, getpval(orig, ROM_STASH_SYM),
                              getplen(orig, ROM_STASH_SYM));
        }
        fill_in_rom_field(orig, ROM_CUM,
                doj->positions[FIX_CUM].iov_base,
                doj->positions[FIX_CUM].iov_len);
        fill_in_rom_field(orig, ROM_LEAVES,
                doj->positions[FIX_LEAVES].iov_base,
                doj->positions[FIX_LEAVES].iov_len);
        if(getplen(doj, FIX_LEG_TYPE) > 0) {
            reset_rom_field(orig, ROM_SPREAD, "6", 1);
        } else {
            reset_rom_field(orig, ROM_SPREAD, "5", 1);
            if(getplen(doj, FIX_CUM) > 0) {
                cum = atoi(getpval(doj, FIX_CUM));
                fill_in_rom_field(orig, ROM_BIN_CUM, &cum, sizeof(int));
            } 
        }
    } else {
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
    reset_rom_field(orig, ROM_SPREAD, "5", 1);
}

static int send_sec_def(int off)
{
    int ret = 0;
    switch (off) {
        case 3:
        case 4:
        case 5:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 26:
        case 38:
        case 41:
        case 42:
        case 55:
        case 57:
        case 58:
        case 59:
        case 66:
        case 70:
        case 71:
        case 72:
        case 84:
        case 85:
        case 86:
        case 97:
        case 100:
        case 101:
        case 179:
        case 180:
        case 184:
        case 185:
        case 188:
        case 189:
        case 190:
        case 207:
        case 205:
            ret = 1;
            break;
        default:
            break;
    };
    return ret;
}


static void send_sec_def_reqs(async_parse_args* apa, con_interface* con)
{
    trans_t* t = (trans_t*)con->parser;
    int temp = 0;
    char* temp_sec =
        get_val_for_tag(con->connection_conf->dc, "FIX SERVER", 10, "Sec Def Types",
                13, &temp);
    if(temp > 0) {
        temp = atoi(temp_sec);
    } else {
        temp = 318;
    }
    ofp* sec = get_fixy_message(t,0x63);
    int i = 0;
    for(; i < temp; ++i) {
        if(send_sec_def(i)) {
            set_num_val(t, sec, 320, i);
            set_num_val(t, sec, 48, i);
            set_fix_val(t, sec, 321, "3", 1);
            set_fix_val(t, sec, 167, "FUT", 3);
            dart_send_fix_message(sec, con);
            reset_object(t, sec);
            set_num_val(t, sec, 320, i);
            set_num_val(t, sec, 48, i);
            set_fix_val(t, sec, 321, "100", 3);
            set_fix_val(t, sec, 167, "OPT", 3);
            dart_send_fix_message(sec, con);
            reset_object(t, sec);
            set_num_val(t, sec, 320, i);
            set_num_val(t, sec, 48, i);
            set_fix_val(t, sec, 321, "101", 3);
            set_fix_val(t, sec, 167, "MLEG", 4);
            dart_send_fix_message(sec, con);
            reset_object(t, sec);
        }
    }
    destroy_fixy_message(t, sec);
}

void ice_handle_cnl_rpl_rej(dart_order_obj* doj, con_interface* ci)
{
    dart_order_obj *ute = 0;
    int found = get_obj_by_id(ci, doj->positions[FIX_CLORDID].iov_base,
            doj->positions[FIX_CLORDID].iov_len, &ute);
    if(found == 0) {
        found = get_obj_by_id(ci, doj->positions[FIX_ORIG_ID].iov_base,
                              doj->positions[FIX_ORIG_ID].iov_len, &ute);
    }
    if (found && lock_n_check(ute)) {
        if(getplen(doj, FIX_CNL_TYPE) > 0 && ((char*)getpval(doj,FIX_CNL_TYPE))[0] == '2') {
            reset_rom_field(ute, ROM_STATUS, "20", 2);
        } else {
            reset_rom_field(ute, ROM_STATUS, "14", 2);
        }
        reset_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
        if(getplen(doj, FIX_TEXT) > 0) {
            databuf_t *out_mess = get_databuf(ci->gk);
            clean_text(ute, out_mess, 
                        ROM_TEXT, getpval(doj, FIX_TEXT),
                        getplen(doj, FIX_TEXT));
            destroy_gk_db(ci->gk, out_mess);
        }
        if(is_spread(ute)) {
            reset_fields_for_parent(ute);
        }
        ci->on_reject(ci, ute);
    }
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
        //short status = 0;
        int destroy_orig = 0;
        char *type = doj->positions[FIX_STATUS].iov_base;
        switch(type[0]) {
            case '0':
                reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                reset_rom_field(orig, ROM_STATUS, "13", 2);
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
                fix_return(ci, orig);
                break;
            case '1':
            case '2':
                {
                    reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                    set_rom_field(orig, ROM_STATUS, getpval(doj, FIX_STATUS), 1);
                    int leaves = ice_reset_fields_for_fill(orig, doj, ci);
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
                reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                reset_rom_field(orig, ROM_STATUS, "3", 1);
                fix_return(ci, orig);
                //done for day
                break;
            case '4':
                reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                reset_rom_field(orig, ROM_STATUS, "4", 1);
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
                reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                reset_rom_field(orig, ROM_STATUS, "5", 1);
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
                reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                reset_rom_field(orig, ROM_STATUS, "6", 1);
                fix_return(ci, orig);
                break;
            case '7':
                //stopped
                reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                reset_rom_field(orig, ROM_STATUS, "7", 1);
                fix_return(ci, orig);
                break;
            case '8':
                reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                reset_rom_field(orig, ROM_STATUS, "8", 1);
                remove_from_maps(ci, getpval(orig, ROM_CLORDID),
                        getplen(orig, ROM_CLORDID)); 
                if(getplen(doj, FIX_TEXT) > 0) {
                    fill_in_rom_field(orig, ROM_TEXT, getpval(doj, FIX_TEXT),
                                      getplen(doj, FIX_TEXT));
                }
                ci->on_reject(ci, orig);
                destroy_orig = 1;
                break;
            case '9':
                //suspended
                reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                reset_rom_field(orig, ROM_STATUS, "9", 1);
                fix_return(ci, orig);
                break;
            case 'A':
                //pending new
                reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                reset_rom_field(orig, ROM_STATUS, "0", 1);
                fix_return(ci, orig);
                break;
            case 'B':
                //calculated
                break;
            case 'C':
                //Expired
                reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                reset_rom_field(orig, ROM_STATUS, "4", 1);
                remove_from_maps(ci, getpval(orig, ROM_CLORDID), 
                        getplen(orig, ROM_CLORDID)); 
                ci->on_out(ci, orig);
                destroy_orig = 1;
                break;
            case 'D':
                //restarted
                reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                reset_rom_field(orig, ROM_STATUS, "13", 2);
                fix_return(ci, orig);
                break;
            case 'E':
                //pending replace
                reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                reset_rom_field(orig, ROM_STATUS, "21", 2);
                fix_return(ci, orig);
                break;
            case 'G':
            case 'H':
                build_fill_not_found_mail(ci, doj);
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
            char* status = getpval(doj, FIX_STATUS);
            char clordid[50];
            if(getplen(doj, FIX_CLORDID) > 0) {
                memset(clordid, '\0', 50);
                memcpy(clordid,getpval(doj, FIX_CLORDID), getplen(doj, FIX_CLORDID));
                send_debug_message("Could not find orig order for: %s, status: %c \n",
                       clordid,  status[0]);
            } else {
                send_debug_message("Could not find orig, status: %c \n",
                       status[0]);
            }
            if(status[0] == '1' || status[0] == '2' || status[0] == 'H' ||
                    status[0] == 'G') {
                build_fill_not_found_mail(ci, doj);
            }
        
        }
    }
}
static void handle_email_req(dart_order_obj* doj, con_interface* con)
{
    send_biz_message(getpval(doj, 26), getplen(doj, 26), "News Flash from the ICE!",
                     24);
}

void from_ice(dart_order_obj * doj, void *a)
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
                ice_handle_cnl_rpl_rej(doj, ci);
                //cnl_rep_rej
                break;
            case 0x4943:
                ci->can_send_orders = 1;
                //Trader logon response
                //send_sec_def_reqs(apa, ci);
                break;
            case 0x64:
                //sec def response
                build_mapping(ci->sym_trans, doj, apa);
                break;
            case 0x42:
                handle_email_req(doj, ci);
                break;
            case 0x4A43:
                ci->can_send_orders = 0;
                ice_send_trader_logon(ci);
                break;
            case 0x4455:
                ice_handle_uds(ci->sym_trans, doj, apa);
                break;
            default:
                send_debug_message("Unknown type, type: %d \n",
                       doj->type);
                break;
        }
    }
}
