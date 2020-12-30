/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "connection_interface.h"
#include "ex_config_helper.h"
#include "debug_logger.h"
#include "async_server.h"
#include "dart_constants.h"
#include "config_loader.h"
#include "transaction_recorder.h"
#include "lifecycle_master.h"

void ci_async_iovec_sender(struct iovec *iov, int iovcnt, char *name,
                           int name_len, con_interface * ob, int sock);

void ci_cleansing_rinse(struct connection_interface *ci)
{
    if(ci) {
        remove_obj_by_client_id(ci->gk, ci->fix_con_name, ci->fix_con_len);
        free(ci->module_name);
        free(ci->fix_con_name);
        free(ci->ip_str);
        free(ci->dest_id);
        if ((ci->ds).dest_name) {
            free((ci->ds).dest_name);
        }
        databuf_destroy(ci->mess_to_send);
        if(ci->session_conf) {
            dart_destroy_config(ci->session_conf->dc);
            destroy_init_args(ci->session_conf);
        } 
        if(ci->connection_conf) {
            destroy_ex_config(ci->connection_conf);
            free(ci->connection_conf);
        }
        if(ci->recon_wrapper) {
            destroy_call_back_wrapper(ci->recon_wrapper);
        }
        if(ci->block_wrapper) {
            destroy_call_back_wrapper(ci->block_wrapper);
        }
        free(ci);
    }
}

void ci_reload_ip_port(struct connection_interface *ci)
{
    dart_reset_maps(ci->session_conf->dc);
}

static void ci_disconnect(struct connection_interface *ci)
{
    //async_parse_args *apa = (struct async_parse_args *) ci->ap->apa;
    disconnect(ci->ap);
}

static void ci_reset_seq(struct connection_interface *ci, int out, int in)
{
    async_parse_args *con = ci->ap;
    con->in_args->reset_time->create_new = 1;
    con->in_args->reset_time->new_incoming = in;
    con->in_args->reset_time->new_outgoing = out;
    if (ci->connected) {
        //hasync_parse_args *apa = (struct async_parse_args *) ci->ap->apa;
        disconnect(ci->ap);
    }
}

void reset_con_interface(con_interface * cb, int num_orders,
                         char *name, int name_len, char *send_name,
                         int send_len,
                         int (*connect) (struct async_parse_args *),
                         int (*build_offset) (struct async_parse_args *),
                         int (*on_enter_order) (struct connection_interface
                                                *, dart_order_obj *,
                                                struct async_parse_args *),
                         int (*on_cancel_order) (struct
                                                 connection_interface *,
                                                 dart_order_obj *),
                         void (*on_replace_order) (struct
                                                   connection_interface *,
                                                   dart_order_obj *),
                         gatekeeper * gk, char *dest_id, int dest_id_len)
{
    cb->connect = connect;
    cb->build_offset = build_offset;
    cb->on_enter_order = on_enter_order;
    cb->on_cancel_order = on_cancel_order;
    cb->on_replace_order = on_replace_order;
    cb->cl_ord_id_map = get_initial_order_map(gk, dest_id, dest_id_len);
    cb->module_name = calloc(name_len + 1, 1);
    memcpy(cb->module_name, name, name_len);
    cb->module_name_len = name_len;
    cb->fix_con_name = calloc(send_len + 1, 1);
    memcpy(cb->fix_con_name, send_name, send_len);
    cb->fix_con_len = send_len;
    cb->ass = NULL;
    cb->session_conf = NULL;
    cb->can_send_orders = 0;
    cb->is_blocked = 0;
    cb->pendingblock = 0;
    cb->adminblockoverride = 0;
    cb->on_connect = on_connect;
    cb->cleansing_rinse = ci_cleansing_rinse;
    cb->disconnect = ci_disconnect;
    cb->reset_seq = ci_reset_seq;
    cb->seq = 0;
    cb->last_sent = 0;
    cb->last_recv = 0;
    cb->ip_str = calloc(64, 1);
    cb->ci_iovec_sender = ci_async_iovec_sender;
    if(dest_id_len > 0) {
        cb->dest_id = calloc(dest_id_len + 1, 1);
        memcpy(cb->dest_id, dest_id, dest_id_len);
    } else {
        cb->dest_id = NULL;
    }
    cb->dest_len = dest_id_len;
    cb->cancel_on_dis = 0;
    cb->do_oats_balance = 1;
    cb->should_buffer = 0;
}

con_interface *create_empty_interface()
{
    con_interface *cb = calloc(sizeof(con_interface), 1);
    if (pthread_spin_init(&cb->mutex, PTHREAD_PROCESS_PRIVATE) != 0) {
        printf("Failed to init fix_seq_blocker.\n");
    }
    if (pthread_spin_init(&cb->send_mutex, PTHREAD_PROCESS_PRIVATE) != 0) {
        printf("Failed to init send_mutex.\n");
    }
    cb->recon_wrapper = 0;
    cb->block_wrapper = 0;
    cb->time_wrapper = 0;
    cb->connect = 0;
    cb->build_offset = 0;
    cb->on_enter_order = 0;
    cb->on_cancel_order = 0;
    cb->on_replace_order = 0;
    cb->cl_ord_id_map = 0;
    cb->module_name = 0;
    cb->module_name_len = 0;
    cb->fix_con_name = 0;
    cb->fix_con_len = 0;
    cb->mess_to_send = databuf_alloc(1024);
    cb->ass = NULL;
    cb->session_conf = NULL;
    cb->can_send_orders = 0;
    cb->is_blocked = 0;
    cb->pendingblock = 0;
    cb->adminblockoverride = 0;
    cb->on_connect = on_connect;
    cb->seq = 0;
    cb->last_sent = 0;
    cb->last_recv = 0;
    cb->cleansing_rinse = ci_cleansing_rinse;
    cb->ip_str = 0;
    cb->disconnect = ci_disconnect;
    cb->reset_seq = ci_reset_seq;
    cb->ci_iovec_sender = ci_async_iovec_sender;
    cb->dest_id = 0;
    cb->dest_len = 0;
    cb->cancel_on_dis = 0;
    cb->do_oats_balance = 1;
    cb->should_buffer = 0;
    cb->tg = 0;
    cb->parser = 0;
    cb->sym_trans = 0;
    cb->sbm = 0;
    return cb;
}
con_interface *init_client_con(char *name, int name_len,
                               int can_send_orders, gatekeeper * gk,
                               async_server_con_set * ass, int sock,
                               async_parse_args * ap, int num_orders)
{
    con_interface *cb = calloc(sizeof(con_interface),1 );
    if (pthread_spin_init(&cb->mutex, PTHREAD_PROCESS_PRIVATE) != 0) {
        printf("Failed to init fix_seq_blocker.\n");
    }
    if (pthread_spin_init(&cb->send_mutex, PTHREAD_PROCESS_PRIVATE) != 0) {
        printf("Failed to init send_mutex.\n");
    }
    cb->cl_ord_id_map =get_initial_order_map(gk, name, name_len);// create_map(num_orders);
    cb->module_name = calloc(name_len + 1, 1);
    memcpy(cb->module_name, name, name_len);
    cb->module_name_len = name_len;
    cb->fix_con_name = calloc(name_len + 1, 1);
    memcpy(cb->fix_con_name, name, name_len);
    cb->fix_con_len = name_len;
    cb->mess_to_send = databuf_alloc(1024);
    cb->session_conf = 0;
    cb->connection_conf = 0;
    cb->ass = ass;
    cb->gk = gk;
    cb->ap = ap;
    cb->sock = sock;
    cb->connect = 0;
    cb->build_offset = 0;
    cb->on_enter_order = 0;
    cb->on_cancel_order = 0;
    cb->on_replace_order = 0;
    cb->on_connect = on_connect;
    cb->ci_iovec_sender = ci_async_iovec_sender;
    cb->cleansing_rinse = ci_cleansing_rinse;
    cb->ip_str = calloc(64, 1);
    cb->disconnect = ci_disconnect;
    cb->reset_seq = ci_reset_seq;
    cb->can_send_orders = can_send_orders;
    cb->is_blocked = 0;
    cb->pendingblock = 0;
    cb->adminblockoverride = 0;
    cb->dest_id = 0;
    cb->dest_len = 0;
    cb->last_sent = 0;
    cb->last_recv = 0;
    cb->cancel_on_dis = 0;
    cb->do_oats_balance = 1;
    cb->should_buffer = 0;
    cb->recon_wrapper = 0;
    cb->block_wrapper = 0;
    cb->time_wrapper = 0;
    cb->parser = 0;
    cb->sym_trans = 0;
    cb->sbm = 0;
    return cb;
}

static int create_start_timer(struct connection_interface* cb)
{
    time_t now;
    time(&now);
    struct tm *ti;
    struct tm lm;
    ti = localtime_r(&now, &lm);
    ti = &lm;
    int timerCreated = 0;
    int cur_hour = ti->tm_hour;
    int cur_min = ti->tm_min;
    int end_hour = cb->connection_conf->end_hour;
    int end_min = cb->connection_conf->end_min;
    int start_hour = cb->connection_conf->start_hour;
    int start_min = cb->connection_conf->start_min;
            send_debug_message
                ("Cur Hor: %d , CurMin: %d, endHour: %d, endMin %d, startHour: %d start min %d.\n",
                 cur_hour, cur_min, end_hour, end_min, start_hour, start_min);
    if(end_hour > start_hour && cur_hour >= end_hour && cur_min >= end_min) {
        int set = get_sec_till_end_time(start_hour, start_min, 7,ti);
        send_debug_message("Setting the recon wrapper to reconnect in : %d seconds \n", set);
        cb->recon_wrapper = add_timer(cb->ass, 0,
                                0, set, 0, cb->ap,async_reconnect_t);
        timerCreated = 1;
    } 
    return timerCreated;
}

void on_connect(struct connection_interface *cb, int is_connected)
{
    switch (is_connected) {
    case 0:
        cb->can_send_orders = 0;
        cb->connected = 0;
        if (!cb->ap->is_server) {
            reload_config(cb->connection_conf);
            reload_init_args(cb->session_conf);
            cb->ap->port = cb->connection_conf->fix_port;
            cb->ap->ip = (char *) cb->connection_conf->fix_ip;
            cb->ap->ip_len = cb->connection_conf->fix_ip_len;
            int noStart = 1;
            //off_thread_reconnect(cb->ap);
            //if (should_session_up(cb)) {
            if(cb->connection_conf->useStart) {
                noStart = create_start_timer(cb);
            } 
            if(noStart) {
                if(cb->recon_wrapper == 0) {
                    cb->recon_wrapper = add_timer(cb->ass, 0,
                                0, 30, 0, cb->ap,async_reconnect_t);
                } else {
                    cb->recon_wrapper->stuff = cb->ap;
                    cb->recon_wrapper->call_back_func = async_reconnect_t;
                    add_back_recon_timer(cb->ass, cb->recon_wrapper);
                }
            }
        } else if (cb->cancel_on_dis) {
            if (cb->on_cancel_all != 0) {
                cb->on_cancel_all(cb, NULL);
            }
        }
        break;
    case 1:
        cb->can_send_orders = 1;
        cb->connected = 1;
        if (databuf_unread(cb->mess_to_send) > 0) {
            ci_async_write_callback(cb->module_name, cb->module_name_len,
                                    cb);
        }
        break;
    case 2:
        cb->can_send_orders = 0;
        cb->connected = 1;
        break;
    }
}

#define set_last_sent(ci)  \
{                                          \
    time(&ci->last_sent);                  \
}                                          \

int con_send_mess(int sock, char *data, size_t len, async_parse_args * ap)
{
    int return_code = -1;
    int error;
    return_code = send(sock, data, len, MSG_DONTWAIT | MSG_NOSIGNAL);
    if (return_code < 0) {
        error = errno;
        if (error == EAGAIN || error == EWOULDBLOCK || error == ENOBUFS
            || error == EINTR || error == 0) {
            return_code = 0;
        } else {
            send_debug_message
                ("Got this error on send: %d , we will now disconnect.\n",
                 error);
            return_code = -1;
            ap->disconnect = 1;
        }
    }
    return return_code;
}

void ci_async_write_callback(const char *name, int name_len, void *cb)
{
    con_interface *ob = (con_interface *) cb;
    if (cb && pthread_spin_lock(&ob->send_mutex) == 0) {
        int unread = databuf_unread(ob->mess_to_send);
        int total_sent = 0;
        int keep_trying = 1;
        while (total_sent < unread && keep_trying) {
            int len_sent =
                con_send_mess(ob->sock, ob->mess_to_send->rd_ptr,
                              databuf_unread(ob->mess_to_send), ob->ap);
            if (len_sent == 0) {
                add_sender(ob->ass, ob, ob->sock);
                keep_trying = 0;
            } else if (len_sent < 0) {
                keep_trying = 0;
                /* We got an error and the socket is closing. */
                databuf_reset(ob->mess_to_send);
            } else {
                total_sent += len_sent;
                ob->mess_to_send->rd_ptr += len_sent;
            }
        }
        if (total_sent >= unread) {
            databuf_reset(ob->mess_to_send);
        }
        if (pthread_spin_unlock(&ob->send_mutex) != 0) {
            send_debug_message
                ("failed to unlock client map guard in async callback.  For this: %s \n",
                 name);
        }
        set_last_sent(ob);
    }
}
static int ci_send_bunches(struct iovec *miov, int iovcnt, int sock, int *err,
                    int *vec_sent)
{
    struct iovec *iov = miov;
    int in_iovec = 0;
    ssize_t ret_code = 0;
    ssize_t bytes_written = 0;
    while (iovcnt > 0 && (ret_code = writev(sock, iov, iovcnt)) >= 0) {
        bytes_written += ret_code;
        while (iov && iovcnt > 0) {
            if (iov->iov_len <= ret_code) {
                ret_code -= iov->iov_len;
                if(in_iovec) {
                    iov->iov_base -= in_iovec;
                    iov->iov_len += in_iovec;
                    in_iovec = 0;
                }
                ++iov;
                --iovcnt;
            } else {
                iov->iov_base += ret_code;
                iov->iov_len -= ret_code;
                in_iovec += ret_code;
                break;
            }
        }
    }
    if(in_iovec) {
        iov->iov_base -= in_iovec;
        iov->iov_len += in_iovec;
    }
    if (ret_code < 0) {
        int error = errno;
        if (error == EAGAIN || error == EWOULDBLOCK || error == ENOBUFS
            || error == EINTR || error == 0) {
            *err = 0;
        } else {
            *err = -1;
        }
    } else {
        *err = 1;
    }
    *vec_sent = iovcnt;
    return bytes_written;
}

int ci_async_send(char *name, int name_len, databuf_t * data, void *cb)
{
    con_interface *ob = (con_interface *) cb;
    int return_code = -1;
    int bytes_sent = 0;
    if (cb && pthread_spin_lock(&ob->send_mutex) == 0) {
        int total_len = databuf_unread(data);
        int error;
        int keep_sending = 1;
        if (databuf_unread(ob->mess_to_send) <= 0) {
            while (keep_sending && total_len > bytes_sent) {
                return_code =
                    send(ob->sock, data->rd_ptr, databuf_unread(data),
                         MSG_DONTWAIT | MSG_NOSIGNAL);
                if (return_code < 0) {
                    error = errno;
                    if (error == EAGAIN || error == EWOULDBLOCK
                        || error == ENOBUFS || error == EINTR
                        || error == 0) {
                        databuf_memcpy(ob->mess_to_send, data->rd_ptr,
                                       databuf_unread(data));
                        add_sender(ob->ass, ob, ob->sock);
                        keep_sending = 0;
                    } else {
                        send_debug_message
                            ("Got this error on send: %d , we will now disconnect.\n",
                             error);
                        keep_sending = 0;
                        ob->ap->disconnect = 1;
                    }
                } else {
                    bytes_sent += return_code;
                    data->rd_ptr += return_code;
                }
            }
            if (pthread_spin_unlock(&ob->send_mutex) != 0) {
                send_debug_message
                    ("failed to unlock client map guard in async callback.  For this: %s \n",
                     name);
            }
        } else {
            databuf_memcpy(ob->mess_to_send, data->rd_ptr,
                           databuf_unread(data));
            if (pthread_spin_unlock(&ob->send_mutex) != 0) {
                send_debug_message
                    ("failed to unlock client map guard in rom sender.  For this: %s \n",
                     name);
            }
            ci_async_write_callback(name, name_len, ob);
        }
        set_last_sent(ob);
    }
    return bytes_sent;
}

void copy_iovecs_to_buff(struct iovec *miov, int iovcnt,
                         databuf_t * buff, int bytes_sent)
{
    struct iovec* iov = miov;
    do {
        if (iov->iov_len <= bytes_sent) {
            bytes_sent -= iov->iov_len;
            ++iov;
        } else {
            break;
        }
    } while (bytes_sent > 0);
    //If bytes sent is > 0 then it is the remainder from a partial send
    //within an iovec.  
    while (iov && iovcnt > 0) {
        databuf_memcpy(buff, iov->iov_base + bytes_sent, iov->iov_len - bytes_sent);
        bytes_sent = 0;
        ++iov;
        --iovcnt;
    }
}

void ci_no_lock_sender(con_interface* con, databuf_t* buff)
{
    int bytes_sent = 0;
    int bytes_to_send = 0;
    if (databuf_unread(con->mess_to_send) > 0) {
        databuf_memcpy(con->mess_to_send, buff->rd_ptr, databuf_unread(buff));
        bytes_to_send = databuf_unread(con->mess_to_send);
        bytes_sent = con_send_mess(con->sock,con->mess_to_send->rd_ptr, 
                bytes_to_send, con->ap);
        if(bytes_sent >= 0) {
            con->mess_to_send->rd_ptr += bytes_sent;
            if (bytes_sent < bytes_to_send && con->ass != 0) {
                add_sender(con->ass, con, con->sock);
            }
        }
    } else {
        bytes_sent =
          con_send_mess(con->sock,buff->rd_ptr, databuf_unread(buff), con->ap);
        if (bytes_sent >= 0 && bytes_sent < databuf_unread(buff)) {
            databuf_memcpy(con->mess_to_send,
                    buff->rd_ptr + bytes_sent, databuf_unread(buff));
            if (con->ass != 0) {
                add_sender(con->ass, con, con->sock);
            }
        }
    }
    set_last_sent(con);
}

void ci_raw_iovec_sender(struct iovec *iov, int iovcnt, char *name,
                           int name_len, con_interface * ob, int sock)
{
    if (pthread_spin_lock(&ob->send_mutex) == 0) {
        int err = 0;
        int iov_unsent = 0;
        int bytes_sent = 0;
        if (databuf_unread(ob->mess_to_send) <= 0) {
            bytes_sent =
                ci_send_bunches(iov, iovcnt, sock, &err, &iov_unsent);
            if (err == 0) {
                send_debug_message
                    ("Just inserted map for this: %s, size:  %lu , sent_iov: %d, iov: %d , sent: %d, unsent: %s \n",
                     name, iov->iov_len,
                     iov_unsent, iovcnt, bytes_sent, (char*)iov->iov_base);
                copy_iovecs_to_buff(iov, iov_unsent, ob->mess_to_send,
                                    bytes_sent);
                if (ob->ass != 0) {
                    add_sender(ob->ass, ob, sock);
                }
            }
            if (pthread_spin_unlock(&ob->send_mutex) != 0) {
                send_debug_message
                    ("failed to unlock client map guard in ron sender.  For this: %s \n",
                     name);
            }
        } else {
            copy_iovecs_to_buff(iov, iovcnt, ob->mess_to_send, 0);
            if (pthread_spin_unlock(&ob->send_mutex) != 0) {
                send_debug_message
                    ("failed to unlock client map guard in rom sender.  For this: %s \n",
                     name);
            }
            ci_async_write_callback(name, name_len, ob);
        }
    }
    set_last_sent(ob);
}

void ci_async_iovec_sender(struct iovec *iov, int iovcnt, char *name,
                           int name_len, con_interface * ob, int sock)
{
    if (ob->can_send_orders) {
        ci_raw_iovec_sender(iov, iovcnt, name, name_len, ob, sock);
    } else {
        if (ob->should_buffer) {
             (pthread_spin_lock(&ob->send_mutex)) ;
            copy_iovecs_to_buff(iov, iovcnt, ob->mess_to_send, 0);
            (pthread_spin_unlock(&ob->send_mutex));
        }
    }
}

void add_int_orderids(con_interface * con)
{
    hashmap_iterator_t hi = hashmap_iterator_begin(con->cl_ord_id_map);
    dart_order_obj *buff = 0;
    queue_t q = create_queue();
    element_t e, temp;
    char *key = 0;
    int key_len = 0;
    int valid = hashmap_iterator_dereference(hi, &key,
                                             &key_len,
                                             (void *) &buff);
    while (valid) {
        enqueue(q, buff);
        valid = hashmap_iterator_next(con->cl_ord_id_map, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &buff);
    }
    hashmap_iterator_destroy(hi);

    e = dequeue(q);
    char ord_num[64];
    while (e) {
        buff = (dart_order_obj *) e->d;
        memset(ord_num, '\0', 64);
        memcpy(ord_num, buff->positions[ROM_CLORDID].iov_base,
               buff->positions[ROM_CLORDID].iov_len);
        unsigned long new_id = strtoul(ord_num, NULL, 10);
        no_copy_insert(con->cl_ord_id_map, (char *) &new_id, 4, buff);
        temp = e;
        e = temp->next;
        temp->d = NULL;
        free_element(q, temp);
    }
    destroy_queue(q);
}

unsigned long def_seq_parse(char *data,
                            unsigned long size,
                            unsigned long bo, unsigned long *seq, void* b)
{
    return 0;
}


int get_seconds_toblock(const dest_schedule* ds)
{
    time_t now;
    time(&now);
        
    int rc = ds->t_next_end - now;
    return rc;
 }

int get_seconds_tounblock(const dest_schedule* ds)
{
    time_t now;
    time(&now);
    
    int rc = ds->t_next_reset - now;
    return rc;
}

int should_session_up(const con_interface * ci)
{
    int rc = 1;
    
    //it should continue to connect if it is not blocked
    if (1 == ci->is_blocked)
    {
        if (ci->ds.dest_name)  //for connection that does not have a ds defined keep reconnecting on
        {
            if (ci->ds.t_next_reset <  ci->ds.t_next_end)
            {
                rc = 0;
            }
        }
    }
    
    return rc;
}

void localtime_to_centraltime_convert(const struct tm* lt, struct tm* ct)
{
    *ct = *lt;
    char buffer[10] = {0};
    int nwritten = strftime(buffer, sizeof buffer, "%Z", ct);
    if (nwritten == 3 &&
        (buffer[1] == 'S' || buffer[1] == 'D') &&
         buffer[2] == 'T')
    {
        switch (buffer[0])
        {
        case 'E':
            ct->tm_hour -= 1;
            break;
        case 'M':
            ct->tm_hour += 1;
            break;
        case 'P':
            ct->tm_hour += 2;
            break;
        }
    }

    mktime(ct);
}

int get_create_new(const dest_schedule* ds)
{
    int rc = 0;
    
    if (ds->t_next_reset <  ds->t_next_end)  //dead zone
    {
        rc = 1;
    }
    else
    {
        if (ds->t_lastlogon < ds->t_next_reset - ds->t_period)
        {
            rc = 1;
        }
    }
    
    return rc;
}
