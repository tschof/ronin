/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _FIX_ADMIN_MASTER_H__
#define _FIX_ADMIN_MASTER_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "client_manager.h"
#include "transaction_recorder.h"
#include "fix_handler.h"

    /**
     * Initializes and runs the main loop that will listen for adds
     * and remvoes and sends heartbeats and test requests.
     */
    void run_fix_admin_loop(fix_con_obj * rec);

    void tickle_connection(fix_con_obj * obj);

    void kill_connection(fix_con_obj * fco);

#if defined(__cplusplus)
}
#endif
#endif                          //FIX_ADMIN_MASTER_H__
