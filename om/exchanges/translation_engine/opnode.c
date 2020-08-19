#include "opnode.h"
#include "op.h"
#include <stdlib.h>

static inline opnode_t *make_opnode(struct op *op)
{
    opnode_t *ret = malloc(sizeof *ret);
    ret->op = op;
    ret->next = 0;
    return ret;
}

void push_opnode(struct op *op, opnode_t ** head)
{
    opnode_t *newnode = make_opnode(op);
    if (newnode != 0) {
        if (*head == 0) {
            *head = newnode;
        } else {
            opnode_t *node;
            for (node = *head; node->next != 0; node = node->next);
            node->next = newnode;
        }
    }
}

int run_operations(opnode_t * ops, translation_unit_t * state,
                   scalar_t * result)
{
    int ret = -1;
    while (ops != 0) {
        if ((ret = ops->op->translate(ops->op, state, result)) == -1) {
            break;
        }
        ops = ops->next;
    }
    return ret;
}

void free_operations(opnode_t * node)
{
    while (node != 0) {
        opnode_t *tmp = node->next;
        node->op->free(node->op);
        free(node);
        node = tmp;
    }
}

void splice_opnodes(opnode_t ** head, opnode_t * tail)
{
    if (*head == 0) {
        *head = tail;
    } else {
        opnode_t *node;
        for (node = *head; node->next != 0; node = node->next);
        node->next = tail;
    }
}
