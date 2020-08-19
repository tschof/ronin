#include "Slice.h"

int consumeBytes(struct Slice *slice, size_t n)
{
    if (n > slice->n)
        return -1;
    slice->p += n;
    slice->n -= n;
    return 0;
}

struct Slice makeSlice(char *p, size_t n)
{
    struct Slice slice = {.p = p,.n = n };
    return slice;
}
