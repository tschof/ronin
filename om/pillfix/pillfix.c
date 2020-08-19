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
#include "dart_constants.h"
#include "config_loader.h"
#include "ex_config_helper.h"
#include "fix_handler.h"
#include "fix_trans.h"
#include "connection_interface.h"
#include "mailer.h"
#include "debug_logger.h"
#include "rom_fix_trans_funcs.h"
#include "suffix_map.h"
#include "pillar.h"

void from_pfix(dart_order_obj * doj, void *a);

int build_pfix_offsets(struct async_parse_args *apa)
{
    //con_interface* con = (con_interface*)apa->ds_book;
    return 0;
}
static void set_sym(trans_t* t, ofp* fix_obj, dart_order_obj* doj)
{
    char* slash = memchr(getpval(doj, ROM_SYM), '/', getplen(doj, ROM_SYM));
    if(slash != NULL) {
        char* symbol = getpval(doj, ROM_SYM);
        int sym_len = slash - symbol;
        set_fix_val(t, fix_obj, 55, symbol, sym_len);
        int suf_len = getplen(doj, ROM_SYM) - (sym_len + 1);
        set_fix_val(t, fix_obj, 65, (slash + 1), suf_len);
    } else {
        set_fix_val(t, fix_obj, 55, getpval(doj, ROM_SYM), getplen(doj, ROM_SYM));
    }
}

static void pfix_on_connect(struct connection_interface *ci, int is_connected)
{
    on_connect(ci, is_connected);
}

static void set_on_behalf(con_interface* ci, dart_order_obj* doj,
        ofp* fix_obj, trans_t* t)
{
    char* inst = doj->positions[ROM_COPY_INSTR].iov_base;
    int ilen = doj->positions[ROM_COPY_INSTR].iov_len;
    char* dest = doj->positions[ROM_DESTID].iov_base;
    int dlen = doj->positions[ROM_DESTID].iov_len;
    char* clear = doj->positions[ROM_CLR_ACC].iov_base;
    int clen = doj->positions[ROM_CLR_ACC].iov_len;
    int mpid_len = 0;
    char* on_behalf = 0x0;
    get_dquote_route(ci->sym_trans, dest, dlen, &mpid_len);
    if((ilen == 2 && inst[0]=='D' && inst[1] == 'Q') ||
            mpid_len > 0) {
        char* badge = (char*)get_dquote_clear(ci->sym_trans, clear, clen, &mpid_len);
        if(mpid_len > 0) {
            set_fix_val(t, fix_obj, 128, badge,mpid_len);
        }
        on_behalf = (char*)get_dquote_mne(ci->sym_trans,clear, clen,&mpid_len);
        if(mpid_len > 0) {
            set_fix_val(t, fix_obj, 116, on_behalf,mpid_len);
        } 
    } else {
        if(clen >= 4) {
            set_fix_val(t, fix_obj, 116, clear, 4);
        } else {
            set_fix_val(t, fix_obj, 116, clear, clen);
        }
    }
}

static void set_type_tif_instr(con_interface* ci, dart_order_obj* doj,
        ofp* fix_obj, trans_t* t)
{
    char* type = getpval(doj, ROM_TYPE);
    int t_len = getplen(doj, ROM_TYPE);
    char* ex_inst = getpval(doj, ROM_EXEC_INST);
    int exlen = getplen(doj, ROM_EXEC_INST);
    char* inst = getpval(doj, ROM_COPY_INSTR);
    int ilen = getplen(doj, ROM_COPY_INSTR);
    char* tif = getpval(doj, ROM_TIF);
    int tiflen = getplen(doj, ROM_TIF);
    if(ilen == 1 && inst[0] == 'A') {
        set_fix_val(t, fix_obj, 9416, "A", 1);
    } else if(ilen == 1 && inst[0] == 'I') {
        set_fix_val(t, fix_obj, 9416, "8", 1);
    } else if(ilen == 2 && inst[0] == 'N' && inst[1] == 'R') {
        set_fix_val(t, fix_obj, 18, "F", 1);
    }

    if(exlen == 1) {
        switch(ex_inst[0]) {
            case 'F':
                set_fix_val(t, fix_obj, 9303, "N", 1);
                set_fix_val(t, fix_obj, 40, type, t_len);
                set_fix_val(t, fix_obj, 59, tif, tiflen);
                break;
            case 'G':
                set_fix_val(t, fix_obj, 9303, "8", 1);
                set_fix_val(t, fix_obj, 40, type, t_len);
                set_fix_val(t, fix_obj, 59, tif, tiflen);
                break;
            case 'R':
                set_fix_val(t, fix_obj, 9303, "R", 1);
                set_fix_val(t, fix_obj, 40, type, t_len);
                set_fix_val(t, fix_obj, 59, tif, tiflen);
                break;
            case 'M':
                set_fix_val(t, fix_obj, 18, "M", 1);
                set_fix_val(t, fix_obj, 40, "P", 1);
                set_fix_val(t, fix_obj, 59, "0", 1);
                break;
            case 'L':
                set_fix_val(t, fix_obj, 18, "L", 1);
                set_fix_val(t, fix_obj, 40, "P", 1);
                set_fix_val(t, fix_obj, 59, "0", 1);
                break;
        };
    } else if (t_len == 1) {
        if(type[0] == '5') {
            set_fix_val(t, fix_obj, 40, "1", 1);
            set_fix_val(t, fix_obj, 59, "7", 1);
        } else {
            set_fix_val(t, fix_obj, 40, type, t_len);
            set_fix_val(t, fix_obj, 59, tif, tiflen);
        }
    } else if(t_len > 1) {
        if(type[0] == '1' && type[1] == '1') {
            set_fix_val(t, fix_obj, 40, "2", 1);
            set_fix_val(t, fix_obj, 59, "7", 1);
        } else if(type[0] == '1' && type[1] == '2') {
            set_fix_val(t, fix_obj, 40, "1", 1);
            set_fix_val(t, fix_obj, 59, "2", 1);
        } 
    }
}

int on_pfix_enter_order(con_interface * ci, dart_order_obj * doj,
        struct async_parse_args *ap)
{
    
    trans_t* t = (trans_t*)ci->parser;
    ofp* fix_obj = 0;
    fix_obj = get_fixy_message(t,0x44);
    if(getplen(doj, ROM_INSTR) > 0) {
        set_rom_field(doj, ROM_COPY_INSTR,
                getpval(doj, ROM_INSTR), getplen(doj, ROM_INSTR));
    }
    set_on_behalf(ci, doj, fix_obj, t);
    int clr_len = 0;
    char* flipper = (char*)get_nyse_flip(ci->sym_trans,
            getpval(doj, ROM_CLR_ACC),
            getplen(doj, ROM_CLR_ACC),
            &clr_len);
    if(clr_len > 0) {
        set_fix_val(t, fix_obj, 1, flipper, clr_len);
    } else {
        set_fix_val(t, fix_obj, 1, getpval(doj,ROM_CLR_ACC), getplen(doj,ROM_CLR_ACC));
    }
    /*Start with order id*/
    int cur_size = create_multiday_in_place_ord_token(ci->tg,
                                                doj->current->data->wr_ptr, 12);
    reset_rom_field(doj, ROM_CLORDID, doj->current->data->wr_ptr,
                    cur_size);
    pthread_spin_lock(&ci->mutex);
    no_copy_insert(ci->cl_ord_id_map, doj->current->data->wr_ptr, cur_size,
                   doj);
    pthread_spin_unlock(&ci->mutex);
    set_fix_val(t, fix_obj, 11, doj->current->data->wr_ptr, cur_size);
    doj->current->data->wr_ptr += cur_size;
    /*Done with order id*/
    set_type_tif_instr(ci, doj, fix_obj, t);
    set_fix_val(t, fix_obj, 54, getpval(doj, ROM_SIDE), getplen(doj, ROM_SIDE));
    set_sym(t, fix_obj, doj);
    set_exec_time(t, fix_obj);
    set_fix_val(t, fix_obj, 386, "1", 1);
    set_fix_val(t, fix_obj, 336, "2", 1);
    set_fix_val(t, fix_obj, 38, getpval(doj, ROM_SHARES), getplen(doj, ROM_SHARES));
    if(getplen(doj, ROM_MAX_FLOOR) > 0) {
        set_fix_val(t, fix_obj, 111, getpval(doj,ROM_MAX_FLOOR),
                getplen(doj,ROM_MAX_FLOOR));
    }
    if(getplen(doj, ROM_MIN_QTY) > 0) {
        set_fix_val(t, fix_obj, 110, getpval(doj, ROM_MIN_QTY),
                getplen(doj, ROM_MIN_QTY));
    }
    if(getplen(doj, ROM_PRICE) > 0) {
        set_fix_val(t, fix_obj, 44, getpval(doj,ROM_PRICE), getplen(doj,ROM_PRICE));
    }
    if(getplen(doj,ROM_STOP) > 0) {
        set_fix_val(t, fix_obj, 99, getpval(doj,ROM_STOP), getplen(doj,ROM_STOP));
    }
    set_fix_val(t, fix_obj, 528, getpval(doj,ROM_CAP), getplen(doj,ROM_CAP));

    if(getplen(doj, ROM_EXPIRE_TIME) > 0) {
        set_fix_val(t, fix_obj, 126, getpval(doj, ROM_EXPIRE_TIME), getplen(doj,ROM_EXPIRE_TIME));
    }
    if(getplen(doj, ROM_SHORT_LEND) > 0) {
        set_fix_val(t, fix_obj, 114, "N", 1);
    }
    set_fix_val(t, fix_obj, 7928, "N", 1);
    set_fix_val(t, fix_obj, 9448, "1234", 4);
    set_fix_val(t, fix_obj, 9449, "ALGO", 4);
    int mpid_len = 0;
    char* mpid = (char*)get_default_on_behalf(ci->sym_trans, &mpid_len);
    if(mpid_len > 0) {
        set_fix_val(t, fix_obj, 9453, mpid, mpid_len);
    } else {
        set_fix_val(t, fix_obj, 9453, "DEGS", 4);
    }
    set_fix_val(t, fix_obj, 9451, getpval(doj, ROM_CLORDID),
            getplen(doj, ROM_CLORDID));
    dart_send_fix_message(fix_obj, ci);
    destroy_fixy_message(t, fix_obj);
    return 1;
}

int on_pfix_cancel_order(con_interface * ci, dart_order_obj * doj)
{
    trans_t* t = (trans_t*)ci->parser;
    ofp* fix_obj = get_fixy_message(t,0x46);
    check_and_resize(doj, 12);
        int cur_size = create_multiday_in_place_ord_token(ci->tg,
                doj->current->data->wr_ptr, 12);

    reset_rom_field(doj, ROM_PENDING_ID, doj->current->data->wr_ptr,
                    cur_size);
    set_fix_val(t, fix_obj, 11, doj->current->data->wr_ptr, cur_size);
    doj->current->data->wr_ptr += cur_size;
    set_fix_val(t, fix_obj, 41, getpval(doj, ROM_CLORDID), getplen(doj, ROM_CLORDID));
    if(getplen(doj, ROM_SYM) > 0) {
        set_sym(t, fix_obj, doj);
    }
    set_on_behalf(ci, doj, fix_obj, t);
    set_exec_time(t, fix_obj);
    set_fix_val(t, fix_obj, 54, getpval(doj, ROM_SIDE), getplen(doj, ROM_SIDE));
    dart_send_fix_message(fix_obj, ci);
    destroy_fixy_message(t, fix_obj);
    return 1;
}

void on_pfix_replace_order(con_interface * ci, dart_order_obj * doj)
{
    trans_t* t = (trans_t*)ci->parser;
    ofp* fix_obj = 0;
    fix_obj = get_fixy_message(t,0x47);
    check_and_resize(doj, 12);
    dart_order_obj *rph = (dart_order_obj *) doj->rep;
    int  cur_size = create_multiday_in_place_ord_token(ci->tg,
                doj->current->data->wr_ptr, 12);

    reset_rom_field(doj, ROM_PENDING_ID, doj->current->data->wr_ptr,
                    cur_size);
    set_fix_val(t, fix_obj, 11, doj->current->data->wr_ptr, cur_size);
    doj->current->data->wr_ptr += cur_size;
    set_fix_val(t, fix_obj, 41, getpval(doj, ROM_CLORDID), getplen(doj, ROM_CLORDID));
    set_rom_field(rph, ROM_CLR_ACC, getpval(doj, ROM_CLR_ACC), getplen(doj, ROM_CLR_ACC));
    set_rom_field(rph, ROM_DESTID, getpval(doj, ROM_DESTID), getplen(doj, ROM_DESTID));
    set_rom_field(rph, ROM_COPY_INSTR, getpval(doj, ROM_COPY_INSTR), getplen(doj, ROM_COPY_INSTR));
    set_on_behalf(ci, rph, fix_obj, t);
    set_type_tif_instr(ci, rph, fix_obj, t);
    if(getplen(doj, ROM_SYM) > 0) {
        set_sym(t, fix_obj, doj);
    }
    set_fix_val(t, fix_obj, 54, getpval(doj, ROM_SIDE), getplen(doj, ROM_SIDE));
    int clr_len = 0;
    char* flipper = (char*)get_nyse_flip(ci->sym_trans,
            getpval(doj, ROM_CLR_ACC),
            getplen(doj, ROM_CLR_ACC),
            &clr_len);
    if(clr_len > 0) {
        set_fix_val(t, fix_obj, 1, flipper, clr_len);
    } else {
        set_fix_val(t, fix_obj, 1, getpval(doj,ROM_CLR_ACC), getplen(doj,ROM_CLR_ACC));
    }
    set_exec_time(t, fix_obj);
    set_fix_val(t, fix_obj, 386, "1", 1);
    set_fix_val(t, fix_obj, 336, "2", 1);
    set_fix_val(t, fix_obj, 528, getpval(doj,ROM_CAP), getplen(doj,ROM_CAP));
    if(getplen(rph, ROM_SHARES) > 0) {
        set_fix_val(t, fix_obj, 38, getpval(rph, ROM_SHARES), getplen(rph, ROM_SHARES));
    } else {
        set_fix_val(t, fix_obj, 38, getpval(doj, ROM_SHARES), getplen(doj, ROM_SHARES));
    }
    if(getplen(rph, ROM_PRICE) > 0) {
        set_fix_val(t, fix_obj, 44, getpval(rph,ROM_PRICE), getplen(rph,ROM_PRICE));
    } else if(getplen(doj, ROM_PRICE) > 0) {
        set_fix_val(t, fix_obj, 44, getpval(doj,ROM_PRICE), getplen(doj,ROM_PRICE));
    }
    if(getplen(rph, ROM_STOP) > 0) {
        set_fix_val(t, fix_obj, 99, getpval(rph,ROM_STOP), getplen(rph,ROM_STOP));
    } else if(getplen(doj, ROM_STOP) > 0) {
        set_fix_val(t, fix_obj, 99, getpval(doj,ROM_STOP), getplen(doj,ROM_STOP));
    }
    if(getplen(rph, ROM_END_TIME) > 0) {
        set_fix_val(t, fix_obj, 126, getpval(rph,ROM_END_TIME), getplen(rph,ROM_END_TIME));
    } else if(getplen(doj, ROM_END_TIME) > 0) {
        set_fix_val(t, fix_obj, 126, getpval(doj,ROM_END_TIME), getplen(doj,ROM_END_TIME));
    }
    if(getplen(doj, ROM_SHORT_LEND) > 0) {
        set_fix_val(t, fix_obj, 114, "N", 1);
    }
    set_fix_val(t, fix_obj, 7928, "N", 1);
    set_fix_val(t, fix_obj, 9448, "1234", 4);
    set_fix_val(t, fix_obj, 9449, "ALGO", 4);
    int mpid_len = 0;
    char* mpid = (char*)get_default_on_behalf(ci->sym_trans, &mpid_len);
    if(mpid_len > 0) {
        set_fix_val(t, fix_obj, 9453, mpid, mpid_len);
    } else {
        set_fix_val(t, fix_obj, 9453, "DEGS", 4);
    }
    set_fix_val(t, fix_obj, 9451, getpval(doj, ROM_CLORDID),
            getplen(doj, ROM_CLORDID));
    dart_send_fix_message(fix_obj, ci);
    destroy_fixy_message(t, fix_obj);
}

static ff_desc* create_pfix_ff_out_array(int* len, char* dir, int d_len)
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

static ff_desc* create_pfix_ff_in_array(int* len, char* dir, int d_len)
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
                        send_fix_logon, build_pfix_offsets,
                        on_pfix_enter_order, on_pfix_cancel_order,
                        on_pfix_replace_order, gk, (char *) ex->dest_id,
                        ex->dest_id_len);
    con->client_mess_parse = parse_fix_message;
    con->on_connect = pfix_on_connect;
    con->ci_obj_callback= from_pfix;
    con->connection_conf = ex;
    con->can_send_orders = 0;
    con->session_conf = fix_args;
    con->tg = generic_creation_func();
    int ret_len = 0;
    char* directory = get_val_for_tag(ex->dc, "FIX SERVER", 10,
            "directory", 9, &ret_len);
    int out_len = 0;
    int in_len = 0;
    ff_desc* out_types = create_pfix_ff_out_array(&out_len,directory, ret_len);
    ff_desc* in_types = create_pfix_ff_in_array(&in_len,directory, ret_len);
    trans_t* t = create_translator(out_types,out_len,
            in_types, in_len);
    con->sym_trans = create_pillar_parser(ex->dc);
    con->parser = t;
    con->tg = token_creation_func(con->dest_id, con->dest_len);
    add_int_orderids(con);
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
                fix_return(ci,orig);
                break;
            case '1':
            case '2':
                {
                    reset_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                    reset_rom_field(orig, ROM_STATUS, type, 1);
                    int leaves = reset_fields_for_fill(orig, doj, ci, ci->on_fill);
                    if (leaves == 0) {
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
                fill_in_rom_field(orig, ROM_TEXT, getpval(doj, FIX_TEXT),
                                  getplen(doj, FIX_TEXT));
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
    }
}

void from_pfix(dart_order_obj * doj, void *a)
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
