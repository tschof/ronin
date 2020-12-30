/**************************************************************************
 * Copyright (c) 2015 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "suffix_map.h"
#include "hashmap.h"
#include "dart_allocator.h"

/**
 *
 */
struct symbol_suffix_mapper {
    hashmap_t suffixs;
    hashmap_t gsflipper;
};


static char* get_file_contents(const char* filename, long* size)
{
    FILE* mfile = 0;
    long res = 0;
    char* buff = 0;
    int ret = 0;
    mfile = fopen(filename, "r");
    if(mfile != 0) {
        fseek(mfile, 0, SEEK_END);
        *size = ftell(mfile);
        rewind(mfile);
        DART_ALLOC(buff, 8, *size + 1, ret);
        res = fread(buff, 1, *size, mfile);
        if(res != *size) {
            free(buff);
            buff = 0;
        }
        fclose(mfile);
    } 
    return buff;
}

static void build_map_from_file(const char* filename, hashmap_t map)
{
    char* buffer = 0;
    long size = 0;
    buffer = get_file_contents(filename, &size);
    if(size > 0) {
        char* start = buffer;
        char* comma = buffer;
        char* end = buffer;
        int tag_len = 0;
        char* val = buffer;
        do {
            if(*end == ',') {
                tag_len = end - start;
                comma = end;
            } else if(*end == '\n') {
                val = (comma + 1);
                insert_map(map, start, tag_len, val, end - val);
                start = end + 1;
            }
            ++end;
            --size;
        } while(size > 0);
    }
}

suffixm* create_suffix_map(char* filename) 
{
    suffixm* b = calloc(1, sizeof(struct symbol_suffix_mapper));
    b->suffixs = create_map(512);
    b->gsflipper = create_map(512);
    build_map_from_file(filename, b->suffixs);
    return b;
}
suffixm* create_suffix_n_flip(char* filename, char* flip_file) 
{
    suffixm* b = calloc(1, sizeof(struct symbol_suffix_mapper));
    b->suffixs = create_map(512);
    b->gsflipper = create_map(512);
    build_map_from_file(filename, b->suffixs);
    build_map_from_file(flip_file, b->gsflipper);
    return b;
}

const char* get_suffix(suffixm* smap, char* rom_suffix, int rlen,
                       int* slen)
{
    char* suffix = 0;
    int len = find_n_fill(smap->suffixs, rom_suffix, rlen, (void**)&suffix);
    if(len > 0) {
        *slen = len;
        return suffix;
    } else {
        *slen = rlen;
        return rom_suffix;
    } 
}

char* get_flip_account(suffixm* smap, char* account, int alen, int *slen)
{
    char* flipacc = 0;
    int len = find_n_fill(smap->gsflipper, account, alen, (void**)&flipacc);
    if(len > 0) {
        *slen = len;
        return flipacc;
    } else {
        *slen = alen;
        return account;
    } 
}
