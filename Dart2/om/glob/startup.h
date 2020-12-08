#ifndef __STARTUP_H__
#define __STARTUP_H__

#include "async_server.h"
#include "message_queue.h"
#include "gatekeeper.h"
#include "lifecycle_master.h"
#include "master_config.h"
#include "oats.h"

struct dartglob_entry;

struct dart_control_structs {
    void* jp;
    gatekeeper* gk;
    oats_t *oat;
    queue_t interfaces;
    async_server_con_set* ass;
    cycle_master* cm;
    master_config* mc;
};
struct dart_control_structs* dartglob_startup(struct dartglob_entry *globs, int argc, char *argv[]);

#endif
