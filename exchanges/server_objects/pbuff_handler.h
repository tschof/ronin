/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _PBUFF_HANDLER_H__
#define _PBUFF_HANDLER_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "message_queue.h"
#include "client_manager.h"

    unsigned long build_pbuff_offsets(queue_t, char *data,
                                      unsigned long size,
                                      unsigned long byte_offset,
                                      unsigned long *seq_num);
    unsigned long parse_pbuff_message(const char *message,
                                      unsigned long len,
                                      int *cut_con, struct parse_con *pc);


#if defined(__cplusplus)
}
#endif
#endif                          //PBUFF_HANDLER_H__
