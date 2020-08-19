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
//#include "transaction_recorder.h"
#include "message_queue.h"
#include "dart_connection_object.h"
#include "common_structs.h"

    typedef struct async_server async_server_con_set;

    typedef struct async_writer {
        struct epoll_event *ee;
        int efd;
        int sock;
    } async_writer;

    typedef struct async_parse_args {
        unsigned long (*parse) (const char *message, unsigned long len,
                                int *cut_con, struct parse_con * pc);
        unsigned long (*seq_parse) (char *data,
                                    unsigned long size, unsigned long bo,
                                    unsigned long *seq);
        void *write_pass_back;
        void (*write_callback) (const char *name, int name_len,
                                void *pass_back);
        struct parse_con *con;
        struct epoll_event *ee;
        int check_activity;
        int efd;
        char *buffer;
        char *wr_ptr;
        char *rd_ptr;
        uint64_t write_offset;
        uint64_t read_offset;
        uint64_t total_size;
        long num_bytes;
        struct async_server *ass;
        app_end_time *t;
        struct async_writer *asw;
        void (*low_activity_pulse) (con_obj * obj);
        void *(*async_cb_func) (void *v);
        int (*init_con) (struct async_parse_args * pa);
        int use_ud;
    } async_parse_args;


    void add_sender(async_server_con_set * set, void *pass_back, int sock);

    /**
     * Yes yuck is the correct response to void stars, but I am just
     * passing these arguments off to a thread so by convention they
     * become void anyway.
     *
     * Anyway the hidden contract in this function is that you must
     * pass in an server_con_set structure defined in
     * client_manager.h.  enjoy.
     */
     async_server_con_set
        * start_async_server(void (*fn_ptr) (struct message * m, void *ob),
                             const char *mod_name, size_t mod_len,
                             uint16_t port, queue_t args, const char *ip,
                             size_t len, int end_hour, int end_min,
                             int end_day, size_t timeout_val, void *ob,
                             void (*connection_callback) (con_obj * co,
                                                          int is_connected,
                                                          char *log,
                                                          int len),
                             unsigned long (*mess_parse) (const char
                                                          *message,
                                                          unsigned long
                                                          len,
                                                          int *cut_con,
                                                          struct parse_con
                                                          * pc),
                             unsigned long (*seq_parse) (char *data,
                                                         unsigned long
                                                         size,
                                                         unsigned long bo,
                                                         unsigned long
                                                         *seq),
                             void (*low_activity_pulse) (con_obj * obj),
                             char *(*create_reset_pad) (unsigned long seq,
                                                        char *ver,
                                                        size_t * len),
                             int check_activity, int num_threads,
                             int time_out, int *cpus, int set_affinity,
                             void *write_pass_back,
                             void (*write_callback) (const char *name,
                                                     int name_len,
                                                     void *pass_back),
                             void *parser_helper,
                             void *(*copy_parser_helper) (void *ph),
                             void (*p_callback) (void *ph,
                                                 struct message * m,
                                                 void *ob));
     /**
      * This function allows users to add a new port or unix domain socket to listen to. 
      */
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
                                             void *ob));

    /**
     * This will set some of the regular option that we always want on
     * our sockets, such as TCP_NODELAY and setting a RCVTIMEO and
     * SENDTIMO
     */
    int async_set_sock_opt(int sock);

    /**
     * Creates a connection object that will be put in the list of
     * asynchronous events.
     */
    int async_connection(void *a_server_set,
                         void (*client_callback) (struct message * m,
                                                  void *ob),
                         const char *mod_name, size_t mod_len,
                         uint16_t port, const char *ip, size_t ip_len,
                         int end_hour, int end_min, int end_day,
                         size_t timeout_val, void *ob,
                         void (*connection_callback) (con_obj * co,
                                                      int is_connected,
                                                      char *log, int len),
                         unsigned long (*client_mess_parse) (const char
                                                             *message,
                                                             unsigned long
                                                             len,
                                                             int *cut_con,
                                                             struct
                                                             parse_con *
                                                             pc),
                         void *args,
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
                                             void *ob));

    void disconnect_by_name(async_server_con_set * set, const char *name,
                            int name_len, int sock);

    void add_new_user(async_server_con_set* set, struct async_parse_args* pa, const char* name,
                      int name_len, struct init_args* in);
    void alter_user(async_server_con_set* set, struct async_parse_args* pa, 
                    const char* name, int name_len,
                    struct init_args* in);
    void add_job_to_queue(async_server_con_set* ascs, void *(*func) (void *), void *arg);

    void clean_write_event_queue(async_server_con_set* set);
    void remove_listener(async_server_con_set* set,
                         async_parse_args* pa) ;
#if defined(__cplusplus)
}
#endif
#endif                          //SERVER_SOCKET_H__
