#ifndef __UTIL_H__
#define __UTIL_H__

#include "../../server_objects/hashmap.h"
#include <stdlib.h>
#include <sys/uio.h>

static inline struct iovec make_iovec_fromstr(char *ptr, size_t len)
{
    struct iovec ret = {.iov_base = ptr,.iov_len = len };
    return ret;
}

void free_string_map(hashmap_t);
void free_map_of_maps_to_string(hashmap_t);

#endif
