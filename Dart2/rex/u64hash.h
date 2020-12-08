/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _U64HASH__H__
#define _U64HASH__H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>

    typedef struct u64hashmap *u64map;
    typedef int iter;

    hashmap_t create_u64map(uint32_t buckets);
    hashmap_t create_u64map_with_hash(unsigned int buckets,
        uint64_t (*hash_func) (const uint64_t *str, uint32_t map_size));

    int16_t delete_u64map(hashmap_t map);

    int16_t insert_u64map(hashmap_t map, uint64_t key,
            const void *data, int len);

    int16_t no_copy_insertu64(hashmap_t map, uint64_t key,
            void *data);

    int16_t unique_no_copy_insertu64(hashmap_t map, uint64_t key,
            void *data);
    int16_t get_u64map_entry(hashmap_t map, uint64_t key,
            void **data);

    int16_t remove_from_u64map(hashmap_t map, uint64_t key);

    int16_t contains_u64key(hashmap_t map, uint64_t key);


    uint64_t u64hashfunc(const uint64_t str,
            uint64_t map_size);
    /* This is an interface for hashmap iterators.  They should only be used
       for debug purposes as the iteration of a hashmap is actually quite
       inefficient as it needs to iterate over a large number of possibly
       empty buckets in order to find the keys. */
    typedef struct hashmap_iterator *hashmap_iterator_t;
    hashmap_iterator_t hashmap_iterator_begin(hashmap_t map);
    int hashmap_iterator_next(hashmap_t map, hashmap_iterator_t iter);
    int hashmap_iterator_dereference(
            hashmap_iterator_t iter, char **key,
            int *keylen, void **data);
    void hashmap_iterator_destroy(hashmap_iterator_t iter);

#if defined(__cplusplus)
}
#endif
#endif                          //_HASH_MAP_H__

