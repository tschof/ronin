#ifndef __DATA_TYPES_H__
#define __DATA_TYPES_H__

#include <stdlib.h>
#include <stdint.h>

struct databuf;

typedef uint64_t price_t;

int setBinaryNumFromStr(void *pdst, const char *psrc, size_t nsrc,
                        size_t nbytes);
void setAlphaNumeric(void *pdst, const char *psrc, size_t nsrc,
                     size_t nbytes);

/* Number of bytes in the target is not needed for binary prices; they
 * are always 8 bytes. We will treat everything as a signed binary
 * price even tough the BOE documentation makes a distinction. */
int setSignedBinaryPrice(void *pdst, const char *psrc, size_t nsrc);

int binaryDateTimeToStr(uint64_t dateTime, char *dstp, size_t dstn);

int binaryPriceToString(price_t price, struct databuf *output);

#endif
