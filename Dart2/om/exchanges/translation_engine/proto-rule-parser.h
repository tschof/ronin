#ifndef __PROTO_RULE_PARSER_H__
#define __PROTO_RULE_PARSER_H__

#include "rule.h"
#include "inputstream.h"
#include "grammar.h"

struct proto_rule_parser {
    parse_state_t *(*parse) (struct proto_rule_parser *, istream_t *);
    void (*free) (struct proto_rule_parser *);
};

typedef struct proto_rule_parser proto_rule_parser_t;

proto_rule_parser_t *make_proto_rule_parser(grammar_parse_func_t);

#endif
