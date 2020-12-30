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


#include "order_token_generator.h"
#include "dart_allocator.h"

#define MICRO_HOUR 3600000000
#define MICRO_MIN 60000000
#define MICRO_SEC 1000000

token_gen* generic_creation_func()
{
    return token_creation_func("20", 2);
}
token_gen* token_creation_func(char* t, int len)
{
    token_gen* tg = 0;
    int ret_val = 0;
    DART_ALLOC(tg, 8, sizeof(token_gen), ret_val); 
    struct timeval tv;
    ret_val = gettimeofday(&tv, 0);
    struct tm tmparts;
    localtime_r(&tv.tv_sec, &tmparts);
    tg->offset = (tmparts.tm_hour * MICRO_HOUR) +
                 (tmparts.tm_min * MICRO_MIN) +
                 (tmparts.tm_sec * MICRO_SEC) + tv.tv_usec;
    tg->counter = 1;
    ret_val = snprintf(tg->datestr,9, "%04d%02d%02d", 
                       tmparts.tm_year + 1900, tmparts.tm_mon + 1,
                       tmparts.tm_mday);
    tg->create_in_place_token = create_in_place_ord_token;
    if(len < 3) {
        const char* padding = "0000";
        sprintf(tg->key, "%s%*.*s", t, 3, 3,padding);
    } else {
        memcpy(tg->key, t, 3);
    }
    return tg;  
}

static unsigned int base_36_ltoa (unsigned int value, char* str )
{
    int radix = 36;
    static char dig[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
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
    return n;
}

int create_in_place_ord_token(token_gen* tg, char* off, int max_len)
{
    unsigned int next = __sync_add_and_fetch(&tg->counter, 1);
    return base_36_ltoa(tg->offset + next,off);
}

int create_multiday_in_place_ord_token(token_gen* tg, char* off, int max_len)
{
    int len = 0;
    if(max_len > 10) {
        memcpy(off, tg->datestr, 8);
        char* o2 = off + 8;
        len += 8;
        len += create_in_place_ord_token(tg, o2, max_len -8);
    }
    return len;
}

static unsigned int base_10_ltoa_with_max(unsigned int value, char* str, int max)
{
    int radix = 10;
    static char dig[] = "0123456789";
    unsigned int n = 0;
    unsigned long v = 0;
    char *p, *q;
    char c;
    v = value;
    do {
        str[n++] = dig[v % radix];
        v /= radix;
    } while (v);
    if(n > max) {
        n = max;
    }
    for (p = str, q = p + (n - 1); p < q; ++p, --q) {
        c = *p;
        *p = *q;
        *q = c;
    }
    return n;
}
int create_multiday_all_num_token(token_gen* tg, int seq, char* off, int max_len, int min_len)
{
    if(max_len < 7) {
        return 0;
    }
    int len = 0;
    char* o2;
    if(max_len <= 12) {
        memcpy(off, tg->key, 3);
        memcpy(off + 3, tg->datestr + 4, 4);
        o2 = off + 7;
        len += 7;
    } else {
        memcpy(off, tg->datestr, 8);
        o2 = off + 8;
        len += 8;
    }
    len += base_10_ltoa_with_max(seq, o2, max_len - 7);
    if(len < min_len) {
        ++o2;
        do {
            *o2++ = '0';
            ++len;
        } while(len < min_len);
    }
    return len;
}
