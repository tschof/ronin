#ifndef __FIXMSG_H__
#define __FIXMSG_H__

#include "databuf.h"
#include <stdlib.h>

struct tm;

int fixmsg_addstr2(databuf_t * databuf, const char *tag,
                   size_t tlen, const char *val, size_t vlen);

int fixmsg_addstr(databuf_t * databuf, const char *tag, const char *val);

int fixmsg_addstr_bylen(databuf_t * databuf, const char *tag,
                        const char *val, size_t len);

int fixmsg_addtime(databuf_t * databuf, const char *tag,
                   const struct tm *timeval);

int fixmsg_adduint(databuf_t * databuf, const char *tag,
                   size_t tlen, unsigned int i);

#endif
