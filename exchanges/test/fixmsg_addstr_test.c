#include "tests.h"
#include "testmacros.h"
#include <fixmsg.h>

int test_fixmsg_addstr(const char **testname, const char **extrareporting)
{
    STARTTEST;
    databuf_t *buf = databuf_alloc(1);
    CHKCOND(fixmsg_addstr(buf, "25", "bar") == 7);
    databuf_destroy(buf);
    STOPTEST;
}

int test_fixmsg_attach(const char **testname, const char **extrareporting)
{
    STARTTEST;
    char buf[1], buf2[2];
    databuf_t dbuf;
    CHKCOND(databuf_attach(&dbuf, buf, sizeof buf) == 0);
    CHKCOND(databuf_memcpy(&dbuf, "1", 1) == 0);
    CHKCOND(databuf_memcpy(&dbuf, "1", 1) == -1);
    CHKCOND(databuf_write(&dbuf, "%d", 42) == -1);
    CHKCOND(databuf_reset(&dbuf) == 0);
    CHKCOND(databuf_write(&dbuf, "%d", 42) == -1);
    databuf_destroy(&dbuf);
    CHKCOND(databuf_realloc(&dbuf, 1024) == -1);
    CHKCOND(databuf_write(&dbuf, "%c", '\x01') == -1);
    CHKCOND(databuf_memcpy(&dbuf, "\x01", 1) == 0);
    CHKCOND(databuf_unread(&dbuf) == 1);
    CHKCOND(databuf_space_left(&dbuf) == 0);
    CHKCOND(databuf_attach(&dbuf, buf2, sizeof buf2) == 0);
    CHKCOND(databuf_unread(&dbuf) == 0);
    CHKCOND(databuf_space_left(&dbuf) == sizeof buf2);
    STOPTEST;
}
