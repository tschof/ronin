/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _SERVER_SOCKET_H__
#define _SERVER_SOCKET_H__

#if defined(__cplusplus)
extern "C" {
#endif

    /**
     * Yes yuck is the correct response to void stars, but I am just
     * passing these arguments off to a thread so by convention they
     * become void anyway.
     *
     * Anyway the hidden contract in this function is that you must
     * pass in an server_con_set structure defined in
     * client_manager.h.  enjoy.
     */
    int start_server_socket(void *server_con_set);

    int start_ud_binder(void *server_con_set);

    /**
     * This will set some of the regular option that we always want on
     * our sockets, such as TCP_NODELAY and setting a RCVTIMEO and
     * SENDTIMO
     */
    int set_sock_opt(int sock);

#if defined(__cplusplus)
}
#endif
#endif                          //SERVER_SOCKET_H__
