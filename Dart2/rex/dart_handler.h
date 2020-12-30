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

#include "polling_server.h"
#include "message_queue.h"
#include "dart_order_obj.h"
#include "log_mnger.h"
#include "dart_order_fact.h"

typedef struct dart_parser dartp;

dartp* create_dart_parser(void* pass_back_pointer,void (*dp_con_notice)(dartp* dp,
                                 int is_connected, dart_order_obj* logon),
                          void (*call_back)(void* pbp, dart_order_obj* doj),
                          dartfact* d, otl* o, int num_obj, int resend, int is_server);
void set_callback(dartp* dp, void(*call_back)(void* pbp, dart_order_obj* doj));

unsigned long build_offsets(char *data,
                            unsigned long size,
                            unsigned long byte_offset,
                            unsigned long *seq_num, void* b);

void send_dart_logon(void* dp);
void send_dart_logon_for_user(dartp* dp, char* name, int nlen);

char *create_rom_resend_pad(unsigned long seq, char *rom, size_t * len);

size_t find_rom_offsets(char *data, size_t * seq,
                        size_t size, size_t byte_offset,
                        size_t beg_seq, size_t end_seq,
                        size_t * beg_off, size_t * end_off);
char *pad_rom_resend(char *rom, size_t len);

void ph_parse_rom_message(int *cut_con, rex_con* rc);
                                   
void ph_build_iovec(dart_order_obj * doj);

void send_dart_message(dartp* dp, dart_order_obj* doj);

void send_raw_csv(dartp* dp, char* data, int len);

void dart_write_callback(void* dp);

void dart_set_rex_con(dartp* dp, rex_con* rc);

rex_con* get_rex_con(dartp* dp);

void set_logged_on(dartp* dp, int is_logged_on);
int get_logged_on(dartp* dp);
void clean_dart_parser(dartp* dp);

void dp_return_doj(dartp* dp, dart_order_obj* doj);

//dart_order_obj* dp_get_doj(dartp* dp);

void reload_dartp_config(dartp* dp);

void* get_pass_back_point(dartp* dp);

void reset_pass_back_point(dartp* dp, void* pbp);

void dart_reconnect(dartp* dp);

int get_sock(dartp* dp);
#if defined(__cplusplus)
}
#endif
#endif
