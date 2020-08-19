/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _COMMON_STRUCTS_H__
#define _COMMON_STRUCTS_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "config_loader.h"
#include "databuf.h"
#include "dart_connection_object.h"
#include "message_queue.h"

#define LOGON_LEN = 15

    /**
     * Pass this function your date string and it will return 
     * a time_t.
     */
    time_t convert_to_sec_since_1970(char* yyyymmddhhmmss);

    /**
     * When you cannon use atoi because you only want to 
     * use some of the numbers in the string.
     */
    long  make_long(const char* p, int size);

    /* This is the structure used to pass data between this library
     * and its users and visa versa.  The name is used to identify the
     * connection from which the order came or to which we are sending
     * the message.
     */
    typedef struct message {
        char *name;
        size_t name_len;
        char *data;
        unsigned long len;
    } message;

    typedef struct incoming_message {
        char *name;
        size_t name_len;
        size_t num;
    } incoming_message;

    /* This structure is used by all detached threads in the system.
     * end_hour and end_min are the hour and min after which the
     * application should close.  The application day can be set to 0
     * - 6 applications that can be up for multiple days.  For
     * applications that just close at a certain time set the end_day
     * to 7.
     */
    typedef struct app_end_time {
        int end_hour;
        int end_min;
        int end_day;
    } app_end_time;

    /* This structure keeps the start hour and min so that as
     * connections come and go we know wheather we need to clean out
     * our sequence files.  The create_new is used if this is a server
     * side connection, where we have no control over when clients
     * login.  In this case we have to use things such as last run
     * time etc, keeping track of such things is not part of this
     * library.  It is up to the users to figure out if on the first
     * login a clients sequences should be cleaned and setting create
     * new = 1 will ensure that for a "server" connection.
     */
    typedef struct seq_reset_time {
        size_t reset_hour;
        size_t reset_min;
        int reset_day;
        time_t last_logon;
        unsigned long new_incoming;
        unsigned long new_outgoing;
        int create_new;
    } seq_reset_time;

    /* This provides us with an open ended number of configuation
     * arguments.  The idea being we can use the same structure for
     * fix and rom connections.
     */
    typedef struct init_args {
        char *name;
        size_t name_len;
        char *sender_comp;
        size_t sc_len;
        char *target_comp;
        size_t tc_len;
        char *target_sub;
        size_t ts_len;
        char *sender_sub;
        size_t ss_len;
        char *username;
        size_t un_len;
        char *password;
        size_t p_len;
        char *fix_version;
        size_t fv_len;
        struct seq_reset_time *reset_time;
        int log_outgoing;
        int cancel_only;
        dart_config *dc;
         size_t(*create_mess_header) (databuf_t * fixmsg,
                                      const char *sendercompid,
                                      size_t sendercomplen,
                                      const char *targetcompid,
                                      size_t targetcomplen,
                                      const char *sendersubid,
                                      size_t sendersublen,
                                      const char *targetsubid,
                                      size_t targetsublen,
                                      unsigned long seq, time_t * t);
    } init_args;

    void destroy_init_args(struct init_args *in);

    void destroy_init_args_list(queue_t list);

    int dart_itoa(int value, char *str, int radix);

    /* This structure is passed back to the parse function and gives
     * the parser pointers to all the functions that they may need to
     * properly handle a message from a client. The send_ptr function
     * is used to send messages back to the connection, the callback
     * function should be set up by users of this library to call into
     * the next level of parseing / order management.  The init_con
     * function will associate the name of the connection with the
     * socket and load up the last received sequence numbers for this
     * connection.  The client connection notice function pointer will
     * give this library a way to tell users that a connection has
     * come or gone, if the update is for a new connection then the
     * is_connected will be a non zero number, otherwise if the
     * connection has gone away is_connected will be zero.
     */
    typedef struct parse_con {
        int (*send_ptr) (struct message * mess);
        void (*callback) (struct message * mess, void *ob);
        void (*record_mess) (struct incoming_message * mess);
        int (*init_con) (con_obj * co);
        void (*remove_con) (con_obj * co);
        void (*connection_notice) (con_obj * co, int is_connected,
                                   char *logon, int logon_len);
        struct init_args *in_args;
        void *book;
        void *parser_helper;
        void *(*copy_parser_helper) (void *ph);
        void (*p_callback) (void *ph, struct message * m, void *ob);
        queue_t in_args_list;
        con_obj *co;
        int is_server;
        int is_logged_on;
        size_t timeout_val;
        app_end_time *t;
        char *mod_name;
        size_t mod_len;
    } parse_con;

    void destroy_parse_con(parse_con * pc);

    /* These funcion pointers allow a parsing object to communicate
     * with both the client (through the send_ptr) and the server
     * (through the callback). */
    typedef struct parse_args {
        unsigned long (*parse) (const char *message, unsigned long len,
                                int *cut_con, struct parse_con * pc);
        unsigned long (*seq_parse) (char *data,
                                    unsigned long size, unsigned long bo,
                                    unsigned long *seq);
        void (*low_activity_pulse) (con_obj * obj);
        struct parse_con *con;
        app_end_time *t;
        int check_activity;
    } parse_args;

#if defined(__cplusplus)
}
#endif
#endif                          //_CLIENT_MANAGER_H__
