#include "ouch_util.h"

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

struct databuf;
#define jump_assert(cond, label)                \
    if (!(cond)) { goto label; }

int ouch_price_string_to_int(const char *pricestr, uint32_t * priceint)
{
    int result = -1;
    char *endptr;
    *priceint = strtol(pricestr, &endptr, 10);
    jump_assert(*priceint <= 199999, done);
    *priceint *= 10000;
    if (*endptr == '.') {
        ++endptr;
        size_t i;
        int multiplier = 1000;
        for (i = 0; i < 4; ++i) {
            if (isdigit(endptr[i]))
                *priceint += (endptr[i] - '0') * multiplier;
            else
                break;
            multiplier /= 10;
        }
    }
    result = 0;
  done:
    return result;
}

#if 0
int ouch_price_string_to_int(const char *pricestr, uint32_t * priceint)
{
    int result = -1;
    char *endptr;
    *priceint = strtol(pricestr, &endptr, 10);
    jump_assert(*priceint <= 199999, done);
    *priceint *= 10000;
    if (*endptr == '.') {
        long int decimalpart = strtol(endptr + 1, &endptr, 10);
        jump_assert(decimalpart < 10000, done);
        if (decimalpart > 0) {
            while (decimalpart < 1000) {
                decimalpart *= 10;
            }
            *priceint += decimalpart;
        }
    }
    result = 0;
  done:
    return result;
}
#endif

#if 0
int ouch_price_int_to_string(unsigned int priceint,
                             struct databuf *pricestr)
{
    int result = -1;
    unsigned int wholenum = priceint / 10000;
    if ((result = databuf_write(pricestr, "%u", wholenum)) != -1) {
        unsigned int decimalnum = priceint % 10000;
        /* decimalnum now represents the number of 10000ths of a
           point.  Alternately, pad represents the number of leading
           zeros to apply to the number when printing. */
        int pad = 0;
        if (decimalnum < 1000) {
            pad = 1;
            if (decimalnum < 100) {
                pad = 2;
                if (decimalnum < 10) {
                    pad = 3;
                }
            }
        }

        databuf_memcpy(pricestr, ".", 1);
        int i;
        for (i = 0; i < pad; ++i) {
            databuf_memcpy(pricestr, "0", 1);
        }
        if (decimalnum % 100 == 0) {
            /* This is only here so that we keep to 2 decimal places
               when we can. */
            decimalnum /= 100;
        }
        result = databuf_write(pricestr, "%u", decimalnum);
    }
    return result;
}
#endif

int ouch_price_int_to_string(unsigned int priceint,
                             struct databuf *pricestr)
{
    double d = priceint;
    d *= 0.0001;
    return databuf_write(pricestr, "%.4f", d);
}

#define OUCH_TIF_IOC 0
#define OUCH_TIF_MARKET_HOURS 99998
#define OUCH_TIF_SYSTEM_HOURS 99999

int tif_rom_to_ouch(const char *tifstr, size_t tifstrlen,
                    uint32_t * tifint)
{
    int result = -1;
    if (tifstrlen == 1) {
        if (tifstr[0] == '3') {
            *tifint = 0;
        } else if (tifstr[0] == '0') {
            *tifint = OUCH_TIF_MARKET_HOURS;
        } else if (tifstr[0] == '5') {
            *tifint = OUCH_TIF_SYSTEM_HOURS;
        }
    }
    return result;
}

int tif_ouch_to_rom(uint32_t tifint, struct databuf *tifstr)
{
    int result = -1;
    if (tifint == 0) {
        result = databuf_memcpy(tifstr, "3", 1);        /* IOC */
    } else if (tifint == 99998) {
        result = databuf_memcpy(tifstr, "0", 1);        /* DAY */
    } else if (tifint == 99999) {
        result = databuf_memcpy(tifstr, "5", 1);        /* GTX */
    } else {
        result = databuf_memcpy(tifstr, "6", 1);        /* GTD */
    }
    return result;
}
