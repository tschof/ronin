#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "tests.h"
#include "testmacros.h"
#include "rb_tree.h"
#include "sti_tree.h"

static void int_destroy(void *a)
{
    free((int *) a);
}
static int int_compare(const void *a, const void *b)
{
    if (*(int *) a > *(int *) b) {
        return 1;
    }
    if (*(int *) a < *(int *) b) {
        return -1;
    }
    return 0;
}

static int u64_compare(const void* a, const void* b)
{
    if (*(uint64_t *) a > *(uint64_t *) b) {
        return 1;
    }
    if (*(uint64_t *) a < *(uint64_t *) b) {
        return -1;
    }
    return 0;
}

void u64_print(const void* a)
{
    printf("%lu", *(uint64_t*)a);
}

void int_print(const void *a)
{
    printf("%d", *(int *) a);
}

void info_print(const void *a)
{
    printf("%s", (char *) a);
}

void test_info_destroy(void *a)
{
    memcpy(a, "This value deleted", 18);
}

void info_destroy(void *a)
{
    free(a);
}

void tree_inserter(rb_tree_t * t, int key, void *value)
{
    int *one = malloc(sizeof(int));
    *one = key;
    rb_tree_insert(t, one, value);
}

int test_rb_masher(const char **testname, const char **extrareporting)
{
    STARTTEST;

    rb_tree_t *test =
        create_rb_tree(int_compare, int_destroy, info_destroy, int_print,
                       info_print);
    REQCOND(test != 0);
    int i = 1;
    char *checker = 0;
    for (; i < 100000; ++i) {
        char *def = calloc(30, 1);
        sprintf(def, "%d", i);
        tree_inserter(test, i, def);
    }
    i = 1;
    int j = 100001;
    for (; i < 100000; ++i) {
        char *d = calloc(30, 1);
        sprintf(d, "%d", j);
        tree_inserter(test, j, d);
        ++j;
        checker = (char *) rb_tree_get_val(test, &i);
        CHKCOND(checker != NULL);
        if (checker) {
            CHKCOND(i == (atoi(checker)));
            rb_tree_remove(test, &i);
            checker = (char *) rb_tree_get_val(test, &i);
            CHKCOND(checker == NULL);
        } else {
            printf("NULL CHECKER: %d \n", i);
        }
    }
    i = 100001;
    for (; i < 200000; ++i) {
        checker = (char *) rb_tree_get_val(test, &i);
        CHKCOND(checker != NULL);
        if (checker) {
            CHKCOND(i == (atoi(checker)));
            rb_tree_remove(test, &i);
            checker = (char *) rb_tree_get_val(test, &i);
            CHKCOND(checker == NULL);
        } else {
            printf("NULL CHECKER: %d \n", i);
        }
    }
    rb_tree_destroy(test);
    CHKCOND(1 == 1);
    STOPTEST;
}

int test_sti_masher(const char** testname, const char** extrareporting)
{
    STARTTEST;
    void* mem = malloc(824);
    REQCOND(mem != 0);
    sti_tree_t* test = create_sti_tree(mem, 824);
    int i = 1;
    int j = 19;
    for(;i < 19;++i, --j) {
        sti_node_t* l = sti_tree_insert(test, i, j);
        CHKCOND(l != 0);
    }
    i = 1;
    j = 19;
    for(;i < 19;++i, --j) {
        short v = sti_tree_get_val(test, i);
        CHKCOND(v == j);
        sti_tree_remove(test, i);
    }
    i =1;
    for(;i < 19; ++i) {
        short v = sti_tree_get_val(test, i);
        CHKCOND(v == -1);
    }
    free(mem);
    STOPTEST;
}

int test_rb_remove(const char **testname, const char **extrareporting)
{
    STARTTEST;

    rb_tree_t *test =
        create_rb_tree(int_compare, int_destroy, test_info_destroy,
                       int_print, info_print);
    REQCOND(test != 0);
    int *one = malloc(sizeof(int));
    int *two = malloc(sizeof(int));
    int *three = malloc(sizeof(int));
    char *val_one = calloc(20, 1);
    char *val_two = calloc(20, 1);
    char *val_three = calloc(20, 1);
    *one = 1;
    *two = 2;
    *three = 3;
    memcpy(val_one, "VAL ONE #1", 10);
    memcpy(val_two, "VAL TWO #2", 10);
    memcpy(val_three, "VAL TRE #3", 10);
    rb_tree_insert(test, one, val_one);
    int test_getter = 1;
    char *dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo != 0);
    rb_tree_insert(test, two, val_two);
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo != 0);
    CHKCOND(strcmp(dodo, val_one) == 0);
    rb_tree_print(test);
    rb_tree_remove(test, &test_getter);
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo == 0);
    CHKCOND(strcmp(val_one, "This value deleted") != 0);
    rb_tree_no_delete_insert(test, three, val_three);
    test_getter = 3;
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo != 0);
    CHKCOND(strcmp(dodo, val_three) == 0);
    rb_tree_delete(test, &test_getter);
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo == 0);
    CHKCOND(strcmp(val_three, "This value deleted") != 0);
    rb_tree_destroy(test);
    STOPTEST;
}

int test_rb_insert(const char **testname, const char **extrareporting)
{
    STARTTEST;
    rb_tree_t *test =
        create_rb_tree(int_compare, int_destroy, test_info_destroy,
                       int_print, info_print);
    REQCOND(test != 0);
    int *one = malloc(sizeof(int));
    int *two = malloc(sizeof(int));
    char *val_one = calloc(20, 1);
    char *val_two = calloc(20, 1);
    *one = 1;
    *two = 2;
    memcpy(val_one, "VAL ONE #1", 10);
    memcpy(val_two, "VAL TWO #2", 10);
    rb_tree_insert(test, one, val_one);
    int test_getter = 1;
    char *dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo != 0);
    rb_tree_insert(test, two, val_two);
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo != 0);
    CHKCOND(strcmp(dodo, val_one) == 0);
    rb_tree_print(test);
    rb_tree_delete(test, &test_getter);
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo == 0);
    CHKCOND(strcmp(val_one, "This value deleted") == 0);
    rb_tree_destroy(test);
    STOPTEST;
}

int test_rb_key_glob(const char** testname,
        const char** extrareporting)
{
    STARTTEST;
    rb_tree_t *test =
        create_rb_tree(u64_compare, NULL, info_destroy, u64_print,
                       info_print);
    REQCOND(test != 0);
    char *val1 = calloc(20, 1);
    char *val2 = calloc(20, 1);
    char *val3 = calloc(20, 1);
    char *val4 = calloc(20, 1);
    char *val5 = calloc(20, 1);
    char *val6 = calloc(20, 1);
    memcpy(val1, "TVF ONE 1", 9);
    memcpy(val2, "TMV TWO 2", 9);
    memcpy(val3, "BllkTHR 3", 9);
    memcpy(val4, "uuLHFOR 4", 9);
    memcpy(val5, "FIVEFIV 5", 9);
    memcpy(val6, "Wehaveva6", 9);
    uint64_t ins = 1;
    rb_tree_nd64_insert(test, &ins, val1);
    ++ins;
    rb_tree_nd64_insert(test, &ins, val2);
    ++ins;
    rb_tree_nd64_insert(test, &ins, val3);
    ++ins;
    rb_tree_nd64_insert(test, &ins, val4);
    ++ins;
    rb_tree_nd64_insert(test, &ins, val5);
    ++ins;
    rb_tree_nd64_insert(test, &ins, val6);

    uint64_t test_getter = 1;
    char *dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo != 0);
    CHKCOND(strncmp(dodo, val1, 9) == 0);
    rb_tree_print(test);
    test_getter = 6;
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo != 0);
    CHKCOND(strncmp(dodo, val6, 9) == 0);
    test_getter = 4;
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo != 0);
    CHKCOND(strncmp(dodo, val4, 9) == 0);
    test_getter = 1;
    rb_tree_delete(test, &test_getter);
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo == 0);
    test_getter = 2;
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo != 0);
    CHKCOND(strncmp(dodo, val2, 9) == 0);
    rb_tree_delete(test, &test_getter);
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo == 0);
    test_getter = 3;
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo != 0);
    CHKCOND(strncmp(dodo, val3, 9) == 0);
    rb_tree_delete(test, &test_getter);
    dodo = rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo == 0);
    test_getter = 4;
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo != 0);
    CHKCOND(strncmp(dodo, val4, 9) == 0);
    rb_tree_delete(test, &test_getter);
    dodo = rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo == 0);
    test_getter = 5;
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo != 0);
    CHKCOND(strncmp(dodo, val5, 9) == 0);
    rb_tree_delete(test, &test_getter);
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo == 0);
    test_getter = 6;
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo != 0);
    CHKCOND(strncmp(dodo, val6, 9) == 0);
    rb_tree_destroy(test);
    STOPTEST;
}

int test_rb_multi_orders(const char **testname,
                         const char **extrareporting)
{
    STARTTEST;
    rb_tree_t *test =
        create_rb_tree(int_compare, int_destroy, info_destroy, int_print,
                       info_print);
    REQCOND(test != 0);
    int *one = malloc(sizeof(int));
    int *two = malloc(sizeof(int));
    int *three = malloc(sizeof(int));
    int *four = malloc(sizeof(int));
    int *five = malloc(sizeof(int));
    int *six = malloc(sizeof(int));
    char *val1 = calloc(20, 1);
    char *val2 = calloc(20, 1);
    char *val3 = calloc(20, 1);
    char *val4 = calloc(20, 1);
    char *val5 = calloc(20, 1);
    char *val6 = calloc(20, 1);
    *one = 1;
    memcpy(val1, "VAL ONE 1", 9);
    *two = 2;
    memcpy(val2, "VAL TWO 2", 9);
    *three = 3;
    memcpy(val3, "VAL THR 3", 9);
    *four = 4;
    memcpy(val4, "VAL FOR 4", 9);
    *five = 5;
    memcpy(val5, "VAL FIV 5", 9);
    *six = 6;
    memcpy(val6, "VAL SIX 6", 9);
    rb_tree_insert(test, one, val1);
    rb_tree_insert(test, two, val2);
    rb_tree_insert(test, three, val3);
    rb_tree_insert(test, four, val4);
    rb_tree_insert(test, five, val5);
    rb_tree_insert(test, six, val6);
    int test_getter = 1;
    char *dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo != 0);
    CHKCOND(strncmp(dodo, val1, 9) == 0);
    rb_tree_print(test);
    test_getter = 6;
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo != 0);
    CHKCOND(strncmp(dodo, val6, 9) == 0);
    test_getter = 4;
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo != 0);
    CHKCOND(strncmp(dodo, val4, 9) == 0);
    test_getter = 1;
    rb_tree_delete(test, &test_getter);
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo == 0);
    test_getter = 2;
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo != 0);
    CHKCOND(strncmp(dodo, val2, 9) == 0);
    rb_tree_delete(test, &test_getter);
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo == 0);
    test_getter = 3;
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo != 0);
    CHKCOND(strncmp(dodo, val3, 9) == 0);
    rb_tree_delete(test, &test_getter);
    dodo = rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo == 0);
    test_getter = 4;
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo != 0);
    CHKCOND(strncmp(dodo, val4, 9) == 0);
    rb_tree_delete(test, &test_getter);
    dodo = rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo == 0);
    test_getter = 5;
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo != 0);
    CHKCOND(strncmp(dodo, val5, 9) == 0);
    rb_tree_delete(test, &test_getter);
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo == 0);
    test_getter = 6;
    dodo = (char *) rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo != 0);
    CHKCOND(strncmp(dodo, val6, 9) == 0);
    rb_tree_delete(test, &test_getter);
    dodo = rb_tree_get_val(test, &test_getter);
    CHKCOND(dodo == 0);
    rb_tree_destroy(test);
    STOPTEST;
}
