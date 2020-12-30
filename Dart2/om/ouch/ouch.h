/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _OUCH_H_
#define _OUHC_H_

#include "common_structs.h"
#include "async_server.h"
#include "dart_order_obj.h"
#include "ex_config_helper.h"

void create_connection(con_interface * con, char *dat_file, int df_len,
                       gatekeeper * gk);
int send_ouch_logon(struct async_parse_args *pa);
unsigned long parse_ouch(const char *message, unsigned long len,
                             int *cut_con, struct async_parse_args *pc);

#endif
