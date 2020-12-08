#ifndef __OPNODE_H__
#define __OPNODE_H__

#include "rdonly_msg.h"
#include "rw_msg.h"
#include "translation_unit.h"
#include <sys/uio.h>

struct op;

struct op_node;
typedef struct op_node opnode_t;

struct op_node {
    struct op *op;
    opnode_t *next;
};

void push_opnode(struct op *, opnode_t **);
void splice_opnodes(opnode_t **, opnode_t *);
int run_operations(opnode_t *, translation_unit_t *, scalar_t *);
void free_operations(opnode_t *);

#endif
