/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _LIFECYCLE_MASTER_H__
#define _LIFECYCLE_MASTER_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include "async_server.h"
#include "gatekeeper.h"
#include "master_config.h"
#include "connection_interface.h"

#define SEC_PER_DAY 86400
#define SEC_PER_HOUR 3600
#define SEC_PER_MIN 60
 
    
typedef struct lifecycle_master cycle_master;

void destroy_life_cycle_master(cycle_master* cm);
cycle_master* create_lifecycle_master(async_server_con_set* asc, 
                                          gatekeeper* gk,
                                          master_config* mc);
/**
 * Register to be called back at shutdown time.
 * This can be used to manage gracefull shutdowns.
 * When you register a callback here you are added 
 * to a map of callbackees.  When the shutdown
 * time comes your callback will be called.
 * When you have finished your shutdown you need
 * to call the shutdown_complete function with 
 * the same key as this function.
 */
void add_shutdown_callback(cycle_master* lcm, 
        const char* key, int key_len, 
        void (*shutdown_cb) (void *cb_ptr),
        void* cb_ptr);
/**
 * Call after your shutdown callback has completed.
 */
void shutdown_complete(cycle_master* lcm,
                       const char* key, int key_len);

/**
 * Added to interface for testing.
 */
int get_sec_till_end_time(int end_hour, int end_min, int end_day,
                          struct tm* newtime);

#if defined(__cplusplus)
}
#endif
#endif //_LIFECYCLE_MASTER_H__

