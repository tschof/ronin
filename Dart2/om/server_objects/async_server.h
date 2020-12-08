/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _ASYNC_SERVER_H__
#define _ASYNC_SERVER_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include "transaction_recorder.h"
#include "message_queue.h"
#include "common_structs.h"
#include "gatekeeper.h"
#include "master_config.h"
#include "sock_recv_manager.h"
#include "call_back_wrapper.h"

/** @file async_server.h Definition of the socket and thread level functions.*/

#define BUFFER_SIZE 65536
#define MAXCONNECTIONS 200

/**
 * \typedef async_server async_server_con_set 
 * This object controls the epoll instance.
 * A handle to it is needed to start any 
 * network or timer related business.
 * @see start_async_server()
 */
typedef struct async_server async_server_con_set;
/**
 * \typedef struct async_parse_args
 * This is a public object that can be tailored to a specific 
 * connection type. Every connection made through async_connection()
 * or when another entity connects to a listening socket started with 
 * add_async_listener() creates an async_parse_args which will be
 * sent back to the user through the connection interface.
 * @see async_connection()
 * @see add_async_listener()
 */
typedef struct async_parse_args {
    void (*parse) (int *cut_con, struct async_parse_args * pc); /*!<   */
    /*unsigned long (*seq_parse) (char *data,
                                unsigned long size, unsigned long bo,
                                unsigned long *seq, void* b);*/
    void *write_pass_back; /*!<   */
    void (*write_callback) (const char *name, int name_len,
                            void *pass_back);  /*!<   */
    void (*connection_notice) (struct async_parse_args * co,
                               int is_connected, char *logon,
                               int logon_len);  /*!<   */
    struct init_args *in_args; /*!<   */
    queue_t in_args_list;  /*!<   */
   // master_config *mc;  /*!<   */
    int is_server;  /*!<   */
    int is_logged_on;  /*!<   */
    sock_recv_manager *con;  /*!<   */
    call_back_wrapper* read_wrapper;  /*!<   */
    call_back_wrapper* write_wrapper;  /*!<   */
    void* phelp;
    char *ip;  /*!<   */
    int ip_len;  /*!<   */
    int port;  /*!<The port of this connection.   */
    gatekeeper *gk;  /*!< a pointer to the underlying gatekeeper object.
                          The gatekeeper controls several object factories that
                          are used by the async_server and throughout the library.
                          @see \file gatekeeper.h  */
    void *ds_book;   /*!< This pointer is returend on callbacks and can be used by users to stash 
                          connection specific objects.   */ 
    struct async_server *ass;  /*!<A pointer to the \struct async_server object that
                                   this async_parse_args is associated with.   */
    void *(*async_cb_func) (void *v);  /*!<This is the user defined callback
                                           function the v will be the ds_book pointer.*/
    int (*init_con) (struct async_parse_args * pa);  /*!<   */
    void (*parse_v) (char *data, long len, void *pc);  /*!<   */
    void (*obj_callback) (dart_order_obj * ph, void *ob);  /*!<   */
    int in_seq;  /*!<   */
    int out_seq;  /*!<   */
    int disconnect;  /*!<   */
    int sock;  /*!<   */
    int removed;
} async_parse_args;

/**
 *
 */
void add_signal_listener(async_server_con_set* ass, call_back_wrapper* cbw);

void remove_and_delete_timer(async_server_con_set* ass, call_back_wrapper* cbw);
/**
 * This creates a timer with all the fixins.  This returns a call_back_wrapper
 * that you can use to reference the timer. Pass in zero for both the interval
 * parameters if you want a one shot timer, otherwise fill out all values.
 */
call_back_wrapper* add_timer(async_server_con_set* ass, int repeat_interval_sec,
                             int repeat_interval_nano, int initial_interval_sec,
                             int initial_interval_nano, void* cb_object,
                             void* (*t_cb_func)(void* cb));

void add_back_recon_timer(async_server_con_set* ass, call_back_wrapper* cbw);
void disconnect_by_name(async_server_con_set * set, const char *name,
                        int name_len);
void disconnect(struct async_parse_args *pa);
void add_sender(async_server_con_set * set, void *pass_back, int sock);
int check_is_connected(async_server_con_set * set, char *name,
                       int name_len);
int remove_connection(async_server_con_set * set, char *name,
                      int name_len, async_parse_args * apa);
int add_connection(async_server_con_set * set, char *name,
                   int name_len, async_parse_args * apa);

/* Yes yuck is the correct response to void stars, but I am just
 * passing these arguments off to a thread so by convention they
 * become void anyway. Anyway the hidden contract in this function
 * is that you must pass in an server_con_set structure. enjoy. */
async_server_con_set
    * start_async_server(gatekeeper * mb, void* ra,
                         int num_threads,
                         int time_out, int *cpus, 
                         int depth);
/**
 * This allows users to begin listening on a port after starting the main
 * framework.
 */
int add_async_listener(async_server_con_set* scs,gatekeeper * mb, master_config * mc,
                         const char *mod_name, size_t mod_len,
                         uint16_t port, queue_t l_args, const char *ip,
                         size_t len, int end_hour, int end_min,
                         int end_day, size_t timeout_val,
                         void (*connection_callback) (async_parse_args * co,
                                                      int is_connected,
                                                      char *log, int len),
                         void(*mess_parse) (int *cut_con,
                                                      struct async_parse_args *
                                                      pc),
                         void *write_pass_back,
                         void (*write_callback) (const char *name,
                                                 int name_len,
                                                 void *pass_back),
                         void (*parse_v) (char *data, long len, void *pc),
                         void (*obj_callback) (dart_order_obj * ph,
                                               void *ob));
       
/* This will set some of the regular option that we always want on
 * our sockets, such as TCP_NODELAY and setting a RCVTIMEO and
 * SENDTIMO */
int async_set_sock_opt(int sock);
void *async_reconnect_t(void * arg);

/* We have already tried so reuse the connection.  This is only
 * used when we have gotten far enought to create a connection. */
void *async_reconnect(async_parse_args * arg);

/* Creates a connection object that will be put in the list of
 * asynchronous events. */
int async_connection(async_server_con_set * a_server_set,
                     const char *mod_name, size_t mod_len,
                     uint16_t port, const char *ip, size_t ip_len,
                     int end_hour, int end_min, int end_day,
                     size_t timeout_val, void *ob,
                     void (*connection_callback) (async_parse_args * co,
                                                  int is_connected,
                                                  char *log, int len),
                     void (*client_mess_parse) (int *cut_con,
                                                         struct
                                                         async_parse_args *
                                                         pc),
                     void *args,
                     int (*init_con) (struct async_parse_args * pa),
                     void *write_pass_back,
                     void (*write_callback) (const char *name,
                                             int name_len,
                                             void *pass_back),
                     void (*parse_v) (char *data, long len, void *pc),
                     void (*obj_callback) (dart_order_obj * ph, void *ob));

struct async_parse_args
*create_parse_args(const char *mod_name, size_t mod_len, uint16_t port,
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
                   int sock, void *write_pass_back,
                   void (*write_callback) (const char *name,
                                           int name_len,
                                           void *pass_back), int is_ud,
                   void (*parse_v) (char *data, long len, void *pc),
                   void (*obj_callback) (dart_order_obj * ph, void *ob));


void join_n_clean(async_server_con_set* ass);

void async_stop_system(async_server_con_set* set);
void add_back_rep_timer(async_server_con_set* ass, call_back_wrapper* cbw);
void add_back_rep_timer_adjust(async_server_con_set* ass, call_back_wrapper* cbw, int new_sec);

#if defined(__cplusplus)
}
#endif
#endif
