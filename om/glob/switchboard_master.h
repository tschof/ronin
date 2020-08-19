/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _SWITCHBOARD_MASTER_H__
#define _SWITCHBOARD_MASTER_H__

#include "dart_order_obj.h"
#include "connection_interface.h"
#include "async_server.h"

void glob_on_connect(async_parse_args * ap, int is_connected,
                     char *log, int len);
void switchboard(dart_order_obj * doj, void *gk);
int cancel_single_order(con_interface * ci, dart_order_obj * doj);
void cancel_all_orders(con_interface * ci, dart_order_obj * doj);
void return_to_sender(dart_order_obj *, void *a);
void on_reject(con_interface *, dart_order_obj * doj);
void on_out(con_interface *, dart_order_obj * doj);
void on_replaced(con_interface *, dart_order_obj * doj);
void on_fill(con_interface * ci, dart_order_obj * doj);

#endif
