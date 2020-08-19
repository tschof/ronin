/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "message_queue.h"

#define FREE_LIST_SIZE 1000

struct queue_s {
    element_t front;
    element_t rear;
    queue_t free_list;
};

queue_t get_free_list(queue_t q)
{
    return q->free_list;
}

void static int_enqueue(queue_t q)
{
    struct queue_elem *elem;
    elem = malloc(sizeof(struct queue_elem));
    if (elem != NULL) {
        elem->d = 0;
        elem->next = NULL;
        q->rear->next = elem;
        q->rear = elem;
    }
}

static queue_t create_free_list(int size)
{
    struct queue_elem *elem = NULL;
    struct queue_s *q = NULL;
    int i = 0;
    elem = malloc(sizeof(struct queue_elem));
    if (elem == NULL) {
        return NULL;
    }
    elem->next = NULL;
    elem->d = NULL;

    q = malloc(sizeof(struct queue_s));
    if (q == NULL) {
        free(elem);
        return NULL;
    }
    q->front = q->rear = elem;
    q->front->d = 0;
    q->free_list = NULL;
    for (; i < size; ++i) {
        int_enqueue(q);
    }
    return q;
}

queue_t create_queue()
{
    return create_sized_queue(FREE_LIST_SIZE);
}

queue_t create_sized_queue(int fr_size)
{
    struct queue_elem *elem = NULL;
    struct queue_s *q = NULL;
    elem = malloc(sizeof(struct queue_elem));
    if (elem == NULL) {
        return NULL;
    }
    elem->next = NULL;
    elem->d = NULL;

    q = malloc(sizeof(struct queue_s));
    if (q == NULL) {
        free(elem);
        return NULL;
    }
    q->front = q->rear = elem;
    q->free_list = create_free_list(fr_size);
    return q;
}

void destroy_queue(queue_t q)
{
    if (q) {
        element_t e, temp;
        e = dequeue(q);
        while (e != NULL) {
            temp = e;
            e = temp->next;
            free(temp);
            temp = 0;
        }
        if (q->front) {
            free(q->front);
            q->front = 0;
        }
        destroy_queue(q->free_list);
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
        push_front(q->free_list, e);
    } else {
        printf("What the fuck, q element or free list is null!!!\n");
    }
}

void enqueue(queue_t q, DATA x)
{
    struct queue_elem *elem;
    if (is_empty(q->free_list)) {
        int i = 0;
        for (; i < FREE_LIST_SIZE; ++i) {
            int_enqueue(q->free_list);
        }
    }
    elem = pop_front(q->free_list);
    if (elem != NULL) {
        elem->d = x;
        elem->next = NULL;
        q->rear->next = elem;
        q->rear = elem;
    }
}
