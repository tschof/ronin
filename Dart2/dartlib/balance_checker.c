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
#include "rb_tree.h"
#include "message_queue.h"
#include "rex_constants.h"
#include "rex_allocator.h"
#include "dart_order_obj.h"
#include "balance_checker.h"
#include "gatekeeper.h"
#include "connection_interface.h"
#include "log_mnger.h"
#include "rex_seq_request.h"
#include "mds_client.h"

typedef struct order_column
{
    char* column_name;
    int field_id;
} order_column;

//order_column order_cln[ORDER_COLUMN_NUM] = {
static order_column order_cln[] = {
    {"sectype", ROM_SEC_TYPE},
    {"omtag", ROM_TAG},
    {"omparenttag", ROM_OM_PARENT},
    {"clienttag", ROM_CLIENT_ID},
    {"clientparenttag", ROM_PARENT},
    {"exchangetag", ROM_EX_TAG},
    {"romexchangetag", ROM_INSTR},
    {"status", ROM_STATUS},
    {"avgprice", ROM_AVG_PRICE},
    {"callput", ROM_P_C},
    {"capacity", ROM_CAP},
    {"clearingaccount", ROM_CLR_ACC},
    {"clearingid", ROM_CLR_ID},
    {"cmtaid", ROM_CMTA},            
    {"cumqty", ROM_CUM},                         
    {"destid", ROM_DESTID},
    {"destrouteid", ROM_DEST},
    {"exdest", ROM_EXDEST},            
    {"firm", ROM_FIRM},                  
    {"instructions", ROM_COPY_INSTR},
    {"leavesqty", ROM_LEAVES},
    {"localacct", ROM_LOCAL_ACC},         
    {"maxfloor", ROM_MAX_FLOOR},
    {"minqty", ROM_MIN_QTY},
    {"mmacct", ROM_MM_ACC},
    {"mmexchange", ROM_MM_HOME},
    {"openclose", ROM_O_C},                     
    {"ordertype", ROM_TYPE},
    {"originalshares", ROM_ORIG_SHARES},
    {"owner", ROM_OWNER},
    {"price", ROM_PRICE},
    {"qty", ROM_SHARES},
    {"side", ROM_SIDE},              
    {"stopprice", ROM_STOP},             
    {"strikeprice", ROM_STRIKE},                   
    {"symbol", ROM_SYM},                
    {"text", ROM_TEXT},
    {"clientecho", ROM_ECHO},        
    {"tif", ROM_TIF},
    {"tradefor", ROM_TRADE_FOR},
    {"trader", ROM_TRADER},
    {"underlying", ROM_UNDER},        
    {"shortlender", ROM_SHORT_LEND},
    {"execinst", ROM_EXEC_INST},              
    {"billstr", ROM_LIQ},                                      
    {"secid", ROM_SEC_ID},                                
    {"cplxtype", ROM_CPLX_TYPE},
    {"pegoffset", ROM_PEG_DIFF},
    {"secdef", ROM_SEC_DEF},
    {"algotype", ROM_ALGO_TYPE},
    {"status_count", ROM_STATUS_CNT},
    {"expiredate", ROM_EXPIRE_DATE},
    {"omtime", ROM_TIME},
    {"extime", EXEC_TIME},
    {"omname", ROM_NAME},
    {"orderexpiresdate", ROM_EXPIRE_TIME}
};

static int ORDER_COLUMN_NUM = sizeof(order_cln) / sizeof(order_column);

static int billing_key_compare(const void* a, const void *b)
{
    if(*(int*)a > *(int*)b) {
        return 1;
    }
    if(*(int*)a < *(int*)b) {
        return -1;
    } 
    return 0;
}
static void billing_int_print(const void* a)
{
    printf("%d", *(int*)a);
}

static void billing_info_print(const void* a)
{
    printf("Printing RC Cola.\n");
}

static void billing_key_destroy(void* a)
{
    free((int*)a);
}
static void billing_info_destroy(void* a)
{
}

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
    hashmap_t traders;
    hashmap_t equity_sym;
    hashmap_t grouplimits;
    rb_tree_t* billing_groups;
    hashmap_t clr_to_bill;
    hashmap_t opt_fut_ex;
    hashmap_t trd_fut_limits;
    hashmap_t allowed_logons;
    ex_config *mc;
    otl* o;
    mdsclient* mdc;
    int db_user_name_len;
    int db_password_len;
    int db_server_len;
    int db_inst_len;
    int check_eq;
    dallocator* clr_maker;
    dallocator* bg_maker;
    dallocator* trd_maker;
    dallocator* init_maker;
    pthread_spinlock_t jp_protector;
};
typedef struct mdupdate {
    short exempt;
    long bid;
    long offer;
}mdupdate;
typedef struct jp_group_limits {
    char* name;
    jp_limits *eq_limits;
    jp_limits *opt_limits;
    jp_limits *fut_limits;
    jp_limits *of_limits;
    int name_len;
    int exempt;
} jp_group_limits;

struct jp_trader_info {
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
};

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
    memset(glob, '\0', sizeof(login_info));
}

static void init_clr_limits(void* glob)
{
    jp_group_limits* jpti = glob;
    jpti->name = glob + sizeof(jp_group_limits);
    jpti->eq_limits = glob + 80;
    jpti->opt_limits =  glob + 176;
    jpti->fut_limits = glob + 272;
    jpti->of_limits = glob + 368;
}

static void init_trader_limits(void* glob)
{
    jp_trader_info* jpti = glob;
    jpti->name = glob + sizeof(jp_trader_info);
    jpti->password = glob + 96;
    jpti->eq_limits = glob + 128;
    jpti->opt_limits =  glob + 224;
    jpti->fut_limits = glob + 320;
    jpti->of_limits = glob + 416;
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
            rc = "OPT on FUT";
            break;
    }

    return rc;
}

void jiggly_price_converter(rcntr* cnt,
        jp_limits_checker * jpc,
        dart_order_obj* doj, char sec_type)
{
    char price[64];
    memset(&price, '\0', 64);
    memcpy(&price,doj->positions[ROM_PRICE].iov_base,
            doj->positions[ROM_PRICE].iov_len); 
    double d = strtod(price, NULL);
    char* key = getpval(doj, ROM_DESTID);
    int key_len = getplen(doj, ROM_DESTID);
    if((sec_type == 'F' || sec_type == 'M') ||
            (key_len > 0 && contains_key(jpc->opt_fut_ex, key, key_len))) {
        d += .0000000001;
        d = d * FUT_MULT;
        int64_t ld = (int64_t)d;
        cnt->price = ld;
    } else {
        d += .000000001;
        d = d * EQ_MULT;
        int64_t id = (int64_t)d;
        cnt->price = id;
    }
}

static jp_limits *get_limits_for_sec(jp_limits_checker * jpc, 
        jp_trader_info * jpti,
        char sec_type)
{
    switch (sec_type) {
        case 'E':
            return jpti->eq_limits;
        case 'O':
            return jpti->opt_limits;
        case 'M':
            return jpti->of_limits;
        case 'F':
            return jpti->fut_limits;
        default:
            return jpti->eq_limits;
    };
}

static int get_symbol_len(char *sym, int s_len)
{
    int len = s_len;
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
        rcntr* rc, long np, char side)
{
    int allowed = 1;
    hashmap_t trd_lims = 0;
    jp_fut_limits* jp = 0;
    int found =
        get_map_entry(jpc->trd_fut_limits,rc->owner,
                rc->olen, (void **) &trd_lims);
    if(found) {
        char* prod = rc->sym;
        int prod_len = get_symbol_len(rc->sym, rc->slen);
        found = get_map_entry(trd_lims, prod, prod_len, (void**)&jp);
        if(found) {
            dollars = (dollars * jp->mult);
            if (check_vals) {
                if (jp->max_ord < (jp->open_ords + orders)) {
                    rc->err_len = sprintf(rc->errStr, "Exceeds Future Max Open Orders");
                    allowed = 0;
                }
                if (jp->max_share < (jp->open_shares + shares)) {
                    rc->err_len = sprintf(rc->errStr, "Exceeds Future Max Open Contracts");
                    allowed = 0;
                }
                if (jp->max_dollars < labs(dollars)) {
                    rc->err_len = sprintf(rc->errStr, "Exceeds Futures Max Dollar per Order");
                    allowed = 0;
                }
                if(side == '1') {
                    if (jp->bp < (jp->cur_balance_buy + dollars)) {
                        rc->err_len = sprintf(rc->errStr, "Exceeds Futures Max Buy Balance");
                        allowed = 0;
                    }
                } else {
                    if (jp->bp < (jp->cur_balance_sell + dollars)) {
                        rc->err_len = sprintf(rc->errStr, "Exceeds Future Max Sell Balance");
                        allowed = 0;
                    }
                }
                if (jp->max_shares_ord < shares) {
                    rc->err_len = sprintf(rc->errStr, "Exceeds Future Max Contracts per Order");
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
            memcpy(trader, rc->owner, rc->olen);
            char p[32];
            memset(p, '\0', 32);
            memcpy(p, prod, prod_len);
            rc->err_len = sprintf(rc->errStr, "%s has no limits for: %s ",
                    trader, p);
        } 
    } else {
        char trader[64];
        memset(trader, '\0', 64);
        memcpy(trader,rc->owner, rc->olen);
        allowed = 0;
        rc->err_len = sprintf(rc->errStr, "%s has no futures limits set.", trader);
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

int is_blocked(jp_trader_info* jpti)
{
    return jpti->blocked;
}

int is_exempt(jp_trader_info* jpti)
{
    return jpti->exempt;
}
static int check_msg_rate(jp_limits* jp)
{
    ++jp->nmes;
    if(jp->nmes >= jp->mrate) {
        struct timeval tv;
        int ret_val = gettimeofday(&tv, 0);
        if(ret_val == 0) {
            unsigned long cur_micro = tv.tv_sec * 1000000;
            cur_micro += tv.tv_usec;
            if(cur_micro - jp->toff < 1000000) {
                return 0;
            } else {
                jp->toff = cur_micro;
                jp->nmes = 0;
            }
        } 
    }
    return 1;
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
}

int ns_check_price(jp_limits_checker* jp, rcntr* rc) 
{
  int allowed = 1;
  if(jp->check_eq) {
    mdupdate* mds = 0;
    pthread_spin_lock(&jp->jp_protector);
    int found = get_map_entry(jp->equity_sym, rc->sym, rc->slen, (void**)&mds);
    if(found && !mds->exempt) {
        if(rc->side == '1') {
            allowed = rc->price < mds->offer;
        } else {
            allowed = rc->price > mds->bid;
        }
    }
    pthread_spin_unlock(&jp->jp_protector);
  }
  return allowed;
}
static int check_price(jp_limits_checker* jp,rcntr* rc, long price, char side)
{
    int allowed = 1;
    mdupdate* mds = 0;
    pthread_spin_lock(&jp->jp_protector);
    int found = get_map_entry(jp->equity_sym, rc->sym, rc->slen, (void**)&mds);
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
static jp_limits *get_limits_for_billinggrp(jp_limits_checker * jc,
        char *key, int len, char sec_type, int* exempt)
{
    jp_limits* rc = NULL;

    jp_group_limits* jpti = NULL;
    int found = get_map_entry(jc->grouplimits, key, len, (void **) &jpti);
    if (found)
    {
        *exempt = jpti->exempt;
        switch(sec_type) {
            default:
                rc = jpti->eq_limits;
                break;
            case 'O':
                rc = jpti->opt_limits;
                break;
            case 'F':
                rc = jpti->fut_limits;
                break;
        }
    }
    return rc;
}

static int billinggrp_update(jp_limits_checker * jpti, char* key, int key_len, char sec_type, long shares,
        long orders, long dollars, long check_vals, int add, rcntr* rc, char side)
{
    int allowed = 1;
    int exempt = 0;
    jp_limits *jp = get_limits_for_billinggrp(jpti, key, key_len, sec_type, &exempt);
    if (check_vals) {
        if (jp)
        {
            if (jp->max_ord < (jp->open_ords + orders)) {
                rc->err_len = sprintf(rc->errStr, "Exceeds Group Max Open Orders Sec(%c)", sec_type);
                allowed = 0;
            }
            if (jp->max_share < (jp->open_shares + shares)) {
                rc->err_len = sprintf(rc->errStr, "Exceeds Group Max Open Shares Sec(%c)", sec_type);
                allowed = 0;
            }
            if (jp->max_doll < labs(dollars)) {
                rc->err_len = sprintf(rc->errStr, "Exceeds Group Max Dollar per Order Sec(%c) ", sec_type);
                allowed = 0;
            }
            if (side == '1') {
                if (jp->bp < labs(jp->cur_balance_buy + dollars)) {
                    rc->err_len = sprintf(rc->errStr, "Exceeds Group Max Balance Buy Sec(%c) ", sec_type);
                    allowed = 0;
                }
            } else {
                if (jp->bp < labs(jp->cur_balance_sell + dollars)) {
                    rc->err_len = sprintf(rc->errStr, "Exceeds Group Max Balance Sell Sec(%c)", sec_type);
                    allowed = 0;
                }
            }
            if (jp->max_shares_ord < shares) {
                rc->err_len = sprintf(rc->errStr, "Exceeds Group Max Shares per order Sec(%c)", sec_type);
                allowed = 0;
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
static int base_update(jp_limits_checker * jpc,jp_trader_info * jpti, char sec_type, long shares,
        long orders, long dollars, long check_vals, int add, rcntr* rc, long np, char side)
{
    int allowed = 1;
    if(sec_type == 'F') {
        allowed = fut_update(jpc, jpti, shares, orders, dollars, check_vals, add, rc, np, side);
    } else {
        jp_limits *jp = get_limits_for_sec(jpc, jpti, sec_type);
        if (check_vals) {
            if(sec_type == 'E' && jpc->check_eq) {
                if(rc->price > 0) {
                    allowed = check_price(jpc, rc, np, side);
                    if(allowed == 0) {
                        rc->err_len = sprintf(rc->errStr, "Price %ld is outside limits",
                                rc->price);
                    }
                }
            }
            if(allowed == 1) {
                allowed = check_msg_rate(jp);
                if(allowed == 0) {
                    rc->err_len = sprintf(rc->errStr, "Exceeds %s Message rate", get_secstr(sec_type));
                }
            }
            if (jp->max_ord < (jp->open_ords + orders)) {
                rc->err_len = sprintf(rc->errStr, "Exceeds %s Max Open Orders", get_secstr(sec_type));
                allowed = 0;
            }
            if (jp->max_share < (jp->open_shares + shares)) {
                rc->err_len = sprintf(rc->errStr, "Exceeds %s Max Open Shares", get_secstr(sec_type));
                allowed = 0;
            }
            if (jp->max_doll < labs(dollars)) {
                rc->err_len = sprintf(rc->errStr, "Exceeds %s Max Dollar per Order", get_secstr(sec_type));
                allowed = 0;
            }
            if(side == '1') {
                if (jp->bp < (jp->cur_balance_buy + dollars)) {
                    rc->err_len = sprintf(rc->errStr, "Exceeds %s Max Balance", get_secstr(sec_type));
                    allowed = 0;
                }
            } else {
                if (jp->bp < (jp->cur_balance_sell + dollars)) {
                    rc->err_len = sprintf(rc->errStr, "Exceeds %s Max Balance", get_secstr(sec_type));
                    allowed = 0;
                }
            }
            if (jp->max_shares_ord < shares) {
                rc->err_len = sprintf(rc->errStr, "Exceeds %s Max Shares per Order", get_secstr(sec_type));
                allowed = 0;
            }
            if(sec_type == 'E') {
                long a = np % 1000000;
                if(np > EQ_MULT && a > 0) {
                    rc->err_len = sprintf(rc->errStr, "Price has incorrect decimalization");
                    allowed = 0;
                    send_debug_message(jpc->o, "Price Mod 100: %ld, Raw: %ld \n", a, np);
                }
            }
            if (jpti->exempt) {
                allowed = 1;
            } else if (allowed == 0) {
                //set_rom_field(doj, ROM_TEXT, errStr, err_len);
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
jp_trader_info* jp_get_trader(jp_limits_checker * jpc, char *trader, int len)
{
    jp_trader_info *jpti = 0;
    get_map_entry(jpc->traders, trader, len, (void **) &jpti);
    return jpti;
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

void jp_exempt_billing_group(jp_limits_checker* jpc,int key,
        int exempt)
{
    jp_group_limits* bjpti =
        (jp_group_limits*)rb_tree_get_val(jpc->billing_groups, &key);
    if(bjpti) {
        bjpti->exempt = exempt;
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
jp_limits* get_equity_limits_for_clraccnt(jp_limits_checker * jc,
        char *key, int len)
{
    jp_group_limits* jpti = NULL;
    int found = get_map_entry(jc->grouplimits, key, len, (void **) &jpti);
    if (found) {
        return jpti->eq_limits;
    }
    return NULL;
}

jp_limits* get_future_limits_for_bill(jp_limits_checker* jc,
        int key)
{
    jp_limits* ret = 0x0;
    jp_group_limits* bjpti =
        (jp_group_limits*)rb_tree_get_val(jc->billing_groups, &key);
    if(bjpti) {
        ret = bjpti->fut_limits;
    }
    return ret;
}
jp_limits*  get_equity_limits_for_bill(jp_limits_checker* jc, int key)
{
    jp_limits* ret = 0x0;
    jp_group_limits* bjpti =
        (jp_group_limits*)rb_tree_get_val(jc->billing_groups, &key);
    if(bjpti) {
        ret = bjpti->eq_limits;
    }
    return ret;
}
jp_limits*  get_equity_option_limits_for_bill(jp_limits_checker* jc, int key)
{
    jp_limits* ret = 0x0;
    jp_group_limits* bjpti =
        (jp_group_limits*)rb_tree_get_val(jc->billing_groups, &key);
    if(bjpti) {
        ret = bjpti->opt_limits;
    }
    return ret;
}
jp_limits*  get_fut_opt_limits_for_bill(jp_limits_checker* jc, int key)
{
    jp_limits* ret = 0x0;
    jp_group_limits* bjpti =
        (jp_group_limits*)rb_tree_get_val(jc->billing_groups, &key);
    if(bjpti) {
        ret = bjpti->of_limits;
    }
    return ret;
}
jp_limits* get_future_limits_for_clraccnt(jp_limits_checker * jc,
        char *key, int len)
{
    jp_group_limits* jpti = NULL;
    int found = get_map_entry(jc->grouplimits, key, len, (void **) &jpti);
    if (found) {
        return jpti->fut_limits;
    }
    return NULL;
}
jp_limits* get_fut_opt_limits_for_clraccnt(jp_limits_checker * jc,
        char *key, int len)
{
    jp_group_limits* jpti = NULL;
    int found = get_map_entry(jc->grouplimits, key, len, (void **) &jpti);
    if (found) {
        return jpti->of_limits;
    }
    return NULL;
}
jp_limits* get_equity_option_limits_for_clraccnt(jp_limits_checker * jc,
        char *key, int len)
{
    jp_group_limits* jpti = NULL;
    int found = get_map_entry(jc->grouplimits, key, len, (void **) &jpti);
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

int  check_trader_msg_rate(jp_limits_checker* jpc, char* trader, int tlen, char sec_type)
{
    int ret = 1;
    jp_trader_info *jpti = 0;
    int found =
        get_map_entry(jpc->traders, trader,
                tlen, (void **) &jpti);
    if (found) {
        if(sec_type == 'E') {
            ret = check_msg_rate(jpti->eq_limits);
        } else if(sec_type == 'O') {
            ret = check_msg_rate(jpti->opt_limits);
        } else {
            ret = check_msg_rate(jpti->fut_limits);
        }
    }
    return ret;
}

int update_balance(jp_limits_checker * jpc, rcntr * rc)
{
    int allow = 1;
    jp_trader_info *jpti = 0;
    int found =
        get_map_entry(jpc->traders, rc->owner,
                rc->olen, (void **) &jpti);
    if (found) {
        long price = rc->price;
        int shares = rc->size;
        long dollar = 0;
        int cum = rc->cum;
        dollar = (price * (shares - cum));
        shares -= cum;
        allow = base_update(jpc, jpti, rc->sec_type, (shares * -1), -1, dollar,
                0, 0, rc, price, rc->side);
        if(allow) { 
            allow = billinggrp_update(jpc, rc->clr_acc, rc->clr_len,
                    rc->sec_type,  (shares * -1), -1, dollar, 0, 0, rc, rc->side);
        }
    }
    return allow;
}

int update_balance_fill(jp_limits_checker * jpc, rcntr * rc)
{
    int allow = 1;
    jp_trader_info *jpti = 0;
    int found =
        get_map_entry(jpc->traders, rc->owner,
                rc->olen, (void **) &jpti);
    if (found) {
        long dollar = 0;
        int orders = 0;
        dollar = (rc->price * rc->last);
        if (rc->cum >= rc->size) {
            orders = -1;
        }
        allow = base_update(jpc, jpti, rc->sec_type, (rc->last * -1), orders,
                dollar, 0, 0, rc, rc->price, rc->side);
        if(allow) { 
            allow = billinggrp_update(jpc, rc->clr_acc, rc->clr_len,
                    rc->sec_type,  (rc->last * -1), orders, dollar, 0, 0, rc, rc->side);
        }
    }
    return allow;
}

static void rewind_update(jp_limits_checker* jpc, jp_trader_info * jpti, char sec_type, long shares,
        long orders, long dollars, int add, char side)
{
    jp_limits *jp = get_limits_for_sec(jpc, jpti, sec_type);
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
int update_balance_new(jp_limits_checker * jpc, rcntr* rc)
{
    int allow = 0;
    jp_trader_info *jpti = 0;
    int found =
        get_map_entry(jpc->traders, rc->owner, 
                rc->olen, (void **) &jpti);
    if (found) {
        if (jpti->blocked) {
            char temp[32];
            memset(temp, '\0', 32);
            memcpy(temp, rc->owner, rc->olen);
            rc->err_len = sprintf(rc->errStr, "%s is Blocked", temp);
            return 0;
        }
        long dollar = rc->price * rc->size;
        allow =
            base_update(jpc, jpti, rc->sec_type, rc->size, 1, dollar, 1, 1, rc, rc->price, rc->side);
        if(allow) { 
            allow = billinggrp_update(jpc, rc->clr_acc, rc->clr_len,
                    rc->sec_type, rc->size, 1, dollar, 1, 1, rc, rc->side);
            if(!allow) {
              rewind_update(jpc, jpti, rc->sec_type, rc->size, 1, dollar, 1, rc->side);
            }
        }

    }
    return allow;
}
/**
 * 
 */
int update_balance_replace(jp_limits_checker * jpc, rcntr * rc, rcntr* rep)
{
    int allow = 0;
    jp_trader_info *jpti = 0;
    int found =
        get_map_entry(jpc->traders, rc->owner,
                rc->olen, (void **) &jpti);
    if (found) {
        if (jpti->blocked) {
            return 0;
        }
        long dollar = (rep->size * rep->price) - (rc->size * rc->price);
        long shares = rep->size - rc->size;
        allow =
            base_update(jpc, jpti, rc->sec_type, shares, 0, dollar,
                    1, 1, rc,rc->price, rc->side);
        if(allow) { 
            allow = billinggrp_update(jpc, rc->clr_acc, rc->clr_len,
                    rc->sec_type,  shares, 0, dollar, 1, 1, rc, rc->side);
            if(!allow) {
              rewind_update(jpc, jpti, rc->sec_type, rc->size, 1, dollar, 1, rc->side);
            }
        }
    }
    return allow;
}
/**
 * Users need to send in the RCNTR of the replace to pull it out.
 */
int update_balance_rep_rej(jp_limits_checker * jpc, rcntr * rc, rcntr* rep)
{
    int allow = 1;
    jp_trader_info *jpti = 0;
    int found =
        get_map_entry(jpc->traders,rc->owner,
                rc->olen, (void **) &jpti);
    if (found) {
        long dollar = (rep->size * rep->price) - (rc->size * rc->price);
        long shares = rep->size - rc->size;
        allow =
            base_update(jpc, jpti, rc->sec_type, (shares * -1), 0,
                    dollar, 0, 0, rc, rc->price, rc->side); 
        if(allow) { 
            allow = billinggrp_update(jpc, rc->clr_acc, rc->clr_len,
                    rc->sec_type,  (shares * -1), -1, dollar, 0, 0, rc, rc->side);
        }
    }
    return allow;
}

int validate_user(jp_limits_checker* jc, char* username, int ulen,
        char* password, int plen)
{
    pthread_spin_lock(&jc->jp_protector);
    int ret = contains_key(jc->allowed_logons, username,
            ulen);
    pthread_spin_unlock(&jc->jp_protector);
    return ret;
}
login_info* get_logon_struct(void* checkah, char* p, int plen,
        char* u, int ulen)
{
    jp_limits_checker* jc = (jp_limits_checker*)checkah;
    pthread_spin_lock(&jc->jp_protector);
    login_info* li = 0;
    int ret = find_n_fill(jc->allowed_logons, u,
            ulen, (void**)&li);
    pthread_spin_unlock(&jc->jp_protector);
    if(ret > 0) {
        return li;
    } else {
        return NULL;
    }
}

static void set_reset_time(jp_limits_checker* jc, login_info *in, ex_config * mc)
{
    in->reset_hour = mc->reset_hour;
    in->reset_min = mc->reset_min;
    in->create_new = mc->create_new;
    in->new_incoming = 0;
    in->new_outgoing = 0;
    in->reset_day = 7;
    in->last_logon = mc->last_logon;
    if (mc->create_new) {
        reset_seq(jc->o, in->name, in->nlen);
    }
}


static void set_engine_logons(jp_limits_checker * jc)
{
    int i, ret_len;
    for (i = 0; i < 200; ++i) {
        login_info *in = get_stuff(jc->init_maker);
        ret_len = sprintf(in->name, "ROM%d", i);
        in->nlen = ret_len;

        ret_len = sprintf(in->password, "ROM%d", i);
        in->plen = ret_len;
        in->do_balance = 0;
        in->cancel_on_dis = 0;
        set_reset_time(jc, in, jc->mc);
        pthread_spin_lock(&jc->jp_protector);
        no_copy_insert(jc->allowed_logons, in->name, in->nlen, in);
        pthread_spin_unlock(&jc->jp_protector);
    }
}

static void clean_equity_sym(jp_limits_checker* jpc)
{
    pthread_spin_lock(&jpc->jp_protector);
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
    pthread_spin_unlock(&jpc->jp_protector);
}

static void clean_flim_map(hashmap_t map)
{
    jp_fut_limits* flimits = 0;
    char *key = 0;
    int key_len = 0;
    hashmap_iterator_t hi = hashmap_iterator_begin(map);
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                (void *) &flimits);
    while (valid) {
        free(flimits);
        valid = hashmap_iterator_next(map, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                    &key_len, (void *) &flimits);
    }
    hashmap_iterator_destroy(hi);
    delete_map(map);
}
static void clean_fut_map(hashmap_t map)
{
    hashmap_t fut = 0;
    char *key = 0;
    int key_len = 0;
    hashmap_iterator_t hi = hashmap_iterator_begin(map);
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                (void *) &fut);
    while (valid) {
        clean_flim_map(fut);
        fut = 0;
        valid = hashmap_iterator_next(map, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                    &key_len, (void *) &fut);
    }
    hashmap_iterator_destroy(hi);
    delete_map(map);
}
void destroy_balance_checker(jp_limits_checker* jplc)
{
    close_mds_client(jplc->mdc);
    delete_map(jplc->traders);
    clean_equity_sym(jplc);
    delete_map(jplc->equity_sym);
    delete_map(jplc->grouplimits);
    rb_tree_destroy(jplc->billing_groups);
    delete_map(jplc->allowed_logons);
    clean_fut_map(jplc->trd_fut_limits);
    delete_map(jplc->opt_fut_ex);
    free(jplc->db_user_name);
    free(jplc->db_password);
    free(jplc->db_server);
    free(jplc->db_inst);
    destroy_mem_glob(jplc->trd_maker);
    destroy_mem_glob(jplc->clr_maker);
    destroy_mem_glob(jplc->bg_maker);
    destroy_mem_glob(jplc->init_maker);
    free(jplc);
}

void update_equity_sym_chart(void* me, int is_connected)
{
    jp_limits_checker* jp = (jp_limits_checker*)me;
    if(!is_connected) {
        clean_equity_sym(jp);
        pthread_spin_lock(&jp->jp_protector);
        jp->check_eq = 0;
        delete_map(jp->equity_sym);
        jp->equity_sym = create_map(8192);
        pthread_spin_unlock(&jp->jp_protector);
    } else {
        jp->check_eq = 1;
    }
}


static int fill_in_config_from_db(jp_limits_checker* jc, ex_config* ex, char* target)
{
    int ret = 0;
    queue_t q = create_sized_queue(256);
    char sqlstr[2048];
    sprintf(sqlstr, "SELECT d.connect_hr, d.connect_min, d.connect_day, "
                              "d.disconnect_hr, d.disconnect_min, "
                              "d.port, "
                              "i.ip FROM dart_connections d, ip_definition i "
                              "WHERE d.source_id = '%s' AND d.target_id = '%s' "
                              "AND i.id = d.ip_id",
                               ex->mod_name, target);
    char err[64];
    memset(err, '\0', 64);
    int error = psql_gendb_get_results(jc->db_user_name, jc->db_user_name_len,
            jc->db_password, jc->db_password_len, jc->db_inst, jc->db_inst_len,
            jc->db_server, jc->db_server_len, q, sqlstr, err, 64);
    if(error == 0) {
        queue_t row = NULL;
        gendb_v *val = NULL;
        element_t erow, ecol;
        while (!is_empty(q)) {
            ret = 1;
            int cnt = 0;
            erow = pop_front(q);
            row = (queue_t) (erow->d);
            while (!is_empty(row)) {
                ecol = pop_front(row);
                val = (gendb_v *) (ecol->d);
                switch (cnt) {
                    case 0:
                        ex->reset_hour = (val->g_un).un_int;
                        break;
                    case 1:
                        ex->reset_min = (val->g_un).un_int;
                        break;
                    case 2:
                        ex->reset_day = (val->g_un).un_int;
                        break;
                    case 3:
                        ex->end_hour = (val->g_un).un_int;
                        break;
                    case 4:
                        ex->end_min = (val->g_un).un_int;
                        break;
                    case 5:
                        ex->port = (val->g_un).un_int;
                        break;
                    case 6:
                        if(val->arr_len > 0) {
                            ex->ip = calloc(val->arr_len + 1, 1);
                            memcpy(ex->ip, ((val->g_un).un_array), val->arr_len);
                            ex->ip_len = val->arr_len;
                        }
                        break;
                }
                ++cnt;
            }
        }
    } else {
        send_debug_message(jc->o, "Failed to load dart_connection to build ex: %s \n", err);
        return ret;
    }
    destroy_queue(q);
    return ret;
}

void update_config_from_db(jp_limits_checker* jc, ex_config* ex, char* target)
{
    fill_in_config_from_db(jc, ex, target);
}
ex_config* create_config_from_db(jp_limits_checker* jc, char* mod_num, int clen,
        char* target)
{

    ex_config* ex = 0x0;
    ex = calloc(1, sizeof(struct ex_con));
    ex->mod_name = mod_num;
    ex->mod_name_len = clen;
    int worked = fill_in_config_from_db(jc, ex, target);
    if(worked) {
        ex->order_allocation = 512;
        return ex;
    } else {
        free(ex);
        return NULL;
    }
}

void set_rom_name_from_num(jp_limits_checker* jc, ex_config* ex,
        char* mod_num, int mod_len)
{
    queue_t q = create_sized_queue(16);
    char sqlstr[2048];

    memset(sqlstr, '\0', 2048);
    sprintf(sqlstr, "SELECT ex_short_name FROM omdestination "
                              "WHERE destid = '%s'",
                               mod_num);

    char err[64];
    memset(err, '\0', 64);
    int error = psql_gendb_get_results( jc->db_user_name, jc->db_user_name_len,
            jc->db_password, jc->db_password_len, jc->db_inst, jc->db_inst_len,jc->db_server,
            jc->db_server_len, q, sqlstr, err, 64);
    if(error == 0) {
        queue_t row = NULL;
        gendb_v *val = NULL;
        element_t erow, ecol;
        while (!is_empty(q)) {
            erow = pop_front(q);
            row = (queue_t) (erow->d);
            while (!is_empty(row)) {
                ecol = pop_front(row);
                val = (gendb_v *) (ecol->d);
                ex->username = calloc(val->arr_len + 1, 1);
                memcpy(ex->username, ((val->g_un).un_array), val->arr_len);
                ex->password = calloc(val->arr_len + 1, 1);
                memcpy(ex->password, ((val->g_un).un_array), val->arr_len);
                ex->ulen = val->arr_len;
                ex->plen = val->arr_len;
            }
        }
    } else {
        send_debug_message(jc->o, "Failed to load ex_short_name to build ex: %s \n", err);
        ex->username= calloc(mod_len + 4, 1);
        ex->password= calloc(mod_len + 4, 1);
        memcpy(ex->username, "ROM", 3);
        memcpy(ex->password, "ROM", 3);
        memcpy(ex->username + 3, mod_num, 
                mod_len);
        ex->ulen = mod_len + 3;
        memcpy(ex->password + 3, mod_num, 
                mod_len);
        ex->plen = mod_len + 3;
    }
    destroy_queue(q);
}

char* get_mod_name_for_user(ex_config* jc, char* username, int* dlen)
{
    queue_t q = create_sized_queue(8);
    char sqlstr[2048];

    sprintf(sqlstr, "SELECT destid FROM omuser_om_map "
                              "WHERE username = '%s'",
                               username);

    char err[64];
    memset(err, '\0', 64);
    int error = psql_gendb_get_results( jc->db_user_name, jc->db_user_name_len,
            jc->db_password, jc->db_password_len, jc->db_inst, jc->db_inst_len,jc->db_server,
            jc->db_server_len, q, sqlstr, err, 64);
    if(error == 0) {
        queue_t row = NULL;
        gendb_v *val = NULL;
        element_t erow, ecol;
        while (!is_empty(q)) {
            erow = pop_front(q);
            row = (queue_t) (erow->d);
            while (!is_empty(row)) {
                ecol = pop_front(row);
                val = (gendb_v *) (ecol->d);
                char* mod = calloc(8, 1);
                int len = sprintf(mod, "%d", (val->g_un).un_int);
                *dlen = len;
                destroy_queue(q);
                return mod;
            }
        }
    } else {
        printf("Failed to find Dest for user: %s \n", username);
    }
    destroy_queue(q);
    return NULL;
}

jp_limits_checker *create_jp_limits_checker(ex_config * ex,  otl* o,
        char* mod_name, int mlen)
{
    return create_limits_checker(ex, o, mod_name, mlen,balance_mds_update,
           update_equity_sym_chart); 
}

jp_limits_checker *create_limits_checker(ex_config * ex,  otl* o,
        char* mod_name, int mlen, 
        void (*mds_update)(void* cbo, char* sym, int slen, long bid, long offer),
        void (*con_update)(void* cbo, int is_connected))
{
    jp_limits_checker *jplc = malloc(sizeof *jplc);
    jplc->traders = create_map(256);
    jplc->equity_sym = create_map(8192);
    jplc->check_eq = 0;
    jplc->grouplimits = create_map(256);
    jplc->billing_groups = create_rb_tree(billing_key_compare,
            billing_key_destroy,
            billing_info_destroy,
            billing_int_print,
            billing_info_print);
    jplc->opt_fut_ex = create_map(32);
    jplc->clr_to_bill = create_map(256);
    jplc->trd_fut_limits = create_map(256);
    jplc->allowed_logons = create_map(256);
    pthread_spin_init(&jplc->jp_protector, PTHREAD_PROCESS_PRIVATE);
    jplc->mc = ex;
    jplc->o = o;
    jplc->trd_maker = create_mem_glob(512, init_trader_limits);
    jplc->clr_maker = create_mem_glob(464, init_clr_limits);
    jplc->bg_maker = create_mem_glob(464, init_clr_limits);
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
    load_traders_for_dest(jplc, jplc->mc->mod_name);
    load_clraccnt_limit(jplc, NULL);
    set_engine_logons(jplc);
    if(mlen > 0) {
        ex_config* mdsconfig = create_config_from_db(jplc, mod_name, mlen, "3466");
        if(mdsconfig && mdsconfig->ip_len > 0) {
            mdsconfig->username = mod_name;
            mdsconfig->ulen = mlen;
            send_debug_message(jplc->o, "Starting MDS IP: %s, user: %s \n", mdsconfig->ip, mdsconfig->username);
            jplc->mdc  = create_mds_client(mdsconfig, o,jplc, 
                    balance_mds_update,update_equity_sym_chart);
        } else {
            jplc->mdc = 0x0;
        }
    } else {
        jplc->mdc = 0x0;
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
                "FutureLimits l WHERE l.Product = f.Product");
    }

    char err[64];
    memset(err, '\0', 64);
    int error = psql_gendb_get_results( jc->db_user_name, jc->db_user_name_len,
            jc->db_password, jc->db_password_len, jc->db_inst, jc->db_inst_len,jc->db_server,
            jc->db_server_len, q, sqlstr, err, 64);
    if(error == 0) {
        queue_t row = NULL;
        gendb_v *val = NULL;
        element_t erow, ecol;
        while (!is_empty(q)) {
            int cnt = 0;
            erow = pop_front(q);
            row = (queue_t) (erow->d);
            hashmap_t user_limits = 0x0;
            jp_fut_limits* flimits = 0;
            char name[128];
            int name_len = 0;
            while (!is_empty(row)) {
                ecol = pop_front(row);
                val = (gendb_v *) (ecol->d);
                switch (cnt) {
                    case 0:
                        if(val->arr_len > 0) {
                            memset(name, '\0', 128);
                            memcpy(name, ((val->g_un).un_array), val->arr_len); 
                            name_len = val->arr_len;
                            /* Check to see if we have limits for this user*/ 
                            res =
                                get_map_entry(jc->trd_fut_limits, name, name_len,
                                        (void **) &user_limits);
                            if (res <= 0) {
                                /* If we do not create and add the user map.*/
                                user_limits = create_map(32);
                                no_copy_insert(jc->trd_fut_limits, name,name_len,
                                        user_limits);

                            }
                        }
                        break;
                    case 1:
                        if(val->arr_len > 0 && user_limits != 0x0) {
                                res = get_map_entry(user_limits,((val->g_un).un_array), val->arr_len,(void**)&flimits);
                                if(res > 0) {
                                    // do nothing we will update limits
                                } else {
                                    int ret = 0;
                                    REX_ALLOC(flimits, 8, sizeof(jp_fut_limits), ret);
                                    if(name_len > 0) {
                                        flimits->nlen = name_len;
                                        memcpy(flimits->name, name, flimits->nlen);
                                    }
                                    memcpy(flimits->product, ((val->g_un).un_array), val->arr_len);
                                    flimits->plen = val->arr_len;
                                    no_copy_insert(user_limits, 
                                            flimits->product, flimits->plen, flimits);
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
        send_debug_message(jc->o, "Failed to load Future Limits: %s \n", err);
    }
    destroy_queue(q);
}

static void load_clr_to_bill(jp_limits_checker* jc)
{
    databuf_t* seq_db = databuf_alloc(1024);
    databuf_write(seq_db, "SELECT * FROM ClearingActBillingGroupMap");
    char* sql_str = calloc(databuf_unread(seq_db)+1, 1);
    memcpy(sql_str, seq_db->rd_ptr, databuf_unread(seq_db));
    queue_t q = create_sized_queue(2);
    char err[64];
    memset(err, '\0', 64);
    int error = psql_gendb_get_results( jc->db_user_name, jc->db_user_name_len,
            jc->db_password, jc->db_password_len, jc->db_inst, jc->db_inst_len,jc->db_server,
            jc->db_server_len, q, sql_str, err, 64);
    if(error == 0) {
        queue_t row = NULL;
        gendb_v* val = NULL;
        element_t erow, ecol;
        erow = 0x0;
        ecol = 0x0;
        while (!is_empty(q)) {
            erow = pop_front(q);
            row = (queue_t) (erow->d);
            int cnt = 0;
            char* clracc = 0x0;
            int clr_len = 0;
            int* data =calloc(1,sizeof(int));
            while (!is_empty(row)) {
                ecol = pop_front(row);
                val = (gendb_v *) (ecol->d);
                switch(cnt) {
                    case 0:
                        {
                            if(val->arr_len > 0) {
                                clracc = calloc(1, val->arr_len +1);
                                memcpy(clracc, (val->g_un).un_array, val->arr_len);
                                clr_len = val->arr_len;
                                no_copy_insert(jc->clr_to_bill, clracc,clr_len, data);
                            }
                        }
                        break;
                    case 1:
                        *data = (val->g_un).un_int;
                        break;
                }
                ++cnt;
                free_element(row, ecol);
                gendb_destroy(val);
            }
            if(clracc != 0x0) {
                jp_group_limits* bjpti =
                    (jp_group_limits*)rb_tree_get_val(jc->billing_groups, data);
                if(bjpti) {
                    no_copy_insert(jc->grouplimits, clracc, clr_len,
                                    bjpti);
                }
            }
            free_element(q, erow);
            destroy_queue(row);
        }
    } else {
        send_debug_message(jc->o, "Clr To Bill FAILED!! The error is: %s \n", err);
    }
}

static char* load_rom_name(char* mod_num, int num_len,
        int* mod_len, jp_limits_checker* jc)
{
    databuf_t* seq_db = databuf_alloc(1024);
    char* mod_name = calloc(16,1);
    databuf_write(seq_db, "SELECT ex_short_name FROM omdestination WHERE contra_acr='%s'", mod_num);
    char* sql_str = calloc(databuf_unread(seq_db)+1, 1);
    memcpy(sql_str, seq_db->rd_ptr, databuf_unread(seq_db));
    queue_t q = create_sized_queue(2);
    char err[64];
    memset(err, '\0', 64);
    int error = psql_gendb_get_results( jc->db_user_name, jc->db_user_name_len,
            jc->db_password, jc->db_password_len, jc->db_inst, jc->db_inst_len,jc->db_server,
            jc->db_server_len, q, sql_str, err, 64);
    if(error == 0) {
        queue_t row = NULL;
        gendb_v* val = NULL;
        element_t erow, ecol;
        erow = 0x0;
        ecol = 0x0;
        while (!is_empty(q)) {
            erow = pop_front(q);
            row = (queue_t) (erow->d);
            while (!is_empty(row)) {
                ecol = pop_front(row);
                val = (gendb_v *) (ecol->d);
                *mod_len =  val->arr_len;
                memcpy(mod_name, (val->g_un).un_array, val->arr_len);
                free_element(row, ecol);
                gendb_destroy(val);
            }
            free_element(q, erow);
            destroy_queue(row);
        }
    } else {
        send_debug_message(jc->o, "Get contra_acr FAILED!! The error is: %s \n", err);
    }
    if(*mod_len == 0) {
        *mod_len = sprintf(mod_name, "ROM%s", mod_num);
    }
    return mod_name;
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
void init_user(jp_limits_checker* jc, login_info* li)
{
    time_t now;
    time(&now);
    li->new_incoming = 0;
    li->new_outgoing = 0;
    int create_new = ll_before_reset_time(li->reset_day,
            li->reset_hour,
            li->reset_min,
            li->last_logon, now);
    if(create_new) {
        /*send_debug_message(jc->o, "Clearing sequence files for: %s hour:%d, min:%d, day:%d, Last logon:%d\n",
          li->name,
          li->reset_hour, li->reset_min,
          li->reset_day, li->last_logon);*/
        reset_seq(jc->o, li->name, li->nlen);
    } else {
        send_debug_message(jc->o, "Continuing sequence for: %s hour:%d, min:%d, day:%d, Last logon:%d\n",
                li->name,
                li->reset_hour, li->reset_min,
                li->reset_day, li->last_logon);
    }
    li->create_new = 0;
}

void load_billing_group(jp_limits_checker* jc, const char* trader)
{
    queue_t q = create_sized_queue(256);
    char sqlstr[256];
    memset(sqlstr, '\0', 256);
    if(trader) {
        sprintf(sqlstr, "SELECT * FROM billinglimits where groupid=%s", trader);
    } else {
        sprintf(sqlstr, "SELECT * FROM billinglimits");
    }
    char err[64];
    memset(err, '\0', 64);
    int error = psql_gendb_get_results( jc->db_user_name, jc->db_user_name_len,
            jc->db_password, jc->db_password_len, jc->db_inst, jc->db_inst_len,jc->db_server,
            jc->db_server_len, q, sqlstr, err, 64);
    if(error == 0) {
        jp_group_limits* jpti = 0;
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
                        {
                            int groupid = (val->g_un).un_int;
                            jpti =
                                (jp_group_limits*)rb_tree_get_val(jc->billing_groups, &groupid);
                            if (jpti == NULL) {
                                jpti = get_stuff(jc->clr_maker);
                                int* in_sock = calloc(sizeof(int), 1);
                                *in_sock = groupid;
                                pthread_spin_lock(&jc->jp_protector);
                                rb_tree_no_delete_insert(jc->billing_groups, in_sock,
                                        jpti);
                                pthread_spin_unlock(&jc->jp_protector);
                            }
                        }
                        break;
                    case 1:
                        jpti->eq_limits->max_shares_ord = (val->g_un).un_int;
                        break;
                    case 2:
                        jpti->opt_limits->max_shares_ord = (val->g_un).un_int;
                        break;
                    case 3:
                        jpti->fut_limits->max_shares_ord = (val->g_un).un_int;
                        break;
                    case 4:
                        jpti->of_limits->max_shares_ord = (val->g_un).un_int;
                        break;
                    case 5:
                        jpti->eq_limits->max_doll =
                            ((unsigned long) (val->g_un).un_double) * EQ_MULT;
                        break;
                    case 6:
                        jpti->opt_limits->max_doll =
                            ((unsigned long) (val->g_un).un_double) * EQ_MULT;
                        break;
                    case 7:
                        jpti->fut_limits->max_doll =
                            ((unsigned long) (val->g_un).un_double) * FUT_MULT;
                        break;
                    case 8:
                        jpti->of_limits->max_doll =
                            ((unsigned long) (val->g_un).un_double) * FUT_MULT;
                        break;
                    case 9:
                        jpti->eq_limits->max_ord = (val->g_un).un_int;
                        break;
                    case 10:
                        jpti->opt_limits->max_ord = (val->g_un).un_int;
                        break;
                    case 11:
                        jpti->fut_limits->max_ord = (val->g_un).un_int;
                        break;
                    case 12:
                        jpti->of_limits->max_ord = (val->g_un).un_int;
                        break;
                    case 13:
                        jpti->eq_limits->max_share = (val->g_un).un_int;
                        break;
                    case 14:
                        jpti->opt_limits->max_share = (val->g_un).un_int;
                        break;
                    case 15:
                        jpti->fut_limits->max_share = (val->g_un).un_int;
                        break;
                    case 16:
                        jpti->of_limits->max_share = (val->g_un).un_int;
                        break;
                    case 17:
                        jpti->eq_limits->bp =
                            ((unsigned long) (val->g_un).un_double) * EQ_MULT;
                        break;
                    case 18:
                        jpti->opt_limits->bp =
                            ((unsigned long) (val->g_un).un_double) * EQ_MULT;
                        break;
                    case 19:
                        jpti->fut_limits->bp =
                            ((unsigned long) (val->g_un).un_double) * FUT_MULT;
                        break;
                    case 20:
                        jpti->of_limits->bp =
                            ((unsigned long) (val->g_un).un_double) * FUT_MULT;
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
        send_debug_message(jc->o, "Failed to load Billing group Limits: %s \n", err);
    }
    destroy_queue(q);
}

void load_traders_for_dest(jp_limits_checker* jc, const char* mod_name)
{
    queue_t q = create_sized_queue(256);
    char err[64];
    memset(err, '\0', 64);
    char* sqlstr = calloc(256, 1);
    char trader[64];
    memset(trader, '\0', 64);
    int slen = sprintf(sqlstr, "SELECT username FROM omuser_om_map WHERE destid = %s", mod_name);
    if(slen > 0) {
        int error = psql_gendb_get_results( jc->db_user_name, jc->db_user_name_len,
                jc->db_password, jc->db_password_len, jc->db_inst, jc->db_inst_len,jc->db_server,
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
                        case 0:   //username
                            if(val->arr_len > 0) {
                                memset(trader, '\0', 64);
                                memcpy(trader, ((val->g_un).un_array),
                                        val->arr_len);
                                load_trader_profiles(jc, trader);
                            }
                    }
                }
            }
        }
    }
}
void load_trader_profiles(jp_limits_checker * jc, const char* trader)
{
    queue_t q = create_sized_queue(256);
    jp_trader_info *jpti = 0;
    login_info *in = 0;
    int mod_len = 0;
    char* mod_name = load_rom_name(jc->mc->mod_name, jc->mc->mod_name_len, &mod_len, jc);
    int res = 0;
    int len = 0;
    char* sqlstr = build_sql_string((char*)trader, mod_name); 

    char err[64];
    memset(err, '\0', 64);
    int error = psql_gendb_get_results( jc->db_user_name, jc->db_user_name_len,
            jc->db_password, jc->db_password_len, jc->db_inst, jc->db_inst_len,jc->db_server,
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
                    case 0:   //username
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
                            } else {
                                pthread_spin_lock(&jc->jp_protector);
                                res = get_map_entry(jc->allowed_logons, jpti->name,
                                        jpti->name_len, (void**)&in);
                                pthread_spin_unlock(&jc->jp_protector);
                            }
                        }
                        break;
                    case 1:  //password
                        if (res <= 0 && val->arr_len > 0) {
                            memcpy(jpti->password, ((val->g_un).un_array), val->arr_len);
                            jpti->pass_len = val->arr_len;
                            in = get_stuff(jc->init_maker);
                            memcpy(in->name, jpti->name, jpti->name_len);
                            in->nlen = jpti->name_len;

                            memcpy(in->password, jpti->password, jpti->pass_len);
                            in->plen = jpti->pass_len;
                            in->do_balance = 1;
                            in->cancel_on_dis = 0;
                            pthread_spin_lock(&jc->jp_protector);
                            no_copy_insert(jc->allowed_logons, in->name, in->nlen, in);
                            pthread_spin_unlock(&jc->jp_protector);
                        }
                        break;
                    case 2:
                        in->reset_day = (val->g_un).un_int;
                        break;//resetday
                    case 3:
                        in->reset_hour = (val->g_un).un_int;
                        break; //reset_hour
                    case 4:
                        in->reset_min = (val->g_un).un_int;
                        break; //reset_min
                    case 6:
                        jpti->eq_limits->bp =
                            ((unsigned long) (val->g_un).un_double) * EQ_MULT;
                        break;
                    case 7:
                        jpti->opt_limits->bp =
                            (unsigned long) (val->g_un).un_double * EQ_MULT;
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
                            (unsigned long) (val->g_un).un_double * EQ_MULT;
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
                        jpti->eq_limits->mrate = (val->g_un).un_int;
                        break; //msg_rate_eq
                    case 27:
                        jpti->opt_limits->mrate = (val->g_un).un_int;
                        break; //msg_rate_opt
                    case 28:
                        jpti->fut_limits->mrate = (val->g_un).un_int;
                        break; //msg_rate_fut
                    case 29:
                        jpti->of_limits->mrate = (val->g_un).un_int;
                        break; //msg_rate_of
                    case 30:
                        in->last_logon = (val->g_un).un_int;
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
            if(in) {
                init_user(jc, in);
            }
        }
    } else {
        send_debug_message(jc->o, "Failed to load Trader Limits: %s \n", err);
    }
    destroy_queue(q);
    free(sqlstr);
    load_future_limits(jc, trader);
}



void load_clraccnt_limit(jp_limits_checker* jc, const char* key)
{
    load_billing_group(jc, key);
    load_clr_to_bill(jc);
}


void set_order_in_interface(jp_limits_checker * jc, gatekeeper * gk,
        rcntr * cnt, int from_a_rom)
{
    dart_order_obj* doj = (dart_order_obj*)cnt->raw;
    con_int* ci = 0;
    int found =
        get_obj_by_client_id(gk, doj->positions[ROM_SENDER].iov_base,
                doj->positions[ROM_SENDER].iov_len,
                (void **) &ci);
    if (found <= 0) {
        ci = init_client_con(doj->positions[ROM_SENDER].iov_base,
                doj->positions[ROM_SENDER].iov_len, gk);
        set_obj_by_client_id(gk, doj->positions[ROM_SENDER].iov_base,
                doj->positions[ROM_SENDER].iov_len, ci);
        send_debug_message(jc->o, "Setting this Sender:: %s sendlen:%d, pointer:%lu rcntr: %lu\n",
                ((char*)doj->positions[ROM_SENDER].iov_base),
                doj->positions[ROM_SENDER].iov_len, ci, cnt);
    }
    if(ci) {
        pthread_spin_lock(&ci->mutex);
        int offset = from_a_rom ? ROM_TAG : ROM_CLIENT_ID;
        if(ci->logon) {
            ci->logon->do_balance = from_a_rom ? 0 : 1;
        } else {
            ci->logon = get_logon_struct(jc,
                    doj->positions[ROM_SENDER].iov_base,
                    doj->positions[ROM_SENDER].iov_len,
                    doj->positions[ROM_SENDER].iov_base,
                    doj->positions[ROM_SENDER].iov_len);
        }
        no_copy_insert(ci->cl_ord_id_map,
                doj->positions[offset].iov_base,
                doj->positions[offset].iov_len, cnt);
        pthread_spin_unlock(&ci->mutex);
        doj->sender = ci;

        if (!from_a_rom) {
            update_balance_new(jc, cnt);
        }
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
    int error = psql_gendb_get_results( jc->db_user_name, jc->db_user_name_len,
            jc->db_password, jc->db_password_len, jc->db_inst, jc->db_inst_len,jc->db_server,
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
        send_debug_message(jc->o, "Failed to load Futures Dests: %s \n", err);
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
    int error = psql_gendb_get_results( jc->db_user_name, jc->db_user_name_len,
            jc->db_password, jc->db_password_len, jc->db_inst, jc->db_inst_len,jc->db_server,
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
        send_debug_message(jc->o, "Failed to load destinations: %s \n", err);
    }
    destroy_queue(q);
    return 0;
}

int get_orders(jp_limits_checker * jc, gatekeeper * gk,
        databuf_t *dsql_str, char *mod_name, int mod_len)
{
    char* sql_str = calloc(databuf_unread(dsql_str)+1, 1);
    memcpy(sql_str, dsql_str->rd_ptr, databuf_unread(dsql_str));
    queue_t q = create_sized_queue(256);
    //jp_trader_info *jpti = 0;
    char err[64];
    memset(err, '\0', 64);
    int error = psql_gendb_get_results( jc->db_user_name, jc->db_user_name_len,
            jc->db_password, jc->db_password_len, jc->db_inst, jc->db_inst_len,jc->db_server,
            jc->db_server_len, q, sql_str, err, 64);
    if(error == 0) {
        queue_t row = NULL;
        gendb_v *val = NULL;
        element_t erow, ecol;
        while (!is_empty(q)) {
            int count = 0;
            erow = pop_front(q);
            row = (queue_t) (erow->d);
            dart_order_obj *doj = (dart_order_obj *) get_gk_obj(gk);
            rcntr* cnt = get_cnt(gk);
            cnt->raw = doj;
            cnt->rlen = sizeof(dart_order_obj);
            cnt->cum = 0;
            cnt->last = 0;
            while (!is_empty(row)) {
                ecol = pop_front(row);
                val = (gendb_v *) (ecol->d);
                if(val->type_in_union != DB_NULL) {
                    switch(val->type_in_union) {
                        case DB_INT:
                            set_int_vals(doj, order_cln[count].field_id, 
                                    (val->g_un).un_int);
                            switch(order_cln[count].field_id)
                            {
                                case ROM_CUM:
                                    cnt->cum = (val->g_un).un_int;
                                    set_rom_field(doj, ROM_BIN_CUM, &((val->g_un).un_int), 4);
                                    break;
                                case ROM_MAX_FLOOR:
                                    set_rom_field(doj, ROM_BIN_MAXFLOOR, &((val->g_un).un_int), 4);
                                    break;
                                case ROM_SHARES:
                                    cnt->size = (val->g_un).un_int;
                                    set_rom_field(doj, ROM_BIN_QTY, &((val->g_un).un_int), 4);
                                    break;
                                case ROM_STATUS_CNT:
                                    set_ex_tag(doj, (val->g_un).un_long);
                                    break;
                            }
                            break;
                        case DB_LONG:
                            switch(order_cln[count].field_id)
                            {
                                case ROM_STATUS_CNT:
                                    set_ex_tag(doj, (val->g_un).un_long);
                                    break;
                            }
                            break;
                        case DB_DOUBLE:
                            set_double_vals(doj, order_cln[count].field_id,
                                    (val->g_un).un_double);
                            if(order_cln[count].field_id == ROM_PRICE) {
                                set_rom_field(doj, ROM_BIN_PRICE, &((val->g_un).un_double), sizeof(double));
                            }
                            break;
                        case DB_ARRAY:
                            set_rom_field(doj, order_cln[count].field_id, (val->g_un).un_array, val->arr_len);
                            break;
                        case DB_CHAR:
                            set_rom_field(doj, order_cln[count].field_id, &((val->g_un).un_char), 1);
                            break;
                        default:
                            send_debug_message(jc->o, "Got a Type we didn't expect: %d, at %d\n",
                                    val->type_in_union, count);
                            break;
                    }
                }
                ++count;
                free_element(row, ecol);
                gendb_destroy(val);
            }
            if(cnt) {
                if(doj->positions[ROM_SEC_TYPE].iov_len > 0) {
                    cnt->sec_type = (((char*)doj->positions[ROM_SEC_TYPE].iov_base)[0]);
                } else {
                    cnt->sec_type = 'E';
                }
                if(doj->positions[ROM_SYM].iov_len > 0) {
                    cnt->sym = doj->positions[ROM_SYM].iov_base; 
                    cnt->slen = doj->positions[ROM_SYM].iov_len;
                } else {
                    cnt->slen = 0;
                }
                if(doj->positions[ROM_PRICE].iov_len > 0) {
                    jiggly_price_converter(cnt,
                            (jp_limits_checker *) get_balance_struct(gk),
                            doj, cnt->sec_type);
                }
                int from_a_rom = 1;
                if(doj->positions[ROM_NAME].iov_len > 0 &&
                        doj->positions[ROM_OWNER].iov_len > 0) {
                    if(mod_len == doj->positions[ROM_NAME].iov_len &&
                            strncmp(mod_name,
                                ((char*)doj->positions[ROM_NAME].iov_base), mod_len) == 0) {
                        set_rom_field(doj, ROM_SENDER, doj->positions[ROM_OWNER].iov_base,
                                doj->positions[ROM_OWNER].iov_len);
                        from_a_rom = 0;
                    } else {
                        set_rom_field(doj, ROM_SENDER, doj->positions[ROM_NAME].iov_base,
                                doj->positions[ROM_NAME].iov_len);
                    }
                }
                set_order_in_interface(jc, gk, cnt,from_a_rom);
                add_to_init_orders(gk, doj->positions[ROM_DEST].iov_base,
                        doj->positions[ROM_DEST].iov_len, cnt);

                int num_len = 0;
                char *mod_num = (char *) get_mod_name(gk, &num_len);
                if(getplen(doj, ROM_DEST) == num_len &&
                        strncmp(getpval(doj, ROM_DEST), mod_num, num_len) == 0 &&
                        strncmp(getpval(doj, ROM_DESTID), mod_num, num_len) != 0) {
                    add_to_init_orders(gk, doj->positions[ROM_DESTID].iov_base,
                            doj->positions[ROM_DESTID].iov_len, cnt);
                }
            }
            free_element(q, erow);
            destroy_queue(row);
        }
    } else {
        send_debug_message(jc->o, "Get orders FAILED!! The error is: %s \n", err);
    }
    destroy_queue(q);
    return 0;
}


int load_orders(jp_limits_checker * jc, gatekeeper * gk,
        const char *destinations)
{
    int mod_len = 0;
    int num_len = 0;
    char *mod_num = (char *) get_mod_name(gk, &num_len);
    char* mod_name = load_rom_name(mod_num, num_len, &mod_len, jc);
    load_destinations(jc, gk);
    load_futures_dests(jc, gk);
    databuf_t* seq_str = databuf_alloc(2048);
    databuf_t* select_str = databuf_alloc(2048);
    databuf_write(select_str, "SELECT ");
    for(int i = 0; i < ORDER_COLUMN_NUM; ++i) {
        if(order_cln[i].field_id == ROM_STATUS_CNT) {
            databuf_write(select_str, "B.%s, ", order_cln[i].column_name);
        } else if (i == ORDER_COLUMN_NUM - 1) {
            databuf_write(select_str, "A.%s ", order_cln[i].column_name);
        } else {
            databuf_write(select_str, "A.%s, ", order_cln[i].column_name);
        }
    }
    databuf_memcpy(seq_str, select_str->rd_ptr, databuf_unread(select_str));
    databuf_write(seq_str, "FROM orders A, "
            "(SELECT omTag, MAX(status_count) as status_count "
            "FROM orders GROUP BY omTag) B "
            "WHERE B.omTag=A.omTag AND B.status_count = A.status_count "
            "AND (A.cplxtype is null OR A.cplxtype = 3 OR A.omparenttag is not null) "
            "AND (A.omname='%s' OR (destrouteid IN(%s) AND A.omname <> '%s')) "
            "AND A.status NOT IN (4,8,2,12)", mod_name,destinations, mod_name);
    get_orders(jc, gk, seq_str, mod_name, mod_len);
    databuf_reset(seq_str);
    databuf_memcpy(seq_str, select_str->rd_ptr, databuf_unread(select_str));
    databuf_write(seq_str, "FROM orders A, "
            "(SELECT omTag, MAX(status_count) as status_count "
            "FROM orders GROUP BY omTag) B, "
            "(SELECT omTag, cplxtype, Max(status_count) as status_count "
            "FROM orders WHERE cplxtype = 5 GROUP BY cplxtype, omTag) C "
            "WHERE B.omTag = A.omTag AND B.omTag = C.omTag AND A.status_count = C.status_count "
            "AND (A.omname='%s' OR (destrouteid IN(%s) AND A.omname <> '%s')) "
            "AND A.status NOT in (4, 8, 2, 12) AND A.cplxtype is NOT null", mod_name,
            destinations, mod_name);
    get_orders(jc, gk, seq_str, mod_name, mod_len);
    return 0;
}

