/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _BOE_UTILS_H__
#define _BOE_UTILS_H__

#define SWAP_BITS_ONE_BYTE (cb, bs, rb)  \\
    unsigned char i, j;                  \\
    unsigned char one = 1;                \\
    unsigned char x = ((bs >> i) ^ (bs >> j)) & ((one << ns) - 1); \\
    rb = bs ^ ((x << i) | (x << j));                               \\ 

#endif
