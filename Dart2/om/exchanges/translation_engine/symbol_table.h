#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

#include "value.h"
#include "../../server_objects/hashmap.h"
#include <sys/uio.h>

struct symbol_table {
    hashmap_t maps;
    hashmap_t scalars;
    void (*free) (struct symbol_table *);
};

typedef struct symbol_table symbol_table_t;

symbol_table_t *make_symbol_table(void);
int symbol_table_store_map(symbol_table_t *, const char *, hashmap_t);
int symbol_table_store_scalar(symbol_table_t *, const char *,
                              struct iovec);
int symbol_table_store_value(symbol_table_t *, const char *, value_t *);

#endif
