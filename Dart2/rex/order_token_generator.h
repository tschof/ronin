/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _ORDER_TOKEN_GENERATOR_H__
#define _ORDER_TOKEN_GENERATOR_H__
#if defined(__cplusplus)
extern "C" {
#endif


typedef struct order_token_generator {
    int (*create_in_place_token)(struct order_token_generator* otg, char* off, int max_len);
    unsigned long offset;
    unsigned long counter;
    char datestr[9];
    char key[3];
} token_gen;
/**
 * Generic creation function, it will initialize the date string and set up offset and
 * counter based on the microseconds past midnight that this connection was launched.
 * This implementation completely ignores the void* passed in .
 */
token_gen* generic_creation_func();
/**
 * Allows applications to pass in a unique key.
 */
token_gen* token_creation_func(char* t, int len);

/**
 * This is a generic function that can be used by most applications.
 */
int create_in_place_ord_token(token_gen* tg, char* off, int max_len);
/**
 * This pre-pends a date string to the token.
 */
int create_multiday_in_place_ord_token(token_gen* tg, char* off, int max_len);

/**
 * Pre pends a date string to and uses the seq passed in to create the token
 * only uses 0-9
 */
int create_multiday_all_num_token(token_gen* tg, int seq, char* off, int max_len, int min_len);

#if defined(__cplusplus)
}
#endif
#endif //_ORDER_TOKEN_GENERATOR_H__
