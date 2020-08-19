/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _GLOBEX_TRANS_H__
#define _GLOBEX_TRANS_H__

#include "ex_config_helper.h"
typedef struct globex_trans g_trans_t;


g_trans_t* create_globex_trans(char* directory, int d_len, ex_config* ex);
/**
 * Returns the value that we use to multiply the internal
 * price to match the cme price or use to divide the price
 * we receive from the cme.
 */
int get_display_factor(g_trans_t* t, char* product_code, int pc_len);
/**
 * We need to validate the clearing id since we need to 
 * embed this in FIX tag 49.  If we have an incorrect value
 * the session will be terminated and we will have to re-logon.
 */
int valid_cl_ord_id(g_trans_t* t,char* clear_id, int len);
/**
 * Here we are returned the romfield (currently either 2 or 24)
 * from which we will use the value to put in the sender sub id
 * field (FIX tag 50).
 */
int rom_field_for_send_sub(g_trans_t* t, char* dest, int dlen);

size_t get_symbol_len(char *sym, size_t s_len);
char* get_sender_location(g_trans_t* t, char* trader, int tlen);
char* get_cust_type(g_trans_t* t, int* len);
char* get_cust_or_firm(g_trans_t* t, int* len);
void reload_display_factors(g_trans_t* t);

#endif //_GLOBEX_TRANS_H__
