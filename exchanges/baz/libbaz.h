#ifndef __LIBBAZ_H__
#define __LIBBAZ_H__

#include <stddef.h>

enum baz_passfail {
    baz_fail = -1,
    baz_pass = 0,
};

struct baz_condition_check {
    enum baz_passfail passfail;
    const char *condition, *file, *function;
    int line;
    struct baz_condition_check *next;
};

struct baz_testsuite {
    size_t npass, nfail;
    struct baz_condition_check *head, *tail;
};

void baz_report(enum baz_passfail passfail, const char *condition,
                const char *file, int line, const char *function);

struct baz_testsuite *baz_get_results(void);
#define BAZ_GET_RESULTS_NAME "baz_get_results"
typedef struct baz_testsuite *(*baz_get_results_func) (void);

#define BAZ_ENTRY_NAME "baz_run_tests"
typedef void (*baz_test_runner_func) (void);

struct baz_testsuite *baz_get_results(void);

#define CHKCOND(condition)                                              \
    if ((condition)) {                                                  \
        baz_report(baz_pass, #condition, __FILE__, __LINE__, __FUNCTION__); \
    } else {                                                            \
        baz_report(baz_fail, #condition, __FILE__, __LINE__, __FUNCTION__); \
    }

#endif
