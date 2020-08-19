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
#include <fcntl.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/timerfd.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <sys/un.h>

#include "async_server.h"
#include "dart_thr_pool.h"
#include "rb_tree.h"
#include "debug_logger.h"
#include "dbg_helper.h"
#include "master_config.h"
#include "gatekeeper.h"

void async_int_destroy(void *a)
{
    free((int *) a);
}

int async_int_compare(const void *a, const void *b)
{
    if (*(int *) a > *(int *) b) {
        return 1;
    }
    if (*(int *) a < *(int *) b) {
        return -1;
    }
    return 0;
}

void async_int_print(const void *a)
{
    printf("%d", *(int *) a);
}

void async_info_print(const void *a)
{
    printf("Printing the object");
}

void async_info_destroy(void *a)
{
    struct async_parse_args * pa = (struct async_parse_args*)a;
    if(!pa->is_server) {
        destroy_queue(pa->in_args_list);
    }
    destroy_sock_recv_manager(pa->con);
    destroy_call_back_wrapper(pa->read_wrapper);
    destroy_call_back_wrapper(pa->write_wrapper);
    free(pa);
}


typedef struct async_server {
    thr_pool_t *thr;
    rb_tree_t *connections;
    hashmap_t parse_args;
    gatekeeper *gk;
    queue_t event_q;
    int *cpus;
    pthread_spinlock_t clean_mutex;
    int num_threads;
    int time_out;
    int keep_running;
    int efd;
} async_server;

typedef struct event_object {
    void *stuff;
    async_server *ass;
    void (*ctl_call_event) (void *s);
    int sock;
} event_object;

int check_is_connected(async_server_con_set * set, char *name,
        int name_len)
{
    int ret = 0;
    if (pthread_spin_lock(&set->clean_mutex) == 0) {
        ret = contains_key(set->parse_args, name, name_len);
        pthread_spin_unlock(&set->clean_mutex);
    }
    return ret;
}

int add_connection(async_server_con_set * set, char *name,
        int name_len, async_parse_args * apa)
{
    int ret = 0;
    if (pthread_spin_lock(&set->clean_mutex) == 0) {
        ret = no_copy_insert(set->parse_args, name, name_len, apa);
        pthread_spin_unlock(&set->clean_mutex);
    }
    return ret;
}

int remove_connection(async_server_con_set * set, char *name,
        int name_len, async_parse_args * apa)
{
    int ret = 0;
    if (pthread_spin_lock(&set->clean_mutex) == 0) {
        ret = remove_from_map(set->parse_args, name, name_len);
        pthread_spin_unlock(&set->clean_mutex);
    }
    return ret;
}

void async_stop_system(async_server_con_set* set)
{
    thr_pool_destroy(set->thr);
    set->keep_running = 0;
}

static void async_close_socket(int sock)
{
    close(sock);
}

int async_set_sock_opt(int sock)
{
    int on = 1;
    int buffer = 65536;
    int flags = O_NONBLOCK;
    flags = fcntl(sock, F_SETFL, O_NONBLOCK);
    if (flags == -1) {
        send_debug_message("Failed to make socket nonblockings %d \n",
                sock);
    }
    if (setsockopt
            (sock, IPPROTO_TCP, TCP_NODELAY, (char *) &on, sizeof(on)) == -1) {
        send_debug_message("Failed to disable NAGLE! %d \n", sock);
    }
    if (setsockopt
            (sock, SOL_SOCKET, SO_REUSEADDR, (const char *) &on,
             sizeof(on)) == -1) {
        send_debug_message("Failed to set reuse adder!%d \n", sock);
    }

    if (setsockopt
            (sock, SOL_SOCKET, SO_RCVBUF, (char *) &buffer,
             sizeof(buffer)) == -1) {
        send_debug_message("Failed to set receive buff size!%d \n", sock);
    }
    if (setsockopt
            (sock, SOL_SOCKET, SO_SNDBUF, (char *) &buffer,
             sizeof(buffer)) == -1) {
        send_debug_message("Failed to set sendbuf size!%d \n", sock);
    }
    return sock;
}

static int async_rec_connection(int port, const char *host, int i)
{
    int sock;
    int dart_err;
    char portstr[6];
    struct addrinfo hints, *res;
    int con;
    int err;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        return -1;
    }
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME;
    snprintf(portstr, sizeof(portstr), "%d", port);
    dart_err = getaddrinfo(host, portstr, &hints, &res);
    if (dart_err != 0) {
        send_debug_message("get addrinfo failed!!%d \n", sock);
        async_close_socket(sock);
        return -1;
    }
    int keep_going = 1;
    while (keep_going) {
        send_debug_message
            ("Attempting to connect to: %s, socket: %d\n",
             host, sock);
        con = connect(sock, res->ai_addr, res->ai_addrlen);
        err = errno;
        if (con == 0 || err == EALREADY || err == EINPROGRESS
                || err == EAGAIN || err == EINTR) {
            freeaddrinfo(res);
            keep_going = 0;
            break;
        } else if ((err == ETIMEDOUT || err == ECONNREFUSED) && (i > 0)) {
            --i;
            if (i < 0) {
                keep_going = 0;
                freeaddrinfo(res);
                async_close_socket(sock);
                break;
            }
        } else {
            send_debug_message
                ("Fatal socket error: %d, killing connection after %d attempts!\n",
                 err, i);
            freeaddrinfo(res);
            async_close_socket(sock);
            keep_going = 0;
            sock = -1;
        }
    }
    return sock;
}

static int ud_connect(const char *ip, int ip_len)
{
    int sock = -1;
    struct sockaddr_un addr;
    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;

    addr.sun_path[0] = '\0';
    char *d_off = addr.sun_path;
    d_off++;
    memcpy(d_off, ip, ip_len);
    if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        async_close_socket(sock);
        sock = -1;
    }
    fcntl(sock, F_SETFL, O_NONBLOCK);
    return sock;
}

static int create_and_bind(int port)
{
    int listenfd;
    struct sockaddr_in addr;
    int rc;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        return -1;
    }
    listenfd = async_set_sock_opt(listenfd);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    rc = bind(listenfd, (struct sockaddr *) &addr, sizeof(addr));
    if (rc < 0) {
        send_debug_message("Bind of socket failed!!!!%d \n", listenfd);
        async_close_socket(listenfd);
        return -1;
    }
    return listenfd;
}

static int ud_create_and_bind(const char *ip, int ip_len)
{
    int sock = -1;
    struct sockaddr_un addr;
    int rc;

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock == -1) {
        return 0;
    }
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    if (ip == NULL) {
        async_close_socket(sock);
        return 0;
    } else {
        addr.sun_path[0] = '\0';
        char *d_off = addr.sun_path;
        d_off++;
        memcpy(d_off, ip, ip_len);
    }
    unlink(addr.sun_path);
    rc = bind(sock, (struct sockaddr *) &addr, sizeof(addr));
    if (rc < 0) {
        send_debug_message("Bind of listening socket failed!!!!\n");
        async_close_socket(sock);
        return -1;
    }
    fcntl(sock, F_SETFL, O_NONBLOCK);
    return sock;
}

static void async_add_back(void *k)
{
    event_object *eo = (event_object *) k;
    if (pthread_spin_lock(&eo->ass->clean_mutex) == 0) {
        async_parse_args *pa =
            rb_tree_get_val(eo->ass->connections, &eo->sock);
        if (pa != NULL) {
            int ret_code = 0;
            ret_code =
                epoll_ctl(pa->read_wrapper->efd, EPOLL_CTL_ADD, pa->sock,
                        pa->read_wrapper->ee);
            if (ret_code == -1) {
                int err = errno;
                if (err == EEXIST) {
                    ret_code =
                        epoll_ctl(pa->read_wrapper->efd, EPOLL_CTL_MOD,
                                pa->sock, pa->read_wrapper->ee);
                    if (ret_code == -1) {
                        err = errno;
                    }
                }
            }
        } else {
            send_debug_message
                ("Unable to find this connection for write events: %d!\n",
                 eo->sock);
            rb_tree_print(eo->ass->connections);
        }
        pthread_spin_unlock(&eo->ass->clean_mutex);
    }
}

void create_recv_epoll_ctl_event(async_parse_args * pa)
{
    event_object *wo = calloc(1, sizeof(struct event_object));
    wo->ass = pa->ass;
    wo->stuff = pa;
    wo->sock = pa->sock;
    wo->ctl_call_event = async_add_back;
    if (pthread_spin_lock(&pa->ass->clean_mutex) == 0) {
        enqueue(pa->ass->event_q, wo);
        pthread_spin_unlock(&pa->ass->clean_mutex);
    }
}

void disconnect_by_name(async_server_con_set * set, const char *name,
        int name_len)
{
    async_parse_args *apa = 0;
    int ret = 0;
    if (pthread_spin_lock(&set->clean_mutex) == 0) {
        ret =
            get_map_entry(set->parse_args, name, name_len, (void **) &apa);
        pthread_spin_unlock(&set->clean_mutex);
    }
    if (ret > 0 && apa) {
        disconnect(apa);
    }
}

void disconnect(struct async_parse_args *pa)
{
    int s = pa->sock;
    if (pthread_spin_lock(&pa->ass->clean_mutex) == 0) {
        rb_tree_delete(pa->ass->connections, &s);
        async_close_socket(s);
        if(pa->write_wrapper) {
            async_close_socket(pa->write_wrapper->sock);
        }
        pa->is_logged_on = 0;
        pthread_spin_unlock(&pa->ass->clean_mutex);
        if(pa->in_args && pa->in_args->name) {
            if(pa->removed == 0) {
                remove_connection(pa->ass, pa->in_args->name,
                                  pa->in_args->name_len, pa);
            }
            pa->connection_notice(pa, 0, NULL, 0);
        }
        destroy_call_back_wrapper(pa->read_wrapper);
        pa->read_wrapper = 0;
        destroy_call_back_wrapper(pa->write_wrapper);
        pa->write_wrapper = 0;
        if (pa->is_server) {
            pthread_spin_lock(&pa->ass->clean_mutex);
            destroy_sock_recv_manager(pa->con);
            pthread_spin_unlock(&pa->ass->clean_mutex);
            free(pa);
        }
    }
}

static void disconnect_wrapper(void* a)
{
    disconnect((async_parse_args*)a);
}

static void *write_master(void *w)
{
    struct async_parse_args *pa = (struct async_parse_args *) w;

    epoll_ctl(pa->write_wrapper->efd, EPOLL_CTL_DEL, pa->write_wrapper->sock,
            pa->write_wrapper->ee);
    pa->write_callback(pa->in_args->name, pa->in_args->name_len,
            pa->write_pass_back);
    return NULL;
}

static void *raw_sock_parser(void *p_args)
{
    struct async_parse_args* pa = (struct async_parse_args*)p_args;
    int cut_connection = 0;
    manage_socket(pa, pa->sock, &cut_connection);
    if (cut_connection) {
        int s = pa->sock;
        send_debug_message("Removing this connection from tree: %d \n", s);
        disconnect(pa);
    } else if (pa->ass->num_threads > 2) {
        create_recv_epoll_ctl_event(pa);
    }
    return NULL;
}

    async_parse_args *create_parse_args(
            const char *mod_name, size_t mod_len,
            uint16_t port, const char *ip,
            size_t ip_len, int end_hour,
            int end_min, int end_day,
            size_t timeout_val, void *ob,
            void (*connection_callback)
            (async_parse_args * co, int is_connected,
             char *log, int len),
            void (*client_mess_parse) (int
                *cut_con,
                struct
                async_parse_args
                * pc),
            void *args,
            int (*init_con) (struct
                async_parse_args *
                pa),
            int sock,
            void *write_pass_back,
            void (*write_callback) (const char
                    *name,
                    int name_len,
                    void
                    *pass_back),
            int is_ud,
            void (*parse_v) (char *data, long len,
                    void *pc),
            void (*obj_callback) (dart_order_obj *
                    ph, void *ob))
{
    struct async_parse_args *pa = calloc(sizeof(struct async_parse_args), 1);
    queue_t in_q = create_sized_queue(128);
    pa->connection_notice = connection_callback;
    pa->is_logged_on = 0;
    pa->ds_book = ob;
    pa->parse_v = parse_v;
    pa->obj_callback = obj_callback;
    enqueue(in_q, (struct init_args *) args);
    pa->in_args_list = in_q;  //args;
    pa->in_args = (struct init_args *) args;
    pa->ip = (char *) ip;
    pa->ip_len = ip_len;
    pa->port = port;
    pa->sock = sock;
    pa->removed = 0;
    pa->parse = (*client_mess_parse);
    //pa->seq_parse = (*seq_parse);
    pa->init_con = init_con;
    pa->con = create_sock_recv_manager();
    pa->write_callback = write_callback;
    pa->write_pass_back = write_pass_back;
    pa->async_cb_func = raw_sock_parser;
    return pa;
}

static struct async_parse_args *clone_parse_args(struct async_parse_args
        *pa, int newSock)
{
    struct async_parse_args *lpa;
    send_debug_message("New sock accepted: {%d} \n", newSock);
    newSock = async_set_sock_opt(newSock);
    lpa = calloc(sizeof(struct async_parse_args), 1);
    if (lpa != NULL) {
        lpa->parse = pa->parse;
        lpa->connection_notice = pa->connection_notice;
        lpa->in_args_list = pa->in_args_list;
        //lpa->mc = pa->ass->mc;
        lpa->in_args = NULL;
        lpa->is_server = 1;
        lpa->sock = newSock;
        lpa->is_logged_on = 0;
        lpa->ip = NULL;
        lpa->ip_len = 0;
        lpa->removed = 0;
        lpa->parse_v = pa->parse_v;
        lpa->gk = pa->gk;
        lpa->obj_callback = pa->obj_callback;
        lpa->ds_book = pa->ds_book;
        lpa->con = create_sock_recv_manager();
        lpa->init_con = pa->init_con;
        lpa->async_cb_func = raw_sock_parser;
        lpa->write_callback = pa->write_callback;
        lpa->write_pass_back = pa->write_pass_back;
    }
    return lpa;
}

static void create_new_conversation(async_parse_args * ascs, int sock)
{
    async_parse_args *pa = 0;
    int *in_sock = calloc(sizeof(int), 1);
    pa = clone_parse_args(ascs, sock);
    pa->read_wrapper = create_call_back_wrapper(pa,
            pa->async_cb_func, disconnect_wrapper, sock);
    pa->read_wrapper->ee->events = EPOLLIN | EPOLLRDHUP | EPOLLET; //EPOLLONESHOT;
    pa->read_wrapper->efd = ascs->ass->efd;
    pa->ass = ascs->ass;
    int dupper = dup(sock);
    if (dupper == -1) {
        send_debug_message("Failed to dup sock:  %d.", sock);
    }
    pa->write_wrapper = create_call_back_wrapper(pa,
            write_master,disconnect_wrapper, dupper);
    pa->write_wrapper->ee->events =  EPOLLOUT | EPOLLET; //EPOLLONESHOT;
    pa->write_wrapper->efd = ascs->ass->efd;
    *in_sock = sock;
    if (pthread_spin_lock(&ascs->ass->clean_mutex) == 0) {
        rb_tree_no_delete_insert(ascs->ass->connections, in_sock, pa);
        pthread_spin_unlock(&ascs->ass->clean_mutex);
    }
    create_recv_epoll_ctl_event(pa);
    send_debug_message("Adding this connection to the tree: %d dup: %d\n", sock, dupper);
}

void* ud_listen_call_back_func(void* p_args)
{
    struct async_parse_args* pa = (struct async_parse_args*)p_args;
    int listenfd = pa->sock;
    while (1) {
        int sock = -1;
        if ((sock = accept(listenfd, NULL, NULL)) == -1) {
            break;
        }
        fcntl(sock, F_SETFL, O_NONBLOCK);
        send_debug_message
            ("Accepted connection: sock %d, on the ud_connection.\n",
             sock);
        async_set_sock_opt(sock);
        create_new_conversation(pa, sock);
    }
    create_recv_epoll_ctl_event(pa);
    return NULL;
}

void* listen_call_back_func(void* p_args)
{
    struct async_parse_args* pa = (struct async_parse_args*)p_args;
    int listenfd = pa->sock;
    while (1) {
        struct sockaddr in_addr;
        socklen_t in_len = sizeof(in_addr);
        char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
        memset(hbuf, '\0', NI_MAXHOST);
        memset(sbuf, '\0', NI_MAXSERV); 
        int sock = accept(listenfd, &in_addr, &in_len);
        if (sock == -1) {
            break;
        }
        int ret_code =
            getnameinfo(&in_addr, in_len, hbuf,
                    sizeof(hbuf), sbuf, sizeof(sbuf),
                    NI_NUMERICHOST | NI_NUMERICSERV);
        if(ret_code == 0) {
            send_debug_message
                ("Accepted connection: sock %d, host = %s, port = %s\n",
                 sock, hbuf, sbuf);
        }
        sock = async_set_sock_opt(sock);
        create_new_conversation(pa, sock);
    }
    create_recv_epoll_ctl_event(pa);
    return NULL;
}
static void async_add_sender(void *k)
{
    struct event_object *ev = (struct event_object *) k;
    if (pthread_spin_lock(&ev->ass->clean_mutex) == 0) {
        async_parse_args *pa =
            rb_tree_get_val(ev->ass->connections, &ev->sock);
        if (pa != NULL) {
            int ret_code = 0;
            pa->write_pass_back = ev->stuff;
            ret_code =
                epoll_ctl(pa->write_wrapper->efd, EPOLL_CTL_ADD, pa->write_wrapper->sock,
                        pa->write_wrapper->ee);
            send_debug_message
                ("Tried an add got this return: %d for sock: %d!\n",
                 ret_code, ev->sock);
            if (ret_code == -1) {
                int err = errno;
                if (err == EEXIST) {
                    ret_code =
                        epoll_ctl(pa->write_wrapper->efd, EPOLL_CTL_MOD,
                                pa->write_wrapper->sock, pa->write_wrapper->ee);
                    if (ret_code == -1) {
                        err = errno;
                    }
                    send_debug_message
                        ("Got an EExist error tried a mod and got this return: %d for sock: %d!\n",
                         ret_code, ev->sock);
                }
            }
        } else {
            send_debug_message
                ("Unable to find this connection for write events: %d!\n",
                 ev->sock);
            rb_tree_print(ev->ass->connections);
        }
        pthread_spin_unlock(&ev->ass->clean_mutex);
    }
}

static void add_timer_task(void* k)
{
    struct event_object* ev = (struct event_object*)k;
    if (pthread_spin_lock(&ev->ass->clean_mutex) == 0) {
        call_back_wrapper* cbw = (call_back_wrapper*)ev->stuff;
        if(cbw) {
            int ret_code = epoll_ctl(cbw->efd, EPOLL_CTL_ADD, cbw->sock,
                    cbw->ee);
            if(ret_code == -1) {
                int err = errno;
                if(err == EEXIST) {
                    epoll_ctl(cbw->efd, EPOLL_CTL_MOD, cbw->sock,
                            cbw->ee);
                }
            }
        }
        pthread_spin_unlock(&ev->ass->clean_mutex);
    }
}
static void remove_timer_task(void* k)
{
    struct event_object* ev = (struct event_object*)k;
    if (pthread_spin_lock(&ev->ass->clean_mutex) == 0) {
        call_back_wrapper* cbw = (call_back_wrapper*)ev->stuff;
        if(cbw) {
            int ret_code = epoll_ctl(cbw->efd, EPOLL_CTL_DEL, cbw->sock,
                    cbw->ee);
            if(ret_code == -1) {
                int err = errno;
                printf("Error removing timer: %d.\n", err);
            }
            destroy_call_back_wrapper(cbw);
        }
        pthread_spin_unlock(&ev->ass->clean_mutex);
    }
}

void add_back_rep_timer(async_server_con_set* ass, call_back_wrapper* cbw)
{
    if (ass->num_threads > 2) {
        event_object *wo = calloc(1, sizeof(struct event_object));
        wo->ass = ass;
        wo->stuff = cbw;
        wo->sock = cbw->sock;
        wo->ctl_call_event = add_timer_task;
        if (pthread_spin_lock(&ass->clean_mutex) == 0) {
            enqueue(ass->event_q, wo);
            pthread_spin_unlock(&ass->clean_mutex);
        }
    }
}

void remove_and_delete_timer(async_server_con_set* ass, call_back_wrapper* cbw)
{
    close(cbw->sock);
    event_object *wo = calloc(1, sizeof(struct event_object));
    wo->ass = ass;
    wo->stuff = cbw;
    wo->sock = cbw->sock;
    wo->ctl_call_event = remove_timer_task;
    if (pthread_spin_lock(&ass->clean_mutex) == 0) {
        enqueue(ass->event_q, wo);
        pthread_spin_unlock(&ass->clean_mutex);
    }
}

void add_back_recon_timer(async_server_con_set* ass, call_back_wrapper* cbw)
{
    struct timespec now;
    struct itimerspec newtime;
    clock_gettime(CLOCK_REALTIME, &now);
    newtime.it_interval.tv_sec = 0;
    newtime.it_interval.tv_nsec = 0;
    newtime.it_value.tv_sec = now.tv_sec + 30;
    newtime.it_value.tv_nsec = 0;
    if(timerfd_settime(cbw->sock, TFD_TIMER_ABSTIME, &newtime, NULL) == -1) {
        printf("Unable to rearm timer.\n");
    }
    event_object *wo = calloc(1, sizeof(struct event_object));
    wo->ass = ass;
    wo->stuff = cbw;
    wo->sock = cbw->sock;
    wo->ctl_call_event = add_timer_task;
    if (pthread_spin_lock(&ass->clean_mutex) == 0) {
        enqueue(ass->event_q, wo);
        pthread_spin_unlock(&ass->clean_mutex);
    }
}
void add_back_rep_timer_adjust(async_server_con_set* ass, call_back_wrapper* cbw, int new_sec)
{
    struct timespec now;
    struct itimerspec newtime;
    clock_gettime(CLOCK_REALTIME, &now);
    newtime.it_interval.tv_sec = new_sec;
    newtime.it_interval.tv_nsec = 0;
    newtime.it_value.tv_sec = now.tv_sec + new_sec;
    newtime.it_value.tv_nsec = 0;
    if(timerfd_settime(cbw->sock, TFD_TIMER_ABSTIME, &newtime, NULL) == -1) {
        printf("Unable to rearm timer.\n");
    }
    if (ass->num_threads > 2) {
        event_object *wo = calloc(1, sizeof(struct event_object));
        wo->ass = ass;
        wo->stuff = cbw;
        wo->sock = cbw->sock;
        wo->ctl_call_event = add_timer_task;
        if (pthread_spin_lock(&ass->clean_mutex) == 0) {
            enqueue(ass->event_q, wo);
            pthread_spin_unlock(&ass->clean_mutex);
        }
    }
}

void add_signal_listener(async_server_con_set* ass, call_back_wrapper* cbw)
{
    cbw->ee->events = EPOLLIN | EPOLLRDHUP | EPOLLET; //EPOLLONESHOT;
    cbw->efd = ass->efd;

    event_object *wo = calloc(1, sizeof(struct event_object));
    wo->ass = ass;
    wo->stuff = cbw;
    wo->sock = cbw->sock;
    wo->ctl_call_event = add_timer_task;
    if (pthread_spin_lock(&ass->clean_mutex) == 0) {
        enqueue(ass->event_q, wo);
        pthread_spin_unlock(&ass->clean_mutex);
    }
}

void static def_timer_shutdown(void* w)
{
}

call_back_wrapper* add_timer(async_server_con_set* ass, int repeat_interval_sec,
        int repeat_interval_nano, int initial_interval_sec,
        int initial_interval_nano, void* cb_object,
        void* (*t_cb_func)(void* cb))
{
    int sock = -1;
    struct itimerspec itspec;
    sock = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK);
    itspec.it_interval.tv_sec = repeat_interval_sec;
    itspec.it_interval.tv_nsec = repeat_interval_nano;
    itspec.it_value.tv_sec = initial_interval_sec;
    itspec.it_value.tv_nsec = initial_interval_nano;
    timerfd_settime(sock, 0, &itspec, NULL);
    call_back_wrapper* cbw = create_call_back_wrapper(cb_object,
            t_cb_func,def_timer_shutdown, sock);
    cbw->ee->events = EPOLLIN | EPOLLET; //EPOLLONESHOT;
    cbw->efd = ass->efd;
    event_object *wo = calloc(1, sizeof(struct event_object));
    wo->ass = ass;
    wo->stuff = cbw;
    wo->sock = sock;
    wo->ctl_call_event = add_timer_task;
    if (pthread_spin_lock(&ass->clean_mutex) == 0) {
        enqueue(ass->event_q, wo);
        pthread_spin_unlock(&ass->clean_mutex);
    }
    return cbw;
}
void clean_event_queue(async_server_con_set * set)
{
    element_t e, temp;
    pthread_spin_lock(&set->clean_mutex);
    e = dequeue(set->event_q);
    pthread_spin_unlock(&set->clean_mutex);
    while (e != NULL) {
        event_object *wo = (event_object *) e->d;
        wo->ctl_call_event(wo);
        temp = e;
        e = temp->next;
        free(wo);
        pthread_spin_lock(&set->clean_mutex);
        free_element(set->event_q, temp);
        pthread_spin_unlock(&set->clean_mutex);
    }
}
void *run_async_listener(void *arg)
{
    async_server_con_set *ascs = (async_server_con_set *) arg;
    struct epoll_event *events = calloc(512, sizeof(struct epoll_event));
    while (ascs->keep_running) {
        int n, i;
        n = epoll_wait(ascs->efd, events, 512, ascs->time_out);
        clean_event_queue(ascs);
        if (n > 0) {
            for (i = 0; i < n; ++i) {
                if ((events[i].events & EPOLLERR)
                        || (events[i].events & EPOLLHUP)
                        || (!(events[i].events & EPOLLIN)
                            && !(events[i].events & EPOLLOUT))) {
                    call_back_wrapper* cbw = ((call_back_wrapper*)events[i].data.ptr);
                    async_parse_args *pa =
                        rb_tree_get_val(ascs->connections, &cbw->sock);
                    if (pa != NULL) {
                        printf("Got a disconnect event %d \n", cbw->sock); 
                        epoll_ctl(cbw->efd, EPOLL_CTL_DEL,
                                cbw->sock, &events[i]);
                        cbw->clean_up(cbw->stuff);
                    } else {
                        printf("Could not find parse_args for disconnect event %d \n", cbw->sock); 
                    }
                } else if (ascs->num_threads > 2) {
                    call_back_wrapper* cbw = ((call_back_wrapper*)events[i].data.ptr);
                    epoll_ctl(cbw->efd, EPOLL_CTL_DEL, cbw->sock,
                            &events[i]);
                    thr_pool_queue(ascs->thr, ((call_back_wrapper*)events[i].data.ptr)->call_back_func,
                            ((call_back_wrapper*)events[i].data.ptr)->stuff);
                } else {
                    ((call_back_wrapper*)events[i].data.ptr)->call_back_func(
                        ((call_back_wrapper*)events[i].data.ptr)->stuff);
                }
            }
        } else if(ascs->num_threads <= 2) {
            one_stop_pop(ascs->thr);
        }
    }
    int ret = close(ascs->efd);
    if(ret < 0) {
        int e = errno;
        send_debug_message("Error on close: %d %d %d %d \n", e, EINTR, EBADF,EIO);
    }
    free(events); 
    return NULL;
}

void *async_connect_1(void *arg)
{
    int connected = 0;
    struct async_parse_args *pa = (struct async_parse_args *) arg;
    connected = pa->init_con(pa);
    if (connected == -1) {
        disconnect(pa);
    } else {
        int *sock = calloc(sizeof(int), 1);
        if (pthread_spin_lock(&pa->ass->clean_mutex) == 0) {
            *sock = pa->sock;
            rb_tree_no_delete_insert(pa->ass->connections, sock, pa);
            pthread_spin_unlock(&pa->ass->clean_mutex);
        }
        create_recv_epoll_ctl_event(pa);
        send_debug_message("Adding this connection to the tree: %d \n",
                *sock);
        send_debug_message("Connected sucessfully to: %d.\n", *sock);
    }
    return NULL;
}

void init_connection(async_server_con_set * acsm, async_parse_args * pa,
        int sock)
{
    pa->read_wrapper = create_call_back_wrapper(pa,
            pa->async_cb_func,disconnect_wrapper, sock);
    pa->read_wrapper->ee->events = EPOLLIN | EPOLLRDHUP | EPOLLET;
    pa->read_wrapper->efd = acsm->efd;
    pa->ass = acsm;
    int dupper= dup(sock);
    if (dupper == -1) {
        send_debug_message("Failed to dup sock: %d.", sock);
    }
    pa->write_wrapper = create_call_back_wrapper(pa,
            write_master,disconnect_wrapper, dupper);
    pa->write_wrapper->ee->events = EPOLLOUT | EPOLLET; 
    pa->write_wrapper->efd = acsm->efd;
    if(acsm->num_threads > 2) {
        thr_pool_queue(acsm->thr, async_connect_1, pa);
    } else {
        async_connect_1(pa);
    }
}

int async_add_signal(async_server_con_set* acsm)
{
    return 1;
} 

int async_connection(async_server_con_set * acsm,
        const char *mod_name, size_t mod_len, uint16_t port,
        const char *ip, size_t ip_len, int end_hour,
        int end_min, int end_day, size_t timeout_val,
        void *ob,
        void (*connection_callback) (async_parse_args * co,
            int is_connected,
            char *log, int len),
        void (*client_mess_parse) (int *cut_con,
            struct
            async_parse_args *
            pc), void *args,
        int (*init_con) (struct async_parse_args * pa),
        void *write_pass_back,
        void (*write_callback) (const char *name,
                int name_len,
                void *pass_back),
        void (*parse_v) (char *data, long len, void *pc),
        void (*obj_callback) (dart_order_obj * ph, void *ob))
{
    int attempts = 1;
    int ret_code = 1;
    int sock = -1;
    int is_ud = 0;
    if (ip_len > 0 && (memchr(ip, '.', ip_len) == 0)) {
        sock = ud_connect(ip, ip_len);
        is_ud = 1;
    } else {
        sock = async_rec_connection(port, ip, attempts);
    }
    if (sock >= 0) {
        async_set_sock_opt(sock);
        struct async_parse_args *pa =
            create_parse_args(mod_name,
                    mod_len, port, ip, ip_len, end_hour,
                    end_min, end_day, timeout_val, ob,
                    (*connection_callback), (*client_mess_parse),
                    args, (*init_con),
                    sock,
                    write_pass_back, (*write_callback), is_ud,
                    (*parse_v),
                    (*obj_callback));
        pa->is_server = 0;
        pa->is_logged_on = 0;
        pa->gk = acsm->gk;
        pa->ass = acsm;
        //pa->mc = acsm->mc;
        init_connection(acsm, pa, sock);
    } else {
        ret_code = 0;
    }
    return ret_code;
}

void *async_reconnect_t(void *arg)
{
    return async_reconnect((async_parse_args *) arg);
}

void *async_reconnect(async_parse_args * arg)
{
    int attempts = 1;
    int sock = -1;
    if (arg->ip_len > 0 && (memchr(arg->ip, '.', arg->ip_len) == 0)) {
        sock = ud_connect(arg->ip, arg->ip_len);
    } else {
        sock = async_rec_connection(arg->port, arg->ip, attempts);
    }
    if (sock >= 0) {
        sock = async_set_sock_opt(sock);
        arg->sock = sock;
        arg->disconnect = 0;
        if (arg && arg->con->wr_ptr == 0) {
            arg->con->wr_ptr = arg->con->buffer;
            arg->con->rd_ptr = arg->con->buffer;
        }
        init_connection(arg->ass, arg, sock);
    } else {
        arg->connection_notice(arg, 0, NULL, 0);
    }
    return NULL;
}

void add_sender(async_server_con_set * set, void *pass_back, int sock)
{
    event_object *wo = calloc(1, sizeof(struct event_object));
    wo->ass = set;
    wo->stuff = pass_back;
    wo->sock = sock;
    wo->ctl_call_event = async_add_sender;

    if (pthread_spin_lock(&set->clean_mutex) == 0) {
        enqueue(set->event_q, wo);
        pthread_spin_unlock(&set->clean_mutex);
    }
}

static int start_async_server_t(async_server_con_set *ascs, void* ra)
{
    int e_controls = -1;
    ascs->num_threads = ascs->num_threads < 2 ? 2 : ascs->num_threads;
    ascs->thr = thr_pool_create(ascs->num_threads, ascs->num_threads, 0,
            ascs->cpus );
    e_controls = epoll_create1(0);
    ascs->efd = e_controls;
    //Start the recorder first, if we are in the 2 thread mode
    //we can get into a state where one thread records messages and
    //epolls otherwise.
    int ret = 0;
    if(ra) {
        ret = create_thr(ascs->thr, record_messages, ra);
    }
    ret = create_thr(ascs->thr, run_async_listener, ascs);
    return ret;
}
void join_n_clean(async_server_con_set* ass)
{
    thr_pool_wait_n_join(ass->thr);
    rb_tree_destroy(ass->connections);
    delete_map(ass->parse_args);
    destroy_queue(ass->event_q);
}

async_server_con_set
* start_async_server(gatekeeper * mb,  void* ra, 
        int num_threads, int time_out,
        int *cpus, int depth)
{

    struct async_server *scs = calloc(sizeof(struct async_server), 1);
    pthread_spin_init(&scs->clean_mutex, PTHREAD_PROCESS_PRIVATE);
    scs->time_out = time_out;
    scs->num_threads = num_threads;
    scs->cpus = cpus;
    scs->gk = mb;
    scs->keep_running = 1;
    scs->event_q = create_sized_queue(depth);
    scs->connections =
        create_rb_tree(async_int_compare, async_int_destroy,
                async_info_destroy, async_int_print,
                async_info_print);
    scs->parse_args = create_map(depth);
    start_async_server_t(scs, ra);
    return scs;
}
int add_async_listener(async_server_con_set* scs,gatekeeper * mb, master_config * mc,
        const char *mod_name, size_t mod_len,
        uint16_t port, queue_t l_args, const char *ip,
        size_t len, int end_hour, int end_min,
        int end_day, size_t timeout_val,
        void (*connection_callback) (async_parse_args * co,
            int is_connected,
            char *log, int len),
        void (*mess_parse) (int *cut_con,
            struct async_parse_args *
            pc),
        void *write_pass_back,
        void (*write_callback) (const char *name,
            int name_len,
            void *pass_back),
        void (*parse_v) (char *data, long len, void *pc),
        void (*obj_callback) (dart_order_obj * ph,
                void *ob)
        )


{

    struct async_parse_args *pa = calloc(sizeof(struct async_parse_args), 1);
    pa->connection_notice = connection_callback;
    pa->is_logged_on = 0;
    pa->in_args_list = l_args;
    pa->in_args = NULL;
    pa->is_server = 1;
    pa->parse_v = parse_v;
    pa->obj_callback = obj_callback;
    pa->gk = mb;
    pa->obj_callback = obj_callback;
    pa->ds_book = NULL;
    pa->parse = (*mess_parse);
    //pa->seq_parse = (*seq_parse);
    pa->gk = mb;
    pa->con = create_sock_recv_manager();
    pa->write_callback = write_callback;
    pa->write_pass_back = write_pass_back;
    pa->ass = scs;
    pa->sock = -1;
    if (len > 0 && (memchr(ip, '.', len) == 0)) {
        pa->sock = ud_create_and_bind(ip, len);
        pa->async_cb_func = ud_listen_call_back_func;
    } else {
        pa->sock = create_and_bind(port);
        pa->async_cb_func = listen_call_back_func;
    }
    int ret_code = listen(pa->sock, MAXCONNECTIONS);
    if (ret_code == -1 || scs->efd == -1) {
        send_debug_message("Listen Failed in Async Listener. %d\n",
                ret_code);
        return -1;
    }
    pa->write_wrapper = create_call_back_wrapper(pa,
            write_master, disconnect_wrapper, pa->sock);
    pa->read_wrapper = create_call_back_wrapper(pa,
            pa->async_cb_func, disconnect_wrapper, pa->sock);
    pa->read_wrapper->ee->events = EPOLLIN | EPOLLRDHUP | EPOLLET; //EPOLLONESHOT;
    pa->read_wrapper->efd = scs->efd;
    if (pthread_spin_lock(&scs->clean_mutex) == 0) {
        int *in_sock = calloc(sizeof(int), 1);
        *in_sock = pa->sock;
        rb_tree_no_delete_insert(scs->connections, in_sock, pa);
        pthread_spin_unlock(&scs->clean_mutex);
    }
    create_recv_epoll_ctl_event(pa);
    return 1;
}
