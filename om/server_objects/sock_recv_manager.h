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


typedef struct sock_recv_manager {
    char *buffer;
    char *wr_ptr;
    char *rd_ptr;
    char *end;
} sock_recv_manager;

sock_recv_manager* create_sock_recv_manager();

void destroy_sock_recv_manager(sock_recv_manager* srm);

/**
 * 
 */
void manage_socket(void* pa, int sock, int* cut_connection);

#if defined(__cplusplus)
}
#endif
#endif
