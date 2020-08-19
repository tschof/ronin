/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _UTPDIRECT_H__
#define _UTPDIRECT_H__

#include "common_structs.h"
#include "ex_config_helper.h"

void create_connection(con_interface * ci, char *dat_file, int df_len,
                       gatekeeper * gk);
int send_utp_logon(struct async_parse_args *pa);
unsigned long parse_utpdirect(const char *message,
                              unsigned long len,
                              int *cut_con, struct async_parse_args *pc);
void create_utp_obj_func(queue_t glob, queue_t q, int num);
void create_databuffs(queue_t glob, queue_t q, int num);
void clear_db(void *db);
void destroy_utp_obj(void *d);

#endif
