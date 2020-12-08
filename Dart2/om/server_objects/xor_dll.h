/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _XOR_DOUBLE_LINK_LIST_H__
#define _XOR_DOUBLE_LINK_LIST_H__
#if defined(__cplusplus)
extern "C" {
#endif

#define NUM_VALS 4

typedef struct xdll_elem *xelement_t;
typedef struct xdll *dllist_t;

dllist_t create_list();
void destroy(dllist_t list);
void push_back(dllist_t list, void *data);
void push_front(dllist_t list, void *data);

int _size(dllist_t);
int is_empty(dllist_t);
void merge_node_with_next(dllist_t list, xelement_t elem);

xelement_t insert(dllist_t list, void *data);
xelement_t erase(dllist_t list, xelement_t);

#if defined(__cplusplus)
}
#endif
#endif
