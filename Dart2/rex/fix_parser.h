/*************************************************************************
 * Copyright (c) 2013 by DART executions, LLC
 *
 * All Rights Reserved
 * ***********************************************************************/

#ifndef FIX_PARSER_H__
#define FIX_PARSER_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "dart_order_obj.h"
#include "log_mnger.h"
#include "fix_trans.h"
#include "polling_server.h"
#include "dart_order_fact.h"


typedef struct fix_parser fixp;

fixp* create_fix_parser(void* pass_back_pointer,
                        void (*call_back)(void* pbp, dart_order_obj* doj),
                        dartfact* d, otl* o, trans_t* t, int num_obj);
fixp* clone_fix_parser(fixp* fp);
void init_fix_obj(void* glob);
void clean_fix_parser(fixp* fp);
void copy_pbp_and_cbp(fixp* fp, fixp* orig);
/**
 *
 */
ofp* get_message_for_type(fixp* fp, short type);

void return_fix_message(fixp* fp, ofp* p);
                        
dart_order_obj* trans_fix_msg(fixp* fp, char* fix_msg, int len, short type);

void no_create_trans(fixp* fp, char* fix_msg, int len,
                     dart_order_obj* doj, short type);
void no_create_copy_trans(fixp* fp, char* fix_msg, int len,
                          dart_order_obj* doj, short type);
void return_dart_obj(fixp* fp, dart_order_obj* doj);

void fp_callback(fixp* fp, dart_order_obj* doj);

void unlock_sender(fixp* fp);

void lock_sender(fixp* fp);

void set_log_info(fixp* fp, login_info* in);

void set_rex_con(fixp* fp, rex_con* rc);

rex_con* get_fs_rex_con(fixp* fp);

trans_t* get_trans(fixp* fp);

void set_fp_header_len(fixp* fp, int header_len);

int get_fp_header_len(fixp* fp);

/**
 * This function finalizes the ofp object passed in for sending on
 * the connection defined by the con parameter.
 * @param obj the ofp object that collects FIX tag value pairs.
 * @param con the fixp object that defines a FIX connection.
 */
void fp_send_fix_message(ofp* obj, fixp* con);

void request_resend(fixp* fp, 
        unsigned long start, unsigned long end);

void set_in_seq(fixp* fp, int in);

void inc_in_seq_n_set_recv(fixp* fp);

void set_out_seq(fixp* fp, int out);

int get_in_seq(fixp* fp);

int get_out_seq(fixp* fp);

/**
 * Sends a logout message on the FIX connection defined by the fco parameter.
 * @param fco a rex_con object that defines a FIX interface.
 * @param message a char array that is a logout message reason.
 * This will be put in FIX tag 58 of the logout.
 * @param m_len the length of the char array message.
 */
void send_logout(fixp * fco, char* message, int m_len);

void start_hb_timer(fixp* fp);

void respond_to_tr(fixp* fp, char* d_off, int test_req);

void send_gap_fill(fixp* fp, long beg_seq, long seq);

void fp_write_callback(void* cb);

void send_iovec(fixp* fp, struct iovec* iov, int iovcnt);

int validate_sender_comp(fixp* fp, dart_order_obj* doj);

void* get_fs_pass_back_pointer(fixp* fp);
void set_fs_pass_back_pointer(fixp* fp, void* pbp);

void reload_fix_config(fixp* fp);
void fix_reconnect(fixp* dp);
void set_fix_parser_con(fixp* fp, int is_connected);
#if defined(__cplusplus)
}
#endif
#endif

