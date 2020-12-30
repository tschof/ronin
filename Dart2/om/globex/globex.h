/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _GLOBEX_H__
#define _GLOBEX_H__

#include "dart_order_obj.h"
#include "databuf.h"

void set_rom_to_cme_price(dart_order_obj* doj, int romfield,
                          int rval,databuf_t* fix);

void set_cme_to_rom(dart_order_obj* doj, int romfield,
                    int rval,databuf_t* fix);
#endif //_GLOBEX_H__
