/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _CLIENT_MANAGER_H__
#define _CLIENT_MANAGER_H__

#if defined(__cplusplus)
extern "C" {
#endif
#include "message_queue.h"
#include "databuf.h"
#include "common_structs.h"
#include "async_server.h"
/*
#include "transaction_recorder.h"
#include "dart_connection_object.h"
#include "dart_thr_pool.h"
#include "config_loader.h"
*/
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 16384
#define SOCK_READ_SIZE 16380
#define MAXCONNECTIONS 200

    /**
     * This struct is sent to the listening thread.  It contains all
     * the configuration information for the parsers.
     */
    typedef struct server_con_set {
        struct parse_args *pa;
        uint16_t port;
        char *ip;
        int ip_len;
        int use_ud;
    } server_con_set;

    struct init_args *copy_init_args(struct init_args *in);

    int get_sock_for_con(const char *name, int name_len);

    int send_bunches(struct iovec *iov, int iovcnt, int sock, int *err,
                     int *vec_sent);

    async_server_con_set *async_start_dart_client_services(void (*fn_ptr)
                                                            (struct message
                                                             * m,
                                                             void *ob),
                                                           const char
                                                           *mod_name,
                                                           size_t mod_len,
                                                           uint16_t port,
                                                           queue_t args,
                                                           const char *ip,
                                                           size_t len,
                                                           int end_hour,
                                                           int end_min,
                                                           int end_day,
                                                           size_t
                                                           timeout_val,
                                                           void *ob, void
                                                            (*connection_callback)
                                                            (con_obj * co,
                                                             int
                                                             is_connected,
                                                             char *log,
                                                             int len),
                                                           unsigned
                                                           long
                                                            (*mess_parse)
                                                            (const char
                                                             *message,
                                                             unsigned long
                                                             len,
                                                             int *cut_con,
                                                             struct
                                                             parse_con *
                                                             pc), unsigned
                                                           long
                                                            (*seq_parse)
                                                            (char *data,
                                                             unsigned long
                                                             size,
                                                             unsigned long
                                                             bo,
                                                             unsigned long
                                                             *seq), void
                                                            (*low_activity_pulse)
                                                            (con_obj *
                                                             obj), char
                                                           *
                                                           (*create_reset_pad)
                                                            (unsigned long
                                                             seq,
                                                             char *ver,
                                                             size_t * len),
                                                           int
                                                           check_activity,
                                                           int num_threads,
                                                           int time_out,
                                                           int *cpus, int
                                                           set_affinity, void
                                                           *write_pass_back, void
                                                            (*write_callback)
                                                            (const char
                                                             *name,
                                                             int name_len,
                                                             void
                                                             *pass_back));

    async_server_con_set *async_start_dart_client_services_2(void (*fn_ptr)
                                                              (struct
                                                               message * m,
                                                               void *ob),
                                                             const char
                                                             *mod_name,
                                                             size_t
                                                             mod_len,
                                                             uint16_t port,
                                                             queue_t args,
                                                             const char
                                                             *ip,
                                                             size_t len,
                                                             int end_hour,
                                                             int end_min,
                                                             int end_day,
                                                             size_t
                                                             timeout_val,
                                                             void *ob, void
                                                              (*connection_callback)
                                                              (con_obj *
                                                               co,
                                                               int
                                                               is_connected,
                                                               char *log,
                                                               int len),
                                                             unsigned
                                                             long
                                                              (*mess_parse)
                                                              (const char
                                                               *message,
                                                               unsigned
                                                               long len,
                                                               int
                                                               *cut_con,
                                                               struct
                                                               parse_con *
                                                               pc),
                                                             unsigned
                                                             long
                                                              (*seq_parse)
                                                              (char *data,
                                                               unsigned
                                                               long size,
                                                               unsigned
                                                               long bo,
                                                               unsigned
                                                               long *seq),
                                                             void
                                                              (*low_activity_pulse)
                                                              (con_obj *
                                                               obj), char
                                                             *
                                                             (*create_reset_pad)
                                                              (unsigned
                                                               long seq,
                                                               char *ver,
                                                               size_t *
                                                               len), int
                                                             check_activity, int
                                                             num_threads,
                                                             int time_out,
                                                             int *cpus, int
                                                             set_affinity, void
                                                             *write_pass_back, void
                                                              (*write_callback)
                                                              (const char
                                                               *name,
                                                               int
                                                               name_len,
                                                               void
                                                               *pass_back),
                                                             void
                                                             *parser_helper, void
                                                             *
                                                             (*copy_parser_helper)
                                                              (void *ph), void
                                                              (*p_callback)
                                                              (void *ph,
                                                               struct
                                                               message * m,
                                                               void *ob));

    int async_dart_connect_2(void (*client_callback)
                              (struct message * m, void *ob),
                             const char *mod_name, size_t mod_len,
                             uint16_t port, const char *ip,
                             size_t len, int end_hour,
                             int end_min, int end_day,
                             size_t timeout_val, void *ob,
                             void (*connection_callback) (con_obj
                                                          * co,
                                                          int
                                                          is_connected,
                                                          char
                                                          *log,
                                                          int
                                                          len), unsigned
                             long (*client_mess_parse) (const char
                                                        *message,
                                                        unsigned
                                                        long len,
                                                        int
                                                        *cut_con,
                                                        struct
                                                        parse_con
                                                        * pc),
                             struct init_args *args,
                             int (*init_con) (struct
                                              async_parse_args *
                                              pa),
                             unsigned long (*seq_parse) (char
                                                         *data,
                                                         unsigned
                                                         long
                                                         size,
                                                         unsigned
                                                         long bo,
                                                         unsigned
                                                         long
                                                         *seq),
                             void (*low_activity_pulse) (con_obj *
                                                         obj),
                             int check_activity,
                             async_server_con_set * ascs,
                             void *write_pass_back,
                             void (*write_callback) (const char
                                                     *name,
                                                     int name_len,
                                                     void
                                                     *pass_back),
                             void *parser_helper,
                             void *(*copy_parser_helper) (void
                                                          *ph),
                             void (*p_callback) (void *ph,
                                                 struct message *
                                                 m, void *ob));

    int async_dart_connect(void (*client_callback)
                            (struct message * m, void *ob),
                           const char *mod_name, size_t mod_len,
                           uint16_t port, const char *ip,
                           size_t len, int end_hour, int end_min,
                           int end_day, size_t timeout_val,
                           void *ob,
                           void (*connection_callback) (con_obj *
                                                        co,
                                                        int
                                                        is_connected,
                                                        char *log,
                                                        int len), unsigned
                           long (*client_mess_parse) (const char
                                                      *message,
                                                      unsigned
                                                      long len,
                                                      int
                                                      *cut_con,
                                                      struct
                                                      parse_con *
                                                      pc),
                           struct init_args *args,
                           int (*init_con) (struct
                                            async_parse_args *
                                            pa),
                           unsigned long (*seq_parse) (char *data,
                                                       unsigned
                                                       long size,
                                                       unsigned
                                                       long bo,
                                                       unsigned
                                                       long *seq),
                           void (*low_activity_pulse) (con_obj *
                                                       obj),
                           int check_activity,
                           async_server_con_set * ascs,
                           void *write_pass_back,
                           void (*write_callback) (const char
                                                   *name,
                                                   int name_len,
                                                   void *pass_back));

    typedef struct async_client_con_set {
        struct async_parse_args *pa;
        uint16_t port;
        const char *host;
        int (*init_connection) (struct async_parse_args * pc);
    } async_client_con_set;

    void async_kick_off_connection(async_server_con_set * ascs, char *name,
                                   size_t name_len);

    int queue_async_send(async_server_con_set * set, const char *name,
                         int name_len, void *pb);

    int linux_only_dart_send(const char *name, int name_len,
                             const char *data, size_t len);

    typedef struct client_con_set {
        struct parse_args *pa;
        uint16_t port;
        int use_ud;
        const char *host;
        int host_len;
        int (*init_connection) (struct parse_args * pc);
    } client_con_set;

    typedef void (*fp) (struct message *);

    /**
     * This starts the server process and the sequence number mapping
     * engine, a thread will be created that binds to the port passed
     * in.  The first parameter is a callback function for messages
     * from clients. Each Client connection is serviced in its own
     * thread with results sent to the function pointer passed in.
     * removed: int reset_hour, int reset_min, int create_new, int
     * reset_hour, int reset_min,int create_new, The orderbook pointer
     * is something that should last the lifetime of the application.
     * You pass in the pointer, NO COPIES WILL BE MADE, this is
     * treated as a token that will not be parsed by the underlying
     * system but will be passed back with all message updates.
     */
    int
     start_dart_client_services(void (*fn_ptr) (struct message * m,
                                                void *ob),
                                const char *mod_name, size_t mod_len,
                                uint16_t port, queue_t args,
                                const char *ip, size_t len,
                                int end_hour, int end_min, int end_day,
                                size_t timeout_val, void *ob,
                                void (*connection_callback) (con_obj *
                                                             co,
                                                             int
                                                             is_connected,
                                                             char *log,
                                                             int len),
                                unsigned long (*mess_parse) (const char
                                                             *message,
                                                             unsigned
                                                             long len,
                                                             int
                                                             *cut_con,
                                                             struct
                                                             parse_con *
                                                             pc),
                                unsigned long (*seq_parse) (char *data,
                                                            unsigned
                                                            long size,
                                                            unsigned
                                                            long bo,
                                                            unsigned
                                                            long *seq),
                                void (*low_activity_pulse) (con_obj *
                                                            obj),
                                char *(*create_reset_pad) (unsigned long
                                                           seq,
                                                           char *ver,
                                                           size_t *
                                                           len),
                                int check_activity);

    int
     start_dart_client_services_2(void (*fn_ptr) (struct message * m,
                                                  void *ob),
                                  const char *mod_name, size_t mod_len,
                                  uint16_t port, queue_t args,
                                  const char *ip, size_t len,
                                  int end_hour, int end_min,
                                  int end_day, size_t timeout_val,
                                  void *ob,
                                  void (*connection_callback) (con_obj *
                                                               co,
                                                               int
                                                               is_connected,
                                                               char
                                                               *log,
                                                               int len),
                                  unsigned long (*mess_parse) (const
                                                               char
                                                               *message,
                                                               unsigned
                                                               long len,
                                                               int
                                                               *cut_con,
                                                               struct
                                                               parse_con
                                                               * pc),
                                  unsigned long (*seq_parse) (char
                                                              *data,
                                                              unsigned
                                                              long size,
                                                              unsigned
                                                              long bo,
                                                              unsigned
                                                              long
                                                              *seq),
                                  void (*low_activity_pulse) (con_obj *
                                                              obj),
                                  char *(*create_reset_pad) (unsigned
                                                             long seq,
                                                             char *ver,
                                                             size_t *
                                                             len),
                                  int check_activity,
                                  void *parser_helper,
                                  void *(*copy_parser_helper) (void
                                                               *ph),
                                  void (*p_callback) (void *ph,
                                                      struct message *
                                                      m, void *ob));

    /**
     * This function connects to another process at the requested IP
     * and port.  the 'client_callback' function pointer should be a
     * function that will take whole messages and process them.  The
     * 'client_mess_parse' function will be given a string of raw
     * bytes and it must determine how many if any messages are in the
     * bytes and return the amount of bytes it used and no longer
     * wishes to be updated with.  Users are free to create their own
     * 'client_mess_parse' The orderbook structure will not be parsed
     * by the underlying framework
     */
    int dart_connect(void (*client_callback) (struct message * m,
                                              void *ob),
                     const char *mod_name, size_t mod_len,
                     uint16_t port, const char *ip, size_t len,
                     int end_hour, int end_min, int end_day,
                     size_t timeout_val, void *ob,
                     void (*connection_callback) (con_obj * co,
                                                  int
                                                  is_connected,
                                                  char *log,
                                                  int len),
                     unsigned long (*client_mess_parse) (const
                                                         char
                                                         *message,
                                                         unsigned
                                                         long len,
                                                         int
                                                         *cut_con,
                                                         struct
                                                         parse_con
                                                         * pc),
                     struct init_args *args,
                     int (*init_con) (struct parse_args * pa),
                     unsigned long (*seq_parse) (char *data,
                                                 unsigned long
                                                 size,
                                                 unsigned long bo,
                                                 unsigned long
                                                 *seq),
                     void (*low_activity_pulse) (con_obj * obj),
                     int check_activity);

    int dart_connect_2(void (*client_callback)
                        (struct message * m, void *ob),
                       const char *mod_name, size_t mod_len,
                       uint16_t port, const char *ip, size_t len,
                       int end_hour, int end_min, int end_day,
                       size_t timeout_val, void *ob,
                       void (*connection_callback) (con_obj * co,
                                                    int
                                                    is_connected,
                                                    char *log,
                                                    int len),
                       unsigned long (*client_mess_parse) (const
                                                           char
                                                           *message,
                                                           unsigned
                                                           long
                                                           len,
                                                           int
                                                           *cut_con,
                                                           struct
                                                           parse_con
                                                           * pc),
                       struct init_args *args,
                       int (*init_con) (struct parse_args * pa),
                       unsigned long (*seq_parse) (char *data,
                                                   unsigned long
                                                   size,
                                                   unsigned long
                                                   bo,
                                                   unsigned long
                                                   *seq),
                       void (*low_activity_pulse) (con_obj * obj),
                       int check_activity, void *parser_helper,
                       void *(*copy_parser_helper) (void *ph),
                       void (*p_callback) (void *ph,
                                           struct message * m, void *ob));

    /**
     * This is the function that users of the library call to send
     * data to remote clients.  If clients do not use this function to
     * send their messages then those messages will not be added to
     * the sequence files and will not be available for resend.
     */
    int send_message(struct message *mess);

    /**
     * Clients call this function to log messages sent to connections.
     */
    void record_sent_message(const struct message *mess);

    void record_sent_message_iovec(struct iovec *iov, int iovcnt,
                                   const char *name, int name_len);

    int dart_send_message(con_obj * sock, char *data, size_t len);

    void dart_rolling_log_coming(const struct message *mess,
                                 const char *log_name,
                                 size_t log_name_len);

    void dart_rolling_log_going(const struct message *mess,
                                const char *log_name, size_t log_name_len);

    void dart_rolling_log_going_iovec(struct iovec *iov, int iovcnt,
                                      const char *name, int name_len,
                                      const char *log_name,
                                      int log_name_len);

    /**
     * Called by the receiving threads when they parse the login
     * messages.  The con_object will have its in_seq and out_seq
     * members filled with th last received incoming and outgoing
     * sequence numbers for that particular connection.
     */
    int add_connection(con_obj * cobj);

    /**
     * This will remove the connection from our map.
     */
    void remove_connection(con_obj * con);

    /**
     * This will search for a connection object with the name passed
     * in. It will send the receive thread a disconnect notice.
     */
    void kick_off_connection(char *name, size_t name_len);

    /**
     * This function will return the init_args structure for any known
     * connection.
     */
    struct init_args *find_config(char *name, size_t name_len);

    int check_is_connected(con_obj * co);

    /**
     * Add a init_args struct associated with a connection name.
     */
    void add_config(struct init_args *pa);

    /**
     * Return value > 0 when a init_arg is successfully removed.
     */
    int remove_config(char *name, size_t name_len);

    /**
     * Updates a init_arg with the new values.
     */
    int update_config(struct init_args *new_init);

    /**
     * This will return 1 if h matches the current hour and the m is
     * >= the current min or the current min is not more than 5 min
     * greater than m.
     */
    int compare_times(int h, int m);

    queue_t set_parse_args_from_list(queue_t args,
                                        char *(*create_reset_pad) (unsigned
                                                                   long
                                                                   seq,
                                                                   char
                                                                   *ver,
                                                                   size_t *
                                                                   len));
#if defined(__cplusplus)
}
#endif
#endif                          //_CLIENT_MANAGER_H__
