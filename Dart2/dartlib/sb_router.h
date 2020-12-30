/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _SB_ROUTER_H__
#define _SB_ROUTER_H__
#if defined(__cplusplus)
extern "C" {
#endif
#include "databuf.h"
typedef struct sbrouter {
    void (*sb_add_destination)(void * dr, char *destid, int dest_id_len,
                         void *con_int);
    int (*sb_reset_dest_routes)(void * dr, char *dest, int dest_len,
                          char *dest_route, int dest_route_len);
    int (*sb_get_destination)(void * dr, char *destid, int dest_id_len,
                        void **dipper);
    void (*sb_add_route_dest_pair)(void * dr, char *destid, int dest_len,
                             char *route, int route_len);
    void (*sb_print_dest_routes)(void * gk, databuf_t * buff);
    int (*sb_set_obj_by_client_id)(void * gk, char *clientid, int clid_len,
                             void *doj);
    int (*sb_get_obj_by_client_id)(void * gk, char *clientid,
                             int client_id_len, void **ret);
    void (*sb_remove_obj_by_client_id)(void* gk, char* clientid,
                                 int client_id_len);
    int (*sb_contains_obj)(void * gk, char *name, int name_len);
    void (*clean_router)(void* sbk);
    void* def_router;
}sbrouter;
#if defined(__cplusplus)
}
#endif
#endif
