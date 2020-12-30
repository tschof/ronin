/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _UTPDIRECT_H__
#define _UTPDIRECT_H__

#include "common_structs.h"
#include "async_server.h"
#include "dart_order_obj.h"
#include "ex_config_helper.h"

void create_connection(con_interface * con, char *dat_file, int df_len,
                       gatekeeper * gk);
int send_ad_logon(struct async_parse_args *pa);
unsigned long parse_addirect(const char *message, unsigned long len,
                             int *cut_con, struct async_parse_args *pc);

#endif
