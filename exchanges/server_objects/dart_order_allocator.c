/**************************************************************************
 * Copyright (c) 200EIGHT_BYTES by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "dart_order_allocator.h"
#include "message_queue.h"
#include "debug_logger.h"

static void *last_offset = 0;
static void *prev_delete = 0;

#define EIGHT_BYTES 8
#define THIRTY_TWO 32
#define SIX_TEEN 16

struct dart_obj_mnj {
    queue_t free_list;
    queue_t glob_list;
    pthread_mutex_t dart_alloc_mutex;
    int init_alloc;
};

struct dart_ex_obj {
    double order_qty;
    pthread_mutex_t dart_ex_lock;
    char sender_name[SIX_TEEN];
    char destination[EIGHT_BYTES];
    size_t sender_name_len;
    size_t destination_len;
    char cl_ord_id[DART_EX_TAG_LEN];
    size_t cl_ord_id_len;
    char rep_ord_id[DART_EX_TAG_LEN];
    size_t rep_ord_id_len;
    char exchange_tag[DART_EX_TAG_LEN];
    size_t ex_tag_len;
    char rom_id[DART_EX_TAG_LEN];
    size_t rom_id_len;
    int leaves_shares;
    int cum_shares;
    int delta;
    int offset_cum;
    int temp_cum;
    int pending_replace;
    int pending_cancel;
    char price[THIRTY_TWO];
    size_t p_len;
    char side[EIGHT_BYTES];
    size_t side_len;
    char symbol[THIRTY_TWO];
    size_t sym_len;
    char type[EIGHT_BYTES];
    size_t type_len;
    char capacity[EIGHT_BYTES];
    size_t cap_len;
    char tif[EIGHT_BYTES];
    size_t tif_len;
    char stop_price[THIRTY_TWO];
    size_t stp_len;
    char account[THIRTY_TWO];
    size_t acc_len;
    char trade_for[THIRTY_TWO];
    size_t tf_len;
    char trader[THIRTY_TWO];
    size_t trader_len;
    dart_obj_manager *mgr;
    double show_amt;
    double min_qty;
    queue_t children;
    char expireMonYear[THIRTY_TWO];
    size_t ex_mon_year_len;
    char expireDay[EIGHT_BYTES];
    size_t ex_day_len;
    char strike[SIX_TEEN];
    size_t strike_len;
    char openClose[EIGHT_BYTES];
    size_t open_close_len;
    char callPut[EIGHT_BYTES];
    size_t call_put_len;
    char secType[EIGHT_BYTES];
    size_t sec_type_len;
    char mm_home[EIGHT_BYTES];
    size_t mm_home_len;
    char mm_account[EIGHT_BYTES];
    size_t mm_acc_len;
    char cmta[EIGHT_BYTES];
    size_t cmta_len;
    char short_locate[EIGHT_BYTES];
    size_t short_locate_len;
    char clr_id[SIX_TEEN];
    size_t clr_id_len;
    int is_spread;
    int is_user_defined;
    int spread_head_qty;
    int num_legs;
    int is_manual;
    char parent_id[DART_EX_TAG_LEN];
    size_t parent_id_len;
    char cur_status[EIGHT_BYTES];
    int cur_status_len;
    char sec_def[DART_EX_TAG_LEN];
    size_t sec_def_len;
    char dest_route[THIRTY_TWO];
    int dest_route_len;
    char smpid[THIRTY_TWO];
    int smpid_len;
    char rom_sym[THIRTY_TWO];
    int rom_sym_len;
    char begin_time[17];
    int beg_set;
    char end_time[17];
    int end_set;
};

static struct dart_ex_obj* def_deo = 0;

char *doa_get_short_locate(dart_ex_order_obj * doj, size_t * len)
{
    char *result = 0;
    if (len != 0) {
        *len = 0;
        if (doj->short_locate_len > 0
            && doj->short_locate_len <= (sizeof doj->short_locate)) {
            result = calloc(doj->short_locate_len + 1, 1);
            if (result != 0) {
                *len = doj->short_locate_len;
                strncpy(result, doj->short_locate, *len + 1);
            }
        }
    }
    return result;
}

void doa_set_short_locate(dart_ex_order_obj * doj,
                          const char *const short_locate, size_t len)
{
    if (doj != 0) {
        if (short_locate == 0 || len == 0) {
            doj->short_locate_len = 0;
            doj->short_locate[0] = '\0';
        } else {
            memcpy(doj->short_locate, short_locate, len);
            doj->short_locate[len] = '\0';
            doj->short_locate_len = len;
        }
    }
}

char *doa_get_current_status(dart_ex_order_obj * doj, int *len)
{
    *len = doj->cur_status_len;
    return doj->cur_status;
}

void doa_set_current_status(dart_ex_order_obj * doj, char *status, int len)
{
    doj->cur_status_len = len;
    memcpy(doj->cur_status, status, len);
}

char *doa_get_rom_id_for_leg(dart_ex_order_obj * doj, size_t * len,
                             char *id, size_t id_len)
{
    element_t e;
    char *ret_val = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    e = front(doj->children);
    while (e != NULL) {
        if (e->d != NULL) {
            dart_leg_obj *l_obj = (dart_leg_obj *) e->d;
            if (strncmp(id, l_obj->id, id_len) == 0) {
                int cp_len = strlen(l_obj->rom_id);
                ret_val = calloc((cp_len + 1), 1);
                memcpy(ret_val, l_obj->rom_id, cp_len);
                *len = cp_len;
            }
        }
        e = e->next;
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_val;
}

void doa_decrement_is_pending_rep_for_leg(dart_ex_order_obj* doj,char* id, size_t id_len)
{
    element_t e;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    e = front(doj->children);
    while (e != NULL) {
        if (e->d != NULL) {
            dart_leg_obj *l_obj = (dart_leg_obj *) e->d;
            if (strncmp(id, l_obj->id, id_len) == 0) {
                l_obj->is_pending_rep--;
            }
        }
        e = e->next;
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}
void doa_set_is_pending_rep_for_leg(dart_ex_order_obj* doj,char* id, size_t id_len, int is_pending)
{
    element_t e;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    e = front(doj->children);
    while (e != NULL) {
        if (e->d != NULL) {
            dart_leg_obj *l_obj = (dart_leg_obj *) e->d;
            if (strncmp(id, l_obj->id, id_len) == 0) {
                l_obj->is_pending_rep= is_pending;
            }
        }
        e = e->next;
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}
int doa_get_are_any_legs_pending_rep(dart_ex_order_obj* doj)
{
    int ret = 0;
    element_t e;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    int childs = doj->num_legs;
    e = front(doj->children);
    while (e != NULL && childs > 0) {
        if (e->d != NULL) {
            dart_leg_obj *l_obj = (dart_leg_obj *) e->d;
            if(l_obj->is_pending_rep > 0) {
                ret  = 1;
                break;
            }
        --childs;
        }
        e = e->next;
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret;
}
int doa_get_is_pending_rep_for_leg(dart_ex_order_obj* doj,char* id, size_t id_len)
{
    int ret = 0;
    element_t e;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    e = front(doj->children);
    while (e != NULL) {
        if (e->d != NULL) {
            dart_leg_obj *l_obj = (dart_leg_obj *) e->d;
            if (strncmp(id, l_obj->id, id_len) == 0) {
                ret = l_obj->is_pending_rep;
            }
        }
        e = e->next;
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret;
}
int doa_get_is_chosen_n_ratio(dart_ex_order_obj* doj,char* id, size_t id_len, int* ratio)
{
    int ret = 0;
    element_t e;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    e = front(doj->children);
    while (e != NULL) {
        if (e->d != NULL) {
            dart_leg_obj *l_obj = (dart_leg_obj *) e->d;
            if (strncmp(id, l_obj->id, id_len) == 0) {
                ret = l_obj->chosen_one;
                *ratio = l_obj->i_ratio;
            }
        }
        e = e->next;
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret;
}

static void allocate_n_queue(dart_obj_manager * mgr, int num_objects)
{
    size_t size = num_objects * (sizeof(struct dart_ex_obj));
    char *glob = calloc(size, 1);
    if (glob) {
        char *glob_off = glob;
        size_t offset = 0;
        enqueue(mgr->glob_list, glob);
        while (offset < size) {
            struct dart_ex_obj *doj = (struct dart_ex_obj *) glob_off;
            if (pthread_mutex_init(&doj->dart_ex_lock, NULL) != 0) {
                printf("Failed to init dart object lock.\n");
            }
            doj->children = create_sized_queue(5);
            doj->mgr = mgr;
            glob_off += sizeof(struct dart_ex_obj);
            offset += sizeof(struct dart_ex_obj);
            enqueue(mgr->free_list, doj);
        }
    }
}

static void reset_order_object(dart_ex_order_obj * doj)
{
    if (doj != NULL) {
        element_t e, temp;
        queue_t temp_q = doj->children;
        if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
            printf("Failed to gain mutex.\n");
        }
        if (!is_empty(doj->children)) {
            e = dequeue(doj->children);
            while (e != NULL) {
                temp = e;
                e = temp->next;
                free_element(doj->children, temp);
            }
        }
        if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
            printf("Failed to release mutex.\n");
        }
        if(pthread_mutex_destroy(&doj->dart_ex_lock) != 0) {
            printf("Faild to destroy mutex on reset.\n");
        }
        memcpy(doj, def_deo, sizeof(struct dart_ex_obj));
        doj->children = temp_q;
        if (pthread_mutex_init(&doj->dart_ex_lock, NULL) != 0) {
            printf("Failed to init dart object lock.\n");
        }
    }
}

static dart_obj_manager *init_mem_pool(int num_objects)
{
    dart_obj_manager *mgr = malloc(sizeof(struct dart_obj_mnj));
    if (pthread_mutex_init(&mgr->dart_alloc_mutex, NULL) != 0) {
        printf("Failed to init obj manager mutex.\n");
    }
    mgr->free_list = create_sized_queue(num_objects);
    mgr->glob_list = create_sized_queue(num_objects);
    mgr->init_alloc = num_objects;
    allocate_n_queue(mgr, num_objects);
    def_deo = calloc(1, sizeof(struct dart_ex_obj));
    return mgr;
}

dart_obj_manager *startup_allocator(int num_objects)
{
    return init_mem_pool(num_objects);
}

/*@null@*/
dart_ex_order_obj *get_empty_dart_order_obj(dart_obj_manager * mgr)
{
    dart_ex_order_obj *ret = NULL;

    element_t e;
    if (pthread_mutex_lock(&mgr->dart_alloc_mutex) == 0) {
        if (is_empty(mgr->free_list)) {
            allocate_n_queue(mgr, mgr->init_alloc * 2);
            mgr->init_alloc = mgr->init_alloc * 2;
            printf("list empty, alloc, %d \n", mgr->init_alloc);
        }
        e = pop_front(mgr->free_list);
        ret = e->d;
        free_element(mgr->free_list, e);
        reset_order_object(ret);
        ret->mgr = mgr;
        if (last_offset == ret) {
            send_debug_message("We be givin out da same one twice yo.\n");
        }
        if (pthread_mutex_unlock(&mgr->dart_alloc_mutex) != 0) {
            printf("Failed to unlock mutex in get_empty_dart. \n");
        }
    }
    last_offset = ret;
    return ret;
}

void destroy_dart_order_obj(dart_obj_manager * mgr,
                            dart_ex_order_obj * doj)
{
    if (pthread_mutex_lock(&mgr->dart_alloc_mutex) == 0) {
        if (prev_delete == doj) {
            send_debug_message("Double delete.\n");
        }
        enqueue(mgr->free_list, doj);
        prev_delete = doj;
        if (pthread_mutex_unlock(&mgr->dart_alloc_mutex) != 0) {
            printf("Failed to unlock mutex in destroy dobj.\n");
        }
    }
}

void destroy_dart_order(dart_ex_order_obj * doj)
{
    destroy_dart_order_obj(doj->mgr, doj);
}

void clean_dart_oo_pool(dart_obj_manager * mgr)
{
    element_t e, temp;
    if (pthread_mutex_lock(&mgr->dart_alloc_mutex) == 0) {
        e = dequeue(mgr->free_list);
        while (e != NULL) {
            dart_ex_order_obj *lobj = 0;
            temp = e;
            e = temp->next;
            lobj = (dart_ex_order_obj *) temp->d;
            destroy_queue(lobj->children);
            lobj->children = 0;
            //free_element(mgr->free_list,temp);
            free(temp);
        }
        e = dequeue(mgr->glob_list);
        while (e != NULL) {
            temp = e;
            e = temp->next;
            free(temp->d);
            //free_element(mgr->glob_list,temp);
            free(temp);
        }
        destroy_queue(mgr->free_list);
        destroy_queue(mgr->glob_list);
        if (pthread_mutex_unlock(&mgr->dart_alloc_mutex) != 0) {
            printf("Failed to unlock in clean_dart_oo_pool \n");
        }
    }
}

void doa_add_child(dart_ex_order_obj * doj, void *child)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    enqueue(doj->children, child);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

queue_t doa_get_children(dart_ex_order_obj * doj)
{
    return doj->children;
}

void doa_set_num_legs(dart_ex_order_obj * doj, int num_legs)
{
    doj->num_legs = num_legs;
}

int doa_get_num_legs(dart_ex_order_obj * doj)
{
    return doj->num_legs;
}

void doa_set_is_manual(dart_ex_order_obj * doj, int is_manual)
{
    doj->is_manual = is_manual;
}

int doa_get_is_manual(dart_ex_order_obj * doj)
{
    return doj->is_manual;
}
void doa_set_is_user_defined(dart_ex_order_obj * doj, char spread_val)
{
    doj->is_user_defined = spread_val == '3';
}

int doa_get_is_user_defined(dart_ex_order_obj * doj)
{
    return doj->is_user_defined;
}
void doa_set_is_spread(dart_ex_order_obj * doj, int is_spread)
{
    doj->is_spread = is_spread;
}

int doa_get_is_spread(dart_ex_order_obj * doj)
{
    return doj->is_spread;
}

void doa_set_order_qty(dart_ex_order_obj * doj, double qty)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->order_qty = qty;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

double doa_get_order_qty(dart_ex_order_obj * doj)
{
    double qty = 0;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    qty = doj->order_qty;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return qty;
}

void doa_set_show_amt(dart_ex_order_obj * doj, double qty)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->show_amt = qty;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

double doa_get_show_amt(dart_ex_order_obj * doj)
{
    double qty = 0;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    qty = doj->show_amt;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return qty;
}

void doa_set_min_qty(dart_ex_order_obj * doj, double qty)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->min_qty = qty;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

double doa_get_min_qty(dart_ex_order_obj * doj)
{
    double qty = 0;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    qty = doj->min_qty;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return qty;
}

void doa_set_sender_name(dart_ex_order_obj * doj,
                         const char *s_name, size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->sender_name_len = len < SIX_TEEN ? len : SIX_TEEN;
    memcpy(doj->sender_name, s_name, doj->sender_name_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_sender_name(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->sender_name_len;
    if (doj->sender_name_len > 0) {
        ret_name = calloc(doj->sender_name_len + 1, 1);
        memcpy(ret_name, doj->sender_name, doj->sender_name_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

void doa_set_clr_id(dart_ex_order_obj * doj, const char *id, size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->clr_id_len = len < SIX_TEEN ? len : 19;
    memcpy(doj->clr_id, id, doj->clr_id_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_clr_id(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_id = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->clr_id_len;
    if (doj->clr_id_len > 0) {
        ret_id = calloc(doj->clr_id_len + 1, 1);
        memcpy(ret_id, doj->clr_id, doj->clr_id_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_id;
}

void doa_set_cl_ord_id(dart_ex_order_obj * doj, const void *ord_id,
                       size_t len)
{
    size_t t_len = 0;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    t_len = len < DART_EX_TAG_LEN ? len : (DART_EX_TAG_LEN - 1);
    memcpy(doj->cl_ord_id, ord_id, t_len);
    doj->cl_ord_id_len = t_len;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_cl_ord_id(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->cl_ord_id_len;
    if (doj->cl_ord_id_len > 0) {
        ret_name = calloc(doj->cl_ord_id_len + 1, 1);
        memcpy(ret_name, doj->cl_ord_id, doj->cl_ord_id_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

void doa_set_rep_ord_id(dart_ex_order_obj * doj,
                        const void *rep_id, size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->rep_ord_id_len =
        len < DART_EX_TAG_LEN ? len : (DART_EX_TAG_LEN - 1);
    memcpy(doj->rep_ord_id, rep_id, doj->rep_ord_id_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_rep_ord_id(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->rep_ord_id_len;
    if (doj->rep_ord_id_len > 0) {
        ret_name = calloc(doj->rep_ord_id_len + 1, 1);
        memcpy(ret_name, doj->rep_ord_id, doj->rep_ord_id_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

void doa_set_parent_id(dart_ex_order_obj * doj,
                       const char *ptag, size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in parent setter.\n");
    }
    doj->parent_id_len =
        len < DART_EX_TAG_LEN ? len : (DART_EX_TAG_LEN - 1);
    memcpy(doj->parent_id, ptag, doj->parent_id_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in parent id setter.\n");
    }
}

char *doa_get_parent_id(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = 0;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed ot lock in parent getter.\n");
    }
    *len = doj->parent_id_len;
    if (doj->parent_id_len > 0) {
        ret_name = calloc(doj->parent_id_len + 1, 1);
        memcpy(ret_name, doj->parent_id, doj->parent_id_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock parent.\n");
    }
    return ret_name;
}

void doa_set_exchange_tag(dart_ex_order_obj * doj,
                          const char *ex_tag, size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->ex_tag_len = len < DART_EX_TAG_LEN ? len : (DART_EX_TAG_LEN - 1);
    memcpy(doj->exchange_tag, ex_tag, doj->ex_tag_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_exchange_tag(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->ex_tag_len;
    if (doj->ex_tag_len > 0) {
        ret_name = calloc(doj->ex_tag_len + 1, 1);
        memcpy(ret_name, doj->exchange_tag, doj->ex_tag_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

void doa_set_destination(dart_ex_order_obj * doj,
                         const char *destination, size_t destlen)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->destination_len = destlen < sizeof doj->destination
        ? destlen : sizeof doj->destination - 1;
    memset(doj->destination, 0, sizeof doj->destination);
    memcpy(doj->destination, destination, doj->destination_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_destination(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->destination_len;
    if (doj->destination_len > 0) {
        ret_name = calloc(doj->destination_len + 1, 1);
        memcpy(ret_name, doj->destination, doj->destination_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

void doa_set_rom_id(dart_ex_order_obj * doj,
                    const char *rom_id, size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->rom_id_len = len < DART_EX_TAG_LEN ? len : (DART_EX_TAG_LEN - 1);
    memcpy(doj->rom_id, rom_id, doj->rom_id_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_rom_id(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->rom_id_len;
    if (doj->rom_id_len > 0) {
        ret_name = calloc(doj->rom_id_len + 1, 1);
        memcpy(ret_name, doj->rom_id, doj->rom_id_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

void doa_set_leaves_shares(dart_ex_order_obj * doj, int leaves)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->leaves_shares = leaves;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

int doa_get_leaves_shares(dart_ex_order_obj * doj)
{
    int ret_leaves = 0;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    ret_leaves = doj->leaves_shares;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_leaves;
}

void doa_set_cum_shares(dart_ex_order_obj * doj, int cum)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->cum_shares = cum;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

int doa_get_cum_shares(dart_ex_order_obj * doj)
{
    int ret_cum = 0;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    ret_cum = doj->cum_shares;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_cum;
}
void doa_set_spread_head_qty(dart_ex_order_obj * doj, int cum)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->spread_head_qty = cum;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

int doa_get_spread_head_qty(dart_ex_order_obj * doj)
{
    int ret_cum = 0;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    ret_cum = doj->spread_head_qty;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_cum;
}
void doa_set_temp_cum(dart_ex_order_obj * doj, int cum)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->temp_cum = cum;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

int doa_get_temp_cum(dart_ex_order_obj * doj)
{
    int ret_cum = 0;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    ret_cum = doj->temp_cum;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_cum;
}
void doa_add_to_temp_cum(dart_ex_order_obj* doj, int off_cum)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->temp_cum += off_cum;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

void doa_set_offset_cum(dart_ex_order_obj * doj, int off_cum)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->offset_cum = off_cum;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

void doa_add_to_offset_cum(dart_ex_order_obj* doj, int off_cum)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->offset_cum += off_cum;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

int doa_get_offset_cum(dart_ex_order_obj * doj)
{
    int ret_offset = 0;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    ret_offset = doj->offset_cum;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_offset;
}

void doa_set_delta(dart_ex_order_obj * doj, int delta)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->delta = delta;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

int doa_get_delta(dart_ex_order_obj * doj)
{
    int ret_delta = 0;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    ret_delta = doj->delta;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_delta;
}


void doa_set_pending_cancel(dart_ex_order_obj * doj, int pending)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->pending_cancel = pending;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

void doa_decrement_pending_cancel(dart_ex_order_obj * doj)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->pending_cancel--;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}
int doa_get_pending_cancel(dart_ex_order_obj * doj)
{
    int ret_pending = 0;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    ret_pending = doj->pending_cancel;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_pending;
}
void doa_set_pending_replace(dart_ex_order_obj * doj, int pending)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->pending_replace = pending;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

void doa_decrement_pending_replace(dart_ex_order_obj* doj)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->pending_replace--;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

int doa_get_pending_replace(dart_ex_order_obj * doj)
{
    int ret_pending = 0;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    ret_pending = doj->pending_replace;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_pending;
}

void doa_set_price(dart_ex_order_obj * doj, const char *price, size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->p_len = len < THIRTY_TWO ? len : THIRTY_TWO - 1;
    memcpy(doj->price, price, doj->p_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_price(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->p_len;
    if (doj->p_len > 0) {
        ret_name = calloc(doj->p_len + 1, 1);
        memcpy(ret_name, doj->price, doj->p_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

void doa_set_side(dart_ex_order_obj * doj, const char *side, size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->side_len = len < EIGHT_BYTES ? len : EIGHT_BYTES - 1;
    memcpy(doj->side, side, doj->side_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_side(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->side_len;
    if (doj->side_len > 0) {
        ret_name = calloc(doj->side_len + 1, 1);
        memcpy(ret_name, doj->side, doj->side_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

void doa_set_rom_symbol(dart_ex_order_obj * doj, const char *sym, size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->rom_sym_len = len < THIRTY_TWO ? len : THIRTY_TWO - 1;
    memcpy(doj->rom_sym, sym, doj->rom_sym_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_rom_symbol(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->rom_sym_len;
    if (doj->sym_len > 0) {
        ret_name = calloc(doj->rom_sym_len + 1, 1);
        memcpy(ret_name, doj->rom_sym, doj->rom_sym_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}
void doa_set_symbol(dart_ex_order_obj * doj, const char *sym, size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->sym_len = len < THIRTY_TWO ? len : THIRTY_TWO - 1;
    memcpy(doj->symbol, sym, doj->sym_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_symbol(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->sym_len;
    if (doj->sym_len > 0) {
        ret_name = calloc(doj->sym_len + 1, 1);
        memcpy(ret_name, doj->symbol, doj->sym_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}
void doa_set_smpid(dart_ex_order_obj * doj, const char *sym, size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->smpid_len = len < THIRTY_TWO ? len : THIRTY_TWO - 1;
    memcpy(doj->smpid, sym, doj->smpid_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_smpid(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->smpid_len;
    if (doj->smpid_len > 0) {
        ret_name = calloc(doj->smpid_len + 1, 1);
        memcpy(ret_name, doj->smpid, doj->smpid_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

void doa_set_dest_route(dart_ex_order_obj * doj, const char *sym, size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->dest_route_len = len < THIRTY_TWO ? len : THIRTY_TWO - 1;
    memcpy(doj->dest_route, sym, doj->dest_route_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_dest_route(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->dest_route_len;
    if (doj->dest_route_len > 0) {
        ret_name = calloc(doj->dest_route_len + 1, 1);
        memcpy(ret_name, doj->dest_route, doj->dest_route_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}
void doa_set_sec_def(dart_ex_order_obj * doj, const char *sym, size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->sec_def_len = len < DART_EX_TAG_LEN ? len : DART_EX_TAG_LEN - 1;
    memcpy(doj->sec_def, sym, doj->sec_def_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_sec_def(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->sec_def_len;
    if (doj->sec_def_len > 0) {
        ret_name = calloc(doj->sec_def_len + 1, 1);
        memcpy(ret_name, doj->sec_def, doj->sec_def_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

void doa_set_type(dart_ex_order_obj * doj, const char *type, size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->type_len = len < EIGHT_BYTES ? len : EIGHT_BYTES - 1;
    memcpy(doj->type, type, doj->type_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_type(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->type_len;
    if (doj->type_len > 0) {
        ret_name = calloc(doj->type_len + 1, 1);
        memcpy(ret_name, doj->type, doj->type_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

void doa_set_capacity(dart_ex_order_obj * doj, const char *cap, size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->cap_len = len < EIGHT_BYTES ? len : EIGHT_BYTES - 1;
    memcpy(doj->capacity, cap, doj->cap_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_capacity(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->cap_len;
    if (doj->cap_len > 0) {
        ret_name = calloc(doj->cap_len + 1, 1);
        memcpy(ret_name, doj->capacity, doj->cap_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

void doa_set_tif(dart_ex_order_obj * doj, const char *tif, size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->tif_len = len < EIGHT_BYTES ? len : EIGHT_BYTES - 1;
    memcpy(doj->tif, tif, doj->tif_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_tif(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->tif_len;
    if (doj->tif_len > 0) {
        ret_name = calloc(doj->tif_len + 1, 1);
        memcpy(ret_name, doj->tif, doj->tif_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

void doa_set_stop_price(dart_ex_order_obj * doj, const char *sp,
                        size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->stp_len =
        len < sizeof doj->stop_price ? len : (sizeof doj->stop_price) - 1;
    memcpy(doj->stop_price, sp, doj->stp_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_stop_price(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->stp_len;
    if (doj->stp_len > 0) {
        ret_name = calloc(doj->stp_len + 1, 1);
        memcpy(ret_name, doj->stop_price, doj->stp_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

void doa_set_account(dart_ex_order_obj * doj, const char *acc, size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->acc_len = len < THIRTY_TWO ? len : THIRTY_TWO - 1;
    memcpy(doj->account, acc, doj->acc_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_account(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->acc_len;
    if (doj->acc_len > 0) {
        ret_name = calloc(doj->acc_len + 1, 1);
        memcpy(ret_name, doj->account, doj->acc_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

void doa_set_trade_for(dart_ex_order_obj * doj, const char *acc,
                       size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->tf_len = len < THIRTY_TWO ? len : THIRTY_TWO - 1;
    memcpy(doj->trade_for, acc, doj->tf_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_trade_for(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->tf_len;
    if (doj->tf_len > 0) {
        ret_name = calloc(doj->tf_len + 1, 1);
        memcpy(ret_name, doj->trade_for, doj->tf_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}
void doa_set_trader(dart_ex_order_obj * doj, const char *acc,
                       size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->trader_len = len < THIRTY_TWO ? len : THIRTY_TWO - 1;
    memcpy(doj->trader, acc, doj->trader_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_trader(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->trader_len;
    if (doj->trader_len > 0) {
        ret_name = calloc(doj->trader_len + 1, 1);
        memcpy(ret_name, doj->trader, doj->trader_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

char* doa_get_end_time(dart_ex_order_obj* doj)
{
    if(doj->end_set == 1) {
       return doj->end_time; 
    } else {
        return 0;
    }
}

char* doa_get_begin_time(dart_ex_order_obj* doj)
{
    if(doj->beg_set == 1) {
        return doj->begin_time;
    } else {
        return 0;
    }
}

void doa_set_end_time(dart_ex_order_obj* doj, char* exp,
                        size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->end_set = 1;
    memcpy(doj->end_time, exp, len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

void doa_set_begin_time(dart_ex_order_obj* doj, char* exp,
                        size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->beg_set = 1;
    memcpy(doj->begin_time, exp, len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_expireMonYear(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->ex_mon_year_len;
    if (doj->ex_mon_year_len > 0) {
        ret_name = calloc(doj->ex_mon_year_len + 1, 1);
        memcpy(ret_name, doj->expireMonYear, doj->ex_mon_year_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

char *doa_get_expire_day(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->ex_day_len;
    if (doj->ex_day_len > 0) {
        ret_name = calloc(doj->ex_day_len + 1, 1);
        memcpy(ret_name, doj->expireDay, doj->ex_day_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

void doa_set_expireMonYear(dart_ex_order_obj * doj, const char *exp,
                           size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->ex_mon_year_len = len < THIRTY_TWO ? len : THIRTY_TWO - 1;
    memcpy(doj->expireMonYear, exp, doj->ex_mon_year_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

void doa_set_expire_day(dart_ex_order_obj * doj, const char *exd,
                        size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->ex_day_len = len < EIGHT_BYTES ? len : EIGHT_BYTES - 1;
    memcpy(doj->expireDay, exd, doj->ex_day_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

void doa_set_strike(dart_ex_order_obj * doj, const char *strike,
                    size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->strike_len = len < SIX_TEEN ? len : SIX_TEEN - 1;
    memcpy(doj->strike, strike, doj->strike_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_strike(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->strike_len;
    if (doj->strike_len > 0) {
        ret_name = calloc(doj->strike_len + 1, 1);
        memcpy(ret_name, doj->strike, doj->strike_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

void doa_set_open_close(dart_ex_order_obj * doj, const char *oc,
                        size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->open_close_len = len < EIGHT_BYTES ? len : EIGHT_BYTES - 1;
    memcpy(doj->openClose, oc, doj->open_close_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_open_close(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->open_close_len;
    if (doj->open_close_len > 0) {
        ret_name = calloc(doj->open_close_len + 1, 1);
        memcpy(ret_name, doj->openClose, doj->open_close_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

void doa_set_call_put(dart_ex_order_obj * doj, const char *cp, size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->call_put_len = len < EIGHT_BYTES ? len : EIGHT_BYTES - 1;
    memcpy(doj->callPut, cp, doj->call_put_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_call_put(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->call_put_len;
    if (doj->call_put_len > 0) {
        ret_name = calloc(doj->call_put_len + 1, 1);
        memcpy(ret_name, doj->callPut, doj->call_put_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

void doa_set_sec_type(dart_ex_order_obj * doj, const char *st, size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->sec_type_len = len < EIGHT_BYTES ? len : EIGHT_BYTES - 1;
    memcpy(doj->secType, st, doj->sec_type_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_sec_type(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->sec_type_len;
    if (doj->sec_type_len > 0) {
        ret_name = calloc(doj->sec_type_len + 1, 1);
        memcpy(ret_name, doj->secType, doj->sec_type_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

void doa_set_mm_home(dart_ex_order_obj * doj, const char *st, size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->mm_home_len = len < EIGHT_BYTES ? len : EIGHT_BYTES - 1;
    memcpy(doj->mm_home, st, doj->mm_home_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_mm_home(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->mm_home_len;
    if (doj->mm_home_len > 0) {
        ret_name = calloc(doj->mm_home_len + 1, 1);
        memcpy(ret_name, doj->mm_home, doj->mm_home_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

void doa_set_mm_account(dart_ex_order_obj * doj, const char *mm_a,
                        size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->mm_acc_len = len < EIGHT_BYTES ? len : EIGHT_BYTES - 1;
    memcpy(doj->mm_account, mm_a, doj->mm_acc_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_mm_account(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->mm_acc_len;
    if (doj->mm_acc_len > 0) {
        ret_name = calloc(doj->mm_acc_len + 1, 1);
        memcpy(ret_name, doj->mm_account, doj->mm_acc_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

void doa_set_cmta(dart_ex_order_obj * doj, const char *cmta, size_t len)
{
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    doj->cmta_len = len < EIGHT_BYTES ? len : EIGHT_BYTES - 1;
    memcpy(doj->cmta, cmta, doj->cmta_len);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_cmta(dart_ex_order_obj * doj, size_t * len)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    *len = doj->cmta_len;
    if (doj->cmta_len > 0) {
        ret_name = calloc(doj->cmta_len + 1, 1);
        memcpy(ret_name, doj->cmta, doj->cmta_len);
    }
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

void doa_set_leg_call_put(dart_ex_order_obj * doj, const char *cp,
                          size_t len, dart_leg_obj * dlj)
{
    size_t l_len = 0;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    l_len =
        len < sizeof(dlj->call_put) ? len : (sizeof(dlj->call_put) - 1);
    memset(dlj->call_put, 0, sizeof(dlj->call_put));
    memcpy(dlj->call_put, cp, l_len);
    dlj->cp_len = l_len;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}
void doa_set_leg_sec_type(dart_ex_order_obj * doj, const char *cp,
                          size_t len, dart_leg_obj * dlj)
{
    size_t l_len = 0;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    l_len =
        len < sizeof(dlj->sec_type) ? len : (sizeof(dlj->sec_type) - 1);
    memset(dlj->sec_type, 0, sizeof(dlj->sec_type));
    memcpy(dlj->sec_type, cp, l_len);
    dlj->sec_len = l_len;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

void doa_set_leg_symbol(dart_ex_order_obj * doj, const char *sym,
                        size_t len, dart_leg_obj * dlj)
{
    size_t l_len = 0;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    l_len = len < sizeof dlj->symbol ? len : (sizeof dlj->symbol) - 1;
    memset(dlj->symbol, 0, sizeof(dlj->symbol));
    memcpy(dlj->symbol, sym, l_len);
    dlj->sym_len = l_len;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

void doa_set_leg_expire_date(dart_ex_order_obj * doj, const char *exp,
                             size_t len, dart_leg_obj * dlj)
{
    size_t l_len = 0;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    l_len =
        len <
        sizeof(dlj->expire_date) ? len : (sizeof(dlj->expire_date) - 1);
    memset(dlj->expire_date, 0, sizeof(dlj->expire_date));
    memcpy(dlj->expire_date, exp, l_len);
    dlj->exp_len = l_len;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

void doa_set_leg_ratio(dart_ex_order_obj * doj, const char *ratio,
                       size_t len, dart_leg_obj * dlj)
{
    size_t l_len = 0;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    l_len = len < sizeof(dlj->ratio) ? len : (sizeof(dlj->ratio) - 1);
    memset(dlj->ratio, 0, sizeof(dlj->ratio));
    memcpy(dlj->ratio, ratio, l_len);
    dlj->ratio_len = l_len;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

void doa_set_leg_side(dart_ex_order_obj * doj, const char *side,
                      size_t len, dart_leg_obj * dlj)
{
    size_t l_len = 0;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    l_len = len < sizeof(dlj->side) ? len : (sizeof(dlj->side) - 1);
    memset(dlj->side, 0, sizeof(dlj->side));
    memcpy(dlj->side, side, l_len);
    dlj->side_len = l_len;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

void doa_set_leg_open_close(dart_ex_order_obj * doj, const char *oc,
                            size_t len, dart_leg_obj * dlj)
{
    size_t l_len = 0;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    l_len =
        len <
        sizeof(dlj->open_close) ? len : (sizeof(dlj->open_close) - 1);
    memset(dlj->open_close, 0, sizeof(dlj->open_close));
    memcpy(dlj->open_close, oc, l_len);
    dlj->oc_len = l_len;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

void doa_set_leg_strike(dart_ex_order_obj * doj, const char *strike,
                        size_t len, dart_leg_obj * dlj)
{
    size_t l_len = 0;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    l_len = len < sizeof(dlj->strike) ? len : (sizeof(dlj->strike) - 1);
    memset(dlj->strike, 0, sizeof(dlj->strike));
    memcpy(dlj->strike, strike, l_len);
    dlj->strike_len = l_len;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

void doa_set_leg_id(dart_ex_order_obj * doj, const char *id, size_t len,
                    dart_leg_obj * dlj)
{
    size_t l_len = 0;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    l_len = len < DART_EX_TAG_LEN ? len : (DART_EX_TAG_LEN - 1);
    memset(dlj->id, 0, DART_EX_TAG_LEN);
    memcpy(dlj->id, id, l_len);
    dlj->id_len  = l_len;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

void doa_set_leg_rom_id(dart_ex_order_obj * doj, const char *id,
                        size_t len, dart_leg_obj * dlj)
{
    size_t l_len = 0;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    l_len = len < DART_EX_TAG_LEN ? len : (DART_EX_TAG_LEN - 1);
    memset(dlj->rom_id, 0, DART_EX_TAG_LEN);
    memcpy(dlj->rom_id, id, l_len);
    dlj->rom_id_len = l_len;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
}

char *doa_get_leg_call_put(dart_ex_order_obj * doj, dart_leg_obj * dlj,
        int* rlen)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    if(dlj->cp_len > 0) {
        ret_name = calloc(sizeof(dlj->call_put), 1);
        memcpy(ret_name, dlj->call_put, dlj->cp_len);
    }
    *rlen = dlj->cp_len;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}
char *doa_get_leg_sec_type(dart_ex_order_obj * doj, dart_leg_obj * dlj,
        int* rlen)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    if(dlj->sec_len > 0) {
        ret_name = calloc(sizeof(dlj->sec_type), 1);
        memcpy(ret_name, dlj->sec_type, dlj->sec_len);
    }
    *rlen = dlj->sec_len;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

char *doa_get_leg_symbol(dart_ex_order_obj * doj, dart_leg_obj * dlj,
        int* rlen)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    if(dlj->sym_len > 0) {
        ret_name = calloc(sizeof(dlj->symbol), 1);
        memcpy(ret_name, dlj->symbol, dlj->sym_len);
    }
    *rlen = dlj->sym_len;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

char *doa_get_leg_expire_date(dart_ex_order_obj * doj, 
        dart_leg_obj * dlj, int* rlen)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    if(dlj->exp_len > 0) {
        ret_name = calloc(SIX_TEEN, 1);
        memcpy(ret_name, dlj->expire_date, dlj->exp_len);
    }
    *rlen = dlj->exp_len;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

char *doa_get_leg_ratio(dart_ex_order_obj * doj,
        dart_leg_obj * dlj, int* rlen)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    if(dlj->ratio_len > 0) {
        ret_name = calloc(sizeof(dlj->ratio), 1);
        memcpy(ret_name, dlj->ratio, dlj->ratio_len);
    }
    *rlen = dlj->ratio_len;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

char *doa_get_leg_side(dart_ex_order_obj * doj,
        dart_leg_obj * dlj, int* rlen)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    if(dlj->side_len > 0) {
        ret_name = calloc(sizeof(dlj->side), 1);
        memcpy(ret_name, dlj->side, dlj->side_len);
    }
    *rlen = dlj->side_len;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

char *doa_get_leg_open_close(dart_ex_order_obj * doj,
        dart_leg_obj * dlj, int* rlen)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    if(dlj->oc_len > 0) {
        ret_name = calloc(sizeof(dlj->open_close), 1);
        memcpy(ret_name, dlj->open_close, dlj->oc_len);
    }
    *rlen = dlj->oc_len;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

char *doa_get_leg_strike(dart_ex_order_obj * doj,
        dart_leg_obj * dlj, int* rlen)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    if(dlj->strike_len > 0) {
        ret_name = calloc(sizeof(dlj->strike), 1);
        memcpy(ret_name, dlj->strike, dlj->strike_len);
    }
    *rlen = dlj->strike_len;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

char *doa_get_leg_id(dart_ex_order_obj * doj,
        dart_leg_obj * dlj, int* rlen)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    if(dlj->id_len > 0) {
        ret_name = calloc(DART_EX_TAG_LEN + 1, 1);
        memcpy(ret_name, dlj->id, dlj->id_len);
    }
    *rlen = dlj->id_len;
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}

char *doa_get_leg_rom_id(dart_ex_order_obj * doj,
        dart_leg_obj * dlj, int* rlen)
{
    char *ret_name = NULL;
    if (pthread_mutex_lock(&doj->dart_ex_lock) != 0) {
        printf("Failed to lock in doa setter.\n");
    }
    ret_name = calloc(DART_EX_TAG_LEN + 1, 1);
    memcpy(ret_name, dlj->rom_id, DART_EX_TAG_LEN);
    if (pthread_mutex_unlock(&doj->dart_ex_lock) != 0) {
        printf("Failed to unlock in doa setter.\n");
    }
    return ret_name;
}
