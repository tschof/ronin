/**************************************************************************
 * Copyright (c) 2015 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _DART_BOE_SEQ_H__
#define _DART_BOE_SEQ_H__

#include "databuf.h"

typedef struct boe_seq_master bsm;

bsm* create_seq_master();


unsigned long parse_boe_outgoing (char *data, unsigned long size,
                                  unsigned long bytes_used,
                                  unsigned long *seq_num,
                                  void* book);

int add_unit_seq_param_group(databuf_t* out_message,bsm* master); 

#endif //_DART_BOE_SEQ_H__
