/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _MESSAGE_QUEUE_H__
#define _MESSAGE_QUEUE_H__
#if defined(__cplusplus)
extern "C" {
#endif

typedef void *DATA;
struct queue_elem {
    DATA d;
    struct queue_elem *next;
};

typedef struct queue_elem *element_t;
typedef struct queue_s *queue_t;

queue_t create_queue();
queue_t create_sized_queue(int size);
int is_empty(queue_t);
element_t dequeue(queue_t);
element_t front(queue_t q);
element_t pop_front(queue_t q);
void enqueue(queue_t, void *data);
void *end_data(queue_t q);
void reset_end(queue_t q);
void destroy_queue(queue_t q);
void free_element(queue_t q, element_t e);
void place_front(queue_t q, void *x);

#if defined(__cplusplus)
}
#endif
#endif
