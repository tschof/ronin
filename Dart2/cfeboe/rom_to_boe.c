/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <sys/fcntl.h>

#include "rom_to_boe.h"
#include "parser_func.h"
#include "rex_constants.h"
#include "dart_order_obj.h"
#include "service_bureau.h"
#include "rom_fix_trans_funcs.h"
#include "log_mnger.h"
#include "boe_utils.h"
#include "boe_master.h"
#include "tm_to_str.h"

static unsigned int null_int = 0x0;
static int codes[26] = {
    0,0,0,0,0,
    1,1,1,0,1,
    1,0,1,1,0,
    0,1,0,0,0,
    1,1,0,1,0,1
};

static char get_capacity_for_clr_acc(boem* b, char* clr, int clrlen)
{
    char code = 'C';
    int cti_len = 0;
    get_mpid_for_clr_acct(b->sbm, clr, clrlen, &cti_len);
    if(cti_len > 0) {
        code = 'C';
    }
    return code;
}

static char get_cti_code_for_clr_acc(boem* b, char* clr,int clrlen)
{
    char code = '2';
    int cti_len = 0;
    get_mpid_for_clr_acct(b->sbm, clr, clrlen, &cti_len);
    if(cti_len > 0) {
        code = '4';
    }
    return code;
}
static char* get_client_sym(boem* b, char* exch_sym, int exech_symlen,
        int* symlen)
{
    return get_mpid_for_clr_acct(b->prod_codes, exch_sym,
            exech_symlen, symlen);
}

static char* get_cfe_sym(boem* b, char* sym, int symlen, int* retlen)
{
    return get_mpid_for_clr_acct(b->rom_to_cfe_simp, sym,
            symlen, retlen);
}

int rom_to_boe_price(char *price, size_t len)
{
    double d = strtod(price, NULL);
    d = d * 10000;
    if(d > 0) {
        d += .00001;
    } else {
        d -= .00001;
    }
    return (int) d;
}

int boe_to_rom_time(long boe_time, char* buff)
{
    time_t epoch_time = boe_time/1000000000;
    long nanos = boe_time % epoch_time;
    nanos /= 1000; //We are only going to display micros.
    struct tm lm;
    gmtime_r(&epoch_time, &lm);
    return sprintf(buff, "%04d%02d%02d-%02d:%02d:%02d.%06lu",
            (lm.tm_year + 1900), (lm.tm_mon + 1), lm.tm_mday,
            lm.tm_hour, lm.tm_min, lm.tm_sec, nanos);
}
static void set_rom_transact_time_stamp(dart_order_obj* doj)
{
    char offset[64];
    struct timeb rawtime;
    ftime(&rawtime);
    struct tm lm;
    gmtime_r(&rawtime.time, &lm);
    tm_to_str_millis(&lm, rawtime.millitm, offset,
            TIMESTAMP_LEN_WITH_MILLIS);
    fill_in_rom_field(doj, EXEC_TIME, offset,
        TIMESTAMP_LEN_WITH_MILLIS);
}

int find_cfe_base_sym(const char* symbol, int len)
{
    int end = len -1;
    if(symbol[end] >= 0x30 && symbol[end] <= 0x39) {
        while((symbol[end] >= 0x30 && symbol[end] <= 0x39)) {
            --end;
        }
        if(end < 0) {
            return len;
        }
        if(codes[symbol[end] - 65]) {
            --end;
        }
        while(symbol[end] >= 0x30 && symbol[end] <= 0x39) {
            --end;
        }
        return end +1;
    } else {
        return len;
    }
}
int find_cfe_base_spread_sym(const char* symbol, int len)
{
    char* dash = memchr(symbol,'-',len);
    if(dash) {
        int nlen = (dash - symbol);
        return find_cfe_base_sym(symbol, nlen);
    } else {
        if(len > 8) {
            len = 8;
        }
        return len;
    }
}

/**
 * For BATS we  translate one ROM field in to possibly three BOE fields.
 * The format for a ROM INSTR field for BATS goes like this:
 * XXYYY-ZZZZZ Where the X fields refer to the RoutingInst, this can be one
 * possibly 2 characters long(the only eligible second characters are 'D' and 'L'.
 * The Y chars refer to the RouteDelivery method, this can be absent but if present
 * it must be three characters long and either RTI or RTF.
 * Next we demand a dash before the Z's which mark the begining of the RoutStrategy.
 * We do not validate any of these fields and require the exchange to reject.
 */
static void trans_extra_routes(databuf_t* buff, dart_order_obj* rph,
        boeno* ord)
{
    char* inst = getpval(rph, ROM_INSTR);
    int full_len = getplen(rph, ROM_INSTR);
    if(inst[1] == 'D' || inst[1] == 'L') {
        inst += 2;
        if(inst[0] == '-') {
            SET_BIT(0x1, 0x4, ord->bf6); /* We have a route strategy */
            inst++;
            full_len -= 3;
            databuf_memcpy(buff, inst, full_len);
            buff->wr_ptr += (6 - full_len);
        } else if(full_len >= 9) {
            char* rdeliv = inst;
            inst += 4;
            full_len -= 6; 
            SET_BIT(0x1, 0x4, ord->bf6); /* We have a route strategy */
            databuf_memcpy(buff, inst, full_len);
            buff->wr_ptr += (6 - full_len);
            SET_BIT(0x1, 0x8, ord->bf6); /* We have a route delivery method */
            databuf_memcpy(buff, rdeliv, 3);
        }
    } else if(inst[1] == '-') {
        inst += 1; 
        full_len -= 2;
        SET_BIT(0x1, 0x4, ord->bf6); /* We have a route strategy */
        databuf_memcpy(buff, inst, full_len);
        buff->wr_ptr += (6 - full_len);
    } else {
        char* dash = memchr(inst, '-', full_len);
        if(dash) {
            dash += 1;
            SET_BIT(0x1, 0x4, ord->bf6); /* We have a route strategy */
            int strat_len = full_len - (dash  - inst); 
            databuf_memcpy(buff, dash, strat_len);
            buff->wr_ptr += (6 - strat_len);
            SET_BIT(0x1, 0x8, ord->bf6); /* We have a route delivery method */
            inst += 1;
            databuf_memcpy(buff, inst, 3);
        } else {
            SET_BIT(0x1, 0x4, ord->bf6); /* We have a route strategy */
            databuf_memcpy(buff, inst, full_len);
            buff->wr_ptr += (6 - full_len);
        }
    }
}

int trans_mat_ymd(char* year_mon, int ymlen, char* day, int dlen)
{
    char packy[16];
    memset(packy, '\0', 16);
    memcpy(packy, year_mon, ymlen);
    if(dlen == 2) {
        memcpy(packy + ymlen, day, 2);
    } else {
        memcpy(packy + ymlen, "0", 1);
        memcpy(packy + ymlen + 1, day, 1);
    }
    return atoi(packy);
}
static char* trans_ord_type(char* rom_type, int rtlen , char* ex_inst, int ex_len)
{
    if(rtlen == 1 && ex_len <= 0) {
        if(rom_type[0] == '5') {
            return "1";
        }
        return rom_type;
    } else if(ex_len > 0) {
        switch(ex_inst[0]) {
            case 'L':
            case 'M':
            case 'm':
            case 'P':
            case 'Q':
            case 'R':
            case 'U':
                return "P";
                break;
            default:
                return rom_type;
        }
    } else {
        if(rtlen == 2 && (strncmp("11", rom_type, 2) == 0) ) {
            return "2";
        }
        if(rtlen == 2 && ((strncmp("12", rom_type, 2) == 0)||
                    (strncmp("10", rom_type, 2) == 0))) {
            return "1";
        }
        return rom_type;
    }
}

void switch_order_ids(dart_order_obj* doj, con_int* ci, rcntr* cnt)
{
    pthread_spin_lock(&ci->mutex);
    remove_from_map(ci->cl_ord_id_map, doj->positions[ROM_CLORDID].iov_base,
                doj->positions[ROM_CLORDID].iov_len);
    pthread_spin_unlock(&ci->mutex);
    int pendlen = 0;
    char* new_id = get_rom_field(doj, ROM_PENDING_ID, &pendlen);
    set_rom_field(doj, ROM_CLORDID, new_id, pendlen);
}
static char* trans_ord_tif(char* rom_type, int rtlen, char* ex_inst, int ex_len,
        char* rom_tif, int tif_len)
{
    if(rtlen >= 2) {
        if(rtlen == 2 && ((strncmp("11", rom_type, 2) == 0) ||
                    (strncmp("10", rom_type, 2) == 0))) {
            return "7";
        }
        if(rtlen == 2 && strncmp("12", rom_type, 2) == 0) {
            return "2";
        }
    }
    if(tif_len > 0) {
        if(rom_tif[0] == '7') {
            return "5";
        } 
        return rom_tif;
    } else {
        return "0";
    }
}
static void set_operator_and_man(databuf_t* buff, con_int* ob,
        dart_order_obj* rph)
{
    /*In the DART world N is yes to manual, not present or Y is automated.*/
    char* man = getpval(rph, ROM_MANUAL);
    if(getplen(rph, ROM_MANUAL) > 0 && man[0] == 'Y') {
        databuf_memcpy(buff, "N", 1);
    } else {
        databuf_memcpy(buff, "Y", 1);
    }
    int operator_len  = getplen(rph, ROM_TRADE_FOR);
    databuf_memcpy(buff, getpval(rph, ROM_TRADE_FOR), 
            operator_len);
    buff->wr_ptr += (18 - operator_len);
}
static void set_clearing_firm(databuf_t* buff, con_int* ob,
        dart_order_obj* rph)
{
    int mpid_len = 0;
    char* on_behalf = get_mpid_for_clr_acct(ob->sbm,
            getpval(rph, ROM_CLR_ID),
            getplen(rph, ROM_CLR_ID),
            &mpid_len);
    if(mpid_len == 4) {
        databuf_memcpy(buff, on_behalf,mpid_len);
    } else {
        mpid_len = getplen(rph, ROM_CLR_ID);
        databuf_memcpy(buff, getpval(rph, ROM_CLR_ID),mpid_len);
        if(mpid_len < 4) {
            buff->wr_ptr += (4 - mpid_len);
        }
    }
}

int create_boe_new_order_message(databuf_t * buff, rcntr *v,
        con_int * ob, token_gen* tg, uint64_t seq)
{
    memset(buff->buffer, 0, 256);
    boeno* ord  =  (boeno*)buff->buffer;
    buff->wr_ptr += sizeof(struct boe_new_ord);
    ord->type = 0x38;
    dart_order_obj *rph = (dart_order_obj *) v->raw;
    boem* b = (boem*)ob->cparser;
    ord->som = 0xBABA;
    ord->seq = 0x0;
    int ord_id_len = create_multiday_in_place_ord_token(tg,
            ord->ord_id, 20);
    memcpy(&ord->side, getpval(rph, ROM_SIDE), 1);
    unsigned int net_qty =
        strtol(rph->positions[ROM_SHARES].iov_base, NULL, 10);
    set_rom_field(rph, ROM_BIN_QTY, &net_qty, sizeof(int));
    set_rom_field(rph, ROM_BIN_CUM, &null_int, sizeof(int));
    set_rom_field(rph, ROM_LEAVES, rph->positions[ROM_SHARES].iov_base,
            rph->positions[ROM_SHARES].iov_len);
    ord->qty = net_qty;
    ord->nnobfields = 7; /*We always have 7 bit fields some may be 0*/
    SET_BIT(0x1, 0x1, ord->bf1); /*All new orders have clearing firm*/
    set_clearing_firm(buff, ob, rph);
    int mpid_len = 0;
    SET_BIT(0x1, 0x2, ord->bf1); /*All new orders have the clearing account*/
    char* clearing_account = getpval(rph, ROM_CLR_ACC);
    mpid_len = getplen(rph, ROM_CLR_ACC);
    if(mpid_len >= 7 && strncmp("CRON", clearing_account, 4) == 0) {
        int off = mpid_len > 7 ? 4 : 3;
        clearing_account += off;
        databuf_memcpy(buff, clearing_account,4);/*We only copy 4 anyway.*/
    } else {
        databuf_memcpy(buff, clearing_account,4);/*We only copy 4 anyway.*/
    }
    if(getplen(rph, ROM_PRICE) > 0) {
        SET_BIT(0x1, 0x4, ord->bf1); /* We have a price*/
        if(getplen(rph, ROM_BIN_PRICE) <= 0) {
            unsigned long np = rom_to_boe_price(rph->positions[ROM_PRICE].iov_base,
                    rph->positions[ROM_PRICE].iov_len);
            set_rom_field(rph, ROM_BIN_PRICE, &np, sizeof(long));
            databuf_memcpy(buff, (char *) &np, sizeof(long));
        } else  {
            long np = 0;
            memcpy(&np, getpval(rph, ROM_BIN_PRICE), getplen(rph, ROM_BIN_PRICE));
            databuf_memcpy(buff,(char*)&np, sizeof(long));
        }
    }
    if(getplen(rph, ROM_EXEC_INST) > 0) {
        char* exec = getpval(rph, ROM_EXEC_INST);
        if(exec[0] == 'f') {
            return -1;/*Intermarket sweeps are not allowed*/
        }
        SET_BIT(0x1, 0x8, ord->bf1); /* We have execution instructions*/
        databuf_memcpy(buff, exec, 1);
    }

    char* symbol = 0;
    int sym_len = 0;
    if(getplen(rph, ROM_SEC_DEF) > 0) {
        SET_BIT(0x1, 0x1, ord->bf2); /* We have a Symbol*/
        symbol = getpval(rph, ROM_SEC_DEF);
        sym_len = getplen(rph, ROM_SEC_DEF);
    } else if(getplen(rph, ROM_SYM) > 0) {
        SET_BIT(0x1, 0x1, ord->bf2); /* We have a Symbol*/
        symbol = getpval(rph, ROM_SYM);
        sym_len = getplen(rph, ROM_SYM);
    } else {
        return -1;
    }
    SET_BIT(0x1, 0x10, ord->bf1); /* DART requires an order type*/
    databuf_memcpy(buff, trans_ord_type(getpval(rph, ROM_TYPE), getplen(rph, ROM_TYPE),
                getpval(rph, ROM_EXEC_INST), getplen(rph, ROM_EXEC_INST)), 1);
    int tif_len = getplen(rph, ROM_TIF);
    if(tif_len > 0) {
        SET_BIT(0x1, 0x20, ord->bf1); /* We have a time in force*/
        char* tif = rph->positions[ROM_TIF].iov_base;
        if(sym_len >= 3 && strncmp(symbol, "VXT", 3) == 0 &&
                (tif[0] == '1' && tif_len == 1)) {
            databuf_memcpy(buff, "0", 1);
        } else {
            databuf_memcpy(buff, trans_ord_tif(getpval(rph, ROM_TYPE),
                        getplen(rph, ROM_TYPE),
                        getpval(rph, ROM_EXEC_INST),
                        getplen(rph, ROM_EXEC_INST),
                        tif,tif_len), 1);
        }
    } else {
       return -1;
    } 
    if(getplen(rph, ROM_MIN_QTY) > 0) {
        SET_BIT(0x1, 0x40, ord->bf1); /* We have a Min qty*/

        char ccmta[32];
        memset(ccmta, '\0', 32);
        memcpy(ccmta, rph->positions[ROM_MIN_QTY].iov_base, 
                rph->positions[ROM_MIN_QTY].iov_len);
        unsigned int min = atoi(ccmta);
        databuf_memcpy(buff, &min, 4);
    }
    char* exch_sym = symbol;
    int retlen = sym_len;
    if(!is_spread(rph)) {
        if(sym_len >= 3 && strncmp(symbol, "VXT", 3) == 0) {
            set_rom_field(rph, ROM_SPREAD, "5", 1);
            send_debug_message(ob->o, "just set spread field: %d\n",
                            rph->positions[ROM_CPLX_TYPE].iov_len);
        }
       
    }
    exch_sym = get_cfe_sym(b, symbol, sym_len, &retlen);
    if(retlen <= 0) {
        exch_sym = symbol;
        retlen = sym_len <= 8 ? sym_len : 8;
    }
    databuf_memcpy(buff, exch_sym, retlen);
    if(retlen < 8) {
        buff->wr_ptr += (8 - retlen);
    }
    SET_BIT(0x1, 0x40, ord->bf2); /* We have a capacity*/
    char cap = get_capacity_for_clr_acc(b,
            getpval(rph, ROM_CLR_ID),
            getplen(rph, ROM_CLR_ID));
    databuf_memcpy(buff, &cap, 1);
    if(getplen(rph, ROM_CLR_ACC) > 0) {
        SET_BIT(0x1, 0x1, ord->bf3); /*We have an account*/
        int acc_len = getplen(rph, ROM_CLR_ACC);
        databuf_memcpy(buff, clearing_account, acc_len);
        buff->wr_ptr += (16 - acc_len);
    }
    if(getplen(rph, ROM_DESC) == 3) {
        SET_BIT(0x1, 0x20, ord->bf3); /*We have a prevent match code*/
        databuf_memcpy(buff, getpval(rph, ROM_DESC), 3);
    }
    if(rph->positions[ROM_O_C].iov_len > 0) {
        SET_BIT(0x1, 0x10, ord->bf4); /*We have an open close */
        char* oc = (char*)rph->positions[ROM_O_C].iov_base;
        if(oc[0] == '1') {
            databuf_memcpy(buff, "O", 1);
        } else if(oc[0] == 'N') {
            databuf_memcpy(buff, "N", 1);
        } else {
            databuf_memcpy(buff, "C", 1);
        }
    }
    if(rph->positions[ROM_CMTA].iov_len > 0) {
        SET_BIT(0x1, 0x20, ord->bf4); /*We have a cmta */
        char ccmta[32];
        memset(ccmta, '\0', 32);
        memcpy(ccmta, rph->positions[ROM_CMTA].iov_base, 
                rph->positions[ROM_CMTA].iov_len);
        int icmta = atoi(ccmta);
        databuf_memcpy(buff, &icmta, 4);
    }
    if(getplen(rph, ROM_STOP) > 0) {
        SET_BIT(0x1, 0x2, ord->bf6); /*We have a stop price */
        unsigned long np = rom_to_boe_price(rph->positions[ROM_STOP].iov_base,
                rph->positions[ROM_STOP].iov_len);
        databuf_memcpy(buff, &np, sizeof(long));
    }
    if(getplen(rph, ROM_INSTR) > 2) {
        /*This check is for the ROUTE strategy and delivery fields*/
        trans_extra_routes(buff, rph, ord);
    }
    SET_BIT(0x1, 0x20, ord->bf7);
    char cti = get_cti_code_for_clr_acc(b,
            getpval(rph, ROM_CLR_ID),
            getplen(rph, ROM_CLR_ID));
    databuf_memcpy(buff, &cti, 1);
    SET_BIT(0x1, 0x40, ord->bf7);
    SET_BIT(0x1, 0x80, ord->bf7);
    set_operator_and_man(buff, ob,rph);
    ord->len = databuf_unread(buff) - 2;    
    /*Since ROM_CLORDID and instructions are the same field we need to wait until 
     * we have used the instructions before we set the id.*/
    set_rom_field(rph, ROM_CLORDID, ord->ord_id, ord_id_len);    
    no_copy_insert(ob->cl_ord_id_map, getpval(rph,ROM_CLORDID),
            getplen(rph,ROM_CLORDID),v);
    set_rom_transact_time_stamp(rph);
    return databuf_unread(buff);
}
int create_boe_cancel_message(databuf_t * buff, rcntr *v,
        con_int * ob, uint64_t seq)
{
    memset(buff->buffer, 0, 256);
    boecnl* ord  =  (boecnl*)buff->buffer;
    buff->wr_ptr += sizeof(struct boe_cnl_ord);
    ord->type = 0x39;
    dart_order_obj *rph = (dart_order_obj *) v->raw;
    ord->som = 0xBABA;
    ord->seq = 0x0;
    memcpy(ord->orig_id, rph->positions[ROM_CLORDID].iov_base,
            rph->positions[ROM_CLORDID].iov_len);   
    ord->ncnlbfields = 1;
    SET_BIT(0x1, 0x1, ord->bf1); /*All new orders have clearing firm*/
    set_clearing_firm(buff, ob, rph);
    /*In the DART world N is yes to manual, not present or Y is automated.*/
    SET_BIT(0x1, 0x40, ord->bf1);
    SET_BIT(0x1, 0x80, ord->bf1);
    set_operator_and_man(buff, ob,rph);
    set_rom_transact_time_stamp(rph);
    ord->len = databuf_unread(buff) - 2; 
    return databuf_unread(buff);
}
int create_boe_replace_message(databuf_t * buff, rcntr *v,
        con_int * ob, token_gen* tg, uint64_t seq)
{
    memset(buff->buffer, 0, 256);
    boerpl* ord  =  (boerpl*)buff->buffer;
    buff->wr_ptr += sizeof(struct boe_rpl_ord);
    //boem* b = (boem*)ob->cparser;
    ord->type = 0x3A;
    dart_order_obj *rph = (dart_order_obj *) v->raw;
    ord->som = 0xBABA;
    ord->seq = 0x0;
    memcpy(ord->orig_id, rph->positions[ROM_CLORDID].iov_base,
            rph->positions[ROM_CLORDID].iov_len);   
    int ord_id_len = create_multiday_in_place_ord_token(tg,
            ord->ord_id, 20);
    set_rom_field(rph, ROM_PENDING_ID, ord->ord_id, ord_id_len);    
    no_copy_insert(ob->cl_ord_id_map, getpval(rph,ROM_PENDING_ID),
            getplen(rph,ROM_PENDING_ID),v);
    ord->nrplbfields = 2;
    /* Since we have a service bureau ports we need a clearing firm.*/
    SET_BIT(0x1, 0x1, ord->bf1);
    set_clearing_firm(buff, ob, rph);
    /* All replaces MUST have a price and qty*/
    SET_BIT(0x1, 0x4, ord->bf1);
    SET_BIT(0x1, 0x8, ord->bf1);
    dart_order_obj *rep = (dart_order_obj *) rph->rep;
    if(rep->positions[ROM_SHARES].iov_len > 0 &&
            rep->positions[ROM_BIN_QTY].iov_len <= 0) {
        unsigned int net_qty =
            strtol(rep->positions[ROM_SHARES].iov_base, NULL, 10);
        set_rom_field(rep, ROM_BIN_QTY, &net_qty, sizeof(int));
        databuf_memcpy(buff, getpval(rep, ROM_BIN_QTY), getplen(rep, ROM_BIN_QTY));
    } else if(getplen(rep, ROM_BIN_QTY) > 0) {
        databuf_memcpy(buff, getpval(rep, ROM_BIN_QTY), getplen(rep, ROM_BIN_QTY));
    } else {
        databuf_memcpy(buff, getpval(rph, ROM_BIN_QTY), getplen(rph, ROM_BIN_QTY));
    }
    if(rep->positions[ROM_PRICE].iov_len > 0 && rep->positions[ROM_BIN_PRICE].iov_len <= 0) {
        unsigned long np = rom_to_boe_price(rep->positions[ROM_PRICE].iov_base,
                rep->positions[ROM_PRICE].iov_len);
        set_rom_field(rep, ROM_BIN_PRICE, &np, sizeof(long));
        databuf_memcpy(buff, (char *) &np, sizeof(long));
    } else if(getplen(rep, ROM_BIN_PRICE) > 0) {
        unsigned long np = 0;
        memcpy(&np, rep->positions[ROM_BIN_PRICE].iov_base,
                rep->positions[ROM_BIN_PRICE].iov_len);
        databuf_memcpy(buff, (char *) &np, sizeof(long));
    } else {
        unsigned long np = 0;
        memcpy(&np, rph->positions[ROM_BIN_PRICE].iov_base,
                rph->positions[ROM_BIN_PRICE].iov_len);
        databuf_memcpy(buff, (char *) &np, sizeof(long));
    }
    if(rep->positions[ROM_TYPE].iov_len > 0) {
        if(rph->positions[ROM_TYPE].iov_len == rep->positions[ROM_TYPE].iov_len) {
            char* orig_type = getpval(rph, ROM_TYPE);
            char* rep_type = getpval(rep, ROM_TYPE);
            if(strncmp(orig_type, rep_type, getplen(rep, ROM_TYPE)) != 0) {
                SET_BIT(0x1, 0x10, ord->bf1);/*Type changed*/
                databuf_memcpy(buff, trans_ord_type(getpval(rep, ROM_TYPE),
                               getplen(rep, ROM_TYPE),
                               getpval(rep, ROM_EXEC_INST),
                               getplen(rep, ROM_EXEC_INST)), 1);
            }
        } else {
            SET_BIT(0x1, 0x10, ord->bf1);/*Type changed*/
            databuf_memcpy(buff, trans_ord_type(getpval(rep, ROM_TYPE),
                           getplen(rep, ROM_TYPE),
                           getpval(rep, ROM_EXEC_INST),
                           getplen(rep, ROM_EXEC_INST)), 1);
        }

    }
    if(getplen(rep, ROM_EXEC_INST) > 0) {
        char* exec = getpval(rep, ROM_EXEC_INST);
        if(exec[0] == 'f') {
            return -1;/*Intermarket sweeps are not allowed*/
        }
        SET_BIT(0x1, 0x40, ord->bf1); /* We have execution instructions*/
        databuf_memcpy(buff, exec, 1);
    }
    if(getplen(rep, ROM_SIDE) > 0) {
        if(getplen(rph, ROM_SIDE) == getplen(rep, ROM_SIDE)) {
            char* orig_side = getpval(rph, ROM_SIDE);
            char* rep_side = getpval(rep, ROM_SIDE);
            if(strncmp(orig_side, rep_side, getplen(rep, ROM_SIDE)) != 0) {
                SET_BIT(0x1, 0x80, ord->bf1); /* We have a new side */
                databuf_memcpy(buff, getpval(rep, ROM_SIDE), 1);
            }
        } else {
            SET_BIT(0x1, 0x80, ord->bf1); /* We have a new side */
            databuf_memcpy(buff, getpval(rep, ROM_SIDE), 1);
        }
    }
    if(getplen(rep, ROM_MAX_FLOOR) > 0) {
        SET_BIT(0x1, 0x1, ord->bf2); /* We have a Max Floor*/
        unsigned int max = atoi(getpval(rep, ROM_MAX_FLOOR));
        databuf_memcpy(buff, &max, 4);
    }
    if(getplen(rep, ROM_STOP) > 0) {
        SET_BIT(0x1, 0x2, ord->bf2); /*We have a stop price */
        unsigned long np = rom_to_boe_price(rep->positions[ROM_STOP].iov_base,
                rep->positions[ROM_STOP].iov_len);
        databuf_memcpy(buff, &np, sizeof(long));
    }
    /*In the DART world N is yes to manual, not present or Y is automated.*/
    SET_BIT(0x1, 0x8, ord->bf2);
    SET_BIT(0x1, 0x10, ord->bf2);
    set_operator_and_man(buff, ob,rph);
    set_rom_transact_time_stamp(rph);
    ord->len = databuf_unread(buff) - 2; 
    return databuf_unread(buff);
}

static void set_time(dart_order_obj* ute, char* mess, con_int* ob) 
{
    long tt = 0;
    memcpy(&tt, (mess + 10), 8);
    char itoa_buff[64];
    int date_len = boe_to_rom_time(tt, itoa_buff);
    fill_in_rom_field(ute, EXEC_TIME, itoa_buff, date_len);
}

static void set_ordid(dart_order_obj* ute, char* mess, con_int* ob)
{
    long bordid = 0;
    memcpy(&bordid, mess + 38, 8);
    databuf_t *itoa_buff = get_databuf(ob->gk);
    dart_itoa(ute, bordid,ROM_EX_TAG, itoa_buff);
    destroy_gk_db(ob->gk, itoa_buff);
}
int boe_parse_order_ack(rcntr * cnt, char *ack, con_int * ob)
{
    dart_order_obj* ute = (dart_order_obj*)cnt->raw;
    set_time(ute, ack, ob);
    set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    set_rom_field(ute, ROM_STATUS, "13", 2);
    set_ordid(ute, ack, ob);
    return 1;
}
int boe_parse_cancel_rej(rcntr * cnt, char *ack, con_int * ob)
{
    dart_order_obj* ute = (dart_order_obj*)cnt->raw;
    set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    set_rom_field(ute, ROM_STATUS, "14", 2);
    set_time(ute, ack, ob);
    char* text = ack + 39;
    int tlen = 0;
    FIND_NULL(text, tlen, 60);
    fill_in_rom_field(ute, ROM_TEXT, text, tlen);
    return 1;
}
int boe_parse_ur_out(rcntr * cnt, char *ack, con_int * ob)
{
    int ret = 0;
    dart_order_obj* ute = (dart_order_obj*)cnt->raw;
    set_time(ute, ack, ob);
    set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    set_rom_field(ute, ROM_STATUS, "4", 1);
    pthread_spin_lock(&ob->mutex);
    if(is_spread(ute)) {
        set_rom_field(ute, ROM_SPREAD, "5", 1);
    } else {
        ret = 1;
        remove_from_map(ob->cl_ord_id_map,
                    ute->positions[ROM_CLORDID].iov_base,
                    ute->positions[ROM_CLORDID].iov_len);
    }
    pthread_spin_unlock(&ob->mutex);
    return ret;
}
int boe_parse_rep_rej(rcntr * cnt, char *ack, con_int * ob)
{
    dart_order_obj* ute = (dart_order_obj*)cnt->raw;
    set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    set_rom_field(ute, ROM_STATUS, "20", 2);
    set_time(ute, ack, ob);
    pthread_spin_lock(&ob->mutex);
    remove_from_map(ob->cl_ord_id_map,
                    ute->positions[ROM_PENDING_ID].iov_base,
                    ute->positions[ROM_PENDING_ID].iov_len);
    pthread_spin_unlock(&ob->mutex);
    if(is_spread(ute)) {
        set_rom_field(ute, ROM_SPREAD, "5", 1);
    }
    char* text = ack + 39;
    int tlen = 0;
    FIND_NULL(text, tlen, 60);
    fill_in_rom_field(ute, ROM_TEXT, text, tlen);
    return 1;
}
int boe_parse_replaced(rcntr * cnt, char *ack, con_int * ob)
{
    dart_order_obj* ute = (dart_order_obj*)cnt->raw;
    int ret_val = 0;
    set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    set_rom_field(ute, ROM_STATUS, "5", 1);
    set_time(ute, ack, ob);
    set_ordid(ute, ack, ob);

    char num_bit_fields = *(ack + 47);
    char bit_one = *(ack + 48);
    char bit_three = *(ack + 50);
    char bit_eight = *(ack + 55);
    unsigned int is_present = 0;
    if(num_bit_fields < 8) {
        send_debug_message(ob->o, "We received fewer than 8 bit fields on a replace: %d\n",
                            num_bit_fields);
    }
    switch_order_ids(ute, ob, cnt);
    dart_order_obj *rep = (dart_order_obj *) ute->rep;
    databuf_t *buff = get_databuf(ob->gk);
    if(is_spread(ute)) {
        set_rom_field(ute, ROM_SPREAD, "5", 1);
    }
    IS_BIT_SET(is_present, bit_three, 6);
    if(rep && is_present) {
        if(rep->positions[ROM_SHARES].iov_len > 0) {
            fill_in_rom_field(ute, ROM_SHARES, rep->positions[ROM_SHARES].iov_base,
                              rep->positions[ROM_SHARES].iov_len);
            int l_q = strtol(rep->positions[ROM_SHARES].iov_base, NULL, 10);
            memcpy(ute->positions[ROM_BIN_QTY].iov_base, &l_q, 4);
            //setting leaves
            int l_cum = 0;
            memcpy(&l_cum, ute->positions[ROM_BIN_CUM].iov_base, 4);
            int leaves = l_q - l_cum;
            dart_itoa(ute, leaves, ROM_LEAVES, buff);
            //End leaves
            cnt->size = rep->risk.size;
        }
    } else {
        send_debug_message(ob->o, "We did not see the share bit three as set: %#010x\n",
                            bit_three);
    }
    IS_BIT_SET(is_present, bit_one, 2);
    if(rep && is_present) {
        if(rep->positions[ROM_BIN_PRICE].iov_len > 0) { 
            memcpy(ute->positions[ROM_BIN_PRICE].iov_base,
                   rep->positions[ROM_BIN_PRICE].iov_base, getplen(rep, ROM_BIN_PRICE));
        }
        if(rep->positions[ROM_PRICE].iov_len > 0) {
            fill_in_rom_field(ute, ROM_PRICE, rep->positions[ROM_PRICE].iov_base,
                              rep->positions[ROM_PRICE].iov_len);
        }
        cnt->price = rep->risk.price;
    } else {
        send_debug_message(ob->o, "We did not see the share bit one as set: %#010x\n",
                            bit_one);
    }

    if(rep) {
        IS_BIT_SET(is_present, bit_one, 0x0);
        if(is_present && getplen(rep, ROM_SIDE) > 0) {
            fill_in_rom_field(ute, ROM_SIDE, getpval(rep, ROM_SIDE), 
                              getplen(rep, ROM_SIDE));
        }
        IS_BIT_SET(is_present, bit_one, 0x1);
        if(is_present && getplen(rep, ROM_PEG_DIFF) > 0) {
            fill_in_rom_field(ute, ROM_PEG_DIFF, getpval(rep, ROM_PEG_DIFF), 
                              getplen(rep, ROM_PEG_DIFF));
        }
        IS_BIT_SET(is_present, bit_one, 0x3);
        if(is_present && getplen(rep, ROM_EXEC_INST) > 0) {
            fill_in_rom_field(ute, ROM_EXEC_INST, getpval(rep, ROM_EXEC_INST), 
                              getplen(rep, ROM_EXEC_INST));
        }
        IS_BIT_SET(is_present, bit_one, 5);
        if(is_present && getplen(rep, ROM_TIF) > 0) {
            fill_in_rom_field(ute, ROM_TIF, getpval(rep, ROM_TIF),
                              getplen(rep, ROM_TIF));
        }
        IS_BIT_SET(is_present, bit_one, 6);
        if(is_present && getplen(rep, ROM_MIN_QTY) > 0) {
            fill_in_rom_field(ute, ROM_MIN_QTY, getpval(rep, ROM_MIN_QTY),
                              getplen(rep, ROM_MIN_QTY));
        }
        IS_BIT_SET(is_present, bit_three, 0x3);
        if(is_present && getplen(rep, ROM_DISP_INSTR) > 0) {
            fill_in_rom_field(ute, ROM_DISP_INSTR, getpval(rep, ROM_DISP_INSTR),
                              getplen(rep, ROM_DISP_INSTR));
        }
        IS_BIT_SET(is_present, bit_three, 4);
        if(is_present && getplen(rep, ROM_MAX_FLOOR) > 0) {
            fill_in_rom_field(ute, ROM_MAX_FLOOR, getpval(rep, ROM_MAX_FLOOR),
                              getplen(rep, ROM_MAX_FLOOR));
        }
        IS_BIT_SET(is_present, bit_eight, 0x2);
        if(is_present && getplen(rep, ROM_STOP) > 0) {
            fill_in_rom_field(ute, ROM_STOP, getpval(rep, ROM_STOP),
                              getplen(rep, ROM_STOP));
        }
        unlock(rep);
    }
    destroy_gk_db(ob->gk, buff);
    return ret_val;
}
int boe_parse_rej(rcntr * cnt, char *ack, con_int * ob)
{
    dart_order_obj* ute = (dart_order_obj*)cnt->raw;
    int ret_val = 0;
    set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    set_rom_field(ute, ROM_STATUS, "8", 1);
    set_time(ute, ack, ob);
    char* text = ack + 39;
    int text_len = 0;
    FIND_NULL(text, text_len, 60);
    fill_in_rom_field(ute, ROM_TEXT, text, text_len);
    return ret_val;
}
unsigned int boe_set_ex_id(dart_order_obj * doj, unsigned long value,
                                int rom_tag,  databuf_t* db)
{
    char *str = db->buffer;
    int radix = 10;
    static char dig[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    unsigned int n = 0;
    unsigned long v = 0;
    char *p, *q;
    char c;
    v = value;
    do {
        str[n++] = dig[v % radix];
        v /= radix;
    } while (v);
    for (p = str, q = p + (n - 1); p < q; ++p, --q) {
        c = *p;
        *p = *q;
        *q = c;
    }
    fill_in_rom_field(doj, rom_tag, db->buffer,n);
    return n;
}

void boe_parse_tas_restate(rcntr* cnt, char* ack, con_int* ob)
{
    dart_order_obj* ute = (dart_order_obj*)cnt->raw;
    boem* b = (boem*)ob->cparser;
    uint8_t num_bit_fields = *(ack + 47);
    uint32_t is_present = 0;
    set_time(ute, ack, ob);
    set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    set_rom_field(ute, ROM_SPREAD, "6", 1);
    set_rom_field(ute, ROM_STATUS, "2", 1);
    databuf_t *buff = get_databuf(ob->gk);
    long execid = 0;
    memcpy(&execid, ack + 38, 8);
    boe_set_ex_id(ute, execid, EXEC_ID, buff);
    databuf_reset(buff);
    char* offset = ack + (47 + num_bit_fields + 1);
    uint8_t bit_four = *(ack + 51);
    uint8_t bit_one = *(ack + 48);
    IS_BIT_SET(is_present, bit_one, 0x0);
    if(is_present) {
        fill_in_rom_field(ute, ROM_SIDE, offset, 1);
        offset += 1;
    }
    IS_BIT_SET(is_present, bit_four, 0x0);
    if(is_present) {
        uint32_t date = 0x0;
        memcpy(&date, offset, 4);
        uint32_t day = date % 100;
        uint32_t yearmon = (date - day)/100;
        send_debug_message(ob->o, "TAS num_bit_fields? %d date: %d, YYYYMM: %d, DD: %d\n",
            num_bit_fields, date, yearmon, day);

        unsigned_dart_itoa(ute, yearmon, ROM_EXPIRE_DATE, buff);
        databuf_reset(buff);
        unsigned_dart_itoa(ute, day, ROM_EXPIRE_DAY, buff);
        databuf_reset(buff);
        offset += 4;
    }
    uint8_t cur_bit = *(ack + 52);
    /*Byte one is at ack + 48 bit byte five is 52*/
    IS_BIT_SET(is_present, cur_bit, 0x2);
    send_debug_message(ob->o, "TAS restate num_bit_fields? %d what about cur bit: %d do we have shares: %d\n",
            num_bit_fields, cur_bit, is_present);
    if(is_present) {
        uint32_t last_shares = 0x0;
        memcpy(&last_shares, offset, 4);
        unsigned_dart_itoa(ute, last_shares,
                                ROM_LAST_SHARES, buff);
        offset += 4;
    }
    cur_bit = *(ack + 59);
    IS_BIT_SET(is_present, cur_bit, 0x4);
    send_debug_message(ob->o, "TAS restate num_bit_fields? %d what about cur bit: %d do we have price: %d\n",
            num_bit_fields, cur_bit, is_present);
    if(is_present) {
        int64_t price = 0x0;
        memcpy(&price, offset, 8);
        char buffer[64];
        memset(buffer, '\0', 64);
        offset += 8;
        double dp = (double)price;
        double lp = (dp / 10000.0) + .0000001;
        int dlen = sprintf(buffer, "%f", lp);
        fill_in_rom_field(ute, ROM_EXEC_PRICE, buffer, dlen);
    }
    IS_BIT_SET(is_present, cur_bit, 0x6);
    send_debug_message(ob->o, "TAS restate num_bit_fields? %d what about cur bit: %d do we have sym: %d\n",
            num_bit_fields, cur_bit, is_present);
    if(is_present) {
        char* text = offset;
        int text_len = 0;
        FIND_NULL(text, text_len, 8);
            int symlen = 0;
            char* tempsym = get_client_sym(b, text, text_len,
                    &symlen);
            if(symlen > 0) {
                fill_in_rom_field(ute, ROM_SEC_DEF, tempsym, symlen);
                fill_in_rom_field(ute, ROM_SYM, tempsym, symlen);
            } else {
                fill_in_rom_field(ute, ROM_SEC_DEF, text, text_len);
            }
    }
}
int boe_parse_filled(rcntr * cnt, char *ack, con_int * ob)
{
    dart_order_obj* ute = (dart_order_obj*)cnt->raw;
    boem* b = (boem*)ob->cparser;
    char num_bit_fields = *(ack + 69);
    char bit_one = *(ack + 70);
    char bit_two = *(ack + 71);
    char bit_four = *(ack + 73);
    char bit_eight = *(ack + 77);
    char bit_thirteen = *(ack + 82);
    unsigned int is_present = 0;
    int ret_val = 0;
    set_time(ute, ack, ob);
    set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    databuf_t *buff = get_databuf(ob->gk);
    long execid = 0;
    memcpy(&execid, ack + 38, 8);
    boe_set_ex_id(ute, execid, EXEC_ID, buff);
    databuf_reset(buff);
    int last_shares = 0;
    memcpy(&last_shares, ack + 46, 4);
    int leaves_shares = 0;
    memcpy(&leaves_shares, ack + 58, 4);
    int l_cum = 0;
    int qty = 0;
    if(getplen(ute, ROM_BIN_QTY) > 0) {
    memcpy(&qty, ute->positions[ROM_BIN_QTY].iov_base, 4);
    }
    if(getplen(ute, ROM_BIN_CUM)) {
    memcpy(&l_cum, ute->positions[ROM_BIN_CUM].iov_base, 4);
    }
    fill_in_rom_field(ute, ROM_BIN_LAST_SHARES, &last_shares, sizeof(int));
    dart_itoa(ute, last_shares, ROM_LAST_SHARES, buff);
    databuf_reset(buff);
    dart_itoa(ute, leaves_shares, ROM_LEAVES, buff);
    databuf_reset(buff);

    long last_px = 0;//(long)*(ack + 50);
    memcpy(&last_px, ack + 50, 8);
    fill_in_rom_field(ute, ROM_BIN_LAST_PRICE, &last_px, sizeof(int));
    double lp = ((double) last_px / 10000.0) + .0000001;
    int dlen = sprintf(buff->buffer, "%f", lp);
    fill_in_rom_field(ute, ROM_EXEC_PRICE, buff->buffer, dlen);
    char* extras = ack + 69 + num_bit_fields + 1;
    IS_BIT_SET(is_present, bit_one, 0x0);
    if(is_present) {
        fill_in_rom_field(ute, ROM_SIDE, extras, 1);
        extras += 1;
    }
    IS_BIT_SET(is_present, bit_two, 0x0);
    if(is_present) {
        char* text = extras;
        int text_len = 0;
        FIND_NULL(text, text_len, 8);
        if(is_spread(ute)) {
            int symlen = 0;
            char* tempsym = get_client_sym(b, text, text_len,
                    &symlen);
            if(symlen > 0) {
                fill_in_rom_field(ute, ROM_SEC_DEF, tempsym, symlen);
                fill_in_rom_field(ute, ROM_SYM, tempsym, symlen);
            } else {
                fill_in_rom_field(ute, ROM_SEC_DEF, text, text_len);
            }
        } else {
            char ltxt[32];
            memset(ltxt, '\0', 32);
            memcpy(ltxt, text, text_len);
            send_debug_message(ob->o, "Execution num_bit_fields? %s \n",
                ltxt);
        }
        extras += 8;
    }
    IS_BIT_SET(is_present, bit_four, 0x0);
    if(is_present) {
        uint32_t date = 0x0;
        memcpy(&date, extras, 4);
        uint32_t day = date % 100;
        uint32_t yearmon = (date - day)/100;
        send_debug_message(ob->o, "Execution num_bit_fields? %d date: %d, YYYYMM: %d, DD: %d\n",
            num_bit_fields, date, yearmon, day);

        unsigned_dart_itoa(ute, yearmon, ROM_EXPIRE_DATE, buff);
        databuf_reset(buff);
        unsigned_dart_itoa(ute, day, ROM_EXPIRE_DAY, buff);
        databuf_reset(buff);
        extras += 4;
    }
    
    IS_BIT_SET(is_present, bit_eight, 0x0);
    if(is_present && num_bit_fields >= 8) {
        char* liq = extras;/*ack + 69 + num_bit_fields + 1; The next two bytes are the FeeCode*/
        if(*(liq + 1) == 0) {
            fill_in_rom_field(ute, ROM_LIQ, liq, 1);
        } else {
            fill_in_rom_field(ute, ROM_LIQ, liq, 2);
        }
    } else {
        char* liq = ack + 62; /*The next two bytes are the liq*/
        if(*(liq) == 0) {
            fill_in_rom_field(ute, ROM_LIQ, "U", 1);
        } else {
            fill_in_rom_field(ute, ROM_LIQ, liq, 2);
        }
    }
    extras += 2;
    IS_BIT_SET(is_present, bit_thirteen, 0x5);
    int is_pending = 0;
    if(is_present) {
        char pending = *(extras);
        if(pending == 'P') {/*TODO this sends the VXT orders into TPOS.*/
            set_rom_field(ute, ROM_SPREAD, "6", 1);
            is_pending = 6;
        }
        extras += 1;
    }
    IS_BIT_SET(is_present, bit_thirteen, 0x7);
    if(is_present) {
        char reptype = *(extras);
        if(reptype == '2') {
            set_rom_field(ute, ROM_SPREAD, "6", 1);
        } else if(reptype == '3') {
            set_rom_field(ute, ROM_SPREAD, "5", 1);
            l_cum += last_shares;
            fill_in_rom_field(ute, ROM_BIN_CUM, &l_cum, 4);
        } else {
            l_cum += last_shares;
            fill_in_rom_field(ute, ROM_BIN_CUM, &l_cum, 4);
        }
    } else {
        l_cum += last_shares;
        fill_in_rom_field(ute, ROM_BIN_CUM, &l_cum, 4);
    }
    if (l_cum >= qty) {
        set_rom_field(ute, ROM_STATUS, "2", 1);
        if(is_spread(ute)) {
            /*Do nothing we do not want to clean this order up.*/;
            send_debug_message(ob->o, "This is a spread we should send back 0: %d\n",
                            ute->positions[ROM_CPLX_TYPE].iov_len);
        } else {
            pthread_spin_lock(&ob->mutex);
            remove_from_map(ob->cl_ord_id_map,
                            ute->positions[ROM_CLORDID].iov_base,
                            ute->positions[ROM_CLORDID].iov_len);
            if(getplen(ute, ROM_PENDING_ID) > 0) {
                remove_from_map(ob->cl_ord_id_map,
                                ute->positions[ROM_PENDING_ID].iov_base,
                                ute->positions[ROM_PENDING_ID].iov_len);
            }
            pthread_spin_unlock(&ob->mutex);
            ret_val = 1;
        }
    } else {
        set_rom_field(ute, ROM_STATUS, "1", 1);
    }
    dart_itoa(ute, l_cum, ROM_CUM, buff);
    databuf_reset(buff);
    destroy_gk_db(ob->gk, buff);
    ret_val = is_pending > 0 ? is_pending: ret_val;
    return ret_val;
}

