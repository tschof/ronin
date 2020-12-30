/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "message_queue.h"
#include "rex_allocator.h"

#define FREE_LIST_SIZE 512

struct queue_s {
    element_t front;
    element_t rear;
    dallocator* free_list;
};
queue_t create_queue()
{
    return create_sized_queue(FREE_LIST_SIZE);
}

static void init_elems(void* g)
{
    memset(g, '\0', sizeof(struct queue_elem));
}

queue_t create_sized_queue(int fr_size)
{
    struct queue_s *q = calloc(sizeof(struct queue_s), 1);
    if (q == NULL) {
        return NULL;
    }
    q->front = q->rear = 0;
    q->free_list = create_mem_glob(sizeof(struct queue_elem), init_elems);
    q->front = q->rear = get_stuff(q->free_list);
    q->front->d = 0;
    return q;
}

void destroy_queue(queue_t q)
{
    if (q) {
        destroy_mem_glob(q->free_list);
        free(q);
        q = 0;
    }
}

void *end_data(queue_t q)
{
    return q->rear->d;
}

element_t front(queue_t q)
{
    return q->front;
}

int is_empty(queue_t q)
{
    if (q && q->front) {
        return (q->front->next == NULL);
    } else {
        return 1;
    }
}

void reset_end(queue_t q)
{
    element_t f;
    f = front(q);
    while (f->next != NULL) {
        f = f->next;
    }
    q->rear = f;
}

element_t dequeue(queue_t q)
{
    if (!is_empty(q)) {
        element_t temp = q->front->next;
        q->front->next = NULL;
        q->rear = q->front;
        q->front->d = 0;
        return temp;
    } else {
        return NULL;
    }
}

element_t pop_front(queue_t q)
{
    if (q && q->front && q->front->next) {
        element_t temp = q->front->next;
        q->front->next = temp->next;
        if (temp->next == NULL) {
            q->rear = q->front;
            q->rear->d = 0;
            q->front->d = 0;
        }
        temp->next = NULL;
        return temp;
    }
    return NULL;
}

static void push_front(queue_t q, element_t e)
{
    e->next = q->front->next;
    if (q->front == q->rear) {
        q->rear = e;
    }
    q->front->next = e;
}

void free_element(queue_t q, element_t e)
{
    if (e && q && q->free_list) {
        e->next = NULL;
        e->d = NULL;
        return_stuff(q->free_list, e);
    }
}

void place_front(queue_t q, DATA x)
{
    struct queue_elem *elem;
    elem = get_stuff(q->free_list);
    if (elem != NULL) {
        elem->d = x;
        push_front(q, elem);
    }
}

void enqueue(queue_t q, DATA x)
{
    struct queue_elem *elem;
    elem = get_stuff(q->free_list);
    if (elem != NULL) {
        elem->d = x;
        elem->next = NULL;
        q->rear->next = elem;
        q->rear = elem;
    }
}
