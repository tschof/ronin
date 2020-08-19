#ifndef __DATABUF_H__
#define __DATABUF_H__

#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct databuf {
        size_t buflen;
        char *buffer;
        char *rd_ptr;
        char *wr_ptr;
        unsigned int flags;
    } databuf_t;

    databuf_t *databuf_alloc(size_t suggestedsize);

    int databuf_attach(databuf_t * databuf, char *buf, size_t len);

    int databuf_reset(databuf_t * databuf);

    void databuf_destroy(databuf_t * databuf);

    int databuf_write(databuf_t * databuf, const char *formatstr, ...);

    int databuf_memcpy(databuf_t * databuf, const void *source,
                       size_t nbytes);

    int databuf_realloc(databuf_t * databuf, size_t growby);

    size_t databuf_unread(databuf_t * databuf);

    size_t databuf_space_left(databuf_t * databuf);

    int databuf_attach(databuf_t * databuf, char *buf, size_t len);

    /* Makes sure there is at least n bytes in the writeable space of
       the buffer. */
    int databuf_reserve(databuf_t * databuf, size_t n);

#define databuf_bufsize(databuf)                \
    (databuf)->buflen

    inline static void databuf_clear(void *p) {
        databuf_reset((databuf_t *) p);
    } void databuf_setrealloc(struct databuf *databuf);
    void databuf_setnorealloc(struct databuf *databuf);

#ifdef __cplusplus
}
#endif
#endif                          //__DATABUF_H__
