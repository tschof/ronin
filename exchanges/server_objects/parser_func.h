/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _PARSER_FUNC_H__
#define _PARSER_FUNC_H__

#if defined(__cplusplus)
extern "C" {
#endif

    long find_offset(const char *haystack, unsigned long hlen,
                     const char *needle, unsigned long nlen);

    char *find_offset3(const char *haystack, unsigned long len,
                       const char *needle);

    char *find_offset2(const char *phaystack, const char *pneedle);

#if defined(__cplusplus)
}
#endif
#endif                          //parser_func_h__
