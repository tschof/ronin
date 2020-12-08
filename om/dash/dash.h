/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _DASH_H_
#define _DASH_H_

#include "dart_order_obj.h"
#include "connection_interface.h"


void dash_handle_parent_fill(dart_order_obj* orig,
        dart_order_obj* parent,
        con_interface* ci,
        void (*next_step) (struct connection_interface *,
            dart_order_obj * doj),
        char* status, int slen);

void dash_calc_leg_ratio(dart_order_obj* child, dart_order_obj* parent);
void dash_reset_shares_on_replace(dart_order_obj* doj);
#endif

