/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _DART_ORDERBOOK_H__
#define _DART_ORDERBOOK_H__

#if defined(__cplusplus)
extern "C" {
#endif
#include <pthread.h>

#include "dart_order_allocator.h"
#include "dart_order_obj.h"
#include "hashmap.h"
#include "databuf.h"
#include "dart_obj_factory.h"
#include "common_structs.h"

    typedef struct dart_rom_pos {
        long beg;
        long end;
    } dart_rom_pos;

    typedef struct fix_tag_def {
        char *name;
        int len;
        int rom_tag;
    } fix_tag_def;

    typedef struct dart_fix_pos romindex_t;

    typedef struct rom_parser_helper {
        dart_order_obj *cur_doj;
        unsigned long read_bytes;
        unsigned long mess_bytes;
        char *d_off;
        char *message_beg;
        int rom_tag;
        void (*parse_v) (long len, struct rom_parser_helper * rph,
                         void *pc);
    } rom_parser_helper;

    typedef char *(*id_gen_func) (size_t * len);

    typedef int (*is_exch_connected) ();

    typedef int (*fieldxlatefunc) (char *rommsg, dart_fix_pos * xlateinfo,
                                   char *mtype, databuf_t * fix,
                                   size_t romfield, int *rejected,
                                   dart_ex_order_obj * order);

    typedef int (*legxlatefunc) (char *rommsg, dart_fix_pos * xlateinfo,
                                 char *mtype, databuf_t * fix,
                                 size_t romfield, int *rejected,
                                 dart_ex_order_obj * order,
                                 dart_leg_obj * l_obj);

    typedef struct legxlater {
        int rom_tag;
        legxlatefunc legfunc;
    } legxlater;

    typedef int (*fixromxlatefunc) (char *fixmsg, dart_rom_pos * xlateinfo,
                                    char mtype, char *status,
                                    databuf_t * rom, size_t romfield,
                                    int *rejtected,
                                    dart_ex_order_obj * order,
                                    pthread_mutex_t * mutex,
                                    hashmap_t map);

    typedef int (*spreadxlatefunc) (char *fixmsg, size_t len, int rom_tag,
                                    databuf_t * rom);

    void *copy_rom_parser_helper(void *ph);

    rom_parser_helper *create_rph(void (*parse_v)
                                   (long len,
                                    struct rom_parser_helper * rph,
                                    void *pc));

    struct exchange_interface;

    typedef struct ob {
        pthread_mutex_t mutex;
        pthread_mutex_t send_mutex;
        dart_obj_manager *mgr;
        hashmap_t cl_ord_id_map;
        hashmap_t rom_ord_id_map;
        hashmap_t fix_to_rom_map_new;
        hashmap_t fix_to_rom_map_cnl;
        hashmap_t fix_to_rom_map_rpl;
        hashmap_t fix_to_rom_map_exe;
        hashmap_t fix_to_rom_map_cnlrej;
        hashmap_t fix_to_rom_trade_cap;
        void *rom_to_fix_status;
        void *rom_to_fix_rpl;
        void *rom_to_fix_cnl;
        void *rom_to_fix_out;
        void *rom_to_fix_leg;
        int leg_array_size;
        id_gen_func generate_id;
        is_exch_connected is_connected;
        fieldxlatefunc rom2fix[ROM_LEN];
        fixromxlatefunc fix2rom[ROM_LEN];
        spreadxlatefunc fix_rom_spreadx[ROM_LEN];
        legxlater r2fleg[ROM_LEN];
        //legxlatefunc r2fleg[ROM_LEN];
        fieldxlatefunc add_header_defs;
        char *module_name;
        size_t module_name_len;
        char *fix_con_name;
        size_t fix_con_len;
        hashmap_t send_buffs;
        void *async_serv_con;
        dof_t *ord_obj;
        dof_t *databufs;
        struct init_args *in_args;
        int is_async;
        int can_send_orders;
        unsigned int seq;
        int no_fix_43;
        int no_spread_head;
        int id_last;
        void *ei;
    } orderbook;

    orderbook *init_order_book(id_gen_func f,
                               is_exch_connected is_con,
                               fieldxlatefunc def_fun,
                               fieldxlatefunc header_fun,
                               fixromxlatefunc fix2romdef,
                               int num_orders,
                               const char *module_name,
                               size_t mod_len,
                               const char *fix_name,
                               size_t f_len, void *ass, int is_async);

    orderbook *def_init_order_book();

    orderbook *no_order_mng_book(id_gen_func f, is_exch_connected is_con,
                                 fieldxlatefunc def_fun,
                                 fieldxlatefunc header_func,
                                 fixromxlatefunc fix2romdef,
                                 int num_orders, const char *module_name,
                                 size_t mod_len, const char *fix_name,
                                 size_t f_len, void *ass, int is_async);

#if defined(__cplusplus)
}
#endif
#endif                          //_CLIENT_MANAGER_H__
