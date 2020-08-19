/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _OPT_COMMON_H__
#define _OPT_COMMON_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "dart_orderbook.h"
#include "hashmap.h"
#include "message_queue.h"

    struct database_conf;

    void opt_database_init(struct database_conf *, const char *destid,
                           orderbook * ob, int query_cum);

#if defined(__cplusplus)
}
#endif
#endif                          //_MAILER_H__
