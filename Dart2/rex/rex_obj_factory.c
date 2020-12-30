
/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "rex_obj_factory.h"
#include "message_queue.h"

struct rex_obj_fact {
    queue_t free_list;
    int init_alloc;
    void (*create_func) (queue_t q, int num);
    void (*destroy_func) (void *);
    void (*clear_obj) (void *obj);
    pthread_spinlock_t rex_obj_fact_mutex;
};

dof_t *create_rex_obj_fact(int initial_amt, void destroy_func(void *),
                            void create_func(queue_t q,
                                             int num),
                            void clear_obj(void *obj))
{
    dof_t *d = calloc(sizeof(struct rex_obj_fact), 1);
    d->init_alloc = initial_amt;
    d->destroy_func = destroy_func;
    d->create_func = create_func;
    d->clear_obj = clear_obj;
    d->free_list = create_sized_queue(initial_amt);
    pthread_spin_init(&d->rex_obj_fact_mutex, PTHREAD_PROCESS_PRIVATE);
    create_func(d->free_list, initial_amt);
    return d;
}

void add_to_free_list(dof_t * d, int num)
{
    if (pthread_spin_lock(&d->rex_obj_fact_mutex) == 0) {
        d->create_func(d->free_list, d->init_alloc * 2);
        if (pthread_spin_unlock(&d->rex_obj_fact_mutex) != 0) {
            printf("Failed to unlock mutex in add_to_free_list. \n");
        }
    }
}

void *get_obj(dof_t * d)
{
    void *ret = NULL;
    element_t e;
    if (pthread_spin_lock(&d->rex_obj_fact_mutex) == 0) {
        if (is_empty(d->free_list)) {
            d->create_func(d->free_list, d->init_alloc * 2);
            d->init_alloc = d->init_alloc * 2;
        }
        e = pop_front(d->free_list);
        ret = e->d;
        free_element(d->free_list, e);
        if (pthread_spin_unlock(&d->rex_obj_fact_mutex) != 0) {
            printf("Failed to unlock mutex in get_empty_rex. \n");
        }
    }
    return ret;
}

void destroy_rex_obj_fact(dof_t * d)
{
    element_t e, temp;
    if (pthread_spin_lock(&d->rex_obj_fact_mutex) == 0) {
        e = dequeue(d->free_list);
        while (e != NULL) {
            temp = e;
            e = temp->next;
            d->destroy_func(temp->d);
            free_element(d->free_list, temp);
        }
        destroy_queue(d->free_list);
        if (pthread_spin_unlock(&d->rex_obj_fact_mutex) != 0) {
            printf("Failed to unlock in clean_rex_oo_pool \n");
        }
    }
}

void destroy_elem(dof_t * d, void *e)
{
    if (pthread_spin_lock(&d->rex_obj_fact_mutex) == 0) {
        d->clear_obj(e);
        place_front(d->free_list, e);
        if (pthread_spin_unlock(&d->rex_obj_fact_mutex) != 0) {
            printf("Failed to unlock mutex in destroy dobj.\n");
        }
    }
}
