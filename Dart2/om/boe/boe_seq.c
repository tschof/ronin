/**************************************************************************
 * Copyright (c) 2015 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "boe_seq.h"
#include "databuf.h"

/**
 * Because BATS uses an 8 byte value for unit numbers  
 * BATS will not have more than 256 matching engines
 * and will not use numbers higher than 256 to identify 
 * those matching engines.
 */
struct boe_seq_master {
    int match_engines[256];
    unsigned char num_units;
};

bsm* create_seq_master()
{
    bsm* b = calloc(1, sizeof(struct boe_seq_master));
    return b;
}


unsigned long parse_boe_outgoing (char *data, unsigned long size,
                                  unsigned long bytes_used,
                                  unsigned long *seq_num,
                                  void* book)
{
    unsigned long ret_bytes = 0;
    unsigned short mess_len = 0;
    bsm* b = (bsm*)book;
    char* cur_off = data;
    memcpy(&mess_len, cur_off, 2);
    do {
        if(mess_len == 0xBABA) {
            memcpy(&mess_len, cur_off + 2, 2);
            unsigned char mu = 0x0;
            unsigned int seq = 0x0;
            memcpy(&mu, cur_off + 5, 1);
            if(mu > 0) {
                memcpy(&seq, cur_off + 6, 4);
                if(b->match_engines[mu] <= 0) {
                    b->num_units++;
                }
                b->match_engines[mu] = seq;
            }
            cur_off += mess_len + 2;
            ret_bytes += mess_len + 2;
        }
        if(ret_bytes < (size - 2)) {
            memcpy(&mess_len, cur_off, 2);
        }
    } while(mess_len == 0xBABA);
    return ret_bytes;
}

int add_unit_seq_param_group(databuf_t* out_message, bsm* master) 
{
    if(master->num_units == 0) {
        return 0;
    }
    unsigned char type = 0x80;
    unsigned char nuu = 0x0;
    short param_len = (master->num_units * 5) + 5;
    databuf_memcpy(out_message, &param_len, 2);
    databuf_memcpy(out_message, &type, 1);
    databuf_memcpy(out_message, &nuu, 1);
    databuf_memcpy(out_message, &master->num_units, 1);
    unsigned char i = 0x0;
    for(;i < 255; ++i) {
        if(master->match_engines[i] > 0) {
            char val = i;
            databuf_memcpy(out_message, &val, 1);
            databuf_memcpy(out_message,
                           &master->match_engines[i], 4);
        }
    }
    return 1;
}

