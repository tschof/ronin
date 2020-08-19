/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _GATEKEEPER_H__
#define _GATEKEEPER_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include "dart_order_obj.h"
#include "hashmap.h"
#include "databuf.h"

typedef struct gatekeeper gatekeeper;

gatekeeper *create_empty_gatekeeper(int order_allocation,
                              char *mod_name, int mod_name_len,
                              void *jp, int create_new);
                              
gatekeeper *create_gatekeeper(int allocation_size,
                              char *mod_name, int mod_name_len,
                              void *jp, int create_new, char *filename,
                              int (*update_eq_limits) (void *jp,
                                                       dart_order_obj *
                                                       doj));
void add_destination(gatekeeper * gk, char *destid, int dest_id_len,
                     void *con_int);
int set_obj_by_client_id(gatekeeper * gk, char *clientid, int clid_len,
                         void *doj);
int get_destination(gatekeeper * gk, char *destid, int dest_id_len,
                    void **dipper);
int get_obj_by_client_id(gatekeeper * gk, char *clientid,
                         int client_id_len, void **ret);
void remove_obj_by_client_id(gatekeeper* gk, char* clientid,
                             int client_id_len);
void remove_from_init_orders(gatekeeper* gk, char* dest, int dest_len);
int contains_obj(gatekeeper * gk, char *name, int name_len);
void *get_gk_obj(gatekeeper * gk);
databuf_t *get_databuf(gatekeeper * gk);
void *get_balance_struct(gatekeeper * gk);
void destroy_gk_obj(gatekeeper * gk, void *obj);
void destroy_gk_db(gatekeeper * gk, databuf_t *db);
void *get_compliance_con(gatekeeper * gk);
void set_compliance_con(gatekeeper * gk, void *cc);
void *get_mod_name(gatekeeper * gk, int *len);
void add_to_init_orders(gatekeeper * gk, char *dest, int dest_len,
                        void *order);
hashmap_t get_initial_order_map(gatekeeper * gk, char *dest, int dest_len);
void add_route_dest_pair(gatekeeper * gk, char *destid, int dest_len,
                         char *route, int route_len);
int reset_dest_routes(gatekeeper * gk, char *dest, int dest_len,
                      char *dest_route, int dest_route_len);
void print_dest_routes(gatekeeper * gk, databuf_t * buff);
size_t get_dom_num(gatekeeper * gk);
char *get_dom_tag(gatekeeper * gk, int *len);
void clean_obj(gatekeeper* gk);
#if defined(__cplusplus)
}
#endif
#endif
