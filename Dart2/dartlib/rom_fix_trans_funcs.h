/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _ROM_FIX_TRANS_FUNCS_H__
#define _ROM_FIX_TRANS_FUNCS_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include "dart_order_obj.h"
#include "connection_interface.h"
#include "fix_trans.h"


char* translate_pending_status(char val);
char* get_rom_status_for_fix(char* val, int val_len, int* ret_len);
void set_defaults_for_dma(dart_order_obj* doj, ex_config* ex);
int trans_sec_type(char* val, int val_len, char* output);
char translate_put_call(char val);
char translate_open_close(char val);
void handle_cnl_rpl_rej(dart_order_obj* doj, con_int* ci);
int get_obj_by_id(con_int* ob, void *clordid, int len,
        rcntr ** obj);
void remove_from_maps(con_int* ci, char *id, int id_len);
void set_exec_time(trans_t* t, ofp* obj);
void set_rep_fields(dart_order_obj * ute, dart_order_obj * rep, 
        con_int* ob);
int is_spread(dart_order_obj * doj);
void apply_spread_fields(trans_t* t, dart_order_obj* obj, ofp* fix_spread);
void set_parent_fill_fields(dart_order_obj* child, dart_order_obj* parent,
                            dart_order_obj* ex_mess, int last_shares, int leaves,
                            databuf_t* t);
void get_cp_len_off(dart_order_obj* doj, ofp* fix_obj);
void validate_offsets(dart_order_obj* doj, char* raw_off, int len);
void validate_offsets_client(dart_order_obj* doj, char* raw_off, int len);
void clean_text(dart_order_obj* doj, databuf_t* out_mess, 
        int rom_num, char* raw_text, int rlen);
#if defined(__cplusplus)
}
#endif
#endif //_ROM_FIX_TRANS_FUNCS_H__

