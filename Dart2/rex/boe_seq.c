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
#include "rex_allocator.h"
#include "log_mnger.h"
#include "rex_resend_request.h"


#define NUM_UNITS 255
/**
 * Because BATS uses an 8 byte value for unit numbers  
 * BATS will not have more than 256 matching engines
 * and will not use numbers higher than 256 to identify 
 * those matching engines.
 */
struct boe_seq_master {
    char* name;
    uint16_t nlen;
    uint32_t match_engines[NUM_UNITS];
    uint8_t num_units;
    uint16_t off;
    int8_t create_new;
    uint16_t full;
    otl* o;
};

bsm* create_seq_master(otl* o, char* name, uint16_t nlen, int create_new)
{
    int ret = 0;
    bsm* b = 0;
    REX_ALLOC(b,8,sizeof(struct boe_seq_master), ret);
    b->o = o;
    b->name = calloc(1, nlen +1);
    memcpy(b->name, name, nlen);
    b->nlen = nlen;
    b->create_new = create_new;
    b->full = sizeof(uint32_t) * NUM_UNITS;
    if(create_new) {
        reset_seq(o, b->name, b->nlen);
        rewind_raw_outbound(b->o, b->name,
                b->nlen, (char*)b->match_engines,sizeof(uint32_t) * NUM_UNITS);
    }
    return b;
}


static int funky_chunky_monkey(char *data, int len, void* cp)
{
    bsm* b = (bsm*)cp;
    memcpy(&b->match_engines + (b->off), 
            data, len);
    if(len < b->full) {
        b->off += len;
        return len;
    } else {
        return 0;
    }
}

void add_seq(bsm* master, uint8_t munit, uint32_t seq)
{
    master->create_new = 0;
    master->match_engines[munit] = seq;
    rewind_raw_outbound(master->o, master->name, master->nlen,
            (char*)master->match_engines,master->full);
}

int add_unit_seq_param_group(databuf_t* out_message, bsm* master) 
{
    if(master->create_new == 0) {
        resend_req *rr = create_chunky_resend(master->name,
                                     master->nlen, 
                                     master->full,
                                     master,funky_chunky_monkey);
        get_resend_request(rr, master->o);
        destroy_resend_req(rr);
        master->create_new = 0;
    }
    uint8_t i = 0x0;
    for(;i < NUM_UNITS; ++i) {
       if(master->match_engines[i] > 0) {
           master->num_units++;
            send_debug_message(master->o, "Num units with sequences:%d,  unit: %u, %u\n",
                        master->num_units, i, master->match_engines[i]);
       } 
    }
    if(master->num_units > 0) {
        unsigned char type = 0x80;
        unsigned char nuu = 0x0;
        short param_len = (master->num_units * 5) + 5;
        databuf_memcpy(out_message, &param_len, 2);
        databuf_memcpy(out_message, &type, 1);
        databuf_memcpy(out_message, &nuu, 1);
        databuf_memcpy(out_message, &master->num_units, 1);
        i = 0x0;
        for(;i < NUM_UNITS; ++i) {
            if(master->match_engines[i] > 0) {
                uint8_t val = i;
                databuf_memcpy(out_message, &val, 1);
                databuf_memcpy(out_message,
                               &master->match_engines[i], 4);
            }
        }
        return 1;
    } else {
        return 0;
    }
}

