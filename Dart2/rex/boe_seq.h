/**************************************************************************
 * Copyright (c) 2015 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _DART_BOE_SEQ_H__
#define _DART_BOE_SEQ_H__

#include <stdint.h>
#include "databuf.h"
#include "log_mnger.h"

typedef struct boe_seq_master bsm;

bsm* create_seq_master(otl* o, char* sender, uint16_t slen, int create_new);
int add_unit_seq_param_group(databuf_t* out_message,bsm* master); 
void add_seq(bsm* master, uint8_t munit, uint32_t seq);

#endif //_DART_BOE_SEQ_H__
