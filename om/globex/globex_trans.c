/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>

#include "hashmap.h"
#include "globex_trans.h"
#include "config_loader.h"
#include "parser_func.h"

static int m_codes[26] = {
    0, 0, 0, 0, 0,
    1, 1, 1, 0, 1,
    1, 0, 1, 1, 0,
    0, 1, 0, 0, 0,
    1, 1, 0, 1, 0, 1
};
struct globex_trans {
    hashmap_t disp_facts;
    hashmap_t clr_ids;
    hashmap_t sender_sub;
    pthread_spinlock_t mutex;
    char* d_factors;
    char* clear_ids;
    char* s_sub;
    char* cust_or_firm;
    char* cust_type_code;
    int cof_len;
    int cti_len;
};
size_t get_symbol_len(char *sym, size_t s_len)
{
    size_t len = s_len;
    char *d_off = sym;
    d_off += (s_len - 1);
    int i = 0;
    for (; i < s_len; ++i) {
        if (sym[i] == 0x20 || sym[i] == 0x2D) {
            d_off = ((sym + i) - 1);
            len = i;
            break;
        }
    }
    i = 0;
    if (*d_off >= 0x30 && *d_off <= 0x39) {
        for (; i < len; ++i, --d_off) {
            if (*d_off < 0x30 || *d_off > 0x39) {
                if (*d_off > 0x5A || *d_off < 0x41
                    || !(m_codes[(*d_off) - 65])) {
                    return s_len;
                }
                len = d_off - sym;
                break;
            }
        }
    }
    return len;
}
static void globex_add_to_map(hashmap_t map, char *buffer, long len)
{
    char *off;
    long c_offset = 0;
    off = strchr(buffer, ',');
    if (off) {
        char *tag;
        char* val;
        char *val_off;
        c_offset = off - buffer;
        val = calloc(10,1);
        tag = calloc(c_offset + 1, 1);
        memcpy(tag, buffer, c_offset);

        val_off = buffer;
        val_off += c_offset + 1;
        int val_len = len - (c_offset + 1);
        memcpy(val, val_off, val_len); 
        insert_map(map, tag, c_offset, val, val_len);
        free(val);
        free(tag);
    }
}

static int build_globex_map(const char *filename, hashmap_t map)
{
    char *buffer;
    long size;
    size = 0;
    buffer = NULL;
    buffer = get_file_contents(filename, &size);
    if (size > 0) {
        long cur_pos = 0;
        char eol[2] = { '\n', '\0' };
        long end_pos = 0;
        int should_parse = 1;
        char *d_off = buffer;
        while (cur_pos < size && should_parse) {
            end_pos = find_offset(d_off, (size - cur_pos), eol, 1);
            if (end_pos > 0) {
                globex_add_to_map(map, d_off, end_pos);
                d_off += end_pos + 1;
                cur_pos += end_pos + 1;
            } else {
                should_parse = 0;
            }
        }
        free(buffer);
    } else {
        return 1;
    }
    return 0;
}
static void build_display_map_from_file(const char* filename, hashmap_t map)
{
    char* buffer = 0;
    long size = 0;
    buffer = get_file_contents(filename, &size);
    if(size > 0) {
        char* start = buffer;
        char* comma = buffer;
        char* end = buffer;
        int tag_len = 0;
        int val = 0;
        do {
            if(*end == ',') {
                tag_len = end - start;
                comma = end;
            } else if(*end == '\n') {
                val = atoi(comma + 1);
                insert_map(map, start, tag_len, &val, sizeof(int));
                start = end + 1;
            }
            ++end;
            --size;
        } while(size > 0);
    }
}
g_trans_t* create_globex_trans(char* directory, int d_len, ex_config* ex)
{
    g_trans_t* t = calloc(1, sizeof(struct globex_trans));
    if (pthread_spin_init(&t->mutex, PTHREAD_PROCESS_PRIVATE) != 0) {
        printf("Failed to init globex_display factor mutex.\n");
    }
    t->disp_facts = create_map(256);
    t->clr_ids = create_map(16);
    t->sender_sub = create_map(64);
    t->d_factors = calloc(1, d_len + 20);
    memcpy(t->d_factors, directory, d_len);
    memcpy(t->d_factors + d_len, "GlobexPriceMult.map", 19);
    t->clear_ids = calloc(1, d_len + 25);
    memcpy(t->clear_ids, directory, d_len);
    memcpy(t->clear_ids + d_len, "GlobexClearingIDs.map", 21);
    t->s_sub = calloc(1, d_len + 20);
    build_display_map_from_file(t->d_factors, t->disp_facts);
    build_globex_map(t->clear_ids, t->clr_ids);
    build_globex_map("GlobexSenderSubs.map", t->sender_sub);
    t->cof_len = 0;
    t->cust_or_firm = get_val_for_tag(ex->dc, "FIX SERVER", 10,
            "Customer or Firm", 16, &t->cof_len);
    t->cti_len = 0;
    t->cust_type_code = get_val_for_tag(ex->dc, "FIX SERVER", 10,
            "Customer Type Code", 18, &t->cti_len);
    return t;
}
/**
 * Returns the value that we use to multiply the internal
 * price to match the cme price.
 */
int get_display_factor(g_trans_t* t, char* product_code, int pc_len)
{
    pthread_spin_lock(&t->mutex);
    int* val = 0;
    int ret = get_symbol_len(product_code, pc_len);
    ret = get_map_entry(t->disp_facts, product_code,
                               ret, (void*)&val); 
    if(ret) {
        ret = *val;
    } else {
        ret = 100;
    }
    pthread_spin_unlock(&t->mutex);
    return ret;
}
void reload_display_factors(g_trans_t* t)
{
    pthread_spin_lock(&t->mutex);
    delete_map(t->disp_facts);
    t->disp_facts = create_map(256);
    build_display_map_from_file(t->d_factors, t->disp_facts);
    pthread_spin_unlock(&t->mutex);
}

char* get_sender_location(g_trans_t* t, char* trader, int tlen)
{
    char* loc = 0;
    int ret = get_map_entry(t->sender_sub, trader, tlen, (void**)&loc);
    if(ret <= 0) {
        loc = 0x0;
    }
    return loc;
}

char* get_cust_type(g_trans_t* t, int* len)
{
    *len = t->cti_len;
    return t->cust_type_code;
}

char* get_cust_or_firm(g_trans_t* t, int* len)
{
    *len = t->cof_len;
    return t->cust_or_firm;
}
/**
 * We need to validate the clearing id since we need to 
 * embed this in FIX tag 49.  If we have an incorrect value
 * the session will be terminated and we will have to re-logon.
 */
int valid_cl_ord_id(g_trans_t* t,char* clear_id, int len)
{
    return  contains_key(t->clr_ids, clear_id, len);
}

