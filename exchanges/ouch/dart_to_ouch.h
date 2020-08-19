#ifndef __DART_TO_OUCH__H__
#define __DART_TO_OUCH__H__

#include <stdlib.h>
#include <stdint.h>

int dart_to_ouch__convert_side(const char *, size_t, char *);
int dart_to_ouch__convert_shares(const char *shares, int shares_length,
                                 uint32_t * binshares,
                                 const char **errmsg);
int dart_to_ouch__convert_tif(char dart_tif, uint32_t * ouch_tif);

#endif
