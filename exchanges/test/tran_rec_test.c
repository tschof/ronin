
#include "tests.h"
#include "testmacros.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "transaction_recorder.h"
#include "databuf.h"


int test_date_roll_check(const char **testname,
                         const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    time_t rawtime;
    struct tm *ti;
    size_t year;
    size_t month;
    time(&rawtime);
    struct tm lm;
    ti = gmtime_r(&rawtime, &lm);
    ti = &lm;
    year = 1900 + ti->tm_year;
    month = 1 + ti->tm_mon;
    databuf_write(test_b, "%sLog%04lu%02lu%02d.%02d.%02d.%02d.LOG",
                  "TUTUEX", year, month, ti->tm_mday,
                  ti->tm_hour, ti->tm_min, ti->tm_sec);
    int check = day_check(ti->tm_mday);
    CHKCOND(check == 1);
STOPTEST}

int test_date_roll_check2(const char **testname,
                          const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    time_t rawtime;
    struct tm *ti;
    size_t year;
    size_t month;
    time(&rawtime);
    struct tm lm;
    ti = gmtime_r(&rawtime, &lm);
    ti = &lm;
    year = 1900 + ti->tm_year;
    month = 1 + ti->tm_mon;
    databuf_write(test_b, "%sLog%04lu%02lu%02d.%02d.%02d.%02d.LOG",
                  "CXOUCH", year, month, (ti->tm_mday - 1),
                  ti->tm_hour, ti->tm_min, ti->tm_sec);
    int check = day_check(ti->tm_mday - 1);
    CHKCOND(check == 0);
STOPTEST}

int test_date_roll_check3(const char **testname,
                          const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    time_t rawtime;
    struct tm *ti;
    size_t year;
    size_t month;
    time(&rawtime);
    struct tm lm;
    ti = gmtime_r(&rawtime, &lm);
    ti = &lm;
    year = 1900 + ti->tm_year;
    month = 1 + ti->tm_mon;
    databuf_write(test_b, "%sLog%04lu%02lu%02d.%02d.%02d.%02d.LOG",
                  "LOGUEX", year, month, (ti->tm_mday + 1),
                  ti->tm_hour, ti->tm_min, ti->tm_sec);
    int check = day_check(ti->tm_mday + 1);
    CHKCOND(check == 0);
STOPTEST}

int test_date_roll_check4(const char **testname,
                          const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    time_t rawtime;
    struct tm *ti;
    size_t year;
    size_t month;
    time(&rawtime);
    struct tm lm;
    ti = gmtime_r(&rawtime, &lm);
    ti = &lm;
    year = 1900 + ti->tm_year;
    month = 1 + ti->tm_mon;
    databuf_write(test_b, "%sLog%04lu%02lu%02d.LOG", "LOGUEX", year,
                  month, ti->tm_mday);
    int check = day_check(ti->tm_mday);
    CHKCOND(check == 1);
    databuf_reset(test_b);
    databuf_write(test_b, "%sLog%04lu%02lu%02d.LOG", "LOGUEX", year,
                  month, (ti->tm_mday - 7));
    check = day_check(ti->tm_mday - 7);
    CHKCOND(check == 0);
STOPTEST}
