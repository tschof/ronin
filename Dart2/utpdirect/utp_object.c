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

#include "utp_object.h"
#include "utpdirect.h"
#include "connection_interface.h"
#include "nyse_seq_generator.h"
#include "databuf.h"
#include "ex_config_helper.h"
#include "rex_constants.h"
#include "dart_order_obj.h"
#include "log_mnger.h"
#include "dart_order_fact.h"


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

int rom_to_utp_price(char *price, size_t len)
{
    double d = strtod(price, NULL);
    d += .0000001;
    d = d * 10000;
    return (int) d;
}

double utp_to_rom_price(int data, char ps)
{
    double d = 10000;
    int a = 0;
    switch (ps) {
    default:
        d = 10000;
        break;
    case 0:
        d = 1;
        break;
    case 1:
        d = 10;
        break;
    case 2:
        d = 100;
        break;
    case 3:
        d = 1000;
        break;
    }
    a = ntohl(data);
    d = ((double) a / d) + .0000001;
    return d;
}

int utp_to_rom_bin_price(int data, char ps)
{
    int d = 1;
    switch (ps) {
    default:
        d = 1;
        break;
    case '0':
        d = 10000;
        break;
    case '1':
        d = 1000;
        break;
    case '2':
        d = 100;
        break;
    case '3':
        d = 10;
        break;
    case '6':
        d = 1000000;
        break;
    }
    int a = ntohl(data);
    return (a * d);
}

int find_utp_field_len(char *start, int max_len)
{
    char *end = start + (max_len - 1);
    while (end[0] == 0 && (end -start) >= 0) {
        --end;
    }
    if(end - start == 0) {
        return 0;
    } else {
        return (end - start) + 1;
    }
}

void get_rej_text(short reason, dart_order_obj * doj)
{
    switch (reason) {
    default:
        set_rom_field(doj, ROM_TEXT, "Unknown", 7);
        break;
    case 3000:
        set_rom_field(doj, ROM_TEXT, "Invalid field", 13);
        break;
    case 3001:
        set_rom_field(doj, ROM_TEXT, "Required Field missing", 22);
        break;
    case 3002:
        set_rom_field(doj, ROM_TEXT,
                        "Both CancelQty and Leaves Qty are required.", 40);
        break;
    case 3003:
        set_rom_field(doj, ROM_TEXT, "Target System unavailable", 25);
        break;
    case 3004:
        set_rom_field(doj, ROM_TEXT, "Symbol is not a valid NYSE symbol",
                        33);
        break;
    case 4000:
        set_rom_field(doj, ROM_TEXT, "NYSE down or stock halted", 25);
        break;
    case 4001:
        set_rom_field(doj, ROM_TEXT, "Invalid stock symbol", 20);
        break;
    case 2:
        set_rom_field(doj, ROM_TEXT, "Symbol already open", 19);
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

void set_date_str(char *output, int tt)
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

int create_new_order_message(databuf_t * buff, void *r, int seq,
                             con_int* ob)
{
    dart_order_obj *rph = (dart_order_obj *) r;
    utpp* uparse = (utpp*)ob->cparser;
    int all_good = 1;
    int cur_size = 0;
    int null_int = 0;
    char *off = 0;
    short mess_type = (htons(0x0041));
    short mess_len = (htons(84));
    int net_seq = htonl(seq);
    databuf_memcpy(buff, (char *) (&mess_type), 2);
    databuf_memcpy(buff, (char *) &mess_len, 2);
    databuf_memcpy(buff, (char *) &net_seq, 4);
    if ((rph->positions[ROM_SHARES].iov_len) > 0
        && rph->positions[ROM_BIN_QTY].iov_len <= 0) {
        int net_qty =
            strtol(rph->positions[ROM_SHARES].iov_base, NULL, 10);
        set_rom_field(rph, ROM_BIN_QTY, &net_qty, sizeof(int));
        set_rom_field(rph, ROM_BIN_CUM, &null_int, sizeof(int));
        set_rom_field(rph, ROM_LEAVES, rph->positions[ROM_SHARES].iov_base,
                      rph->positions[ROM_SHARES].iov_len);
        net_qty = htonl(net_qty);
        databuf_memcpy(buff, (char *) &net_qty, 4);
    } else if (rph->positions[ROM_BIN_QTY].iov_len > 0) {
        int net_qty = 0;
        memcpy(&net_qty, rph->positions[ROM_BIN_QTY].iov_base, 4);
        set_rom_field(rph, ROM_BIN_CUM, &null_int, sizeof(int));
        set_rom_field(rph, ROM_LEAVES, rph->positions[ROM_SHARES].iov_base,
                      rph->positions[ROM_SHARES].iov_len);
        net_qty = htonl(net_qty);
        databuf_memcpy(buff, (char *) &net_qty, 4);
    } else {
        all_good = 0;
    }
    if ((rph->positions[ROM_MAX_FLOOR].iov_len) > 0) {
        int maxfloor =
            htonl(strtol
                  (rph->positions[ROM_MAX_FLOOR].iov_base, NULL, 10));
        set_rom_field(rph, ROM_BIN_MAXFLOOR, &maxfloor, sizeof(int));
        databuf_memcpy(buff, (char *) &maxfloor, 4);
    } else {
        databuf_memcpy(buff, (char *) &null_int, 4);
    }
    if ((rph->positions[ROM_PRICE].iov_len) > 0
        && rph->positions[ROM_BIN_PRICE].iov_len <= 0) {
        int np = rom_to_utp_price(rph->positions[ROM_PRICE].iov_base,
                                  rph->positions[ROM_PRICE].iov_len);
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
        databuf_memcpy(buff, (char *) &null_int, 4);
    }
    *buff->wr_ptr++ = '4';
    if ((rph->positions[ROM_SYM].iov_len) > 0) {
        cur_size =
            utp_translate_rom_symbol(buff->wr_ptr,
                                     rph->positions[ROM_SYM].iov_base,
                                     rph->positions[ROM_SYM].iov_len);
        buff->wr_ptr += cur_size;
    } else {
        all_good = 0;
    }
    buff->wr_ptr = buff->buffer + 32;
    if ((rph->positions[ROM_EXEC_INST].iov_len) > 0) {
        off = (char *) rph->positions[ROM_EXEC_INST].iov_base;
        *buff->wr_ptr++ = off[0];
    } else {
        *buff->wr_ptr++ = '\0';
    }
    off = rph->positions[ROM_SIDE].iov_base;
    *buff->wr_ptr++ = off[0];

    off = rph->positions[ROM_TYPE].iov_base;
    if (strncmp(off, "11", 2) == 0) {
        *buff->wr_ptr++ = 'B';
    } else {
        *buff->wr_ptr++ = off[0];
    }
    if ((rph->positions[ROM_TIF].iov_len) > 0) {
        off = rph->positions[ROM_TIF].iov_base;
        *buff->wr_ptr++ = off[0];
    } else {
        *buff->wr_ptr++ = '\0';
    }
    *buff->wr_ptr++ = 'A';

    if ((rph->positions[ROM_INSTR].iov_len) > 0) {
        off = rph->positions[ROM_INSTR].iov_base;
        if (off[0] == 'A') {
            *buff->wr_ptr++ = 'D';
        } else {
            *buff->wr_ptr++ = off[0];
        }
        set_rom_field(rph, ROM_COPY_INSTR,
                        rph->positions[ROM_INSTR].iov_base,
                        rph->positions[ROM_INSTR].iov_len);
    } else {
        buff->wr_ptr++;
    }
    if (rph->positions[ROM_MAX_FLOOR].iov_len > 0) {
        *buff->wr_ptr++ = 'Y';
    } else {
        buff->wr_ptr++;
    }
    char *on_behalf = get_rom_field(rph, ROM_ON_BEHALF, &cur_size);
    databuf_memcpy(buff, on_behalf, cur_size);
    buff->wr_ptr = buff->buffer + 54;
    if ((rph->positions[ROM_CLR_ACC].iov_len) > 0) {
        databuf_memcpy(buff, rph->positions[ROM_CLR_ACC].iov_base,
                       rph->positions[ROM_CLR_ACC].iov_len);
    }
    check_and_resize(rph, 17);
    cur_size = in_place_order_id(get_seq_struct(uparse), rph->current->data->wr_ptr);
    reset_rom_field(rph, ROM_CLORDID, rph->current->data->wr_ptr,
                    cur_size);
    set_rom_field(rph, ROM_EX_TAG, rph->current->data->wr_ptr,
                    cur_size);
    buff->wr_ptr = buff->buffer + 64;
    memcpy(buff->wr_ptr, rph->current->data->wr_ptr, cur_size);
    buff->wr_ptr += 17;
    pthread_spin_lock(&ob->mutex);
    no_copy_insert(ob->cl_ord_id_map, rph->current->data->wr_ptr, cur_size,
                   rph);
    pthread_spin_unlock(&ob->mutex);
    rph->current->data->wr_ptr += cur_size;
    buff->wr_ptr += 3;
    return all_good;
}

int create_cancel_message(databuf_t * buff, void *r, int seq,
                          con_int* ob)
{
    int all_good = 1;
    int cur_size = 0;
    char *off = 0;
    dart_order_obj *ute = (dart_order_obj *) r;
    utpp* uparse = (utpp*)ob->cparser;
    short mess_type = (htons(0x0061));
    short mess_len = (htons(92));
    int net_seq = htonl(seq);
    int ex_id = 0;
    int orig = 0;
    memcpy(&orig, ute->positions[ROM_BIN_QTY].iov_base, 4);
    orig = htonl(orig);
    databuf_memcpy(buff, (char *) (&mess_type), 2);
    databuf_memcpy(buff, (char *) &mess_len, 2);
    databuf_memcpy(buff, (char *) &net_seq, 4);
    databuf_memcpy(buff, (char *) &ex_id, 4);
    databuf_memcpy(buff, (char *) &orig, 4);
    buff->wr_ptr = buff->buffer + 24;
    databuf_memcpy(buff, ute->positions[ROM_SYM].iov_base,
                   ute->positions[ROM_SYM].iov_len);
    buff->wr_ptr = buff->buffer + 35;
    off = ute->positions[ROM_SIDE].iov_base;
    *buff->wr_ptr++ = off[0];
    databuf_memcpy(buff, ute->positions[ROM_ON_BEHALF].iov_base,
                   ute->positions[ROM_ON_BEHALF].iov_len);
    buff->wr_ptr = buff->buffer + 46;
    databuf_memcpy(buff, ute->positions[ROM_CLR_ACC].iov_base,
                   ute->positions[ROM_CLR_ACC].iov_len);

    buff->wr_ptr = buff->buffer + 56;
    /*If we have a pending ID already we override it*/
    if(getplen(ute, ROM_PENDING_ID) > 0) {
        cur_size = in_place_order_id(get_seq_struct(uparse),
                                     getpval(ute, ROM_PENDING_ID));
        databuf_memcpy(buff, getpval(ute, ROM_PENDING_ID), cur_size);
    } else {
        check_and_resize(ute, 17);
        cur_size = in_place_order_id(get_seq_struct(uparse),
                                    ute->current->data->wr_ptr);
        reset_rom_field(ute, ROM_PENDING_ID, ute->current->data->wr_ptr,
                        cur_size);
        databuf_memcpy(buff, ute->current->data->wr_ptr, cur_size);
        ute->current->data->wr_ptr += cur_size;
    }
    buff->wr_ptr = buff->buffer + 73;
    memcpy(buff->wr_ptr, ute->positions[ROM_CLORDID].iov_base,
                   ute->positions[ROM_CLORDID].iov_len);
    buff->wr_ptr += 17;
    buff->wr_ptr += 2;
    return all_good;
}

int create_replace_message(databuf_t * buff, void *r, int seq,
                           con_int* ob)
{
    int all_good = 1;
    int cur_size = 0;
    int null_int = 0;
    char *off = 0;
    dart_order_obj *ute = (dart_order_obj *) r;
    utpp* uparse = (utpp*)ob->cparser;
    short mess_type = (htons(0x0071));
    short mess_len = (htons(104));
    int net_seq = htonl(seq);
    int ex_id = 0;              //htonl(ute->me_order_id);
    dart_order_obj *rph = (dart_order_obj *) ute->rep;
    databuf_memcpy(buff, (char *) (&mess_type), 2);
    databuf_memcpy(buff, (char *) &mess_len, 2);
    databuf_memcpy(buff, (char *) &net_seq, 4);
    databuf_memcpy(buff, (char *) &ex_id, 4);

    if ((rph->positions[ROM_SHARES].iov_len) > 0
        && rph->positions[ROM_BIN_QTY].iov_len <= 0) {
        int net_qty =
            strtol(rph->positions[ROM_SHARES].iov_base, NULL, 10);
        net_qty = htonl(net_qty);
        databuf_memcpy(buff, (char *) &net_qty, 4);
    } else if (rph->positions[ROM_BIN_QTY].iov_len > 0) {
        int net_qty = 0;
        memcpy(&net_qty, rph->positions[ROM_BIN_QTY].iov_base, 4);
        net_qty = htonl(net_qty);
        databuf_memcpy(buff, (char *) &net_qty, 4);
    } else {
        int net_qty = 0;
        memcpy(&net_qty, ute->positions[ROM_BIN_QTY].iov_base, 4);
        net_qty = htonl(net_qty);
        databuf_memcpy(buff, (char *) &net_qty, 4);
    }
    if ((rph->positions[ROM_MAX_FLOOR].iov_len) > 0) {
        int max_floor =
            htonl(strtol
                  (rph->positions[ROM_MAX_FLOOR].iov_base, NULL, 10));
        databuf_memcpy(buff, (char *) &max_floor, 4);
    } else {
        databuf_memcpy(buff, (char *) &null_int, 4);
    }
    if ((rph->positions[ROM_PRICE].iov_len) > 0
        && rph->positions[ROM_BIN_PRICE].iov_len <= 0) {
        int np = rom_to_utp_price(rph->positions[ROM_PRICE].iov_base,
                                  rph->positions[ROM_PRICE].iov_len);
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
        memcpy(&np, rph->positions[ROM_BIN_PRICE].iov_base,
               rph->positions[ROM_BIN_PRICE].iov_len);
        np = htonl(np);
        databuf_memcpy(buff, &np, 4);
    }
    *buff->wr_ptr++ = '4';
    databuf_memcpy(buff, ute->positions[ROM_SYM].iov_base,
                   ute->positions[ROM_SYM].iov_len);
    buff->wr_ptr = buff->buffer + 36;
    if (ute->positions[ROM_EXEC_INST].iov_len > 0) {
        off = ute->positions[ROM_EXEC_INST].iov_base;
        *buff->wr_ptr++ = off[0];
    } else {
        buff->wr_ptr++;
    }
    if ((rph->positions[ROM_INSTR].iov_len) > 0) {
        off = rph->positions[ROM_INSTR].iov_base;
        if (off[0] == 'A') {
            *buff->wr_ptr++ = 'D';
        } else {
            off = rph->positions[ROM_INSTR].iov_base;
            *buff->wr_ptr++ = off[0];
        }
    } else {
        *buff->wr_ptr++ = '\0';
    }
    off = ute->positions[ROM_SIDE].iov_base;
    *buff->wr_ptr++ = off[0];
    if ((rph->positions[ROM_TYPE].iov_len) > 0) {
        if (strncmp(off, "11", 2) == 0) {
            *buff->wr_ptr++ = 'B';
        } else {
            off = rph->positions[ROM_TYPE].iov_base;
            *buff->wr_ptr++ = off[0];
        }
    } else {
        if (strncmp(off, "11", 2) == 0) {
            *buff->wr_ptr++ = 'B';
        } else {
            off = ute->positions[ROM_TYPE].iov_base;
            *buff->wr_ptr++ = off[0];
        }
    }
    if ((rph->positions[ROM_TIF].iov_len) > 0) {
        off = rph->positions[ROM_TIF].iov_base;
        *buff->wr_ptr++ = off[0];
    } else {
        off = ute->positions[ROM_TIF].iov_base;
        *buff->wr_ptr++ = off[0];
    }
    *buff->wr_ptr++ = 'A';
    if (rph->positions[ROM_MAX_FLOOR].iov_len > 0) {
        *buff->wr_ptr++ = 'Y';
    } else {
        *buff->wr_ptr++ = '\0';
    }
    char *on_behalf = get_rom_field(ute, ROM_ON_BEHALF, &cur_size);
    databuf_memcpy(buff, on_behalf, cur_size);
    buff->wr_ptr = buff->buffer + 58;
    if ((rph->positions[ROM_CLR_ACC].iov_len) > 0) {
        databuf_memcpy(buff, rph->positions[ROM_CLR_ACC].iov_base,
                       rph->positions[ROM_CLR_ACC].iov_len);
    }
    buff->wr_ptr = buff->buffer + 68;
    /**
     * If we have a pending id with a length > 0 we overwrite it.
     * Since UTP direct order ids are all 17 bytes long.
     */
    if(getplen(ute, ROM_PENDING_ID) > 0) {
        cur_size = in_place_order_id(get_seq_struct(uparse),
                                     getpval(ute, ROM_PENDING_ID));
        memcpy(buff->wr_ptr, getpval(ute, ROM_PENDING_ID), cur_size);
        buff->wr_ptr += 17;
    } else {
        check_and_resize(ute, 17);
        cur_size = in_place_order_id(get_seq_struct(uparse), ute->current->data->wr_ptr);
        reset_rom_field(ute, ROM_PENDING_ID, ute->current->data->wr_ptr,
                        cur_size);
        memcpy(buff->wr_ptr, ute->current->data->wr_ptr, cur_size);
        buff->wr_ptr += 17;
        ute->current->data->wr_ptr += cur_size;
    }
    pthread_spin_lock(&ob->mutex);
    no_copy_insert(ob->cl_ord_id_map, getpval(ute,ROM_PENDING_ID), getplen(ute,ROM_PENDING_ID),
                   ute);
    pthread_spin_unlock(&ob->mutex);
    buff->wr_ptr = buff->buffer + 85;
    memcpy(buff->wr_ptr, ute->positions[ROM_CLORDID].iov_base,
                   ute->positions[ROM_CLORDID].iov_len);
    buff->wr_ptr += 17;
    buff->wr_ptr += 2;
    return all_good;
}

int parse_order_ack(dart_order_obj * ute, char *ack, con_int* ob)
{
    int tt = 0;
    memcpy(&tt, (ack + 12), 4);
    set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    set_rom_field(ute, ROM_STATUS, "13", 2);
    tt = ntohl(tt);
    databuf_t *itoa_buff = get_databuf(ob->gk);
    set_date_str(itoa_buff->buffer, tt);
    fill_in_rom_field(ute, EXEC_TIME, itoa_buff->buffer, 17);
    destroy_gk_db(ob->gk, itoa_buff);
    return 1;
}

int parse_cancel_ack(dart_order_obj * ute, char *ack, con_int* ob)
{
    int tt = 0;
    memcpy(&tt, (ack + 12), 4);
    set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    set_rom_field(ute, ROM_STATUS, "6", 1);
    tt = ntohl(tt);
    databuf_t *itoa_buff = get_databuf(ob->gk);
    set_date_str(itoa_buff->buffer, tt);
    fill_in_rom_field(ute, EXEC_TIME, itoa_buff->buffer, 17);
    destroy_gk_db(ob->gk, itoa_buff);
    return 1;
}

int parse_ur_out(dart_order_obj * ute, char *ack, con_int* ob)
{
    int tt = 0;
    memcpy(&tt, (ack + 12), 4);
    set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    set_rom_field(ute, ROM_STATUS, "4", 1);
    tt = ntohl(tt);
    databuf_t *itoa_buff = get_databuf(ob->gk);
    set_date_str(itoa_buff->buffer, tt);
    fill_in_rom_field(ute, EXEC_TIME, itoa_buff->buffer, 17);
    destroy_gk_db(ob->gk, itoa_buff);
    set_rom_field(ute, ROM_LEAVES, "0", 1);
    pthread_spin_lock(&ob->mutex);
    remove_from_map(ob->cl_ord_id_map,
                    ute->positions[ROM_CLORDID].iov_base,
                    ute->positions[ROM_CLORDID].iov_len);
    pthread_spin_unlock(&ob->mutex);
    return 1;
}

int parse_cancel_rep(dart_order_obj * ute, char *ack, con_int* ob)
{
    int tt = 0;
    memcpy(&tt, (ack + 12), 4);
    set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    set_rom_field(ute, ROM_STATUS, "6", 1);
    tt = ntohl(tt);
    databuf_t *itoa_buff = get_databuf(ob->gk);
    set_date_str(itoa_buff->buffer, tt);
    fill_in_rom_field(ute, EXEC_TIME, itoa_buff->buffer, 17);
    destroy_gk_db(ob->gk, itoa_buff);
    return 1;
}

int parse_replaced(dart_order_obj * ute, char *ack, con_int* ob)
{
    int tt = 0;
    memcpy(&tt, (ack + 12), 4);
    set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    set_rom_field(ute, ROM_STATUS, "5", 1);
    tt = ntohl(tt);
    databuf_t *buff = get_databuf(ob->gk);
    set_date_str(buff->buffer, tt);
    fill_in_rom_field(ute, EXEC_TIME, buff->buffer, 17);
    dart_order_obj *rep = (dart_order_obj *) ute->rep;
    fill_in_rom_field(ute, ROM_SHARES, rep->positions[ROM_SHARES].iov_base,
                  rep->positions[ROM_SHARES].iov_len);
    int l_q = strtol(rep->positions[ROM_SHARES].iov_base, NULL, 10);
    memcpy(ute->positions[ROM_BIN_QTY].iov_base, &l_q, 4);
    memcpy(ute->positions[ROM_BIN_PRICE].iov_base,
           rep->positions[ROM_BIN_PRICE].iov_base, 4);
    fill_in_rom_field(ute, ROM_PRICE, rep->positions[ROM_PRICE].iov_base,
                  rep->positions[ROM_PRICE].iov_len);
    //setting leaves
    int l_cum = 0;
    memcpy(&l_cum, ute->positions[ROM_BIN_CUM].iov_base, 4);
    int leaves = l_q - l_cum;
    dart_itoa(ute, leaves, ROM_LEAVES, buff);

    //End leaves
    pthread_spin_lock(&ob->mutex);
    remove_from_map(ob->cl_ord_id_map,
                    ute->positions[ROM_CLORDID].iov_base,
                    ute->positions[ROM_CLORDID].iov_len);
    /*Save off the old position, we are going to perform a switcharoo.*/
    void* old_ord_off = getpval(ute, ROM_CLORDID);
    set_rom_field(ute, ROM_CLORDID, ute->positions[ROM_PENDING_ID].iov_base,
                    ute->positions[ROM_PENDING_ID].iov_len);
    set_rom_field(ute, ROM_PENDING_ID, old_ord_off, getplen(ute,ROM_PENDING_ID));
    reset_doj(((dartfact*)rep->dalloc), rep);
    destroy_gk_db(ob->gk, buff);
    ute->rep = 0;
    pthread_spin_unlock(&ob->mutex);
    return 1;
}

int parse_rej(dart_order_obj * ute, char *ack, con_int* ob)
{
    int tt = 0;
    int ret_rej = 1;
    char rej_type = '1';
    short rej_reason = 0;
    memcpy(&tt, (ack + 12), 4);
    memcpy(&rej_reason, ack + 16, 2);
    memcpy(&rej_type, (ack + 18), 1);
    switch (rej_type) {
    case '1':
        set_rom_field(ute, ROM_STATUS, "8", 1);
        set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
        pthread_spin_lock(&ob->mutex);
        remove_from_map(ob->cl_ord_id_map,
                        ute->positions[ROM_CLORDID].iov_base,
                        ute->positions[ROM_CLORDID].iov_len);
        pthread_spin_unlock(&ob->mutex);
        break;
    case '2':
        set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
        set_rom_field(ute, ROM_STATUS, "14", 2);
        ret_rej = 2;
        break;
    case '3':
        set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
        set_rom_field(ute, ROM_STATUS, "20", 2);
        pthread_spin_lock(&ob->mutex);
        remove_from_map(ob->cl_ord_id_map,
                        ute->positions[ROM_PENDING_ID].iov_base,
                        ute->positions[ROM_PENDING_ID].iov_len);
        pthread_spin_unlock(&ob->mutex);
        ret_rej = 3;
        break;
    }
    rej_reason = ntohs(rej_reason);
    get_rej_text(rej_reason, ute);
    tt = ntohl(tt);
    databuf_t *itoa_buff = get_databuf(ob->gk);
    set_date_str(itoa_buff->buffer, tt);
    fill_in_rom_field(ute, EXEC_TIME, itoa_buff->buffer, 17);
    destroy_gk_db(ob->gk, itoa_buff);
    return ret_rej;
}

int parse_filled(dart_order_obj * ute, char *ack, con_int* ob)
{
    int tt = 0;
    int last_shares = 0;
    int leaves = 0;
    int last_price = 0;
    int ret_val = 0;
    char ps = '4';
    databuf_t *buff = get_databuf(ob->gk);
    memcpy(&tt, (ack + 12), 4);
    memcpy(&leaves, (ack + 16), 4);
    memcpy(&last_shares, (ack + 20), 4);
    memcpy(&last_price, (ack + 24), 4);
    memcpy(&ps, ack + 28, 1);
    leaves = ntohl(leaves);
    last_shares = ntohl(last_shares);
    int rblp = utp_to_rom_bin_price(last_price, ps);
    fill_in_rom_field(ute, ROM_BIN_LAST_PRICE, &rblp, sizeof(int));
    double lp = utp_to_rom_price(last_price, ps);
    int dlen = sprintf(buff->buffer, "%f", lp);
    fill_in_rom_field(ute, ROM_EXEC_PRICE, buff->buffer, dlen);
    set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    int l_cum = 0;
    int qty = 0;
    memcpy(&qty, ute->positions[ROM_BIN_QTY].iov_base, 4);
    memcpy(&l_cum, ute->positions[ROM_BIN_CUM].iov_base, 4);
    l_cum += last_shares;
    fill_in_rom_field(ute, ROM_BIN_LAST_SHARES, &last_shares, sizeof(int));
    fill_in_rom_field(ute, ROM_BIN_CUM, &l_cum, 4);
    if (l_cum >= qty) {
        set_rom_field(ute, ROM_STATUS, "2", 1);
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
    } else {
        set_rom_field(ute, ROM_STATUS, "1", 1);
    }
    dart_itoa(ute, l_cum, ROM_CUM, buff);
    dart_itoa(ute, last_shares, ROM_LAST_SHARES, buff);

    dart_itoa(ute, leaves, ROM_LEAVES, buff);
    //End leaves
    tt = ntohl(tt);
    set_date_str(buff->buffer, tt);
    fill_in_rom_field(ute, EXEC_TIME, buff->buffer, 17);
    destroy_gk_db(ob->gk, buff);
    int execid_len = find_utp_field_len((ack + 69), 10);
    set_rom_field(ute, EXEC_ID, ack + 69, execid_len);
    char liq = '0';
    memcpy(&liq, ack + 30, 1);
    if(liq >= '1' && liq <= '9') {
        set_rom_field(ute, ROM_LIQ, ack + 30, 1);
    }
    return ret_val;
}

int utp_translate_rom_symbol(char *output, char *symbol, size_t sym_len)
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
        *off++ = ' ';
        memcpy(off, slash_off + 1, ex_len);
        ret_len = l_len + 1 + ex_len;
        *slash_off = ' ';
    }
    return ret_len;
}
