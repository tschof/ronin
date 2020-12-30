/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _MDS_CLIENT_H__
#define _MDS_CLIENT_H__
#if defined(__cplusplus)
extern "C" {
#endif
#include "ex_config_helper.h"
#include "log_mnger.h"

typedef struct mds_client_con mdsclient;

mdsclient* create_mds_client(ex_config* ex, otl* o,void* cbo, 
                             void (*mds_update)(void* cbo, char* sym, int slen, long bid, long offer),
                             void (*con_update)(void* cbo, int is_connected));

void close_mds_client(mdsclient* mdc);

#if defined(__cplusplus)
}
#endif
#endif
