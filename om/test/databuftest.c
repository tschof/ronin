#include "tests.h"
#include "testmacros.h"
#include <assert.h>
#include <stdio.h>
#include <databuf.h>
#include <string.h>
#include <time.h>

int testdatabuf(const char **testname, const char **extrareporting)
{
    STARTTEST;
    databuf_t *dbuff = databuf_alloc(1);
    const char *testbuf = "12345678901234567890";
    REQCOND(dbuff != 0);

    CHKCOND(databuf_write(dbuff, "%.*s", 10, testbuf) == 10);
    CHKCOND(databuf_write(dbuff, "%.*s", 10, testbuf + 10) == 10);
    CHKCOND(strcmp(dbuff->rd_ptr, testbuf) == 0);

    databuf_t* tbuff = databuf_alloc(1);
    CHKCOND(databuf_write(tbuff, "%s+%d=", testbuf, 2400) == 26);

    databuf_destroy(dbuff);
    databuf_destroy(tbuff);
    STOPTEST;
}

int testdatabuf_realloc(const char **testname, const char **extrareporting)
{
    STARTTEST;
    char *rom_res =
        "!#!E,,,L1,2,ANF,1,,,,,,,,,,,,,,,,,,,,,,,,201101,C,43,,,O,,,1,,,,,,,,,,,,,,,,,,,,,,,,22,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,!#!E,,,L2,2,ANF,1,,,,,,,,,,,,,,,,,,,,,,,,201101,P,42,,,O,,,1,,,,,,,,,,,,,,,,,,,,,,,,22,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,!#!";
    int len = strlen(rom_res);
    databuf_t *dbuf = databuf_alloc(3000);
    REQCOND(dbuf != 0);
    int i = 0;
    for (i = 0; i < 20000; ++i) {
        databuf_memcpy(dbuf, rom_res, len);
        if (i % 10 == 0) {
            dbuf->rd_ptr += 1;
            CHKCOND(dbuf->rd_ptr != 0);
            CHKCOND((dbuf->wr_ptr - 1)[0] == '!');
        }
    }
    databuf_reset(dbuf);
    CHKCOND(databuf_unread(dbuf) == 0);
    databuf_destroy(dbuf);

    databuf_t* buff = databuf_alloc(10);
    REQCOND(buff != 0);
    databuf_memcpy(buff, "1234567", 7);
    CHKCOND(databuf_unread(buff) == 7);
    buff->rd_ptr += 2;
    CHKCOND(databuf_unread(buff) == 5);
    CHKCOND(strncmp("34567", buff->rd_ptr, 5) == 0);
    CHKCOND(strncmp("1234567", buff->buffer, 7) == 0);
    databuf_memcpy(buff, "8888888888", 10);
    CHKCOND(databuf_unread(buff) == 15);
    CHKCOND(strncmp("345678888888888", buff->buffer, 15) == 0);
    CHKCOND(strncmp("345678888888888", buff->rd_ptr, 15) == 0);
    databuf_destroy(buff);
    STOPTEST;
}

