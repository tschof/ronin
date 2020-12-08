/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

#include "boe_master.h"
#include "rom_to_boe.h"
#include "boe_utils.h"
#include "rex_constants.h"
#include "tm_to_str.h"



static void set_header(void*p, dart_order_obj* doj)
{
  set_rom_field(doj, 0, "ClOrdid", 7);
  set_rom_field(doj, 2, "Ordid", 5);
  set_rom_field(doj, 4, "CustomerOrderTime", 17);
  set_rom_field(doj, 6, "TransactTime", 12);
  set_rom_field(doj, 8, "Symbol", 6);
  set_rom_field(doj, 10, "SecurityType", 12);
  set_rom_field(doj, 12, "OrdType", 7);
  set_rom_field(doj, 14, "MaturityMonth", 13);
  set_rom_field(doj, 16, "MaturityDay", 11);
  set_rom_field(doj, 18, "OnBehalfOfCompID", 16);
  set_rom_field(doj, 20, "CMTANumber", 10);
  set_rom_field(doj, 22, "Price", 5);
  set_rom_field(doj, 24, "StopPX", 6);
  set_rom_field(doj, 26, "OrderQty", 8);
  set_rom_field(doj, 28, "Side", 4);
  set_rom_field(doj, 30, "CTICode", 7);
  set_rom_field(doj, 32, "OrderCapacity", 13);
  set_rom_field(doj, 34, "Operatorid", 10);
  set_rom_field(doj, 36, "Account", 7);
  set_rom_field(doj, 38, "TimeInForce", 11);
  set_rom_field(doj, 40, "ManualOrderIndicator", 20);
  set_rom_field(doj, 42, "RiskReset", 9);
  set_rom_field(doj, 44, "PreventMatch", 12);
  set_rom_field(doj, 46, "SpecialInstruction", 18);
  set_rom_field(doj, 48, "CancelOrderIdInst", 17);
  set_rom_field(doj, 50, "TransactTime", 12);
  set_rom_field(doj, 52, "ReplacedOrderType", 17);
  set_rom_field(doj, 54, "NewReplacePrice", 15);
  set_rom_field(doj, 56, "NewStopPX", 9);
  set_rom_field(doj, 58, "RejectReason", 12);
  set_rom_field(doj, 60, "NewReplacedQty", 14);
  set_rom_field(doj, 62, "ExecType", 8);
  set_rom_field(doj, 64, "ExecID", 6);
  set_rom_field(doj, 66, "SendingTime", 11);
  set_rom_field(doj, 68, "FillPrice", 9);
  set_rom_field(doj, 70, "TradeQty", 8);
  set_rom_field(doj, 72, "LeavesQty", 9);
  set_rom_field(doj, 74, "ExecTransType", 13);
  set_rom_field(doj, 76, "ExecRefID", 9);
}
static void set_ret_vals(boem* master, char* message, int rlen, 
    dart_order_obj* doj) 
{
    char* clordid = (char*)message + 18;
    int clordid_len = 0;
    {
        FIND_NULL(clordid, clordid_len, 20);
    }
    set_rom_field(doj, 0, clordid, clordid_len);
    long tt = 0;
    memcpy(&tt, (message + 10), 8);
    char itoa_buff[64];
    int date_len = boe_to_rom_time(tt, itoa_buff);
    set_rom_field(doj, 50, itoa_buff, date_len);
    set_rom_field(doj, 66, itoa_buff, date_len);
}

static void set_ord_id(boem* master, char* message, int rlen,
    dart_order_obj* doj) 
{
    con_int* ob = master->ci;
    long bordid = 0;
    memcpy(&bordid, message + 38, 8);
    databuf_t *dbuff = get_databuf(ob->gk);
    dart_itoa(doj, bordid,2, dbuff);
    destroy_gk_db(ob->gk, dbuff);
}
static void set_fields_for_open(boem* master, char* message,
    int rlen, dart_order_obj* doj)
{
  set_ret_vals(master, message, rlen, doj);
  set_ord_id(master, message, rlen, doj);
  set_rom_field(doj, 62, "0", 1);
}
static void set_fields_for_reject(boem* master, char* message,
    int rlen, dart_order_obj* doj)
{
  set_ret_vals(master, message, rlen, doj);
  boerej* rej = (boerej*) message;
  set_rom_field(doj, 62, "8", 1);
  set_rom_field(doj, 58,&rej->rr, 1);
}
static void set_fields_for_replace(boem* master, char* message,
    int rlen, dart_order_obj* doj)
{
  set_ret_vals(master, message, rlen, doj);
  set_ord_id(master, message, rlen, doj);
  set_rom_field(doj, 62, "5", 1);
}
static void set_fields_for_replace_rej(boem* master, char* message,
    int rlen, dart_order_obj* doj)
{
  set_ret_vals(master, message, rlen, doj);
  boemodrej* rej = (boemodrej*) message;
  set_rom_field(doj, 62, "8", 1);
  set_rom_field(doj, 58,&rej->rr, 1);
}
static void set_fields_for_out(boem* master, char* message, 
    int rlen, dart_order_obj* doj)
{
  set_ret_vals(master, message, rlen, doj);
  set_rom_field(doj, 62, "4", 1);
}
static void set_fields_for_cnl_rej(boem* master, char* message, 
    int rlen, dart_order_obj* doj)
{
  set_ret_vals(master, message, rlen, doj);
  boecnlrej* rej = (boecnlrej*) message;
  set_rom_field(doj, 62, "8", 1);
  set_rom_field(doj, 58,&rej->rr, 1);
}
static void set_fields_for_fill(boem* master, char* message, 
    int rlen, dart_order_obj* doj)
{
  set_ret_vals(master, message, rlen, doj);
  boefill* rej = (boefill*) message;
  databuf_t *buff = (databuf_t *) get_databuf(master->gk);
  dart_itoa(doj, rej->execid,64, buff);
  databuf_reset(buff);
  dart_itoa(doj, rej->lshares,70, buff);
  databuf_reset(buff);
  dart_itoa(doj, rej->leaves,72, buff);//Leaves
  databuf_reset(buff);

  if(rej->leaves > 0) {
    set_rom_field(doj, 62, "1", 1);
  } else {
    set_rom_field(doj, 62, "2", 1);
  }
  {
    char buff[64];
    memset(buff, '\0', 64);
    double lp = ((double) rej->lprice/ 10000.0) + .0000001;
    int dlen = sprintf(buff, "%f", lp);
    set_rom_field(doj, 68, buff, dlen);
  }
  destroy_gk_db(master->gk, buff);
}
static void set_fields_for_bust(boem* master, char* message, 
    int rlen, dart_order_obj* doj)
{
  set_ret_vals(master, message, rlen, doj);
  set_ord_id(master, message, rlen, doj);
  boebust* rej = (boebust*) message;
  databuf_t *buff = (databuf_t *) get_databuf(master->gk);
  dart_itoa(doj, rej->execid,64, buff);
  dart_itoa(doj, rej->execid,76, buff);
  databuf_reset(buff);
  dart_itoa(doj, rej->lshares,70, buff);
  databuf_reset(buff);

  if(rej->real_price == 0) {
    set_rom_field(doj, 74, "1", 1);
  } else {
    set_rom_field(doj, 74, "2", 1);
  }
  {
    char buff[64];
    memset(buff, '\0', 64);
    double lp = ((double) rej->lprice/ 10000.0) + .0000001;
    int dlen = sprintf(buff, "%f", lp);
    set_rom_field(doj, 68, buff, dlen);
  }
  destroy_gk_db(master->gk, buff);
}
static void set_time_for_tph(time_t time_sec, int time_micro, 
    dart_order_obj* doj, int rom_field)
{
    char t_str[32];
    memset(t_str, '\0', 32);
    struct tm lm;
    gmtime_r(&time_sec, &lm);
    if(tm_to_str_millis(&lm, (time_micro/1000),
                     t_str, 32) == 0) {
        set_rom_field(doj, rom_field, t_str, 21);
    }
}
static void set_fields_for_new(boem* master, char* message, 
    int rlen, dart_order_obj* doj, time_t sec, int micro)
{
  if(micro > 100) {
    set_time_for_tph(sec, micro - 100, doj,4); 
  } else {
    set_time_for_tph(sec -1 , micro + 1000, doj,4); 
  }
  set_time_for_tph(sec, micro, doj,6); 
  char* offset = message + sizeof(struct boe_new_ord);
  int is_present = 0x0;
  int used_clr_acc = 0x0;
  boeno* ord = (boeno*)message;
  {
    int clordid_len = 0x0;
    FIND_NULL(ord->ord_id, clordid_len, 20);
    set_rom_field(doj, 0,ord->ord_id, clordid_len);
  }
  set_rom_field(doj, 28, &ord->side, 1);
  databuf_t *buff = (databuf_t *) get_databuf(master->gk);
  dart_itoa(doj, ord->qty,26, buff);
  destroy_gk_db(master->gk, buff);
  IS_BIT_SET(is_present, ord->bf1, 0x0);
  if(is_present) {
    set_rom_field(doj, 18, offset, 4);
    offset += 4;
  }
  IS_BIT_SET(is_present, ord->bf1, 0x1);
  if(is_present) {
    set_rom_field(doj, 36, offset, 4);
    offset += 4;
    used_clr_acc = 1;
  }
  IS_BIT_SET(is_present, ord->bf1, 0x2);
  if(is_present) {
    char buff[64];
    memset(buff, '\0', 64);
    long last_px = 0;//(long)*(ack + 50);
    memcpy(&last_px, offset, 8);
    //fill_in_rom_field(ldoj, ROM_BIN_LAST_PRICE, &last_px, sizeof(int));
    double lp = ((double) last_px / 10000.0) + .0000001;
    int dlen = sprintf(buff, "%f", lp);
    set_rom_field(doj, 22, buff, dlen);
    offset += 8;
  }
  IS_BIT_SET(is_present, ord->bf1, 0x4);
  if(is_present) {
    set_rom_field(doj,12, offset, 1);
    offset += 1;
  }
  IS_BIT_SET(is_present, ord->bf1, 0x5);
  if(is_present) {//TIF
    set_rom_field(doj,38, offset, 1);
    offset += 1;
  }
  IS_BIT_SET(is_present, ord->bf1, 0x6);
  if(is_present) {
    /*    uint32_t minqty = 0x0;
    memcpy(offset, &minqty, 4);
    unsigned_dart_itoa(doj, minqty, rom_tag,  buff);*/
    offset += 4;
  }
  IS_BIT_SET(is_present, ord->bf2, 0x0);
  if(is_present) {
    char symbol[8];
    int slen = 0;
    memset(symbol, '\0', 8);
    memcpy(symbol, offset, 8);
    FIND_NULL(symbol, slen, 8);
    set_rom_field(doj, 8, symbol,slen);
    offset += 8;
  }
  IS_BIT_SET(is_present, ord->bf2, 0x6);
  if(is_present) {//Capacity
    set_rom_field(doj,32,offset, 1);
    offset += 1;
  }
  IS_BIT_SET(is_present, ord->bf3, 0x0);
  if(is_present) {
    if(used_clr_acc == 0x0) {
      char symbol[16];
      int slen = 0;
      memset(symbol, '\0', 16);
      memcpy(symbol, offset, 15);
      FIND_NULL(symbol, slen, 15);
      set_rom_field(doj, 36, symbol,slen);
    }
    offset += 16;
  }
  IS_BIT_SET(is_present, ord->bf3, 0x5);
  if(is_present) {
    set_rom_field(doj, 44, offset,3);//Prevent
    offset += 3;
  }
  IS_BIT_SET(is_present, ord->bf4, 0x4);
  if(is_present) {
    offset += 1;
  }
  IS_BIT_SET(is_present, ord->bf4, 0x5);
  if(is_present) {
    set_rom_field(doj, 20, offset,4);//CMTA
    offset += 4;
  }
  IS_BIT_SET(is_present, ord->bf6, 0x1);
  if(is_present) {
    char buff[64];
    memset(buff, '\0', 64);
    long last_px = 0;
    memcpy(&last_px, offset, 8);
    double lp = ((double) last_px / 10000.0) + .0000001;
    int dlen = sprintf(buff, "%f", lp);
    set_rom_field(doj, 24, buff, dlen);
    offset += 8;
  }
  set_rom_field(doj, 30, offset, 1);//CTI
  offset += 1;
  set_rom_field(doj, 40, offset, 1);//Man
  offset += 1;
  {
    char operator[18];
    int olen = 0;
    memset(operator, '\0', 18);
    memcpy(operator, offset, 18);
    FIND_NULL(operator, olen, 18);
    set_rom_field(doj, 34, operator,olen);
  }
}
static void set_fields_for_cnl(boem* master, char* message, 
    int rlen, dart_order_obj* doj, time_t sec, int micro)
{
  set_time_for_tph(sec, micro, doj,6); 
  char* offset = message + sizeof(struct boe_cnl_ord);
  int is_present = 0x0;
  boecnl* ord = (boecnl*)message;
  {
    int clordid_len = 0x0;
    FIND_NULL(ord->orig_id, clordid_len, 20);
    set_rom_field(doj, 0,ord->orig_id, clordid_len);
    set_rom_field(doj, 48,ord->orig_id, clordid_len);
  }
  IS_BIT_SET(is_present, ord->bf1, 0x0);
  if(is_present) {
    set_rom_field(doj, 18, offset, 4);
    offset += 4;
  }
  set_rom_field(doj, 40, offset, 1);//Man
  offset += 1;
  {
    char operator[18];
    int olen = 0;
    memset(operator, '\0', 18);
    memcpy(operator, offset, 18);
    FIND_NULL(operator, olen, 18);
    set_rom_field(doj, 34, operator,olen);
  }

}
static void set_fields_for_rpl(boem* master, char* message, 
    int rlen, dart_order_obj* doj, time_t sec, int micro)
{
  set_time_for_tph(sec, micro, doj,6); 

  char* offset = message + sizeof(struct boe_rpl_ord);
  int is_present = 0x0;
  boerpl* ord = (boerpl*)message;
  {
    int clordid_len = 0x0;
    int ord_id_len = 0x0;
    FIND_NULL(ord->orig_id, clordid_len, 20);
    {
      FIND_NULL(ord->orig_id, ord_id_len, 20);
    }
    set_rom_field(doj, 0,ord->ord_id, ord_id_len);
    set_rom_field(doj, 48,ord->orig_id, clordid_len);
  }
  IS_BIT_SET(is_present, ord->bf1, 0x0);
  if(is_present) {
    set_rom_field(doj, 18, offset, 4);
    offset += 4;
  }
  databuf_t *buff = (databuf_t *) get_databuf(master->gk);
  uint32_t qty = 0x0;
  memcpy(&qty, offset, 4);
  dart_itoa(doj, qty,60, buff);
  destroy_gk_db(master->gk, buff);
  offset += 4;
  {
    char buff[64];
    memset(buff, '\0', 64);
    long last_px = 0;
    memcpy(&last_px, offset, 8);
    double lp = ((double) last_px / 10000.0) + .0000001;
    int dlen = sprintf(buff, "%f", lp);
    set_rom_field(doj, 54, buff, dlen);
    offset += 8;
  }
  IS_BIT_SET(is_present, ord->bf1, 0x4);
  if(is_present) {
    set_rom_field(doj,52, offset, 1);
    offset += 1;
  }
  IS_BIT_SET(is_present, ord->bf1, 0x7);
  if(is_present) {
    set_rom_field(doj, 28, &offset, 1);
    offset += 1;
  }
  IS_BIT_SET(is_present, ord->bf2, 0x1);
  if(is_present) {
    char buff[64];
    memset(buff, '\0', 64);
    long last_px = 0;//Stop
    memcpy(&last_px, offset, 8);
    double lp = ((double) last_px / 10000.0) + .0000001;
    int dlen = sprintf(buff, "%f", lp);
    set_rom_field(doj, 56, buff, dlen);
    offset += 8;
  }
  set_rom_field(doj, 40, offset, 1);//Man
  offset += 1;
  {
    char operator[18];
    int olen = 0;
    memset(operator, '\0', 18);
    memcpy(operator, offset, 18);
    FIND_NULL(operator, olen, 18);
    set_rom_field(doj, 34, operator,olen);
  }
}

void cfe_audit_trail(void* p, char* message, int rlen, dart_order_obj* doj,
    time_t sec, int micro)
{
  boem* master = (boem*)p;
  if(rlen == 0) {
    set_header(p, doj);
  } else {
    uint8_t munit = 0x0;
    uint32_t seq = 0x0;
    unsigned char mtype = 0x0;
    unsigned short mess_len = 0x0;
    memcpy(&mtype, message + 4, 1);
    memcpy(&mess_len, message + 2, 2);
    memcpy(&munit, message + 5, 1);
    memcpy(&seq, message + 6, 4);
    switch(mtype) {
        case 0x25:
        case 0x28:
          set_fields_for_open(master, message, rlen, doj);
          break;
          case 0x26: //rejected
          set_fields_for_reject(master, message, rlen, doj);
          break;
          case 0x27: //modified
          set_fields_for_replace(master, message, rlen, doj);
          break;
          case 0x29: //replace rejected
          set_fields_for_replace_rej(master, message, rlen, doj);
          break;
          case 0x2A:  //cancelled
          set_fields_for_out(master, message, rlen, doj);
          break;
          case 0x2B: //cancel rejected
          set_fields_for_cnl_rej(master, message, rlen, doj);
          break;
          case 0x2C: //execution
          set_fields_for_fill(master, message, rlen, doj);
          break;
          case 0x2D: //Trade cancel or correct
          set_fields_for_bust(master, message, rlen, doj);
          break;
          case 0x38:
          set_fields_for_new(master, message, rlen, doj, sec, micro);
          break;
          case 0x39:
          set_fields_for_cnl(master, message, rlen, doj, sec, micro);
          break;
          case 0x3A:
          set_fields_for_rpl(master, message, rlen, doj, sec, micro);
          break;
    }
  }
}
