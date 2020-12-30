#ifndef __GRAMMAR_H__
#define __GRAMMAR_H__

#include "tokenizer.h"
#include "rule.h"
#include "message_layout.h"

/* This is a purely ephemeral object that keeps the bundle of objects
 * that need to be updated together in one place while the parsing is
 * done. */
typedef struct parse_state {
    tokenizer_t *tokenizer;
    message_layout_t *layout;
    rule_t *rules;
    symbol_table_t *global_vars;
} parse_state_t;

enum parse_status {
    parse_error,
    keep_going,
    parse_done,
};

typedef enum parse_status parse_status_t;
struct parse_result;
typedef struct parse_result parse_result_t;
typedef parse_result_t(*grammar_parse_func_t) (parse_state_t *);

typedef struct parse_result {
    parse_status_t status;
    grammar_parse_func_t next;
} parse_result_;

parse_result_t parse_entrypoint(parse_state_t *);

#endif
