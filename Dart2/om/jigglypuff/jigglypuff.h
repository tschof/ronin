/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _JIGGLY_PUFF_LIMITS_CHECKER_H__
#define _JIGGLY_PUFF_LIMITS_CHECKER_H__

#include "master_config.h"
#include "message_queue.h"
#include "dart_order_obj.h"
#include "gatekeeper.h"
#include "connection_interface.h"
#include "lifecycle_master.h"

#define JP_SEC_TYPE_EQ 0
#define JP_SEC_TYPE_OPT 1
#define JP_SEC_TYPE_FUT 2
#define JP_SEC_TYPE_FX  3

typedef struct jp_limits {
    unsigned long bp;
    unsigned long max_doll;
    long max_ord;
    long max_shares_ord;
    long max_share;
    long cur_balance_buy;
    long cur_balance_sell;
    long open_ords;
    long open_shares;
} jp_limits;

typedef struct jp_limits_checker jp_limits_checker;

void destroy_jiggly(jp_limits_checker* jplc);
jp_limits_checker *create_jp_limits_checker(master_config * ex, char* filename, int flen);
void load_trader_profiles(jp_limits_checker * jc, const char* trader);

//billingrpid == NULL => reload everything(don't loose the current balance!!!)
void load_billinggrp_limit(jp_limits_checker* jc, const char* billinggrpid);

int load_orders(jp_limits_checker * jc, gatekeeper * gk,
                const char *destinations);
queue_t get_allowed_logons(jp_limits_checker * jc);
jp_limits *get_equity_limits_for_trader(jp_limits_checker * jc,
                                        char *trader, int trader_len);
jp_limits *get_of_limits_for_trader(jp_limits_checker * jc,
                                    char *trader, int len);
void get_future_limits_for_trader(jp_limits_checker * jc,
                                        char *trader, int len,
                                        databuf_t* buff, gatekeeper* gk,
                                        char* sym, int slen);
jp_limits *get_option_limits_for_trader(jp_limits_checker * jc,
                                        char *trader, int len);
jp_limits* get_equity_limits_for_billinggrp(jp_limits_checker * jc,
                                        char *trader, int len);
jp_limits* get_future_limits_for_billinggrp(jp_limits_checker * jc,
                                        char *trader, int len);
jp_limits* get_fut_opt_limits_for_billinggrp(jp_limits_checker * jc,
                                        char *trader, int len);
jp_limits* get_equity_option_limits_for_billinggrp(jp_limits_checker * jc,
                                        char *trader, int len);
//void set_order_in_interface(jp_limits_checker * jc, gatekeeper * gk,
//                            dart_order_obj * doj, char *mod_name,
//                            int mod_len);
int check_rom(dart_order_obj * doj, const char *mod_name,
               int mod_name_len);

/* Update a traders balance the int price is assumed to be price
 * multiplied by 10000. */
int update_balance(jp_limits_checker * jpc, dart_order_obj * doj);
int update_balance_fill(jp_limits_checker * jpc, dart_order_obj * doj);
int update_balance_new(jp_limits_checker * jpc, dart_order_obj * doj, int check_vals);
int update_balance_replace(jp_limits_checker * jpc, dart_order_obj * doj);
int update_balance_rep_rej(jp_limits_checker * jpc, dart_order_obj * doj);
void jp_block_trader(jp_limits_checker * jpc, char *trader, int len,
                     int blocked);
void jp_exempt_trader(jp_limits_checker * jpc, char *trader, int len,
                      int exempt);

void jp_exempt_billing_group(jp_limits_checker* jpc,char* trader, int len,
        int exempt);
int load_con_schedule(dest_schedule* ds, jp_limits_checker *jlc, int destid, 
                      const char* modename,  //modename as to OATS
                      int modename_len);
void localtime_to_centraltime_convert(const struct tm* lt, struct tm* ct);
void jp_start_stop_equity_check(jp_limits_checker* jpc, int should_check);
void jp_exempt_sym(jp_limits_checker* jpc, char* sym, int len, int exempt);
int load_nickel_tick(jp_limits_checker* jc);
#endif
