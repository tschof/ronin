#include "timeval.h"

void normalize_timeval(struct timeval *tv)
{
    if (tv->tv_usec > 999999) {
        tv->tv_sec += tv->tv_usec / 1000000;
        tv->tv_usec = tv->tv_usec % 1000000;
    }
}

int cmptimeval(struct timeval left, struct timeval right)
{
    if (left.tv_sec < right.tv_sec
        || (left.tv_sec == right.tv_sec && left.tv_usec < right.tv_usec)) {
        return -1;
    }
    if (left.tv_sec == right.tv_sec && left.tv_usec == right.tv_usec) {
        return 0;
    }
    return 1;
}
