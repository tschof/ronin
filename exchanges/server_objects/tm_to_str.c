#include "tm_to_str.h"
#include <string.h>
#include <stdio.h>

#define FULL_VALIDATION
/* #define UNROLLED_LOOP */

const char *yr_str[] =
    { "2011", "2012", "2013", "2014", "2015", "2016", "2017", "2018",
    "2019","2020","2021","2022","2023"
};

const char *digit_str[] =
    { "00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
    "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
    "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
    "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
    "50", "51", "52", "53", "54", "55", "56", "57", "58", "59"
};

/**
 * \brief Converts a struct tm into a human-readable string.
 *
 * \returns 0 on success, -1 on error
 *
 * The format followed is YYYYMMDD-HH:MM:SS, 17 characters
 */
int tm_to_str(const struct tm *tm, char *buf, size_t len)
{
    int retval = -1;
    if (tm != 0 && buf != 0 && len >= TIMESTAMP_LEN) {
#ifdef FULL_VALIDATION
        if (tm->tm_year > 110 && tm->tm_year < 150
            && tm->tm_mon > -1 && tm->tm_mon < 12
            && tm->tm_mday > 0 && tm->tm_mday < 32
            && tm->tm_hour > -1 && tm->tm_hour < 24
            && tm->tm_min > -1 && tm->tm_min < 60
            && tm->tm_sec > -1 && tm->tm_sec < 60) {
#endif
#ifdef UNROLLED_LOOP
            const char *idx = yr_str[tm->tm_year - 111];
            *buf++ = *idx++;
            *buf++ = *idx++;
            *buf++ = *idx++;
            *buf++ = *idx++;
            idx = digit_str[tm->tm_mon + 1];
            *buf++ = *idx++;
            *buf++ = *idx++;
            idx = digit_str[tm->tm_mday];
            *buf++ = *idx++;
            *buf++ = *idx++;
            *buf++ = '-';
            idx = digit_str[tm->tm_hour];
            *buf++ = *idx++;
            *buf++ = *idx++;
            *buf++ = ':';
            idx = digit_str[tm->tm_min];
            *buf++ = *idx++;
            *buf++ = *idx++;
            *buf++ = ':';
            idx = digit_str[tm->tm_sec];
            *buf++ = *idx++;
            *buf++ = *idx++;
            retval = 0;
#else
            memcpy(buf, yr_str[tm->tm_year - 111], 4);
            buf += 4;
            memcpy(buf, digit_str[tm->tm_mon + 1], 2);
            buf += 2;
            memcpy(buf, digit_str[tm->tm_mday], 2);
            buf += 2;
            *buf++ = '-';
            memcpy(buf, digit_str[tm->tm_hour], 2);
            buf += 2;
            *buf++ = ':';
            memcpy(buf, digit_str[tm->tm_min], 2);
            buf += 2;
            *buf++ = ':';
            memcpy(buf, digit_str[tm->tm_sec], 2);
            retval = 0;
#endif
#ifdef FULL_VALIDATION
        } else {
            printf("Failed validation: %d %d %d %d %d %d \n:", tm->tm_year,
                   tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min,
                   tm->tm_sec);
        }
#endif
    }
    return retval;
}

int tm_to_str_millis(const struct tm *tm, unsigned int millis,
                     char *buf, size_t len)
{
    int retval = -1;
    if (len >= TIMESTAMP_LEN_WITH_MILLIS
        && millis < 1000 && tm_to_str(tm, buf, len) == 0) {
        buf += TIMESTAMP_LEN;
        *buf++ = '.';
        *buf++ = (millis / 100) + '0';
        millis %= 100;
        *buf++ = (millis / 10) + '0';
        *buf++ = (millis % 10) + '0';
        retval = 0;
    }
    return retval;
}

int tm_to_str_micros(const struct tm *tm, unsigned int micros, char *buf,
                     size_t len)
{
    if (len < TIMESTAMP_LEN_WITH_MICROS) {
        return -1;
    }
    if (micros > 999999) {
        return -1;
    }
    if (tm_to_str(tm, buf, len) != 0) {
        return -1;
    }
    buf += TIMESTAMP_LEN;
    *buf++ = '.';
    unsigned int divisor, i;
    for (i = 0, divisor = 100000; i < 6; ++i) {
        if (divisor > 0) {
            *buf++ = (micros / divisor) + '0';
            micros %= divisor;
            divisor /= 10;
        } else {
            *buf++ = '0';
        }
    }
    return 0;
}
