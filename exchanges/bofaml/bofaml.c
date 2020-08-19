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
#include "version.h"
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
#include "order_token_creator.h"
#include "complex_parser.h"
#include "options_common.h"
#include "string_table.h"
static pthread_mutex_t connection_lock = PTHREAD_MUTEX_INITIALIZER;

static token_struct_t *idgenerator = 0;
static char datestr[9] = { '\0' };

static char *fix_client_name;
static int fix_client_len;
//static char *sender_sub_id = 0;
static const char mapping_key_name[] = "ClearingFirmMap";
static strtable_t *clearing_id_table;
static char *module_name = NULL;        // = databuf_alloc(100);
static int module_name_len = 0;
static hashmap_t bofaml_route_codes;

static int is_con_now = 0;
static int is_bofaml_connected = 1;
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
    sprintf(datestr, "%02d%02d%04d", month, ti->tm_mday, year);
}


static void bofaml_add_to_map(hashmap_t map, char *buffer, long len)
{
    char *off;
    long c_offset = 0;
    off = strchr(buffer, ',');
    if (off) {
        char *tag;
        char *val_off;
        c_offset = off - buffer;
        tag = calloc(c_offset + 1, 1);
        memcpy(tag, buffer, c_offset);

        val_off = buffer;
        val_off += c_offset + 1;
        insert_map(map, tag, c_offset, val_off,len - (c_offset + 1));
        free(tag);
    } else {
        printf("Could not find a comma in this mess: %s \n", buffer);
    }
}

int  bofaml_build_map_from_file(const char *filename, hashmap_t map)
{
    char *buffer;
    long size;
    size = 0;
    buffer = NULL;
    buffer = get_file_contents(filename, &size);
    if (size > 0) {
        long cur_pos = 0;
        char eol[2] = { '\n', '\0' };
        long end_pos = 0;
        int should_parse = 1;
        char *d_off = buffer;
        while (cur_pos < size && should_parse) {
            end_pos = find_offset(d_off, (size - cur_pos), eol, 1);
            if (end_pos > 0) {
                bofaml_add_to_map(map, d_off, end_pos);
                d_off += end_pos + 1;
                cur_pos += end_pos + 1;
            } else {
                should_parse = 0;
            }
        }
        free(buffer);
    } else {
        return 1;
    }
    return 0;
}

int bofaml_trans_account(char *rom, romindex_t * map, char *type,
                          databuf_t * fix, size_t field, int *rejected,
                          dart_ex_order_obj * order);
int bofaml_trans_mm_account(char *rom, romindex_t * map, char *type,
                          databuf_t * fix, size_t field, int *rejected,
                          dart_ex_order_obj * order);
int bofaml_trans_capacity(char *rom, romindex_t * map, char *type,
                        databuf_t * fix, size_t field, int *rejected,
                        dart_ex_order_obj * order);
int bofaml_trans_routes(char *rom, romindex_t * map, char *type,
                        databuf_t * fix, size_t field, int *rejected,
                        dart_ex_order_obj * order);
int bofaml_trans_expire_my(char *rom, romindex_t * map, char *type,
                         databuf_t * fix, size_t field, int *rejected,
                         dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0 && !doa_get_is_spread(order)) {
        size_t len = map[field].end - map[field].beg;
        if (len > 0) {
            char *p = rom + map[field].beg;
            set_val_in_order_struct(p, len, field, order, type);
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
        } else if (rom[0] != 'E' && map[field].name != NULL) {
            char *p = doa_get_expireMonYear(order, &len);
            if (len > 0 && p != NULL) {
                result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
                free(p);
            }
        }
    }
    return result;
}
int bofaml_clearing_id_table_mapping(char *rom, romindex_t * map, char *type,
                                  databuf_t * fix, size_t field,
                                  int *rejected, dart_ex_order_obj * order)
{
    int retval = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        if (len > 0) {
            const char *ptr = rom + map[field].beg;
            set_val_in_order_struct((char*)ptr, len, field, order, type);
            str_t *val = lookup_str(clearing_id_table, ptr, (size_t) len);
            if (val != 0 && val->len > 0) {
                retval = fixmsg_addstr_bylen(fix, map[field].name,
                                             val->str, val->len);
            }
        } else {
            char *p = doa_get_clr_id(order, &len);
            if (len > 0 && p != NULL) {
                str_t *val = lookup_str(clearing_id_table, p, (size_t) len);
                if (val != 0 && val->len > 0) {
                    retval = fixmsg_addstr_bylen(fix, map[field].name,
                                                 val->str, val->len);
                }
                free(p);
            }
        }

    }
    return retval;
}

int bofaml_trans_mat_day(char *rom, romindex_t * map, char *type,
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

static char *bofaml_generate_id(size_t * nlen)
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

int get_is_bofaml_connected()
{
    int ret = 0;
    pthread_mutex_lock(&connection_lock);
    ret = is_bofaml_connected;
    pthread_mutex_unlock(&connection_lock);
    return ret;
}

void set_is_bofaml_connected(int is)
{
    pthread_mutex_lock(&connection_lock);
    is_bofaml_connected = is;
    pthread_mutex_unlock(&connection_lock);
}

int set_bofaml_defaults(char *rommsg, struct dart_fix_pos *positions,
                      char *mtype, databuf_t * output,
                      size_t romfield, int *rejected,
                      dart_ex_order_obj * order)
{
    //databuf_write(output, "%c50=%s", FIX_FIELD_DELIM, sender_sub_id);
    if(mtype[0] != 'F') {
        databuf_write(output, "%c21=1", FIX_FIELD_DELIM);
    }
    return 1;
}

int set_bofaml_transacttime(char *rom, romindex_t * map, char *type,
                          databuf_t * fix, size_t field, int *rejected,
                          dart_ex_order_obj * order)
{
    if (map[field].name != 0) {
        struct tm *gmt = 0;
        time_t now = 0;
        time(&now);
        struct tm timestruct;
        gmt = gmtime_r(&now, &timestruct);
        return fixmsg_addtime(fix, "60", gmt);
    } else {
        return 0;
    }
}

void send_cancel_all();
void con_callBack(con_obj * co, int is_connected, char *log, int len);
void fix_server_connection_callBack(con_obj * co, int is_connected,
                                    char *log, int len);
void test_lac(con_obj * co);
void destroy_rom_queue(queue_t q);
int send_bofaml_logon(struct parse_args *pa);

int main(int argc, char **argv)
{
    int ret;
    struct init_args *fix_args;
    ex_config *ex = NULL;
    struct tm *newtime;
    int keep_running = 1;
    queue_t rom_q;
    time_t long_time;
    orderbook *ob = NULL;       // init_order_book(next_order_id, def_rom2fix,50000);
    rom_q = create_queue();
    ex = init_config("bofaml.dat", 10);
    fix_args = load_fix_config("bofaml.dat", 10);
    load_rom_config(ex->dc, rom_q);
    fix_client_name = fix_args->target_comp;
    fix_client_len = fix_args->tc_len;
    init_date_str();
    idgenerator = init_order_token_creator();
    ob = init_order_book(bofaml_generate_id, get_is_bofaml_connected,
                         def_rom2fix, set_bofaml_defaults,
                         def_xlate_fixrom, ex->order_allocation,
                         ex->mod_name, ex->mod_name_len, fix_client_name,
                         fix_client_len, NULL, 0);
    opt_database_init(&ex->db, ex->dest_id, ob, 0);
    set_option_rom2fix(ob);
    set_option_fix_to_rom(ob);
    fs_set_spread_book(ob);
    set_rom2fix_leg_book(ob);
    ob->fix2rom[14] = def_fixrom_status_xlate;
    ob->rom2fix[10] = bofaml_trans_capacity;
    ob->rom2fix[29] = bofaml_trans_routes;
    ob->rom2fix[50] = set_bofaml_transacttime;
    ob->rom2fix[30] = bofaml_trans_expire_my;
    ob->fix2rom[15] = def_handle_ordids;
    ob->rom2fix[36] = bofaml_trans_mm_account;
    ob->rom2fix[12] = bofaml_trans_account;
    ob->rom2fix[62] = bofaml_trans_mat_day;
    module_name_len = ex->mod_name_len;
    module_name = calloc(module_name_len + 1, 1);
    memcpy(module_name, ex->mod_name, module_name_len);;
    start_dart_mail_service(ex->mail_server, ex->mail_server_len,
                            ex->biz_group, ex->b_len, ex->tech_group,
                            ex->t_len, ex->end_hour, ex->end_min, 7);


    char *clearing_firm_mapping_rule
        = get_val_for_tag(fix_args->dc, "FIX SERVER", 10, mapping_key_name,
                          sizeof mapping_key_name - 1, &ret);
    if (clearing_firm_mapping_rule) {
        if ((clearing_id_table
             = parse_table_conf(clearing_firm_mapping_rule)) != 0) {
            ob->rom2fix[11] = bofaml_clearing_id_table_mapping;
        } else {
            fprintf(stderr,
                    "Fatal error: cannot parse configuration rule for %s\n",
                    mapping_key_name);

            return 1;
        }
        free(clearing_firm_mapping_rule);
    }
    bofaml_route_codes = create_map(1000);
    bofaml_build_map_from_file("BOFAML_ROUTES", bofaml_route_codes);
    ret =
        start_dart_client_services(&complex_from_rom_func, ex->mod_name,
                                   ex->mod_name_len, ex->rom_port, rom_q,
                                   ex->rom_ip, ex->rom_ip_len,
                                   ex->end_hour, ex->end_min, 7,
                                   ex->heart_beat_int, ob, con_callBack,
                                   parse_rom_message, build_offsets,
                                   test_lac, create_rom_resend_pad, 0);
    ret =
        dart_connect(&from_fix_func, ex->mod_name, ex->mod_name_len,
                     ex->fix_port, ex->fix_ip, ex->fix_ip_len,
                     ex->end_hour, ex->end_min, 7, ex->heart_beat_int, ob,
                     fix_server_connection_callBack, parse_fix_message,
                     fix_args, send_bofaml_logon, build_fix_offsets,
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
            if (!get_is_bofaml_connected()) {
                set_is_bofaml_connected(1);
                ret =
                    dart_connect(&from_fix_func, ex->mod_name,
                                 ex->mod_name_len, ex->fix_port,
                                 ex->fix_ip, ex->fix_ip_len, ex->end_hour,
                                 ex->end_min, 7, ex->heart_beat_int, ob,
                                 fix_server_connection_callBack,
                                 parse_fix_message, fix_args,
                                 send_bofaml_logon, build_fix_offsets,
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

int send_bofaml_logon(struct parse_args *pa)
{
    char *mblk;
    mblk = calloc(600, 1);
    if (mblk != NULL) {
        size_t len =
            sprintf(mblk, "98=0%c108=30%c", 
                    FIX_FIELD_DELIM, FIX_FIELD_DELIM);
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

static char get_security_exchange(char *mm_home)
{
    int ihome = strtol(mm_home, NULL, 10);
    switch (ihome) {
    case 42:
        return '1';
    case 35:
        return 'X';
    case 22:
        return 'W';
    case 21:
        return 'Y';
    case 34:
        return 'N';
    case 44:
        return 'B';
    case 5:
        return 'Q';
    case 261:
        return 'C';
    default:
        return 'Z';
    }
}

int bofaml_trans_account(char *rom, romindex_t * map, char *type,
                          databuf_t * fix, size_t field, int *rejected,
                          dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        if (len > 0) {
            char *p = rom + map[field].beg;
            char* w_pro = calloc(1, len + 5);
            memcpy(w_pro, p, len);
            memcpy(w_pro + len, "-PRO", 4);
            set_val_in_order_struct(w_pro, len + 4, field, order, type);
            result = fixmsg_addstr_bylen(fix, map[field].name, w_pro, len + 4);
            free(w_pro);
        } else if (rom[0] != 'E') {
            char *p = doa_get_account(order, &len);
            if ((len > 0 && p != NULL) ) {
                result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
            }
            if (p) {
                free(p);
            }
        }
    }
    return result;
}

int bofaml_trans_mm_account(char *rom, romindex_t * map, char *type,
                          databuf_t * fix, size_t field, int *rejected,
                          dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        if (len > 0) {
            char *p = rom + map[field].beg;
            set_val_in_order_struct(p, len, field, order, type);
            size_t mm_home_len =
                map[ROM_MM_HOME].end - map[ROM_MM_HOME].beg;
            if (mm_home_len > 0) {
                char *mm_home = rom + map[ROM_MM_HOME].beg;
                char sec_exch = get_security_exchange(mm_home);
                if (sec_exch != 'X') {
                    result =
                        fixmsg_addstr_bylen(fix, map[field].name, p, len);
                }
            } else {
                result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
            }
        } else if (rom[0] != 'E') {
            char *p = doa_get_mm_account(order, &len);
            size_t clen = 0;
            char *c = doa_get_capacity(order, &clen);
            if ((len > 0 && p != NULL) && (clen > 0 && c[0] != '6')) {
                result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
            }
            if (p) {
                free(p);
            }
            if (c) {
                free(c);
            }
        }
    }
    return result;
}

int bofaml_trans_capacity(char *rom, romindex_t * map, char *type,
                        databuf_t * fix, size_t field, int *rejected,
                        dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[10].end - map[10].beg;
        char *p = rom + map[10].beg;
        if (len > 0 && p[0] != '\0' && rom[0] == 'E') {
            char cust_firm = '1';
            if (p[0] == 'B') {
                //do nothing
            } else if(p[0] == 'X') {
                cust_firm = '0';   
            } else {
                cust_firm = '1';
                size_t mm_home_len =
                    map[ROM_MM_HOME].end - map[ROM_MM_HOME].beg;
                if (mm_home_len > 0) {
                    char *mm_home = rom + map[ROM_MM_HOME].beg;
                    char sec_exch = get_security_exchange(mm_home);
                    if (sec_exch == 'X') {
                        cust_firm = '1';
                        char *mm_account = rom + map[ROM_MM_ACC].beg;
                        int acc_len =
                            map[ROM_MM_ACC].end - map[ROM_MM_ACC].beg;
                        result =
                            fixmsg_addstr_bylen(fix, "109", mm_account,
                                                acc_len);
                    }
                    doa_set_mm_home(order, &sec_exch, 1);
                }
            }
            doa_set_capacity(order, &cust_firm, 1);
            result = fixmsg_addstr_bylen(fix, map[10].name, &cust_firm, 1);
        } else {
            p = doa_get_capacity(order, &len);
            if (p != 0 && len > 0) {
                int should_free = 1;
                if (p[0] == 'B') {
                    result =
                        fixmsg_addstr_bylen(fix, map[10].name, "1", 1);
                    doa_set_capacity(order, "1", 1);
                    free(p);
                    should_free = 0;
                    p = "1";
                    len = 1;
                } else if (p[0] == 'S') {
                    size_t s_len = 0;
                    char *s = doa_get_mm_home(order, &s_len);
                    if (s != 0 && s_len > 0) {
                        char sec_exch = get_security_exchange(s);
                        if (sec_exch == 'X') {
                            result =
                                fixmsg_addstr_bylen(fix, map[10].name, "1",
                                                    1);
                            doa_set_capacity(order, "1", 1);
                            free(p);
                            should_free = 0;
                            p = "1";
                            len = 1;
                        } else {
                            result =
                                fixmsg_addstr_bylen(fix, map[10].name, "1",
                                                    1);
                            doa_set_capacity(order, "1", 1);
                            free(p);
                            should_free = 0;
                            p = "1";
                            len = 1;
                        }
                        free(s);
                    } else {
                        result =
                            fixmsg_addstr_bylen(fix, map[10].name, "1", 1);
                    }

                } else if(p[0] == 'X') {
                    result =
                        fixmsg_addstr_bylen(fix, map[10].name, "0", 1);
                    doa_set_capacity(order, "0", 1);
                    free(p);
                    should_free = 0;
                    p = "0";
                    len = 1;
                } else {
                    result =
                        fixmsg_addstr_bylen(fix, map[10].name, p, len);
                }
                if (p[0] == '4' || p[0] == '6') {
                    if (p[0] == '6') {
                        size_t acc_len = 0;
                        char *mm_account =
                            doa_get_mm_account(order, &acc_len);
                        result =
                            fixmsg_addstr_bylen(fix, "109", mm_account,
                                                acc_len);
                        free(mm_account);
                    }

                }
                if (should_free) {
                    free(p);
                }
            }
        }
    }
    return result;
}

int bofaml_trans_routes(char *rom, romindex_t * map, char *type,
                        databuf_t * fix, size_t field, int *rejected,
                        dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        if (len > 0) {
            char *p = rom + map[field].beg;
            doa_set_dest_route(order, p, len);
            set_val_in_order_struct(p, len, field, order, type);
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
        } else {
            len = map[13].end - map[13].beg;
            if(len > 0) {
                char* p = rom + map[13].beg;
                char* route = 0;
                int found = find_n_fill(bofaml_route_codes, p, len, (void *) &route);
                if(found) {
                    result = fixmsg_addstr_bylen(fix, map[field].name, route, found);
                    doa_set_dest_route(order, route, found);
                }
            } else {
                size_t route_len = 0;
                char* route = doa_get_dest_route(order, &route_len);
                if(route_len > 0) {
                    result = fixmsg_addstr_bylen(fix, map[field].name, route, route_len);
                }
            }
        }
    }
    return result;
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

void fix_server_connection_callBack(con_obj * co, int is_connected,
                                    char *log, int len)
{
    char *n = (char *) get_name(co);
    printf("Info about this FIX Connection: %s, Is it connected {%d} ? \n",
           n, is_connected);
    if (is_connected) {
    } else {
        fix_connection_callback(co, is_connected);
        set_is_bofaml_connected(is_connected);
    }
    if (n) {
        free(n);
    }
}

void test_lac(con_obj * co)
{
    char *n = (char *) get_name(co);
    printf("Low activity reported on this connection: %s \n", n);
    if (n) {
        free(n);
    }
}

void con_callBack(con_obj * co, int is_connected, char *log, int len)
{
    char *n = (char *) get_name(co);
    printf("Info about this Connection: %s, Is it connected {%d} ? \n",
           n, is_connected);
    is_con_now = is_connected;
    if (n) {
        free(n);
    }
}
