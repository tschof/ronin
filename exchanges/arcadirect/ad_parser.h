/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _DART_UTP_DIRECT_H__
#define _DART_UTP_DIRECT_H__

#if defined(__cplusplus)
extern "C" {
#endif
#include <pthread.h>

#include "databuf.h"
#include "dart_orderbook.h"


    int find_ad_field_len(char *start, int max_len);
    int create_new_order_message(databuf_t * buff, void *rph, int seq,
                                 orderbook * ob);
    int create_var_3_message(databuf_t * buff, void *r, int seq,
                             orderbook * ob);

    int create_cancel_message(databuf_t * buff, void *rph, int seq,
                              orderbook * ob);

    int create_replace_message(databuf_t * buff, void *rph, int seq,
                               orderbook * ob);

    int rom_to_ad_price(char *price, size_t len);

    double ad_to_rom_price(int data, char ps);
    int ad_translate_rom_symbol(char *oadut, char *symbol, size_t sym_len);
    int parse_order_ack(dart_order_obj * ute, databuf_t * buff, char *ack,
                        orderbook * ob);
    void init_today();
    void set_date_str(databuf_t * output, int tt);


    int parse_order_ack(dart_order_obj * ute, databuf_t * buff, char *ack,
                        orderbook * ob);
    int parse_cancel_ack(dart_order_obj * ute, databuf_t * buff, char *ack,
                         orderbook * ob);
    int parse_ur_out(dart_order_obj * ute, databuf_t * buff, char *ack,
                     orderbook * ob);
    int parse_cancel_rep(dart_order_obj * ute, databuf_t * buff, char *ack,
                         orderbook * ob);
    int parse_replaced(dart_order_obj * ute, databuf_t * buff, char *ack,
                       orderbook * ob);
    int parse_rej(dart_order_obj * ute, databuf_t * buff, char *ack,
                  orderbook * ob);
    int parse_filled(dart_order_obj * ute, databuf_t * buff, char *ack,
                     orderbook * ob);
    int parse_ex_filled(dart_order_obj * ute, databuf_t * buff, char *ack,
                        orderbook * ob);

    struct database_conf;

    void ad_database_init(struct database_conf *dbconf, const char *destID,
                          orderbook * ob);

#if defined(__cplusplus)
}
#endif
#endif                          //_CLIENT_MANAGER_H__
