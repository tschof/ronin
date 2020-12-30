/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _PARSER_FUNC_H__
#define _PARSER_FUNC_H__
#if defined(__cplusplus)
extern "C" {
#endif
#include <sys/socket.h>
#include "databuf.h"

long make_long(const char* num, int size);
time_t convert_to_sec_since_1970(char* date);
long convert_to_nanosec_since_1970(char* date);

long find_offset(const char *haystack, unsigned long hlen,
                 const char *needle, unsigned long nlen);
char *find_offset3(const char *haystack, unsigned long len,
                   const char *needle);
char *find_offset2(const char *phaystack, const char *pneedle);

int ci_send_bunches(struct iovec *miov, int iovcnt, int sock, int *err,
                    int *vec_sent);
void copy_iovecs_to_buff(struct iovec *miov, int iovcnt,
                         databuf_t * buff, int bytes_sent);

#define GET_OFF(haystack, hlen, needle, nlen, found)                            \
    found = 0;                                                                  \
    char* s2;                                                                   \
    char* s1;                                                                   \
    do {                                                                        \
        s2 = needle;                                                            \
        s1 = haystack;                                                          \
        while(*s1 && *s2 && !(*s1 - *s2)) {                                     \
            ++s1;                                                               \
            ++s2;                                                               \
        }                                                                       \
        if(!*s2) {                                                              \
            found = 1;                                                          \
            break;                                                              \
        }                                                                       \
        ++haystack;                                                             \
        --hlen;                                                                 \
    } while(hlen > nlen);                                                       \
        
#define NEXT_ONE(off, end)                                   \
    do {                                                     \
        ++off;                                               \
        --end;                                               \
    } while(*off != FIX_FIELD_DELIM && end > 0);             \


#if defined(__cplusplus)
}
#endif
#endif
