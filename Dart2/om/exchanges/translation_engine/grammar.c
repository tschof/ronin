#include "grammar.h"
#include "op.h"
#include "../../server_objects/hashmap.h"
#include "value.h"
#include "symbol_table.h"
#include "message_layout.h"
#include "util.h"

static opnode_t *parse_arg(tokenizer_t *);

static opnode_t *parse_split(tokenizer_t * tokenizer)
{
    opnode_t *ret = 0;
    token_t token;
    require_token(token, tokenizer, token_type_open_paren, done);
    if ((ret = parse_arg(tokenizer)) == 0) {
        goto done;
    }
    uint16_t outarrayidx;
    char sep;
    require_token(token, tokenizer, token_type_comma, done);
    require_token(token, tokenizer, token_type_string, done);
    if (strlen(token.data.token) > 1) {
        ret = 0;
        /* Free the, possibly existing, opnodes. */
        goto done;
    }
    sep = token.data.token[0];
    free(token.data.token);
    require_token(token, tokenizer, token_type_comma, done);
    require_token(token, tokenizer, token_type_number, done);
    outarrayidx = (uint16_t) token.data.number;
    require_token(token, tokenizer, token_type_close_paren, done);
    push_opnode(make_split_op(outarrayidx, sep), &ret);
  done:
    return ret;
}

static int parse_dict(tokenizer_t * tokenizer, hashmap_t map)
{
    int ret = -1;
    token_t token;
    require_token(token, tokenizer, token_type_square_bracket_open, done);
    for (;;) {
        require_token(token, tokenizer, token_type_string, done);
        const char *key = token.data.token;
        require_token(token, tokenizer, token_type_colon, done);
        require_token(token, tokenizer, token_type_string, done);
        if (!no_copy_insert(map, key, (int) strlen(key), token.data.token)) {
            goto done;
        }
        token = tokenizer->gettoken(tokenizer);
        if (token.type == token_type_square_bracket_close) {
            ret = 0;
            break;
        } else if (token.type != token_type_comma) {
            break;
        }
    }
  done:
    return ret;
}

static opnode_t *parse_lookupmap(tokenizer_t * tokenizer)
{
    opnode_t *ret = 0;
    token_t token;
    require_token(token, tokenizer, token_type_open_paren, done);
    if ((ret = parse_arg(tokenizer)) == 0) {
        goto done;
    }
    require_token(token, tokenizer, token_type_comma, done);
    require_token(token, tokenizer, token_type_symbol, done);
    char *varname = token.data.token;
    require_token(token, tokenizer, token_type_close_paren, done);
    push_opnode(make_lookupmap_op(varname), &ret);
  done:
    return ret;
}

static opnode_t *parse_map(tokenizer_t * tokenizer)
{
    opnode_t *ret = 0;
    token_t token;
    require_token(token, tokenizer, token_type_open_paren, done);
    if ((ret = parse_arg(tokenizer)) == 0) {
        goto done;
    }
    require_token(token, tokenizer, token_type_comma, done);
    hashmap_t map = create_map(1024);
    if (parse_dict(tokenizer, map) == -1) {
        goto done;
    }
    token = tokenizer->gettoken(tokenizer);
    opnode_t *defaultop = 0;
    if (token.type == token_type_comma) {
        defaultop = parse_arg(tokenizer);
        require_token(token, tokenizer, token_type_close_paren, done);
    } else if (token.type != token_type_close_paren) {
        goto done;
    }
    push_opnode(make_map_op(map, defaultop), &ret);
  done:
    return ret;
}

static opnode_t *parse_func(const char *funcname, tokenizer_t * tokenizer)
{
    opnode_t *ret = 0;
    if (strcmp(funcname, "split") == 0) {
        ret = parse_split(tokenizer);
    } else if (strcmp(funcname, "map") == 0) {
        ret = parse_map(tokenizer);
    } else if (strcmp(funcname, "lookupmap") == 0) {
        ret = parse_lookupmap(tokenizer);
    }
    return ret;
}

static opnode_t *parse_arg(tokenizer_t * tokenizer)
{
    opnode_t *ret = 0;
    token_t token = tokenizer->gettoken(tokenizer);
    switch (token.type) {
    case token_type_number:
        /* Shorthand for a direct/basic operation which maps from a
           field number. */
        push_opnode(make_direct_op(token.data.number), &ret);
        break;
    case token_type_string:
        /* Shorthand for a string constant. */
        push_opnode(make_string_constant_op(token.data.token), &ret);
        break;
    case token_type_symbol:
        ret = parse_func(token.data.token, tokenizer);
        break;
    default:
        ret = 0;
    }
    return ret;
}

typedef value_t(*function) (value_node_t *);

static int parse_expression(tokenizer_t *, value_t *, symbol_table_t *);

static int eval_dict(tokenizer_t * tokenizer, value_t * val,
                     symbol_table_t * symtable __attribute__ ((unused)))
{
    int ret = -1;
    val->type = data_type_map;
    val->data.map = create_map(1024);
    token_t token = {.type = token_type_none };
    char *key = 0;
    if (val->data.map != 0) {
        require_token(token, tokenizer, token_type_square_bracket_open,
                      done);
        for (;;) {
            require_token(token, tokenizer, token_type_string, done);
            key = token.data.token;
            require_token(token, tokenizer, token_type_colon, done);
            require_token(token, tokenizer, token_type_string, done);
            if (!no_copy_insert
                (val->data.map, key, (int) strlen(key),
                 token.data.token)) {
                goto done;
            }
            token = tokenizer->gettoken(tokenizer);
            if (token.type == token_type_square_bracket_close) {
                ret = 0;
                break;
            } else if (token.type != token_type_comma) {
                break;
            }
        }
    }
  done:
    if (ret == -1) {
        free(key);
        free_val(val);
        if (token.type == token_type_symbol) {
            free(token.data.token);
        }
    }
    return ret;
}

static int eval_load_map_from_file(tokenizer_t * tokenizer, value_t * val,
                                   symbol_table_t * symtable)
{
    int ret = -1;
    val->type = token_type_none;
    if ((ret = parse_expression(tokenizer, val, symtable)) == 0) {
        if (val->type == data_type_scalar) {
            /* A scalar argument to load_map_from_file represents a file
             * name which we open to parse the dictionary. */
            istream_t *file =
                make_file_inputstream(val->data.scalar.iov_base);
            if (file != 0) {
                tokenizer_t *filetok = make_tokenizer(file);
                ret = eval_dict(filetok, val, symtable);
                filetok->free(filetok);
            }
        } else if (val->type == data_type_map) {
            ret = eval_dict(tokenizer, val, symtable);
        }
        /* There is only one scalar argument to load_map_from_file. */
        token_t token = tokenizer->gettoken(tokenizer);
        if (token.type != token_type_close_paren) {
            ret = -1;
        }
    }
    if (ret == -1) {
        free_val(val);
    }
    return ret;
}

static int eval_expr(parse_state_t *, value_t *);

static int eval_load_map_from_file2(parse_state_t * state, value_t * val)
{
    int ret = -1;
    val->type = token_type_none;
    if ((ret = eval_expr(state, val)) == 0) {
        if (val->type == data_type_scalar) {
            /* A scalar argument to load_map_from_file represents a file
             * name which we open to parse the dictionary. */
            istream_t *file =
                make_file_inputstream(val->data.scalar.iov_base);
            if (file != 0) {
                tokenizer_t *filetok = make_tokenizer(file);
                if (filetok != 0) {
                    val->type = data_type_map;
                    val->data.map = create_map(1024);
                    if ((ret = parse_dict(filetok, val->data.map)) == -1) {
                        free_string_map(val->data.map);
                    }
                    filetok->free(filetok);
                }
            }
        } else if (val->type == data_type_map) {
            val->type = data_type_map;
            val->data.map = create_map(1024);
            if ((ret = parse_dict(state->tokenizer, val->data.map)) == -1) {
                free_string_map(val->data.map);
            }
        }
        /* There is only one scalar argument to load_map_from_file. */
        token_t token = state->tokenizer->gettoken(state->tokenizer);
        if (token.type != token_type_close_paren) {
            ret = -1;
            free_val(val);
        }
    }
    if (ret == -1) {
        free_val(val);
    }
    return ret;
}

typedef struct function_parser {
    const char *name;
    int (*parse) (tokenizer_t * tokenizer, value_t * val,
                  symbol_table_t * symtable);
} function_parser_t;

static const function_parser_t function_parsers[] = {
    {"load_map_from_file", eval_load_map_from_file}
};

typedef struct function_parser2 {
    const char *name;
    int (*parse) (parse_state_t *, value_t *);
} function_parser2_t;

static const function_parser2_t function_parsers2[] = {
    {"load_map_from_file", eval_load_map_from_file2},
};

#define narray(array) (sizeof (array) / sizeof (array)[0])

static int eval_symbol(tokenizer_t * tokenizer, value_t * val,
                       symbol_table_t * symtable)
{
    int ret = -1;
    token_t token;
    require_token(token, tokenizer, token_type_symbol, done);
    char *symname = token.data.token;
    token = tokenizer->gettoken(tokenizer);
    if (token.type == token_type_open_paren) {
        size_t i, nparsers;
        for (i = 0, nparsers = narray(function_parsers); i < nparsers; ++i) {
            if (strcmp(function_parsers[i].name, symname) == 0) {
                ret = function_parsers[i].parse(tokenizer, val, symtable);
                break;
            }
        }
    } else {
        /* Must be a variable. */
        tokenizer->putback(tokenizer, token);

        /* Check for scalar THEN map; in that order. */
    }
    free(symname);
  done:
    return ret;
}

static int eval_symbol2(parse_state_t * state, value_t * result)
{
    int ret = -1;
    token_t token;
    tokenizer_t *tokenizer = state->tokenizer;
    require_token(token, tokenizer, token_type_symbol, done);
    char *symname = token.data.token;
    token = tokenizer->gettoken(tokenizer);
    if (token.type == token_type_open_paren) {
        /* This is the name of a function, go ahead, parse and
         * evaluate it. */
        size_t i, nparsers;
        for (i = 0, nparsers = narray(function_parsers2); i < nparsers;
             ++i) {
            if (strcmp(function_parsers2[i].name, symname) == 0) {
                ret = function_parsers2[i].parse(state, result);
                break;
            }
        }
    } else {
        /* This is a variable, do a lookup and use that as the
         * value. */
        tokenizer->putback(tokenizer, token);
        /* TODO: Lookup in symbol table, first in scalar, then in map.
         * If the name is duplicated then you will get the scalar.
         * This is not ideal, but maybe in another iteration the dual
         * symbol tables can be merged into one and a single lookup
         * done. */
    }
  done:
    free(symname);
    return ret;
}

/* This is the generic function to evaluate an expression to find the
 * result, which MUST be a value of some sort, whether a scalar or a
 * map.  The result is stored in the value_t. */
static int eval_expr(parse_state_t * state, value_t * result)
{
    int ret = -1;
    token_t token;
    tokenizer_t *tokenizer = state->tokenizer;
    token = tokenizer->gettoken(tokenizer);
    switch (token.type) {
    case token_type_string:
        result->type = data_type_scalar;
        result->data.scalar.iov_base = token.data.token;
        result->data.scalar.iov_len = strlen(token.data.token);
        ret = 0;
        break;
    case token_type_square_bracket_open:
        tokenizer->putback(tokenizer, token);
        result->type = data_type_map;
        result->data.map = create_map(1024);
        if (parse_dict(tokenizer, result->data.map) == -1) {
            free_string_map(result->data.map);
            result->type = data_type_undefined;
        } else {
            ret = 0;
        }
        break;
    case token_type_symbol:
        tokenizer->putback(tokenizer, token);
        ret = eval_symbol2(state, result);
        break;
    default:
        result->type = data_type_undefined;
        ret = -1;
    }
    return ret;
}

static int parse_expression(tokenizer_t * tokenizer, value_t * val,
                            symbol_table_t * symtable)
{
    int ret = 0;
    token_t token;
    token = tokenizer->gettoken(tokenizer);
    switch (token.type) {
    case token_type_string:
        val->type = data_type_scalar;
        val->data.scalar.iov_base = token.data.token;
        val->data.scalar.iov_len = strlen(token.data.token);
        break;
    case token_type_square_bracket_open:
        tokenizer->putback(tokenizer, token);
        ret = eval_dict(tokenizer, val, symtable);
        break;
    case token_type_symbol:
        tokenizer->putback(tokenizer, token);
        ret = eval_symbol(tokenizer, val, symtable);
        break;
    default:
        ret = -1;
    }
    return ret;
}

static parse_result_t parse_message_rules(parse_state_t * state)
{
    parse_result_t ret = {.status = parse_error,.next = 0 };
    token_t token = state->tokenizer->gettoken(state->tokenizer);
    if (token.type == token_type_none) {
        /* We're done, bail with an all okay but don't keep going. */
        ret.status = parse_done;
        ret.next = 0;
        goto done;
    } else if (token.type != token_type_number) {
        goto done;
    }
    size_t outfield = token.data.number;
    if (append_to_message_layout(state->layout, outfield)) {
        /* Could not append. */
        goto done;
    }
    token = state->tokenizer->gettoken(state->tokenizer);
    if (token.type == token_type_getsfrom) {
        opnode_t *rule_ops = parse_arg(state->tokenizer);
        if (rule_ops != 0) {
            push_rule(rule_ops, outfield, &state->rules);
            ret.status = keep_going;
            ret.next = parse_message_rules;
            token = state->tokenizer->gettoken(state->tokenizer);
        }
    }
    if (token.type == token_type_semicolon) {
        /* Must end each rule with a semicolon. */
        ret.status = keep_going;
        ret.next = parse_message_rules;
    } else {
        /* Report an error. */
        ret.status = parse_error;
        ret.next = 0;
    }
  done:
    return ret;
}

static parse_result_t parse_global_variable_declarations(parse_state_t *
                                                         state)
{
    parse_result_t ret = {.status = parse_error,.next = 0 };
    tokenizer_t *tokenizer = state->tokenizer;
    token_t token = tokenizer->gettoken(tokenizer);
    char *varname = 0;
    if (token.type == token_type_blockend) {
        /* BEGIN block is over, continue on with the regular parsing. */
        ret.status = keep_going;
        ret.next = parse_message_rules;
    } else if (token.type == token_type_symbol) {
        if (strcmp(token.data.token, "var") != 0) {
            goto done;
        }
        require_token(token, tokenizer, token_type_symbol, done);
        varname = token.data.token;
        require_token(token, tokenizer, token_type_equal, done);
        value_t value;
        int result = eval_expr(state, &value);
        if (!result) {
            token = tokenizer->gettoken(tokenizer);
            if (token.type != token_type_semicolon) {
                free_val(&value);
                goto done;
            }
            if (!symbol_table_store_value
                (state->global_vars, varname, &value)) {
                ret.status = keep_going;
                ret.next = parse_global_variable_declarations;
            } else {
                free_val(&value);
            }
        }
    }
  done:
    free(varname);
    return ret;
}

static parse_result_t parse_begin_block(parse_state_t * state)
{
    parse_result_t ret = {.status = parse_error,.next = 0 };
    tokenizer_t *tokenizer = state->tokenizer;
    token_t token;
    require_token(token, tokenizer, token_type_symbol, done);
    if (strcmp(token.data.token, "BEGIN") != 0) {
        goto done;
    }
    require_token(token, tokenizer, token_type_blockstart, done);
    ret.status = keep_going;
    ret.next = parse_global_variable_declarations;
  done:
    if (ret.status == parse_error && token.type == token_type_symbol) {
        free(token.data.token);
    }
    return ret;
}

/* Right now the entrypoint to any set of rules. The next field of the
 * parse_result_t that is returned will point to the next parsing
 * function to be used.  Initially, there are two main forms of
 * higher-level parsing, BEGIN block and message rules.  These are
 * distinct in that the BEGIN block specifies rules that are parsed
 * AND EVALUATED at the time the parser is invoked.  As a result there
 * are restrictions on what rules can appear in the BEGIN block.
 * Currently the only operation that can be done is to setup global
 * variables that can be then used in the message processing
 * operations.  The message processing operations are parsed ONLY and
 * a binary representation is produced as a series of rule_t objects
 * that can be used at runtime to evaluate the translation of an input
 * message to an output message. */

/* FURTHER NOTE: The BEGIN block is rather anemic right now, but may
 * be useful at a later date to specify interesting and specific rules
 * that modify core parts of the application.  An END block may be
 * useful to do things like dump counters or log something upon
 * termination. */

parse_result_t parse_entrypoint(parse_state_t * state)
{
    parse_result_t ret = {.status = parse_error,.next = 0 };
    tokenizer_t *tokenizer = state->tokenizer;
    token_t token = tokenizer->gettoken(tokenizer);
    if (token.type == token_type_symbol
        && strcmp(token.data.token, "BEGIN") == 0) {
        ret.status = keep_going;
        ret.next = parse_begin_block;
    } else {
        ret.status = keep_going;
        ret.next = parse_message_rules;
    }
    tokenizer->putback(tokenizer, token);
    return ret;
}

#ifdef COMPILE_UNIT_TESTS
#include "../../unit-tester/unit-tester.h"

declare_unit_test(eval_dict)
{
    /* Try a good dictionary declaration. */
    istream_t *in =
        make_string_inputstream
        (" [  \"foo\" : \"bar\",\n\t \"answer\" : \"42\"]");
    check(in != 0);
    if (in != 0) {
        tokenizer_t *tokenizer = make_tokenizer(in);
        check(tokenizer != 0);
        if (tokenizer != 0) {
            value_t val;
            int result = eval_dict(tokenizer, &val, 0);
            check(result == 0);
            check(val.type == data_type_map);
            check(val.data.map != 0);
            void *data;
            check(get_map_entry(val.data.map, "foo", 3, &data) > 0);
            const char *str = data;
            check(strcmp(str, "bar") == 0);
            check(get_map_entry(val.data.map, "answer", 6, &data) > 0);
            str = data;
            check(strcmp(str, "42") == 0);
            tokenizer->free(tokenizer);
        }
    }
    /* Try an invalid dictionary declaration. */
    in = make_string_inputstream("[\"foo\":,\"sf\":\"asd\"]");
    check(in != 0);
    if (in != 0) {
        tokenizer_t *tokenizer = make_tokenizer(in);
        check(tokenizer != 0);
        if (tokenizer != 0) {
            value_t val;
            int result = eval_dict(tokenizer, &val, 0);
            check(result == -1);
            tokenizer->free(tokenizer);
        }
    }
}

declare_unit_test(parse_expression_scalar)
{
    istream_t *in = make_string_inputstream("\"foo\"");
    check(in != 0);
    if (in != 0) {
        tokenizer_t *tokenizer = make_tokenizer(in);
        check(tokenizer != 0);
        if (tokenizer != 0) {
            value_t val;
            int result = parse_expression(tokenizer, &val, 0);
            check(result == 0);
            if (result == 0) {
                check(val.type == data_type_scalar);
                if (val.type == data_type_scalar) {
                    check(strcmp(val.data.scalar.iov_base, "foo") == 0);
                    check(val.data.scalar.iov_len == 3);
                }
                free_val(&val);
            }
            tokenizer->free(tokenizer);
        }
    }
}

declare_unit_test(parse_expression_map)
{
    istream_t *in =
        make_string_inputstream
        (" [  \"foo\" : \"bar\",\n\t \"answer\" : \"42\"]");
    check(in != 0);
    if (in != 0) {
        tokenizer_t *tokenizer = make_tokenizer(in);
        check(tokenizer != 0);
        if (tokenizer != 0) {
            value_t val;
            int result = parse_expression(tokenizer, &val, 0);
            check(result == 0);
            check(val.type == data_type_map);
            check(val.data.map != 0);
            void *data;
            check(get_map_entry(val.data.map, "foo", 3, &data) > 0);
            const char *str = data;
            check(strcmp(str, "bar") == 0);
            check(get_map_entry(val.data.map, "answer", 6, &data) > 0);
            str = data;
            check(strcmp(str, "42") == 0);
            tokenizer->free(tokenizer);
        }
    }
}

declare_unit_test(parse_expression_load_map_from_file)
{
    istream_t *in =
        make_string_inputstream("load_map_from_file (\"mapfile1.in\")");
    check(in != 0);
    if (in != 0) {
        tokenizer_t *tokenizer = make_tokenizer(in);
        check(tokenizer != 0);
        if (tokenizer != 0) {
            value_t val;
            int result = parse_expression(tokenizer, &val, 0);
            check(result == 0);
            check(val.type == data_type_map);
            check(val.data.map != 0);
            void *data;
            check(get_map_entry(val.data.map, "FOO", 3, &data) > 0);
            const char *str = data;
            check(strcmp(str, "12345") == 0);
            check(get_map_entry(val.data.map, "BAR", 3, &data) > 0);
            str = data;
            check(strcmp(str, "67890") == 0);
            tokenizer->free(tokenizer);
        }
    }
}

#endif
