/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#include "lf_list.h"

#ifndef S_SPLINT_S
unsigned long cas(unsigned long *ptr, unsigned long old, unsigned long new)
{
    unsigned chr ret;
    __asm__ __volatile__("  lock\n"
                         "  cmpxchgl %2,%1\n"
                         "  sete %0\n":"=q"(ret), "=m"(*ptr)
                         :"r"(new), "m"(*ptr), "a"(old)
                         :"memory");
    return (unsigned long) ret;
}
#endif
