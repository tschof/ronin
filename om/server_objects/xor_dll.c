/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdlib.h>

struct xdll_elem {
    char *data[NUM_VALS];
    unsigned long link;
    int count;
    int cur_off;
};

struct xdll {
    xelement_t front;
    xelement_t rear;
    int size;
};

dllist_t create_list()
{
    struct xdll_elem *elem = malloc(sizeof(struct xdll_elem));
    struct xdll *list = malloc(sizeof(struct xdll));
    if (elem && list) {
        elem->link = 0;
        elem->count = 0;
        elem->cur_off = 0;
        list->size = 0;
        list->front = elem;
        list->rear = elem;
    } else {
        return NULL;
    }
}

void destroy(dllist_t list)
{
}

void push_back(dllist_t list, void *data)
{
}

void push_front(dllist_t list, void *data)
{
}

int get_size(dllist_t list)
{
    return list->size;
}

int is_empty(dllist_t list)
{
    return (list->size == 0);
}

void merge_node_with_next(dllist_t list, xelement_t elem)
{
}

xelement_t insert(dllist_t list, xelement_t elem, void *data)
{
    if (elem->count == NUM_VALS) {
        struct xdll_elem *e = malloc(sizeof(struct xdll_elem));

    }
}

xelement_t erase(dllist_t list, xelement_t)
{
}
