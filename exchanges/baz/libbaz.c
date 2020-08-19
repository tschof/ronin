#include "libbaz.h"
#include <stdlib.h>
#include <stdio.h>

struct baz_testsuite *testsuite = 0;

__attribute__ ((constructor))
static void baz_init(void)
{
    testsuite = malloc(sizeof *testsuite);
    testsuite->head = testsuite->tail = 0;
    testsuite->npass = testsuite->nfail = 0;
}

__attribute__ ((destructor))
static void baz_fini(void)
{
    while (testsuite->head != 0) {
        struct baz_condition_check *tmp = testsuite->head;
        testsuite->head = testsuite->head->next;
        free(tmp);
    }
    free(testsuite);
}

void baz_report(enum baz_passfail passfail, const char *condition,
                const char *file, int line, const char *function)
{
    if (passfail == baz_pass) {
        ++testsuite->npass;
        return;
    }
    struct baz_condition_check *condchk = malloc(sizeof *condchk);
    condchk->passfail = passfail;
    condchk->condition = condition;
    condchk->file = file;
    condchk->line = line;
    condchk->function = function;
    condchk->next = 0;
    if (testsuite->head == 0) {
        testsuite->head = condchk;
        testsuite->tail = condchk;
    } else {
        testsuite->tail->next = condchk;
        testsuite->tail = condchk;
    }
    if (condchk->passfail == baz_pass)
        ++(testsuite->npass);
    else
        ++(testsuite->nfail);
}

struct baz_testsuite *baz_get_results(void)
{
    return testsuite;
}
