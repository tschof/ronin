/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>


#include "boe_parser.h"
#include "suffix_map.h"


#define DAY_SEC 86400
#define HOUR_SEC 3600
#define MIN_SEC 60

struct boe_time_keeper {
    time_t prev_midnight_sec;
    char dart_date_str[9];
    suffixm* suffix_map;
    suffixm* clr_accs;
};

static void set_time_str(btk* b, struct tm* ti)
{
    int year = ti->tm_year + 1900;
    int month = 1 + ti->tm_mon;
    sprintf(b->dart_date_str, "%04d%02d%02d", year, month,
                ti->tm_mday);
}

static void set_boe_struct(btk* b)
{
    time_t rawtime;
    struct tm *ti;
    struct tm lm;
    time(&rawtime);
    ti = gmtime_r(&rawtime, &lm);
    ti = &lm;
    set_time_str(b, ti);
    b->prev_midnight_sec = ti->tm_hour * HOUR_SEC;
    b->prev_midnight_sec += (ti->tm_min * MIN_SEC);
    b->prev_midnight_sec += ti->tm_sec;
    b->prev_midnight_sec = rawtime - b->prev_midnight_sec;
}

btk* create_boe_time_keeper(char* filename)
{
    btk* b = calloc(1, sizeof(struct boe_time_keeper));
    set_boe_struct(b);
    b->suffix_map = create_suffix_map(filename);
    b->clr_accs = create_suffix_map("GSFlip.map");
    return b;
}
const char* boe_get_suffix(btk* b, char* rom_suffix, int rlen,
                       int* slen)
{
   return get_suffix(b->suffix_map, rom_suffix, rlen, slen);
}
const char* boe_get_clr_account(btk* b, char* clr_acc, int clen,
        int* slen)
{
   return get_suffix(b->clr_accs, clr_acc, clen, slen);
}
static int boe_unsigned_itoa(char* str, unsigned long value)
{
    int radix = 10;
    static char dig[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    long n = 0;
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
    return n;
}
const char *boe_digit_str[] =
    { "00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
    "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
    "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
    "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
    "50", "51", "52", "53", "54", "55", "56", "57", "58", "59"
};
int boe_to_rom_time(btk* b, long boe_time, char* buff)
{
    long epoch_time = boe_time/1000000000;
    long nanos = boe_time % epoch_time;
    nanos /= 1000; //We are only going to display micros.
    long secs = (epoch_time - b->prev_midnight_sec);
    char* off = buff;
    if(secs < 86400) {
        memcpy(off, b->dart_date_str, 8);
    } else {
        set_boe_struct(b);
        secs = (epoch_time - b->prev_midnight_sec);
        memcpy(off, b->dart_date_str, 8);
    }
    off += 8;
    memcpy(off, "-", 1);
    off += 1;
    long hours = secs / HOUR_SEC;
    int mins = (secs % 3600) / 60;
    int sec = (secs % 3600) % 60;
    memcpy(off, boe_digit_str[hours], 2);
    off += 2;
    memcpy(off, ":", 1);
    off += 1;
    memcpy(off, boe_digit_str[mins], 2);
    off += 2;
    memcpy(off, ":", 1);
    off += 1;
    memcpy(off, boe_digit_str[sec], 2);
    off += 2;
    memcpy(off, ".", 1);
    off += 1;
    int len = boe_unsigned_itoa(off, nanos);
    off += len;
    if(len < 6) {
        int i = len;
        for(;i<6;++i) {
            *off++ = '0';
            ++len;
        }
    }
    return 18 + len;
}


