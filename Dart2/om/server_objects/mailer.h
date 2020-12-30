/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _MAILER_H__
#define _MAILER_H__
#if defined(__cplusplus)
extern "C" {
#endif

int start_dart_mail_service(const char *mail_server,
                            int m_len, const char *biz_group,
                            int b_len,
                            const char *tech_group,
                            int t_len, int end_hour,
                            int end_min, int end_day);
int send_biz_message(const char *message, int len,
                     const char *title, int t_len);
int send_tech_message(const char *message, int len,
                      const char *title, int t_len);

#if defined(__cplusplus)
}
#endif
#endif
