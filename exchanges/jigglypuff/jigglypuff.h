/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _JIGGLY_PUFF_LIMITS_CHECKER_H__
#define _JIGGLY_PUFF_LIMITS_CHECKER_H__

#include "dart_server_exports.h"
#include "message_queue.h"
#if defined(__cplusplus)
extern "C" {
#endif

#define JP_SEC_TYPE_EQ 0    
#define JP_SEC_TYPE_OPT 1
#define JP_SEC_TYPE_FUT 2
#define JP_SEC_TYPE_FX  3


typedef struct jp_limits_checker jp_limits_checker;


jp_limits_checker* create_jp_limits_checker(ex_config ex);
int load_trader_profiles(jp_limits_checker* jc);
/**
 * Update a traders balance the int price is assumed to be
 * price multiplied by 10000.
 */
int update_balance(char* trader, int tlen, int shares, int price, int buy_sell, int sec_type);

void jp_block_trader(jp_limits_checker* jpc, char* trader, int len, int blocked);
void jp_exempt_trader(jp_limits_checker* jpc, char* trader, int len, int exempt);
#if defined(__cplusplus)
}
#endif
#endif                          //JIGGLY_PUFF_LIMITS_CHECKER_
