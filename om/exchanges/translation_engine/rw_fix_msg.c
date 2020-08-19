#include "rw_fix_msg.h"
#include <stdint.h>
#include <sys/uio.h>
#include <stdio.h>
#include <string.h>

struct iovmap {
    uint16_t tag, idx;
};

int cmp_iovmap(const void *l, const void *r)
{
    const struct iovmap *left = l, *right = r;
    if (left->tag < right->tag) {
        return -1;
    } else if (left->tag > right->tag) {
        return 1;
    } else {
        return 0;
    }
}

struct rw_fix_msg {
    rw_msg_t rwmsg;
    size_t nfields;
    struct iovec *iovp;
    struct iovmap *map;
    void *placeholder;
};

static inline size_t calculate_sizeof_iovp(size_t n)
{
    return n * sizeof(struct iovec) * 3;
}

static inline size_t calculate_sizeof_lookuptablep(size_t n)
{
    return n * sizeof(struct iovmap);
}

static inline size_t calculate_variable_bytes(size_t n)
{
    return calculate_sizeof_iovp(n) + calculate_sizeof_lookuptablep(n);
}

static inline size_t iovidx_tag(size_t i)
{
    return i * 3;
}

static inline size_t iovidx_val(size_t i)
{
    return (i * 3) + 1;
}

static inline size_t iovidx_delim(size_t i)
{
    return (i * 3) + 2;
}

static struct iovec rw_fix_msg_get(rw_msg_t * msg, size_t tag)
{
    struct iovec ret = { 0, 0 };
    if (tag <= UINT16_MAX) {
        struct rw_fix_msg *m = (struct rw_fix_msg *) msg;
        struct iovmap key = {.tag = (uint16_t) tag };
        void *result =
            bsearch(&key, m->map, m->nfields, sizeof(struct iovmap),
                    cmp_iovmap);
        if (result != 0) {
            struct iovmap *map = result;
            if (map->idx <= m->nfields) {
                ret = m->iovp[iovidx_val(map->idx)];
            }
        }
    }
    return ret;
}

static int rw_fix_msg_dump(rw_msg_t * msg, FILE * fd)
{
    struct rw_fix_msg *m = (struct rw_fix_msg *) msg;
    int ret = -1;
    if (m != 0 && fd != 0) {
        size_t i;
        for (i = 0; i < m->nfields; ++i) {
            if (m->iovp[iovidx_val(i)].iov_len > 0) {
                struct iovec *iov = &m->iovp[iovidx_tag(i)];
                if (fprintf(fd, "%.*s%.*s%.*s",
                            (int) iov[0].iov_len, (char *) iov[0].iov_base,
                            (int) iov[1].iov_len, (char *) iov[1].iov_base,
                            (int) iov[2].iov_len,
                            (char *) iov[2].iov_base) != -1) {
                    ret = 0;
                }
            }
        }
    }
    return ret;
}

static void rw_fix_msg_free(void *p)
{
    struct rw_fix_msg *m = p;
    size_t i;
    for (i = 0; i < m->nfields; ++i) {
        free(m->iovp[iovidx_tag(i)].iov_base);
    }
    free(m);
}

static inline ssize_t lookup_index(struct rw_fix_msg *msg, size_t field)
{
    ssize_t index = -1;
    if (field <= UINT16_MAX) {
        uint16_t key = (uint16_t) field;
        void *searchresult = bsearch(&key, msg->map, msg->nfields,
                                     sizeof msg->map[0], cmp_iovmap);
        if (searchresult != 0) {
            struct iovmap *mapping = searchresult;
            index = mapping->idx;
        }
    }
    return index;
}

static int rw_fix_msg_set(rw_msg_t * msg, size_t tag, struct iovec val)
{
    int ret = -1;
    if (msg != 0) {
        struct rw_fix_msg *m = (struct rw_fix_msg *) msg;
        ssize_t idx = lookup_index(m, tag);
        if (idx != -1) {
            m->iovp[iovidx_val(idx)] = val;
            m->iovp[iovidx_tag(idx)].iov_len
                = strlen(m->iovp[iovidx_tag(idx)].iov_base);
            m->iovp[iovidx_delim(idx)].iov_len = 1;
            ret = 0;
        }
    }
    return ret;
}

static void rw_fix_msg_clear(rw_msg_t * msg)
{
    size_t i;
    struct rw_fix_msg *m = (struct rw_fix_msg *) msg;
    for (i = 0; i < m->nfields; ++i) {
        m->iovp[iovidx_val(i)].iov_len = 0;
    }
}

static char *make_tag_buffer(size_t tag)
{
    size_t len = 8;
    char *ret = malloc(len);
    if (ret != 0) {
        int nwritten = snprintf(ret, len, "%zu=", tag);
        if (nwritten > 0 && (size_t) nwritten > len) {
            /* Not enough room, reallocate */
            len = nwritten;
            if ((ret = realloc(ret, len)) == 0) {
                goto done;
            }
            nwritten = snprintf(ret, len, "%zu=", tag);
        }
        if (nwritten < 0) {
            free(ret);
            ret = 0;
        }

    }
  done:
    return ret;
}

static int
initialize_iovec(struct rw_fix_msg *m, size_t * layoutp, size_t layoutn)
{
    size_t i;
    for (i = 0; i < layoutn; ++i) {
        char *tag = make_tag_buffer(layoutp[i]);
        if (tag != 0) {
            struct iovec *iovp = &m->iovp[iovidx_tag(i)];
            iovp->iov_base = tag;
            iovp = &m->iovp[iovidx_val(i)];
            iovp->iov_base = 0;
            iovp->iov_len = 0;
            iovp = &m->iovp[iovidx_delim(i)];
            iovp->iov_base = "\x01";
        } else {
            return -1;
        }
    }
    return 0;
}

static void
initialize_iovec_mapping(struct iovmap *map, size_t * layoutp,
                         size_t layoutn)
{
    uint16_t i;
    for (i = 0; i < layoutn; ++i) {
        map[i].tag = layoutp[i];
        map[i].idx = i;
    }
    qsort(map, layoutn, sizeof map[0], cmp_iovmap);
}

rw_msg_t *make_rw_fix_msg(message_layout_t * layout)
{
    if (layout == 0) {
        return 0;
    }
    size_t *layoutp = layout->ordered_fields;
    size_t layoutn = layout->num;
    struct rw_fix_msg *ret = 0;
    if (layoutp != 0 && layoutn > 0) {
        size_t nalloc = calculate_variable_bytes(layoutn);
        nalloc += sizeof *ret - sizeof ret->placeholder;
        if ((ret = malloc(nalloc)) != 0) {
            /* Setup the vtable */
            ret->rwmsg.get = rw_fix_msg_get;
            ret->rwmsg.free = rw_fix_msg_free;
            ret->rwmsg.dump = rw_fix_msg_dump;
            ret->rwmsg.set = rw_fix_msg_set;
            ret->rwmsg.clear = rw_fix_msg_clear;

            /* Setup the variables */
            ret->nfields = layoutn;
            char *p = (char *) &ret->placeholder;
            ret->iovp = (struct iovec *) p;
            p += calculate_sizeof_iovp(ret->nfields);
            ret->map = (struct iovmap *) p;
            initialize_iovec(ret, layoutp, layoutn);
            initialize_iovec_mapping(ret->map, layoutp, layoutn);
        }
    }
    return (rw_msg_t *) ret;
}

#ifdef COMPILE_UNIT_TESTS
#include "../../unit-tester/unit-tester.h"

declare_unit_test(basic_rw_fix_msg)
{
    check(make_rw_fix_msg(0) == 0);
    size_t layout[] = {
        1, 11, 60, 55, 65, 54, 114, 38, 111, 40, 44, 9622, 211, 110,
        47, 59, 126, 18, 7928, 9303, 9479, 439, 440, 9618, 9732
    };
    check(make_rw_fix_msg(make_message_layout((size_t *) & layout, 0)) ==
          0);
    check(make_rw_fix_msg(make_message_layout(0, 42)) == 0);
    rw_msg_t *m = make_rw_fix_msg(make_message_layout((size_t *) & layout,
                                                      sizeof layout /
                                                      sizeof layout[0]));
    check(m != 0);
    check(m->set != 0);
    if (m->set != 0) {
        struct iovec iov = {.iov_base = "JH423DEW",.iov_len = 8 };
        int ret = m->set(m, 11, iov);
        check(ret != -1);
    }
    check(m->get != 0);
    if (m->get != 0) {
        struct iovec ret = m->get(m, 11);
        check(ret.iov_base != 0 && ret.iov_len != 0);
        if (ret.iov_base != 0 && ret.iov_len != 0) {
            check(strncmp(ret.iov_base, "JH423DEW", 8) == 0);
        }
        /* if (m->dump != 0) */
        /*        { */
        /*          m->dump (m, stdout); */
        /*          puts ("\n"); */
        /*        } */
    }
    check(m->clear != 0);
    if (m->clear != 0) {
        m->clear(m);
    }
    if (m->get != 0) {
        struct iovec ret = m->get(m, 11);
        check(ret.iov_len == 0);
    }
    check(m->free != 0);
    if (m->free != 0) {
        m->free(m);
    }
}

#endif
