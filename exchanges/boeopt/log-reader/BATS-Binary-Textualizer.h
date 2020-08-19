#ifndef __BATS_BINARY_TEXTUALIZER_H__
#define __BATS_BINARY_TEXTUALIZER_H__

#include <stdlib.h>
#include "server_objects/databuf.h"

ssize_t frame (const void *, size_t);
int textualize (const void *, size_t, databuf_t *);

#endif
