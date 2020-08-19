#ifndef __UNIT_TESTER_H__
#define __UNIT_TESTER_H__

#include <stdlib.h>
#include <assert.h>

#define unit_tester_guid_prefix cd604540_c240_401f_ad3f_f4c069206c7a
#define testfunc_prefix unit_test_case__

struct unit_test_suite_impl;
struct unit_test_suite;
typedef void (*unit_test_report_function) (struct unit_test_suite *,
                                           const char *condition,
                                           const char *file, int line,
                                           const char *function);
typedef struct unit_test_suite *(*get_unit_test_suite_func) (void);

struct unit_test_suite {
    unit_test_report_function report_pass, report_fail;
    struct unit_test_suite_impl *impl;
};

struct unit_test_suite *get_unit_test_suite(void);

struct unit_test_suite *get_unit_test_suite(void);
void free_unit_test_suite(struct unit_test_suite *);

/* This preprocessor definition must be defined somewhere, once, in
 * the shared object.  This declares a pair of functions that manages
 * the lifetime of a test suite object that is used by the framework
 * and the other test macros to report test case results. */
#define declare_unit_tester                                     \
    struct unit_test_suite *get_unit_test_suite(void)           \
    {                                                           \
        static struct unit_test_suite *suite = 0;               \
        if (suite == 0) {                                       \
            suite = malloc(sizeof *suite);                      \
            suite->report_pass = suite->report_fail = 0;        \
            suite->impl = 0;                                    \
        }                                                       \
        return suite;                                           \
    }                                                           \
                                                                \
    void free_unit_test_suite(struct unit_test_suite *suite)    \
    {                                                           \
        free(suite);                                            \
    }

#define declare_unit_test(name)                                 \
    void unit_test_case___##name(void)

#define check(condition)                                        \
    do {                                                        \
        struct unit_test_suite *suite = get_unit_test_suite();  \
        assert(suite != 0);                                     \
        if ((condition)) {                                      \
            suite->report_pass(suite, #condition, __FILE__,     \
                               __LINE__, __FUNCTION__);         \
        } else {                                                \
            suite->report_fail(suite, #condition, __FILE__,     \
                               __LINE__, __FUNCTION__);         \
        }                                                       \
    } while (0)

#endif
