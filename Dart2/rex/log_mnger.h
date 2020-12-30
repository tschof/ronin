#ifndef __LOG_MNGER_H__
#define __LOG_MNGER_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "dart_order_obj.h"

typedef struct off_thread_logger otl;

otl* create_otl(char* dir, int dir_len);

void set_thr_id(otl* o, int id);
void log_check(otl* o);
void start_logging(void* o);
void stop_logging(otl* o);

void record_inbound_message(otl* o, int n_messages, char* name, int nlen);
void rewind_raw_outbound(otl* o, char* name, int nlen, char* raw, int rlen);

void record_raw_message(otl* o, char* name, int nlen, char* raw, int rlen, char inc_out_seq);

void record_rolling_log(otl* o, char* name, int nlen, char* raw, int rlen, char inc_out_seq);

void get_sequence_numbers(void* sr, otl* o);

void get_resend_request(void* rr, otl* o);

void send_debug_message(otl* o, const char* formatstr, ...);

void reset_seq(otl* o, char* name, int nlen);

void record_dart_message(otl* o, dart_order_obj* doj);

void record_dart_message_for_sender(otl* o, dart_order_obj* doj,
                             char* sender, int slen, void* dp);
void record_dma_message(otl* o, char* name, int nlen, char* raw, int rlen,
                        void (*pdma)(void* p, char* d, int dlen, dart_order_obj* doj),
                        void* p);
void record_audit_trail(otl* o, char* raw,int rlen,
                        void (*pdma)(void* p, char* raw,
                          int rlen, dart_order_obj* doj,
                          time_t sec, int micro),
                        void* p);
#if defined(__cplusplus)
}
#endif
#endif
