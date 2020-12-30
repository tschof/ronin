#include "proto-rule-parser.h"
#include "tokenizer.h"
#include "rwbuf.h"
#include "grammar.h"

struct proto_rule_parser_impl {
    proto_rule_parser_t parent;
    grammar_parse_func_t next;
};

static parse_state_t *proto_rule_parse(proto_rule_parser_t * parser,
                                       istream_t * istr)
{
    struct proto_rule_parser_impl *parserimpl =
        (struct proto_rule_parser_impl *) parser;
    parse_state_t *state = malloc(sizeof *state);
    state->tokenizer = make_tokenizer(istr);
    state->layout = make_empty_message_layout();
    state->rules = 0;
    state->global_vars = make_symbol_table();
    if (state->tokenizer != 0) {
        parse_result_t result = {.status = keep_going,.next =
                parserimpl->next
        };
        while (result.status == keep_going && result.next != 0) {
            result = result.next(state);
        }
    }
    return state;
}

static void proto_rule_free(struct proto_rule_parser *p)
{
    free(p);
}

proto_rule_parser_t *make_proto_rule_parser(grammar_parse_func_t parsefunc)
{
    struct proto_rule_parser_impl *p = malloc(sizeof *p);
    p->parent.parse = proto_rule_parse;
    p->parent.free = proto_rule_free;
    p->next = parsefunc;
    return (proto_rule_parser_t *) p;
}

#ifdef COMPILE_UNIT_TESTS
#include "../../unit-tester/unit-tester.h"
#include "rw_fix_msg.h"

#include "rdonly_csv_msg.h"

#define narray(array) (sizeof (array) / sizeof (array)[0])

/* This test is a general one to parse the parseinput-1.in file, see
   if it produces a proper set of rules and to convert a csv message
   into a fix one. */
declare_unit_test(parse_entrypoint)
{
    proto_rule_parser_t *parser = make_proto_rule_parser(parse_entrypoint);
    check(parser != 0);
    if (parser != 0) {
        istream_t *istr = make_file_inputstream("parseinput-2.in");
        check(istr != 0);
        /* rule_t *rules = parser->parse(parser, istr); */
        /* check(rules != 0); */
        parser->free(parser);
    }
}

#endif
