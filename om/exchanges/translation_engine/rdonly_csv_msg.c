#include "rdonly_msg.h"
#include <string.h>
#include <sys/uio.h>
#include <stdio.h>

struct dartcsv_rdonly_msg {
    rdonlymsg_t parent;
    struct iovec *iovp;
    size_t iovn;
    char *buffer;
};

static size_t count_fields(const char *dartcsv)
{
    size_t count;
    for (count = 0; dartcsv != 0; dartcsv = strchr(dartcsv, ','), ++count) {
        if (dartcsv) {
            ++dartcsv;
        }
    }
    return count;
}

static void dartcsv_rdonly_msg_free(void *msg)
{
    if (msg != 0) {
        struct dartcsv_rdonly_msg *m = (struct dartcsv_rdonly_msg *) msg;
        free(m->buffer);
        free(m->iovp);
        free(m);
    }
}

static struct iovec dartcsv_rdonly_msg_get(rdonlymsg_t * msg, size_t field)
{
    struct iovec ret = { 0, 0 };
    if (msg != 0) {
        struct dartcsv_rdonly_msg *m = (struct dartcsv_rdonly_msg *) msg;
        if (field < m->iovn) {
            ret = m->iovp[field];
        }
    }
    return ret;
}

rdonlymsg_t *make_dartcsv_rdonly_msg(char *dartcsv_raw)
{
    struct dartcsv_rdonly_msg *m = malloc(sizeof *m);
    if (m != 0) {
        m->parent.get = dartcsv_rdonly_msg_get;
        m->parent.free = dartcsv_rdonly_msg_free;
        m->parent.dump = 0;
        m->buffer = strdup(dartcsv_raw);
        m->iovn = count_fields(m->buffer);
        m->iovp = malloc(sizeof(struct iovec) * m->iovn);
        size_t i;
        dartcsv_raw = m->buffer;
        for (i = 0; i != m->iovn;
             m->iovp[i].iov_base =
             strsep(&dartcsv_raw, ",\n"), m->iovp[i].iov_len =
             strlen((char *) m->iovp[i].iov_base), ++i);
    }
    return (rdonlymsg_t *) m;
}

#ifdef COMPILE_UNIT_TESTS
#include "../../unit-tester/unit-tester.h"

declare_unit_test(dartcsv_rdonly)
{
    char *raw_csv_message =
        strdup
        ("E,,FIXH3TRA,11856000711,1,QQQ,953,71.2,2,5,A,501,AFH21209,145,,RND,,FIXH3TRA-90130503-275,,,,,H3TRA,501,H3TRA,,,FIXH3TRA,,,,,,,,E,,,,,,145,,,,,,,,,20130503-12:03:30,,,,,,USD,,,,,,,,,,,,,,,,,,,,,,,foo\n");
    rdonlymsg_t *m = make_dartcsv_rdonly_msg(raw_csv_message);
    free(raw_csv_message);
    check(m != 0 && m->get != 0 && m->free != 0);
    if (m->get != 0) {
        /* size_t i; */
        /* for (i = 0; i < 80; ++i) */
        /*        { */
        /*          struct iovec iov = m->get (m, i); */
        /*          if (iov.iov_len > 0) */
        /*            { */
        /*              printf ("%zu: '%.*s' (%zu)\n", i, (int) iov.iov_len, (char *) iov.iov_base, */
        /*                      iov.iov_len); */
        /*            } */
        /*        } */
        struct iovec iov;
        iov = m->get(m, 0);
        check(iov.iov_len == 1);
        check(strncmp(iov.iov_base, "E", iov.iov_len) == 0);
        iov = m->get(m, 2);
        check(iov.iov_len == 8);
        check(strncmp(iov.iov_base, "FIXH3TRA", iov.iov_len) == 0);
        iov = m->get(m, 79);
        check(iov.iov_len == 3);
        check(strncmp(iov.iov_base, "foo", iov.iov_len) == 0);
    }
    if (m->free != 0) {
        m->free(m);
    }
}

#endif
