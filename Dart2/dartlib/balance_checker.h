/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _BALANCE_CHECKER_H__
#define _BALANCE_CHECKER_H__

#include "ex_config_helper.h"
#include "message_queue.h"
#include "dart_order_obj.h"
#include "gatekeeper.h"
#include "risk_cntr_structs.h"

#define FUT_MULT 1000000000
#define EQ_MULT  100000000    //10000
#define EQ_DIV 10000        //100

#define EQ_PRECISION 8
#define FUT_PRECISION 9

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
    long mrate;
    long nmes;
    unsigned long toff;
} jp_limits;

typedef struct jp_limits_checker jp_limits_checker;
typedef struct jp_trader_info jp_trader_info;

void destroy_balance_checker(jp_limits_checker* jplc);
jp_limits_checker *create_limits_checker(ex_config * ex,  otl* o,
        char* mod_name, int mlen, 
        void (*mds_update)(void* cbo, char* sym, int slen, long bid, long offer),
        void (*con_update)(void* cbo, int is_connected));
jp_limits_checker *create_jp_limits_checker(ex_config * ex, otl* o,
                                            char* file, int len);
void load_trader_profiles(jp_limits_checker * jc, const char* trader);
void load_traders_for_dest(jp_limits_checker* jc, const char* mod_name);
void load_clraccnt_limit(jp_limits_checker* jc, const char* key);
void load_billing_group(jp_limits_checker* jc, const char* group);
int validate_user(jp_limits_checker* jc, char* username, int ulen,
        char* password, int plen);
login_info* get_logon_struct(void* checkah, char* p, int plen,
                             char* u, int ulen);
int load_orders(jp_limits_checker * jc, gatekeeper * gk,
                const char *destinations);
jp_limits *get_equity_limits_for_trader(jp_limits_checker * jc,
                                        char *trader, int trader_len);
jp_limits *get_of_limits_for_trader(jp_limits_checker * jc,
                                    char *trader, int len);
void get_future_limits_for_trader(jp_limits_checker * jc,
        char *trader, int len, databuf_t* buff, gatekeeper* gk,
        char* sym, int slen);
jp_limits *get_option_limits_for_trader(jp_limits_checker * jc,
                                        char *trader, int len);
jp_limits* get_equity_limits_for_clraccnt(jp_limits_checker * jc,
                                        char *trader, int len);
jp_limits* get_future_limits_for_clraccnt(jp_limits_checker * jc,
                                        char *trader, int len);
jp_limits* get_fut_opt_limits_for_clraccnt(jp_limits_checker * jc,
                                        char *trader, int len);
jp_limits* get_equity_option_limits_for_clraccnt(jp_limits_checker * jc,
                                        char *trader, int len);
jp_limits*  get_equity_limits_for_bill(jp_limits_checker* jlc, int ikey);
jp_limits*  get_equity_option_limits_for_bill(jp_limits_checker* jlc, int ikey);
jp_limits*  get_future_limits_for_bill(jp_limits_checker* jlc, int ikey);
jp_limits*  get_fut_opt_limits_for_bill(jp_limits_checker* jlc, int ikey);
void set_order_in_interface(jp_limits_checker * jc, gatekeeper * gk,
                            rcntr * cnt,int from_a_rom);

/* Update a traders balance the int price is assumed to be price
 * multiplied by 10000. */
int update_balance(jp_limits_checker * jpc, rcntr* doj);
int update_balance_fill(jp_limits_checker * jpc, rcntr * doj);
int update_balance_new(jp_limits_checker * jpc, rcntr * doj);
int update_balance_replace(jp_limits_checker * jpc, rcntr * doj, rcntr* rep);
int update_balance_rep_rej(jp_limits_checker * jpc, rcntr * doj, rcntr* rep);
void jp_block_trader(jp_limits_checker * jpc, char *trader, int len,
                     int blocked);
void jp_exempt_trader(jp_limits_checker * jpc, char *trader, int len,
                      int exempt);
int check_trader_msg_rate(jp_limits_checker* jpc, char* trader, int tlen, char sec_type);
void jp_start_stop_equity_check(jp_limits_checker* jpc, int should_check);
void jp_exempt_sym(jp_limits_checker* jpc, char* sym, int len, int exempt);
void jiggly_price_converter(rcntr* cnt,
                              jp_limits_checker * jpc,
                              dart_order_obj* doj, char sec_type);
void jp_exempt_billing_group(jp_limits_checker* jpc,int key,
        int exempt);
int ns_check_price(jp_limits_checker* jp, rcntr* rc);
jp_trader_info* jp_get_trader(jp_limits_checker * jpc, char *trader, int len);
int is_blocked(jp_trader_info* jpti);
int is_exempt(jp_trader_info* jpti);
ex_config* create_config_from_db(jp_limits_checker* jc, char* mod_num, int mnl, char* target);
void update_config_from_db(jp_limits_checker* jc, ex_config* ex, char* target);
char* get_mod_name_for_user(ex_config* jc, char* username, int* dlen);
void set_rom_name_from_num(jp_limits_checker* jc, ex_config* ex,
        char* mod_num, int mod_len);
#endif
