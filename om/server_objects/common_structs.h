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
#include "hashmap.h"
#include "dart_obj_factory.h"
#include "dart_order_obj.h"
#include "message_queue.h"


typedef struct incoming_message {
    char *name;
    size_t name_len;
    size_t num;
} incoming_message;

/**
 * This structure is used by all detached threads in the system.
 * end_hour and end_min are the hour and min after which the
 * application should close.  The application day can be set to 0 - 6
 * applications that can be up for multiple days.  For applications
 * that just close at a certain time set the end_day to 7.
 */
typedef struct app_end_time {
    int end_hour;
    int end_min;
    int end_day;
} app_end_time;

/**
 * This structure keeps the start hour and min so that as connections
 * come and go we know wheather we need to clean out our sequence
 * files.  The create_new is used if this is a server side connection,
 * where we have no control over when clients login.  In this case we
 * have to use things such as last run time etc, keeping track of such
 * things is not part of this library.  It is up to the users to
 * figure out if on the first login a clients sequences should be
 * cleaned and setting create new = 1 will ensure that for a "server"
 * connection.
 */
typedef struct seq_reset_time {
    size_t reset_hour;
    size_t reset_min;
    int reset_day;
    int create_new;
    time_t last_logon;
    unsigned long new_incoming;
    unsigned long new_outgoing;
} seq_reset_time;

/**
 * This provides us with an open ended number of configuation
 * arguments.  The idea being we can use the same structure for fix
 * and rom connections.
 */
typedef struct init_args {
    char *name;
    char *sender_comp;
    char *target_comp;
    char *target_sub;
    char *sender_sub;
    char *username;
    char *password;
    char *fix_version;
    char* accesskey;
    char* secretkey;
    struct seq_reset_time *reset_time;
    dart_config *dc;
    int log_outgoing;
    int cancel_only;
    int do_balance;
    short name_len;
    short sc_len;
    short tc_len;
    short ts_len;
    short ss_len;
    short un_len;
    short p_len;
    short fv_len;
    short accesslen;
    short secretlen;
} init_args;

void destroy_init_args(struct init_args *in);
void destroy_init_args_list(queue_t list);
long make_long(const char* num, int size);
time_t convert_to_sec_since_1970(char* yyyymmddhhmmss);
long convert_to_nanosec_since_1970(char* yyyymmddhhmmss);
#if defined(__cplusplus)
}
#endif
#endif
