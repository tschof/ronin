/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _DART_FIX_CONNECTION_OBJECT_H__
#define _DART_FIX_CONNECTION_OBJECT_H__

#include "dart_connection_object.h"
#include "transaction_recorder.h"
#include "databuf.h"

#if defined(__cplusplus)
extern "C" {
#endif

    typedef struct dart_fix_connection_obj fix_con_obj;

    fix_con_obj *create_fix_con_obj(const char *sender_comp, size_t sc_len,
                                    const char *target_comp, size_t tc_len,
                                    const char *sendersub,
                                    size_t sendersublen,
                                    const char *targetsub,
                                    size_t targetsublen,
                                    unsigned long in_seq,
                                    unsigned long out_seq,
                                    app_end_time * end_hour, int tickle,
                                    int disconnect, size_t timeout_val,
                                    char *fix_version, size_t fv_len,
                                    char *mod_name, size_t mod_len,
                                    con_obj * obj,
                                    size_t(*create_mess_header)
                                     (databuf_t * fixmsg,
                                      const char *sendercompid,
                                      size_t sendercomplen,
                                      const char *targetcompid,
                                      size_t targetcomplen,
                                      const char *sendersubid,
                                      size_t sendersublen,
                                      const char *targetsubid,
                                      size_t targetsublen,
                                      unsigned long seq, time_t * t));

    void destroy_fix_con_obj(fix_con_obj * obj);

    void dfc_run_admin_loop(fix_con_obj * fco);

    void dfc_kill_connection(fix_con_obj * fco);

    size_t dfc_create_fix_header(fix_con_obj * connobj, databuf_t * fixmsg,
                                 const char *sendercompid,
                                 size_t sendercomplen,
                                 const char *targetcompid,
                                 size_t targetcomplen,
                                 const char *sendersubid,
                                 size_t sendersublen,
                                 const char *targetsubid,
                                 size_t targetsublen, unsigned long seq,
                                 time_t * t);

    void dfc_send_tickle(fix_con_obj * fco);

    void dfc_set_con_obj(fix_con_obj * fco, con_obj * co);

    void dfc_set_mod_name(fix_con_obj * foc, const char *mod_name,
                          size_t mod_len);

    const char *dfc_get_mod_name(fix_con_obj * fco, size_t * len);

    void dfc_set_incoming_seq(fix_con_obj * fco, unsigned long in_seq);

    void dfc_set_outgoing_seq(fix_con_obj * fco, unsigned long out_seq);

    unsigned long dfc_get_incoming_seq(fix_con_obj * obj);

    unsigned long dfc_get_outgoing_seq(fix_con_obj * obj);

    void dfc_set_timeout(fix_con_obj * fco, size_t tv);

    void dfc_increment_outgoing_seq(fix_con_obj * obj);

    void dfc_increment_incoming_seq(fix_con_obj * obj);

    void dfc_set_tickle(fix_con_obj * fco, int tickle);

    void dfc_set_disconnect(fix_con_obj * fco, int disconnect);

    int dfc_get_disconnected(fix_con_obj * fco);

    const char *dfc_get_sender_comp_id(fix_con_obj * fco, size_t * len);

    const char *dfc_get_target_comp_id(fix_con_obj * fco, size_t * len);

    const char *dfc_get_sendersubid(fix_con_obj * fco, size_t * len);

    const char *dfc_get_targetsubid(fix_con_obj * fco, size_t * len);

    const char *dfc_get_fix_version(fix_con_obj * fco, size_t * len);

    void dfc_lock_seq_blocker(fix_con_obj * fco);

    void dfc_unlock_seq_blocker(fix_con_obj * fco);

    void dfc_set_last_mess_sec(fix_con_obj * fco, time_t rawtime);

    void dfc_set_last_mess_rec(fix_con_obj * fco, time_t rawtime);

    time_t dfc_get_last_mess_sec(fix_con_obj * fco);

    time_t dfc_get_last_mess_rec(fix_con_obj * fco);

    int dfc_get_socket(fix_con_obj * fco);

    int dfc_get_disconnect(fix_con_obj * fco);

    con_obj *dfc_get_con_obj(fix_con_obj * fco);

    int dfc_get_tickle(fix_con_obj * fco);

    void dfc_get_timeout(fix_con_obj * fco, size_t * tv);

    void dfc_get_end_values(fix_con_obj * fco, int *hour, int *min,
                            int *day);

    int dfc_get_log_outgoing(fix_con_obj * obj);

    void dfc_set_log_outgoing(fix_con_obj * obj, int log);

    int dfc_get_pending_reset(fix_con_obj* obj);

    void dfc_set_pending_reset(fix_con_obj* obj, int pr);
#if defined(__cplusplus)
}
#endif
#endif                          //_DART_FIX_CONNECTION_OBJECT_H__
