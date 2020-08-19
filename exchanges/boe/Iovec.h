#ifndef __IOVEC_H__
#define __IOVEC_H__

#include <stdlib.h>

struct iovec;

void iovec_writeall(int fd, struct iovec *p, size_t n);

#endif
