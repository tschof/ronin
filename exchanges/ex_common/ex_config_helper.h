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

#include "config_loader.h"
#include "../ex_db/database_conf.h"
#include <stdint.h>

    typedef struct ex_con {
        const char *mail_server;
        int mail_server_len;
        const char *tech_group;
        int t_len;
        const char *biz_group;
        int b_len;
        const char *fix_ip;
        int fix_ip_len;
        const char *rom_ip;
        int rom_ip_len;
        uint16_t rom_port;
        uint16_t fix_port;
        int end_hour;
        int end_min;
        int end_day;
        int reset_hour;
        int reset_min;
        const char *dest_id;
        int dest_id_len;
        struct database_conf db;
        int heart_beat_int;
        int order_allocation;
        const char *mod_name;
        int mod_name_len;
        dart_config *dc;
    } ex_config;

    ex_config *init_config(const char *file_name, int len);
    ex_config *get_config(const char *file_name, int len,
                          const char *server, int server_len);

    void destroy_ex_config(ex_config * ex);

#if defined(__cplusplus)
}
#endif
#endif                          //_EX_CONFIG_HELPER_H__
