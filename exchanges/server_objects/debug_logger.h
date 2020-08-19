#ifndef __DEBUG_LOGGER_H__
#define __DEBUG_LOGGER_H__

#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

    int send_debug_message(const char *formatstr, ...);

    int write_debug_message(const void *source, size_t nbytes);

#ifdef __cplusplus
}
#endif
#endif                          //__DATABUF_H__
