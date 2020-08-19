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
#include "hashmap.h"
#include "globex_display_fact.h"
static pthread_mutex_t connection_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t config_lock = PTHREAD_MUTEX_INITIALIZER;
static hashmap_t display_factors;
static hashmap_t sender_sub_ids;
static hashmap_t clearing_ids;

static token_struct_t *idgenerator = 0;
static char datestr[9] = { '\0' };

static char *fix_client_name;
static int fix_client_len;

static char *module_name = NULL;        // = databuf_alloc(100);
static int module_name_len = 0;

static int is_globex_connected = 1;
static int can_send_orders = 0;
static char *sender_comp_id = 0;
static int sci_len = 0;
static char *firm_id = 0;
static int fid_len = 0;
static char *sender_location_id = 0;
static int sen_loc_len = 0;
static char *session_id = 0;
static int sess_id_len = 0;
static char *customer_type_code = 0;
static int ctc_len = 0;
static char *customer_or_firm = 0;
static int cof_len = 0;
static char *fti = 0;
static int fti_len = 0;
static char *password = 0;
static int pass_len = 0;
static char *app_name = 0;
static int app_name_len = 0;
static char *app_version = 0;
static int app_version_len = 0;
static char *app_vender = 0;
static int app_vender_len = 0;
static char *fee_billing = 0;
static char *end_day = 0;
static int end_day_len = 0;
static int fee_bill_len = 0;
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

static char *globex_generate_id(size_t * nlen)
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

int get_is_globex_connected()
{
    int ret = 0;
    pthread_mutex_lock(&connection_lock);
    ret = is_globex_connected;
    pthread_mutex_unlock(&connection_lock);
    return ret;
}

void set_is_globex_connected(int is)
{
    pthread_mutex_lock(&connection_lock);
    is_globex_connected = is;
    pthread_mutex_unlock(&connection_lock);
}

int get_can_send_orders()
{
    int ret = 0;
    pthread_mutex_lock(&connection_lock);
    ret = can_send_orders;
    pthread_mutex_unlock(&connection_lock);
    return ret;
}

void set_can_send_orders(int can)
{
    pthread_mutex_lock(&connection_lock);
    can_send_orders = can;
    pthread_mutex_unlock(&connection_lock);
}

int set_globex_defaults(char *rommsg, struct dart_fix_pos *positions,
                        char *mtype, databuf_t * output,
                        size_t romfield, int *rejected,
                        dart_ex_order_obj * order)
{
    databuf_write(output, "%c21=1", FIX_FIELD_DELIM);
    databuf_write(output, "%c204=%s", FIX_FIELD_DELIM, customer_or_firm);
    databuf_write(output, "%c9702=%s", FIX_FIELD_DELIM,
                  customer_type_code);
    databuf_write(output, "%c9706=%s", FIX_FIELD_DELIM, fee_billing);
    if (mtype && (mtype[0] == 'F' || mtype[0] == 'G')) {
        databuf_write(output, "%c9768=Y", FIX_FIELD_DELIM);
    }
    return 1;
}

int rom_to_globex_program(char *rommsg, dart_fix_pos * xlateinfo,
                          char *mtype, databuf_t * fix,
                          size_t romfield, int *rejected,
                          dart_ex_order_obj * order);
int globex_cmplx_type(char *rommsg, dart_fix_pos * xlateinfo,
                      char *mtype, databuf_t * fix,
                      size_t romfield, int *rejected,
                      dart_ex_order_obj * order);
int globex_sec_def_trans(char *rommsg, dart_fix_pos * xlateinfo,
                         char *mtype, databuf_t * fix,
                         size_t romfield, int *rejected,
                         dart_ex_order_obj * order);
int rom_to_globex_account(char *rommsg, dart_fix_pos * xlateinfo,
                          char *mtype, databuf_t * fix,
                          size_t romfield, int *rejected,
                          dart_ex_order_obj * order);
int rom_to_globex_sender_sub(char *rommsg, dart_fix_pos * map,
                             char *mtype, databuf_t * fix,
                             size_t field, int *rejected,
                             dart_ex_order_obj * order);
int rom_to_globex_tif(char *rom, romindex_t * map, char *type,
                      databuf_t * fix, size_t field, int *rejected,
                      dart_ex_order_obj * order);

int rom_to_globex_price(char *rommsg, dart_fix_pos * xlateinfo,
                        char *mtype, databuf_t * fix,
                        size_t romfield, int *rejected,
                        dart_ex_order_obj * order);
int globex_to_rom_price(char *fixmsg, dart_rom_pos * xlateinfo,
                        char mtype, char *status,
                        databuf_t * rom, size_t romfield,
                        int *rejtected,
                        dart_ex_order_obj * order,
                        pthread_mutex_t * mutex, hashmap_t map);
int globex_to_rom_spread(char *fixmsg, dart_rom_pos * xlateinfo,
                         char mtype, char *status,
                         databuf_t * rom, size_t romfield,
                         int *rejtected,
                         dart_ex_order_obj * order,
                         pthread_mutex_t * mutex, hashmap_t map);

int globex_to_rom_symbol(char *fixmsg, dart_rom_pos * xlateinfo,
                         char mtype, char *status,
                         databuf_t * rom, size_t field,
                         int *rejtected,
                         dart_ex_order_obj * order,
                         pthread_mutex_t * mutex, hashmap_t map);
int globex_symbol_hack(char *rommsg, dart_fix_pos * map,
                       char *type, databuf_t * fix,
                       size_t field, int *rejected,
                       dart_ex_order_obj * order);

int rom_to_globex_smpid(char *rommsg, dart_fix_pos * map,
                             char *mtype, databuf_t * fix,
                             size_t field, int *rejected,
                             dart_ex_order_obj * order);
void send_cancel_all();
void con_callBack(con_obj * co, int is_connected, char *log, int len);
void fix_server_connection_callBack(con_obj * co, int is_connected,
                                    char *log, int len);
void test_lac(con_obj * co);
void destroy_rom_queue(queue_t q);
int send_globex_logon(struct parse_args *pa);
static void globex_from_rom_func(struct message *mess, void *b);
int set_globex_exTag(char *mess, struct dart_fix_pos *positions,
                     char *mtype, databuf_t * output, size_t i,
                     int *rejected, dart_ex_order_obj * ob);

size_t create_globex_fix_header(databuf_t * fixmsg,
                                const char *sendercompid,
                                size_t sendercomplen,
                                const char *targetcompid,
                                size_t targetcomplen,
                                const char *sendersubid,
                                size_t sendersublen,
                                const char *targetsubid,
                                size_t targetsublen,
                                unsigned long seq, time_t * now);


int globex_main()
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
    pthread_mutex_lock(&config_lock);
    display_factors = load_display_factors("GlobexPriceMult.map");
    sender_sub_ids = load_sender_sub_ids("GlobexSenderSub.map");
    clearing_ids = load_clearing_ids("GlobexClearingIDs.map");
    pthread_mutex_unlock(&config_lock);
    ex = init_config("GlobexConfig.dat", 16);
    fix_args = load_fix_config("GlobexConfig.dat", 16);
    load_rom_config(ex->dc, rom_q);
    fix_client_name = fix_args->target_comp;
    fix_client_len = fix_args->tc_len;
    init_date_str();
    fix_args->create_mess_header = create_globex_fix_header;
    sender_comp_id = fix_args->sender_comp;
    sci_len = fix_args->sc_len;
    idgenerator = init_order_token_creator();
    ob = init_order_book(globex_generate_id, get_can_send_orders,
                         def_rom2fix, set_globex_defaults,
                         def_xlate_fixrom, ex->order_allocation,
                         ex->mod_name, ex->mod_name_len, fix_client_name,
                         fix_client_len, NULL, 0);
    opt_database_init(&ex->db, ex->dest_id, ob, 0);

    ob->rom2fix[37] = def_rom2fix_no_op;
    ob->fix2rom[15] = def_handle_ordids;
    ob->fix2rom[17] = def_add_rom_tag;
    ob->rom2fix[35] = def_trans_sec_type;
    ob->rom2fix[38] = def_trans_open_close;
    fs_set_spread_book(ob);
    set_rom2fix_leg_book(ob);
    ob->rom2fix[7] = rom_to_globex_price;
    ob->rom2fix[19] = rom_to_globex_price;
    ob->rom2fix[55] = globex_sec_def_trans;
    ob->rom2fix[5] = globex_symbol_hack;
    ob->rom2fix[66] = globex_cmplx_type;
    ob->rom2fix[17] = set_globex_exTag;
    ob->rom2fix[50] = set_transacttime;
    ob->rom2fix[12] = rom_to_globex_account;
    ob->rom2fix[24] = rom_to_globex_sender_sub;
    ob->rom2fix[40] = rom_to_globex_smpid;
    ob->rom2fix[9] = rom_to_globex_tif;
    ob->rom2fix[21] =rom_to_globex_program;
    ob->fix2rom[7] = globex_to_rom_price;
    ob->fix2rom[14] = def_fixrom_status_xlate;
    ob->fix2rom[44] = globex_to_rom_price;
    ob->fix2rom[15] = def_handle_ordids;
    ob->fix2rom[66] = globex_to_rom_spread;
    ob->fix2rom[5] = globex_to_rom_symbol;
    module_name_len = ex->mod_name_len;
    module_name = calloc(module_name_len + 1, 1);
    memcpy(module_name, ex->mod_name, module_name_len);;
    start_dart_mail_service(ex->mail_server, ex->mail_server_len,
                            ex->biz_group, ex->b_len, ex->tech_group,
                            ex->t_len, ex->end_hour, ex->end_min, 7);
    firm_id =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "Firm ID ", 7,
                        &fid_len);
    sender_location_id =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10,
                        "SenderLocationID ", 16, &sen_loc_len);
    session_id =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "Session ID ", 10,
                        &sess_id_len);
    customer_type_code =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10,
                        "Customer Type Code", 18, &ctc_len);
    customer_or_firm =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "Customer or Firm",
                        16, &cof_len);
    fti =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "FTI", 3,
                        &fti_len);
    password =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "password", 8,
                        &pass_len);
    app_name =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "Application Name",
                        16, &app_name_len);
    app_version =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10,
                        "Application Version", 19, &app_version_len);
    app_vender =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10,
                        "Application Vender", 18, &app_vender_len);
    fee_billing =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "Fee Billing", 11,
                        &fee_bill_len);
    end_day =
        get_val_for_tag(fix_args->dc, "TIMES", 5, "End Day", 7,
                        &end_day_len);
    int i_end_day = 7;
    if (end_day) {
        i_end_day = atoi(end_day);
    }


    ret =
        start_dart_client_services(&globex_from_rom_func, ex->mod_name,
                                   ex->mod_name_len, ex->rom_port, rom_q,
                                   ex->rom_ip, ex->rom_ip_len,
                                   ex->end_hour, ex->end_min, i_end_day,
                                   ex->heart_beat_int, ob, con_callBack,
                                   parse_rom_message, build_offsets,
                                   test_lac, create_rom_resend_pad, 0);
    ret =
        dart_connect(&from_fix_func, ex->mod_name, ex->mod_name_len,
                     ex->fix_port, ex->fix_ip, ex->fix_ip_len,
                     ex->end_hour, ex->end_min, i_end_day,
                     ex->heart_beat_int, ob,
                     fix_server_connection_callBack, parse_fix_message,
                     fix_args, send_globex_logon, build_fix_offsets,
                     low_activity_sig, 1);

    while (keep_running) {
        time(&long_time);
        struct tm lm;
        newtime = localtime_r(&long_time, &lm);
        newtime = &lm;
        if (newtime->tm_hour == ex->end_hour
            && newtime->tm_min >= ex->end_min
            && newtime->tm_min < (ex->end_min + 5)
            && ((newtime->tm_wday == ex->end_day)
                || (ex->end_day == 7))) {
            keep_running = 0;
            printf("Done with our test!!!!end_hour %d end_min %d now %d\n",
                   ex->end_hour, ex->end_min, newtime->tm_hour);
        } else {
            sleep(25);

            if (!get_is_globex_connected()) {
                set_is_globex_connected(1);
                ret =
                    dart_connect(&from_fix_func, ex->mod_name,
                                 ex->mod_name_len, ex->fix_port,
                                 ex->fix_ip, ex->fix_ip_len, ex->end_hour,
                                 ex->end_min, i_end_day,
                                 ex->heart_beat_int, ob,
                                 fix_server_connection_callBack,
                                 parse_fix_message, fix_args,
                                 send_globex_logon, build_fix_offsets,
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

int send_globex_logon(struct parse_args *pa)
{
    char *mblk;
    mblk = calloc(1000, 1);
    if (mblk != NULL) {
        size_t len = sprintf(mblk,
                             "1603=%s%c1604=%s%c1605=%s%c108=30%c95=%d%c96=%s%c98=0%c",
                             app_name, FIX_FIELD_DELIM, app_version,
                             FIX_FIELD_DELIM, app_vender, FIX_FIELD_DELIM,
                             FIX_FIELD_DELIM,
                             pass_len, FIX_FIELD_DELIM, password,
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
    fix_connection_callback(co, is_connected);
    set_can_send_orders(is_connected);
    set_is_globex_connected(is_connected);
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
}

static int globex_create_send_comp_id(char *rommsg, long rom_len,
                                      char *fix_type, int *ft_len,
                                      struct dart_fix_pos
                                      positions[ROM_LEN],
                                      orderbook * book, char ord_type,
                                      dart_ex_order_obj * obj)
{
    char *d_off = 0;
    int t_len = 0;
    int ret_val = 1;
    d_off = fix_type;
    *d_off++ = ord_type;
    *d_off++ = '\x01';
    t_len += 2;
    memcpy(d_off, "49=", 3);
    d_off += 3;
    t_len += 3;
    memcpy(d_off, sender_comp_id, sci_len);
    d_off += sci_len;
    t_len += sci_len;
    if (ord_type == 'D' && (positions[11].end - positions[11].beg > 0)) {
        char *beg_off = rommsg;
        long len = positions[11].end - positions[11].beg;
        beg_off += positions[11].beg;

        if (contains_key(clearing_ids, beg_off, len)) {
            memcpy(d_off, beg_off, len);
            d_off += len;
            t_len += len;
            doa_set_clr_id(obj, beg_off, len);
        } else {
            ret_val = 0;
        }
    } else if (ord_type != 'D') {
        size_t len = 0;
        char *cl_id = doa_get_clr_id(obj, &len);
        if (len > 0) {
            memcpy(d_off, cl_id, len);
            d_off += len;
            t_len += len;
            free(cl_id);
        } else {
            memcpy(d_off, firm_id, fid_len);
            d_off += fid_len;
            t_len += fid_len;
        }
    } else {
        memcpy(d_off, firm_id, fid_len);
        d_off += fid_len;
        t_len += fid_len;
    }
    memcpy(d_off, fti, fti_len);
    t_len += fti_len;
    *ft_len = t_len;
    return ret_val;
}

int globex_cmplx_type(char *rommsg, dart_fix_pos * map,
                      char *mtype, databuf_t * fix,
                      size_t field, int *rejected,
                      dart_ex_order_obj * order)
{
    int result = 0;
    size_t len = map[field].end - map[field].beg;
    if (len > 0) {
        doa_set_is_spread(order, 5);
    } else if (mtype[0] == 'D') {
        doa_set_is_spread(order, 0);
    }
    return result;
}

int globex_symbol_hack(char *rommsg, dart_fix_pos * map,
                       char *type, databuf_t * fix,
                       size_t field, int *rejected,
                       dart_ex_order_obj * order)
{
    int result = 0;
    size_t len = map[field].end - map[field].beg;
    if (len > 0 && map[field].len > 0) {
        char *sym = rommsg + map[field].beg;
        size_t l_len = len > 5 ? 5 : len;
        set_val_in_order_struct(sym, len, 5, order, type);
        result =
            fixmsg_addstr2(fix, map[field].name, map[field].len, sym,
                           l_len);
    }
    return result;
}

int globex_sec_def_trans(char *rommsg, dart_fix_pos * map,
                         char *type, databuf_t * fix,
                         size_t field, int *rejected,
                         dart_ex_order_obj * order)
{
    int result = 0;
    size_t len = map[field].end - map[field].beg;
    if (len > 0 && map[field].len > 0) {
        char *sym = rommsg + map[field].beg;
        set_val_in_order_struct(sym, len, 55, order, type);
        result =
            fixmsg_addstr2(fix, map[field].name, map[field].len, sym, len);
    } else if (type[0] == 'D') {
        size_t sym_len = map[5].end - map[5].beg;
        char *sym = rommsg + map[5].beg;
        if (sym_len > 0) {
            set_val_in_order_struct(sym, sym_len, 55, order, type);
            result = fixmsg_addstr2(fix, "107", 3, sym, sym_len);
        } else {
            *rejected = 1;
            result = -1;
        }
    } else {
        size_t sym_len = 0;
        char *sym = doa_get_sec_def(order, &sym_len);
        if (sym) {
            result = fixmsg_addstr2(fix, "107", 3, sym, sym_len);
            free(sym);
        } else {
            size_t sym_len = map[5].end - map[5].beg;
            char *sym = rommsg + map[5].beg;
            if (sym_len > 0) {
                set_val_in_order_struct(sym, sym_len, 55, order, type);
                result = fixmsg_addstr2(fix, "107", 3, sym, sym_len);
            } else {
                *rejected = 1;
                result = -1;
            }
        }
    }
    return result;
}



int rom_to_globex_price(char *rommsg, dart_fix_pos * map,
                        char *type, databuf_t * fix,
                        size_t field, int *rejected,
                        dart_ex_order_obj * order)
{
    int result = 0;
    size_t len = map[field].end - map[field].beg;
    if (len > 0 && map[field].len > 0) {
        int found = 0;
        int rVal = 0;
        int *rom_val = 0;
        char *p = rommsg + map[field].beg;
        size_t sym_len = map[5].end - map[5].beg;
        char *sym = 0;
        int delete_sym = 0;
        int del_spread_sym = 0;
        char *spread_sym = 0;
        if (sym_len > 0) {
            sym = rommsg + map[5].beg;
        } else if (type[0] == 'D') {
            sym_len = map[55].end - map[55].beg;
            sym = rommsg + map[55].beg;
        } else {
            sym = doa_get_symbol(order, &sym_len);
            delete_sym = 1;
        }
        if(sym && sym_len > 0) {
            spread_sym = memchr(sym, '-', sym_len);
            sym_len = get_symbol_len(sym, sym_len);
            if(spread_sym) { 
               spread_sym = calloc(10, 1);
               memcpy(spread_sym, sym, sym_len);
               memcpy(spread_sym + sym_len, "-", 1);
               del_spread_sym = 1;
            } else if(sym_len > 6 && strncmp("UD:", sym, 3) ==0) {
                sym_len = 6;
            }
        }
        set_val_in_order_struct(p, len, field, order, type);
        pthread_mutex_lock(&config_lock);
        if(!spread_sym) {
        found =
            find_n_fill(display_factors, sym, sym_len, (void *) &rom_val);
        } else {
        found =
            find_n_fill(display_factors, spread_sym, sym_len + 1, (void *) &rom_val);
        if(found <= 0) {
        found =
            find_n_fill(display_factors, sym, sym_len, (void *) &rom_val);
        }
        }
        pthread_mutex_unlock(&config_lock);
        if (found > 0) {
            rVal = *rom_val;
            if ((rVal) == 1) {
                result =
                    fixmsg_addstr2(fix, map[field].name, map[field].len, p,
                                   len);
                if (delete_sym) {
                    free(sym);
                }
                if(del_spread_sym) {
                    free(spread_sym);
                }
                return result;
            }
        } else {
            rVal = 100;
        }
        databuf_write(fix, "%c%s=", FIX_FIELD_DELIM, map[field].name);
        char *dec = memchr(p, '.', len);
        int len_left = 0;
        if (dec) {
            databuf_memcpy(fix, p, dec - p);
            dec += 1;
            len_left = len - ((dec - p));
        } else {
            databuf_memcpy(fix, p, len);
        }
        while (rVal > 1) {
            if (len_left > 0) {
                databuf_memcpy(fix, dec, 1);
            } else {
                databuf_memcpy(fix, "0", 1);
            }
            rVal = rVal / 10;
            if (dec) {
                dec += 1;
                --len_left;
            }
        }
        if (len_left > 0) {
            databuf_memcpy(fix, ".", 1);
            while (len_left > 0) {
                databuf_memcpy(fix, dec, 1);
                dec += 1;
                --len_left;
            }
        }
        if (delete_sym) {
            free(sym);
        }
        if(del_spread_sym) {
            free(spread_sym);
        }
    }
    return result;
}

static void check_tas_order(char *rom, romindex_t * map, char *type,
                      databuf_t * fix, size_t field, int *rejected,
                      dart_ex_order_obj * order)
{
    char *p = rom + map[field].beg;
    size_t len = map[field].end - map[field].beg;
    int sec_def_len = map[55].end - map[55].beg;
    char* sec_def = rom + map[field].beg;
    if(sec_def_len >= 3 && strncmp("CLT", sec_def, 3) == 0) {
        set_val_in_order_struct("0", len, field, order, type);
        fixmsg_addstr2(fix, map[field].name, map[field].len, "0", 1);
    } else {
        set_val_in_order_struct(p, len, field, order, type);
        fixmsg_addstr2(fix, map[field].name, map[field].len, p, len);
    }
}

int rom_to_globex_tif(char *rom, romindex_t * map, char *type,
                      databuf_t * fix, size_t field, int *rejected,
                      dart_ex_order_obj * order)
{
    int result = 0;
    size_t len = map[field].end - map[field].beg;
    if (len > 0 && map[field].len > 0) {
        char *p = rom + map[field].beg;
        if(p[0] == '1' && len == 1) {
            check_tas_order(rom, map, type, fix, field, rejected, order);
        } else {
            set_val_in_order_struct(p, len, field, order, type);
            result =
                fixmsg_addstr2(fix, map[field].name, map[field].len, p, len);
        }
        if (p[0] == '3') {
            size_t m_len = map[18].end - map[18].beg;
            if (m_len <= 0) {
                fixmsg_addstr2(fix, "110", 3, "0", 1);
            } else {
                char *mq = rom + map[18].beg;
                fixmsg_addstr2(fix, "110", 3, mq, m_len);
            }
        }
    }
    return result;
}

int rom_to_globex_program(char *rom, dart_fix_pos * map,
                          char *type, databuf_t * fix,
                          size_t field, int *rejected,
                          dart_ex_order_obj * order)
{
    size_t len = map[field].end - map[field].beg;
    if (len > 0 && map[field].len > 0) {
        char *p = rom + map[field].beg;
        if(p[0] == 'Y') {
            doa_set_is_manual(order, 0);
            databuf_write(fix, "%c1028=N", FIX_FIELD_DELIM);
        } else {
            doa_set_is_manual(order, 1);
            databuf_write(fix, "%c1028=Y", FIX_FIELD_DELIM);
        }
    } else if (rom[0] != 'E') {
        int man = doa_get_is_manual(order);
        if(man) {
            databuf_write(fix, "%c1028=Y", FIX_FIELD_DELIM);
        } else {
            databuf_write(fix, "%c1028=N", FIX_FIELD_DELIM);
        }
    } else {
        doa_set_is_manual(order, 0);
        databuf_write(fix, "%c1028=N", FIX_FIELD_DELIM);
    }
    return 1;
}
int rom_to_globex_account(char *rommsg, dart_fix_pos * map,
                          char *mtype, databuf_t * fix,
                          size_t field, int *rejected,
                          dart_ex_order_obj * order)
{
    int result = 0;
    size_t len = map[field].end - map[field].beg;
    if (len > 0 && map[field].len > 0) {
        char *p = rommsg + map[field].beg;
        set_val_in_order_struct(p, len, field, order, mtype);
        result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
    } else {
        char *p = doa_get_account(order, &len);
        if (len > 0 && p != NULL) {
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
            free(p);
        }
    }

    return result;

}

int rom_to_globex_smpid(char *rommsg, dart_fix_pos * map,
                             char *mtype, databuf_t * fix,
                             size_t field, int *rejected,
                             dart_ex_order_obj * order)
{
    int result = 0;
    size_t len = map[field].end - map[field].beg;
    if (len > 0) {
        char *p = rommsg + map[field].beg;
        doa_set_smpid(order, p, len);
        result = fixmsg_addstr_bylen(fix, "7928", p, len);
    } else {
        char *p = doa_get_smpid(order, &len);
        if (len > 0 && p != NULL) {
            result = fixmsg_addstr_bylen(fix, "7928", p, len);
            free(p);
        }
    }

    return result;
}


int rom_to_globex_sender_sub(char *rommsg, dart_fix_pos * map,
                             char *mtype, databuf_t * fix,
                             size_t field, int *rejected,
                             dart_ex_order_obj * order)
{
    int result = 0;
    int found = 0;
    int *rom_val = 0;
    size_t len = 0;
    size_t dest_len = map[13].end - map[13].beg;
    if(dest_len > 0 && mtype[0] == 'D') {
        char* dest = rommsg + map[13].beg;
        doa_set_destination(order, dest, dest_len);
        found =
            find_n_fill(sender_sub_ids, dest, dest_len, (void *) &rom_val);
        field = found == 0 ? 2 : *rom_val;
        len = map[field].end - map[field].beg;
        if (len > 0) {
            char *p = rommsg + map[field].beg;
            if(field == 2) {
                doa_set_trader(order, p, len);
            } else {
                doa_set_trade_for(order, p, len);
            }
            result = fixmsg_addstr_bylen(fix, "50", p, len);
            return result;
        } 
    } else {
        char* p = 0;
        char* dest = doa_get_destination(order, &dest_len);
        if(dest_len > 0) {
            found =
                find_n_fill(sender_sub_ids, dest, dest_len, (void *) &rom_val);
            if(found > 0 && *rom_val == 2) {
                p = doa_get_trader(order, &len);
            } else { 
                p = doa_get_trade_for(order, &len);
            }
        } else {
            p = doa_get_trader(order, &len);
        }
        if (len > 0 && p != NULL) {
            result = fixmsg_addstr_bylen(fix, "50", p, len);
            free(p);
        }
    }
    return result;
}

int globex_to_rom_price(char *fixmsg, dart_rom_pos * map,
                        char mtype, char *status,
                        databuf_t * rom, size_t field,
                        int *rejected,
                        dart_ex_order_obj * order,
                        pthread_mutex_t * mutex, hashmap_t hmap)
{
    int result = 0;
    size_t len = map[field].end - map[field].beg;
    if (len > 0) {
        int rVal = 0;
        int found = 0;
        int *rom_val = 0;
        char *p = fixmsg + map[field].beg;
        size_t sym_len = 0;//map[55].end - map[55].beg;
        char *sym = 0;
        int del_spread_sym = 0;
        char *spread_sym = 0;
        int delete_sym = 0;
        sym = doa_get_symbol(order, &sym_len);
        delete_sym = 1;
        set_val_in_order_struct(p, len, field, order, &mtype);
        if(sym && sym_len > 0) {
            spread_sym = memchr(sym, '-', sym_len);
            sym_len = get_symbol_len(sym, sym_len);
           if(spread_sym) { 
               spread_sym = calloc(10, 1);
               memcpy(spread_sym, sym, sym_len);
               memcpy(spread_sym + sym_len, "-", 1);
               del_spread_sym = 1;
           }
        }
        pthread_mutex_lock(&config_lock);
        if(!spread_sym) {
        found =
            find_n_fill(display_factors, sym, sym_len, (void *) &rom_val);
        } else {
        found =
            find_n_fill(display_factors, spread_sym, sym_len + 1, (void *) &rom_val);
        if(found <= 0) {
        found =
            find_n_fill(display_factors, sym, sym_len, (void *) &rom_val);
        }
        }
        pthread_mutex_unlock(&config_lock);
        if (found > 0) {
            rVal = *rom_val;
            if ((rVal) == 1) {
                databuf_memcpy(rom, p, len);
                if (delete_sym) {
                    free(sym);
                }
                if(del_spread_sym) {
                    free(spread_sym);
                }
                return result;
            }
        } else {
            rVal = 100;
        }
        char dstr[50];
        memset(dstr, '\0', 50);
        double rp = 0;
        double cmep = 0;
        memcpy(dstr, p, len);
        rp = atof(dstr);
        cmep = rp / ((double) (rVal));
        cmep += 1e-10;
        databuf_write(rom, "%.7f", cmep);
        if (delete_sym) {
            free(sym);
        }
        if(del_spread_sym) {
            free(spread_sym);
        }
    }
    return result;
}

int globex_to_rom_spread(char *fixmsg, dart_rom_pos * xlateinfo,
                         char mtype, char *status,
                         databuf_t * rom, size_t field,
                         int *rejtected,
                         dart_ex_order_obj * order,
                         pthread_mutex_t * mutex, hashmap_t map)
{
    int result = 0;
    if (doa_get_is_spread(order)) {
        size_t len = xlateinfo[field].end - xlateinfo[field].beg;
        char *p = fixmsg + xlateinfo[field].beg;
        if (len > 0 && strncmp("2", p, 1) == 0) {
            databuf_memcpy(rom, "6", 1);
        } else if (len > 0 && strncmp("3", p, 1) == 0) {
            databuf_memcpy(rom, "5", 1);
        }
    }
    return result;
}

int globex_to_rom_symbol(char *fixmsg, dart_rom_pos * xlateinfo,
                         char mtype, char *status,
                         databuf_t * rom, size_t field,
                         int *rejtected,
                         dart_ex_order_obj * order,
                         pthread_mutex_t * mutex, hashmap_t map)
{
    size_t sym_len = xlateinfo[55].end - xlateinfo[55].beg;
    if (sym_len > 0) {
        char *sym = fixmsg;
        sym += xlateinfo[55].beg;
        databuf_memcpy(rom, sym, sym_len);
    } else {
        char *sym = doa_get_symbol(order, &sym_len);
        if (sym_len > 0) {
            databuf_memcpy(rom, sym, sym_len);
        }
        free(sym);
    }
    return 1;
}

static void def_globex_send_comp_id(char *ret_str, int *len)
{
    char *d_off = ret_str;
    int t_len = 0;
    memcpy(d_off, sender_comp_id, sci_len);
    d_off += sci_len;
    t_len += sci_len;
    memcpy(d_off, firm_id, fid_len);
    d_off += fid_len;
    t_len += fid_len;
    memcpy(d_off, fti, fti_len);
    t_len += fti_len;
    *len = t_len;
}

static long globex_rom2fix_translate_fields(struct message *mess,
                                            char *rommsg,
                                            databuf_t * output,
                                            long fix_len, char ord_type,
                                            char *fix_type, int *ft_len,
                                            struct dart_fix_pos
                                            positions[ROM_LEN],
                                            int *rejected,
                                            orderbook * book)
{
    int i = 1;
    /* Verify type is exactly one character */
    dart_ex_order_obj *obj = NULL;
    int ret;
    char *beg_off;
    long len;
    book->add_header_defs(rommsg, positions, &ord_type, output, 0,
                          rejected, obj);
    if (*rejected != 0) {
        return databuf_unread(output);
    }
    if (ord_type == 'D') {
        size_t clordid_len = 0;
        int good_clear = 1;
        char *cl_ord_id = book->generate_id(&clordid_len);
        obj =
            establish_order(rommsg, &book->mutex, book->cl_ord_id_map,
                            book->rom_ord_id_map, positions, mess,
                            cl_ord_id, clordid_len, book);
        fixmsg_addstr(output, "11", cl_ord_id);
        free(cl_ord_id);
        good_clear = globex_create_send_comp_id(rommsg, fix_len, fix_type,
                                                ft_len, positions, book,
                                                ord_type, obj);
        if (good_clear == 0) {
            databuf_reset(output);
            *rejected = 1;
            char reason[50];
            memset(reason, '\0', 50);
            sprintf(reason, "Invalid clearing id");
            return create_rom_reject(rommsg, output, reason);
        }

    } else {
        beg_off = rommsg;
        beg_off += positions[17].beg;
        len = (positions[17].end - positions[17].beg);
        pthread_mutex_lock(&book->mutex);
        ret =
            get_map_entry(book->rom_ord_id_map, beg_off, len,
                          (void *) &obj);
        pthread_mutex_unlock(&book->mutex);
        if (ret > 0 && (ord_type == 'F' || !doa_get_pending_replace(obj))) {
            size_t t_len = 0;
            char *l_type = doa_get_type(obj, &t_len);
            size_t clordid_len = 0;
            size_t rep_ord_len = 0;
            char *rep_ord_id = book->generate_id(&rep_ord_len);
            char *cl_ord_id = doa_get_cl_ord_id(obj, &clordid_len);
            fixmsg_addstr(output, "11", rep_ord_id);
            doa_set_rep_ord_id(obj, rep_ord_id, rep_ord_len);
            pthread_mutex_lock(&book->mutex);
            no_copy_insert(book->cl_ord_id_map, rep_ord_id, rep_ord_len,
                           obj);
            pthread_mutex_unlock(&book->mutex);
            fixmsg_addstr(output, "41", cl_ord_id);

            globex_create_send_comp_id(rommsg, fix_len, fix_type,
                                       ft_len, positions,
                                       book, ord_type, obj);
            free(l_type);
            free(rep_ord_id);
            free(cl_ord_id);
        } else if (ret > 0 && doa_get_pending_replace(obj)) {
            int rej_len = 0;
            char *l_message = "Order is already pending Replace.";
            databuf_reset(output);
            rej_len =
                build_cnl_rpl_rej(beg_off, len, fix_type, output,
                                  l_message);
            *rejected = 1;
            return rej_len;
        } else {
            int rej_len = 0;
            char *l_message =
                ord_type ==
                'F' ? "Unable to find rom tag too late to cancel?" :
                "Unable to find rom tag too late to replace?";
            databuf_reset(output);
            rej_len =
                build_cnl_rpl_rej(beg_off, len, fix_type, output,
                                  l_message);
            *rejected = 1;
            return rej_len;
        }
    }

    for (i = 1; i < ROM_LEN; ++i) {
        if (book->rom2fix[i] != 0) {
            if (book->rom2fix[i]
                (rommsg, positions, &ord_type, output, i, rejected,
                 obj) == -1 || *rejected != 0) {
                fprintf(stderr, "%s: Processing %d failed\n", __FUNCTION__,
                        i);
                databuf_reset(output);
                *rejected = 1;
                char reason[50];
                memset(reason, '\0', 50);
                sprintf(reason, "Failed to parse ROM field %d", i);
                return create_rom_reject(rommsg, output, reason);
            }
        }
    }

    if (!*rejected) {
        databuf_memcpy(output, "\x01", 1);
    }

    return databuf_unread(output);
}

static void reload_price_config()
{
    pthread_mutex_lock(&config_lock);
    delete_map(display_factors);
    display_factors = 0;
    display_factors = load_display_factors("GlobexPriceMult.map");
    pthread_mutex_unlock(&config_lock);
}

static long globex_trans_rom_message(struct message *mess,
                                     char *rom_message, long rom_len,
                                     databuf_t * output,
                                     char *ret_fix_type, int *ft_len,
                                     int *rejected, orderbook * ob)
{

    long ret_val;
    struct dart_fix_pos l_positions[ROM_LEN];
    int i = 0;
    char rom_type;
    ret_val = 0;

    for (; i < ROM_LEN; ++i) {
        l_positions[i].beg = 0;
        l_positions[i].end = 0;
        l_positions[i].name = NULL;
        l_positions[i].len = 0;
    }
    rom_type = rom_message[0];
    if (rom_type == 'S') {
        build_rom_pos2(rom_message, rom_len, ob->rom_to_fix_status,
                       l_positions, 0);
        ret_val =
            globex_rom2fix_translate_fields(mess, rom_message, output,
                                            rom_len, '8', ret_fix_type,
                                            ft_len, l_positions, rejected,
                                            ob);
    } else {
        switch (rom_type) {
        case 'E':
            build_rom_pos2(rom_message, rom_len, ob->rom_to_fix_out,
                           l_positions, 0);
            ret_val = globex_rom2fix_translate_fields(mess, rom_message,
                                                      output, rom_len,
                                                      'D', ret_fix_type,
                                                      ft_len, l_positions,
                                                      rejected, ob);
            break;
        case 'R':
            build_rom_pos2(rom_message, rom_len, ob->rom_to_fix_rpl,
                           l_positions, 0);
            ret_val = globex_rom2fix_translate_fields(mess, rom_message,
                                                      output, rom_len,
                                                      'G', ret_fix_type,
                                                      ft_len, l_positions,
                                                      rejected, ob);
            break;
        case 'C':
            build_rom_pos2(rom_message, rom_len, ob->rom_to_fix_cnl,
                           l_positions, 0);
            ret_val = globex_rom2fix_translate_fields(mess, rom_message,
                                                      output, rom_len,
                                                      'F', ret_fix_type,
                                                      ft_len, l_positions,
                                                      rejected, ob);
            break;
        case 'P':
            ret_val = 0;
            reload_price_config();
        default:
            ret_val = 0;
            break;
        }
    }
    return ret_val;
}

static void globex_from_rom_func(struct message *mess, void *b)
{
    databuf_t *output = databuf_alloc(mess->len * 8);
    orderbook *ob = (orderbook *) b;
    long total_len;
    struct message m1;
    struct message log;
    char *fix_type = calloc(20, 1);
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
        total_len =
            globex_trans_rom_message(&m1, mess->data, mess->len, output,
                                     fix_type, &ft_len, &rejected,
                                     (orderbook *) ob);
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
        char *l_message = "Not connected to Globex.";
        rejected = 1;
        m1.name = mess->name;
        m1.name_len = mess->name_len;
        total_len = create_rom_reject(mess->data, output, l_message);
        if (total_len > 0) {
            m1.data = output->buffer;
            m1.len = databuf_unread(output);
            send_message_to_roms(&m1, ob);
        } else {
            databuf_destroy(output);
            return;
        }
    }
    databuf_destroy(output);
}

size_t create_globex_fix_header(databuf_t * fixmsg,
                                const char *sendercompid,
                                size_t sendercomplen,
                                const char *targetcompid,
                                size_t targetcomplen,
                                const char *sendersubid,
                                size_t sendersublen,
                                const char *targetsubid,
                                size_t targetsublen,
                                unsigned long seq, time_t * now)
{
    size_t retval = 0;
    struct tm tmval;
    struct tm *t = &tmval;
    time(now);
    if ((t = gmtime_r(now, &tmval)) == 0) {
        perror("Calling gmtime_r");
    }
    if (databuf_unread(fixmsg) > 10) {
        //do nothing
    } else {
        char ret_str[40];
        int len = 0;
        def_globex_send_comp_id(ret_str, &len);
        fixmsg_addstr2(fixmsg, "49", 2, ret_str, len);
        fixmsg_addstr2(fixmsg, "50", 2, sendersubid, sendersublen);
    }
    if (fixmsg_addstr2(fixmsg, "56", 2, targetcompid, targetcomplen) > -1
        && fixmsg_addstr2(fixmsg, "57", 2, targetsubid, targetsublen) > -1
        && fixmsg_adduint(fixmsg, "34", 2, seq) > -1
        && fixmsg_addstr2(fixmsg, "142", 3, sender_location_id,
                          sen_loc_len) > -1
        && fixmsg_addtime(fixmsg, "52", t) > -1) {
        if (databuf_unread(fixmsg) > 0 || databuf_realloc(fixmsg, 1) == 0) {
            *fixmsg->wr_ptr++ = '\x01';
            retval = databuf_unread(fixmsg);
        }
    }
    return retval;
}

int set_globex_exTag(char *mess, struct dart_fix_pos *positions,
                     char *mtype, databuf_t * output, size_t i,
                     int *rejected, dart_ex_order_obj * ob)
{
    if (mtype[0] == 'D') {
    } else {
        size_t cl_ord_id_len = 0;
        char *cl_ord_id = doa_get_exchange_tag(ob, &cl_ord_id_len);
        databuf_write(output, "%c37=%s", FIX_FIELD_DELIM, cl_ord_id);
        free(cl_ord_id);
    }
    return 1;
}
