/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _COMPLEX_H__
#define _COMPLEX_H__

#if defined(__cplusplus)
extern "C" {
#endif
#include "client_manager.h"
#include "dart_orderbook.h"
#include "databuf.h"



     void fs_set_spread_book(orderbook * ob);
     void set_rom2fix_leg_book(orderbook * ob);
    //void set_rom2fix_leg_book2(orderbook* ob);
     void complex_from_rom_func(struct message *mess, void *b);
     long from_rom_determine_spread(struct message *mess,
                                                char *rom_message,
                                                long rom_len,
                                                databuf_t * output,
                                                char *fix_type,
                                                int *ft_len,
                                                int *rejected,
                                                orderbook * ob,
                                                long (*rom_to_fix_override)
                                                 (struct message * mess,
                                                  char *fix_message,
                                                  databuf_t * output,
                                                  long fix_len,
                                                  char *fix_type,
                                                  struct dart_fix_pos
                                                  positions[ROM_LEN],
                                                  int *rejected,
                                                  orderbook * ob, int
                                                  is_spread));
     int get_mat_month_year(char *fix_mess, size_t len,
                                        int rom_tag, databuf_t * output);
     int break_n_set_maturity_day(char *fix_mess, size_t len,
                                              int rom_tag,
                                              databuf_t * output);
     int set_leg_value(char *fix_mess, size_t len,
                                   char *fix_tag, size_t t_len,
                                   databuf_t * output);
     int set_leg_symbol(char *fix_mess, size_t len,
                                    int rom_tag, databuf_t * output);
     int set_leg_sec_id(char *fix_mess, size_t len,
                                    int rom_tag, databuf_t * output);

     int set_cfi_code(char *fix_mess, size_t len, int rom_tag,
                                  databuf_t * output);
     int set_leg_strike(char *fix_mess, size_t len,
                                    int rom_tag, databuf_t * output);
     int set_leg_sec_type(char *fix_mess, size_t len,
                                      int rom_tag, databuf_t * output);
     int set_leg_pos_eff(char *fix_mess, size_t len,
                                     int rom_tag, databuf_t * output);
     int set_leg_sec_desc(char *fix_mess, size_t len,
                                      int rom_tag, databuf_t * output);
     int set_leg_side(char *fix_mess, size_t len, int rom_tag,
                                  databuf_t * output);
     int set_leg_ratio(char *fix_mess, size_t len, int rom_tag,
                                   databuf_t * output);
     int set_leg_multi(char *fix_mess, size_t len, int rom_tag,
                                   databuf_t * output);
     long check_for_spreads(struct message *mess,
                                        char *fix_message,
                                        databuf_t * output, long fix_len,
                                        char rom_type, struct dart_rom_pos
                                        positions[ROM_LEN], int *rejected,
                                        orderbook * ob);
     long fix_rom_spread_override(struct message *mess,
                                              char *fix_message,
                                              databuf_t * output,
                                              long fix_len, char rom_type,
                                              struct dart_rom_pos
                                              positions[ROM_LEN],
                                              int *rejected,
                                              orderbook * ob);
     int trans_mleg_put_call(char *rom, dart_fix_pos * map,
                                         char *mtype, databuf_t * fix,
                                         size_t field, int *rejected,
                                         dart_ex_order_obj * order,
                                         dart_leg_obj * l_obj);

    int trans_mleg_sec_type(char *rom, dart_fix_pos * map,
                        char *mtype, databuf_t * fix,
                        size_t field, int *rejected,
                        dart_ex_order_obj * order, dart_leg_obj * l_obj);
     int trans_leg_mat_date(char *rom, dart_fix_pos * map,
                                        char *type, databuf_t * fix,
                                        size_t field, int *rejected,
                                        dart_ex_order_obj * order,
                                        dart_leg_obj * l_obj);
     int trans_mleg_stike(char *rom, dart_fix_pos * map,
                                      char *mtype, databuf_t * fix,
                                      size_t field, int *rejected,
                                      dart_ex_order_obj * order,
                                      dart_leg_obj * l_obj);
     int trans_mleg_symbol(char *rom, dart_fix_pos * map,
                                       char *mtype, databuf_t * fix,
                                       size_t field, int *rejected,
                                       dart_ex_order_obj * order,
                                       dart_leg_obj * l_obj);
     int trans_mleg_side(char *rom, dart_fix_pos * map,
                                     char *mtype, databuf_t * fix,
                                     size_t field, int *rejected,
                                     dart_ex_order_obj * order,
                                     dart_leg_obj * l_obj);
     int trans_mleg_ratio(char *rom, dart_fix_pos * map,
                                      char *mtype, databuf_t * fix,
                                      size_t field, int *rejected,
                                      dart_ex_order_obj * order,
                                      dart_leg_obj * l_obj);
     int trans_mleg_open_close(char *rom, dart_fix_pos * map,
                                           char *mtype, databuf_t * fix,
                                           size_t field, int *rejected,
                                           dart_ex_order_obj * order,
                                           dart_leg_obj * l_obj);
     void set_option_fix_to_rom(orderbook * ob);
     int def_option_ord_ids(char *fixmsg,
                                        dart_rom_pos * positions,
                                        char mtype, char *status,
                                        databuf_t * rom, size_t romfield,
                                        int *rejtected,
                                        dart_ex_order_obj * order,
                                        pthread_mutex_t * mutex,
                                        hashmap_t ord_map);
     int def_option_rom_tag(char *fixmsg,
                                        dart_rom_pos * positions,
                                        char mtype, char *status,
                                        databuf_t * rom, size_t romfield,
                                        int *rejtected,
                                        dart_ex_order_obj * order,
                                        pthread_mutex_t * mutex,
                                        hashmap_t ord_map);
long deconstruct_rom_spread(struct message *mess,
                            char *rom, long rom_len, char *leg_off,
                            databuf_t * output,
                            char *fix_type, int *ft_len, int *rejected,
                            orderbook * ob, int id_last);
void build_pos_array(struct dart_fix_pos *l_positions,
                            struct fix_tag_def *ftd[ROM_LEN]);
void build_pos_array2(struct dart_fix_pos* l_positions,
                             struct fix_tag_def* ftd[ROM_LEN],
                             int leg_array_size);
struct fix_tag_def **get_tag_def_bundle_only(char rom_type, orderbook * ob);
void set_fix_type(char rom_type, char *fix_type, int* ft_len, int is_spread, int no_fix_43);
#if defined(__cplusplus)
}
#endif
#endif                          //_MAILER_H__
