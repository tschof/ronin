#include "databuf.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

const unsigned int DATABUF_FLAGS_NOREALLOC = 0x1;

void databuf_setnorealloc(struct databuf *databuf)
{
    databuf->flags |= DATABUF_FLAGS_NOREALLOC;
}

void databuf_setrealloc(struct databuf *databuf)
{
    databuf->flags &= ~DATABUF_FLAGS_NOREALLOC;
}

databuf_t *databuf_alloc(size_t suggestedsize)
{
    databuf_t *pbuf = malloc(sizeof(databuf_t));
    if (pbuf) {
        memset(pbuf, 0, sizeof(databuf_t));
        pbuf->buffer = malloc(suggestedsize);
        if (pbuf->buffer) {
            memset(pbuf->buffer, 0, suggestedsize);
            pbuf->buflen = suggestedsize;
            pbuf->rd_ptr = pbuf->wr_ptr = pbuf->buffer;
        } else {
            free(pbuf);
            pbuf = 0;
        }
    }
    return pbuf;
}

int databuf_attach(databuf_t * databuf, char *buf, size_t len)
{
    int retval = -1;
    if (databuf != 0 && buf != 0 && len > 0) {
        databuf->rd_ptr = buf;
        databuf->wr_ptr = buf;
        databuf->buffer = buf;
        databuf->buflen = len;
        databuf->flags = DATABUF_FLAGS_NOREALLOC;
        retval = 0;
    }
    return retval;
}

int databuf_sanity_check(databuf_t * databuf)
{
    int result = -1;
    if (databuf != 0
        && databuf->buffer != 0
        && databuf->wr_ptr >= databuf->rd_ptr
        && databuf->rd_ptr >= databuf->buffer) {
        result = 0;
    }
    assert(result == 0);
    return result;
}

int databuf_reset(databuf_t * databuf)
{
    int result = databuf_sanity_check(databuf);
    if (result == 0) {
        databuf->rd_ptr = databuf->wr_ptr = databuf->buffer;
    }
    return result;
}

void databuf_destroy(databuf_t * databuf)
{
    if (databuf != 0 && ((databuf->flags & DATABUF_FLAGS_NOREALLOC) == 0)) {
        free(databuf->buffer);
        free(databuf);
    }
}

size_t databuf_space_left(databuf_t * databuf)
{
    return ((databuf->buffer + databuf->buflen) - databuf->wr_ptr);
}

size_t databuf_unread(databuf_t * databuf)
{
    return (databuf->wr_ptr - databuf->rd_ptr);
}

int databuf_realloc(databuf_t * databuf, size_t growby)
{
    int result = -1;
    if (databuf != 0 && ((databuf->flags & DATABUF_FLAGS_NOREALLOC) == 0)) {
        char *tmpbuf = databuf->buffer;
        databuf->buffer =
            realloc(databuf->buffer, growby + databuf->buflen);
        if (databuf->buffer != 0) {
            if (databuf->buffer != tmpbuf) {
                databuf->rd_ptr =
                    (databuf->rd_ptr - tmpbuf) + databuf->buffer;
                databuf->wr_ptr =
                    (databuf->wr_ptr - tmpbuf) + databuf->buffer;
            }
            databuf->buflen += growby;
            result = 0;
        }
    }
    return result;
}

size_t calc_realloc_buffersize(databuf_t * databuf, size_t nminbytes)
{
    const size_t nspace = databuf_space_left(databuf);
    const size_t nspace_if_doubled = nspace + databuf->buflen;
    return ((nspace_if_doubled <
             nminbytes) ? nminbytes : nspace_if_doubled);
}

int databuf_write(databuf_t * databuf, const char *formatstr, ...)
{
    va_list argp;
    int result = databuf_sanity_check(databuf);
    if (result == 0) {
        size_t nspace = databuf_space_left(databuf);
        va_start(argp, formatstr);
        result = vsnprintf(databuf->wr_ptr, nspace, formatstr, argp);
        va_end(argp);

        /* TODO: If result == -1, will it be converted to a really big size_t
           value here?  Check and refactor if necessary */
        if ((result >= nspace)
            &&
            ((result =
              databuf_realloc(databuf,
                              calc_realloc_buffersize(databuf,
                                                      result + 1))) ==
             0)) {
            /* Need to reallocate */
            nspace = databuf_space_left(databuf);
            va_start(argp, formatstr);
            result = vsnprintf(databuf->wr_ptr, nspace, formatstr, argp);
            va_end(argp);
        }

        if (result > -1 && result < nspace) {
            databuf->wr_ptr += result;
        }
    }

    return result;
}

int databuf_memcpy(databuf_t * databuf, const void *source, size_t nbytes)
{
    int result = databuf_sanity_check(databuf);
    if (result == 0 && source && nbytes) {
        size_t nspace = databuf_space_left(databuf);
        if (nbytes >= nspace) {
            size_t newsize = calc_realloc_buffersize(databuf, nbytes + 1);
            result = databuf_realloc(databuf, newsize);
            nspace = databuf_space_left(databuf);
        }
        if (nbytes <= nspace) {
            memcpy(databuf->wr_ptr, source, nbytes);
            *(databuf->wr_ptr + nbytes) = '\0';
            /* Leave the write pointer writing at the null terminator,
               the terminator should not show up in the output except
               after all writing is done */
            databuf->wr_ptr += nbytes;
            result = 0;
        }
    }
    return result;
}

int databuf_reserve(databuf_t * databuf, size_t n)
{
    int retval;
    size_t nleft = databuf_space_left(databuf);
    if (n < nleft) {
        retval = 0;
    } else {
        retval = databuf_realloc(databuf, (n - nleft) + 1);
    }
    return retval;
}
