#include "DataTypes.h"
#include "server_objects/tm_to_str.h"
#include "server_objects/databuf.h"
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>

/* Just does a memcpy and assumes there is enough room, advancing the
 * pointer.  This is correct for the BOE message types since before
 * calling this function the size of the buffer is already checked and
 * accounted for. */

int setBinaryNumFromStr(void *pdst, const char *psrc, size_t nsrc,
                        size_t nbytes)
{
    char *endptr;
    errno = 0;
    long int i = strtol(psrc, &endptr, 10);
    if (errno != 0 || endptr == psrc || (endptr - psrc) != nsrc)
        return -1;
    switch (nbytes) {
    case 1:
        {
            if (i > CHAR_MAX || i < CHAR_MIN)
                return -1;
            int8_t tmp = i;
            memcpy(pdst, &tmp, sizeof tmp);
            break;
        }
    case 2:
        {
            if (i > SHRT_MAX || i < SHRT_MIN)
                return -1;
            int16_t tmp = i;
            memcpy(pdst, &tmp, sizeof tmp);
            break;
        }
    case 4:
        if (i > INT_MAX || i < INT_MIN)
            return -1;
        int32_t tmp = i;
        memcpy(pdst, &tmp, sizeof tmp);
        break;
    case 8:
        memcpy(pdst, &i, sizeof i);
        break;
    default:
        return -1;
    }
    return 0;
}

void setAlphaNumeric(void *pdst, const char *psrc, size_t nsrc,
                     size_t nbytes)
{
    if (nsrc < nbytes)
        memset(pdst, 0, nbytes);
    memcpy(pdst, psrc, nsrc);
}

int setSignedBinaryPrice(void *pdst, const char *psrc, size_t nsrc)
{
    char *endptr;
    errno = 0;
    double d = strtod(psrc, &endptr);
    if (errno != 0)
        return -1;
    if (endptr < (psrc + nsrc))
        return -1;
    if (d < 0.0)
        d -= 0.00000001;
    else
        d += 0.00000001;
    double integral;
    double fractional = modf(d, &integral);
    int64_t price = (int64_t) integral;
    price *= 10000;
    fractional *= 10000;
    price += (int64_t) fractional;
    memcpy(pdst, &price, sizeof price);
    return 0;
}

int binaryDateTimeToStr(uint64_t dateTime, char *dstp, size_t dstn)
{
    struct timeval tv = {
        .tv_sec = dateTime / 1000000000,
        .tv_usec = (dateTime % 1000000000) / 1000,
    };
    struct tm localtm;
    gmtime_r(&tv.tv_sec, &localtm);
    return tm_to_str_micros(&localtm, tv.tv_usec, dstp, dstn);
}

int binaryPriceToString(price_t price, struct databuf *output)
{
    return databuf_write(output, "%ld.%04ld", (long) price / 10000,
                         (long) price % 10000) != -1 ? 0 : -1;
}
