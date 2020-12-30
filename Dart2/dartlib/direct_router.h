/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _DIRECT_ROUTER_H__
#define _DIRECT_ROUTER_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include "sb_router.h"
#include "log_mnger.h"
#include "risk_cntr_structs.h"

sbrouter* create_direct_router(otl* o, char* m, int mlen);

#if defined(__cplusplus)
}
#endif
#endif
