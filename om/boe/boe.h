/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _BOE_H_
#define _BOE_H_

#include "common_structs.h"
#include "async_server.h"
#include "dart_order_obj.h"
#include "ex_config_helper.h"

void create_boe_logon(databuf_t* buff, async_parse_args *pa,
                      void* con);
void create_connection(con_interface * con, char *dat_file, int df_len,
                       gatekeeper * gk);
int send_boe_logon(struct async_parse_args *pa);
unsigned long parse_boe(const char *message, unsigned long len,
                             int *cut_con, struct async_parse_args *pc);

#endif
