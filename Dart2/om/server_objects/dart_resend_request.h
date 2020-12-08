/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _DART_RESEND_REQUEST_H__
#define _DART_RESEND_REQUEST_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include <stdio.h>

typedef struct dart_resend_req resend_req;

resend_req *create_resend_request(const char *name, size_t name_len,
                                  size_t beg, size_t end,
                                  size_t(*parse_req) (char *data,
                                                      size_t * seq_off,
                                                      size_t size,
                                                      size_t bytes_used,
                                                      size_t beg_seq,
                                                      size_t end_seq,
                                                      size_t * beg_off,
                                                      size_t * end_off));
void destroy_resend_req(resend_req * rr);
const char *drr_get_resend_output(resend_req * rr, size_t * len);
const char *drr_get_name(resend_req * rr, size_t * len);
void drr_signal_sleeper(resend_req * rr);
int drr_lock_update_mutex(resend_req * rr);
void drr_wait_on_cond(resend_req * rr);
void drr_create_resend_out(resend_req * rr,  int fp);
void drr_unlock_update_mutex(resend_req * rr);

#if defined(__cplusplus)
}
#endif
#endif
