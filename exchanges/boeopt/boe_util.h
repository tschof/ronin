#ifndef __BOE_UTIL_H__
#define __BOE_UTIL_H__

#include <stdlib.h>
#include "server_objects/tm_to_str.h"
struct databuf;

/* The boeprice output field must be exactly 10 characters and it is
 * assumed that those characters have been initialized to the ASCII
 * character '0' (zero). */
int boe_put_price(const char *pricein, size_t pricein_length,
                  char *boeprice);

int boe_get_price(const char *pricein, struct databuf *priceout);

/* BOE numeric fields can be of any size and are assumed to be
 * pre-filled with '0' characters. */
int boe_set_numeric(const char *in, size_t in_length, char *boe_field,
                    size_t boe_field_length);


/* BOE timestamps are represented as a series of ASCII characters
 * representing a number defined to be the number of milliseconds
 * since the current day's midnight, UTC. This function takes that
 * text as input and writes the full DART CSV timestamp to the buffer
 * provided, returning -1 in the case of insufficient space or a
 * problem in the conversion. To ensure enough space for writing the
 * DART CSV timestamp, make sure there are TIMESTAMP_LEN_WITH_MILLIS
 * bytes available in the output buffer. */
struct boe_timestamp_converter;
struct boe_timestamp_converter *create_boe_timestamp_converter(void);
int boe_millisFromMidnightToDartTimestamp(struct boe_timestamp_converter
                                          *, const char *, size_t, char *,
                                          size_t);
void destroy_boe_timestamp_converter(struct boe_timestamp_converter *);

#endif
