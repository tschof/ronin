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
#include "ex_config_helper.h"
#include "parser_func.h"

static void set_vals(ex_config * ex)
{
    int ret_len = 0;
    char *f_port = NULL;
    char *hb_int_str = NULL;
    char *end_str = NULL;
    char *num_objects = NULL;
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
        get_val_for_tag(ex->dc, ex->server, ex->server_len, "Mod Name", 8,
                        &ret_len);
    ex->mod_name_len = ret_len;
    ex->ip =
        get_val_for_tag(ex->dc, ex->server, ex->server_len, "IP", 2,
                        &ret_len);
    ex->ip_len = ret_len;
    f_port =
        get_val_for_tag(ex->dc, ex->server, ex->server_len, "Port", 4,
                        &ret_len);
    if (f_port) {
        ex->port = atoi(f_port);
        free(f_port);
    } else {
        ex->port = 0;
    }
    hb_int_str =
        get_val_for_tag(ex->dc, ex->server, ex->server_len,
                        "Heartbeat Int", 13, &ret_len);
    if (hb_int_str != NULL) {
        ex->heart_beat_int = atoi(hb_int_str);
        free(hb_int_str);
    } else {
        ex->heart_beat_int = 30;
    }
    ex->version = get_val_for_tag(ex->dc, "SERVER", 6, "Version", 7, &ret_len);
    if(ret_len <= 0) {
        ex->version = calloc(8,1);
        memcpy(ex->version, "FIX.4.2", 7);
        ex->vlen = 7;
    }
    ex->username = get_val_for_tag(ex->dc, "SERVER", 6, "Username", 8, &ret_len);
    ex->ulen = ret_len;
    ex->password = get_val_for_tag(ex->dc, "SERVER", 6, "Password", 8, &ret_len);
    ex->plen = ret_len;
    ex->directory = get_val_for_tag(ex->dc, "SERVER", 6, "directory", 9, &ret_len);
    ex->dir_len = ret_len;
    ex->destinations = get_val_for_tag(ex->dc, "SERVER", 6, "Dests", 5, &ret_len);
    ex->dest_len = ret_len;

    ex->cpus = get_val_for_tag(ex->dc, "SERVER", 6, "CPU", 3, &ret_len);
    ex->cpulen = ret_len;

    end_str = NULL;
    end_str = get_val_for_tag(ex->dc, "SERVER", 6, "Threads", 7, &ret_len);
    if(ret_len > 0) {
        ex->num_threads = atoi(end_str);
    } else {
        ex->num_threads = 2;
    }
    end_str = 0;
    end_str = get_val_for_tag(ex->dc, "SERVER", 6, "Time out", 8, &ret_len);
    if(ret_len > 0) {
        ex->time_out = atoi(end_str);
    } else {
        ex->time_out = 0;
    }

    num_objects =
        get_val_for_tag(ex->dc, ex->server, ex->server_len,
                        "Order Allocation", 16, &ret_len);
    if (num_objects == NULL) {
        ex->order_allocation = 512;
        free(num_objects);
    } else {
        ex->order_allocation = atoi(num_objects);
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
    end_str = NULL;
    end_str =
        get_val_for_tag(ex->dc, "TIMES", 5, "Reset Day", 9, &ret_len);
    if (end_str != NULL) {
        ex->reset_day = atoi(end_str);
        free(end_str);
    } else {
        ex->reset_day = 7;
    }
    ex->dest_id =
        get_val_for_tag(ex->dc, ex->server, ex->server_len, "Dest ID", 7,
                        &ret_len);
    ex->dest_id_len = ret_len;
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
    ex->db_inst_len = ret_len;
    if(ret_len <= 0) {
        ex->db_inst = "rom";
        ex->db_inst_len = 8;
    }
    char* new_incoming = get_val_for_tag(ex->dc, "SERVER", 6, 
                                         "Reset Incoming", 14,
                                         &ret_len);
    if(ret_len > 0 && new_incoming != NULL) {
        ex->new_incoming = atoi(new_incoming);
        free(new_incoming);
    } else {
        ex->new_incoming = 0;
    }
    char* new_outgoing = get_val_for_tag(ex->dc, "SERVER", 6, 
                                         "Reset Outgoing", 14,
                                         &ret_len);
    if(ret_len > 0 && new_outgoing != NULL) {
        ex->new_outgoing = atoi(new_outgoing);
        free(new_outgoing);
    } else {
        ex->new_outgoing = 0;
    }
    char* now;
    int year;
    int month;
    int len = 0;
    char* last_run_date =
        get_val_for_tag(ex->dc, "TIMES", 5, "Last Run Day", 12,
                        &ret_len);
    time_t rawtime;
    time(&rawtime);
    struct tm *ti = 0;
    struct tm lm;
    ti = localtime_r(&rawtime, &lm);
    ti = &lm;
    now = calloc(15, 1);
    year = 1900 + ti->tm_year;
    month = 1 + ti->tm_mon;
    len = sprintf(now, "%04d%02d%02d%02d%02d%02d", year, month, ti->tm_mday,
                  ti->tm_hour, ti->tm_min, ti->tm_sec);
    ex->create_new = 0;
    if(ret_len == 14) {
        ex->last_logon = convert_to_sec_since_1970(last_run_date);
    } else {
        ex->last_logon = 0;
    }
    ex->log_outgoing = 1;
    ex->cancel_only = 0;
    set_val_for_tag(ex->dc, "TIMES", 5, "Last Run Day", 12, now, len);
    free(now);
    free(last_run_date);
}
char* get_config_val(ex_config* ex, const char* header, int hlen,
                      const char* tag, int tlen, int* ret_len)
{
    if(ex->dc != 0) {
        return get_no_copy_val(ex->dc, header, hlen, tag, tlen, ret_len);
    } else {
        *ret_len = 0;
        return NULL;
    }
}

ex_config *init_config(const char *file_name, int len)
{
    return get_config(file_name, len, "SERVER", 6);
}
int put_val_int_buff(ex_config* ex, const char* header, int hlen,
                     const char* tag, int tlen, char* output,
                     int max_len)
{
    lock_config_loader(ex->dc);
    int ret = cp_to_buff(ex->dc, header, hlen, tag, tlen,
                         output, max_len);
    unlock_config_loader(ex->dc);
    return ret;
}

static void clean_ex_config(ex_config* ex)
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
    if (ex->ip) {
        free((char *) ex->ip);
    }
    if (ex->dest_id) {
        free((char *) ex->dest_id);
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
}

void reload_config(ex_config * ex)
{
    if(ex->dc != NULL) {
        lock_config_loader(ex->dc);
        clean_ex_config(ex);
        rex_reset_maps(ex->dc);
        set_vals(ex);
        unlock_config_loader(ex->dc);
    }
}

ex_config *get_config(const char *file_name, int len, const char *server,
                      int server_len)
{
    ex_config *ex = malloc(sizeof(ex_config));
    ex->dc = initialize_config(file_name, len);
    ex->server = calloc(server_len + 1, 1);
    memcpy(ex->server, server, server_len);
    ex->server_len = server_len;
    set_vals(ex);
    return ex;
}

void destroy_ex_config(ex_config * ex)
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
    if (ex->ip) {
        free((char *) ex->ip);
    }
    if (ex->dest_id) {
        free((char *) ex->dest_id);
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
    if(ex->server) {
        free(ex->server);
    }
    rex_destroy_config(ex->dc);
}
char *build_rom_name(const char *om_tag, size_t om_tag_len,
                     size_t * r_name_len)
{
    char *ret = calloc(15, 1);
    char *r_off = ret;
    if (om_tag_len > 0 && ret != NULL) {
        char *first_off = NULL;
        char *sec_off = NULL;
        char *d_off = (char *) om_tag;
        first_off = strchr(om_tag, '-');
        if (first_off != NULL && (first_off - om_tag) < om_tag_len) {
            d_off = first_off;
            d_off += 1;
            sec_off = strchr(d_off, '-');
            if (sec_off != NULL && (sec_off - om_tag) < om_tag_len) {
                int len = 0;
                sec_off -= 7;
                len = sec_off - d_off;
                memcpy(r_off, "ROM", 3);
                r_off += 3;
                memcpy(r_off, d_off, len);
                *r_name_len = (3 + len);
            }
        }
    }
    return ret;
}
