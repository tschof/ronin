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
#include "message_queue.h"
#include "gatekeeper.h"
#include "service_bureau.h"
#include "order_token_generator.h"
#include "dart_order_obj.h"
#include "ex_config_helper.h"
#include "log_mnger.h"
#include "polling_server.h"

typedef struct con_interface {
    gatekeeper* gk;
    ex_config* ex;
    otl* o; 
    rex_con* rc;
    void* cparser;
    void* ph;
    pserver* p;
    pthread_spinlock_t mutex;
    char* mod_name;
    int mod_name_len;
    int should_buffer;
    int connected;
    int can_send_orders;
    int is_blocked;
    int is_server;
    int sock;
    sbureau_match* sbm;
    token_gen* tg;
    hashmap_t cl_ord_id_map;
    login_info* logon;
    int (*on_enter_order) (struct con_interface* ci, rcntr* doj);
    int (*on_replace_order)(struct con_interface* ci, rcntr* doj);
    int (*on_cancel_order)(struct con_interface* ci, rcntr* doj);
    void(*on_cancel_all)(struct con_interface* ci);
    void(*on_rej)(struct con_interface* ci, rcntr* doj);
    void(*on_replaced)(struct con_interface* ci, rcntr* doj);
    void(*on_out)(struct con_interface* ci, rcntr* doj);
    void(*on_fill)(struct con_interface* ci, rcntr* doj);
    void(*on_update)(struct con_interface* ci, rcntr* doj);
    void(*reset_seq)(struct con_interface* ci, int out, int in);
    void(*reload_ip_port)(struct con_interface* ci, dart_order_obj* doj);
    void(*cleansing_rinse)(struct con_interface* ci);
    void(*send_to_con)(struct con_interface* ci, rcntr* doj);
    /*TODO should we change from_con and update_oats
     * to connection interface rather than void*?*/
    void (*from_oats_update)(struct con_interface* ci, dart_order_obj* doj);
    void(*update_oats)(struct con_interface* pb, int is_connected);
} con_int;

login_info* validate_logon(void* rc, dart_order_obj* cur_doj);
con_int* clone_interface(con_int* ci, dart_order_obj* doj);

con_int* create_empty_interface();

con_int* init_client_con(char* name, int nlen,
                         gatekeeper* gk);
void add_int_orderids(con_int* con);

void reload_dest_ip_port(con_int* con, dart_order_obj* doj);


#if defined(__cplusplus)
}
#endif
#endif
