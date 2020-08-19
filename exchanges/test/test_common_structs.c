#include "tests.h"
#include "testmacros.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

#include "common_structs.h"
#include "dart_seq_request.h"

int test_itoa(const char **testname, const char **extrareporting)
{
    STARTTEST 
    char* buff = calloc(30, 1);
    char* buff2 = calloc(30, 1);
    int len = 0;
    int len2 = 0;
    REQCOND(buff != 0);
    int i = 0;
    for(;i < 11000; ++i) {
        len = dart_itoa(i, buff, 10);
        len2 = sprintf(buff2, "%d", i); 
        CHKCOND(len == len2);
        CHKCOND(strcmp(buff, buff2) == 0);
        memset(buff, '\0', 30);
    }
    free(buff);
    free(buff2);
    STOPTEST
}

static int reset_day_offsets[7] [7] = 
{
    {7,6,5,4,3,2,1},
    {1,7,6,5,4,3,2},
    {2,1,7,6,5,4,3},
    {3,2,1,7,6,5,4},
    {4,3,2,1,7,6,5},
    {5,4,3,2,1,7,6},
    {6,5,4,3,2,1,7}    
};

int test_time_t_convert(const char** testname, const char** extrareporting)
{
    STARTTEST 
    char* buff = calloc(30, 1);
    REQCOND(buff != 0);
    memcpy(buff, "20130510230134", 14);
    time_t ret = convert_to_sec_since_1970(buff);
    struct tm t;
    localtime_r(&ret, &t);
    CHKCOND((t.tm_year + 1900) == 2013);
    CHKCOND(t.tm_sec == 34);
    CHKCOND(t.tm_min == 1);
    CHKCOND(reset_day_offsets[0][0] == 7);
    CHKCOND(reset_day_offsets[1][0] == 1);
    CHKCOND(reset_day_offsets[2][4] == 5);
    CHKCOND(reset_day_offsets[1][6] == 2);
    STOPTEST
}
int test_make_long(const char** testname, const char** extrareporting)
{
    STARTTEST 
    char* buff = calloc(30, 1);
    REQCOND(buff != 0);
    int len = sprintf(buff, "%d", 2345);
    int result = make_long(buff, len);
    CHKCOND(result == 2345);
    sprintf(buff, "%d", 999898);
    result = make_long(buff, 2);
    CHKCOND(result == 99);
    sprintf(buff, "%d", 201305);
    STOPTEST
}
int test_last_logon_logic(const char** testname, const char** extrareporting)
{
    STARTTEST 
    char* buff = calloc(30, 1);
    REQCOND(buff != 0);
    struct init_args* in = calloc(1, sizeof(struct init_args));
    struct seq_reset_time* srt = calloc(1, sizeof(struct seq_reset_time));
    //year different and last logon day less than current day.
    memcpy(buff, "20130509220324", 14);
    srt->last_logon = convert_to_sec_since_1970(buff);
    memcpy(buff, "20130510230425", 14);
    time_t rawtime = convert_to_sec_since_1970(buff);
    srt->reset_hour = 23;
    srt->reset_min = 9;
    srt->reset_day = 7;
    in->reset_time = srt;
    int result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 0);
    //Year is different current year day is less than last logon year day.
    memcpy(buff, "20121231235955", 14);
    srt->last_logon = convert_to_sec_since_1970(buff);
    memcpy(buff, "20130101000000", 14);
    rawtime = convert_to_sec_since_1970(buff);
    srt->reset_hour = 0;
    srt->reset_min = 0;
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 1);
    //Hour is behind
    memcpy(buff, "20130101225955", 14);
    srt->last_logon = convert_to_sec_since_1970(buff);
    memcpy(buff, "20130101230000", 14);
    rawtime = convert_to_sec_since_1970(buff);
    srt->reset_hour = 23;
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 1);
    //Current hour is before reset.
    memcpy(buff, "20130101225955", 14);
    srt->last_logon = convert_to_sec_since_1970(buff);
    memcpy(buff, "20130101225956", 14);
    rawtime = convert_to_sec_since_1970(buff);
    srt->reset_hour = 23;
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 0);
    //Current hour is before reset.
    memcpy(buff, "20130111025955", 14);
    srt->last_logon = convert_to_sec_since_1970(buff);
    memcpy(buff, "20130111025956", 14);
    rawtime = convert_to_sec_since_1970(buff);
    srt->reset_hour = 23;
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 0);
    //Overnight session test.
    memcpy(buff, "20130110182500", 14);
    srt->last_logon = convert_to_sec_since_1970(buff);
    memcpy(buff, "20130111025956", 14);
    rawtime = convert_to_sec_since_1970(buff);
    srt->reset_hour = 18;
    srt->reset_min = 20;
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 0);

    //Overnight session test positive reset.
    memcpy(buff, "20130110182500", 14);
    srt->last_logon = convert_to_sec_since_1970(buff);
    memcpy(buff, "20130111185956", 14);
    rawtime = convert_to_sec_since_1970(buff);
    srt->reset_hour = 18;
    srt->reset_min = 20;
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 1);
    //We start evening
    memcpy(buff, "20130915091530", 14);
    srt->last_logon = convert_to_sec_since_1970(buff);
    memcpy(buff, "20130915182020", 14);
    rawtime = convert_to_sec_since_1970(buff);
    srt->reset_hour = 9;
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 0);

    //We start evening reset tomorrow
    memcpy(buff, "20130915091530", 14);
    srt->last_logon = convert_to_sec_since_1970(buff);
    memcpy(buff, "20130915182020", 14);
    rawtime = convert_to_sec_since_1970(buff);
    srt->reset_hour = 9;
    srt->reset_day = 1;
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 0);

   //logged in yesterday at the reset time. 
    memcpy(buff, "20130915091530", 14);
    srt->last_logon = convert_to_sec_since_1970(buff);
    memcpy(buff, "20130916090020", 14);
    rawtime = convert_to_sec_since_1970(buff);
    srt->reset_hour = 9;
    srt->reset_day = 1;
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 1);

    //weekday checks
    srt->reset_hour = 23;
    srt->reset_min = 9;
    srt->reset_day = 0;
    memcpy(buff, "20130101025955", 14);
    srt->last_logon = convert_to_sec_since_1970(buff);
    memcpy(buff, "20130104025956", 14);
    rawtime = convert_to_sec_since_1970(buff);
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 0);

    //weekday should reset
    srt->reset_hour = 23;
    srt->reset_min = 9;
    srt->reset_day = 0;
    memcpy(buff, "20121229025955", 14);
    srt->last_logon = convert_to_sec_since_1970(buff);
    memcpy(buff, "20130101025956", 14);
    rawtime = convert_to_sec_since_1970(buff);
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 1);
    //Sunday to Monday daily reset at 7:00pm should not reset
    srt->reset_hour = 19;
    srt->reset_min = 0;
    srt->reset_day = 7;
    memcpy(buff, "20140713070255", 14);
    srt->last_logon = convert_to_sec_since_1970(buff);
    memcpy(buff, "20140714000956", 14);
    rawtime = convert_to_sec_since_1970(buff);
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 0);

    srt->reset_hour = 0;
    srt->reset_min = 10;
    srt->reset_day = 7;
    srt->last_logon = 1416982202;
    memcpy(buff, "20141126181900", 14);
    rawtime = convert_to_sec_since_1970(buff);

    struct tm ti;
    gmtime_r(&rawtime, &ti);
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 0);
    
    memcpy(buff, "20141127001900", 14);
    rawtime = convert_to_sec_since_1970(buff);
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 1);
    memcpy(buff, "20141127000900", 14);
    rawtime = convert_to_sec_since_1970(buff);
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 0);
    srt->reset_hour = 7;
    srt->reset_min = 20;
    srt->reset_day = 7;
    //srt->last_logon = 1435696150;
    srt->last_logon = 1454937606;
    localtime_r(&srt->last_logon, &ti);
    printf("Last logon hour: %d, min: %d \n", ti.tm_hour, ti.tm_min) ;
    memcpy(buff, "20160208112408", 14);
    rawtime = convert_to_sec_since_1970(buff);
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 0);
    STOPTEST
}
