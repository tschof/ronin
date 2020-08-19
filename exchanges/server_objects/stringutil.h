#ifndef __STRINGUTIL_H__
#define __STRINGUTIL_H__

#include <stdlib.h>

struct databuf;

/* Copy the text buffer defined by the first two arguments to the
 * databuf_t, replacing any comma characters with the string indicated
 * by the last two arguments. */
void filtercommas(const char *p, size_t n, struct databuf *output,
                  const char *preplace, size_t nreplace);

#endif
