#ifndef __VALUE_H__
#define __VALUE_H__

#include "data_types.h"

typedef union data {
    scalar_t scalar;
    array_t array;
    map_t map;
} data_t;

typedef struct value {
    data_type_t type;
    data_t data;
} value_t;

#define value_initializer {.type = data_type_undefined}

typedef struct value_node {
    value_t val;
    struct value_node *next;
} value_node_t;

void free_val(value_t *);

#endif
