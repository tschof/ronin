/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "dart_orderbook.h"
#include "fix_rom_trans.h"

static int def_r2f_leg(char *rommsg, dart_fix_pos * xlateinfo,
                       char *mtype, databuf_t * fix,
                       size_t romfield, int *rejected,
                       dart_ex_order_obj * order, dart_leg_obj * l_obj)
{
    return 0;
}

static void build_def_trans(fieldxlatefunc fun, orderbook * ob)
{
    size_t i = 0;
    for (; i < ROM_LEN; ++i) {
        ob->rom2fix[i] = fun;
    }
}

static void build_leg_rf(legxlatefunc fun, orderbook * ob)
{
    size_t i = 0;
    for (; i < ROM_LEN; ++i) {
        ob->r2fleg[i].rom_tag = 0;
        ob->r2fleg[i].legfunc = fun;
    }
}

static void build_def_fixromx(fixromxlatefunc f, orderbook * ob)
{
    size_t i = 0;
    for (; i < ROM_LEN; ++i) {
        ob->fix2rom[i] = f;
    }
}

static int def_spreader(char *fix, size_t len, int rom_tag,
                        databuf_t * buf)
{
    return 0;
}

static void build_def_spread(orderbook * ob)
{
    size_t i = 0;
    for (; i < ROM_LEN; ++i) {
        ob->fix_rom_spreadx[i] = def_spreader;
    }
}

void *copy_rom_parser_helper(void *ph)
{
    rom_parser_helper *orig_rph = (rom_parser_helper *) ph;
    rom_parser_helper *ret = malloc(sizeof(struct rom_parser_helper));
    ret->cur_doj = explicit_doo_create();
    ret->read_bytes = 0;
    ret->mess_bytes = 0;
    ret->d_off = 0;
    ret->message_beg = 0;
    ret->rom_tag = 0;
    ret->parse_v = orig_rph->parse_v;
    return ret;
}

rom_parser_helper *create_rph(void (*parse_v)
                              (long len, struct rom_parser_helper * rph,
                               void *pc))
{
    rom_parser_helper *ret = malloc(sizeof(struct rom_parser_helper));
    ret->cur_doj = explicit_doo_create();
    ret->read_bytes = 0;
    ret->mess_bytes = 0;
    ret->d_off = 0;
    ret->message_beg = 0;
    ret->rom_tag = 0;
    ret->parse_v = parse_v;
    return ret;
}

orderbook *init_order_book(id_gen_func f, is_exch_connected is_con,
                           fieldxlatefunc def_fun,
                           fieldxlatefunc header_func,
                           fixromxlatefunc fix2romdef, int num_orders,
                           const char *module_name, size_t mod_len,
                           const char *fix_name, size_t f_len, void *ass,
                           int is_async)
{
    orderbook *ob = malloc(sizeof(struct ob));
    if (pthread_mutex_init(&ob->mutex, NULL) != 0) {
        printf("Failed to init fix_seq_blocker.\n");
    }
    if (pthread_mutex_init(&ob->send_mutex, NULL) != 0) {
        printf("Failed to init send_mutex.\n");
    }
    ob->mgr = startup_allocator(num_orders);
    ob->cl_ord_id_map = create_map(num_orders);
    ob->rom_ord_id_map = create_map(num_orders);
    ob->module_name = calloc(mod_len + 1, 1);
    ob->module_name_len = mod_len;
    memcpy(ob->module_name, module_name, mod_len);
    ob->generate_id = f;
    ob->is_connected = is_con;
    ob->add_header_defs = header_func;
    ob->fix_con_name = calloc(f_len + 1, 1);
    ob->fix_con_len = f_len;
    build_def_spread(ob);
    build_leg_rf(def_r2f_leg, ob);
    memcpy(ob->fix_con_name, fix_name, f_len);
    build_def_trans(def_fun, ob);
    build_def_fixromx(fix2romdef, ob);
    initalize_maps(ob);
    ob->fix_to_rom_trade_cap = create_map(20);
    ob->send_buffs = create_map(100);
    ob->async_serv_con = ass;
    ob->is_async = is_async;
    ob->can_send_orders = 0;
    ob->no_fix_43 = 0;
    ob->no_spread_head = 0;
    ob->id_last = 0;
    return ob;
}

orderbook *no_order_mng_book(id_gen_func f, is_exch_connected is_con,
                             fieldxlatefunc def_fun,
                             fieldxlatefunc header_func,
                             fixromxlatefunc fix2romdef, int num_orders,
                             const char *module_name, size_t mod_len,
                             const char *fix_name, size_t f_len, void *ass,
                             int is_async)
{
    orderbook *ob = malloc(sizeof(struct ob));
    if (pthread_mutex_init(&ob->mutex, NULL) != 0) {
        printf("Failed to init fix_seq_blocker.\n");
    }
    if (pthread_mutex_init(&ob->send_mutex, NULL) != 0) {
        printf("Failed to init send_mutex.\n");
    }
    ob->mgr = NULL;
    ob->cl_ord_id_map = create_map(num_orders);
    ob->rom_ord_id_map = create_map(num_orders);
    ob->module_name = calloc(mod_len + 1, 1);
    ob->module_name_len = mod_len;
    memcpy(ob->module_name, module_name, mod_len);
    ob->generate_id = f;
    ob->is_connected = is_con;
    ob->add_header_defs = header_func;
    ob->fix_con_name = calloc(f_len + 1, 1);
    ob->fix_con_len = f_len;
    build_def_spread(ob);
    build_leg_rf(def_r2f_leg, ob);
    memcpy(ob->fix_con_name, fix_name, f_len);
    build_def_trans(def_fun, ob);
    build_def_fixromx(fix2romdef, ob);
    initalize_maps(ob);
    ob->fix_to_rom_trade_cap = create_map(20);
    ob->send_buffs = create_map(100);
    ob->async_serv_con = ass;
    ob->is_async = is_async;
    ob->can_send_orders = 0;
    ob->no_fix_43 = 0;
    ob->no_spread_head = 0;
    ob->id_last = 0;
    return ob;
}

orderbook *def_init_order_book()
{
    orderbook *ob = malloc(sizeof(struct ob));
    initalize_maps(ob);
    if (pthread_mutex_init(&ob->mutex, NULL) != 0) {
        printf("Failed to init fix_seq_blocker.\n");
    }
    if (pthread_mutex_init(&ob->send_mutex, NULL) != 0) {
        printf("Failed to init send_mutex.\n");
    }
    ob->cl_ord_id_map = NULL;
    ob->rom_ord_id_map = NULL;
    ob->module_name = NULL;
    ob->module_name_len = 0;
    ob->no_fix_43 = 0;
    ob->no_spread_head = 0;
    ob->id_last = 0;
    return ob;
}
