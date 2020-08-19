/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/


#ifndef _CBOE_SEQ_GENERATOR_H__
#define _CBOE_SEQ_GENERATOR_H__
#include "common_structs.h"

#if defined(__cplusplus)
extern "C" {
#endif

    void start_seq_generator(const char *reserved_ids, int rid_len,
                             const char *name, int name_len, int seq,
                             struct init_args* in, int end_hour, int end_min);

    //int next_order_id(char *buffer, int len);
    char *next_order_id(size_t * len);

    void set_time_str(struct tm* ti);

#if defined(__cplusplus)
}
#endif
#endif                          //_CBOE_SEQ_GENERATOR_H__
