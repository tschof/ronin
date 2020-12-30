/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _EUREX_H__
#define _EUREX_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include "databuf.h"

int eur_to_rom_time(uint64_t eur_time, databuf_t* buff);
uint32_t get_eurex_secuity_id(int field, dart_order_obj* doj);
#if defined(__cplusplus)
}
#endif
#endif
