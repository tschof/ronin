/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _UTPDIRECT_H__
#define _UTPDIRECT_H__

#include "message_queue.h"
#include "connection_interface.h"
#include "nyse_seq_generator.h"

typedef struct utp_parser utpp;
void create_connection(con_int* ci, char *dat_file, int df_len,
                       gatekeeper * gk);
void send_utp_logon(void *pa);
void create_utp_obj_func(queue_t glob, queue_t q, int num);
void create_databuffs(queue_t glob, queue_t q, int num);
void clear_db(void *db);
void destroy_utp_obj(void *d);
nyse_seq_struct* get_seq_struct(utpp* uparse);
#endif
