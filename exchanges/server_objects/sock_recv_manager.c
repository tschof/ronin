/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
#include "sock_recv_manager.h"
#include "dart_connection_object.h"
#include "client_manager.h"

/**
 * Reallocates the receive buffer.  The unsigned long offsets are
 * passed as pointers to preserve their values after the call.
 */
char *realloc_receive(unsigned long *total_size,
                             unsigned long *write_offset,
                             unsigned long *read_offset, char *wr_ptr,
                             char *rd_ptr, char *sock_reader)
{
    char *temp;
    *total_size += SOCK_READ_SIZE;
    temp = calloc(*total_size + 1, 1);
    if (temp == NULL) {
        return NULL;
    }
    memcpy(temp, rd_ptr, (*write_offset - *read_offset));
    free(sock_reader);
    sock_reader = 0;
    *write_offset = (*write_offset - *read_offset);
    *read_offset = 0;
    return temp;
}

/**
 * Thread callback function when a new client connection is accepted.
 * only the low level receives are handled in this file all parsing is
 * done in the function pointer "parse"'s file.
 */
void *manage_socket(void *p_args)
{
    struct parse_args *pa = (struct parse_args *) p_args;
    time_t long_time;
    struct tm *newtime;
    char *sock_reader = NULL;
    char *wr_ptr = NULL;
    char *rd_ptr = NULL;
    unsigned long write_offset;
    unsigned long read_offset;
    unsigned long total_size;
    long num_bytes;
    int run;
    int cut_connection;
    int err;
    int num_timeouts;
    int sock = get_sock(pa->con->co);
    //Check to see if we should end. 
    time(&long_time);
    struct tm llin_newtime;
    newtime = localtime_r(&long_time, &llin_newtime);
    newtime = &llin_newtime;
    if (pa == NULL) {
        return NULL;
    }
    if (newtime->tm_hour == pa->t->end_hour
        && newtime->tm_min >= pa->t->end_min
        && newtime->tm_min < (pa->t->end_min + 5)
        && ((newtime->tm_wday == pa->t->end_day)
            || (pa->t->end_day == 7))) {
        printf("Time to end!!Sock Recv manager: %d min %d \n",
               pa->t->end_hour, pa->t->end_min);
        return NULL;
    }
    if (get_disconnect(pa->con->co)) {
        printf("Told To Disconnect before manage \n");
        pa->con->is_logged_on = 0;
        pa->con->connection_notice(pa->con->co, 0, NULL, 0);
        if (get_is_removed(pa->con->co) == 0) {
            remove_connection(pa->con->co);
        }
        //destroy_con_obj(pa->con->co);
        free(pa->con);
        free(pa);
        return NULL;
    }
    num_timeouts = 0;
    cut_connection = 0;
    run = 1;
    write_offset = 0;
    read_offset = 0;
    total_size = BUFFER_SIZE;
    sock_reader = calloc(BUFFER_SIZE + 1, 1);
    if (sock_reader == NULL) {
        return NULL;
    }
    wr_ptr = sock_reader;
    rd_ptr = sock_reader;
    num_bytes = 0;
    printf("Got into manage sock\n");
    while (run) {
        if (write_offset >= total_size) {
            sock_reader =
                realloc_receive(&total_size, &write_offset, &read_offset,
                                wr_ptr, rd_ptr, sock_reader);
            wr_ptr = sock_reader;
            wr_ptr += write_offset;
            rd_ptr = sock_reader;
        }

        switch (num_bytes =
                recv(sock, wr_ptr, (total_size - write_offset), 0)) {
        case -1:
            err = errno;
            struct tm lin_newtime;
            if (err == EAGAIN || err == EINTR || err == 0) {
                time(&long_time);
                newtime = localtime_r(&long_time, &lin_newtime);
                num_timeouts++;
                if (lin_newtime.tm_hour == pa->t->end_hour
                    && lin_newtime.tm_min >= pa->t->end_min
                    && lin_newtime.tm_min < (pa->t->end_min + 1)
                    && ((newtime->tm_wday == pa->t->end_day)
                        || (pa->t->end_day == 7))) {
                    printf("Time to end!!Sock Recv manager: %d min %d \n",
                           pa->t->end_hour, pa->t->end_min);
                    run = 0;
                    wr_ptr = 0;
                    rd_ptr = 0;
                    break;
                } else if (get_disconnect(pa->con->co)) {
                    printf("Told To Disconnect!!\n");
                    run = 0;
                    wr_ptr = 0;
                    rd_ptr = 0;
                    break;
                } else if (pa->check_activity && num_timeouts > 10) {
                    pa->low_activity_pulse(pa->con->co);
                }
            } else {
                printf("Failed on the listen1\n");
                switch (err) {
                case EADDRINUSE:
                    printf("The address is already in use?\n");
                    break;
                case EBADF:
                    printf
                        ("the argument sockfd is not a valid descriptor\n");
                    break;
                case ENOTSOCK:
                    printf("Listenfd is not a socket!\n");
                    break;
                case EOPNOTSUPP:
                    printf
                        ("This socket type does not support the listen operation\n");
                    break;
                }
                run = 0;
                wr_ptr = 0;
                rd_ptr = 0;
                break;
            }
            break;
        case 0:
            printf("Connection Closed!!\n");
            run = 0;
            wr_ptr = 0;
            rd_ptr = 0;
            break;
        default:
            num_timeouts = 0;
            //fall out of the switch statement.
            break;
        }
        if (num_bytes > 0) {
            unsigned long bytes_read = (unsigned long) num_bytes;
            unsigned long total_bytes_read =
                bytes_read + (write_offset - read_offset);
            unsigned long bytes_used =
                pa->parse(rd_ptr, total_bytes_read, &cut_connection,
                          pa->con);
            if (cut_connection || get_disconnect(pa->con->co)) {
                printf("Parser told us to Cut Connection!!\n");
                wr_ptr = 0;
                rd_ptr = 0;
                run = 0;
                break;
            }
            if (bytes_used >= total_bytes_read) {
                write_offset = 0;
                read_offset = 0;
                wr_ptr = sock_reader;
                rd_ptr = sock_reader;
            } else {
                read_offset += bytes_used;
                write_offset += bytes_read;
                if (read_offset >= total_size ||
                    write_offset >= total_size) {
                    rd_ptr += bytes_used;
                    sock_reader =
                        realloc_receive(&total_size, &write_offset,
                                        &read_offset, wr_ptr, rd_ptr,
                                        sock_reader);
                    if (sock_reader == NULL) {
                        run = 0;
                        wr_ptr = 0;
                        rd_ptr = 0;
                        break;
                    }
                    wr_ptr = sock_reader;
                    wr_ptr += write_offset;
                    rd_ptr = sock_reader;
                } else {
                    rd_ptr = rd_ptr + bytes_used;
                    wr_ptr = wr_ptr + bytes_read;
                }
            }
        }
    }
    if (sock_reader) {
        free(sock_reader);
    }
    wr_ptr = 0;
    rd_ptr = 0;
    pa->con->is_logged_on = 0;
    if(get_is_removed(pa->con->co) != 1) {
        remove_connection(pa->con->co);
    }
    if (get_should_notify(pa->con->co)) {
        pa->con->connection_notice(pa->con->co, 0, NULL, 0);
    }
    //destroy_con_obj(pa->con->co);
    if (pa->con->mod_name) {
        free(pa->con->mod_name);
        pa->con->mod_name = 0;
    }
    free(pa->con);
    free(pa);
    return 0;
}
