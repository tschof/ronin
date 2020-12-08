#ifndef __STARTUP_H__
#define __STARTUP_H__

#include "polling_server.h"
#include "message_queue.h"
#include "gatekeeper.h"
#include "lifecycle_master.h"
#include "ex_config_helper.h"
#include "balance_checker.h"

struct dartglob_entry;

struct dart_control_structs {
    jp_limits_checker* jp;
    gatekeeper* gk;
    queue_t interfaces;
    pserver* ass;
    cycle_master* cm;
    ex_config* mc;
};
struct dart_control_structs* dartglob_startup(struct dartglob_entry *listeners,
                                              struct dartglob_entry *globs,
                                              int argc, char *argv[]);

#endif
