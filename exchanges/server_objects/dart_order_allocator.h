/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _DART_ORDER_ALLOCATOR_H__
#define _DART_ORDER_ALLOCATOR_H__

#include "message_queue.h"
#if defined(__cplusplus)
extern "C" {
#endif

#define NUM_ORDERS 50000

#define DART_EX_TAG_LEN 64

    typedef struct dart_leg_obj {
        char sec_type[5];
        char symbol[32];
        char call_put[8];
        char expire_date[16];
        char ratio[16];
        char side[8];
        char open_close[8];
        char strike[32];
        char id[DART_EX_TAG_LEN];
        char rom_id[DART_EX_TAG_LEN];
        int chosen_one;
        int is_pending_rep;
        int i_ratio;
        short sec_len;
        short sym_len;
        short cp_len;
        short exp_len;
        short ratio_len;
        short side_len;
        short oc_len;
        short strike_len;
        short id_len;
        short rom_id_len;
    } dart_leg_obj;

    typedef struct dart_obj_mnj dart_obj_manager;

    typedef struct dart_ex_obj dart_ex_order_obj;

    dart_ex_order_obj *get_empty_dart_order_obj(dart_obj_manager * mgr);

    void destroy_dart_order_obj(dart_obj_manager * mgr,
                                dart_ex_order_obj * doj);

    void destroy_dart_order(dart_ex_order_obj * doj);

    queue_t doa_get_children(dart_ex_order_obj * doj);
    void doa_add_child(dart_ex_order_obj * doj, void *child);

    void doa_set_is_spread(dart_ex_order_obj * doj, int is_spread);
    int doa_get_is_spread(dart_ex_order_obj * doj);

    void doa_set_is_user_defined(dart_ex_order_obj * doj, char spread_val);
    int doa_get_is_user_defined(dart_ex_order_obj * doj);
    void doa_set_num_legs(dart_ex_order_obj * doj, int num_legs);
    int doa_get_num_legs(dart_ex_order_obj * doj);

    dart_obj_manager *startup_allocator(int num_objects);

    void clean_dart_oo_pool(dart_obj_manager * mgr);

    void doa_set_order_qty(dart_ex_order_obj * doj, double qty);
    double doa_get_order_qty(dart_ex_order_obj * doj);

    void doa_set_show_amt(dart_ex_order_obj * doj, double qty);
    double doa_get_show_amt(dart_ex_order_obj * doj);

    void doa_set_min_qty(dart_ex_order_obj * doj, double qty);
    double doa_get_min_qty(dart_ex_order_obj * doj);

    void doa_set_sender_name(dart_ex_order_obj * doj,
                             const char *s_name, size_t len);
    char *doa_get_sender_name(dart_ex_order_obj * doj, size_t * len);

    void doa_set_cl_ord_id(dart_ex_order_obj * doj,
                           const void *ord_id, size_t len);
    char *doa_get_cl_ord_id(dart_ex_order_obj * doj, size_t * len);

    void doa_set_rep_ord_id(dart_ex_order_obj * doj,
                            const void *rep_id, size_t len);
    char *doa_get_rep_ord_id(dart_ex_order_obj * doj, size_t * len);

    void doa_set_destination(dart_ex_order_obj * doj,
                             const char *exchange, size_t exchlen);
    char *doa_get_destination(dart_ex_order_obj * doj, size_t * len);

    void doa_set_exchange_tag(dart_ex_order_obj * doj,
                              const char *ex_tag, size_t len);
    char *doa_get_exchange_tag(dart_ex_order_obj * doj, size_t * len);

    void doa_set_rom_id(dart_ex_order_obj * doj,
                        const char *rom_id, size_t len);
    char *doa_get_rom_id(dart_ex_order_obj * doj, size_t * len);

    void doa_set_leaves_shares(dart_ex_order_obj * doj, int leaves);
    int doa_get_leaves_shares(dart_ex_order_obj * doj);

    void doa_set_cum_shares(dart_ex_order_obj * doj, int cum);
    int doa_get_cum_shares(dart_ex_order_obj * doj);

    void doa_set_delta(dart_ex_order_obj * doj, int delta);
    int doa_get_delta(dart_ex_order_obj * doj);

    void doa_set_pending_replace(dart_ex_order_obj * doj, int pr);
    int doa_get_pending_replace(dart_ex_order_obj * doj);

    void doa_set_price(dart_ex_order_obj * doj,
                       const char *price, size_t len);
    char *doa_get_price(dart_ex_order_obj * doj, size_t * len);

    void doa_set_side(dart_ex_order_obj * doj,
                      const char *side, size_t len);
    char *doa_get_side(dart_ex_order_obj * doj, size_t * len);
    void doa_set_rom_symbol(dart_ex_order_obj * doj, const char *sym,
                            size_t len);
    char *doa_get_rom_symbol(dart_ex_order_obj * doj, size_t * len);

    void doa_set_symbol(dart_ex_order_obj * doj,
                        const char *sym, size_t len);
    char *doa_get_symbol(dart_ex_order_obj * doj, size_t * len);

    void doa_set_type(dart_ex_order_obj * doj,
                      const char *type, size_t len);
    char *doa_get_type(dart_ex_order_obj * doj, size_t * len);

    void doa_set_capacity(dart_ex_order_obj * doj,
                          const char *cap, size_t len);
    char *doa_get_capacity(dart_ex_order_obj * doj, size_t * len);

    void doa_set_tif(dart_ex_order_obj * doj, const char *tif, size_t len);
    char *doa_get_tif(dart_ex_order_obj * doj, size_t * len);

    void doa_set_stop_price(dart_ex_order_obj * doj,
                            const char *sp, size_t len);
    char *doa_get_stop_price(dart_ex_order_obj * doj, size_t * len);

    void doa_set_account(dart_ex_order_obj * doj,
                         const char *acc, size_t len);
    char *doa_get_account(dart_ex_order_obj * doj, size_t * len);

    void doa_set_expireMonYear(dart_ex_order_obj * doj,
                               const char *exp, size_t len);
    char *doa_get_expireMonYear(dart_ex_order_obj * doj, size_t * len);

    void doa_set_expire_day(dart_ex_order_obj * doj,
                            const char *exd, size_t len);
    char *doa_get_expire_day(dart_ex_order_obj * doj, size_t * len);

    void doa_set_strike(dart_ex_order_obj * doj,
                        const char *strike, size_t len);
    char *doa_get_strike(dart_ex_order_obj * doj, size_t * len);

    void doa_set_open_close(dart_ex_order_obj * doj,
                            const char *oc, size_t len);
    char *doa_get_open_close(dart_ex_order_obj * doj, size_t * len);

    void doa_set_call_put(dart_ex_order_obj * doj, const char *cp,
                          size_t len);
    char *doa_get_call_put(dart_ex_order_obj * doj, size_t * len);

    void doa_set_sec_type(dart_ex_order_obj * doj, const char *st,
                          size_t len);
    char *doa_get_sec_type(dart_ex_order_obj * doj, size_t * len);

    void doa_set_mm_home(dart_ex_order_obj * doj, const char *st,
                         size_t len);
    char *doa_get_mm_home(dart_ex_order_obj * doj, size_t * len);

    void doa_set_mm_account(dart_ex_order_obj * doj,
                            const char *mm_a, size_t len);
    char *doa_get_mm_account(dart_ex_order_obj * doj, size_t * len);

    void doa_set_cmta(dart_ex_order_obj * doj, const char *cmta,
                      size_t len);
    char *doa_get_cmta(dart_ex_order_obj * doj, size_t * len);

    void doa_set_leg_call_put(dart_ex_order_obj * doj,
                              const char *cp, size_t len,
                              dart_leg_obj * dlj);
    void doa_set_leg_sec_type(dart_ex_order_obj * doj,
                              const char *cp, size_t len,
                              dart_leg_obj * dlj);

    void doa_set_leg_symbol(dart_ex_order_obj * doj,
                            const char *sym, size_t len,
                            dart_leg_obj * dlj);

    void doa_set_leg_expire_date(dart_ex_order_obj * doj,
                                 const char *exp, size_t len,
                                 dart_leg_obj * dlj);

    void doa_set_leg_ratio(dart_ex_order_obj * doj,
                           const char *ratio, size_t len,
                           dart_leg_obj * dlj);

    void doa_set_leg_side(dart_ex_order_obj * doj,
                          const char *side, size_t len,
                          dart_leg_obj * dlj);

    void doa_set_leg_open_close(dart_ex_order_obj * doj,
                                const char *oc, size_t len,
                                dart_leg_obj * dlj);

    void doa_set_leg_strike(dart_ex_order_obj * doj,
                            const char *strike, size_t len,
                            dart_leg_obj * dlj);

    void doa_set_leg_id(dart_ex_order_obj * doj, const char *id,
                        size_t len, dart_leg_obj * dlj);

    void doa_set_leg_rom_id(dart_ex_order_obj * doj,
                            const char *id, size_t len,
                            dart_leg_obj * dlj);

    void doa_set_short_locate(dart_ex_order_obj * doj,
                              const char *const short_locate, size_t len);

    char *doa_get_leg_call_put(dart_ex_order_obj * doj,
                               dart_leg_obj * dlj,int* rlen);
    char *doa_get_leg_sec_type(dart_ex_order_obj * doj,
                               dart_leg_obj * dlj,int* rlen);

    char *doa_get_leg_symbol(dart_ex_order_obj * doj,
            dart_leg_obj * dlj,int* rlen);

    char *doa_get_leg_expire_date(dart_ex_order_obj * doj,
                                  dart_leg_obj * dlj,int* rlen);

    char *doa_get_leg_ratio(dart_ex_order_obj * doj,
            dart_leg_obj * dlj,int* rlen);

    char *doa_get_leg_side(dart_ex_order_obj * doj,
            dart_leg_obj * dlj,int* rlen);

    char *doa_get_leg_open_close(dart_ex_order_obj * doj,
                                 dart_leg_obj * dlj,int* rlen);

    char *doa_get_leg_strike(dart_ex_order_obj * doj,
            dart_leg_obj * dlj,int* rlen);

    char *doa_get_leg_id(dart_ex_order_obj * doj,
            dart_leg_obj * dlj,int* rlen);

    char *doa_get_leg_rom_id(dart_ex_order_obj * doj,
            dart_leg_obj * dlj,int* rlen);

    char *doa_get_rom_id_for_leg(dart_ex_order_obj * doj,
                                 size_t * len, char *id, size_t id_len);

    char *doa_get_short_locate(dart_ex_order_obj * doj, size_t * len);

    void doa_set_parent_id(dart_ex_order_obj * doj,
                           const char *ptag, size_t len);
    char *doa_get_parent_id(dart_ex_order_obj * doj, size_t * len);

    void doa_set_clr_id(dart_ex_order_obj * doj, const char *id,
                        size_t len);
    char *doa_get_clr_id(dart_ex_order_obj * doj, size_t * len);

    char *doa_get_current_status(dart_ex_order_obj * doj, int *len);
    void doa_set_current_status(dart_ex_order_obj * doj, char *status,
                                int len);

    void doa_set_sec_def(dart_ex_order_obj * doj, const char *sym,
                         size_t len);
    char *doa_get_sec_def(dart_ex_order_obj * doj, size_t * len);

    void doa_set_trade_for(dart_ex_order_obj * doj, const char *acc,
                           size_t len);
    char *doa_get_trade_for(dart_ex_order_obj * doj, size_t * len);
    int doa_get_is_manual(dart_ex_order_obj * doj);
    void doa_set_is_manual(dart_ex_order_obj * doj, int is_manual);
    void doa_set_trader(dart_ex_order_obj * doj, const char *acc,
                       size_t len);
    char *doa_get_trader(dart_ex_order_obj * doj, size_t * len);

    int doa_get_temp_cum(dart_ex_order_obj * doj);
    void doa_set_temp_cum(dart_ex_order_obj * doj, int cum);
    void doa_add_to_temp_cum(dart_ex_order_obj* doj, int off_cum);
    void doa_add_to_offset_cum(dart_ex_order_obj* doj, int off_cum);
    int doa_get_offset_cum(dart_ex_order_obj * doj);
    void doa_set_offset_cum(dart_ex_order_obj * doj, int off_cum);
    int doa_get_are_any_legs_pending_rep(dart_ex_order_obj* doj);
    void doa_set_is_pending_rep_for_leg(dart_ex_order_obj* doj,
                                        char* id, size_t id_len,
                                        int is_pending);
    int doa_get_is_pending_rep_for_leg(dart_ex_order_obj* doj,
                                       char* id, size_t id_len);
    void doa_decrement_is_pending_rep_for_leg(dart_ex_order_obj* doj,char* id, size_t id_len);
    void doa_decrement_pending_replace(dart_ex_order_obj* doj);
    void doa_set_pending_cancel(dart_ex_order_obj * doj, int pending);
    int doa_get_pending_cancel(dart_ex_order_obj * doj);
    void doa_decrement_pending_cancel(dart_ex_order_obj * doj);
    int doa_get_is_chosen_n_ratio(dart_ex_order_obj* doj,char* id, size_t id_len, int* ratio);
    void doa_set_spread_head_qty(dart_ex_order_obj * doj, int cum);
    int doa_get_spread_head_qty(dart_ex_order_obj * doj);
    void doa_set_dest_route(dart_ex_order_obj * doj, const char *sym, size_t len);
    char *doa_get_dest_route(dart_ex_order_obj * doj, size_t * len);
    void doa_set_smpid(dart_ex_order_obj * doj, const char *sym, size_t len);
    char* doa_get_smpid(dart_ex_order_obj * doj, size_t* len);
    void doa_set_begin_time(dart_ex_order_obj* doj, char* exp,
                        size_t len);
    void doa_set_end_time(dart_ex_order_obj* doj, char* exp,
                        size_t len);
    char* doa_get_end_time(dart_ex_order_obj* doj);
    char* doa_get_begin_time(dart_ex_order_obj* doj);
#if defined(__cplusplus)
}
#endif
#endif                          //_MAILER_H__
