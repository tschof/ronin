/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "hashmap.h"
#include "databuf.h"
#include "nyse_seq_generator.h"

#define NYSE_SEQ_LEN 4
#define NYSE_SEQ_BASE 26
#define NYSE_MAX_BRANCH_COMBO 18250
#define NYSE_MIN_BRANCH_COMBO 26
#define NYSE_MAX_SEQ 9999
#define NYSE_SEQ_SIZE 20
#define DART_EX_TAG_LEN 64


struct nyse_sequence_struct {
    hashmap_t reserved_id_map;
    char time_str[16];
    char seq_str[12];
    char branch_code[NYSE_SEQ_LEN];
    int sequence;
    int branch_code_seq;
    int cur_len;
};


static const char baseChars[26] = {
    'A', 'B', 'C', 'D', 'E', 'F',
    'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
    'W', 'X', 'Y', 'Z'
};

static void clean_reserved_id_map(struct nyse_sequence_struct* ci)
{
    char* rlf;
    char *key = 0;
    int key_len = 0;
    hashmap_iterator_t hi = hashmap_iterator_begin(ci->reserved_id_map);
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                                     (void *) &rlf);
    while (valid) {
        free(rlf);
        rlf = 0;
        valid = hashmap_iterator_next(ci->reserved_id_map, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &rlf);
    }
    hashmap_iterator_destroy(hi);
    delete_map(ci->reserved_id_map);
}

void destroy_seq_struct(struct nyse_sequence_struct* nss)
{
    clean_reserved_id_map(nss);
    free(nss);
}

static void res_id_breakdown(const char *res_ids, int rid_len,
        nyse_seq_struct* nyss)
{
    long len_left;
    long val_end;
    char *vend_ptr = NULL;
    char *d_off = NULL;
    int should_parse = 1;
    d_off = (char *) res_ids;
    val_end = 0;
    len_left = rid_len;
    while (should_parse && len_left > 0) {
        vend_ptr = strchr(d_off, ',');
        if (vend_ptr != NULL) {
            val_end = vend_ptr - d_off;
            if (insert_map
                    (nyss->reserved_id_map, d_off, val_end, d_off, val_end)) {
            } else {
                printf("Insertion failed in res_id_breakdown.\n");
            }
            d_off += val_end + 1;
        } else {
            should_parse = 0;
        }
    }
}

static int is_reserved_branch_code(nyse_seq_struct* nyse_seq, const char *b, int len)
{
    return contains_key(nyse_seq->reserved_id_map, b, len);
}

static void reverse_bc(nyse_seq_struct* nyse_seq)
{
    char first;
    char third;
    if (nyse_seq->branch_code[2] != '\0') {
        first = nyse_seq->branch_code[0];
        third = nyse_seq->branch_code[2];
        nyse_seq->branch_code[0] = third;
        nyse_seq->branch_code[2] = first;
    } else {
        third = nyse_seq->branch_code[1];
        first = nyse_seq->branch_code[0];
        nyse_seq->branch_code[0] = third;
        nyse_seq->branch_code[1] = first;
    }
}

static void set_branch_code(nyse_seq_struct* nyse_seq)
{
    int remainder = 0;
    int pos = 0;
    int value = nyse_seq->branch_code_seq;
    memset(nyse_seq->branch_code, '\0', 4);
    while (value > 0 && pos < 3) {
        remainder = value % NYSE_SEQ_BASE;
        value = ((value - remainder) / NYSE_SEQ_BASE);
        nyse_seq->branch_code[pos] = baseChars[remainder];
        ++pos;
    }
    nyse_seq->cur_len = pos;
    reverse_bc(nyse_seq);
}

static void increment_branch_code(nyse_seq_struct* nyse_seq)
{
    ++nyse_seq->branch_code_seq;
    if (nyse_seq->branch_code_seq > NYSE_MAX_BRANCH_COMBO ||
            nyse_seq->branch_code_seq < NYSE_MIN_BRANCH_COMBO) {
        nyse_seq->branch_code_seq = NYSE_MIN_BRANCH_COMBO;
    }
    set_branch_code(nyse_seq);
}

nyse_seq_struct* start_seq_generator(const char *reserved_ids, int rid_len,
        const char *name, int name_len, int seq)
{
    nyse_seq_struct* nyss = calloc(1, sizeof(struct nyse_sequence_struct));
    time_t rawtime;
    struct tm *ti;
    int year;
    int month;
    time(&rawtime);
    nyss->branch_code_seq = seq < NYSE_MIN_BRANCH_COMBO ? NYSE_MIN_BRANCH_COMBO : seq;
    struct tm lm;
    ti = gmtime_r(&rawtime, &lm);
    ti = &lm;
    year = 1900 + ti->tm_year;
    month = 1 + ti->tm_mon;
    memset(nyss->time_str, '\0', 9);
    memset(nyss->seq_str, '\0', 9);
    sprintf(nyss->time_str, "%02d%02d%04d", month, ti->tm_mday, year);
    nyss->reserved_id_map = create_map(64);
    res_id_breakdown(reserved_ids, rid_len, nyss);
    nyss->sequence = 1;
    memset(nyss->branch_code, '\0', NYSE_SEQ_LEN);
    set_branch_code(nyss);
    return nyss;
}
static void nyse_unsigned_itoa(nyse_seq_struct* nyse_seq, char* out, unsigned long value)
{
    int radix = 10;
    static char dig[] = "0123456789";
    long n = 0;
    unsigned long v = 0;
    char *p;
    v = value;
    do {
        nyse_seq->seq_str[n++] = dig[v % radix];
        v /= radix;
    } while (v);
    p = out + (4 -n);
    do {
        *p++ = nyse_seq->seq_str[n - 1];
        --n;
    } while(n);
}

int in_place_order_id(nyse_seq_struct* nyse_seq, char *buff)
{
    if (nyse_seq->sequence > NYSE_MAX_SEQ) {
        nyse_seq->sequence = 1;
        increment_branch_code(nyse_seq);
        while (is_reserved_branch_code(nyse_seq, nyse_seq->branch_code, 3)) {
            increment_branch_code(nyse_seq);
        }
    }
    memcpy(buff, nyse_seq->branch_code, nyse_seq->cur_len);
    buff += nyse_seq->cur_len;
    *buff++ = ' ';
    memcpy(buff, "0000", 4);
    nyse_unsigned_itoa(nyse_seq, buff, nyse_seq->sequence);
    buff += 4;
    *buff++ = '/';
    memcpy(buff, nyse_seq->time_str,8);
    ++nyse_seq->sequence;
    return (14 + nyse_seq->cur_len);
}
