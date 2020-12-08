/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/timerfd.h>
#include <signal.h>
#include <sys/signalfd.h>
#include <sys/un.h>


#define MDS_BUFFER_SIZE 65536
#include "mds_client.h"
#include "ex_config_helper.h"
#include "dart_allocator.h"
#include "debug_logger.h"

#define MDS_NANO_MULT 30000000
struct mds_client_con {
    ex_config* ex;
    pthread_t thread;
    int cpu;
    int sock;
    void* pass_back;
    int run;
    void(*update)(void* cbo, char* sym, int slen, long bid, long offer);
    void(*con_update)(void* cbo, int is_connected);
};

static int mds_send_wrapper(int sock, char* data, int len, int* disconnect)
{
    int sent = 0;
    int ret = 0;
    do {
        ret = send(sock, data, len, MSG_DONTWAIT | MSG_NOSIGNAL);
        if(ret < 0) {
            ret = errno;
            if(ret == EAGAIN || ret == EWOULDBLOCK || ret == EINTR ||
                    ret == 0) {
                ret = 0;
            } else {
                send_debug_message("Got this error on send: %d, killing con \n",
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
static int send_mdc_logon(mdsclient* mdc)
{
    char b [32];
    memset(b, '\0', 32);
    int len = 0;
    int ulen = mdc->ex->mod_name_len;
    len = ulen + 1;
    memcpy(b, &len, 4);
    memcpy(b + 4, "L", 1);
    memcpy(b + 5, mdc->ex->mod_name, ulen);
    int dis = 0;
    return mds_send_wrapper(mdc->sock, b, len + 4, &dis);
}
static int send_mdc_hb(mdsclient* mdc)
{
    char b [32];
    memset(b, '\0', 32);
    int len = 0;
    char* user = (char*)mdc->ex->mod_name;
    int ulen = mdc->ex->mod_name_len;
    len = ulen + 1;
    memcpy(b, &len, 4);
    memcpy(b + 4, "H", 1);
    memcpy(b + 5, user, ulen);
    int dis = 0;
    mds_send_wrapper(mdc->sock, b, len + 4, &dis);
    if(dis == 1) {
        return 0;
    } else {
        return 1;
    }
}

static int create_mds_con(mdsclient* p, int port, const char* host, int * err)
{
 send_debug_message("Connecting to mds: %d, %s", port, host);
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
        char error_str [16];
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
        send_debug_message("Connection Failed on getaddrinfo: %d, %s: %s\n",
                port, host, error_str); 
        close(sock);
        return -1;
    }
    *err =  connect(sock, res->ai_addr, res->ai_addrlen);
    if(*err < 0 && errno != EINPROGRESS) {
        send_debug_message("Connection Failed on connect: %d, %s, error: %d\n",
                port, host, errno); 
        return -1;
    } else {
        err = 0;
        return sock;
    }

}
static void get_cpu(mdsclient* mdc)
{
    int ret_len = 0;
    char* end_str = get_config_val(mdc->ex, "SERVER", 6, "cpu", 3, &ret_len);
    if(ret_len > 0) {
        mdc->cpu = atoi(end_str);
    } else {
        mdc->cpu = -1;
    }
}
static void connect_mds(mdsclient* mdc)
{
    int s = 0;
    struct timespec sleep_val = {0};
    sleep_val.tv_sec = 30;
    sleep_val.tv_nsec = MDS_NANO_MULT;
    mdc->sock = create_mds_con(mdc, mdc->ex->fix_port, mdc->ex->fix_ip, &s);
    while(mdc->sock == -1) {
        nanosleep(&sleep_val, NULL);
        reload_config(mdc->ex);
        mdc->sock = create_mds_con(mdc, mdc->ex->fix_port, mdc->ex->fix_ip, &s);
    }
}
static int parse_mds_message(mdsclient* mdc, char* d, int dlen)
{
    char* data = d;
    int len = 0;
    int used = 0;
    while(dlen > 4) { 
        memcpy(&len, data, 4);
        if((len + 4) <= dlen) {
            if(data[4] == 'S') {
                int slen = (len -17);
                long bid = 0;
                long offer = 0;
                char* nbbo_off = data + 5 + slen;
                memcpy(&bid, nbbo_off, 8);
                nbbo_off += 8;
                memcpy(&offer, nbbo_off, 8);
                char* sym = data + 5;
                mdc->update(mdc->pass_back, sym, slen, bid, offer);
            } else if(data[4] == 'H') {
                send_debug_message("Got a heartbeat: %d \n", len);
            }
            len += 4;
            data += len;
            dlen -= len;
            used += len;
        } else {
            break;
        }
    }
    return used;
}

static void recv_loop(mdsclient* mdc)
{
    send_debug_message("Starting recv loop in MDS \n");
    int ret = 0;
    char* buffer = 0;
    DART_ALLOC(buffer, 8,MDS_BUFFER_SIZE + 1, ret);
    char* wr_ptr = buffer;
    char* rd_ptr = buffer;
    char* end = buffer + MDS_BUFFER_SIZE;
    fcntl(mdc->sock, F_SETFL, O_NONBLOCK);
    fd_set rfds;
    struct timeval tv;
    do {
        tv.tv_sec = 30;
        tv.tv_usec = 0;
        FD_ZERO(&rfds);
        FD_SET(mdc->sock, &rfds);
        ret = select(mdc->sock + 1,&rfds, NULL, NULL, &tv);
        if(ret == -1) {
            int e = errno;
            if (e != EAGAIN || e != EWOULDBLOCK) {
                send_debug_message("MDS Error on select: %d \n", e);
                goto LOG_DONE;
            }
        } else if(ret == 0) {
            int worked = send_mdc_hb(mdc);
            if(!worked) {
                goto LOG_DONE;
            }
        } else {
            int num_bytes =
                recv(mdc->sock, wr_ptr,
                        (end - wr_ptr), MSG_DONTWAIT);
            if (num_bytes > 0) {
                wr_ptr += num_bytes;
                int used = parse_mds_message(mdc, rd_ptr, wr_ptr - rd_ptr);
                rd_ptr += used;
                if (rd_ptr == wr_ptr ||
                        rd_ptr - wr_ptr >= 0) {
                    wr_ptr = buffer;
                    rd_ptr = buffer;
                } else {
                    if(end - wr_ptr <= 0 ) {
                        char *temp = 0;
                        long total = ((end - buffer) + MDS_BUFFER_SIZE);
                        long write_offset = wr_ptr - rd_ptr;
                        DART_ALLOC(temp, 8, total + 1, ret);
                        if (temp == NULL) {
                            goto LOG_DONE;
                        }
                        send_debug_message("MDS Reallocing total: %ld , unread: %ld \n",
                                total, write_offset);
                        memcpy(temp, rd_ptr, (wr_ptr - rd_ptr));
                        free(buffer);
                        buffer = 0;
                        buffer = temp;
                        end = buffer + total;
                        wr_ptr = buffer;
                        wr_ptr += write_offset;
                        rd_ptr = buffer;
                    }
                } 
            } else if (num_bytes == -1) {
                int e = errno;
                if (e != EAGAIN || e != EWOULDBLOCK) {
                    send_debug_message("MDS Error on read: %d \n", e);
                    goto LOG_DONE;
                }
            } 
        }
    } while (mdc->run);
LOG_DONE:
    free(buffer);
}

static void* run_mds(void* s)
{
    mdsclient* mdc = (mdsclient*)s;
    get_cpu(mdc);
    if(mdc->cpu >= 0) {
        sigset_t blocks;
        sigemptyset(&blocks);
        sigaddset(&blocks, SIGPIPE);
        pthread_sigmask(SIG_BLOCK, &blocks, NULL);
        pthread_t thread = pthread_self();
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(mdc->cpu, &cpuset);
        int s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
        if (s != 0) {
            if (s == EFAULT) {
                send_debug_message
                    ("Invalid mem addr: %d Thread: %d, cpu: %d.", s,
                     1, mdc->cpu);
            } else if (s == EINVAL) {
                send_debug_message
                    ("Bad processor or cpu: %d Thread: %d, cpu: %d.", s,
                     1, mdc->cpu);
            } else if (s == ESRCH) {
                send_debug_message("Bad Thread: %d Thread: %d, cpu: %d.",
                                   s, 1, mdc->cpu);
            } else {
                send_debug_message
                    ("Unknown error: %d Thread: %d, cpu: %d.", s,
                     1, mdc->cpu);
            }
        } else {
            send_debug_message("Thread number: %d, cpu affinity: %d.",
                               1, mdc->cpu);
        }
    }
    while(mdc->run) {
        connect_mds(mdc); 
        if(send_mdc_logon(mdc)){
            mdc->con_update(mdc->pass_back, 1);
            recv_loop(mdc);
        }
        mdc->con_update(mdc->pass_back, 0);
        close(mdc->sock);
    }
    return NULL;
}


mdsclient* create_mds_client(char* config_file_name, int clen, void* cbo, 
                             void (*mds_update)(void* cbo, char* sym, int slen, long bid, long offer),
                             void (*con_update)(void* cbo, int is_connected))
{ 
    mdsclient* mdc = 0;
    int ret = 0;
    DART_ALLOC(mdc, 8, sizeof(struct mds_client_con), ret);
    mdc->ex = init_config(config_file_name, clen);
    mdc->update = mds_update;
    mdc->con_update = con_update;
    mdc->pass_back = cbo;
    mdc->run = 1;
    int error = pthread_create(&mdc->thread, NULL, run_mds, mdc);
    if(error) {
        send_debug_message("FAILED to create MDS THREAD: %d, cpu affinity: %d.",
                           1, mdc->cpu);
        mdc->run = 0;
        return NULL;
    } else {
        send_debug_message("Created MDS THREAD: %d, cpu affinity: %d.",
                           1, mdc->cpu);
        return mdc;
    }
}

void close_mds_client(mdsclient* mdc)
{
    if(mdc) {
        mdc->run = 0;
        pthread_join(mdc->thread, NULL);
        free(mdc);
    }
}
