/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

#include "generic_db.h"
#include "ex_config_helper.h"
#include "hashmap.h"
#include "message_queue.h"
#include "common_structs.h"
#include "dart_constants.h"
#include "dart_allocator.h"
#include "dart_order_obj.h"
#include "jigglypuff.h"
#include "gatekeeper.h"
#include "connection_interface.h"
#include "transaction_recorder.h"
#include "debug_logger.h"
#include "mds_client.h"
//#include "lifecycle_master.h"

#define FUT_MULT 100000000
#define EQ_MULT 10000
static int m_codes[26] = {
    0, 0, 0, 0, 0,
    1, 1, 1, 0, 1,
    1, 0, 1, 1, 0,
    0, 1, 0, 0, 0,
    1, 1, 0, 1, 0, 1
};

struct jp_limits_checker {
    char *db_user_name;
    char *db_password;
    char *db_server;
    char *db_inst;
    hashmap_t equity_sym;
    hashmap_t traders;
    hashmap_t clr_billinggrplimits; //used by order update
    hashmap_t billinggrplimits; //used by limit update requested by ADMIN
    hashmap_t opt_fut_ex;
    hashmap_t trd_fut_limits;
    queue_t allowed_logons;
    master_config *mc;
    mdsclient* mdc;
    int check_eq;
    int db_user_name_len;
    int db_password_len;
    int db_server_len;
    int db_inst_len;
    dallocator* clr_maker;
    dallocator* trd_maker;
    dallocator* init_maker;
    pthread_spinlock_t jp_protector;
};

typedef struct mdupdate {
    short exempt;
    long bid;
    long offer;
}mdupdate;
typedef struct jp_billinggrp_limits {
    int id;
    int exempt;
    jp_limits *eq_limits;
    jp_limits *opt_limits;
    jp_limits *fut_limits;
    jp_limits *of_limits;
} jp_billinggrp_limits;

typedef struct jp_trader_info {
    char *name;
    char *password;
    jp_limits *eq_limits;
    jp_limits *opt_limits;
    jp_limits *fut_limits;
    jp_limits *of_limits;
    int blocked;
    int exempt;
    int name_len;
    int pass_len;
} jp_trader_info;

typedef struct jp_fut_limits {
    char name[32];
    char product[32];
    short plen;
    short nlen;
    int mult;
    long max_ord;
    long max_shares_ord;
    long max_share;
    long bp;
    long max_dollars;
    long cur_balance_buy;
    long cur_balance_sell;
    long open_ords;
    long open_shares;
} jp_fut_limits;

static void init_da_args(void* glob)
{
    memset(glob, '\0', 312);
    struct init_args* in = glob;
    in->reset_time = glob + 168;
    in->name = glob + 216;
    in->username = glob + 248;
    in->password = glob + 280;
}

static void init_clr_limits(void* glob)
{
    jp_billinggrp_limits* jpti = glob;
    jpti->eq_limits = glob + 40;
    jpti->opt_limits =  glob + 112;
    jpti->fut_limits = glob + 184;
    jpti->of_limits = glob + 256;
}
static void init_trader_limits(void* glob)
{
    jp_trader_info* jpti = glob;
    jpti->name = glob + sizeof(jp_trader_info);
    jpti->password = glob + 96;
    jpti->eq_limits = glob + 128;
    jpti->opt_limits =  glob + 200;
    jpti->fut_limits = glob + 272;
    jpti->of_limits = glob + 344;
}
static char* get_secstr(char sec)
{
    char* rc = NULL;

    switch(sec)
    {
        case 'E':
            rc = "Equity";
            break;
        case 'O':
            rc = "Option";
            break;
        case 'F':
            rc = "Future";
            break;
        case 'X':
            rc = "Currency";
            break;
        case 'M':
            rc = "Options on Futures";
            break;
    }

    return rc;
}

static long jiggly_price_converter(const char *price,
        jp_limits_checker * jpc,
        dart_order_obj* doj, int is_rep)
{
    double d = strtod(price, NULL);
    char* key = getpval(doj, ROM_DESTID);
    int key_len = getplen(doj, ROM_DESTID);
    long ld = 0;
    if(key_len > 0 && contains_key(jpc->opt_fut_ex, key, key_len)) {
        d += .000000001;
        d = d * FUT_MULT;
        ld = labs((long)d);
        if(is_rep) {
            set_rom_field(((dart_order_obj*)doj->rep),
                    ROM_BIN_PRICE, &ld, sizeof(long));
        } else {
            set_rom_field(doj, ROM_BIN_PRICE, &ld, sizeof(long));
        }
    } else {
        d = d * 10000;
        d += .0000001;
        ld =labs((long)d);
        int id = (int)ld;
        if(is_rep) {
            set_rom_field(((dart_order_obj*)doj->rep),
                    ROM_BIN_PRICE, &id, sizeof(int));
        } else {
            set_rom_field(doj, ROM_BIN_PRICE, &id, sizeof(int));
        }
    }
    return ld;
}

static jp_limits* determine_billinggrp_option_limits(jp_limits_checker * jpc, 
        jp_billinggrp_limits * jpti,
        dart_order_obj* doj)
{
    char* key = getpval(doj, ROM_DESTID);
    int key_len = getplen(doj, ROM_DESTID);
    if(contains_key(jpc->opt_fut_ex, key, key_len)) {
        return jpti->of_limits;
    } else {
        return jpti->opt_limits;
    }
}
static jp_limits *get_limits_for_billinggrp(jp_limits_checker * jc,
        char *key, int len, char sec_type, dart_order_obj* doj, int* grpid,
        int* exempt)
{
    jp_limits* rc = NULL;

    jp_billinggrp_limits* jpti = NULL;
    int found = get_map_entry(jc->clr_billinggrplimits, key, len, (void **) &jpti);
    if (found)
    {
        *grpid = jpti->id;
        *exempt = jpti->exempt;
        switch(sec_type) {
            default:
                rc = jpti->eq_limits;
                break;
            case 'O':
                rc = determine_billinggrp_option_limits(jc, jpti, doj);
                break;
            case 'F':
                rc = jpti->fut_limits;
                break;
        }
    }
    return rc;
}
static jp_limits* determine_option_limits(jp_limits_checker * jpc, 
        jp_trader_info * jpti,
        dart_order_obj* doj)
{
    char* key = getpval(doj, ROM_DESTID);
    int key_len = getplen(doj, ROM_DESTID);
    if(contains_key(jpc->opt_fut_ex, key, key_len)) {
        return jpti->of_limits;
    } else {
        return jpti->opt_limits;
    }
}

static jp_limits *get_limits_for_sec(jp_limits_checker * jpc, 
        jp_trader_info * jpti,
        char sec_type,
        dart_order_obj* doj)
{
    switch (sec_type) {
        case 'E':
            return jpti->eq_limits;
        case 'O':
            return determine_option_limits(jpc, jpti, doj);
        case 'F':
            return jpti->fut_limits;
        default:
            return jpti->eq_limits;
    };
}

static size_t get_symbol_len(char *sym, size_t s_len)
{
    size_t len = s_len;
    char *d_off = sym;
    d_off += (s_len - 1);
    int i = 0;
    for (; i < s_len; ++i) {
        if (sym[i] == 0x20 || sym[i] == 0x2D) {
            d_off = ((sym + i) - 1);
            len = i;
            break;
        }
    }
    i = 0;
    if (*d_off >= 0x30 && *d_off <= 0x39) {
        for (; i < len; ++i, --d_off) {
            if (*d_off < 0x30 || *d_off > 0x39) {
                if (*d_off > 0x5A || *d_off < 0x41
                    || !(m_codes[(*d_off) - 65])) {
                    return s_len;
                }
                len = d_off - sym;
                break;
            }
        }
    }
    return len;
}
static int fut_update(jp_limits_checker* jpc,jp_trader_info * jpti,
                      long shares, long orders,
                      long dollars, long check_vals, int add,
                      dart_order_obj* doj, long np, char side)
{
    int allowed = 1;
    hashmap_t trd_lims = 0;
    jp_fut_limits* jp = 0;
    char errStr[128];
    int err_len = 0;
    int found =
        get_map_entry(jpc->trd_fut_limits, doj->positions[ROM_TRADER].iov_base,
                doj->positions[ROM_TRADER].iov_len, (void **) &trd_lims);
    if(found) {
        char* prod = doj->positions[ROM_SYM].iov_base;
        int prod_len = get_symbol_len(doj->positions[ROM_SYM].iov_base,
                doj->positions[ROM_SYM].iov_len);
        found = get_map_entry(trd_lims, prod, prod_len, (void**)&jp);
        if(found) {
            dollars = (dollars * jp->mult);
            if (check_vals) {
                if (jp->max_ord < (jp->open_ords + orders)) {
                    err_len = sprintf(errStr, "Exceeds Future Max Open Orders");
                    allowed = 0;
                }
                if (jp->max_share < (jp->open_shares + shares)) {
                    err_len = sprintf(errStr, "Exceeds Future Max Open Contracts");
                    allowed = 0;
                }
                if (jp->max_dollars < labs(dollars)) {
                    err_len = sprintf(errStr, "Exceeds Futures Max Dollar per Order");
                    allowed = 0;
                }
                if(side == '1') {
                    if (jp->bp < (jp->cur_balance_buy + dollars)) {
                        err_len = sprintf(errStr, "Exceeds Futures Max Buy Balance");
                        allowed = 0;
                    }
                } else {
                    if (jp->bp < (jp->cur_balance_sell + dollars)) {
                        err_len = sprintf(errStr, "Exceeds Future Max Sell Balance");
                        allowed = 0;
                    }
                }
                if (jp->max_shares_ord < shares) {
                    err_len = sprintf(errStr, "Exceeds Future Max Contracts per Order");
                    allowed = 0;
                }
            }
            if (jpti->exempt) {
                allowed = 1;
            }
        } else {
            allowed = 0;
            char trader[64];
            memset(trader, '\0', 64);
            memcpy(trader, getpval(doj, ROM_TRADER), getplen(doj, ROM_TRADER));
            char p[32];
            memset(p, '\0', 32);
            memcpy(p, prod, prod_len);
            err_len = sprintf(errStr, "%s is not configured for product: %s ",
                    trader, p);
        } 
    } else {
        char trader[64];
        memset(trader, '\0', 64);
        memcpy(trader, getpval(doj, ROM_TRADER), getplen(doj, ROM_TRADER));
        allowed = 0;
        err_len = sprintf(errStr, "%s does not have futures limits set.", trader);
    }
    if(allowed == 0) {
        set_rom_field(doj, ROM_TEXT, errStr, err_len);
    }
    if(jp != NULL && allowed == 1) { 
        if(side == '1') {
            if (add) {
                jp->cur_balance_buy += dollars;
            } else {
                jp->cur_balance_buy -= dollars;
            }
        } else {
            if (add) {
                jp->cur_balance_sell += dollars;
            } else {
                jp->cur_balance_sell -= dollars;
            }
        }
        jp->open_ords += orders;
        jp->open_shares += shares;
    }
    return allowed;
}

static long with_multiplier(long ldollar, dart_order_obj* doj)
{
    double mult = 1;
    if (doj->positions[ROM_BIN_MULT].iov_len > 0)
    {
        memcpy(&mult, doj->positions[ROM_BIN_MULT].iov_base, sizeof(mult));
    }
    return ldollar * mult;
}

void balance_mds_update(void* cbo, char* sym, int slen, long bid, long offer)
{
    mdupdate* temp = 0;
    jp_limits_checker* jp = (jp_limits_checker*)cbo;
    pthread_spin_lock(&jp->jp_protector);
    if(find_n_fill(jp->equity_sym, sym, slen, (void**)&temp)) {
        temp->bid = bid;
        temp->offer = offer;
    } else {
        temp = calloc(1, sizeof(mdupdate));
        temp->exempt = 0;
        temp->bid = bid;
        temp->offer = offer;
        no_copy_insert(jp->equity_sym, sym, slen,temp);
    }
    pthread_spin_unlock(&jp->jp_protector);
        char dsym[32];
        memset(dsym, '\0', 32);
        memcpy(dsym, sym, slen);
        send_debug_message(
                "MDS insert sym: %s, bid: %ld, offer: %ld \n",
                dsym, bid, offer);
}
static int check_price(jp_limits_checker* jp,dart_order_obj* doj, long price, char side)
{
    int allowed = 1;
    mdupdate* mds = 0;
    char* sym = doj->positions[ROM_SYM].iov_base;
    int slen = doj->positions[ROM_SYM].iov_len;
    pthread_spin_lock(&jp->jp_protector);
    int found = get_map_entry(jp->equity_sym, sym, slen, (void**)&mds);
    if(found && !mds->exempt) {
        if(side == '1') {
            allowed = price < mds->offer;
        } else {
            allowed = price > mds->bid;
        }
    }
    pthread_spin_unlock(&jp->jp_protector);
    return allowed;
}


static int base_update(jp_limits_checker * jpc,jp_trader_info * jpti, char sec_type, long shares,
        long orders, long dollars, long check_vals, int add, dart_order_obj * doj, long np, char side)
{
    int allowed = 1;
        
    if(sec_type == 'F') {
        allowed = fut_update(jpc, jpti, shares, orders, dollars, check_vals, add, doj, np, side);
    }
    else {
        dollars = with_multiplier(dollars, doj);
        jp_limits *jp = get_limits_for_sec(jpc, jpti, sec_type, doj);
        if (check_vals) {
            char errStr[128];
            int err_len = 0; 
            if(sec_type == 'E' && jpc->check_eq) {
                if(doj->positions[ROM_TYPE].iov_len > 0 &&
                       ((char*)doj->positions[ROM_TYPE].iov_base)[0] == '2') { 
                    allowed = check_price(jpc, doj, np, side);
                    if(allowed == 0) {
                        char price[32];
                        memset(price, '\0', 32);
                        memcpy(price, doj->positions[ROM_PRICE].iov_base, 
                                doj->positions[ROM_PRICE].iov_len);
                        err_len = sprintf(errStr, "Order Price %s is outside allowed limits",
                                          price);
                    }
                }
            } 
            if (jp->max_ord < (jp->open_ords + orders)) {
                err_len = sprintf(errStr, "Exceeds %s Max Open Orders", get_secstr(sec_type));
                allowed = 0;
            }
            if (jp->max_share < (jp->open_shares + shares)) {
                err_len = sprintf(errStr, "Exceeds %s Max Open Shares", get_secstr(sec_type));
                allowed = 0;
            }
            if (jp->max_doll < labs(dollars)) {
                err_len = sprintf(errStr, "Exceeds %s Max Dollar per Order", get_secstr(sec_type));
                allowed = 0;
            }
            if(side == '1') {
                if (jp->bp < (jp->cur_balance_buy + dollars)) {
                    err_len = sprintf(errStr, "Exceeds %s Max Balance", get_secstr(sec_type));
                    allowed = 0;
                }
            } else {
                if (jp->bp < (jp->cur_balance_sell + dollars)) {
                    err_len = sprintf(errStr, "Exceeds %s Max Balance", get_secstr(sec_type));
                    allowed = 0;
                }
            }
            if (jp->max_shares_ord < shares) {
                err_len = sprintf(errStr, "Exceeds %s Max Shares per Order", get_secstr(sec_type));
                allowed = 0;
            }
            if(sec_type == 'E') {
                long a = np % 100;
                if(np > 10000 && a > 0) {
                    err_len = sprintf(errStr, "Price has incorrect decimalization.");
                    allowed = 0;
                }
            }
            if (jpti->exempt) {
                allowed = 1;
            } else if (allowed == 0) {
                set_rom_field(doj, ROM_TEXT, errStr, err_len);
                return allowed;
            }
        }
        if(side == '1') {
            if (add) {
                jp->cur_balance_buy += dollars;
            } else {
                jp->cur_balance_buy -= dollars;
            }
        } else {
            if (add) {
                jp->cur_balance_sell += dollars;
            } else {
                jp->cur_balance_sell -= dollars;
            }
        }
        jp->open_ords += orders;
        jp->open_shares += shares;
    }
    return allowed;
}

static void rewind_update(jp_limits_checker* jpc, jp_trader_info * jpti, char sec_type, long shares,
        long orders, long dollars, long check_vals, int add, dart_order_obj * doj, char side)
{
    jp_limits *jp = get_limits_for_sec(jpc, jpti, sec_type, doj);
    if(jp) {
        if(side == '1') {
            jp->cur_balance_buy -= dollars;
        } else {
            jp->cur_balance_sell -= dollars;
        }
        jp->open_ords -= orders;
        jp->open_shares -= shares;
    }
}


static int billinggrp_update(jp_limits_checker * jpti, char* key, int key_len, char sec_type, long shares,
        long orders, long dollars, long check_vals, int add, dart_order_obj * doj, char side)
{
    int allowed = 1;
    int grpid = -1;
    int exempt = 0;
    jp_limits *jp = get_limits_for_billinggrp(jpti, key, key_len, sec_type, doj, &grpid, &exempt);

    if ('F' == sec_type)
    {
        hashmap_t trd_lims = 0;
        jp_fut_limits* jp = 0;
        int found =
        get_map_entry(jpti->trd_fut_limits, doj->positions[ROM_TRADER].iov_base,
                doj->positions[ROM_TRADER].iov_len, (void **) &trd_lims);
        if(found) 
        {
            char* prod = doj->positions[ROM_SYM].iov_base;
            int prod_len = get_symbol_len(doj->positions[ROM_SYM].iov_base,
                    doj->positions[ROM_SYM].iov_len);
            found = get_map_entry(trd_lims, prod, prod_len, (void**)&jp);
            if(found) 
            {
                dollars = (dollars * jp->mult);
            }
        }
    }

    if (check_vals) {
        char errStr[128];
        int err_len = 0; 
        if (jp)
        {
            if (jp->max_ord < (jp->open_ords + orders)) {
                err_len = sprintf(errStr, "Exceeds BillingGrp(id=%d) limits for Sec(%c) Max Open Orders", grpid, sec_type);
                allowed = 0;
            }
            if (jp->max_share < (jp->open_shares + shares)) {
                err_len = sprintf(errStr, "Exceeds BillingGrp limits for sec (%c) Max Open Shares", sec_type);
                allowed = 0;
            }
            if (jp->max_doll < labs(dollars)) {
                err_len = sprintf(errStr, "Exceeds BillingGrp(id=%d) limits for Sec(%c) Max Dollar per Order", grpid, sec_type);
                allowed = 0;
            }
            if (side == '1') {
                if (jp->bp < labs(jp->cur_balance_buy + dollars)) {
                    err_len = sprintf(errStr, "Exceeds BillingGrp(id=%d) limits for Sec(%c) Max Balance",grpid, sec_type);
                    allowed = 0;
                }
            } else {
                if (jp->bp < labs(jp->cur_balance_sell + dollars)) {
                    err_len = sprintf(errStr, "Exceeds BillingGrp(id=%d) limits for Sec(%c) Max Balance", grpid, sec_type);
                    allowed = 0;
                }
            }
            if (jp->max_shares_ord < shares) {
                err_len = sprintf(errStr, "Exceeds BillingGrp(id=%d) limits for Sec(%c) Max Open Orders",grpid,  sec_type );
                allowed = 0;
            }

            if (!allowed)
            {
                set_rom_field(doj, ROM_TEXT, errStr, err_len);
            }
        }
    }
    if(exempt) {
        allowed = 1;
    }

    if (allowed)
    {
        if (jp)
        {
            if(side == '1') {
                if (add) {
                    jp->cur_balance_buy += dollars;
                } else {
                    jp->cur_balance_buy -= dollars;
                }
            } else {
                if (add) {
                    jp->cur_balance_sell += dollars;
                } else {
                    jp->cur_balance_sell -= dollars;
                }
            }
            jp->open_ords += orders;
            jp->open_shares += shares;
        }
    }

    return allowed;
}

void jp_block_trader(jp_limits_checker * jpc, char *trader, int len,
        int blocked)
{
    jp_trader_info *jpti = 0;
    int found = get_map_entry(jpc->traders, trader, len, (void **) &jpti);
    if (found) {
        jpti->blocked = blocked;
    }
}
void jp_exempt_sym(jp_limits_checker* jpc, char* sym, int len, int exempt)
{
    pthread_spin_lock(&jpc->jp_protector);
    mdupdate *jpti = 0;
    int found = get_map_entry(jpc->equity_sym, sym, len, (void **) &jpti);
    if (found) {
        jpti->exempt = exempt;
    }
    pthread_spin_unlock(&jpc->jp_protector);
}

void jp_start_stop_equity_check(jp_limits_checker* jpc, int should_check)
{
    jpc->check_eq = should_check;
}

void jp_exempt_trader(jp_limits_checker * jpc, char *trader, int len,
        int exempt)
{
    jp_trader_info *jpti = 0;
    int found = get_map_entry(jpc->traders, trader, len, (void **) &jpti);
    if (found) {
        jpti->exempt = exempt;
    }
}
void jp_exempt_billing_group(jp_limits_checker* jpc,char* trader, int  len,
        int exempt)
{
    jp_billinggrp_limits *jpti = 0;
    int found = get_map_entry(jpc->billinggrplimits, trader, len, (void **) &jpti);
    if (found) {
        jpti->exempt = exempt;
    }
}

jp_limits *get_of_limits_for_trader(jp_limits_checker * jc,
        char *trader, int len)
{
    jp_trader_info *jpti = 0;
    int found = get_map_entry(jc->traders, trader, len, (void **) &jpti);
    if (found) {
        return jpti->of_limits;
    }
    return NULL;
}

static void build_fut_limits_message(gatekeeper* gk,
        struct jp_fut_limits *limits, char type,
        char *trader,
        int trader_len, databuf_t* buff)
{
    int mod_len = 0;
    double mbp = (double) limits->bp / FUT_MULT;
    double mdol = (double) limits->max_dollars / FUT_MULT;
    unsigned long lcb = labs(limits->cur_balance_buy);
    double cdol = (double) lcb / FUT_MULT;
    double cdols = (double) (limits->cur_balance_sell / FUT_MULT);
    databuf_write(buff, "J,,24,");
    databuf_memcpy(buff, trader, trader_len);
    databuf_write(buff,
            ",,%s,%s,,,,,,,%ld,%ld,%ld,%ld,%f,%f|%f,%ld,%f,,,,,,,,,,,,,,,%c,,,,,,,,,,,,\n",
            limits->product, get_mod_name(gk, &mod_len), limits->max_ord,
            limits->open_ords, limits->max_share,
            limits->open_shares, mbp, cdol, cdols, limits->max_shares_ord,
            mdol, type);
}
static void print_fut_limits(gatekeeper* gk, hashmap_t futs, char* trader,
                             int tlen, char* sym, int slen, databuf_t* buff)
{
    jp_fut_limits* rlf;
    char *key = 0;
    int key_len = 0;
    hashmap_iterator_t hi = hashmap_iterator_begin(futs);
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                                     (void *) &rlf);
    while (valid) {
        build_fut_limits_message(gk, rlf, 'F', trader, tlen, buff);
        valid = hashmap_iterator_next(futs, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &rlf);
    }
    hashmap_iterator_destroy(hi);
}
void get_future_limits_for_trader(jp_limits_checker * jc,
        char *trader, int len, databuf_t* buff, gatekeeper* gk,
        char* sym, int slen)
{
    hashmap_t futs = 0;
    int found = get_map_entry(jc->trd_fut_limits, trader, len, (void **) &futs);
    if (found && slen <= 0) {
        print_fut_limits(gk, futs, trader, len, sym, slen, buff);
    } else if(found && slen > 0) {
        jp_fut_limits* jfp = 0;
        found = get_map_entry(futs, sym, slen, (void**)&jfp);
        if(found) {
            build_fut_limits_message(gk, jfp, 'F', trader, len, buff);
        }
    }
    if(found == 0) {
        int mod_len = 0;
        databuf_write(buff, "J,,24,");
        databuf_memcpy(buff, trader, len);
        if(slen > 0) {
            databuf_memcpy(buff, ",,", 2);
            databuf_memcpy(buff, sym, slen);
            databuf_write(buff,",%s,,,,,,,0,0,0,,,,,,,,,,,,,,,,,,,,F,,,,,,,,,,,,\n",
                          get_mod_name(gk, &mod_len)); 
        } else {
            databuf_write(buff,",,,%s,,,,,,,0,0,0,,,,,,,,,,,,,,,,,,,,F,,,,,,,,,,,,\n",
                          get_mod_name(gk, &mod_len)); 
        }
    }
}

jp_limits *get_option_limits_for_trader(jp_limits_checker * jc,
        char *trader, int len)
{
    jp_trader_info *jpti = 0;
    int found = get_map_entry(jc->traders, trader, len, (void **) &jpti);
    if (found) {
        return jpti->opt_limits;
    }
    return NULL;
}
jp_limits* get_equity_limits_for_billinggrp(jp_limits_checker * jc,
        char *key, int len)
{
    jp_billinggrp_limits* jpti = NULL;
    int found = get_map_entry(jc->billinggrplimits, key, len, (void **) &jpti);
    if (found) {
        return jpti->eq_limits;
    }
    return NULL;
}
jp_limits* get_future_limits_for_billinggrp(jp_limits_checker * jc,
        char *key, int len)
{
    jp_billinggrp_limits* jpti = NULL;
    int found = get_map_entry(jc->billinggrplimits, key, len, (void **) &jpti);
    if (found) {
        return jpti->fut_limits;
    }
    return NULL;
}
jp_limits* get_fut_opt_limits_for_billinggrp(jp_limits_checker * jc,
        char *key, int len)
{
    jp_billinggrp_limits* jpti = NULL;
    int found = get_map_entry(jc->billinggrplimits, key, len, (void **) &jpti);
    if (found) {
        return jpti->of_limits;
    }
    return NULL;
}
jp_limits* get_equity_option_limits_for_billinggrp(jp_limits_checker * jc,
        char *key, int len)
{
    jp_billinggrp_limits* jpti = NULL;
    int found = get_map_entry(jc->billinggrplimits, key, len, (void **) &jpti);
    if (found) {
        return jpti->opt_limits;
    }
    return NULL;
}


jp_limits *get_equity_limits_for_trader(jp_limits_checker * jc,
        char *trader, int len)
{
    jp_trader_info *jpti = 0;
    int found = get_map_entry(jc->traders, trader, len, (void **) &jpti);
    if (found) {
        return jpti->eq_limits;
    }
    return NULL;
}

int update_balance(jp_limits_checker * jpc, dart_order_obj * doj)
{
    int allow = 1;
    int sign = 0;
    jp_trader_info *jpti = 0;
    int found =
        get_map_entry(jpc->traders, doj->positions[ROM_TRADER].iov_base,
                doj->positions[ROM_TRADER].iov_len, (void **) &jpti);
    if (found) {
        char *side = doj->positions[ROM_SIDE].iov_base;
        if (side[0] == '1') {
            sign = 0;
        }
        char *sec_type = "E";
        if (doj->positions[ROM_SEC_TYPE].iov_len > 0) {
            sec_type = doj->positions[ROM_SEC_TYPE].iov_base;
        }
        long price = 0;
        int shares = 0;
        long dollar = 0;
        if (doj->positions[ROM_BIN_PRICE].iov_len <= 0) {
            jiggly_price_converter(doj->positions[ROM_PRICE].iov_base,
                    jpc, doj, 0);
        }
        if (doj->positions[ROM_BIN_QTY].iov_len <= 0) {
            int net_qty =
                strtol(doj->positions[ROM_SHARES].iov_base, NULL, 10);
            set_rom_field(doj, ROM_BIN_QTY, &net_qty, sizeof(int));
        }
        if (doj->positions[ROM_BIN_CUM].iov_len <= 0) {
            int net_qty =
                strtol(doj->positions[ROM_CUM].iov_base, NULL, 10);
            set_rom_field(doj, ROM_BIN_CUM, &net_qty, sizeof(int));
        }
        memcpy(&price, doj->positions[ROM_BIN_PRICE].iov_base, getplen(doj, ROM_BIN_PRICE));
        memcpy(&shares, doj->positions[ROM_BIN_QTY].iov_base, 4);
        int cum = 0;
        memcpy(&cum, doj->positions[ROM_BIN_CUM].iov_base, 4);
        dollar = (price * (shares - cum));
        shares -= cum;
        allow = base_update(jpc, jpti, sec_type[0], (shares * -1), -1, dollar,
                0, sign, doj, price, side[0]);
        if(allow) { 
            allow = billinggrp_update(jpc, getpval(doj, ROM_CLR_ACC), getplen(doj, ROM_CLR_ACC),
                    sec_type[0],  (shares * -1), -1, dollar, 0, sign, doj, side[0]);
        }
    }
    return allow;
}

int update_balance_fill(jp_limits_checker * jpc, dart_order_obj * doj)
{
    int allow = 1;
    int sign = 0;
    jp_trader_info *jpti = 0;
    int found =
        get_map_entry(jpc->traders, doj->positions[ROM_TRADER].iov_base,
                doj->positions[ROM_TRADER].iov_len, (void **) &jpti);
    if (found) {
        char *side = doj->positions[ROM_SIDE].iov_base;
        if (side[0] == '1') {
            sign = 0;
        }
        char *sec_type = "E";
        if (doj->positions[ROM_SEC_TYPE].iov_len > 0) {
            sec_type = doj->positions[ROM_SEC_TYPE].iov_base;
        }
        long price = 0;
        int shares = 0;
        long dollar = 0;
        int orders = 0;
        if (doj->positions[ROM_BIN_PRICE].iov_len <= 0) {
            jiggly_price_converter(doj->positions[ROM_PRICE].iov_base,
                    jpc, doj, 0);
        }
        if (doj->positions[ROM_BIN_QTY].iov_len <= 0) {
            int net_qty =
                strtol(doj->positions[ROM_SHARES].iov_base, NULL, 10);
            set_rom_field(doj, ROM_BIN_QTY, &net_qty, sizeof(int));
        }
        if (doj->positions[ROM_BIN_CUM].iov_len <= 0) {
            int net_qty =
                strtol(doj->positions[ROM_CUM].iov_base, NULL, 10);
            set_rom_field(doj, ROM_BIN_CUM, &net_qty, sizeof(int));
        }
        memcpy(&price, doj->positions[ROM_BIN_PRICE].iov_base, getplen(doj, ROM_BIN_PRICE));
        memcpy(&shares, doj->positions[ROM_BIN_LAST_SHARES].iov_base, 4);
        dollar = (price * shares);
        char *status = doj->positions[ROM_STATUS].iov_base;
        if (status[0] == '2') {
            orders = -1;
        }
        allow = base_update(jpc, jpti, sec_type[0], (shares * -1), orders,
                dollar, 0, sign, doj, price, side[0]);

        if(allow) { 
            allow = billinggrp_update(jpc, getpval(doj, ROM_CLR_ACC), getplen(doj, ROM_CLR_ACC),
                    sec_type[0], (shares * -1), orders, dollar, 0, sign, doj, side[0]);
        }
    }
    return allow;
}

int update_balance_new(jp_limits_checker * jpc, dart_order_obj * doj, int check_vals)
{
    int allow = 0;
    int sign = 1;
    jp_trader_info *jpti = 0;
    int found =
        get_map_entry(jpc->traders, doj->positions[ROM_TRADER].iov_base,
                doj->positions[ROM_TRADER].iov_len, (void **) &jpti);
    if (found) {
        if (check_vals && jpti->blocked) {
            databuf_t *buff = databuf_alloc(128);
            databuf_memcpy(buff, doj->positions[ROM_SENDER].iov_base,
                    doj->positions[ROM_SENDER].iov_len);
            databuf_memcpy(buff, " is Blocked", 11);
            set_rom_field(doj, ROM_TEXT, buff->buffer, databuf_unread(buff));
            databuf_destroy(buff);
            return 0;
        }
        char *side = doj->positions[ROM_SIDE].iov_base;
        if (side[0] == '1') {
            sign = 1;
        }
        long np =
            jiggly_price_converter(doj->positions[ROM_PRICE].iov_base, jpc, doj, 0);
        int net_qty =
            strtol(doj->positions[ROM_SHARES].iov_base, NULL, 10);
        set_rom_field(doj, ROM_BIN_QTY, &net_qty, sizeof(int));
        
        if(doj->positions[ROM_BIN_CUM].iov_len <= 0) {
            int cum = 0;
            set_rom_field(doj, ROM_BIN_CUM, &cum, sizeof(int));
        } 
        long dollar = np * net_qty;
        char *sec_type = "E";
        if (doj->positions[ROM_SEC_TYPE].iov_len > 0) {
            sec_type = doj->positions[ROM_SEC_TYPE].iov_base;
        }
        allow =
            base_update(jpc, jpti, sec_type[0], net_qty, 1, dollar, check_vals, sign, doj, np, side[0]);

        if (allow) {
            allow = billinggrp_update(jpc, getpval(doj, ROM_CLR_ACC), getplen(doj, ROM_CLR_ACC),
                    sec_type[0], net_qty, 1, dollar, check_vals, sign, doj, side[0]);
            if(!allow) {
                rewind_update(jpc, jpti, sec_type[0], net_qty, 1, dollar, check_vals, sign, doj, side[0]);
            }
        }
    }
    return allow;
}

int update_balance_replace(jp_limits_checker * jpc, dart_order_obj * doj)
{
    int allow = 0;
    int sign = 1;
    jp_trader_info *jpti = 0;
    int found =
        get_map_entry(jpc->traders, doj->positions[ROM_TRADER].iov_base,
                doj->positions[ROM_TRADER].iov_len, (void **) &jpti);
    if (found) {
        if (jpti->blocked) {
            return 0;
        }
        char *side = doj->positions[ROM_SIDE].iov_base;
        if (side[0] == '1') {
            sign = 1;
        }
        long dollar = 0;
        long orig_price = 0;
        long np = 0;
        int leave = 0;
        int new_qty = 0;
        int old_qty = 0;
        int cum = 0;
        memcpy(&orig_price, doj->positions[ROM_BIN_PRICE].iov_base, getplen(doj, ROM_BIN_PRICE));
        memcpy(&old_qty, doj->positions[ROM_BIN_QTY].iov_base, 4);
        memcpy(&cum, doj->positions[ROM_BIN_CUM].iov_base, 4);
        leave = old_qty - cum;
        if (((dart_order_obj *) doj->rep)->positions[ROM_PRICE].iov_len >
                0) {
            np = jiggly_price_converter(((dart_order_obj *) doj->
                        rep)->positions[ROM_PRICE].
                    iov_base, jpc, doj, 1);
        } else {
            np = orig_price;
        }
        memcpy(&orig_price, doj->positions[ROM_BIN_PRICE].iov_base, 4);
        if (((dart_order_obj *) doj->rep)->positions[ROM_SHARES].iov_len >
                0) {
            new_qty =
                strtol(((dart_order_obj *) doj->
                            rep)->positions[ROM_SHARES].iov_base, NULL, 10);
            set_rom_field(((dart_order_obj *) doj->rep), ROM_BIN_QTY,
                    &new_qty, sizeof(int));
        } else {
            new_qty = old_qty;
        }
        dollar = ((np - orig_price) * leave) + ((new_qty - old_qty) * np);
        char *sec_type = "E";
        if (doj->positions[ROM_SEC_TYPE].iov_len > 0) {
            sec_type = doj->positions[ROM_SEC_TYPE].iov_base;
        }
        allow =
            base_update(jpc, jpti, sec_type[0], (new_qty - old_qty), 0, dollar,
                    1, sign, doj,np, side[0]);

        if (allow) {
            allow = billinggrp_update(jpc, getpval(doj, ROM_CLR_ACC), getplen(doj, ROM_CLR_ACC),
                    sec_type[0], (new_qty - old_qty), 0, dollar, 1, sign, doj, side[0]);
            if(!allow) {
                rewind_update(jpc,jpti, sec_type[0], (new_qty - old_qty), 0, dollar, 
                        1, sign, doj, side[0]);
            }
        }
    }
    return allow;
}

int update_balance_rep_rej(jp_limits_checker * jpc, dart_order_obj * doj)
{
    int allow = 1;
    int sign = 0;
    jp_trader_info *jpti = 0;
    int found =
        get_map_entry(jpc->traders, doj->positions[ROM_TRADER].iov_base,
                doj->positions[ROM_TRADER].iov_len, (void **) &jpti);
    if (found) {
        char *side = doj->positions[ROM_SIDE].iov_base;
        if (side[0] == '1') {
            sign = 0;
        }
        char *sec_type = "E";
        if (doj->positions[ROM_SEC_TYPE].iov_len > 0) {
            sec_type = doj->positions[ROM_SEC_TYPE].iov_base;
        }
        long dollar = 0;
        int orig_price = 0;
        long np = 0;
        int leave = 0;
        int new_qty = 0;
        int old_qty = 0;
        int cum = 0;
        memcpy(&orig_price, doj->positions[ROM_BIN_PRICE].iov_base, 4);
        memcpy(&old_qty, doj->positions[ROM_BIN_QTY].iov_base, 4);
        memcpy(&cum, doj->positions[ROM_BIN_CUM].iov_base, 4);
        leave = old_qty - cum;
        if (((dart_order_obj *) doj->rep)->positions[ROM_PRICE].iov_len >
                0) {
            np = jiggly_price_converter(((dart_order_obj *) doj->
                        rep)->positions[ROM_PRICE].
                    iov_base, jpc, doj, 1);
        } else {
            np = orig_price;
        }
        if (((dart_order_obj *) doj->rep)->positions[ROM_SHARES].iov_len >
                0) {
            new_qty =
                strtol(((dart_order_obj *) doj->
                            rep)->positions[ROM_SHARES].iov_base, NULL, 10);
            set_rom_field(((dart_order_obj *) doj->rep), ROM_BIN_QTY,
                    &new_qty, sizeof(int));
        } else {
            new_qty = old_qty;
        }
        dollar = ((np - orig_price) * leave) + ((new_qty - old_qty) * np);
        allow =
            base_update(jpc, jpti, sec_type[0], ((new_qty - old_qty) * -1), 0,
                    dollar, 0, sign, doj, np, side[0]); 
        if (allow)
        {
            allow = billinggrp_update(jpc, getpval(doj, ROM_CLR_ACC), getplen(doj, ROM_CLR_ACC),
                    sec_type[0], ((new_qty - old_qty) * -1), 0, dollar, 0, sign, doj,side[0]);
        }
    }
    return allow;
}

queue_t get_allowed_logons(jp_limits_checker * jc)
{
    return jc->allowed_logons;
}

static unsigned long gen_build_off(char *data, unsigned long size,
        unsigned long byte_offset,
        unsigned long *seq_num, void* book)
{
    return size;
}

static char *gen_resend_pad(unsigned long seq, char *rom, size_t * len)
{
    return NULL;
}

static void reset_seq(struct init_args *in_args)
{
    sequence_req *sr =
        create_sequence_request(in_args->name, in_args->name_len,
                in_args,
                gen_build_off,
                gen_resend_pad, in_args);
    clean_seq(sr);
}

static void set_reset_time(struct init_args *in, master_config * mc)
{
    in->reset_time->reset_hour = mc->reset_hour;
    in->reset_time->reset_min = mc->reset_min;
    in->reset_time->create_new = mc->create_new;
    in->reset_time->new_incoming = 0;
    in->reset_time->new_outgoing = 0;
    in->reset_time->reset_day = 7;
    in->reset_time->last_logon = 0;
    if (mc->create_new) {
        reset_seq(in);
    }
}

static void set_engine_logons(jp_limits_checker * jc)
{
    int i, ret_len;
    for (i = 0; i < 200; ++i) {
        struct init_args *in = get_stuff(jc->init_maker);
        ret_len = sprintf(in->name, "ROM%d", i);
        in->name_len = ret_len;

        ret_len = sprintf(in->username, "ROM%d", i);
        in->un_len = ret_len;

        ret_len = sprintf(in->password, "ROM%d", i);
        in->p_len = ret_len;
        in->do_balance = 0;
        set_reset_time(in, jc->mc);
        enqueue(jc->allowed_logons, in);
    }
}

static void clean_equity_sym(jp_limits_checker* jpc)
{
    if(jpc && jpc->equity_sym) {
    //pthread_spin_lock(&jpc->jp_protector);
    mdupdate *rlf = 0;
    char *key = 0;
    int key_len = 0;
    hashmap_iterator_t hi = hashmap_iterator_begin(jpc->equity_sym);
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                                     (void *) &rlf);
    while (valid) {
        free(rlf);
        rlf = 0;
        valid = hashmap_iterator_next(jpc->equity_sym, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &rlf);
    }
    hashmap_iterator_destroy(hi);
    }
    //pthread_spin_unlock(&jpc->jp_protector);
}
void destroy_jiggly(jp_limits_checker* jplc)
{
    delete_map(jplc->traders);
    delete_map(jplc->clr_billinggrplimits);
    delete_map(jplc->billinggrplimits);
    destroy_queue(jplc->allowed_logons);
    free(jplc->db_user_name);
    free(jplc->db_password);
    free(jplc->db_server);
    free(jplc->db_inst);
    destroy_mem_glob(jplc->trd_maker);
    destroy_mem_glob(jplc->clr_maker);
    destroy_mem_glob(jplc->init_maker);
    if(jplc->mdc) {
        clean_equity_sym(jplc);
        close_mds_client(jplc->mdc);
    }
    free(jplc);
}

void update_equity_sym_chart(void* me, int is_connected)
{
    
    jp_limits_checker* jp = (jp_limits_checker*)me;
    pthread_spin_lock(&jp->jp_protector);
    if(!is_connected) {
        jp->check_eq = 0;
    } else {
        jp->check_eq = 1;
    }
    pthread_spin_unlock(&jp->jp_protector);
    send_debug_message("Check Equity? %d \n", jp->check_eq);
}
jp_limits_checker *create_jp_limits_checker(master_config * ex, char* filename, int flen)
{
    jp_limits_checker *jplc = malloc(sizeof *jplc);
    jplc->traders = create_map(256);
    jplc->clr_billinggrplimits = create_map(256);
    jplc->billinggrplimits = create_map(256);
    jplc->opt_fut_ex = create_map(32);
    jplc->allowed_logons = create_sized_queue(256);
    jplc->mc = ex;
    jplc->check_eq = 0;
    pthread_spin_init(&jplc->jp_protector, PTHREAD_PROCESS_PRIVATE);
    jplc->equity_sym = create_map(8192);
    jplc->trd_fut_limits = create_map(256);
    jplc->trd_maker = create_mem_glob(416, init_trader_limits);
    jplc->clr_maker = create_mem_glob(328, init_clr_limits);
    jplc->init_maker = create_mem_glob(312, init_da_args);
    jplc->db_user_name = calloc(ex->db_user_name_len + 1, 1);
    memcpy(jplc->db_user_name, ex->db_user_name, ex->db_user_name_len);
    jplc->db_user_name_len = ex->db_user_name_len;
    jplc->db_password = calloc(ex->db_password_len + 1, 1);
    memcpy(jplc->db_password, ex->db_password, ex->db_password_len);
    jplc->db_password_len = ex->db_password_len;
    jplc->db_server = calloc(ex->db_server_len + 1, 1);
    memcpy(jplc->db_server, ex->db_server, ex->db_server_len);
    jplc->db_server_len = ex->db_server_len;
    jplc->db_inst = calloc(ex->db_inst_len + 1, 1);
    memcpy(jplc->db_inst, ex->db_inst, ex->db_inst_len);
    jplc->db_inst_len = ex->db_inst_len;
    load_trader_profiles(jplc, NULL);
    load_billinggrp_limit(jplc, NULL);
    set_engine_logons(jplc);
    if(flen > 0) {
        jplc->mdc  = create_mds_client(filename, flen, jplc, 
                                       balance_mds_update,update_equity_sym_chart);
    } else {
        jplc->check_eq = 0;
        jplc->mdc = 0;
    }
    return jplc;
}

static void load_future_limits(jp_limits_checker* jc, const char* trader)
{
    queue_t q = create_sized_queue(256);
    int res = 0;
    char sqlstr[2048];

    if (trader) {
        sprintf(sqlstr, "SELECT l.username, l.Product, l.MaxContracts, "
                "l.TotalMaxContracts, l.MaxOrders, l.MaxDollars, "
                "l.BuyingPower, f.Multiplier FROM Future f, "
                "FutureLimits l WHERE l.Product = f.Product AND "
                "l.username = '%s'", trader);
    } else {
        sprintf(sqlstr, "SELECT l.username, l.Product, l.MaxContracts, "
                "l.TotalMaxContracts, l.MaxOrders, l.MaxDollars, "
                "l.BuyingPower, f.Multiplier FROM Future f, "
                "FutureLimits l WHERE l.Product = f.Product ");
    }
    char err[64];
    memset(err, '\0', 64);
    int error = psql_gendb_get_results(jc->db_user_name, jc->db_user_name_len,
            jc->db_password, jc->db_password_len, jc->db_inst, jc->db_inst_len, jc->db_server,
            jc->db_server_len, q, sqlstr, err, 64);
    if(error == 0) {
    queue_t row = NULL;
    gendb_v *val = NULL;
    element_t erow, ecol;
    while (!is_empty(q)) {
        int cnt = 0;
        erow = pop_front(q);
        row = (queue_t) (erow->d);
        jp_fut_limits* flimits = 0;
        while (!is_empty(row)) {
            ecol = pop_front(row);
            val = (gendb_v *) (ecol->d);
            switch (cnt) {
                case 0:
                    if(val->arr_len > 0) {
                        int ret = 0;
                        DART_ALLOC(flimits, 8, sizeof(jp_fut_limits), ret);
                        flimits->nlen = val->arr_len;
                        hashmap_t llimits = 0;
                        memcpy(flimits->name,  ((val->g_un).un_array), flimits->nlen);
                        /* Check to see if we have limits for this user*/ 
                        res =
                            get_map_entry(jc->trd_fut_limits, flimits->name, flimits->nlen,
                                    (void **) &llimits);
                        if (res <= 0) {
                            /* If we do not create and add the user map.*/
                            hashmap_t lulimits = create_map(32);
                            no_copy_insert(jc->trd_fut_limits, flimits->name,flimits->nlen,
                                    lulimits);

                        }
                    }
                    break;
                case 1:
                    if(val->arr_len > 0 && flimits != 0) {
                        memcpy(flimits->product, ((val->g_un).un_array), val->arr_len);
                        flimits->plen = val->arr_len;
                        hashmap_t lulimits = 0;
                        res = get_map_entry(jc->trd_fut_limits, flimits->name, flimits->nlen,
                                (void**)&lulimits);
                        if(res > 0) {
                            jp_fut_limits* chk_lims = 0;
                            res = get_map_entry(lulimits,flimits->product, flimits->plen,(void**)&chk_lims);
                            if(res > 0) {
                                res = remove_from_map(lulimits, flimits->product, flimits->plen);
                                if(res > 0) {
                                    free(chk_lims);
                                }
                            }
                            no_copy_insert(lulimits, flimits->product, flimits->plen, flimits); 
                        } 
                    }
                    break;
                case 2:
                    flimits->max_shares_ord = (val->g_un).un_int;
                    break;
                case 3:
                    flimits->max_share = (val->g_un).un_int;
                    break;
                case 4:
                    flimits->max_ord = (val->g_un).un_int;
                    break;
                case 5:
                    flimits->max_dollars =
                        ((long) (val->g_un).un_double) * FUT_MULT;
                    break;
                case 6:
                    flimits->bp =
                        ((long) (val->g_un).un_double) * FUT_MULT;
                    break;
                case 7:
                    flimits->mult =
                        ((long) (val->g_un).un_double);
                    break;
                default:
                    break;

            }
            ++cnt;
            free_element(row, ecol);
            gendb_destroy(val);
        }
        free_element(q, erow);
        destroy_queue(row);
    }
    } else {
        send_debug_message("Failed to load Future limits:, %s \n", err);
    }
    destroy_queue(q);
}
char* build_sql_string(char* trader, char* rom)
{
    char* sqlstr = calloc(2048,1);

    if (trader) {
        sprintf(sqlstr, "SELECT u.username, u.password, u.resetday, u.reset_hour,u.reset_min, "
                "tl.TraderAcr, tl.EquityBuyingPower, tl.OptionBuyingPower, "
                "tl.FutureBuyingPower, tl.OFBuyingPower, tl.EquityMaxShares, "
                "tl.OptionMaxContracts, tl.FutureMaxContracts, tl.OFMaxContracts, "
                "tl.EquityMaxDollars, tl.OptionMaxDollars, tl.FutureMaxDollars, "
                "tl.OFMaxDollars, tl.EquityMaxOrders, tl.OptionMaxOrders, "
                "tl.FutureMaxOrders, tl.OFMaxOrders, tl.EquityTotalMaxShares, "
                "tl.OptionTotalMaxContracts, tl.FutureTotalMaxContracts, tl.OFTotalMaxContracts, "
                "tl.MsgRateLimit_E, tl.MsgRateLimit_O, tl.MsgRateLimit_F, "
                "tl.MsgRateLimit_OF,"
                "(SELECT logon_time_t FROM clientlastlogon "
                "WHERE server = '%s' AND login = u.username)"
                "FROM traderorderlimitsnew tl, "
                "omuser u WHERE u.username = tl.traderacr AND "
                "u.username = '%s'", rom, trader);
    } else {
        sprintf(sqlstr, "SELECT u.username, u.password, u.resetday, u.reset_hour,u.reset_min, "
                "tl.TraderAcr, tl.EquityBuyingPower, tl.OptionBuyingPower, "
                "tl.FutureBuyingPower, tl.OFBuyingPower, tl.EquityMaxShares, "
                "tl.OptionMaxContracts, tl.FutureMaxContracts, tl.OFMaxContracts, "
                "tl.EquityMaxDollars, tl.OptionMaxDollars, tl.FutureMaxDollars, "
                "tl.OFMaxDollars, tl.EquityMaxOrders, tl.OptionMaxOrders, "
                "tl.FutureMaxOrders, tl.OFMaxOrders, tl.EquityTotalMaxShares, "
                "tl.OptionTotalMaxContracts, tl.FutureTotalMaxContracts, tl.OFTotalMaxContracts, "
                "tl.MsgRateLimit_E, tl.MsgRateLimit_O, tl.MsgRateLimit_F, "
                "tl.MsgRateLimit_OF,"
                "(SELECT logon_time_t FROM ClientLastLogon "
                "WHERE server = '%s' AND login = u.username)"
                "FROM TraderOrderLimitsNew tl, "
                "OMuser u WHERE u.username = tl.TraderAcr"
                , rom);
    }
    return sqlstr;
}

void load_trader_profiles(jp_limits_checker * jc, const char* trader)
{
    queue_t q = create_sized_queue(256);
    jp_trader_info *jpti = 0;
    int res = 0;
    int len = 0;
    char *mod_name = calloc(16, 1);
    sprintf(mod_name, "ROM%s", jc->mc->mod_name);
    char* sqlstr = build_sql_string((char*)trader, mod_name);
    struct init_args* in = 0x0;
    char err[64];
    memset(err, '\0', 64);
    int error = psql_gendb_get_results(jc->db_user_name, jc->db_user_name_len,
            jc->db_password, jc->db_password_len, jc->db_inst, jc->db_inst_len, jc->db_server,
            jc->db_server_len, q, sqlstr, err, 64);
    if(error == 0) {
    queue_t row = NULL;
    gendb_v *val = NULL;
    element_t erow, ecol;
    while (!is_empty(q)) {
        int cnt = 0;
        erow = pop_front(q);
        row = (queue_t) (erow->d);
        while (!is_empty(row)) {
            ecol = pop_front(row);
            val = (gendb_v *) (ecol->d);
            switch (cnt) {
                case 0:
                    if(val->arr_len > 0) {
                        len = val->arr_len;
                        res =
                            get_map_entry(jc->traders, ((val->g_un).un_array), len,
                                    (void **) &jpti);
                        if (res <= 0) {
                            jpti = get_stuff(jc->trd_maker);
                            memcpy(jpti->name, ((val->g_un).un_array), len);
                            jpti->name_len = len;
                            no_copy_insert(jc->traders, jpti->name, jpti->name_len,
                                    jpti);
                        }
                    }
                    break;
                case 1:
                    if (res <= 0 && val->arr_len > 0) {
                        memcpy(jpti->password, ((val->g_un).un_array), val->arr_len);
                        jpti->pass_len = val->arr_len;
                        in = get_stuff(jc->init_maker);
                        memcpy(in->name, jpti->name, jpti->name_len);
                        in->name_len = jpti->name_len;

                        memcpy(in->username, jpti->name, jpti->name_len);
                        in->un_len = jpti->name_len;

                        memcpy(in->password, jpti->password, jpti->pass_len);
                        in->p_len = jpti->pass_len;
                        in->do_balance = 1;
                        set_reset_time(in, jc->mc);
                        enqueue(jc->allowed_logons, in);
                    }
                    break;
                    case 2:
                        if(in) {
                            in->reset_time->reset_day = (val->g_un).un_int;
                        }
                        break;//resetday
                    case 3:
                        if(in) {
                            in->reset_time->reset_hour = (val->g_un).un_int;
                        }
                        break; //reset_hour
                    case 4:
                        if(in) {
                            in->reset_time->reset_min = (val->g_un).un_int;
                        }
                        break; //reset_min
                    case 6:
                        jpti->eq_limits->bp =
                            ((unsigned long) (val->g_un).un_double) * EQ_MULT;
                        break;
                    case 7:
                        jpti->opt_limits->bp =
                            (unsigned long) (val->g_un).un_double * FUT_MULT;
                        break;
                    case 8:
                        jpti->fut_limits->bp =
                            (unsigned long) (val->g_un).un_double * FUT_MULT;
                        break;
                    case 9:
                        jpti->of_limits->bp =
                            (unsigned long) (val->g_un).un_double * FUT_MULT;
                        break;
                    case 10:
                        jpti->eq_limits->max_shares_ord = (val->g_un).un_int;
                        break;
                    case 11:
                        jpti->opt_limits->max_shares_ord = (val->g_un).un_int;
                        break;
                    case 12:
                        jpti->fut_limits->max_shares_ord = (val->g_un).un_int;
                        break;
                    case 13:
                        jpti->of_limits->max_shares_ord = (val->g_un).un_int;
                        break;
                    case 14:
                        jpti->eq_limits->max_doll =
                            ((unsigned long) (val->g_un).un_double) * EQ_MULT;
                        break;
                    case 15:
                        jpti->opt_limits->max_doll =
                            (unsigned long) (val->g_un).un_double * FUT_MULT;
                        break;
                    case 16:
                        jpti->fut_limits->max_doll =
                            (unsigned long) (val->g_un).un_double * FUT_MULT;
                        break;
                    case 17:
                        jpti->of_limits->max_doll =
                            (unsigned long) (val->g_un).un_double * FUT_MULT;
                        break;
                    case 18:
                        jpti->eq_limits->max_ord = (val->g_un).un_int;
                        break;
                    case 19:
                        jpti->opt_limits->max_ord = (val->g_un).un_int;
                        break;
                    case 20:
                        jpti->fut_limits->max_ord = (val->g_un).un_int;
                        break;
                    case 21:
                        jpti->of_limits->max_ord = (val->g_un).un_int;
                        break;
                    case 22:
                        jpti->eq_limits->max_share = (val->g_un).un_int;
                        break;
                    case 23:
                        jpti->opt_limits->max_share = (val->g_un).un_int;
                        break;
                    case 24:
                        jpti->fut_limits->max_share = (val->g_un).un_int;
                        break;
                    case 25:
                        jpti->of_limits->max_share = (val->g_un).un_int;
                        break;
                    case 26:
                        //jpti->eq_limits->mrate = (val->g_un).un_int;
                        break; //msg_rate_eq
                    case 27:
                        //jpti->opt_limits->mrate = (val->g_un).un_int;
                        break; //msg_rate_opt
                    case 28:
                        //jpti->fut_limits->mrate = (val->g_un).un_int;
                        break; //msg_rate_fut
                    case 29:
                        //jpti->of_limits->mrate = (val->g_un).un_int;
                        break; //msg_rate_of
                    case 30:
                        if(in) {
                            in->reset_time->last_logon = (val->g_un).un_int;
                        }
                        break; //last_logon
                default:
                    break;
            }
            ++cnt;
            free_element(row, ecol);
            gendb_destroy(val);
        }
        free_element(q, erow);
        destroy_queue(row);
    }
    } else {
        send_debug_message("Failed to load Trader Limits: %s \n", err);
    }
    destroy_queue(q);
    load_future_limits(jc, trader);
}


static void load_clr_billinggrpid(hashmap_t map, jp_limits_checker* jc)
{
    queue_t results = create_sized_queue(256);
    char sqlstr[1024];
    
    sprintf(sqlstr, "SELECT clearingAccount, groupID FROM ClearingActBillingGroupMap");
    char err[2048];
    memset(err, '\0', 2048);

    int error = psql_gendb_get_results(jc->db_user_name, jc->db_user_name_len,
            jc->db_password, jc->db_password_len, jc->db_inst, jc->db_inst_len, jc->db_server,
            jc->db_server_len, results, sqlstr, err, 2048);
    if(error == 0) { 
    queue_t row = NULL;
    gendb_v* pVal = NULL;
    element_t rowE, colE;

    while (is_empty(results) == 0)
    {
        int cnt = 0;
        rowE = pop_front(results);
        row = (queue_t)(rowE->d);
        char* clr = NULL;
        int* billinggrpid = NULL;
        
        while (is_empty(row) == 0)
        {
            colE = pop_front(row);
            pVal = (gendb_v*)(colE->d);

            switch(cnt)
            {
                case 0:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        clr = calloc(pVal->arr_len + 1, sizeof(char));
                        strncpy(clr, (pVal->g_un).un_array, pVal->arr_len);
                    }
                    break;
                case 1:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        
                        billinggrpid = (int*)malloc(sizeof(int));
                        *billinggrpid = (pVal->g_un).un_int;
                        no_copy_insert(map, clr, strlen(clr), billinggrpid);
                        free(clr);
                    }
                    break;
                default:
                    break;
            }
        
            cnt++;
            free_element(row, colE);
            gendb_destroy(pVal);
        }

        free_element(results, rowE);
        destroy_queue(row);
    }
    } else {
        send_debug_message("Failed to load Clearing account to bill group: %s \n", err);
    }
    destroy_queue(results);   
}

void load_billinggrp_limit(jp_limits_checker* jc, const char* billinggrpid)
{
    hashmap_t clr_to_billinggrp = create_map(256);
    load_clr_billinggrpid(clr_to_billinggrp, jc);
    
    queue_t results = create_sized_queue(256);
    char sqlstr[1024];

    if (billinggrpid)
    {
        sprintf(sqlstr, "select * from BillingLimits where GroupID=%s", billinggrpid);
    }
    else
    {
        sprintf(sqlstr, "select * from BillingLimits");
    }
    char err[2048];
    memset(err, '\0', 2048); 
    int error = psql_gendb_get_results(jc->db_user_name, jc->db_user_name_len,
            jc->db_password, jc->db_password_len, jc->db_inst, jc->db_inst_len, jc->db_server,
            jc->db_server_len, results, sqlstr, err, 2048);

    if(error == 0) {
    jp_billinggrp_limits* jpti = 0;
    queue_t row = NULL;
    gendb_v* pVal = NULL;
    element_t rowE, colE;

    char *key = 0;
    int *id = NULL;
    int key_len = 0;
    hashmap_iterator_t hi;
    int valid = 0;
                    
    while (is_empty(results) == 0)
    {
        int cnt = 0;
        rowE = pop_front(results);
        row = (queue_t)(rowE->d);
        int newgrpid = 0;
        
        while (is_empty(row) == 0)
        {
            colE = pop_front(row);
            pVal = (gendb_v*)(colE->d);

            switch(cnt)
            {
                case 0:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        char buf[128];
                        sprintf(buf, "%d", (pVal->g_un).un_int);
                        int res = get_map_entry(jc->billinggrplimits, buf, strlen(buf), (void **) &jpti);
                        if (0 == res)
                        {
                            jpti = get_stuff(jc->clr_maker);
                            jpti->id = (pVal->g_un).un_int;
                            no_copy_insert(jc->billinggrplimits, buf, strlen(buf), jpti);
                            newgrpid = 1;
                        }
                    }
                    break;
                case 1:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        jpti->eq_limits->max_shares_ord = (pVal->g_un).un_int;
                    }
                    break;
                case 2:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        jpti->opt_limits->max_shares_ord = (pVal->g_un).un_int;
                    }
                    break;
                case 3:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        jpti->fut_limits->max_shares_ord = (pVal->g_un).un_int;
                    }
                    break;
                case 4:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        jpti->of_limits->max_shares_ord = (pVal->g_un).un_int;
                    }
                    break;
                case 5:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        jpti->eq_limits->max_doll = (pVal->g_un).un_double * 10000;
                    }
                    break;
                case 6:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        jpti->opt_limits->max_doll = (pVal->g_un).un_double * FUT_MULT;
                    }
                    break; 
                case 7:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        jpti->fut_limits->max_doll = (pVal->g_un).un_double * FUT_MULT;
                    }
                    break; 
                case 8:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        jpti->of_limits->max_doll = (pVal->g_un).un_double * FUT_MULT;
                    }
                    break; 
                case 9:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        jpti->eq_limits->max_ord = (pVal->g_un).un_int;
                    }
                    break;
                case 10:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        jpti->opt_limits->max_ord = (pVal->g_un).un_int;
                    }
                    break;
                case 11:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        jpti->fut_limits->max_ord = (pVal->g_un).un_int;
                    }
                    break;
                case 12:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        jpti->of_limits->max_ord = (pVal->g_un).un_int;
                    }
                case 13:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        jpti->eq_limits->max_share = (pVal->g_un).un_int;
                    }
                    break;
                case 14:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        jpti->opt_limits->max_share = (pVal->g_un).un_int;
                    }
                    break;
                case 15:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        jpti->fut_limits->max_share = (pVal->g_un).un_int;
                    }
                    break;
                case 16:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        jpti->of_limits->max_share = (pVal->g_un).un_int;
                    }
                    break;
                case 17:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        jpti->eq_limits->bp = (pVal->g_un).un_double * 10000;
                    }
                    break; 
                case 18:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        jpti->opt_limits->bp = (pVal->g_un).un_double * FUT_MULT;
                    }
                    break; 
                case 19:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        jpti->fut_limits->bp = (pVal->g_un).un_double * FUT_MULT;
                    }
                    break; 
                case 20:
                    if(pVal->type_in_union != DB_NULL) 
                    {
                        jpti->of_limits->bp = (pVal->g_un).un_double * FUT_MULT;
                    }
                default:
                    break;
            }

            cnt++;
            free_element(row, colE);
            gendb_destroy(pVal);
        }

        if (newgrpid)
        {
            hi = hashmap_iterator_begin(clr_to_billinggrp);
            valid = hashmap_iterator_dereference(hi, &key, &key_len,
                                             (void *) &id);
            
            while (valid) 
            {
                if (jpti->id == *id)
                {
                    if (contains_key(jc->clr_billinggrplimits, key, key_len) == 0)
                    {
                        no_copy_insert(jc->clr_billinggrplimits, key, key_len, jpti);
                    }
                }
                
                valid = hashmap_iterator_next(clr_to_billinggrp, hi);
                
                if (!valid) {
                    break;
                }
                
                valid =
                    hashmap_iterator_dereference(hi, &key,
                                                 &key_len, (void *) &id);
            }
            hashmap_iterator_destroy(hi);
            
        }
        
        free_element(results, rowE);
        destroy_queue(row);
    }

    hi = hashmap_iterator_begin(clr_to_billinggrp);
    valid = hashmap_iterator_dereference(hi, &key, &key_len,
                                     (void *) &id);
    while (valid) 
    {
        free (id);
        valid = hashmap_iterator_next(clr_to_billinggrp, hi);
        
        if (!valid) {
            break;
        }

        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &id);
    }
    hashmap_iterator_destroy(hi);
   
    } else {
        send_debug_message("Faild to load clearing account and billing limits. %s \n", err);
    } 
    delete_map(clr_to_billinggrp);
    destroy_queue(results);
}

int check_rom(dart_order_obj * doj, const char *mod_name,
        int mod_name_len)
{
    if (doj->positions[ROM_TAG].iov_len > 0) {
        char *dash = memchr(doj->positions[ROM_TAG].iov_base, '-',
                doj->positions[ROM_TAG].iov_len);
        char *last_dash = memrchr(doj->positions[ROM_TAG].iov_base, '-',
                doj->positions[ROM_TAG].iov_len);
        if (dash && last_dash) {
            char *ord_rom = dash + 1;
            int ord_rom_len = (last_dash - 7) - ord_rom;
            if ((mod_name_len == ord_rom_len)
                    && strncmp(mod_name, ord_rom, mod_name_len) == 0) {
                return 0;
            } else {
                char temp_sender[200];
                memset(temp_sender, '\0', 200);
                memcpy(temp_sender, ord_rom, ord_rom_len);
                int len = sprintf(doj->current->data->wr_ptr, "ROM%s",
                        temp_sender);
                reset_rom_field(doj, ROM_SENDER,
                        doj->current->data->wr_ptr, len);
                doj->current->data->wr_ptr += len;
                return 1;
            }
        }
    }
    return 0;
}

static void set_order_in_interface(jp_limits_checker * jc, gatekeeper * gk,
        dart_order_obj * doj, char *mod_name,
        int mod_len)
{
    int from_a_rom = check_rom(doj, mod_name, mod_len);
    con_interface *ci = 0;
    int found =
        get_obj_by_client_id(gk, doj->positions[ROM_SENDER].iov_base,
                doj->positions[ROM_SENDER].iov_len,
                (void **) &ci);
    if (found <= 0) {
        ci = init_client_con(doj->positions[ROM_SENDER].iov_base,
                doj->positions[ROM_SENDER].iov_len, 0, gk,
                NULL, -1, NULL, 32768);
        ci->should_buffer = from_a_rom ? 0 : 1;
        set_obj_by_client_id(gk, doj->positions[ROM_SENDER].iov_base,
                doj->positions[ROM_SENDER].iov_len, ci);
    }
    pthread_spin_lock(&ci->mutex);
    int offset = from_a_rom ? ROM_TAG : ROM_CLIENT_ID;
    ci->do_oats_balance = from_a_rom ? 0 : 1;
    no_copy_insert(ci->cl_ord_id_map,
            doj->positions[offset].iov_base,
            doj->positions[offset].iov_len, doj);
    pthread_spin_unlock(&ci->mutex);
    doj->sender = ci;

    if (ci->do_oats_balance) {
        update_balance_new(jc, doj, 0);
    }
}


void set_double_vals(dart_order_obj * doj, int rom_field, double val)
{
    char temp_str[200];
    int len = sprintf(temp_str, "%f", val);
    set_rom_field(doj, rom_field, temp_str, len);
}

void set_int_vals(dart_order_obj * doj, int rom_field, int val)
{
    char temp_str[200];
    int len = sprintf(temp_str, "%d", val);
    set_rom_field(doj, rom_field, temp_str, len);
}

static void load_futures_dests(jp_limits_checker* jc, gatekeeper* gk)
{
    char* sql_str = 
        "SELECT DestID FROM OMDestination WHERE active_flag = 1 AND ( ex_short_name LIKE 'GLBX%' OR ex_short_name LIKE 'ICE%' ) ";
    queue_t q = create_sized_queue(64);
    char err[64];
    memset(err, '\0', 64);
    int error = psql_gendb_get_results(jc->db_user_name, jc->db_user_name_len,
            jc->db_password, jc->db_password_len, jc->db_inst, jc->db_inst_len, jc->db_server,
            jc->db_server_len, q, sql_str, err, 64);
    if(error == 0) {
    element_t erow, ecol;
    queue_t row = NULL;
    gendb_v *val = NULL;
    char dest_id[8];
    int dest_len = 0;
    while (!is_empty(q)) {
        int cnt = 0;
        erow = pop_front(q);
        row = (queue_t) (erow->d);
        while (!is_empty(row)) {
            ecol = pop_front(row);
            val = (gendb_v *) (ecol->d);
            switch (cnt) {
                case 0:
                    dest_len = sprintf(dest_id, "%d", ((val->g_un).un_int));
                    insert_map(jc->opt_fut_ex, dest_id, dest_len, dest_id, dest_len);
                    break;
                default:
                    break;
            }
            ++cnt;
            free_element(row, ecol);
            gendb_destroy(val);
        }
        free_element(q, erow);
        destroy_queue(row);
    }
    } else {
        send_debug_message("Faild to load Futures Dests: %s \n", err);
    }
    destroy_queue(q);
}



int load_destinations(jp_limits_checker * jc, gatekeeper * gk)
{
    char sql_str[500];
    int mod_len = 0;
    char *mod_name = (char *) get_mod_name(gk, &mod_len);
    //int mod_len = 0;
    //char* mod_name = (char*) get_mod_name(gk, &mod_len);
    int sql_len = sprintf(sql_str,
            "SELECT DestID, DestRouteID FROM OMDestination WHERE active_flag = 1");
    if (sql_len > 500) {
        return 1;
    }
    queue_t q = create_sized_queue(256);
    char err[64];
    memset(err, '\0', 64);
    int error = psql_gendb_get_results(jc->db_user_name, jc->db_user_name_len,
            jc->db_password, jc->db_password_len, jc->db_inst, jc->db_inst_len, jc->db_server,
            jc->db_server_len, q, sql_str, err, 64);
    if(error == 0) {
    queue_t row = NULL;
    gendb_v *val = NULL;
    element_t erow, ecol;
    char dest_id[8];
    int dest_len = 0;
    char dest_route[8];
    int route_len = 0;

    while (!is_empty(q)) {
        int cnt = 0;
        erow = pop_front(q);
        row = (queue_t) (erow->d);
        while (!is_empty(row)) {
            ecol = pop_front(row);
            val = (gendb_v *) (ecol->d);
            switch (cnt) {
                case 0:
                    dest_len = sprintf(dest_id, "%d", ((val->g_un).un_int));
                    break;
                case 1:
                    route_len =
                        sprintf(dest_route, "%d", ((val->g_un).un_int));
                    break;
                default:
                    break;
            }
            ++cnt;
            free_element(row, ecol);
            gendb_destroy(val);
        }
        if (mod_len == route_len
                && strncmp(mod_name, dest_route, mod_len) == 0) {
            add_route_dest_pair(gk, dest_id, dest_len, dest_id, dest_len);
        } else {
            add_route_dest_pair(gk, dest_id, dest_len, dest_route,
                    route_len);
        }
        free_element(q, erow);
        destroy_queue(row);
    }
    } else {
        send_debug_message("Failed to load destinations: %s \n", err);
    }
    destroy_queue(q);
    return 0;
}

static void clean_date_rm_time(dart_order_obj* doj, char* date, int dlen)
{
    char clean[16];
    memcpy(clean, date, 4);
    memcpy(clean + 4, date +5, 2);
    set_rom_field(doj, ROM_EXPIRE_DATE, clean,
                6);
}

static void remove_trailing_zeros_from_strike(dart_order_obj* doj, int rom_field, double val)
{
    char temp_str[200];
    int len = sprintf(temp_str, "%.02f", val);
    set_rom_field(doj, rom_field, temp_str, len);
}

int get_orders(jp_limits_checker * jc, gatekeeper * gk,
        const char *sql_str, char *mod_name, int mod_len)
{
    queue_t q = create_sized_queue(256);
    //jp_trader_info *jpti = 0;
    double price = 0;
    long iprice = 0;
    char err[64];
    memset(err, '\0', 64);
    int error = psql_gendb_get_results(jc->db_user_name, jc->db_user_name_len,
            jc->db_password, jc->db_password_len, jc->db_inst, jc->db_inst_len, jc->db_server,
            jc->db_server_len, q, sql_str, err, 64);
    if(error == 0) {
    queue_t row = NULL;
    gendb_v *val = NULL;
    element_t erow, ecol;
    while (!is_empty(q)) {
        int cnt = 0;
        int local_int = 0;
        erow = pop_front(q);
        row = (queue_t) (erow->d);
        dart_order_obj *doj = (dart_order_obj *) get_gk_obj(gk);
        char sec = 'E';
        while (!is_empty(row)) {
            ecol = pop_front(row);
            val = (gendb_v *) (ecol->d);
            switch (cnt) {
                case 0:
                    {
                        if(val->arr_len > 0) {
                            set_rom_field(doj, ROM_P_C, ((val->g_un).un_array),
                                    val->arr_len);
                        }
                    }
                    break;
                case 1:
                    if(val->arr_len > 0) {
                        set_rom_field(doj, ROM_CAP, ((val->g_un).un_array),
                                val->arr_len);
                    }
                    break;
                case 2:
                    if(val->arr_len > 0) {
                        set_rom_field(doj, ROM_CLR_ACC, ((val->g_un).un_array),
                                val->arr_len);
                    }
                    break;
                case 3:
                    if(val->arr_len > 0) {
                        set_rom_field(doj, ROM_CLR_ID, ((val->g_un).un_array),
                                val->arr_len);
                    }
                    break;
                case 4:
                    if(val->arr_len > 0) {
                        set_rom_field(doj, ROM_CMTA, ((val->g_un).un_array),
                                val->arr_len);
                    }
                    break;
                case 5:
                    local_int = ((val->g_un).un_int);
                    set_rom_field(doj, ROM_BIN_CUM, &local_int, 4);
                    set_int_vals(doj, ROM_CUM, ((val->g_un).un_int));
                    break;
                case 6:
                    // This is the DestRouteID we put it in both ROM slots
                    set_int_vals(doj, ROM_DESTID, ((val->g_un).un_int));
                    break;
                case 7:
                    if(val->arr_len > 0) {
                        set_rom_field(doj, ROM_EX_TAG, ((val->g_un).un_array),
                                val->arr_len);
                    }
                    break;
                case 8:
                    set_int_vals(doj, ROM_LEAVES, ((val->g_un).un_int));
                    break;
                case 9:
                    local_int = ((val->g_un).un_int);
                    set_rom_field(doj, ROM_BIN_MAXFLOOR, &local_int, 4);
                    set_int_vals(doj, ROM_MAX_FLOOR, ((val->g_un).un_int));
                    break;
                case 10:
                    if(val->arr_len > 0) {
                        set_rom_field(doj, ROM_MM_ACC, ((val->g_un).un_array),
                                val->arr_len);
                    }
                    break;
                case 11:
                    if(val->arr_len > 0) {
                        set_rom_field(doj, ROM_MM_HOME, ((val->g_un).un_array),
                                val->arr_len);
                    }
                    break;
                case 12:
                    if(val->arr_len > 0) {
                        set_rom_field(doj, ROM_TAG, ((val->g_un).un_array),
                                val->arr_len);
                    }
                    break;
                case 13:
                    if(val->arr_len > 0) {
                        set_rom_field(doj, ROM_CLORDID, ((val->g_un).un_array),
                                val->arr_len);
                    }
                    break;
                case 14:
                    set_int_vals(doj, ROM_TYPE, ((val->g_un).un_int));
                    break;
                case 15:
                    if(val->arr_len > 0) {
                        set_rom_field(doj, ROM_SEC_TYPE, ((val->g_un).un_array),
                                val->arr_len);
                        sec = ((char*)doj->positions[ROM_SEC_TYPE].iov_base)[0];
                    }
                    break;
                case 16:
                    local_int = ((val->g_un).un_int);
                    set_int_vals(doj, ROM_SHARES, local_int);
                    set_rom_field(doj, ROM_BIN_QTY, &local_int, 4);
                    break;
                case 17:
                    set_double_vals(doj, ROM_PRICE, ((val->g_un).un_double));
                    price = ((val->g_un).un_double);
                    if(sec == 'E') {
                        price += .0000001;
                        price = price * 10000;
                    } else {
                        price += .000000001;
                        price = price * FUT_MULT;
                    }
                    iprice = (long) price;
                    set_rom_field(doj, ROM_BIN_PRICE, &iprice, 8);
                    break;
                case 18:
                    set_int_vals(doj, ROM_SIDE, ((val->g_un).un_int));
                    break;
                case 19:
                    set_double_vals(doj, ROM_STOP, ((val->g_un).un_double));
                    price = ((val->g_un).un_double);
                    if(sec == 'E') {
                        price += .0000001;
                        price = price * 10000;
                    } else {
                        price += .000000001;
                        price = price * FUT_MULT;
                    }
                    iprice = (long) price;
                    set_rom_field(doj, ROM_BIN_STOP, &iprice, 8);
                    break;
                case 20:
                    remove_trailing_zeros_from_strike(doj, ROM_STRIKE, ((val->g_un).un_double));
                    break;
                case 21:
                    if(val->arr_len > 0) {
                        set_rom_field(doj, ROM_SYM, ((val->g_un).un_array),
                                val->arr_len);
                    }
                    break;
                case 22:
                    if(val->arr_len > 0) {
                        set_rom_field(doj, ROM_CLIENT_ID, ((val->g_un).un_array),
                                val->arr_len);
                    }
                    break;
                case 23:
                    set_int_vals(doj, ROM_TIF, ((val->g_un).un_int));
                    break;
                case 24:
                    if(val->arr_len > 0 && val->arr_len >= 8) {
                        clean_date_rm_time(doj, ((val->g_un).un_array), 
                                val->arr_len);
                    }
                    break;
                case 25:
                    set_int_vals(doj, ROM_MIN_QTY, ((val->g_un).un_int));
                    break;
                case 26:
                    price = ((val->g_un).un_double);
                    price += .0000001;
                    //iprice = (long) price;
                    set_rom_field(doj, ROM_BIN_MULT, &price, sizeof(price));
                    break;
                case 27:
                    if(val->arr_len > 0) {
                        set_rom_field(doj, ROM_O_C, ((val->g_un).un_array),
                                val->arr_len);
                    }
                    break;
                case 28:
                    if(val->type_in_union != DB_NULL) {
                        set_int_vals(doj, ROM_CPLX_TYPE, ((val->g_un).un_int));
                    }
                    break;
                case 29:
                    if(val->arr_len > 0) {
                        set_rom_field(doj, ROM_PARENT, ((val->g_un).un_array),
                                val->arr_len);
                        set_rom_field(doj, ROM_OM_PARENT, ((val->g_un).un_array),
                                val->arr_len);
                    }
                    break;
                case 30:
                    if(val->arr_len > 0) {
                        set_rom_field(doj, ROM_TRADER, ((val->g_un).un_array),
                                val->arr_len);
                    }
                    break;
                case 31:
                    if(val->arr_len > 0) {
                        set_rom_field(doj, ROM_OWNER, ((val->g_un).un_array),
                                val->arr_len);
                        set_rom_field(doj, ROM_SENDER, ((val->g_un).un_array),
                                val->arr_len);
                    }
                    break;
                case 32:
                    if(val->arr_len > 0 && val->arr_len >= 8) {
                        char* exp = ((val->g_un).un_array);
                        set_rom_field(doj, ROM_EXPIRE_DAY, exp + 8,
                                2);
                    }
                    break;
                case 33:
                    // This is the DestRouteID we put it in both ROM slots
                    set_int_vals(doj, ROM_DEST, ((val->g_un).un_int));
                    break;
                case 34:
                    if(val->arr_len > 0) {
                        set_rom_field(doj, ROM_SEC_DEF, ((val->g_un).un_array),
                                val->arr_len);
                    }
                    break;
                case 35:
                    if(val->arr_len > 0) {
                        set_rom_field(doj, ROM_TRADE_FOR, ((val->g_un).un_array),
                                val->arr_len);
                    }
                    break;
                case 36:
                    if(val->type_in_union != DB_NULL) {
                        set_ex_tag(doj, ((val->g_un).un_int));
                    }
                    break;
                case 37:
                    if(getplen(doj,ROM_CPLX_TYPE) > 0) {
                        char* cplx = getpval(doj, ROM_CPLX_TYPE);
                        if(cplx[0] == 6) {
                            set_int_vals(doj, ROM_SHARES, ((val->g_un).un_int));
                        }
                    }
                    break;
                default:
                    break;
            }
            ++cnt;
            free_element(row, ecol);
            gendb_destroy(val);
        }
        set_order_in_interface(jc, gk, doj, mod_name, mod_len);
        add_to_init_orders(gk, doj->positions[ROM_DEST].iov_base,
                doj->positions[ROM_DEST].iov_len, doj);
        if(getplen(doj, ROM_DEST) == mod_len &&
                strncmp(getpval(doj, ROM_DEST), mod_name, mod_len) == 0 &&
                strncmp(getpval(doj, ROM_DESTID), mod_name, mod_len) != 0) {
            add_to_init_orders(gk, doj->positions[ROM_DESTID].iov_base,
                    doj->positions[ROM_DESTID].iov_len, doj);
        }
        free_element(q, erow);
        destroy_queue(row);
    }
    } else {
        send_debug_message("Failed to load orders: %s \n", err);
    }
    destroy_queue(q);
    return 0;
}

int load_orders(jp_limits_checker * jc, gatekeeper * gk,
        const char *destinations)
{
    char sql_str[2000];
    int mod_len = 0;
    char *mod_name = (char *) get_mod_name(gk, &mod_len);
    char romname[17];
    char romnum[10];
    strncpy(romnum, mod_name, mod_len);
    romnum[mod_len] = '\0';
    sprintf(romname, "ROM%s", romnum);

    load_destinations(jc, gk);
    load_futures_dests(jc, gk);
    sprintf(sql_str,
            "SELECT A.callPut, A.capacity, A.clearingAccount, A.clearingID, A.CMTAID, "
            "A.cumQty, A.DestID, A.exchangeTag, A.leavesQty, A.maxFloor, A.mmAcct,A.mmExchange, A.omTag, A.romexchangetag, "
            "A.orderType, A.secType, A.qty, A.price, A.side, A.stopPrice, A.strikePrice, "
            "A.symbol, A.clienttag, A.tif, A.expireDate, A.minQty, A.contract_size,A.openClose,A.cplxtype,A.omparenttag,A.trader,A.owner, "
            "A.expireDate, A.DestRouteID, A.SecDef, A.tradeFor, B.status_count,A.OriginalShares "
            "FROM Orders A,"
            "(SELECT omTag, MAX(status_count) as status_count "
            "FROM orders GROUP BY omTag) B "
            "WHERE B.omTag=A.omTag AND B.status_count = A.status_count "
            "AND (A.cplxtype is null OR A.cplxtype = 3 OR A.omparenttag is not null) "
            "AND (A.omname='%s' OR (destrouteid IN(%s) AND A.omname <> '%s')) "
            "AND A.status NOT IN (4,8,2,12)", romname, destinations, romname);
    get_orders(jc, gk, sql_str, mod_name, mod_len);
    memset(sql_str, '\0', 2000);
    sprintf(sql_str,
            "SELECT A.callPut, A.capacity, A.clearingAccount, A.clearingID, A.CMTAID, "
            "A.cumQty, A.DestID, A.exchangeTag, A.leavesQty, A.maxFloor, A.mmAcct,A.mmExchange, A.omTag, A.romexchangetag, "
            "A.orderType, A.secType, A.qty, A.price, A.side, A.stopPrice, A.strikePrice, "
            "A.symbol, A.clienttag, A.tif, A.expireDate, A.minQty, A.contract_size,A.openClose,A.cplxtype,A.omparenttag,A.trader,A.owner, "
            "A.expireDate, A.DestRouteID, A.SecDef, A.tradeFor, B.status_count,A.OriginalShares "
            "FROM Orders A, "
            "(SELECT omTag, MAX(status_count) as status_count "
            "FROM orders GROUP BY omTag) B, "
            "(SELECT omTag, cplxtype, Max(status_count) as status_count "
            "FROM orders WHERE cplxtype = 5 GROUP BY cplxtype, omTag) C "
            "WHERE B.omTag = A.omTag AND B.omTag = C.omTag AND A.status_count = C.status_count "
            "AND (A.omname='%s' OR (destrouteid IN(%s) AND A.omname <> '%s')) "
            "AND A.status NOT in (4, 8, 2, 12) AND A.cplxtype is NOT null", romname,
            destinations, romname);

    get_orders(jc, gk, sql_str, mod_name, mod_len);
    return 0;
}


unsigned short nextwday_dif[7][7] = {
    { 7, 1, 2, 3, 4, 5, 6 },
    { 6, 7, 1, 2, 3, 4, 5 },
    { 5, 6, 7, 1, 2, 3, 4 },
    { 4, 5, 6, 7, 1, 2, 3 },
    { 3, 4, 5, 6, 7, 1, 2 },
    { 2, 3, 4, 5, 6, 7, 1 },
    { 1, 2, 3, 4, 5, 6, 7 }
};
