#ifndef __TRADINGINTERFACE_H__
#define __TRADINGINTERFACE_H__

#include "server_objects/dart_connection_object.h"
#include "server_objects/dart_orderbook.h"
#include "server_objects/common_structs.h"
#include "ex_common/order_token_creator.h"

#include "dstring.h"

#include <time.h>
#include <stdint.h>

struct TradingInterface;
struct rom_parser_helper;

struct TradingInterfaceConnection {
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

struct TradingInterface {
    int (*configure) (struct TradingInterface *, const char *);
    int (*listen) (struct TradingInterface *);
    int (*connect) (struct TradingInterface *);
    int (*is_connected) (struct TradingInterface *);
    int (*send_heartbeat) (struct TradingInterface *);
    int (*run) (struct TradingInterface *);
    int (*build_offset) (struct parse_args *);

    struct ex_con *exchange_conf;
    struct init_args *session_conf;
    struct ob *book;
    queue_t rom_conf;
    time_t stopTime;

    token_struct_t *token_creator;

    struct TradingInterfaceConnection server_connection;
    struct TradingInterfaceConnection client_connection;

    void *privateData;
};

/* Configure an TradingInterfaceConnection to process DART CSV
 * messages from a ROM. */
void TradingInterface_initROMConnection(struct TradingInterfaceConnection
                                        *);
void rom_on_connection_update(con_obj *, int, char *, int);
void rom_on_idle(con_obj *);
time_t TradingInterface_getStopTime(struct TradingInterface *);
void TradingInterface_init(struct TradingInterface *p);

#endif
