/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _MASTER_CONFIG_H__
#define _MASTER_CONFIG_H__
#if defined(__cplusplus)
extern "C" {
#endif

typedef struct master_config {
    const char *mail_server;
    const char *tech_group;
    const char *biz_group;
    const char *server_ip;
    const char *destinations;
    const char *db_user_name;
    const char *db_password;
    const char *db_server;
    const char *db_inst;
    const char *mod_name;
    char *cpus;
    dart_config *dc;
    char* dir;
    int dir_len;
    int mail_server_len;
    int t_len;
    int b_len;
    int cpu_len;
    int dest_len;
    int server_ip_len;
    int server_port;
    int end_hour;
    int end_min;
    int end_day;
    int reset_hour;
    int reset_min;
    int create_new;
    int db_user_name_len;
    int db_password_len;
    int db_server_len;
    int db_inst_len;
    int order_allocation;
    int mod_name_len;
} master_config;

master_config *init_mconfig(const char *file_name, int len);
master_config *get_mconfig(const char *file_name, int len,
                           const char *server, int server_len);
void destroy_master_config(master_config * ex);

#if defined(__cplusplus)
}
#endif
#endif
