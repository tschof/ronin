/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

#include "otto.h"
#include "rex_allocator.h"
#include "rex_constants.h"
#include "otto_parser.h"

#define SEC_NANOS 1000000000
#define MILLI_NANOS 1000000
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

int64_t rom_to_otto_price(char *price, size_t len)
{
    if(len > 0) {
        double d = strtod(price, NULL);
        if(d < 0.0) {
            d -= .0000001;
            d = d * 1000000;
        } else {
            d += .0000001;
            d = d * 1000000;
        }
        int64_t dd = (int64_t)d; 
        int64_t r = Swap8Bytes(dd);
        return r;
    } else {
        return 0; 
    }
}

int otto_to_rom_price(int64_t price, databuf_t* val)
{
    price = Swap8Bytes(price);
    double dprice = (double) price;
    dprice = (dprice/1000000.0);
    if(dprice > 0) {
        dprice += .0000001;
    } else {
        dprice -= .0000001;
    }
    return databuf_write(val, "%f", dprice);
}
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

static uint32_t get_int_for_rom(char* rom_field, int rom_len)
{
    char occ[32];
    memset(&occ, '\0', 32);
    memcpy(&occ, rom_field, rom_len);
    uint32_t ret_val = atoi(occ);
    return htonl(ret_val);
}

static uint16_t get_short_for_rom(char* rom_field, int rom_len)
{
    char occ[32];
    memset(&occ, '\0', 32);
    memcpy(&occ, rom_field, rom_len);
    uint16_t ret_val = atoi(occ);
    return htons(ret_val);
}

static void add_order(con_int* ci, char* clordid, int clordid_len, rcntr* cnt)
{
    pthread_spin_lock(&ci->mutex);
    /*START LOCK*/
    no_copy_insert(ci->cl_ord_id_map, clordid, clordid_len, cnt);
    /*END LOCK*/
    pthread_spin_unlock(&ci->mutex);
}

static void set_new_ord_fields(dart_order_obj* rph, ottop* otp, con_int* ci,
        token_gen* tg, ottono* ono, rcntr* cnt) 
{
    set_rom_field(rph, ROM_EXCHANGE_SYM,&ono->instrumentid, sizeof(uint32_t));
    memset(&ono->clordid, ' ', 16);
    int clordid_len = create_in_place_ord_token(tg, ono->clordid, 16);
    add_order(ci, ono->clordid, clordid_len, cnt);
    set_rom_field(rph, ROM_CLORDID,ono->clordid, clordid_len);
    if(rph->positions[ROM_CMTA].iov_len > 0) {
        ono->cmta = get_int_for_rom(rph->positions[ROM_CMTA].iov_base,
                rph->positions[ROM_CMTA].iov_len);;
    }
    if(rph->positions[ROM_MM_ACC].iov_len > 0) {
        memset(&ono->clearingaccount, ' ', 4);
        memcpy(&ono->clearingaccount, rph->positions[ROM_MM_ACC].iov_base,
                rph->positions[ROM_MM_ACC].iov_len);
    } else {
        memset(&ono->clearingaccount, ' ', 4);
    }
    if(rph->positions[ROM_CLR_ID].iov_len > 0 ) {
        ono->occaccount = get_int_for_rom(rph->positions[ROM_CLR_ID].iov_base,
                rph->positions[ROM_CLR_ID].iov_len);
    }
    if(rph->positions[ROM_CLR_ACC].iov_len > 0) {
        memset(&ono->custacc, ' ', 10);
        memcpy(&ono->custacc, rph->positions[ROM_CLR_ACC].iov_base,
                rph->positions[ROM_CLR_ACC].iov_len);
    } else {
        memset(&ono->custacc, ' ', 10);
    }

    
    memset(&ono->preferredparty, ' ', 3);
    ono->aloinst = 'N';
    ono->iso = 'N'; 
    char* exec = rph->positions[ROM_EXEC_INST].iov_base;
    int exec_len = rph->positions[ROM_EXEC_INST].iov_len;
    if(exec_len > 0) {
        if(exec[0] == 'A') {
            ono->aloinst = 'R';
        } else if(exec[0] == 'E') {
            ono->auctiontype = 'E';
        }        
    }
    if(rph->positions[ROM_SIDE].iov_len >= 1) { 
        char* dside = rph->positions[ROM_SIDE].iov_base;        
        switch(dside[0]) {                    
            case '1':                         
                ono->side = 'B';        
                break;                        
            case '2':                         
                ono->side = 'S';        
                break;                        
        }                                     
    }
    if(rph->positions[ROM_TYPE].iov_len > 0) {
        char* dtype = rph->positions[ROM_TYPE].iov_base;
        switch(dtype[0]) {
            case '1':
                ono->ordtype = 'M';
                break;
            case '2':
                ono->ordtype = 'L';
                break;
        }
    }
    if(rph->positions[ROM_PRICE].iov_len > 0 && ono->ordtype == 'L') {
        char sprice[32];
        memset(&sprice, '\0', 32);
        memcpy(&sprice, rph->positions[ROM_PRICE].iov_base, 
                rph->positions[ROM_PRICE].iov_len);
        ono->price = rom_to_otto_price(sprice,
                rph->positions[ROM_PRICE].iov_len);
    }
    if(rph->positions[ROM_SHARES].iov_len > 0) {
        ono->qty = get_int_for_rom(rph->positions[ROM_SHARES].iov_base,
                rph->positions[ROM_SHARES].iov_len);
    }
    char* tif = rph->positions[ROM_TIF].iov_base;
    int tif_len = rph->positions[ROM_TIF].iov_len;
    if(tif_len > 0) {
        switch(tif[0]) {
            case '1':
            case '0':
                ono->tif = 'D';
                break;
            case '2':
            case '3':
                ono->tif = 'I';
                break;
            case '4':
                ono->tif = 'F';
                break;
        }
    }
    if(rph->positions[ROM_MIN_QTY].iov_len > 0) {
       ono->minqty = get_int_for_rom(rph->positions[ROM_MIN_QTY].iov_base,
              rph->positions[ROM_MIN_QTY].iov_len); 
    } else if(ono->tif == 'F') {
        ono->minqty = ono->qty;
    }
    if(rph->positions[ROM_CAP].iov_len > 0) {
        char* cap = rph->positions[ROM_CAP].iov_base;
        switch(cap[0]) {
            case 'B':
                ono->capacity = 'K';
                break;
            case 'S':
                ono->capacity = 'O';
                break;
            case 'R':
                ono->capacity = 'F';
                break;
            case 'X':
                ono->capacity = 'P';
		break;
        }
    }
    if(rph->positions[ROM_AUCTION_ID].iov_len > 0 && 
            (exec_len > 0 && exec[0] == 'P')) {
        ono->auctiontype = 'P';
        ono->auctionid = get_int_for_rom(rph->positions[ROM_AUCTION_ID].iov_base,
                rph->positions[ROM_AUCTION_ID].iov_len);
    } else if(rph->positions[ROM_AUCTION_ID].iov_len > 0 &&
            exec_len > 0) {
        ono->auctiontype = exec[0];
        ono->auctionid = get_int_for_rom(rph->positions[ROM_AUCTION_ID].iov_base,
                rph->positions[ROM_AUCTION_ID].iov_len);
    } else if(rph->positions[ROM_AUCTION_ID].iov_len > 0) {
        ono->auctiontype = 'E';
        ono->auctionid = get_int_for_rom(rph->positions[ROM_AUCTION_ID].iov_base,
                rph->positions[ROM_AUCTION_ID].iov_len);
    } else if(tif_len > 0 && tif[0] == '2') {
        ono->auctiontype = 'O';
    } else if(exec_len > 0 && exec[0] == 'E') {
        ono->auctiontype = 'E';
    } else {
        ono->auctiontype = 'N';
    }
    ono->flashinst = ' ';
    if(rph->positions[ROM_MAX_FLOOR].iov_len > 0) {
        ono->displayqty = get_short_for_rom(rph->positions[ROM_MAX_FLOOR].iov_base,
                rph->positions[ROM_MAX_FLOOR].iov_len);
        ono->displaywhen = 'I';
        ono->displaymethod = 'I';
    } else {
        ono->displaywhen = 'N';
        ono->displaymethod = 'N';
    }
    if(rph->positions[ROM_O_C].iov_len > 0) {
        if(rph->positions[ROM_O_C].iov_len == 1) {
        ono->positioneffectmask = get_short_for_rom(rph->positions[ROM_O_C].iov_base,
                rph->positions[ROM_O_C].iov_len);
        } else if (rph->positions[ROM_O_C].iov_len > 1) {
            char* f = rph->positions[ROM_O_C].iov_base;
            int len = rph->positions[ROM_O_C].iov_len;
            uint16_t bitfield = 0;
            uint16_t on = 0x1;
            uint16_t chnum = 0;
            int i = 0;
            for(; i < len; ++i) {
                char cf[2];
                memset(&cf, ' ', 2);
                memcpy(&cf, &f[i], 1);
                uint8_t poc = atoi(cf);
                if(poc) {
                    bitfield |= on << chnum;
                }
                ++chnum;
            }
            ono->positioneffectmask = bitfield;
        }
    }
    ono->persistinst = 'N';
    
}
ottono* create_otto_new_order_message(rcntr *cnt,
                             ottop * otp, con_int* ci,token_gen* tg)
{
    int ret = 0;
    dart_order_obj* rph = (dart_order_obj*)cnt->raw;
    ottono* ono = 0;
    REX_ALLOC(ono, 8, sizeof(ottono), ret);
    ono->type = 'A';
    if(rph->positions[ROM_AUCTION_ID].iov_len > 0) {
        ono->priceprotection = 'L';
    } else {
        ono->priceprotection = 'N';
    }
    if(rph->positions[ROM_SEC_DEF].iov_len > 0) {
        ono->instrumentid = get_instrument_with_sec_def(otp,
                rph->positions[ROM_SEC_DEF].iov_base, 
                rph->positions[ROM_SEC_DEF].iov_len);
        if(ono->instrumentid == 0) {
            ono->instrumentid = get_int_for_rom(rph->positions[ROM_SEC_DEF].iov_base,
                    rph->positions[ROM_SEC_DEF].iov_len);
        }
    } else {
        free(ono);
        return NULL;
    }
    ono->stocklegshortsale = 'N';
    memcpy(ono->stocklegMpid, "    ", 4);
    if(rph->positions[ROM_INSTR].iov_len > 0) {
        char* inst = rph->positions[ROM_INSTR].iov_base;
        if(inst[0] == 'S') {
            memcpy(ono->stocklegMpid, "DEGS", 4);
        }
    }
    set_new_ord_fields(rph, otp, ci, tg, ono, cnt);
    if(rph->positions[ROM_SHORT_LEND].iov_len > 0) { 
        ono->stocklegshortsale = 'H';
        memcpy(ono->stocklegMpid, "DEGS", 4);
    } 
    return ono;
}
ottocnl* create_otto_cancel_message(rcntr* cnt,
                          ottop * ob, con_int* ci,token_gen* tg)
{
    int ret = 0;
    ottocnl* ocnl = 0;
    REX_ALLOC(ocnl, 8, sizeof(ottono), ret);
    ocnl->type = 'C';
    dart_order_obj* rph = (dart_order_obj*)cnt->raw;
    memset(&ocnl->clordid, ' ', 16);
    memcpy(ocnl->clordid, rph->positions[ROM_CLORDID].iov_base,
            rph->positions[ROM_CLORDID].iov_len);
    return ocnl;
}
ottorep* create_otto_replace_message(rcntr* cnt,
                           ottop * ob, con_int* ci,token_gen* tg)
{
    int ret = 0;
    dart_order_obj* rph = (dart_order_obj*)cnt->raw;
    dart_order_obj* rep = (dart_order_obj*)rph->rep;
    ottorep* orep = 0;
    REX_ALLOC(orep, 8, sizeof(ottono), ret);
    orep->type = 'R';
    orep->priceprotection = 'N';
    if(rph->positions[ROM_CPLX_TYPE].iov_len > 0) {
        char* cmplx = rph->positions[ROM_CPLX_TYPE].iov_base;
        if(cmplx[0] == '3' || cmplx [0] == '5') {
            orep->priceprotection = 'L';
        }
        int rep_len = rep->positions[ROM_EXEC_INST].iov_len;
        char* exec = rep->positions[ROM_EXEC_INST].iov_base;
        if(rep_len > 0 && exec[0] == 'N') {
            orep->priceprotection = 'N';
        } else if(rph->positions[ROM_EXEC_INST].iov_len > 0) {
            exec = rph->positions[ROM_EXEC_INST].iov_base;
            if(exec[0] == 'N') {
                orep->priceprotection = 'N';
            }
        }
    }
    memset(&orep->clordid, ' ', 16);
    int clordid_len = create_in_place_ord_token(tg, orep->clordid, 16);
    set_rom_field(rph, ROM_PENDING_ID, orep->clordid, clordid_len);
    memset(&orep->origclordid, ' ', 16);
    memcpy(orep->origclordid, rph->positions[ROM_CLORDID].iov_base,
            rph->positions[ROM_CLORDID].iov_len);
    char* tif = 0;
    int tlen = 0;
    if(rep->positions[ROM_TIF].iov_len > 0) {
        tif = rep->positions[ROM_TIF].iov_base;
        tlen = rep->positions[ROM_TIF].iov_len;
    } else if(rph->positions[ROM_TIF].iov_len > 0 ) {
        tif = rph->positions[ROM_TIF].iov_base;
        tlen = rph->positions[ROM_TIF].iov_len;
    }
    if(tlen > 0) {
        switch(tif[0]) {
            case '1':
            case '0':
                orep->tif = 'D';
                break;
            case '2':
            case '3':
                orep->tif = 'I';
                break;
            case '4':
                orep->tif = 'F';
                break;
        }
    }
    char* type = 0;
    tlen = 0;
    if(rep->positions[ROM_TYPE].iov_len > 0) {
        type = rep->positions[ROM_TYPE].iov_base;
        tlen = rep->positions[ROM_TYPE].iov_len;
    } else if(rph->positions[ROM_TYPE].iov_len > 0) {
        type = rph->positions[ROM_TYPE].iov_base;
        tlen = rph->positions[ROM_TYPE].iov_len;
    }
    if(tlen > 0) {
        switch(type[0]) {
            case '1':
                orep->ordtype = 'M';
                break;
            case '2':
                orep->ordtype = 'L';
                break;
        }
    }
    char* price = 0;
    tlen = 0;
    if(rep->positions[ROM_PRICE].iov_len > 0) {
        price = rep->positions[ROM_PRICE].iov_base;
        tlen = rep->positions[ROM_PRICE].iov_len;
    } else if(rph->positions[ROM_PRICE].iov_len > 0) {
        price = rph->positions[ROM_PRICE].iov_base;
        tlen = rph->positions[ROM_PRICE].iov_len;
    }
    if(tlen > 0) {
        char sprice[32];
        memset(&sprice, '\0', 32);
        memcpy(&sprice,price, tlen);
        orep->price = rom_to_otto_price(sprice,tlen);
    }
    if(rep->positions[ROM_SHARES].iov_len > 0) {
        orep->qty = get_int_for_rom(rep->positions[ROM_SHARES].iov_base,
                rep->positions[ROM_SHARES].iov_len);
    } else if(rph->positions[ROM_SHARES].iov_len > 0) {
        orep->qty = get_int_for_rom(rph->positions[ROM_SHARES].iov_base,
                rph->positions[ROM_SHARES].iov_len);
    }
    if(rph->positions[ROM_CLR_ACC].iov_len > 0) {
        memset(&orep->custacc, ' ', 10);
        memcpy(&orep->custacc, rph->positions[ROM_CLR_ACC].iov_base,
                rph->positions[ROM_CLR_ACC].iov_len);
    }
    return orep;
}
ottono* create_otto_new_cmplx(rcntr* cnt, 
                            ottop* otp, con_int* ci, token_gen* tg,
                            uint32_t instrumentid, char* mpid, int mpidlen)
{
    int ret = 0;
    dart_order_obj* rph = (dart_order_obj*)cnt->raw;
    ottono* ono = 0;
    REX_ALLOC(ono, 8, sizeof(ottono), ret);
    ono->type = 'A';
    ono->stocklegshortsale = 'N';
    memcpy(ono->stocklegMpid, "    ", 4);
    ono->instrumentid = instrumentid;
    if(rph->positions[ROM_INSTR].iov_len > 0) {
        char* inst = rph->positions[ROM_INSTR].iov_base;
        if(inst[0] == 'S') {
            memcpy(ono->stocklegMpid, "DEGS", 4);
        }
    }
    set_new_ord_fields(rph, otp, ci, tg, ono, cnt);
    if(rph->positions[ROM_SHORT_LEND].iov_len > 0) { 
        ono->stocklegshortsale = 'H';
        memcpy(ono->stocklegMpid, "DEGS", 4);
    }
    ono->priceprotection = 'L';
    if(rph->positions[ROM_EXEC_INST].iov_len > 0) {
        char* exec = rph->positions[ROM_EXEC_INST].iov_base;
        if(exec[0] == 'N') {
            ono->priceprotection = 'N';
        } 
        
    }
    uint16_t bitfield = 0;
    uint16_t on = 0x1;
    uint16_t chnum = 0;
    children* lch = rph->childs;
    while(lch != NULL) {
        dart_order_obj* ldoj = (dart_order_obj*)lch->payload;
        if(ldoj->positions[ROM_SEC_TYPE].iov_len > 0) {
            char* rst = ldoj->positions[ROM_SEC_TYPE].iov_base;
            if(rst[0] == 'E' && ldoj->positions[ROM_SIDE].iov_len > 0) {
                char* side = ldoj->positions[ROM_SIDE].iov_base;
                if(side[0] == '5') {
                    ono->stocklegshortsale = 'H';
                    memcpy(ono->stocklegMpid, "DEGS", 4);
                }
            }
        }
        if(ldoj->positions[ROM_O_C].iov_len > 0) {
            uint16_t poc = get_short_for_rom(ldoj->positions[ROM_O_C].iov_base,
                    ldoj->positions[ROM_O_C].iov_len);
            if(poc) {
                bitfield |= on << chnum;
            }
        }
        lch = lch->next;
        ++chnum;
    }
    ono->positioneffectmask = bitfield;
    databuf_t *buff = get_databuf(ci->gk);
    unsigned_dart_itoa(rph, bitfield, ROM_O_C, buff);
    destroy_gk_db(ci->gk, buff);
    return ono;
}

void set_child_status(dart_order_obj* doj, char* status, int slen)
{
    children* lch = doj->childs;
    while(lch != NULL) {
        dart_order_obj* ldoj = (dart_order_obj*)lch->payload;
        set_rom_field(ldoj, 0, "S", 1);
        set_rom_field(ldoj, ROM_STATUS, status, slen);
        set_rom_field(ldoj, ROM_SPREAD, "6", 1);
        lch = lch->next;
    }
}
void update_order_ack(rcntr* obj, ottooa* oak, databuf_t* db)
{
    dart_order_obj* doj = (dart_order_obj*)obj->raw;
    unsigned_dart_itoa(doj, oak->orderid,
                                ROM_EX_TAG, db);
    set_rom_field(doj, 0, "S", 1);
    set_rom_field(doj, ROM_STATUS, "13", 2);
    char dstr[32];
    memset(dstr, '\0', 32);
    uint64_t tt = Swap8Bytes(oak->timestamp);
    set_date_str(dstr, tt);
    fill_in_rom_field(doj, EXEC_TIME, dstr, 17);
    if(doj->num_childs > 0) {
        set_child_status(doj, "13", 2);
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
    pthread_spin_lock(&ci->mutex);
    /*START LOCK*/
    no_copy_insert(ci->cl_ord_id_map, new_id, pendlen, cnt);
    /*END LOCK*/
    pthread_spin_unlock(&ci->mutex);
}
void parse_replaced(rcntr* obj, ottorepl* orpl, con_int* ci)
{
    dart_order_obj* doj = (dart_order_obj*)obj->raw;
    dart_order_obj* rep = (dart_order_obj*)doj->rep;
    switch_order_ids(doj, ci, obj);
    if(rep->positions[ROM_SHARES].iov_len > 0) {
        set_rom_field(doj, ROM_SHARES, rep->positions[ROM_SHARES].iov_base,
                rep->positions[ROM_SHARES].iov_len);
        obj->size = rep->risk.size;
    }
    if(rep->positions[ROM_PRICE].iov_len > 0) {
        set_rom_field(doj, ROM_PRICE, rep->positions[ROM_PRICE].iov_base,
                rep->positions[ROM_PRICE].iov_len);
        obj->price = rep->risk.price;
    }
    if(rep->positions[ROM_TIF].iov_len > 0) {
        set_rom_field(doj, ROM_TIF, rep->positions[ROM_TIF].iov_base,
                rep->positions[ROM_TIF].iov_len);
    }
    if(rep->positions[ROM_TYPE].iov_len > 0) {
        set_rom_field(doj, ROM_TYPE, rep->positions[ROM_TYPE].iov_base,
                rep->positions[ROM_TYPE].iov_len);
    }
    char dstr[32];
    memset(dstr, '\0', 32);
    uint64_t tt = Swap8Bytes(orpl->timestamp);
    set_date_str(dstr, tt);
    fill_in_rom_field(doj, EXEC_TIME, dstr, 17);
    set_rom_field(doj, ROM_STATUS, "5", 1);
    if(doj->num_childs > 0) {
        set_child_status(doj, "5", 1);
    }
}
void parse_ur_out(rcntr* obj,ottocnled* ocnl)
{
    dart_order_obj* doj = (dart_order_obj*)obj->raw;
    set_rom_field(doj, 0, "S", 1);
    set_rom_field(doj, ROM_STATUS, "4", 1);
    if(doj->num_childs > 0) {
        set_child_status(doj, "4", 1);
    }
}

static dart_order_obj* get_child_for_exe(dart_order_obj* doj, ottoexe* oexe)
{
    children* lch = doj->childs;
    dart_order_obj* ldoj = 0;
    while(lch != NULL) {
        ldoj = (dart_order_obj*)lch->payload;
        int inst = 0;
        char* sec_type = ldoj->positions[ROM_SEC_TYPE].iov_base;
        memcpy(&inst, ldoj->positions[ROM_EXCHANGE_SYM].iov_base,
                ldoj->positions[ROM_EXCHANGE_SYM].iov_len);
        if(oexe->ordexectype == 'C' && inst == oexe->legistrumentid) {
            break;    
        } else if(oexe->ordexectype == 'D'&& sec_type[0] == 'E') {
            break;
        } 
        lch = lch->next;
    }
    return ldoj;
}
static rcntr* parse_stock_leg(dart_order_obj* parent,
        ottoexe* oexe, rcntr* master, databuf_t* db)
{
    rcntr* cnt = 0;
    dart_order_obj* child = get_child_for_exe(parent, oexe);
    if(child) {
        set_rom_field(child, 0, "S", 1);
        cnt = &child->risk;
        cnt->raw = child;
        cnt->rlen = sizeof(dart_order_obj);
        unsigned_dart_itoa(child, oexe->orderid,
                                ROM_EX_TAG, db);
        databuf_reset(db);
        unsigned_dart_itoa(child, oexe->matchid, EXEC_ID, db);
        databuf_reset(db);
        otto_to_rom_price(oexe->exeprice, db);
        set_rom_field(child, ROM_EXEC_PRICE, db->rd_ptr, databuf_unread(db));
        databuf_reset(db);
        unsigned_dart_itoa(child, oexe->lastshares, ROM_LAST_SHARES, db);
        unsigned_dart_itoa(child, oexe->liquidityind, ROM_LIQ, db);
    } else {
        unsigned_dart_itoa(parent, oexe->orderid,
                                ROM_EX_TAG, db);
        set_rom_field(parent, 0, "S", 1);
        databuf_reset(db);
        unsigned_dart_itoa(parent, oexe->matchid, EXEC_ID, db);
        databuf_reset(db);
        otto_to_rom_price(oexe->exeprice, db);
        set_rom_field(parent, ROM_EXEC_PRICE, db->rd_ptr, databuf_unread(db));
        databuf_reset(db);
        unsigned_dart_itoa(parent, oexe->lastshares, ROM_LAST_SHARES, db);
        unsigned_dart_itoa(parent, oexe->liquidityind, ROM_LIQ, db);
        set_rom_field(parent, ROM_CPLX_TYPE, "6", 1);
        set_rom_field(parent, ROM_STATUS, "1", 1);
        set_rom_field(parent, ROM_SEC_TYPE, "E", 1);
        return master;
    }
    return cnt;
}
static rcntr* parse_option_leg(dart_order_obj* parent,
        ottoexe* oexe, rcntr* master, databuf_t* db, ottop *otp)
{
    rcntr* cnt = 0;
    dart_order_obj* child = get_child_for_exe(parent, oexe);
    if(child) {
        set_rom_field(child, 0, "S", 1);
        cnt = &child->risk;
        cnt->raw = child;
        cnt->rlen = sizeof(dart_order_obj);
        unsigned_dart_itoa(child, oexe->orderid,
                                ROM_EX_TAG, db);
        databuf_reset(db);
        unsigned_dart_itoa(child, oexe->matchid, EXEC_ID, db);
        databuf_reset(db);
        otto_to_rom_price(oexe->exeprice, db);
        set_rom_field(child, ROM_EXEC_PRICE, db->rd_ptr, databuf_unread(db));
        databuf_reset(db);
        unsigned_dart_itoa(child, oexe->lastshares, ROM_LAST_SHARES, db);
        unsigned_dart_itoa(child, oexe->liquidityind, ROM_LIQ, db);
        cnt->cum += oexe->lastshares;
        if(cnt->size <= cnt->cum) {
            set_rom_field(child, ROM_STATUS, "2", 1);
        } else {
            set_rom_field(child, ROM_STATUS, "1", 1);
        }
        return cnt;
    } else {
        unsigned_dart_itoa(parent, oexe->orderid,
                                ROM_EX_TAG, db);
        databuf_reset(db);
        unsigned_dart_itoa(parent, oexe->matchid, EXEC_ID, db);
        databuf_reset(db);
        otto_to_rom_price(oexe->exeprice, db);
        set_rom_field(parent, ROM_EXEC_PRICE, db->rd_ptr, databuf_unread(db));
        databuf_reset(db);
        unsigned_dart_itoa(parent, oexe->lastshares, ROM_LAST_SHARES, db);
        unsigned_dart_itoa(parent, oexe->liquidityind, ROM_LIQ, db);
        if(master->size <= master->cum) {
            set_rom_field(parent, ROM_STATUS, "2", 1);
        } else {
            set_rom_field(parent, ROM_STATUS, "1", 1);
        }
        set_rom_field(parent, ROM_CPLX_TYPE, "6", 1);
        set_rom_field(parent, ROM_SEC_TYPE, "O", 1);
        set_rom_symbol(otp, oexe->legistrumentid, parent);
        return master;
    }
}
rcntr* parse_filled(rcntr* obj,ottoexe* oexe, databuf_t* db,ottop *otp )
{
    oexe->lastshares = ntohl(oexe->lastshares);
    oexe->orderid = ntohl(oexe->orderid);
    oexe->matchid = ntohl(oexe->matchid);
    dart_order_obj* doj = (dart_order_obj*)obj->raw;
    set_rom_field(doj, 0, "S", 1);
    if(oexe->side == 'B') {
        set_rom_field(doj, ROM_SIDE, "1", 1);
    } else {
        set_rom_field(doj, ROM_SIDE, "2", 1);
    }
    char dstr[32];
    memset(dstr, '\0', 32);
    uint64_t tt = Swap8Bytes(oexe->timestamp);
    set_date_str(dstr, tt);
    fill_in_rom_field(doj, EXEC_TIME, dstr, 17);
    if(oexe->ordexectype == 'C') {
        return parse_option_leg(doj, oexe, obj, db, otp);
    } else if(oexe->ordexectype == 'D') {
        return parse_stock_leg(doj, oexe, obj, db);
    }
    unsigned_dart_itoa(doj, oexe->orderid,
                                ROM_EX_TAG, db);
    databuf_reset(db);
    unsigned_dart_itoa(doj, oexe->matchid, EXEC_ID, db);
    databuf_reset(db);
    otto_to_rom_price(oexe->exeprice, db);
    set_rom_field(doj, ROM_EXEC_PRICE, db->rd_ptr, databuf_unread(db));
    databuf_reset(db);
    obj->last = oexe->lastshares;
    obj->cum += oexe->lastshares;
    if(obj->cum < obj->size) {
        set_rom_field(doj, ROM_STATUS, "1", 1);
    } else {
        set_rom_field(doj, ROM_STATUS, "2", 1);
    }
    if(doj->positions[ROM_CPLX_TYPE].iov_len > 0) {
        set_rom_field(doj, ROM_CPLX_TYPE, "5", 1);
    }
    unsigned_dart_itoa(doj, oexe->lastshares, ROM_LAST_SHARES, db);
    unsigned_dart_itoa(doj, obj->cum, ROM_CUM, db);
    unsigned_dart_itoa(doj, obj->size - obj->cum, ROM_LEAVES, db);
    unsigned_dart_itoa(doj, oexe->liquidityind, ROM_LIQ, db);
    unsigned_dart_itoa(doj, ntohl(oexe->instrumentid), ROM_SEC_DEF, db);
    return obj;
}
void update_short_order_ack(rcntr* obj,
        ottooashorty* oak)
{
}

int parse_rej(rcntr* obj, ottorej* rej)
{
    dart_order_obj* doj = (dart_order_obj*)obj->raw;
    if(rej->rejectmsgtype == 'C') {
        set_rom_field(doj, ROM_STATUS, "14", 2);
    } else if(rej->rejectmsgtype == 'R') {
        set_rom_field(doj, ROM_STATUS, "20", 2);
    } else {
        set_rom_field(doj, ROM_STATUS, "8", 1);
    }
    set_rom_field(doj, 0, "S", 1);
    char dstr[32];
    memset(dstr, '\0', 32);
    uint64_t tt = Swap8Bytes(rej->timestamp);
    set_date_str(dstr, tt);
    fill_in_rom_field(doj, EXEC_TIME, dstr, 17);
    uint16_t rej_code = ntohs(rej->rejectcode);
    switch(rej_code) {
        case 10:
            set_rom_field(doj, ROM_TEXT, "Invalid Firm", 12);
            break;
        case 11:
            set_rom_field(doj, ROM_TEXT, "Invalid Instrument",18);
            break;
        case 12:
            set_rom_field(doj, ROM_TEXT, "Invalid InstrumentType",22);
            break;
        case 13:
            set_rom_field(doj, ROM_TEXT, "Invalid Quantity",16);
            break;
        case 14:
            set_rom_field(doj, ROM_TEXT, "Invalid Price",13);
            break;
        case 15:
            set_rom_field(doj, ROM_TEXT, "Invalid Side",12);
            break;
        case 16:
            set_rom_field(doj, ROM_TEXT, "Invalid Tif",11);
            break;
        case 17:
            set_rom_field(doj, ROM_TEXT, "Invalid Iso",11);
            break;
        case 18:
            set_rom_field(doj, ROM_TEXT, "Invalid AuctionType",19);
            break;
        case 19:
            set_rom_field(doj, ROM_TEXT, "Invalid AuctionID",17);
            break;
        case 20:
            set_rom_field(doj, ROM_TEXT, "Invalid OrderType",17);
            break;
        case 21:
            set_rom_field(doj, ROM_TEXT, "Invalid PrefParty",17);
            break;
        case 22:
            set_rom_field(doj, ROM_TEXT, "Invalid ALO",12);
            break;
        case 23:
            set_rom_field(doj, ROM_TEXT, "Invalid Capacity",16);
            break;
        case 24:
            set_rom_field(doj, ROM_TEXT, "Invalid FlashInst",17);
            break;
        case 25:
            set_rom_field(doj, ROM_TEXT, "Invalid Trade",13);
            break;
        case 26:
            set_rom_field(doj, ROM_TEXT, "Invalid Format",14);
            break;
        case 27:
            set_rom_field(doj, ROM_TEXT, "Invalid CrossType",17);
            break;
        case 28:
            set_rom_field(doj, ROM_TEXT, "Invalid MinQty",14);
            break;
        case 29:
            set_rom_field(doj, ROM_TEXT, "Invalid PricePro",16);
            break;
        case 30:
            set_rom_field(doj, ROM_TEXT, "Invalid Reserve",15);
            break;
        case 31:
            set_rom_field(doj, ROM_TEXT, "Invalid Persist",15);
            break;
        case 32:
            set_rom_field(doj, ROM_TEXT, "Invalid ShortSaleInd",20);
            break;
        case 33:
            set_rom_field(doj, ROM_TEXT, "Invalid Product",15);
            break;
        case 34:
            set_rom_field(doj, ROM_TEXT, "Invalid Scope",13);
            break;
        case 35:
            set_rom_field(doj, ROM_TEXT, "Invalid Clearing",16);
            break;
        case 36:
            set_rom_field(doj, ROM_TEXT, "Invalid PosEff",14);
            break;
        case 37:
            set_rom_field(doj, ROM_TEXT, "Invalid CrossID",15);
            break;
        case 38:
            set_rom_field(doj, ROM_TEXT, "Invalid Matchid",15);
            break;
        case 39:
            set_rom_field(doj, ROM_TEXT, "Invalid Clordid",15);
            break;
        case 40:
            set_rom_field(doj, ROM_TEXT, "Invalid Kill",12);
            break;
        case 41:
            set_rom_field(doj, ROM_TEXT, "Invalid LegCount",16);
            break;
        case 42:
            set_rom_field(doj, ROM_TEXT, "Invalid LegType",15);
            break;
        case 43:
            set_rom_field(doj, ROM_TEXT, "Invalid LegRatio",16);
            break;
        case 44:
            set_rom_field(doj, ROM_TEXT, "Invalid MPID",12);
            break;
        case 45:
            set_rom_field(doj, ROM_TEXT, "Invalid Time",12);
            break;
        case 101:
            set_rom_field(doj, ROM_TEXT, "Not Free",8);
            break;
        case 102:
            set_rom_field(doj, ROM_TEXT, "Pref Not allowed",16);
            break;
        case 103:
            set_rom_field(doj, ROM_TEXT, "Stock Combo Not allowed",23);
            break;
        case 104:
            set_rom_field(doj, ROM_TEXT, "Instrument Halted",17);
            break;
        case 105:
            set_rom_field(doj, ROM_TEXT, "Kill Switch in effect",21);
            break;
        case 106:
            set_rom_field(doj, ROM_TEXT, "System Closed",13);
            break;
        case 107:
            set_rom_field(doj, ROM_TEXT, "TestMode",8);
            break;
        case 108:
            set_rom_field(doj, ROM_TEXT, "Order Not Found",15);
            break;
        case 109:
            set_rom_field(doj, ROM_TEXT, "Too Late to Cancel",18);
            break;
        case 110:
            set_rom_field(doj, ROM_TEXT, "Instrument Closed",17);
            break;
        case 111:
            set_rom_field(doj, ROM_TEXT, "Instrument State",16);
            break;
        case 112:
            set_rom_field(doj, ROM_TEXT, "Action Not Allowed",18);
            break;
        case 113:
            set_rom_field(doj, ROM_TEXT, "LULD In Effect",14);
            break;
        case 114:
            set_rom_field(doj, ROM_TEXT, "Too Many Combos",15);
            break;
    };
    return 0;
}
