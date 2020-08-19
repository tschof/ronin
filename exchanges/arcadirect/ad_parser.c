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
#include "dart_orderbook.h"
#include "dart_constants.h"
#include "ex_common.h"
#include "db_layer.h"
#include "databuf.h"
#include "dart_constants.h"
#include "order_token_creator.h"
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

static char *today = 0;
static int day_light_sav = 0;


unsigned int ad_create_order_token(orderbook * ob)
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

int rom_to_ad_dis_off(char *price, size_t len)
{
    double d = strtod(price, NULL);
    d = d * 100;
    d += .000001;
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

void ad_database_init(struct database_conf *dbconf, const char *destID,
                      orderbook * ob)
{
    int res = 0;
    element_t e, temp;
    queue_t q = create_queue();
    e = NULL;
    temp = NULL;
    res = get_db_objects(dbconf, q, destID);
    e = dequeue(q);
    while (e != NULL) {
        struct dart_ex_result_set *ders = 0;
        dart_order_obj *obj = (dart_order_obj *) get_obj(ob->ord_obj);
        ders = (struct dart_ex_result_set *) e->d;
        if (ders != NULL) {
            set_rom_field(obj, ROM_SHARES, &ders->qty, 4);
            int l_len = strlen(ders->orderTag);
            set_rom_field(obj, ROM_CLORDID, ders->orderTag, l_len);
            no_copy_insert(ob->cl_ord_id_map, ders->orderTag, l_len, obj);
            l_len = strlen(ders->omTag);
            set_rom_field(obj, ROM_TAG, ders->omTag, l_len);
            no_copy_insert(ob->rom_ord_id_map, ders->omTag, l_len, obj);
            size_t rom_inst_len = 0;
            char *rom_inst = build_rom_name(ders->omTag, 32,
                                            &rom_inst_len);
            if (rom_inst != NULL) {
                set_rom_field(obj, ROM_SENDER, rom_inst, rom_inst_len);
                free(rom_inst);
            }
            set_rom_field(obj, ROM_CUM, &ders->cumQty, 4);
            switch (ders->side) {
            case 1:
                set_rom_field(obj, ROM_SIDE, "1", 1);
                break;
            case 2:
                set_rom_field(obj, ROM_SIDE, "2", 1);
                break;
            case 5:
                set_rom_field(obj, ROM_SIDE, "5", 1);
                break;
            }
            int sym_len = strlen(ders->symbol);
            set_rom_field(obj, ROM_SYM, ders->symbol, sym_len);
            set_rom_field(obj, ROM_ON_BEHALF, ob->in_args->sender_comp,
                          ob->in_args->sc_len);
            int account_len = strlen(ders->clearingAccount);
            set_rom_field(obj, ROM_CLR_ACC, ders->clearingAccount,
                          account_len);
            double d = ders->price;
            d = d * 10000;
            d += .0000001;
            int setter = (int) d;
            set_rom_field(obj, ROM_BIN_PRICE, &setter, sizeof(int));
            switch (ders->orderType) {
            case 1:
                set_rom_field(obj, ROM_TYPE, "1", 1);
                break;
            case 2:
                set_rom_field(obj, ROM_TYPE, "2", 1);
                break;
            case 3:
                set_rom_field(obj, ROM_TYPE, "3", 1);
                break;
            case 5:
                set_rom_field(obj, ROM_TYPE, "5", 1);
                break;
            case 11:
                set_rom_field(obj, ROM_TYPE, "B", 1);
                break;
            }
        }
        temp = e;
        e = temp->next;
        if (ders != NULL) {
            free(ders);
        }
        temp->d = NULL;
        free(temp);
        temp = NULL;
    }
    destroy_queue(q);
}

int find_ad_field_len(char *start, int max_len)
{
    char *end = start + (max_len - 1);
    while (end[0] == 0) {
        --end;
    }
    return (end - start) + 1;
}

const char *get_rej_text(short reason)
{
    switch (reason) {
    default:
        return "Unknown";
    case 3000:
        return "Invalid field";
    case 3001:
        return "Required Field missing";
    case 3002:
        return "Both CancelQty and Leaves Qty are required.";
    case 3003:
        return "Target System unavailable";
    case 3004:
        return "Symbol is not a valid NYSE symbol";
    case 4000:
        return "NYSE down or stock halted";
    case 4001:
        return "Invalid stock symbol";
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

void set_date_str(databuf_t * output, int tt)
{
    if (today == 0) {
        init_today();
    }
    databuf_memcpy(output, today, 9);
    int total_secs = tt / 1000;
    int hour = ((total_secs / 3600) + day_light_sav) % 24;
    int min = (total_secs % 3600) / 60;
    int sec = (total_secs % 3600) % 60;
    const char *idx = digit_str[hour];
    *output->wr_ptr++ = *idx++;
    *output->wr_ptr++ = *idx++;
    *output->wr_ptr++ = ':';
    idx = digit_str[min];
    *output->wr_ptr++ = *idx++;
    *output->wr_ptr++ = *idx++;
    *output->wr_ptr++ = ':';
    idx = digit_str[sec];
    *output->wr_ptr++ = *idx++;
    *output->wr_ptr++ = *idx++;
}

int create_var_3_message(databuf_t * buff, void *r, int seq,
                         orderbook * ob)
{
    dart_order_obj *rph = (dart_order_obj *) r;
    int all_good = 1;
    int cur_size = 0;
    int null_int = 0;
    char q[10];
    char *off = 0;
    unsigned char mess_type = 0x03;
    short mess_len = (htons(164));
    int net_seq = 0;

    unsigned int clordid = ad_create_order_token(ob);
    *buff->wr_ptr++ = 'D';
    databuf_memcpy(buff, &mess_type, 1);
    databuf_memcpy(buff, (char *) &mess_len, 2);
    databuf_memcpy(buff, (char *) &net_seq, 4);
    net_seq = htonl(clordid);
    databuf_memcpy(buff, (char *) &net_seq, 4);
    buff->wr_ptr += 4;          //Skip the portfolio cross.
    if ((rph->positions[ROM_SHARES].val_len) > 0) {
        off = rph->data->buffer + rph->positions[ROM_SHARES].beg;
        int net_qty = strtol(off, NULL, 10);
        set_rom_field(rph, ROM_BIN_QTY, &net_qty, sizeof(int));
        set_rom_field(rph, ROM_BIN_CUM, &null_int, sizeof(int));
        net_qty = htonl(net_qty);
        databuf_memcpy(buff, (char *) &net_qty, 4);
    } else {
       all_good = 0;
    } 
    //buff->wr_ptr +=16;
    buff->wr_ptr += 8;
    if(rph->positions[34].val_len > 0) {
        off = rph->data->buffer + rph->positions[34].beg;
        int np = strtol(off, NULL, 10);
        np = htonl(np);
        databuf_memcpy(buff, (char*)&np,4);
    } else {
        buff->wr_ptr += 4;
    }
    buff->wr_ptr += 4;
    if((rph->positions[64].val_len) > 0) {
        off = rph->data->buffer + rph->positions[64].beg;
        int np = rom_to_ad_dis_off(off, rph->positions[64].val_len);
        np = htonl(np);
        databuf_memcpy(buff, (char *) &np, 4);
    } else {
        buff->wr_ptr += 4;      //Skip the discretion offset
    }
    buff->wr_ptr += 8;
    if ((rph->positions[ROM_PRICE].val_len) > 0) {
        off = rph->data->buffer + rph->positions[ROM_PRICE].beg;
        memset(q, 0, 10);
        memcpy(q, off, rph->positions[ROM_PRICE].val_len);
        int np = rom_to_ad_price(q, rph->positions[ROM_PRICE].val_len);
        np = htonl(np);
        set_rom_field(rph, ROM_BIN_PRICE, &np, sizeof(int));
        databuf_memcpy(buff, (char *) &np, 4);
    } else {
        databuf_memcpy(buff, (char *) &null_int, 4);
    }
    buff->wr_ptr += 6;          //Skip the ex destination.
    *buff->wr_ptr++ = '4';
    buff->wr_ptr += 2;
    if ((rph->positions[ROM_SYM].val_len) > 0) {
        off = rph->data->buffer + rph->positions[ROM_SYM].beg;
        cur_size =
            ad_translate_rom_symbol(rph->data->wr_ptr, off,
                                    rph->positions[ROM_SYM].val_len);
        databuf_memcpy(buff, rph->data->wr_ptr, cur_size);
        reset_rom_field(rph, ROM_SYM,
                        (rph->data->wr_ptr - rph->data->buffer), cur_size);
        rph->data->wr_ptr += cur_size;
    } else {
        all_good = 0;
    }
    buff->wr_ptr = buff->buffer + 77;
    char *on_behalf = get_rom_field(rph, ROM_ON_BEHALF, &cur_size);
    databuf_memcpy(buff, on_behalf, cur_size);
    buff->wr_ptr = buff->buffer + 92;
    if ((rph->positions[ROM_EXEC_INST].val_len) > 0) {
        off = rph->data->buffer + rph->positions[ROM_EXEC_INST].beg;
        *buff->wr_ptr++ = off[0];
    } else {
        *buff->wr_ptr++ = '\0';
    }
    off = rph->data->buffer + rph->positions[ROM_SIDE].beg;
    *buff->wr_ptr++ = off[0];

    off = rph->data->buffer + rph->positions[ROM_TYPE].beg;
    if (strncmp(off, "11", 2) == 0) {
        *buff->wr_ptr++ = 'B';
    } else {
        *buff->wr_ptr++ = off[0];
    }
    if ((rph->positions[ROM_TIF].val_len) > 0) {
        off = rph->data->buffer + rph->positions[ROM_TIF].beg;
        *buff->wr_ptr++ = off[0];
    } else {
        *buff->wr_ptr++ = '\0';
    }
    *buff->wr_ptr++ = 'A';
    buff->wr_ptr++;
    databuf_memcpy(buff, "123", 3);
    *buff->wr_ptr++ = '\0';
    if ((rph->positions[ROM_CLR_ACC].val_len) > 0) {
        off = rph->data->buffer + rph->positions[ROM_CLR_ACC].beg;
        databuf_memcpy(buff, off, rph->positions[ROM_CLR_ACC].val_len);
    }
    buff->wr_ptr = buff->buffer + 147;
    //buff->wr_ptr += (35 + boff); 
    //*buff->wr_ptr++ = '0';
    //*buff->wr_ptr++ = '1';
    buff->wr_ptr += 10;
    //*buff->wr_ptr++ = '\0';
    if((rph->positions[ROM_INSTR].val_len) > 0) {
       off = rph->data->buffer + rph->positions[ROM_INSTR].beg;
      *buff->wr_ptr++ = off[0];
    } else {
        buff->wr_ptr++;
    }
    buff->wr_ptr += 5;
    *buff->wr_ptr++ = '\n';

    set_rom_field(rph, ROM_CLORDID, &clordid, 4);
    pthread_mutex_lock(&ob->mutex);
    no_copy_insert(ob->cl_ord_id_map, (char *) &clordid, 4, rph);
    char *rom_id = rph->data->buffer + rph->positions[ROM_TAG].beg;
    int rom_id_len =
        rph->positions[ROM_TAG].end - rph->positions[ROM_TAG].beg;
    no_copy_insert(ob->rom_ord_id_map, rom_id, rom_id_len, rph);
    pthread_mutex_unlock(&ob->mutex);
    return all_good;
}

int create_new_order_message(databuf_t * buff, void *r, int seq,
                             orderbook * ob)
{
    dart_order_obj *rph = (dart_order_obj *) r;
    int all_good = 1;
    int cur_size = 0;
    int null_int = 0;
    char q[10];
    char *off = 0;
    unsigned char mess_type = 0x01;
    short mess_len = (htons(76));
    int net_seq = 0;

    unsigned int clordid = ad_create_order_token(ob);
    *buff->wr_ptr++ = 'D';
    databuf_memcpy(buff, &mess_type, 1);
    databuf_memcpy(buff, (char *) &mess_len, 2);
    databuf_memcpy(buff, (char *) &net_seq, 4);
    net_seq = htonl(clordid);
    databuf_memcpy(buff, (char *) &net_seq, 4);
    buff->wr_ptr += 4;          //Skip the portfolio cross.
    if ((rph->positions[ROM_SHARES].val_len) > 0) {
        off = rph->data->buffer + rph->positions[ROM_SHARES].beg;
        int net_qty = strtol(off, NULL, 10);
        set_rom_field(rph, ROM_BIN_QTY, &net_qty, sizeof(int));
        set_rom_field(rph, ROM_BIN_CUM, &null_int, sizeof(int));
        net_qty = htonl(net_qty);
        databuf_memcpy(buff, (char *) &net_qty, 4);
    } else {
        all_good = 0;
    }
    if ((rph->positions[ROM_PRICE].val_len) > 0) {
        off = rph->data->buffer + rph->positions[ROM_PRICE].beg;
        memset(q, 0, 10);
        memcpy(q, off, rph->positions[ROM_PRICE].val_len);
        int np = rom_to_ad_price(q, rph->positions[ROM_PRICE].val_len);
        np = htonl(np);
        set_rom_field(rph, ROM_BIN_PRICE, &np, sizeof(int));
        databuf_memcpy(buff, (char *) &np, 4);
    } else {
        databuf_memcpy(buff, (char *) &null_int, 4);
    }
    buff->wr_ptr += 2;          //Skip the ex destination.
    *buff->wr_ptr++ = '4';
    if ((rph->positions[ROM_SYM].val_len) > 0) {
        off = rph->data->buffer + rph->positions[ROM_SYM].beg;
        cur_size =
            ad_translate_rom_symbol(rph->data->wr_ptr, off,
                                    rph->positions[ROM_SYM].val_len);
        databuf_memcpy(buff, rph->data->wr_ptr, cur_size);
        reset_rom_field(rph, ROM_SYM,
                        (rph->data->wr_ptr - rph->data->buffer), cur_size);
        rph->data->wr_ptr += cur_size;
    } else {
        all_good = 0;
    }
    buff->wr_ptr = buff->buffer + 35;
    char *on_behalf = get_rom_field(rph, ROM_ON_BEHALF, &cur_size);
    databuf_memcpy(buff, on_behalf, cur_size);
    buff->wr_ptr = buff->buffer + 50;
    if ((rph->positions[ROM_EXEC_INST].val_len) > 0) {
        off = rph->data->buffer + rph->positions[ROM_EXEC_INST].beg;
        *buff->wr_ptr++ = off[0];
    } else {
        *buff->wr_ptr++ = '\0';
    }
    off = rph->data->buffer + rph->positions[ROM_SIDE].beg;
    *buff->wr_ptr++ = off[0];

    off = rph->data->buffer + rph->positions[ROM_TYPE].beg;
    if (strncmp(off, "11", 2) == 0) {
        *buff->wr_ptr++ = 'B';
    } else {
        *buff->wr_ptr++ = off[0];
    }
    if ((rph->positions[ROM_TIF].val_len) > 0) {
        off = rph->data->buffer + rph->positions[ROM_TIF].beg;
        *buff->wr_ptr++ = off[0];
    } else {
        *buff->wr_ptr++ = '\0';
    }
    *buff->wr_ptr++ = 'A';
    databuf_memcpy(buff, "123", 3);
    *buff->wr_ptr++ = '\0';
    if ((rph->positions[ROM_CLR_ACC].val_len) > 0) {
        off = rph->data->buffer + rph->positions[ROM_CLR_ACC].beg;
        databuf_memcpy(buff, off, rph->positions[ROM_CLR_ACC].val_len);
    }
    *buff->wr_ptr++ = '\0';
    if ((rph->positions[ROM_INSTR].val_len) > 0) {
        off = rph->data->buffer + rph->positions[ROM_INSTR].beg;
        *buff->wr_ptr++ = off[0];
    } else {
        buff->wr_ptr++;
    }
    buff->wr_ptr = buff->buffer + 75;
    *buff->wr_ptr++ = '\n';

    set_rom_field(rph, ROM_CLORDID, &clordid, 4);
    pthread_mutex_lock(&ob->mutex);
    no_copy_insert(ob->cl_ord_id_map, (char *) &clordid, 4, rph);
    char *rom_id = rph->data->buffer + rph->positions[ROM_TAG].beg;
    int rom_id_len =
        rph->positions[ROM_TAG].end - rph->positions[ROM_TAG].beg;
    no_copy_insert(ob->rom_ord_id_map, rom_id, rom_id_len, rph);
    pthread_mutex_unlock(&ob->mutex);
    return all_good;
}

int create_cancel_message(databuf_t * buff, void *r, int seq,
                          orderbook * ob)
{
    int all_good = 1;
    dart_order_obj *ute = (dart_order_obj *) r;
    unsigned char mess_type = 0x01;
    short mess_len = (htons(72));
    int net_seq = 0;
    long ex_id = 0;
    int orig = 0;
    unsigned int clordid = 0;
    memcpy(&orig, (ute->data->buffer + ute->positions[ROM_BIN_QTY].beg),
           4);
    memcpy(&ex_id, (ute->data->buffer + ute->positions[ROM_EX_ID].beg), 8);
    memcpy(&clordid, (ute->data->buffer + ute->positions[ROM_CLORDID].beg),
           4);
    clordid = htonl(clordid);
    orig = htonl(orig);

    *buff->wr_ptr++ = 'F';
    databuf_memcpy(buff, (char *) (&mess_type), 1);
    databuf_memcpy(buff, (char *) &mess_len, 2);
    databuf_memcpy(buff, (char *) &net_seq, 4);
    databuf_memcpy(buff, (char *) &ex_id, 8);
    databuf_memcpy(buff, (char *) &clordid, 4);
    buff->wr_ptr += 12;
    databuf_memcpy(buff, (char *) &orig, 4);
    buff->wr_ptr = buff->buffer + 32;
    databuf_memcpy(buff, (ute->data->buffer + ute->positions[ROM_SYM].beg),
                   ute->positions[ROM_SYM].val_len);
    buff->wr_ptr = buff->buffer + 48;
    *buff->wr_ptr++ = (ute->data->buffer + ute->positions[ROM_SIDE].beg)[0];
    buff->wr_ptr = buff->buffer + 71;
    *buff->wr_ptr++ = '\n';
    return all_good;
}

int create_replace_message(databuf_t * buff, void *r, int seq,
                           orderbook * ob)
{
    int all_good = 1;
    char q[10];
    char *off = 0;
    dart_order_obj *ute = (dart_order_obj *) r;
    unsigned char mess_type = 0x01;
    short mess_len = (htons(88));
    int net_seq = 0;
    long ex_id = 0;
    int orig_id = 0;
    unsigned int clordid = ad_create_order_token(ob);
    int net_clordid = htonl(clordid);
    memcpy(&ex_id, (ute->data->buffer + ute->positions[ROM_EX_ID].beg), 8);
    memcpy(&orig_id, (ute->data->buffer + ute->positions[ROM_CLORDID].beg),
           4);
    dart_order_obj *rph = (dart_order_obj *) ute->rep;
    *buff->wr_ptr++ = 'G';
    databuf_memcpy(buff, (char *) (&mess_type), 1);
    databuf_memcpy(buff, (char *) &mess_len, 2);
    databuf_memcpy(buff, (char *) &net_seq, 4);
    databuf_memcpy(buff, (char *) &ex_id, 8);
    databuf_memcpy(buff, (char *) &net_clordid, 4);
    net_clordid = htonl(orig_id);
    databuf_memcpy(buff, (char *) &net_clordid, 4);

    if ((rph->positions[ROM_SHARES].val_len) > 0) {
        off = rph->data->buffer + rph->positions[ROM_SHARES].beg;
        int net_qty = strtol(off, NULL, 10);
        net_qty = htonl(net_qty);
        databuf_memcpy(buff, (char *) &net_qty, 4);
    } else {
        int net_qty = 0;
        memcpy(&net_qty,
               (rph->data->buffer + rph->positions[ROM_BIN_QTY].beg), 4);
        net_qty = htonl(net_qty);
        databuf_memcpy(buff, (char *) &net_qty, 4);
    }
    buff->wr_ptr += 4;
    if ((rph->positions[ROM_PRICE].val_len) > 0) {
        off = rph->data->buffer + rph->positions[ROM_PRICE].beg;
        memset(q, 0, 10);
        memcpy(q, off, rph->positions[ROM_PRICE].val_len);
        int np = rom_to_ad_price(q, rph->positions[ROM_PRICE].val_len);
        np = htonl(np);
        set_rom_field(rph, ROM_BIN_PRICE, &np, sizeof(int));
        databuf_memcpy(buff, (char *) &np, 4);
    } else {
        databuf_memcpy(buff,
                       (ute->data->buffer +
                        ute->positions[ROM_BIN_PRICE].beg), 4);
    }
    buff->wr_ptr += 4;
    *buff->wr_ptr++ = '4';
    buff->wr_ptr += 3;
    databuf_memcpy(buff, ute->data->buffer + ute->positions[ROM_SYM].beg,
                   ute->positions[ROM_SYM].val_len);
    buff->wr_ptr = buff->buffer + 60;
    if (ute->positions[ROM_EXEC_INST].val_len > 0) {
        *buff->wr_ptr++ =
            (ute->data->buffer + ute->positions[ROM_EXEC_INST].beg)[0];
    } else {
        buff->wr_ptr++;
    }
    *buff->wr_ptr++ = (ute->data->buffer + ute->positions[ROM_SIDE].beg)[0];
    if ((rph->positions[ROM_TYPE].val_len) > 0) {
        off = rph->data->buffer + rph->positions[ROM_TYPE].beg;
        *buff->wr_ptr++ = off[0];
    } else {
        off = ute->data->buffer + ute->positions[ROM_TYPE].beg;
        *buff->wr_ptr++ = off[0];
    }
    if ((rph->positions[ROM_TIF].val_len) > 0) {
        off = rph->data->buffer + rph->positions[ROM_TIF].beg;
        *buff->wr_ptr++ = off[0];
    } else {
        off = ute->data->buffer + ute->positions[ROM_TIF].beg;
        *buff->wr_ptr++ = off[0];
    }
    *buff->wr_ptr++ = 'A';
    databuf_memcpy(buff, "123", 3);
    *buff->wr_ptr++ = '\0';
    buff->wr_ptr += 5;
    if ((rph->positions[ROM_CLR_ACC].val_len) > 0) {
        off = rph->data->buffer + rph->positions[ROM_CLR_ACC].beg;
        databuf_memcpy(buff, off, rph->positions[ROM_CLR_ACC].val_len);
    }
    buff->wr_ptr = buff->buffer + 87;
    *buff->wr_ptr++ = '\n';
    reset_rom_field(ute, ROM_ORIG_ID, ute->positions[ROM_CLORDID].beg,
                    ute->positions[ROM_CLORDID].val_len);
    set_rom_field(ute, ROM_CLORDID, &clordid, 4);
    pthread_mutex_lock(&ob->mutex);
    no_copy_insert(ob->cl_ord_id_map, (char *) &clordid, 4, ute);
    pthread_mutex_unlock(&ob->mutex);
    return all_good;
}

int parse_order_ack(dart_order_obj * ute, databuf_t * buff, char *ack,
                    orderbook * ob)
{
    int tt = 0;
    set_rom_field(ute, ROM_EX_ID, ack + 16, 8);
    memcpy(&tt, (ack + 8), 4);
    databuf_memcpy(buff, "13,", 3);
    unsigned int clordid = 0;
    long ex_id = 0;
    memcpy(&ex_id, ack + 16, 8);
    ex_id = ntohll(ex_id);
    memcpy(&clordid, ute->data->buffer + ute->positions[ROM_CLORDID].beg,
           ute->positions[ROM_CLORDID].val_len);
    databuf_write(buff, "%u,%lu", clordid, ex_id);
    *buff->wr_ptr++ = ',';
    databuf_memcpy(buff, ute->data->buffer + ute->positions[ROM_TAG].beg,
                   ute->positions[ROM_TAG].val_len);
    databuf_write(buff, ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,");
    tt = ntohl(tt);
    set_date_str(buff, tt);
    databuf_memcpy(buff, ",,,,,,,,,,,,,,,,\n", 17);
    return 1;
}

int parse_cancel_ack(dart_order_obj * ute, databuf_t * buff, char *ack,
                     orderbook * ob)
{
    int tt = 0;
    set_rom_field(ute, ROM_EX_ID, ack + 16, 8);
    memcpy(&tt, (ack + 8), 4);
    databuf_memcpy(buff, "6,", 2);
    unsigned int clordid = 0;
    long ex_id = 0;
    memcpy(&ex_id, ack + 16, 8);
    ex_id = ntohll(ex_id);
    memcpy(&clordid, ute->data->buffer + ute->positions[ROM_CLORDID].beg,
           ute->positions[ROM_CLORDID].val_len);
    databuf_write(buff, "%u,%lu", clordid, ex_id);
    *buff->wr_ptr++ = ',';
    databuf_memcpy(buff, ute->data->buffer + ute->positions[ROM_TAG].beg,
                   ute->positions[ROM_TAG].val_len);
    databuf_write(buff, ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,");
    tt = ntohl(tt);
    set_date_str(buff, tt);
    databuf_memcpy(buff, ",,,,,,,,,,,,,,,,\n", 17);
    return 1;
}

int parse_ur_out(dart_order_obj * ute, databuf_t * buff, char *ack,
                 orderbook * ob)
{
    int tt = 0;
    set_rom_field(ute, ROM_EX_ID, ack + 16, 8);
    memcpy(&tt, (ack + 8), 4);
    databuf_memcpy(buff, "4,", 2);
    unsigned int clordid = 0;
    memcpy(&clordid, ute->data->buffer + ute->positions[ROM_CLORDID].beg,
           ute->positions[ROM_CLORDID].val_len);
    databuf_write(buff, "%u", clordid);
    *buff->wr_ptr++ = ',';
    *buff->wr_ptr++ = ',';
    databuf_memcpy(buff, ute->data->buffer + ute->positions[ROM_TAG].beg,
                   ute->positions[ROM_TAG].val_len);
    databuf_write(buff, ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,");
    tt = ntohl(tt);
    set_date_str(buff, tt);
    databuf_memcpy(buff, ",,,,,,,,,,,,,,,,\n", 17);
    pthread_mutex_lock(&ob->mutex);
    remove_from_map(ob->cl_ord_id_map, (char *) &clordid, 4);
    remove_from_map(ob->rom_ord_id_map,
                    ute->data->buffer + ute->positions[ROM_TAG].beg,
                    ute->positions[ROM_TAG].val_len);
    pthread_mutex_unlock(&ob->mutex);
    return 1;
}

int parse_cancel_rep(dart_order_obj * ute, databuf_t * buff, char *ack,
                     orderbook * ob)
{
    int tt = 0;
    set_rom_field(ute, ROM_EX_ID, ack + 16, 8);
    memcpy(&tt, (ack + 8), 4);
    databuf_memcpy(buff, "6,", 2);
    unsigned int clordid = 0;
    memcpy(&clordid, ute->data->buffer + ute->positions[ROM_CLORDID].beg,
           ute->positions[ROM_CLORDID].val_len);
    databuf_write(buff, "%u", clordid);
    *buff->wr_ptr++ = ',';
    *buff->wr_ptr++ = ',';
    databuf_memcpy(buff, ute->data->buffer + ute->positions[ROM_TAG].beg,
                   ute->positions[ROM_TAG].val_len);
    databuf_write(buff, ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,");
    tt = ntohl(tt);
    set_date_str(buff, tt);
    databuf_memcpy(buff, ",,,,,,,,,,,,,,,,\n", 17);
    return 1;
}

int parse_replaced(dart_order_obj * ute, databuf_t * buff, char *ack,
                   orderbook * ob)
{
    int tt = 0;
    set_rom_field(ute, ROM_EX_ID, ack + 16, 8);
    memcpy(&tt, (ack + 8), 4);
    databuf_memcpy(buff, "5,", 2);
    unsigned int clordid = 0;
    long ex_id = 0;
    memcpy(&ex_id, ack + 16, 8);
    ex_id = ntohll(ex_id);
    memcpy(&clordid, ute->data->buffer + ute->positions[ROM_CLORDID].beg,
           ute->positions[ROM_CLORDID].val_len);
    databuf_write(buff, "%u,%lu", clordid, ex_id);
    *buff->wr_ptr++ = ',';
    databuf_memcpy(buff, ute->data->buffer + ute->positions[ROM_TAG].beg,
                   ute->positions[ROM_TAG].val_len);
    databuf_write(buff, ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,");
    tt = ntohl(tt);
    set_date_str(buff, tt);
    databuf_memcpy(buff, ",,,,,,,,,,,,,,,,\n", 17);
    dart_order_obj *rep = (dart_order_obj *) ute->rep;
    char *off = rep->data->buffer + rep->positions[ROM_SHARES].beg;
    set_rom_field(ute, 6, off, rep->positions[ROM_SHARES].val_len);
    int l_q = strtol(off, NULL, 10);
    memcpy(ute->data->buffer + ute->positions[ROM_BIN_QTY].beg, &l_q, 4);
    memcpy(ute->data->buffer + ute->positions[ROM_BIN_PRICE].beg,
           rep->data->buffer + ute->positions[ROM_BIN_PRICE].beg, 4);
    set_rom_field(ute, 7, rep->data->buffer + rep->positions[7].beg,
                  rep->positions[7].val_len);
    int rem_id = 0;
    memcpy(&rem_id, ute->data->buffer + ute->positions[ROM_ORIG_ID].beg,
           ute->positions[ROM_ORIG_ID].val_len);
    pthread_mutex_lock(&ob->mutex);
    remove_from_map(ob->cl_ord_id_map, (char *) &rem_id, 4);
    destroy_elem(ob->ord_obj, rep);
    pthread_mutex_unlock(&ob->mutex);
    return 1;
}

int parse_rej(dart_order_obj * ute, databuf_t * buff, char *ack,
              orderbook * ob)
{
    int tt = 0;
    char rej_type = '1';
    char rej_reason[41];
    unsigned int clordid = 0;
    memcpy(&tt, (ack + 8), 4);
    memcpy(&rej_reason, ack + 16, 2);
    memcpy(&rej_type, (ack + 20), 1);
    switch (rej_type) {
    case '1':
        databuf_memcpy(buff, "8,", 2);
        memcpy(&clordid,
               ute->data->buffer + ute->positions[ROM_CLORDID].beg,
               ute->positions[ROM_CLORDID].val_len);
        pthread_mutex_lock(&ob->mutex);
        remove_from_map(ob->cl_ord_id_map, (char *) &clordid, 4);
        remove_from_map(ob->rom_ord_id_map,
                        ute->data->buffer + ute->positions[ROM_TAG].beg,
                        ute->positions[ROM_TAG].val_len);
        pthread_mutex_unlock(&ob->mutex);
        break;
    case '2':
        databuf_memcpy(buff, "14,", 3);
        reset_rom_field(ute, ROM_CLORDID, ute->positions[ROM_ORIG_ID].beg,
                        ute->positions[ROM_ORIG_ID].val_len);
        break;
    case '3':
        databuf_memcpy(buff, "20,", 3);
        reset_rom_field(ute, ROM_CLORDID, ute->positions[ROM_ORIG_ID].beg,
                        ute->positions[ROM_ORIG_ID].val_len);
        break;
    }
    clordid = 0;
    memcpy(&clordid, ute->data->buffer + ute->positions[ROM_CLORDID].beg,
           ute->positions[ROM_CLORDID].val_len);
    databuf_write(buff, "%u", clordid);
    *buff->wr_ptr++ = ',';
    *buff->wr_ptr++ = ',';
    databuf_memcpy(buff, ute->data->buffer + ute->positions[ROM_TAG].beg,
                   ute->positions[ROM_TAG].val_len);
    memcpy(rej_reason, ack + 21, 40);
    databuf_write(buff, ",,,,,,,,,%s,,,,,,,,,,,,,,,,,,,,,,,,", rej_reason);
    tt = ntohl(tt);
    set_date_str(buff, tt);
    databuf_memcpy(buff, ",,,,,,,,,,,,,,,,\n", 17);
    return rej_type;
}

int parse_filled(dart_order_obj * ute, databuf_t * buff, char *ack,
                 orderbook * ob)
{
    int tt = 0;
    int last_shares = 0;
    int last_price = 0;
    int ret_val = 0;
    char ps = '4';
    unsigned long ex_id = 0;
    memcpy(&ex_id, ack + 24, 8);
    ex_id = ntohll(ex_id);
    unsigned long ord_id = 0;
    memcpy(&ord_id, ack + 16, 8);
    ord_id = ntohll(ord_id);
    memcpy(&tt, (ack + 8), 4);
    memcpy(&last_shares, (ack + 32), 4);
    memcpy(&last_price, (ack + 36), 4);
    memcpy(&ps, ack + 40, 1);
    last_shares = ntohl(last_shares);
    double lp = ad_to_rom_price(last_price, ps);
    int l_cum = 0;
    int qty = 0;
    memcpy(&qty, ute->data->buffer + ute->positions[ROM_BIN_QTY].beg, 4);
    memcpy(&l_cum, ute->data->buffer + ute->positions[ROM_BIN_CUM].beg, 4);
    l_cum += last_shares;
    fill_in_rom_field(ute, ROM_BIN_CUM, &l_cum, 4);
    if (l_cum >= qty) {
        databuf_memcpy(buff, "2,", 2);
        unsigned int clordid = 0;
        memcpy(&clordid,
               ute->data->buffer + ute->positions[ROM_CLORDID].beg,
               ute->positions[ROM_CLORDID].val_len);
        pthread_mutex_lock(&ob->mutex);
        remove_from_map(ob->cl_ord_id_map, (char *) &clordid, 4);
        remove_from_map(ob->rom_ord_id_map,
                        ute->data->buffer + ute->positions[ROM_TAG].beg,
                        ute->positions[ROM_TAG].val_len);
        pthread_mutex_unlock(&ob->mutex);
        ret_val = 1;
    } else {
        databuf_memcpy(buff, "1,", 2);
    }
    unsigned int clordid = 0;
    memcpy(&clordid, ute->data->buffer + ute->positions[ROM_CLORDID].beg,
           ute->positions[ROM_CLORDID].val_len);
    databuf_write(buff, "%u,%lu,", clordid, ord_id);
    databuf_memcpy(buff, ute->data->buffer + ute->positions[ROM_TAG].beg,
                   ute->positions[ROM_TAG].val_len);
    databuf_write(buff, ",,,,,,");
    databuf_write(buff, ",,,,,,,,,,,,,,,,,,,%d,,%f,,,,,,", last_shares,
                  lp);
    tt = ntohl(tt);
    set_date_str(buff, tt);
    databuf_write(buff, ",,,,%lu", ex_id);
    databuf_memcpy(buff, ",,,,,,,,,,,,,,,", 15);
    *buff->wr_ptr++ = *(ack + 41);
    *buff->wr_ptr++ = ',';
    *buff->wr_ptr++ = '\n';
    return ret_val;
}

int parse_ex_filled(dart_order_obj * ute, databuf_t * buff, char *ack,
                    orderbook * ob)
{
    int ret_val = 0;
    return ret_val;
}

int ad_translate_rom_symbol(char *output, char *symbol, size_t sym_len)
{
    char *slash_off = 0;
    int ret_len = 0;
    char *off = output;
    slash_off = memchr(symbol, '/', sym_len);
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
