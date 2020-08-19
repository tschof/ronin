#include "tests.h"
#include "testmacros.h"
#include <assert.h>
#include <stdio.h>
#include <databuf.h>
#include <string.h>
#include "fixmsg.h"
#include <time.h>

int testdatabuf(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *dbuff = databuf_alloc(1);
    const char *testbuf = "12345678901234567890";
    REQCOND(dbuff != 0);

    CHKCOND(databuf_write(dbuff, "%.*s", 10, testbuf) == 10);
    CHKCOND(databuf_write(dbuff, "%.*s", 10, testbuf + 10) == 10);
    CHKCOND(strcmp(dbuff->rd_ptr, testbuf) == 0);

STOPTEST}

int testfixdatabuf(const char **testname, const char **extrareporting)
{
    STARTTEST;
    databuf_t *dbuff = databuf_alloc(256);
    size_t nwritten;
    struct tm test_tm = { 0 };
    REQCOND(dbuff != 0);
    CHKCOND(fixmsg_addstr(dbuff, "44", "42.00") == 9);
    CHKCOND(fixmsg_addstr_bylen(dbuff, "11", "1234567890", 5) == 9);
    CHKCOND(strncmp
            (dbuff->rd_ptr, "\x01" "44=42.00" "\x01" "11=12345",
             databuf_unread(dbuff)) == 0);
    nwritten = databuf_unread(dbuff);
    memset(&test_tm, 0, sizeof test_tm);
    test_tm.tm_year = 111;
    test_tm.tm_mon = 8;
    test_tm.tm_mday = 27;
    test_tm.tm_hour = 17;
    test_tm.tm_min = 5;
    test_tm.tm_sec = 0;
    CHKCOND(fixmsg_addtime(dbuff, "52", &test_tm) == 0);
    CHKCOND((databuf_unread(dbuff) - nwritten) == 21);
    CHKCOND(strncmp
            (dbuff->rd_ptr,
             "\x01" "44=42.00" "\x01" "11=12345" "\x01"
             "52=20110927-17:05:00", databuf_unread(dbuff)) == 0);
    STOPTEST;
}

int testdatabuf_realloc(const char **testname, const char **extrareporting)
{
    STARTTEST;
    char *rom_res =
        "!#!E,,,L1,2,ANF,1,,,,,,,,,,,,,,,,,,,,,,,,201101,C,43,,,O,,,1,,,,,,,,,,,,,,,,,,,,,,,,22,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,!#!E,,,L2,2,ANF,1,,,,,,,,,,,,,,,,,,,,,,,,201101,P,42,,,O,,,1,,,,,,,,,,,,,,,,,,,,,,,,22,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,!#!";
    int len = strlen(rom_res);
    databuf_t* dbuf = databuf_alloc(3000);
    REQCOND(dbuf != 0);
    int i = 0;
    for(i =0; i < 20000;++i) {
        databuf_memcpy(dbuf, rom_res, len);
        if(i%10 == 0) {
            dbuf->rd_ptr += 1;
            CHKCOND(dbuf->rd_ptr != 0);
            CHKCOND((dbuf->wr_ptr -1)[0] == '!');
        }
    }
    databuf_reset(dbuf);
    CHKCOND(databuf_unread(dbuf) == 0);
    databuf_destroy(dbuf);
    STOPTEST;
}
