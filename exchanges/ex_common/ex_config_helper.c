/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "config_loader.h"
#include "ex_config_helper.h"

ex_config *init_config(const char *file_name, int len)
{
    return get_config(file_name, len, "FIX SERVER", 10);
}

void init_database_conf(struct database_conf *dbconf, dart_config * loader)
{
    const char *sectionptr = "DB INFO";
    int sectionlen = strlen(sectionptr), len;
    dbconf->server =
        get_val_for_tag(loader, sectionptr, sectionlen, "Server", 6, &len);
    dbconf->database =
        get_val_for_tag(loader, sectionptr, sectionlen, "Database", 8,
                        &len);
    if (dbconf->database == 0 || len == 0) {
        dbconf->database = strdup("rom");
    }
    dbconf->username =
        get_val_for_tag(loader, sectionptr, sectionlen, "User", 4, &len);
    dbconf->password =
        get_val_for_tag(loader, sectionptr, sectionlen, "Password", 8,
                        &len);
}

void free_database_conf(struct database_conf *dbconf)
{
    free(dbconf->server);
    free(dbconf->database);
    free(dbconf->username);
    free(dbconf->password);
}

ex_config *get_config(const char *file_name, int len, const char *server,
                      int server_len)
{
    ex_config *ex = malloc(sizeof(ex_config));
    int ret_len = 0;
    char *f_port = NULL;
    char *r_port = NULL;
    char *hb_int_str = NULL;
    char *end_str = NULL;
    char *num_objects = NULL;
    if ((ex->dc = initialize_config(file_name, len)) == 0) {
        free(ex);
        ex = 0;
        return ex;
    }
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
    ex->fix_ip =
        get_val_for_tag(ex->dc, server, server_len, "Fix IP", 6, &ret_len);
    ex->fix_ip_len = ret_len;
    f_port =
        get_val_for_tag(ex->dc, server, server_len, "Fix Port", 8,
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
        get_val_for_tag(ex->dc, server, server_len, "Heartbeat Int", 13,
                        &ret_len);
    if (hb_int_str != NULL) {
        ex->heart_beat_int = atoi(hb_int_str);
        free(hb_int_str);
    } else {
        ex->heart_beat_int = 30;
    }

    num_objects =
        get_val_for_tag(ex->dc, server, server_len, "Order Allocation", 16,
                        &ret_len);
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
    ex->dest_id =
        get_val_for_tag(ex->dc, server, server_len, "Dest ID", 7,
                        &ret_len);
    ex->dest_id_len = ret_len;
    init_database_conf(&ex->db, ex->dc);
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
    if (ex->mod_name) {
        free((char *) ex->mod_name);
    }
    free_database_conf(&ex->db);
}
