#include "stringutil.h"
#include "databuf.h"
#include <string.h>

void filtercommas(const char *p, size_t n, struct databuf *output,
                  const char *preplace, size_t nreplace)
{
    const char *end = p + n, *comma;
    while ((comma = memchr(p, ',', end - p)) != 0) {
        databuf_memcpy(output, p, comma - p);
        databuf_memcpy(output, preplace, nreplace);
        p = comma + 1;
    }
    if (p < end)
        databuf_memcpy(output, p, end - p);
}
