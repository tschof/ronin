#ifndef __RWBUF_H__
#define __RWBUF_H__

#include "dartstring.h"

struct rwbuf;
typedef struct rwbuf rwbuf_t;

struct rwbuf {
    dstr_t str;
    size_t rd, wr;
};

static inline void init_rwbuf_empty(rwbuf_t * buf)
{
    init_dstr(&buf->str);
    buf->rd = buf->wr = 0;
}

static inline void init_rwbuf(rwbuf_t * buf, char *p, size_t len)
{
    buf->str = make_dstr(p, len);
    buf->rd = 0;
    buf->wr = len;
}

static inline size_t rwbuf_get_unread(rwbuf_t * buf)
{
    return buf->wr - buf->rd;
}

static inline void *rwbuf_get_rdptr(rwbuf_t * buf)
{
    return buf->str.p + buf->rd;
}

static inline void *rwbuf_get_wrptr(rwbuf_t * buf)
{
    return buf->str.p + buf->wr;
}

#endif
