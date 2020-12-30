/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <errno.h>
#include "parser_func.h"

void copy_iovecs_to_buff(struct iovec *miov, int iovcnt,
                         databuf_t * buff, int bytes_sent)
{
    struct iovec* iov = miov;
    do {
        if (iov->iov_len <= bytes_sent) {
            bytes_sent -= iov->iov_len;
            ++iov;
        } else {
            break;
        }
    } while (bytes_sent > 0);
    //If bytes sent is > 0 then it is the remainder from a partial send
    //within an iovec.  
    while (iov && iovcnt > 0) {
        databuf_memcpy(buff, iov->iov_base + bytes_sent, iov->iov_len - bytes_sent);
        bytes_sent = 0;
        ++iov;
        --iovcnt;
    }
}
int ci_send_bunches(struct iovec *miov, int iovcnt, int sock, int *err,
                    int *vec_sent)
{
    struct iovec *iov = miov;
    int in_iovec = 0;
    ssize_t ret_code = 0;
    size_t bytes_written = 0;
    while (iovcnt > 0 && (ret_code = writev(sock, iov, iovcnt)) >= 0) {
        bytes_written += (size_t) ret_code;
        while (iov && iovcnt > 0) {
            if (iov->iov_len <= ret_code) {
                ret_code -= iov->iov_len;
                if(in_iovec) {
                    iov->iov_base -= in_iovec;
                    iov->iov_len += in_iovec;
                    in_iovec = 0;
                }
                ++iov;
                --iovcnt;
            } else {
                iov->iov_base += ret_code;
                iov->iov_len -= ret_code;
                in_iovec += ret_code;
                break;
            }
        }
    }
    if(in_iovec) {
        iov->iov_base -= in_iovec;
        iov->iov_len += in_iovec;
    }
    if (ret_code < 0) {
        int error = errno;
        if (error == EAGAIN || error == EWOULDBLOCK || error == ENOBUFS
            || error == EINTR || error == 0) {
            *err = 0;
        } else {
            *err = -1;
        }
    } else {
        *err = 1;
    }
    *vec_sent = iovcnt;
    return bytes_written;
}
long find_offset(const char *haystack, unsigned long hlen,
                 const char *needle, unsigned long nlen)
{
    char *cp = (char *) haystack;
    char *s1;
    char *s2;
    unsigned long curr_off;
    unsigned long l_off;
    if (nlen > hlen) {
        return -1;
    }
    curr_off = 0;
    while (curr_off < hlen) {
        s1 = cp;
        s2 = (char *) needle;
        l_off = curr_off;
        while (l_off < hlen && *s1 && *s2 && !(*s1 - *s2)) {
            ++l_off, ++s1, ++s2;
        }
        if (!*s2) {
            return curr_off;
        }
        ++cp;
        ++curr_off;
    }
    return -1;

}

long make_long(const char* num, int size)
{
    const char* end;
    long ret = 0;
    end = num + size;
    while(num < end) {
        ret = ret * 10 + *num - '0';
        ++num;
    }
    return ret;
}

/**
 * Times will be assumed to be in UTC and in the format:
 * YYYYMMDD-HH:MM:SS
 */
long convert_to_nanosec_since_1970(char* yyyymmddhhmmss)
{
    struct tm t, local;
    memset(&t, 0, sizeof(struct tm));
    t.tm_year = make_long(yyyymmddhhmmss, 4) - 1900;
    t.tm_mon = make_long(yyyymmddhhmmss + 4, 2) -1;
    t.tm_mday = make_long(yyyymmddhhmmss + 6, 2);
    t.tm_hour = make_long(yyyymmddhhmmss + 9, 2);
    t.tm_min = make_long(yyyymmddhhmmss + 12, 2);
    t.tm_sec = make_long(yyyymmddhhmmss +15, 2);
    t.tm_isdst = -1;
    long ret =  mktime(&t);
    localtime_r(&ret, &local);
    /*mktime returns local time we need UTC*/
    ret += local.tm_gmtoff;
    return (ret * 1000000000);
}
time_t convert_to_sec_since_1970(char* yyyymmddhhmmss)
{
    struct tm t;
    memset(&t, 0, sizeof(struct tm));
    t.tm_year = make_long(yyyymmddhhmmss, 4) - 1900;
    t.tm_mon = make_long(yyyymmddhhmmss + 4, 2) -1;
    t.tm_mday = make_long(yyyymmddhhmmss + 6, 2);
    t.tm_hour = make_long(yyyymmddhhmmss + 8, 2);
    t.tm_min = make_long(yyyymmddhhmmss + 10, 2);
    t.tm_sec = make_long(yyyymmddhhmmss +12, 2);
    t.tm_isdst = -1;
    return mktime(&t);
}

