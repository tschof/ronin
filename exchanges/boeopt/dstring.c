#include "dstring.h"

struct dstring *dstring_attach(struct dstring *str, char *p,
                               uint16_t len, int growable)
{
    str->p = p;
    str->nlen = 0;
    str->nalloc = len;
    str->flags = growable != 0 ? DSTRING_GROWABLE : 0x0000;
    return str;
}
