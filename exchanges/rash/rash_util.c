#include "rash_util.h"

#include <string.h>
#include <server_objects/databuf.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>

/* Rash prices are 10 digit strings of which the first 6 characters
   are the whole number and the last 4 are the decimal part. */
#define RASHPRICE_WHOLENUM_DIGITS 6
#define RASHPRICE_DECIMAL_PLACES 4

int rash_put_price(const char *pricein, size_t pricein_length,
                   char *rashprice)
{
    int retval = -1;
    const char *end = pricein + pricein_length;
    const char *decimalpt = memchr(pricein, '.', pricein_length);
    if (decimalpt == 0) {
        decimalpt = end;
    }
    for (; *pricein == '0' && pricein != decimalpt; ++pricein);
    /* pricein now points to 1) the first significant digit of the whole
       number, 2) the decimal point if there are no significant digits
       in the whole number, or 3) the end of the string. */
    size_t nwhole = decimalpt - pricein;
    if (nwhole < 7) {
        rashprice += RASHPRICE_WHOLENUM_DIGITS - nwhole;
        if (nwhole > 0) {
            memcpy(rashprice, pricein, nwhole);
            rashprice += nwhole;
        }
        pricein = decimalpt;
        if (pricein != end && *pricein == '.') {
            ++pricein;
        }
        if (pricein != end) {
            /* Strip the trailing zeros */
            for (; end != pricein && *(end - 1) == '0'; --end);
            size_t ndecimal = end - pricein;
            if (ndecimal < 5) {
                memcpy(rashprice, pricein, ndecimal);
                retval = 0;
            }
        } else {
            retval = 0;
        }
    }
    return retval;
}

int rash_get_price(const char *pricein, struct databuf *priceout)
{
    int retval = -1;
    const char *decimalpart = pricein + RASHPRICE_WHOLENUM_DIGITS;
    for (; *pricein == '0' && pricein < decimalpart; ++pricein);
    if (pricein != decimalpart
        && databuf_memcpy(priceout, pricein, decimalpart - pricein) != -1
        && databuf_memcpy(priceout, ".", 1) != -1) {
        const char *end = decimalpart + RASHPRICE_DECIMAL_PLACES;
        /* Default to 2 decimal places unless more are needed. */
        if (decimalpart[3] == '0') {
            --end;
            if (decimalpart[2] == '0') {
                --end;
            }
        }
        if (databuf_memcpy(priceout, decimalpart, end - decimalpart) != -1) {
            retval = 0;
        }
    }
    return retval;
}

/* The chief assumption of the following numeric field functions is
   that the rash numeric field has already been initialized to all '0'
   (zero) characters before calling this function.  This is currently
   a safe assumption as we initialize entire structures before
   processing them with these functions. */
int rash_set_numeric(const char *in, size_t in_length, char *rash_field,
                     size_t rash_field_length)
{
    int retval = -1;
    const char *in_end = in + in_length;

    /* Remove insignificant digits */
    for (; *in == '0' && in != in_end; ++in);
    in_length = in_end - in;

    /* We seem to have a problem in that RASH will simply drop our
     * connection if there is anything other than ASCII digits in a
     * numeric field. Therefore we will have to validate *every*
     * character. */
    const char *pvalidate;
    for (pvalidate = in; pvalidate != in_end; ++pvalidate) {
        if (isdigit(*pvalidate) == 0) {
            return -1;
        }
    }

    if (in_length <= rash_field_length) {
        if (in_length > 0) {
            rash_field = (rash_field + rash_field_length) - in_length;
            memcpy(rash_field, in, in_length);
        }
        /* If in_length is equal to 0, the input value must be zero;
         * this is a valid placement which we signal by returning 0. */
        retval = 0;
    }
    return retval;
}

int rash_get_numeric(const char *rash_field, size_t rash_field_length,
                     struct databuf *out, int zero_as_empty_string)
{
    int retval = -1;
    const char *end = rash_field + rash_field_length;
    while (*rash_field == '0' && rash_field != end) {
        ++rash_field;
    }
    if (rash_field != end) {
        if (databuf_memcpy(out, rash_field, end - rash_field) != -1) {
            retval = 0;
        }
    } else if (zero_as_empty_string == 0) {
        if (databuf_memcpy(out, "0", 1) != -1) {
            retval = 0;
        }
    }
    return retval;
}

struct rash_timestamp_converter {
    struct tm currentDate;
    int isEasternTimeZone;
};

int rash_millisFromMidnightToDartTimestamp(struct rash_timestamp_converter
                                           *tsconverter,
                                           const char *pmillis,
                                           size_t nmillis,
                                           char *ptimestamp,
                                           size_t ntimestamp)
{
    int result = -1;

    struct tm nytime = tsconverter->currentDate;

    /* Can't call strtol on the time stamp as it sits in the RASH
     * message, must copy it out and null-terminate it first. */
    char tmp[nmillis + 1];
    memcpy(tmp, pmillis, nmillis);
    tmp[nmillis] = '\0';

    char *endptr;
    long int millis = strtol(tmp, &endptr, 10);
    if (tsconverter->isEasternTimeZone == 0)
        millis -= 3600000;      /* Remove 1 hour */
    long int secs = millis / 1000;
    millis %= 1000;

    nytime.tm_hour = secs / 3600;       /* Hours */
    if (nytime.tm_hour > 23)
        goto done;
    secs %= 3600;

    nytime.tm_min = secs / 60;
    if (nytime.tm_min > 60)
        goto done;
    secs %= 60;
    if (secs > 60)
        goto done;

    nytime.tm_sec = secs;
    time_t t = mktime(&nytime);
    struct tm utcTime;
    if (gmtime_r(&t, &utcTime) != 0) {
        result =
            tm_to_str_millis(&utcTime, millis, ptimestamp, ntimestamp);
    }
  done:
    return result;
}

struct rash_timestamp_converter *create_rash_timestamp_converter(void)
{
    struct rash_timestamp_converter *retval = malloc(sizeof *retval);
    tzset();
    if (timezone == 18000)
        retval->isEasternTimeZone = 1;
    else if (timezone == 21600)
        retval->isEasternTimeZone = 0;
    else {
        printf("Unrecognized timezone %ld, not adjusting timestamps\n",
               timezone);
        retval->isEasternTimeZone = 1;
    }
    time_t now = time(NULL);
    int tmperr;
    if (now != -1 && localtime_r(&now, &(retval->currentDate)) != 0)
        goto done;
    tmperr = errno;
    free(retval);
    retval = 0;
    errno = tmperr;
  done:
    return retval;
}

void destroy_rash_timestamp_converter(struct rash_timestamp_converter
                                      *tsconverter)
{
    free(tsconverter);
}
