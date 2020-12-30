/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include  "xorlist.h"
#include "rex_allocator.h"



#define ELEM_SIZE 16
#define ELEM_PER_PAGE 256
struct xorList {
     elemx* head;
     elemx* freeList;
};

static elemx* x_or(elemx* x, elemx* y)
{
    return (elemx*)(((uintptr_t)(x)) ^ ((uintptr_t)(y)));
}
void init_list(xorList* x, int numElements)
{
    elemx* list = x->freeList;
    elemx* prev = 0;
    do {
        elemx* e = list;
        e->data = 0;
        e->link = x_or(prev, NULL);
        if(prev) {
            prev->link = x_or(e, x_or(prev->link, NULL));
        }
        prev = e;
        list += ELEM_SIZE;
        numElements--;
    } while (numElements);
}
static void init_free_list(xorList* x, int numElements)
{
    elemx* list = x->freeList;
    elemx* cur = 0;
    do {
        cur = list;
        cur->data = 0;
        list += ELEM_SIZE;
        cur->link = list;
        numElements--;
    } while(numElements);
}
xorList* create_list(size_t size)
{
    xorList* x = 0;
    int ret = 0;
    REX_ALLOC(x,8, sizeof(struct xorList), ret);
    x->freeList = 0;
    REX_ALLOC(x->freeList, 8, size * (sizeof (struct elementX)), ret);
    init_free_list(x, size);
    return x;
}
elemx* xiter(xorList* x)
{
    return x->head;
}

void xdestroy_list(xorList* x)
{
    free(x->freeList);
    free(x);
}

void* xpop_front(xorList* x)
{
    return NULL;
}

int xput_front(xorList* x, void* data)
{
    int ret = 0;
    if(x->freeList) {
        elemx* e = x->freeList;
        x->freeList = e->link;
        e->data = data;
        e->link = x_or(x->head, NULL);
        if(x->head) {
            x->head->link = x_or(e, x_or(x->head->link, NULL));
        }
        x->head = e;
        ret = 1;
    }
    return ret;
}

