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
#include <pthread.h>
#include <arpa/inet.h>

#include "connection_interface.h"
#include "ex_config_helper.h"
#include "dart_constants.h"
#include "databuf.h"
#include "dart_constants.h"
#include "dart_order_obj.h"
#include "debug_logger.h"
#include "ouch_parser.h"
#include "suffix_map.h"


#define ntohll(x) ( ( (uint64_t)(ntohl( (uint32_t)((x << 32) >> 32) )) << 32) | ntohl( ((uint32_t)(x >> 32)) ) )
#define htonll(x) ntohll(x)
#define SEC_NANOS 1000000000
#define MILLI_NANOS 1000000
#define HOUR_MILLIS 3600000
#define MIN_MILLIS 60000
#define EST_OFF 5
#define EDT_OFF 4

static int ouch_ioc = 0x0;
static int ouch_market_hours = 0x1869e;
static int ouch_day = 0x1869f;


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

/*static unsigned char one = 0x01;
static unsigned char zero = 0x0;
static unsigned char two = 0x02;

static char *today = 0;
static int day_light_sav = 0;
*/
static char *today = 0;
static int day_light_sav = 0;

void init_today()
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

void set_date_str(char *output, long tt)
{
    if (today == 0) {
        init_today();
    }
    if (output == 0) {
        return;
    }
    char *start = output;
    memcpy(start, today, 9);
    start += 9;
    long total_secs = tt / SEC_NANOS;
    int hour = ((total_secs / 3600) + day_light_sav) % 24;
    long min = (total_secs % 3600) / 60;
    long sec = (total_secs % 3600) % 60;
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
int ouch_create_order_token(con_interface * ob)
{
    return __sync_add_and_fetch(&(ob->seq), 1);
}

#define ROM_TO_OUCH_PRICE(price, d)    \
    d = strtod(price, NULL);           \
    d = d * 10000;                     \
    d += .0000001;                     \


#define OUCH_CONV_SIDE(dside, dside_len, buff)\
    if(dside_len >= 1) {                      \
        switch(dside[0]) {                    \
            case '1':                         \
                *buff->wr_ptr++ = 'B';        \
                break;                        \
            case '2':                         \
                *buff->wr_ptr++ = 'S';        \
                break;                        \
            case '5':                         \
                *buff->wr_ptr++ = 'T';        \
                break;                        \
            case '6':                         \
                *buff->wr_ptr++ = 'E';        \
                break;                        \
        }                                     \
    }                                         \

#define OUCH_CONV_SHARES(dshares, buff, rph)        \
    int net_qty = strtol(dshares, NULL, 10);                \
    set_rom_field(rph, ROM_BIN_QTY, &net_qty, sizeof(int)); \
    net_qty = htonl(net_qty);                               \
    databuf_memcpy(buff, (char *) &net_qty, 4);             \

#define OUCH_CONV_TIF(rph, buff)                            \
    int tflen = getplen(rph, ROM_TIF);                      \
    if(tflen >= 0) {                                        \
        char* tval = getpval(rph, ROM_TIF);                  \
        if(tval[0] == '3') {                                \
            databuf_memcpy(buff, &ouch_ioc, 4);              \
        } else if(tval[0] == '2') {                         \
            int temp_tif = htonl(ouch_market_hours);        \
            databuf_memcpy(buff, &(temp_tif), 4);     \
        } else {                                            \
            int temp_tif = htonl(ouch_day);                 \
            databuf_memcpy(buff, &temp_tif, 4);              \
        }                                                   \
    }

#define OUCH_CONV_DISPLAY(rph, buff)                        \
        if(getplen(rph, ROM_DISP_INSTR) > 0) {              \
            char* display = getpval(rph, ROM_DISP_INSTR);   \
            switch(display[0]) {                            \
                case 'A':                                   \
                   *buff->wr_ptr++ = 'P';                   \
                    break;                                  \
                case '8':                                   \
                   *buff->wr_ptr++ = 'N';                   \
                    break;                                  \
                default:                                    \
                    *buff->wr_ptr++ = display[0];           \
            }                                               \
        } else if(getplen(rph, ROM_EXEC_INST) > 0)  {       \
            char* exe = getpval(rph, ROM_EXEC_INST);        \
            switch(exe[0]) {                                \
                case 'M':                                   \
                case 'P':                                   \
                case 'R':                                   \
                  *buff->wr_ptr++ = 'M';                    \
                   break;                                   \
                default:                                    \
                  *buff->wr_ptr++ = exe[0];                 \
            }                                               \
        } else if(getplen(rph, ROM_MAX_FLOOR) > 0) {        \
            *buff->wr_ptr++ = 'N';                          \
        } else {                                            \
            *buff->wr_ptr++ = 'Y';                          \
        }                                                   \

#define OUCH_CONV_MIN_QTY(rph, buff)                                   \
        if(getplen(rph, ROM_MIN_QTY) > 0) {                            \
            int bin_min = strtol(getpval(rph, ROM_MIN_QTY), NULL, 10); \
            bin_min = htonl(bin_min);                                  \
            databuf_memcpy(buff, (char*) &bin_min, 4);                 \
        } else {                                                       \
            databuf_memcpy(buff, &ouch_ioc, 4);                         \
        }                                                              \


void ouch_convert_cross(dart_order_obj* rph, databuf_t* buff,
                        char* inst, int inst_len)                                     
{
    if(inst_len >= 6) {                                            
        switch(inst[5]) {                                         
            case 'O':                                              
            case 'C':                                              
               *buff->wr_ptr++ = inst[5];                          
                break;                                             
            default:                                              
               *buff->wr_ptr++ = 'N';                              
        }                                                          
    } else if (inst_len == 1) {                                    
        switch(inst[0]) {                                          
            case 'O':                                              
            case 'C':                                              
            case 'N':                                              
            case 'H':                                              
            case 'S':                                              
                *buff->wr_ptr++ = inst[0];                         
        }                                                          
    } else {                                                       
        char* type = getpval(rph, ROM_TYPE);                      
        int tlen = getplen(rph, ROM_TYPE);                         
        char* tif = getpval(rph, ROM_TIF);                         
        int tiflen = getplen(rph, ROM_TIF);                       
        if(tiflen == 1 && tif[0] == '2') {                         
            *buff->wr_ptr++ = 'O';                                
        } else if(tlen == 1  && type[0] == '5') {                  
            *buff->wr_ptr++ = 'C';                                 
        } else if(tlen >= 2 && ((strncmp("11", type, 2) == 0) ||   
                    (strncmp("10", type, 2) == 0))) {             
            *buff->wr_ptr++ = 'C';                                 
        } else {                                                   
            *buff->wr_ptr++ = 'N';                                 
        }                                                          
    }                                                             
}
int create_new_order_message(databuf_t * buff, void *r, int seq,
                             con_interface * ob)
{
    int ret = 0;
    memset(buff->buffer, ' ', 50);
    dart_order_obj *rph = (dart_order_obj *) r;
    rph->rep = 0;
    short nlen = 50;
    nlen = htons(nlen);
    databuf_memcpy(buff, &nlen, 2);
    *buff->wr_ptr++ = 'U';
    *buff->wr_ptr++ = 'O';
    int inst_len = getplen(rph, ROM_INSTR) ;                       
    char* inst = getpval(rph, ROM_INSTR);                          
    ret = create_in_place_ord_token(ob->tg, buff->wr_ptr, 14);
    fill_in_rom_field(rph, ROM_CLORDID, buff->wr_ptr, ret);
    no_copy_insert(ob->cl_ord_id_map,getpval(rph, ROM_CLORDID),
            getplen(rph, ROM_CLORDID), rph);
    buff->wr_ptr += 14;
    char* v = getpval(rph, ROM_SIDE);
    int v_len = getplen(rph, ROM_SIDE);
    OUCH_CONV_SIDE(v, v_len, buff);
    v = getpval(rph, ROM_SHARES);
    OUCH_CONV_SHARES(v, buff, rph);
    int n = 0;
    set_rom_field(rph, ROM_BIN_CUM, &n, sizeof(int)); 
    v = getpval(rph, ROM_SYM);
    v_len = getplen(rph, ROM_SYM);
    if(v_len > 0) {
        char* slash = memchr(v, '/', v_len);
        if(slash != NULL) {
            suffixm* b = (suffixm*)ob->parser;
            int sym_len = slash - v;
            int suf_len = getplen(rph, ROM_SYM) - (sym_len + 1);
            databuf_memcpy(buff, v, sym_len);
            int fsufflen = 0;
            const char* bsuffix =  get_suffix(b, (slash + 1), suf_len,
                       &fsufflen);
            databuf_memcpy(buff, bsuffix, fsufflen);
            buff->wr_ptr += (8 - (sym_len + fsufflen));
        } else {
            databuf_memcpy(buff, v, v_len);
            buff->wr_ptr += (8 - v_len);
        }
    } else {
        buff->wr_ptr += 8;
    }
    v = getpval(rph, ROM_PRICE);
    v_len = getplen(rph, ROM_PRICE);
    if(v_len > 0) {
        double d = 0;
        ROM_TO_OUCH_PRICE(v, d);
        int np = (int)d;
        set_rom_field(rph, ROM_BIN_PRICE, &np, sizeof(int));
        np = htonl(np);
        databuf_memcpy(buff, (char *) &np, 4);
    } else {
        int z = 0x7fffffff;
        z = htonl(z);
        databuf_memcpy(buff, (char*)&z, 4);
    }
    OUCH_CONV_TIF(rph, buff);
    int mpid_len = 0;
    char* on_behalf = get_mpid_for_clr_acct(ob->sbm,
            getpval(rph, ROM_CLR_ID),
            getplen(rph, ROM_CLR_ID),
            &mpid_len);
    if(mpid_len > 0) {
        databuf_memcpy(buff, on_behalf,mpid_len);
    } else {
        buff->wr_ptr += 4;
    }
    OUCH_CONV_DISPLAY(rph, buff);
    *buff->wr_ptr++ = 'A';
    //No iso
    *buff->wr_ptr++ = 'N'; 
    OUCH_CONV_MIN_QTY(rph, buff);
    ouch_convert_cross(rph, buff, inst, inst_len);
    //For now no retail
    *buff->wr_ptr++ = 'N'; 
    return databuf_unread(buff);
}

int create_replace_message(databuf_t * buff, void *r, int seq,
                             con_interface * ob)
{
    int ret = 0;
    memset(buff->buffer, ' ', 50);
    dart_order_obj *rph = (dart_order_obj *) r;
    dart_order_obj *ute = (dart_order_obj *) rph->rep;
    short nlen = 48;
    nlen = htons(nlen);
    databuf_memcpy(buff, &nlen, 2);
    *buff->wr_ptr++ = 'U';
    *buff->wr_ptr++ = 'U';
    int id_len = getplen(rph, ROM_CLORDID);
    databuf_memcpy(buff, getpval(rph, ROM_CLORDID),id_len);
    buff->wr_ptr += (14 - id_len); 
    ret = create_in_place_ord_token(ob->tg, buff->wr_ptr, 14);
    fill_in_rom_field(rph, ROM_PENDING_ID, buff->wr_ptr, ret);
    buff->wr_ptr += 14; 
    //now put this in the map
    char* clordid = getpval(rph, ROM_PENDING_ID);
    int clordid_len = getplen(rph, ROM_PENDING_ID);
    pthread_spin_lock(&ob->mutex);
    no_copy_insert(ob->cl_ord_id_map, clordid, clordid_len, rph);
    pthread_spin_unlock(&ob->mutex);
    char* v = getpval(ute, ROM_SHARES);
    int v_len = getplen(ute, ROM_SHARES);
    if(v_len > 0) {
        OUCH_CONV_SHARES(v, buff, ute);
    } else {
        v = getpval(rph, ROM_SHARES);
        OUCH_CONV_SHARES(v, buff, rph);
    }
    v_len = getplen(ute, ROM_PRICE);
    if(v_len > 0) {
        v = getpval(ute, ROM_PRICE);
        double d = 0;
        ROM_TO_OUCH_PRICE(v, d);
        int np = (int)d;
        set_rom_field(ute, ROM_BIN_PRICE, &np, sizeof(int));
        np = htonl(np);
        databuf_memcpy(buff, (char *) &np, 4);
    } else if(getplen(rph, ROM_PRICE) > 0) {
        v = getpval(rph, ROM_PRICE);
        double d = 0;
        ROM_TO_OUCH_PRICE(v, d);
        int np = (int)d;
        np = htonl(np);
        databuf_memcpy(buff, (char *) &np, 4);
    } else {
        int z = 0x7fffffff;
        z = htonl(z);
        databuf_memcpy(buff, (char *) &z, 4);
    }
    if(getplen(ute, ROM_TIF) > 0) {
        OUCH_CONV_TIF(ute, buff);
    } else {
        OUCH_CONV_TIF(rph, buff);
    }
    //buff->wr_ptr += 4;
    OUCH_CONV_DISPLAY(rph, buff);
    //No iso
    *buff->wr_ptr++ = 'N'; 
    if(getplen(ute, ROM_MIN_QTY) > 0) {
        OUCH_CONV_MIN_QTY(ute, buff);
    } else {
        OUCH_CONV_MIN_QTY(rph, buff);
    }
    return 1;
}

int create_cancel_message(databuf_t * buff, void *r, int seq,
                             con_interface * ob)
{
    int ret = 0;
    memset(buff->buffer, ' ', 50);
    dart_order_obj *rph = (dart_order_obj *) r;
    short nlen = 20;
    nlen = htons(nlen);
    databuf_memcpy(buff, &nlen, 2);
    *buff->wr_ptr++ = 'U';
    *buff->wr_ptr++ = 'X';
    int id_len = getplen(rph, ROM_CLORDID);
    databuf_memcpy(buff, getpval(rph, ROM_CLORDID),id_len);
    buff->wr_ptr += (14 - id_len); 
    databuf_memcpy(buff, &ret, 4);
    return 1;
}

#define OUCH_TO_ROM_PRICE(ack, rph, buff)                       \
    int price = 0;                                              \
    memcpy(&price, ack, 4);                                     \
    price = ntohl(price);                                       \
    int price_len = sprintf(buff->buffer, "%f",                 \
                          ((double) price/10000 + .0000001));   \
    fill_in_rom_field(rph, ROM_PRICE, buff->buffer, price_len); \
     

int parse_order_ack(dart_order_obj * ute, char *ack, con_interface * ob)
{
    long tt = 0;
    int ret = 0;
    set_rom_field(ute, ROM_EX_ID, ack + 49, 8);
    reset_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    char state = 'L';
    memcpy(&state, ack + 64, 1);
    if(state == 'L') {
        reset_rom_field(ute, ROM_STATUS, "13", 2);
    } else {
        pthread_spin_lock(&ob->mutex);
        remove_from_map(ob->cl_ord_id_map,getpval(ute, ROM_CLORDID), 
                        getplen(ute, ROM_CLORDID));
        pthread_spin_unlock(&ob->mutex);
        reset_rom_field(ute, ROM_STATUS, "4", 1);
        ret = 1;
    }
    memcpy(&tt, (ack + 1), 8);
    long ex_id = 0;
    memcpy(&ex_id, ack + 49, 8);
    ex_id = ntohll(ex_id);
    databuf_t *itoa_buff = get_databuf(ob->gk);
    unsigned_dart_itoa(ute, ex_id, ROM_EX_TAG,itoa_buff);
    tt = ntohll(tt);
    set_date_str(itoa_buff->buffer, tt);
    set_rom_field(ute, EXEC_TIME, itoa_buff->buffer, 17);
    OUCH_TO_ROM_PRICE(ack + 36, ute, itoa_buff);
    destroy_gk_db(ob->gk, itoa_buff);
    return ret;
}

void get_cnl_text(char reason, dart_order_obj * doj)
{
    switch (reason) {
    default:
        reset_rom_field(doj, ROM_TEXT, "User requested", 14);
        break;
    case 'I':
        reset_rom_field(doj, ROM_TEXT, "IOC no further matches", 22);
        break;
    case 'T':
        reset_rom_field(doj, ROM_TEXT, "Expired", 7);
        break;
    case 'S':
        reset_rom_field(doj, ROM_TEXT,
                        "Supervisory.", 11);
        break;
    case 'D':
        reset_rom_field(doj, ROM_TEXT, "Regulatory Restrictions", 23);
        break;
    case 'Q':
        reset_rom_field(doj, ROM_TEXT, "Self match prevention",
                        21);
        break;
    case 'Z':
        reset_rom_field(doj, ROM_TEXT, "System Cancel", 13);
        break;
    }
}
void get_rej_text(char reason, dart_order_obj * doj)
{
    switch (reason) {
    default:
        reset_rom_field(doj, ROM_TEXT, "Other", 5);
        break;
    case 'T':
        reset_rom_field(doj, ROM_TEXT, "Test Stocks Only", 16);
        break;
    case 'Z':
        reset_rom_field(doj, ROM_TEXT, "Shares exceed safety", 20);
        break;
    case 'S':
        reset_rom_field(doj, ROM_TEXT,
                        "Invalid Stock", 13);
        break;
    case 'D':
        reset_rom_field(doj, ROM_TEXT, "Invalid Display", 15);
        break;
    case 'C':
        reset_rom_field(doj, ROM_TEXT, "NASDAQ Closed",
                        13);
        break;
    case 'L':
        reset_rom_field(doj, ROM_TEXT, "Invalid Clearing", 16);
        break;
    case 'M':
        reset_rom_field(doj, ROM_TEXT, "Bad Time for type", 17);
        break;
    case 'R':
        reset_rom_field(doj, ROM_TEXT, "Bad Type for Cross", 18);
        break;
    case 'X':
        reset_rom_field(doj, ROM_TEXT, "Invalid Price", 13);
        break;
    case 'N':
        reset_rom_field(doj, ROM_TEXT, "Invalid Min QTY", 15);
        break;
    }
}
int parse_ur_out(dart_order_obj * ute, char *ack, con_interface * ob)
{
    int ret = 0;
    long tt = 0;
    memcpy(&tt, (ack + 1), 8);
    reset_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    reset_rom_field(ute, ROM_STATUS, "4", 1);
    databuf_t *itoa_buff = get_databuf(ob->gk);
    tt = ntohll(tt);
    set_date_str(itoa_buff->buffer, tt);
    fill_in_rom_field(ute, EXEC_TIME, itoa_buff->buffer, 17);
    char* reason = ack + 27;
    get_cnl_text(reason[0], ute);
    destroy_gk_db(ob->gk, itoa_buff);
    pthread_spin_lock(&ob->mutex);
    remove_from_map(ob->cl_ord_id_map,getpval(ute, ROM_CLORDID), 
                    getplen(ute, ROM_CLORDID));
    pthread_spin_unlock(&ob->mutex);
    return ret;
}
int parse_replaced(dart_order_obj * ute, char *ack, con_interface * ob)
{
    int ret = 0;
    long tt = 0;
    memcpy(&tt, (ack + 1), 8);
    reset_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    char state = 'L';
    memcpy(&state, ack + 64, 1);
    if(state == 'L') {
        reset_rom_field(ute, ROM_STATUS, "5", 1);
    } else {
        pthread_spin_lock(&ob->mutex);
        remove_from_map(ob->cl_ord_id_map,getpval(ute, ROM_PENDING_ID), 
                        getplen(ute, ROM_PENDING_ID));
        pthread_spin_unlock(&ob->mutex);
        reset_rom_field(ute, ROM_STATUS, "4", 1);
        ret = 1;
    }
    dart_order_obj *rep = (dart_order_obj *) ute->rep;
    databuf_t *itoa_buff = get_databuf(ob->gk);
    if(getplen(rep, ROM_SHARES) > 0) {
        fill_in_rom_field(ute, ROM_SHARES, getpval(rep, ROM_SHARES),
                          getplen(rep, ROM_SHARES));
    } 
    tt = ntohll(tt);
    set_date_str(itoa_buff->buffer, tt);
    fill_in_rom_field(ute, EXEC_TIME, itoa_buff->buffer, 17);
    int l_q = 0;
    memcpy(&l_q, ack + 24, 4);
    l_q = ntohl(l_q);
    fill_in_rom_field(ute, ROM_BIN_QTY, &l_q,4);
    OUCH_TO_ROM_PRICE(ack + 36, ute, itoa_buff); 
    if(rep->positions[ROM_BIN_PRICE].iov_len > 0) { 
        memcpy(ute->positions[ROM_BIN_PRICE].iov_base,
               rep->positions[ROM_BIN_PRICE].iov_base,
               getplen(rep, ROM_BIN_PRICE));
    }
    //setting leaves
    int l_cum = 0;
    memcpy(&l_cum, ute->positions[ROM_BIN_CUM].iov_base, 4);
    int leaves = l_q - l_cum;
    databuf_reset(itoa_buff);
    dart_itoa(ute, leaves, ROM_LEAVES, itoa_buff);
    //End leaves
    pthread_spin_lock(&ob->mutex);
    remove_from_map(ob->cl_ord_id_map,getpval(ute, ROM_CLORDID), 
                    getplen(ute, ROM_CLORDID));
    pthread_spin_unlock(&ob->mutex);
    fill_in_rom_field(ute, ROM_CLORDID, getpval(ute, ROM_PENDING_ID),
            getplen(ute, ROM_PENDING_ID));
    destroy_gk_db(ob->gk, itoa_buff);
    return ret;
}
int parse_rej(dart_order_obj * ute, char *ack, con_interface * ob)
{
    int ret = 0;
    reset_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    long tt = 0;
    memcpy(&tt, (ack + 1), 8);
    databuf_t *itoa_buff = get_databuf(ob->gk);
    tt = ntohll(tt);
    set_date_str(itoa_buff->buffer, tt);
    fill_in_rom_field(ute, EXEC_TIME, itoa_buff->buffer, 17);
    char* reason = ack + 23;
    get_rej_text(reason[0], ute);
    if(ute->rep != 0) {
        reset_rom_field(ute, ROM_STATUS, "20", 2);
        ret = 3;
    } else {
        reset_rom_field(ute, ROM_STATUS, "8", 1);
    }
    destroy_gk_db(ob->gk, itoa_buff);
    return ret;
}

int parse_cnl_rej(dart_order_obj* ute, char* ack, con_interface* ob)
{
    int ret = 0;
    long tt = 0;
    reset_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    memcpy(&tt, (ack + 1), 8);
    databuf_t *itoa_buff = get_databuf(ob->gk);
    tt = ntohll(tt);
    set_date_str(itoa_buff->buffer, tt);
    fill_in_rom_field(ute, EXEC_TIME, itoa_buff->buffer, 17);
    reset_rom_field(ute, ROM_STATUS, "14", 2);
    destroy_gk_db(ob->gk, itoa_buff);
    return ret;
}

static void validate_liq_code(dart_order_obj* ute, char* liq)
{
    switch(liq[0]) {
        case 'A':
        case 'R':
        case 'O':
        case 'M':
        case 'C':
        case 'L':
        case 'H':
        case 'K':
        case 'J':
        case 'm':
        case 'k':
        case '0':
        case '7':
        case '8':
        case 'd':
        case 'e':
        case 'f':
        case 'j':
        case 'r':
        case 't':
        case '6':
            set_rom_field(ute, ROM_LIQ, liq, 1);
            break;
        default:
            set_rom_field(ute, ROM_LIQ, "UNK", 3);
            break;
    }
}

int parse_filled(dart_order_obj * ute, char *ack, con_interface * ob)
{
    int ret = 0;
    long tt = 0;
    int last_shares = 0;
    reset_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    memcpy(&tt, (ack + 1), 8);
    databuf_t *itoa_buff = get_databuf(ob->gk);
    tt = ntohll(tt);
    set_date_str(itoa_buff->buffer, tt);
    fill_in_rom_field(ute, EXEC_TIME, itoa_buff->buffer, 17);
    unsigned long ex_id = 0;
    memcpy(&ex_id, ack + 32, 8);
    ex_id = ntohll(ex_id);
    unsigned_dart_itoa(ute, ex_id, EXEC_ID, itoa_buff);
    int price = 0;                                              
    memcpy(&price, ack + 27, 4);                                     
    price = ntohl(price);                                      
    int price_len = sprintf(itoa_buff->buffer, "%f",                 
                          ((double) price/10000 + .0000001));   
    fill_in_rom_field(ute, ROM_EXEC_PRICE, itoa_buff->buffer, price_len); 
    fill_in_rom_field(ute, ROM_BIN_LAST_PRICE, &price, sizeof(int));
    memcpy(&last_shares, (ack + 23), 4);
    last_shares = ntohl(last_shares);
    fill_in_rom_field(ute, ROM_BIN_LAST_SHARES, &last_shares, sizeof(int));
    int l_cum = 0;
    int qty = 0;
    memcpy(&qty, ute->positions[ROM_BIN_QTY].iov_base, 4);
    memcpy(&l_cum, ute->positions[ROM_BIN_CUM].iov_base, 4);
    l_cum += last_shares;
    fill_in_rom_field(ute, ROM_BIN_CUM, &l_cum, 4);
    if (l_cum >= qty) {
        reset_rom_field(ute, ROM_STATUS, "2", 1);
        pthread_spin_lock(&ob->mutex);
        remove_from_map(ob->cl_ord_id_map,
                        getpval(ute, ROM_CLORDID), getplen(ute, ROM_CLORDID));
        pthread_spin_unlock(&ob->mutex);
        ret = 1;
    } else {
        reset_rom_field(ute, ROM_STATUS, "1", 1);
    }
    dart_itoa(ute, l_cum, ROM_CUM, itoa_buff);
    dart_itoa(ute, last_shares, ROM_LAST_SHARES, itoa_buff);
    int leaves = qty - l_cum;
    dart_itoa(ute, leaves, ROM_LEAVES, itoa_buff);
    char* liq = ack + 31;
    validate_liq_code(ute, liq);
    destroy_gk_db(ob->gk, itoa_buff);
    return ret;
}

