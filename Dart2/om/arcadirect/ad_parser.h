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

int find_ad_field_len(char *start, int max_len);
int create_new_order_message(databuf_t * buff, void *rph, int seq,
                             con_interface * ob);
int create_cancel_message(databuf_t * buff, void *rph, int seq,
                          con_interface * ob);
int create_replace_message(databuf_t * buff, void *rph, int seq,
                           con_interface * ob);
int rom_to_ad_price(char *price, size_t len);
double ad_to_rom_price(int data, char ps);
int ad_translate_rom_symbol(char *oadut, char *symbol, size_t sym_len);
void init_today();
void set_date_str(char *output, long tt);
int parse_order_ack(dart_order_obj * ute, char *ack, con_interface * ob);
int parse_cancel_ack(dart_order_obj * ute, char *ack, con_interface * ob);
int parse_ur_out(dart_order_obj * ute, char *ack, con_interface * ob);
int parse_cancel_rep(dart_order_obj * ute, char *ack, con_interface * ob);
int parse_replaced(dart_order_obj * ute, char *ack, con_interface * ob);
int parse_rej(dart_order_obj * ute, char *ack, con_interface * ob);
int parse_filled(dart_order_obj * ute, char *ack, con_interface * ob);
int parse_ex_filled(dart_order_obj * ute, char *ack, con_interface * ob);

#define CNL_ACK_V1 40
#define KILLED_V1 40
#define KILLED_V2 88
#define CNL_RPL_ACK_V1 40
#define RPL_V1 40
#define CNL_RPL_REJ_V1 80
#define BUST_COR_V1 48
#define ACK_V1 48
#define FILL_V1 88
#define FILL_V2 208

#endif
