#include <stdio.h>
#include "libbaz.h"

struct baz_testsuite *get_results(void)
{
    return baz_get_results();
}

void baz_run_tests(void)
{
    CHKCOND(printf("%s\n", __FUNCTION__) == -1);
}
