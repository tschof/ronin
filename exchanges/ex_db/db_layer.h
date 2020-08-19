/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _DB_LAYER_H__
#define _DB_LAYER_H__

#include "message_queue.h"

#if defined(__cplusplus)
extern "C" {
#endif

    typedef struct dart_ex_result_set {
        char callPut[3];
        char capacity[40];
        char clearingAccount[32];
        char clearingID[32];
        char CMTA[30];
        int cumQty;
        char exchangeTag[128];
        int leavesQTY;
        int maxFloor;
        char mmAcct[32];
        char omTag[40];
        char orderTag[40];
        int orderType;
        double price;
        int qty;
        char secType[4];
        int side;
        double stopPrice;
        double strikePrice;
        char symbol[40];
        char tag[128];
        int tif;
        char expireDate[40];
        char short_lender[14];
        int cplxtype;
        char parent_om_tag[128];
        int mm_exchange;
        char open_close[5];
        char trade_for[32];
        char trader[32];
        char expDay[5];
        char destination[6];
        char sec_def[128];
        char smpid[32];
    } dart_ex_result_set;

    struct database_conf;

    int get_db_objects(struct database_conf *, queue_t results,
                       const char *destid);

#if defined(__cplusplus)
}
#endif
#endif                          //_DB_LAYER_H__
