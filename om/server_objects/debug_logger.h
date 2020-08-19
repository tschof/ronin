#ifndef __DEBUG_LOGGER_H__
#define __DEBUG_LOGGER_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include <stddef.h>
#include <stdlib.h>

int send_debug_message(const char *formatstr, ...);
int write_debug_message(const void *source, size_t nbytes);

#if defined(__cplusplus)
}
#endif
#endif
