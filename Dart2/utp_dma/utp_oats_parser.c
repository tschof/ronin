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
#include <arpa/inet.h>

#include "utp_dma.h"
#include "utp.h"
#include "utp_oats_parser.h"
#include "databuf.h"
#include "connection_interface.h"
#include "dart_handler.h"
#include "rex_constants.h"


#define HOUR_MILLIS 3600000
#define MIN_MILLIS 60000
#define EST_OFF 5
#define EDT_OFF 4

const char *yr_str[] =
    { "2011", "2012", "2013", "2014", "2015", "2016", "2017", "2018",
    "2019"
};

const char *digit_str[] =
    { "00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
    "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
    "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
    "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
    "50", "51", "52", "53", "54", "55", "56", "57", "58", "59"
};

static char *today = 0;
static int day_light_sav = 0;


static void init_today()
{
    today = calloc(50, 1);
    char *buf = today;
    time_t rawtime;
    struct tm *ti;
    time(&rawtime);
    struct tm lm;
    ti = gmtime_r(&rawtime, &lm);
    ti = &lm;
    const char *idx = yr_str[ti->tm_year - 111];
    *buf++ = *idx++;
    *buf++ = *idx++;
    *buf++ = *idx++;
    *buf++ = *idx++;
    idx = digit_str[ti->tm_mon + 1];
    *buf++ = *idx++;
    *buf++ = *idx++;
    idx = digit_str[ti->tm_mday];
    *buf++ = *idx++;
    *buf++ = *idx++;
    *buf++ = '-';
    ti = localtime_r(&rawtime, &lm);
    ti = &lm;
    day_light_sav = ti->tm_isdst;
    if (day_light_sav == 0) {
        day_light_sav = EST_OFF;
    } else {
        day_light_sav = EDT_OFF;
    }

}

static void set_date_str(char *output, int tt)
{
    if (today == 0) {
        init_today();
    }
    char *start = output;
    memcpy(start, today, 9);
    start += 9;
    int total_secs = tt / 1000;
    int hour = ((total_secs / 3600) + day_light_sav) % 24;
    int min = (total_secs % 3600) / 60;
    int sec = (total_secs % 3600) % 60;
    const char *idx = digit_str[hour];
    *start++ = *idx++;
    *start++ = *idx++;
    *start++ = ':';
    idx = digit_str[min];
    *start++ = *idx++;
    *start++ = *idx++;
    *start++ = ':';
    idx = digit_str[sec];
    *start++ = *idx++;
    *start++ = *idx++;
}

static double utp_to_rom_price(int data, char ps)
{
    double d = 10000;
    int a = 0;
    switch (ps) {
    default:
        d = 10000;
        break;
    case '0':
        d = 1;
        break;
    case '1':
        d = 10;
        break;
    case '2':
        d = 100;
        break;
    case '3':
        d = 1000;
        break;
    case '6':
        d = 1000000;
        break;
    }
    a = ntohl(data);
    d = ((double) a / d) + .0000001;
    return d;
}

static void set_defaults(dart_order_obj* doj, ex_config* ex)
{
    int ret_len = 0;
    char* dest_id = get_config_val(ex, "SERVER", 6,
                          "Dest ID", 7, &ret_len);
    if(ret_len > 0) {
        set_rom_field(doj, ROM_DESTID, dest_id, ret_len);
        set_rom_field(doj, ROM_DEST, dest_id, ret_len);
    }
    char* clr_id = get_config_val(ex, "SERVER", 6,
                            "CLR ID",6, &ret_len);
    if(ret_len > 0) {
        set_rom_field(doj, ROM_CLR_ID, clr_id, ret_len);
                
    }
    char* firm = get_config_val(ex, "SERVER", 6,
                            "FIRM",4, &ret_len);
    if(ret_len > 0) {
        set_rom_field(doj, ROM_FIRM, firm, ret_len);
    }
}
void utp_cs_parse_oats(void* p, char* d, int dlen, dart_order_obj* doj)
{
    void* d_off = (void*)d;
    con_int* ci = (con_int*)p;
    con_int* oat = get_compliance_con(ci->gk);
    dartp* dp = (dartp*)oat->cparser;
    doj->dest = dp;
    databuf_t* db = get_databuf(ci->gk);
    set_defaults(doj, ci->ex);
    short type = 0;
    memcpy(&type, d, 2);
    type = ntohs(type);
    switch(type) {
        case 0x0091:
            {
                set_rom_field(doj, 0, "S", 1);
                set_rom_field(doj, ROM_STATUS, "13", 2);
                utpack* ack = d_off;
                int tt = ack->time;
                tt = ntohl(tt);
                set_date_str(db->buffer, tt);
                fill_in_rom_field(doj, EXEC_TIME, db->buffer, 17);
                int sym_len = get_utp_field_len(ack->clordid, 17);
                if(sym_len > 0) {
                    set_rom_field(doj, ROM_CLORDID, ack->clordid, sym_len);
                }
                sym_len = get_utp_field_len(ack->account, 10);
                if(sym_len > 0) {
                    set_rom_field(doj, ROM_CLR_ACC, ack->account, sym_len);
                }
                sym_len = get_utp_field_len(ack->target, 5);
                if(sym_len > 0) {
                    set_rom_field(doj, ROM_TRADER, ack->target, sym_len);
                    set_rom_field(doj, ROM_TRADE_FOR, ack->target, sym_len);
                    set_rom_field(doj, ROM_LOCAL_ACC, ack->target, sym_len);
                }
            }
            break;
        case 0x00A1:
            {
                set_rom_field(doj, 0, "S", 1);
                set_rom_field(doj, ROM_STATUS, "6", 1);
                utpcnlack* ack = d_off;
                int tt = ack->time;
                tt = ntohl(tt);
                set_date_str(db->buffer, tt);
                fill_in_rom_field(doj, EXEC_TIME, db->buffer, 17);
                int sym_len = get_utp_field_len(ack->clordid, 17);
                if(sym_len > 0) {
                    set_rom_field(doj, ROM_CLORDID, ack->clordid, sym_len);
                }
                sym_len = get_utp_field_len(ack->account, 10);
                if(sym_len > 0) {
                    set_rom_field(doj, ROM_CLR_ACC, ack->account, sym_len);
                }
                sym_len = get_utp_field_len(ack->target, 5);
                if(sym_len > 0) {
                    set_rom_field(doj, ROM_TRADER, ack->target, sym_len);
                    set_rom_field(doj, ROM_TRADE_FOR, ack->target, sym_len);
                    set_rom_field(doj, ROM_LOCAL_ACC, ack->target, sym_len);
                }
            }
            break;
        case 0x00D1:
            {
                set_rom_field(doj, 0, "S", 1);
                set_rom_field(doj, ROM_STATUS, "4", 1);
                utpout* ack = d_off;
                int tt = ack->time;
                tt = ntohl(tt);
                set_date_str(db->buffer, tt);
                fill_in_rom_field(doj, EXEC_TIME, db->buffer, 17);
                int sym_len = get_utp_field_len(ack->orig_clordid, 17);
                if(sym_len > 0) {
                    set_rom_field(doj, ROM_CLORDID, ack->orig_clordid, sym_len);
                }
                sym_len = get_utp_field_len(ack->account, 10);
                if(sym_len > 0) {
                    set_rom_field(doj, ROM_CLR_ACC, ack->account, sym_len);
                }
                sym_len = get_utp_field_len(ack->target, 5);
                if(sym_len > 0) {
                    set_rom_field(doj, ROM_TRADER, ack->target, sym_len);
                    set_rom_field(doj, ROM_TRADE_FOR, ack->target, sym_len);
                    set_rom_field(doj, ROM_LOCAL_ACC, ack->target, sym_len);
                }
            }
            break;
        case 0x00B1:
            break;
        case 0x00E1:
            break;
        case 0x00F1:
            {
                set_rom_field(doj, 0, "S", 1);
                //set_rom_field(doj, 14, "4", 1);
                utprej* ack = d_off;
                int tt = ack->time;
                tt = ntohl(tt);
                set_date_str(db->buffer, tt);
                fill_in_rom_field(doj, EXEC_TIME, db->buffer, 17);
                if(ack->rej_msg_type == '1') {
                    set_rom_field(doj, ROM_STATUS, "8", 1);
                } else {
                    set_rom_field(doj, ROM_STATUS, "14", 2);
                }
                int sym_len = get_utp_field_len(ack->clordid, 17);
                if(sym_len > 0) {
                    set_rom_field(doj, ROM_PENDING_ID, ack->clordid, sym_len);
                }
                sym_len = get_utp_field_len(ack->orig_clordid, 17);
                if(sym_len > 0) {
                    set_rom_field(doj, ROM_CLORDID, ack->orig_clordid, sym_len);
                } else {
                    sym_len = get_utp_field_len(ack->clordid, 17);
                    if(sym_len > 0) {
                        set_rom_field(doj, ROM_CLORDID, ack->clordid, sym_len);
                    }
                }
                sym_len = get_utp_field_len(ack->account, 10);
                if(sym_len > 0) {
                    set_rom_field(doj, ROM_CLR_ACC, ack->account, sym_len);
                }
                sym_len = get_utp_field_len(ack->text, 40);
                if(sym_len > 0) {
                    set_rom_field(doj, ROM_TEXT, ack->text, sym_len);
                }
                sym_len = get_utp_field_len(ack->target, 5);
                if(sym_len > 0) {
                    set_rom_field(doj, ROM_TRADER, ack->target, sym_len);
                    set_rom_field(doj, ROM_TRADE_FOR, ack->target, sym_len);
                    set_rom_field(doj, ROM_LOCAL_ACC, ack->target, sym_len);
                }
            }
            break;
        case 0x0081:
            {
                set_rom_field(doj, 0, "S", 1);
                utpfill* fill = d_off;
                int tt = fill->time;
                tt = ntohl(tt);
                set_date_str(db->buffer, tt);
                fill_in_rom_field(doj, EXEC_TIME, db->buffer, 17);
                databuf_reset(db);
                int c = fill->leaves;
                c = ntohl(c);
                if(c > 0) {
                    set_rom_field(doj, ROM_STATUS, "1", 1);
                } else {
                    set_rom_field(doj, ROM_STATUS, "2", 1);
                }
                unsigned_dart_itoa(doj, c, ROM_LEAVES, db);
                databuf_reset(db);
                c = fill->last;
                c = ntohl(c);
                unsigned_dart_itoa(doj, c, ROM_LAST_SHARES, db);
                databuf_reset(db);
                c = fill->price;
                double p = utp_to_rom_price(c, fill->pscale);
                int dlen = sprintf(db->buffer, "%f", p);
                fill_in_rom_field(doj, ROM_EXEC_PRICE, db->buffer, dlen);
                databuf_reset(db);
                set_rom_field(doj, ROM_SIDE, &fill->side, 1);
                dlen = get_utp_field_len(fill->billing_rate, 6);
                if(dlen > 0) {
                    set_rom_field(doj, ROM_LIQ, fill->billing_rate,dlen);
                }
                dlen = get_utp_field_len(fill->execid, 10);
                if(dlen > 0) {
                    set_rom_field(doj, EXEC_ID, fill->execid, dlen);
                }
                dlen = get_utp_field_len(fill->clordid, 17);
                if(dlen > 0) {
                    set_rom_field(doj, ROM_CLORDID, fill->clordid, dlen);
                }
                dlen = get_utp_field_len(fill->target, 5);
                if(dlen > 0) {
                    set_rom_field(doj, ROM_TRADER, fill->target, dlen);
                    set_rom_field(doj, ROM_TRADE_FOR, fill->target, dlen);
                    set_rom_field(doj, ROM_LOCAL_ACC, fill->target, dlen);
                }
            }
            break;
        case 0x0101:
            {
                set_rom_field(doj, 0, "S", 1);
                utpvfill* fill = d_off;
                int tt = fill->time;
                tt = ntohl(tt);
                set_date_str(db->buffer, tt);
                fill_in_rom_field(doj, EXEC_TIME, db->buffer, 17);
                databuf_reset(db);
                int c = fill->leaves;
                c = ntohl(c);
                if(c > 0) {
                    set_rom_field(doj, ROM_STATUS, "1", 1);
                } else {
                    set_rom_field(doj, ROM_STATUS, "2", 1);
                }
                unsigned_dart_itoa(doj, c, ROM_LEAVES, db);
                databuf_reset(db);
                c = fill->last;
                c = ntohl(c);
                unsigned_dart_itoa(doj, c, ROM_LAST_SHARES, db);
                databuf_reset(db);
                c = fill->lprice;
                double p = utp_to_rom_price(c, fill->lppscale);
                int dlen = sprintf(db->buffer, "%f", p);
                fill_in_rom_field(doj, ROM_PRICE, db->buffer, dlen);
                databuf_reset(db);
                set_rom_field(doj, ROM_SIDE, &fill->side, 1);
                dlen = get_utp_field_len(fill->billing_rate, 6);
                if(dlen > 0) {
                    set_rom_field(doj, ROM_LIQ, fill->billing_rate,dlen);
                }
                dlen = get_utp_field_len(fill->exec_id, 10);
                if(dlen > 0) {
                    set_rom_field(doj, EXEC_ID, fill->exec_id, dlen);
                }
                dlen = get_utp_field_len(fill->clordid, 17);
                if(dlen > 0) {
                    set_rom_field(doj, ROM_CLORDID, fill->clordid, dlen);
                }
                dlen = get_utp_field_len(fill->target, 5);
                if(dlen > 0) {
                    set_rom_field(doj, ROM_TRADER, fill->target, dlen);
                    set_rom_field(doj, ROM_TRADE_FOR, fill->target, dlen);
                    set_rom_field(doj, ROM_LOCAL_ACC, fill->target, dlen);
                }
            }
            break;
        case 0x00C1:
            break;
        case 0x0041:
            {
                set_rom_field(doj, 0, "E", 1);
                utpno* new_ord = d_off;
                int c = new_ord->qty;
                c = ntohl(c);
                unsigned_dart_itoa(doj, c, ROM_SHARES, db);
                databuf_reset(db);
                c = new_ord->price;
                double p = utp_to_rom_price(c, new_ord->scale);
                int dlen = sprintf(db->buffer, "%f", p);
                fill_in_rom_field(doj, ROM_PRICE, db->buffer, dlen);
                databuf_reset(db);
                c = new_ord->max_floor;
                c = ntohl(c);
                unsigned_dart_itoa(doj, c, ROM_MAX_FLOOR, db);
                databuf_reset(db);
                int sym_len = get_utp_field_len(new_ord->sym, 11);
                set_rom_field(doj, ROM_SYM, new_ord->sym, sym_len);
                //set_rom_field(doj, ROM_EXEC_INST, &new_ord->exec_inst, 1);
                set_rom_field(doj, ROM_SIDE, &new_ord->side, 1);
                if(doj->positions[ROM_SIDE].iov_len == 1) {
                    char* type = doj->positions[ROM_SIDE].iov_base;
                    if(type[0] == '5') {
                        set_rom_field(doj, ROM_SHORT_LEND,
                                doj->positions[ROM_CLR_ID].iov_base,
                                doj->positions[ROM_CLR_ID].iov_len);
                    }
                }
                set_rom_field(doj, ROM_TYPE, &new_ord->mtype, 1);
                set_rom_field(doj, ROM_TIF, &new_ord->tif, 1);
                set_rom_field(doj, ROM_CAP, &new_ord->cap, 1);
                if(new_ord->route != 0x0) {
                    set_rom_field(doj, ROM_INSTR, &new_ord->route, 1);
                }
                sym_len = get_utp_field_len(new_ord->on_be, 5);
                if(sym_len > 0) {
                }
                sym_len = get_utp_field_len(new_ord->sender_sub, 5);
                if(sym_len > 0) {
                }
                sym_len = get_utp_field_len(new_ord->account, 10);
                if(sym_len > 0) {
                    set_rom_field(doj, ROM_CLR_ACC, new_ord->account, sym_len);
                }
                sym_len = get_utp_field_len(new_ord->clordid, 17);
                if(sym_len > 0) {
                    set_rom_field(doj, ROM_CLORDID, new_ord->clordid, sym_len);
                }
                sym_len = get_utp_field_len(new_ord->sender_sub, 5);
                if(sym_len > 0) {
                    set_rom_field(doj, ROM_TRADER, new_ord->sender_sub, sym_len);
                    set_rom_field(doj, ROM_OWNER, new_ord->sender_sub, sym_len);
                    set_rom_field(doj, ROM_TRADE_FOR, new_ord->sender_sub, sym_len);
                    set_rom_field(doj, ROM_LOCAL_ACC, new_ord->sender_sub, sym_len);
                }
            }
            break;
        case 0x0061:
            {
                set_rom_field(doj, 0, "S", 1);
                set_rom_field(doj, ROM_STATUS, "26", 2);
                utpcnl* cnl = d_off;
                int c = cnl->orig_ord_qty;
                c = ntohl(c);
                unsigned_dart_itoa(doj, c, ROM_ORIG_SHARES, db);
                databuf_reset(db);
                c = cnl->cnl_qty;
                c = ntohl(c);
                unsigned_dart_itoa(doj, c, ROM_SHARES, db);
                databuf_reset(db);
                c = cnl->leaves;
                c = ntohl(c);
                unsigned_dart_itoa(doj, c, ROM_LEAVES, db);
                int sym_len = get_utp_field_len(cnl->sym, 11);
                set_rom_field(doj, ROM_SYM, cnl->sym, sym_len);
                set_rom_field(doj, ROM_SIDE, &cnl->side, 1);
                sym_len = get_utp_field_len(cnl->on_be, 5);
                if(sym_len > 0) {
                }
                sym_len = get_utp_field_len(cnl->sender_sub, 5);
                if(sym_len > 0) {
                }
                sym_len = get_utp_field_len(cnl->account, 10);
                if(sym_len > 0) {
                    set_rom_field(doj, ROM_CLR_ACC, cnl->account, sym_len);
                }
                sym_len = get_utp_field_len(cnl->clordid, 17);
                if(sym_len > 0) {
                    set_rom_field(doj, ROM_ORIG_ID, cnl->clordid, sym_len);
                }
                sym_len = get_utp_field_len(cnl->orig_clordid, 17);
                if(sym_len > 0) {
                    set_rom_field(doj, ROM_CLORDID, cnl->orig_clordid, sym_len);
                }
                sym_len = get_utp_field_len(cnl->sender_sub, 5);
                if(sym_len > 0) {
                    set_rom_field(doj, ROM_TRADER, cnl->sender_sub, sym_len);
                    set_rom_field(doj, ROM_TRADE_FOR, cnl->sender_sub, sym_len);
                    set_rom_field(doj, ROM_LOCAL_ACC, cnl->sender_sub, sym_len);
                }
            }
            break;
    };
    destroy_gk_db(ci->gk, db);
}
