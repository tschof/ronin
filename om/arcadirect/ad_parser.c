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

#include "ad_parser.h"
#include "connection_interface.h"
#include "ex_config_helper.h"
#include "dart_constants.h"
#include "databuf.h"
#include "dart_constants.h"
#include "dart_order_obj.h"
#include "debug_logger.h"


#define ntohll(x) ( ( (uint64_t)(ntohl( (uint32_t)((x << 32) >> 32) )) << 32) | ntohl( ((uint32_t)(x >> 32)) ) )
#define htonll(x) ntohll(x)
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

static unsigned char one = 0x01;
static unsigned char zero = 0x0;
static unsigned char two = 0x02;

static char *today = 0;
static int day_light_sav = 0;


int ad_create_order_token(con_interface * ob)
{
    return __sync_add_and_fetch(&(ob->seq), 1);
}

int rom_to_ad_price(char *price, size_t len)
{
    double d = strtod(price, NULL);
    d = d * 10000;
    d += .0000001;
    return (int) d;
}

double ad_to_rom_price(int data, char ps)
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
    }
    a = ntohl(data);
    d = ((double) a / d) + .0000001;
    return d;
}

int ad_to_rom_bin_price(int data, char ps)
{
    int d = 1;
    switch (ps) {
    default:
        d = 1;
        break;
    case 0:
        d = 10000;
        break;
    case 1:
        d = 1000;
        break;
    case 2:
        d = 100;
        break;
    case 3:
        d = 10;
        break;
    }
    int a = ntohl(data);
    return (a * d);
}

int find_ad_field_len(char *start, int max_len)
{
    char *end = start + (max_len - 1);
    while (end[0] == 0) {
        --end;
    }
    return (end - start) + 1;
}

void get_rej_text(short reason, dart_order_obj * doj)
{
    switch (reason) {
    default:
        reset_rom_field(doj, ROM_TEXT, "Unknown", 7);
    case 3000:
        reset_rom_field(doj, ROM_TEXT, "Invalid field", 13);
    case 3001:
        reset_rom_field(doj, ROM_TEXT, "Required Field missing", 22);
    case 3002:
        reset_rom_field(doj, ROM_TEXT,
                        "Both CancelQty and Leaves Qty are required.", 40);
    case 3003:
        reset_rom_field(doj, ROM_TEXT, "Target System unavailable", 26);
    case 3004:
        reset_rom_field(doj, ROM_TEXT, "Symbol is not a valid ARCA symbol",
                        33);
    case 4000:
        reset_rom_field(doj, ROM_TEXT, "ARCA down or stock halted", 25);
    case 4001:
        reset_rom_field(doj, ROM_TEXT, "Invalid stock symbol", 20);;
    }
}

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
    long total_secs = tt / 1000000;
    long hour = (total_secs / 3600) % 24;//+ day_light_sav) % 24;
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

static void put_price_in(databuf_t* buff, dart_order_obj* rph)
{
    if ((getplen(rph, ROM_PRICE)) > 0
        && getplen(rph, ROM_BIN_PRICE) <= 0) {
        int np = rom_to_ad_price(getpval(rph, ROM_PRICE), getplen(rph, ROM_PRICE));
        np = htonl(np);
        set_rom_field(rph, ROM_BIN_PRICE, &np, sizeof(int));
        databuf_memcpy(buff, (char *) &np, 4);
    } else if (getplen(rph, ROM_BIN_PRICE) > 0) {
        int np = 0;
        memcpy(&np, getpval(rph, ROM_BIN_PRICE),
               getplen(rph, ROM_BIN_PRICE));
        np = htonl(np);
        databuf_memcpy(buff, &np, 4);
    } else {
        int null_int = 0;
        databuf_memcpy(buff, (char *) &null_int, 4);
    }
}


static int create_variant_3_order(databuf_t * buff, dart_order_obj * rph,
                                  con_interface* co)
{
    int temp = 0;
    int q = 0;
    char* sec_type = getpval(rph, ROM_SEC_TYPE);
    if ((temp = getplen(rph, ROM_STRIKE)) > 0) {
        q = rom_to_ad_price(getpval(rph, ROM_STRIKE), 0);
        q = htonl(q);
        databuf_memcpy(buff, (char *) &q, 4);
    } else {
        buff->wr_ptr += 4;
    }
    if ((temp = getplen(rph, ROM_MIN_QTY)) > 0) {
        q = atoi(getpval(rph, ROM_MIN_QTY));
        q = htonl(q);
        databuf_memcpy(buff, (char *) &q, 4);
    } else {
        buff->wr_ptr += 4;
    }
    if ((temp = getplen(rph, ROM_MAX_FLOOR)) > 0) {
        q = atoi(getpval(rph, ROM_MAX_FLOOR));
        q = htonl(q);
        databuf_memcpy(buff, (char *) &q, 4);
    } else {
        buff->wr_ptr += 4;
    }
    //For now we do not support Display Range and Discretion offset orders
    buff->wr_ptr += 8;
    if ((temp = getplen(rph, ROM_PEG_DIFF)) > 0) {
        q = rom_to_ad_price(getpval(rph, ROM_PEG_DIFF), 0);
        q = htonl(q);
        databuf_memcpy(buff, (char *) &q, 4);
    } else {
        buff->wr_ptr += 4;
    }
    if (getplen(rph, ROM_STOP) > 0) {
        double d = strtod(getpval(rph, ROM_STOP), NULL);
        d = d * 100;
        d += .0000001;
        q = (int)d;
        databuf_memcpy(buff, (char*) &q, 4);
    } else {
        buff->wr_ptr += 4;
    }
    put_price_in(buff, rph);
    short ex_dest =102;
    ex_dest = htons(ex_dest);
    databuf_memcpy(buff, &ex_dest, 2);
    buff->wr_ptr += 2; //Skip the ExDestination for now (H's orders are full of bugus ones)
    //The second two bytes of the above skipped 4 are for the underlying qty which we have
    //no equivelant for in ROM.
    if(getplen(rph, ROM_P_C) > 0) {
        char* pc = (char*)getpval(rph,ROM_P_C);
        if(pc[0] == 'C') {
            databuf_memcpy(buff,&one, 1);
        } else {
            databuf_memcpy(buff,&zero, 1);
        }
    } else {
        buff->wr_ptr++;
    }
    if(getplen(rph, ROM_MM_HOME) > 0) {
        int val_len = 0;
        char* local_mm_ex = get_no_copy_val(co->connection_conf->dc, "FIX SERVER", 10,
                      "MM HOME", 7,  &val_len);
        char* mmh = (char*)getpval(rph, ROM_MM_HOME);
        if(getplen(rph, ROM_MM_HOME) == val_len && 
                strncmp(local_mm_ex, mmh, val_len) == 0) {
            databuf_memcpy(buff,&one, 1);
        } else {
            databuf_memcpy(buff,&two, 1);
        }
    } else {
        buff->wr_ptr++;
    }    
    *buff->wr_ptr++ = '4';
    buff->wr_ptr++;
    if(getplen(rph, ROM_O_C) > 0) {
        char* oc = getpval(rph, ROM_O_C);
        *buff->wr_ptr++ = oc[0] == '1'? 'O':'C';
    } else {
        buff->wr_ptr++;
    }
    if ((getplen(rph, ROM_SYM)) > 0) {
        ad_translate_rom_symbol(buff->wr_ptr,
                                           getpval(rph, ROM_SYM),
                                           getplen(rph, ROM_SYM));

    }
    buff->wr_ptr += 8;
    if(getplen(rph, ROM_EXPIRE_DATE) > 0 && getplen(rph, ROM_EXPIRE_DAY) > 0) {
        databuf_memcpy(buff, getpval(rph, ROM_EXPIRE_DATE), 6);
        if(getplen(rph, ROM_EXPIRE_DAY) == 1) {
            databuf_memcpy(buff, "0", 1);
            databuf_memcpy(buff, getpval(rph, ROM_EXPIRE_DAY), 1);
        } else {
            databuf_memcpy(buff, getpval(rph, ROM_EXPIRE_DAY), 2);
        }
    } else {
        buff->wr_ptr += 8;
    }
    int cur_size = 0;
    char *on_behalf = get_rom_field(rph, ROM_ON_BEHALF, &cur_size);
    databuf_memcpy(buff, on_behalf, cur_size);
    //Leaving out Company GroupID
    buff->wr_ptr += (5 - cur_size);
    //DeliverToCompID
    buff->wr_ptr += 5;
    //SenderSubID
    buff->wr_ptr += 5;
    char* off = 0;
    if ((getplen(rph, ROM_EXEC_INST)) > 0) {
        off = getpval(rph, ROM_EXEC_INST);
        *buff->wr_ptr++ = off[0];
    } else {
        *buff->wr_ptr++ = '\0';
    }
    off = getpval(rph, ROM_SIDE);
    *buff->wr_ptr++ = off[0];

    off = getpval(rph, ROM_TYPE);
    if (strncmp(off, "11", 2) == 0) {
        *buff->wr_ptr++ = 'B';
    } else {
        *buff->wr_ptr++ = off[0];
    }
    if ((getplen(rph, ROM_TIF)) > 0) {
        off = getpval(rph, ROM_TIF);
        *buff->wr_ptr++ = off[0];
    } else {
        *buff->wr_ptr++ = '\0';
    }
    if(getplen(rph, ROM_SEC_TYPE) == 0 || 
        sec_type[0] == 'E') {            
        *buff->wr_ptr++ = 'A';
        *buff->wr_ptr++ = '\0';
        databuf_memcpy(buff, "2", 1);
        *buff->wr_ptr++ = '\0';
        *buff->wr_ptr++ = '\0';
        *buff->wr_ptr++ = '\0';
    } else {
        *buff->wr_ptr++ = '\0';
        char* capacity = getpval(rph, ROM_CAP);
        if(getplen(rph, ROM_CAP) > 0) {
           *buff->wr_ptr ++ = capacity[0] == 'B'? '1':'3'; 
        } else {
            *buff->wr_ptr++ = '\0';
        }
        databuf_memcpy(buff, "2", 1);
        *buff->wr_ptr += 3;
    }
    if ((getplen(rph, ROM_CLR_ACC)) > 0) {
        databuf_memcpy(buff, getpval(rph, ROM_CLR_ACC),
                       getplen(rph, ROM_CLR_ACC));
    }
    buff->wr_ptr = buff->buffer + 128;
    if(getplen(rph, ROM_SEC_TYPE) > 0 &&
            sec_type[0] == 'O' && getplen(rph, ROM_CLR_ID) > 0) {
        databuf_memcpy(buff, getpval(rph, ROM_CLR_ID), getplen(rph, ROM_CLR_ID));
    }
    buff->wr_ptr = buff->buffer + 133;
    if(getplen(rph, ROM_CMTA) > 0) {
        databuf_memcpy(buff, getpval(rph, ROM_CMTA), getplen(rph, ROM_CMTA));
    }
    buff->wr_ptr = buff->buffer + 138;
    //jump expire time fields for now
    buff->wr_ptr = buff->buffer + 145;
    //jump discretionInst(1) Proactive Discretion(1) and ProactiveifLocked(1)
    buff->wr_ptr = buff->buffer + 148;

    {
        char* instr = getpval(rph, ROM_DISP_INSTR);
        int instr_len = getplen(rph, ROM_DISP_INSTR);
        //Skip ExecBroker for now
        if(instr_len == 2 && instr[0] == 'P' &&
                instr[1] == 'O') {
            buff->wr_ptr = buff->buffer + 150;
            *buff->wr_ptr++ = 'P';    
            *buff->wr_ptr++ = 'O';    
            *buff->wr_ptr++ = 'O';    
            *buff->wr_ptr++ = '\0';
            *buff->wr_ptr++ = '\0';
        } else {
            buff->wr_ptr = buff->buffer + 155;
        }
        *buff->wr_ptr++ = 'N';
        *buff->wr_ptr++ = '\0';
        if(instr_len > 0 &&
                instr[0] == 'A') {
            *buff->wr_ptr++ = 'A';    
        } else if(instr_len == 1 && instr[0] == 'P') {
            *buff->wr_ptr++ = 'P';    
        } else if(instr_len == 1 && instr[0] == 'p') {
            *buff->wr_ptr++ = 'p';    
        } else {
            buff->wr_ptr++;    
        }
    }
    buff->wr_ptr = buff->buffer + 163;
    *buff->wr_ptr++ = '\n';
    return 1;
}

int create_new_order_message(databuf_t * buff, void *r, int seq,
                             con_interface * ob)
{
    dart_order_obj *rph = (dart_order_obj *) r;
    int all_good = 1;
    int cur_size = 0;
    int null_int = 0;
    char *off = 0;
    unsigned char mess_type = 0x01;
    short mess_len = (htons(76));
    int net_seq = 0;
    short ex_dest = 102;

    unsigned int clordid = ad_create_order_token(ob);
    if(getplen(rph, ROM_MAX_FLOOR) > 0 ||
        getplen(rph, ROM_STRIKE) > 0 ||
        getplen(rph, ROM_EXEC_INST) > 0 ||
        getplen(rph, ROM_DISP_INSTR) > 0) {
        mess_type = 0x03;
        mess_len = (htons(164));
    } 
    *buff->wr_ptr++ = 'D';
    databuf_memcpy(buff, &mess_type, 1);
    databuf_memcpy(buff, (char *) &mess_len, 2);
    databuf_memcpy(buff, &net_seq, 4);
    net_seq = htonl(clordid);
    databuf_memcpy(buff, (char *) &net_seq, 4);
    buff->wr_ptr += 4;          //Skip the portfolio cross.
    if (getplen(rph, ROM_SHARES) > 0 && getplen(rph, ROM_BIN_QTY) <= 0) {
        int net_qty = strtol(getpval(rph, ROM_SHARES), NULL, 10);
        set_rom_field(rph, ROM_BIN_QTY, &net_qty, sizeof(int));
        set_rom_field(rph, ROM_BIN_CUM, &null_int, sizeof(int));
        set_rom_field(rph, ROM_LEAVES, getpval(rph, ROM_SHARES),
                      getplen(rph, ROM_SHARES));
        net_qty = htonl(net_qty);
        databuf_memcpy(buff, (char *) &net_qty, 4);
    } else if (getplen(rph, ROM_BIN_QTY) > 0) {
        int net_qty = 0;
        memcpy(&net_qty, getpval(rph, ROM_BIN_QTY), sizeof(int));
        set_rom_field(rph, ROM_BIN_CUM, &null_int, sizeof(int));
        set_rom_field(rph, ROM_LEAVES, getpval(rph, ROM_SHARES),
                      getplen(rph, ROM_SHARES));
        net_qty = htonl(net_qty);
        databuf_memcpy(buff, (char *) &net_qty, 4);
    } else {
        all_good = 0;
    }
    /*
     * We may have to add more checks to see if we need to use a 
     * more complete variant.
     */
    if (mess_type == 0x01) {
        /*Here we finish the variant one orders.*/
        put_price_in(buff, rph);
        //Populate the ex destination
        ex_dest = htons(ex_dest);
        databuf_memcpy(buff, &ex_dest, 2);
        *buff->wr_ptr++ = '4';
        if ((getplen(rph, ROM_SYM)) > 0) {
            cur_size = ad_translate_rom_symbol(buff->wr_ptr,
                                               getpval(rph, ROM_SYM),
                                               getplen(rph, ROM_SYM));
            buff->wr_ptr += cur_size;
        } else {
            all_good = 0;
        }
        buff->wr_ptr = buff->buffer + 35;
        char *on_behalf = get_rom_field(rph, ROM_ON_BEHALF, &cur_size);
        databuf_memcpy(buff, on_behalf, cur_size);
        buff->wr_ptr = buff->buffer + 50;
        if ((getplen(rph, ROM_EXEC_INST)) > 0) {
            off = getpval(rph, ROM_EXEC_INST);
            *buff->wr_ptr++ = off[0];
        } else {
            *buff->wr_ptr++ = '\0';
        }
        off = getpval(rph, ROM_SIDE);
        *buff->wr_ptr++ = off[0];

        off = getpval(rph, ROM_TYPE);
        if (strncmp(off, "11", 2) == 0) {
            *buff->wr_ptr++ = 'B';
        } else {
            *buff->wr_ptr++ = off[0];
        }
        if ((getplen(rph, ROM_TIF)) > 0) {
            off = getpval(rph, ROM_TIF);
            *buff->wr_ptr++ = off[0];
        } else {
            *buff->wr_ptr++ = '\0';
        }
        *buff->wr_ptr++ = 'A';
        databuf_memcpy(buff, "123", 3);
        *buff->wr_ptr++ = '\0';
        if ((getplen(rph, ROM_CLR_ACC)) > 0) {
            databuf_memcpy(buff, getpval(rph, ROM_CLR_ACC),
                           getplen(rph, ROM_CLR_ACC));
        }
        *buff->wr_ptr++ = '\0';
        if ((getplen(rph, ROM_INSTR)) > 0) {
            off = getpval(rph, ROM_INSTR);
            *buff->wr_ptr++ = off[0];
            reset_rom_field(rph, ROM_COPY_INSTR,
                            getpval(rph, ROM_INSTR),
                            getplen(rph, ROM_INSTR));
        } else {
            buff->wr_ptr++;
        }
        buff->wr_ptr = buff->buffer + 75;
        *buff->wr_ptr++ = '\n';

    } else {
        all_good = create_variant_3_order(buff, rph, ob);
    }
    set_rom_field(rph, ROM_BIN_CLORDID, &clordid, 4);
    databuf_t *itoa_buff = get_databuf(ob->gk);
    unsigned_dart_itoa(rph, clordid, ROM_CLORDID, itoa_buff);
    pthread_spin_lock(&ob->mutex);
    no_copy_insert(ob->cl_ord_id_map, (char *) &clordid, 4, rph);
    pthread_spin_unlock(&ob->mutex);
    destroy_gk_db(ob->gk, itoa_buff);
    return all_good;
}

int create_cancel_message(databuf_t * buff, void *r, int seq,
                          con_interface * ob)
{
    int all_good = 1;
    dart_order_obj *ute = (dart_order_obj *) r;
    unsigned char mess_type = 0x01;
    short mess_len = (htons(72));
    int net_seq = 0;
    long ex_id = 0;
    unsigned int clordid = 0;
    if (ute->positions[ROM_EX_ID].iov_base) {
        memcpy(&ex_id, (ute->positions[ROM_EX_ID].iov_base), 8);
    }
    if (ute->positions[ROM_BIN_CLORDID].iov_base) {
        memcpy(&clordid, (ute->positions[ROM_BIN_CLORDID].iov_base), 4);
    }
    clordid = htonl(clordid);

    *buff->wr_ptr++ = 'F';
    databuf_memcpy(buff, (char *) (&mess_type), 1);
    databuf_memcpy(buff, (char *) &mess_len, 2);
    databuf_memcpy(buff, (char *) &net_seq, 4);
    databuf_memcpy(buff, (char *) &ex_id, 8);
    databuf_memcpy(buff, (char *) &clordid, 4);
    buff->wr_ptr += 6;
    short ex_dest = htons(102);
    memcpy(buff->wr_ptr, &ex_dest, 2);
    buff->wr_ptr += 6;
    buff->wr_ptr = buff->buffer + 32;
    if (ute->positions[ROM_SYM].iov_base) {
        ad_translate_rom_symbol(buff->wr_ptr, getpval(ute, ROM_SYM),
                       getplen(ute, ROM_SYM));
    }
    buff->wr_ptr = buff->buffer + 48;
    char *off = ute->positions[ROM_SIDE].iov_base;
    *buff->wr_ptr++ = off[0];   //ute->side;
    buff->wr_ptr = buff->buffer + 71;
    *buff->wr_ptr++ = '\n';
    return all_good;
}

int create_replace_message(databuf_t * buff, void *r, int seq,
                           con_interface * ob)
{
    int all_good = 1;
    char q[10];
    char *off = 0;
    dart_order_obj *ute = (dart_order_obj *) r;
    unsigned char mess_type = 0x01;
    short mess_len = (htons(88));
    int net_seq = 0;
    long ex_id = 0;             //htonl(ute->me_order_id);
    int orig_id = 0;
    unsigned clordid = ad_create_order_token(ob);
    unsigned net_clordid = htonl(clordid);
    if (ute->positions[ROM_EX_ID].iov_base) {
        memcpy(&ex_id, (ute->positions[ROM_EX_ID].iov_base), 8);
    }
    if (ute->positions[ROM_BIN_CLORDID].iov_base) {
        memcpy(&orig_id, (ute->positions[ROM_BIN_CLORDID].iov_base), 4);
    }
    dart_order_obj *rph = (dart_order_obj *) ute->rep;
    *buff->wr_ptr++ = 'G';
    databuf_memcpy(buff, (char *) (&mess_type), 1);
    databuf_memcpy(buff, (char *) &mess_len, 2);
    databuf_memcpy(buff, (char *) &net_seq, 4);
    databuf_memcpy(buff, (char *) &ex_id, 8);
    databuf_memcpy(buff, (char *) &net_clordid, 4);
    net_clordid = htonl(orig_id);
    databuf_memcpy(buff, (char *) &net_clordid, 4);

    if ((rph->positions[ROM_SHARES].iov_len) > 0
        && rph->positions[ROM_BIN_QTY].iov_len <= 0) {
        int net_qty =
            strtol(rph->positions[ROM_SHARES].iov_base, NULL, 10);
        set_rom_field(rph, ROM_BIN_QTY, &net_qty, sizeof(int));
        net_qty = htonl(net_qty);
        databuf_memcpy(buff, (char *) &net_qty, 4);
    } else if (rph->positions[ROM_BIN_QTY].iov_len > 0) {
        int net_qty = 0;
        memcpy(&net_qty, rph->positions[ROM_BIN_QTY].iov_base,
               sizeof(int));
        net_qty = htonl(net_qty);
        databuf_memcpy(buff, (char *) &net_qty, 4);
    } else {
        int net_qty = 0;
        memcpy(&net_qty, (ute->positions[ROM_BIN_QTY].iov_base), 4);
        net_qty = htonl(net_qty);
        databuf_memcpy(buff, (char *) &net_qty, 4);
    }
    buff->wr_ptr += 4;
    if ((rph->positions[ROM_PRICE].iov_len) > 0
        && rph->positions[ROM_BIN_PRICE].iov_len <= 0) {
        off = rph->positions[ROM_PRICE].iov_base;
        memset(q, 0, 10);
        memcpy(q, off, rph->positions[ROM_PRICE].iov_len);
        int np = rom_to_ad_price(q, rph->positions[ROM_PRICE].iov_len);
        set_rom_field(rph, ROM_BIN_PRICE, &np, sizeof(int));
        np = htonl(np);
        databuf_memcpy(buff, (char *) &np, 4);
    } else if (rph->positions[ROM_BIN_PRICE].iov_len > 0) {
        int np = 0;
        memcpy(&np, rph->positions[ROM_BIN_PRICE].iov_base,
               rph->positions[ROM_BIN_PRICE].iov_len);
        np = htonl(np);
        databuf_memcpy(buff, &np, 4);
    } else {
        int np = 0;
        memcpy(&np, ute->positions[ROM_BIN_PRICE].iov_base,
               ute->positions[ROM_BIN_PRICE].iov_len);
        np = htonl(np);
        databuf_memcpy(buff, &np, 4);
    }
    short ex_dest = htons(102);
    databuf_memcpy(buff, &ex_dest, 2);
    buff->wr_ptr += 2;
    *buff->wr_ptr++ = '4';
    buff->wr_ptr += 3;
    if (ute->positions[ROM_SYM].iov_base) {
        ad_translate_rom_symbol(buff->wr_ptr, getpval(ute, ROM_SYM),
                       getplen(ute, ROM_SYM));
    }
    buff->wr_ptr = buff->buffer + 60;
    if (ute->positions[ROM_EXEC_INST].iov_len > 0) {
        off = ute->positions[ROM_EXEC_INST].iov_base;
        *buff->wr_ptr++ = off[0];
    } else {
        buff->wr_ptr++;
    }
    off = ute->positions[ROM_SIDE].iov_base;
    *buff->wr_ptr++ = off[0];   //ute->side;
    if ((rph->positions[ROM_TYPE].iov_len) > 0) {
        off = rph->positions[ROM_TYPE].iov_base;
        *buff->wr_ptr++ = off[0];
    } else {
        off = ute->positions[ROM_TYPE].iov_base;
        *buff->wr_ptr++ = off[0];
    }
    if ((rph->positions[ROM_TIF].iov_len) > 0) {
        off = rph->positions[ROM_TIF].iov_base;
        *buff->wr_ptr++ = off[0];
    } else {
        off = ute->positions[ROM_TIF].iov_base;
        *buff->wr_ptr++ = off[0];
    }
    *buff->wr_ptr++ = 'A';
    databuf_memcpy(buff, "123", 3);
    *buff->wr_ptr++ = '\0';
    buff->wr_ptr += 5;
    if ((rph->positions[ROM_CLR_ACC].iov_len) > 0) {
        databuf_memcpy(buff, rph->positions[ROM_CLR_ACC].iov_base,
                       rph->positions[ROM_CLR_ACC].iov_len);
    }
    buff->wr_ptr = buff->buffer + 87;
    *buff->wr_ptr++ = '\n';
    reset_rom_field(ute, ROM_ORIG_ID, ute->positions[ROM_CLORDID].iov_base,
                    ute->positions[ROM_CLORDID].iov_len);
    reset_rom_field(ute, ROM_BIN_ORIG_ID,
                    ute->positions[ROM_BIN_CLORDID].iov_base,
                    ute->positions[ROM_BIN_CLORDID].iov_len);
    databuf_t *itoa_buff = get_databuf(ob->gk);
    unsigned_dart_itoa(ute, clordid, ROM_CLORDID, itoa_buff);
    set_rom_field(ute, ROM_BIN_CLORDID, &clordid, 4);
    destroy_gk_db(ob->gk, itoa_buff);
    pthread_spin_lock(&ob->mutex);
    no_copy_insert(ob->cl_ord_id_map, (char *) &clordid, 4, ute);
    pthread_spin_unlock(&ob->mutex);
    return all_good;
}

int parse_order_ack(dart_order_obj * ute, char *ack, con_interface * ob)
{
    long tt = 0;
    set_rom_field(ute, ROM_EX_ID, ack + 28, 8);
    reset_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    reset_rom_field(ute, ROM_STATUS, "13", 2);
    memcpy(&tt, (ack + 16), 8);
    long ex_id = 0;
    memcpy(&ex_id, ack + 28, 8);
    ex_id = ntohll(ex_id);
    databuf_t *itoa_buff = get_databuf(ob->gk);
    unsigned_dart_itoa(ute, ex_id, ROM_EX_TAG,itoa_buff);
    tt = ntohll(tt);
    set_date_str(itoa_buff->buffer, tt);
    fill_in_rom_field(ute, EXEC_TIME, itoa_buff->buffer, 17);
    destroy_gk_db(ob->gk, itoa_buff);
    return 1;
}

int parse_cancel_ack(dart_order_obj * ute, char *ack, con_interface * ob)
{
    long tt = 0;
    set_rom_field(ute, ROM_EX_ID, ack + 28, 8);
    reset_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    reset_rom_field(ute, ROM_STATUS, "6", 1);
    memcpy(&tt, (ack + 16), 8);
    long ex_id = 0;
    memcpy(&ex_id, ack + 28, 8);
    ex_id = ntohll(ex_id);
    databuf_t *itoa_buff = get_databuf(ob->gk);
    unsigned_dart_itoa(ute, ex_id, ROM_EX_TAG, itoa_buff);
    tt = ntohll(tt);
    set_date_str(itoa_buff->buffer, tt);
    fill_in_rom_field(ute, EXEC_TIME, itoa_buff->buffer, 17);
    destroy_gk_db(ob->gk, itoa_buff);
    return 1;
}

int parse_ur_out(dart_order_obj * ute, char *ack, con_interface * ob)
{
    long tt = 0;
    set_rom_field(ute, ROM_EX_ID, ack + 28, 8);
    memcpy(&tt, (ack + 16), 8);
    reset_rom_field(ute, ROM_STATUS, "4", 1);
    reset_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    tt = ntohll(tt);
    databuf_t *itoa_buff = get_databuf(ob->gk);
    set_date_str(itoa_buff->buffer, tt);
    fill_in_rom_field(ute, EXEC_TIME, itoa_buff->buffer, 17);
    if (ute->positions[ROM_LEAVES].iov_len > 0) {
        reset_rom_field(ute, ROM_LEAVES, "0", 1);
    }
    pthread_spin_lock(&ob->mutex);
    remove_from_map(ob->cl_ord_id_map,
                    (char *) ute->positions[ROM_BIN_CLORDID].iov_base, 4);
    pthread_spin_unlock(&ob->mutex);
    destroy_gk_db(ob->gk, itoa_buff);
    return 1;
}

int parse_cancel_rep(dart_order_obj * ute, char *ack, con_interface * ob)
{
    long tt = 0;
    set_rom_field(ute, ROM_EX_ID, ack + 28, 8);
    memcpy(&tt, (ack + 16), 8);
    reset_rom_field(ute, ROM_STATUS, "6", 1);
    reset_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    tt = ntohll(tt);
    databuf_t *itoa_buff = get_databuf(ob->gk);
    set_date_str(itoa_buff->buffer, tt);
    fill_in_rom_field(ute, EXEC_TIME, itoa_buff->buffer, 17);
    destroy_gk_db(ob->gk, itoa_buff);
    return 1;
}

int parse_replaced(dart_order_obj * ute, char *ack, con_interface * ob)
{
    long tt = 0;
    set_rom_field(ute, ROM_EX_ID, ack + 28, 8);
    memcpy(&tt, (ack + 16), 8);
    reset_rom_field(ute, ROM_STATUS, "5", 1);
    reset_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    long ex_id = 0;
    memcpy(&ex_id, ack + 28, 8);
    ex_id = ntohll(ex_id);
    databuf_t *itoa_buff = get_databuf(ob->gk);
    unsigned_dart_itoa(ute, ex_id, ROM_EX_TAG, itoa_buff);
    tt = ntohll(tt);
    set_date_str(itoa_buff->buffer, tt);
    fill_in_rom_field(ute, EXEC_TIME, itoa_buff->buffer, 17);
    dart_order_obj *rep = (dart_order_obj *) ute->rep;
    char *off = rep->positions[ROM_SHARES].iov_base;
    set_rom_field(ute, ROM_SHARES, off,
                  rep->positions[ROM_SHARES].iov_len);
    int l_q = strtol(off, NULL, 10);
    memcpy(ute->positions[ROM_BIN_QTY].iov_base, &l_q, 4);
    memcpy(ute->positions[ROM_BIN_PRICE].iov_base,
           rep->positions[ROM_BIN_PRICE].iov_base, 4);
    set_rom_field(ute, ROM_PRICE, rep->positions[ROM_PRICE].iov_base,
                  rep->positions[ROM_PRICE].iov_len);
    //setting leaves
    int l_cum = 0;
    memcpy(&l_cum, ute->positions[ROM_BIN_CUM].iov_base, 4);
    int leaves = l_q - l_cum;
    databuf_reset(itoa_buff);
    dart_itoa(ute, leaves, ROM_LEAVES, itoa_buff);
    //End leaves
    int rem_id = 0;
    memcpy(&rem_id, ute->positions[ROM_BIN_ORIG_ID].iov_base,
           ute->positions[ROM_BIN_ORIG_ID].iov_len);
    pthread_spin_lock(&ob->mutex);
    remove_from_map(ob->cl_ord_id_map, (char *) &rem_id, 4);
    pthread_spin_unlock(&ob->mutex);
    destroy_gk_db(ob->gk, itoa_buff);
    return 1;
}

int parse_rej(dart_order_obj * ute, char *ack, con_interface * ob)
{
    long tt = 0;
    int ret_rej = 1;
    char rej_type = '1';
    memcpy(&tt, (ack + 16), 8);
    memcpy(&rej_type, (ack + 32), 1);
    reset_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    switch (rej_type) {
    case '1':
        reset_rom_field(ute, ROM_STATUS, "8", 1);
        pthread_spin_lock(&ob->mutex);
        remove_from_map(ob->cl_ord_id_map,
                        ute->positions[ROM_BIN_CLORDID].iov_base, 4);
        pthread_spin_unlock(&ob->mutex);
        break;
    case '2':
        reset_rom_field(ute, ROM_STATUS, "14", 2);
        reset_rom_field(ute, ROM_CLORDID,
                        ute->positions[ROM_ORIG_ID].iov_base,
                        ute->positions[ROM_ORIG_ID].iov_len);
        reset_rom_field(ute, ROM_BIN_CLORDID,
                        ute->positions[ROM_BIN_ORIG_ID].iov_base,
                        ute->positions[ROM_BIN_ORIG_ID].iov_len);
        ret_rej = 2;
        break;
    case '3':
        reset_rom_field(ute, ROM_STATUS, "20", 2);
        reset_rom_field(ute, ROM_CLORDID,
                        ute->positions[ROM_ORIG_ID].iov_base,
                        ute->positions[ROM_ORIG_ID].iov_len);
        reset_rom_field(ute, ROM_BIN_CLORDID,
                        ute->positions[ROM_BIN_ORIG_ID].iov_base,
                        ute->positions[ROM_BIN_ORIG_ID].iov_len);
        ret_rej = 3;
        break;
    }
    int rej_len = find_ad_field_len(ack + 33, 40);
    set_rom_field(ute, ROM_TEXT, ack + 33, rej_len);
    tt = ntohll(tt);
    databuf_t *itoa_buff = get_databuf(ob->gk);
    set_date_str(itoa_buff->buffer, tt);
    fill_in_rom_field(ute, EXEC_TIME, itoa_buff->buffer, 17);
    destroy_gk_db(ob->gk, itoa_buff);
    return ret_rej;
}

int parse_filled(dart_order_obj * ute, char *ack, con_interface * ob)
{
    long tt = 0;
    int last_shares = 0;
    int last_price = 0;
    int ret_val = 0;
    char ps = '4';
    unsigned long ex_id = 0;
    memcpy(&ex_id, ack + 36, 8);
    ex_id = ntohll(ex_id);
    databuf_t *itoa_buff = get_databuf(ob->gk);
    unsigned_dart_itoa(ute, ex_id, EXEC_ID, itoa_buff);
    unsigned long ord_id = 0;
    memcpy(&ord_id, ack + 28, 8);
    ord_id = ntohll(ord_id);
    unsigned_dart_itoa(ute, ord_id, ROM_EX_TAG, itoa_buff);

    memcpy(&tt, (ack + 16), 8);
    memcpy(&last_shares, (ack + 64), 4);
    memcpy(&last_price, (ack + 68), 4);
    memcpy(&ps, ack + 72, 1);
    last_shares = ntohl(last_shares);
    int rblp = ad_to_rom_bin_price(last_price, ps);
    fill_in_rom_field(ute, ROM_BIN_LAST_PRICE, &rblp, sizeof(int));
    double lp = ad_to_rom_price(last_price, ps);
    fill_in_rom_field(ute, ROM_BIN_LAST_SHARES, &last_shares, sizeof(int));
    int price_len = sprintf(itoa_buff->buffer, "%f", lp);
    fill_in_rom_field(ute, ROM_EXEC_PRICE, itoa_buff->buffer,price_len);
    //ute->current->data->wr_ptr += price_len;
    int l_cum = 0;
    int qty = 0;
    memcpy(&qty, ute->positions[ROM_BIN_QTY].iov_base, 4);
    memcpy(&l_cum, ute->positions[ROM_BIN_CUM].iov_base, 4);
    l_cum += last_shares;
    fill_in_rom_field(ute, ROM_BIN_CUM, &l_cum, 4);
    reset_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    if (l_cum >= qty) {
        reset_rom_field(ute, ROM_STATUS, "2", 1);
        pthread_spin_lock(&ob->mutex);
        remove_from_map(ob->cl_ord_id_map,
                        ute->positions[ROM_BIN_CLORDID].iov_base, 4);
        pthread_spin_unlock(&ob->mutex);
        ret_val = 1;
    } else {
        reset_rom_field(ute, ROM_STATUS, "1", 1);
    }
    dart_itoa(ute, l_cum, ROM_CUM, itoa_buff);
    dart_itoa(ute, last_shares, ROM_LAST_SHARES, itoa_buff);
    int leaves = qty - l_cum;
    dart_itoa(ute, leaves, ROM_LEAVES, itoa_buff);
    tt = ntohll(tt);
    set_date_str(itoa_buff->buffer, tt);
    fill_in_rom_field(ute, EXEC_TIME, itoa_buff->buffer, 17);
    char* liq = ack + 73;
    if(liq == 0) {
    } else {
        fill_in_rom_field(ute, ROM_LIQ, liq, 1);
    }
    destroy_gk_db(ob->gk, itoa_buff);
    return ret_val;
}

int parse_ex_filled(dart_order_obj * ute, char *ack, con_interface * ob)
{
    int ret_val = 0;
    return ret_val;
}

int ad_translate_rom_symbol(char *output, char *symbol, size_t sym_len)
{
    char *slash_off = 0;
    int ret_len = 0;
    char *off = output;
    slash_off = memchr(symbol, '/', sym_len);   //find_offset(symbol, sym_len, "/", 1);
    if (slash_off == 0) {
        memcpy(off, symbol, sym_len);
        ret_len = sym_len;
    } else {
        int ex_len = (sym_len - ((slash_off + 1) - symbol));
        int l_len = slash_off - symbol;
        memcpy(off, symbol, l_len);
        off += l_len;
        char *suffix = slash_off + 1;
        if (strncmp(suffix, "PR", 2) == 0) {
            *off++ = '-';
            if (ex_len > 2) {
                memcpy(off, slash_off + 3, (ex_len - 2));
                ret_len = l_len + 1 + (ex_len - 2);
            } else {
                ret_len = l_len + 1;
            }
        } else if (strncmp(suffix, "WS", 2) == 0) {
            *off++ = '+';
            if (ex_len > 2) {
                memcpy(off, slash_off + 3, (ex_len - 2));
                ret_len = l_len + 1 + (ex_len - 2);
            } else {
                ret_len = l_len + 1;
            }
        } else if (strncmp(suffix, "WI", 2) == 0) {
            if (ex_len > 2) {
                memcpy(off, slash_off + 3, (ex_len - 2));
                ret_len = l_len + 1 + (ex_len - 2);
                off += (ex_len - 2);
                *off++ = '#';
            } else {
                *off++ = '#';
                ret_len = l_len + 1;
            }
        } else {
            *off++ = '.';
            memcpy(off, slash_off + 1, ex_len);
            ret_len = l_len + 1 + ex_len;
        }
    }
    return ret_len;
}
