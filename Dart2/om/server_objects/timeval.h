#ifndef __TIMEVAL_H__
#define __TIMEVAL_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include <sys/time.h>

void normalize_timeval(struct timeval *);
int cmptimeval(struct timeval, struct timeval);

#if defined(__cplusplus)
}
#endif
#endif
