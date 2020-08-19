/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _CONNECTOR_H__
#define _CONNECTOR_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "transaction_recorder.h"
#include "client_manager.h"

#define MAXCONRETRIES 3

    void connect_dart(struct client_con_set *ccs);


#if defined(__cplusplus)
}
#endif
#endif
