
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

#include "databuf.h"
#include "dart_allocator.h"

#define DATABUF_FLAGS_NOREALLOC  0x1
#define DATABUF_FLAGS_NO_FREE 0x2

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
        databuf->flags = DATABUF_FLAGS_NOREALLOC | DATABUF_FLAGS_NO_FREE;
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
        databuf = 0;
    } else if (databuf != 0) {
        free(databuf);
        databuf = 0;
    }
}

#define DB_CAN_FIT(databuf, bytes)             \
    ((databuf->buffer + databuf->buflen) - databuf->wr_ptr) >= bytes;  \

int databuf_space_left(databuf_t * databuf)
{
    return ((databuf->buffer + databuf->buflen) - databuf->wr_ptr);
}

size_t databuf_unread(databuf_t * databuf)
{
    return (databuf->wr_ptr - databuf->rd_ptr);
}

int databuf_realloc(databuf_t * databuf, size_t growby)
{
    int result = 0;
    if (databuf != 0 && ((databuf->flags & DATABUF_FLAGS_NOREALLOC) == 0)) {
        char *temp;
        long write_offset = (databuf->wr_ptr - databuf->rd_ptr);
        DART_ALLOC(temp, 8, growby + databuf->buflen, result);
        memcpy(temp, databuf->rd_ptr, write_offset);
        if(databuf->flags & DATABUF_FLAGS_NO_FREE) {
            /*We allow growing but the memory chunk originally
             * allocated is managed elseware. Now we reset the flag
             * variable so next time we grow the buffer we will delete
             * this memory.*/
            databuf->flags = 0;
        } else {
            free(databuf->buffer);
        }
        databuf->buflen += growby;
        databuf->buffer = temp;
        databuf->rd_ptr = databuf->buffer;
        databuf->wr_ptr = databuf->buffer;
        databuf->wr_ptr += write_offset;
    }
    return result;
}

int calc_realloc_buffersize(databuf_t * databuf, size_t nminbytes)
{
    int nspace = databuf_space_left(databuf);
    int nspace_if_doubled = nspace + databuf->buflen;
    return ((nspace_if_doubled <
             nminbytes) ? nminbytes : nspace_if_doubled);
}

int databuf_write(databuf_t * databuf, const char *formatstr, ...)
{
    va_list argp;
    int result = 0;
    int nspace = databuf_space_left(databuf);
    va_start(argp, formatstr);
    result = vsnprintf(databuf->wr_ptr, nspace, formatstr, argp);
    va_end(argp);

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

    return result;
}

int databuf_safe_inc(databuf_t* db, char c)
{
    int result = DB_CAN_FIT(db, 1);
    if(result) {
        /*Do nothing we can fit*/
    } else {
        result = databuf_realloc(db, calc_realloc_buffersize(db, 2));
    }
    *db->wr_ptr++ = c;
    return result;
}

int databuf_memcpy(databuf_t * databuf, const void *source, size_t nbytes)
{
    int result = DB_CAN_FIT(databuf, nbytes);
    if (source && nbytes && result) {
            memcpy(databuf->wr_ptr, source, nbytes);
            /* Leave the write pointer writing at the null terminator,
               the terminator should not show up in the output except
               after all writing is done */
            databuf->wr_ptr += nbytes;
            result = 0;
    } else if(source && nbytes && ((databuf->flags & DATABUF_FLAGS_NOREALLOC) == 0)) {
        int newsize = calc_realloc_buffersize(databuf, nbytes + 1);
        result = databuf_realloc(databuf, newsize);
        int nspace = databuf_space_left(databuf);
        if(nspace > nbytes) {
            memcpy(databuf->wr_ptr, source, nbytes);
            databuf->wr_ptr += nbytes;
            result = 0;
        }
    } else {
        result = -1;
    }
    return result;
}
