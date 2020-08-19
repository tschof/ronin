#include "libbaz.h"
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

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

void report_results(struct baz_testsuite *testsuite)
{
    if (testsuite == 0 || (testsuite->npass == 0 && testsuite->nfail == 0)) {
        printf("No tests run\n");
        return;
    }
    print_pct(stdout, testsuite->npass, testsuite->nfail);
    printf(" %zu/%zu PASSED\n", testsuite->npass,
           testsuite->npass + testsuite->nfail);
    struct baz_condition_check *p;
    for (p = testsuite->head; p != 0; p = p->next) {
        if (p->passfail == baz_fail) {
            printf("%s:%d: fail: in '%s' condition failed: '%s'\n", p->file,
                   p->line, p->function, p->condition);
        }
    }
}

#define dlerrorchk(condition)                   \
    if (!(condition)) {                         \
        char *msg = dlerror();                  \
        fprintf(stderr, "%s\n", msg);           \
        return;                                 \
    }

void load_so_and_run_test(const char *soname)
{
    int retval = -1;
    void *sohandle = dlopen(soname, RTLD_LAZY);
    dlerrorchk(sohandle != 0);
    baz_test_runner_func runtests = dlsym(sohandle, BAZ_ENTRY_NAME);
    dlerrorchk(runtests != 0);
    runtests();
    baz_get_results_func get_results =
        dlsym(sohandle, BAZ_GET_RESULTS_NAME);
    if (get_results != 0)
        report_results(get_results());
    else
        printf("No results\n");
    dlerrorchk(dlclose(sohandle) == 0);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "usage: baz so-test1 so-test2 ...\n");
        return EXIT_FAILURE;
    }
    int i;
    for (i = 1; i < argc; ++i) {
        load_so_and_run_test(argv[i]);
    }
    return EXIT_SUCCESS;
}
