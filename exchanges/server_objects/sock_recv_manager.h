/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _SOCK_RECV_MANAGER_H__
#define _SOCK_RECV_MANAGER_H__

#if defined(__cplusplus)
extern "C" {
#endif

    /**
     * Yes yuck is the correct response to void stars, but I am just
     * passing these arguments off to a thread so by convention they
     * become void anyway.
     *
     * Anyway the hidden contract in this function is that you must
     * pass in an parse_args structure defined in client_manager.h.
     * enjoy.
     */
    void *manage_socket(void *parse_args);

    char *realloc_receive(unsigned long *total_size,
                             unsigned long *write_offset,
                             unsigned long *read_offset, char *wr_ptr,
                             char *rd_ptr, char *sock_reader);
#if defined(__cplusplus)
}
#endif
#endif                          //_SOCK_RECV_MANAGER_H__
