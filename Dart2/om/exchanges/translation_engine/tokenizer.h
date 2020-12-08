#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include <stdint.h>
#include "inputstream.h"

enum token_type {
    token_type_keyword,
    token_type_number,
    token_type_string,
    token_type_symbol,
    token_type_open_paren = '(',
    token_type_close_paren = ')',
    token_type_blockstart = '{',
    token_type_blockend = '}',
    token_type_square_bracket_open = '[',
    token_type_square_bracket_close = ']',
    token_type_comma = ',',
    token_type_plus = '+',
    token_type_minus = '-',
    token_type_colon = ':',
    token_type_semicolon = ';',
    token_type_equal = '=',
    token_type_getsfrom,
    token_type_none,            /* Set when there's just whitespace or the input
                                 * string is empty. Not necessarily an error, but
                                 * contains nothing useful. */
    token_type_error = 255,
};

enum keyword {
    not_a_keyword,
    keyword_type,               /* "type" */
    keyword_fix,                /* "fix"  */
};

union token_data {
    char *token;                /* TODO: Text would be a better name? */
    int64_t number;
    enum keyword keyword;
};

typedef struct token {
    enum token_type type;
    union token_data data;
} token_t;

struct tokenizer {
    token_t(*gettoken) (struct tokenizer *);
    void (*putback) (struct tokenizer *, token_t);
    void (*free) (struct tokenizer *);
};

typedef struct tokenizer tokenizer_t;

tokenizer_t *make_tokenizer(istream_t *);

int next_token(char *, char **, token_t *);

#define require_token(token, tokenizer, token_type, label)              \
    if (((token) = (tokenizer)->gettoken ((tokenizer))).type != (token_type)) { \
        goto label;                                                     \
    }

#define require_token_and_clean(token, tokenizer, token_type, label)    \
    if (((token) = (tokenizer)->gettoken ((tokenizer))).type != (token_type)) { \
        if ((token).type == token_type_symbol) {                        \
            free(token.data.token);                                     \
        }                                                               \
        goto label;                                                     \
    }

#define require_symbol(token, tokenizer, str, label)                    \
    if (((token) = (tokenizer)->gettoken ((tokenizer))).type != token_type_symbol || \
        strcmp((token).data.token, str) != 0) {                         \
        if ((token).type == token_type_symbol) {                        \
            free(token.data.token);                                     \
        }                                                               \
        goto label;                                                     \
    }

#endif
