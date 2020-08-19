#ifndef __OP_H__
#define __OP_H__

#include "opnode.h"
#include "rdonly_msg.h"
#include "rw_msg.h"
#include "../../server_objects/hashmap.h"
#include "translation_unit.h"
#include <stdint.h>
#include <sys/uio.h>

struct op;
typedef struct op op_t;

struct op {
    int (*translate) (op_t *, translation_unit_t *, scalar_t *);
    void (*free) (void *);
};

op_t *make_direct_op(size_t);
op_t *make_string_constant_op(const char *);
op_t *make_split_op(uint16_t, char);
op_t *make_map_op(hashmap_t, opnode_t *);
op_t *make_lookupmap_op(char *);

#endif
