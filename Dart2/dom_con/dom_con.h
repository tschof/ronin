/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _DOM_CON_H__
#define _DOM_CON_H__

#include "dart_order_obj.h"
#include "ex_config_helper.h"

void create_connection(con_int* con, char *dat_file,
                       int df_len, gatekeeper * gk);
void ex_dom_con(void* a, dart_order_obj * doj);

void dc_send(con_int* ci, rcntr* doj);
void dc_reset_seq(con_int* ci, int out, int in);
void dc_cleansing_rinse(con_int* ci);
#endif
