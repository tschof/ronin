#include "../baz/libbaz.h"
#include "tm_to_str.h"
#include "stringutil.h"
#include "databuf.h"

#include <strings.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

struct timeval subtractTimevals(struct timeval left, struct timeval right)
{
    int64_t leftms = (left.tv_sec * 1000000) + left.tv_usec;
    int64_t rightms = (right.tv_sec * 1000000) + right.tv_usec;
    int64_t diffms = leftms - rightms;
    struct timeval result = {
        .tv_sec = diffms / 1000000,
        .tv_usec = diffms % 1000000
    };
    return result;
}

void tm_to_str_micros_test(void)
{
    CHKCOND(tm_to_str_micros(0, 0, 0, 0) == -1);
    char buf[256];
    bzero(buf, sizeof buf);
    struct tm ltm;
    time_t now = time(0);
    gmtime_r(&now, &ltm);
    CHKCOND(tm_to_str_micros(&ltm, 999999, buf, sizeof buf) == 0);
    CHKCOND(strncmp(buf + TIMESTAMP_LEN, ".999999", 7) == 0);
    CHKCOND(tm_to_str_micros(&ltm, 0, buf, sizeof buf) == 0);
    CHKCOND(strncmp(buf + TIMESTAMP_LEN, ".000000", 7) == 0);
    CHKCOND(tm_to_str_micros(&ltm, 1234, buf, sizeof buf) == 0);
    CHKCOND(strncmp(buf + TIMESTAMP_LEN, ".001234", 7) == 0);
}

void testfiltercommas(void)
{
    struct databuf *buf = databuf_alloc(1);
    const char *inputp = "there,are,lots,of,commas,in,here,";
    size_t inputn = strlen(inputp);
    filtercommas(inputp, inputn, buf, ".....", 5);
    const char *verifyp =
        "there.....are.....lots.....of.....commas.....in.....here.....";
    size_t verifyn = strlen(verifyp);
    CHKCOND(databuf_unread(buf) == verifyn);
    if (databuf_unread(buf) == verifyn) {
        CHKCOND(strncmp(buf->rd_ptr, verifyp, verifyn) == 0);
    }
    char tmpbuf[inputn + 24];
    memset(tmpbuf, ',', sizeof tmpbuf);
    memcpy(tmpbuf, inputp, inputn);
    databuf_reset(buf);
    filtercommas(tmpbuf, inputn, buf, ".....", 5);
    verifyp =
        "there.....are.....lots.....of.....commas.....in.....here.....";
    verifyn = strlen(verifyp);
    CHKCOND(databuf_unread(buf) == verifyn);
    if (databuf_unread(buf) == verifyn) {
        CHKCOND(strncmp(buf->rd_ptr, verifyp, verifyn) == 0);
    }
    databuf_destroy(buf);
}

void baz_run_tests(void)
{
    tm_to_str_micros_test();
    testfiltercommas();
}
