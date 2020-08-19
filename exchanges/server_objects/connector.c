/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "connector.h"
#include "sock_recv_manager.h"
#include "dart_connection_object.h"
#include "debug_logger.h"

static void con_close_socket(int sock)
{
    int err = 0;
    shutdown(sock, SHUT_RDWR);
    err = close(sock);
}

static int create_socket()
{
    int sock;
    int on = 1;
    int buffer = BUFFER_SIZE;
    struct timeval tv;
    struct timeval s_tv;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        return -1;
    }
    if (setsockopt
        (sock, IPPROTO_TCP, TCP_NODELAY, (char *) &on, sizeof(on)) == -1) {
        send_debug_message("Failed to disable NAGLE!");
        con_close_socket(sock);
        return -1;
    }
    if (setsockopt
        (sock, SOL_SOCKET, SO_REUSEADDR, (const char *) &on,
         sizeof(on)) == -1) {
        send_debug_message("Failed to set reuse adder!");
        con_close_socket(sock);
        return -1;
    }

    if (setsockopt
        (sock, SOL_SOCKET, SO_RCVBUF, (char *) &buffer,
         sizeof(buffer)) == -1) {
        send_debug_message("Failed to set receive buff size!");
        con_close_socket(sock);
        return -1;
    }
    if (setsockopt
        (sock, SOL_SOCKET, SO_SNDBUF, (char *) &buffer,
         sizeof(buffer)) == -1) {
        send_debug_message("Failed to set sendbuf size!");
        con_close_socket(sock);
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
        send_debug_message("Failed to set send timeout!\n");
    }
    return sock;
}

static int ud_connection(struct client_con_set *css)
{
    int sock = -1;
    struct sockaddr_un addr;
    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;

    addr.sun_path[0] = '\0';
    char *d_off = addr.sun_path;
    d_off++;
    memcpy(d_off, css->host, css->host_len);
    if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        con_close_socket(sock);
        sock = -1;
    }
    return sock;
}

static int rec_connection(struct client_con_set *ccs, int i)
{
    int sock;
    int dart_err;
    char portstr[6];
    struct addrinfo hints, *res;
    int con;
    int err;
    sock = create_socket();
    if (sock < 0) {
        send_debug_message("Could not create socket!!!\n");
        return -1;
    }
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME;
    snprintf(portstr, sizeof(portstr), "%d", ccs->port);
    dart_err = getaddrinfo(ccs->host, portstr, &hints, &res);
    if (dart_err != 0) {
        send_debug_message("get addrinfo failed!! \n");
        con_close_socket(sock);
        return -1;
    }
    con = connect(sock, res->ai_addr, res->ai_addrlen);
    err = errno;
    if (con == 0 || err == EALREADY || err == EINPROGRESS || err == EAGAIN
        || err == EINTR) {
        freeaddrinfo(res);
        return sock;
    } else if ((err == ETIMEDOUT || err == ECONNREFUSED) && (i > 0)) {
        con_close_socket(sock);
        freeaddrinfo(res);
        return rec_connection(ccs, --i);
    } else {
        send_debug_message
            ("Fatal socket error: %d, killing connection after %d attempts!\n",
             err, i);
        freeaddrinfo(res);
        con_close_socket(sock);
        return -1;
    }
}

static void *run_connection_loop(void *s)
{
    struct client_con_set *css = (struct client_con_set *) s;
    time_t long_time;
    struct tm *newtime;
    int sock;
    int attempts;
    int keep_running;
    sock = -1;
    keep_running = 1;
    attempts = MAXCONRETRIES;
    time(&long_time);
    struct tm lm;
    newtime = localtime_r(&long_time, &lm);
    newtime = &lm;
    while (keep_running) {
        if (newtime->tm_hour == css->pa->t->end_hour &&
            (newtime->tm_min < (css->pa->t->end_min + 5) &&
             newtime->tm_min >= css->pa->t->end_min)
            && ((newtime->tm_wday == css->pa->t->end_day)
                || (css->pa->t->end_day == 7))) {
            con_close_socket(sock);
            sock = -1;
            keep_running = 0;
            break;
        }
        if (css->use_ud) {
            sock = ud_connection(css);
        } else {
            sock = rec_connection(css, attempts);
        }
        if (sock >= 0) {
            struct parse_args *lpa;
            struct parse_con *pcon;
            con_obj *co;
            if (!css->use_ud) {
                struct timeval s_tv;
                s_tv.tv_sec = 0;
                s_tv.tv_usec = 1;
                if (setsockopt
                    (sock, SOL_SOCKET, SO_SNDTIMEO, (char *) &s_tv,
                     sizeof(s_tv))) {
                    send_debug_message("Failed to set SND timeout!\n");
                }
            }
            lpa = malloc(sizeof(struct parse_args));
            pcon = malloc(sizeof(struct parse_con));
            if (lpa != NULL && pcon != NULL) {
                char *name = (char *) get_name(css->pa->con->co);
                lpa->parse = css->pa->parse;
                pcon->send_ptr = css->pa->con->send_ptr;
                pcon->callback = css->pa->con->callback;
                pcon->record_mess = css->pa->con->record_mess;
                pcon->init_con = css->pa->con->init_con;
                pcon->remove_con = css->pa->con->remove_con;
                pcon->connection_notice = css->pa->con->connection_notice;
                pcon->in_args_list = css->pa->con->in_args_list;
                pcon->book = css->pa->con->book;
                pcon->in_args = css->pa->con->in_args;
                pcon->is_logged_on = 0;
                pcon->t = css->pa->t;
                pcon->is_server = 0;
                pcon->timeout_val = css->pa->con->timeout_val;
                co = create_con_obj(name,
                                    get_name_len(css->pa->con->co),
                                    sock, 0, 1, 1);
                pcon->co = co;
                if (css->pa->con->mod_len > 0
                    && css->pa->con->mod_len < 100) {
                    pcon->mod_name = calloc(css->pa->con->mod_len + 1, 1);
                    memcpy(pcon->mod_name, css->pa->con->mod_name,
                           css->pa->con->mod_len);
                    pcon->mod_len = css->pa->con->mod_len;
                } else {
                    pcon->mod_len = 0;
                    pcon->mod_name = NULL;
                }
                lpa->con = pcon;
                lpa->t = css->pa->t;
                lpa->low_activity_pulse = css->pa->low_activity_pulse;
                lpa->check_activity = css->pa->check_activity;
                free(name);
                /* This will block and depending on how the connection
                 * comes down we may just shut down the whole
                 * shebang. */
                css->init_connection(lpa);
                manage_socket(lpa);
                keep_running = 0;
                break;
            }
        } else {
            con_obj *co;
            char *name = (char *) get_name(css->pa->con->co);
            send_debug_message
                ("Fatal socket error: %d, killing connection!\n", sock);
            keep_running = 0;
            if(sock > 0) {
                con_close_socket(sock);
            }
            sock = -1;
            co = create_con_obj(name,
                                get_name_len(css->pa->con->co), 0, 0, 1,
                                1);
            css->pa->con->connection_notice(co, 0, NULL, 0);
            free(name);
            destroy_con_obj(co);
        }
    }
    destroy_con_obj(css->pa->con->co);
    if (css->host) {
        free((char *) css->host);
    }
    if (css->pa->con->mod_name) {
        free(css->pa->con->mod_name);
        css->pa->con->mod_name = 0;
    }
    destroy_init_args_list(css->pa->con->in_args_list);
    destroy_queue(css->pa->con->in_args_list);
    free(css->pa->con);
    free(css->pa);
    free(css);
    return 0;
}

void connect_dart(struct client_con_set *ccs)
{
    pthread_t l_thread;
    pthread_attr_t thread_attr;
    int status;
    status = pthread_attr_init(&thread_attr);
    if (status != 0) {
        send_debug_message("Pthread attr init failed!\n");
    }
    status = pthread_attr_setdetachstate(&thread_attr,
                                         PTHREAD_CREATE_DETACHED);
    if (status != 0) {
        send_debug_message("Thread set detached failed!\n");
    }
    pthread_create(&l_thread, &thread_attr, run_connection_loop, ccs);

}
