
/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "dart_obj_factory.h"
#include "message_queue.h"

struct dart_obj_fact {
    queue_t free_list;
    queue_t glob_list;
    int init_alloc;
    void (*create_func) (queue_t glob, queue_t q, int num);
    void (*destroy_func) (void *);
    void (*clear_obj) (void *obj);
    pthread_mutex_t dart_obj_fact_mutex;
};

dof_t *create_dart_obj_fact(int initial_amt, void destroy_func(void *),
                            void create_func(queue_t glob, queue_t q,
                                             int num),
                            void clear_obj(void *obj))
{
    dof_t *d = malloc(sizeof(struct dart_obj_fact));
    d->init_alloc = initial_amt;
    d->destroy_func = destroy_func;
    d->create_func = create_func;
    d->clear_obj = clear_obj;
    d->free_list = create_sized_queue(initial_amt);
    d->glob_list = create_sized_queue(initial_amt);
    if (pthread_mutex_init(&d->dart_obj_fact_mutex, NULL) != 0) {
        printf("Failed to init obj manager mutex.\n");
    }
    create_func(d->glob_list, d->free_list, initial_amt);
    return d;
}

void add_to_free_list(dof_t * d, int num)
{
    if (pthread_mutex_lock(&d->dart_obj_fact_mutex) == 0) {
        d->create_func(d->glob_list, d->free_list, d->init_alloc * 2);
        if (pthread_mutex_unlock(&d->dart_obj_fact_mutex) != 0) {
            printf("Failed to unlock mutex in add_to_free_list. \n");
        }
    }
}

void *get_obj(dof_t * d)
{
    void *ret = NULL;
    element_t e;
    if (pthread_mutex_lock(&d->dart_obj_fact_mutex) == 0) {
        if (is_empty(d->free_list)) {
            d->create_func(d->glob_list, d->free_list, d->init_alloc * 2);
            d->init_alloc = d->init_alloc * 2;
            printf("list empty, alloc, %d \n", d->init_alloc);
        }
        e = pop_front(d->free_list);
        ret = e->d;
        free_element(d->free_list, e);
        if (pthread_mutex_unlock(&d->dart_obj_fact_mutex) != 0) {
            printf("Failed to unlock mutex in get_empty_dart. \n");
        }
    }
    return ret;
}

void destroy_dart_obj_fact(dof_t * d)
{
    element_t e, temp;
    if (pthread_mutex_lock(&d->dart_obj_fact_mutex) == 0) {
        e = dequeue(d->free_list);
        while (e != NULL) {
            temp = e;
            e = temp->next;
            d->destroy_func(temp->d);
            free(temp);
        }
        e = dequeue(d->glob_list);
        while (e != NULL) {
            temp = e;
            e = temp->next;
            free(temp->d);
            free(temp);
        }
        destroy_queue(d->free_list);
        destroy_queue(d->glob_list);
        if (pthread_mutex_unlock(&d->dart_obj_fact_mutex) != 0) {
            printf("Failed to unlock in clean_dart_oo_pool \n");
        }
    }
}

void destroy_elem(dof_t * d, void *e)
{
    if (pthread_mutex_lock(&d->dart_obj_fact_mutex) == 0) {
        d->clear_obj(e);
        enqueue(d->free_list, e);
        if (pthread_mutex_unlock(&d->dart_obj_fact_mutex) != 0) {
            printf("Failed to unlock mutex in destroy dobj.\n");
        }
    }
}
