/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _PILLAR_OATS_PARSER_H__
#define _PILLAR_OATS_PARSER_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include "dart_order_obj.h"

uint8_t get_bitfield_instruction(uint64_t bits, uint64_t start, uint64_t end);    
void pillar_cs_parse_oats(void* p, char* d, int dlen, dart_order_obj* doj);
void trans_side(uint8_t iside, dart_order_obj* doj);

#if defined(__cplusplus)
}
#endif
#endif

