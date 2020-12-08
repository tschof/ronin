#ifndef __DARTSTRING_H__
#define __DARTSTRING_H__

#include <string.h>

/* This is a basic attempt at having a common way to augment the C
 * run-time library string functions. */

struct dartstring {
    char *p;
    size_t n, capacity;
};

typedef struct dartstring dstr_t;

#define dstr_init {0,0,0}

static inline void init_dstr(dstr_t * str)
{
    str->p = 0;
    str->n = str->capacity = 0;
}

static inline void dstr_delete(dstr_t * str)
{
    free(str->p);
}

static inline int is_init(dstr_t * str)
{
    if (str->p == 0) {
        return 0;
    }
    return 1;
}

static inline int is_empty(dstr_t * str)
{
    if (is_init(str) == 0) {
        return 1;
    }
    if (str->n == 0) {
        return 1;
    }
    return 0;
}

static inline void dstr_append(dstr_t * str, const char *p)
{
    size_t origlen = str->n;
    str->n += strlen(p);
    str->p = realloc(str->p, str->n + 1);
    if (str->p) {
        strcpy(str->p + origlen, p);
    }
}

static inline dstr_t make_dstr(char *ptr, size_t len)
{
    dstr_t retval = {.p = ptr,.n = len,.capacity = len + 1 };
    return retval;
}

static inline void dstr_copy(dstr_t * to, char *from, size_t n)
{
    if (to->p == 0) {
        to->p = strndup(from, n);
        to->n = n;
        to->capacity = to->n + 1;
    } else {
        if (to->capacity <= n) {
            to->p = realloc(to->p, n + 1);
            to->capacity = n + 1;
        }
        memcpy(to->p, from, n);
        to->p[n] = '\0';
        to->n = n;
    }
}

static inline int dstr_unallocated(dstr_t * str)
{
    if (str->p != 0) {
        return 1;
    } else {
        return 0;
    }
}

#endif
