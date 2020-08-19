/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _ICE_SYM_TRANS_H__
#define _ICE_SYM_TRANS_H__
#include "dart_order_obj.h"
#include "fix_trans.h"

typedef struct ice_sym_trans ice_sym_t;

typedef struct ice_sec_def {
    char ronin_sym[10];
    int r_sym_len;
    char mat_mon_year[10];
    int mmy_len;
    char sec_type;
    char mat_day[3];
    int m_day_len;
    char p_or_c;
    char strike[16];
    int strike_len;
}ice_sec_def;

ice_sym_t* init_sym_trans(char* filename);
/**
 * This is really for futures and exchange defined spreads.
 */
char* get_ice_sym(ice_sym_t* ist, char* rsym, int rsym_len, 
                  int* ice_len);
char* get_dart_sym(ice_sym_t* ist, char* rsym, int rsym_len,
                    int * ice_len);

void build_mapping(ice_sym_t* ist, dart_order_obj* obj, void* apa);
void ice_handle_uds(ice_sym_t* ist, dart_order_obj* doj, void* apa);

int get_is_nybot(ice_sym_t* ist, char* sym, int sym_len);

void set_mifid_fields(ice_sym_t* ist, dart_order_obj* obj,
        ofp* fix_obj, trans_t* t);

void print_symbol_lists(ice_sym_t* gk, databuf_t * buff);

ice_sec_def* get_sec_def(ice_sym_t* ist, char* sym, int sym_len);
#endif //_ICE_SYM_TRANS_H__
