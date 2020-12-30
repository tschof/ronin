/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _CONNECTION_INTERFACE_H__
#define _CONNECTION_INTERFACE_H__

#if defined(__cplusplus)
extern "C" {
#endif
#include <pthread.h>

#include "hashmap.h"
#include "async_server.h"
#include "common_structs.h"
#include "order_token_generator.h"
#include "service_bureau.h"
/**
 */
typedef struct DEST_SCHEDULE
{
    char* dest_name;
    int dest_name_len;
    time_t t_next_end;
    time_t t_next_reset;
    time_t t_period;    //daily 86400, weekly 86400 * 7 and any strange session!!!
    time_t t_lastlogon; 
} dest_schedule;

typedef struct connection_interface {
    int (*connect) (struct async_parse_args *);
    int (*build_offset) (struct async_parse_args *);
    void (*client_mess_parse) (int *cut_con,
                                        struct async_parse_args * pc);
    struct ex_con *connection_conf;
    struct init_args *session_conf;
    pthread_spinlock_t mutex;
    pthread_spinlock_t send_mutex;
    int connected;
    int should_buffer;
    hashmap_t cl_ord_id_map;
    char *ip_str;
    char *module_name;
    size_t module_name_len;
    char *fix_con_name;
    size_t fix_con_len;
    gatekeeper *gk;
    async_parse_args *ap;
    async_server_con_set *ass;
    databuf_t *mess_to_send;
    call_back_wrapper* time_wrapper;
    call_back_wrapper* recon_wrapper;
    call_back_wrapper* block_wrapper;
    time_t stop_time;
    time_t last_sent;
    time_t last_recv;
    char *dest_id;
    int can_send_orders;
    int is_blocked;
    int adminblockoverride;   // when this flag is on timed block/unblock should do nothing.
    int pendingblock; // 1: to be blocked, 0: to be unblocked
    int cancel_on_dis;
    int do_oats_balance;
    int sock;
    int dest_len;
    token_gen* tg;
    void* parser;
    void* sym_trans;
    sbureau_match* sbm;
    
    dest_schedule ds;
            
    void (*ci_iovec_sender) (struct iovec * iov, int iovcnt,
                             char *name, int name_len,
                             struct connection_interface * ob, int sock);
    void (*on_reject) (struct connection_interface *,
                       dart_order_obj * doj);
    void (*on_out) (struct connection_interface *, dart_order_obj * doj);
    void (*on_replaced) (struct connection_interface *,
                         dart_order_obj * doj);
    void (*on_fill) (struct connection_interface *, dart_order_obj * doj);

    /* Member functions to handle each different type of order
     * action to be sent through this connection interface. */
    int (*on_enter_order) (struct connection_interface *,
                           dart_order_obj *, struct async_parse_args *);
    int (*on_cancel_order) (struct connection_interface *,
                            dart_order_obj *);
    void (*on_cancel_all) (struct connection_interface *,
                           dart_order_obj *);
    void (*on_replace_order) (struct connection_interface *,
                              dart_order_obj *);
    void (*on_connect) (struct connection_interface * ci,
                        int is_connected);
    void (*ci_parse_v) (char *data, long len, void *pc);
    void (*ci_obj_callback) (dart_order_obj * ph, void *ob);
    void (*disconnect) (struct connection_interface *);
    void (*reset_seq) (struct connection_interface *, int out, int in);
    void (*cleansing_rinse) (struct connection_interface *);
    unsigned int seq;
} con_interface;

con_interface *create_empty_interface();

void reset_con_interface(con_interface * ci,
                         int num_orders, char *name, int name_len,
                         char *send_name, int send_len,
                         int (*connect) (struct async_parse_args *),
                         int (*build_offset) (struct async_parse_args
                                              *),
                         int (*on_enter_order) (struct
                                                connection_interface *,
                                                dart_order_obj *,
                                                struct async_parse_args *),
                         int (*on_cancel_order) (struct
                                                 connection_interface
                                                 *, dart_order_obj *),
                         void (*on_replace_order) (struct
                                                   connection_interface
                                                   *,
                                                   dart_order_obj *),
                         gatekeeper * gk, char *dest_id, int dest_id_len);

/* This is used to create connection interfaces for incoming clients
 * to the ssystem. */
con_interface *init_client_con(char *name, int name_len,
                               int can_send_orders, gatekeeper * gk,
                               async_server_con_set * ass, int sock,
                               async_parse_args * ap, int num_orders);
unsigned long def_seq_parse(char *data,
                            unsigned long size,
                            unsigned long bo, unsigned long *seq, void* b);
void ci_async_write_callback(const char *name, int name_len, void *cb);
void ci_async_iovec_sender(struct iovec *iov, int iovcnt, char *name,
                           int name_len, con_interface * ob, int sock);
void ci_raw_iovec_sender(struct iovec *iov, int iovcnt, char *name,
                           int name_len, con_interface * ob, int sock);
void ci_no_lock_sender(con_interface* ob, databuf_t* db);
int ci_async_send(char *name, int name_len, databuf_t * data, void *cb);
void add_int_orderids(con_interface * con);
void copy_iovecs_to_buff(struct iovec *iov, int total,
                         databuf_t * buff, int bytes_sent);
void timed_recon(con_interface* con);

void ci_reload_ip_port(struct connection_interface *ci);
void on_connect(struct connection_interface *ci, int is_connected);
void ci_cleansing_rinse(struct connection_interface *ci);

void localtime_to_centraltime_convert(const struct tm* lt, struct tm* ct);
int get_seconds_toblock(const dest_schedule* ds);
int get_seconds_tounblock(const dest_schedule* ds);
int should_session_up(const con_interface * ci);
int get_create_new(const dest_schedule* ds);

#if defined(__cplusplus)
}
#endif
#endif
