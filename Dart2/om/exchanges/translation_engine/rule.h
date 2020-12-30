#ifndef __RULE_H__
#define __RULE_H__

#include <stdlib.h>
#include "rdonly_msg.h"
#include "rw_msg.h"
#include "opnode.h"

struct rule_node;
typedef struct rule_node rule_t;

struct rule_node {
    opnode_t *ops;
    size_t outfield;
    rule_t *next;
};

rule_t *make_rule(opnode_t *, size_t);
void push_rule(opnode_t *, size_t, rule_t **);
void free_rules(rule_t *);

#endif
