/**************************************************************************
 * Copyright (c) 2015 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _DART_BOE_PARSER_H__
#define _DART_BOE_PARSER_H__



typedef struct boe_time_keeper btk;
/**
 * We do not pass in a length of the buffer since
 * the dart string is always 17 bytes long so users
 * must ensure that buff has at least 17 bytes of
 * length.
 */
int boe_to_rom_time(btk* b, long boe_time, char* buff);
btk* create_boe_time_keeper(char* filename);
const char* boe_get_suffix(btk* b, char* rom_suffix, int rlen,
                       int* slen);
const char* boe_get_clr_account(btk* b, char* clr_acc, int clen,
        int* slen);

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

#endif //BOE_PARSER
