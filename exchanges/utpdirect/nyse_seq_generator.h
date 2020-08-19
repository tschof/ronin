/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _NYSE_SEQ_GENERATOR_H__
#define _NYSE_SEQ_GENERATOR_H__


#if defined(__cplusplus)
extern "C" {
#endif

    void start_seq_generator(const char *reserved_ids, int rid_len,
                             const char *name, int name_len, int seq,
                             int create_new, int end_hour, int end_min);

    char *next_order_id(size_t * len);


    int in_place_order_id(char *buff);

#if defined(__cplusplus)
}
#endif
#endif                          //_NYSE_SEQ_GENERATOR_H__
