/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _CX_OUCH_HELPER__
#define _CX_OUCH_HELPER__

#if defined(__cplusplus)
extern "C" {
#endif
#include "client_manager.h"
/*
 * Since there is no way to resend messages with the Currenex Ouch platform
 * These functions are defined with empty implementations.
 */

    unsigned long build_cx_offsets(char *data,
                                   unsigned long size,
                                   unsigned long byte_offset,
                                   unsigned long *seq_num);


    char *create_cx_resend_pad(unsigned long seq, char *rom, size_t * len);


    size_t find_cx_offsets(char *data, size_t * seq,
                           size_t size, size_t byte_offset,
                           size_t beg_seq, size_t end_seq,
                           size_t * beg_off, size_t * end_off);

    void to_cx_func(struct message *mess, void *b);
    struct init_args *load_cx_config(const char *filename, int len);
    void cx_send_message_to_roms(struct message *mess);

#if defined(__cplusplus)
}
#endif
#endif                          //_MAILER_H__
