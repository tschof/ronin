/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#define	EX_CTLIB_VERSION	CS_VERSION_125

#define MAXSTRING   40

#include "db_layer.h"
#include "database_conf.h"
#include "version.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctpublic.h>

/* Callback routines for library errors and server messages. */
CS_RETCODE CS_PUBLIC csmsg_callback(CS_CONTEXT * context,
                                    CS_CLIENTMSG * clientmsg);
CS_RETCODE CS_PUBLIC clientmsg_callback(CS_CONTEXT * context,
                                        CS_CONNECTION * connection,
                                        CS_CLIENTMSG * clientmsg);
CS_RETCODE CS_PUBLIC servermsg_callback(CS_CONTEXT * context,
                                        CS_CONNECTION * connection,
                                        CS_SERVERMSG * servermsg);

int get_db_objects(struct database_conf *dbconf, queue_t results,
                   const char *destid)
{
    char *statement;
    CS_CONTEXT *context;
    CS_CONNECTION *connection;
    CS_COMMAND *cmd = 0;
    CS_DATAFMT *datafmt = calloc(1, sizeof(CS_DATAFMT));
    CS_INT datalength[35];
    CS_SMALLINT indicator[35];
    CS_INT count;
    CS_RETCODE ret;
    CS_INT result_type;
    CS_RETCODE results_ret;

    CS_CHAR callPut[3];
    CS_CHAR capacity[MAXSTRING];
    CS_CHAR clearingAccount[32];
    CS_CHAR clearingID[32];
    CS_CHAR CMTA[30];
    CS_INT cumQty;
    CS_CHAR exchangeTag[128];
    CS_INT leavesQTY;
    CS_INT maxFloor;
    CS_CHAR mmAcct[32];
    CS_CHAR omTag[MAXSTRING];
    CS_CHAR orderTag[MAXSTRING];
    CS_INT orderType;
    CS_FLOAT price;
    CS_INT qty;
    CS_CHAR secType[4];
    CS_INT side;
    CS_FLOAT stopPrice;
    CS_FLOAT strikePrice = 0;
    CS_CHAR symbol[MAXSTRING];
    CS_CHAR tag[128];
    CS_INT tif;
    CS_CHAR expireDate[MAXSTRING];
    CS_CHAR short_lender[13];
    CS_INT cplxtype = 0;
    CS_CHAR parent_om_tag[128];
    CS_INT mm_exchange;
    CS_CHAR open_close[5];
    CS_CHAR trade_for[30];
    CS_CHAR trader[30];
    CS_CHAR expDay[30];
    CS_CHAR sec_def[128];
    CS_CHAR smpid[30];
    CS_BOOL fmt_exp = CS_TRUE;
    CS_INT destination;

    memset(callPut, '\0', 3);
    memset(capacity, '\0', MAXSTRING);
    memset(clearingAccount, '\0', 32);
    memset(clearingID, '\0', 32);
    memset(CMTA, '\0', 30);
    memset(exchangeTag, '\0', 128);
    memset(mmAcct, '\0', 32);
    memset(omTag, '\0', MAXSTRING);
    memset(orderTag, '\0', MAXSTRING);
    memset(secType, '\0', 4);
    memset(symbol, '\0', MAXSTRING);
    memset(tag, '\0', 128);
    memset(expireDate, '\0', MAXSTRING);
    memset(short_lender, '\0', sizeof short_lender);
    memset(parent_om_tag, '\0', 128);
    memset(open_close, '\0', 5);
    memset(trade_for, '\0', 30);
    memset(trader, '\0', 30);
    memset(expDay, '\0', 30);
    statement = calloc(900, 1);
    memset(sec_def, '\0', 128);
    memset(smpid, '\0', 30);
    CS_INT state_len = sprintf(statement,
            "SELECT callPut, capacity, clearingAccount, clearingID, CMTAID, "
            "cumQty, exchangeTag, leavesQty, maxFloor, mmAcct, omTag, "
            "orderTag, orderType, price, qty, secType, side, stopPrice, "
            "strikePrice, symbol, tag, tif, expDate, shortLender, cplxtype, "
            "parentOmTag, mmExchange, openClose, tradeFor, "
            "(case when expireDate is NULL THEN NULL else "
            "substring(convert(varchar,expireDate,112),7,2)end), DestID, "
            "SecDef, trader, GiveUp FROM %s..Orders WHERE DestRouteID = %s AND "
            "status <> 2 AND status <> 4 AND status <> 8 AND status <> 12",
            dbconf->database, destid);

    context = (CS_CONTEXT *) NULL;
    ret = cs_ctx_alloc(EX_CTLIB_VERSION, &context);
    if (ret != CS_SUCCEED) {
        printf("cs_ctx_alloc failed!!\n");
        return -1;
    }

    ret = ct_init(context, EX_CTLIB_VERSION);
    if (ret != CS_SUCCEED) {
        printf("cs_init failed!!\n");
        return -1;
    }
    ret = ct_config(context, CS_SET, CS_EXPOSE_FMTS, &fmt_exp,
                    CS_UNUSED, NULL);
    if (ret != CS_SUCCEED) {
        printf
            ("failed to set the expose formats variable CNTXT AFTER.!\n");
    }

    ret = cs_config(context, CS_SET, CS_MESSAGE_CB,
                    (CS_VOID *) csmsg_callback, CS_UNUSED, NULL);
    if (ret != CS_SUCCEED) {
        printf("cs_config failed!!\n");
    }
    ret = ct_con_alloc(context, &connection);
    if (ret != CS_SUCCEED) {
        printf("ct_con_alloc failed!!\n");
    }
    ret = ct_con_props(connection, CS_SET, CS_USERNAME,
                       dbconf->username, CS_NULLTERM, NULL);
    if (ret != CS_SUCCEED) {
        printf("could not set username failed!!\n");
    }
    ret = ct_con_props(connection, CS_SET, CS_PASSWORD,
                       dbconf->password, CS_NULLTERM, NULL);
    if (ret != CS_SUCCEED) {
        printf("failed to set password!!\n");
    }
    ret = ct_con_props(connection, CS_SET, CS_EXPOSE_FMTS, &fmt_exp,
                       CS_UNUSED, NULL);
    if (ret != CS_SUCCEED) {
        printf("failed to set the expose formats variable.!\n");
    }
    ret = ct_connect(connection, dbconf->server, strlen(dbconf->server));
    if (ret != CS_SUCCEED) {
        printf("Failed to connect!!\n");
    }
    ret = ct_cmd_alloc(connection, &cmd);
    if (ret != CS_SUCCEED) {
        printf("cmd_alloc failed!!\n");
    }
    ret = ct_command(cmd, CS_LANG_CMD, statement, state_len, CS_UNUSED);//CS_NULLTERM, CS_UNUSED);
    if (ret != CS_SUCCEED) {
        printf("ct_command failed!!\n");
    }
    ret = ct_send(cmd);
    if (ret != CS_SUCCEED) {
        printf("FAILED  ct send: \n");
    }
    while ((results_ret = ct_results(cmd, &result_type)) == CS_SUCCEED) {
        switch ((int) result_type) {
        case CS_ROWFMT_RESULT:
            ret = ct_describe(cmd, 1, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_describe failed call put RFMNT. \n");
            }
            break;
        case CS_ROW_RESULT:
            ret = ct_describe(cmd, 1, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_describe failed call put. \n");
            }
            ret = ct_bind(cmd, 1, datafmt,
                          callPut, &datalength[0], &indicator[0]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed!! CALL PUT \n");
            }
            ret = ct_describe(cmd, 2, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_describe failed capacity. \n");
            }
            ret = ct_bind(cmd, 2, datafmt,
                          capacity, &datalength[1], &indicator[1]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed!! CAPACITY\n");
            }
            ret = ct_describe(cmd, 3, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_describe failed clearingAccount. \n");
            }
            ret = ct_bind(cmd, 3, datafmt,
                          clearingAccount, &datalength[2], &indicator[2]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed!! CLEARING ACCOUNT \n");
            }
            ret = ct_describe(cmd, 4, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_describe failed clearingID. \n");
            }
            ret = ct_bind(cmd, 4, datafmt,
                          clearingID, &datalength[3], &indicator[3]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed!! CLEARING ID\n");
            }
            ret = ct_describe(cmd, 5, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_descibe failed CMTA\n");
            }
            ret = ct_bind(cmd, 5, datafmt,
                          CMTA, &datalength[4], &indicator[4]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed!! CMTA \n");
            }
            ret = ct_describe(cmd, 6, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_descibe failed!!:\n");
            }
            ret = ct_bind(cmd, 6, datafmt,
                          &cumQty, &datalength[5], &indicator[5]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed on CUMQTY!!\n");
            }
            ret = ct_describe(cmd, 7, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_describe failed EXCHANGE TAG. \n");
            }
            ret = ct_bind(cmd, 7, datafmt,
                          exchangeTag, &datalength[6], &indicator[6]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed!! EX TAG\n");
            }
            ret = ct_describe(cmd, 8, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_descibe failed!!\n");
            }
            ret = ct_bind(cmd, 8, datafmt,
                          &leavesQTY, &datalength[7], &indicator[7]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed LEAVES QTY!!\n");
            }
            ret = ct_describe(cmd, 9, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_descibe failed!!\n");
            }
            ret = ct_bind(cmd, 9, datafmt,
                          &maxFloor, &datalength[8], &indicator[8]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed MaxFloor!!\n");
            }
            ret = ct_describe(cmd, 10, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_describe failed mmAcct. \n");
            }
            ret = ct_bind(cmd, 10, datafmt,
                          mmAcct, &datalength[9], &indicator[9]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed mmAcct!!\n");
            }
            ret = ct_describe(cmd, 11, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_descibe failed!! omTag \n");
            }
            ret = ct_bind(cmd, 11, datafmt,
                          omTag, &datalength[10], &indicator[10]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed!! OM TAG\n");
            }
            ret = ct_describe(cmd, 12, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_descibe failed!! orderTag\n");
            }
            ret = ct_bind(cmd, 12, datafmt,
                          orderTag, &datalength[11], &indicator[11]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed OrderTag!!\n");
            }
            ret = ct_describe(cmd, 13, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_descibe failed!!\n");
            }
            ret = ct_bind(cmd, 13, datafmt,
                          &orderType, &datalength[12], &indicator[12]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed orderType!!\n");
            }
            ret = ct_describe(cmd, 14, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_descibe failed!!\n");
            }
            ret = ct_bind(cmd, 14, datafmt,
                          &price, &datalength[13], &indicator[13]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed price!!\n");
            }
            ret = ct_describe(cmd, 15, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_descibe failed!!\n");
            }
            ret = ct_bind(cmd, 15, datafmt,
                          &qty, &datalength[14], &indicator[14]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed QTY!!\n");
            }
            ret = ct_describe(cmd, 16, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_descibe failed!! secType\n");
            }
            ret = ct_bind(cmd, 16, datafmt,
                          secType, &datalength[15], &indicator[15]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed secType!!\n");
            }
            ret = ct_describe(cmd, 17, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_descibe failed!!\n");
            }
            ret = ct_bind(cmd, 17, datafmt,
                          &side, &datalength[16], &indicator[16]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed side!!\n");
            }
            ret = ct_describe(cmd, 18, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_descibe failed!!\n");
            }
            ret = ct_bind(cmd, 18, datafmt,
                          &stopPrice, &datalength[17], &indicator[17]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed stopPrice!!\n");
            }
            ret = ct_describe(cmd, 18, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_descibe failed!!\n");
            }
            ret = ct_bind(cmd, 19, datafmt,
                          &strikePrice, &datalength[18], &indicator[18]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed strikePrice!!\n");
            }
            ret = ct_describe(cmd, 20, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_descibe failed!! symbol \n");
            }
            ret = ct_bind(cmd, 20, datafmt,
                          symbol, &datalength[19], &indicator[19]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed symbol!!\n");
            }
            ret = ct_describe(cmd, 21, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_descibe failed!!\n");
            }
            ret = ct_bind(cmd, 21, datafmt,
                          tag, &datalength[20], &indicator[20]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed TAG!!\n");
            }
            ret = ct_describe(cmd, 22, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_descibe failed!!\n");
            }
            ret = ct_bind(cmd, 22, datafmt,
                          &tif, &datalength[21], &indicator[21]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed TIF!!\n");
            }
            ret = ct_describe(cmd, 23, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_bind_failed expireDate \n");
            }
            ret = ct_bind(cmd, 23, datafmt,
                          expireDate, &datalength[22], &indicator[22]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed expireDate!!\n");
            }

            ret = ct_describe(cmd, 24, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_bind_failed shortLender\n");
            }
            ret =
                ct_bind(cmd, 24, datafmt, short_lender, &datalength[23],
                        &indicator[23]);

            ret = ct_describe(cmd, 25, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_bind_failed cplxtype\n");
            }
            ret =
                ct_bind(cmd, 25, datafmt, &cplxtype, &datalength[24],
                        &indicator[24]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed for cplxtype");
            }
            ret = ct_describe(cmd, 26, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_bind_failed parentOmTag\n");
            }
            ret =
                ct_bind(cmd, 26, datafmt, parent_om_tag, &datalength[25],
                        &indicator[25]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed for parentOmTag");
            }
            ret = ct_describe(cmd, 27, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_bind_failed mm_exchange\n");
            }
            ret =
                ct_bind(cmd, 27, datafmt, &mm_exchange, &datalength[26],
                        &indicator[26]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed for mm_exchange");
            }
            ret = ct_describe(cmd, 28, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_bind_failed open_close\n");
            }
            ret =
                ct_bind(cmd, 28, datafmt, open_close, &datalength[27],
                        &indicator[27]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed for open_close");
            }
            ret = ct_describe(cmd, 29, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_bind_failed trade_for\n");
            }
            ret =
                ct_bind(cmd, 29, datafmt, trade_for, &datalength[28],
                        &indicator[28]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed for trade_for");
            }
            ret = ct_describe(cmd, 30, datafmt);
            if (ret != CS_SUCCEED) {
                printf("ct_bind_failed expDay\n");
            }
            ret =
                ct_bind(cmd, 30, datafmt, expDay, &datalength[29],
                        &indicator[29]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed for expDay");
            }
            if (ct_describe(cmd, 31, datafmt) != CS_SUCCEED) {
                printf("ct_describe failed for DestID\n");
            }
            if (ct_bind(cmd, 31, datafmt, &destination, &datalength[30],
                        &indicator[30]) != CS_SUCCEED) {
                printf("ct_bind failed for DestID\n");
            }
            if (ct_describe(cmd, 32, datafmt) != CS_SUCCEED) {
                printf("ct_describe failed for sec_def\n");
            }
            ret =
                ct_bind(cmd, 32, datafmt, sec_def, &datalength[31],
                        &indicator[31]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed for sec_def");
            }

            if (ct_describe(cmd, 33, datafmt) != CS_SUCCEED) {
                printf("ct_describe failed for trader\n");
            }
            ret =
                ct_bind(cmd, 33, datafmt, trader, &datalength[32],
                        &indicator[32]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed for trader");
            }

            if (ct_describe(cmd, 34, datafmt) != CS_SUCCEED) {
                printf("ct_describe failed for smpid\n");
            }
            ret =
                ct_bind(cmd, 34, datafmt, smpid, &datalength[33],
                        &indicator[33]);
            if (ret != CS_SUCCEED) {
                printf("ct_bind failed for smpid");
            }
            count = 1;
            while (((ret = ct_fetch(cmd, CS_UNUSED, CS_UNUSED,
                                    CS_UNUSED, &count)) == CS_SUCCEED) ||
                   (ret == CS_ROW_FAIL)) {
                if (ret == CS_ROW_FAIL) {
                    /**
                     *
    CS_CHAR open_close[5];
    CS_BOOL fmt_exp = CS_TRUE;
                     *
                     */
                    printf
                        ("ERROR: State_len: %dCount %d, RET: %d, Capacity: %s, Symbol: %s, om tag: %s, cp: %s, "
                         "account: %s, id: %s, CMTA: %s, cum: %d, EXTAG: %s, Leave: %d, "
                         "floor: %d, MMACT: %s, OTag: %s, OType: %d, price %f, qty: %d, "
                         "secType: %s, side %d, stop: %f, strike: %f, tag: %s, tif: %d, "
                         "expDate: %s, lender: %s, cplxtype: %d, parent: %s, mmex: %d, "
                         "tradefor: %s, trader: %s, expDay: %s, sec_def: %s, smpid: %s, dest: %d\n",state_len, count,ret,
                         capacity, symbol, omTag, callPut, clearingAccount,
                         clearingID, CMTA, cumQty, exchangeTag, leavesQTY, maxFloor,
                         mmAcct,orderTag, orderType, price, qty, secType, side, stopPrice,
                         strikePrice, tag, tif, expireDate, short_lender, cplxtype,
                         parent_om_tag, mm_exchange, trade_for, trader, expDay, sec_def,
                         smpid, destination);
                } else {
                    dart_ex_result_set *ders =
                        calloc(1,sizeof(struct dart_ex_result_set));
                    memset(ders, 0, sizeof(struct dart_ex_result_set));
                    memcpy(ders->callPut, callPut, 1);
                    memcpy(ders->capacity, capacity, MAXSTRING);
                    memcpy(ders->clearingAccount, clearingAccount, 30);
                    memcpy(ders->clearingID, clearingID, 30);
                    memcpy(ders->CMTA, CMTA, 29);
                    ders->cumQty = cumQty;
                    memcpy(ders->exchangeTag, exchangeTag, 126);
                    ders->leavesQTY = leavesQTY;
                    ders->maxFloor = maxFloor;
                    memcpy(ders->mmAcct, mmAcct, 30);
                    memcpy(ders->omTag, omTag, MAXSTRING);
                    memcpy(ders->orderTag, orderTag, MAXSTRING);
                    ders->orderType = orderType;
                    ders->price = price;
                    ders->qty = qty;
                    memcpy(ders->secType, secType, 4);
                    ders->side = side;
                    ders->stopPrice = stopPrice;
                    ders->strikePrice = strikePrice;
                    memcpy(ders->symbol, symbol, MAXSTRING);
                    memcpy(ders->tag, tag, 127);
                    ders->tif = tif;
                    memcpy(ders->expireDate, expireDate, MAXSTRING);
                    memcpy(ders->short_lender, short_lender, 12);
                    ders->cplxtype = cplxtype;
                    memcpy(ders->parent_om_tag, parent_om_tag, 125);
                    ders->mm_exchange = mm_exchange;
                    memcpy(ders->open_close, open_close, 1);
                    memcpy(ders->trade_for, trade_for, 20);
                    memcpy(ders->trader, trader, 20);
                    memcpy(ders->expDay, expDay, 2);
                    memcpy(ders->sec_def, sec_def, 127);
                    memcpy(ders->smpid, smpid, 16);
                    snprintf(ders->destination, sizeof ders->destination,
                             "%d", destination);
                    ders->destination[sizeof ders->destination - 1] = '\0';
                    enqueue(results, ders);
                    memset(callPut, '\0', 3);
                    memset(capacity, '\0', MAXSTRING);
                    memset(clearingAccount, '\0', 32);
                    memset(clearingID, '\0', 32);
                    memset(CMTA, '\0', 30);
                    memset(exchangeTag, '\0', 128);
                    memset(mmAcct, '\0', 32);
                    memset(omTag, '\0', MAXSTRING);
                    memset(orderTag, '\0', MAXSTRING);
                    memset(secType, '\0', 4);
                    memset(symbol, '\0', MAXSTRING);
                    memset(tag, '\0', 128);
                    memset(expireDate, '\0', MAXSTRING);
                    memset(short_lender, '\0', sizeof short_lender);
                    memset(parent_om_tag, '\0', 128);
                    memset(open_close, '\0', 5);
                    memset(trade_for, '\0', 30);
                    memset(trader, '\0', 30);
                    memset(smpid, '\0', 30);
                    memset(expDay, '\0', 30);
                    memset(sec_def, '\0', 128);
                }
            }
            break;
        case CS_CMD_SUCCEED:
            printf("No rows returned.\n");
            break;
        case CS_CMD_FAIL:
            printf("Got a CMD FAIL.\n");
            break;
        case CS_CMD_DONE:
            break;
        default:
            printf("ct_results returned unexpected result type.\n");
            break;
        }
    }

    switch ((int) results_ret) {
    case CS_END_RESULTS:
        break;
    case CS_FAIL:
        printf("ct_results returned a fail.\n");
        break;
    default:
        printf("ct_results returned unexpected return code\n");
        break;
    }

    ret = ct_cmd_drop(cmd);
    if (ret != CS_SUCCEED) {
        printf("ct_cmd_drop failed!!\n");
    }
    ret = ct_close(connection, CS_UNUSED);
    if (ret != CS_SUCCEED) {
        printf("ct_close failed!!\n");
    }
    ret = ct_con_drop(connection);
    if (ret != CS_SUCCEED) {
        printf("ct_con_drop failed!!\n");
    }
    ret = ct_exit(context, CS_UNUSED);
    if (ret != CS_SUCCEED) {
        printf("ct_exit failed!!\n");
    }
    ret = cs_ctx_drop(context);
    if (ret != CS_SUCCEED) {
        printf("ct_ctx_drop failed!!\n");
    }
    free(statement);
    return 0;
}

CS_RETCODE CS_PUBLIC servermsg_callback(CS_CONTEXT * cp,
                                        CS_CONNECTION * chp,
                                        CS_SERVERMSG * msgp)
{
    printf("servermsg_callback called!!");
    return (CS_SUCCEED);
}

CS_RETCODE CS_PUBLIC clientmsg_callback(CS_CONTEXT * cp,
                                        CS_CONNECTION * chp,
                                        CS_CLIENTMSG * msgp)
{
    printf("clientmsg_callback called!!");
    return (CS_SUCCEED);
}

CS_RETCODE CS_PUBLIC csmsg_callback(CS_CONTEXT * cp, CS_CLIENTMSG * emsgp)
{
    printf("csmsg_callback called!!");
    return (CS_SUCCEED);
}
