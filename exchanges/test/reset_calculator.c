#include "testmacros.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define STDTIMESTRING "%04d%02d%02d-%02d:%02d"

int std_time_str_to_int(const char *str, time_t * t)
{
    int result = -1;
    if (str != 0 && t != 0) {
        struct tm ltm = { 0 };
        int result =
            sscanf(str, STDTIMESTRING, &(ltm.tm_year), &(ltm.tm_mon),
                   &(ltm.tm_mday), &(ltm.tm_hour), &(ltm.tm_min));
        if (result == 5) {
            ltm.tm_year -= 1900;
            ltm.tm_mon -= 1;
            *t = mktime(&ltm);
            result = 0;
        }
    }
    return result;
}

int calculate_next_time(int day_of_week, int hour, int minute,
                        time_t now, time_t * next_time)
{
    int result = -1;
    if (day_of_week > -1 && day_of_week < 7) {
        struct tm *ptm = localtime(&now);
        /* Fixup the day */
        if (ptm->tm_wday < day_of_week) {
            ptm->tm_mday = day_of_week - ptm->tm_wday;
        } else if (ptm->tm_wday > day_of_week) {
            ptm->tm_mday += (day_of_week + 7) - ptm->tm_wday;
        } else {
            /* Same day, are we past the cutoff time? */
            if (ptm->tm_hour > hour && ptm->tm_min > minute) {
                ptm->tm_mday += 7;
            }
        }
        ptm->tm_hour = hour;
        ptm->tm_min = minute;
        ptm->tm_sec = 0;
        ptm->tm_isdst = -1;
        *next_time = mktime(ptm);
        if (*next_time != -1) {
            result = 0;
        }
    }
    return result;
}

char *next_reset_time(time_t now)
{
    char *next_time = 0;
    if (now != 0) {
        struct tm *reset = localtime(&now);
        if (reset != 0) {
            reset->tm_mday += 7 - reset->tm_wday;
            reset->tm_hour = 17;
            reset->tm_min = 0;
            reset->tm_sec = 0;
            time_t tmp = mktime(reset);
            char *str = ctime(&tmp);
            size_t nlen = strlen(str);
            next_time = malloc(nlen + 1);
            strcpy(next_time, str);
            next_time[nlen] = '\0';
        }
    }
    return 0;
}

int test_reset_calculator(const char **testname,
                          const char **extrareporting)
{
    STARTTEST;
    char *nextreset = next_reset_time(0);
    REQCOND(nextreset == 0);
    time_t tmp = 0;
    CHKCOND(std_time_str_to_int("20110125-25:30", &tmp) == 0);
    char *ptimestr = ctime(&tmp);
    if (ptimestr != 0) {
        printf("%s\n", ptimestr);
    }
    time_t next_time = 0;
    CHKCOND(calculate_next_time(0, 17, 30, tmp, &next_time) == 0);
    char *pnext_time_str = ctime(&next_time);
    if (pnext_time_str != 0) {
        printf("%s\n", pnext_time_str);
    }
    STOPTEST;
}
