#include "string_table.h"
#include <string.h>
#include <ctype.h>

struct strnode {
    str_t from, to;
    struct strnode *next;
};

static char *trim_string(char *text)
{
    if (text) {
        for (; text[0] && isspace(*text); ++text);
        char *endptr;
        for (endptr = text; endptr[0] && !isspace(*endptr); ++endptr);
        *endptr = '\0';
    }
    return text;
}

static void parse_key_value(char *text, char **first, char **second)
{
    char *endptr;
    *first = strtok_r(text, ":", &endptr);
    *first = trim_string(*first);
    *second = strtok_r(0, ":", &endptr);
    *second = trim_string(*second);
}

str_t make_str(const char *input)
{
    str_t s;
    s.str = strdup(input);
    s.len = strlen(s.str);
    return s;
}

struct strnode *make_strnode(const char *from, const char *to)
{
    struct strnode *node = malloc(sizeof *node);
    node->from = make_str(from);
    node->to = make_str(to);
    node->next = 0;
    return node;
}

void add_strnode(struct strnode **head, const char *from, const char *to)
{
    if (*head == 0) {
        *head = make_strnode(from, to);
    } else {
        struct strnode *node;
        for (node = *head; node->next != 0; node = node->next);
        node->next = make_strnode(from, to);
    }
}

void free_strnode(struct strnode *node)
{
    free(node->from.str);
    free(node->to.str);
    free(node);
}

void free_strnode_list(struct strnode *node)
{
    while (node) {
        struct strnode *next = node->next;
        free_strnode(node);
        node = next;
    }
}

struct strnode *parse_table_conf(char *text)
{
    struct strnode *head = 0;
    char *endptr = 0, *first, *second;
    for (text = strtok_r(text, ",", &endptr); text != 0;
         text = strtok_r(0, ",", &endptr)) {
        parse_key_value(text, &first, &second);
        add_strnode(&head, first, second);
    }
    return head;
}

str_t *lookup_str(struct strnode * node, const char *str, size_t len)
{
    str_t *retval = 0;
    struct strnode *temp;
    for (temp = node; temp != 0; temp = temp->next) {
        if (len == temp->from.len
            && strncmp(temp->from.str, str, len) == 0) {
            retval = &temp->to;
            break;
        }
    }
    return retval;
}
