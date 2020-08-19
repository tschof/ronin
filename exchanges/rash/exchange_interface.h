#ifndef __EXCHANGE_INTERFACE_H__
#define __EXCHANGE_INTERFACE_H__

#include "server_objects/dart_connection_object.h"
#include "server_objects/common_structs.h"
#include "ex_common/order_token_creator.h"

#include "dstring.h"
#include "id_generator.h"
#include "dart_std_id_generator.h"

#include <time.h>
#include <stdint.h>

struct exchange_interface;
struct rom_parser_helper;

struct exchange_interface_connection {
    void (*on_connection_update) (con_obj *, int, char *, int);
    unsigned long (*frame_message) (const char *, unsigned long, int *,
                                    struct parse_con *);
    void (*on_message) (struct message *, void *);
    void (*on_idle) (con_obj *);
    unsigned long (*parse_sequence) (char *, unsigned long, unsigned long,
                                     unsigned long *);
    char *(*create_reset_pad) (unsigned long, char *, size_t *);
    int (*on_connection_established) (struct parse_args *);

    struct rom_parser_helper *stream_state;
    void *(*clone_stream_state) (void *);
    void (*on_message_using_stream_state) (void *, struct message *,
                                           void *);

    int check_idle_flag;
    int heartbeat_interval;
    con_obj *connection;
};

struct ob;
struct error_report;

struct exchange_interface {
    int (*configure) (struct exchange_interface *, const char *);
    int (*listen) (struct exchange_interface *);
    int (*connect) (struct exchange_interface *);
    int (*is_connected) (struct exchange_interface *);
    int (*send_heartbeat) (struct exchange_interface *);
    int (*run) (struct exchange_interface *);
    int (*build_offset) (struct parse_args *);
     time_t(*get_stop_time) (struct exchange_interface *);

    struct ex_con *exchange_conf;
    struct init_args *session_conf;
    struct ob *book;
    queue_t rom_conf;
    time_t stop_time;

    struct id_generator *id_generator;

    struct exchange_interface_connection server_connection;
    struct exchange_interface_connection client_connection;

    /* Member functions to handle each different type of order action
     * to be sent through this exchange interface. */
    int (*on_enter_order) (struct exchange_interface *, struct message *,
                           struct parse_con *, void *, struct databuf *,
                           struct error_report *);
    void (*on_cancel_order) (struct exchange_interface *,
                             struct message *);
    void (*on_replace_order) (struct exchange_interface *,
                              struct message *);
};

/* Configure an exchange_interface_connection to process DART CSV
 * messages from a ROM. */
void exchange_interface_rom_connection_init(struct
                                            exchange_interface_connection
                                            *);
void rom_on_connection_update(con_obj *, int, char *, int);
void rom_on_idle(con_obj *);
int exchange_interface_listen(struct exchange_interface *);
time_t exchange_interface_get_stop_time(struct exchange_interface *);
void exchange_interface__init(struct exchange_interface *p);

#endif
