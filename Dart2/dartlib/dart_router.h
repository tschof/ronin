/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _DART_ROUTER_H__
#define _DART_ROUTER_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include "sb_router.h"
#include "log_mnger.h"

sbrouter* create_dart_router(int allocation, otl* o,
                             char* m, int mlen, char* filename);

#if defined(__cplusplus)
}
#endif
#endif
