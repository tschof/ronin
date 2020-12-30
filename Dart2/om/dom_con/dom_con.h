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

void create_connection(con_interface * con, char *dat_file,
                       int df_len, gatekeeper * gk);
int send_ad_logon(struct async_parse_args *pa);
unsigned long parse_dom_con(const char *message,
                            unsigned long len,
                            int *cut_con, struct async_parse_args *pc);
void dc_rom_helper_func(dart_order_obj * r, void *b);
void create_dc_obj_func(queue_t glob, queue_t q, int num);
void create_databuffs(queue_t glob, queue_t q, int num);
void clear_db(void *db);
void destroy_ad_obj(void *d);

#endif
