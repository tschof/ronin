#ifndef __DSTRING_H__
#define __DSTRING_H__

#include <stdint.h>

#define DSTRING_GROWABLE 0x0001

struct dstring {
    char *p;
    uint16_t nlen, nalloc, flags;
};

struct dstring *dstring_attach(struct dstring *, char *, uint16_t, int);

#endif
