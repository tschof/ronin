#include "symbol_table.h"
#include "util.h"
#include <string.h>

static void symtable_free(symbol_table_t * symtable)
{
    if (symtable != 0) {
        free_string_map(symtable->scalars);
        free_map_of_maps_to_string(symtable->maps);
    }
}

symbol_table_t *make_symbol_table(void)
{
    symbol_table_t *ret = malloc(sizeof *ret);
    ret->maps = create_map(1024);
    ret->scalars = create_map(1024);
    ret->free = symtable_free;
    return ret;
}

int symbol_table_store_map(symbol_table_t * symtable, const char *name,
                           hashmap_t map)
{
    if (no_copy_insert(symtable->maps, name, strlen(name), map)) {
        return 0;
    } else {
        return -1;
    }
}

int symbol_table_store_scalar(symbol_table_t * symtable, const char *name,
                              struct iovec val)
{
    int ret = -1;
    char *copy = strdup(val.iov_base);
    if (copy != 0) {
        if (no_copy_insert(symtable->scalars, name, strlen(name), copy)) {
            ret = 0;
        } else {
            free(copy);
        }
    }
    return ret;
}

int symbol_table_store_value(symbol_table_t * symtable, const char *name,
                             value_t * val)
{
    int ret = -1;
    if (symtable != 0 && name != 0) {
        size_t len = strlen(name);
        hashmap_t map = 0;
        void *data = 0;
        if (val->type == data_type_scalar) {
            data = val->data.scalar.iov_base;
            map = symtable->scalars;
        } else if (val->type == data_type_map) {
            data = val->data.map;
            map = symtable->maps;
        }
        if (map != 0 && data != 0 && no_copy_insert(map, name, len, data)) {
            ret = 0;
        }
    }
    return ret;
}
