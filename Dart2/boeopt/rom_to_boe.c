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

#include "rom_to_boe.h"
#include "parser_func.h"
#include "rex_constants.h"
#include "dart_order_obj.h"
#include "service_bureau.h"
#include "rom_fix_trans_funcs.h"
#include "log_mnger.h"
#include "boe_master.h"
#include "boe_utils.h"

#define LEAVE_MAX 10000000

#define ROM_BIN_LEAVES 176
#define ROM_PENDING_CNL 182
static unsigned int null_int = 0x0;
static int codes[26] = {
    0,0,0,0,0,
    1,1,1,0,1,
    1,0,1,1,0,
    0,1,0,0,0,
    1,1,0,1,0,1
};

long rom_to_boe_price(char *price, size_t len)
{
    double d = strtod(price, NULL);
    d = d * 10000;
    if(d > 0) {
        d += .00001;
    } else {
        d -= .00001;
    }
    return (long) d;
}
char get_capacity(
    char* cap, int caplen,
    char* destid, int dlen)
{
  char code = 'F';
  if(caplen > 0) {
    switch(cap[0]) {
      case 'X':
        code = 'U';
        break;
      case 'S':
        if(dlen == 3 && 
                (strncmp("183", destid, dlen) == 0 ||
                 strncmp("626", destid, dlen) == 0 ||
                 strncmp("226", destid, dlen) == 0)) {
            code = 'M';
        } else {
            code = 'N';
        }
        break;
      case 'B':
        code = 'J';
        break;
    }
  }
    return code;
}

char get_cti_code_for_clr_acc(boem* b, char* clr,int clrlen)
{
    char code = '2';
    int cti_len = 0;
    get_mpid_for_clr_acct(b->sbm, clr, clrlen, &cti_len);
    if(cti_len > 0) {
        code = '4';
    }
    return code;
}
char* get_client_sym(boem* b, char* exch_sym, int exech_symlen,
        int* symlen)
{
    return get_mpid_for_clr_acct(b->prod_codes, exch_sym,
            exech_symlen, symlen);
}

static uint64_t get_long_for_rom(char* rom_field, int rom_len)
{
    char occ[32];
    memset(&occ, '\0', 32);
    memcpy(&occ, rom_field, rom_len);
    return atol(occ);
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
uint64_t get_send_time()
{
	time_t rawtime;
	    time(&rawtime);
	    uint64_t ret = rawtime * 1000000000;
	    return ret;
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

static void trans_ord_instr(databuf_t* buff, dart_order_obj* rph)
{
    if(getplen(rph, ROM_EXEC_INST) > 0 && getplen(rph, ROM_INSTR) <= 0) {
        char* ex_inst = getpval(rph, ROM_EXEC_INST);
        switch(ex_inst[0]) {
            case 'L':
            case 'M':
            case 'm':
            case 'P':
            case 'Q':
            case 'R':
            case 'U':
                databuf_memcpy(buff, "P", 1);
                buff->wr_ptr += 3;
                return;
        }
        databuf_memcpy(buff, "B", 1);
        buff->wr_ptr += 3;
        return;
    }
    if(getplen(rph, ROM_INSTR) == 1) {
        char* inst = getpval(rph, ROM_INSTR);
        if(inst[0] == 'I' || inst[0] == 'A') {
            databuf_memcpy(buff, "P", 1);
        } else {
            databuf_memcpy(buff, inst, 1);
        }
        buff->wr_ptr += 3;
    } else if(getplen(rph, ROM_INSTR) > 1) {
        char* inst = getpval(rph, ROM_INSTR);
        if(inst[1] == 'D' || inst[1] == 'L' || inst[1] == 'S') {
            databuf_memcpy(buff, inst, 2);
            buff->wr_ptr += 2;
        } else {
            databuf_memcpy(buff, inst, 1);
            buff->wr_ptr += 3;
        }  
    } else {
        databuf_memcpy(buff, "R", 1);
        buff->wr_ptr += 3;
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
    ord->som = 0xBABA;
    ord->seq = 0x0;
    int ord_id_len = create_multiday_in_place_ord_token(tg,
            ord->ord_id, 20);
    memcpy(&ord->side, getpval(rph, ROM_SIDE), 1);
    unsigned int net_qty =
        strtol(rph->positions[ROM_SHARES].iov_base, NULL, 10);
    set_rom_field(rph, ROM_BIN_QTY, &net_qty, sizeof(int));
    set_rom_field(rph, ROM_BIN_CUM, &null_int, sizeof(int));
    set_rom_field(rph, ROM_PENDING_CNL, "0", 1);
    char strleave[64];
    memset(strleave, '\0', 64);
    memcpy(strleave, rph->positions[ROM_SHARES].iov_base,
            rph->positions[ROM_SHARES].iov_len);
    set_rom_field(rph, ROM_LEAVES, rph->positions[ROM_SHARES].iov_base,
            rph->positions[ROM_SHARES].iov_len);
    uint32_t leaves = strtol(strleave, NULL, 10);
    set_rom_field(rph, ROM_BIN_LEAVES, &leaves, sizeof(uint32_t));
    ord->qty = net_qty;
    ord->nnobfields = 9; /*We always have 7 bit fields some may be 0*/
    SET_BIT(0x1, 0x1, ord->bf1); /*All new orders have clearing firm*/
    set_clearing_firm(buff, ob, rph);
    char* capacity = rph->positions[ROM_CAP].iov_base;
    int clen = rph->positions[ROM_CAP].iov_len;
    if(getplen(rph, ROM_CAP) > 0 && capacity[0] == 'S' &&
        rph->positions[ROM_MM_ACC].iov_len > 0) { 
      SET_BIT(0x1, 0x2, ord->bf1); /*Only MM new orders have the clearing account*/
      char* mmacc = rph->positions[ROM_MM_ACC].iov_base;
      int mmlen = rph->positions[ROM_MM_ACC].iov_len;
      int cplen = mmlen > 4 ? 4 : mmlen;
      databuf_memcpy(buff, mmacc, cplen);
      buff->wr_ptr += (4 - cplen);
    } 
    if(getplen(rph, ROM_PRICE) > 0) {
        SET_BIT(0x1, 0x4, ord->bf1); /* We have a price*/
        if(getplen(rph, ROM_BIN_PRICE) <= 0) {
            long np = rom_to_boe_price(rph->positions[ROM_PRICE].iov_base,
                    rph->positions[ROM_PRICE].iov_len);
            set_rom_field(rph, ROM_BIN_PRICE, &np, sizeof(long));
            databuf_memcpy(buff, (char *) &np, sizeof(long));
        } else  {
            long np = 0;
            memcpy(&np, getpval(rph, ROM_BIN_PRICE), getplen(rph, ROM_BIN_PRICE));
            databuf_memcpy(buff,(char*)&np, sizeof(long));
        }
    }
    if(getplen(rph, ROM_EXEC_INST) == 1) {
        char* exec = getpval(rph, ROM_EXEC_INST);
        if(exec[0] == 'f') {
            return -1;/*Intermarket sweeps are not allowed*/
        }
        switch(exec[0]) {
            case '1':
            case 'r':
            case 'G':
                {
                    SET_BIT(0x1, 0x8, ord->bf1); /* We have execution instructions*/
                    databuf_memcpy(buff, exec, 1);
                }
                break;
        };
    }

    char* symbol = 0;
    int sym_len = 0;
    if(getplen(rph, ROM_SYM) > 0) {
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
            databuf_memcpy(buff, trans_ord_tif(getpval(rph, ROM_TYPE),
                        getplen(rph, ROM_TYPE),
                        getpval(rph, ROM_EXEC_INST),
                        getplen(rph, ROM_EXEC_INST),
                        tif,tif_len), 1);
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
    if(getplen(rph, ROM_MAX_FLOOR) > 0) {
        SET_BIT(0x1, 0x80, ord->bf1); /* We have a Max floor*/
        char ccmta[32];
        memset(ccmta, '\0', 32);
        memcpy(ccmta, rph->positions[ROM_MAX_FLOOR].iov_base, 
                rph->positions[ROM_MAX_FLOOR].iov_len);
        unsigned int min = atoi(ccmta);
        databuf_memcpy(buff, &min, 4);
    }
    databuf_memcpy(buff, symbol, sym_len);
    if(sym_len < 8) {
        buff->wr_ptr += (8 - sym_len);
    }
    SET_BIT(0x1, 0x40, ord->bf2); /* We have a capacity*/
    char cap = get_capacity(
            capacity, clen,
            rph->positions[ROM_DESTID].iov_base,
            rph->positions[ROM_DESTID].iov_len);
    databuf_memcpy(buff, &cap, 1);
    if(getplen(rph, ROM_INSTR) > 0 ||
            getplen(rph, ROM_EXEC_INST) > 0) {
        SET_BIT(0x1, 0x80, ord->bf2); /* We have a route*/
        trans_ord_instr(buff, rph);
        //SET_BIT(0x1, 0x8, ord->bf1); /* We have execution instructions*/
        //databuf_memcpy(buff, exec, 1);
    }
    if(getplen(rph, ROM_CLR_ACC) > 0) {
        SET_BIT(0x1, 0x1, ord->bf3); /*We have an account*/
        int acc_len = getplen(rph, ROM_CLR_ACC);
        char* lclr = getpval(rph, ROM_CLR_ACC);
        databuf_memcpy(buff, lclr, acc_len);
        buff->wr_ptr += (16 - acc_len);
    }
    if(rph->positions[ROM_DISP_INSTR].iov_len  > 0) {
        SET_BIT(0x1, 0x2, ord->bf3); /*We have an account*/
        databuf_memcpy(buff, rph->positions[ROM_DISP_INSTR].iov_base,
                rph->positions[ROM_DISP_INSTR].iov_len);
    }
    if(getplen(rph, ROM_DESC) == 3) {
        SET_BIT(0x1, 0x20, ord->bf3); /*We have a prevent match code*/
        databuf_memcpy(buff, getpval(rph, ROM_DESC), 3);
    }
    if(getplen(rph, ROM_EXPIRE_DATE) > 0 &&
        rph->positions[ROM_EXPIRE_DAY].iov_len > 0) { 
        SET_BIT(0x1, 0x1, ord->bf4); /*We have an maturity date*/
        int expdate = trans_mat_ymd(rph->positions[ROM_EXPIRE_DATE].iov_base,
                                    rph->positions[ROM_EXPIRE_DATE].iov_len,
                                    rph->positions[ROM_EXPIRE_DAY].iov_base,
                                    rph->positions[ROM_EXPIRE_DAY].iov_len);
        databuf_memcpy(buff, &expdate, 4);
    }
    if(getplen(rph, ROM_STRIKE) > 0) {
        SET_BIT(0x1, 0x2, ord->bf4); /*We have a strike*/
        long np = rom_to_boe_price(rph->positions[ROM_STRIKE].iov_base,
                    rph->positions[ROM_STRIKE].iov_len);
        databuf_memcpy(buff, &np, 8);
    }
    if(getplen(rph, ROM_P_C) > 0) {
      SET_BIT(0x1, 0x4, ord->bf4); /*We have an open close */
       char* pc = rph->positions[ROM_P_C].iov_base;
       if(pc[0] == 'P') {
         databuf_memcpy(buff, "0", 1);
       } else {
         databuf_memcpy(buff, "1", 1);
       }
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
        long np = rom_to_boe_price(rph->positions[ROM_STOP].iov_base,
                rph->positions[ROM_STOP].iov_len);
        databuf_memcpy(buff, &np, sizeof(long));
    }
    int instr_len = getplen(rph, ROM_INSTR);
    if(instr_len > 2) {
        /*This check is for the ROUTE strategy and delivery fields*/
        trans_extra_routes(buff, rph, ord);
    } 
    if(getplen(rph, ROM_AUCTION_ID) > 1) {
      SET_BIT(0x1, 0x40, ord->bf6); /*We have a stop price */
      uint64_t auction_id = get_long_for_rom(rph->positions[ROM_AUCTION_ID].iov_base,
          rph->positions[ROM_AUCTION_ID].iov_len);
      databuf_memcpy(buff, &auction_id, 8);
    }
    if(getplen(rph, ROM_FREQ_TRADE) > 0) {
      SET_BIT(0x1, 0x10, ord->bf8); /*We have a frequent trader */
      databuf_memcpy(buff, rph->positions[ROM_FREQ_TRADE].iov_base, 6);
    }
    if(getplen(rph, ROM_DISP_ROUTE) > 0) {
      SET_BIT(0x1, 0x40, ord->bf8); /*We have a floor route */
      databuf_memcpy(buff,"PARO", 4);
      SET_BIT(0x1, 0x80, ord->bf8); /*We have a floor route */
      databuf_memcpy(buff, rph->positions[ROM_DISP_ROUTE].iov_base, 1);
    }
    /*if(getplen(rph, ROM_MM_ACC) > 0) {
      SET_BIT(0x1, 0x1, ord->bf9);
      databuf_memcpy(buff, rph->positions[ROM_MM_ACC].iov_base, 
              rph->positions[ROM_MM_ACC].iov_len);
    }*/
    ord->len = databuf_unread(buff) - 2;    
    /*Since ROM_CLORDID and instructions are the same field we need to wait until 
     * we have used the instructions before we set the id.*/
    set_rom_field(rph, ROM_CLORDID, ord->ord_id, ord_id_len);    
    no_copy_insert(ob->cl_ord_id_map, getpval(rph,ROM_CLORDID),
            getplen(rph,ROM_CLORDID),v);
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
    set_rom_field(rph, ROM_PENDING_CNL, "1", 1);
    ord->ncnlbfields = 2;
    SET_BIT(0x1, 0x1, ord->bf1); /*All new orders have clearing firm*/
    set_clearing_firm(buff, ob, rph);
    SET_BIT(0x2, 0x8, ord->bf2); /*Add in date time*/
    uint64_t sTime = get_send_time();
    send_debug_message(ob->o,
                "Adding this send time: %lu \n", sTime);
    databuf_memcpy(buff,&sTime, 8); 
    ord->len = databuf_unread(buff) - 2; 
    return databuf_unread(buff);
}
int create_ex_boe_cmplx_ord(databuf_t* buff, rcntr* v, 
        con_int* ob, token_gen* tg, uint64_t seq)
{
    boecmplxord* ord = (boecmplxord*)buff->buffer;
    memset(buff->buffer, 0, 256);
    buff->wr_ptr += sizeof(struct boe_cmplx_order);
    ord->som = 0xBABA;
    ord->type = 0x4B;
    ord->seq = 0x0;
    dart_order_obj *rph = (dart_order_obj *) v->raw;
    
    char* instr = getpval(rph, ROM_INSTR);
    int instr_len = getplen(rph, ROM_INSTR);
    if(instr_len == 1) {
        set_rom_field(rph, ROM_COPY_INSTR, instr,instr_len);
        if(instr[0] == 'S') {
            rph->positions[ROM_INSTR].iov_len = 0;
        } 
    }
    int ord_id_len = create_multiday_in_place_ord_token(tg,
            ord->ord_id, 20);
    memcpy(&ord->side, getpval(rph, ROM_SIDE), 1);
    uint32_t net_qty =
        strtol(rph->positions[ROM_SHARES].iov_base, NULL, 10);
    set_rom_field(rph, ROM_BIN_QTY, &net_qty, sizeof(int));
    set_rom_field(rph, ROM_BIN_CUM, &null_int, sizeof(int));
    char strleave[64];
    memset(strleave, '\0', 64);
    memcpy(strleave, rph->positions[ROM_SHARES].iov_base,
            rph->positions[ROM_SHARES].iov_len);
    set_rom_field(rph, ROM_LEAVES, rph->positions[ROM_SHARES].iov_base,
            rph->positions[ROM_SHARES].iov_len);
    uint32_t leaves = strtol(strleave, NULL, 10);
    set_rom_field(rph, ROM_BIN_LEAVES, &leaves, sizeof(uint32_t));
    ord->qty = net_qty;
    ord->num_bits = 6;
    if(rph->positions[ROM_O_C].iov_len > 0) {
        int oclen = rph->positions[ROM_O_C].iov_len;
        int i = 0;
        for(;i < oclen; ++i) {
            char* oc = (char*)rph->positions[ROM_O_C].iov_base;
            if(oc[i] == '1') {
                databuf_memcpy(buff, "O", 1);
            } else if(oc[i] == 'N') {
                databuf_memcpy(buff, "N", 1);
            } else {
                databuf_memcpy(buff, "C", 1);
            }
            ord->num_legs++;
        }
    }
    SET_BIT(0x1, 0x1, ord->bf1); /*All new orders have clearing firm*/
    set_clearing_firm(buff, ob, rph);
    char* capacity = rph->positions[ROM_CAP].iov_base;
    int clen = rph->positions[ROM_CAP].iov_len;
    if(getplen(rph, ROM_CAP) > 0 && capacity[0] == 'S' &&
        rph->positions[ROM_MM_ACC].iov_len > 0) { 
        SET_BIT(0x1, 0x2, ord->bf1); /*All new orders have the clearing account*/
      char* mmacc = rph->positions[ROM_MM_ACC].iov_base;
      int mmlen = rph->positions[ROM_MM_ACC].iov_len;
      int cplen = mmlen > 4 ? 4 : mmlen;
      databuf_memcpy(buff, mmacc, cplen);
      buff->wr_ptr += (4 - cplen);
    } 
    if(getplen(rph, ROM_PRICE) > 0) {
        SET_BIT(0x1, 0x4, ord->bf1); /* We have a price*/
        if(getplen(rph, ROM_BIN_PRICE) <= 0) {
            long np = rom_to_boe_price(rph->positions[ROM_PRICE].iov_base,
                    rph->positions[ROM_PRICE].iov_len);
            set_rom_field(rph, ROM_BIN_PRICE, &np, sizeof(long));
            databuf_memcpy(buff, (char *) &np, sizeof(long));
        } else  {
            long np = 0;
            memcpy(&np, getpval(rph, ROM_BIN_PRICE), getplen(rph, ROM_BIN_PRICE));
            databuf_memcpy(buff,(char*)&np, sizeof(long));
        }
    }
    SET_BIT(0x1, 0x8, ord->bf1); /* DART requires an order type*/
    databuf_memcpy(buff, trans_ord_type(getpval(rph, ROM_TYPE), getplen(rph, ROM_TYPE),
                getpval(rph, ROM_EXEC_INST), getplen(rph, ROM_EXEC_INST)), 1);
    int tif_len = getplen(rph, ROM_TIF);
    if(tif_len > 0) {
        SET_BIT(0x1, 0x10, ord->bf1); /* We have a time in force*/
        char* tif = rph->positions[ROM_TIF].iov_base;
            databuf_memcpy(buff, trans_ord_tif(getpval(rph, ROM_TYPE),
                        getplen(rph, ROM_TYPE),
                        getpval(rph, ROM_EXEC_INST),
                        getplen(rph, ROM_EXEC_INST),
                        tif,tif_len), 1);
    } 
    SET_BIT(0x1, 0x20, ord->bf1); /* We have a Symbol*/
    int sec_len =rph->positions[ROM_SEC_DEF].iov_len;
    if(sec_len > 8) {
        return 0;
    } else { 
        databuf_memcpy(buff,rph->positions[ROM_SEC_DEF].iov_base,
                sec_len);
        if(rph->positions[ROM_SEC_DEF].iov_len < 8) {
            buff->wr_ptr += (8 - rph->positions[ROM_SEC_DEF].iov_len);
        }
    }
    SET_BIT(0x1, 0x40, ord->bf1); /* We have a capacity*/
    char cap = get_capacity(
            capacity, clen,
            rph->positions[ROM_DESTID].iov_base,
            rph->positions[ROM_DESTID].iov_len);
    databuf_memcpy(buff, &cap, 1);
    if(rph->positions[ROM_DISP_INSTR].iov_len  > 0) {
        /*This check is for the ROUTE strategy and delivery fields*/
        SET_BIT(0x1, 0x80, ord->bf1); /* We have a route INST*/
        char* route = rph->positions[ROM_DISP_INSTR].iov_base;
        int rlen = rph->positions[ROM_DISP_INSTR].iov_len;
        int cplen = rlen > 2 ? 2 : rlen;
        databuf_memcpy(buff, route, cplen);
        buff->wr_ptr += (4 - cplen);
    }

    if(getplen(rph, ROM_CLR_ACC) > 0) {
        SET_BIT(0x1, 0x1, ord->bf2); /*We have an account*/
        char* lclr = getpval(rph, ROM_CLR_ACC);
        int acc_len = getplen(rph, ROM_CLR_ACC);
        databuf_memcpy(buff, lclr, acc_len);
        buff->wr_ptr += (16 - acc_len);
    }


    if(getplen(rph, ROM_DESC) == 3) {
        SET_BIT(0x1, 0x02, ord->bf2); /*We have a prevent match code*/
        databuf_memcpy(buff, getpval(rph, ROM_DESC), 3);
    }
    if(rph->positions[ROM_CMTA].iov_len > 0) {
        SET_BIT(0x1, 0x8, ord->bf2); /*We have a cmta */
        char ccmta[32];
        memset(ccmta, '\0', 32);
        memcpy(ccmta, rph->positions[ROM_CMTA].iov_base, 
                rph->positions[ROM_CMTA].iov_len);
        int icmta = atoi(ccmta);
        databuf_memcpy(buff, &icmta, 4);
    }
    if(getplen(rph, ROM_AUCTION_ID) > 1) {
      SET_BIT(0x1, 0x80, ord->bf2); /*We have a stop price */
      uint64_t auction_id = get_long_for_rom(rph->positions[ROM_AUCTION_ID].iov_base,
          rph->positions[ROM_AUCTION_ID].iov_len);
      databuf_memcpy(buff, &auction_id, 8);
    }
    if(instr_len == 1 && instr[0] == 'S') {
      SET_BIT(0x1, 0x20, ord->bf3); /*We have a equity leg*/
      set_clearing_firm(buff, ob, rph);
    }

    if(getplen(rph, ROM_EXEC_INST) == 1) {
        char* co = rph->positions[ROM_EXEC_INST].iov_base;
        if(co[0] == 'Y') {
          SET_BIT(0x1, 0x20, ord->bf4); /*We have a stop price */
          databuf_memcpy(buff, co, 1);
        }
    }

    if(getplen(rph, ROM_SHORT_LEND) > 0) {
        SET_BIT(0x1, 0x1, ord->bf5); /*We have a stop price */
        databuf_memcpy(buff, "5", 1);
    }
    ord->len = databuf_unread(buff) - 2;    
    /*Since ROM_CLORDID and instructions are the same field we need to wait until 
     * we have used the instructions before we set the id.*/
    set_rom_field(rph, ROM_CLORDID, ord->ord_id, ord_id_len);    
    no_copy_insert(ob->cl_ord_id_map, getpval(rph,ROM_CLORDID),
            getplen(rph,ROM_CLORDID),v);
    return databuf_unread(buff);
}
int match_sym_ex_with_child(con_int* ob, dart_order_obj* doj, char* sym, int slen, long strike, uint32_t date, char* put_call) 
{
    int pclen = strlen(put_call); 
    int pcmatch = 0;
    long np = rom_to_boe_price(doj->positions[ROM_STRIKE].iov_base,
                    doj->positions[ROM_STRIKE].iov_len);
    uint32_t expdate = trans_mat_ymd(doj->positions[ROM_EXPIRE_DATE].iov_base,
                                    doj->positions[ROM_EXPIRE_DATE].iov_len,
                                    doj->positions[ROM_EXPIRE_DAY].iov_base,
                                    doj->positions[ROM_EXPIRE_DAY].iov_len);
    int lpc_len = doj->positions[ROM_P_C].iov_len;
    char* pc = doj->positions[ROM_P_C].iov_base;
    send_debug_message(ob->o,
                "Looking to match:sym: %s, child strike: %lu,strike %ld, expdate %u, date %u, pc:%s, tpc:%c \n",
                sym, strike, np, date, expdate, put_call, pc[0]);
       if(pclen == 2 && lpc_len > 0 &&
               put_call[1] == pc [0]) {
           pcmatch = 1;
       } else if(pclen == 1 && lpc_len <= 0){
           pcmatch = 1;
       }
       if(np == strike && date == expdate && pcmatch) {
         return 0;
       } else {
         return 1;
       }
}

static int prepare_legs(dart_order_obj* ldoj, uint32_t net_qty)
{
    int is_short = 0;
        if(ldoj->positions[ROM_SIDE].iov_len == 1) {
            char* side = ldoj->positions[ROM_SIDE].iov_base;
            if(side[0] == '5') {
                is_short = 1;
            }
        }
        uint32_t leg_qty =
            strtol(ldoj->positions[ROM_SHARES].iov_base, NULL, 10);
        uint32_t leg_total = leg_qty * net_qty;
        set_rom_field(ldoj, ROM_BIN_QTY, &leg_total, sizeof(int));
        set_rom_field(ldoj, ROM_BIN_CUM, &null_int, sizeof(int));
        return is_short;
}
/**
 *
 */
int create_boe_cmplx_order(databuf_t* buff, rcntr* v,
    con_int* ob, token_gen* tg, uint64_t seq, char* ack)
{
    boecmplxacc* acceptor = (boecmplxacc*)ack;
    memset(buff->buffer, 0, 256);
    boecmplxord* ord = (boecmplxord*)buff->buffer;
    buff->wr_ptr += sizeof(struct boe_cmplx_order);
    ord->som = 0xBABA;
    ord->type = 0x4B;
    ord->seq = 0x0;
    dart_order_obj *rph = (dart_order_obj *) v->raw;
    int ord_id_len = create_multiday_in_place_ord_token(tg,
            ord->ord_id, 20);
    uint32_t net_qty =
        strtol(rph->positions[ROM_SHARES].iov_base, NULL, 10);
    set_rom_field(rph, ROM_BIN_QTY, &net_qty, sizeof(int));
    set_rom_field(rph, ROM_BIN_CUM, &null_int, sizeof(int));
    char strleave[64];
    memset(strleave, '\0', 64);
    memcpy(strleave, rph->positions[ROM_SHARES].iov_base,
            rph->positions[ROM_SHARES].iov_len);
    set_rom_field(rph, ROM_LEAVES, rph->positions[ROM_SHARES].iov_base,
            rph->positions[ROM_SHARES].iov_len);
    uint32_t leaves = strtol(strleave, NULL, 10);
    set_rom_field(rph, ROM_BIN_LEAVES, &leaves, sizeof(uint32_t));
    ord->qty = net_qty;
    ord->num_bits = 6;
    int price_invert = 1;
    int is_short = 0;
    int has_equity = 0;
    /**
     * Iterate the return to match the open and close correctly
     */
    if(acceptor->revised_legs == '2') {
        uint8_t num_bit_fields = *(ack + 52);
        char* ack_off = ack + 52 + num_bit_fields + 1;
        uint8_t nackleg = *(ack_off);
        ord->num_legs = nackleg;
        ack_off += 1;
        int i = 0;
        int8_t rej = 0;
        char* pside = rph->positions[ROM_SIDE].iov_base;
        send_debug_message(ob->o,
                "Legs revised: %d, num legs: %u, number of bit fields: %u\n",
                acceptor->revised_legs, nackleg, num_bit_fields);
        for(;i < nackleg; ++i) {
            int still_lookin = 1;
            dart_order_obj* ldoj = 0x0;
            children* lch = rph->childs;
            char sym[9];
            memset(sym, '\0', 9);
            memcpy(sym, ack_off, 8);
            ack_off += 8;
            char put_call[9];
            memset(put_call, '\0', 9);
            memcpy(put_call, ack_off, 6);
            ack_off += 6;
            uint32_t date = 0;
            memcpy(&date, ack_off, 4);
            ack_off += 4;
            long strike = 0;
            memcpy(&strike, ack_off, 8);
            ack_off += 8;
            ack_off += 4;//Jump the ratioqty 4
            uint8_t side = *(ack_off);
            ack_off += 1;
            send_debug_message(ob->o,
                        "Looking for leg : %s, date: %u, stike: %lu put_call: %s side: %c, spread sym: %s\n",
                        sym, date, strike, put_call,side, acceptor->symbol);
            //double lp = ((double) strike / 10000.0) + .0000001;
            do {
                ldoj = (dart_order_obj*)lch->payload;
                still_lookin = match_sym_ex_with_child(ob, ldoj, sym, 8,strike, date, put_call); 
                if(still_lookin > 0) {
                    lch = lch->next;
                } else {
                    if(rph->positions[ROM_SIDE].iov_len > 0 &&
                        ldoj->positions[ROM_SIDE].iov_len > 0) {
                        char* lside = ldoj->positions[ROM_SIDE].iov_base;
                        if(pside[0] == '1' && (lside[0] == '2' || lside[0] == '5') &&
                                side == '1') {
                        send_debug_message(ob->o,
                            "Leg sides switched switching our side: sym %s, date: %u, stike: %lu put_call: %s, CBOE side: %c,leg side %c, side %c\n",
                            sym, date, strike, put_call, side, lside[0], pside[0]);
                            rej = 1; //Sides switched do not send the order as is.
                        } else if(pside[0] == '2' && lside[0] == '1' && side == '2') {
                            rej = 1;
                        send_debug_message(ob->o,
                            "Leg sides switched switching our side: sym %s, date: %u, stike: %lu put_call: %s, CBOE side: %c,leg side %c, side %c\n",
                            sym, date, strike, put_call, side, lside[0], pside[0]);
                        }
                        send_debug_message(ob->o,
                            "Found Matching leg : %s, date: %u, stike: %lu put_call: %s, CBOE side: %c,leg side %c, side %c\n",
                            sym, date, strike, put_call, side, lside[0], pside[0]);
                    }
                    if(ldoj->positions[ROM_O_C].iov_len > 0) {
                        char* oc = (char*)ldoj->positions[ROM_O_C].iov_base;
                        if(oc[0] == '1') {
                            databuf_memcpy(buff, "O", 1);
                        } else if(oc[0] == 'N') {
                            databuf_memcpy(buff, "N", 1);
                        } else {
                            databuf_memcpy(buff, "C", 1);
                        }
                    } else {
                        databuf_memcpy(buff, "N", 1);
                    }
                    if(ldoj->positions[ROM_SEC_TYPE].iov_len > 0 ) {
                        char* sec = ldoj->positions[ROM_SEC_TYPE].iov_base;
                        if(sec[0] == 'E') {
                            has_equity = 1;
                        }
                    }
                    is_short = prepare_legs(ldoj, net_qty);
                }
            } while(lch && still_lookin);
            
            if(still_lookin ==1) {
                set_rom_field(rph, 0, "S", 1);
                set_rom_field(rph, ROM_STATUS, "8", 1);
                set_rom_field(rph, ROM_TEXT, "Unable to find legs", 19);
                return 0;
            }
        }
        if(rej == 1) {
                char lpside[2];
                memcpy(lpside, pside, 1);
                if(pside[0] == '1') {
                    fill_in_rom_field(rph, ROM_SIDE, 
                                "2", 1);
                } else if(pside[0] == '2') {
                    fill_in_rom_field(rph, ROM_SIDE, 
                                "1", 1);
                }
                set_rom_field(rph, ROM_TEXT, "Side Price flip", 15);
                price_invert = -1;
                    send_debug_message(ob->o,
                        "Leg sides switched switching our side: side %c, orig side %c\n",
                             pside[0], lpside[0]);
        }
    /**
     * end iteration.
     */
    } else {
        children* lch = rph->childs;
        if(lch != 0x0) {
            do {
                dart_order_obj* ldoj = (dart_order_obj*)lch->payload;
                if(ldoj->positions[ROM_O_C].iov_len > 0) {
                    char* oc = (char*)ldoj->positions[ROM_O_C].iov_base;
                    if(oc[0] == '1') {
                        databuf_memcpy(buff, "O", 1);
                    } else if(oc[0] == 'N') {
                        databuf_memcpy(buff, "N", 1);
                    } else {
                        databuf_memcpy(buff, "C", 1);
                    }
                } else {
                    databuf_memcpy(buff, "N", 1);
                }
                if(ldoj->positions[ROM_SEC_TYPE].iov_len > 0 ) {
                        char* sec = ldoj->positions[ROM_SEC_TYPE].iov_base;
                        if(sec[0] == 'E') {
                            has_equity = 1;
                        }
                }
                is_short = prepare_legs(ldoj, net_qty);
                ord->num_legs++;
                lch = lch->next;
            }while(lch);
        }
    }

    memcpy(&ord->side, getpval(rph, ROM_SIDE), 1);
    SET_BIT(0x1, 0x1, ord->bf1); /*All new orders have clearing firm*/
    set_clearing_firm(buff, ob, rph);
    char* capacity = rph->positions[ROM_CAP].iov_base;
    int clen = rph->positions[ROM_CAP].iov_len;
    if(getplen(rph, ROM_CAP) > 0 && capacity[0] == 'S' &&
        rph->positions[ROM_MM_ACC].iov_len > 0) { 
        SET_BIT(0x1, 0x2, ord->bf1); /*All new orders have the clearing account*/
      char* mmacc = rph->positions[ROM_MM_ACC].iov_base;
      int mmlen = rph->positions[ROM_MM_ACC].iov_len;
      int cplen = mmlen > 4 ? 4 : mmlen;
      databuf_memcpy(buff, mmacc, cplen);
      buff->wr_ptr += (4 - cplen);
    } 
    if(getplen(rph, ROM_PRICE) > 0) {
        SET_BIT(0x1, 0x4, ord->bf1); /* We have a price*/
        long np = 0;
        if(getplen(rph, ROM_BIN_PRICE) <= 0) {
            np = rom_to_boe_price(rph->positions[ROM_PRICE].iov_base,
                    rph->positions[ROM_PRICE].iov_len);
            np *= price_invert;
            set_rom_field(rph, ROM_BIN_PRICE, &np, sizeof(long));
            set_rom_field(rph, ROM_BIN_ORDID, &price_invert, sizeof(int));
            databuf_memcpy(buff, (char *) &np, sizeof(long));
        } else  {
            memcpy(&np, getpval(rph, ROM_BIN_PRICE), getplen(rph, ROM_BIN_PRICE));
            np *= price_invert;
            set_rom_field(rph, ROM_BIN_ORDID, &price_invert, sizeof(int));
            databuf_memcpy(buff,(char*)&np, sizeof(long));
        }
    }
    SET_BIT(0x1, 0x8, ord->bf1); /* DART requires an order type*/
    databuf_memcpy(buff, trans_ord_type(getpval(rph, ROM_TYPE), getplen(rph, ROM_TYPE),
                getpval(rph, ROM_EXEC_INST), getplen(rph, ROM_EXEC_INST)), 1);
    int tif_len = getplen(rph, ROM_TIF);
    if(tif_len > 0) {
        SET_BIT(0x1, 0x10, ord->bf1); /* We have a time in force*/
        char* tif = rph->positions[ROM_TIF].iov_base;
            databuf_memcpy(buff, trans_ord_tif(getpval(rph, ROM_TYPE),
                        getplen(rph, ROM_TYPE),
                        getpval(rph, ROM_EXEC_INST),
                        getplen(rph, ROM_EXEC_INST),
                        tif,tif_len), 1);
    } 
    SET_BIT(0x1, 0x20, ord->bf1); /* We have a Symbol*/
    databuf_memcpy(buff, acceptor->symbol, 8);
    {
      int sym_len = 0;
      FIND_NULL(acceptor->symbol,sym_len, 8);
      set_rom_field(rph, ROM_SEC_DEF, acceptor->symbol, sym_len);
    }
    SET_BIT(0x1, 0x40, ord->bf1); /* We have a capacity*/
    char cap = get_capacity(
            capacity, clen,
            rph->positions[ROM_DESTID].iov_base,
            rph->positions[ROM_DESTID].iov_len);
    databuf_memcpy(buff, &cap, 1);
    if(rph->positions[ROM_DISP_INSTR].iov_len  > 0) {
        /*This check is for the ROUTE strategy and delivery fields*/
        SET_BIT(0x1, 0x80, ord->bf1); /* We have a route INST*/
        char* route = rph->positions[ROM_DISP_INSTR].iov_base;
        int rlen = rph->positions[ROM_DISP_INSTR].iov_len;
        int cplen = rlen > 2 ? 2 : rlen;
        databuf_memcpy(buff, route, cplen);
        buff->wr_ptr += (4 - cplen);
    }

    if(getplen(rph, ROM_CLR_ACC) > 0) {
        SET_BIT(0x1, 0x1, ord->bf2); /*We have an account*/
        char* lclr = getpval(rph, ROM_CLR_ACC);
        int acc_len = getplen(rph, ROM_CLR_ACC);
        databuf_memcpy(buff, lclr, acc_len);
        buff->wr_ptr += (16 - acc_len);
    }


    if(getplen(rph, ROM_DESC) == 3) {
        SET_BIT(0x1, 0x02, ord->bf2); /*We have a prevent match code*/
        databuf_memcpy(buff, getpval(rph, ROM_DESC), 3);
    }
    if(rph->positions[ROM_CMTA].iov_len > 0) {
        SET_BIT(0x1, 0x8, ord->bf2); /*We have a cmta */
        char ccmta[32];
        memset(ccmta, '\0', 32);
        memcpy(ccmta, rph->positions[ROM_CMTA].iov_base, 
                rph->positions[ROM_CMTA].iov_len);
        int icmta = atoi(ccmta);
        databuf_memcpy(buff, &icmta, 4);
    }
    if(getplen(rph, ROM_AUCTION_ID) > 1) {
      SET_BIT(0x1, 0x80, ord->bf2); /*We have a stop price */
      uint64_t auction_id = get_long_for_rom(rph->positions[ROM_AUCTION_ID].iov_base,
          rph->positions[ROM_AUCTION_ID].iov_len);
      databuf_memcpy(buff, &auction_id, 8);
    }
    if(has_equity) {
        SET_BIT(0x1, 0x20, ord->bf3); /* We have a Symbol*/
        set_clearing_firm(buff, ob, rph);
    }
    if(getplen(rph, ROM_EXEC_INST) == 1) {
        char* co = rph->positions[ROM_EXEC_INST].iov_base;
        if(co[0] == 'Y') {
          SET_BIT(0x1, 0x20, ord->bf4); /*We have a stop price */
          databuf_memcpy(buff, co, 1);
        }
    }
    if(is_short) {
        SET_BIT(0x1, 0x1, ord->bf5); /*We have a stop price */
        databuf_memcpy(buff, "5", 1);
    }
    ord->len = databuf_unread(buff) - 2;    
    /*Since ROM_CLORDID and instructions are the same field we need to wait until 
     * we have used the instructions before we set the id.*/
    set_rom_field(rph, ROM_CLORDID, ord->ord_id, ord_id_len);    
    no_copy_insert(ob->cl_ord_id_map, getpval(rph,ROM_CLORDID),
            getplen(rph,ROM_CLORDID),v);
    return databuf_unread(buff);
}
/*
 */
int create_boe_cmplx_instrument(databuf_t* buff, rcntr* v,
    con_int* ob, token_gen* tg, uint64_t seq)
{
    memset(buff->buffer, 0, 256);
    boecmplxreq* ord = (boecmplxreq*)buff->buffer;
    buff->wr_ptr += sizeof(struct boe_new_cmplx_request);
    ord->som = 0xBABA;
    ord->type = 0x4C;
    ord->seq = 0x0;
    dart_order_obj *rph = (dart_order_obj *) v->raw;
    int ord_id_len = create_multiday_in_place_ord_token(tg,
            ord->ord_id, 20);
    if(rph->positions[ROM_INSTR].iov_len > 0) {
      set_rom_field(rph, ROM_DISP_INSTR, rph->positions[ROM_INSTR].iov_base,
          rph->positions[ROM_INSTR].iov_len);
    }
    ord->num_bits = 0x1;
    ord->bit_one = 0x0f;

    children* lch = rph->childs;
    do {
        dart_order_obj* ldoj = (dart_order_obj*)lch->payload;
        databuf_t* ordiddb = get_databuf(ob->gk);
        dart_itoa(ldoj, ord->num_legs, ROM_CLORDID, ordiddb);
        set_rom_field(ldoj, ROM_BIN_CLORDID, &ord->num_legs, sizeof(uint8_t));
        destroy_gk_db(ob->gk, ordiddb);
        int sym_len = ldoj->positions[ROM_SYM].iov_len;
        databuf_memcpy(buff, ldoj->positions[ROM_SYM].iov_base,
            sym_len);
        if(sym_len < 8) {
          buff->wr_ptr += (8 - sym_len);
        }
        if(ldoj->positions[ROM_P_C].iov_len > 0) {
          databuf_memcpy(buff, "O", 1);
          databuf_memcpy(buff, ldoj->positions[ROM_P_C].iov_base, 1);
          buff->wr_ptr += 4;
        } else {
          databuf_memcpy(buff, "E", 1);
          buff->wr_ptr += 5;
        }
        if(ldoj->positions[ROM_EXPIRE_DATE].iov_len > 0 && 
                ldoj->positions[ROM_EXPIRE_DAY].iov_len > 0) {
            int expdate = trans_mat_ymd(ldoj->positions[ROM_EXPIRE_DATE].iov_base,
                                        ldoj->positions[ROM_EXPIRE_DATE].iov_len,
                                        ldoj->positions[ROM_EXPIRE_DAY].iov_base,
                                        ldoj->positions[ROM_EXPIRE_DAY].iov_len);
            databuf_memcpy(buff, &expdate, 4);
        } else {
            uint32_t z = 0x0;
            databuf_memcpy(buff, &z, 4);
        }
        if(ldoj->positions[ROM_STRIKE].iov_len > 0) {
            long np = rom_to_boe_price(ldoj->positions[ROM_STRIKE].iov_base,
                        ldoj->positions[ROM_STRIKE].iov_len);
            databuf_memcpy(buff, &np, 8);
        } else {
            uint64_t z = 0x0;
            databuf_memcpy(buff, &z, 8);
        }
        char shares[10];
        memset(&shares, '\0', 10);
        memcpy(&shares, ldoj->positions[ROM_SHARES].iov_base,
                ldoj->positions[ROM_SHARES].iov_len);
        uint32_t legratio = atoi(shares);
        databuf_memcpy(buff, &legratio, 4);
        ldoj->risk.size = legratio * v->size;
        char* side = ldoj->positions[ROM_SIDE].iov_base;
        if(side[0] == '5') {
            databuf_memcpy(buff, "2", 1);
        } else {
            databuf_memcpy(buff, side, 1);
        }
        lch = lch->next;
        ord->num_legs++;
    }while(lch);
    set_clearing_firm(buff, ob, rph);
    set_rom_field(rph, ROM_CLORDID, ord->ord_id, ord_id_len);    
    no_copy_insert(ob->cl_ord_id_map, getpval(rph,ROM_CLORDID),
            getplen(rph,ROM_CLORDID),v);
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
    if(rep->positions[ROM_PRICE].iov_len > 0) {
        long np = rom_to_boe_price(rep->positions[ROM_PRICE].iov_base,
                rep->positions[ROM_PRICE].iov_len);
        if(is_spread(rph) && rph->positions[ROM_BIN_ORDID].iov_len > 0) {
            int price_invert = 1;
            memcpy(&price_invert, rph->positions[ROM_BIN_ORDID].iov_base, sizeof(int));
            if(price_invert == -1 || price_invert == 1) {
                np *= price_invert;
            }
            set_rom_field(rep, ROM_BIN_PRICE, &np, sizeof(long));
            databuf_memcpy(buff, (char *) &np, sizeof(long));
        } else {
            set_rom_field(rep, ROM_BIN_PRICE, &np, sizeof(long));
            databuf_memcpy(buff, (char *) &np, sizeof(long));
        }
    } else {
        long np = 0;
        memcpy(&np, rph->positions[ROM_BIN_PRICE].iov_base,
                rph->positions[ROM_BIN_PRICE].iov_len);
        if(is_spread(rph) && rph->positions[ROM_BIN_ORDID].iov_len > 0) {
            int price_invert = 1;
            memcpy(&price_invert, rph->positions[ROM_BIN_ORDID].iov_base, sizeof(int));
            if(price_invert == -1 || price_invert == 1) {
                np *= price_invert;
            }
            set_rom_field(rep, ROM_BIN_PRICE, &np, sizeof(long));
            databuf_memcpy(buff, (char *) &np, sizeof(long));
        } else {
            databuf_memcpy(buff, (char *) &np, sizeof(long));
        }
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
    /*if(getplen(rep, ROM_SIDE) > 0) {
        if(getplen(rph, ROM_SIDE) == getplen(rep, ROM_SIDE)) {
            char* orig_side = getpval(rph, ROM_SIDE);
            char* rep_side = getpval(rep, ROM_SIDE);
            if(strncmp(orig_side, rep_side, getplen(rep, ROM_SIDE)) != 0) {
                SET_BIT(0x1, 0x80, ord->bf1); // We have a new side 
                databuf_memcpy(buff, getpval(rep, ROM_SIDE), 1);
            }
        } else {
            SET_BIT(0x1, 0x80, ord->bf1); // We have a new side 
            databuf_memcpy(buff, getpval(rep, ROM_SIDE), 1);
        }
    }*/
    if(getplen(rep, ROM_MAX_FLOOR) > 0) {
        SET_BIT(0x1, 0x1, ord->bf2); /* We have a Max Floor*/
        unsigned int max = atoi(getpval(rep, ROM_MAX_FLOOR));
        databuf_memcpy(buff, &max, 4);
    }
    if(getplen(rep, ROM_STOP) > 0) {
        SET_BIT(0x1, 0x2, ord->bf2); /*We have a stop price */
        long np = rom_to_boe_price(rep->positions[ROM_STOP].iov_base,
                rep->positions[ROM_STOP].iov_len);
        databuf_memcpy(buff, &np, sizeof(long));
    }
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
    set_rom_field(ute, ROM_PENDING_CNL, "0", 1);
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
    set_rom_field(ute, ROM_PENDING_CNL, "0", 1);
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
int boe_parse_order_restated(rcntr* cnt, char* ack, con_int* ob)
{
    int remove_order = 0;
    dart_order_obj* ute = (dart_order_obj*)cnt->raw;
    set_time(ute, ack, ob);
    uint8_t num_ret_bit = *(ack + 48);
    uint32_t is_present;
    if(num_ret_bit >= 5) {
        uint8_t bit_five = *(ack + 53);
        IS_BIT_SET(is_present, bit_five, 0x1);
        if(is_present) {
            uint32_t ord_qty = 0x0;
            memcpy(&ord_qty, ack + 54, 4);
            uint32_t leaves = 0x0;
            memcpy(&leaves, ack + 58, 4);
            char* is_pending_cnl = ute->positions[ROM_PENDING_CNL].iov_base; 
            send_debug_message(ob->o,
                "We bit five is present: %d, leaves: %d, qty: %d\n",
                bit_five, leaves, ord_qty);
            if(leaves == 0) {
                set_rom_field(ute, ROM_LEAVES, "0", 1);
                uint8_t pending = 1;
                set_rom_field(ute,ROM_BIN_PENDING_ID, &pending, 1);
            }  else if(ute->positions[ROM_PENDING_CNL].iov_len > 0 &&
                    is_pending_cnl[0] == '1' && ((cnt->cum + leaves < cnt->size))) {
                send_debug_message(ob->o,
                "Leaves and cum not equal to size five is present: %d, leaves: %d, qty: %d, our qty %d\n",
                bit_five, leaves, ord_qty, cnt->size);
                databuf_t *buff = get_databuf(ob->gk);
                unsigned_dart_itoa(ute, leaves,
                                ROM_LEAVES, buff);
                uint8_t pending = 1;
                set_rom_field(ute,ROM_BIN_PENDING_ID, &pending, 1);
                destroy_gk_db(ob->gk, buff);
            }
        } else {
            send_debug_message(ob->o,
                "We bit five does not have leaves set: %d\n",
                bit_five);
        }
    } else {
        send_debug_message(ob->o,
                "We received fewer than 5 bit fields on a restate: %d\n",
                num_ret_bit);
    } 
    return remove_order;
}
int boe_parse_replaced(rcntr * cnt, char *ack, con_int * ob)
{
    dart_order_obj* ute = (dart_order_obj*)cnt->raw;
    int ret_val = 0;
    set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    set_rom_field(ute, ROM_STATUS, "5", 1);
    set_time(ute, ack, ob);
    set_ordid(ute, ack, ob);

    uint8_t num_bit_fields = *(ack + 47);
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
    if(rep) {
        if(rep->positions[ROM_SHARES].iov_len > 0) {
            fill_in_rom_field(ute, ROM_SHARES, rep->positions[ROM_SHARES].iov_base,
                              rep->positions[ROM_SHARES].iov_len);
            int l_q = strtol(rep->positions[ROM_SHARES].iov_base, NULL, 10);
            memcpy(ute->positions[ROM_BIN_QTY].iov_base, &l_q, 4);
            //setting leaves
            int l_cum = 0;
            memcpy(&l_cum, ute->positions[ROM_BIN_CUM].iov_base, 4);
            int leaves = l_q - l_cum;
	    if(leaves > 0 && leaves < LEAVE_MAX) {
		    dart_itoa(ute, leaves, ROM_LEAVES, buff);
	    } else {
            fill_in_rom_field(ute, ROM_LEAVES, rep->positions[ROM_SHARES].iov_base,
                              rep->positions[ROM_SHARES].iov_len);
	    }
            //End leaves
            cnt->size = rep->risk.size;
            if(is_spread(ute)&& ute->num_childs > 0) {
              children* lch = ute->childs;
              dart_order_obj* ldoj = 0x0;
              do {
                  ldoj = (dart_order_obj*)lch->payload;
                  uint32_t leg_qty =
                    strtol(ldoj->positions[ROM_SHARES].iov_base, NULL, 10);
                  uint32_t cum = strtol(ldoj->positions[ROM_CUM].iov_base, NULL, 10);
                  uint32_t leg_total = leg_qty * (l_q - cum);
                  fill_in_rom_field(ldoj, ROM_BIN_QTY, &leg_total, sizeof(int));
                  databuf_reset(buff);
		  if(leg_total > 0 && leg_total < LEAVE_MAX) {
			  dart_itoa(ldoj, leg_total, ROM_LEAVES, buff);
		  } else {
			  leg_total = leg_qty * l_q;
			  dart_itoa(ldoj, leg_total, ROM_LEAVES, buff);
		  }
                  databuf_reset(buff);
                  lch = lch->next;
              } while(lch);
            }
        }
    } else {
        send_debug_message(ob->o, "We did not see the share bit three as set: %#010x\n",
                            bit_three);
    }
    if(rep) {
        if(rep->positions[ROM_PRICE].iov_len > 0) {
            fill_in_rom_field(ute, ROM_PRICE, rep->positions[ROM_PRICE].iov_base,
                              rep->positions[ROM_PRICE].iov_len);
            long np = rom_to_boe_price(rep->positions[ROM_PRICE].iov_base,
                    rep->positions[ROM_PRICE].iov_len);
            set_rom_field(ute, ROM_BIN_PRICE, &np, sizeof(long));
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
    if(getplen(ute, ROM_CPLX_TYPE) > 0) {
        set_rom_field(ute, ROM_CPLX_TYPE, "5", 1);
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

int match_ex_with_child(dart_order_obj* doj, long strike, uint32_t date, char* put_call, 
    databuf_t* buff, con_int* ob)
{
    char* tpc = "1";
    long np = rom_to_boe_price(doj->positions[ROM_STRIKE].iov_base,
                    doj->positions[ROM_STRIKE].iov_len);
    uint32_t expdate = trans_mat_ymd(doj->positions[ROM_EXPIRE_DATE].iov_base,
                                    doj->positions[ROM_EXPIRE_DATE].iov_len,
                                    doj->positions[ROM_EXPIRE_DAY].iov_base,
                                    doj->positions[ROM_EXPIRE_DAY].iov_len);
       char* pc = doj->positions[ROM_P_C].iov_base;
       if(pc[0] == 'P') {
         tpc = "0";
       } 
       if(np == strike && date == expdate && put_call[0] == tpc[0]) {
       send_debug_message(ob->o, "We say match: np: %ld, strike %ld, date %u, expdate %u, pc: %c, tcp %c \n",
               np, strike, date, expdate, put_call[0], tpc[0]);
         return 0;
       } else {
       send_debug_message(ob->o, "We say no match: np: %ld, strike %ld, date %u, expdate %u, pc: %c, tcp %c \n",
               np, strike, date, expdate, put_call[0], tpc[0]);
         return 1;
       }
}

int boe_parse_filled(rcntr * cnt, char *ack, con_int * ob)
{
    dart_order_obj* ute = (dart_order_obj*)cnt->raw;
    //boem* b = (boem*)ob->cparser;
    char num_bit_fields = *(ack + 69);
    char bit_one = *(ack + 70);
    char bit_two = *(ack + 71);
    char bit_three = *(ack + 72);
    char bit_four = *(ack + 73);
    char bit_five = *(ack + 74);
    char bit_six = *(ack + 75);
    char bit_seven = *(ack + 76);
    char bit_eight = *(ack + 77);
    char bit_nine = *(ack + 78);
    char bit_ten = *(ack + 79);
    char bit_thirteen = *(ack + 82);
    unsigned int is_present = 0;
    long strike = 0;
    uint32_t date = 0;
    uint32_t yearmon = 0;
    uint32_t day = 0;
    char put_call[8];
    int ret_val = 13;
    char liq[3];
    memset(liq, '\0', 3);
    int liq_len = 0;
    set_time(ute, ack, ob);
    set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    databuf_t *buff = get_databuf(ob->gk);
    long execid = 0;
    memcpy(&execid, ack + 38, 8);
    int last_shares = 0;
    memcpy(&last_shares, ack + 46, 4);
    int leaves_shares = 0;
    memcpy(&leaves_shares, ack + 58, 4);
    int l_cum = 0;
    int qty = 0;
    char side[2] ;
    char* extras = ack + 69 + num_bit_fields + 1;
    IS_BIT_SET(is_present, bit_one, 0x0);
    if(is_present) {
        memcpy(side, extras, 1);
        fill_in_rom_field(ute, ROM_SIDE, extras, 1);
        extras += 1;
    }
    IS_BIT_SET(is_present, bit_one, 0x2);
    if(is_present) {
      /*price*/
      extras += 8;
    }
    IS_BIT_SET(is_present, bit_one, 0x3);
    if(is_present) {
      /*execinst*/
      extras += 1;
    }
    IS_BIT_SET(is_present, bit_one, 0x4);
    if(is_present) {
      /*ordtype*/
      extras += 1;
    }
    IS_BIT_SET(is_present, bit_one, 0x5);
    if(is_present) {
      /*tif*/
      extras += 1;
    }
    IS_BIT_SET(is_present, bit_one, 0x6);
    if(is_present) {
      /*minqty*/
      extras += 4;
    }
    IS_BIT_SET(is_present, bit_one, 0x7);
    if(is_present) {
      /*maxreomvepct*/
      extras += 1;
    }
    IS_BIT_SET(is_present, bit_two, 0x0);
    if(is_present) {
        extras += 8;
    }
    IS_BIT_SET(is_present, bit_two, 0x6);
    if(is_present) {
      /*capacity*/
      extras += 1;
    }
    IS_BIT_SET(is_present, bit_two, 0x7);
    if(is_present) {
      /*contratrader*/
      extras += 4;
    }
    IS_BIT_SET(is_present, bit_three, 0x0);
    if(is_present) {
      /*account*/
      extras += 16;
    }
    IS_BIT_SET(is_present, bit_three, 0x1);
    if(is_present) {
      /*clearingfirm*/
      extras += 4;
    }
    IS_BIT_SET(is_present, bit_three, 0x2);
    if(is_present) {
      /*clearingaccount*/
      extras += 4;
    }
    IS_BIT_SET(is_present, bit_three, 0x3);
    if(is_present) {
      /*displayindicator*/
      extras += 1;
    }
    IS_BIT_SET(is_present, bit_three, 0x4);
    if(is_present) {
      /*maxfloor*/
      extras += 4;
    }
    IS_BIT_SET(is_present, bit_three, 0x6);
    if(is_present) {
      /*orderqty*/
      extras += 4;
    }
    IS_BIT_SET(is_present, bit_three, 0x7);
    if(is_present) {
      /*preventmatch*/
      extras += 3;
    }
    IS_BIT_SET(is_present, bit_four, 0x0);
    if(is_present) {
        date = 0x0;
        memcpy(&date, extras, 4);
        day = date % 100;
        yearmon = (date - day)/100;
        send_debug_message(ob->o, "Execution num_bit_fields? %d date: %d, YYYYMM: %d, DD: %d\n",
            num_bit_fields, date, yearmon, day);
        extras += 4;
        databuf_reset(buff);
        unsigned_dart_itoa(ute, yearmon,
                            ROM_EXPIRE_DATE,  buff);
        databuf_reset(buff);
        unsigned_dart_itoa(ute, day,
                            ROM_EXPIRE_DAY,  buff);
        databuf_reset(buff);
    }
    IS_BIT_SET(is_present, bit_four, 0x1);
    if(is_present) {
        strike = 0;
        memcpy(&strike, extras, 8);
        extras += 8;
        double lp = ((double) strike / 10000.0) + .0000001;
        int dlen = sprintf(buff->buffer, "%f", lp);
        set_rom_field(ute, ROM_STRIKE, buff->buffer, dlen);
        databuf_reset(buff);
    }
    IS_BIT_SET(is_present, bit_four, 0x2);
    if(is_present) {
      memcpy(&put_call, extras, 1);
      extras += 1;
      if(put_call[0] == '0') {
          set_rom_field(ute, ROM_P_C, "P", 1);
      } else {
          set_rom_field(ute, ROM_P_C, "C", 1);
      }
    }
    IS_BIT_SET(is_present, bit_four, 0x3);
    if(is_present) {
      /*openclose*/
      extras += 1;
    }
    IS_BIT_SET(is_present, bit_four, 0x4);
    if(is_present) {
      /*clordidbatch*/
      extras += 20;
    }
    IS_BIT_SET(is_present, bit_four, 0x5);
    if(is_present) {
      /*correctedsize*/
      extras += 4;
    }
    IS_BIT_SET(is_present, bit_five, 0x0);
    if(is_present) {
      /*origclordid*/
      extras += 20;
    }
    IS_BIT_SET(is_present, bit_five, 0x1);
    if(is_present) {
      /*leavesqty*/
      extras += 4;
    }
    IS_BIT_SET(is_present, bit_five, 0x2);
    if(is_present) {
      /*lastshares*/
      extras += 4;
    }
    IS_BIT_SET(is_present, bit_five, 0x3);
    if(is_present) {
      /*lastpx*/
      extras += 8;
    }
    IS_BIT_SET(is_present, bit_five, 0x4);
    if(is_present) {
      /*displayprice*/
      extras += 8;
    }
    IS_BIT_SET(is_present, bit_five, 0x5);
    if(is_present) {
      /*workingprice*/
      extras += 8;
    }
    IS_BIT_SET(is_present, bit_five, 0x6);
    if(is_present) {
      /*baseliquidityIndicator*/
      extras += 1;
    }
    IS_BIT_SET(is_present, bit_five, 0x7);
    if(is_present) {
      /*expiretime*/
      extras += 8;
    }
    IS_BIT_SET(is_present, bit_six, 0x0);
    if(is_present) {
      /*secondaryorderid*/
      extras += 8;
    }
    IS_BIT_SET(is_present, bit_six, 0x2);
    if(is_present) {
      /*cantracapacity*/
      extras += 1;
    }
    IS_BIT_SET(is_present, bit_six, 0x3);
    if(is_present) {
      /*attributedquote*/
      extras += 1;
    }
    IS_BIT_SET(is_present, bit_six, 0x5);
    if(is_present) {
      /*bulkorderids*/
      extras += 8;
    }
    IS_BIT_SET(is_present, bit_six, 0x6);
    if(is_present) {
      /*bulkrejectreasons*/
      extras += 1;
    }
    IS_BIT_SET(is_present, bit_seven, 0x0);
    if(is_present) {
      /*subliquidityindicator*/
      extras += 1;
    }
    
    IS_BIT_SET(is_present, bit_eight, 0x0);
    if(is_present && num_bit_fields >= 8) {
        if(extras[0] != 0x0) {
            memcpy(liq, extras, 2);
            FIND_NULL(liq, liq_len, 2);
        }
    } else {
        /*memcpy(liq, ack + 62, 2);
        FIND_NULL(liq, liq_len, 2);*/
    }
    extras += 2;
    IS_BIT_SET(is_present, bit_eight, 0x1);
    if(is_present) {
      /*echotext*/
      extras += 64;
    }
    IS_BIT_SET(is_present, bit_eight, 0x2);
    if(is_present) {
      /*stoppx*/
      extras += 8;
    }
    IS_BIT_SET(is_present, bit_eight, 0x3);
    if(is_present) {
      /*routingist*/
      extras += 4;
    }
    IS_BIT_SET(is_present, bit_eight, 0x4);
    if(is_present) {
      /*routestrategy*/
      extras += 6;
    }
    IS_BIT_SET(is_present, bit_eight, 0x5);
    if(is_present) {
      /*routedeliverymethod*/
      extras += 3;
    }
    IS_BIT_SET(is_present, bit_eight, 0x6);
    if(is_present) {
      /*exdestination*/
      extras += 1;
    }
    IS_BIT_SET(is_present, bit_nine, 0x0);
    if(is_present) {
      /*marketingfeecode*/
      extras += 2;
    }
    IS_BIT_SET(is_present, bit_nine, 0x1);
    if(is_present) {
      /*targetpartyid*/
      extras += 4;
    }
    IS_BIT_SET(is_present, bit_nine, 0x2);
    if(is_present) {
      /*auctionid*/
      extras += 8;
    }
    IS_BIT_SET(is_present, bit_nine, 0x5);
    if(is_present) {
      /*cmtanumber*/
      extras += 4;
    }
    IS_BIT_SET(is_present, bit_nine, 0x6);
    if(is_present) {
      /*crosstype*/
      extras += 1;
    }
    IS_BIT_SET(is_present, bit_nine, 0x7);
    if(is_present) {
      /*crossprioritization*/
      extras += 1;
    }
    IS_BIT_SET(is_present, bit_ten, 0x0);
    if(is_present) {
      /*crossid*/
      extras += 20;
    }
    IS_BIT_SET(is_present, bit_ten, 0x1);
    if(is_present) {
      /*allocqty*/
      extras += 4;
    }
    IS_BIT_SET(is_present, bit_ten, 0x2);
    if(is_present) {
      /*giveupfirmid*/
      extras += 4;
    }
    IS_BIT_SET(is_present, bit_ten, 0x3);
    if(is_present) {
      /*routingfirmid*/
      extras += 4;
    }
    IS_BIT_SET(is_present, bit_ten, 0x5);
    if(is_present) {
      /*crossexclusionindicator*/
      extras += 1;
    }
    IS_BIT_SET(is_present, bit_thirteen, 0x0);
    if(is_present) {
      /*cumqty*/
      extras += 4;
    }
    IS_BIT_SET(is_present, bit_thirteen, 0x1);
    if(is_present) {
      /*dayorderqty*/
      extras += 4;
    }
    IS_BIT_SET(is_present, bit_thirteen, 0x2);
    if(is_present) {
      /*daycumqty*/
      extras += 4;
    }
    IS_BIT_SET(is_present, bit_thirteen, 0x3);
    if(is_present) {
      /*avgpx*/
      extras += 8;
    }
    IS_BIT_SET(is_present, bit_thirteen, 0x4);
    if(is_present) {
      /*dayavgpx*/
      extras += 8;
    }
    IS_BIT_SET(is_present, bit_thirteen, 0x6);
    if(is_present) {
      /*drillthruprotection*/
      extras += 8;
    }
    IS_BIT_SET(is_present, bit_thirteen, 0x7);
    if(is_present) {
        char reptype = *(extras);
        if(reptype == '2') {
            //set_rom_field(ute, ROM_SPREAD, "6", 1);
            children* lch = ute->childs;
            if(lch) {
                int i = 0;
                int still_lookin = 1;
                dart_order_obj* ldoj = 0x0;
                do {
                    ldoj = (dart_order_obj*)lch->payload;
                    still_lookin = match_ex_with_child(ldoj, strike, date, put_call, buff, ob); 
                    databuf_reset(buff);
                    if(still_lookin > 0) {
                        lch = lch->next;
                        ++i;
                    }
                } while(lch && still_lookin);
                if(still_lookin == 0 && ldoj) {
                  memcpy(&qty, ldoj->positions[ROM_BIN_QTY].iov_base, 4);
                  memcpy(&l_cum, ldoj->positions[ROM_BIN_CUM].iov_base, 4);
                  dart_itoa(ldoj, last_shares, ROM_LAST_SHARES, buff);
                  databuf_reset(buff);
                  set_time(ldoj, ack, ob);
                  l_cum += last_shares;
                  ret_val = i;
                  long last_px = 0;//(long)*(ack + 50);
                  memcpy(&last_px, ack + 50, 8);
                  double lp = ((double) last_px / 10000.0) + .0000001;
                  int dlen = sprintf(buff->buffer, "%f", lp);
                  set_rom_field(ldoj, ROM_EXEC_PRICE, buff->buffer, dlen);
                  set_rom_field(ldoj, 0, ROM_ZERO_STATUS, 1);
                  if(l_cum >= qty && leaves_shares == 0) {
                    set_rom_field(ldoj, ROM_STATUS, "2", 1);
                  } else {
                    set_rom_field(ldoj, ROM_STATUS, "1", 1);
                  }
                  fill_in_rom_field(ldoj, ROM_BIN_CUM, &l_cum, 4);
                  if(liq_len > 0) { 
                      set_rom_field(ldoj, ROM_LIQ, liq, liq_len);
                  }
                  fill_in_rom_field(ldoj, ROM_SIDE, side, 1);
                  boe_set_ex_id(ldoj, execid, EXEC_ID, buff);
                  databuf_reset(buff);
                    dart_itoa(ldoj, l_cum, ROM_CUM, buff);
                    databuf_reset(buff);
                }
            } else {
                set_rom_field(ute, ROM_SPREAD, "6", 1);
                dart_itoa(ute, last_shares, ROM_LAST_SHARES, buff);
                databuf_reset(buff);
		if(leaves_shares >= 0 && leaves_shares < LEAVE_MAX) {
			dart_itoa(ute, leaves_shares, ROM_LEAVES, buff);
		}
                databuf_reset(buff);
                set_time(ute, ack, ob);
                if(liq_len > 0) { 
                    set_rom_field(ute, ROM_LIQ, liq, liq_len);
                }
                boe_set_ex_id(ute, execid, EXEC_ID, buff);
                databuf_reset(buff);
                if(leaves_shares == 0) {
                    set_rom_field(ute, ROM_STATUS, "2", 1);
                  } else {
                    set_rom_field(ute, ROM_STATUS, "1", 1);
                  }
            }
        } else if(reptype == '3') {
            set_rom_field(ute, ROM_SPREAD, "5", 1);
            if(ute->positions[ROM_BIN_QTY].iov_len > 0) {
                memcpy(&qty, ute->positions[ROM_BIN_QTY].iov_base, 4);
            } 
            if(ute->positions[ROM_BIN_CUM].iov_len > 0) {
                memcpy(&l_cum, ute->positions[ROM_BIN_CUM].iov_base, 4);
            }
            fill_in_rom_field(ute, ROM_BIN_LAST_SHARES, &last_shares, sizeof(int));
            dart_itoa(ute, last_shares, ROM_LAST_SHARES, buff);
            databuf_reset(buff);
	    if(leaves_shares >= 0 && leaves_shares < LEAVE_MAX) {
		    dart_itoa(ute, leaves_shares, ROM_LEAVES, buff);
	    }
            databuf_reset(buff);

            long last_px = 0;//(long)*(ack + 50);
            memcpy(&last_px, ack + 50, 8);
            double lp = ((double) last_px / 10000.0) + .0000001;
            int dlen = sprintf(buff->buffer, "%f", lp);
            fill_in_rom_field(ute, ROM_EXEC_PRICE, buff->buffer, dlen);
            ret_val = 13;
            l_cum += last_shares;
            set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
            fill_in_rom_field(ute, ROM_BIN_CUM, &l_cum, 4);
            if(liq_len > 0) {
                set_rom_field(ute, ROM_LIQ, liq, liq_len);
            }
            boe_set_ex_id(ute, execid, EXEC_ID, buff);
            databuf_reset(buff);
            dart_itoa(ute, l_cum, ROM_CUM, buff);
            databuf_reset(buff);
        } else {
            if(ute->positions[ROM_BIN_QTY].iov_len > 0) {
              memcpy(&qty, ute->positions[ROM_BIN_QTY].iov_base, 4);
            }
            if(ute->positions[ROM_BIN_CUM].iov_len > 0) {
              memcpy(&l_cum, ute->positions[ROM_BIN_CUM].iov_base, 4);
            }
          fill_in_rom_field(ute, ROM_BIN_LAST_SHARES, &last_shares, sizeof(int));
          dart_itoa(ute, last_shares, ROM_LAST_SHARES, buff);
          databuf_reset(buff);
	  if(leaves_shares >= 0 && leaves_shares < LEAVE_MAX) {
		  dart_itoa(ute, leaves_shares, ROM_LEAVES, buff);
	  }
          databuf_reset(buff);

          long last_px = 0;//(long)*(ack + 50);
          memcpy(&last_px, ack + 50, 8);
          double lp = ((double) last_px / 10000.0) + .0000001;
          int dlen = sprintf(buff->buffer, "%f", lp);
          fill_in_rom_field(ute, ROM_EXEC_PRICE, buff->buffer, dlen);
            l_cum += last_shares;
            fill_in_rom_field(ute, ROM_BIN_CUM, &l_cum, 4);
            set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
            if(liq_len > 0) {
                set_rom_field(ute, ROM_LIQ, liq, liq_len);
            }
            boe_set_ex_id(ute, execid, EXEC_ID, buff);
            databuf_reset(buff);
            dart_itoa(ute, l_cum, ROM_CUM, buff);
            databuf_reset(buff);
        }
    } else {
            if(ute->positions[ROM_BIN_QTY].iov_len > 0) {
              memcpy(&qty, ute->positions[ROM_BIN_QTY].iov_base, 4);
            }
            if(ute->positions[ROM_BIN_CUM].iov_len > 0) {
              memcpy(&l_cum, ute->positions[ROM_BIN_CUM].iov_base, 4);
            }
          fill_in_rom_field(ute, ROM_BIN_LAST_SHARES, &last_shares, sizeof(int));
          dart_itoa(ute, last_shares, ROM_LAST_SHARES, buff);
          databuf_reset(buff);
	  if(leaves_shares >= 0 && leaves_shares < LEAVE_MAX) {
		  dart_itoa(ute, leaves_shares, ROM_LEAVES, buff);
	  }
          databuf_reset(buff);

          long last_px = 0;//(long)*(ack + 50);
          memcpy(&last_px, ack + 50, 8);
          double lp = ((double) last_px / 10000.0) + .0000001;
          int dlen = sprintf(buff->buffer, "%f", lp);
          fill_in_rom_field(ute, ROM_EXEC_PRICE, buff->buffer, dlen);
            l_cum += last_shares;
            fill_in_rom_field(ute, ROM_BIN_CUM, &l_cum, 4);
            set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
            if(liq_len > 0) {
                set_rom_field(ute, ROM_LIQ, liq, liq_len);
            }
            boe_set_ex_id(ute, execid, EXEC_ID, buff);
            databuf_reset(buff);
            dart_itoa(ute, l_cum, ROM_CUM, buff);
            databuf_reset(buff);
    }
    int should_len = 0x0;
    char* sc =  get_rom_field(ute, ROM_BIN_PENDING_ID, &should_len);
    uint8_t should_cancel = 0x0;
    if(should_len > 0) {
        memcpy(&should_cancel, sc, 1);
    }
    if (l_cum >= qty && leaves_shares == 0) {
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
            ret_val = -1;
        }
    } else if(!is_spread(ute) && should_cancel && leaves_shares == 0) {
        send_debug_message(ob->o, "We are about to generate cnl: leaves: %d, last: %d, cum: %d, our size: %d \n", 
                leaves_shares, last_shares, cnt->cum, cnt->size);
        set_rom_field(ute, ROM_STATUS, "2", 1);
        ret_val = -2;
    } else {
        set_rom_field(ute, ROM_STATUS, "1", 1);
    }
    destroy_gk_db(ob->gk, buff);
    return ret_val;
}

