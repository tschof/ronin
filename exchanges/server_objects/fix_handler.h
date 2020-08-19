/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _FIX_HANDLER_H__
#define _FIX_HANDLER_H__

#include <pthread.h>
#include <time.h>

#include "message_queue.h"
#include "client_manager.h"
#include "transaction_recorder.h"
#include "dart_fix_connection_object.h"
#include "dart_connection_object.h"

#if defined(__cplusplus)
extern "C" {
#endif

    unsigned long build_fix_offsets(char *data,
                                    unsigned long size,
                                    unsigned long byte_offset,
                                    unsigned long *seq_num);

    unsigned long parse_fix_message(const char *rd_ptr,
                                    unsigned long len,
                                    int *cut_con, struct parse_con *pc);

    int send_logon(struct parse_args *pa);

    int send_con_logon(struct parse_con *con, app_end_time * t,
                       char *tag_list, int len);

    int send_fix_message(struct message *m, char *type, size_t type_len);

    void low_activity_sig(con_obj * co);

    void fix_connection_callback(con_obj * co, int is_connected);

    fix_con_obj *build_fix_con_obj(struct parse_args *pa);

    char *create_fix_resend_pad(unsigned long seq,
                                char *fix_version, size_t * len);

    int send_gen_logon(struct parse_args *pa, char *tag_list, int len);

    size_t create_fix_header(databuf_t * fixmsg,
                             const char *sendercompid,
                             size_t sendercomplen,
                             const char *targetcompid,
                             size_t targetcomplen,
                             const char *sendersubid,
                             size_t sendersublen,
                             const char *targetsubid,
                             size_t targetsublen,
                             unsigned long seq, time_t * now);

    int send_heart_beat(fix_con_obj * fco);

    int send_test_req(fix_con_obj * fco);

    int send_logout(fix_con_obj * fco);

    size_t find_fix_offsets(char *data, size_t * seq, size_t size,
                            size_t byte_offset, size_t beg_seq,
                            size_t end_seq, size_t * beg_off,
                            size_t * end_off);

#if defined(__cplusplus)
}
#endif
#endif                          //FIX_HANDLER_H__
