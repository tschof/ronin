/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/


#ifndef _NYSE_ORDER_TRACKER_H__
#define _NYSE_ORDER_TRACKER_H__

#include "dart_orderbook.h"
#include "client_manager.h"
#include "databuf.h"

#if defined(__cplusplus)
extern "C" {
#endif

    struct database_conf;
    void nyse_from_rom_func(struct message *mess, void *b);

    void init_nyse_tracker(char *obo_comp_id, int obo_len,
                           struct database_conf *, const char *destID,
                           int d_len, orderbook * ob, struct init_args* in);

    long nyse_fix_to_rom_override(struct message *mess, char *fix_message,
                                  databuf_t * output, long fix_len,
                                  long out_len, char rom_type,
                                  struct dart_rom_pos positions[ROM_LEN],
                                  int *rejeceted, orderbook * ob);

    long nyse_rom_to_fix_override(struct message *mess, char *fix_message,
                                  databuf_t * output, long fix_len,
                                  long out_len, char *fix_type,
                                  struct dart_fix_pos positions[ROM_LEN],
                                  int *rejected, orderbook * ob);


    int set_nyse_type_param(char *b_off, struct dart_fix_pos *positions,
                            char *mtype, databuf_t * d_off,
                            size_t rom_off, int *rejected,
                            dart_ex_order_obj * obj);
    int set_nyse_capacity(char *b_off,
                          struct dart_fix_pos positions[ROM_LEN],
                          char *mtype, databuf_t * d_off,
                          size_t romfield, int *rejected,
                          dart_ex_order_obj * obj);

    int set_nyse_defaults(char *rommsg, struct dart_fix_pos *positions,
                          char *mtype, databuf_t * fix,
                          size_t romfield, int *rejected,
                          dart_ex_order_obj * order);
    int nyse_side_override(char *rommsg, struct dart_fix_pos *positions,
                           char *mtype, databuf_t * output,
                           size_t i, int *rejected,
                           dart_ex_order_obj * order);
    int nyse_symbol_override(char *rommsg, struct dart_fix_pos *positions,
                             char *mtype, databuf_t * output,
                             size_t i, int *rejected,
                             dart_ex_order_obj * order);
    int nyse_account_override(char *rommsg, struct dart_fix_pos *positions,
                              char *mtype, databuf_t * output,
                              size_t i, int *rejected,
                              dart_ex_order_obj * order);
    int set_nyse_exTag(char *mess, struct dart_fix_pos *positions,
                       char *mtype, databuf_t * output, size_t i,
                       int *rejected, dart_ex_order_obj * ob);
    int set_nyse_display(char *rommsg, struct dart_fix_pos *positions,
                         char *mtype, databuf_t * output,
                         size_t i, int *rejected,
                         dart_ex_order_obj * order);
#if defined(__cplusplus)
}
#endif
#endif                          //_NYSE_ORDER_TRACKER_H__
