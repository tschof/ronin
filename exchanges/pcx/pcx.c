/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

#include "client_manager.h"
#include "message_queue.h"
#include "parser_func.h"
#include "dart_constants.h"
#include "config_loader.h"
#include "ex_common.h"
#include "ex_config_helper.h"
#include "dart_orderbook.h"
#include "rom_handler.h"
#include "rom2fix.h"
#include "mailer.h"
#include "fix_handler.h"
#include "fixmsg.h"
#include "version.h"
#include "order_token_creator.h"
#include "complex_parser.h"
#include "options_common.h"
#include "string_table.h"

static pthread_mutex_t connection_lock = PTHREAD_MUTEX_INITIALIZER;

static token_struct_t *idgenerator = 0;
static char datestr[9] = { '\0' };

static char *fix_client_name;
static int fix_client_len;

static char *module_name = NULL;
static int module_name_len = 0;
static int is_amex = 0;

static int is_con_now = 0;
static int is_pcx_connected = 1;
static strtable_t *clearing_id_table;

static void init_date_str()
{
    struct tm *ti;
    time_t long_time;
    time(&long_time);
    int year;
    int month;
    struct tm lm;
    ti = localtime_r(&long_time, &lm);
    ti = &lm;
    year = 1900 + ti->tm_year;
    month = 1 + ti->tm_mon;
    sprintf(datestr, "%04d%02d%02d", year, month, ti->tm_mday);
}

int pcx_trans_side(char *rommsg, dart_fix_pos * map,
                   char *mtype, databuf_t * fix,
                   size_t field, int *rejected, dart_ex_order_obj * order);
int rom_to_pcx_price(char *rommsg, dart_fix_pos * xlateinfo,
                     char *mtype, databuf_t * fix,
                     size_t romfield, int *rejected,
                     dart_ex_order_obj * order);

int pcx_to_rom_price(char *fixmsg, dart_rom_pos * xlateinfo,
                     char mtype, char *status,
                     databuf_t * rom, size_t romfield,
                     int *rejtected,
                     dart_ex_order_obj * order,
                     pthread_mutex_t * mutex, hashmap_t map);
void pcx_complex_from_rom_func(struct message *mess, void *b);
static char *pcx_generate_id(size_t * nlen)
{
    char *cl_ord_id = 0;
    if (nlen != 0) {
        int retval = 0;
        cl_ord_id = calloc(DART_EX_TAG_LEN, 1);
        memcpy(cl_ord_id, datestr, sizeof datestr - 1);
        if ((retval = create_order_token(idgenerator, cl_ord_id +
                                         (sizeof datestr - 1),
                                         DART_EX_TAG_LEN - 1)) > 0) {
            *nlen = (size_t) retval + (sizeof datestr - 1);
        } else {
            *nlen = 0;
        }
    }
    return cl_ord_id;
}

int get_is_pcx_connected()
{
    int ret = 0;
    pthread_mutex_lock(&connection_lock);
    ret = is_pcx_connected;
    pthread_mutex_unlock(&connection_lock);
    return ret;
}

void set_is_pcx_connected(int is)
{
    pthread_mutex_lock(&connection_lock);
    is_pcx_connected = is;
    pthread_mutex_unlock(&connection_lock);
}

int pcx_opt_account_trans(char *rom, romindex_t * map, char *type,
                          databuf_t * fix, size_t field, int *rejected,
                          dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        if (len > 0) {
            char *p = rom + map[field].beg;
            if (len == 8 && (strncmp((p + 4), "1209", 4) == 0)) {
                len = 4;
            }
            set_val_in_order_struct(p, len, field, order, type);
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
            result = fixmsg_addstr_bylen(fix, "526", p, len);
        } else if (rom[0] != 'E') {
            char *p = doa_get_account(order, &len);
            if (len > 0 && p != NULL) {
                if (len == 8 && (strncmp((p + 4), "1209", 4) == 0)) {
                    len = 4;
                }
                result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
                result = fixmsg_addstr_bylen(fix, "526", p, len);
                free(p);
            }
        }
    }
    return result;
}

int pcx_trans_mat_day(char *rom, romindex_t * map, char *type,
                      databuf_t * fix, size_t field, int *rejected,
                      dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        char *p = rom + map[field].beg;
        if (len == 1) {
            char val[3];
            val[0] = '0';
            val[1] = p[0];
            val[2] = '\0';
            set_val_in_order_struct(val, 2, field, order, type);
            result = fixmsg_addstr_bylen(fix, map[field].name, val, 2);
        } else if (len > 0) {
            set_val_in_order_struct(p, len, field, order, type);
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
        } else {
            p = doa_get_expire_day(order, &len);
            if (len > 0 && p != NULL) {
                result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
                free(p);
            }
        }
    }
    return result;
}

int pcx_clearing_id_table_mapping(char *rom, romindex_t * map, char *type,
                                  databuf_t * fix, size_t field,
                                  int *rejected, dart_ex_order_obj * order)
{
    int retval = 0;
    if (map[field].name != 0) {
        ssize_t len = map[field].end - map[field].beg;
        if (len > 0) {
            const char *ptr = rom + map[field].beg;
            str_t *val = lookup_str(clearing_id_table, ptr, (size_t) len);
            if (val != 0 && val->len > 0) {
                retval = fixmsg_addstr_bylen(fix, map[field].name,
                                             val->str, val->len);
            }
        }
    }
    return retval;
}
int set_ex_instr(char *rom, romindex_t * map, char *type,
                  databuf_t * fix, size_t field, int *rejected,
                  dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        char *p = rom + map[field].beg;
        if (len == 1 && p[0] == 'A') {
            set_val_in_order_struct("C", 1, field, order, type);
            result =
                fixmsg_addstr2(fix, map[field].name, map[field].len, "C",
                               1);
        } else if(len > 0) {
            set_val_in_order_struct(p, len, field, order, type);
            result =
                fixmsg_addstr2(fix, map[field].name, map[field].len, p,
                               len);
        }
    }
    return result;
}

int pcx_trans_tif(char *rom, romindex_t * map, char *type,
                  databuf_t * fix, size_t field, int *rejected,
                  dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        if (len > 0) {
            char *p = rom + map[field].beg;
            set_val_in_order_struct(p, len, field, order, type);
            result =
                fixmsg_addstr2(fix, map[field].name, map[field].len, p,
                               len);
        } else {
            char *p = doa_get_tif(order, &len);
            if (p != 0 && len > 0) {
                result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
                free(p);
            }
        }
    }
    return result;
}

int pcx_trans_type(char *rom, romindex_t * map, char *type,
                   databuf_t * fix, size_t field, int *rejected,
                   dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        if (len > 0) {
            char *p = rom + map[field].beg;
            set_val_in_order_struct(p, len, field, order, type);
            result =
                fixmsg_addstr2(fix, map[field].name, map[field].len, p,
                               len);
        } else {
            char *p = doa_get_type(order, &len);
            if (p != 0 && len > 0) {
                result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
                free(p);
            }
        }
    }
    return result;
}

int pcx_trans_strike(char *rom, romindex_t * map, char *type,
                     databuf_t * fix, size_t field, int *rejected,
                     dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        if (len > 0) {
            char *p = rom + map[field].beg;
            set_val_in_order_struct(p, len, field, order, type);
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
        } else {
            char *p = doa_get_strike(order, &len);
            if (len > 0 && p != NULL) {
                result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
                free(p);
            }
        }
    }
    return result;
}

int pcx_trans_capacity(char *rom, romindex_t * map, char *type,
                       databuf_t * fix, size_t field, int *rejected,
                       dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[10].end - map[10].beg;
        char *p = rom + map[10].beg;
        if (len > 0 && p[0] != '\0') {
            char cust_firm = '1';
            if (p[0] == 'B') {
                //do nothing
            } else if(p[0] == 'X') {
                    cust_firm = '8';   
            } else {
                cust_firm = '3';
            }
            doa_set_capacity(order, &cust_firm, 1);
            result = fixmsg_addstr_bylen(fix, map[10].name, &cust_firm, 1);
        } else {
            p = doa_get_capacity(order, &len);
            if (p != 0 && len > 0) {
                if (p[0] == 'B' || p[0] == '1') {
                    result =
                        fixmsg_addstr_bylen(fix, map[10].name, "1", 1);
                } else if(p[0] == 'X' || p[0] == '8') {
                        result =
                            fixmsg_addstr_bylen(fix, map[10].name, "8", 1);
                } else {
                    result =
                        fixmsg_addstr_bylen(fix, map[10].name, "3", 1);
                }
                free(p);
            }
        }
    }
    return result;
}

int pcx_trans_mm_account(char *rom, romindex_t * map, char *type,
                         databuf_t * fix, size_t field, int *rejected,
                         dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        if (len > 0) {
            int mm_ex = strtol(rom + map[37].beg, NULL, 10);
            char *mm_name = 0;
            if (mm_ex == 34) {
                mm_name = rom + map[field].beg;
            } else {
                mm_name = calloc(len + 4, 1);
                memcpy(mm_name, "MM-", 3);
                memcpy(mm_name + 3, rom + map[field].beg, len);
                len += 3;
            }
            set_val_in_order_struct(mm_name, len, field, order, type);
            result =
                fixmsg_addstr_bylen(fix, map[field].name, mm_name, len);
        } else if (rom[0] != 'E') {
            size_t mm_len = 0;
            char *p = doa_get_mm_account(order, &len);
            char *mm_ex = doa_get_mm_home(order, &mm_len);
            if (len > 0 && p != NULL) {
                if (mm_len == 0 || strstr("MM-", p) != NULL) {
                    result =
                        fixmsg_addstr_bylen(fix, map[field].name, p, len);
                } else if (mm_len > 0 && strncmp("34", mm_ex, 2) != 0) {
                    char *mm_name = 0;
                    mm_name = calloc(len + 4, 1);
                    memcpy(mm_name, "MM-", 3);
                    memcpy(mm_name + 3, p, len);
                    len += 3;
                    result =
                        fixmsg_addstr_bylen(fix, map[field].name, mm_name,
                                            len);
                    free(mm_name);
                } else {
                    result =
                        fixmsg_addstr_bylen(fix, map[field].name, p, len);
                }
                free(p);
            }
        }
    }
    return result;
}

int set_pcx_defaults(char *rommsg, struct dart_fix_pos *positions,
                     char *mtype, databuf_t * output,
                     size_t romfield, int *rejected,
                     dart_ex_order_obj * order)
{
    databuf_write(output, "%c21=1", FIX_FIELD_DELIM);
    databuf_write(output, "%c57=ARCA", FIX_FIELD_DELIM);
    databuf_write(output, "%c386=1", FIX_FIELD_DELIM);
    databuf_write(output, "%c336=P2", FIX_FIELD_DELIM);
    if (is_amex) {
        databuf_write(output, "%c207=XASE", FIX_FIELD_DELIM);
    }
    return 1;
}

void send_cancel_all();
void con_callBack(con_obj * co, int is_connected, char *log, int len);
void fix_server_connection_callBack(con_obj * co, int is_connected,
                                    char *log, int len);
void test_lac(con_obj * co);
void destroy_rom_queue(queue_t q);
int send_pcx_logon(struct parse_args *pa);

static const char mapping_key_name[] = "ClearingFirmMap";

int main(int argc, char **argv)
{
    int ret;
    struct init_args *fix_args;
    ex_config *ex = NULL;
    struct tm *newtime;
    int keep_running = 1;
    queue_t rom_q;
    time_t long_time;
    orderbook *ob = NULL;
    rom_q = create_queue();
    ex = init_config("PCXConfig.dat", 13);
    fix_args = load_fix_config("PCXConfig.dat", 13);
    load_rom_config(ex->dc, rom_q);
    fix_client_name = fix_args->target_comp;
    fix_client_len = fix_args->tc_len;
    init_date_str();
    idgenerator = init_order_token_creator();
    ob = init_order_book(pcx_generate_id, get_is_pcx_connected,
                         def_rom2fix, set_pcx_defaults,
                         def_xlate_fixrom, ex->order_allocation,
                         ex->mod_name, ex->mod_name_len, fix_client_name,
                         fix_client_len, NULL, 0);
    opt_database_init(&ex->db, ex->dest_id, ob, 0);
    set_option_rom2fix(ob);
    set_option_fix_to_rom(ob);
    ob->fix2rom[14] = def_fixrom_status_xlate;
    ob->fix2rom[7] = pcx_to_rom_price;
    ob->fix2rom[15] = def_handle_ordids;
    ob->rom2fix[10] = pcx_trans_capacity;
    ob->rom2fix[12] = pcx_opt_account_trans;
    ob->rom2fix[4] = pcx_trans_side;
    ob->rom2fix[7] = rom_to_pcx_price;
    ob->rom2fix[8] = pcx_trans_type;
    ob->rom2fix[32] = pcx_trans_strike;
    ob->rom2fix[9] = pcx_trans_tif;
    ob->rom2fix[36] = pcx_trans_mm_account;
    ob->rom2fix[50] = set_transacttime;
    ob->rom2fix[57] = set_ex_instr;
    ob->rom2fix[62] = pcx_trans_mat_day;

    ob->no_spread_head = 1;
    //ob->no_fix_43 = 1;
    int ret_len = 0;
    char *clearing_firm_mapping_rule
        = get_val_for_tag(fix_args->dc, "FIX SERVER", 10, mapping_key_name,
                          sizeof mapping_key_name - 1, &ret_len);
    if (clearing_firm_mapping_rule) {
        if ((clearing_id_table
             = parse_table_conf(clearing_firm_mapping_rule)) != 0) {
            ob->rom2fix[11] = pcx_clearing_id_table_mapping;
        } else {
            fprintf(stderr,
                    "Fatal error: cannot parse configuration rule for %s\n",
                    mapping_key_name);

            return 1;
        }
        free(clearing_firm_mapping_rule);
    }

    char *branch_code =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "AMEX", 4,
                        &ret_len);
    if (branch_code != NULL) {
        is_amex = 1;
    }
    fs_set_spread_book(ob);
    set_rom2fix_leg_book(ob);
    module_name_len = ex->mod_name_len;
    module_name = calloc(module_name_len + 1, 1);
    memcpy(module_name, ex->mod_name, module_name_len);;
    start_dart_mail_service(ex->mail_server, ex->mail_server_len,
                            ex->biz_group, ex->b_len, ex->tech_group,
                            ex->t_len, ex->end_hour, ex->end_min, 7);


    ret =
        start_dart_client_services(&pcx_complex_from_rom_func,
                                   ex->mod_name, ex->mod_name_len,
                                   ex->rom_port, rom_q, ex->rom_ip,
                                   ex->rom_ip_len, ex->end_hour,
                                   ex->end_min, 7, ex->heart_beat_int, ob,
                                   con_callBack, parse_rom_message,
                                   build_offsets, test_lac,
                                   create_rom_resend_pad, 0);
    ret =
        dart_connect(&from_fix_func, ex->mod_name, ex->mod_name_len,
                     ex->fix_port, ex->fix_ip, ex->fix_ip_len,
                     ex->end_hour, ex->end_min, 7, ex->heart_beat_int, ob,
                     fix_server_connection_callBack, parse_fix_message,
                     fix_args, send_pcx_logon, build_fix_offsets,
                     low_activity_sig, 1);

    while (keep_running) {
        time(&long_time);
        struct tm lm;
        newtime = localtime_r(&long_time, &lm);
        newtime = &lm;
        if (newtime->tm_hour == ex->end_hour
            && newtime->tm_min >= ex->end_min) {
            keep_running = 0;
            printf("Done with our test!!!!end_hour %d end_min %d now %d\n",
                   ex->end_hour, ex->end_min, newtime->tm_hour);
        } else {
            sleep(25);

            if (!get_is_pcx_connected()) {
                set_is_pcx_connected(1);
                ret =
                    dart_connect(&from_fix_func, ex->mod_name,
                                 ex->mod_name_len, ex->fix_port,
                                 ex->fix_ip, ex->fix_ip_len, ex->end_hour,
                                 ex->end_min, 7, ex->heart_beat_int, ob,
                                 fix_server_connection_callBack,
                                 parse_fix_message, fix_args,
                                 send_pcx_logon, build_fix_offsets,
                                 low_activity_sig, 1);
            }
        }
    }
    clean_dart_oo_pool(ob->mgr);
    free_init_args(fix_args);
    destroy_ex_config(ex);
    destroy_rom_queue(rom_q);
    destroy_queue(rom_q);
    return 0;
}

int send_pcx_logon(struct parse_args *pa)
{
    char *mblk;
    mblk = calloc(600, 1);
    if (mblk != NULL) {
        size_t len =
            sprintf(mblk, "57=ARCA%c98=0%c108=30%c", FIX_FIELD_DELIM,
                    FIX_FIELD_DELIM,
                    FIX_FIELD_DELIM);
        if (len <= 0) {
            send_logon(pa);
        } else {
            send_gen_logon(pa, mblk, len);
        }
        free(mblk);
        return 0;
    }
    return -1;
}

void destroy_rom_queue(queue_t q)
{
    element_t e, temp;
    e = dequeue(q);
    while (e != NULL) {
        temp = e;
        e = temp->next;
        free_init_args((struct init_args *) temp->d);
        free(temp);
    }
}

int pcx_trans_side(char *rommsg, dart_fix_pos * map,
                   char *mtype, databuf_t * fix,
                   size_t field, int *rejected, dart_ex_order_obj * order)
{
    size_t len = map[field].end - map[field].beg;
    char *p = rommsg + map[field].beg;
    if (doa_get_is_spread(order)) {
        fixmsg_addstr2(fix, map[field].name, map[field].len, "B", 1);
    } else if (len > 0) {
        fixmsg_addstr2(fix, map[field].name, map[field].len, p, len);
    }
    return 1;
}

int rom_to_pcx_price(char *rommsg, dart_fix_pos * map,
                     char *mtype, databuf_t * fix,
                     size_t field, int *rejected,
                     dart_ex_order_obj * order)
{
    size_t len = map[field].end - map[field].beg;
    char *p = rommsg + map[field].beg;
    if (len > 0 && doa_get_is_spread(order)) {
        char m = p[0];
        if (m == '-') {
            fixmsg_addstr2(fix, map[field].name, map[field].len, p + 1,
                           len - 1);
            set_val_in_order_struct(p + 1, len + 1, field, order, mtype);
        } else {
            char sub_p[100];
            memset(sub_p, '\0', 100);
            memcpy(sub_p, p, len);
            databuf_write(fix, "%c%s=-%s", FIX_FIELD_DELIM,
                          map[field].name, sub_p);
        }
    } else if (len > 0) {
        fixmsg_addstr2(fix, map[field].name, map[field].len, p, len);
    }
    return 1;
}

int pcx_to_rom_price(char *fixmsg, dart_rom_pos * xlateinfo,
                     char mtype, char *status,
                     databuf_t * rom, size_t field,
                     int *rejtected,
                     dart_ex_order_obj * order,
                     pthread_mutex_t * mutex, hashmap_t map)
{
    size_t len = xlateinfo[field].end - xlateinfo[field].beg;
    char *p = fixmsg + xlateinfo[field].beg;
    if (len > 0 && doa_get_is_spread(order)) {
        char m = p[0];
        if (m == '-') {
            databuf_memcpy(rom, p + 1, len - 1);
        } else {
            char sub_p[100];
            memset(sub_p, '\0', 100);
            memcpy(sub_p, p, len);
            databuf_write(rom, "-%s", sub_p);
        }
    } else if (len > 0) {
        databuf_memcpy(rom, p, len);
    }
    return 1;
}

void fix_server_connection_callBack(con_obj * co, int is_connected,
                                    char *log, int len)
{
    printf("Info about this FIX Connection: %s, Is it connected {%d} ? \n",
           get_name(co), is_connected);
    if (is_connected) {
    } else {
        fix_connection_callback(co, is_connected);
        set_is_pcx_connected(is_connected);
    }
}

void test_lac(con_obj * co)
{
    printf("Low activity reported on this connection: %s \n",
           get_name(co));
}

void con_callBack(con_obj * co, int is_connected, char *log, int len)
{
    printf("Info about this Connection: %s, Is it connected {%d} ? \n",
           get_name(co), is_connected);
    is_con_now = is_connected;
}

void send_reject_to_rom(struct message *mess, databuf_t * output,
                        orderbook * ob, char *l_message,
                        struct message *m1)
{
    m1->name = mess->name;
    m1->name_len = mess->name_len;
    int total_len = create_rom_reject(mess->data, output, l_message);
    if (total_len > 0) {
        m1->data = output->buffer;
        m1->len = databuf_unread(output);
        send_message_to_roms(m1, ob);
    }
}
static void numeric_only_leg_id(dart_ex_order_obj * obj,
                                dart_leg_obj * leg_obj,
                                int leg_pos, databuf_t * output)
{
    char leg_id[5];
    int len = sprintf(leg_id, "%d", leg_pos);
    databuf_write(output, "%c654=", FIX_FIELD_DELIM);
    databuf_memcpy(output, leg_id, (len ));
    doa_set_leg_id(obj, leg_id, (len), leg_obj);
}
static long pcx_trans_rom_spread_leg(char *rom, databuf_t * output,
                          long fix_len, char *fix_type,
                          struct dart_fix_pos positions[ROM_LEN],
                          int *rejected, orderbook * ob,
                          dart_ex_order_obj * parent, dart_leg_obj * lobj)
{
    int i = 0;
    for (; i < ob->leg_array_size; ++i) {
        if (ob->r2fleg[i].rom_tag != 0) {
            if (ob->r2fleg[i].legfunc
                (rom, positions, fix_type, output, ob->r2fleg[i].rom_tag,
                 rejected, parent, lobj) == -1) {
                fprintf(stderr, "%s: Processing %d failed\n", __FUNCTION__,
                        i);
                return 0;
            }
        }
    }

    return 0;
}
static void pcx_spread_loop(char *rom, databuf_t * output,
                        long fix_len, char *fix_type,
                        struct dart_fix_pos positions[ROM_LEN],
                        int *rejected, orderbook * ob,
                        int num_legs, dart_ex_order_obj * obj)
{
    int leg_pos = 1;
    element_t g;
    struct dart_fix_pos l_pos[ROM_LEN];
    build_pos_array(l_pos, ob->rom_to_fix_leg);
    g = front(doa_get_children(obj));
    while (g != NULL) {
        if (g->d != NULL) {
            dart_leg_obj *leg_obj = (dart_leg_obj *) g->d;
            pcx_trans_rom_spread_leg(rom, output, fix_len,
                                 fix_type, l_pos, rejected,
                                 ob, obj, leg_obj);
            numeric_only_leg_id(obj, leg_obj, leg_pos, output);
        }
        ++leg_pos;
        g = g->next;
    }

    if (!*rejected) {
        databuf_memcpy(output, "\x01", 1);
    }
}

long pcx_from_rom_determine_spread(struct message *mess,
                                   char *rom_message, long rom_len,
                                   databuf_t * output,
                                   char *fix_type, int *ft_len,
                                   int *rejected, orderbook * ob,
                                   long (*rom_to_fix_override) (struct
                                                                message *
                                                                mess,
                                                                char
                                                                *fix_message,
                                                                databuf_t *
                                                                output,
                                                                long
                                                                fix_len,
                                                                char
                                                                *fix_type,
                                                                struct
                                                                dart_fix_pos
                                                                positions
                                                                [ROM_LEN],
                                                                int
                                                                *rejected,
                                                                orderbook *
                                                                ob,
                                                                int
                                                                is_spread))
{
    int ret_code = 0;
    if (rom_message[0] == 'E') {
        char *leg_off = NULL;
        if ((leg_off = strstr(rom_message, ROM_SPREAD_SEP)) != NULL
            && (leg_off - rom_message) < rom_len) {
            ret_code =
                deconstruct_rom_spread(mess, rom_message, rom_len, leg_off,
                                       output, fix_type, ft_len, rejected,
                                       ob, 1);
        } else {
            ret_code =
                trans_rom_message(mess, rom_message, rom_len, output,
                                  fix_type, ft_len, rejected, ob,
                                  rom_to_fix_override);
        }
    } else {
        struct fix_tag_def **ftd =
            get_tag_def_bundle_only(rom_message[0], ob);
        struct dart_fix_pos positions[ROM_LEN];
        char *beg_off = NULL;
        long len = 0;
        size_t ret = 0;
        dart_ex_order_obj *obj = NULL;
        build_pos_array(positions, ftd);
        build_rom_pos2(rom_message, rom_len, ftd, positions, 0);
        beg_off = rom_message;
        beg_off += positions[17].beg;
        len = (positions[17].end - positions[17].beg);
        pthread_mutex_lock(&ob->mutex);
        ret =
            get_map_entry(ob->rom_ord_id_map, beg_off, len, (void *) &obj);
        pthread_mutex_unlock(&ob->mutex);
        if (ret) {
            if (doa_get_is_spread(obj)) {
                if (rom_message[0] == 'R') {
                    struct fix_tag_def **ftd =
                        get_tag_def_bundle_only(rom_message[0], ob);
                    struct dart_fix_pos positions[ROM_LEN];
                    char *beg_off = NULL;
                    long len = 0;
                    size_t ret = 0;
                    dart_ex_order_obj *obj = NULL;
                    build_pos_array(positions, ftd);
                    build_rom_pos2(rom_message, rom_len, ftd, positions, 0);
                    beg_off = rom_message;
                    beg_off += positions[17].beg;
                    len = (positions[17].end - positions[17].beg);
                    pthread_mutex_lock(&ob->mutex);
                    ret = get_map_entry(ob->rom_ord_id_map, beg_off, len, (void *) &obj);
                    pthread_mutex_unlock(&ob->mutex);
                    if (ret) {
                        if (doa_get_is_spread(obj) && !(rom_message[0] == 'C' && ob->no_fix_43)) {

                            set_fix_type(rom_message[0], fix_type, ft_len, 1, ob->no_fix_43);
                            rom2fix_translate_fields(mess, rom_message,
                                                     output, rom_len,
                                                     fix_type, positions, rejected, ob,1);
                            if(doa_get_num_legs(obj) > 0) {
                                databuf_write(output, "555=%d", doa_get_num_legs(obj));
                                pcx_spread_loop(rom_message, output, rom_len, fix_type,
                                            positions, rejected, ob,
                                            doa_get_num_legs(obj), obj);
                            }
                        }
                    }
                    /**rejected = 1;
                    char *l_message =
                        "Replacing Spread orders is not allowed on ARCA.";
                    create_rom_reject(rom_message, output, l_message);*/
                } else {
                    memcpy(fix_type, "F", 1);
                    *ft_len = 1;
                    ob->add_header_defs(rom_message, positions, fix_type,
                                        output, 0, rejected, obj);
                    size_t t_len = 0;
                    char *l_type = doa_get_type(obj, &t_len);
                    size_t clordid_len = 0;
                    size_t rep_id_len = 0;
                    char *rep_ord_id = ob->generate_id(&rep_id_len);
                    char *cl_ord_id = doa_get_cl_ord_id(obj, &clordid_len);
                    fixmsg_addstr_bylen(output, "11", rep_ord_id,
                                        rep_id_len);
                    doa_set_rep_ord_id(obj, rep_ord_id, rep_id_len);
                    pthread_mutex_lock(&ob->mutex);
                    no_copy_insert(ob->cl_ord_id_map, rep_ord_id,
                                   rep_id_len, obj);
                    pthread_mutex_unlock(&ob->mutex);
                    fixmsg_addstr_bylen(output, "41", cl_ord_id,
                                        clordid_len);

                    free(l_type);
                    free(rep_ord_id);
                    free(cl_ord_id);
                    element_t e;
                    e = front(doa_get_children(obj));
                    dart_leg_obj *leg_obj = (dart_leg_obj *) e->next->d;
                    if (leg_obj != NULL) {
                        fixmsg_addstr_bylen(output, "54", "B", 1);
                        char *qty = rom_message;
                        qty += positions[6].beg;
                        fixmsg_addstr_bylen(output, "38", qty,
                                            positions[6].end -
                                            positions[6].beg);
                        fixmsg_addstr_bylen(output, "167", "OPT", 3);
                        int llen = 0;
                        char *pc = doa_get_leg_call_put(obj, leg_obj, &llen);
                        if (pc && pc[0] == 'C') {
                            databuf_write(output, "%c%s=%s",
                                          FIX_FIELD_DELIM, "201", "1");
                        } else {
                            databuf_write(output, "%c%s=%s",
                                          FIX_FIELD_DELIM, "201", "0");
                        }
                        char *sym = doa_get_leg_symbol(obj, leg_obj, &llen);
                        databuf_write(output, "%c%s=%s", FIX_FIELD_DELIM,
                                      "55", sym);
                        char *exp_date =
                            doa_get_leg_expire_date(obj, leg_obj, &llen);
                        if(llen > 0) {
                            fixmsg_addstr_bylen(output, "200", exp_date, 6);
                            fixmsg_addstr_bylen(output, "205", exp_date + 6,
                                            2);
                            free(exp_date);
                        }

                        char *strike = doa_get_leg_strike(obj, leg_obj, &llen);
                        if(llen > 0) {
                            databuf_write(output, "%c%s=%s", FIX_FIELD_DELIM,
                                          "202", strike);
                            free(strike);
                        }
                        databuf_memcpy(output, "\x01", 1);
                        free(pc);
                        free(sym);
                    }
                }
            } else {
                set_fix_type(rom_message[0], fix_type, ft_len, 0,
                             ob->no_fix_43);
                rom_to_fix_override(mess, rom_message, output, rom_len,
                                    fix_type, positions, rejected, ob, 0);
            }
        }
    }
    return databuf_unread(output);
}

void pcx_complex_from_rom_func(struct message *mess, void *b)
{
    databuf_t *output = databuf_alloc(mess->len * 8);
    orderbook *ob = (orderbook *) b;
    long total_len;
    struct message m1;
    struct message log;
    char fix_type[3];
    int ft_len = 0;
    int rejected = 0;
    total_len = 0;

    log.name = mess->name;
    log.name_len = mess->name_len;
    log.data = mess->data;
    log.len = mess->len;
    dart_rolling_log_coming(&log, ob->module_name, ob->module_name_len);

    if (ob->is_connected()) {
        m1.name = mess->name;
        m1.name_len = mess->name_len;
        memset(fix_type, 0, 3);
        total_len =
            pcx_from_rom_determine_spread(&m1, mess->data, mess->len,
                                          output, fix_type, &ft_len,
                                          &rejected, (orderbook *) ob,
                                          &rom2fix_translate_fields);
        if (rejected) {
            m1.data = output->buffer;
            m1.len = databuf_unread(output);
            send_message_to_roms(&m1, ob);
        } else if (total_len > 0) {
            m1.name = ob->fix_con_name;
            m1.name_len = ob->fix_con_len;
            m1.data = output->buffer;
            m1.len = databuf_unread(output);
            send_fix_message(&m1, fix_type, ft_len);
        }
    } else {
        rejected = 1;
        char *l_message = "Not connected to Exchange.";
        send_reject_to_rom(mess, output, ob, l_message, &m1);
    }
    databuf_destroy(output);
}
