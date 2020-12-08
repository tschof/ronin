#include "tokenizer.h"
#include "rwbuf.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

static enum keyword identify_keyword(char *str)
{
    enum keyword keyword = not_a_keyword;
    if (strcmp(str, "type") == 0) {
        keyword = keyword_type;
    } else if (strcmp(str, "fix") == 0) {
        keyword = keyword_fix;
    }
    return keyword;
}

/* ToDo: Determine the correct behavior for this. If we return an
 * error for having a number followed by a non-digit character we may
 * erroneously rule out valid constructs such as "123,"; if we
 * completely ignore non-digit characters we may allow something like
 * "123d" and process the d as the beginning of a valid token; maybe
 * we can say that only a non-alpha character can follow? */
static int consume_number(char *token, char **end, int64_t * n)
{
    int retval = -1;
    errno = 0;
    *n = strtol(token, end, 10);
    if (errno == 0 && *end && isalpha(**end) == 0) {
        retval = 0;
    }
    return retval;
}

/* We always return a new string that has been preprocessed, or
 * nothing at all indicating a failure to parse a valid string. A new
 * string is always returned and must be free'd later; this is just
 * easier to always do than having conditional logic based on whether
 * a new string was really needed or not. */
static char *consume_string(char *line, char **end)
{
    char *str = 0, *p;
    if (*line != '"')
        return 0;

    /* Start with the actual string */
    ++line;

    /* First find the end of the string */
    for (p = line; *p && *p != '"'; ++p, *end = p) {
        /* Skip past escaped double-quotes, they are part of the
         * string and not the end-of-string delimiter. */
        if (*p == '\\' && *(p + 1)) {
            char escaped_character = *(p + 1);
            if (escaped_character == '"' || escaped_character == '\\') {
                ++p;
            } else {
                fprintf(stderr, "unsupported escape code \\%c\n",
                        escaped_character);
                return 0;
            }
        }
    }

    /* Was the string terminated properly? */
    if (*p == '\0')
        return 0;

    size_t len = *end - line;
    str = malloc(len + 1);

    for (p = str; line != *end; ++line)
        if (*line != '\\')
            *p++ = *line;
        else if (*(line + 1) == '\\')
            /* Special case in which there is an escaped backslack,
             * \\, which should result in just a single backslash in
             * the output */
            *p++ = '\\';

    *p = '\0';
    ++(*end);

    return str;
}

static void consume_symbol(char *line, char **end)
{
    if (*line && (*line == '_' || isalpha(*line))) {
        ++line;
        while (*line && (isalnum(*line) || *line == '_')) {
            ++line;
        }
    }
    *end = line;
}

static enum token_type initial_classification(const char *c)
{
    enum token_type type = token_type_error;
    if (*c == '{')
        type = token_type_blockstart;
    else if (*c == '}')
        type = token_type_blockend;
    else if (*c == '"')
        type = token_type_string;
    else if (isdigit(*c) != 0)
        type = token_type_number;
    else if (*c == '-' || *c == '+') {
        if (*(c + 1) != '\0' && isdigit(*(c + 1)) != 0) {
            type = token_type_number;
        } else {
            type = *c;
        }
    } else if (*c == '_' || isalpha(*c) != 0) {
        type = token_type_symbol;
    } else if (*c == '<' && *(c + 1) != '\0' && *(c + 1) == '-') {
        type = token_type_getsfrom;
    } else if (*c == '(') {
        type = token_type_open_paren;
    } else if (*c == ')') {
        type = token_type_close_paren;
    } else if (*c == ',') {
        type = token_type_comma;
    } else if (*c == '[') {
        type = token_type_square_bracket_open;
    } else if (*c == ']') {
        type = token_type_square_bracket_close;
    } else if (*c == ':') {
        type = token_type_colon;
    } else if (*c == ';') {
        type = token_type_semicolon;
    } else if (*c == '=') {
        type = token_type_equal;
    }
    return type;
}

static char *strdup_begend(char *in, char *end)
{
    char *str = 0;
    if (end > in) {
        const size_t len = end - in;
        str = malloc(len + 1);
        if (str != 0) {
            memcpy(str, in, len);
            str[len] = '\0';
        }
    }
    return str;
}

int next_token(char *input, char **end, token_t * t)
{
    int okay = 0;
    t->type = token_type_none;

    if (input == 0 || strlen(input) == 0)
        goto done;

    /* Discard whitespace */
    while (*input != '\0' && isspace(*input) != 0) {
        ++input;
    }

    *end = input;

    if (*input == '\0')
        goto done;

    /* Classify based on the first character or two. */
    t->type = initial_classification(input);

    okay = 0;

    /* Consume the rest of the input string according to the initial
     * type classification. */
    switch (t->type) {
    case token_type_symbol:
        consume_symbol(input, end);
        t->data.token = strdup_begend(input, *end);
        enum keyword keyword = identify_keyword(t->data.token);
        if (keyword != not_a_keyword) {
            free(t->data.token);
            t->data.keyword = keyword;
            t->type = token_type_keyword;
        }
        break;
    case token_type_string:
        if ((t->data.token = consume_string(input, end)) == 0) {
            okay = -1;
        }
        break;
    case token_type_number:
        okay = consume_number(input, end, &t->data.number);
        break;
    case token_type_getsfrom:
        *end = input + 2;
        break;
    case token_type_blockstart:
    case token_type_blockend:
    case token_type_open_paren:
    case token_type_close_paren:
    case token_type_comma:
    case token_type_square_bracket_open:
    case token_type_square_bracket_close:
    case token_type_colon:
    case token_type_semicolon:
    case token_type_equal:
        *end = input + 1;
        break;
    default:
        okay = -1;              /* Anything else is NOT okay. */
    }
  done:
    return okay;
}

struct tokenizer_impl {
    tokenizer_t parent;
    istream_t *istream;
    dstr_t buffer;
    char *p, *end;
    token_t temp;
};

/* A note regarding the tokenization process: the actual tokenization
   is done by next_token(), which takes just a char * input and places
   the address of the next unconsumed byte into a char ** passed in.
   The tokenizer "object" handles the additional task of reading data
   from the input stream (as represented by the istream_t "object")
   into temporary buffers and managing these temporary buffers while
   calling on the next_token function to get the actual next token. */
static token_t tokenizer_gettoken(tokenizer_t * tokenizer)
{
    token_t token;
    token.type = token_type_none;
    if (tokenizer != 0) {
        struct tokenizer_impl *t = (struct tokenizer_impl *) tokenizer;
        if (t->temp.type != token_type_none) {
            token = t->temp;
            t->temp.type = token_type_none;
        } else {
            while (token.type == token_type_none) {
                if (t->p == (t->buffer.p + t->buffer.n)) {
                    /* The buffer has been exhausted, get another line from the
                       input. */
                    if (t->istream->getline(t->istream, &t->buffer) == -1) {
                        token.type = token_type_error;
                        break;
                    }
                    t->p = t->buffer.p;
                }
                if (next_token(t->p, &t->end, &token) != -1) {
                    t->p = t->end;
                } else {
                    token.type = token_type_error;
                }
            }
        }
    }
    return token;
}

static void tokenizer_putback(struct tokenizer *t, token_t token)
{
    if (t != 0) {
        struct tokenizer_impl *tokenizer = (struct tokenizer_impl *) t;
        tokenizer->temp = token;
    }
}

static void tokenizer_free(struct tokenizer *t)
{
    if (t != 0) {
        struct tokenizer_impl *impl = (struct tokenizer_impl *) t;
        dstr_delete(&impl->buffer);
        impl->istream->free(impl->istream);
        free(impl);
    }
}

tokenizer_t *make_tokenizer(istream_t * istr)
{
    struct tokenizer_impl *ret = malloc(sizeof *ret);
    if (ret != 0) {
        ret->parent.gettoken = tokenizer_gettoken;
        ret->parent.putback = tokenizer_putback;
        ret->parent.free = tokenizer_free;
        ret->istream = istr;
        ret->buffer.p = 0;
        ret->buffer.n = 0;
        ret->p = ret->end = 0;
        ret->temp.type = token_type_none;
    }
    return (tokenizer_t *) ret;
}

#ifdef COMPILE_UNIT_TESTS
#include "../../unit-tester/unit-tester.h"

declare_unit_tester;

declare_unit_test(number_tokenizer)
{
    char *end;
    char *testinput = strdup("1234");
    int64_t n;
    check(consume_number(testinput, &end, &n) == 0);
    check((end - testinput) == 4u);
    check(n == 1234);
    free(testinput);
    testinput = strdup("1234 ");
    check(consume_number(testinput, &end, &n) == 0);
    check((end - testinput) == 4u);
    check(*end == ' ');
    check(n == 1234);
    free(testinput);
    testinput = strdup("  123");
    check(consume_number(testinput, &end, &n) == 0);
    check(n == 123);
    free(testinput);
    testinput = strdup("123d");
    check(consume_number(testinput, &end, &n) == -1);
    free(testinput);
    testinput = strdup("123!");
    check(consume_number(testinput, &end, &n) == 0);
    check(n == 123);
    free(testinput);
}

declare_unit_test(test_symbol_tokenizer)
{
    char *input = "a", *end;
    consume_symbol(input, &end);
    check((end - input) == 1u);
    input = "foobar ";
    consume_symbol(input, &end);
    check((end - input) == 6u);
}

declare_unit_test(test_string_tokenizer)
{
    char *input = strdup("\"1234\""), *end;
    char *str;
    check((str = consume_string(input, &end)) != 0);
    check(strcmp(str, "1234") == 0);
    free(input);
    free(str);
    input = strdup("  \"12345\"");
    check((str = consume_string(input, &end)) == 0);
    free(input);
    input = strdup("\"foo \\\"bar\\\" asdf\"");
    check((str = consume_string(input, &end)) != 0);
    check(strcmp(str, "foo \"bar\" asdf") == 0);
    free(input);
    free(str);
    input = strdup("\"\\\"");
    check((str = consume_string(input, &end)) == 0);
    free(input);
}

declare_unit_test(initial_classifier)
{
    check(initial_classification("{") == token_type_blockstart);
    check(initial_classification("}") == token_type_blockend);
    check(initial_classification(" ") == token_type_error);
    check(initial_classification("_") == token_type_symbol);
    check(initial_classification("___") == token_type_symbol);
    check(initial_classification("_123") == token_type_symbol);
    check(initial_classification("foobar") == token_type_symbol);
    check(initial_classification("1234") == token_type_number);
    check(initial_classification("+1234") == token_type_number);
    check(initial_classification("-1234") == token_type_number);
    check(initial_classification("+ 1234") == token_type_plus);
    check(initial_classification("-  ") == token_type_minus);
    check(initial_classification("\"12345\"") == token_type_string);
    check(initial_classification("(") == token_type_open_paren);
    check(initial_classification(")") == token_type_close_paren);
}

declare_unit_test(next_token)
{
    char *input = strdup("type \"D\"{\n9303 <- 15\n}"), *end, *p;
    p = input;
    struct token t;
    check(next_token(p, &end, &t) == 0);
    check(t.type == token_type_keyword);
    check(t.data.keyword == keyword_type);
    p = end;
    check(next_token(p, &end, &t) == 0);
    check(t.type == token_type_string);
    check(t.data.token != 0);
    if (t.data.token) {
        check(strcmp(t.data.token, "D") == 0);
    }
    free(t.data.token);
    p = end;
    check(next_token(p, &end, &t) == 0);
    check(t.type == token_type_blockstart);
    p = end;
    check(next_token(p, &end, &t) == 0);
    check(t.type == token_type_number);
    check(t.data.number == 9303);
    p = end;
    check(next_token(p, &end, &t) == 0);
    check(t.type == token_type_getsfrom);
    p = end;
    check(next_token(p, &end, &t) == 0);
    check(t.type == token_type_number);
    check(t.data.number == 15);
    p = end;
    check(next_token(p, &end, &t) == 0);
    check(t.type == token_type_blockend);
    p = end;
    check(next_token(p, &end, &t) == 0);
    free(input);

    istream_t *istream = make_file_inputstream("parseinput-1.in");
    check(istream != 0);
    if (istream != 0) {
        tokenizer_t *t = make_tokenizer(istream);
        check(t != 0);
        token_t token;
        if (t != 0) {
            token = t->gettoken(t);
            check(token.type == token_type_number);
            if (token.type == token_type_number) {
                check(token.data.number == 1);
            }
            token = t->gettoken(t);
            check(token.type == token_type_getsfrom);
            token = t->gettoken(t);
            check(token.type == token_type_number);
            if (token.type == token_type_number) {
                check(token.data.number == 12);
            }
            token = t->gettoken(t);
            check(token.type == token_type_semicolon);
            token = t->gettoken(t);
            check(token.type == token_type_number);
            if (token.type == token_type_number) {
                check(token.data.number == 55);
            }
            token = t->gettoken(t);
            check(token.type == token_type_getsfrom);
            t->putback(t, token);
            token = t->gettoken(t);
            check(token.type == token_type_getsfrom);
            token = t->gettoken(t);
            check(token.type == token_type_symbol);
            if (token.type == token_type_symbol) {
                check(strcmp(token.data.token, "split") == 0);
                free(token.data.token);
            }
            token = t->gettoken(t);
            check(token.type == token_type_open_paren);
            token = t->gettoken(t);
            check(token.type == token_type_number);
            if (token.type == token_type_number) {
                check(token.data.number == 5);
            }
            token = t->gettoken(t);
            check(token.type == token_type_comma);
            token = t->gettoken(t);
            check(token.type == token_type_string);
            if (token.type == token_type_string) {
                check(strcmp(token.data.token, "/") == 0);
                free(token.data.token);
            }
            token = t->gettoken(t);
            check(token.type == token_type_comma);
            token = t->gettoken(t);
            check(token.type == token_type_number);
            if (token.type == token_type_number) {
                check(token.data.number == 0);
            }
            token = t->gettoken(t);
            check(token.type == token_type_close_paren);
            token = t->gettoken(t);
            check(token.type == token_type_semicolon);
            t->free(t);
        }
    }
}

#endif
