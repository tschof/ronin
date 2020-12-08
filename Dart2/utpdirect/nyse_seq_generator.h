/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _NYSE_SEQ_GENERATOR_H__
#define _NYSE_SEQ_GENERATOR_H__
typedef struct nyse_sequence_struct nyse_seq_struct;
nyse_seq_struct* start_seq_generator(const char *reserved_ids, int rid_len,
                                     const char *name, int name_len, int seq);
int in_place_order_id(nyse_seq_struct* nss, char *buff);

void destroy_seq_struct(struct nyse_sequence_struct* nss);
#endif
