/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _EX_CONFIG_HELPER_H__
#define _EX_CONFIG_HELPER_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <time.h>
#include "config_loader.h"

typedef struct ex_con {
    char* username;
    char* password;
    char *mail_server;
    char *tech_group;
    char *biz_group;
    char *ip;
    char *dest_id;
    char *db_user_name;
    char *db_password;
    char *db_server;
    char *db_inst;
    char *mod_name;
    char* version;
    char *server;
    char *cpus;
    char* directory;
    char* destinations;
    rex_config *dc;
    uint16_t port;
    int mail_server_len;
    int t_len;
    int b_len;
    int ip_len;
    int end_hour;
    int end_min;
    int end_day;
    int reset_hour;
    int reset_min;
    int reset_day;
    int ulen;
    int plen;
    int dest_id_len;
    int db_user_name_len;
    int db_password_len;
    int db_server_len;
    int dest_len;
    int db_inst_len;
    int heart_beat_int;
    int order_allocation;
    int mod_name_len;
    int server_len;
    time_t last_logon;
    int new_incoming;
    int new_outgoing;
    int create_new;
    int log_outgoing;
    int cancel_only;
    int vlen;
    int cpulen;
    int num_threads;
    int dir_len;
    int time_out;
} ex_config;

ex_config *init_config(const char *file_name, int len);
ex_config *get_config(const char *file_name, int len,
                      const char *server, int server_len);
void reload_config(ex_config * ex);
void destroy_ex_config(ex_config * ex);
char *build_rom_name(const char *om_tag, size_t om_tag_len,
                     size_t * r_name_len);
/*
 * This will get you a non copied value from the config file.
 * you need to call lock_config_loader(ex_config->dc) before
 * you call this and call unlock_config_loader(ex_config->dc) after.
 */
char* get_config_val(ex_config* ex, const char* header, int hlen,
                      const char* tag, int tlen, int* ret_len);
/*
 * Use this if you want to put a config value in a particular 
 * memory location.  It ensures that the config value placed 
 * does not take more than max_len.  It returns either 0
 * or the length of the config.
 */
int put_val_int_buff(ex_config* ex, const char* header, int hlen,
                     const char* tag, int tlen, char* output,
                     int max_len);

#if defined(__cplusplus)
}
#endif
#endif