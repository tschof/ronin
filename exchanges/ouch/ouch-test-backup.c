#include "soupbin.h"
#include "../unit-test-framework/unit-test-framework.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

DART_TEST_CASE(foo)
{
    DART_TEST_CASE_BEGIN;
    char *p = 0;
    CHKCOND((p = malloc(512)) != 0);
    free(p);
    return reporter;
}

DART_TEST_CASE(foo2)
{
    DART_TEST_CASE_BEGIN;
    CHKCOND(0);
    CHKCOND(1);
    return reporter;
}

DART_TEST_CASE(soupbin_create_login)
{
    /* struct soupbin_hearbeat heartbeat; */
    /* CHKCOND(soupbin_heartbat_init(&heartbeat) != -1); */
    /* return reporter; */
}

DART_BEGIN_TEST_CASE(foo3)
{
}

DART_END_TEST_CASE
    DECLARE_TEST_CASES_START DECLARE_TEST_CASE(foo) DECLARE_TEST_CASE(foo2)
DECLARE_TEST_CASES_STOP DECLARE_TEST_ENTRY_POINT("NASDAQ OUCH Tests")
