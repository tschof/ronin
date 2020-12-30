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
void fs_send(con_int* ci, dart_order_obj* doj);
void fs_reset_seq(con_int* ci, int out, int in);
void fs_cleansing_rinse(con_int* ci);
#endif
