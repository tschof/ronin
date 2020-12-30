/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _XORLIST_H__
#define _XORLIST_H__
#if defined(__cplusplus)
extern "C" {
#endif

typedef struct elementX {
    void* data;
    struct elementX* link;
}elemx;

typedef struct xorList xorList;

xorList* xcreate_list(size_t size);

void xdestroy_list(xorList* x);

elemx* xiter(xorList* x);

void* xpop_front(xorList* x);

int xput_front(xorList* x, void* data);


#if defined(__cplusplus)
}
#endif
#endif
