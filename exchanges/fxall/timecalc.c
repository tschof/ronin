#include "timecalc.h"
#include <stdio.h>
#include <stdlib.h>

#define STDTIMESTRING "%04d%02d%02d-%02d:%02d"

int std_time_int_to_str(time_t t, char **strbuf)
{
    int result = -1;
    if (t > -1 && strbuf != 0) {
        *strbuf = malloc(16);
        if (*strbuf != 0) {
            struct tm *ltm = localtime(&t);
            if (ltm != 0
                && sprintf(*strbuf, STDTIMESTRING, ltm->tm_year + 1900,
                           ltm->tm_mon + 1, ltm->tm_mday, ltm->tm_hour,
                           ltm->tm_min) != -1) {
                result = 0;
            }
        }
    }
    return result;
}

int std_time_str_to_int(const char *str, time_t * t)
{
    int result = -1;
    if (str != 0 && t != 0) {
        struct tm ltm = { 0 };
        int nfields =
            sscanf(str, STDTIMESTRING, &(ltm.tm_year), &(ltm.tm_mon),
                   &(ltm.tm_mday), &(ltm.tm_hour), &(ltm.tm_min));
        if (nfields == 5) {
            ltm.tm_year -= 1900;
            ltm.tm_mon -= 1;
            *t = mktime(&ltm);
            result = 0;
        }
    }
    return result;
}

int add_days(const char *const str, int ndays, char **new_day_str)
{
    int result = -1;
    if (str != 0 && ndays > -1 && ndays << 7 && new_day_str != 0) {
        struct tm ltm = { 0 };
        int nfields =
            sscanf(str, STDTIMESTRING, &(ltm.tm_year), &(ltm.tm_mon),
                   &(ltm.tm_mday), &(ltm.tm_hour), &(ltm.tm_min));
        if (nfields == 5) {
            ltm.tm_year -= 1900;
            ltm.tm_mon -= 1;
            ltm.tm_mday += ndays;
            ltm.tm_yday += ndays;
            (void) mktime(&ltm);        /* Only reason to call mktime() is to normalize
                                           the struct tm; the time_t result is not used
                                         */
            *new_day_str = malloc(15);
            if (*new_day_str != 0) {
                int nwritten = sprintf(*new_day_str, STDTIMESTRING,
                                       ltm.tm_year + 1900, ltm.tm_mon + 1,
                                       ltm.tm_mday, ltm.tm_hour,
                                       ltm.tm_min);
                if (nwritten <= 15) {
                    result = 0;
                }
            }
        }
    }
    return result;
}

/*
  @arg str Takes 
  Sun = 0;
  Mon = 1;
  Tue = 2;
  Wed = 3;
  Thu = 4;
  Fri = 5;
  Sat = 6;
 */
int day_str_to_int(const char *const str)
{
    int day = -1;
    if (str != 0) {
        switch (str[0]) {
        case 'F':
        case 'f':
            /* Fri */
            day = 5;
            break;
        case 'M':
        case 'm':
            /* Mon */
            day = 1;
            break;
        case 'S':
        case 's':
            if (str[1] == 'a' || str[1] == 'A') {
                /* Sat */
                day = 6;
            } else if (str[1] == 'u' || str[1] == 'U') {
                /* Sun */
                day = 0;
            }
            break;
        case 'T':
        case 't':
            if (str[1] == 'h' || str[1] == 'H') {
                /* Thu */
                day = 4;
            } else if (str[1] == 'u' || str[1] == 'U') {
                /* Tue */
                day = 2;
            }
            break;
        case 'W':
        case 'w':
            /* Wed */
            day = 3;
            break;
        }
    }
    return day;
}

int calculate_next_time(int day_of_week, int hour, int minute,
                        time_t now, time_t * next_time)
{
    int result = -1;
    if (day_of_week > -1 && day_of_week < 7) {
        struct tm *now_tm = localtime(&now);
        /* Fixup the day */
        if (now_tm->tm_wday < day_of_week) {
            now_tm->tm_mday = day_of_week - now_tm->tm_wday;
        } else if (now_tm->tm_wday > day_of_week) {
            now_tm->tm_mday += (day_of_week + 7) - now_tm->tm_wday;
        } else {
            /* Same day, are we past the cutoff time? */
            if (now_tm->tm_hour >= hour && now_tm->tm_min >= minute) {
                now_tm->tm_mday += 7;
            }
        }
        now_tm->tm_hour = hour;
        now_tm->tm_min = minute;
        now_tm->tm_sec = 0;
        now_tm->tm_isdst = -1;
        *next_time = mktime(now_tm);
        if (*next_time != -1) {
            result = 0;
        }
    }
    return result;
}
