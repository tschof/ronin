
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
#include "fix_handler.h"
#include "fix_trans.h"
#include "fix_parser.h"
#include "fix_parser_factory.h"
#include "polling_server.h"
#include "rom_fix_trans_funcs.h"
#include "connection_factory.h"


void from_nfx(void* a, dart_order_obj * doj);



static void trans_nfx_cap(ofp* fix_obj, char* cap)
{
        switch(cap[0]) {
            case 'A':
                set_fix_val(fix_obj, 528, "A", 1);
                set_fix_val(fix_obj, 582, "1", 1);
                break;
            case 'P':
                set_fix_val(fix_obj, 528, "P", 1);
                set_fix_val(fix_obj, 582, "1", 1);
                break;
            case 'X':
                set_fix_val(fix_obj, 528, "A", 1);
                set_fix_val(fix_obj, 582, "1", 1);
                break;
        }
}

static void add_spread_legs(trans_t* t, dart_order_obj* doj, 
                            ofp* fix_obj, con_int* ci, int new)
{
    add_num_val(fix_obj, 555, doj->num_childs);
    children* lch = doj->childs;
    while(lch != NULL) {
        dart_order_obj* ldoj = (dart_order_obj*)lch->payload;
        if(new) {
            int cur_size = create_in_place_ord_token(ci->tg,
                                                    ldoj->current->data->wr_ptr, 12);
            reset_rom_field(ldoj, ROM_CLORDID, ldoj->current->data->wr_ptr,
                            cur_size);
            pthread_spin_lock(&ci->mutex);
            no_copy_insert(ci->cl_ord_id_map, ldoj->current->data->wr_ptr, cur_size,
                          ldoj);
            pthread_spin_unlock(&ci->mutex);
            ldoj->current->data->wr_ptr += cur_size;
            set_fix_val(fix_obj, 654, getpval(ldoj, ROM_CLORDID),
                        getplen(ldoj, ROM_CLORDID));
            ldoj->dest = ci;
            ldoj->sender = doj->sender;
        } else {
            set_fix_val(fix_obj, 654, getpval(ldoj, ROM_CLORDID),
                        getplen(ldoj, ROM_CLORDID));
        }
        apply_spread_fields(t,ldoj, fix_obj);
        lch = lch->next;
    }
}

int on_nfx_enter_order(con_int * ci, rcntr * risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    
    fixp* fp = (fixp*)ci->cparser;
    trans_t* t = get_trans(fp);
    ofp* fix_obj = 0;
    if(doj->childs == 0) {
        fix_obj = get_message_for_type(fp,0x44);
    } else {
        fix_obj = get_message_for_type(fp,0x4241);
    }
    if(getplen(doj, ROM_INSTR) > 0) {
        set_rom_field(doj, ROM_COPY_INSTR,
                getpval(doj, ROM_INSTR), getplen(doj, ROM_INSTR));
    }
    int cur_size = create_multiday_in_place_ord_token(ci->tg,
                                                doj->current->data->wr_ptr, 12);
    reset_rom_field(doj, ROM_CLORDID, doj->current->data->wr_ptr,
                    cur_size);
    pthread_spin_lock(&ci->mutex);
    no_copy_insert(ci->cl_ord_id_map, doj->current->data->wr_ptr, cur_size,
                   doj);
    pthread_spin_unlock(&ci->mutex);
    set_fix_val(fix_obj, 11, doj->current->data->wr_ptr, cur_size);
    doj->current->data->wr_ptr += cur_size;
    
    set_fix_val(fix_obj, 453, "1", 1);
    set_fix_val(fix_obj, 448, "46424", 5);
    set_fix_val(fix_obj, 447, "D", 1);
    set_fix_val(fix_obj, 452, "11", 2);
    
    if(getplen(doj, ROM_SYM) > 0) {
        set_fix_val(fix_obj, 55, getpval(doj, ROM_SYM), getplen(doj, ROM_SYM));
    }
    set_fix_val(fix_obj, 54, getpval(doj, ROM_SIDE), getplen(doj, ROM_SIDE));
    if(is_spread(doj)) {
        set_rom_field(doj, ROM_STASH_SIDE, getpval(doj, ROM_SIDE), getplen(doj, ROM_SIDE));
    }
    set_exec_time(t, fix_obj);
    set_fix_val(fix_obj, 38, getpval(doj, ROM_SHARES), getplen(doj, ROM_SHARES));
    if(getplen(doj, ROM_MAX_FLOOR) > 0) {
        set_fix_val( fix_obj, 111, getpval(doj,ROM_MAX_FLOOR), getplen(doj,ROM_MAX_FLOOR));
    }
    set_fix_val(fix_obj, 40, getpval(doj,ROM_TYPE), getplen(doj,ROM_TYPE));
    if(getplen(doj, ROM_PRICE) > 0) {
        set_fix_val( fix_obj, 44, getpval(doj,ROM_PRICE), getplen(doj,ROM_PRICE));
    }
    if(getplen(doj,ROM_STOP) > 0) {
        set_fix_val(fix_obj, 99, getpval(doj,ROM_STOP), getplen(doj,ROM_STOP));
    }
    set_fix_val(fix_obj, 59, getpval(doj,ROM_TIF), getplen(doj,ROM_TIF));
    set_fix_val(fix_obj, 1, getpval(doj,ROM_CLR_ACC), getplen(doj,ROM_CLR_ACC));
    if(getplen(doj, ROM_O_C) > 0) {
        char* oc_c = (char*)getpval(doj, ROM_O_C);
        add_single_char_val(fix_obj, "77",2, translate_open_close(oc_c[0]));
    }
    if(getplen(doj, ROM_EXPIRE_TIME) > 0) {
        set_fix_val(fix_obj, 432, getpval(doj,ROM_EXPIRE_TIME), getplen(doj,ROM_EXPIRE_TIME));
    }
    if(getplen(doj, ROM_CAP) > 0) {
        trans_nfx_cap(fix_obj, getpval(doj, ROM_CAP));
    }
    if(doj->childs != 0) {
        add_spread_legs(t, doj, fix_obj, ci, 1);
    }
    set_fix_val(fix_obj, 70, getpval(doj,ROM_TRADE_FOR), getplen(doj,ROM_TRADE_FOR));
    fp_send_fix_message(fix_obj, fp);
    destroy_fixy_message(t, fix_obj);
    return 1;
}

int on_nfx_cancel_order(con_int * ci, rcntr * risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    fixp* fp = (fixp*)ci->cparser;
    trans_t* t = get_trans(fp);
    ofp* fix_obj = get_fixy_message(t,0x46);
    check_and_resize(doj, 12);
        int cur_size = create_multiday_in_place_ord_token(ci->tg,
                doj->current->data->wr_ptr, 12);

    reset_rom_field(doj, ROM_PENDING_ID, doj->current->data->wr_ptr,
                    cur_size);
    set_fix_val(fix_obj, 11, doj->current->data->wr_ptr, cur_size);
            pthread_spin_lock(&ci->mutex);
            no_copy_insert(ci->cl_ord_id_map, doj->current->data->wr_ptr, cur_size,
                          doj);
            pthread_spin_unlock(&ci->mutex);
    doj->current->data->wr_ptr += cur_size;
    set_fix_val(fix_obj, 41, getpval(doj, ROM_CLORDID), getplen(doj, ROM_CLORDID));
    if(getplen(doj, ROM_SYM) > 0) {
        set_fix_val(fix_obj, 55, getpval(doj, ROM_SYM), getplen(doj, ROM_SYM));
    }
    if(is_spread(doj)) {
        if(getplen(doj, ROM_STASH_SIDE) > 0) {
            set_fix_val(fix_obj, 54, getpval(doj, ROM_STASH_SIDE), getplen(doj, ROM_STASH_SIDE));
        } else {
            set_rom_field(doj, ROM_STASH_SIDE, getpval(doj, ROM_SIDE), getplen(doj, ROM_SIDE));
            set_fix_val(fix_obj, 54, getpval(doj, ROM_STASH_SIDE), getplen(doj, ROM_STASH_SIDE));
        }
    } else {
        set_fix_val(fix_obj, 54, getpval(doj, ROM_SIDE), getplen(doj, ROM_SIDE));
    }
    set_fix_val(fix_obj, 38, getpval(doj, ROM_SHARES), getplen(doj, ROM_SHARES));
    set_exec_time(t, fix_obj);
    fp_send_fix_message(fix_obj, fp);
    destroy_fixy_message(t, fix_obj);
    return 1;
}

int on_nfx_replace_order(con_int * ci, rcntr * risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    fixp* fp = (fixp*)ci->cparser;
    trans_t* t = get_trans(fp);
    ofp* fix_obj = 0;
    if(doj->childs == 0) {
        fix_obj = get_fixy_message(t,0x47);
    } else { 
        fix_obj = get_fixy_message(t,0x4341);
    } 
    check_and_resize(doj, 12);
    int  cur_size = create_multiday_in_place_ord_token(ci->tg,
                doj->current->data->wr_ptr, 12);

    reset_rom_field(doj, ROM_PENDING_ID, doj->current->data->wr_ptr,
                    cur_size);
    set_fix_val(fix_obj, 11, doj->current->data->wr_ptr, cur_size);
            pthread_spin_lock(&ci->mutex);
            no_copy_insert(ci->cl_ord_id_map, doj->current->data->wr_ptr, cur_size,
                          doj);
            pthread_spin_unlock(&ci->mutex);
    doj->current->data->wr_ptr += cur_size;
    set_fix_val(fix_obj, 41, getpval(doj, ROM_CLORDID), getplen(doj, ROM_CLORDID));
    set_fix_val(fix_obj, 453, "1", 1);
    set_fix_val(fix_obj, 448, "46424", 5);
    set_fix_val(fix_obj, 447, "D", 1);
    set_fix_val(fix_obj, 452, "11", 2);
    if(getplen(doj, ROM_SYM) > 0) {
        set_fix_val(fix_obj, 55, getpval(doj, ROM_SYM), getplen(doj, ROM_SYM));
    }
    if(is_spread(doj)) {
        if(getplen(doj, ROM_STASH_SIDE) > 0) {
            set_fix_val(fix_obj, 54, getpval(doj, ROM_STASH_SIDE), getplen(doj, ROM_STASH_SIDE));
        } else {
            set_rom_field(doj, ROM_STASH_SIDE, getpval(doj, ROM_SIDE), getplen(doj, ROM_SIDE));
            set_fix_val(fix_obj, 54, getpval(doj, ROM_STASH_SIDE), getplen(doj, ROM_STASH_SIDE));
        }
    } else {
        set_fix_val(fix_obj, 54, getpval(doj, ROM_SIDE), getplen(doj, ROM_SIDE));
    }
    set_fix_val(fix_obj, 1, getpval(doj,ROM_CLR_ACC), getplen(doj,ROM_CLR_ACC));
    set_exec_time(t, fix_obj);
    dart_order_obj *rph = (dart_order_obj *) doj->rep;
    if(getplen(rph, ROM_SHARES) > 0) {
        set_fix_val(fix_obj, 38, getpval(rph, ROM_SHARES), getplen(rph, ROM_SHARES));
    } else {
        set_fix_val(fix_obj, 38, getpval(doj, ROM_SHARES), getplen(doj, ROM_SHARES));
    }
    if(getplen(rph, ROM_MAX_FLOOR) > 0) {
        set_fix_val(fix_obj, 111, getpval(rph,ROM_MAX_FLOOR), getplen(rph,ROM_MAX_FLOOR));
    }
    if(getplen(rph, ROM_TYPE) > 0) {
        set_fix_val(fix_obj, 40, getpval(rph,ROM_TYPE), getplen(rph,ROM_TYPE));
    } else {
        set_fix_val(fix_obj, 40, getpval(doj,ROM_TYPE), getplen(doj,ROM_TYPE));
    }
    if(getplen(rph, ROM_PRICE) > 0) {
        set_fix_val(fix_obj, 44, getpval(rph,ROM_PRICE), getplen(rph,ROM_PRICE));
    } else {
        set_fix_val(fix_obj, 44, getpval(doj,ROM_PRICE), getplen(doj,ROM_PRICE));
    }
    if(getplen(rph, ROM_STOP) > 0) {
        set_fix_val(fix_obj, 99, getpval(rph,ROM_STOP), getplen(rph,ROM_STOP));
    }
    if(getplen(rph, ROM_TIF) > 0) {
        set_fix_val(fix_obj, 59, getpval(rph,ROM_TIF), getplen(rph,ROM_TIF));
    }
    if(getplen(doj, ROM_O_C) > 0) {
        char* oc_c = (char*)getpval(doj, ROM_O_C);
        add_single_char_val(fix_obj, "77",2, translate_open_close(oc_c[0]));
    }
    if(getplen(rph, ROM_EXPIRE_TIME) > 0) {
        set_fix_val(fix_obj, 432, getpval(rph,ROM_EXPIRE_TIME), getplen(rph,ROM_EXPIRE_TIME));
    }
    if(getplen(rph, ROM_TRADE_FOR) > 0) {
        set_fix_val(fix_obj, 70, getpval(rph,ROM_TRADE_FOR), getplen(rph,ROM_TRADE_FOR));
    } else {
        set_fix_val(fix_obj, 70, getpval(doj,ROM_TRADE_FOR), getplen(doj,ROM_TRADE_FOR));
    }
    if(getplen(doj, ROM_P_C) > 0) {
        char* pc = (char*)getpval(doj, ROM_P_C);
        add_single_char_val(fix_obj, "201", 3, translate_put_call(pc[0]));
    }
    if(getplen(doj, ROM_CAP) > 0) {
        trans_nfx_cap(fix_obj, getpval(doj, ROM_CAP));
    }
    if(doj->childs != 0) {
        add_spread_legs(t, doj, fix_obj, ci, 0);
    }
    fp_send_fix_message(fix_obj, fp);
    destroy_fixy_message(t, fix_obj);
    return 1;
}

static void nfx_logout(con_int* ci)
{
    fixp* fp = (fixp*)ci->cparser;
    send_logout(fp, "BYE", 3);
    ci->rc->disconnect = 1;
    ci->can_send_orders = 0;
    ci->connected = 0;
}

static void nfx_on_connect(void *v, int is_connected, dart_order_obj* doj)
{
    fixp* fp = (fixp*)v;
    con_int* ci = get_fs_pass_back_pointer(fp);
    rex_con* rc = get_fs_rex_con(fp);
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
            ci->sock = rc->sock;
            break;
        case 2:
            ci->can_send_orders = 0;
            ci->connected = 1;
            ci->sock = rc->sock;
            break;
    }
    ci->update_oats(ci, is_connected);
}
static ff_desc* create_nfx_ff_out_array(int* len, char* dir, int d_len)
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

static ff_desc* create_nfx_ff_in_array(int* len, char* dir, int d_len)
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

static void build_children(con_int* ci)
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
        if (lock_n_check(current)) {
            if(current->positions[ROM_OM_PARENT].iov_len > 0) {
                dart_order_obj* parent = 0;
                int found = get_map_entry(ci->cl_ord_id_map,
                              current->positions[ROM_OM_PARENT].iov_base,
                              current->positions[ROM_OM_PARENT].iov_len,
                              (void **) &parent);
                if(found) {
                    add_child_obj(parent, current);
                }
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

void create_connection(con_int * con, char *filename, int name_len,
        gatekeeper * gk)
{
    con->cl_ord_id_map = get_initial_order_map(gk, con->mod_name,
                                              con->mod_name_len);
    con->tg = token_creation_func(con->ex->dest_id, con->ex->dest_len);
    build_children(con);
    con->sbm = 0;
    con->on_enter_order = on_nfx_enter_order;
    con->on_replace_order = on_nfx_replace_order;
    con->on_cancel_order = on_nfx_cancel_order;
    con->can_send_orders = 0;
    add_int_orderids(con);
    login_info* li = build_logon_info_from_config(con->ex);

    int out_len = 0;
    int in_len = 0;
    ff_desc* out_types = create_nfx_ff_out_array(&out_len,con->ex->directory, con->ex->dir_len);
    ff_desc* in_types = create_nfx_ff_in_array(&in_len,con->ex->directory, con->ex->dir_len);
    trans_t* t = create_translator(out_types,out_len,
                                    in_types, in_len);
    dartfact* d = create_dart_order_factory(con->ex->order_allocation);
    fixp* fp = create_fix_parser(con,
                                 from_nfx,d, con->o, t, con->ex->order_allocation);
    set_fp_header_len(fp, 13);
    set_log_info(fp, li);
    rex_con* rc = create_rex_con(con->p, con->ex, fp, fp,
                                 li,fp_write_callback, 
                                 nfx_on_connect, parse_fix_message, NULL);
    set_rex_con(fp, rc);
    con->cleansing_rinse = nfx_logout;
    rc->parser = fp;
    con->cparser = fp;
    rc->connect = send_fix_logon;
    con->rc = rc;
    rex_connect(con->p, rc);
}

void build_fill_not_found_mail(con_int * ob, 
        dart_order_obj* obj)
{
    char* title = calloc(1,50);
    int t_len = 0;
    if(getplen(obj, FIX_EXE_TYPE) > 0) {
        char* t = (char*)getpval(obj, FIX_EXE_TYPE);
        if(t[0] == '1') {
            memcpy(title, "Bust from NFX", 13);
            t_len = 13;
        } else {
            memcpy(title, "Correction from NFX", 19);
            t_len = 19;
        }
    } else {
        memcpy(title, "Unmatched Fill from NFX", 23);
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
        databuf_memcpy(cur_off, "Last Prnfx: ", 12);
        databuf_memcpy(cur_off, getpval(obj, FIX_EXEC_PRICE),
                        getplen(obj, FIX_EXEC_PRICE));
        databuf_memcpy(cur_off, "\n", 1);
    }
    send_biz_message(cur_off->buffer, databuf_unread(cur_off), title, t_len);
    databuf_destroy(cur_off);
}



static int reset_fields_for_fill(dart_order_obj * orig,
        dart_order_obj * doj, con_int* ob,
        void (*next_step) (con_int *, rcntr * risk))
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
    if(leaves == 0) {
        set_rom_field(orig, ROM_STATUS, "2", 1);
    } else {
        set_rom_field(orig, ROM_STATUS, "1", 1);
    }
    /**
     * Here we see if we need to send parent back to ROM/users.
     */
    if(is_spread(orig)) {
        char* mark = getpval(orig, ROM_LEG_MARK);
        if(getplen(orig, ROM_LEG_MARK) > 0 && mark[0] == '1') {
            rcntr* gp = 0;
            int found = get_obj_by_id(ob, doj->positions[FIX_CLORDID].iov_base,
                    doj->positions[FIX_CLORDID].iov_len, &gp);
            if(found == 0) {
                found = get_obj_by_id(ob, doj->positions[FIX_ORIG_ID].iov_base,
                                      doj->positions[FIX_ORIG_ID].iov_len, &gp);
            }
            if(found && gp->rlen == sizeof(dart_order_obj)) {
                dart_order_obj* parent = (dart_order_obj*)gp->raw;
                if(lock_n_check(parent)) {
                    databuf_reset(itoa_buff);
                    set_parent_fill_fields(orig, parent,
                                           doj, qty, cum, itoa_buff);
                    next_step(ob, &parent->risk);
                    unlock(parent);
                }
            }
        }
    } 
    destroy_gk_db(ob->gk, itoa_buff);
    return leaves;
}

static void reset_fields_for_parent(dart_order_obj* orig,
                                    dart_order_obj* doj,
                                    con_int* ci,
                                    void (*next_step) (con_int *,
                                           rcntr * risk))
{
    char* mark = getpval(orig, ROM_LEG_MARK);
    if(getplen(orig, ROM_LEG_MARK) > 0 && mark[0] == '1') {
            rcntr* gp = 0;
        int found = get_obj_by_id(ci, doj->positions[FIX_CLORDID].iov_base,
                doj->positions[FIX_CLORDID].iov_len, &gp);
        if(found == 0) {
            found = get_obj_by_id(ci, doj->positions[FIX_ORIG_ID].iov_base,
                                  doj->positions[FIX_ORIG_ID].iov_len, &gp);
        }
        if(found && gp->rlen == sizeof(dart_order_obj)) {
            dart_order_obj* parent = (dart_order_obj*)gp->raw;
            if(lock_n_check(parent)) {
                set_rom_field(parent, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(parent, ROM_STATUS, getpval(orig, ROM_STATUS), getplen(orig, ROM_STATUS));
                char* status = getpval(orig, ROM_STATUS);
                if(status[0] == '5') {
                    dart_order_obj *rep = (dart_order_obj *) parent->rep;
                    if(rep != 0) {
                        set_rep_fields(parent, rep, ci);
                    }
                }
                next_step(ci, &parent->risk);
                unlock(parent);
            }
        }
    }
}

static void send_leg_rej_updates(dart_order_obj* doj,
                                 con_int* ci)
{
    if(doj->childs != 0) {
        children* lch = doj->childs;
        while(lch != NULL) {
            dart_order_obj* ldoj = (dart_order_obj*)lch->payload;
            set_rom_field(ldoj, 0, ROM_ZERO_STATUS, 1);
            set_rom_field(ldoj, ROM_STATUS, "8", 1);
            remove_from_maps(ci, getpval(ldoj, ROM_CLORDID),
                    getplen(ldoj, ROM_CLORDID)); 
            ci->on_rej(ci, &ldoj->risk);
            lch = lch->next;
        }
    } 
}


static void handle_ex_report(dart_order_obj* doj, con_int* ci)
{
    rcntr* gp = 0;
    int found = 0;
    if(doj->positions[FIX_LEG_TAG].iov_len <= 1) {
        found = get_obj_by_id(ci, doj->positions[FIX_CLORDID].iov_base,
                doj->positions[FIX_CLORDID].iov_len, &gp);
        if(found == 0) {
            found = get_obj_by_id(ci, doj->positions[FIX_ORIG_ID].iov_base,
                                  doj->positions[FIX_ORIG_ID].iov_len, &gp);
        }
    } else {
        /*This is the first clue we have a spread leg*/
        found = get_obj_by_id(ci, doj->positions[FIX_LEG_TAG].iov_base,
                doj->positions[FIX_LEG_TAG].iov_len, &gp);
        if(found == 0) {
            found = get_obj_by_id(ci, doj->positions[FIX_CLORDID].iov_base,
                    doj->positions[FIX_CLORDID].iov_len, &gp);
        }
    }
    if (found && gp->rlen == sizeof(dart_order_obj)) {
        dart_order_obj *orig = (dart_order_obj*)gp->raw;
        if(lock_n_check(orig)) {
        //short status = 0;
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
                    reset_fields_for_parent(orig, doj, ci, ci->on_update);
                }
                ci->on_update(ci, &orig->risk);
                break;
            case '1':
            case '2':
            case 'F':
                {
                    set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                    //set_rom_field(orig, ROM_STATUS, getpval(doj, FIX_STATUS), 1);
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
                    ci->on_fill(ci, &orig->risk);
                }
                break;
            case '3':
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "3", 1);
                if(is_spread(orig)) {
                    reset_fields_for_parent(orig,doj,ci,ci->on_update);
                }
                ci->on_update(ci, &orig->risk);
                //done for day
                break;
            case '4':
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "4", 1);
                if(is_spread(orig)) {
                    reset_fields_for_parent(orig,doj,ci,ci->on_out);
                }
                remove_from_maps(ci, getpval(orig, ROM_CLORDID),
                        getplen(orig, ROM_CLORDID));
                if(getplen(orig, ROM_PENDING_ID) > 0) {
                    remove_from_maps(ci, getpval(orig, ROM_PENDING_ID), 
                            getplen(orig, ROM_PENDING_ID));
                } 
                ci->on_out(ci, &orig->risk);
                destroy_orig = 1;
                break;
            case '5':
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "5", 1);
                if(is_spread(orig)) {
                    reset_fields_for_parent(orig, doj,ci, ci->on_replaced);
                }
                if (orig->rep != NULL) {
                    dart_order_obj *rep = (dart_order_obj *) orig->rep;
                    set_rep_fields(orig, rep, ci);
                } 
                ci->on_replaced(ci, &orig->risk);
                break;
            case '6':
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "6", 1);
                if(is_spread(orig)) {
                    reset_fields_for_parent(orig,doj,ci,ci->on_update);
                }
                ci->on_update(ci, &orig->risk);
                break;
            case '7':
                //stopped
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "7", 1);
                ci->on_update(ci, &orig->risk);
                break;
            case '8':
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "8", 1);
                remove_from_maps(ci, getpval(orig, ROM_CLORDID),
                        getplen(orig, ROM_CLORDID)); 
                if(getplen(doj, FIX_TEXT) > 0) {
                    fill_in_rom_field(orig, ROM_TEXT, getpval(doj, FIX_TEXT),
                                      getplen(doj, FIX_TEXT));
                }
                if(is_spread(orig)) {
                    if(getplen(orig, ROM_OM_PARENT) > 0) {
                        reset_fields_for_parent(orig, doj, ci, ci->on_rej);
                    } else {
                        send_leg_rej_updates(orig, ci);
                    }
                }
                ci->on_rej(ci, &orig->risk);
                destroy_orig = 1;
                break;
            case '9':
                //suspended
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "9", 1);
                ci->on_update(ci, &orig->risk);
                break;
            case 'A':
                //pending new
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "0", 1);
                ci->on_update(ci, &orig->risk);
                break;
            case 'B':
                //calculated
                break;
            case 'C':
                //Expired
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "4", 1);
                if(is_spread(orig)) {
                    reset_fields_for_parent(orig,doj,ci,ci->on_out);
                }
                remove_from_maps(ci, getpval(orig, ROM_CLORDID), 
                        getplen(orig, ROM_CLORDID)); 
                ci->on_out(ci, &orig->risk);
                destroy_orig = 1;
                break;
            case 'D':
                //restarted
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "13", 2);
                ci->on_update(ci, &orig->risk);
                break;
            case 'E':
                //pending replace
                set_rom_field(orig, 0, ROM_ZERO_STATUS, 1);
                set_rom_field(orig, ROM_STATUS, "21", 2);
                if(is_spread(orig)) {
                    reset_fields_for_parent(orig,doj,ci,ci->on_update);
                }
                ci->on_update(ci, &orig->risk);
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
        }
    } else {
        if(getplen(doj, FIX_CLORDID) > 0 && getplen(doj, FIX_STATUS)) {
            char* status = getpval(doj, FIX_STATUS);
            char clordid[50];
            if(getplen(doj, FIX_CLORDID) > 0) {
                memset(clordid, '\0', 50);
                memcpy(clordid,getpval(doj, FIX_CLORDID), getplen(doj, FIX_CLORDID));
                send_debug_message(ci->o, "Could not find orig order for: %s, status: %c \n",
                       clordid,  status[0]);
            } else {
                send_debug_message(ci->o, "Could not find orig, status: %c \n",
                       status[0]);
            }
            if(status[0] == '1' || status[0] == '2' || status[0] == 'H' ||
                    status[0] == 'G') {
                build_fill_not_found_mail(ci, doj);
            }
        
        }
    }
}
static void handle_email_req(dart_order_obj* doj, con_int* con)
{
    send_biz_message(getpval(doj, 26), getplen(doj, 26), "News Flash from the PHASER!",
                     24);
}

void from_nfx(void* a, dart_order_obj * doj)
{
    con_int *ci = (con_int*) a;
    if (doj && ci) {
        doj->dest = ci;
        switch(doj->type) {
            case 0x38:
                handle_ex_report(doj, ci);
                //execution report
                break;
            case 0x39:
                handle_cnl_rpl_rej(doj, ci);
                //cnl_rep_rej
                break;
            case 0x42:
                handle_email_req(doj, ci);
                break;
            case 0x4A43:
                //Logon Response Positive
                ci->can_send_orders = 1;
                ci->connected = 1;
                break;
            default:
                break;
        }
    }
}
