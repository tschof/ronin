#ifndef __SYMBOL_MAP_H__
#define __SYMBOL_MAP_H__

struct symbolmap;
typedef struct symbolmap symbolmap_t;
symbolmap_t *load_symbol_map(const char *);
int lookup_symbol(symbolmap_t * sm, string_t from, string_t * to);
void free_symbol_map(symbolmap_t *);
int find_future_root(const char *, size_t, const char **, size_t *);

struct synchronized_symbolmap;
typedef struct synchronized_symbolmap synch_symbolmap_t;
synch_symbolmap_t *create_synchronized_symbolmap(const char *);
int reload_synchronized_symbolmap(synch_symbolmap_t *, const char *);
int synchronized_lookup_symbol(synch_symbolmap_t *, string_t, string_t *);
void destroy_synchronized_symbolmap(synch_symbolmap_t *);

int parse_futures_symbol(string_t symbol, string_t * root,
                         string_t * expiration);

#endif
