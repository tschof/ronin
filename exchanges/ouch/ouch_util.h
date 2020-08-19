#ifndef __OUCH_UTIL_H__
#define __OUCH_UTIL_H__

#include "server_objects/databuf.h"
#include <inttypes.h>

/* Takes a string, either null-terminated or other delimiter
 * terminated. We do not care what the delimiter is as long as it is
 * there. A null for a null-terminated string or a comma for a field
 * in a ROM CSV message, it's all good, all because we're using strtol
 * which goes until it finds a non-digit character.  */
int ouch_price_string_to_int(const char *pricestr, unsigned int *priceint);

/* Takes an OUCH price integer, in host endianness, and write the ROM
 * CSV string representation to the databuf. */
int ouch_price_int_to_string(unsigned int priceint,
                             struct databuf *pricestr);

int tif_ouch_to_rom(uint32_t tifint, struct databuf *tifstr);

#endif
