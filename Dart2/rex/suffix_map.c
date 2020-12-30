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
#include "rex_allocator.h"

/**
 *
 */
struct symbol_suffix_mapper {
    hashmap_t suffixs;
};


static char* suf_get_file_contents(const char* filename, long* size)
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
        REX_ALLOC(buff, 8, *size + 1, ret);
        res = fread(buff, 1, *size, mfile);
        if(res != *size) {
            free(buff);
            buff = 0;
        }
        fclose(mfile);
    } 
    return buff;
}

static void suf_build_map_from_file(const char* filename, hashmap_t map)
{
    char* buffer = 0;
    long size = 0;
    buffer = suf_get_file_contents(filename, &size);
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
    suf_build_map_from_file(filename, b->suffixs);
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
