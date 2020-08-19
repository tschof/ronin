/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _DART_SEQ_REQUEST_H__
#define _DART_SEQ_REQUEST_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "common_structs.h"
    typedef struct dart_seq_request sequence_req;

    sequence_req *create_sequence_request(const char *name,
                                          size_t name_len,
                                          struct init_args* in, unsigned
                                          long (*p_messages) (char *data,
                                                              unsigned long
                                                              size,
                                                              unsigned long
                                                              bytes_used,
                                                              unsigned long
                                                              *seq_num),
                                          char *(*create_pad) (unsigned
                                                               long seq,
                                                               char
                                                               *fix_ver,
                                                               size_t *
                                                               len));

    void destroy_sequence_req(sequence_req * sr);

    unsigned long dsr_get_incoming(sequence_req * sr);

    unsigned long dsr_get_outgoing(sequence_req * sr);

    void dsr_set_incoming(sequence_req * sr, unsigned long incoming);

    void dsr_set_outgoing(sequence_req * sr, unsigned long outgoing);

    void dsr_lock_con_mutex(sequence_req * sr);

    void dsr_wait_on_cond(sequence_req * rs);

    void dsr_signal_cond(sequence_req * sr);

    unsigned long dsr_get_new_incoming(sequence_req * sr);

    unsigned long dsr_get_new_outgoing(sequence_req * sr);

    const char *dsr_get_name(sequence_req * sr, size_t * name_len);

    int dsr_get_create_new(sequence_req * sr);

    unsigned long dsr_parse_data(sequence_req * sr, char *data,
                                 unsigned long size,
                                 unsigned long bytes_used,
                                 unsigned long *seq_num);

    char *dsr_create_pad(sequence_req * sr, unsigned long seq,
                         size_t * len);
    int ll_before_reset_time(struct init_args* in, time_t rawtime);

#if defined(__cplusplus)
}
#endif
#endif                          //_DART_SEQ_REQUEST_H__
