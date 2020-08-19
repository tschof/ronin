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
#include "order_token_creator.h"
#include "complex_parser.h"
#include "options_common.h"
#include "fixmsg.h"
static pthread_mutex_t connection_lock = PTHREAD_MUTEX_INITIALIZER;

static token_struct_t *idgenerator = 0;
static char datestr[9] = { '\0' };

static char *fix_client_name;
static int fix_client_len;

static char *module_name = NULL;        // = databuf_alloc(100);
static int module_name_len = 0;

static int is_con_now = 0;
static int is_ise_connected = 1;

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

static char *ise_generate_id(size_t * nlen)
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


int ise_xlate_billing_code(char *fixmsg, dart_rom_pos * positions,
                     char mtype, char *status,
                     databuf_t * rom, size_t romfield,
                     int *rejtected, dart_ex_order_obj * order,
                     pthread_mutex_t * mutex, hashmap_t ord_map)
{
    char *beg_off = fixmsg;
    size_t len = (positions[romfield].end - positions[romfield].beg);
    beg_off += positions[romfield].beg;
    size_t len2 = positions[74].end - positions[74].beg;
    char* counter_cf = fixmsg;
    counter_cf += positions[74].beg;
    if(len2 > 0) {
        databuf_memcpy(rom, beg_off, len);
        databuf_memcpy(rom, counter_cf, len2);
    } else {
        databuf_memcpy(rom, beg_off, len);
    }
    return 1;
}

int get_is_ise_connected()
{
    int ret = 0;
    pthread_mutex_lock(&connection_lock);
    ret = is_ise_connected;
    pthread_mutex_unlock(&connection_lock);
    return ret;
}

void set_is_ise_connected(int is)
{
    pthread_mutex_lock(&connection_lock);
    is_ise_connected = is;
    pthread_mutex_unlock(&connection_lock);
}

int ise_tras_text(char *rom, romindex_t * map, char *type,
                  databuf_t * fix, size_t field, int *rejected,
                  dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[12].end - map[12].beg;
        if (len > 0) {
            char *p = rom + map[12].beg;
            if (len == 8 && (strncmp((p + 4), "1209", 4) == 0)) {
                len = 4;
            }
            //set_val_in_order_struct(p, len, field, order, type);
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
        } else if (rom[0] != 'E') {
            char *p = doa_get_account(order, &len);
            if (len > 0 && p != NULL) {
                if (len == 8 && (strncmp((p + 4), "1209", 4) == 0)) {
                    len = 4;
                }
                result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
                free(p);
            }
        }
    }
    return result;
}

int ise_trans_clid(char *rom, romindex_t * map, char *type,
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
        } else if (rom[0] != 'E') {
            char *p = doa_get_clr_id(order, &len);
            if (len > 0 && p != NULL) {
                result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
                free(p);
            }
        }
    }
    return result;
}

int ise_tras_ex_ist(char *rom, romindex_t * map, char *type,
                    databuf_t * fix, size_t field, int *rejected,
                    dart_ex_order_obj * order)
{

    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        char *p = rom + map[field].beg;
        if (memchr(p, 'f', len) != 0) {
            databuf_reset(fix);
            *rejected = 1;
            result =
                create_rom_reject(rom, fix,
                                  "ISO Sweep orders not allowed");
        } else if(len == 1 && p[0] == 'A') {
            result = fixmsg_addstr_bylen(fix, "9202", p, len);
        } else {
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
        }
    }
    return result;
}

int set_ise_defaults(char *rommsg, struct dart_fix_pos *positions,
                     char *mtype, databuf_t * output,
                     size_t romfield, int *rejected,
                     dart_ex_order_obj * order)
{
    databuf_write(output, "%c21=1", FIX_FIELD_DELIM);
    databuf_write(output, "%c207=XISX", FIX_FIELD_DELIM);
    return 1;
}

void send_cancel_all();
void con_callBack(con_obj * co, int is_connected, char *log, int len);
void fix_server_connection_callBack(con_obj * co, int is_connected,
                                    char *log, int len);
void test_lac(con_obj * co);
void destroy_rom_queue(queue_t q);
int send_ise_logon(struct parse_args *pa);

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
    ex = init_config("ise.dat", 13);
    fix_args = load_fix_config("ise.dat", 13);
    load_rom_config(ex->dc, rom_q);
    fix_client_name = fix_args->target_comp;
    fix_client_len = fix_args->tc_len;
    init_date_str();
    idgenerator = init_order_token_creator();
    ob = init_order_book(ise_generate_id, get_is_ise_connected,
                         def_rom2fix, set_ise_defaults,
                         def_xlate_fixrom, ex->order_allocation,
                         ex->mod_name, ex->mod_name_len, fix_client_name,
                         fix_client_len, NULL, 0);
    opt_database_init(&ex->db, ex->dest_id, ob, 0);
    set_option_rom2fix(ob);
    ob->rom2fix[57] = ise_tras_ex_ist;
    ob->rom2fix[15] = ise_tras_text;
    ob->rom2fix[11] = ise_trans_clid;
    ob->rom2fix[50] = set_transacttime;
    set_option_fix_to_rom(ob);
    fs_set_spread_book(ob);
    set_rom2fix_leg_book(ob);
    ob->fix2rom[14] = def_fixrom_status_xlate;
    ob->fix2rom[69] = ise_xlate_billing_code;
    module_name_len = ex->mod_name_len;
    module_name = calloc(module_name_len + 1, 1);
    memcpy(module_name, ex->mod_name, module_name_len);;
    start_dart_mail_service(ex->mail_server, ex->mail_server_len,
                            ex->biz_group, ex->b_len, ex->tech_group,
                            ex->t_len, ex->end_hour, ex->end_min, 7);


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
                     fix_args, send_ise_logon, build_fix_offsets,
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

            if (!get_is_ise_connected()) {
                set_is_ise_connected(1);
                ret =
                    dart_connect(&from_fix_func, ex->mod_name,
                                 ex->mod_name_len, ex->fix_port,
                                 ex->fix_ip, ex->fix_ip_len, ex->end_hour,
                                 ex->end_min, 7, ex->heart_beat_int, ob,
                                 fix_server_connection_callBack,
                                 parse_fix_message, fix_args,
                                 send_ise_logon, build_fix_offsets,
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

int send_ise_logon(struct parse_args *pa)
{
    char *mblk;
    mblk = calloc(600, 1);
    if (mblk != NULL) {
        size_t len = sprintf(mblk, "98=0%c108=30%c", FIX_FIELD_DELIM,
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

void fix_server_connection_callBack(con_obj * co, int is_connected,
                                    char *log, int len)
{
    printf("Info about this FIX Connection: %s, Is it connected {%d} ? \n",
           get_name(co), is_connected);
    if (is_connected) {
    } else {
        fix_connection_callback(co, is_connected);
        set_is_ise_connected(is_connected);
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
