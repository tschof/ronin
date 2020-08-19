/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _ORDER_TOKEN_CREATOR_H__
#define _ORDER_TOKEN_CREATOR_H__

#if defined(__cplusplus)
extern "C" {
#endif

    /* This structure contains the counters that will be incremented
     * every time create_order_token is called. */
    typedef struct token_struct token_struct_t;

    /* This takes an order token struct a buffer to store the order
     * token and the length of the buffer.  It will fill that buffer
     * with an incremented order token and return the length written
     * into the buffer. */
    int create_order_token(token_struct_t *, char *, int);

    /* This creates an order token prefixed with the date, as figured
     * from local time when init_order_token_creator was called. */
    int create_multiday_unique_order_token(token_struct_t *, char *, int);

    /* Creates an order token object that can be used to create
     * incremented tokens.  This function computes the time offset. */
    token_struct_t *init_order_token_creator(void);

#if defined(__cplusplus)
}
#endif
#endif
