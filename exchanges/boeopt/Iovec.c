#include "Iovec.h"
#include <sys/uio.h>
#include <stdio.h>

void iovec_advance(size_t nbytes, struct iovec **p, size_t * n,
                   struct iovec *end)
{
    while (*p < end && nbytes > 0) {
        if ((*p)->iov_len >= nbytes) {
            (*p)->iov_base += nbytes;
            (*p)->iov_len -= nbytes;
            break;
        }
        nbytes -= (*p)->iov_len, ++(*p);
    }
}

void iovec_writeall(int fd, struct iovec *p, size_t n)
{
    struct iovec *end = p + n;
    while (p != end) {
        int nwrite = writev(fd, p, end - p);
        if (nwrite == -1) {
            perror("iovec_writeall");
            return;
        }
        if (nwrite == 0) {
            while (p != end) {
                if (p->iov_len > 0) {
                    fprintf(stderr, "0 write, but data still here\n");
                    return;
                }
                ++p;
            }
        }
        iovec_advance((size_t) nwrite, &p, &n, end);
        while (p != end && p->iov_len == 0) {
            ++p;
        }
    }
    if (p != end) {
        fprintf(stderr, "p != end\n");
    }
}
