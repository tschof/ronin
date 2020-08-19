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

#include "async_server.h"
#include "message_queue.h"
#include "dart_order_obj.h"

typedef struct rom_parser_helper {
    dart_order_obj *cur_doj;
    unsigned long read_bytes;
    unsigned long mess_bytes;
    char *d_off;
    char *message_beg;
    int rom_tag;
    void (*parse_v) (long len, struct rom_parser_helper * rph, void *pc);
} rom_parser_helper;

unsigned long build_offsets(char *data,
                            unsigned long size,
                            unsigned long byte_offset,
                            unsigned long *seq_num, void* b);

int init_async_rom_logon(struct async_parse_args *pc);

char *create_rom_resend_pad(unsigned long seq, char *rom, size_t * len);

size_t find_rom_offsets(char *data, size_t * seq,
                        size_t size, size_t byte_offset,
                        size_t beg_seq, size_t end_seq,
                        size_t * beg_off, size_t * end_off);
char *pad_rom_resend(char *rom, size_t len);

void ph_build_non_fix_rom_pos(char *raw, long rom_len, void *pc);
void handle_async_csv_logon(char type, const char *data,
                            unsigned long len, int *cut_con,
                            struct async_parse_args *pc);
void ph_parse_rom_message(int *cut_con,
                                   struct async_parse_args *pc);
void ph_build_iovec(dart_order_obj * doj);

#if defined(__cplusplus)
}
#endif
#endif
