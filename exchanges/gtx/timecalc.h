#ifndef __TIMECALC_H__
#define __TIMECALC_H__

#include <time.h>

int std_time_str_to_int(const char *str, time_t * t);
int std_time_int_to_str(time_t t, char **strbuf);
int calculate_next_time(int day_of_week, int hour, int minute,
                        time_t now, time_t * next_time);
int add_days(const char *const str, int ndays, char **new_day_str);
#endif                          //__TIMECALC_H__
