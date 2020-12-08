#ifndef __DATA_TYPES_H__
#define __DATA_TYPES_H__

#include "../../server_objects/hashmap.h"
#include <stdlib.h>
#include <sys/uio.h>

enum data_type {
    data_type_scalar,
    data_type_map,
    data_type_undefined,
};
typedef enum data_type data_type_t;

/* Here are our basic data types. */

typedef struct iovec scalar_t;

typedef struct array {
    scalar_t *scalar_ptr;
    size_t scalar_num;
} array_t;

typedef hashmap_t map_t;

#endif
