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

#include "gatekeeper.h"
#include "hashmap.h"
#include "dart_constants.h"
#include "dart_obj_factory.h"
#include "dart_order_obj.h"
#include "debug_logger.h"
#include "common_structs.h"
#include "config_loader.h"
#include "order_obj_fact.h"
#include "dart_allocator.h"
#include "connection_interface.h"

#define MICRO_HOUR 3600000000
#define MICRO_MIN 60000000
#define MICRO_SEC 1000000

struct gatekeeper {
    hashmap_t id_to_dest;
    hashmap_t route_to_dest;
    hashmap_t unused_routes;
    hashmap_t name_to_con;
    hashmap_t db_mapping;
    hashmap_t init_con_orders;
    hashmap_t logical_dests;
    void *jp;
    void *cc;
    char *mod_name;
    int mod_name_len;
    char *dom_tag;
    int dom_len;
    size_t dom_num;
    oofact *ord_obj;
    dallocator* databufs;
    pthread_spinlock_t db_protector;
    int allocation;
    int (*update_eq_limits) (void *jp, dart_order_obj * doj);
};

size_t create_time_offset()
{
    time_t rawtime;
    struct tm *ti;
    struct timeval tv;
    size_t dart_sec = 0;
    size_t off_time = 0;
    struct tm lm;
    gettimeofday(&tv, NULL);
    time(&rawtime);
    ti = gmtime_r(&rawtime, &lm);
    ti = &lm;
    dart_sec =
        (ti->tm_hour * MICRO_HOUR) + (ti->tm_min * MICRO_MIN) +
        (ti->tm_sec * MICRO_SEC);
    off_time = tv.tv_usec - dart_sec;
    return tv.tv_usec - off_time;
}

static void create_gk_databuffs(void* g)
{
    databuf_t* node = g;
    node->buffer = g + sizeof(databuf_t);
    node->rd_ptr = node->wr_ptr = node->buffer;
    node->flags = 0x1;
    node->buflen = 512 - sizeof(databuf_t);
}


size_t get_dom_num(gatekeeper * gk)
{
    return __sync_add_and_fetch(&(gk->dom_num), 1);
}

char *get_dom_tag(gatekeeper * gk, int *len)
{
    *len = gk->dom_len;
    return gk->dom_tag;
}

void *get_compliance_con(gatekeeper * gk)
{
    return gk->cc;
}

void set_compliance_con(gatekeeper * gk, void *cc)
{
    gk->cc = cc;
}

void *get_balance_struct(gatekeeper * gk)
{
    return gk->jp;
}

void *get_mod_name(gatekeeper * gk, int *len)
{
    *len = gk->mod_name_len;
    return gk->mod_name;
}
static void clean_init_orders_map(gatekeeper* gk)
{
    hashmap_t rlf;
    char *key = 0;
    int key_len = 0;
    hashmap_iterator_t hi = hashmap_iterator_begin(gk->init_con_orders);
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                                     (void *) &rlf);
    while (valid) {
        delete_map(rlf);
        rlf = 0;
        valid = hashmap_iterator_next(gk->init_con_orders, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &rlf);
    }
    hashmap_iterator_destroy(hi);
}

static void clean_name_to_con(gatekeeper* gk)
{
    con_interface *rlf;
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
}
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

void clean_obj(gatekeeper* gk)
{
    if(gk->init_con_orders) {
        clean_init_orders_map(gk);
        clean_name_to_con(gk);
        delete_map(gk->init_con_orders);
        delete_map(gk->id_to_dest);
        delete_map(gk->unused_routes);
        delete_map(gk->name_to_con);
        delete_map(gk->db_mapping);
        clean_route_map(gk->route_to_dest);
        clean_route_map(gk->logical_dests);
    }
    clean_fact(gk->ord_obj);
    destroy_mem_glob(gk->databufs);
    free(gk->dom_tag);
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
gatekeeper *create_empty_gatekeeper(int order_allocation,
                              char *mod_name, int mod_name_len,
                              void *jp, int create_new)
{
    gatekeeper *gk = malloc(sizeof(struct gatekeeper));
    gk->id_to_dest = 0;
    gk->route_to_dest =0;
    gk->unused_routes = 0;
    gk->name_to_con = 0;
    gk->db_mapping = 0;
    gk->allocation = order_allocation;
    gk->init_con_orders = 0;
    gk->ord_obj = create_order_obj_factory(order_allocation);
    gk->databufs = create_mem_glob(512, create_gk_databuffs);
    gk->logical_dests = 0;
    gk->dom_tag = 0;
    gk->cc = 0;
    gk->jp = jp;
    gk->dom_len = 0;
    gk->update_eq_limits = 0;
    gk->mod_name = mod_name;
    gk->mod_name_len = mod_name_len;
    pthread_spin_init(&gk->db_protector, PTHREAD_PROCESS_PRIVATE);
    return gk;
}

gatekeeper *create_gatekeeper(int order_allocation,
                              char *mod_name, int mod_name_len,
                              void *jp, int create_new, char *filename,
                              int (*update_eq_limits) (void *jp,
                                                       dart_order_obj *
                                                       doj))
{
    gatekeeper *gk = malloc(sizeof(struct gatekeeper));
    gk->id_to_dest = create_map(128);
    gk->route_to_dest = create_map(128);
    gk->unused_routes = create_map(128);
    gk->name_to_con = create_map(512);
    gk->db_mapping = create_map(512);
    gk->allocation = order_allocation;
    gk->init_con_orders = create_map(order_allocation);
    gk->ord_obj = create_order_obj_factory(order_allocation);
    gk->databufs = create_mem_glob(512, create_gk_databuffs);
    gk->logical_dests = create_map(64);
    gk_build_map_from_file(filename, gk->logical_dests);
    if (create_new) {
        gk->dom_num = 1;
    } else {
        gk->dom_num = create_time_offset();
    }
    time_t rawtime;
    struct tm *ti;
    int year;
    int month;
    time(&rawtime);
    gk->dom_tag = calloc(64, 1);
    gk->cc = 0;
    struct tm lm;
    ti = gmtime_r(&rawtime, &lm);
    ti = &lm;
    year = ti->tm_year - 100;
    month = 1 + ti->tm_mon;
    gk->jp = jp;
    gk->dom_len =
        sprintf(gk->dom_tag, "-%s%03d%02d%02d-", mod_name, year, month,
                ti->tm_mday);
    gk->update_eq_limits = update_eq_limits;
    gk->mod_name = mod_name;
    gk->mod_name_len = mod_name_len;
    pthread_spin_init(&gk->db_protector, PTHREAD_PROCESS_PRIVATE);
    return gk;
}

void add_destination(gatekeeper * gk, char *destid, int dest_id_len,
                     void *con_int)
{
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
                send_debug_message("Not a real route dest: %s route: %s\n", destid,
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

int set_obj_by_client_id(gatekeeper * gk, char *clientid, int clid_len,
                         void *doj)
{
    int ret_val = no_copy_insert(gk->name_to_con, clientid, clid_len, doj);
    if (ret_val == 0) {
        send_debug_message("Failed to add this client id: %s\n", clientid);
    }
    return ret_val;
}

void add_route_dest_pair(gatekeeper * gk, char *destid, int dest_len,
                         char *route, int route_len)
{
    char *real_route = 0;
    int len = get_map_entry(gk->logical_dests, destid, dest_len,
                            (void **) &real_route);
    if (len > 0) {
        insert_map(gk->route_to_dest, destid, dest_len, real_route, len);
    } else {
        insert_map(gk->route_to_dest, destid, dest_len, route, route_len);
    }
}

void print_dest_routes(gatekeeper * gk, databuf_t * buff)
{
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

int reset_dest_routes(gatekeeper * gk, char *dest, int dest_len,
                      char *dest_route, int dest_route_len)
{
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
        send_debug_message("Mapped in this dest: %s dest_route: %s\n",
                           dest, dest_route);
    } else {
        ret_val =
            get_map_entry(gk->unused_routes, dest_route, dest_route_len,
                          &new_guy);
        if (ret_val > 0) {
            no_copy_insert(gk->id_to_dest, dest, dest_len, new_guy);
            remove_from_map(gk->unused_routes, dest_route, dest_route_len);
            send_debug_message("Mapped in this dest: %s dest_route: %s\n",
                               dest, dest_route);
        }
        send_debug_message("Failed to map this dest: %s dest_route: %s\n",
                           dest, dest_route);
    }
    return 1;
}

int get_destination(gatekeeper * gk, char *destid, int dest_id_len,
                    void **dipper)
{
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

int get_obj_by_client_id(gatekeeper * gk, char *clientid,
                         int client_id_len, void **ret)
{
    return get_map_entry(gk->name_to_con, clientid, client_id_len,
                         (void **) ret);
}
void remove_obj_by_client_id(gatekeeper* gk, char* clientid,
                             int client_id_len)
{
    remove_from_map(gk->name_to_con, clientid, client_id_len);
}

int contains_obj(gatekeeper * gk, char *name, int name_len)
{
    return contains_key(gk->name_to_con, name, name_len);
}

void *get_gk_obj(gatekeeper * gk)
{
    dart_order_obj* doj = 0;
    if (pthread_spin_lock(&gk->db_protector) == 0) {
        doj = get_doj(gk->ord_obj);
        rom_style_reset(doj);
        if (pthread_spin_unlock(&gk->db_protector) != 0) {
            printf("Failed to unlock mutex in get_databuf. \n");
        }
    }
    return doj;
}

databuf_t *get_databuf(gatekeeper * gk)
{
    databuf_t* db = 0;
    if (pthread_spin_lock(&gk->db_protector) == 0) {
        db = (databuf_t *) get_stuff(gk->databufs);
        if (pthread_spin_unlock(&gk->db_protector) != 0) {
            printf("Failed to unlock mutex in get_databuf. \n");
        }
    }
    return db;
}

void destroy_gk_obj(gatekeeper * gk, void *obj)
{
    if (pthread_spin_lock(&gk->db_protector) == 0) {
        reset_doj(gk->ord_obj, obj);
        if (pthread_spin_unlock(&gk->db_protector) != 0) {
            printf("Failed to unlock mutex in get_databuf. \n");
        }
    }
}

void destroy_gk_db(gatekeeper * gk, databuf_t *db)
{
    if (pthread_spin_lock(&gk->db_protector) == 0) {
        databuf_reset(db);
        return_stuff(gk->databufs, db);
        if (pthread_spin_unlock(&gk->db_protector) != 0) {
            printf("Failed to unlock mutex in destroy_gk_db. \n");
        }
    }
}

void add_to_init_orders(gatekeeper * gk, char *dest, int dest_len,
                        void *order)
{
    hashmap_t dest_map = 0;
    dart_order_obj *doj = (dart_order_obj *) order;
    int ret = get_map_entry(gk->init_con_orders, dest, dest_len,
                            (void **) &dest_map);
    if (ret <= 0) {
        dest_map = create_map(gk->allocation);
        no_copy_insert(gk->init_con_orders, dest, dest_len, dest_map);
    }
    no_copy_insert(dest_map, doj->positions[ROM_CLORDID].iov_base,
                   doj->positions[ROM_CLORDID].iov_len, order);
    no_copy_insert(dest_map, doj->positions[ROM_TAG].iov_base,
                   doj->positions[ROM_TAG].iov_len, order);

}

void remove_from_init_orders(gatekeeper* gk, char* dest, int dest_len)
{
    remove_from_map(gk->init_con_orders, dest, dest_len);
}

hashmap_t get_initial_order_map(gatekeeper * gk, char *dest, int dest_len)
{
    hashmap_t dest_map = 0;
    int ret = get_map_entry(gk->init_con_orders, dest, dest_len,
                            (void **) &dest_map);
    if (ret <= 0) {
        dest_map = create_map(gk->allocation);
        no_copy_insert(gk->init_con_orders, dest, dest_len, dest_map);
    }
    return dest_map;
}
