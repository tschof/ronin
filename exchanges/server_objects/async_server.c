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
#include <sys/un.h>

#include "dart_thr_pool.h"
#include "client_manager.h"
#include "dart_connection_object.h"
#include "rb_tree.h"
#include "debug_logger.h"
#include "dbg_helper.h"
#include "transaction_recorder.h"

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
    //do nothing.
}


typedef struct async_server {
    struct async_parse_args *pa;
    pthread_mutex_t clean_mutex;
    struct epoll_event *events;
    int num_threads;
    int time_out;
    thr_pool_t *thr;
    rb_tree_t *connections;
    int *cpus;
    int set_affinity;
    int efd;
    queue_t event_q;
} async_server;

typedef struct event_object {
    void * stuff;
    int sock;
    async_server* ass;
    void (*ctl_call_event) (void* s);
} event_object;

static void async_close_socket(int sock)
{
    int err = 0;
    shutdown(sock, SHUT_RDWR);
    err = close(sock);
}

int async_set_sock_opt(int sock)
{
    int on = 1;
    int buffer = 16384;
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

static void realloc_receive(struct async_parse_args *pa)
{
    char *temp;
    pa->total_size += SOCK_READ_SIZE;
    temp = calloc(pa->total_size + 1, 1);
    if (temp == NULL) {
        return;
    }
    memcpy(temp, pa->rd_ptr, (pa->write_offset - pa->read_offset));
    free(pa->buffer);
    pa->buffer = 0;
    pa->buffer = temp;
    pa->write_offset = (pa->write_offset - pa->read_offset);
    pa->read_offset = 0;
    pa->wr_ptr = pa->buffer;
    pa->wr_ptr += pa->write_offset;
    pa->rd_ptr = pa->buffer;
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
    con = connect(sock, res->ai_addr, res->ai_addrlen);
    err = errno;
    if (con == 0 || err == EALREADY || err == EINPROGRESS || err == EAGAIN
        || err == EINTR) {
        freeaddrinfo(res);
        async_set_sock_opt(sock);
        return sock;
    } else if ((err == ETIMEDOUT || err == ECONNREFUSED) && (i > 0)) {
        async_close_socket(sock);
        freeaddrinfo(res);
        return async_rec_connection(port, host, --i);
    } else {
        send_debug_message
            ("Fatal socket error: %d, killing connection after %d attempts!\n",
             err, i);
        freeaddrinfo(res);
        async_close_socket(sock);
        return -1;
    }
}

static int ud_connect(const char *ip, int ip_len)
{
    int sock = -1;
    struct sockaddr_un addr;
    int flags = O_NONBLOCK;
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
    flags = fcntl(sock, F_SETFL, O_NONBLOCK);
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
        return -1;
    }
    return listenfd;
}

static int ud_create_and_bind(const char *ip, int ip_len)
{
    int sock = -1;
    int flags = O_NONBLOCK;
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
    flags = fcntl(sock, F_SETFL, O_NONBLOCK);
    return sock;
}

static void *write_master(void *w)
{
    struct async_parse_args *pa = (struct async_parse_args *) w;
    pa->write_callback(get_name(pa->con->co), get_name_len(pa->con->co),
                       pa->write_pass_back);
    return NULL;
}


static void master_disconnect(async_server_con_set * set,
                              async_parse_args * pa, int s)
{
    if (pthread_mutex_lock(&pa->ass->clean_mutex) == 0) {
        epoll_ctl(set->efd, EPOLL_CTL_DEL, pa->asw->sock, pa->asw->ee);
        epoll_ctl(set->efd, EPOLL_CTL_DEL,
                  get_sock(pa->con->co), set->events);
        int dupe = pa->asw->sock;
        rb_tree_delete(pa->ass->connections, &s);
        async_close_socket(s);
        async_close_socket(dupe);
        pa->con->is_logged_on = 0;
        if (get_is_removed(pa->con->co) == 0) {
            remove_connection(pa->con->co);
        }
        pthread_mutex_unlock(&pa->ass->clean_mutex);
        if (get_should_notify(pa->con->co)) {
            pa->con->connection_notice(pa->con->co, 0, NULL, 0);
        }
        destroy_con_obj(pa->con->co);
        pa->con->co = 0;
        destroy_parse_con(pa->con);
        if (pa->buffer) {
            free(pa->buffer);
            pa->buffer = 0;
        }
        if(pa->asw) {
            if(pa->asw->ee) {
                free(pa->asw->ee);
            }
            free(pa->asw);
        }
        if(pa->ee) {
            free(pa->ee);
        }

        free(pa);
        pa = 0;
    }
}
static void listner_async_disconnect(void* k)
{
    event_object* eo = (event_object*)k;
    if (pthread_mutex_lock(&eo->ass->clean_mutex) == 0) {
        async_parse_args *pa = (struct async_parse_args*)eo->stuff;
        pthread_mutex_unlock(&eo->ass->clean_mutex);
        if(pa) {
            master_disconnect(eo->ass, pa, eo->sock);
        }
    }
}

void remove_listener(async_server_con_set* set,
                     async_parse_args* pa) 
{
    event_object* wo = calloc(1, sizeof(struct event_object));
    wo->ass = set;
    wo->stuff = pa;
    wo->sock = get_sock(pa->con->co);
    wo->ctl_call_event = listner_async_disconnect;
    if (pthread_mutex_lock(&set->clean_mutex) == 0) {
        enqueue(set->event_q, wo);
        pthread_mutex_unlock(&set->clean_mutex);
    }
}

static void async_disconnect(void* k)
{
    event_object* eo = (event_object*)k;
    if (pthread_mutex_lock(&eo->ass->clean_mutex) == 0) {
        async_parse_args *pa = rb_tree_get_val(eo->ass->connections, &eo->sock);
        pthread_mutex_unlock(&eo->ass->clean_mutex);
        if(pa) {
            master_disconnect(eo->ass, pa, eo->sock);
        }
    }
}

void disconnect_by_name(async_server_con_set * set, const char *name,
                        int name_len, int sock)
{
    event_object* wo = calloc(1, sizeof(struct event_object));
    wo->ass = set;
    wo->stuff = (char*)name;
    wo->sock = sock;
    wo->ctl_call_event = async_disconnect;
    if (pthread_mutex_lock(&set->clean_mutex) == 0) {
        enqueue(set->event_q, wo);
        pthread_mutex_unlock(&set->clean_mutex);
    }
}

static void async_add_back(void* k)
{
    event_object* eo = (event_object*)k;
    if (pthread_mutex_lock(&eo->ass->clean_mutex) == 0) {
        async_parse_args *pa = rb_tree_get_val(eo->ass->connections, &eo->sock);
        if (pa != NULL) {
            int ret_code = 0;
            ret_code =
                epoll_ctl(pa->efd, EPOLL_CTL_ADD, get_sock(pa->con->co),
                          pa->ee);
            if (ret_code == -1) {
                int err = errno;
                if (err == ENOENT) {
                    ret_code =
                        epoll_ctl(pa->efd, EPOLL_CTL_MOD, get_sock(pa->con->co),
                                  pa->ee);
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
        pthread_mutex_unlock(&eo->ass->clean_mutex);
    }
}

void create_recv_epoll_ctl_event(async_parse_args* pa)
{
    event_object* wo = calloc(1, sizeof(struct event_object));
    wo->ass = pa->ass;
    wo->stuff = pa;
    wo->sock = get_sock(pa->con->co);
    wo->ctl_call_event = async_add_back;
    if (pthread_mutex_lock(&pa->ass->clean_mutex) == 0) {
        enqueue(pa->ass->event_q, wo);
        pthread_mutex_unlock(&pa->ass->clean_mutex);
    }
}

/**
 * This needs to be altered so that we check username and password.
 */
static void find_n_replace_args(const char *name, int name_len,
                                       queue_t in_args, 
                                       struct init_args* new_init)
{
    if (name != NULL && name_len > 0 && in_args != NULL) {
        element_t e;
        e = front(in_args);
        while (e != NULL) {
            struct init_args *init;
            init = (struct init_args *) e->d;
            if (init != NULL && name_len > 0 &&
                strncmp(name, init->name, name_len) == 0) {
                //Memory leak, fixed in all in one.  We do not know
                //who has this old init arg they will clean up on 
                //destruction.
                e->d = new_init;
                break;
            } else {
                e = e->next;
            }
        }
    } else {
        send_debug_message("Name was null or name_len was 0 or less.\n");
    }
}
void add_new_user(async_server_con_set* set, struct async_parse_args* pa, const char* name,
                  int name_len, struct init_args* in)
{
    if(pthread_mutex_lock(&set->clean_mutex) == 0) {
        struct init_args* init =  copy_init_args(in);
        enqueue(pa->con->in_args_list, init);
        pthread_mutex_unlock(&set->clean_mutex);
    }
}

void alter_user(async_server_con_set* set, struct async_parse_args* pa,
                const char* name, int name_len,
                struct init_args* in)
{
    if(pthread_mutex_lock(&set->clean_mutex) == 0) {
        struct init_args* init =  copy_init_args(in);
        find_n_replace_args(name, name_len, pa->con->in_args_list, init); 
        pthread_mutex_unlock(&set->clean_mutex);
    }
}
static void *raw_sock_parser(void *p_args)
{
    long num_bytes = 0;
    struct async_parse_args *pa = (struct async_parse_args *) p_args;
    int run = 1;
    int cut_connection = 0;
    while (run) {
        num_bytes =
            recv(get_sock(pa->con->co), pa->wr_ptr,
                 (pa->total_size - pa->write_offset), 0);
        if (num_bytes > 0) {
            unsigned long bytes_read = (unsigned long) num_bytes;
            unsigned long total_bytes_read =
                bytes_read + (pa->write_offset - pa->read_offset);
            unsigned long bytes_used =
                pa->parse(pa->rd_ptr, total_bytes_read, &cut_connection,
                          pa->con);
            if (cut_connection || get_disconnect(pa->con->co)) {
                send_debug_message("Parser told us to Cut Connection!!");
                pa->wr_ptr = 0;
                pa->rd_ptr = 0;
                run = 0;
                cut_connection = 1;
                break;
            }
            if (bytes_used >= total_bytes_read) {
                pa->write_offset = 0;
                pa->read_offset = 0;
                pa->wr_ptr = pa->buffer;
                pa->rd_ptr = pa->buffer;
            } else {
                pa->read_offset += bytes_used;
                pa->write_offset += bytes_read;
                if (pa->read_offset >= pa->total_size ||
                    pa->write_offset >= pa->total_size) {
                    pa->rd_ptr += bytes_used;
                    realloc_receive(pa);
                    if (pa->buffer == NULL) {
                        int s = get_sock(pa->con->co);
                        send_debug_message
                            ("Removing this connection from tree "
                             "realloc failed: %d \n", s);
                        run = 0;
                        pa->wr_ptr = 0;
                        pa->rd_ptr = 0;
                        cut_connection = 1;
                        break;
                    }
                } else {
                    pa->rd_ptr = pa->rd_ptr + bytes_used;
                    pa->wr_ptr = pa->wr_ptr + bytes_read;
                }
            }
        } else if (num_bytes == -1) {
            int e = errno;
            if (e != EAGAIN) {
                run = 0;
                cut_connection = 1;
                send_debug_message("Error on read: %d \n", e);
            }
            break;
        } else {
            run = 0;
            pa->wr_ptr = 0;
            pa->rd_ptr = 0;
            run = 0;
            cut_connection = 1;
            break;
        }
    }
    if (cut_connection) {
        int s = get_sock(pa->con->co);
        send_debug_message("Removing this connection from tree: %d \n", s);
        event_object* wo = calloc(1, sizeof(struct event_object));
        wo->ass = pa->ass;
        wo->stuff = pa;
        wo->sock = s;
        wo->ctl_call_event = async_disconnect;
        if (pthread_mutex_lock(&pa->ass->clean_mutex) == 0) {
            enqueue(pa->ass->event_q, wo);
            pthread_mutex_unlock(&pa->ass->clean_mutex);
        }
    } else {
        create_recv_epoll_ctl_event(pa);
    }
    return NULL;
}

static struct async_parse_args
*create_parse_args(void (*client_callback)
                    (struct message * m, void *ob),
                   const char *mod_name, size_t mod_len, uint16_t port,
                   const char *ip, size_t ip_len, int end_hour,
                   int end_min, int end_day, size_t timeout_val,
                   void *ob, void (*connection_callback) (con_obj * co,
                                                          int
                                                          is_connected,
                                                          char *log,
                                                          int len),
                   unsigned long (*client_mess_parse) (const char
                                                       *message,
                                                       unsigned long
                                                       len,
                                                       int *cut_con,
                                                       struct parse_con
                                                       * pc),
                   void *args,
                   int (*init_con) (struct async_parse_args * pa),
                   unsigned long (*seq_parse) (char *data,
                                               unsigned long size,
                                               unsigned long bo,
                                               unsigned long *seq),
                   void (*low_activity_pulse) (con_obj * obj),
                   int check_activity, int sock, void *write_pass_back,
                   void (*write_callback) (const char *name, int name_len,
                                           void *pass_back), int is_ud,
                   void *parser_helper,
                   void *(*copy_parser_helper) (void *ph),
                   void (*p_callback) (void *ph, struct message * m,
                                       void *ob))
{
    struct parse_con *pcon = malloc(sizeof(struct parse_con));
    struct app_end_time *t = malloc(sizeof(struct app_end_time));
    struct async_parse_args *pa = malloc(sizeof(struct async_parse_args));
    queue_t in_q = create_sized_queue(80);
    con_obj *co = 0;
    sock = async_set_sock_opt(sock);
    t->end_hour = end_hour;
    t->end_min = end_min;
    t->end_day = end_day;
    pcon->send_ptr = send_message;
    pcon->callback = (*client_callback);
    pcon->record_mess = record_inbound_message;
    pcon->init_con = add_connection;
    pcon->remove_con = remove_connection;
    pcon->connection_notice = connection_callback;
    pcon->timeout_val = timeout_val;
    pcon->is_logged_on = 0;
    pcon->book = ob;
    pcon->t = t;
    pcon->parser_helper = parser_helper;
    pcon->copy_parser_helper = copy_parser_helper;
    pcon->p_callback = p_callback;
    if (mod_len > 0 && mod_len < 100) {
        pcon->mod_name = calloc(mod_len + 1, 1);
        memcpy(pcon->mod_name, mod_name, mod_len);
        pcon->mod_len = mod_len;
    } else {
        pcon->mod_len = 0;
        pcon->mod_name = NULL;
    }
    enqueue(in_q, (struct init_args *) args);
    pcon->in_args_list = in_q;
    pcon->in_args = (struct init_args *) args;
    co = create_con_obj(pcon->in_args->name, pcon->in_args->name_len, sock,
                        0, 1, 1);
    if (co == NULL) {
        free(pcon);
        return 0;
    }
    pcon->co = co;
    if (pa == NULL) {
        destroy_con_obj(co);
        free(pcon);
        return 0;
    }
    pa->parse = (*client_mess_parse);
    pa->seq_parse = (*seq_parse);
    pa->init_con = init_con;
    pa->low_activity_pulse = (*low_activity_pulse);
    pa->t = t;
    pa->con = pcon;
    pa->check_activity = check_activity;
    pa->buffer = calloc(BUFFER_SIZE + 1, 1);
    pa->wr_ptr = pa->buffer;
    pa->rd_ptr = pa->buffer;
    pa->write_offset = 0;
    pa->read_offset = 0;
    pa->total_size = BUFFER_SIZE;
    pa->write_callback = write_callback;
    pa->write_pass_back = write_pass_back;
    pa->async_cb_func = raw_sock_parser;
    add_config(pcon->in_args);
    return pa;
}

static struct async_parse_args *clone_parse_args(struct async_parse_args
                                                 *pa, int newSock)
{
    struct async_parse_args *lpa;
    struct parse_con *pcon;
    con_obj *co;
    send_debug_message("New sock accepted: {%d} \n", newSock);
    newSock = async_set_sock_opt(newSock);
    lpa = malloc(sizeof(struct async_parse_args));
    pcon = malloc(sizeof(struct parse_con));
    if (pcon != NULL && lpa != NULL) {
        char *name = (char *) get_name(pa->con->co);
        lpa->parse = pa->parse;
        pcon->send_ptr = pa->con->send_ptr;
        pcon->callback = pa->con->callback;
        pcon->record_mess = pa->con->record_mess;
        pcon->init_con = pa->con->init_con;
        pcon->remove_con = pa->con->remove_con;
        pcon->connection_notice = pa->con->connection_notice;
        pcon->timeout_val = pa->con->timeout_val;
        pcon->in_args_list = pa->con->in_args_list;
        pcon->in_args = NULL;
        pcon->t = pa->t;
        pcon->is_server = 1;
        pcon->is_logged_on = 0;
        pcon->copy_parser_helper = pa->con->copy_parser_helper;
        pcon->p_callback = pa->con->p_callback;
        if (pa->con->parser_helper != NULL) {
            pcon->parser_helper =
                pa->con->copy_parser_helper(pa->con->parser_helper);
        } else {
            pcon->parser_helper = NULL;
        }
        co = create_con_obj(name,
                            get_name_len(pa->con->co), newSock, 0, 1, 1);
        pcon->co = co;
        pcon->book = pa->con->book;
        if (pa->con->mod_len > 0 && pa->con->mod_len < 100) {
            pcon->mod_name = calloc(pa->con->mod_len + 1, 1);
            memcpy(pcon->mod_name, pa->con->mod_name, pa->con->mod_len);
            pcon->mod_len = pa->con->mod_len;
        } else {
            pcon->mod_len = 0;
            pcon->mod_name = NULL;
        }
        lpa->con = pcon;
        lpa->low_activity_pulse = pa->low_activity_pulse;
        lpa->check_activity = pa->check_activity;
        lpa->init_con = pa->init_con;
        lpa->t = malloc(sizeof(struct app_end_time));//pa->t;
        lpa->t->end_hour = pa->t->end_hour;
        lpa->t->end_min = pa->t->end_min;
        lpa->t->end_day = pa->t->end_day;
        lpa->buffer = calloc(BUFFER_SIZE + 1, 1);
        lpa->wr_ptr = lpa->buffer;
        lpa->rd_ptr = lpa->buffer;
        lpa->write_offset = 0;
        lpa->read_offset = 0;
        lpa->total_size = BUFFER_SIZE;
        lpa->async_cb_func = raw_sock_parser;
        lpa->write_callback = pa->write_callback;
        lpa->write_pass_back = pa->write_pass_back;
        free(name);
    }
    return lpa;
}

static int time_to_close(struct async_server *vals)
{
    time_t long_time;
    struct tm *newtime;
    struct tm lm;
    int ret_val = 0;
    time(&long_time);
    newtime = localtime_r(&long_time, &lm);
    newtime = &lm;
    if (newtime->tm_hour == vals->pa->t->end_hour
        && newtime->tm_min >= vals->pa->t->end_min
        && newtime->tm_min < (vals->pa->t->end_min + 5)
        && ((newtime->tm_wday == vals->pa->t->end_day)
            || (vals->pa->t->end_day == 7))) {
        send_debug_message
            ("Time to end Server Socket: hour: %d min: %d!!\n",
             vals->pa->t->end_hour, vals->pa->t->end_min);
        ret_val = 1;
    }
    return ret_val;
}

static void create_new_conversation(async_parse_args * ascs, int sock,
                                    struct epoll_event *le)
{
    struct async_parse_args *pa = 0;
    int *in_sock = malloc(sizeof(int));
    pa = clone_parse_args(ascs, sock);
    le->data.ptr = pa;
    le->events = EPOLLIN | EPOLLRDHUP | EPOLLET;//EPOLLONESHOT;
    pa->ee = le;
    pa->efd = ascs->ass->efd;
    pa->ass = ascs->ass;
    pa->asw = malloc(sizeof(struct async_writer));
    pa->asw->sock = dup(sock);
    if (pa->asw->sock == -1) {
        send_debug_message("Failed to dup sock:  %d.", sock);
    }
    pa->asw->ee = calloc(1, sizeof(struct epoll_event));
    pa->asw->ee->events = EPOLLOUT | EPOLLET; //EPOLLONESHOT;
    pa->asw->ee->data.ptr = pa;
    pa->asw->efd = ascs->ass->efd;
    *in_sock = sock;
    if (pthread_mutex_lock(&ascs->ass->clean_mutex) == 0) {
        rb_tree_no_delete_insert(ascs->ass->connections, in_sock, pa);
        pthread_mutex_unlock(&ascs->ass->clean_mutex);
    }
    create_recv_epoll_ctl_event(pa);
    send_debug_message("Adding this connection to the tree: %d \n", sock);
}
void* ud_listen_call_back_func(void* p_args)
{
    struct async_parse_args* pa = (struct async_parse_args*)p_args;
    int listenfd = get_sock(pa->con->co);
    while (1) {
        int sock = -1;
        int flags = O_NONBLOCK;
        if ((sock = accept(listenfd, NULL, NULL)) == -1) {
            break;
        }
        flags = fcntl(sock, F_SETFL, O_NONBLOCK);
        struct epoll_event *le =
            calloc(1, sizeof(struct epoll_event));
        send_debug_message
            ("Accepted connection: sock %d, on the ud_connection.\n",
             sock);
        async_set_sock_opt(sock);
        create_new_conversation(pa, sock, le);
    }
    create_recv_epoll_ctl_event(pa);
    return NULL;
}

void* listen_call_back_func(void* p_args)
{
    struct async_parse_args* pa = (struct async_parse_args*)p_args;
    int listenfd = get_sock(pa->con->co);
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
        struct epoll_event *le =
            calloc(1, sizeof(struct epoll_event));
        int ret_code =
            getnameinfo(&in_addr, in_len, hbuf,
                        sizeof(hbuf), sbuf, sizeof(sbuf),
                        NI_NUMERICHOST | NI_NUMERICSERV);
        if(ret_code == 0) {
            send_debug_message
                ("Accepted connection: sock %d, host = %s, port = %s\n",
                 sock, hbuf, sbuf);
        }
        ret_code = async_set_sock_opt(sock);
        create_new_conversation(pa, sock, le);
    }
    create_recv_epoll_ctl_event(pa);
    return NULL;
}

void *run_async_listener(void *arg)
{
    async_server_con_set *ascs = (async_server_con_set *) arg;
    int ret_code = 0;
    int keep_running = 1;
    ascs->events = calloc(1001, sizeof(struct epoll_event));
    if (ret_code == -1 || ascs->efd == -1) {
        send_debug_message("Listen Failed in Async Listener. %d\n",
                           ret_code);
    }
    while (keep_running) {
        int n, i;
        n = epoll_wait(ascs->efd, ascs->events, 1000, ascs->time_out);
        clean_write_event_queue(ascs);
        if (n <= 0) {
            if (time_to_close(ascs)) {
                send_debug_message("Time to close all bets off!!! \n");
                keep_running = 0;
            }
        } else {
            for (i = 0; i < n; ++i) {
                struct async_parse_args *epa =
                    (struct async_parse_args *) ascs->events[i].data.ptr;
                if ((ascs->events[i].events & EPOLLERR)
                    || (ascs->events[i].events & EPOLLHUP)
                    || (!(ascs->events[i].events & EPOLLIN)
                        && !(ascs->events[i].events & EPOLLOUT))) {
                    if (epa->con->co != 0) {
                        int sock = get_sock(epa->con->co);
                        master_disconnect(ascs, epa, sock);
                    } else {
                        break;
                    }
                } else {
                    if ((ascs->events[i].events & EPOLLOUT)) {
                        epoll_ctl(ascs->efd, EPOLL_CTL_DEL, 
                                  epa->asw->sock, &ascs->events[i]);
                        thr_pool_queue(ascs->thr,
                                       write_master,
                                       ascs->events[i].data.ptr);
                    }
                    if ((ascs->events[i].events & EPOLLIN)) {
                        epoll_ctl(ascs->efd, EPOLL_CTL_DEL, 
                                  get_sock(epa->con->co), &ascs->events[i]);
                        thr_pool_queue(ascs->thr,
                                       ((struct async_parse_args *)
                                        ascs->events[i].data.
                                        ptr)->async_cb_func,
                                       ascs->events[i].data.ptr);
                    }
                }
            }
        }
    }
    return NULL;
}

void *async_connect_1(void *arg)
{
    int connected = 0;
    struct async_parse_args *pa = (struct async_parse_args *) arg;
    connected = pa->init_con(pa);
    if (!connected) {
        async_close_socket(get_sock(pa->con->co));
        async_close_socket(pa->asw->sock);
        if (get_is_removed(pa->con->co) == 0) {
            remove_connection(pa->con->co);
        }
        pa->con->connection_notice(pa->con->co, 0, NULL, 0);

        if (pthread_mutex_lock(&pa->ass->clean_mutex) == 0) {
            destroy_con_obj(pa->con->co);
            destroy_parse_con(pa->con);
            free(pa->buffer);
            pthread_mutex_unlock(&pa->ass->clean_mutex);
            free(pa);
        }
    } else {
        int *sock = malloc(sizeof(int));
        if (pthread_mutex_lock(&pa->ass->clean_mutex) == 0) {
            *sock = get_sock(pa->con->co);
            rb_tree_no_delete_insert(pa->ass->connections, sock, pa);
            pthread_mutex_unlock(&pa->ass->clean_mutex);
        }
        send_debug_message("Adding this connection to the tree: %d \n",
                           *sock);
        create_recv_epoll_ctl_event(pa);
    }
    return NULL;
}

int async_connection(void *a_server_set,
                     void (*client_callback) (struct message * m,
                                              void *ob),
                     const char *mod_name, size_t mod_len, uint16_t port,
                     const char *ip, size_t ip_len, int end_hour,
                     int end_min, int end_day, size_t timeout_val,
                     void *ob, void (*connection_callback) (con_obj * co,
                                                            int
                                                            is_connected,
                                                            char *log,
                                                            int len),
                     unsigned long (*client_mess_parse) (const char
                                                         *message,
                                                         unsigned long len,
                                                         int *cut_con,
                                                         struct parse_con *
                                                         pc), void *args,
                     int (*init_con) (struct async_parse_args * pa),
                     unsigned long (*seq_parse) (char *data,
                                                 unsigned long size,
                                                 unsigned long bo,
                                                 unsigned long *seq),
                     void (*low_activity_pulse) (con_obj * obj),
                     int check_activity, void *write_pass_back,
                     void (*write_callback) (const char *name,
                                             int name_len,
                                             void *pass_back),
                     void *parser_helper,
                     void *(*copy_parser_helper) (void *ph),
                     void (*p_callback) (void *ph, struct message * m,
                                         void *ob))
{
    int attempts = 1;
    int ret_code = 1;
    int sock = -1;
    int is_ud = 0;
    async_server_con_set *acsm = (async_server_con_set *) a_server_set;
    if (ip_len > 0 && (memchr(ip, '.', ip_len) == 0)) {
        sock = ud_connect(ip, ip_len);
        is_ud = 1;
    } else {
        sock = async_rec_connection(port, ip, attempts);
    }
    if (sock >= 0) {
        struct async_parse_args *pa =
            create_parse_args((*client_callback), mod_name,
                              mod_len, port, ip, ip_len, end_hour,
                              end_min, end_day, timeout_val, ob,
                              (*connection_callback), (*client_mess_parse),
                              args, (*init_con), (*seq_parse),
                              (*low_activity_pulse), check_activity, sock,
                              write_pass_back, (*write_callback), is_ud,
                              parser_helper, (*copy_parser_helper),
                              (*p_callback));
        pa->con->is_server = 0;
        pa->con->is_logged_on = 0;
        pa->ee = calloc(1, sizeof(struct epoll_event));
        pa->ee->data.ptr = pa;
        pa->ee->events = EPOLLIN | EPOLLRDHUP | EPOLLET; //EPOLLONESHOT;
        pa->efd = acsm->efd;
        pa->ass = acsm;
        pa->asw = malloc(sizeof(struct async_writer));
        pa->asw->sock = dup(sock);
        if (pa->asw->sock == -1) {
            send_debug_message("Failed to dup sock: %d.", sock);
        }
        pa->asw->ee = calloc(1, sizeof(struct epoll_event));
        pa->asw->ee->events = EPOLLOUT | EPOLLET; //EPOLLONESHOT;
        pa->asw->ee->data.ptr = pa;
        pa->asw->efd = acsm->efd;
        pa->use_ud = is_ud;
        thr_pool_queue(acsm->thr, async_connect_1, pa);
    } else {
        con_obj *co;
        char *name = (((struct init_args *) args)->name);
        send_debug_message
            ("Fatal socket error: %d, killing connection %s!\n", sock,
             name);
        co = create_con_obj(name, (((struct init_args *) args)->name_len),
                            0, 0, 1, 1);
        connection_callback(co, 0, NULL, 0);
        destroy_con_obj(co);
        ret_code = -1;
    }
    return ret_code;
}

static void async_add_sender(void* k)
{
    struct event_object* ev = (struct event_object*)k;
    if (pthread_mutex_lock(&ev->ass->clean_mutex) == 0) {
        async_parse_args *pa = rb_tree_get_val(ev->ass->connections, &ev->sock);
        if (pa != NULL) {
            int ret_code = 0;
            pa->write_pass_back = ev->stuff;
            ret_code =
                epoll_ctl(pa->asw->efd, EPOLL_CTL_ADD, pa->asw->sock,
                          pa->asw->ee);
            if (ret_code == -1) {
                int err = errno;
                if (err == EEXIST) {
                    ret_code =
                        epoll_ctl(pa->asw->efd, EPOLL_CTL_MOD,
                                  pa->asw->sock, pa->asw->ee);
                    if (ret_code == -1) {
                        err = errno;
                    }
                }
            }
        } else {
            send_debug_message
                ("Unable to find this connection for write events: %d!\n",
                 ev->sock);
            rb_tree_print(ev->ass->connections);
        }
        pthread_mutex_unlock(&ev->ass->clean_mutex);
    }
}

void clean_write_event_queue(async_server_con_set* set)
{
    element_t e, temp;
    pthread_mutex_lock(&set->clean_mutex);
    e = dequeue(set->event_q);
    pthread_mutex_unlock(&set->clean_mutex);
    while (e != NULL) {
        event_object* wo = (event_object*)e->d;
        wo->ctl_call_event(wo);
        temp = e;
        e = temp->next;
        free(wo);
        pthread_mutex_lock(&set->clean_mutex);
        free_element(set->event_q, temp);
        pthread_mutex_unlock(&set->clean_mutex);
    }
}

void add_sender(async_server_con_set * set, void *pass_back, int sock)
{
    event_object* wo = calloc(1, sizeof(struct event_object));
    wo->ass = set;
    wo->stuff = pass_back;
    wo->sock = sock;
    wo->ctl_call_event = async_add_sender;
     
    if (pthread_mutex_lock(&set->clean_mutex) == 0) {
        enqueue(set->event_q, wo);
        pthread_mutex_unlock(&set->clean_mutex);
    }
}

void add_job_to_queue(async_server_con_set* ascs, void *(*func) (void *), void *arg)
{
    thr_pool_queue(ascs->thr, func, arg);
}

static int start_async_server_t(void *server_con_set)
{
    int e_controls = -1;
    async_server_con_set *ascs = (async_server_con_set *) server_con_set;
    ascs->num_threads = ascs->num_threads < 1 ? 1 : ascs->num_threads;
    ascs->thr = thr_pool_create(ascs->num_threads, ascs->num_threads, 0,
                                ascs->cpus, ascs->set_affinity);
    e_controls = epoll_create1(0);
    ascs->efd = e_controls;
    thr_pool_queue(ascs->thr, run_async_listener, ascs);
    return 0;
}

async_server_con_set
    * start_async_server(void (*fn_ptr) (struct message * m, void *ob),
                         const char *mod_name, size_t mod_len,
                         uint16_t port, queue_t l_args, const char *ip,
                         size_t len, int end_hour, int end_min,
                         int end_day, size_t timeout_val, void *ob,
                         void (*connection_callback) (con_obj * co,
                                                      int is_connected,
                                                      char *log, int len),
                         unsigned long (*mess_parse) (const char *message,
                                                      unsigned long len,
                                                      int *cut_con,
                                                      struct parse_con *
                                                      pc),
                         unsigned long (*seq_parse) (char *data,
                                                     unsigned long size,
                                                     unsigned long bo,
                                                     unsigned long *seq),
                         void (*low_activity_pulse) (con_obj * obj),
                         char *(*create_reset_pad) (unsigned long seq,
                                                    char *ver,
                                                    size_t * len),
                         int check_activity, int num_threads, int time_out,
                         int *cpus, int set_affinity,
                         void *write_pass_back,
                         void (*write_callback) (const char *name,
                                                 int name_len,
                                                 void *pass_back),
                         void *parser_helper,
                         void *(*copy_parser_helper) (void *ph),
                         void (*p_callback) (void *ph, struct message * m,
                                             void *ob))
{

    struct parse_con *pcon = malloc(sizeof(struct parse_con));
    struct app_end_time *t = malloc(sizeof(struct app_end_time));
    struct async_parse_args *pa = malloc(sizeof(struct async_parse_args));
    con_obj *co = create_empty_con_obj();
    struct async_server *scs = malloc(sizeof(struct async_server));
    t->end_hour = end_hour;
    t->end_min = end_min;
    t->end_day = end_day;
    pcon->send_ptr = send_message;
    pcon->callback = (*fn_ptr);
    pcon->record_mess = record_inbound_message;
    pcon->init_con = add_connection;
    pcon->remove_con = remove_connection;
    pcon->connection_notice = connection_callback;
    pcon->timeout_val = timeout_val;
    pcon->is_logged_on = 0;
    pcon->in_args_list = l_args;
    pcon->in_args = NULL;
    pcon->is_server = 1;
    pcon->t = t;
    pcon->parser_helper = parser_helper;
    pcon->copy_parser_helper = copy_parser_helper;
    pcon->p_callback = p_callback;
    if (mod_len > 0 && mod_len < 100) {
        pcon->mod_name = calloc(mod_len + 1, 1);
        memcpy(pcon->mod_name, mod_name, mod_len);
        pcon->mod_len = mod_len;
    } else {
        pcon->mod_len = 0;
        pcon->mod_name = NULL;
    }
    if (co == NULL) {
        free(pcon);
        return NULL;
    }
    pcon->book = ob;
    pcon->co = co;

    if (pcon == NULL) {
        return NULL;
    }
    if (pa == NULL) {
        destroy_con_obj(co);
        free(pcon);
        return NULL;
    }
    pa->parse = (*mess_parse);
    pa->seq_parse = (*seq_parse);
    pa->low_activity_pulse = (*low_activity_pulse);
    pa->t = t;
    pa->con = pcon;
    pa->check_activity = check_activity;
    pa->async_cb_func = raw_sock_parser;
    pa->write_callback = write_callback;
    pa->write_pass_back = write_pass_back;
    pa->asw = NULL;
    if (scs == NULL) {
        destroy_con_obj(co);
        free(pcon);
        free(pa);
        return NULL;
    }
    pthread_mutex_init(&scs->clean_mutex, NULL);
    scs->pa = pa;
    scs->time_out = time_out;
    scs->num_threads = num_threads;
    scs->cpus = cpus;
    scs->set_affinity = set_affinity;
    scs->event_q = create_sized_queue(100);
    scs->connections =
        create_rb_tree(async_int_compare, async_int_destroy,
                       async_info_destroy, async_int_print,
                       async_info_print);
    pa->ass = scs;
    start_async_server_t(scs);
    return scs;
}

struct async_parse_args* add_async_listener(async_server_con_set* scs, void (*fn_ptr) (struct message * m, void *ob),
                         const char *mod_name, size_t mod_len,
                         uint16_t port, queue_t l_args, const char *ip,
                         size_t len, int end_hour, int end_min,
                         int end_day, size_t timeout_val, void *ob,
                         void (*connection_callback) (con_obj * co,
                                                      int is_connected,
                                                      char *log, int len),
                         unsigned long (*mess_parse) (const char *message,
                                                      unsigned long len,
                                                      int *cut_con,
                                                      struct parse_con *
                                                      pc),
                         unsigned long (*seq_parse) (char *data,
                                                     unsigned long size,
                                                     unsigned long bo,
                                                     unsigned long *seq),
                         void (*low_activity_pulse) (con_obj * obj),
                         char *(*create_reset_pad) (unsigned long seq,
                                                    char *ver,
                                                    size_t * len),
                         int check_activity, 
                         void *write_pass_back,
                         void (*write_callback) (const char *name,
                                                 int name_len,
                                                 void *pass_back),
                         void *parser_helper,
                         void *(*copy_parser_helper) (void *ph),
                         void (*p_callback) (void *ph, struct message * m,
                                             void *ob))
{

    struct parse_con *pcon = malloc(sizeof(struct parse_con));
    struct app_end_time *t = malloc(sizeof(struct app_end_time));
    struct async_parse_args *pa = malloc(sizeof(struct async_parse_args));
    t->end_hour = end_hour;
    t->end_min = end_min;
    t->end_day = end_day;
    pcon->send_ptr = send_message;
    pcon->callback = (*fn_ptr);
    pcon->record_mess = record_inbound_message;
    pcon->init_con = add_connection;
    pcon->remove_con = remove_connection;
    pcon->connection_notice = connection_callback;
    pcon->timeout_val = timeout_val;
    pcon->is_logged_on = 0;
    pcon->in_args_list = set_parse_args_from_list(l_args, (*create_reset_pad));
    pcon->in_args = NULL;
    pcon->is_server = 1;
    pcon->t = t;
    pcon->parser_helper = parser_helper;
    pcon->copy_parser_helper = copy_parser_helper;
    pcon->p_callback = p_callback;
    if (mod_len > 0 && mod_len < 100) {
        pcon->mod_name = calloc(mod_len + 1, 1);
        memcpy(pcon->mod_name, mod_name, mod_len);
        pcon->mod_len = mod_len;
    } else {
        pcon->mod_len = 0;
        pcon->mod_name = NULL;
    }
    pcon->book = ob;

    if (pa == NULL) {
        free(pcon);
        return NULL;
    }
    pa->parse = (*mess_parse);
    pa->seq_parse = (*seq_parse);
    pa->low_activity_pulse = (*low_activity_pulse);
    pa->t = t;
    pa->con = pcon;
    pa->check_activity = check_activity;
    pa->write_callback = write_callback;
    pa->write_pass_back = write_pass_back;
    pa->asw = NULL;
    pa->ass = scs;
    pa->ee = calloc(1, sizeof(struct epoll_event));
    pa->ee->data.ptr = pa;
    pa->ee->events = EPOLLIN | EPOLLRDHUP | EPOLLET; //EPOLLONESHOT;
    pa->efd = scs->efd;
    int listenfd = -1;
    if (len > 0 && (memchr(ip, '.', len) == 0)) {
        listenfd = ud_create_and_bind(ip, len);
        pa->async_cb_func = ud_listen_call_back_func;
    } else {
        listenfd = create_and_bind(port);
        pa->async_cb_func = listen_call_back_func;
    }
    int ret_code = listen(listenfd, MAXCONNECTIONS);
    if (ret_code == -1 || scs->efd == -1) {
        send_debug_message("Listen Failed in Async Listener. %d\n",
                           ret_code);
        free(pcon);
        return NULL;
    }
    pcon->co =  create_con_obj(mod_name, mod_len, listenfd,
                        0, 1, 1);
    if (pthread_mutex_lock(&scs->clean_mutex) == 0) {
        int *in_sock = malloc(sizeof(int));
        *in_sock = listenfd;
        rb_tree_no_delete_insert(scs->connections, in_sock, pa);
        pthread_mutex_unlock(&scs->clean_mutex);
    }
    create_recv_epoll_ctl_event(pa);
    return pa;
}
