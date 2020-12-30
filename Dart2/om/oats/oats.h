/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _OATS_H__
#define _OATS_H__

#include "common_structs.h"
#include "async_server.h"
#include "dart_order_obj.h"
#include "connection_interface.h"
#include "ex_config_helper.h"

typedef struct oats oats_t;
oats_t *create_oats_struct(con_interface * oci);
void destroy_oats(oats_t* oat);
void init_oats_seq(async_parse_args* pa);
con_interface *create_oats_connection(char *dat_file, int df_len,
                                      gatekeeper * gk);
void parse_oats(int *cut_con, struct async_parse_args *pc);
void oats_rom_helper_func(dart_order_obj * r, void *b);
void create_oats_obj_func(queue_t glob, queue_t q, int num);
char *get_rom_val(const char *message, unsigned long len, int *val_len,
                  int rom_val);
void add_dest_connection(oats_t * oat, char *dest_name, int name_len,
                         databuf_t * buff);
void add_client_connection(oats_t * oat, char *client_name,
                           int name_len, databuf_t * buff);
void remove_dest_connection(oats_t * oat, char *dest_name,
                            int name_len, databuf_t * buff);
void remove_client_connection(oats_t * oat, char *dest_name,
                              int name_len, databuf_t * buff);
void send_oats_update(async_parse_args * ap, int is_connected, char *name,
                      int name_len, int client_or_serv,
                      con_interface * ci);

#endif
