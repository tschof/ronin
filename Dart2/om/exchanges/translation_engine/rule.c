#include "rule.h"
#include <sys/uio.h>
#include "opnode.h"

rule_t *make_rule(opnode_t * ops, size_t outfield)
{
    rule_t *ret = malloc(sizeof *ret);
    ret->ops = ops;
    ret->outfield = outfield;
    ret->next = 0;
    return ret;
}

void push_rule(opnode_t * opnodes, size_t outfield, rule_t ** head)
{
    rule_t *newnode = make_rule(opnodes, outfield);
    if (newnode != 0) {
        if (*head == 0) {
            *head = newnode;
        } else {
            rule_t *node;
            for (node = *head; node->next != 0; node = node->next);
            node->next = newnode;
        }
    }
}

void free_rules(rule_t * rules)
{
    while (rules != 0) {
        rule_t *tmp = rules->next;
        free_operations(rules->ops);
        free(rules);
        rules = tmp;
    }
}
