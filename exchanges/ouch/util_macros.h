#ifndef __UTIL_MACROS_H__
#define __UTIL_MACROS_H__

/* Some macros to make writing common idioms less tedious. */

#define jump_assert(cond, label)                \
    if (!(cond)) { goto label; }

#endif
