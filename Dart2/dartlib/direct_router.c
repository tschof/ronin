/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include "direct_router.h"
#include "sb_router.h"
#include "rex_allocator.h"
#include "rex_obj_factory.h"
#include "connection_interface.h"

typedef struct directr {
    con_int* dest;
    char* mod_name;
    int mod_name_len;
    otl* o;
    hashmap_t clients;
    pthread_spinlock_t rprotector;
}directr;

static void diradd_destination(void * k, char *destid, int dest_id_len,
                     void *c)
{
    directr* dr = (directr*)k;
    dr->dest = (con_int*)c;
}
static int dirset_obj_by_client_id(void * k, char *clientid, int clid_len,
                         void *doj)
{
    int ret_val = 0;
    directr* gk = (directr*)k;
    if (pthread_spin_lock(&gk->rprotector) == 0) {
        if(contains_key(gk->clients, clientid, clid_len)) {
            remove_from_map(gk->clients, clientid, clid_len);
        }
        ret_val = no_copy_insert(gk->clients, clientid, clid_len, doj);
        if (ret_val == 0) {
            send_debug_message(gk->o, "Failed to add this client id: %s\n", clientid);
        }
        pthread_spin_unlock(&gk->rprotector);
    }
    return ret_val;
}

static void diradd_route_dest_pair(void * k, char *destid, int dest_len,
                         char *route, int route_len)
{
}

static void dirprint_dest_routes(void * k, databuf_t * buff)
{
}

static int dirreset_dest_routes(void * k, char *dest, int dest_len,
                      char *dest_route, int dest_route_len)
{
    return 1;
}

static int dirget_destination(void * k, char *destid, int dest_id_len,
                    void **dipper)
{
    directr* gk = (directr*)k;
    *dipper = gk->dest;
    return 1; 
}

static int dirget_obj_by_client_id(void * k, char *clientid,
                         int client_id_len, void **ret)
{
    int val = 0;
    directr* gk = (directr*)k;
    if (pthread_spin_lock(&gk->rprotector) == 0) {
        val =  get_map_entry(gk->clients, clientid, client_id_len,
                         (void **) ret);
        pthread_spin_unlock(&gk->rprotector);
    } 
    return val;
}
static void dirremove_obj_by_client_id(void* k, char* clientid,
                             int client_id_len)
{

    directr* gk = (directr*)k;
    if (pthread_spin_lock(&gk->rprotector) == 0) {
        remove_from_map(gk->clients, clientid, client_id_len);
        pthread_spin_unlock(&gk->rprotector);
    }
}

static int dircontains_obj(void * k, char *name, int name_len)
{
    return 1;
}

void clean_direct_router(void* v)
{
    sbrouter* sb = (sbrouter*)v;
    directr* gk = (directr*)sb->def_router;
    free(gk);
    free(sb);
}

sbrouter* create_direct_router(otl* o,
                             char* mod_name, int mod_name_len)
{
    sbrouter* sb = 0;
    directr* gk = 0;
    int ret = 0;
    {
        REX_ALLOC(sb,8, sizeof(struct sbrouter), ret);
    }
    {
        REX_ALLOC(gk, 8, sizeof(struct directr), ret);
    }
    gk->mod_name = mod_name;
    gk->mod_name_len = mod_name_len;
    sb->sb_add_destination = diradd_destination;
    sb->sb_reset_dest_routes = dirreset_dest_routes;
    sb->sb_get_destination = dirget_destination;
    sb->sb_add_route_dest_pair = diradd_route_dest_pair;
    sb->sb_print_dest_routes = dirprint_dest_routes;
    sb->sb_reset_dest_routes = dirreset_dest_routes;
    sb->sb_set_obj_by_client_id = dirset_obj_by_client_id;
    sb->sb_get_obj_by_client_id = dirget_obj_by_client_id;
    sb->sb_remove_obj_by_client_id = dirremove_obj_by_client_id;
    sb->sb_contains_obj = dircontains_obj;
    sb->clean_router = clean_direct_router;
    sb->def_router = gk;
    gk->clients = create_map(16);
    gk->o = o;
    pthread_spin_init(&gk->rprotector, PTHREAD_PROCESS_PRIVATE);
    return sb;
}
