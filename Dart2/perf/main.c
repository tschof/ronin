#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include "hashmap.h"
#include "rb_tree.h"


static int hu64_compare(const void* a, const void* b)
{
    if (*(uint64_t *) a > *(uint64_t *) b) {
        return 1;
    }
    if (*(uint64_t *) a < *(uint64_t *) b) {
        return -1;
    }
    return 0;
}

void hu64_print(const void* a)
{
    printf("%lu", *(uint64_t*)a);
}

void hint_print(const void *a)
{
    printf("%d", *(int *) a);
}

void hinfo_print(const void *a)
{
    printf("%s", (char *) a);
}

void htest_info_destroy(void *a)
{
    memcpy(a, "This value deleted", 18);
}

void hinfo_destroy(void *a)
{
}

void test_a_hash(hashmap_t map, int sandit, char* name) 
{
    uint64_t i = 0;
    uint64_t j = 13;
    struct timeval tv;
    struct timeval* lv;
    uint64_t size_from = 0;
    suseconds_t now = 0;
    suseconds_t before = 0;
    /*6432 test*/
    gettimeofday(&tv, NULL);
    before = tv.tv_usec;
    for(;i < sandit; ++i) {
        uint64_t k = j * i;
        no_copy_insert(map, (const char*)&k,
                sizeof(uint64_t), &tv);
    }
    gettimeofday(&tv, NULL);
    now = tv.tv_usec;
    printf("Insert for %s: %ld \n",name, now - before);
    i = 0;
    j = 13;
    gettimeofday(&tv, NULL);
    before = tv.tv_usec;
    for(;i < sandit; ++i) {
        uint64_t k = j * i;
        size_from += get_map_entry(map, (const char*)&k,
                sizeof(uint64_t), (void**)&lv);
    }
    gettimeofday(&tv, NULL);
    now = tv.tv_usec;
    printf("Get_map_val for %s: %ld \n",name, now - before);
}

void test_a_tree(int sandit, rb_tree_t *test)
{
    struct timeval tv;
    suseconds_t now = 0;
    suseconds_t before = 0;
    int t = 1;
    int i = 0;
    int j = 13;
    gettimeofday(&tv, NULL);
    before = tv.tv_usec;
    for (; i < sandit; ++i) {
        uint64_t k = j * i;
        rb_tree_nd64_insert(test, &k, &t);
    }
    gettimeofday(&tv, NULL);
    now = tv.tv_usec;
    printf("Insert for tree: %ld \n", now - before);
    i = 0;
    j = 13;
    gettimeofday(&tv, NULL);
    before = tv.tv_usec;
    for(;i < sandit; ++i) {
        uint64_t k = j * i;
         t +=  *((int *) rb_tree_get_val(test, &k));
    }
    gettimeofday(&tv, NULL);
    now = tv.tv_usec;
    printf("Get_map_val for tree: %ld \n", now - before);
}

int main(int argc, char *argv[])
{
    int ok = 1;
    int sandit = 16384;
    int outer = 500;
    int i = 0;
    /*Murmur test*/
    for(;i < outer; ++i) { 
        hashmap_t unique_ints_murmur = create_map_with_hash(sandit, murmerhash);
        test_a_hash(unique_ints_murmur, sandit, "murmur");
        delete_map(unique_ints_murmur);
    }
    i = 0;
    /*Start 6432*/
    for(;i < outer; ++i) { 
        hashmap_t unique_ints_6432 = create_map_with_hash(sandit, hash6432);
        test_a_hash(unique_ints_6432, sandit, "6432");
        delete_map(unique_ints_6432);
    }
    /*END 6432*/
    i = 0;
    /*Murmur test*/
    for(;i < outer; ++i) { 
        hashmap_t unique_ints_murmur = create_map_with_hash(sandit, murmerhash);
        test_a_hash(unique_ints_murmur, sandit, "murmur");
        delete_map(unique_ints_murmur);
    }
    /*End Murmur*/
    i = 0;
    /*Start Tree*/
    for(;i < outer; ++i) { 
        rb_tree_t *test =
            create_rb_tree(hu64_compare, NULL, hinfo_destroy, hu64_print,
                           hinfo_print);
        test_a_tree(sandit, test);
        rb_tree_destroy(test);
    }
    i = 0;
    /*Start 6432*/
    for(;i < outer; ++i) { 
        hashmap_t unique_ints_6432 = create_map_with_hash(sandit, hash6432);
        test_a_hash(unique_ints_6432, sandit, "6432");
        delete_map(unique_ints_6432);
    }
    i = 0;
    /*Start Tree*/
    for(;i < outer; ++i) { 
        rb_tree_t *test =
            create_rb_tree(hu64_compare, NULL, hinfo_destroy, hu64_print,
                           hinfo_print);
        test_a_tree(sandit, test);
        rb_tree_destroy(test);
    }
    return ok;
}

