/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "rex_allocator.h"
#include "hashmap.h"

#define CHAR_BIT 8


struct hashentry_s {
    char *key;
    int key_len;
    void *data;
    int len;
    int is_copy;
    struct hashentry_s *prev, *next;
};

struct hashbucket_s {
    struct hashentry_s *head, *tail;
};

struct hashmap_s {
    unsigned int size;
    iter end_iter;
    dallocator* free_entries;
    dallocator* free_keys;
    uint32_t (*hash_func) (const unsigned char* str, int str_len, uint32_t map_size);
    struct hashbucket_s *buckets;
};

uint32_t hashfuncFNV(const unsigned char *str, int str_len,
        unsigned int map_size)
{
    uint32_t h = 5381;
    uint32_t c;
    while ((c = *str++)) {
        h = (h * 293) ^ c;
    } 
    return h & (map_size - 1);
}

uint32_t hashfunc(const unsigned char *str, int str_len, unsigned int map_size)
{
    int h = 0;
    int i = 0;
    for (; i < str_len; ++i) {
        h = ((h << 5) + h) ^ str[i]; 
    }
    return h & (map_size - 1);
}

uint32_t hashfuncOAT(const unsigned char *str, int str_len,
        unsigned int map_size)
{
    const uint32_t m = 0x5bd1e995;
    const int r = 24;
    uint32_t h = map_size ^ str_len;
    const uint8_t* data = (const uint8_t*)str;
    while(str_len >= 4) {
        uint32_t k = *(uint32_t*)data;
        k *= m;
        k ^= k >> r;
        k *= m;
        h *= m;
        h ^= k;
        data += 4;
        str_len -= 4;
    }
    switch(str_len) {
        case 3: h ^= data[2] << 16;
        case 2: h ^= data[1] << 8;
        case 1: h ^= data[0];
                h *= m;
    };
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;
    return h & (map_size - 1);
}

static void init_keys(void* g)
{
    memset(g, '\0', 128);
}
static void init_entries(void* g)
{
    memset(g, '\0', sizeof(struct hashentry_s));
}
/*
 *  unsigned long hash, c;
 *  hash = 5381;
 *  while (c = *str++) {
 *    hash = ((hash << 5) + hash) + c;
 *  }
 *  return hash;
 */
/*@null@*/
hashmap_t create_map(unsigned int buckets)
{
    struct hashmap_s *ptr = 0;
    int ret = 0;
    REX_ALLOC(ptr, 8,sizeof(struct hashmap_s), ret);
    if (!ptr) {
        return NULL;
    }
    ptr->size = next_power_of_two(buckets);
    REX_ALLOC(ptr->buckets, 8, (ptr->size * sizeof(struct hashbucket_s)), ret);
    if (!ptr->buckets) {
        free(ptr);
        return NULL;
    }
    ptr->end_iter = 0;
    ptr->free_keys = create_mem_glob(128, init_keys);
    ptr->free_entries = create_mem_glob(sizeof(struct hashentry_s), init_entries);
    ptr->hash_func = hashfunc;
    return ptr;
}

hashmap_t create_map_with_hash(uint32_t buckets,
        uint32_t (*hash_func) (const unsigned char *str, int str_len, uint32_t map_size))
{
    struct hashmap_s *ptr = 0;
    int ret = 0;
    REX_ALLOC(ptr, 8,sizeof(struct hashmap_s), ret);
    if (!ptr) {
        return NULL;
    }
    ptr->size = next_power_of_two(buckets);
    REX_ALLOC(ptr->buckets, 8, (ptr->size * sizeof(struct hashbucket_s)), ret);
    if (!ptr->buckets) {
        free(ptr);
        return NULL;
    }
    ptr->end_iter = 0;
    ptr->free_keys = create_mem_glob(128, init_keys);
    ptr->free_entries = create_mem_glob(sizeof(struct hashentry_s), init_entries);
    ptr->hash_func = hash_func;
    return ptr;
}
int delete_map(hashmap_t map)
{
    if (map == NULL) {
        return 0;
    }
    destroy_mem_glob(map->free_keys);
    destroy_mem_glob(map->free_entries);
    free(map->buckets);
    free(map);
    return 0;
}

int unique_no_copy_insert(hashmap_t map, const char *key,
        int key_len, void *data)
{
    if(contains_key(map, key, key_len)) {
        return 0;
    } else {
        return no_copy_insert(map, key, key_len, data);
    }
}
int no_copy_insert(hashmap_t map, const char *key, int key_len, void *data)
{
    struct hashentry_s *ptr;
    uint32_t hash;
    char *key_copy;

    if (map == NULL || key == NULL || key_len <= 0) {
        return 0;
    }
    hash = map->hash_func((unsigned char *) key, key_len, map->size);
    key_copy = get_stuff(map->free_keys);
    if (key_copy == NULL) {
        return 0;
    }
    memset(key_copy, '\0',128);// key_len + 1);
    memcpy(key_copy, key, (key_len < 127 ? key_len: 127));
    if (!key_copy) {
        return 0;
    }
    ptr = get_stuff(map->free_entries);
    if (!ptr) {
        return_stuff(map->free_keys, key_copy);
        return 0;
    }
    ptr->key = key_copy;
    ptr->key_len = key_len;
    ptr->data = data;
    ptr->len = 4;
    ptr->is_copy = 0;
    ptr->next = NULL;
    ptr->prev = map->buckets[hash].tail;
    if (map->buckets[hash].tail) {
        map->buckets[hash].tail->next = ptr;
    }
    map->buckets[hash].tail = ptr;
    if (!map->buckets[hash].head) {
        map->buckets[hash].head = ptr;
    }
    map->end_iter++;
    return 1;
}

int insert_map(hashmap_t map, const char *key, int key_len,
        const void *data, int len)
{
    struct hashentry_s *ptr;
    uint32_t hash;
    char *key_copy;
    //int key_len;
    void *data_copy;

    if (map == NULL || key == NULL || key_len <= 0) {
        return 0;
    }
    if (!data || len < 1) {
        return 0;
    }
    hash = map->hash_func((unsigned char *) key, key_len, map->size);
    //key_len = strlen(key) + 1;
    key_copy = get_stuff(map->free_keys);
    if (key_copy == NULL) {
        return 0;
    }
    memset(key_copy, '\0', 128);
    memcpy(key_copy, key, (key_len < 128 ? key_len : 127));
    if (data) {
        data_copy = calloc(len + 1, 1);
        if (!data_copy) {
            return_stuff(map->free_keys, key_copy);
            return 0;
        }
        memcpy(data_copy, data, len);
    } else {
        data_copy = NULL;
    }
    ptr = get_stuff(map->free_entries);
    if (ptr == NULL) {
        return_stuff(map->free_keys, key_copy);
        if (data_copy) {
            free(data_copy);
        }
        return 0;
    }
    ptr->key = key_copy;
    ptr->key_len = key_len;
    ptr->data = data_copy;
    ptr->is_copy = 1;
    ptr->len = len;
    ptr->next = NULL;
    ptr->prev = map->buckets[hash].tail;
    if (map->buckets[hash].tail) {
        map->buckets[hash].tail->next = ptr;
    }
    map->buckets[hash].tail = ptr;
    if (!map->buckets[hash].head) {
        map->buckets[hash].head = ptr;
    }
    map->end_iter++;
    return 1;
}

int contains_key(hashmap_t map, const char *key, int key_len)
{
    uint32_t hash;
    struct hashentry_s *ptr;
    if (map == NULL || key == NULL || key_len <= 0)
        return 0;

    hash = map->hash_func((unsigned char *) key, key_len, map->size);

    ptr = map->buckets[hash].head;

    while (ptr) {
        if (ptr->key_len == key_len &&
                (strncmp(ptr->key, key, key_len) == 0)) {
            return 1;
        } else {
            ptr = ptr->next;
        }
    }
    return 0;
}

int get_map_entry(hashmap_t map, const char *key, int key_len, void **data)
{
    uint32_t hash;
    struct hashentry_s *ptr;

    if (map == NULL || key == NULL || key_len <= 0)
        return 0;

    hash = map->hash_func((unsigned char *) key, key_len, map->size);

    ptr = map->buckets[hash].head;

    while (ptr) {
        if (ptr->key_len == key_len &&
                (strncmp(ptr->key, key, key_len) == 0)) {
            *data = ptr->data;
            return ptr->len;
        } else {
            ptr = ptr->next;
        }
    }
    return 0;
}

int find_n_fill(hashmap_t map, const char *key, int key_len, void **data)
{
    uint32_t hash;
    struct hashentry_s *ptr;

    if (map == NULL || key == NULL || key_len <= 0)
        return 0;

    hash = map->hash_func((unsigned char *) key, key_len, map->size);

    ptr = map->buckets[hash].head;

    while (ptr) {
        if (ptr->key_len == key_len &&
                (strncmp(ptr->key, key, key_len) == 0)) {
            *data = ptr->data;
            return ptr->len;
        } else {
            ptr = ptr->next;
        }
    }
    return 0;

}

int remove_from_map(hashmap_t map, const char *key, int key_len)
{
    uint32_t hash;
    struct hashentry_s *ptr, *next;
    int count = 0;

    if (map == NULL || key == NULL || key_len <= 0)
        return 0;

    hash = map->hash_func((unsigned char *) key, key_len, map->size);

    ptr = map->buckets[hash].head;

    while (ptr) {
        if (ptr->key_len == key_len &&
                (strncmp(ptr->key, key, key_len) == 0)) {
            next = ptr->next;
            if (ptr->prev) {
                ptr->prev->next = ptr->next;
            }
            if (ptr->next) {
                ptr->next->prev = ptr->prev;
            }
            if (map->buckets[hash].head == ptr) {
                map->buckets[hash].head = ptr->next;
            }
            if (map->buckets[hash].tail == ptr) {
                map->buckets[hash].tail = ptr->prev;
            }
            return_stuff(map->free_keys, ptr->key);
            if (ptr->is_copy) {
                free(ptr->data);
            }
            ptr->key = 0;
            ptr->key_len = 0;
            ptr->data = 0;
            ptr->len = 0;
            return_stuff(map->free_entries, ptr);
            ++count;
            --map->end_iter;
            ptr = next;
            continue;
        }
        ptr = ptr->next;
    }
    return count;
}

struct hashmap_iterator {
    int bucket;
    struct hashentry_s *entry;
};

int hashmap_iterator_next(hashmap_t map, hashmap_iterator_t iter)
{
    int result = 0;
    if (map != 0 && iter != 0) {
        if (iter->entry != 0) {
            /* Find the next node in this bucket */
            iter->entry = iter->entry->next;
        }
        while (iter->entry == 0 && iter->bucket < map->size) {
            /* Find the next bucket with a node */
            ++iter->bucket;
            if(iter->bucket < map->size) {
                iter->entry = map->buckets[iter->bucket].head;
            }
        }
        if (iter->entry != 0) {
            result = 1;
        }
    }
    return result;
}

hashmap_iterator_t hashmap_iterator_begin(hashmap_t map)
{
    hashmap_iterator_t iter = 0;
    if (map != 0) {
        iter = calloc(sizeof(struct hashmap_iterator), 1);
        if (iter != 0) {
            iter->bucket = 0;
            iter->entry = map->buckets[iter->bucket].head;
            if (iter->entry == 0) {
                /* Nothing at the first entry, iterate to the next */
                if (hashmap_iterator_next(map, iter) == -1) {
                    /* Nothing in the map, return nil */
                    free(iter);
                    iter = 0;
                }
            }
        }
    }
    return iter;
}

int hashmap_iterator_dereference(hashmap_iterator_t iter,
        char **key, int *keylen, void **data)
{
    int result = 0;
    if (key != 0 && keylen != 0 && iter && iter->entry && data) {
        *key = iter->entry->key;
        *keylen = iter->entry->len;
        *data = iter->entry->data;
        result = 1;
    }
    return result;
}

void hashmap_iterator_destroy(hashmap_iterator_t iter)
{
    free(iter);
}

