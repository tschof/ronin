#include "tests.h"
#include "testmacros.h"
#include <tm_to_str.h>
#include <string.h>
#include <time.h>

static int test_unsigned_itoa(char* str, unsigned long value)
{
    int radix = 10;
    static char dig[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    long n = 0;
    unsigned long v = 0;
    char *p, *q;
    char c;
    v = value;
    do {
        str[n++] = dig[v % radix];
        v /= radix;
    } while (v);
    for (p = str, q = p + (n - 1); p < q; ++p, --q) {
        c = *p;
        *p = *q;
        *q = c;
    }
    return n;
}

static void check_tm_stuff(char* buf, long retval)
{
    memset(buf, '\0', 64);
    char* off = buf;
    long exp = retval * 10;
    while (exp < 1000000) {
        *off++ = '0';
        exp *= 10;
    }
    test_unsigned_itoa(off,retval);
}

int test_tm_to_str(const char **testname, const char **extrareporting)
{
    STARTTEST;
    *testname = "tm_to_str() test";

    struct tm t;
    char buf[64] = { '\0' };

    memset(&t, 0, sizeof t);

    t.tm_year = 111;
    t.tm_mon = 6;
    t.tm_mday = 11;
    t.tm_hour = 1;
    t.tm_min = 0;
    t.tm_sec = 0;

#define CHKCVT(retval)                                          \
    CHKCOND(tm_to_str(&t, buf, sizeof buf) == (retval));

    CHKCVT(0);
    CHKCOND(strcmp(buf, "20110711-01:00:00") == 0);

    t.tm_year = 110;
    CHKCVT(-1);

    t.tm_year = 112;
    CHKCVT(0);
    CHKCOND(strcmp(buf, "20120711-01:00:00") == 0);

    t.tm_year = 120;
    CHKCVT(-1);
    t.tm_year = 119;
    CHKCVT(0);

    t.tm_mon = -1;
    CHKCVT(-1);
    t.tm_mon = 12;
    CHKCVT(-1);
    t.tm_mon = 2;
    CHKCVT(0);
    t.tm_mday = 0;
    CHKCVT(-1);
    t.tm_mday = 32;
    CHKCVT(-1);
    t.tm_mday = 15;
    CHKCVT(0);
    t.tm_hour = -1;
    CHKCVT(-1);
    t.tm_hour = 0;
    CHKCVT(0);
    t.tm_hour = 24;
    CHKCVT(-1);
    t.tm_hour = 23;
    CHKCVT(0);
    t.tm_min = -1;
    CHKCVT(-1);
    t.tm_min = 0;
    CHKCVT(0);
    t.tm_min = 60;
    CHKCVT(-1);
    t.tm_min = 59;
    CHKCVT(0);
    t.tm_sec = -1;
    CHKCVT(-1);
    t.tm_sec = 0;
    CHKCVT(0);
    t.tm_sec = 60;
    CHKCVT(-1);
    t.tm_sec = 59;
    CHKCVT(0);

    CHKCOND(tm_to_str(0, buf, sizeof buf) == -1);
    CHKCOND(tm_to_str(&t, 0, sizeof buf) == -1);
    CHKCOND(tm_to_str(&t, buf, 17) == 0);
    CHKCOND(tm_to_str(&t, buf, 16) == -1);
    CHKCOND(tm_to_str(&t, buf, 0) == -1);

    CHKCOND(tm_to_str_millis(&t, 500, buf, sizeof buf) == 0);
    CHKCOND(strcmp(buf, "20190315-23:59:59.500") == 0);

    CHKCOND(tm_to_str_millis(&t, 123, buf, sizeof buf) == 0);
    CHKCOND(strcmp(buf, "20190315-23:59:59.123") == 0);

    CHKCOND(tm_to_str_millis(&t, 999, buf, sizeof buf) == 0);
    CHKCOND(strcmp(buf, "20190315-23:59:59.999") == 0);

    CHKCOND(tm_to_str_millis(0, 999, buf, sizeof buf) == -1);
    CHKCOND(tm_to_str_millis(&t, 0, 0, sizeof buf) == -1);
    CHKCOND(tm_to_str_millis(&t, 0, buf, 0) == -1);
    CHKCOND(tm_to_str_millis(&t, 0, buf, sizeof buf) == 0);
    CHKCOND(tm_to_str_millis(&t, 0, buf, 21) == 0);
    CHKCOND(tm_to_str_millis(&t, 0, buf, 20) == -1);
    CHKCOND(tm_to_str_millis(&t, 0, buf, 19) == -1);

    int i = 0;
    for(;i < 100000;++i) {
        memset(buf, '\0', 64);
        tm_to_str_micro(buf,24); 
        CHKCOND(buf[23] != '\0');
        CHKCOND(buf[24] == '\0');
    }
    check_tm_stuff(buf, 1);
    CHKCOND(buf[5] == '1');
    check_tm_stuff(buf, 10);
    CHKCOND(buf[4] == '1');
    check_tm_stuff(buf, 100);
    CHKCOND(buf[3] == '1');
    check_tm_stuff(buf, 1000);
    CHKCOND(buf[2] == '1');
    check_tm_stuff(buf, 10000);
    CHKCOND(buf[1] == '1');
    check_tm_stuff(buf, 100000);
    CHKCOND(buf[0] == '1');
    check_tm_stuff(buf, 99999);
    CHKCOND(buf[5] == '9');
    CHKCOND(buf[0] == '0');
    CHKCOND(buf[6] == '\0');
    STOPTEST;
}
