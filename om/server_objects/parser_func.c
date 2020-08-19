/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#include "parser_func.h"

long find_offset(const char *haystack, unsigned long hlen,
                 const char *needle, unsigned long nlen)
{
    char *cp = (char *) haystack;
    char *s1;
    char *s2;
    unsigned long curr_off;
    unsigned long l_off;
    if (nlen > hlen) {
        return -1;
    }
    curr_off = 0;
    while (curr_off < hlen) {
        s1 = cp;
        s2 = (char *) needle;
        l_off = curr_off;
        while (l_off < hlen && *s1 && *s2 && !(*s1 - *s2)) {
            ++l_off, ++s1, ++s2;
        }
        if (!*s2) {
            return curr_off;
        }
        ++cp;
        ++curr_off;
    }
    return -1;

}


