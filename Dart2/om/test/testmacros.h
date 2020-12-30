#ifndef __TEST_MACROS_H__
#define __TEST_MACROS_H__

#include <stdio.h>

#define STARTTEST                               \
    int nfailures = 0;

#define CHKCOND(cond)                                           \
if (!(cond)) {                                                  \
    printf("%s:%d: error: %s\n", __FILE__, __LINE__, #cond);    \
    ++nfailures;                                                \
 }                                                              \
*testname = __FUNCTION__;

#define REQCOND(cond)                           \
    if (!(cond)) {                              \
        ++nfailures;                            \
        *extrareporting = #cond " failed";      \
        goto EXITTEST;                          \
    }

#define STOPTEST                                \
    goto EXITTEST;                              \
EXITTEST:                                       \
 return nfailures;

#endif                          //__TEST_MACROS_H__
