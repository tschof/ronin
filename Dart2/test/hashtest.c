#include "tests.h"
#include "testmacros.h"
#include "rex_allocator.h"
#include "order_token_generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include "hashmap.h"
#include "rb_tree.h"
#include "mailer.h"

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

int test_hash(const char **testname, const char **extrareporting)
{
    STARTTEST;
    hashmap_t test = create_map(20);
    hashmap_t test2 = create_map(20);
    REQCOND(test != 0);
    char keys[20][2] =
        { "21", "22", "23", "24", "25", "26", "27", "28", "29", "10", "11",
        "12", "13", "14", "15", "16", "17", "18", "19", "20"
    };
    int i = 0;
    struct timeval tv;
    suseconds_t now = 0;
    suseconds_t before = 0;
    gettimeofday(&tv, NULL);
    before = tv.tv_usec;
    insert_map(test, "1", 2, "3", 1);
    insert_map(test, "2", 2, "3", 1);
    insert_map(test, "3", 2, "3", 1);
    insert_map(test, "4", 2, "3", 1);
    insert_map(test, "5", 2, "3", 1);
    insert_map(test, "6", 2, "3", 1);
    insert_map(test, "7", 2, "3", 1);
    insert_map(test, "8", 2, "3", 1);
    insert_map(test, "9", 2, "3", 1);
    insert_map(test, "10", 2, "3", 1);
    insert_map(test, "11", 2, "3", 1);
    insert_map(test, "12", 2, "3", 1);
    insert_map(test, "13", 2, "3", 1);
    insert_map(test, "14", 2, "3", 1);
    insert_map(test, "15", 2, "3", 1);
    insert_map(test, "16", 2, "3", 1);
    insert_map(test, "17", 2, "3", 1);
    insert_map(test, "18", 2, "3", 1);
    insert_map(test, "19", 2, "3", 1);
    insert_map(test, "20", 2, "3", 1);
    gettimeofday(&tv, NULL);
    now = tv.tv_usec;
    CHKCOND((now - before) < 20);
    gettimeofday(&tv, NULL);
    before = tv.tv_usec;
    for (; i < 20; ++i) {
        no_copy_insert(test2, keys[i], 2, &tv);
    }
    gettimeofday(&tv, NULL);
    now = tv.tv_usec;
    CHKCOND((now - before) < 10);
    delete_map(test);
    delete_map(test2);
    CHKCOND(1 == 1);
    STOPTEST;
}

struct test_master {
    char *data;
    int len;
};

#define MOD_LEN 10000

int test_modulus(const char **testname, const char **extrareporting)
{
    STARTTEST;
    hashmap_t test = create_map(1);
    REQCOND(test != 0);
    struct timeval tv;
    struct timeval one;
    struct timeval two;
    struct timeval three;
    struct timeval four;
    struct timeval five;
    struct timeval six;
    int mod_val = 0;
    int shift_val = 0;
    int oat_val = 0;
    gettimeofday(&tv, NULL);
    int i = 0;
    struct test_master m_arr[MOD_LEN];
    token_gen* tg = generic_creation_func();
    for (; i < MOD_LEN; ++i) {
        m_arr[i].data = calloc(60, 1);
        m_arr[i].len = create_multiday_all_num_token(tg, i, m_arr[i].data, 16, 6);
        //sprintf(m_arr[i].data, "PARNELL-20101215-%d", i);
       // m_arr[i].len = strlen(m_arr[i].data);
    }
    gettimeofday(&three, NULL);
    for (i = 0; i < MOD_LEN; ++i) {
        hashfunc((const unsigned char *) m_arr[i].data, m_arr[i].len,
                 1024);
    }
    gettimeofday(&four, NULL);
    gettimeofday(&five, NULL);
    for (i = 0; i < MOD_LEN; ++i) {
        hashfuncOAT((const unsigned char *) m_arr[i].data, m_arr[i].len,
                    1024);
    }
    gettimeofday(&six, NULL);
    gettimeofday(&one, NULL);
    for (i = 0; i < MOD_LEN; ++i) {
        hashfuncFNV((const unsigned char *) m_arr[i].data, m_arr[i].len,
                    1024);
    }
    gettimeofday(&two, NULL);
    mod_val =
        ((two.tv_sec - one.tv_sec) * 1000000) + (two.tv_usec -
                                                 one.tv_usec);
    shift_val =
        ((four.tv_sec - three.tv_sec) * 1000000) + (four.tv_usec -
                                                    three.tv_usec);
    oat_val =
        ((six.tv_sec - five.tv_sec) * 1000000) + (six.tv_usec -
                                                  five.tv_usec);
    printf("Mod time: %d Shift time: %d OAT time: %d \n", mod_val,
           shift_val, oat_val);
    for (i = 0; i < MOD_LEN; ++i) {
        free(m_arr[i].data);
    }
    CHKCOND(mod_val > shift_val);
    STOPTEST;
}

int test_integer_hashes(const char** testname, const char** extrareporting)
{
    STARTTEST;
    hashmap_t unique_ints_6432 = create_map(4056);
    hashmap_t unique_ints_murmur = create_map(4056);
    REQCOND(unique_ints_6432 != 0);
    REQCOND(unique_ints_murmur != 0);
    char value[256];
    memset(value, '\0', 256);
    void* v = 0x0;
    uint64_t i = 0;
    uint64_t j = 13;
    int cm = 0;
    int c6432 = 0;
    for(;i < 4056; ++i) {
        uint64_t k = j * i;
        uint32_t val1 = hash6432((const unsigned char*) &k, sizeof(uint64_t),
                4056);
        int ret_val = unique_no_copy_insert(unique_ints_6432, (const char*)&val1,
                sizeof(uint32_t), v);
        if(ret_val == 0) {++c6432;}
        uint32_t val2 = murmerhash((const unsigned char*) &k, sizeof(uint64_t),
                4056);
        ret_val = unique_no_copy_insert(unique_ints_murmur, (const char *)&val2,
                sizeof(uint32_t), v);
        if(ret_val == 0) {++cm;}
    }
    printf("Collisions for 6432: %d, for murmur: %d \n", c6432, cm);
    CHKCOND(cm >= 0);
    CHKCOND(c6432 >= 0);
    STOPTEST;
}

uint64_t test_a_hash(hashmap_t map, int sandit, char* name) 
{
    uint64_t i = 0;
    uint64_t j = 13;
    struct timeval tv;
    struct timeval* lv;
    uint64_t size_from = 0;
    /*suseconds_t now = 0;
    suseconds_t before = 0;*/
    /*6432 test*/
    gettimeofday(&tv, NULL);
    //before = tv.tv_usec;
    for(;i < sandit; ++i) {
        uint64_t k = j * i;
        no_copy_insert(map, (const char*)&k,
                sizeof(uint64_t), &tv);
    }
    gettimeofday(&tv, NULL);
    //now = tv.tv_usec;
    i = 0;
    j = 13;
    gettimeofday(&tv, NULL);
    //before = tv.tv_usec;
    for(;i < sandit; ++i) {
        uint64_t k = j * i;
        size_from += get_map_entry(map, (const char*)&k,
                sizeof(uint64_t), (void**)&lv);
    }
    return size_from;
}

void test_a_tree(int sandit, rb_tree_t *test)
{
    int t = 1;
    int i = 0;
    int j = 13;
    for (; i < sandit; ++i) {
        uint64_t k = j * i;
        rb_tree_nd64_insert(test, &k, &t);
    }
    i = 0;
    j = 13;
    for(;i < sandit; ++i) {
        uint64_t k = j * i;
         t +=  *((int *) rb_tree_get_val(test, &k));
    }
}
int sockMerchant(int n, int ar_count, int* ar)
{
    int i, j, t;int ret = 0;
    for(i = ar_count; i > 0; --i) {
        for(j = 1; j < i; ++j) {
            if(ar[j - 1] > ar[j]) {
                t = ar[j -1];
                ar[j - 1] = ar[j];
                ar[j] = t;
            }
        }
    }
    i = 0;
    j = 0;
    t = 0;
    while(i < n) {
        while(ar[j] == ar[i]) {
            ++t;
            ++i;
        }
        ret += t/2;
        t = 0;
        j = i;
    }
    return ret;
}
int test_tree_vs_hash(const char** testname, const char** extrareporting)
{
    STARTTEST;

int mailStart = start_dart_mail_service("west.smtp.mx.exch028.serverdata.net",
                            35, "parnellflynn@gmail.com",
                            19,
                            "parnellflynn@gmail.com",
                            19, 22,
                            22, 7 );
mailStart = send_biz_message("Test Mess", 9,
                     "IMAP TEST", 9);
printf("Was the mail started: %d, \n", mailStart);
    int sandit = 16384;
    int outer = 500;
    int i = 0;
    int t1[9] = {1,2,3,4,5,6,7,8,9};
    int res = sockMerchant(9,9,t1);
    CHKCOND(res == 0);
    int t2[15] = {9,8,3,4,5,6,7,8,9,1,3,5,2,3,4};
    res = sockMerchant(15,15,t2);
    CHKCOND(res == 4);
    /*Murmur test*/
    for(;i < outer; ++i) { 
        hashmap_t unique_ints_murmur = create_map_with_hash(sandit, murmerhash);
        REQCOND(unique_ints_murmur != 0);
        uint64_t size_from = test_a_hash(unique_ints_murmur, sandit, "murmur");
        CHKCOND(size_from == (sandit * sizeof(char*)));
        delete_map(unique_ints_murmur);
    }
    i = 0;
    /*Start 6432*/
    for(;i < outer; ++i) { 
        hashmap_t unique_ints_6432 = create_map_with_hash(sandit, hash6432);
        REQCOND(unique_ints_6432 != 0);
        uint64_t size_from = test_a_hash(unique_ints_6432, sandit, "6432");
        CHKCOND(size_from == (sandit * sizeof(char*)));
        delete_map(unique_ints_6432);
    }
    /*END 6432*/
    i = 0;
    /*Murmur test*/
    for(;i < outer; ++i) { 
        hashmap_t unique_ints_murmur = create_map_with_hash(sandit, murmerhash);
        REQCOND(unique_ints_murmur != 0);
        uint64_t size_from = test_a_hash(unique_ints_murmur, sandit, "murmur");
        CHKCOND(size_from == (sandit * sizeof(char*)));
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
        REQCOND(unique_ints_6432 != 0);
        uint64_t size_from = test_a_hash(unique_ints_6432, sandit, "6432");
        CHKCOND(size_from == (sandit * sizeof(char*)));
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
    STOPTEST;
}

int test_next_pow_two(const char **testname, const char **extrareporting)
{
    STARTTEST;
    hashmap_t test = create_map(1);
    REQCOND(test != 0);
    unsigned int ret = 0;
    int size = 1;
    ret = next_power_of_two(size);
    CHKCOND(ret == 1);
    size = 2;
    ret = next_power_of_two(size);
    CHKCOND(ret == 2);
    size = 1000;
    ret = next_power_of_two(size);
    CHKCOND(ret == 1024);
    size = 80000;
    ret = next_power_of_two(size);
    CHKCOND(ret == 131072);
    size = 50000;
    ret = next_power_of_two(size);
    CHKCOND(ret == 65536);
    STOPTEST;
}

int test_small_bucket(const char **testname, const char **extrareporting)
{
    STARTTEST;
    hashmap_t test = create_map(1);
    REQCOND(test != 0);
    char *data = "Duh";
    int i = 0;
    int j = 1;
    char *test_key = calloc(50, 1);
    size_t insert_ret = 0;
    int get_ret = 0;
    for (; i < 50; ++i) {
        sprintf(test_key, "PARNELL-20101215-%d", i);
        insert_ret =
            no_copy_insert(test, test_key, (strlen(test_key)), data);
        CHKCOND(insert_ret > 0);
    }
    i = 0;
    for (; i < 50; ++i) {
        sprintf(test_key, "PARNELL-20101215-%d", i);
        get_ret = get_map_entry(test, test_key, (strlen(test_key)),
                                (void **) &data);
        CHKCOND(get_ret > 0);
    }
    i = 0;
    for (; i < 50; ++i) {
        sprintf(test_key, "PARNELL-20101215-%d", i);
        get_ret = remove_from_map(test, test_key, (strlen(test_key)));
        CHKCOND(get_ret > 0);
        for (j = (i + 1); j < 49; ++j) {
            sprintf(test_key, "PARNELL-20101215-%d", j);
            get_ret = get_map_entry(test, test_key, (strlen(test_key)),
                                    (void **) &data);
            CHKCOND(get_ret > 0);
            if (get_ret <= 0) {
                printf("%s i= %d, j= %d\n", test_key, i, j);
            }
        }
        sprintf(test_key, "PARNELL-20101215-%d", i);
        get_ret = get_map_entry(test, test_key, (strlen(test_key)),
                                (void **) &data);
        CHKCOND(get_ret == 0);

    }
    delete_map(test);
    STOPTEST;
}

int test_cx_cross_hash(const char **testname, const char **extrareporting)
{
    STARTTEST;
    hashmap_t test = create_map(20000);
    REQCOND(test != 0);
    size_t h1 =
        hashfunc((unsigned char *) "FXMAN8-90110518-1415601", 23, 20000);
    size_t h2 =
        hashfunc((unsigned char *) "FXMAN8-90110518-1415594", 23, 20000);
    size_t h6 =
        hashfunc((unsigned char *) "FXMAN8-90110518-1415589", 23, 20000);

    size_t h5 =
        hashfunc((unsigned char *) "FXMAN8-90110518-1415590", 23, 20000);
    size_t h3 =
        hashfunc((unsigned char *) "FXMAN8-90110518-1415576", 23, 20000);
    size_t h4 =
        hashfunc((unsigned char *) "FXMAN8-90110518-1415577", 23, 20000);
    size_t h7 =
        hashfunc((unsigned char *) "FXMAN8-90110518-1415581", 23, 20000);
    size_t h8 =
        hashfunc((unsigned char *) "FXMAN8-90110518-1415582", 23, 20000);
    size_t h9 =
        hashfunc((unsigned char *) "FXMAN8-90110518-1415530", 23, 20000);
    size_t h10 =
        hashfunc((unsigned char *) "FXMAN8-90110518-1415528", 23, 20000);
    size_t h11 = hashfuncFNV((unsigned char *) "553426187", 10, 50000);
    size_t h12 = hashfuncFNV((unsigned char *) "553426188", 10, 50000);
    size_t h13 =
        hashfuncFNV((unsigned char *) "MM8N-60110810-668220", 20, 50000);
    size_t h14 =
        hashfuncFNV((unsigned char *) "MM8F-60110810-668218", 20, 50000);
    size_t h15 =
        hashfuncFNV((unsigned char *) "MM8D-60110810-672000", 20, 50000);
    size_t h16 =
        hashfuncFNV((unsigned char *) "MM8N-60110810-672060", 20, 50000);
    size_t h17 =
        hashfuncFNV((unsigned char *) "MM8N-60110810-670034", 20, 50000);
    size_t h18 =
        hashfuncFNV((unsigned char *) "MM8F-60110810-670037", 20, 50000);
    CHKCOND(h1 != h2);
    CHKCOND(h7 != h8);
    CHKCOND(h3 != h4);
    CHKCOND(h5 != h6);
    CHKCOND(h1 != h2);
    CHKCOND(h10 != h9);
    CHKCOND(h11 != h12);
    CHKCOND(h13 != h14);
    CHKCOND(h15 != h16);
    CHKCOND(h17 != h18);
    delete_map(test);
    STOPTEST;
}

struct mult_thread_test {
    hashmap_t test;
    char *key;
    int key_len;
    char *value;
    int val_len;
};

void *hash_thread_tester2(void *arg)
{
    struct mult_thread_test *mtl = (struct mult_thread_test *) arg;
    int i = 0;
    for (; i < 200000; ++i) {
        remove_from_map(mtl->test, mtl->key, mtl->key_len);
        no_copy_insert(mtl->test, mtl->key, mtl->key_len, mtl->value);
    }
    return NULL;

}

void *hash_thread_tester(void *arg)
{
    struct mult_thread_test *mtl = (struct mult_thread_test *) arg;
    int i = 0;
    int res = 0;
    char *val = 0;
    for (; i < 200000; ++i) {
        res =
            get_map_entry(mtl->test, mtl->key, mtl->key_len,
                          (void *) &val);
        if (res == 0) {
            printf("Failed to find entry: %s\n", mtl->key);
        } else if (strcmp(val, mtl->value) != 0) {
            printf("No Match: %s %s \n", mtl->value, val);
        }
    }
    return NULL;
}

int test_mult_thread_access(const char **testname,
                            const char **extrareporting)
{
    STARTTEST;
    hashmap_t test = create_map(1);
    REQCOND(test != 0);
    pthread_t num1;
    pthread_t num2;
    char *key1 = "FXMAN8-90110518-1415576";
    int key1_len = 23;
    char *key2 = "FXMAN8-90110518-1415577";
    int key2_len = 23;
    char *val1 = "VALUE_ONE";
    int val1_len = 9;
    char *val2 = "VALUE_TWO";
    int val2_len = 9;
    void *result;
    struct mult_thread_test *mt1 = malloc(sizeof(struct mult_thread_test));
    struct mult_thread_test *mt2 = malloc(sizeof(struct mult_thread_test));
    no_copy_insert(test, key1, key1_len, val1);
    no_copy_insert(test, key2, key2_len, val2);
    mt1->key = key1;
    mt1->key_len = key1_len;
    mt1->value = val1;
    mt1->val_len = val1_len;
    mt1->test = test;
    pthread_create(&num1, NULL, &hash_thread_tester, mt1);
    mt2->key = key2;
    mt2->key_len = key2_len;
    mt2->value = val2;
    mt2->val_len = val2_len;
    mt2->test = test;
    pthread_create(&num2, NULL, &hash_thread_tester2, mt2);
    sleep(2);
    pthread_join(num1, &result);
    pthread_join(num2, &result);
    delete_map(test);
    CHKCOND(1 == 1);
    STOPTEST;
}

int test_contains_key(const char **testname, const char **extrareporting)
{
    STARTTEST;
    hashmap_t test = create_map(1);
    REQCOND(test != 0);
    char *data = "Duh";
    int i = 0;
    int j = 1;
    char *test_key = calloc(50, 1);
    size_t insert_ret = 0;
    int get_ret = 0;
    for (; i < 50; ++i) {
        sprintf(test_key, "PARNELL-20101215-%d", i);
        insert_ret =
            no_copy_insert(test, test_key, (strlen(test_key)), data);
        CHKCOND(insert_ret > 0);
    }
    i = 0;
    for (; i < 50; ++i) {
        sprintf(test_key, "PARNELL-20101215-%d", i);
        get_ret = find_n_fill(test, test_key, (strlen(test_key)),
                              (void **) &data);
        CHKCOND(get_ret > 0);
    }
    i = 0;
    for (; i < 50; ++i) {
        sprintf(test_key, "PARNELL-20101215-%d", i);
        get_ret = remove_from_map(test, test_key, (strlen(test_key)));
        CHKCOND(get_ret > 0);
        for (j = (i + 1); j < 49; ++j) {
            sprintf(test_key, "PARNELL-20101215-%d", j);
            get_ret = contains_key(test, test_key, (strlen(test_key)));
            CHKCOND(get_ret > 0);
            if (get_ret <= 0) {
                printf("%s i= %d, j= %d\n", test_key, i, j);
            }
        }
        sprintf(test_key, "PARNELL-20101215-%d", i);
        get_ret = contains_key(test, test_key, (strlen(test_key)));
        CHKCOND(get_ret == 0);

    }
    delete_map(test);
    CHKCOND(1 == 1);
    STOPTEST;
}
