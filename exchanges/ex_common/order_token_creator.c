/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include "order_token_creator.h"

#define MICRO_HOUR 3600000000
#define MICRO_MIN 60000000
#define MICRO_SEC 1000000

struct token_struct {
    uint64_t offset;
    uint64_t counter;
    char datestr[9];
};

token_struct_t *init_order_token_creator(void)
{
    token_struct_t *retval = 0;
    struct timeval tv;
    if (gettimeofday(&tv, 0) == 0) {
        struct tm tmparts;
        if (localtime_r(&tv.tv_sec, &tmparts) != 0) {
            retval = malloc(sizeof *retval);
            retval->offset =
                (tmparts.tm_hour * MICRO_HOUR) +
                (tmparts.tm_min * MICRO_MIN) +
                (tmparts.tm_sec * MICRO_SEC) + tv.tv_usec;
            retval->counter = 1;
            if (snprintf
                (retval->datestr, sizeof retval->datestr, "%04d%02d%02d",
                 tmparts.tm_year + 1900, tmparts.tm_mon + 1,
                 tmparts.tm_mday) != 8) {
                free(retval);
                retval = 0;
            }
        }
    }
    return retval;
}

/*! \brief Creates an ASCII string representing the integer value
 *         passed in according to the radix passed in.
 */
int Ltoa(signed long long int value, char *str, size_t len,
         unsigned int radix)
{
    static char dig[] = "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz";

    int n = 0, neg = 0;
    signed long long int v;
    char *p, *q;
    char c;

    if (radix <= sizeof dig) {
        if (value < 0) {
            value = -value;
            neg = 1;
        }

        v = value;
        do {
            if (n > len) {
                n = -1;
                goto exitfunc;
            }
            str[n++] = dig[v % radix];
            v /= radix;
        } while (v);

        if (neg) {
            str[n++] = '-';
        }
        str[n] = '\0';

        for (p = str, q = p + (n - 1); p < q; ++p, --q) {
            c = *p, *p = *q, *q = c;
        }
    } else {
        n = -1;
    }

  exitfunc:
    return n;
}

/* The INCR_AND_FETCH macro does just what it sounds like: atomically
 * increments the var pointed to and returns the resulting incremented
 * number.  It increments and then returns instead of returning and
 * then incrementing for cross-platform consistency; gcc provides
 * compiler intrinsics for both while Windows only provides support
 * (as far as I can tell) for the implementation chosen here.
 *
 * The variable pointed to doesn't quite matter under gcc as these are
 * compiler intrinsics or builtin functions, the compiler detects the
 * size and type of integer and provides the proper implementation.
 *
 * See
 * http://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Atomic-Builtins.html
 * for more information.  Under Windows a LONG data type is expected
 * (32-bits, signed for the 32-bit Windows platform we are developing
 * for).
 */
#define INCR_AND_FETCH(var) __sync_add_and_fetch(&(var), 1)

int create_order_token(token_struct_t * t, char *buffer, int buffer_len)
{
    long incremented_counter = INCR_AND_FETCH(t->counter);
    return Ltoa(t->offset + incremented_counter, buffer, buffer_len, 36);
}

int create_multiday_unique_order_token(token_struct_t * t, char *bufptr,
                                       int buflen)
{
    int retval = -1;
    if (buflen > 8) {
        memcpy(bufptr, t->datestr, 8);
        bufptr += 8;
        buflen -= 8;
        int nwritten = create_order_token(t, bufptr, buflen);
        if (nwritten != -1) {
            retval = nwritten + 8;
        }
    }
    return retval;
}
