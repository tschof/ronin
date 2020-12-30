/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

#include "sock_recv_manager.h"
#include "rex_allocator.h"
#include "polling_server.h"

#define BUFFER_SIZE 65536
#define MIN_BUFF_SIZE 256

sock_mnger* create_sock_recv_manager()
{
    int ret = 0;
    sock_mnger* srm = 0;
    REX_ALLOC(srm, 8, sizeof(sock_mnger), ret);
    srm->buffer = 0;
    REX_ALLOC(srm->buffer, 8,BUFFER_SIZE + 1, ret);
    srm->wr_ptr = srm->buffer;
    srm->rd_ptr = srm->buffer;
    srm->end = srm->buffer + BUFFER_SIZE;
    return srm;
}

void destroy_sock_recv_manager(sock_mnger* srm)
{
    if(srm && srm->buffer) {
        free(srm->buffer);
        free(srm);
    } 
}



static void realloc_receive(rex_con* rc, sock_mnger *pa)
{
    int ret = 0;
    char *temp;
    long total = ((pa->end - pa->buffer) + BUFFER_SIZE);
    long write_offset = pa->wr_ptr - pa->rd_ptr;
    REX_ALLOC(temp, 8, total + 1, ret);
    if (temp == NULL) {
        return;
    }
    send_debug_message(rc->o, "Reallocing total: %ld , unread: %ld \n",
            total, write_offset);
    memcpy(temp, pa->rd_ptr, (pa->wr_ptr - pa->rd_ptr));
    free(pa->buffer);
    pa->buffer = 0;
    pa->buffer = temp;
    pa->end = pa->buffer + total;
    pa->wr_ptr = pa->buffer;
    pa->wr_ptr += write_offset;
    pa->rd_ptr = pa->buffer;
}
/**
 *
 */
void manage_socket(void* args, int* cut_connection)
{
    long num_bytes = 0;
    int run = 1;
    rex_con* apa = (rex_con*)args;
    sock_mnger* pa = apa->con;
    do {
        num_bytes =
            recv(apa->sock, pa->wr_ptr,
                    (pa->end - pa->wr_ptr), 0);
        if (num_bytes > 0) {
            pa->wr_ptr += num_bytes;
            apa->parse(cut_connection, apa);
            if (*cut_connection == 1 || apa->disconnect) {
                send_debug_message(apa->o, "Parser told us to Cut Connection!!");
                run = 0;
                *cut_connection = 1;
                break;
            }
            long longrd = pa->rd_ptr - pa->wr_ptr;
            if (pa->rd_ptr == pa->wr_ptr ||
                   longrd  > 0) {
                pa->wr_ptr = pa->buffer;
                pa->rd_ptr = pa->buffer;
                if(longrd > 0) {
                    send_debug_message(apa->o, "RD ptr was a head of the write pointer!!\n");
                } 
            } else {
                if(pa->end - pa->wr_ptr <= MIN_BUFF_SIZE) {
                    realloc_receive(apa, pa);
                    if (pa->buffer == NULL) {
                        send_debug_message
                            (apa->o, "Unable to allocate new buffer removing connection from tree: %d \n",
                             apa->sock);
                        *cut_connection = 1;
                        run = 0;
                        break;
                    }
                }
            } 
        } else if (num_bytes == -1) {
            int e = errno;
            if (e != EAGAIN) {
                run = 0;
                *cut_connection = 1;
                send_debug_message(apa->o, "Error on read: %d \n", e);
            }
            break;
        } else {
            run = 0;
            *cut_connection = 1;
            break;
        }
    } while (run);
}
