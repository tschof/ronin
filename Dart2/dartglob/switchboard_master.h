/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _SWITCHBOARD_MASTER_H__
#define _SWITCHBOARD_MASTER_H__

#include "dart_order_obj.h"
#include "connection_interface.h"

void ex_con_connect(void * ap, int is_connected,
                     char *log, int len);
void glob_log_to_oats(con_int* ap, int is_connected);
void glob_log_to_oats_dest(con_int* ap, int is_connected);
void switchboard(void* cb, dart_order_obj * doj);
int cancel_single_order(con_int* ci, rcntr * doj);
void cancel_all_orders(con_int* ci, rcntr * doj);
void return_to_sender(con_int* ci, rcntr * doj);
void on_reject(con_int*, rcntr * doj);
void on_out(con_int*, rcntr * doj);
void on_replaced(con_int*, rcntr * doj);
void on_fill(con_int* ci, rcntr * doj);

#endif
