#include "fixmsg.h"
#include <errno.h>
#include <time.h>
#include <string.h>
#include <sys/timeb.h>
#include "tm_to_str.h"

int fixmsg_addstr2(databuf_t * databuf, const char *tag, size_t tlen,
                   const char *val, size_t vlen)
{
    int retval = -1;
    if (databuf != 0 && tag != 0 && tlen > 0 && val != 0 && vlen > 0) {
        const size_t nrequired = tlen + vlen + 2;
        if (nrequired > databuf_space_left(databuf)
            && databuf_realloc(databuf, nrequired) < 0) {
            goto on_error;
        }
        *databuf->wr_ptr++ = '\x01';
        memcpy(databuf->wr_ptr, tag, tlen);
        databuf->wr_ptr += tlen;
        *databuf->wr_ptr++ = '=';
        memcpy(databuf->wr_ptr, val, vlen);
        databuf->wr_ptr += vlen;
        retval = nrequired;
    } else if (vlen == 0 || val == 0 || val[0] == '\0') {
        retval = 0;
    } else {
        errno = EINVAL;
    }
  on_error:
    return retval;
}

int fixmsg_addstr(databuf_t * databuf, const char *tag, const char *val)
{
    int rv = -1;
    if (val != 0) {
        int valuelen = strlen(val);
        rv = fixmsg_addstr_bylen(databuf, tag, val, valuelen);
    } else {
        errno = EINVAL;
    }
    return rv;
}

int fixmsg_addstr_bylen(databuf_t * databuf, const char *tag,
                        const char *val, size_t len)
{
    int rv = -1;
    if (databuf != 0 && tag != 0 && tag[0] != '\0' && val != 0
        && val[0] != '\0' && len > 0) {
        int taglen = strlen(tag);
        int nspace = databuf_space_left(databuf);
        const char *placeholder;
        if ((taglen + len + 2) > nspace
            && databuf_realloc(databuf, taglen + len + 2) < 0) {
            goto on_error;
        }
        placeholder = databuf->wr_ptr;
        *(databuf->wr_ptr) = '\x01';
        ++(databuf->wr_ptr);
        memcpy(databuf->wr_ptr, tag, taglen);
        databuf->wr_ptr += taglen;
        *(databuf->wr_ptr) = '=';
        ++(databuf->wr_ptr);
        memcpy(databuf->wr_ptr, val, len);
        databuf->wr_ptr += len;
        rv = databuf->wr_ptr - placeholder;
    } else if (len == 0 || val == 0 || val[0] == '\0') {
        rv = 0;
    } else {
        errno = EINVAL;
    }
  on_error:
    return rv;
}

int fixmsg_addtime(databuf_t * databuf, const char *tag,
                   const struct tm *timeval)
{
    int rv = -1;
    if (databuf != 0 && tag != 0 && tag[0] != '\0' && timeval != 0) {
        struct timeb rtime;
        ftime(&rtime);
        if (databuf_memcpy(databuf, "\x01", 1) > -1
            && databuf_memcpy(databuf, tag, strlen(tag)) > -1
            && databuf_memcpy(databuf, "=", 1) > -1
            && tm_to_str_millis(timeval, rtime.millitm, databuf->wr_ptr,
                         databuf_space_left(databuf)) == 0) {
            /* Handle reallocation if necessary. */
            databuf->wr_ptr += 21;//17;
            rv = 0;
        }
    } else {
        errno = EINVAL;
    }
    return rv;
}

int fixmsg_adduint(databuf_t * databuf, const char *tag,
                   size_t tlen, unsigned int i)
{
    int retval = -1;
    if (databuf != 0 && tag != 0 && tlen > 0) {
        int sohlen, taglen, delimlen, ilen;
        if ((sohlen = databuf_memcpy(databuf, "\x01", 1)) > -1
            && (taglen = databuf_memcpy(databuf, tag, tlen)) > -1
            && (delimlen = databuf_memcpy(databuf, "=", 1)) > -1
            && (ilen = databuf_write(databuf, "%d", i)) > -1) {
            retval = sohlen + taglen + delimlen + ilen;
        }
    }
    return retval;
}
