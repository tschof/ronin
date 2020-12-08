/***********************************************************
 *
 *
 *
 ***********************************************************/
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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

#include "polling_server.h"
#include "log_mnger.h"
#include "rb_tree.h"
#include "hashmap.h"
#include "rex_thr_pool.h"
#include "message_queue.h"
#include "rex_allocator.h"

#define MAXCONNECTIONS 200

struct polling_server {
    rb_tree_t* connections;
    hashmap_t ex_connections;
    thr_pool_t* thr;
    queue_t event_q;
    otl* o;
    pthread_spinlock_t lock;
    int num_threads;
    int time_out;
    int efd;
    int keep_running;
};

typedef struct event_object {
    void* stuff;
    pserver* server;
    void (*ctl_call_event) (void *s);
    int sock;
} event_object;

static int connection_key_compare(const void* a, const void *b)
{
    if(*(int*)a > *(int*)b) {
        return 1;
    }
    if(*(int*)a < *(int*)b) {
        return -1;
    } 
    return 0;
}
static void connection_int_print(const void* a)
{
    printf("%d", *(int*)a);
}

static void connection_info_print(const void* a)
{
    printf("Printing RC Cola.\n");
}

static void connection_key_destroy(void* a)
{
    free((int*)a);
}
static void connection_info_destroy(void* a)
{
    rex_con* rc = (rex_con*)a;
    destroy_sock_recv_manager(rc->con);
    destroy_call_back_wrapper(rc->read_wrapper);
    destroy_call_back_wrapper(rc->write_wrapper);
}
void disconnect(rex_con* rc)
{
    int s = rc->sock;
    if(pthread_spin_lock(&rc->p->lock) == 0) {
        rb_tree_delete(rc->p->connections, &s);
        close(s);
        if(rc->write_wrapper) {
            close(rc->write_wrapper->sock);
        }
        pthread_spin_unlock(&rc->p->lock);
        if(rc->removed == 0 && rc->logon) {
            remove_connection(rc->p, rc);
        }
        destroy_call_back_wrapper(rc->read_wrapper);
        rc->read_wrapper = 0;
        destroy_call_back_wrapper(rc->write_wrapper);
        rc->write_wrapper = 0;
        rc->connection_notice(rc->con_pass_back, 0, NULL);
        if(rc->is_server) {
            pthread_spin_lock(&rc->p->lock);
            destroy_sock_recv_manager(rc->con);
            pthread_spin_unlock(&rc->p->lock);
            free(rc);
        }
    }
}

void dis_con(void* rc)
{
    disconnect((rex_con*)rc);
}
static void write_master(void* w)
{
    rex_con* rc = (rex_con*)w;
    epoll_ctl(rc->write_wrapper->efd, EPOLL_CTL_DEL, rc->write_wrapper->sock,
            rc->write_wrapper->ee);
    rc->write_callback(rc);
}

static void async_add_back(void* k)
{
    event_object *eo = (event_object *) k;
    if (pthread_spin_lock(&eo->server->lock) == 0) {
        rex_con *pa =
            rb_tree_get_val(eo->server->connections, &eo->sock);
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
                (eo->server->o, "Unable to find this connection for write events: %d!\n",
                 eo->sock);
            rb_tree_print(eo->server->connections);
        }
        pthread_spin_unlock(&eo->server->lock);
    }
}
static void set_sock_opt(otl* o, int sock)
{
    int on = 1;
    int buffer = 65536;
    fcntl(sock, F_SETFL, O_NONBLOCK);

    if(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
                (char*)&on, sizeof(on)) == -1) {
        send_debug_message(o, "Failed to make socket nonblocking %d \n", sock);
    }
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
                (const char*)&on, sizeof(on)) == -1) {
        send_debug_message(o, "Failed to set reuse addr! %d \n", sock);
    }
    if(setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&buffer,
                sizeof(buffer)) == -1) {
        send_debug_message(o, "Failed to set receive buff size addr! %d \n", sock);
    }
    if(setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&buffer,
                sizeof(buffer)) == -1) {
        send_debug_message(o, "Failed to set send buff size addr! %d \n", sock);
    }
}

static int create_n_bind(otl* o, int port)
{
    int listenfd;
    struct sockaddr_in addr;
    int rc;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        return -1;
    }
    set_sock_opt(o, listenfd);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    rc = bind(listenfd, (struct sockaddr *) &addr, sizeof(addr));
    if (rc < 0) {
        send_debug_message(o, "Bind of socket failed!!!!%d \n", listenfd);
        close(listenfd);
        return -1;
    }
    return listenfd;
}

static int ud_create_n_bind(otl* o, const char* ip, int ip_len)
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
        close(sock);
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
        send_debug_message(o, "Bind of listening socket failed!!!!\n");
        close(sock);
        return -1;
    }
    fcntl(sock, F_SETFL, O_NONBLOCK);
    return sock;
}

rex_con* create_rex_con(pserver* p, struct ex_con* ex,
        void* parse_pass_back,void* con_pass_back,
        login_info* linfo,
        void (*write_callback) ( void* pass_back),
        void (*connection_notice)(void* rc,
            int is_connected, dart_order_obj* logon),
        void (*parse)(int* cut_con, rex_con* rc),
        login_info* (*is_valid)(void* rc, dart_order_obj* logon))
{
    rex_con* rc = 0;
    int r = 0;
    REX_ALLOC(rc, 8, sizeof(struct rex_connection), r);
    if(linfo == 0) {
        rc->logon = 0;
    } else {
        rc->logon = linfo;
    }
    rc->write_callback = write_callback;
    rc->connection_notice = connection_notice;
    rc->parse = parse;
    rc->is_valid = is_valid;
    rc->p = p;
    rc->o = p->o;
    rc->conf = ex;
    rc->parser = parse_pass_back;
    rc->con_pass_back = con_pass_back;
    rc->con = create_sock_recv_manager();
    rc->read_wrapper = 0;
    rc->write_wrapper = 0;
    return rc;
}
static void create_recv_epoll_ctl_event(rex_con* pa)
{
    int ret = 0;
    event_object* wo = 0;
    REX_ALLOC(wo, 8, sizeof(struct event_object), ret);
    wo->server = pa->p;
    wo->stuff = pa;
    wo->sock = pa->sock;
    wo->ctl_call_event = async_add_back;
    if(pthread_spin_lock(&pa->p->lock) == 0) {
        enqueue(pa->p->event_q, wo);
        pthread_spin_unlock(&pa->p->lock);
    }
}

static int create_n_connect(pserver* p, int port, const char* host, int * err)
{
    char portstr[6];
    int sock = socket(AF_INET, SOCK_STREAM,0);
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME;
    snprintf(portstr, sizeof(portstr), "%d", port);
    int adder_ret = getaddrinfo(host, portstr, &hints, &res);
    if(adder_ret != 0) {
        char error_str [50];
        switch(adder_ret) {
            case EAI_ADDRFAMILY:
                sprintf(error_str, "ADDRFAMILY, %d ", adder_ret);
                break;
            case EAI_AGAIN:
                sprintf(error_str, "AGAIN, %d ", adder_ret);
                break;
            case EAI_BADFLAGS:
                sprintf(error_str, "BADFLAGS, %d ", adder_ret);
                break;
            case EAI_FAIL:
                sprintf(error_str, "FAIL, %d ", adder_ret);
                break;
            case EAI_FAMILY:
                sprintf(error_str, "FAMILY, %d ", adder_ret);
                break;
            case EAI_MEMORY:
                sprintf(error_str, "MEMORY, %d ", adder_ret);
                break;
            case EAI_NODATA:
                sprintf(error_str, "NODATA, %d ", adder_ret);
                break;
            case EAI_NONAME:
                sprintf(error_str, "NONAME, %d ", adder_ret);
                break;
            case EAI_SERVICE:
                sprintf(error_str, "SERVICE, %d ", adder_ret);
                break;
            case EAI_SOCKTYPE:
                sprintf(error_str, "SOCKTYPE, %d ", adder_ret);
                break;
            case EAI_SYSTEM:
                sprintf(error_str, "SYSTEM, %d ", adder_ret);
                break;
            default:
                sprintf(error_str, "UNKNOWN, %d ", adder_ret);
                break;
        } 
        send_debug_message(p->o, "Connection Failed on getaddrinfo: %d, %s: %s\n",
                port, host, error_str); 
        close(sock);
        return -1;
    }
    set_sock_opt(p->o, sock);   
    *err =  connect(sock, res->ai_addr, res->ai_addrlen);
    if(*err < 0 && errno != EINPROGRESS) {
        send_debug_message(p->o, "Connection Failed on connect: %d, %s, error: %d\n",
                port, host, errno); 
        return -1;
    } else {
        err = 0;
        return sock;
    }

}
static void raw_sock_parser(void* pargs)
{
    rex_con* rc = (rex_con*)pargs;
    int cut_connection = 0;
    manage_socket(rc, &cut_connection);
    if(cut_connection) {
        send_debug_message(rc->p->o, "Manage socket returned cut connection: %d \n", rc->sock);
        disconnect(rc);
    } else if(rc->p->num_threads > 2) {
        create_recv_epoll_ctl_event(rc);
    }
}
static void add_timer_task(void* k)
{
    struct event_object* ev = (struct event_object*)k;
    if(pthread_spin_lock(&ev->server->lock) == 0) {
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
        pthread_spin_unlock(&ev->server->lock);
    }
}
void create_new_conversation(rex_con* rc)
{
    if(rc->read_wrapper == 0) {
        rc->read_wrapper = create_call_back_wrapper(rc, 
                raw_sock_parser, dis_con, rc->sock);
    }
    rc->read_wrapper->ee->events = EPOLLIN | EPOLLRDHUP | EPOLLET;
    rc->read_wrapper->efd = rc->p->efd;
    int dupper = dup(rc->sock);
    if(dupper == -1) {
        send_debug_message(rc->p->o, "Failed to dup sock: %d \n", rc->sock);
    }
    if(rc->write_wrapper == 0) {
        rc->write_wrapper = create_call_back_wrapper(rc, 
                write_master, dis_con, dupper);
    }
    rc->write_wrapper->ee->events = EPOLLOUT | EPOLLRDHUP | EPOLLET;
    rc->write_wrapper->efd = rc->p->efd;
    int * in_sock = calloc(sizeof(int), 1);
    *in_sock = rc->sock;
    if(pthread_spin_lock(&rc->p->lock) == 0) {
        rb_tree_no_delete_insert(rc->p->connections, in_sock, rc);
        pthread_spin_unlock(&rc->p->lock);
    }
    create_recv_epoll_ctl_event(rc);
    send_debug_message(rc->p->o, "Adding this connection: %d, dup: %d\n",
            rc->sock, dupper);
}

static void ud_listen_call_back(void* arg)
{
    rex_con* pa = (rex_con*)arg;
    int listenfd = pa->sock;
     do {
        int sock = -1;
        if ((sock = accept(listenfd, NULL, NULL)) == -1) {
            break;
        }
        send_debug_message
            (pa->p->o, "Accepted connection: sock %d, on the ud_connection.\n",
             sock);
        set_sock_opt(pa->p->o, sock);
        rex_con* rc = create_rex_con(pa->p, pa->conf,
                                     pa->parser, pa->con_pass_back,NULL,
                                     pa->write_callback,
                                     pa->connection_notice,
                                     pa->parse, pa->is_valid);
        rc->sock = sock;
        rc->is_server = 1;
        rc->o = pa->o;
        create_new_conversation(rc);
    } while(1);
    create_recv_epoll_ctl_event(pa);
}

static void listen_call_back(void* arg)
{
    rex_con* pa = (rex_con*)arg;
    int listenfd = pa->sock;
    do {
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
                (pa->p->o, "Accepted connection: sock %d, host = %s, port = %s\n",
                 sock, hbuf, sbuf);
        }
        rex_con* rc = create_rex_con(pa->p, pa->conf,
                                     pa->parser, pa->con_pass_back, NULL,
                                     pa->write_callback,
                                     pa->connection_notice,
                                     pa->parse, pa->is_valid);
        rc->sock = sock;
        rc->is_server = 1;
        rc->o = pa->o;
        rc->parser = pa->parser;
        set_sock_opt(rc->p->o, sock);
        create_new_conversation(rc);
    } while(1);
    create_recv_epoll_ctl_event(pa);
}

static void connect_response(void* k)
{
    rex_con* rc = (rex_con*)k;
    epoll_ctl(rc->connect_wrapper->efd, EPOLL_CTL_DEL,
            rc->connect_wrapper->sock,
            rc->connect_wrapper->ee);
    int result = 0;
    socklen_t rlen = sizeof(result);
    if(getsockopt(rc->sock, SOL_SOCKET, SO_ERROR, &result, &rlen) < 0) {
        send_debug_message(rc->p->o, "Connect failed Spectacularly: %d, %s,%d \n",
                rc->conf->port, rc->conf->ip, rc->sock); 
        close(rc->sock);
        rc->connection_notice(rc->con_pass_back, 0, NULL);
    } else if(result != 0) {
        send_debug_message(rc->p->o, "Connect failed: %d, %s,%d CODE: %d \n",
                rc->conf->port, rc->conf->ip, rc->sock, result); 
        close(rc->sock);
        rc->connection_notice(rc->con_pass_back, 0, NULL);
    } else {
        create_new_conversation(rc);
        rc->connect(rc->con_pass_back);
    }
}

static void connect_callback(void* k)
{
    struct event_object* ev = (struct event_object*)k;
    if(pthread_spin_lock(&ev->server->lock) == 0) {
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
        pthread_spin_unlock(&ev->server->lock);
    }
} 

void add_signal_listener(pserver* p, call_back_wrapper* cbw)
{
    cbw->ee->events = EPOLLIN | EPOLLRDHUP | EPOLLET;
    cbw->efd = p->efd;
    int ret = 0;
    event_object* wo = 0;
    REX_ALLOC(wo, 8, sizeof(struct event_object), ret);
    wo->server = p;
    wo->sock = cbw->sock;
    wo->ctl_call_event = add_timer_task;
    if(pthread_spin_lock(&p->lock) == 0) {
        enqueue(p->event_q, wo);
        pthread_spin_unlock(&p->lock);
    }
}

static void def_timer_shutdown(void* w)
{
}

call_back_wrapper* add_timer(pserver* p, int repeat_int_sec,
        int repeat_int_nano, int init_int_sec, int init_int_nano,
        void* cb_obj, void (*t_cb_func)(void* cb))
{
    int sock = -1;
    struct itimerspec itspec;
    sock = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK);
    itspec.it_interval.tv_sec = repeat_int_sec;
    itspec.it_interval.tv_nsec = repeat_int_nano;
    itspec.it_value.tv_sec = init_int_sec;
    itspec.it_value.tv_nsec = init_int_nano;
    timerfd_settime(sock, 0, &itspec, NULL);
    call_back_wrapper* cbw = create_call_back_wrapper(cb_obj,
            t_cb_func,def_timer_shutdown, sock);
    cbw->ee->events = EPOLLIN | EPOLLET; //EPOLLONESHOT;
    cbw->efd = p->efd;
    int ret = 0;
    event_object *wo = 0;
    REX_ALLOC(wo, 8, sizeof(struct event_object), ret);
    wo->server = p;
    wo->stuff = cbw;
    wo->sock = sock;
    wo->ctl_call_event = add_timer_task;
    if (pthread_spin_lock(&p->lock) == 0) {
        enqueue(p->event_q, wo);
        pthread_spin_unlock(&p->lock);
    }
    return cbw;
}

static void create_connect_event(rex_con* pa)
{
    int ret = 0;
    event_object* wo = 0;
    REX_ALLOC(wo, 8, sizeof(struct event_object), ret);
    wo->server = pa->p;
    wo->sock = pa->sock;
    wo->ctl_call_event = connect_callback;
    if(pa->connect_wrapper == 0) {
        pa->connect_wrapper = create_call_back_wrapper(pa,
                connect_response,def_timer_shutdown, pa->sock);
    } else {
        pa->connect_wrapper->sock = pa->sock;
    }
    pa->connect_wrapper->ee->events = EPOLLOUT | EPOLLET; 
    pa->connect_wrapper->efd = pa->p->efd;
    wo->stuff = pa->connect_wrapper;
    if(pthread_spin_lock(&pa->p->lock) == 0) {
        enqueue(pa->p->event_q, wo);
        pthread_spin_unlock(&pa->p->lock);
    }
}

void disconnect_by_name(pserver* p, const char* name,
        int nlen)
{
    rex_con* r = 0;
    int ret = 0;
    if(pthread_spin_lock(&p->lock) == 0) {
        ret = get_map_entry(p->ex_connections, name, nlen,(void*)&r);
        pthread_spin_unlock(&p->lock);
        if(ret > 0 && r) {
            disconnect(r);
        }
    }
}

static void async_add_sender(void *k)
{
    struct event_object* ev = (struct event_object*)k;
    if(pthread_spin_lock(&ev->server->lock) == 0) {
        rex_con* rc = (rex_con*)ev->stuff;
        if(rc != NULL && rc->write_wrapper != NULL) {
            int ret = 0;
            ret = epoll_ctl(rc->write_wrapper->efd, EPOLL_CTL_ADD,
                    rc->write_wrapper->sock, rc->write_wrapper->ee);
            if(ret == -1) {
                ret = errno;
                if(ret == EEXIST) {
                    ret = epoll_ctl(rc->write_wrapper->efd, EPOLL_CTL_MOD,
                            rc->write_wrapper->sock, rc->write_wrapper->ee);
                    if(ret == -1) {
                        ret = errno;
                    }
                }
                send_debug_message(ev->server->o,
                        "After EEXIST detected modded and got this: %d for sock: %d \n",
                        ret, ev->sock);
            }
        } else {
            send_debug_message(ev->server->o,
                    "Unable to find this connection for write events: %d, \n",
                    ev->sock);
        }
        pthread_spin_unlock(&ev->server->lock);
    }
}

void add_sender(pserver* p, rex_con* rc)
{
    int ret = 0;
    event_object* wo = 0;
    REX_ALLOC(wo, 8, sizeof(struct event_object), ret);
    wo->server = p;
    wo->stuff = rc;
    wo->sock = rc->sock;
    wo->ctl_call_event = async_add_sender;
    if(pthread_spin_lock(&p->lock) == 0) {
        enqueue(p->event_q, wo);
        pthread_spin_unlock(&p->lock);
    }
}

int check_is_connected(pserver* p, char* name, int nlen)
{
    int ret = 0;
    if(pthread_spin_lock(&p->lock) == 0) {
        ret = contains_key(p->ex_connections, name, nlen);
        pthread_spin_unlock(&p->lock);
    }
    return ret;
}

int remove_connection(pserver* p, rex_con* rc)
{
    int ret = 0;
    if(pthread_spin_lock(&p->lock) == 0) {
        ret = remove_from_map(p->ex_connections,
                rc->logon->name, rc->logon->nlen);
        pthread_spin_unlock(&p->lock);
    }
    return ret;
}

int add_connection(pserver* p, rex_con* rc)
{
    int ret = 0;
    if(pthread_spin_lock(&p->lock) == 0) {
        ret = no_copy_insert(p->ex_connections,
                rc->logon->name, rc->logon->nlen, rc);
        pthread_spin_unlock(&p->lock);
    }
    return ret;
}

static void clean_event_queue(pserver* p)
{
    element_t e, temp;
    pthread_spin_lock(&p->lock);
    e = dequeue(p->event_q);
    pthread_spin_unlock(&p->lock);
    while(e != NULL) {
        event_object *wo = (event_object*)e->d;
        wo->ctl_call_event(wo);
        temp = e;
        e = temp->next;
        free(wo);
        pthread_spin_lock(&p->lock);
        free_element(p->event_q, temp);
        pthread_spin_unlock(&p->lock);
    }
}
static void run_epoll_loop(void* arg)
{
    pserver* p = (pserver*)arg;
    int eret = 0;
    struct epoll_event* events = 0;
    REX_ALLOC(events, 8, (512 * sizeof(struct epoll_event)), eret);
    do {
        int n, i;
        n = epoll_wait(p->efd, events, 512, p->time_out);
        clean_event_queue(p);
        if(n > 0) {
            for(i = 0; i < n; ++i) {
                if((events[i].events & EPOLLERR) ||
                        (events[i].events & EPOLLHUP) ||
                        (!(events[i].events & EPOLLIN) &&
                        !(events[i].events & EPOLLOUT))) {
                    call_back_wrapper* cbw = (call_back_wrapper*)events[i].data.ptr;
                    rex_con* rc = rb_tree_get_val(p->connections, &cbw->sock);
                    if(rc != NULL) {
                        epoll_ctl(cbw->efd, EPOLL_CTL_DEL, cbw->sock,
                                &events[i]);
                        cbw->clean_up(cbw->stuff);
                    } else {
                        send_debug_message(p->o,
                                "Failed to find connection class for disconnect.%d \n",
                                cbw->sock);
                        if(cbw && cbw->call_back_func) {
                            cbw->call_back_func(cbw->stuff);
                        }
                    }
                } else if(p->num_threads > 2) {
                    call_back_wrapper* cbw = (call_back_wrapper*)events[i].data.ptr;
                    epoll_ctl(cbw->efd, EPOLL_CTL_DEL, cbw->sock,
                            &events[i]);
                    thr_pool_queue(p->thr, cbw->call_back_func, cbw->stuff);
                } else {
                    call_back_wrapper* cbw = (call_back_wrapper*)events[i].data.ptr;
                    cbw->call_back_func(cbw->stuff);
                }
            }
        } else if(p->num_threads <= 2) {
            one_stop_pop(p->thr);
        }
    } while(p->keep_running);

    close(p->efd);
    free(events);
}

static void run_epoll_loop2(void* arg)
{
    pserver* p = (pserver*)arg;
    set_thr_id(p->o,pthread_self());
    int eret = 0;
    struct epoll_event* events = 0;
    REX_ALLOC(events, 8, (512 * sizeof(struct epoll_event)), eret);
    do {
        int n, i;
        n = epoll_wait(p->efd, events, 512, p->time_out);
        clean_event_queue(p);
        if(n > 0) {
            for(i = 0; i < n; ++i) {
                if((events[i].events & EPOLLERR) ||
                        (events[i].events & EPOLLHUP) ||
                        (!(events[i].events & EPOLLIN) &&
                        !(events[i].events & EPOLLOUT))) {
                    call_back_wrapper* cbw = (call_back_wrapper*)events[i].data.ptr;
                    rex_con* rc = rb_tree_get_val(p->connections, &cbw->sock);
                    if(rc != NULL) {
                        epoll_ctl(cbw->efd, EPOLL_CTL_DEL, cbw->sock,
                                &events[i]);
                        cbw->clean_up(cbw->stuff);
                    } else {
                        send_debug_message(p->o,
                                "Failed to find connection class for disconnect.%d \n",
                                cbw->sock);
                        if(cbw && cbw->call_back_func) {
                            cbw->call_back_func(cbw->stuff);
                        }
                    }
                } else {
                    call_back_wrapper* cbw = (call_back_wrapper*)events[i].data.ptr;
                    cbw->call_back_func(cbw->stuff);
                }
            }
        }
        one_stop_pop(p->thr);
        log_check(p->o);
    } while(p->keep_running);

    close(p->efd);
    free(events);
}
pserver* start_lib_server(int num_threads, int to, int* cpus, int depth,
        otl* o)
{
    int ret = 0;
    pserver* p = 0;
    REX_ALLOC(p, 8, sizeof(struct polling_server), ret);
    pthread_spin_init(&p->lock, PTHREAD_PROCESS_PRIVATE);
    p->time_out = 0;
    p->keep_running = 1;
    p->time_out = to;
    p->event_q = create_sized_queue(depth);
    p->ex_connections = create_map(depth);
    p->connections = create_rb_tree(connection_key_compare,
            connection_key_destroy,
            connection_info_destroy,
            connection_int_print,
            connection_info_print);
    p->num_threads = num_threads < 2 ? 2 : num_threads;
    p->thr = thr_pool_create(o, p->num_threads, p->num_threads, 0,
            cpus);
    p->efd = epoll_create1(0);
    p->o = o;
    create_thr(p->thr, run_epoll_loop2, p);
    return p;
}
void start_job(pserver* p, void(*user_job)(void* d), void* uj)
{
    create_thr(p->thr, user_job, uj);
}

pserver* start_server(int num_threads, int time_out,
        int* cpus, int depth, otl* o)
{
    int ret = 0;
    pserver* p = 0;
    REX_ALLOC(p, 8, sizeof(struct polling_server), ret);
    pthread_spin_init(&p->lock, PTHREAD_PROCESS_PRIVATE);
    p->time_out = time_out;
    p->keep_running = 1;
    p->event_q = create_sized_queue(depth);
    p->ex_connections = create_map(depth);
    p->connections = create_rb_tree(connection_key_compare,
            connection_key_destroy,
            connection_info_destroy,
            connection_int_print,
            connection_info_print);
    p->num_threads = num_threads < 2 ? 2 : num_threads;
    p->thr = thr_pool_create(o, p->num_threads, p->num_threads, 0,
            cpus);
    p->efd = epoll_create1(0);
    p->o = o;
    create_thr(p->thr, start_logging, o);
    create_thr(p->thr, run_epoll_loop, p);
    return p;
}
rex_con* add_listener(pserver* p, struct ex_con* ex,
        void* parse_pass_back,void* con_pass_back,
        void (*write_callback) (void* pass_back),
        void (*connection_notice)(void* rc,
            int is_connected, dart_order_obj* logon),
        void (*parse)(int* cut_con, rex_con* rc),
        login_info* (*is_valid)(void* rc, dart_order_obj* logon))
{
    int is_ud = 0;
    rex_con* rc = create_rex_con(p, ex, parse_pass_back, con_pass_back,
            NULL, write_callback, connection_notice, parse, is_valid);
    rc->is_server = 1;
    if(ex->ip_len > 0 &&
            memchr(ex->ip, '.', ex->ip_len) == 0) {
        rc->sock = ud_create_n_bind(p->o, ex->ip, ex->ip_len);
        is_ud = 1;
    } else {
        rc->sock = create_n_bind(p->o, ex->port);
    }
    int ret = listen(rc->sock, MAXCONNECTIONS); 
    if(ret < 0 || p->efd < 0) {
        send_debug_message(p->o, "Listen Failed!!! %d, error: %d \n",
                rc->sock, ret);
        destroy_sock_recv_manager(rc->con);
        free(rc);
        return NULL;
    } else {
        rc->write_wrapper = create_call_back_wrapper(rc,
                write_master, dis_con, rc->sock);
        if(is_ud) {
            rc->read_wrapper = create_call_back_wrapper(rc,
                    ud_listen_call_back, dis_con, rc->sock);
        } else {
            rc->read_wrapper = create_call_back_wrapper(rc,
                    listen_call_back, dis_con, rc->sock);
        }
        rc->read_wrapper->ee->events = EPOLLIN | EPOLLRDHUP | EPOLLET;
        rc->read_wrapper->efd = p->efd;
        if(pthread_spin_lock(&p->lock) == 0) {
            int* in_sock = calloc(sizeof(int), 1);
            *in_sock = rc->sock;
            rb_tree_no_delete_insert(p->connections, in_sock, rc);
            pthread_spin_unlock(&p->lock);
        }
        create_recv_epoll_ctl_event(rc);
    }
    return rc;
}
void rex_connect(pserver* p,  rex_con* rc)
{

    int err = 0; 
    int sock = create_n_connect(p, rc->conf->port, rc->conf->ip, &err);
    if(sock > 0) {
        rc->sock = sock;
        rc->is_server = 0;
        if(err < 0) {
            create_connect_event(rc);
        } else {
            create_new_conversation(rc);
            rc->connect(rc->con_pass_back);
        }
    } else {
        send_debug_message(p->o, "Connection Failed: %d, %s\n",
                rc->conf->port, rc->conf->ip);
        rc->connection_notice(rc->con_pass_back, 0, NULL);
    }
}

void rex_direct_connect(pserver* p, rex_con* rc)
{
    int err = 0; 
    int sock = create_n_connect(p, rc->conf->port, rc->conf->ip, &err);
    if(sock > 0) {
        rc->sock = sock;
        rc->is_server = 0;
        if(err < 0) {
            create_connect_event(rc);
        } else {
            rc->connect(rc->con_pass_back);
        }
    } else {
        send_debug_message(p->o, "Connection Failed: %d, %s\n",
                rc->conf->port, rc->conf->ip);
        rc->connection_notice(rc->con_pass_back, 0, NULL);
    }
}

void reconnect(void* vd)
{
    rex_con* rc = (rex_con*)vd;
    int err = 0;
    int sock = create_n_connect(rc->p, rc->conf->port, rc->conf->ip, &err);
    if(sock > 0) {
        rc->disconnect = 0;
        rc->sock = sock;
        if(err < 0) {
            create_connect_event(rc);
        } else {
            create_new_conversation(rc);
            rc->connect(rc->con_pass_back);
        }
    } else {
        send_debug_message(rc->p->o, "Connection Failed on reconnect: %d, %s\n",
                rc->conf->port, rc->conf->ip);
        rc->connection_notice(rc->con_pass_back, 0, NULL);
    }
}
static void clean_con_map(hashmap_t map)
{
    rex_con* fut = 0;
    char *key = 0;
    int key_len = 0;
    hashmap_iterator_t hi = hashmap_iterator_begin(map);
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                                     (void *) &fut);
    while (valid) {
        fut->connection_notice(fut->con_pass_back, 0, NULL);
        fut->removed = 1;
        disconnect(fut);
        valid = hashmap_iterator_next(map, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &fut);
    }
    hashmap_iterator_destroy(hi);
}
void join_n_clean(pserver* p)
{
    thr_pool_wait_n_join(p->thr);
    clean_con_map(p->ex_connections);
    delete_map(p->ex_connections);
    rb_tree_destroy(p->connections);
    destroy_queue(p->event_q);
}

void stop_system(pserver* p)
{
    thr_pool_destroy(p->thr);
    p->keep_running = 0;
}

void add_recon_timer(pserver* p, call_back_wrapper* cbw)
{
    struct timespec now;
    struct itimerspec newtime;
    if(cbw->sock == -1) {
        cbw->sock = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK);
        cbw->ee->events = EPOLLIN | EPOLLET;
    }
    cbw->efd = p->efd;
    clock_gettime(CLOCK_REALTIME, &now);
    newtime.it_interval.tv_sec = 0;
    newtime.it_interval.tv_nsec = 0;
    newtime.it_value.tv_sec = now.tv_sec + 30;
    newtime.it_value.tv_nsec = 0;
    if(timerfd_settime(cbw->sock, TFD_TIMER_ABSTIME, &newtime, NULL) == -1) {
        printf("Unable to rearm timer.\n");
    }
    int ret = 0;
    event_object *wo =  0;
    REX_ALLOC(wo, 8, sizeof(struct event_object), ret);
    wo->server = p;
    wo->stuff = cbw;
    wo->sock = cbw->sock;
    wo->ctl_call_event = add_timer_task;
    if (pthread_spin_lock(&p->lock) == 0) {
        enqueue(p->event_q, wo);
        pthread_spin_unlock(&p->lock);
    }
}

void add_back_rep_timer(pserver* p, call_back_wrapper* cbw)
{
    if (p->num_threads > 2) {
        int ret = 0;
        cbw->efd = p->efd;
        event_object *wo = 0;
        REX_ALLOC(wo, 8, sizeof(struct event_object), ret);
        wo->server = p;
        wo->stuff = cbw;
        wo->sock = cbw->sock;
        wo->ctl_call_event = add_timer_task;
        if (pthread_spin_lock(&p->lock) == 0) {
            enqueue(p->event_q, wo);
            pthread_spin_unlock(&p->lock);
        }
    }
}

void add_back_rep_timer_adjust(pserver* p, call_back_wrapper* cbw, int new_sec)
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
    cbw->efd = p->efd;
    if (p->num_threads > 2) {
        int ret = 0;
        event_object *wo = 0;
        REX_ALLOC(wo, 8, sizeof(struct event_object), ret);
        wo->server = p;
        wo->stuff = cbw;
        wo->sock = cbw->sock;
        wo->ctl_call_event = add_timer_task;
        if (pthread_spin_lock(&p->lock) == 0) {
            enqueue(p->event_q, wo);
            pthread_spin_unlock(&p->lock);
        }
    }
}
otl* get_logger(pserver* p)
{
    return p->o;
}

int gen_send(rex_con* rc, char* data, int len, int* disconnect)
{
    int sent = 0;
    int ret = 0;
    do {
        ret = send(rc->sock, data, len, MSG_DONTWAIT | MSG_NOSIGNAL);
        if(ret < 0) {
            ret = errno;
            if(ret == EAGAIN || ret == EWOULDBLOCK || ret == EINTR ||
                    ret == 0) {
                ret = 0;
            } else {
                send_debug_message(rc->o, "Got this error on send: %d, killing con \n",
                        ret);
                ret = -1;
                *disconnect = 1;
            }
        } else {
            data += ret;
            sent += ret;
            len -= ret;
        }
    } while(len > 0 && ret > 0);
    return sent;
}

