/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _ROM_HANDLER_H__
#define _ROM_HANDLER_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "message_queue.h"
#include "client_manager.h"
#include "dart_orderbook.h"

    unsigned long build_offsets(char *data,
                                unsigned long size,
                                unsigned long byte_offset,
                                unsigned long *seq_num);

    unsigned long parse_rom_message(const char *message,
                                    unsigned long len,
                                    int *cut_con, struct parse_con *pc);

    int init_rom_logon(struct parse_args *pc);

    int init_rom_logon_no_seq(struct parse_args *pc);

    int init_async_rom_logon(struct async_parse_args *pc);

    char *create_rom_resend_pad(unsigned long seq, char *rom,
                                size_t * len);

    void handle_csv_logon(char type, const char *data, unsigned long len,
                          int *cut_con, struct parse_con *pc);

    size_t find_rom_offsets(char *data, size_t * seq,
                            size_t size, size_t byte_offset,
                            size_t beg_seq, size_t end_seq,
                            size_t * beg_off, size_t * end_off);

    char *pad_rom_resend(char *rom, size_t len);

    void ph_build_dart_obj(long rom_len,
                                  struct rom_parser_helper *rph, void *pc);


    unsigned long ph_parse_rom_message(const char *raw_bytes,
                                       unsigned long len, int *cut_con,
                                       struct parse_con *pc);

#if defined(__cplusplus)
}
#endif
#endif                          //ROM_HANDLER_H__
