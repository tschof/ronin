/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "config_loader.h"
#include "master_config.h"
#include "common_structs.h"

master_config *init_mconfig(const char *file_name, int len)
{
    return get_mconfig(file_name, len, "GLOB", 4);
}

master_config *get_mconfig(const char *file_name, int len,
                           const char *server, int server_len)
{
    master_config *ex = malloc(sizeof(master_config));
    int ret_len = 0;
    char *r_port = NULL;
    char *end_str = NULL;
    char *num_objects = NULL;
    char *last_run_date = 0;
    struct tm *ti;
    char *now;
    int year;
    int month;
    time_t rawtime;
    char *reset_day = 0;
    int day_of_reset = 7;
    ex->dc = initialize_config(file_name, len);
    ex->mail_server =
        get_val_for_tag(ex->dc, "MAIL SERVER", 11, "Server", 6, &ret_len);
    ex->mail_server_len = ret_len;
    ex->tech_group =
        get_val_for_tag(ex->dc, "MAIL SERVER", 11, "tech group", 10,
                        &ret_len);
    ex->t_len = ret_len;
    ex->biz_group =
        get_val_for_tag(ex->dc, "MAIL SERVER", 11, "biz group", 9,
                        &ret_len);
    ex->b_len = ret_len;

    ex->mod_name =
        get_val_for_tag(ex->dc, server, server_len, "Mod Name", 8,
                        &ret_len);
    ex->mod_name_len = ret_len;
    r_port =
        get_val_for_tag(ex->dc, "ROM SERVER", 10, "Rom Port", 8, &ret_len);
    if (r_port) {
        ex->server_port = atoi(r_port);
        free(r_port);
    } else {
        ex->server_port = 0;
    }
    ex->server_ip =
        get_val_for_tag(ex->dc, "ROM SERVER", 10, "Rom Host", 8, &ret_len);
    ex->server_ip_len = ret_len;

    ex->destinations =
        get_val_for_tag(ex->dc, server, server_len, "Dests", 5, &ret_len);
    ex->dest_len = ret_len;

    ex->dir = get_val_for_tag(ex->dc, "GLOB", 4, "directory", 9, &ret_len);
    ex->dir_len = ret_len;

    ex->cpus = get_val_for_tag(ex->dc, "GLOB", 4, "CPU", 3, &ret_len);
    ex->cpu_len = ret_len;
    num_objects =
        get_val_for_tag(ex->dc, server, server_len, "Order Allocation", 16,
                        &ret_len);
    if (num_objects == NULL) {
        ex->order_allocation = 80000;
    } else {
        ex->order_allocation = atoi(num_objects);
        free(num_objects);
    }
    end_str = get_val_for_tag(ex->dc, "TIMES", 5, "End Hour", 8, &ret_len);
    if (end_str != NULL) {
        ex->end_hour = atoi(end_str);
        free(end_str);
    } else {
        ex->end_hour = 17;
    }
    end_str = NULL;
    end_str = get_val_for_tag(ex->dc, "TIMES", 5, "End Min", 7, &ret_len);
    if (end_str != NULL) {
        ex->end_min = atoi(end_str);
        free(end_str);
    } else {
        ex->end_min = 17;
    }
    end_str = NULL;
    end_str = get_val_for_tag(ex->dc, "TIMES", 5, "End Day", 7, &ret_len);
    if (end_str != NULL) {
        ex->end_day = atoi(end_str);
        free(end_str);
    } else {
        ex->end_day = 7;
    }
    end_str = NULL;
    end_str =
        get_val_for_tag(ex->dc, "TIMES", 5, "Reset Hour", 10, &ret_len);
    if (end_str != NULL) {
        ex->reset_hour = atoi(end_str);
        free(end_str);
    } else {
        ex->reset_hour = 0;
    }
    end_str = NULL;
    end_str =
        get_val_for_tag(ex->dc, "TIMES", 5, "Reset Min", 9, &ret_len);
    if (end_str != NULL) {
        ex->reset_min = atoi(end_str);
        free(end_str);
    } else {
        ex->reset_min = 0;
    }
    reset_day =
        get_val_for_tag(ex->dc, "TIMES", 5, "Reset Day", 9, &ret_len);
    if (ret_len > 0 && reset_day != NULL) {
        day_of_reset = atoi(reset_day);
        free(reset_day);
    }
    end_str = NULL;
    ex->db_user_name =
        get_val_for_tag(ex->dc, "DB INFO", 7, "User", 4, &ret_len);
    ex->db_user_name_len = ret_len;
    ex->db_password =
        get_val_for_tag(ex->dc, "DB INFO", 7, "Password", 8, &ret_len);
    ex->db_password_len = ret_len;
    ex->db_server =
        get_val_for_tag(ex->dc, "DB INFO", 7, "Server", 6, &ret_len);
    ex->db_server_len = ret_len;
    ex->db_inst =
        get_val_for_tag(ex->dc, "DB INFO", 7, "Instance", 8, &ret_len);
    if(ret_len <= 0) {
        ex->db_inst = "rom.dbo.";
        ex->db_inst_len = 8;
    } else {
        ex->db_inst_len = ret_len;
    }

    last_run_date =
        get_val_for_tag(ex->dc, "TIMES", 5, "Last Run Day", 12, &ret_len);
    time(&rawtime);
    struct tm lm;
    ti = localtime_r(&rawtime, &lm);
    ti = &lm;
    now = calloc(15, 1);
    year = 1900 + ti->tm_year;
    month = 1 + ti->tm_mon;
    sprintf(now, "%04d%02d%02d%02d%02d%02d", year, month, ti->tm_mday,
            ti->tm_hour, ti->tm_min, ti->tm_sec);
    if (last_run_date && strncmp(now, last_run_date, 8) == 0 && ret_len <= 8) {
        ex->create_new = 0;
        if (ti->tm_hour == ex->reset_hour &&
            (ti->tm_min >= ex->reset_min &&
             (ex->reset_min + 5) >= ti->tm_min) &&
            (day_of_reset == 7 || day_of_reset == ti->tm_wday)) {
            ex->create_new = 1;
        }
    } else if(ret_len == 14 && strncmp(now, last_run_date, 8) == 0 && day_of_reset == 7) {
        ex->create_new = 0;
        time_t last_logon = convert_to_sec_since_1970(last_run_date);
        struct tm t;
        memset(&t, 0, sizeof(struct tm));
        t.tm_year = ti->tm_year;
        t.tm_mon = ti->tm_mon;
        t.tm_mday = ti->tm_mday;
        t.tm_hour = ex->reset_hour;
        t.tm_min = ex->reset_min;
        t.tm_sec = 0;
        t.tm_isdst = -1;
        time_t reset_time = mktime(&t);
        ex->create_new = (rawtime >= reset_time && last_logon < reset_time) ? 1 : 0;
    }else if (day_of_reset == 7 || day_of_reset == ti->tm_wday) {
        ex->create_new = 1;
    } else {
        ex->create_new = 0;
    }
    set_val_for_tag(ex->dc, "TIMES", 5, "Last Run Day", 12, now, 14);
    if (now) {
        free(now);
    }
    if (last_run_date) {
        free(last_run_date);
    }
    return ex;
}

void destroy_master_config(master_config * ex)
{

    if (ex->mail_server) {
        free((char *) ex->mail_server);
    }
    if (ex->tech_group) {
        free((char *) ex->tech_group);
    }

    if (ex->biz_group) {
        free((char *) ex->biz_group);
    }
    if (ex->server_ip) {
        free((char *) ex->server_ip);
    }
    if (ex->db_user_name) {
        free((char *) ex->db_user_name);
    }
    if (ex->db_password) {
        free((char *) ex->db_password);
    }
    if (ex->db_server) {
        free((char *) ex->db_server);
    }
    if (ex->mod_name) {
        free((char *) ex->mod_name);
    }
    if(ex->destinations) {
        free((char*)ex->destinations);
    }
    if(ex->cpus) {
        free((char*)ex->cpus);
    }
    dart_destroy_config(ex->dc);
    free(ex);
}
