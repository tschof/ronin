/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include "server_socket.h"
#include "client_manager.h"
#include "debug_logger.h"
#include "sock_recv_manager.h"

static pthread_t listen_thread;

static void *run_server_socket(void *s);


static void serv_close_socket(int sock)
{
    int err = 0;
    shutdown(sock, SHUT_RDWR);
    err = close(sock);
}

/**
 * Start the pthread detached.
 */
int start_server_socket(void *dart_init)
{
    pthread_attr_t thread_attr;
    int status;
    size_t stack_size;
    status = pthread_attr_init(&thread_attr);
    if (status != 0) {
        send_debug_message("Pthread attr init failed!\n");
        return -1;
    }
    status = pthread_attr_setdetachstate(&thread_attr,
                                         PTHREAD_CREATE_DETACHED);

    if (status != 0) {
        send_debug_message("Thread set detached failed!\n");
        return -1;
    }

    status = pthread_attr_getstacksize(&thread_attr, &stack_size);
    //send_debug_message("Default Stack size= {%lu} \n", stack_size);
    pthread_create(&listen_thread, &thread_attr, run_server_socket,
                   dart_init);
    pthread_detach(listen_thread);
    return 0;
}

int set_sock_opt(int sock)
{
    int on = 1;
    int buffer = BUFFER_SIZE;
    struct timeval tv;
    struct timeval s_tv;
    if (setsockopt
        (sock, IPPROTO_TCP, TCP_NODELAY, (char *) &on, sizeof(on)) == -1) {
        send_debug_message("Failed to disable NAGLE!\n");
        serv_close_socket(sock);
        return -1;
    }
    if (setsockopt
        (sock, SOL_SOCKET, SO_REUSEADDR, (const char *) &on,
         sizeof(on)) == -1) {
        send_debug_message("Failed to set reuse adder!\n");
        serv_close_socket(sock);
        return -1;
    }

    if (setsockopt
        (sock, SOL_SOCKET, SO_RCVBUF, (char *) &buffer,
         sizeof(buffer)) == -1) {
        send_debug_message("Failed to set receive buff size!\n");
        serv_close_socket(sock);
        return -1;
    }
    if (setsockopt
        (sock, SOL_SOCKET, SO_SNDBUF, (char *) &buffer,
         sizeof(buffer)) == -1) {
        send_debug_message("Failed to set sendbuf size!\n");
        serv_close_socket(sock);
        return -1;
    }
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    s_tv.tv_sec = 0;
    s_tv.tv_usec = 1;
    if (setsockopt
        (sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof(tv))) {
        send_debug_message("Failed to set recv timeout!\n");
    }
    if (setsockopt
        (sock, SOL_SOCKET, SO_SNDTIMEO, (char *) &s_tv, sizeof(s_tv))) {
        send_debug_message("Failed to set SND timeout!\n");
    }
    return sock;
}

static int time_to_close(struct server_con_set *vals)
{
    time_t long_time;
    struct tm *newtime;
    int ret_val = 0;
    time(&long_time);
    struct tm lm;
    newtime = localtime_r(&long_time, &lm);
    newtime = &lm;
    if (newtime->tm_hour == vals->pa->t->end_hour
        && newtime->tm_min >= vals->pa->t->end_min
        && newtime->tm_min < (vals->pa->t->end_min + 5)
        && ((newtime->tm_wday == vals->pa->t->end_day)
            || (vals->pa->t->end_day == 7))) {
        ret_val = 1;
    }
    return ret_val;
}

static void create_sock_parser(struct server_con_set *vals, int newSock)
{
    struct parse_args *lpa;
    struct parse_con *pcon;
    con_obj *co;
    int status;
    pthread_t l_thread;
    pthread_attr_t thread_attr;
    send_debug_message("New sock accepted: {%d} \n", newSock);
    if (!vals->use_ud) {
        newSock = set_sock_opt(newSock);
    }

    lpa = malloc(sizeof(struct parse_args));
    pcon = malloc(sizeof(struct parse_con));
    if (pcon != NULL && lpa != NULL) {
        char *name = (char *) get_name(vals->pa->con->co);
        lpa->parse = vals->pa->parse;
        pcon->send_ptr = vals->pa->con->send_ptr;
        pcon->callback = vals->pa->con->callback;
        pcon->record_mess = vals->pa->con->record_mess;
        pcon->init_con = vals->pa->con->init_con;
        pcon->remove_con = vals->pa->con->remove_con;
        pcon->connection_notice = vals->pa->con->connection_notice;
        pcon->timeout_val = vals->pa->con->timeout_val;
        pcon->in_args_list = vals->pa->con->in_args_list;
        pcon->in_args = NULL;
        pcon->t = vals->pa->t;
        pcon->is_server = 1;
        pcon->is_logged_on = 0;
        pcon->copy_parser_helper = vals->pa->con->copy_parser_helper;
        pcon->p_callback = vals->pa->con->p_callback;
        if (vals->pa->con->parser_helper != NULL) {
            pcon->parser_helper =
                vals->pa->con->copy_parser_helper(vals->pa->con->
                                                  parser_helper);
        } else {
            pcon->parser_helper = NULL;
        }
        co = create_con_obj(name,
                            get_name_len(vals->pa->con->co),
                            newSock, 0, 1, 1);
        pcon->co = co;
        pcon->book = vals->pa->con->book;
        if (vals->pa->con->mod_len > 0 && vals->pa->con->mod_len < 100) {
            pcon->mod_name = calloc(vals->pa->con->mod_len + 1, 1);
            memcpy(pcon->mod_name, vals->pa->con->mod_name,
                   vals->pa->con->mod_len);
            pcon->mod_len = vals->pa->con->mod_len;
        } else {
            pcon->mod_len = 0;
            pcon->mod_name = NULL;
        }
        lpa->con = pcon;
        lpa->low_activity_pulse = vals->pa->low_activity_pulse;
        lpa->check_activity = vals->pa->check_activity;
        lpa->t = vals->pa->t;
        if (name) {
            free(name);
        }
        status = pthread_attr_init(&thread_attr);
        if (status != 0) {
            send_debug_message("Pthread attr init failed!\n");
        }
        status = pthread_attr_setdetachstate(&thread_attr,
                                             PTHREAD_CREATE_DETACHED);
        if (status != 0) {
            send_debug_message("Thread set detached failed!\n");
        }
        pthread_create(&l_thread, &thread_attr, manage_socket, lpa);
    } else {
        if (pcon != NULL) {
            free(pcon);
        }
        if (lpa != NULL) {
            free(lpa);
        }
    }
}

void *run_ud_socket(void *s)
{
    struct server_con_set *vals = (struct server_con_set *) s;
    //  struct timespec timeout;

    int listenfd;
    struct sockaddr_un addr;
    int rc;
    int keep_running = 1;
    int err = 0;

    listenfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listenfd == -1) {
        return NULL;
    }
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    if (vals->ip == NULL) {
        serv_close_socket(listenfd);
        return NULL;
    } else {
        addr.sun_path[0] = '\0';
        char *d_off = addr.sun_path;
        d_off++;
        memcpy(d_off, vals->ip, vals->ip_len);
    }
    unlink(addr.sun_path);
    rc = bind(listenfd, (struct sockaddr *) &addr, sizeof(addr));
    if (rc < 0) {
        send_debug_message("Bind of listening socket failed!!!!\n");
        serv_close_socket(listenfd);
        return NULL;
    }
    while (keep_running) {
        rc = listen(listenfd, MAXCONNECTIONS);
        if (rc < 0) {
            err = errno;
            if (err == EAGAIN || err == EINTR) {
                send_debug_message("Time out!\n");
                if (time_to_close(vals)) {
                    keep_running = 0;
                    shutdown(listenfd, 2);
                    serv_close_socket(listenfd);
                    return 0;
                }
            } else {
                switch (err) {
                case EADDRINUSE:
                    keep_running = 0;
                    send_debug_message("The address is already in use?\n");
                    break;
                case EBADF:
                    keep_running = 0;
                    send_debug_message
                        ("the argument sockfd is not a valid descriptor\n");
                    break;
                case ENOTSOCK:
                    keep_running = 0;
                    send_debug_message("Listenfd is not a socket!\n");
                    break;
                case EOPNOTSUPP:
                    keep_running = 0;
                    send_debug_message
                        ("This socket type does not support the listen operation\n");
                    break;
                }
                if (keep_running == 0) {
                    shutdown(listenfd, 2);
                    serv_close_socket(listenfd);
                    return 0;
                }
            }
        } else {
            if (time_to_close(vals)) {
                shutdown(listenfd, 2);
                serv_close_socket(listenfd);
                return 0;
            }

            int newSock = -1;
            if ((newSock = accept(listenfd, NULL, NULL)) == -1) {
                send_debug_message("Accept failed.\n");
                continue;
            }
            if (newSock >= 0) {
                create_sock_parser(vals, newSock);
            }
        }
    }

    return NULL;
}

int start_ud_binder(void *dart_init)
{
    pthread_attr_t thread_attr;
    int status;
    size_t stack_size;
    status = pthread_attr_init(&thread_attr);
    if (status != 0) {
        send_debug_message("Pthread attr init failed!\n");
        return -1;
    }
    status = pthread_attr_setdetachstate(&thread_attr,
                                         PTHREAD_CREATE_DETACHED);

    if (status != 0) {
        send_debug_message("Thread set detached failed!\n");
        return -1;
    }

    status = pthread_attr_getstacksize(&thread_attr, &stack_size);
    //send_debug_message("Default Stack size= {%lu} \n", stack_size);
    pthread_create(&listen_thread, &thread_attr, run_ud_socket, dart_init);
    pthread_detach(listen_thread);
    return 0;
}

/**
 * Here we cast the void* into an server_con_set structure defined in
 * client_manager.h.
 */
void *run_server_socket(void *s)
{
    struct server_con_set *vals = (struct server_con_set *) s;
    //  struct timespec timeout;

    int listenfd;
    struct sockaddr_in addr;
    int rc;
    int keep_running = 1;
    int err;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        return NULL;
    }
    listenfd = set_sock_opt(listenfd);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    if (vals->ip == NULL) {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
        addr.sin_addr.s_addr = inet_addr(vals->ip);
    }
    addr.sin_port = htons(vals->port);
    rc = bind(listenfd, (struct sockaddr *) &addr, sizeof(addr));
    if (rc < 0) {
        send_debug_message("Bind of listening socket failed!!!!\n");
        serv_close_socket(listenfd);
        return NULL;
    }
    while (keep_running) {
        rc = listen(listenfd, MAXCONNECTIONS);
        if (rc < 0) {
            err = errno;
            if (err == EAGAIN || err == EINTR) {
                send_debug_message("Time out!\n");
                if (time_to_close(vals)) {
                    keep_running = 0;
                    shutdown(listenfd, 2);
                    serv_close_socket(listenfd);
                    return 0;
                }
            } else {
                switch (err) {
                case EADDRINUSE:
                    keep_running = 0;
                    send_debug_message("The address is already in use?\n");
                    break;
                case EBADF:
                    keep_running = 0;
                    send_debug_message
                        ("the argument sockfd is not a valid descriptor\n");
                    break;
                case ENOTSOCK:
                    keep_running = 0;
                    send_debug_message("Listenfd is not a socket!\n");
                    break;
                case EOPNOTSUPP:
                    keep_running = 0;
                    send_debug_message
                        ("This socket type does not support the listen operation\n");
                    break;
                }
                if (keep_running == 0) {
                    shutdown(listenfd, 2);
                    serv_close_socket(listenfd);
                    return 0;
                }
            }
        } else {
            if (time_to_close(vals)) {
                keep_running = 0;
                shutdown(listenfd, 2);
                serv_close_socket(listenfd);
                return 0;
            }
            struct sockaddr_in laddr;
            int newSock;
            int accept_tries;
            socklen_t addr_len;
            memset(&laddr, 0, sizeof(laddr));
            addr_len = sizeof(laddr);
            newSock = -1;
            accept_tries = 0;
            // This is linux specific.
            if (errno != EINTR) {
                err = EAGAIN;
            } else {
                err = errno;
                if (time_to_close(vals)) {
                    keep_running = 0;
                    shutdown(listenfd, 2);
                    serv_close_socket(listenfd);
                    return 0;
                }
            }
            while (newSock < 0 && (err == EAGAIN || err == ENETDOWN ||
                                   err == EPROTO || err == ENOPROTOOPT ||
                                   err == EHOSTDOWN || err == ENONET ||
                                   err == EHOSTUNREACH || err == EOPNOTSUPP
                                   || err == ENETUNREACH || err == EINTR)
                   && accept_tries < 5) {
                newSock =
                    accept(listenfd, (struct sockaddr *) &laddr,
                           &addr_len);
                err = errno;
                ++accept_tries;
            }
            if (newSock >= 0) {
                create_sock_parser(vals, newSock);
            }

        }
    }
    serv_close_socket(listenfd);
    return 0;
}
