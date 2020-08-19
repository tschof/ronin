/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _HASH_MAP_H__
#define _HASH_MAP_H__

#if defined(__cplusplus)
extern "C" {
#endif

    typedef struct hashmap_s *hashmap_t;

    typedef int iter;

    hashmap_t create_map(unsigned int buckets);

    int delete_map(hashmap_t map);

    int insert_map(hashmap_t map, const char *key,
                   int key_len, const void *data, int len);

    int no_copy_insert(hashmap_t map, const char *key,
                       int key_len, void *data);

    int get_map_entry(hashmap_t map, const char *key,
                      int key_len, void **data);

    int find_n_fill(hashmap_t map, const char *key,
                    int key_len, void **data);

    int remove_from_map(hashmap_t map, const char *key, int key_len);

    int contains_key(hashmap_t map, const char *key, int key_len);

    int hashfunc(const unsigned char *str, int str_len,
                 unsigned int map_size);

    int hashfuncFNV(const unsigned char *str, int str_len,
                    unsigned int map_size);

    int hashfuncOAT(const unsigned char *str, int str_len,
                    unsigned int map_size);

    unsigned int next_power_of_two(unsigned int size);

    /* This is an interface for hashmap iterators.  They should only
       be used for debug purposes as the iteration of a hashmap is
       actually quite inefficient as it needs to iterate over a large
       number of possibly empty buckets in order to find the keys. */
    typedef struct hashmap_iterator *hashmap_iterator_t;

    hashmap_iterator_t hashmap_iterator_begin(hashmap_t map);

    int hashmap_iterator_next(hashmap_t map, hashmap_iterator_t iter);

    int hashmap_iterator_dereference(hashmap_t map,
                                     hashmap_iterator_t iter, char **key,
                                     int *keylen, void** data);

    void hashmap_iterator_destroy(hashmap_iterator_t iter);

#if defined(__cplusplus)
}
#endif
#endif                          //_HASH_MAP_H__
