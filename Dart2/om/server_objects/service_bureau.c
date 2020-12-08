/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "service_bureau.h"
#include "dart_allocator.h"
#include "hashmap.h"
#include "config_loader.h"
#include "parser_func.h"

struct service_bureau_struct {
    char* filename;
    hashmap_t id_to_mnemonic;
};

static void gen_add_to_map(hashmap_t map, char *buffer, long len)
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
    } else {
        printf("Could not find a comma in this mess: %s \n", buffer);
    }
}

static int gen_build_clear_id_map(const char *filename, hashmap_t map)
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
                gen_add_to_map(map, d_off, end_pos);
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
sbureau_match* create_service_bureau_matcher(char* filename)
{
    sbureau_match* sbm = 0;
    int ret = 0;
    DART_ALLOC(sbm,8, sizeof(struct service_bureau_struct), ret);
    sbm->id_to_mnemonic = create_map(8);
    ret = gen_build_clear_id_map(filename, sbm->id_to_mnemonic);
    return sbm;
}

char* get_mpid_for_clr_acct(sbureau_match* sbm, char* clr_acct, int cl_acct_len, int* mpid_len)
{
        char* rom_val= 0;
        *mpid_len =
            find_n_fill(sbm->id_to_mnemonic, clr_acct, cl_acct_len, (void *) &rom_val);
        return rom_val;
}
