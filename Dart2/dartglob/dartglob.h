#ifndef __DARTGLOB_H__
#define __DARTGLOB_H__

#define dartglob_entrypoint_name "create_connection"
struct fix_interface;
struct ex_con;
struct init_args;

#include "message_queue.h"
#include "connection_interface.h"
#include "gatekeeper.h"

struct dartglob_entry {
    char *shared_object_name;
    char *config_file_name;
    void *dlhandle;
    void (*entrypoint) (con_int* con, char *dat_file, int df_len,
                        gatekeeper * gk);
    void *token;
    con_int*interface;
    struct ex_con *exchconf;
    struct init_args *fixconf;
    struct dartglob_entry *next;
};

struct dartglob_entry *dartglob_parse_config(const char *fname);
int dartglob_load_shared_objects(struct dartglob_entry *entries);

#endif
