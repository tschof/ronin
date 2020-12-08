/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _OUCH_PARSER_H__
#define _OUCH_PARSER_H__

#include "databuf.h"
#include "connection_interface.h"


#define ouch_find_token_len(p, l)        \
    do {                                \
        --l;                            \
    } while(l > 0 && *(p+l) != ' ');       \

int find_ouch_field_len(char *start, int max_len);
int create_new_order_message(databuf_t * buff, void *rph, int seq,
                             con_interface * ob);
int create_cancel_message(databuf_t * buff, void *rph, int seq,
                          con_interface * ob);
int create_replace_message(databuf_t * buff, void *rph, int seq,
                           con_interface * ob);
int rom_to_ouch_price(char *price, size_t len);
double ouch_to_rom_price(int data, char ps);
int ouch_translate_rom_symbol(char *oadut, char *symbol, size_t sym_len);
void init_today();
void set_date_str(char *output, long tt);
int parse_order_ack(dart_order_obj * ute, char *ack, con_interface * ob);
int parse_ur_out(dart_order_obj * ute, char *ack, con_interface * ob);
int parse_replaced(dart_order_obj * ute, char *ack, con_interface * ob);
int parse_rej(dart_order_obj * ute, char *ack, con_interface * ob);
int parse_filled(dart_order_obj * ute, char *ack, con_interface * ob);
int parse_cnl_rej(dart_order_obj* ute, char* ack, con_interface* ob);

#endif
