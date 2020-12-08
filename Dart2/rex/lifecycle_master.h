/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _LIFECYCLE_MASTER_H__
#define _LIFECYCLE_MASTER_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include "polling_server.h"
#include "ex_config_helper.h"

typedef struct lifecycle_master cycle_master;

void destroy_life_cycle_master(cycle_master* cm);
/**
 * This creates an object that will manage the run time 
 * of a process. It creates a signal handler for control c
 * catches.  The eod timer takes the end hour, and end min
 * values from the ex_config in order to determine the run
 * time.
 */
cycle_master* create_lifecycle_master(pserver* asc, 
                                      void (*signal_callback)(void* v),
                                      void (*eod_timer_clean_up) (void* v),
                                          ex_config* mc);
cycle_master* create_gen_life_master(pserver* asc, ex_config* mc);
/**
 * Added to interface for testing.
 */
int get_sec_till_end_time(int end_hour, int end_min, int end_day,
                          struct tm* newtime);
#if defined(__cplusplus)
}
#endif
#endif //_LIFECYCLE_MASTER_H__

