#ifndef __STRING_TABLE_H__
#define __STRING_TABLE_H__

#include <stdlib.h>

typedef struct str {
    char *str;
    size_t len;
} str_t;

typedef struct strnode strtable_t;

strtable_t *parse_table_conf(char *);
str_t *lookup_str(strtable_t *, const char *, size_t);
void free_strnode(strtable_t *);

#endif
