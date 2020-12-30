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


int trans_sec_type(char* val, int val_len, char* output);
char translate_put_call(char val);
char translate_open_close(char val);
void handle_cnl_rpl_rej(dart_order_obj* doj, con_interface* ci);
int get_obj_by_id(con_interface * ob, void *clordid, int len,
        dart_order_obj ** obj);
void fix_return(con_interface* con, dart_order_obj * doj);
void remove_from_maps(con_interface * ci, char *id, int id_len);
void set_exec_time(trans_t* t, ofp* obj);
void set_rep_fields(dart_order_obj * ute, dart_order_obj * rep, 
        con_interface* ob);
int is_spread(dart_order_obj * doj);
void apply_spread_fields(trans_t* t, dart_order_obj* obj, ofp* fix_spread);
void set_parent_fill_fields(dart_order_obj* child, dart_order_obj* parent,
                            dart_order_obj* ex_mess, int last_shares, int leaves,
                            databuf_t* t);
void build_children(con_interface* ci);
int reset_fields_for_fill(dart_order_obj * orig,
        dart_order_obj * doj, con_interface* ob,
        void (*next_step) (struct connection_interface *,
                                           dart_order_obj * doj));
void send_leg_rej_updates(dart_order_obj* doj,
                                 con_interface* ci);
void set_spread_rep_fields(dart_order_obj* orig, 
                                  dart_order_obj* fix,
                                  con_interface* ci);
void clean_text(dart_order_obj* doj, databuf_t* out_mess, 
        int rom_num, char* raw_text, int rlen);
#if defined(__cplusplus)
}
#endif
#endif //_ROM_FIX_TRANS_FUNCS_H__

