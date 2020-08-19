/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _DART_UTP_DIRECT_H__
#define _DART_UTP_DIRECT_H__

#include <pthread.h>

#include "databuf.h"
#include "connection_interface.h"

int find_utp_field_len(char *start, int max_len);
int create_new_order_message(databuf_t * buff, void *rph, int seq,
                             con_interface * ob);
int create_cancel_message(databuf_t * buff, void *rph, int seq,
                          con_interface * ob);
int create_replace_message(databuf_t * buff, void *rph, int seq,
                           con_interface * ob);
int rom_to_utp_price(char *price, size_t len);
double utp_to_rom_price(int data, char ps);
int utp_translate_rom_symbol(char *output, char *symbol, size_t sym_len);
void init_today();
void set_date_str(char *output, int tt);
int parse_order_ack(dart_order_obj * ute, char *ack, con_interface * ob);
int parse_cancel_ack(dart_order_obj * ute, char *ack, con_interface * ob);
int parse_ur_out(dart_order_obj * ute, char *ack, con_interface * ob);
int parse_cancel_rep(dart_order_obj * ute, char *ack, con_interface * ob);
int parse_replaced(dart_order_obj * ute, char *ack, con_interface * ob);
int parse_rej(dart_order_obj * ute, char *ack, con_interface * ob);
int parse_filled(dart_order_obj * ute, char *ack, con_interface * ob);

#endif
