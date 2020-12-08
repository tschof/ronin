#ifndef __TM_TO_STR_H__
#define __TM_TO_STR_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include <time.h>
#include <stdlib.h>

/* Converts a struct tm into a human-readable string. The format
 * followed is YYYYMMDD-HH:MM:SS, 17 characters. */

#define TIMESTAMP_LEN 17
#define TIMESTAMP_LEN_WITH_MILLIS 21

int tm_to_str(const struct tm *tm, char *buf, size_t len);
int tm_to_str_millis(const struct tm *tm, unsigned int millis,
                     char *buf, size_t len);

#if defined(__cplusplus)
}
#endif
#endif
