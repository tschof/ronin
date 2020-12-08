/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _CFE_AUDIT_TRAIL_H__
#define _CFE_AUDIT_TRAIL_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include "dart_order_obj.h"

void cfe_audit_trail(void* p, char* orig, int olen,
    dart_order_obj* doj, time_t sec, int micro);


#if defined(__cplusplus)
}
#endif
#endif

