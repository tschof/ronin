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

static void set_vals(ex_config * ex)
{
    int ret_len = 0;
    char *f_port = NULL;
    char *r_port = NULL;
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
    ex->fix_ip =
        get_val_for_tag(ex->dc, ex->server, ex->server_len, "Fix IP", 6,
                        &ret_len);
    ex->fix_ip_len = ret_len;
    f_port =
        get_val_for_tag(ex->dc, ex->server, ex->server_len, "Fix Port", 8,
                        &ret_len);
    if (f_port) {
        ex->fix_port = atoi(f_port);
        free(f_port);
    } else {
        ex->fix_port = 0;
    }
    r_port =
        get_val_for_tag(ex->dc, "ROM SERVER", 10, "Rom Port", 8, &ret_len);
    if (r_port) {
        ex->rom_port = atoi(r_port);
        free(r_port);
    } else {
        ex->rom_port = 0;
    }
    ex->rom_ip =
        get_val_for_tag(ex->dc, "ROM SERVER", 10, "Rom Host", 8, &ret_len);
    ex->rom_ip_len = ret_len;
    hb_int_str =
        get_val_for_tag(ex->dc, ex->server, ex->server_len,
                        "Heartbeat Int", 13, &ret_len);
    if (hb_int_str != NULL) {
        ex->heart_beat_int = atoi(hb_int_str);
        free(hb_int_str);
    } else {
        ex->heart_beat_int = 30;
    }

    num_objects =
        get_val_for_tag(ex->dc, ex->server, ex->server_len,
                        "Order Allocation", 16, &ret_len);
    if (num_objects == NULL) {
        ex->order_allocation = 80000;
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
    ex->useStart = 0;
    end_str =
        get_val_for_tag(ex->dc, "TIMES", 5, "Start Hour", 10, &ret_len);
    if (end_str != NULL) {
        ex->start_hour = atoi(end_str);
        free(end_str);
        ex->useStart = 1;
    } else {
        ex->start_hour = 0;
    }
    end_str = NULL;
    end_str =
        get_val_for_tag(ex->dc, "TIMES", 5, "Start Min", 9, &ret_len);
    if (end_str != NULL) {
        ex->start_min = atoi(end_str);
        free(end_str);
        ex->useStart = 1;
    } else {
        ex->start_min = 0;
    }
    end_str = NULL;
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
    if(ret_len <= 0) {
        ex->db_inst = "rom.dbo.";
        ex->db_inst_len = 8;
    } else {
        ex->db_inst_len = ret_len;
    }
}
char* get_config_val(ex_config* ex, const char* header, int hlen,
                      const char* tag, int tlen, int* ret_len)
{
    return get_no_copy_val(ex->dc, header, hlen, tag, tlen, ret_len);
}

ex_config *init_config(const char *file_name, int len)
{
    return get_config(file_name, len, "FIX SERVER", 10);
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
    if (ex->fix_ip) {
        free((char *) ex->fix_ip);
    }
    if (ex->rom_ip) {
        free((char *) ex->rom_ip);
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
    lock_config_loader(ex->dc);
    clean_ex_config(ex);
    dart_reset_maps(ex->dc);
    set_vals(ex);
    unlock_config_loader(ex->dc);
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
    if (ex->fix_ip) {
        free((char *) ex->fix_ip);
    }
    if (ex->rom_ip) {
        free((char *) ex->rom_ip);
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
    dart_destroy_config(ex->dc);
}

static void clean_init_args(struct init_args *ex)
{
    if (ex->name) {
        free((char *) ex->name);
    }
    ex->name_len = 0;
    if (ex->sender_comp) {
        free((char *) ex->sender_comp);
    }
    ex->sc_len = 0;
    if (ex->target_comp) {
        free((char *) ex->target_comp);
    }
    ex->tc_len = 0;
    if (ex->target_sub) {
        free((char *) ex->target_sub);
    }
    ex->ts_len = 0;
    if (ex->sender_sub) {
        free((char *) ex->sender_sub);
    }
    ex->ss_len = 0;
    if (ex->username) {
        free((char *) ex->username);
    }
    ex->un_len = 0;
    if (ex->password) {
        free((char *) ex->password);
    }
    ex->p_len = 0;
    if (ex->fix_version) {
        free((char *) ex->fix_version);
    }
    ex->fv_len = 0;
    if (ex->reset_time) {
        free(ex->reset_time);
    }
}

static void load_init_args(struct init_args *in_args)
{
    int ret_len;
    struct seq_reset_time *rt;
    char *reset_h;
    char *reset_m;
    char *reset_day;
    char *last_run_date;
    time_t rawtime;
    char *new_incoming;
    char *new_outgoing;
    struct tm *ti;
    char *now;
    int year;
    int month;
    ret_len = 0;
    in_args->name = get_val_for_tag(in_args->dc, "FIX SERVER", 10, "Name",
                        4, &ret_len);
    in_args->name_len = ret_len;
    if(in_args->name_len <= 0) {
        in_args->name =
            get_val_for_tag(in_args->dc, "FIX SERVER", 10, "Target Comp ID",
                            14, &ret_len);
        in_args->name_len = ret_len;
    }
    in_args->sender_comp =
        get_val_for_tag(in_args->dc, "FIX SERVER", 10, "Sender Comp ID",
                        14, &ret_len);
    in_args->sc_len = ret_len;
    in_args->target_comp =
        get_val_for_tag(in_args->dc, "FIX SERVER", 10, "Target Comp ID",
                        14, &ret_len);
    in_args->tc_len = ret_len;
    in_args->target_sub =
        get_val_for_tag(in_args->dc, "FIX SERVER", 10, "Target Sub ID", 13,
                        &ret_len);
    in_args->ts_len = ret_len;
    in_args->sender_sub =
        get_val_for_tag(in_args->dc, "FIX SERVER", 10, "Sender Sub ID", 13,
                        &ret_len);
    in_args->ss_len = ret_len;
    in_args->username =
        get_val_for_tag(in_args->dc, "FIX SERVER", 10, "Username", 8,
                        &ret_len);
    in_args->un_len = ret_len;
    in_args->password =
        get_val_for_tag(in_args->dc, "FIX SERVER", 10, "Password", 8,
                        &ret_len);
    in_args->p_len = ret_len;

    in_args->fix_version =
        get_val_for_tag(in_args->dc, "FIX SERVER", 10, "Fix Version", 11,
                        &ret_len);
    if (in_args->fix_version == NULL) {
        in_args->fix_version = calloc(8, 1);
        memcpy(in_args->fix_version, "FIX.4.2", 7);
        in_args->fv_len = 7;
    } else {
        in_args->fv_len = ret_len;
    }
    in_args->accesskey =
        get_val_for_tag(in_args->dc, "FIX SERVER", 10, "Accesskey", 9,
                &ret_len);
    in_args->accesslen = ret_len;
    in_args->secretkey =
        get_val_for_tag(in_args->dc, "FIX SERVER", 10, "Secretkey", 9,
                &ret_len);
    in_args->secretlen = ret_len;
    rt = calloc(sizeof(struct seq_reset_time),1);
    reset_h =
        get_val_for_tag(in_args->dc, "TIMES", 5, "Reset Hour", 10,
                        &ret_len);
    if (reset_h != NULL) {
        rt->reset_hour = atoi(reset_h);
        free(reset_h);
    } else {
        rt->reset_hour = 0;
    }
    reset_m =
        get_val_for_tag(in_args->dc, "TIMES", 5, "Reset Min", 9, &ret_len);
    if (reset_m != NULL) {
        rt->reset_min = atoi(reset_m);
        free(reset_m);
    } else {
        rt->reset_min = 0;
    }
    reset_day =
        get_val_for_tag(in_args->dc, "TIMES", 5, "Reset Day", 9, &ret_len);
    if (ret_len > 0 && reset_day != NULL) {
        rt->reset_day = atoi(reset_day);
    } else {
        rt->reset_day = 7;
    }
    new_incoming =
        get_val_for_tag(in_args->dc, "FIX SERVER", 10, "Reset Incoming",
                        14, &ret_len);
    if (new_incoming != NULL) {
        rt->new_incoming = atoi(new_incoming);
        free(new_incoming);
    } else {
        rt->new_incoming = 0;
    }
    new_outgoing =
        get_val_for_tag(in_args->dc, "FIX SERVER", 10, "Reset Outgoing",
                        14, &ret_len);
    if (new_outgoing != NULL) {
        rt->new_outgoing = atoi(new_outgoing);
        free(new_outgoing);
    } else {
        rt->new_outgoing = 0;
    }
    last_run_date =
        get_val_for_tag(in_args->dc, "TIMES", 5, "Last Run Day", 12,
                        &ret_len);
    time(&rawtime);
    struct tm lm;
    ti = localtime_r(&rawtime, &lm);
    ti = &lm;
    now = calloc(15, 1);
    year = 1900 + ti->tm_year;
    month = 1 + ti->tm_mon;
    sprintf(now, "%04d%02d%02d%02d%02d%02d", year, month, ti->tm_mday,
                  ti->tm_hour, ti->tm_min, ti->tm_sec);
    rt->create_new = 0;
    if(ret_len == 14) {
        rt->last_logon = convert_to_sec_since_1970(last_run_date);
    } else {
        rt->last_logon = 0;
    }
    in_args->log_outgoing = 1;
    in_args->reset_time = rt;
    in_args->cancel_only = 0;
    set_val_for_tag(in_args->dc, "TIMES", 5, "Last Run Day", 12, now, 14);
    free(now);
    free(last_run_date);
}

void reload_init_args(struct init_args *in_args)
{
    clean_init_args(in_args);
    dart_reset_maps(in_args->dc);
    load_init_args(in_args);
}

struct init_args *load_fix_config(const char *filename, int file_len)
{
    struct init_args *in_args;
    in_args = malloc(sizeof(struct init_args));
    in_args->dc = initialize_config(filename, file_len);
    if (in_args->dc == NULL) {
        fprintf(stderr, "No config file found: %s\n", filename);
        return 0;
    }
    load_init_args(in_args);
    return in_args;
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
