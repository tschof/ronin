#ifndef __DSTRING_H__
#define __DSTRING_H__

#include <stdint.h>
#include <stdlib.h>

#define DSTRING_GROWABLE 0x0001

struct dstring {
    char *p;
    uint16_t nlen, nalloc, flags;
};

struct dstring *dstring_attach(struct dstring *, char *, uint16_t, int);

typedef struct rdonly_str {
    const char *p;
    size_t n;
} rdonlystr_t;

#endif
