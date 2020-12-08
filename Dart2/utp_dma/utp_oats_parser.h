/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _UTP_OATS_PARSER_H__
#define _UTP_OATS_PARSER_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include "dart_order_obj.h"

void utp_cs_parse_oats(void* p, char* d, int dlen, dart_order_obj* doj);


#if defined(__cplusplus)
}
#endif
#endif
