/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _ISE_H_
#define _ISE_H_

#include "dart_order_obj.h"
#include "connection_interface.h"


void ise_handle_parent_fill(dart_order_obj* orig,
        dart_order_obj* parent,
        con_interface* ci,
        void (*next_step) (struct connection_interface *,
            dart_order_obj * doj),
        char* status, int slen);

void ise_calc_leg_ratio(dart_order_obj* child, dart_order_obj* parent);
void reset_shares_on_replace(dart_order_obj* doj);
#endif

