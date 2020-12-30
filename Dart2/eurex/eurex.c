/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/time.h>

#include "ETILayoutsNS_Derivatives.h"
#include "message_queue.h"
#include "rex_allocator.h"
#include "parser_func.h"
#include "rex_constants.h"
#include "rex_seq_request.h"
#include "config_loader.h"
#include "ex_config_helper.h"
#include "dart_handler.h"
#include "mailer.h"
#include "connection_interface.h"
#include "connection_factory.h"
#include "log_mnger.h"
#include "dart_order_fact.h"
#include "rom_fix_trans_funcs.h"
#include "eurex.h"
#include "oats.h"
#include "balance_checker.h"

typedef struct eurex_mifid_helper {
    hashmap_t acc_to_pcid;
    hashmap_t acc_to_pdecider;
    hashmap_t acc_to_executer;
}emifid;

typedef struct eurex_parser {
    otl* logger;
    login_info* logon;
    con_int* pbp;
    rex_con* rc;
    databuf_t* mess_to_send;
    pthread_spinlock_t send_mutex;
    call_back_wrapper* recon_wrap;
    call_back_wrapper* hb_timer;
    hashmap_t prod_codes;
    hashmap_t users;
    hashmap_t user_pass;
    hashmap_t acctypes;
    uint32_t seq_num;
    uint64_t sec_sessionid;
    int is_logged_on;
    time_t last_sent;
    uint64_t clordid;
    uint32_t smp_id;
    emifid* mifid_vals;
}eurexp;

static void eurex_cancel_all_orders(con_int* ci);
uint32_t get_eurex_secuity_id(int field, dart_order_obj* doj)
{
    char cp_cat[32];
    uint64_t ret = 0;
    memset(&cp_cat, '\0', 32);
    memcpy(&cp_cat, doj->positions[field].iov_base,
            doj->positions[field].iov_len);
    ret = atol((const char*)&cp_cat);
    uint32_t r2 =  ret & 0xffffffff;
    return r2;
}
uint64_t get_eurex_multi_leg_sec_id(int field, dart_order_obj* doj)
{
    char cp_cat[32];
    uint64_t ret = 0;
    memset(&cp_cat, '\0', 32);
    memcpy(&cp_cat, doj->positions[field].iov_base,
            doj->positions[field].iov_len);
    ret = atol((const char*)&cp_cat);
    return ret;
}
int eur_to_rom_time(uint64_t eur_time, databuf_t* buff)
{
    struct tm t;
    time_t epoch_time = eur_time/1000000000;
    long nanos = eur_time % epoch_time;
    nanos /= 1000; //We are only going to display micros.
    gmtime_r(&epoch_time, &t);
    return databuf_write(buff, "%04d%02d%02d-%02d:%02d:%02d.%d",
            (t.tm_year + 1900), (t.tm_mon + 1), t.tm_mday,
            t.tm_hour, t.tm_min, t.tm_sec, nanos);
}

char* eur_translate_open_close(char val)
{
    switch(val) {
        case '1':
            return "O";
        case 'O':
            return "1";
        case '0':
            return "C";
        case 'C':
            return "0";
        default:
            return "0";
    }
}
int64_t rom_to_eur_price(char *price, size_t len)
{
    if(len > 0) {
        double d = strtod(price, NULL);
	if(d > 0) {
	d += .000000001;
	} else {
	d -= .000000001;
	}
        d = d * 100000000;
        return (int64_t) d;
    } else {
        return NO_VALUE_SLONG; 
    }
}

int eur_to_rom_price(int64_t price, databuf_t* val)
{
    double dprice = (double) price;
    dprice = (dprice/100000000.0);
    if(dprice > 0) {
        dprice += .000000001;
    } else {
        dprice -= .000000001;
    }
    return databuf_write(val, "%f", dprice);
}
#define EMICRO_HOUR 3600000000
#define EMICRO_MIN 60000000
#define EMICRO_SEC 1000000
uint64_t eur_create_time_offset()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_usec + (tv.tv_sec * EMICRO_SEC));
}

static char* eur_get_file_contents(const char* filename, long* size)
{
    FILE* mfile = 0;
    long res = 0;
    char* buff = 0;
    int ret = 0;
    mfile = fopen(filename, "r");
    if(mfile != 0) {
        fseek(mfile, 0, SEEK_END);
        *size = ftell(mfile);
        rewind(mfile);
        REX_ALLOC(buff, 8, *size + 1, ret);
        res = fread(buff, 1, *size, mfile);
        if(res != *size) {
            free(buff);
            buff = 0;
        }
        fclose(mfile);
    } 
    return buff;
}

static void str_eur_build_map_from_file(const char* filename, hashmap_t map)
{
    char* buffer = 0;
    long size = 0;
    buffer = eur_get_file_contents(filename, &size);
    if(size > 0) {
        char* start = buffer;
        char* comma = buffer;
        char* end = buffer;
        int tag_len = 0;
        char* val = buffer;
        do {
            if(*end == ',') {
                tag_len = end - start;
                comma = end;
            } else if(*end == '\n') {
                val = (comma + 1);
                insert_map(map, start, tag_len, val, end - val);
                start = end + 1;
            }
            ++end;
            --size;
        } while(size > 0);
    }
}
static void eur_build_map_from_file(const char* filename, hashmap_t map)
{
    char* buffer = 0;
    long size = 0;
    buffer = eur_get_file_contents(filename, &size);
    if(size > 0) {
        char* start = buffer;
        char* comma = buffer;
        char* end = buffer;
        int tag_len = 0;
        int val = 0;
        do {
            if(*end == ',') {
                tag_len = end - start;
                comma = end;
            } else if(*end == '\n') {
                val = atoi(comma + 1);
                insert_map(map, start, tag_len, &val, sizeof(int));
                start = end + 1;
            }
            ++end;
            --size;
        } while(size > 0);
    }
}
static eurexp* create_eurex_parser(otl* logger, con_int* pbp, login_info* u)
{
    eurexp* fp = 0;
    int ret = 0;
    REX_ALLOC(fp, 8, sizeof(struct eurex_parser), ret);
    fp->logger = logger;
    fp->pbp = pbp;
    fp->logon = u;
    fp->mess_to_send = databuf_alloc(1024);
    fp->sec_sessionid = atoi(pbp->ex->username);
    pthread_spin_init(&fp->send_mutex, PTHREAD_PROCESS_PRIVATE);
    fp->is_logged_on = 0;
    pbp->cparser = fp;
    fp->clordid = eur_create_time_offset();
    ret = 0;
    fp->prod_codes = create_map(2048);
    char* filename = get_val_for_tag(pbp->ex->dc, "SERVER", 6, "PCODE LOC", 9,
            &ret);
    if(ret > 0) {
        eur_build_map_from_file(filename, fp->prod_codes);
    }
    fp->users = create_map(32);
    fp->user_pass = create_map(32);
    fp->acctypes = create_map(32);
    eur_build_map_from_file("ClearingIDToUser.map", fp->users);
    eur_build_map_from_file("UserIDToPass.map", fp->user_pass);
    str_eur_build_map_from_file("ClearingIDToAccType.map", fp->acctypes);
    REX_ALLOC(fp->mifid_vals, 8, sizeof(emifid), ret);
    fp->mifid_vals->acc_to_pcid = create_map(16);
    fp->mifid_vals->acc_to_pdecider = create_map(16);
    fp->mifid_vals->acc_to_executer= create_map(16);
    eur_build_map_from_file("PartyIDClientIDs.map", fp->mifid_vals->acc_to_pcid);
    eur_build_map_from_file("PartyIDInvestmentDecisionMakers.map", fp->mifid_vals->acc_to_pdecider);
    eur_build_map_from_file("ExecutingTraders.map", fp->mifid_vals->acc_to_executer);
    char* tempsmp = get_val_for_tag(pbp->ex->dc, "SERVER", 6, "SMPID", 5, &ret);
    if(ret > 0) {
        fp->smp_id = atoi(tempsmp);
    } else {
        fp->smp_id = 13638;
    }
    return fp;
}

static int get_user_id(eurexp* ep, char* clr_id, int clr_id_len)
{
    uint32_t cid = NO_VALUE_UINT;
    uint32_t* finder = 0;
    int ret = find_n_fill(ep->users, clr_id, clr_id_len,
            (void **)&finder);
    if(ret > 0) {
        cid = *finder;
    }
    return cid;
}
static int get_party_client_id(eurexp* ep, char* clearing_acc, int calen)
{
    uint32_t cid = NO_VALUE_UINT;
    uint32_t* finder = 0;
    int ret = find_n_fill(ep->mifid_vals->acc_to_pcid, clearing_acc, calen,
            (void **)&finder);
    if(ret == sizeof(uint32_t)) {
        cid = *finder;
    }
    return cid;
}
static void get_acc_type(eurexp* ep, char* clearing_acc, int clen,
        databuf_t* db)
{
    char* finder = 0x0;
    int ret = find_n_fill(ep->acctypes, clearing_acc, clen, (void**)&finder);
    if(ret == 2) {
        databuf_memcpy(db, finder, ret);
    } else {
        databuf_memcpy(db, clearing_acc, 2);
    }
}
static int get_party_investment_dm(eurexp* ep, char* clearing_acc, int calen)
{
    uint32_t pidm = NO_VALUE_UINT;
    uint32_t* finder = 0;
    int ret = find_n_fill(ep->mifid_vals->acc_to_pdecider, clearing_acc, calen,
            (void **)&finder);
    if(ret == sizeof(uint32_t)) {
        pidm = *finder;
    }
    return pidm;
}
static int get_executing_trader(eurexp* ep, char* trader_acc, int calen)
{
    uint32_t et = NO_VALUE_UINT;
    uint32_t* finder = 0;
    int ret = find_n_fill(ep->mifid_vals->acc_to_executer, trader_acc, calen,
            (void **)&finder);
    if(ret == sizeof(uint32_t)) {
        et = *finder;
    }
    return et;
}
static int get_prod_code(eurexp* ep, dart_order_obj* doj, databuf_t* db)
{
    int len = doj->positions[ROM_SEC_DEF].iov_len;
    char* sd = doj->positions[ROM_SEC_DEF].iov_base;
    int32_t* field = 0;
    int ret = find_n_fill(ep->prod_codes, sd, len,
            (void **)&field);
    if(ret > 0) {
        dart_signed_itoa(doj, *field, ROM_PROD_CODE, db);
        return *field;
    } else {
        return NO_VALUE_SINT;
    }
}
static void eur_direct_send(eurexp* up, void* data, int len)
{
    int bytes_sent = 0;
    if(pthread_spin_lock(&up->send_mutex) == 0) {
        time(&up->last_sent);
        int disconnect = 0;
        bytes_sent = gen_send(up->rc, data, len,
                &disconnect);
        if(bytes_sent < len && !disconnect) {
            databuf_memcpy(up->mess_to_send, data + bytes_sent,
                    (len - bytes_sent));
            add_sender(up->rc->p, up->rc);
        } else if(disconnect) {
            up->rc->disconnect = disconnect;
            up->is_logged_on = 0;
        }
        pthread_spin_unlock(&up->send_mutex);
    } 
}
static void send_eur_logout(eurexp* ep, int user_id)
{
    UserLogoutRequestT urt;
    urt.MessageHeaderIn.BodyLen = sizeof(urt);
    urt.MessageHeaderIn.TemplateID = TID_USER_LOGOUT_REQUEST;
    urt.Username = user_id;//ep->user_logon_id;
    if(ep->rc) {
        ep->seq_num++;
        urt.RequestHeader.MsgSeqNum = ep->seq_num;
        eur_direct_send(ep, &urt, sizeof(urt));
        LogoutRequestT lrt;
        lrt.MessageHeaderIn.BodyLen = sizeof(LogoutRequestT);
        lrt.MessageHeaderIn.TemplateID = TID_LOGOUT_REQUEST;                              
        ep->seq_num++;
        lrt.RequestHeader.MsgSeqNum = ep->seq_num;
        eur_direct_send(ep, &lrt, sizeof(lrt));
    }
}

static void iterate_logouts(eurexp* ep)
{
    uint32_t* rlf;
    char *key = 0;
    int key_len = 0;
    hashmap_iterator_t hi = hashmap_iterator_begin(ep->users);
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                                     (void *) &rlf);
    while (valid) {
        uint32_t userid = *rlf;
        send_eur_logout(ep, userid);
        valid = hashmap_iterator_next(ep->users, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &rlf);
    }
    hashmap_iterator_destroy(hi);
}
static int eurex_sender(eurexp* up, databuf_t* buff)
{
    int bytes_sent = 0;
    int disconnect = 0;
    int len = databuf_unread(buff);
    bytes_sent = gen_send(up->rc, buff->rd_ptr, len,
            &disconnect);
    if(bytes_sent < len && !disconnect) {
        databuf_memcpy(up->mess_to_send, buff->rd_ptr + bytes_sent,
                (len - bytes_sent));
        add_sender(up->rc->p, up->rc);
    } else if(disconnect) {
        up->rc->disconnect = disconnect;
        up->is_logged_on = 0;
    }
    return bytes_sent;
}
void eurex_write_callback(void* cb)
{
    rex_con* rc = (rex_con*)cb;
    eurexp* up = (eurexp*)rc->parser;
    pthread_spin_lock(&up->send_mutex);
    int blen = databuf_unread(up->mess_to_send);
    int bytes_sent = eurex_sender(up, up->mess_to_send);
    up->mess_to_send->rd_ptr += bytes_sent;
    pthread_spin_unlock(&up->send_mutex);
    if(bytes_sent < blen && up->rc->disconnect <= 0) {
        add_sender(rc->p, rc);
    } 
}
void send_eurex_session_logon(void* dd)//(eurexp* ep, con_int* ci, rex_con* rc)
{
    eurexp* ep = (eurexp*)dd;
    con_int *ci = (con_int *) ep->pbp;
    LogonRequestT lrt;
    memset(&lrt, '\0', sizeof(LogonRequestT));
    lrt.MessageHeaderIn.BodyLen = sizeof(LogonRequestT);
    lrt.MessageHeaderIn.TemplateID = TID_LOGON_REQUEST;
    lrt.RequestHeader.MsgSeqNum = 1;
    ep->seq_num = 1;
    lrt.HeartBtInt = 30000;
    lrt.PartyIDSessionID = ep->sec_sessionid;
    memcpy(&lrt.DefaultCstmApplVerID, ETI_INTERFACE_VERSION , 3);
    memcpy(&lrt.Password, ci->ex->password, ci->ex->plen);
    memcpy(&lrt.ApplUsageOrders, "B", 1);
    memcpy(&lrt.ApplUsageQuotes, "N", 1);
    memcpy(&lrt.OrderRoutingIndicator, "Y",1);
    memcpy(&lrt.FIXEngineName, "DART EURO GATEWAY", 17);
    memcpy(&lrt.FIXEngineVersion, "v4.4.5", 6);
    memcpy(&lrt.FIXEngineVendor, "DART Executions LLC",19);
    memcpy(&lrt.ApplicationSystemName, "DART OMS", 8);
    memcpy(&lrt.ApplicationSystemVersion, "v4.5.6.7", 8);
    memcpy(&lrt.ApplicationSystemVendor,"DART Executions LLC",19); 
    eur_direct_send(ep, &lrt, sizeof(LogonRequestT));
}
void eurex_cleansing_rinse(con_int* ci)
{
}

static int32_t get_int_for_rom(int field, dart_order_obj* doj)
{
    char cp_cat[32];
    int32_t ret = 0;
    memset(&cp_cat, '\0', 32);
    memcpy(&cp_cat, doj->positions[field].iov_base,
            doj->positions[field].iov_len);
    ret = atoi((const char*)&cp_cat);
    return ret;
}
static int64_t get_qty_for_rom(int field, dart_order_obj* doj)
{
    char cp_cat[32];
    int64_t ret = 0;
    memset(&cp_cat, '\0', 32);
    memcpy(&cp_cat, doj->positions[field].iov_base,
            doj->positions[field].iov_len);
    ret = atol((const char*)&cp_cat);
    return ret * 10000;/*As of version 7.0 DBAG changed qty to imply 4 decimal places*/
}
static uint32_t get_uint_for_rom(int field, dart_order_obj* doj)
{
    char cp_cat[32];
    uint32_t ret = 0;
    memset(&cp_cat, '\0', 32);
    memcpy(&cp_cat, doj->positions[field].iov_base,
            doj->positions[field].iov_len);
    ret = atoi((const char*)&cp_cat);
    return ret;
}
static uint8_t get_8bit_for_rom(int field, dart_order_obj* doj)
{
    char cp_cat[32];
    uint8_t ret = 0;
    memset(&cp_cat, '\0', 32);
    memcpy(&cp_cat, doj->positions[field].iov_base,
            doj->positions[field].iov_len);
    ret = atoi((const char*)&cp_cat);
    return ret;
}

static uint8_t get_eur_type_for_rom(dart_order_obj* doj)
{
    if(doj->positions[ROM_TYPE].iov_len == 2) {
        char* t = doj->positions[ROM_TYPE].iov_base;
        if(t[1] == '1') {
            return 2;
        } else {
            return 1;
        }
    } else {
        return get_8bit_for_rom(ROM_TYPE, doj);
    }
}
/**
 * Eurex now pads all qty with four "decimal" places.
 */
static void trans_eur_to_rom_qty(dart_order_obj* doj, 
        int rom_field,int64_t qty,  databuf_t* db)
{
    qty /= 10000;
    dart_itoa(doj, qty, rom_field, db);

}

static void eurex_destroy_cmplx_ord(rcntr* cnt, con_int* eu)
{
    dart_order_obj* doj = (dart_order_obj*)cnt->raw;
    con_int* ci = 0x0;
    if(doj->sender != 0x0) {
        ci = ((con_int*)doj->sender);
    } else {
        get_obj_by_client_id(eu->gk, cnt->owner,cnt->olen, (void**)&ci);        
        if(!ci) {
            return;
        }
    }
    int check_tag = ci->logon->do_balance ? ROM_CLIENT_ID : ROM_TAG;
    pthread_spin_lock(&ci->mutex);
    remove_from_map(ci->cl_ord_id_map, doj->positions[check_tag].iov_base,
            doj->positions[check_tag].iov_len);
    pthread_spin_unlock(&ci->mutex);
    dp_return_doj((dartp*)ci->cparser, doj);
    return_cnt(ci->gk, cnt);
}
int create_multi_leg_order(con_int* ci, rcntr* risk, 
        eurexp* ep, dart_order_obj* doj, uint64_t clordid, int32_t marketid)
{
    int ret = 1;
    NewOrderComplexRequestT nocrt;
    memset(&nocrt, '\0', sizeof(nocrt));
    uint32_t bl = sizeof(NewOrderComplexRequestT) - (18 * sizeof(LegOrdGrpSeqT));
    nocrt.MessageHeaderIn.BodyLen = bl;
    nocrt.MessageHeaderIn.TemplateID = TID_NEW_ORDER_COMPLEX_REQUEST;
    nocrt.RequestHeader.SenderSubID = get_user_id(ep,
            doj->positions[ROM_CLR_ACC].iov_base,
            doj->positions[ROM_CLR_ACC].iov_len);//ep->user_logon_id;
    nocrt.Price = rom_to_eur_price(doj->positions[ROM_PRICE].iov_base,
            doj->positions[ROM_PRICE].iov_len); 
    nocrt.ClOrdID = clordid;
    if(doj->positions[ROM_EXPIRE_TIME].iov_len > 0) {
        char exp_str[9];
        memset(&exp_str, '\0', 9);
        memcpy(&exp_str, doj->positions[ROM_EXPIRE_TIME].iov_base, 8);
        nocrt.ExpireDate = atoi((const char*)&exp_str);
    } else {
        nocrt.ExpireDate =NO_VALUE_UINT;
    }
    nocrt.PartyIDClientID = get_party_client_id(ep,
            doj->positions[ROM_CLR_ACC].iov_base,
            doj->positions[ROM_CLR_ACC].iov_len);
    nocrt.PartyIdInvestmentDecisionMaker = get_party_investment_dm(ep,
            doj->positions[ROM_CLR_ACC].iov_base,
            doj->positions[ROM_CLR_ACC].iov_len);
    nocrt.ExecutingTrader = get_executing_trader(ep,
            doj->positions[ROM_TRADER].iov_base,
            doj->positions[ROM_TRADER].iov_len);
    if(getplen(doj, ROM_MANUAL) > 0) {
        char* v = getpval(doj, ROM_MANUAL);
        if(v[0] == 'N') {
            nocrt.PartyIdInvestmentDecisionMakerQualifier = ENUM_EXECUTINGTRADERQUALIFIER_HUMAN;
            nocrt.ExecutingTraderQualifier = ENUM_EXECUTINGTRADERQUALIFIER_HUMAN;
        } else {
            nocrt.PartyIdInvestmentDecisionMakerQualifier = ENUM_EXECUTINGTRADERQUALIFIER_ALGO;
            nocrt.ExecutingTraderQualifier = ENUM_EXECUTINGTRADERQUALIFIER_ALGO;
        }
    } else {
        nocrt.PartyIdInvestmentDecisionMakerQualifier = ENUM_EXECUTINGTRADERQUALIFIER_HUMAN;
        nocrt.ExecutingTraderQualifier = ENUM_EXECUTINGTRADERQUALIFIER_HUMAN;
    }
    nocrt.OrderQty = get_qty_for_rom(ROM_SHARES, doj);
    nocrt.MarketSegmentID =marketid;//get_int_for_rom(ROM_PROD_CODE, doj);
    nocrt.SecurityID = get_eurex_multi_leg_sec_id(ROM_SEC_DEF, doj);
    if(doj->positions[ROM_DESC].iov_len > 0) {
        nocrt.MatchInstCrossID = get_uint_for_rom(ROM_DESC, doj);
    } else {
        nocrt.MatchInstCrossID = ep->smp_id;//NO_VALUE_UINT;
    }
    nocrt.ApplSeqIndicator = 1;
    nocrt.Side = get_8bit_for_rom(ROM_SIDE, doj);
    nocrt.PriceValidityCheckType = 1;
    nocrt.ProductComplex = get_8bit_for_rom(ROM_CPLX_TYPE, doj); 
    nocrt.TimeInForce = get_8bit_for_rom(ROM_TIF, doj);
    if(doj->positions[ROM_EXEC_INST].iov_len > 0) {
        nocrt.ExecInst = get_8bit_for_rom(ROM_EXEC_INST, doj);
    } else {
        nocrt.ExecInst = 2;
    }
    nocrt.OrdType = get_8bit_for_rom(ROM_TYPE, doj);
    if(doj->positions[ROM_CAP].iov_len > 0) {
        char* c = doj->positions[ROM_CAP].iov_base;
        if(c[0] == 'S') {
            nocrt.TradingCapacity = 6;
            nocrt.OrderOrigination = NO_VALUE_UCHAR;
        } else {
            nocrt.TradingCapacity = 5;
            nocrt.OrderOrigination = NO_VALUE_UCHAR;
        }
    } else {
        memcpy(risk->errStr, "Missing Capacity", 16);
        risk->err_len = 16;
        ret = -1;
        goto EXIT_ENTER_CPLX_ORDER;
    }
    nocrt.NoLegs = 2;
    if(doj->positions[ROM_CLR_ACC].iov_len > 0) {
        databuf_t* db = get_databuf(ci->gk);
        get_acc_type(ep, doj->positions[ROM_CLR_ACC].iov_base,
                doj->positions[ROM_CLR_ACC].iov_len, db);
        memcpy(&nocrt.LegOrdGrp[0].LegAccount, db->rd_ptr, 2);
        memcpy(&nocrt.LegOrdGrp[1].LegAccount, db->rd_ptr, 2);
        destroy_gk_db(ci->gk, db);
        int cp_len =  doj->positions[ROM_CLR_ACC].iov_len < LEN_FREE_TEXT1 ? 
            doj->positions[ROM_CLR_ACC].iov_len : LEN_FREE_TEXT1 -1;
        memset(&nocrt.FreeText1, ' ', LEN_FREE_TEXT1);
        memcpy(&nocrt.FreeText1, doj->positions[ROM_CLR_ACC].iov_base,
                    cp_len);
    }
    int oclen = doj->positions[ROM_O_C].iov_len;
    if(oclen <= 0) {
        ret = -1;
        memcpy(risk->errStr, "Missing Pos eff", 15);
        risk->err_len = 15;
        goto EXIT_ENTER_CPLX_ORDER;
    } 
    int i = 0;
    for(;i < oclen; ++i) {
        memcpy(&nocrt.LegOrdGrp[i].LegPositionEffect, eur_translate_open_close(
                    (((char*)doj->positions[ROM_O_C].iov_base)[i])), 1);
    }

    pthread_spin_lock(&ep->send_mutex);
    nocrt.RequestHeader.MsgSeqNum = ++ep->seq_num;
    databuf_t* db = get_databuf(ci->gk);
    unsigned_dart_itoa(doj, ep->seq_num, ROM_LEG_MARK,db);
    destroy_gk_db(ci->gk, db);
    pthread_spin_unlock(&ep->send_mutex);
    eur_direct_send(ep, &nocrt, bl);
EXIT_ENTER_CPLX_ORDER:
    return ret;
}
/**
 * Since this is not a DMA connection the raw parameter
 * will be a dart_order_obj rather than a raw unparsed
 * message.
 */
int on_eurex_enter_order(con_int * ci, rcntr * risk)
{
    int ret = 0;
    int32_t marketid = NO_VALUE_SINT;                                    
    databuf_t* db = get_databuf(ci->gk);
    eurexp* ep = (eurexp*)ci->cparser;
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    if(doj->positions[ROM_SEC_DEF].iov_len <= 0) {
        ret = -1;
        memcpy(risk->errStr, "Missing sec def", 15);
        risk->err_len = 15;
        goto EXIT_ENTER_ORDER;
    }
    if(doj->positions[ROM_PROD_CODE].iov_len <= 0) {
        marketid = get_prod_code(ep, doj, db);
        if(ret == NO_VALUE_SINT) {
            memcpy(risk->errStr, "No prod for ID", 14);
            goto EXIT_ENTER_ORDER;
        }
    } else {
        marketid = get_int_for_rom(ROM_PROD_CODE, doj);
    }
    pthread_spin_lock(&ci->mutex);
    /*START LOCK*/
    uint64_t clordid = ++ep->clordid; 
    u64_dart_itoa(doj, clordid, ROM_CLORDID,db);
    char* str_clordid = doj->positions[ROM_CLORDID].iov_base;
    int cl_len = doj->positions[ROM_CLORDID].iov_len;
    ret = no_copy_insert(ci->cl_ord_id_map, str_clordid, cl_len, risk);
    /*END LOCK*/
    pthread_spin_unlock(&ci->mutex);
    if(ret) {
        if(doj->positions[ROM_CPLX_TYPE].iov_len > 0 &&
                ((char*)doj->positions[ROM_CPLX_TYPE].iov_base)[0] != '0') {
            ret = create_multi_leg_order(ci, risk, ep, doj, clordid, marketid);
            destroy_gk_db(ci->gk, db);
            goto EXIT_ENTER_ORDER;
        }
        NewOrderSingleRequestT nosrt;
        memset(&nosrt, '\0', sizeof(NewOrderSingleRequestT));
        nosrt.MessageHeaderIn.BodyLen = sizeof(NewOrderSingleRequestT);
        nosrt.MessageHeaderIn.TemplateID = TID_NEW_ORDER_SINGLE_REQUEST;
        nosrt.RequestHeader.SenderSubID = get_user_id(ep,
            doj->positions[ROM_CLR_ACC].iov_base,
            doj->positions[ROM_CLR_ACC].iov_len);//ep->user_logon_id;
        nosrt.Price = rom_to_eur_price(doj->positions[ROM_PRICE].iov_base,
                doj->positions[ROM_PRICE].iov_len); 
        if(doj->positions[ROM_STOP].iov_len > 0) {
            nosrt.StopPx = rom_to_eur_price(doj->positions[ROM_STOP].iov_base,
                    doj->positions[ROM_STOP].iov_len);
        } else {
            nosrt.StopPx = NO_VALUE_SLONG;                                   
        }
        nosrt.ClOrdID = clordid;
        nosrt.OrderOrigination = 5;
        nosrt.PartyIDClientID = get_party_client_id(ep,
            doj->positions[ROM_CLR_ACC].iov_base,
            doj->positions[ROM_CLR_ACC].iov_len);
        nosrt.PartyIdInvestmentDecisionMaker = get_party_investment_dm(ep,
            doj->positions[ROM_CLR_ACC].iov_base,
            doj->positions[ROM_CLR_ACC].iov_len);
        nosrt.ExecutingTrader = get_executing_trader(ep,
            doj->positions[ROM_TRADER].iov_base,
            doj->positions[ROM_TRADER].iov_len);
        if(getplen(doj, ROM_MANUAL) > 0) {
            char* v = getpval(doj, ROM_MANUAL);
            if(v[0] == 'N') {
                nosrt.PartyIdInvestmentDecisionMakerQualifier = ENUM_EXECUTINGTRADERQUALIFIER_HUMAN;
                nosrt.ExecutingTraderQualifier = ENUM_EXECUTINGTRADERQUALIFIER_HUMAN;
            } else {
                nosrt.PartyIdInvestmentDecisionMakerQualifier = ENUM_EXECUTINGTRADERQUALIFIER_ALGO;
                nosrt.ExecutingTraderQualifier = ENUM_EXECUTINGTRADERQUALIFIER_ALGO;
            }
        } else {
            nosrt.PartyIdInvestmentDecisionMakerQualifier = ENUM_EXECUTINGTRADERQUALIFIER_HUMAN;
            nosrt.ExecutingTraderQualifier = ENUM_EXECUTINGTRADERQUALIFIER_HUMAN;
        }
        if(doj->positions[ROM_EXPIRE_TIME].iov_len > 0) {
            char exp_str[9];
            memset(&exp_str, '\0', 9);
            memcpy(&exp_str, doj->positions[ROM_EXPIRE_TIME].iov_base, 8);
            nosrt.ExpireDate = atoi((const char*)&exp_str);
        } else {
            nosrt.ExpireDate =NO_VALUE_UINT;
        }
        nosrt.OrderQty = get_qty_for_rom(ROM_SHARES, doj);
        nosrt.SimpleSecurityID = get_eurex_secuity_id(ROM_SEC_DEF, doj);
        nosrt.MarketSegmentID = marketid;
        if(doj->positions[ROM_DESC].iov_len > 0) {
            nosrt.MatchInstCrossID = get_uint_for_rom(ROM_DESC, doj);
        } else {
            nosrt.MatchInstCrossID = ep->smp_id;//NO_VALUE_UINT;
        }
        nosrt.ApplSeqIndicator = 1;
        nosrt.Side = get_8bit_for_rom(ROM_SIDE, doj);
        nosrt.PriceValidityCheckType = 1;
        nosrt.TimeInForce = get_8bit_for_rom(ROM_TIF, doj);
        if(doj->positions[ROM_EXEC_INST].iov_len > 0) {
            nosrt.ExecInst = get_8bit_for_rom(ROM_EXEC_INST, doj);
        } else {
            nosrt.ExecInst = 2;
        }
        if(doj->positions[ROM_TYPE].iov_len >= 1) {
            nosrt.OrdType = get_eur_type_for_rom(doj);
        } else {
            memcpy(risk->errStr, "Missing type", 12);
            risk->err_len = 12;
            destroy_gk_db(ci->gk, db);
            ret = -1;
            goto EXIT_ENTER_ORDER;
        }
        if(doj->positions[ROM_TYPE].iov_len ==2) {
            nosrt.TradingSessionSubID = 4;
        } else {
            nosrt.TradingSessionSubID =NO_VALUE_UCHAR;
        }
        if(doj->positions[ROM_CAP].iov_len > 0) {
            char* c = doj->positions[ROM_CAP].iov_base;
            if(c[0] == 'S') {
                nosrt.TradingCapacity = 6;
                nosrt.OrderOrigination = NO_VALUE_UCHAR;
            } else {
                nosrt.TradingCapacity = 5;
                nosrt.OrderOrigination = NO_VALUE_UCHAR;
            }
        } else {
            memcpy(risk->errStr, "Missing Capacity", 16);
            risk->err_len = 16;
            ret = -1;
            destroy_gk_db(ci->gk, db);
            goto EXIT_ENTER_ORDER;
        }
        if(doj->positions[ROM_CLR_ACC].iov_len > 0) {
            databuf_t* db = get_databuf(ci->gk);
            get_acc_type(ep, doj->positions[ROM_CLR_ACC].iov_base,
                doj->positions[ROM_CLR_ACC].iov_len, db);
            memcpy(&nosrt.Account,db->rd_ptr, 2);
            destroy_gk_db(ci->gk, db);
            int cp_len =  doj->positions[ROM_CLR_ACC].iov_len < LEN_FREE_TEXT1 ? 
                doj->positions[ROM_CLR_ACC].iov_len : LEN_FREE_TEXT1 - 1;
            memset(&nosrt.FreeText1, ' ', LEN_FREE_TEXT1);
            memcpy(&nosrt.FreeText1, doj->positions[ROM_CLR_ACC].iov_base,
                    cp_len);
        }
        memcpy(&nosrt.PartyIDLocationID, "GB", 2);
        if(doj->positions[ROM_O_C].iov_len <= 0) {
            memcpy(&nosrt.PositionEffect, "O", 1);
            set_rom_field(doj, ROM_O_C, "1", 1);
        } else {
            memcpy(&nosrt.PositionEffect, eur_translate_open_close(
                        (((char*)doj->positions[ROM_O_C].iov_base)[0])), 1);
        }
        pthread_spin_lock(&ep->send_mutex);
        nosrt.RequestHeader.MsgSeqNum = ++ep->seq_num;
        unsigned_dart_itoa(doj, ep->seq_num, ROM_LEG_MARK,  db);
        pthread_spin_unlock(&ep->send_mutex);
        eur_direct_send(ep, &nosrt, sizeof(NewOrderSingleRequestT));
    }
    destroy_gk_db(ci->gk, db);
EXIT_ENTER_ORDER:
    return ret;
}
static int replace_multi_leg_order(con_int* ci, rcntr* risk, eurexp* ep, 
        dart_order_obj* doj, char* clordid)
{
    int ret = 1;
    ModifyOrderComplexRequestT mocrt;
    memset(&mocrt, '\0', sizeof(ModifyOrderComplexRequestT));
    mocrt.OrderID = NO_VALUE_ULONG;
    mocrt.ClOrdID = NO_VALUE_ULONG;
    mocrt.OrigClOrdID = atol(clordid);
    dart_order_obj* rep = (dart_order_obj*)doj->rep;
    databuf_t* db = get_databuf(ci->gk);
    uint32_t bl = sizeof(ModifyOrderComplexRequestT) - (18 * sizeof(LegOrdGrpSeqT));
    mocrt.MessageHeaderIn.BodyLen = bl;
    mocrt.MessageHeaderIn.TemplateID = TID_MODIFY_ORDER_COMPLEX_REQUEST; 
    mocrt.RequestHeader.SenderSubID = get_user_id(ep,
            doj->positions[ROM_CLR_ACC].iov_base,
            doj->positions[ROM_CLR_ACC].iov_len);//ep->user_logon_id;
    mocrt.OrderOrigination = 5;
    mocrt.PartyIDClientID = get_party_client_id(ep,
        doj->positions[ROM_CLR_ACC].iov_base,
        doj->positions[ROM_CLR_ACC].iov_len);
    mocrt.PartyIdInvestmentDecisionMaker = get_party_investment_dm(ep,
        doj->positions[ROM_CLR_ACC].iov_base,
        doj->positions[ROM_CLR_ACC].iov_len);
    mocrt.ExecutingTrader = get_executing_trader(ep,
            doj->positions[ROM_TRADER].iov_base,
            doj->positions[ROM_TRADER].iov_len);
    if(getplen(doj, ROM_MANUAL) > 0) {
        char* v = getpval(doj, ROM_MANUAL);
        if(v[0] == 'N') {
            mocrt.PartyIdInvestmentDecisionMakerQualifier = ENUM_EXECUTINGTRADERQUALIFIER_HUMAN;
            mocrt.ExecutingTraderQualifier = ENUM_EXECUTINGTRADERQUALIFIER_HUMAN;
        } else {
            mocrt.PartyIdInvestmentDecisionMakerQualifier = ENUM_EXECUTINGTRADERQUALIFIER_ALGO;
            mocrt.ExecutingTraderQualifier = ENUM_EXECUTINGTRADERQUALIFIER_ALGO;
        }
    } else {
        mocrt.PartyIdInvestmentDecisionMakerQualifier = ENUM_EXECUTINGTRADERQUALIFIER_HUMAN;
        mocrt.ExecutingTraderQualifier = ENUM_EXECUTINGTRADERQUALIFIER_HUMAN;
    }
    if(rep->positions[ROM_PRICE].iov_len > 0) {
        mocrt.Price = rom_to_eur_price(rep->positions[ROM_PRICE].iov_base,
                rep->positions[ROM_PRICE].iov_len);
    } else {
        mocrt.Price = rom_to_eur_price(doj->positions[ROM_PRICE].iov_base,
                doj->positions[ROM_PRICE].iov_len);
    }
    if(rep->positions[ROM_SHARES].iov_len > 0) {
        mocrt.OrderQty = get_qty_for_rom(ROM_SHARES, rep);
    } else {
        mocrt.OrderQty = get_qty_for_rom(ROM_SHARES, doj);
    }
    mocrt.Side = get_8bit_for_rom(ROM_SIDE, doj);
    mocrt.PriceValidityCheckType = 1;
    mocrt.TimeInForce = get_8bit_for_rom(ROM_TIF, doj);
    if(doj->positions[ROM_PROD_CODE].iov_len > 0) {
        mocrt.MarketSegmentID =get_int_for_rom(ROM_PROD_CODE, doj);
    } else {
        mocrt.MarketSegmentID = get_prod_code(ep, doj, db);
    }
    mocrt.ApplSeqIndicator = 1;
    mocrt.SecurityID = get_eurex_multi_leg_sec_id(ROM_SEC_DEF, doj);
    mocrt.ProductComplex = get_8bit_for_rom(ROM_CPLX_TYPE, doj); 
    if(doj->positions[ROM_DESC].iov_len > 0) {
        mocrt.MatchInstCrossID = get_uint_for_rom(ROM_DESC, doj);
    } else {
        mocrt.MatchInstCrossID = ep->smp_id;//NO_VALUE_UINT;
    }
    if(rep->positions[ROM_TYPE].iov_len > 0) {
        mocrt.OrdType = get_eur_type_for_rom(rep);
    } else {
        mocrt.OrdType = get_eur_type_for_rom(doj);
    }
    if(rep->positions[ROM_EXEC_INST].iov_len > 0) {
        mocrt.ExecInst = get_8bit_for_rom(ROM_EXEC_INST, doj);
    } else { 
        if(doj->positions[ROM_EXEC_INST].iov_len > 0) {
            mocrt.ExecInst = get_8bit_for_rom(ROM_EXEC_INST, doj);
        } else {
            mocrt.ExecInst = 2;
        }
    }
    if(rep->positions[ROM_CAP].iov_len > 0) {
        char* c = doj->positions[ROM_CAP].iov_base;
        if(c[0] == 'S') {
            mocrt.TradingCapacity = 6;
            mocrt.OrderOrigination = NO_VALUE_UCHAR;
        } else {
            mocrt.TradingCapacity = 5;
            mocrt.OrderOrigination = NO_VALUE_UCHAR;
        }
    } else { 
        if(doj->positions[ROM_CAP].iov_len > 0) {
            char* c = doj->positions[ROM_CAP].iov_base;
            if(c[0] == 'S') {
                mocrt.TradingCapacity = 6;
                mocrt.OrderOrigination = NO_VALUE_UCHAR;
            } else {
                mocrt.TradingCapacity = 5;
                mocrt.OrderOrigination = NO_VALUE_UCHAR;
            }
        }
    }
    if(doj->positions[ROM_EXPIRE_TIME].iov_len > 0) {
        char exp_str[9];
        memset(&exp_str, '\0', 9);
        memcpy(&exp_str, doj->positions[ROM_EXPIRE_TIME].iov_base, 8);
        mocrt.ExpireDate = atoi((const char*)&exp_str);
    } else {
        mocrt.ExpireDate =NO_VALUE_UINT;
    }
    mocrt.OwnershipIndicator = 0;
    mocrt.NoLegs = 2;
    if(doj->positions[ROM_CLR_ACC].iov_len > 0) {
        databuf_t* db = get_databuf(ci->gk);
        get_acc_type(ep, doj->positions[ROM_CLR_ACC].iov_base,
                doj->positions[ROM_CLR_ACC].iov_len, db);
        memcpy(&mocrt.LegOrdGrp[0].LegAccount, db->rd_ptr, 2);
        memcpy(&mocrt.LegOrdGrp[1].LegAccount, db->rd_ptr, 2);
        destroy_gk_db(ci->gk, db);
        int cp_len =  doj->positions[ROM_CLR_ACC].iov_len < LEN_FREE_TEXT1? 
            doj->positions[ROM_CLR_ACC].iov_len : LEN_FREE_TEXT1 - 1;
        memset(&mocrt.FreeText1, ' ', LEN_FREE_TEXT1);
        memcpy(&mocrt.FreeText1, doj->positions[ROM_CLR_ACC].iov_base,
                    cp_len);
    }
    int oclen = doj->positions[ROM_O_C].iov_len;
    int i = 0;
    for(;i < oclen; ++i) {
        memcpy(&mocrt.LegOrdGrp[i].LegPositionEffect, eur_translate_open_close(
                    (((char*)doj->positions[ROM_O_C].iov_base)[i])), 1);
    }
    pthread_spin_lock(&ep->send_mutex);
    mocrt.RequestHeader.MsgSeqNum = ++ep->seq_num;
    mocrt.TargetPartyIDSessionID = NO_VALUE_UINT;
    unsigned_dart_itoa(doj, ep->seq_num,ROM_LEG_MARK,  db);
    destroy_gk_db(ci->gk, db);
    pthread_spin_unlock(&ep->send_mutex);
    eur_direct_send(ep, &mocrt,bl); //232);
    return ret;
}
int on_eurex_replace_order(con_int* ci, rcntr* risk)
{
    int ret = 1;
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    char str_clordid[32];
    databuf_t* db = get_databuf(ci->gk);
    memset(str_clordid, '\0', 32);
    memcpy(str_clordid, doj->positions[ROM_CLORDID].iov_base,
            doj->positions[ROM_CLORDID].iov_len);

    eurexp* ep = (eurexp*)ci->cparser;
    if(doj->positions[ROM_CPLX_TYPE].iov_len > 0 &&
            ((char*)doj->positions[ROM_CPLX_TYPE].iov_base)[0] != '0') {
        ret = replace_multi_leg_order(ci, risk, ep, doj, str_clordid);
        goto EXIT_REPLACE_ORDER;
    }
    ModifyOrderSingleRequestT mosrt;  
    memset(&mosrt, '\0', sizeof(ModifyOrderSingleRequestT));    
    mosrt.MessageHeaderIn.BodyLen = sizeof(ModifyOrderSingleRequestT);
    mosrt.MessageHeaderIn.TemplateID = TID_MODIFY_ORDER_SINGLE_REQUEST; 
    mosrt.OrderID = NO_VALUE_ULONG;
    mosrt.ClOrdID = NO_VALUE_ULONG;
    mosrt.OrigClOrdID = atol(str_clordid);
    mosrt.OrderOrigination = 5;
    mosrt.PartyIDClientID = get_party_client_id(ep,
        doj->positions[ROM_CLR_ACC].iov_base,
        doj->positions[ROM_CLR_ACC].iov_len);
    mosrt.PartyIdInvestmentDecisionMaker = get_party_investment_dm(ep,
        doj->positions[ROM_CLR_ACC].iov_base,
        doj->positions[ROM_CLR_ACC].iov_len);
    mosrt.ExecutingTrader = get_executing_trader(ep,
            doj->positions[ROM_TRADER].iov_base,
            doj->positions[ROM_TRADER].iov_len);
    if(getplen(doj, ROM_MANUAL) > 0) {
        char* v = getpval(doj, ROM_MANUAL);
        if(v[0] == 'N') {
            mosrt.PartyIdInvestmentDecisionMakerQualifier = ENUM_EXECUTINGTRADERQUALIFIER_HUMAN;
            mosrt.ExecutingTraderQualifier = ENUM_EXECUTINGTRADERQUALIFIER_HUMAN;
        } else {
            mosrt.PartyIdInvestmentDecisionMakerQualifier = ENUM_EXECUTINGTRADERQUALIFIER_ALGO;
            mosrt.ExecutingTraderQualifier = ENUM_EXECUTINGTRADERQUALIFIER_ALGO;
        }
    } else {
        mosrt.PartyIdInvestmentDecisionMakerQualifier = ENUM_EXECUTINGTRADERQUALIFIER_HUMAN;
        mosrt.ExecutingTraderQualifier = ENUM_EXECUTINGTRADERQUALIFIER_HUMAN;
    }
    dart_order_obj* rep = (dart_order_obj*)doj->rep;
    if(rep->positions[ROM_PRICE].iov_len > 0) {
        mosrt.Price = rom_to_eur_price(rep->positions[ROM_PRICE].iov_base,
                rep->positions[ROM_PRICE].iov_len);
    } else {
        mosrt.Price = rom_to_eur_price(doj->positions[ROM_PRICE].iov_base,
                doj->positions[ROM_PRICE].iov_len);
    }
    if(rep->positions[ROM_STOP].iov_len > 0) {
        mosrt.StopPx = rom_to_eur_price(rep->positions[ROM_STOP].iov_base,
                rep->positions[ROM_STOP].iov_len);
    } else if(doj->positions[ROM_STOP].iov_len > 0) {
        mosrt.StopPx = rom_to_eur_price(doj->positions[ROM_STOP].iov_base,
                doj->positions[ROM_STOP].iov_len);
    } else {
        mosrt.StopPx = NO_VALUE_SLONG;                                   
    }
    if(rep->positions[ROM_SHARES].iov_len > 0) {
        mosrt.OrderQty = get_qty_for_rom(ROM_SHARES, rep);
    } else {
        mosrt.OrderQty = get_qty_for_rom(ROM_SHARES, doj);
    }
    if(doj->positions[ROM_EXPIRE_TIME].iov_len > 0) {
        char exp_str[9];
        memset(&exp_str, '\0', 9);
        memcpy(&exp_str, doj->positions[ROM_EXPIRE_TIME].iov_base, 8);
        mosrt.ExpireDate = atoi((const char*)&exp_str);
    } else {
        mosrt.ExpireDate =NO_VALUE_UINT;
    }
    if(doj->positions[ROM_PROD_CODE].iov_len > 0) {
        mosrt.MarketSegmentID =get_int_for_rom(ROM_PROD_CODE, doj);
    } else {
        mosrt.MarketSegmentID = get_prod_code(ep, doj, db);
    }
    mosrt.SimpleSecurityID = get_eurex_secuity_id(ROM_SEC_DEF, doj);
    if(doj->positions[ROM_DESC].iov_len > 0) {
        mosrt.MatchInstCrossID = get_uint_for_rom(ROM_DESC, doj);
    } else {
        mosrt.MatchInstCrossID = ep->smp_id;//NO_VALUE_UINT;
    }
    memcpy(&mosrt.PartyIDLocationID, "GB", 2);
    mosrt.Side = get_8bit_for_rom(ROM_SIDE, doj);
    mosrt.PriceValidityCheckType = 1;
    mosrt.TimeInForce = get_8bit_for_rom(ROM_TIF, doj);
    if(rep->positions[ROM_TYPE].iov_len > 0) {
        mosrt.OrdType = get_eur_type_for_rom(rep);
    } else {
        mosrt.OrdType = get_eur_type_for_rom(doj);
    }
    if(rep->positions[ROM_CLR_ACC].iov_len > 0) {
        databuf_t* db = get_databuf(ci->gk);
        get_acc_type(ep, doj->positions[ROM_CLR_ACC].iov_base,
                doj->positions[ROM_CLR_ACC].iov_len, db);
        memcpy(&mosrt.Account,db->rd_ptr, 2);
        destroy_gk_db(ci->gk, db);
            int cp_len =  rep->positions[ROM_CLR_ACC].iov_len < LEN_FREE_TEXT1? 
                rep->positions[ROM_CLR_ACC].iov_len : LEN_FREE_TEXT1 - 1;
            memset(&mosrt.FreeText1, ' ', LEN_FREE_TEXT1);
            memcpy(&mosrt.FreeText1, rep->positions[ROM_CLR_ACC].iov_base,
                    cp_len);
    } else {
        databuf_t* db = get_databuf(ci->gk);
        get_acc_type(ep, doj->positions[ROM_CLR_ACC].iov_base,
                doj->positions[ROM_CLR_ACC].iov_len, db);
        memcpy(&mosrt.Account, db->rd_ptr, 2);
        destroy_gk_db(ci->gk, db);
            int cp_len =  doj->positions[ROM_CLR_ACC].iov_len < LEN_FREE_TEXT1 ? 
                doj->positions[ROM_CLR_ACC].iov_len : LEN_FREE_TEXT1 - 1;
            memset(&mosrt.FreeText1, ' ', LEN_FREE_TEXT1);
            memcpy(&mosrt.FreeText1, doj->positions[ROM_CLR_ACC].iov_base,
                    cp_len);
    }
    if(rep->positions[ROM_O_C].iov_len > 0) {
        memcpy(&mosrt.PositionEffect, eur_translate_open_close(
                    (((char*)rep->positions[ROM_O_C].iov_base)[0])), 1);
    } else if(doj->positions[ROM_O_C].iov_len > 0) {
        memcpy(&mosrt.PositionEffect, eur_translate_open_close(
                    (((char*)doj->positions[ROM_O_C].iov_base)[0])), 1);
    } else {
        memcpy(&mosrt.PositionEffect, "C", 1);
    }
    if(rep->positions[ROM_EXEC_INST].iov_len > 0) {
        mosrt.ExecInst = get_8bit_for_rom(ROM_EXEC_INST, doj);
    } else { 
        if(doj->positions[ROM_EXEC_INST].iov_len > 0) {
            mosrt.ExecInst = get_8bit_for_rom(ROM_EXEC_INST, doj);
        } else {
            mosrt.ExecInst = 2;
        }
    }
    if(doj->positions[ROM_TYPE].iov_len ==2) {
        mosrt.TradingSessionSubID = 4;
    } else {
        mosrt.TradingSessionSubID =NO_VALUE_UCHAR; //ENUM_TRADING_SESSION_SUBID_ANY_AUCTION; 
    }
    if(rep->positions[ROM_CAP].iov_len > 0) {
        char* c = doj->positions[ROM_CAP].iov_base;
        if(c[0] == 'S') {
            mosrt.TradingCapacity = 6;
            mosrt.OrderOrigination = NO_VALUE_UCHAR;
        } else {
            mosrt.TradingCapacity = 5;
            mosrt.OrderOrigination = NO_VALUE_UCHAR;
        }
    } else { 
        if(doj->positions[ROM_CAP].iov_len > 0) {
            char* c = doj->positions[ROM_CAP].iov_base;
            if(c[0] == 'S') {
                mosrt.TradingCapacity = 6;
                mosrt.OrderOrigination = NO_VALUE_UCHAR;
            } else {
                mosrt.TradingCapacity = 5;
                mosrt.OrderOrigination = NO_VALUE_UCHAR;
            }
        }
    }
    mosrt.OrderID = NO_VALUE_ULONG;
    mosrt.ClOrdID = NO_VALUE_ULONG;
    mosrt.TargetPartyIDSessionID = NO_VALUE_UINT;
    mosrt.ApplSeqIndicator = 1;
    mosrt.OwnershipIndicator = 0;
    pthread_spin_lock(&ep->send_mutex);
    mosrt.RequestHeader.MsgSeqNum = ++ep->seq_num;
    unsigned_dart_itoa(doj, ep->seq_num,ROM_LEG_MARK,  db);
    destroy_gk_db(ci->gk, db);
    mosrt.RequestHeader.SenderSubID = get_user_id(ep,
            doj->positions[ROM_CLR_ACC].iov_base,
            doj->positions[ROM_CLR_ACC].iov_len);//ep->user_logon_id;
    pthread_spin_unlock(&ep->send_mutex);
    eur_direct_send(ep, &mosrt, sizeof(ModifyOrderSingleRequestT));
EXIT_REPLACE_ORDER:
    return ret;
}
static int cancel_multi_leg_order(con_int* ci, rcntr* risk,
        eurexp* ep, dart_order_obj* doj, char* clordid)
{
    DeleteOrderComplexRequestT docrt;
    memset(&docrt, '\0', sizeof(DeleteOrderComplexRequestT));
    docrt.MessageHeaderIn.BodyLen = sizeof(DeleteOrderComplexRequestT);
    docrt.MessageHeaderIn.TemplateID = TID_DELETE_ORDER_COMPLEX_REQUEST; 
    docrt.OrigClOrdID = atol(clordid);
    databuf_t* db = get_databuf(ci->gk);
    docrt.SecurityID = get_eurex_multi_leg_sec_id(ROM_SEC_DEF, doj);
    if(doj->positions[ROM_PROD_CODE].iov_len > 0) {
        docrt.MarketSegmentID =get_int_for_rom(ROM_PROD_CODE, doj);
    } else {
        docrt.MarketSegmentID = get_prod_code(ep, doj, db);
    }
    docrt.PartyIdInvestmentDecisionMaker = get_party_investment_dm(ep,
        doj->positions[ROM_CLR_ACC].iov_base,
        doj->positions[ROM_CLR_ACC].iov_len);
    docrt.ExecutingTrader = get_executing_trader(ep,
            doj->positions[ROM_TRADER].iov_base,
            doj->positions[ROM_TRADER].iov_len);
    if(getplen(doj, ROM_MANUAL) > 0) {
        char* v = getpval(doj, ROM_MANUAL);
        if(v[0] == 'N') {
            docrt.PartyIdInvestmentDecisionMakerQualifier = ENUM_EXECUTINGTRADERQUALIFIER_HUMAN;
            docrt.ExecutingTraderQualifier = ENUM_EXECUTINGTRADERQUALIFIER_HUMAN;
        } else {
            docrt.PartyIdInvestmentDecisionMakerQualifier = ENUM_EXECUTINGTRADERQUALIFIER_ALGO;
            docrt.ExecutingTraderQualifier = ENUM_EXECUTINGTRADERQUALIFIER_ALGO;
        }
    } else {
        docrt.PartyIdInvestmentDecisionMakerQualifier = ENUM_EXECUTINGTRADERQUALIFIER_HUMAN;
        docrt.ExecutingTraderQualifier = ENUM_EXECUTINGTRADERQUALIFIER_HUMAN;
    }
    docrt.OrderID = NO_VALUE_ULONG;
    docrt.ClOrdID = NO_VALUE_ULONG;
    docrt.OrderOrigination = 5;
    docrt.TargetPartyIDSessionID = NO_VALUE_UINT;
    pthread_spin_lock(&ep->send_mutex);
    docrt.RequestHeader.MsgSeqNum = ++ep->seq_num;
    unsigned_dart_itoa(doj, ep->seq_num,ROM_LEG_MARK,  db);
    destroy_gk_db(ci->gk, db);
    docrt.RequestHeader.SenderSubID = get_user_id(ep,
            doj->positions[ROM_CLR_ACC].iov_base,
            doj->positions[ROM_CLR_ACC].iov_len);//ep->user_logon_id;
    pthread_spin_unlock(&ep->send_mutex);
    eur_direct_send(ep, &docrt, sizeof(DeleteOrderComplexRequestT));
    return 1;
}
int on_eurex_cancel_order(con_int* ci, rcntr* risk)
{
    int ret = 1;
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    char str_clordid[32];
    memset(str_clordid, '\0', 32);
    memcpy(str_clordid, doj->positions[ROM_CLORDID].iov_base,
            doj->positions[ROM_CLORDID].iov_len);

    eurexp* ep = (eurexp*)ci->cparser;
    if(doj->positions[ROM_CPLX_TYPE].iov_len > 0 &&
            ((char*)doj->positions[ROM_CPLX_TYPE].iov_base)[0] != '0') {
        ret = cancel_multi_leg_order(ci, risk, ep, doj, str_clordid);
        goto EXIT_CANCEL_ORDER;
    }
    databuf_t* db = get_databuf(ci->gk);
    DeleteOrderSingleRequestT dosrt;
    memset(&dosrt, '\0', sizeof(DeleteOrderSingleRequestT));
    dosrt.MessageHeaderIn.BodyLen = sizeof(DeleteOrderSingleRequestT);
    dosrt.MessageHeaderIn.TemplateID = TID_DELETE_ORDER_SINGLE_REQUEST; 
    dosrt.OrderID = NO_VALUE_ULONG;
    dosrt.ClOrdID = NO_VALUE_ULONG;
    dosrt.OrderOrigination = 5;
    dosrt.TargetPartyIDSessionID = NO_VALUE_UINT;
    dosrt.PartyIdInvestmentDecisionMaker = get_party_investment_dm(ep,
        doj->positions[ROM_CLR_ACC].iov_base,
        doj->positions[ROM_CLR_ACC].iov_len);
    dosrt.ExecutingTrader = get_executing_trader(ep,
            doj->positions[ROM_TRADER].iov_base,
            doj->positions[ROM_TRADER].iov_len);
    if(getplen(doj, ROM_MANUAL) > 0) {
        char* v = getpval(doj, ROM_MANUAL);
        if(v[0] == 'N') {
            dosrt.PartyIdInvestmentDecisionMakerQualifier = ENUM_EXECUTINGTRADERQUALIFIER_HUMAN;
            dosrt.ExecutingTraderQualifier = ENUM_EXECUTINGTRADERQUALIFIER_HUMAN;
        } else {
            dosrt.PartyIdInvestmentDecisionMakerQualifier = ENUM_EXECUTINGTRADERQUALIFIER_ALGO;
            dosrt.ExecutingTraderQualifier = ENUM_EXECUTINGTRADERQUALIFIER_ALGO;
        }
    } else {
        dosrt.PartyIdInvestmentDecisionMakerQualifier = ENUM_EXECUTINGTRADERQUALIFIER_HUMAN;
        dosrt.ExecutingTraderQualifier = ENUM_EXECUTINGTRADERQUALIFIER_HUMAN;
    }
    dosrt.OrigClOrdID = atol(str_clordid);
    dosrt.SimpleSecurityID = get_eurex_secuity_id(ROM_SEC_DEF, doj);
    if(doj->positions[ROM_PROD_CODE].iov_len > 0) {
        dosrt.MarketSegmentID =get_int_for_rom(ROM_PROD_CODE, doj);
    } else {
        dosrt.MarketSegmentID = get_prod_code(ep, doj, db);
    }
    pthread_spin_lock(&ep->send_mutex);
    dosrt.RequestHeader.MsgSeqNum = ++ep->seq_num;
    unsigned_dart_itoa(doj, ep->seq_num,ROM_LEG_MARK,  db);
    destroy_gk_db(ci->gk, db);
    dosrt.RequestHeader.SenderSubID = get_user_id(ep,
            doj->positions[ROM_CLR_ACC].iov_base,
            doj->positions[ROM_CLR_ACC].iov_len);//ep->user_logon_id;
    pthread_spin_unlock(&ep->send_mutex);
    eur_direct_send(ep, &dosrt, sizeof(DeleteOrderSingleRequestT));
EXIT_CANCEL_ORDER:
    return ret;
}
static void eurex_cancel_all_orders(con_int* ci)
{
    databuf_t* db = get_databuf(ci->gk);
    rcntr* cnt = 0;
    hashmap_iterator_t hi = hashmap_iterator_begin(ci->cl_ord_id_map);
    char *key = 0;
    int key_len = 0;
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                (void *) &cnt);
    while (valid && cnt && cnt->raw) {
        on_eurex_cancel_order(ci, cnt);
        valid = hashmap_iterator_next(ci->cl_ord_id_map, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                    &key_len, (void *) &cnt);
    }
    hashmap_iterator_destroy(hi);
    destroy_gk_db(ci->gk, db);
}

static void eurex_on_connect(void* pass_back, int is_connected,
        dart_order_obj* log)
{
    eurexp* uparse = (eurexp*)pass_back;
    con_int* ci = uparse->pbp;
    if(is_connected < 0) {
        ci->update_oats(ci, 0);
    } else {
        ci->update_oats(ci, is_connected);
    }
    switch(is_connected) {
        case 0:
            ci->can_send_orders = 0;
            ci->connected = 0;
            reload_config(ci->ex);
            if(uparse->recon_wrap == 0) {
                uparse->recon_wrap = add_timer(ci->p,0,
                        0,30,0,
                        uparse->rc,
                        reconnect);
            } else {
                add_recon_timer(ci->p, uparse->recon_wrap);
            }
            break;
        case 1:
            ci->sock = uparse->rc->sock;
            ci->can_send_orders = 1;
            ci->connected = 1;
            break;
        case 2:
            ci->sock = uparse->rc->sock;
            ci->can_send_orders = 0;
            ci->connected = 1;
            break;
        case -1:
            eurex_cancel_all_orders(ci);
            iterate_logouts(ci->cparser);//send_eur_logout(ci->cparser);
            ci->can_send_orders = 0;
            ci->connected = 0;
            break;
    }
}

static void parse_eurex(int *cut_con, rex_con *rc);

static void eurex_send_oats_message(con_int* ci, rcntr* cnt)
{
    con_int* oat = get_compliance_con(ci->gk);
    if(oat) {
        send_oats_message(oat, cnt);
    }
}
static void log_n_go(con_int* sender, rcntr* cnt, con_int* ci)
{
    eurex_send_oats_message(ci, cnt);
}
static void log_n_out(con_int* sender, rcntr* cnt, con_int* ci)
{
    jp_limits_checker *jlc =
        (jp_limits_checker *) get_balance_struct(ci->gk);
    update_balance(jlc, cnt);
    eurex_send_oats_message(ci, cnt);
}
static void log_n_fill(con_int* sender, rcntr* cnt, con_int* ci)
{
    jp_limits_checker *jlc =
        (jp_limits_checker *) get_balance_struct(ci->gk);
    update_balance_fill(jlc, cnt);
    eurex_send_oats_message(ci, cnt);
}
static void log_n_rej(con_int* sender, rcntr* cnt, con_int* ci)
{
    dart_order_obj* doj = (dart_order_obj*)cnt->raw;
    int s_len = doj->positions[ROM_STATUS].iov_len;
    if(s_len == 1) {
        jp_limits_checker *jlc =
            (jp_limits_checker *) get_balance_struct(ci->gk);
        update_balance(jlc, cnt);
    } 
    eurex_send_oats_message(ci, cnt);
}

static void set_status(NewOrderResponseT* nort, dart_order_obj* doj)
{
    set_rom_field(doj, 0, "S",1);
    if(nort->OrdStatus[0] == '0' && nort->ExecType[0] == '0') {
        set_rom_field(doj, ROM_STATUS, "13",2);
    } else if(nort->ExecType[0] == 'L') {
        set_rom_field(doj, ROM_STATUS, "13",2);
    } else {
        set_rom_field(doj, ROM_STATUS, nort->OrdStatus,1);
    }
}
static void handle_eur_open(eurexp* ep, con_int* ci, 
        NewOrderResponseT* nort)
{
    databuf_t* db = get_databuf(ci->gk);
    uint64_t lret = no_set_unsigned_itoa(db->wr_ptr, nort->ClOrdID);
    pthread_spin_lock(&ci->mutex);
    /*START LOCK*/
    rcntr* cnt = 0;
    int ret = get_map_entry(ci->cl_ord_id_map, db->rd_ptr, lret, (void**)&cnt);
    /*END LOCK*/
    pthread_spin_unlock(&ci->mutex);
    if(ret) {
        dart_order_obj* doj = (dart_order_obj*)cnt->raw;
        set_status(nort, doj);
        databuf_reset(db);
        int len = eur_to_rom_time(nort->ResponseHeaderME.SendingTime, db); 
        set_rom_field(doj, EXEC_TIME, db->rd_ptr, len);
        databuf_reset(db);
        u64_dart_itoa(doj, nort->ResponseHeaderME.SendingTime, ROM_DISP_INSTR, db);
        databuf_reset(db);
        if(nort->OrdStatus[0] == '4') {
            pthread_spin_lock(&ci->mutex);
            remove_from_map(ci->cl_ord_id_map, doj->positions[ROM_CLORDID].iov_base,
                    doj->positions[ROM_CLORDID].iov_len);
            pthread_spin_unlock(&ci->mutex);
            if(doj->sender != NULL && ((con_int*)doj->sender)->on_out) {
                ((con_int*)doj->sender)->on_out((con_int*)doj->sender,
                    cnt);
            } else {
                con_int* sender = 0;
                get_obj_by_client_id(ci->gk, 
                        cnt->owner,cnt->olen, (void**)&sender);        
                if(sender && sender->on_update) {
                    doj->sender = sender;
                    sender->on_out(sender, cnt);
                } else {
                    if(sender) {
                        log_n_out(sender, cnt, ci);
                    }
                    send_debug_message(ci->o,"Could not find sender for: %ld \n",
                            nort->ClOrdID);
                }
            }   
        } else {
            if(doj->positions[ROM_LEAVES].iov_len <= 0) {
                set_rom_field(doj, ROM_LEAVES, doj->positions[ROM_SHARES].iov_base,
                        doj->positions[ROM_SHARES].iov_len);
            }
            if(doj->sender != NULL && ((con_int*)doj->sender)->on_update) {
                ((con_int*)doj->sender)->on_update((con_int*)doj->sender,
                    cnt);
            } else {
                con_int* sender = 0;
                get_obj_by_client_id(ci->gk, 
                        cnt->owner,cnt->olen, (void**)&sender);        
                if(sender && sender->on_update) {
                    doj->sender = sender;
                    sender->on_update(sender, cnt);
                } else {
                    if(sender) {
                        log_n_go(sender, cnt, ci);
                    }
                    send_debug_message(ci->o,"Could not find sender for: %ld \n",
                            nort->ClOrdID);
                }
            }   
        }
    } else {
        send_debug_message(ci->o,"Could not find: %ld, in map.\n", nort->ClOrdID);
    }
    destroy_gk_db(ci->gk, db);   
}

static void set_restatement_reason(dart_order_obj* doj, uint16_t rcode)
{
    switch(rcode) {
        case 0x01:
            set_rom_field(doj, ROM_TEXT, "Order book restatement", 22);
            break;
        case 0x65:
            set_rom_field(doj, ROM_TEXT, "Order add accepted", 18);
            break;
        case 0x66:
            set_rom_field(doj, ROM_TEXT, "Order modify accepted", 21);
            break;
        case 0x67:
            set_rom_field(doj, ROM_TEXT, "Order delete accepted", 21);
            break;
        case 0x69:
            set_rom_field(doj, ROM_TEXT, "Order IOC accepted", 18);
            break;
        case 0x6c:
            set_rom_field(doj, ROM_TEXT, "Order book executed", 19);
            break;
        case 0x87:
            set_rom_field(doj, ROM_TEXT, "Order Market executed", 21);
            break;
        case 0x95:
            set_rom_field(doj, ROM_TEXT, "Order Auction activated", 23);
            break;
        case 0x96:
            set_rom_field(doj, ROM_TEXT, "Order Auction inactivated", 25);
            break;
        case 0xa4:
            set_rom_field(doj, ROM_TEXT, "One Cancels the other", 21);
            break;
        case 0xac:
            set_rom_field(doj, ROM_TEXT, "Stop Triggered", 14);
            break;
        case 0xd4:
            set_rom_field(doj, ROM_TEXT, "Order book or Cancel Accepted", 29);
            break;
        case 0xc7:
            set_rom_field(doj, ROM_TEXT, "End of Freeze state", 19);
            break;
        case 0xb5:
            set_rom_field(doj, ROM_TEXT, "Ownership Changed", 17);
            break;
        default:
            break;
    }
}

static void send_back_fill(con_int* ci, dart_order_obj* doj, rcntr* cnt)
{
    if(doj->sender != 0x0 && ((con_int*)doj->sender)->on_fill) {
        ((con_int*)doj->sender)->on_fill(doj->sender, cnt);
    } else {
        con_int* sender = 0;
        get_obj_by_client_id(ci->gk, cnt->owner,cnt->olen, (void**)&sender);        
        if(sender && sender->on_fill) {
            sender->on_fill(sender, cnt);
        } else if(sender) {
            log_n_fill(sender, cnt, ci);
        }
    }
}
static void record_leg_executions(int num_legs,
        FillsGrpSeqT* fp, int num_fills,  eurexp* ep,
        con_int* ci, dart_order_obj* doj, databuf_t* db,
        rcntr* cnt)
{
    send_debug_message(ci->o, "Spread, numlegs: %d, numfills %d \n",
            num_legs, num_fills);
    void* offset = fp;
    InstrmntLegExecGrpSeqT* ileg = (offset + (num_fills * sizeof(FillsGrpSeqT)));
    int len = 0;
    int i = 0;
    for(;i < num_legs;++i) {
        databuf_reset(db);
        len = eur_to_rom_price(ileg[i].LegLastPx, db);
        set_rom_field(doj, ROM_EXEC_PRICE, db->rd_ptr, len);
        databuf_reset(db);
        trans_eur_to_rom_qty(doj, ROM_LAST_SHARES,
                ileg[i].LegLastQty, db);
        databuf_reset(db);
        len = dart_itoa(doj,  
                ileg[i].LegExecID, EXEC_ID, db);
        databuf_reset(db);
        len = unsigned_dart_itoa(doj,  
                ileg[i].LegSide, ROM_SIDE,db);
        databuf_reset(db);
        len = u64_dart_itoa(doj, 
                ileg[i].LegSecurityID,ROM_SEC_DEF, db);
        databuf_reset(db);
        len = dart_itoa(doj,
                fp[ileg[i].FillRefID].FillLiquidityInd,ROM_LIQ, db);
        set_rom_field(doj, ROM_CPLX_TYPE, "6", 1);
        send_debug_message(ci->o,"Leg Execution: SecID %ld,execID: %ld,last:%ld,side: %ld. fill idx: %u\n",
                ileg[i].LegSecurityID, ileg[i].LegExecID, ileg[i].LegLastQty,
                ileg[i].LegSide, ileg[i].FillRefID);
        send_back_fill(ci, doj, cnt);
    }
}

static int iterate_eur_broad_fills(eurexp* ep, con_int* ci, 
        OrderExecReportBroadcastT* oerbt, dart_order_obj* doj,
        rcntr* cnt, databuf_t* db) 
{
    int removed = 0;
    int i = 0;
    for(;i < oerbt->NoFills;++i) {
        databuf_reset(db);
        int len = eur_to_rom_price(oerbt->FillsGrp[i].FillPx, db);
        set_rom_field(doj, ROM_EXEC_PRICE, db->rd_ptr, len); 
        databuf_reset(db);
        trans_eur_to_rom_qty(doj, ROM_LAST_SHARES,
                oerbt->FillsGrp[i].FillQty, db);
        databuf_reset(db);
        len = dart_itoa(doj, 
                oerbt->FillsGrp[i].FillExecID,EXEC_ID, db);
        databuf_reset(db);
        len = unsigned_dart_itoa(doj, 
                oerbt->FillsGrp[i].FillMatchID,ROM_TEXT, db);
        databuf_reset(db);
        len = unsigned_dart_itoa(doj, 
                oerbt->FillsGrp[i].FillLiquidityInd, ROM_LIQ,db);
        cnt->last = oerbt->FillsGrp[i].FillQty/10000;
        set_rom_field(doj, ROM_STATUS, "1", 1);
        if(i == (oerbt->NoFills - 1)) {
            databuf_reset(db);
            cnt->cum = oerbt->CumQty/10000;
            trans_eur_to_rom_qty(doj,  ROM_LEAVES,oerbt->LeavesQty, db);
            databuf_reset(db);
            trans_eur_to_rom_qty(doj, ROM_CUM, oerbt->CumQty, db);
            set_rom_field(doj, ROM_STATUS, oerbt->OrdStatus, 1);
            if(oerbt->OrdStatus[0] == '2' && (cnt->size - cnt->cum) <= 0) {
                pthread_spin_lock(&ci->mutex);
                remove_from_map(ci->cl_ord_id_map, doj->positions[ROM_CLORDID].iov_base,
                        doj->positions[ROM_CLORDID].iov_len);
                pthread_spin_unlock(&ci->mutex);
                removed = 1;
            }
        }
        send_back_fill(ci, doj, cnt);
    }
    return removed;
}
static void handle_eur_book_broad(eurexp* ep, con_int* ci,
        OrderExecReportBroadcastT* oerbt)
{
    databuf_t* db = get_databuf(ci->gk);
    int len = no_set_unsigned_itoa(db->wr_ptr, oerbt->ClOrdID);
    pthread_spin_lock(&ci->mutex);
    /*START LOCK*/
    rcntr* cnt = 0;
    int ret = get_map_entry(ci->cl_ord_id_map, db->rd_ptr, len, (void**)&cnt);
    /*END LOCK*/
    pthread_spin_unlock(&ci->mutex);
    if(ret) {
        //do nothing
    } else {
        databuf_reset(db);
        len = no_set_unsigned_itoa(db->wr_ptr, oerbt->OrigClOrdID);
        pthread_spin_lock(&ci->mutex);
        int ret = get_map_entry(ci->cl_ord_id_map, db->rd_ptr, len, (void**)&cnt);
        pthread_spin_unlock(&ci->mutex);
        if(ret) {
            //do nothing
        } else {
            send_debug_message(ci->o,"Could not find: %ld,orig: %ld,leave:%ld,cum: %ld.\n",
                    oerbt->ClOrdID, oerbt->OrigClOrdID, oerbt->LeavesQty,
                    oerbt->CumQty);
            return;
        }
    }
    dart_order_obj* doj = cnt->raw;
    set_rom_field(doj, 0, "S",1);
    databuf_reset(db);
    int tlen = eur_to_rom_time(oerbt->RBCHeaderME.SendingTime, db); 
    set_rom_field(doj, EXEC_TIME, db->rd_ptr, tlen);
    databuf_reset(db);
    u64_dart_itoa(doj, oerbt->RBCHeaderME.SendingTime, ROM_DISP_INSTR, db);
    databuf_reset(db);
    if(oerbt->NoLegExecs > 0) {
        send_debug_message(ci->o,"CMPLX exe: %ld,status: %s,leave:%ld,cum: %ld.\n",
                oerbt->ClOrdID, oerbt->OrdStatus, oerbt->LeavesQty,
                oerbt->CumQty);
        int sdlen = doj->positions[ROM_SEC_DEF].iov_len;
        char* temp_sec_def = calloc(1, sdlen);
        memcpy(temp_sec_def, doj->positions[ROM_SEC_DEF].iov_base, sdlen);
        if(oerbt->NoFills > 1) {
            set_rom_field(doj, ROM_STATUS, "1", 1);
        } else {
            set_rom_field(doj, ROM_STATUS, oerbt->OrdStatus, 1);
        }
        record_leg_executions(oerbt->NoLegExecs,
                oerbt->FillsGrp, oerbt->NoFills, ep, ci, doj, db, cnt);
        set_rom_field(doj, ROM_SEC_DEF, temp_sec_def, sdlen);
        set_rom_field(doj, ROM_CPLX_TYPE, "5", 1);
        int kill = iterate_eur_broad_fills(ep, ci, oerbt, doj, cnt, db);
        if(kill) {
            eurex_destroy_cmplx_ord(cnt, ci);
        }
    } else {
        if(oerbt->NoFills > 0) {
            iterate_eur_broad_fills(ep, ci, oerbt, doj, cnt, db);
        } else {
            databuf_reset(db);
            trans_eur_to_rom_qty(doj, ROM_LEAVES, oerbt->LeavesQty, db);
            databuf_reset(db);
            trans_eur_to_rom_qty(doj, ROM_CUM, oerbt->CumQty, db);
            databuf_reset(db);
            trans_eur_to_rom_qty(doj, ROM_SHARES, oerbt->OrderQty, db);
            if(oerbt->OrdStatus[0] == '0') {
                set_rom_field(doj, ROM_STATUS, "13", 2);
            } else {
                set_rom_field(doj, ROM_STATUS, oerbt->OrdStatus, 1);
            }
            set_restatement_reason(doj, oerbt->ExecRestatementReason);
            if(oerbt->OrdStatus[0] == '4') {
                pthread_spin_lock(&ci->mutex);
                remove_from_map(ci->cl_ord_id_map, doj->positions[ROM_CLORDID].iov_base,
                        doj->positions[ROM_CLORDID].iov_len);
                pthread_spin_unlock(&ci->mutex);
                if(doj->sender != 0x0 && ((con_int*)doj->sender)->on_out) {
                    ((con_int*)doj->sender)->on_out(doj->sender, cnt);
                } else {
                    con_int* sender = 0;
                    get_obj_by_client_id(ci->gk, cnt->owner,cnt->olen, (void**)&sender);        
                    if(sender && sender->on_out) {
                        sender->on_out(sender, cnt);
                    } else if(sender) {
                        log_n_out(sender, cnt, ci);
                    }
                }
            } else {
                if(doj->sender != 0x0 && ((con_int*)doj->sender)->on_update) {
                    ((con_int*)doj->sender)->on_update(doj->sender, cnt);
                } else {
                    con_int* sender = 0;
                    get_obj_by_client_id(ci->gk, cnt->owner,cnt->olen, (void**)&sender);        
                    if(sender && sender->on_update) {
                        sender->on_update(sender, cnt);
                    } 
                }
            }
        }
    }
}

static int iterate_eur_exec_fill(eurexp* ep, con_int* ci,
        OrderExecNotificationT* oent, dart_order_obj* doj,
        rcntr* cnt, databuf_t* db)
{
    int removed = 0;
    int i = 0;
    for(;i < oent->NoFills;++i) {
        databuf_reset(db);
        int len = eur_to_rom_price(oent->FillsGrp[i].FillPx, db);
        set_rom_field(doj, ROM_EXEC_PRICE, db->rd_ptr, len); 
        databuf_reset(db);
        trans_eur_to_rom_qty(doj, ROM_LAST_SHARES,
                oent->FillsGrp[i].FillQty, db);
        databuf_reset(db);
        len = dart_itoa(doj, 
                oent->FillsGrp[i].FillExecID,EXEC_ID, db);
        databuf_reset(db);
        len = unsigned_dart_itoa(doj, 
                oent->FillsGrp[i].FillMatchID,ROM_TEXT, db);
        databuf_reset(db);
        len = dart_itoa(doj, 
                oent->FillsGrp[i].FillLiquidityInd, ROM_LIQ,db);

        cnt->last = oent->FillsGrp[i].FillQty /10000;
        set_rom_field(doj, ROM_STATUS, "1", 1);
        if(i == (oent->NoFills - 1)) {
            databuf_reset(db);
            cnt->cum = oent->CumQty/ 10000;
            trans_eur_to_rom_qty(doj, ROM_LEAVES, oent->LeavesQty, db);
            databuf_reset(db);
            trans_eur_to_rom_qty(doj,  ROM_CUM,oent->CumQty, db);
            set_rom_field(doj, ROM_STATUS, oent->OrdStatus, 1);
            if(oent->OrdStatus[0] == '2' && (cnt->size - cnt->cum) <= 0) {
                pthread_spin_lock(&ci->mutex);
                remove_from_map(ci->cl_ord_id_map, doj->positions[ROM_CLORDID].iov_base,
                        doj->positions[ROM_CLORDID].iov_len);
                pthread_spin_unlock(&ci->mutex);
                removed = 1;
            }
        }
        send_back_fill(ci, doj, cnt);
    }
    return removed;
}
static void handle_eur_book_exec(eurexp* ep, con_int* ci,
        OrderExecNotificationT* oent)
{
    databuf_t* db = get_databuf(ci->gk);
    int len = no_set_unsigned_itoa(db->wr_ptr, oent->ClOrdID);
    pthread_spin_lock(&ci->mutex);
    /*START LOCK*/
    rcntr* cnt = 0;
    int ret = get_map_entry(ci->cl_ord_id_map, db->rd_ptr, len, (void**)&cnt);
    /*END LOCK*/
    pthread_spin_unlock(&ci->mutex);
    if(ret) {
        //do nothing
    } else {
        databuf_reset(db);
        len = no_set_unsigned_itoa(db->wr_ptr, oent->OrigClOrdID);
        pthread_spin_lock(&ci->mutex);
        int ret = get_map_entry(ci->cl_ord_id_map, db->rd_ptr, len, (void**)&cnt);
        pthread_spin_unlock(&ci->mutex);
        if(ret) {
            //do nothing
        } else {
            send_debug_message(ci->o,"Could not find: %ld,orig: %ld,leave:%ld,cum: %ld.\n",
                    oent->ClOrdID, oent->OrigClOrdID, oent->LeavesQty,
                    oent->CumQty);
            return;
        }
    }
    dart_order_obj* doj = cnt->raw;
    set_rom_field(doj, 0, "S",1);
    databuf_reset(db);
    int tlen = eur_to_rom_time(oent->RBCHeaderME.SendingTime, db); 
    set_rom_field(doj, EXEC_TIME, db->rd_ptr, tlen);
    databuf_reset(db);
    u64_dart_itoa(doj, oent->RBCHeaderME.SendingTime, ROM_DISP_INSTR, db);
    databuf_reset(db);
    if(oent->NoLegExecs > 0) {
        send_debug_message(ci->o,"Cmplx exe: %ld,status: %s,leave:%ld,cum: %ld.\n",
                oent->ClOrdID, oent->OrdStatus, oent->LeavesQty,
                oent->CumQty);
        int sdlen = doj->positions[ROM_SEC_DEF].iov_len;
        char* temp_sec_def = calloc(1, sdlen);
        memcpy(temp_sec_def, doj->positions[ROM_SEC_DEF].iov_base, sdlen);
        if(oent->NoFills > 1) {
            set_rom_field(doj, ROM_STATUS, "1", 1);
        } else {
            set_rom_field(doj, ROM_STATUS, oent->OrdStatus, 1);
        }
        record_leg_executions(oent->NoLegExecs,
                oent->FillsGrp, oent->NoFills, ep, ci, doj, db, cnt);
        set_rom_field(doj, ROM_SEC_DEF, temp_sec_def, sdlen);
        set_rom_field(doj, ROM_CPLX_TYPE, "5", 1);
        int kill = iterate_eur_exec_fill(ep, ci, oent, doj, cnt, db);
        if(kill) {
            eurex_destroy_cmplx_ord(cnt, ci);
        }
    } else {
        if(oent->NoFills > 0) {
            iterate_eur_exec_fill(ep, ci, oent, doj, cnt, db);
        } else {
            databuf_reset(db);
            trans_eur_to_rom_qty(doj, ROM_LEAVES,oent->LeavesQty,  db);
            databuf_reset(db);
            trans_eur_to_rom_qty(doj, ROM_CUM,oent->CumQty,  db);
            if(oent->ExecType[0] == '0') {
                set_rom_field(doj, ROM_STATUS, "13", 2);
            } else {
                set_rom_field(doj, ROM_STATUS, oent->OrdStatus, 1);
            }
            if(oent->OrdStatus[0] == '4') {
                pthread_spin_lock(&ci->mutex);
                remove_from_map(ci->cl_ord_id_map,  doj->positions[ROM_CLORDID].iov_base,
                        doj->positions[ROM_CLORDID].iov_len);
                pthread_spin_unlock(&ci->mutex);
                if(doj->sender != 0x0 && ((con_int*)doj->sender)->on_out) {
                    ((con_int*)doj->sender)->on_out(doj->sender, cnt);
                } else {
                    con_int* sender = 0;
                    get_obj_by_client_id(ci->gk, cnt->owner,cnt->olen, (void**)&sender);        
                    if(sender && sender->on_out) {
                        sender->on_out(sender, cnt);
                    } else if(sender) {
                        log_n_out(sender, cnt, ci);
                    }
                }
            } else {
                if(doj->sender != 0x0 && ((con_int*)doj->sender)->on_update) {
                    ((con_int*)doj->sender)->on_update(doj->sender, cnt);
                } else {
                    con_int* sender = 0;
                    get_obj_by_client_id(ci->gk, cnt->owner,cnt->olen, (void**)&sender);        
                    if(sender && sender->on_update) {
                        sender->on_update(sender, cnt);
                    } 
                }
            }
        }
    }
    destroy_gk_db(ci->gk, db);   
}
static void handle_eur_bc_out(eurexp* ep, con_int* ci,
        DeleteOrderBroadcastT* dobt)
{
    databuf_t* db = get_databuf(ci->gk);
    int len = no_set_unsigned_itoa(db->wr_ptr, dobt->ClOrdID);
    pthread_spin_lock(&ci->mutex);
    /*START LOCK*/
    rcntr* cnt = 0;
    int ret = get_map_entry(ci->cl_ord_id_map, db->rd_ptr, len, (void**)&cnt);
    /*END LOCK*/
    pthread_spin_unlock(&ci->mutex);
    if(ret) {
        //do nothing
    } else {
        databuf_reset(db);
        len = no_set_unsigned_itoa(db->wr_ptr, dobt->OrigClOrdID);
        pthread_spin_lock(&ci->mutex);
        int ret = get_map_entry(ci->cl_ord_id_map, db->rd_ptr, len, (void**)&cnt);
        pthread_spin_unlock(&ci->mutex);
        if(ret) {
            //do nothing
        } else {
            send_debug_message(ci->o,"Could not find: %ld,orig: %ld,leave:%ld,cum: %ld.\n",
                    dobt->ClOrdID, dobt->OrigClOrdID, dobt->CxlQty,
                    dobt->CumQty);
            return;
        }
    }
    dart_order_obj* doj = cnt->raw;
    set_rom_field(doj, 0, "S",1);
    databuf_reset(db);
    int tlen = eur_to_rom_time(dobt->RBCHeaderME.SendingTime, db); 
    set_rom_field(doj, EXEC_TIME, db->rd_ptr, tlen);
    databuf_reset(db);
    u64_dart_itoa(doj, dobt->RBCHeaderME.SendingTime, ROM_DISP_INSTR, db);
    databuf_reset(db);
    set_rom_field(doj, ROM_STATUS, dobt->ExecType, 1);
    if(dobt->ExecType[0] == '4') {
        pthread_spin_lock(&ci->mutex);
        remove_from_map(ci->cl_ord_id_map,  doj->positions[ROM_CLORDID].iov_base,
                doj->positions[ROM_CLORDID].iov_len);
        pthread_spin_unlock(&ci->mutex);
        set_rom_field(doj, ROM_LEAVES, "0", 1);
        set_rom_field(doj, ROM_LAST_SHARES, "0", 1);
        if(doj->sender != 0x0 && ((con_int*)doj->sender)->on_out != 0x0) {
            ((con_int*)doj->sender)->on_out(doj->sender, cnt);
        } else {
            con_int* sender = 0;
            get_obj_by_client_id(ci->gk, cnt->owner,cnt->olen, (void**)&sender);        
            if(sender && sender->on_out) {
                sender->on_out(sender, cnt);
            } else if(sender) {
                log_n_out(sender, cnt, ci);
            }
        }
    } else {
        if(doj->sender != 0x0 && ((con_int*)doj->sender)->on_update) {
            ((con_int*)doj->sender)->on_update(doj->sender, cnt);
        } else {
            con_int* sender = 0;
            get_obj_by_client_id(ci->gk, cnt->owner,cnt->olen, (void**)&sender);        
            if(sender && sender->on_update) {
                sender->on_update(sender, cnt);
            } else if(sender) {
                log_n_go(sender, cnt, ci);
            }
        }
    }
    destroy_gk_db(ci->gk, db);   
}
static void handle_eur_out(eurexp* ep, con_int* ci,
        DeleteOrderResponseT* dort)
{
    databuf_t* db = get_databuf(ci->gk);
    int len = no_set_unsigned_itoa(db->wr_ptr, dort->ClOrdID);
    pthread_spin_lock(&ci->mutex);
    /*START LOCK*/
    rcntr* cnt = 0;
    int ret = get_map_entry(ci->cl_ord_id_map, db->rd_ptr, len, (void**)&cnt);
    /*END LOCK*/
    pthread_spin_unlock(&ci->mutex);
    if(ret) {
        //do nothing
    } else {
        databuf_reset(db);
        len = no_set_unsigned_itoa(db->wr_ptr, dort->OrigClOrdID);
        pthread_spin_lock(&ci->mutex);
        int ret = get_map_entry(ci->cl_ord_id_map, db->rd_ptr, len, (void**)&cnt);
        pthread_spin_unlock(&ci->mutex);
        if(ret) {
            //do nothing
        } else {
            send_debug_message(ci->o,"Could not find: %ld,orig: %ld,leave:%ld,cum: %ld.\n",
                    dort->ClOrdID, dort->OrigClOrdID, dort->CxlQty,
                    dort->CumQty);
            return;
        }
    }
    dart_order_obj* doj = cnt->raw;
    set_rom_field(doj, 0, "S",1);
    databuf_reset(db);
    int tlen = eur_to_rom_time(dort->ResponseHeaderME.SendingTime, db); 
    set_rom_field(doj, EXEC_TIME, db->rd_ptr, tlen);
    databuf_reset(db);
    u64_dart_itoa(doj, dort->ResponseHeaderME.SendingTime, ROM_DISP_INSTR, db);
    databuf_reset(db);
    set_rom_field(doj, ROM_STATUS, dort->ExecType, 1);
    if(dort->ExecType[0] == '4') {
        pthread_spin_lock(&ci->mutex);
        remove_from_map(ci->cl_ord_id_map,  doj->positions[ROM_CLORDID].iov_base,
                doj->positions[ROM_CLORDID].iov_len);
        pthread_spin_unlock(&ci->mutex);
        set_rom_field(doj, ROM_LEAVES, "0", 1);
        set_rom_field(doj, ROM_LAST_SHARES, "0", 1);
        if(doj->sender != 0x0 && ((con_int*)doj->sender)->on_out != 0x0) {
            ((con_int*)doj->sender)->on_out(doj->sender, cnt);
        } else {
            con_int* sender = 0;
            get_obj_by_client_id(ci->gk, cnt->owner,cnt->olen, (void**)&sender);        
            if(sender && sender->on_out != 0x0) {
                sender->on_out(sender, cnt);
            } else if(sender) {
                log_n_out(sender, cnt, ci);
            }
        }
    } else {
        if(doj->sender != 0x0 && ((con_int*)doj->sender)->on_update) {
            ((con_int*)doj->sender)->on_update(doj->sender, cnt);
        } else {
            con_int* sender = 0;
            get_obj_by_client_id(ci->gk, cnt->owner,cnt->olen, (void**)&sender);        
            if(sender && sender->on_update) {
                sender->on_update(sender, cnt);
            } else if(sender) {
                log_n_go(sender, cnt, ci);
            }
        }
    }
    destroy_gk_db(ci->gk, db);   
}
static int compare_seq(dart_order_obj* doj, RejectT* rej)
{
    char c_seq[32];
    memset(c_seq, '\0', 32);
    memcpy(c_seq, doj->positions[ROM_LEG_MARK].iov_base, 
            doj->positions[ROM_LEG_MARK].iov_len);
    uint32_t sseq = atoi(c_seq);
    return sseq == rej->NRResponseHeaderME.MsgSeqNum;
}
static void handle_eur_reject(eurexp* ep, con_int* ci, RejectT* rej)
{
    databuf_t* db = get_databuf(ci->gk);
    send_debug_message(ci->o, "Received Reject code: %u, %s \n",
            rej->SessionRejectReason, rej->VarText);
    pthread_spin_lock(&ci->mutex);
    /*START LOCK*/
    rcntr* cnt = 0;
    hashmap_iterator_t hi = hashmap_iterator_begin(ci->cl_ord_id_map);
    dart_order_obj *current = 0;
    char *key = 0;
    int key_len = 0;
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                (void *) &cnt);
    while (valid && cnt) {
        if(cnt->raw) {
            con_int* ct = 0;
            current = (dart_order_obj*)cnt->raw;
            if(compare_seq(current, rej)) {
                if (current->sender != NULL) {
                    ct = (con_int*) current->sender;
                    set_rom_field(current, 0, "S", 1);
                    if(current->positions[ROM_STATUS].iov_len == 2) {
                        if(strncmp("26",
                                    current->positions[ROM_STATUS].iov_base, 2) == 0) {
                            set_rom_field(current, ROM_STATUS, "14", 2);
                        } else if(strncmp("27",
                                    current->positions[ROM_STATUS].iov_base, 2) == 0) {
                            set_rom_field(current, ROM_STATUS, "20", 2);
                        } else {
                            set_rom_field(current, ROM_STATUS, "8", 1);
                        }
                    } else {
                        set_rom_field(current, ROM_STATUS, "8", 1);
                    }
                    databuf_reset(db);
                    int tlen = eur_to_rom_time(rej->NRResponseHeaderME.SendingTime, db); 
                    set_rom_field(current, EXEC_TIME, db->rd_ptr, tlen);
                    databuf_reset(db);
                    u64_dart_itoa(current, rej->NRResponseHeaderME.SendingTime, ROM_DISP_INSTR, db);
                    databuf_reset(db);
                    clean_text(current, db, ROM_TEXT, rej->VarText, rej->VarTextLen);
                    if(ct->on_rej) {
                        ct->on_rej(ct, cnt);
                    } else {
                        log_n_rej(ct, cnt, ci);
                    }
                    break;
                }
            }
        }
        //unlock(current);
        valid = hashmap_iterator_next(ci->cl_ord_id_map, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                    &key_len, (void *) &cnt);
    }
    hashmap_iterator_destroy(hi);
    /*END LOCK*/
    pthread_spin_unlock(&ci->mutex);
    destroy_gk_db(ci->gk, db);
}
static void handle_eur_replace_resp(eurexp* ep, con_int* ci,
        ModifyOrderResponseT* mort)
{
    databuf_t* db = get_databuf(ci->gk);
    int len = no_set_unsigned_itoa(db->wr_ptr, mort->ClOrdID);
    pthread_spin_lock(&ci->mutex);
    /*START LOCK*/
    rcntr* cnt = 0;
    int ret = get_map_entry(ci->cl_ord_id_map, db->rd_ptr, len, (void**)&cnt);
    /*END LOCK*/
    pthread_spin_unlock(&ci->mutex);
    if(ret) {
        //do nothing
    } else {
        databuf_reset(db);
        len = no_set_unsigned_itoa(db->wr_ptr, mort->OrigClOrdID);
        pthread_spin_lock(&ci->mutex);
        int ret = get_map_entry(ci->cl_ord_id_map, db->rd_ptr, len, (void**)&cnt);
        pthread_spin_unlock(&ci->mutex);
        if(ret) {
            //do nothing
        } else {
            send_debug_message(ci->o,"Could not find: %ld,orig: %ld,leave:%ld,cum: %ld.\n",
                    mort->ClOrdID, mort->OrigClOrdID, mort->LeavesQty,
                    mort->CumQty);
            return;
        }
    }
    databuf_reset(db);
    dart_order_obj* doj = cnt->raw;
    dart_order_obj* rep = doj->rep;
    set_rom_field(doj, 0, "S",1);
    if(rep->positions[ROM_PRICE].iov_len > 0) {
        set_rom_field(doj, ROM_PRICE, rep->positions[ROM_PRICE].iov_base, 
                rep->positions[ROM_PRICE].iov_len); 
        cnt->price = rep->risk.price;
    }
    if(rep->positions[ROM_SHARES].iov_len > 0) {
        set_rom_field(doj, ROM_SHARES, rep->positions[ROM_SHARES].iov_base, 
                rep->positions[ROM_SHARES].iov_len); 
        cnt->size = rep->risk.size;
    }
    trans_eur_to_rom_qty(doj,ROM_LEAVES, 
            mort->LeavesQty, db);
    databuf_reset(db);
    trans_eur_to_rom_qty(doj, ROM_CUM,
            mort->CumQty, db);
    databuf_reset(db);
    len = dart_itoa(doj, 
            mort->OrderID, ROM_EX_TAG,db);
    databuf_reset(db);
    fill_in_rom_field(doj, ROM_STATUS, "5", 1);
    databuf_reset(db);
    int tlen = eur_to_rom_time(mort->ResponseHeaderME.SendingTime, db); 
    set_rom_field(doj, EXEC_TIME, db->rd_ptr, tlen);
    databuf_reset(db);
    u64_dart_itoa(doj, mort->ResponseHeaderME.SendingTime, ROM_DISP_INSTR, db);
    databuf_reset(db);
    if(mort->OrdStatus[0] == '4') {
        fill_in_rom_field(doj, ROM_STATUS, "4", 1);
        pthread_spin_lock(&ci->mutex);
        remove_from_map(ci->cl_ord_id_map,  doj->positions[ROM_CLORDID].iov_base,
                doj->positions[ROM_CLORDID].iov_len);
        pthread_spin_unlock(&ci->mutex);
        if(doj->sender != 0x0 && ((con_int*)doj->sender)->on_out) {
            ((con_int*)doj->sender)->on_out(doj->sender, cnt);
        } else {
            con_int* sender = 0;
            get_obj_by_client_id(ci->gk, cnt->owner,cnt->olen, (void**)&sender);        
            if(sender && sender->on_out) {
                sender->on_out(sender, cnt);
            } else if(sender) {
                log_n_out(sender, cnt, ci);
            }
        }
    } else {
        if(doj->sender != 0x0 && ((con_int*)doj->sender)->on_update) {
            ((con_int*)doj->sender)->on_update(doj->sender, cnt);
        } else {
            con_int* sender = 0;
            get_obj_by_client_id(ci->gk, cnt->owner,cnt->olen, (void**)&sender);        
            if(sender && sender->on_update) {
                sender->on_update(sender, cnt);
            } else if(sender) {
                log_n_go(sender, cnt, ci);
            }
        }
    }
    destroy_gk_db(ci->gk, db);   
}

static void send_back_imm_rep(eurexp* ep, rcntr* cnt, databuf_t* db, 
        OrderExecResponseT* oert, con_int* ci)
{
    databuf_reset(db);
    dart_order_obj* doj = cnt->raw;
    dart_order_obj* rep = doj->rep;
    set_rom_field(doj, 0, "S",1);
    if(rep->positions[ROM_PRICE].iov_len > 0) {
        set_rom_field(doj, ROM_PRICE, rep->positions[ROM_PRICE].iov_base, 
                rep->positions[ROM_PRICE].iov_len); 
        cnt->price = rep->risk.price;
    }
    if(rep->positions[ROM_SHARES].iov_len > 0) {
        set_rom_field(doj, ROM_SHARES, rep->positions[ROM_SHARES].iov_base, 
                rep->positions[ROM_SHARES].iov_len); 
        cnt->size = rep->risk.size;
    }
    trans_eur_to_rom_qty(doj, ROM_LEAVES,
            oert->LeavesQty, db);
    databuf_reset(db);
    trans_eur_to_rom_qty(doj,ROM_CUM, 
            oert->CumQty, db);
    databuf_reset(db);
    unsigned_dart_itoa(doj, 
            oert->ExecID, ROM_EX_TAG,db);
    databuf_reset(db);
    fill_in_rom_field(doj, ROM_STATUS, "5", 1);
    databuf_reset(db);
    if(doj->sender != 0x0 && ((con_int*)doj->sender)->on_update) {
        ((con_int*)doj->sender)->on_update(doj->sender, cnt);
    } else {
        con_int* sender = 0;
        get_obj_by_client_id(ci->gk, cnt->owner,cnt->olen, (void**)&sender);        
        if(sender && sender->on_update) {
            sender->on_update(sender, cnt);
        } else if(sender) {
            log_n_go(sender, cnt, ci);
        }
    }
}

static int iterate_eur_imm_fills(eurexp* ep, con_int* ci,
        OrderExecResponseT* oert, dart_order_obj* doj, rcntr* cnt,
        databuf_t* db)
{
    int removed = 0;
    int i = 0;
    for(;i < oert->NoFills;++i) {
        databuf_reset(db);
        int len = eur_to_rom_price(oert->FillsGrp[i].FillPx, db);
        set_rom_field(doj, ROM_EXEC_PRICE, db->rd_ptr, len); 
        databuf_reset(db);
        trans_eur_to_rom_qty(doj, ROM_LAST_SHARES,
                oert->FillsGrp[i].FillQty, db);
        databuf_reset(db);
        len = dart_itoa(doj,
                oert->FillsGrp[i].FillExecID,EXEC_ID, db);
        databuf_reset(db);
        len = unsigned_dart_itoa(doj, 
                oert->FillsGrp[i].FillMatchID,ROM_TEXT, db);
        databuf_reset(db);
        len = unsigned_dart_itoa(doj, 
                oert->FillsGrp[i].FillLiquidityInd, ROM_LIQ,db);

        cnt->last = oert->FillsGrp[i].FillQty / 10000;
        set_rom_field(doj, ROM_STATUS, "1", 1);
        if(i == (oert->NoFills - 1)) {
            databuf_reset(db);
            cnt->cum = oert->CumQty / 10000;
            trans_eur_to_rom_qty(doj, ROM_LEAVES,oert->LeavesQty,  db);
            databuf_reset(db);
            trans_eur_to_rom_qty(doj, ROM_CUM,oert->CumQty,  db);
            set_rom_field(doj, ROM_STATUS, oert->OrdStatus, 1);
            if(oert->OrdStatus[0] == '2' && (cnt->size - cnt->cum) == 0) {
                pthread_spin_lock(&ci->mutex);
                remove_from_map(ci->cl_ord_id_map, doj->positions[ROM_CLORDID].iov_base,
                        doj->positions[ROM_CLORDID].iov_len);
                pthread_spin_unlock(&ci->mutex);
                removed = 1;
            }
        }
        send_back_fill(ci, doj, cnt);
    }
    return removed;
}
static void handle_eur_imm_exec(eurexp* ep, con_int* ci,
        OrderExecResponseT* oert)
{
    databuf_t* db = get_databuf(ci->gk);
    int len = no_set_unsigned_itoa(db->wr_ptr, oert->ClOrdID);
    pthread_spin_lock(&ci->mutex);
    /*START LOCK*/
    rcntr* cnt = 0;
    int ret = get_map_entry(ci->cl_ord_id_map, db->rd_ptr, len, (void**)&cnt);
    /*END LOCK*/
    pthread_spin_unlock(&ci->mutex);
    if(ret) {
        //do nothing
    } else {
        databuf_reset(db);
        len = no_set_unsigned_itoa(db->wr_ptr, oert->OrigClOrdID);
        pthread_spin_lock(&ci->mutex);
        int ret = get_map_entry(ci->cl_ord_id_map, db->rd_ptr, len, (void**)&cnt);
        pthread_spin_unlock(&ci->mutex);
        if(ret) {
            //do nothing
        } else {
            send_debug_message(ci->o,"Could not find: %ld,orig: %ld,leave:%ld,cum: %ld.\n",
                    oert->ClOrdID, oert->OrigClOrdID, oert->LeavesQty,
                    oert->CumQty);
            return;
        }
    }
    dart_order_obj* doj = cnt->raw;
    set_rom_field(doj, 0, "S",1);
    databuf_reset(db);
    len = unsigned_dart_itoa(doj, 
            oert->OrderID, ROM_EX_TAG,db);
    databuf_reset(db);
    int tlen = eur_to_rom_time(oert->ResponseHeaderME.SendingTime, db); 
    set_rom_field(doj, EXEC_TIME, db->rd_ptr, tlen);
    databuf_reset(db);
    u64_dart_itoa(doj, oert->ResponseHeaderME.SendingTime, ROM_DISP_INSTR, db);
    databuf_reset(db);
    if(oert->ExecRestatementReason == ENUM_EXEC_RESTATEMENT_REASON_ORDER_MODIFIED) {
        send_back_imm_rep(ep, cnt, db, oert, ci);
    }
    if(oert->NoLegExecs > 0) {
        send_debug_message(ci->o,"CMPLX exe: %ld,status: %s,leave:%ld,cum: %ld.\n",
                oert->ClOrdID, oert->OrdStatus, oert->LeavesQty,
                oert->CumQty);
        int sdlen = doj->positions[ROM_SEC_DEF].iov_len;
        char* temp_sec_def = calloc(1, sdlen);
        memcpy(temp_sec_def, doj->positions[ROM_SEC_DEF].iov_base, sdlen);
        if(oert->NoFills > 1) {
            set_rom_field(doj, ROM_STATUS, "1", 1);
        } else {
            set_rom_field(doj, ROM_STATUS, oert->OrdStatus, 1);
        }
        record_leg_executions(oert->NoLegExecs,
                oert->FillsGrp, oert->NoFills, ep, ci, doj, db, cnt);
        set_rom_field(doj, ROM_SEC_DEF, temp_sec_def, sdlen);
        set_rom_field(doj, ROM_CPLX_TYPE, "5", 1);
        int kill = iterate_eur_imm_fills(ep, ci, oert, doj, cnt,db);
        if(kill) {
            eurex_destroy_cmplx_ord(cnt, ci);
        }
    } else {
        iterate_eur_imm_fills(ep, ci, oert, doj, cnt,db);
    }
    destroy_gk_db(ci->gk, db);   
}

static int check_not_affect(dart_order_obj* doj, DeleteAllOrderBroadcastT* daob)
{
    if(doj->positions[ROM_CLORDID].iov_len <= 0) {
        return 0;
    }
    int ret = 0;
    char sordid[64];
    memset(&sordid, '\0', 64);
    memcpy(&sordid, doj->positions[ROM_CLORDID].iov_base, doj->positions[ROM_CLORDID].iov_len);
    uint64_t clord = atol(sordid);
    int i = 0;
    for(;i < daob->NoNotAffectedOrders;++i) {
        if(daob->NotAffectedOrdersGrp[i].NotAffOrigClOrdID == clord) {
            return 1;
        }
    }
    return ret;
}

static int check_persistent(dart_order_obj* doj)
{
    int ret = 0;
    if(doj->positions[ROM_EXEC_INST].iov_len > 0) {
        char* val = doj->positions[ROM_EXEC_INST].iov_base;
        if(val[0] == '1' || val[0] == '5') {
            ret = 1;
        }
    }
    return ret;
}

static int check_segment_n_intr(dart_order_obj* doj,int32_t marketSeg, int64_t sec_id)
{
    int send_cancel = 0;
    int plen = doj->positions[ROM_PROD_CODE].iov_len;
    if(plen > 0) {
        char cseg[64];
        memset(&cseg, '\0',64);
        memcpy(&cseg, doj->positions[ROM_PROD_CODE].iov_base,
                doj->positions[ROM_PROD_CODE].iov_len);
        int32_t lseg = atol(cseg);
        if(lseg == marketSeg) {
            send_cancel = 1;
        } else {
            return 0;
        }
    }
    if(sec_id != NO_VALUE_SLONG) {
        char csec[64];
        memset(&csec, '\0', 64);
        memcpy(&csec, doj->positions[ROM_SEC_DEF].iov_base,
                doj->positions[ROM_SEC_DEF].iov_len);
        int64_t lsec = atol(csec);
        if(lsec == sec_id) {
            send_cancel = 1;
        } else {
            return 0;
        }
    } else {
        send_cancel = 1;
    }
    return send_cancel;
}

static void remove_the_mass_canceled(eurexp* ep, con_int* ci, queue_t delete_list)
{
    element_t e, temp;
    e = dequeue(delete_list);
    while(e != NULL) {
        databuf_t* db = (databuf_t*)e->d;
        pthread_spin_lock(&ci->mutex);
        remove_from_map(ci->cl_ord_id_map, db->rd_ptr,databuf_unread(db));
        pthread_spin_unlock(&ci->mutex);
        destroy_gk_db(ci->gk, db);
        temp = e;
        e = temp->next;
        free_element(delete_list, temp);
    }
}

static void handle_eur_mass_cancel(eurexp* ep, con_int* ci, DeleteAllOrderBroadcastT* daob)
{
    send_debug_message(ci->o,
            "Received a Mass Cancel Notification, reason: %d, execinst: %d, side: %d, Segment: %d, secid: %ld \n",
            daob->MassActionReason, daob->ExecInst, daob->Side,
            daob->MarketSegmentID, daob->SecurityID);
    databuf_t* db = get_databuf(ci->gk);
    queue_t delete_list = create_sized_queue(64);
    rcntr* cnt = 0;
    hashmap_iterator_t hi = hashmap_iterator_begin(ci->cl_ord_id_map);
    char *key = 0;
    int key_len = 0;
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                (void *) &cnt);
    while (valid && cnt ) {
        if(cnt->raw == 0x0) {
            goto KEEP_ON_CHUGGING;
        }
        dart_order_obj* doj = (dart_order_obj*)cnt->raw;
        set_rom_field(doj, 0, "S",1);
        if(check_not_affect(doj, daob)) {
            goto KEEP_ON_CHUGGING;
        } else {
            if(daob->ExecInst == 2 &&
                    check_persistent(doj)) {
                goto KEEP_ON_CHUGGING;
            } 
            if(check_segment_n_intr(doj,daob->MarketSegmentID, daob->SecurityID)) {
                char* status = doj->positions[ROM_STATUS].iov_base;
                int slen = doj->positions[ROM_STATUS].iov_len; 
                if(slen == 1) {
                    if(status[0] == '2' || status[0] == '4' || status[0] == '8') {
                        goto KEEP_ON_CHUGGING;
                    }
                }
                uint8_t side = get_8bit_for_rom(ROM_SIDE, doj);
                if(daob->Side != NO_VALUE_UINT && daob->Side != side) {
                    goto KEEP_ON_CHUGGING;
                } 
                set_rom_field(doj, ROM_STATUS, "4",1);
                databuf_t* clord = get_databuf(ci->gk);
                databuf_memcpy(clord, doj->positions[ROM_CLORDID].iov_base,
                        doj->positions[ROM_CLORDID].iov_len);
                enqueue(delete_list, clord);
                if(doj->sender != 0x0 && ((con_int*)doj->sender)->on_out) {
                    ((con_int*)doj->sender)->on_out(doj->sender, cnt);
                } else {
                    con_int* sender = 0;
                    get_obj_by_client_id(ci->gk, cnt->owner,cnt->olen, (void**)&sender);        
                    if(sender && sender->on_out) {
                        sender->on_out(sender, cnt);
                    } else if(sender) {
                        log_n_out(sender, cnt, ci);
                    }
                }
            }
        }
KEEP_ON_CHUGGING:
        //on_eurex_cancel_order(ci, cnt);
        valid = hashmap_iterator_next(ci->cl_ord_id_map, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                    &key_len, (void *) &cnt);
    }
    hashmap_iterator_destroy(hi);
    remove_the_mass_canceled(ep, ci, delete_list);
    destroy_queue(delete_list);
    destroy_gk_db(ci->gk, db);
}

static void handle_eur_mass_cancel_event(eurexp* ep, con_int* ci,
        DeleteAllOrderQuoteEventBroadcastT* daoqebt)
{
    send_debug_message(ci->o,
            "Received a Mass Cancel Event, reason: %d, execinst: %d, Segment: %d, secid: %ld \n",
            daoqebt->MassActionReason, daoqebt->ExecInst,
            daoqebt->MarketSegmentID, daoqebt->SecurityID);
    databuf_t* db = get_databuf(ci->gk);
    queue_t delete_list = create_sized_queue(64);
    rcntr* cnt = 0;
    hashmap_iterator_t hi = hashmap_iterator_begin(ci->cl_ord_id_map);
    char *key = 0;
    int key_len = 0;
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                (void *) &cnt);
    while (valid && cnt) {
        if(cnt->raw == 0x0) {
            goto SKIP_KILL;
        }
        dart_order_obj* doj = (dart_order_obj*)cnt->raw;
        set_rom_field(doj, 0, "S",1);
        if(daoqebt->ExecInst == 2 &&
                check_persistent(doj)) {
            goto SKIP_KILL;
        } else if(daoqebt->ExecInst == 1 &&
                !check_persistent(doj)) {
            goto SKIP_KILL;
        }
        if(check_segment_n_intr(doj,daoqebt->MarketSegmentID, daoqebt->SecurityID)) {
            char* status = doj->positions[ROM_STATUS].iov_base;
            int slen = doj->positions[ROM_STATUS].iov_len; 
            if(slen == 1) {
                if(status[0] == '2' || status[0] == '4' || status[0] == '8') {
                    goto SKIP_KILL;
                }
            }
            set_rom_field(doj, ROM_STATUS, "4",1);
            databuf_t* clord = get_databuf(ci->gk);
            databuf_memcpy(clord, doj->positions[ROM_CLORDID].iov_base,
                    doj->positions[ROM_CLORDID].iov_len);
            enqueue(delete_list, clord);
            if(doj->sender != 0x0 && ((con_int*)doj->sender)->on_out) {
                ((con_int*)doj->sender)->on_out(doj->sender, cnt);
            } else {
                con_int* sender = 0;
                get_obj_by_client_id(ci->gk, cnt->owner,cnt->olen, (void**)&sender);        
                if(sender && sender->on_out) {
                    sender->on_out(sender, cnt);
                } else if(sender) {
                    log_n_out(sender, cnt, ci);
                }
            }
        }
SKIP_KILL:
        valid = hashmap_iterator_next(ci->cl_ord_id_map, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                    &key_len, (void *) &cnt);
    }
    hashmap_iterator_destroy(hi);
    remove_the_mass_canceled(ep, ci, delete_list);
    destroy_queue(delete_list);
    destroy_gk_db(ci->gk, db);
}
static void send_eur_hb(void * v)
{
    eurexp *fco = (eurexp *) v;
    if(fco->is_logged_on) {
        time_t now = 0;
        time(&now);
        char buff[256];
        read(fco->hb_timer->sock,buff, 256); 
        long diff = now - fco->last_sent;
        if(diff >= 30) {
            /*SEND HB*/
            HeartbeatT hb;
            hb.MessageHeaderIn.BodyLen = sizeof(HeartbeatT);//16;
            hb.MessageHeaderIn.TemplateID = TID_HEARTBEAT;
            eur_direct_send(fco, &hb, sizeof(HeartbeatT));
        }
        diff = diff >= 30 ? 30 : 30 - diff;
        add_back_rep_timer_adjust(fco->rc->p,fco->hb_timer,diff);
    }
}
void start_eur_hb_timer(eurexp* fp)
{
    fp->hb_timer =  add_timer(fp->rc->p,30,
            0, 1,0, fp,send_eur_hb);
}


static void log_on_user(eurexp* ep, char* password, int ret_len, uint32_t userid)
{
    UserLoginRequestT ulr;
    memset(&ulr, '\0', sizeof(ulr));
    ulr.MessageHeaderIn.BodyLen = sizeof(ulr);
    ulr.MessageHeaderIn.TemplateID = TID_USER_LOGIN_REQUEST;                           
    ep->seq_num++;
    ulr.RequestHeader.MsgSeqNum = ep->seq_num;
    ulr.Username = userid;//ep->user_logon_id;
    /*int ret_len = 0;
    char* password = get_config_val(ci->ex, "SERVER", 6,
            "user pass", 9, &ret_len);*/
    memcpy(ulr.Password, password, ret_len);
    eur_direct_send(ep, &ulr, sizeof(ulr));
}
static void log_on_users(eurexp* ep, con_int* ci)
{
    int* rlf;
    char *key = 0;
    int key_len = 0;
    hashmap_iterator_t hi = hashmap_iterator_begin(ep->user_pass);
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                                     (void *) &rlf);
    while (valid) {
        uint32_t userid = *rlf;
        key_len = strlen(key);
        log_on_user(ep, key, key_len, userid);
        send_debug_message(ci->o, "Eurex Sent logon: %u, pass: %s, len: %d \n", userid, key, key_len);
        valid = hashmap_iterator_next(ep->user_pass, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &rlf);
    }
    hashmap_iterator_destroy(hi);
}
static void check_logon_resp(eurexp* pc, rex_con* rc, con_int* ci,
        LogonResponseT* lrt, int lrt_size)
{
    pc->is_logged_on = 1;
    send_debug_message(ci->o,
            "Received Logon Response from EUREX, Version: %s, Mode: %u \n",
            lrt->DefaultCstmApplVerID, lrt->TradSesMode);
    send_debug_message(ci->o, "Eurex SessionInstanceID: %u \n",
            lrt->SessionInstanceID);
    send_debug_message(ci->o,
            "Eurex Throttle Time Interval: %d \n", lrt->ThrottleTimeInterval);
    send_debug_message(ci->o, "Eurex Throttle Message limit: %u \n",
            lrt->ThrottleNoMsgs);
    send_debug_message(ci->o, "Eurex Throttle Disconnect Limit: %u \n", 
            lrt->ThrottleDisconnectLimit);
    send_debug_message(ci->o, "Eurex HB interval: %u \n", lrt->HeartBtInt);
    start_eur_hb_timer(pc);
    log_on_users(pc, ci);
}

static void parse_eurex(int *cut_con, rex_con *rc)
{
    eurexp* pc = (eurexp*)rc->parser;
    char* d_off = rc->con->rd_ptr;
    long len = (rc->con->wr_ptr - rc->con->rd_ptr);
    unsigned long bytes_used = 0;
    long len_left = (long) len;
    con_int*ob = (con_int*) pc->pbp;
    do {
        if(sizeof(MessageHeaderOutCompT) <= len_left) {
            MessageHeaderOutCompT* mgo = (void*)d_off;
            uint32_t bl = mgo->BodyLen;
            if(bl <= len_left) {
                switch(mgo->TemplateID) {
                    case TID_LOGON_RESPONSE:
                        check_logon_resp(pc, rc, ob,
                                (LogonResponseT*)d_off, sizeof(LogonResponseT));
                        rc->connection_notice(pc, 1, NULL);
                        break;
                    case TID_HEARTBEAT_NOTIFICATION:
                        send_eur_hb(pc);
                        break;
                    case TID_FORCED_USER_LOGOUT_NOTIFICATION:
                        {
                            ForcedUserLogoutNotificationT* fulnt = (ForcedUserLogoutNotificationT*)d_off; 
                            send_debug_message(ob->o,
                                    "Eurex User Logged OUT: %u user: %u %s \n",
                                    mgo->TemplateID, fulnt->Username, fulnt->VarText);
                            break; 
                        }
                    case TID_LOGOUT_RESPONSE:
                    case TID_FORCED_LOGOUT_NOTIFICATION:
                        send_debug_message(ob->o,
                                "Eurex User/session Logged OUT: %u \n",
                                mgo->TemplateID);

                        iterate_logouts(pc);//send_eur_logout(pc);
                        ob->can_send_orders = 0;
                        ob->connected = 0;
                        *cut_con = 1;
                        break;
                    case TID_USER_LOGIN_RESPONSE:
                        ob->can_send_orders = 1;
                        ob->connected = 1;
                        send_debug_message(ob->o,
                                "Eurex User Logged In: %u \n",bl);
                               // pc->user_logon_id);
                        ob->update_oats(ob, ob->connected);
                        break;
                    case TID_NEW_ORDER_RESPONSE:
                        handle_eur_open(pc, ob,
                                (NewOrderResponseT*)d_off);
                        break;
                    case TID_ORDER_EXEC_RESPONSE:
                        handle_eur_imm_exec(pc, ob, 
                                (OrderExecResponseT*)d_off);
                        break; 
                    case TID_ORDER_EXEC_NOTIFICATION:
                        handle_eur_book_exec(pc, ob,
                                (OrderExecNotificationT*)d_off);
                        break;
                    case TID_ORDER_EXEC_REPORT_BROADCAST:
                        handle_eur_book_broad(pc, ob,
                                (OrderExecReportBroadcastT*)d_off);
                        break;  
                    case TID_MODIFY_ORDER_RESPONSE:
                        handle_eur_replace_resp(pc, ob,
                                (ModifyOrderResponseT*)d_off);
                        break;  
                    case TID_DELETE_ORDER_RESPONSE: 
                        handle_eur_out(pc, ob,
                                (DeleteOrderResponseT*)d_off);
                        break;
                    case TID_DELETE_ORDER_BROADCAST:
                        handle_eur_bc_out(pc, ob,
                                (DeleteOrderBroadcastT*)d_off);
                        break; 
                    case TID_REJECT:
                        handle_eur_reject(pc, ob,(RejectT*)d_off);
                        break; 
                    case TID_DELETE_ALL_ORDER_BROADCAST:
                        handle_eur_mass_cancel(pc, ob, (DeleteAllOrderBroadcastT*)d_off);
                        break; 
                    case TID_DELETE_ALL_ORDER_QUOTE_EVENT_BROADCAST:
                        handle_eur_mass_cancel_event(pc,ob,
                                (DeleteAllOrderQuoteEventBroadcastT*)d_off);
                        break;
                    default:
                        break; 
                };
                len_left -= bl;
                bytes_used += bl;
                d_off += bl; 
            } else {
                goto EUR_PARSE_DONE;
            }
        } else {
            goto EUR_PARSE_DONE;
        }
    } while(bytes_used < len);
EUR_PARSE_DONE:
    rc->con->rd_ptr += bytes_used;
}
void create_connection(con_int * ci, char *filename, int name_len,
        gatekeeper * gk)
{
    ci->cl_ord_id_map = get_initial_order_map(gk, ci->ex->dest_id,
            ci->ex->dest_id_len);
    ci->on_enter_order = on_eurex_enter_order;
    ci->on_replace_order = on_eurex_replace_order;
    ci->on_cancel_order = on_eurex_cancel_order;
    ci->cleansing_rinse = eurex_cleansing_rinse;
    ci->on_cancel_all = eurex_cancel_all_orders;
    ci->can_send_orders = 0;
    add_int_orderids(ci);
    login_info* li = build_logon_info_from_config(ci->ex);
    eurexp* uparse = create_eurex_parser(ci->o, ci, li);
    rex_con* rc = create_rex_con(ci->p, ci->ex, uparse, uparse,li,
            eurex_write_callback, 
            eurex_on_connect,
            parse_eurex, NULL);
    rc->parser = uparse;
    rc->connect = send_eurex_session_logon;
    uparse->rc= rc;
    ci->rc = rc;
    rex_connect(ci->p, rc);
}
