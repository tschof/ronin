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
#include "lifecycle_master.h"


int test_timer(const char** testname, const char** extrareporting)
{
    STARTTEST 
    char* buff = calloc(30, 1);
    REQCOND(buff != 0);
    struct tm t;
    t.tm_sec = 0;
    t.tm_min = 0;
    t.tm_hour = 23;
    t.tm_mon = 1;
    t.tm_year = 1000;
    t.tm_wday = 2;
    t.tm_yday = 230;
    t.tm_isdst = 1;
    int end_hour = 17;
    int end_min = 24;
    int end_day = 7;
    int val =  get_sec_till_end_time(end_hour, end_min, end_day,
                          &t);
    CHKCOND(val == 66240);
    t.tm_hour = 6;
    t.tm_min = 32;
    val = get_sec_till_end_time(end_hour, end_min, end_day, &t);
    CHKCOND(val == 39120);
    t.tm_hour = 17;
    t.tm_min = 5;
    val = get_sec_till_end_time(end_hour, end_min, end_day, &t);
    CHKCOND(val == 1140); 
    end_day = 5;
    val = get_sec_till_end_time(end_hour, end_min, end_day, &t);
    CHKCOND(val == 260340);
    end_day = 2;
    val = get_sec_till_end_time(end_hour, end_min, end_day, &t);
    CHKCOND(val == 1140); 



    end_hour = 17;
    end_min = 10;
    end_day = 7;
    t.tm_hour =19;
    t.tm_min = 1;
    int sec_till_drop = get_sec_till_end_time(end_hour, end_min,
                                              end_day, &t); 

    CHKCOND(sec_till_drop == 79740);
    memset(buff, '\0', 30);
    memcpy(buff, "20130510230134", 14);
    struct local;
    memset(&t, 0, sizeof(struct tm));
    t.tm_year = make_long(buff, 4) - 1900;
    t.tm_mon = make_long(buff + 4, 2) -1;
    t.tm_mday = make_long(buff + 6, 2);
    t.tm_hour = make_long(buff + 9, 2);
    t.tm_min = make_long(buff + 12, 2);
    t.tm_sec = make_long(buff +15, 2);
    t.tm_isdst = 1;
    mktime(&t);
    memset(&t, 0, sizeof(struct tm));
    t.tm_year = make_long(buff, 4) - 1900;
    t.tm_mon = make_long(buff + 4, 2) -1;
    t.tm_mday = make_long(buff + 6, 2);
    t.tm_hour = make_long(buff + 9, 2);
    t.tm_min = make_long(buff + 12, 2);
    t.tm_sec = make_long(buff +15, 2);
    t.tm_isdst = 0;
    mktime(&t);
    STOPTEST
}

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
    ret = convert_to_sec_since_1970("20110113090253");
    struct tm* pt = gmtime(&ret);
    CHKCOND((pt->tm_year + 1900) == 2011);
    CHKCOND(pt->tm_sec == 53);
    ret = convert_to_sec_since_1970("20110113090253");
    pt = localtime(&ret);
    CHKCOND((pt->tm_year + 1900) == 2011);
    CHKCOND(pt->tm_sec == 53);
    long time2 = convert_to_nanosec_since_1970("20110113-09:02:53");
    CHKCOND(time2 == 1294909373000000000);
    ret = 1294909373;
    pt = gmtime(&ret);
    CHKCOND(pt->tm_sec == 53);
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
/*Keith test*/
    long nanos_per_sec = 1000000000;
    unsigned long val = 1294909373757324000;//1234567890123456789;
    long epoch_time = 0;
    long nanos = 0;
    long testy = 0;
    //unsigned long long testy = 1294909373757324000;
    epoch_time = val/nanos_per_sec;
    unsigned long d = 1UL << epoch_time;
    nanos = val & (d - 1);
    testy = val % epoch_time; 
    
    printf("IN: val=%lu; ouput: secs: %lu; nanos: %ld; testy: %ld\n", val, epoch_time, nanos, testy);



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
    memcpy(buff, "20120210230425", 14);
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

    memcpy(buff, "20140522132207", 14);
    rawtime = convert_to_sec_since_1970(buff);
    memcpy(buff, "20140521193507", 14);
    srt->last_logon = convert_to_sec_since_1970(buff);
    srt->reset_hour = 19;
    srt->reset_min = 30;
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 0);


    memcpy(buff, "20140522193507", 14);
    rawtime = convert_to_sec_since_1970(buff);
    memcpy(buff, "20140521193507", 14);
    srt->last_logon = convert_to_sec_since_1970(buff);
    srt->reset_hour = 19;
    srt->reset_min = 30;
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 1);

    memcpy(buff, "20140522093007", 14);
    rawtime = convert_to_sec_since_1970(buff);
    memcpy(buff, "20140521093007", 14);
    srt->last_logon = convert_to_sec_since_1970(buff);
    srt->reset_hour = 9;
    srt->reset_min = 30;
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 1);


    memcpy(buff, "20140522093007", 14);
    rawtime = convert_to_sec_since_1970(buff);
    memcpy(buff, "20140522093507", 14);
    srt->last_logon = convert_to_sec_since_1970(buff);
    srt->reset_hour = 9;
    srt->reset_min = 30;
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 0);


    memcpy(buff, "20140915093007", 14);
    rawtime = convert_to_sec_since_1970(buff);
    memcpy(buff, "20140908093507", 14);
    srt->last_logon = convert_to_sec_since_1970(buff);
    srt->reset_hour = 9;
    srt->reset_min = 30;
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 1);

    memcpy(buff, "20140915080000", 14);
    rawtime = convert_to_sec_since_1970(buff);
    memcpy(buff, "20140912093007", 14);
    srt->last_logon = convert_to_sec_since_1970(buff);
    srt->reset_hour = 9;
    srt->reset_min = 30;
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 1);


    memcpy(buff, "20140915080000", 14);
    rawtime = convert_to_sec_since_1970(buff);
    memcpy(buff, "20140912093007", 14);
    srt->last_logon = convert_to_sec_since_1970(buff);
    srt->reset_hour = 9;
    srt->reset_min = 30;
    srt->reset_day = 0;
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 1);

    memcpy(buff, "20140914080000", 14);
    rawtime = convert_to_sec_since_1970(buff);
    memcpy(buff, "20140910093007", 14);
    srt->last_logon = convert_to_sec_since_1970(buff);
    srt->reset_hour = 9;
    srt->reset_min = 30;
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 0);

    memcpy(buff, "20140914080000", 14);
    rawtime = convert_to_sec_since_1970(buff);
    memcpy(buff, "20140910093007", 14);
    srt->last_logon = convert_to_sec_since_1970(buff);
    srt->reset_hour = 9;
    srt->reset_min = 30;
    srt->reset_day = 5;
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 1);

    memcpy(buff, "20140914080000", 14);
    rawtime = convert_to_sec_since_1970(buff);
    memcpy(buff, "20140912093007", 14);
    srt->last_logon = convert_to_sec_since_1970(buff);
    srt->reset_hour = 9;
    srt->reset_min = 30;
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 0);

    memcpy(buff, "20160614142500", 14);
    rawtime = convert_to_sec_since_1970(buff);
    srt->last_logon = 1460494891;
    srt->reset_hour = 9;
    srt->reset_day = 7;
    srt->reset_min = 30;
    result = ll_before_reset_time(in, rawtime);
    CHKCOND(result == 0);
    STOPTEST
}
