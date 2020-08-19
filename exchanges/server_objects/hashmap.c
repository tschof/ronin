/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdlib.h>
#include <string.h>
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
    struct hashbucket_s *buckets;
};

int hashfuncFNV(const unsigned char *str, int str_len,
                unsigned int map_size)
{
    int h = 0;
    int i = 0;
    for (; i < str_len; ++i) {  //while(*str) {
        h = ((h << 5) + h) ^ str[i];    //+ *str++;
    }
    return h % map_size;
}

unsigned int next_power_of_two(unsigned int size)
{
    int i;
    if (size == 0) {
        return 1;
    }
    --size;

    for (i = 1; i < sizeof(unsigned int) * CHAR_BIT; i <<= 1) {
        size = size | size >> i;
    }
    return size + 1;
}

int hashfunc(const unsigned char *str, int str_len, unsigned int map_size)
{
    int h = 0;
    int i = 0;
    for (; i < str_len; ++i) {  //while(*str) {
        h = ((h << 5) + h) ^ str[i];    //+ *str++;
    }
    return h & (map_size - 1);
}

int hashfuncOAT(const unsigned char *str, int str_len,
                unsigned int map_size)
{
    unsigned h = 0;
    int i;
    for (i = 0; i < str_len; ++i) {
        h = (h << 4) ^ (h >> 28) ^ str[i];
    }
    return h & (map_size - 1);
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
    struct hashmap_s *ptr;

    ptr = calloc(1, sizeof(struct hashmap_s));
    if (!ptr) {
        return NULL;
    }
    ptr->size = next_power_of_two(buckets);
    ptr->buckets = calloc(ptr->size, sizeof(struct hashbucket_s));
    if (!ptr->buckets) {
        free(ptr);
        return NULL;
    }
    ptr->end_iter = 0;
    return ptr;
}

static void delete_bucket(struct hashbucket_s *bucket)
{
    struct hashentry_s *nextptr;
    struct hashentry_s *ptr;
    if (bucket == NULL || bucket->head == NULL)
        return;

    ptr = bucket->head;
    while (ptr) {
        nextptr = ptr->next;
        free(ptr->key);
        if (ptr->is_copy) {
            free(ptr->data);
        }
        free(ptr);
        ptr = nextptr;
    }
    return;
}

int delete_map(hashmap_t map)
{
    int i;
    if (map == NULL) {
        return 0;
    }
    for (i = 0; i != map->size; ++i) {
        if (map->buckets[i].head != NULL) {
            delete_bucket(&map->buckets[i]);
        }
    }
    free(map->buckets);
    free(map);
    return 0;
}

int no_copy_insert(hashmap_t map, const char *key, int key_len, void *data)
{
    struct hashentry_s *ptr;
    int hash;
    char *key_copy;
    if (map == NULL || key == NULL) {
        return 0;
    }
    hash = hashfunc((unsigned char *) key, key_len, map->size);
    key_copy = malloc(key_len + 1);
    if (key_copy == NULL) {
        return 0;
    }
    memset(key_copy, '\0', key_len + 1);
    memcpy(key_copy, key, key_len);
    if (!key_copy) {
        return 0;
    }
    ptr = malloc(sizeof(struct hashentry_s));
    if (!ptr) {
        free(key_copy);
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
    unsigned long hash;
    char *key_copy;
    void *data_copy;

    if (map == NULL || key == NULL) {
        return 0;
    }
    if (!data || len < 1) {
        return 0;
    }
    hash = hashfunc((unsigned char *) key, key_len, map->size);
    //key_len = strlen(key) + 1;
    key_copy = malloc(key_len + 1);
    if (key_copy == NULL) {
        return 0;
    }
    memset(key_copy, '\0', key_len + 1);
    memcpy(key_copy, key, key_len);
    if (data) {
        data_copy = malloc(len);
        if (!data_copy) {
            free(key_copy);
            return 0;
        }
        memcpy(data_copy, data, len);
    } else {
        data_copy = NULL;
    }
    ptr = malloc(sizeof(struct hashentry_s));
    if (ptr == NULL) {
        free(key_copy);
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
    int hash;
    struct hashentry_s *ptr;
    if (map == NULL || key == NULL)
        return 0;

    hash = hashfunc((unsigned char *) key, key_len, map->size);

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
    int hash;
    struct hashentry_s *ptr;

    if (map == NULL || key == NULL)
        return 0;

    hash = hashfunc((unsigned char *) key, key_len, map->size);

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
    int hash;
    struct hashentry_s *ptr;

    if (map == NULL || key == NULL)
        return 0;

    hash = hashfunc((unsigned char *) key, key_len, map->size);

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
    int hash;
    struct hashentry_s *ptr, *next;
    int count = 0;

    if (map == NULL || key == NULL)
        return 0;

    hash = hashfunc((unsigned char *) key, key_len, map->size);

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
            free(ptr->key);
            if (ptr->is_copy) {
                free(ptr->data);
            }
            free(ptr);
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
            iter->entry = map->buckets[iter->bucket].head;
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
        iter = malloc(sizeof(struct hashmap_iterator));
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

int hashmap_iterator_dereference(hashmap_t map, hashmap_iterator_t iter,
                                 char **key, int *keylen, void** data)
{
    int result = 0;
    if (key != 0 && keylen != 0) {
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
