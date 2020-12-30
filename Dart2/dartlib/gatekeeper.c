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
#include "rex_constants.h"
#include "connection_interface.h"
#include "rex_obj_factory.h"
#include "dart_order_obj.h"
#include "config_loader.h"
#include "dart_order_fact.h"
#include "rex_allocator.h"
#include "dart_router.h"
#include "sb_router.h"

#define MICRO_HOUR 3600000000
#define MICRO_MIN 60000000
#define MICRO_SEC 1000000

struct gatekeeper {
    hashmap_t init_con_orders;
    sbrouter* dr;
    otl* o;
    pserver* p;
    void *jp;
    con_int *cc;
    char *mod_name;
    int mod_name_len;
    char *dom_tag;
    int dom_len;
    size_t dom_num;
    dartfact *ord_obj;
    dallocator* databufs;
    pthread_spinlock_t db_protector;
    dof_t* risk_fact;
    int allocation;
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


static void create_risk_cntr(queue_t q, int num)
{
    int loops = num;
    int i = 0;
    for(;i < loops; ++i) {
        int ret = REX_PG_SIZE/sizeof(rcntr);
        void* f =0;
        REX_ALLOC(f, REX_PG_SIZE,REX_PG_SIZE, ret);
        ret = 0;
        for(;ret < 32; ++ret) {
            rcntr* rc = f;
            enqueue(q, rc);
            f += sizeof(rcntr);
        }
    }
}

static void clear_risk_cntr(void* db)
{
    memset(db, '\0', sizeof(struct risk_cntr_struct));
}

static void destroy_risk_cntr(void* d)
{
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

void clean_obj(gatekeeper* gk)
{
    if(gk->init_con_orders) {
        clean_init_orders_map(gk);
        delete_map(gk->init_con_orders);
    }
    clean_fact(gk->ord_obj);
    destroy_mem_glob(gk->databufs);
    if(gk->dr != 0x0) {
        gk->dr->clean_router(gk->dr);
    }
    free(gk->dom_tag);
    destroy_rex_obj_fact(gk->risk_fact);
}
dartfact* get_gk_fact(gatekeeper* gk)
{
    return gk->ord_obj;
}
gatekeeper *create_empty_gatekeeper(int order_allocation,
                              char *mod_name, int mod_name_len,
                              void *jp, int create_new, otl* o)
{
    gatekeeper *gk = malloc(sizeof(struct gatekeeper));
    gk->dr = 0;
    gk->allocation = order_allocation;
    gk->init_con_orders = 0;
    gk->ord_obj = create_dart_order_factory(order_allocation);
    gk->databufs = create_mem_glob(512, create_gk_databuffs);
    gk->dom_tag = 0;
    gk->cc = 0;
    gk->jp = jp;
    gk->o = o;
    gk->p = 0;
    gk->dom_len = 0;
    gk->mod_name = mod_name;
    gk->mod_name_len = mod_name_len;
    gk->risk_fact = create_rex_obj_fact(1024, destroy_risk_cntr,
                                        create_risk_cntr,
                                        clear_risk_cntr);
    pthread_spin_init(&gk->db_protector, PTHREAD_PROCESS_PRIVATE);
    return gk;
}

gatekeeper *create_gatekeeper(int order_allocation, otl* o, pserver* p, 
                              char *mod_name, int mod_name_len,
                              void *jp, int create_new, char *filename)
{
    gatekeeper *gk = malloc(sizeof(struct gatekeeper));
    gk->allocation = order_allocation;
    gk->dr = create_dart_router(order_allocation, o, mod_name,
                                mod_name_len, filename);
    gk->p = p;
    gk->init_con_orders = create_map(order_allocation);
    gk->risk_fact = create_rex_obj_fact(1024, destroy_risk_cntr,
                                        create_risk_cntr,
                                        clear_risk_cntr);
    gk->ord_obj = create_dart_order_factory(order_allocation);
    gk->databufs = create_mem_glob(512, create_gk_databuffs);
    if (create_new) {
        gk->dom_num = 1;
    } else {
        gk->dom_num = create_time_offset();
    }
    gk->o = o;
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
    gk->mod_name = mod_name;
    gk->mod_name_len = mod_name_len;
    pthread_spin_init(&gk->db_protector, PTHREAD_PROCESS_PRIVATE);
    return gk;
}
gatekeeper *create_dd_gatekeeper(sbrouter* dr, otl* o, pserver* p, 
                              char *mod_name, int mod_name_len,
                              void *jp, int create_new)
{
    gatekeeper *gk = malloc(sizeof(struct gatekeeper));
    gk->allocation = 8;
    gk->dr = dr;
    gk->p = p;
    gk->init_con_orders = create_map(8);
    gk->risk_fact = create_rex_obj_fact(1024, destroy_risk_cntr,
                                        create_risk_cntr,
                                        clear_risk_cntr);
    gk->ord_obj = create_dart_order_factory(8);
    gk->databufs = create_mem_glob(512, create_gk_databuffs);
    if (create_new) {
        gk->dom_num = 1;
    } else {
        gk->dom_num = create_time_offset();
    }
    gk->o = o;
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
    gk->mod_name = mod_name;
    gk->mod_name_len = mod_name_len;
    pthread_spin_init(&gk->db_protector, PTHREAD_PROCESS_PRIVATE);
    return gk;
}

void add_destination(gatekeeper * gk, char *destid, int dest_id_len,
                     void *con_int)
{
    gk->dr->sb_add_destination(gk->dr->def_router, destid, dest_id_len, con_int);
}

int set_obj_by_client_id(gatekeeper * gk, char *clientid, int clid_len,
                         void *doj)
{
    return gk->dr->sb_set_obj_by_client_id(gk->dr->def_router, clientid,
                                           clid_len, doj);
}

void add_route_dest_pair(gatekeeper * gk, char *destid, int dest_len,
                         char *route, int route_len)
{
    gk->dr->sb_add_route_dest_pair(gk->dr->def_router, destid, dest_len,
                                    route, route_len);
}

void print_dest_routes(gatekeeper * gk, databuf_t * buff)
{
    gk->dr->sb_print_dest_routes(gk->dr->def_router, buff);
}

int reset_dest_routes(gatekeeper * gk, char *dest, int dest_len,
                      char *dest_route, int dest_route_len)
{
    return gk->dr->sb_reset_dest_routes(gk->dr->def_router, dest,
                                        dest_len, dest_route, dest_route_len);
}

int get_destination(gatekeeper * gk, char *destid, int dest_id_len,
                    void **dipper)
{
    return gk->dr->sb_get_destination(gk->dr->def_router, destid, dest_id_len,
                                        dipper); 
}

int get_obj_by_client_id(gatekeeper * gk, char *clientid,
                         int client_id_len, void **ret)
{
    return gk->dr->sb_get_obj_by_client_id(gk->dr->def_router, clientid, client_id_len,
                         (void **) ret);
}
void remove_obj_by_client_id(gatekeeper* gk, char* clientid,
                             int client_id_len)
{
    gk->dr->sb_remove_obj_by_client_id(gk->dr->def_router, clientid,
                                       client_id_len);
}

int contains_obj(gatekeeper * gk, char *name, int name_len)
{
    return gk->dr->sb_contains_obj(gk->dr->def_router, name, name_len);
}

void *get_gk_obj(gatekeeper * gk)
{
    dart_order_obj* doj = 0;
    if (pthread_spin_lock(&gk->db_protector) == 0) {
        doj = get_doj(gk->ord_obj);
        dart_style_reset(doj);
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
    rcntr* cnt = (rcntr*)order;
    dart_order_obj *doj = (dart_order_obj *) cnt->raw;
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
    char tempordid[64];
    memset(&tempordid, '\0',64);
    memcpy(&tempordid, doj->positions[ROM_CLORDID].iov_base, doj->positions[ROM_CLORDID].iov_len);
    send_debug_message(gk->o, "Adding to init orders map: %lu dest:%s, dest_len:%d, ordid:%s, rcntr: %lu\n",
                           dest_map,
                           dest, dest_len,
                           tempordid, cnt);
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
    send_debug_message(gk->o, "getting orders map: %lu dest:%s, dest_len:%d\n",
                           dest_map,
                           dest, dest_len);
    return dest_map;
}
char *get_sender_name_from_copy(dart_order_obj * doj)
{
    struct db_node *db = doj->head;
    if (doj->positions[ROM_SENDER].iov_len != 0) {
        while (db != NULL
               && !(doj->positions[ROM_SENDER].iov_base < db->end_ptr
                    && doj->positions[ROM_SENDER].iov_base >
                    db->start_ptr)) {
            db = db->next;
        }
        if (db == NULL && doj->positions[ROM_SENDER].iov_len > 0) {
            return doj->positions[ROM_SENDER].iov_base;
        } else if (doj->positions[ROM_SENDER].iov_len > 0) {
            int len = doj->positions[ROM_SENDER].iov_base - db->start_ptr;
            char *offset = db->data->buffer + len;
            return offset;
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

int check_vals(dart_order_obj * doj, char *sender, int sender_len,
               char *clordid, int ordid_len)
{
    if(sender != 0 && doj != 0 && doj->positions != 0 &&
            getpval(doj,ROM_SENDER) != 0) {
        int is_same =
            strncmp(sender, getpval(doj, ROM_SENDER), getplen(doj, ROM_SENDER));
        is_same +=
            strncmp(clordid, doj->positions[ROM_CLIENT_ID].iov_base,
                    ordid_len);
        if (is_same == 0) {
            return 1;
        } else {
            return 0;
        }
    }
    return 0;
}


void set_dom_tag(dart_order_obj * doj, void *g)
{
    int dom_len = 0;
    gatekeeper *gk = (gatekeeper *) g;
    char *dom_str = get_dom_tag(gk, &dom_len);
    databuf_t *itoa_buff = get_databuf(gk);
    databuf_memcpy(itoa_buff,
           doj->positions[ROM_SENDER].iov_base,
           doj->positions[ROM_SENDER].iov_len);
    databuf_memcpy(itoa_buff, dom_str, dom_len);
    size_t v = get_dom_num(gk);
    int len = no_set_unsigned_itoa(itoa_buff->wr_ptr, v);
    itoa_buff->wr_ptr += len;
    int dom_tag_len = databuf_unread(itoa_buff);
    databuf_memcpy(itoa_buff, "-", 1);
    len = no_set_unsigned_itoa(itoa_buff->wr_ptr, doj->ex_mess);
    itoa_buff->wr_ptr += len;
    ++doj->ex_mess;
    int space = databuf_space_left(doj->current->data);
    len = databuf_unread(itoa_buff);
    if (space < (len + 5)) {
        update_buffer(doj);
    }
    databuf_memcpy(doj->current->data, itoa_buff->rd_ptr, len);
    doj->positions[ROM_EXECUTION_TAG].iov_len = len;
    doj->positions[ROM_EXECUTION_TAG].iov_base =
        doj->current->data->wr_ptr - len;
    doj->current->data->wr_ptr += 5;
    reset_rom_field(doj, ROM_TAG, getpval(doj, ROM_EXECUTION_TAG), dom_tag_len);
    destroy_gk_db(gk, itoa_buff);
}

void set_spread_dom_tag(dart_order_obj* doj, void* g)
{
    set_dom_tag(doj, g);
    if(doj->childs != 0) {
        children* lch = doj->childs;
        while(lch != NULL) {
            dart_order_obj* ldoj = (dart_order_obj*)lch->payload;
            set_dom_tag(ldoj, g);
            set_rom_field(ldoj,ROM_OM_PARENT, getpval(doj, ROM_TAG),
                          getplen(doj, ROM_TAG));
            lch = lch->next;
        }
    } 
}
void set_ex_tag(dart_order_obj* doj, int ex)
{
    doj->ex_mess = ex;
    char itoa_buff[64];
    int len = getplen(doj, ROM_TAG);
    memcpy(itoa_buff, getpval(doj, ROM_TAG), len);
    memcpy(itoa_buff + len, "-", 1);
    len += 1;
    len += no_set_unsigned_itoa(itoa_buff + len, doj->ex_mess);
    ++doj->ex_mess;
    set_rom_field(doj, ROM_EXECUTION_TAG, itoa_buff, len);
}

void increment_ex_tag(dart_order_obj* doj)
{
    char itoa_buff[64];
    int len = getplen(doj, ROM_TAG);
    memcpy(itoa_buff, getpval(doj, ROM_TAG), len);
    memcpy(itoa_buff + len, "-", 1);
    len += 1;
    len += no_set_unsigned_itoa(itoa_buff + len, doj->ex_mess);
    ++doj->ex_mess;
    fill_in_rom_field(doj, ROM_EXECUTION_TAG, itoa_buff, len);
}
void gk_disconnect_by_name(gatekeeper* gk, char* trader, int trader_len)
{
    disconnect_by_name(gk->p, trader, trader_len);
}

void gk_add_connection(gatekeeper* gk, void* v)
{
    con_int* ci = (con_int*)v;
    if(ci->rc && ci->rc->logon) {
        add_connection(gk->p, ci->rc);
    }
}

void gk_remove_connection(gatekeeper* gk, void* v)
{
    con_int* ci = (con_int*)v;
    if(ci->rc && ci->rc->logon) {
        remove_connection(gk->p, ci->rc);
    }
}
void gk_disconnect_dest(gatekeeper* gk, char* name, int nlen)
{
     con_int* dipper = 0;
     int found =  gk->dr->sb_get_destination(gk->dr->def_router, name, nlen,
                                             (void**)&dipper);   
     if(found) {
         dipper->rc->disconnect = 1;
         if(dipper->rc->logon != 0x0) {
            disconnect_by_name(gk->p,  dipper->rc->logon->name,
                               dipper->rc->logon->nlen);
         }
     }
}

void* get_router(gatekeeper* gk)
{
    return gk->dr;
}
rcntr* get_cnt(gatekeeper* gk)
{
    if (pthread_spin_lock(&gk->db_protector) == 0) {
        rcntr* rcnt = get_obj(gk->risk_fact);
        pthread_spin_unlock(&gk->db_protector);
        return rcnt;
    } else {
        return NULL;
    }
}

void return_cnt(gatekeeper* gk, rcntr* cnt)
{
    if (pthread_spin_lock(&gk->db_protector) == 0) {
        destroy_elem(gk->risk_fact, cnt);
        pthread_spin_unlock(&gk->db_protector);
    }
}

