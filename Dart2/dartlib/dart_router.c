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


#include "hashmap.h"
#include "rex_allocator.h"
#include "connection_interface.h"
#include "dart_router.h"


typedef struct drouter {
    hashmap_t id_to_dest;
    hashmap_t route_to_dest;
    hashmap_t unused_routes;
    hashmap_t name_to_con;
    hashmap_t logical_dests;
    char* mod_name;
    int mod_name_len;
    otl* o;
    pthread_spinlock_t rprotector;
}drouter;

/*static void clean_name_to_con(drouter* gk)
{
    con_int *rlf;
    char *key = 0;
    int key_len = 0;
    hashmap_iterator_t hi = hashmap_iterator_begin(gk->name_to_con);
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                                     (void *) &rlf);
    while (valid) {
        rlf->cleansing_rinse(rlf);
        rlf = 0;
        valid = hashmap_iterator_next(gk->name_to_con, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &rlf);
    }
    hashmap_iterator_destroy(hi);
}*/
static void clean_route_map(hashmap_t map)
{
    char* rlf;
    char *key = 0;
    int key_len = 0;
    hashmap_iterator_t hi = hashmap_iterator_begin(map);
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                                     (void *) &rlf);
    while (valid) {
        free(rlf);
        rlf = 0;
        valid = hashmap_iterator_next(map, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &rlf);
    }
    hashmap_iterator_destroy(hi);
    delete_map(map);
}

static void dr_add_destination(void * k, char *destid, int dest_id_len,
                     void *con_int)
{
    drouter* gk = (drouter*)k;
    char *real_route = 0;
    int ret_val = get_map_entry(gk->route_to_dest, destid, dest_id_len,
                                (void **) &real_route);
    if (ret_val > 0) {
        if(ret_val != dest_id_len || 
           (ret_val == dest_id_len && strncmp(destid, real_route, ret_val) != 0)) {

            if(ret_val != gk->mod_name_len || 
                    strncmp(gk->mod_name, real_route, ret_val) != 0) {
                ret_val =
                    no_copy_insert(gk->unused_routes, destid, dest_id_len,
                                   con_int);
                send_debug_message(gk->o, "Not a real route dest: %s route: %s\n", destid,
                                   real_route);
                goto end;
            }
        }
    }
    ret_val =
        no_copy_insert(gk->id_to_dest, destid, dest_id_len, con_int);
end:
    return;
}
static int dr_set_obj_by_client_id(void * k, char *clientid, int clid_len,
                         void *doj)
{
    drouter* gk = (drouter*)k;
    if(contains_key(gk->name_to_con, clientid, clid_len)) {
        remove_from_map(gk->name_to_con, clientid, clid_len);
    }
    int ret_val = no_copy_insert(gk->name_to_con, clientid, clid_len, doj);
    if (ret_val == 0) {
        send_debug_message(gk->o, "Failed to add this client id: %s\n", clientid);
    }
    return ret_val;
}

static void dr_add_route_dest_pair(void * k, char *destid, int dest_len,
                         char *route, int route_len)
{
    drouter* gk = (drouter*)k;
    char *real_route = 0;
    int len = get_map_entry(gk->logical_dests, destid, dest_len,
                            (void **) &real_route);
    if (len > 0) {
        insert_map(gk->route_to_dest, destid, dest_len, real_route, len);
    } else {
        insert_map(gk->route_to_dest, destid, dest_len, route, route_len);
    }
}

static void dr_print_dest_routes(void * k, databuf_t * buff)
{
    drouter* gk = (drouter*)k;
    char *dest_route = 0;
    char *key = 0;
    int key_len = 0;
    hashmap_iterator_t hi = hashmap_iterator_begin(gk->route_to_dest);
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                                     (void *) &dest_route);
    while (valid) {
        databuf_write(buff, "%s=%s:", key, dest_route);
        valid = hashmap_iterator_next(gk->route_to_dest, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &dest_route);
    }
    hashmap_iterator_destroy(hi);
}

static int dr_reset_dest_routes(void * k, char *dest, int dest_len,
                      char *dest_route, int dest_route_len)
{
    drouter* gk = (drouter*)k;
    int ret_val = contains_key(gk->id_to_dest, dest_route, dest_route_len);
    if (ret_val <= 0) {
        return 0;
    }
    ret_val = contains_key(gk->route_to_dest, dest, dest_len);
    if (ret_val) {
        ret_val = remove_from_map(gk->route_to_dest, dest, dest_len);
    }
    insert_map(gk->route_to_dest, dest, dest_len, dest_route,
               dest_route_len);
    void *muck = 0;
    ret_val = get_map_entry(gk->id_to_dest, dest, dest_len, &muck);
    if (ret_val) {
        ret_val = no_copy_insert(gk->unused_routes, dest, dest_len, muck);
        remove_from_map(gk->id_to_dest, dest, dest_len);
    }
    void *new_guy = 0;
    ret_val =
        get_map_entry(gk->id_to_dest, dest_route, dest_route_len,
                      &new_guy);
    if (ret_val > 0) {
        no_copy_insert(gk->id_to_dest, dest, dest_len, new_guy);
        send_debug_message(gk->o, "Mapped in this dest: %s dest_route: %s\n",
                           dest, dest_route);
    } else {
        ret_val =
            get_map_entry(gk->unused_routes, dest_route, dest_route_len,
                          &new_guy);
        if (ret_val > 0) {
            no_copy_insert(gk->id_to_dest, dest, dest_len, new_guy);
            remove_from_map(gk->unused_routes, dest_route, dest_route_len);
            send_debug_message(gk->o, "Mapped in this dest: %s dest_route: %s\n",
                               dest, dest_route);
        }
        send_debug_message(gk->o, "Failed to map this dest: %s dest_route: %s\n",
                           dest, dest_route);
    }
    return 1;
}

static int dr_get_destination(void * k, char *destid, int dest_id_len,
                    void **dipper)
{
    drouter* gk = (drouter*)k;
    int dest_ret = get_map_entry(gk->id_to_dest, destid, dest_id_len, dipper);
    if(dest_ret== 0) {
        char *real_route = 0;
        int len = get_map_entry(gk->logical_dests, destid, dest_id_len,
                                (void **) &real_route);
        if(len > 0) {
            dest_ret = get_map_entry(gk->id_to_dest, real_route, len, dipper);
        }
    }
    return dest_ret; 
}

static int dr_get_obj_by_client_id(void * k, char *clientid,
                         int client_id_len, void **ret)
{
    drouter* gk = (drouter*)k;
    return get_map_entry(gk->name_to_con, clientid, client_id_len,
                         (void **) ret);
}
static void dr_remove_obj_by_client_id(void* k, char* clientid,
                             int client_id_len)
{
    drouter* gk = (drouter*)k;
    remove_from_map(gk->name_to_con, clientid, client_id_len);
}

static int dr_contains_obj(void * k, char *name, int name_len)
{
    drouter* gk = (drouter*)k;
    return contains_key(gk->name_to_con, name, name_len);
}

static void gk_build_map_from_file(const char* filename, hashmap_t map)
{
    char* buffer = 0;
    long size = 0;
    buffer = get_file_contents(filename, &size);
    if(size > 0) {
        char* start = buffer;
        char* comma = buffer;
        char* end = buffer;
        int tag_len = 0;
        do {
            if(*end == ',') {
                tag_len = end - start;
                comma = end;
            } else if(*end == '\n') {
                ++comma;//val = atoi(comma + 1);
                insert_map(map, start, tag_len, comma, end - comma);
                start = end + 1;
            }
            ++end;
            --size;
        } while(size > 0);
        free(buffer);
    }
}
void clean_dart_router(void* v)
{
    sbrouter* sb = (sbrouter*)v;
    drouter* gk = (drouter*)sb->def_router;
    //clean_name_to_con(gk);
    delete_map(gk->id_to_dest);
    delete_map(gk->unused_routes);
    delete_map(gk->name_to_con);
    clean_route_map(gk->route_to_dest);
    clean_route_map(gk->logical_dests);
}

sbrouter* create_dart_router(int allocation, otl* o,
                             char* mod_name, int mod_name_len,
                             char* filename)
{
    sbrouter* sb = 0;
    drouter* gk = 0;
    int ret = 0;
    {
        REX_ALLOC(sb,8, sizeof(struct sbrouter), ret);
    }
    {
        REX_ALLOC(gk, 8, sizeof(struct drouter), ret);
    }
    gk->id_to_dest = create_map(allocation);
    gk->route_to_dest = create_map(allocation);
    gk->unused_routes = create_map(allocation);
    gk->name_to_con = create_map(allocation);
    gk->logical_dests = create_map(allocation);
    gk_build_map_from_file(filename, gk->logical_dests);
    gk->mod_name = mod_name;
    gk->mod_name_len = mod_name_len;
    gk->o = o;
    sb->sb_add_destination = dr_add_destination;
    sb->sb_reset_dest_routes = dr_reset_dest_routes;
    sb->sb_get_destination = dr_get_destination;
    sb->sb_add_route_dest_pair = dr_add_route_dest_pair;
    sb->sb_print_dest_routes = dr_print_dest_routes;
    sb->sb_reset_dest_routes = dr_reset_dest_routes;
    sb->sb_set_obj_by_client_id = dr_set_obj_by_client_id;
    sb->sb_get_obj_by_client_id = dr_get_obj_by_client_id;
    sb->sb_remove_obj_by_client_id = dr_remove_obj_by_client_id;
    sb->sb_contains_obj = dr_contains_obj;
    sb->clean_router = clean_dart_router;
    sb->def_router = gk;
    pthread_spin_init(&gk->rprotector, PTHREAD_PROCESS_PRIVATE);
    return sb;
}
