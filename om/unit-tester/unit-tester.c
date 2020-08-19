#include "unit-tester.h"
#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>

#define dlerrorchk(condition)                   \
    if (!(condition)) {                         \
        char *msg = dlerror();                  \
        fprintf(stderr, "%s\n", msg);           \
        return -1;                              \
    }

enum passfail { fail = -1, pass = 0 };
struct result {
    enum passfail passfail;
    char *condition, *file, *function;
    int line;
    struct result *next;
};

struct unit_test_suite_impl {
    struct result *head, *tail;
    size_t npass, nfail;
};

static struct unit_test_suite_impl *make_unit_test_suite_impl(void)
{
    struct unit_test_suite_impl *impl = malloc(sizeof *impl);
    if (impl) {
        impl->head = impl->tail = 0;
        impl->npass = impl->nfail = 0;
    }
    return impl;
}

static void add_report_test_case(struct unit_test_suite_impl **impl,
                                 struct result *result)
{
    if (*impl == 0) {
        *impl = make_unit_test_suite_impl();
        (*impl)->head = (*impl)->tail = result;
     } else {
        (*impl)->tail->next = result;
        (*impl)->tail = (*impl)->tail->next;
    }
    if (result->passfail == pass) {
        ++((*impl)->npass);
    } else if (result->passfail == fail) {
        ++((*impl)->nfail);
    } else {
        fprintf(stderr, "%s:%d internal error; invalid pass/fail flag %d\n",
                __FILE__, __LINE__, result->passfail);
    }
}

static struct result *make_result(enum passfail passfail, const char *condition,
                                  const char *file, int line, const char *function)
{
    struct result *result = malloc(sizeof *result);
    result->passfail = passfail;
    result->condition = strdup(condition);
    result->file = strdup(file);
    result->line = line;
    result->function = strdup(function);
    result->next = 0;
    return result;
}

static void free_result(struct result *result)
{
    free(result->function);
    free(result->file);
    free(result->condition);
    free(result);
}

static void report_test_case_passed(struct unit_test_suite *suite,
                                    const char *condition, const char *file,
                                    int line, const char *function)
{
    add_report_test_case(&suite->impl,
                         make_result(pass, condition, file, line, function));
}

static void report_test_case_failed(struct unit_test_suite *suite,
                                    const char *condition, const char *file,
                                    int line, const char *function)
{
    add_report_test_case(&suite->impl,
                         make_result(fail, condition, file, line, function));
}

static struct unit_test_suite *(*get_suite)(void) = 0;

/* Trigger the shared object to create a unit_test_suite struct and
 * return it so we can set the relevant pass/fail functions and
 * initialize the data structures we use to keep track of the test
 * cases. */
static int prep_test_framework(void *sohandle)
{
    void *sym = dlsym(sohandle, "get_unit_test_suite");
    dlerrorchk(sym != 0);
    memcpy(&get_suite, &sym, sizeof sym);
    struct unit_test_suite *suite = get_suite();
    if (suite == 0) {
        fprintf(stderr, "get_unit_test_suite() returned null pointer\n");
        return -1;
    }
    suite->report_pass = report_test_case_passed;
    suite->report_fail = report_test_case_failed;
    suite->impl = 0;
    return 0;
}

static int print_progress_graphic(FILE * out, size_t n, size_t total)
{
    if (total < n)
        return -1;
    size_t i;
    fprintf(out, "{ ");
    for (i = 0; i < n; ++i)
        fprintf(out, "#");
    for (i = 0; i < (total - n); ++i)
        fprintf(out, "-");
    fprintf(out, " }");
    return 0;
}

static int print_pct(FILE * out, size_t npass, size_t nfail)
{
    size_t ntotal = npass + nfail;
    size_t npct = ((double) (npass) / (double) (ntotal)) * 10.0;
    return print_progress_graphic(out, npct, 10u);
}

static void report_summary(struct unit_test_suite_impl *impl)
{
    if (impl->npass == 0 && impl->nfail == 0) {
        printf("No tests run\n");
    }
    printf(" %zu/%zu PASSED\n", impl->npass, impl->npass + impl->nfail);
    print_pct(stdout, impl->npass, impl->nfail);
    printf("\n");
}

void report_results(struct unit_test_suite_impl *impl)
{
    report_summary(impl);
}

int load_shared_object_and_run_tests(const char *shared_object_name, int nfunc,
                                     char *functions[])
{
    void *so_handle = dlopen(shared_object_name, RTLD_LAZY);
    dlerrorchk(so_handle != 0);

    /* Before running the tests, switch the current working directory
     * to the directory containing the test shared object.  The test
     * shared object should be able to assume that input and output
     * files without fully qualified path names should be located in
     * the same directory as the shared object. */
    char *path = strdup(shared_object_name);
    if (path) {
        char *dir = dirname(path);
        if (dir && chdir(dir) == -1) {
            perror(dir);
        }
        free(path);
    }

    if (prep_test_framework(so_handle) == -1) {
        fprintf(stderr, "encountered error prepping test framework for %s\n",
                shared_object_name);
        return -1;
    }

    int i;
    void (*test_case_function) (void);
    for (i = 0; i < nfunc; ++i) {
        test_case_function = dlsym(so_handle, functions[i]);
        if (test_case_function != 0)
            test_case_function();
        else
            fprintf(stderr, "%s\n", dlerror());
    }

    /* Report results */
    struct unit_test_suite *suite = get_suite();
    if (suite == 0) {
        return -1;
    }
    struct unit_test_suite_impl *impl = suite->impl;
    if (impl == 0) {
        return -1;
    }

    /* Print an overrall summary of pass/fail */
    report_summary(impl);

    /* Now walk the list of structures and list the information for
     * all failed cases, freeing memory as we go. */
    struct result *result;
    result = impl->head;
    while (result != 0) {
        struct result *next = result->next;
        if (result->passfail == fail) {
            printf("%s:%d: FAIL in %s condition failed: '%s'\n",
                   result->file, result->line, result->function,
                   result->condition);
        }
        free_result(result);
        result = next;
    }
    get_suite = 0;
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "usage: unit-tester unit-test-shared-object "
                "<test-function-1> <test-function-2> ... <test-function-n>\n");    
    }
    if (load_shared_object_and_run_tests(argv[1], argc - 2, &argv[2]) == -1) {
        
    }
    return 0;
}
