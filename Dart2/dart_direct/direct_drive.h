/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _MDS_CLIENT_H__
#define _MDS_CLIENT_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include "polling_server.h"
#include "message_queue.h"
#include "gatekeeper.h"
#include "lifecycle_master.h"
#include "ex_config_helper.h"
#include "balance_checker.h"

struct dartglob_entry;

struct ddart_control_structs {
    jp_limits_checker* jp;
    gatekeeper* gk;
    queue_t interfaces;
    pserver* ass;
    cycle_master* cm;
    ex_config* mc;
};

/**
 * This is similar to the dartglob start sequence but in this case there is 
 * only one listener and one connection.
 */
struct ddart_control_structs* start_direct_drive(struct dartglob_entry *listener,
                                                 struct dartglob_entry *glob,
                                                 int argc, char *argv[]);

#if defined(__cplusplus)
}
#endif
#endif
