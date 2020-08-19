#include "dart_to_ouch.h"
#include "ouch41.h"
#include <errno.h>
#include <string.h>

int dart_to_ouch__convert_side(const char *dart_side,
                               size_t dart_side_length, char *ouch_side)
{
    int retval = -1;
    if (dart_side_length == 1) {
        switch (*dart_side) {
        case '1':
            *ouch_side = 'B';
            retval = 0;
            break;
        case '2':
            *ouch_side = 'S';
            retval = 0;
            break;
        case '5':
            *ouch_side = 'T';
            retval = 0;
            break;
        case '6':
            *ouch_side = 'E';
            retval = 0;
            break;
        }
    }
    return retval;
}

int dart_to_ouch__convert_shares(const char *shares, int shares_length,
                                 uint32_t * binshares, const char **errmsg)
{
    int retval = -1;
    char *endptr;
    errno = 0;
    long int tmplong = strtol(shares, &endptr, 10);
    if (tmplong > 0 && errno == 0 && endptr != shares) {
        if (tmplong < 1000000) {
            *binshares = tmplong;
            retval = 0;
        } else {
            *errmsg =
                strdup("invalid shares: must be less than 1,000,000");
        }
    } else {
        *errmsg = strdup("invalid shares");
    }
    return retval;
}

int dart_to_ouch__convert_tif(char dart_tif, uint32_t * ouch_tif)
{
    int retval = -1;
    if (dart_tif == '0') {
        *ouch_tif = OUCH_TIF_DAY;
        retval = 0;
    } else if (dart_tif == '3') {
        *ouch_tif = OUCH_TIF_IOC;
        retval = 0;
    }
    return retval;
}
