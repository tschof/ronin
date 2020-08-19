#include "util.h"

void free_string_map(hashmap_t map)
{
    if (map == 0) {
        return;
    }
    char *key;
    int len;
    void *data;
    hashmap_iterator_t iter = hashmap_iterator_begin(map);
    while (hashmap_iterator_next(map, iter)) {
        if (hashmap_iterator_dereference(iter, &key, &len, &data)) {
            free(key);
            free(data);
        }
    }
    delete_map(map);
}

void free_map_of_maps_to_string(hashmap_t map)
{
    if (map == 0) {
        return;
    }
    char *key;
    int len;
    void *data;
    hashmap_iterator_t iter = hashmap_iterator_begin(map);
    while (hashmap_iterator_next(map, iter)) {
        if (hashmap_iterator_dereference(iter, &key, &len, &data)) {
            free(key);
            hashmap_t submap = data;
            free_string_map(submap);
        }
    }
    delete_map(map);
}
