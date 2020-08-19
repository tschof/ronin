/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ex_common.h"
#include "options_common.h"
#include "db_layer.h"
#include "generic_db.h"
#include "../ex_db/database_conf.h"

static int get_cum_shares(struct database_conf *dbconf, char *omtag)
{
    int ret = 0;
    char sql_str[2000];
    sprintf(sql_str,
            "declare @lastRep       DATETIME "
            "DECLARE @romtag        NUMERIC(19,0) "
            "SELECT @romtag = (SELECT _OrdNum FROM Orders WHERE omTag = '%s') "
            "SELECT @lastRep = max (_timeStamp) FROM (SELECT _timeStamp FROM OrderHistory "
            "WHERE status = 5 AND _OrdNum = @romtag)dt_OrderHistory "
            "SELECT sum (exQty) FROM Executions "
            "WHERE _timeStamp < @lastRep AND _OrdNum = @romtag", omtag);
    queue_t q = create_sized_queue(6);
    gendb_get_results(dbconf->username, strlen(dbconf->username),
                      dbconf->password, strlen(dbconf->password),
                      dbconf->server, strlen(dbconf->server), q, sql_str);
    queue_t row = NULL;
    gendb_v *val = NULL;
    element_t erow, ecol;
    while (!is_empty(q)) {
        int cnt = 0;
        int local_int = 0;
        erow = pop_front(q);
        row = (queue_t) (erow->d);
        ecol = pop_front(row);
        val = (gendb_v *) (ecol->d);
        local_int = ((val->g_un).un_int);
        ret = local_int;
        ++cnt;
        free(ecol);
        free(val);
    }
    destroy_queue(q);
    return ret;
}

void opt_database_init(struct database_conf *dbconf, const char *destid,
                       orderbook * ob, int query_cum)
{
    int res = 0;
    element_t e, temp;
    queue_t q = create_queue();
    queue_t childrens = create_queue();
    e = NULL;
    temp = NULL;
    res = get_db_objects(dbconf, q, destid);
    e = dequeue(q);
    while (e != NULL) {
        struct dart_ex_result_set *ders;
        dart_ex_order_obj *obj;
        databuf_t *buff = databuf_alloc(500);
        char *ord_id = NULL;
        char *rom_id = NULL;
        int rom_id_len = 0;
        size_t rom_inst_len = 0;
        char *rom_inst = NULL;
        int s_len = 0;
        size_t len = 0;
        if (buff == NULL) {
            printf("Calloc failed in init_nyse_tracker.\n");
            return;
        }

        ders = NULL;
        ders = (struct dart_ex_result_set *) e->d;
        if (ders != NULL) {
            size_t cl_len = 0;
            size_t rom_len = 0;
            double st = 0;
            char strst[30];
            char mm_exch[10];
            obj = get_empty_dart_order_obj(ob->mgr);
            doa_set_order_qty(obj, ders->qty);
            doa_set_cl_ord_id(obj, ders->orderTag, strlen(ders->orderTag));
            rom_id_len = strlen(ders->omTag);
            doa_set_rom_id(obj, ders->omTag, rom_id_len);
            rom_inst =
                build_rom_name(ders->omTag, (size_t) rom_id_len,
                               &rom_inst_len);
            if (rom_inst != NULL) {
                doa_set_sender_name(obj, rom_inst, rom_inst_len);
                free(rom_inst);
            }
            if (query_cum) {
                int l_cum = get_cum_shares(dbconf, ders->omTag);
                doa_set_offset_cum(obj, l_cum);
            }
            doa_set_exchange_tag(obj, ders->exchangeTag,
                                 strlen(ders->exchangeTag));
            doa_set_leaves_shares(obj, ders->leavesQTY);
            doa_set_cum_shares(obj, ders->cumQty);
            doa_set_delta(obj, 0);
            doa_set_pending_replace(obj, 0);
            databuf_reset(buff);
            s_len = databuf_write(buff, "%f", ders->price);
            doa_set_price(obj, buff->buffer, s_len);
            databuf_reset(buff);
            s_len = databuf_write(buff, "%d", ders->side);
            doa_set_side(obj, buff->buffer, s_len);
            databuf_reset(buff);
            doa_set_symbol(obj, ders->symbol, strlen(ders->symbol));
            if(ders->sec_def) {
                doa_set_sec_def(obj, ders->sec_def, strlen(ders->sec_def));
            }
            s_len = databuf_write(buff, "%d", ders->orderType);
            doa_set_type(obj, buff->buffer, s_len);
            databuf_reset(buff);
            doa_set_capacity(obj, ders->capacity, strlen(ders->capacity));
            s_len = databuf_write(buff, "%d", ders->tif);
            doa_set_tif(obj, buff->buffer, s_len);
            databuf_reset(buff);
            int clear_acc_len = strlen(ders->clearingAccount);
            if (clear_acc_len == 8
                && (strncmp((ders->clearingAccount + 4), "1209", 4) ==
                    0)) {
                doa_set_account(obj, ders->clearingAccount, 4);
            } else {
                doa_set_account(obj, ders->clearingAccount, clear_acc_len);
            }
            doa_set_trade_for(obj, ders->trade_for,
                              strlen(ders->trade_for));
            doa_set_trader(obj, ders->trader,
                            strlen(ders->trader));
            if(ders->smpid) {
                doa_set_smpid(obj, ders->smpid, strlen(ders->smpid));
            }
            ord_id = doa_get_cl_ord_id(obj, &cl_len);

            /* We need to add the options specific fields here.  Also
             * no inserts for the legs. */
            len = 0;
            rom_id = doa_get_rom_id(obj, &rom_len);
            doa_set_expireMonYear(obj, ders->expireDate,
                                  strlen(ders->expireDate));
            doa_set_expire_day(obj, ders->expDay, strlen(ders->expDay));
            doa_set_open_close(obj, ders->open_close,
                               strlen(ders->open_close));
            doa_set_sec_type(obj, ders->secType, 1);
            if (ders->CMTA != 0) {
                doa_set_cmta(obj, ders->CMTA, strlen(ders->CMTA));
            }
            st = ders->strikePrice;
            sprintf(strst, "%f", st);
            doa_set_strike(obj, strst, strlen(strst));
            doa_set_call_put(obj, ders->callPut, strlen(ders->callPut));
            doa_set_mm_account(obj, ders->mmAcct, strlen(ders->mmAcct));
            sprintf(mm_exch, "%d", ders->mm_exchange);
            doa_set_mm_home(obj, mm_exch, strlen(mm_exch));
            doa_set_account(obj, ders->clearingAccount,
                            strlen(ders->clearingAccount));
            doa_set_short_locate(obj, ders->short_lender,
                                 strlen(ders->short_lender));
            doa_set_clr_id(obj, ders->clearingID,
                           strlen(ders->clearingID));
            doa_set_parent_id(obj, ders->parent_om_tag,
                              strlen(ders->parent_om_tag));
            doa_set_destination(obj, ders->destination,
                                strlen(ders->destination));
            if (ders->cplxtype > 0) {
                doa_set_is_spread(obj, 1);
            }
            if (ders->cplxtype != 6) {
                no_copy_insert(ob->cl_ord_id_map, ord_id, cl_len, obj);
                no_copy_insert(ob->rom_ord_id_map, rom_id, rom_len, obj);
            } else {
                enqueue(childrens, obj);
            }
        }
        if (ord_id != NULL) {
            free(ord_id);
        }
        if (rom_id != NULL) {
            free(rom_id);
        }
        temp = e;
        e = temp->next;
        if (ders != NULL) {
            free(ders);
        }
        temp->d = NULL;
        free(temp);
        temp = NULL;
        databuf_destroy(buff);
    }
    destroy_queue(q);
    e = dequeue(childrens);
    while (e != NULL) {
        dart_leg_obj *dlo = calloc(sizeof(struct dart_leg_obj), 1);
        dart_ex_order_obj *doj = (dart_ex_order_obj *) e->d;
        if (doj != NULL) {
            size_t tlen = 0;
            char *parent_tag = doa_get_parent_id(doj, &tlen);
            if (parent_tag != NULL) {
                dart_ex_order_obj *poj = 0;
                int ret_code =
                    get_map_entry(ob->rom_ord_id_map, parent_tag, tlen,
                                  (void *) &poj);
                if (ret_code > 0) {
                    char *ctemp = 0;
                    char *cday = 0;
                    char *t_exdb = calloc(12, 1);
                    size_t dlen = 0;
                    ctemp = doa_get_symbol(doj, &tlen);
                    memcpy(dlo->symbol, ctemp, tlen);
                    free(ctemp);
                    ctemp = 0;
                    ctemp = doa_get_call_put(doj, &tlen);
                    memcpy(dlo->call_put, ctemp, tlen);
                    free(ctemp);
                    ctemp = 0;
                    ctemp = doa_get_expireMonYear(doj, &tlen);
                    cday = doa_get_expire_day(doj, &dlen);
                    if (tlen > 0 && dlen > 0 && ctemp && cday) {
                        dlen = sprintf(t_exdb, "%s%s", ctemp, cday);
                    }
                    memcpy(dlo->expire_date, t_exdb, dlen);
                    free(ctemp);
                    ctemp = 0;
                    free(cday);
                    cday = 0;
                    free(t_exdb);
                    t_exdb = 0;
                    int shares = (int) doa_get_order_qty(doj);
                    sprintf(dlo->ratio, "%d", shares);
                    ctemp = doa_get_side(doj, &tlen);
                    memcpy(dlo->side, ctemp, tlen);
                    free(ctemp);
                    ctemp = 0;
                    ctemp = doa_get_open_close(doj, &tlen);
                    memcpy(dlo->open_close, ctemp, tlen);
                    free(ctemp);
                    ctemp = 0;
                    ctemp = doa_get_strike(doj, &tlen);
                    memcpy(dlo->strike, ctemp, tlen);
                    free(ctemp);
                    ctemp = 0;
                    ctemp = doa_get_cl_ord_id(doj, &tlen);
                    memcpy(dlo->id, ctemp, tlen);
                    free(ctemp);
                    ctemp = 0;
                    ctemp = doa_get_rom_id(doj, &tlen);
                    memcpy(dlo->rom_id, ctemp, tlen);
                    free(ctemp);
                    ctemp = 0;
                    doa_add_child(poj, dlo);
                }
                free(parent_tag);
            }
            destroy_dart_order(doj);
        } else {
            free(dlo);
        }
        temp = e;
        e = temp->next;
        temp->d = NULL;
        free(temp);
        temp = NULL;
    }
    destroy_queue(childrens);
}
