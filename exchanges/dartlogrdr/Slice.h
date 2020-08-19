#ifndef __SLICE_H__
#define __SLICE_H__

#include <stdlib.h>

struct Slice {
    char *p;
    size_t n;
};

struct Slice makeSlice(char *, size_t);
int consumeBytes(struct Slice *, size_t);

typedef struct Slice slice;

#endif
