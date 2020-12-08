/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _OATS_H__
#define _OATS_H__

#include "dart_order_obj.h"
#include "connection_interface.h"
#include "ex_config_helper.h"
#include "gatekeeper.h"
#include "message_queue.h"

typedef struct oats oats_t;
void destroy_oats(con_int* ci);
void create_oats_connection(con_int* ci, char *dat_file, int df_len,
                                      gatekeeper * gk);
void parse_oats(int *cut_con, con_int *pc);
void oats_rom_helper_func(dart_order_obj * r, void *b);
void create_oats_obj_func(queue_t glob, queue_t q, int num);
char *get_rom_val(const char *message, unsigned long len, int *val_len,
                  int rom_val);
void add_dest_connection(con_int * oat, char *dest_name, int name_len,
                         databuf_t * buff);
void add_client_connection(con_int * oat, char *client_name,
                           int name_len, databuf_t * buff);
void remove_dest_connection(con_int * oat, char *dest_name,
                            int name_len, databuf_t * buff);
void remove_client_connection(con_int * oat, char *dest_name,
                              int name_len, databuf_t * buff);
void send_oats_update(con_int * ap, int is_connected, char *name,
                      int name_len, int client_or_serv,
                      con_int* ci);

void send_oats_message(con_int* oats_con, rcntr* doj);

#endif
