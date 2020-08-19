/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _FIX_ROM_TRANS_H__
#define _FIX_ROM_TRANS_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "transaction_recorder.h"
#include "hashmap.h"
#include "databuf.h"
#include "dart_orderbook.h"

    void initalize_maps(orderbook * b);

    hashmap_t get_map_for_type(char type, char *rom_type, orderbook * ob);

    long default_fix_to_rom_override(struct message *mess,
                                     char *fix_message,
                                     databuf_t * output,
                                     long fix_len,
                                     char rom_type, struct dart_rom_pos
                                     positions[ROM_LEN],
                                     int *rejected, orderbook * ob);

    long default_rom_to_fix_override(struct message *mess,
                                     char *fix_message,
                                     databuf_t * output,
                                     long fix_len,
                                     char *fix_type, struct dart_fix_pos
                                     positions[ROM_LEN],
                                     int *rejected, orderbook * ob, int is_spread);

    long trans_fix_message(struct message *mess,
                           char *fix_message, long fix_len,
                           databuf_t * output,
                           int *rejected, orderbook * ob,
                           long (*fix_to_rom_override) (struct
                                                        message *
                                                        mess,
                                                        char
                                                        *fix_message,
                                                        struct
                                                        databuf *
                                                        output,
                                                        long
                                                        fix_len,
                                                        char
                                                        rom_type,
                                                        struct
                                                        dart_rom_pos
                                                        positions
                                                        [ROM_LEN],
                                                        int
                                                        *rejected,
                                                        orderbook * b));

    long trans_rom_message(struct message *mess,
                           char *rom_message, long rom_len,
                           databuf_t * output,
                           char *fix_type, int *ft_len,
                           int *rejected, orderbook * ob,
                           long (*rom_to_fix_override) (struct
                                                        message *
                                                        mess,
                                                        char
                                                        *fix_message,
                                                        databuf_t
                                                        * output,
                                                        long
                                                        fix_len,
                                                        char
                                                        *fix_type,
                                                        struct
                                                        dart_fix_pos
                                                        positions
                                                        [ROM_LEN],
                                                        int
                                                        *rejected,
                                                        orderbook * ob,
                                                        int is_spread));

    long translate_put_call(char val, databuf_t * output);

    long translate_sec_type(char *val, databuf_t * output, long val_len);

    long determine_trans_type(int rom_tag, char *value,
                              databuf_t * output, long value_len);

    void tpos_symbol_trans(const char *fix_message,
                           databuf_t * output, char *fix_type,
                           const struct dart_fix_pos positions[],
                           int sym_off, int sec_type_off);

    long rom_breakdown(struct message *mess, char *rom_message,
                       long rom_len, databuf_t * output,
                       struct fix_tag_def *ftd[ROM_LEN],
                       struct dart_fix_pos l_positions[ROM_LEN],
                       char *fix_type, int *rejected,
                       orderbook * ob,
                       long (*rom_to_fix_override) (struct message
                                                    * mess,
                                                    char
                                                    *fix_message,
                                                    databuf_t *
                                                    output,
                                                    long fix_len,
                                                    char
                                                    *fix_type,
                                                    struct
                                                    dart_fix_pos
                                                    positions
                                                    [ROM_LEN],
                                                    int *rejected,
                                                    orderbook * b,
                                                    int is_spread));

    void build_rom_pos2(char *rom_message, long rom_len,
                        struct fix_tag_def *ftd[ROM_LEN],
                        struct dart_fix_pos *l_positions, long offset);

    void build_dart_positions(struct dart_rom_pos l_positions[],
                              char *fix_message, long fix_len,
                              hashmap_t trans_map);

    char *get_file_contents(const char *filename, long *size);

    int build_map_from_file(const char *filename, hashmap_t map);

    int is_month_code(char *code);

    size_t get_symbol_len(char *sym, size_t s_len);

    void create_month_codes();

    int get_base_sym_len(const char *symbol, int sym_len);

#if defined(__cplusplus)
}
#endif
#endif                          //FIX_ROM_TRANS_H__
