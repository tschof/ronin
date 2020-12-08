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

#define COUNT_SET_BITS(v, c)        \
    for(c= 0; v; c++)               \
       v &= v - 1;                  \

/**
 * @param f is the flag that determines if we set or clear bit.
 * @param m the bit mask
 * @param w the value to modify
 * if (f) w |= m else w &= ~m
 */
#define SET_BIT(f, m, w)            \
    w ^= (-f ^ w) & m;              \


/**
 * @param r the return value
 * @param b the char to set
 * @param bit the bit you are lookin fer.
 */
#define IS_BIT_SET(r, b, bit)          \
    r = b & (0x1 << bit);              \


#define FIND_NULL(str, counter, max)   \
    do {                          \
        ++counter;                \
    } while(str[counter] != 0x0 && counter < max)  \

#endif
