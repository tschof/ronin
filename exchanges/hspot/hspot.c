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
#include "order_token_creator.h"
#include "complex_parser.h"
#include "options_common.h"
#include "fixmsg.h"
#include "hashmap.h"
#include "hspot.h"
static pthread_mutex_t connection_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t config_lock = PTHREAD_MUTEX_INITIALIZER;

static token_struct_t *idgenerator = 0;
static char datestr[9] = { '\0' };

static char *fix_client_name;
static int fix_client_len;

static char *module_name = NULL;        // = databuf_alloc(100);
static int module_name_len = 0;
static  orderbook *ob = NULL; 
static int is_hspt_connected = 1;
static char *sender_comp_id = 0;
static int sci_len = 0;
static char *firm_id = 0;
static int fid_len = 0;
static char *customer_or_firm = 0;
static int cof_len = 0;
static char *user = 0;
static int user_len = 0;
static char *password = 0;
static int pass_len = 0;
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

static char *hspt_generate_id(size_t * nlen)
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

int get_is_hspt_connected()
{
    int ret = 0;
    pthread_mutex_lock(&connection_lock);
    ret = is_hspt_connected;
    pthread_mutex_unlock(&connection_lock);
    return ret;
}
int get_can_send_orders()
{
    int ret = 0;
    pthread_mutex_lock(&connection_lock);
    ret = ob->can_send_orders;
    pthread_mutex_unlock(&connection_lock);
    return ret;
}

void set_is_hspt_connected(int is)
{
    pthread_mutex_lock(&connection_lock);
    is_hspt_connected = is;
    pthread_mutex_unlock(&connection_lock);
}

int set_hspt_defaults(char *rommsg, struct dart_fix_pos *map,
                        char *mtype, databuf_t * output,
                        size_t romfield, int *rejected,
                        dart_ex_order_obj * order)
{
    int result = 0;
    int field = 12;
    size_t len = map[field].end - map[field].beg;
    if (len > 0) {
        char *p = rommsg + map[field].beg;
        result = fixmsg_addstr_bylen(output, "116", p, len);
    } 
    fixmsg_addstr_bylen(output, "21", "1", 1);
    return result;
}

int hspt_fixrom_status_xlate(char *fixmsg, dart_rom_pos * positions,
                            char mtype, char *status,
                            databuf_t * rom, size_t romfield,
                            int *rejtected, dart_ex_order_obj * order,
                            pthread_mutex_t * mutex, hashmap_t ord_map)
{
    char *beg_off = fixmsg;
    size_t len = (positions[romfield].end - positions[romfield].beg);
    beg_off += positions[romfield].beg;
    if (romfield == 14 && mtype != 'U' && beg_off[0] != 'F') {
        len = determine_trans_type(romfield, beg_off, rom, len);
        status = fixmsg;
        status += positions[romfield].beg;
    } else if (romfield == 14 && mtype == 'U') {
        len = set_status_for_cnrej(fixmsg, positions, status, rom);
    } else if (beg_off[0] == 'F') {
        len = positions[61].end - positions[61].beg;        
        if(len > 0) {
            status = fixmsg;
            status += positions[61].beg;
            databuf_memcpy(rom, status, len);
        }
    }
    return 1;
}



long hspt_send_alert_for_message(char *fix_message,
                            long fix_len,
                            struct dart_rom_pos positions[ROM_LEN])
{
    int len = 0;
    long ret_len = 0;
    //char debugger[3000];
    //int dlen = 0;
    databuf_t *subject = databuf_alloc(256);
    databuf_t *msgbody = databuf_alloc(256);

    databuf_write(subject, "Alert Deal Broken from: %s", fix_client_name);

    len = positions[15].end - positions[15].beg;
    databuf_write(msgbody, "Message Values:\nClOrdID = %.*s\n", len,
                  fix_message + positions[15].beg);
    if ((len = positions[60].end - positions[60].beg) > 0)
        databuf_write(msgbody, "OrigOrdID = %.*s\n", len,
                      fix_message + positions[60].beg);
    if ((len = positions[7].end - positions[7].beg) > 0)
        databuf_write(msgbody, "Price = %.*s\n", len,
                      fix_message + positions[7].beg);
    if ((len = positions[17].end - positions[17].beg) > 0)
        databuf_write(msgbody, "Deal ID = %.*s\n", len,
                      fix_message + positions[17].beg);
    if ((len = positions[42].end - positions[42].beg) > 0)
        databuf_write(msgbody, "Last Shares = %.*s\n", len,
                      fix_message + positions[42].beg);
    if ((len = positions[6].end - positions[6].beg) > 0)
        databuf_write(msgbody, "Shares = %.*s\n", len,
                      fix_message + positions[6].beg);
    databuf_write(msgbody, "Raw Message:\n%.*s\n", (int) fix_len,
                  fix_message);

    ret_len = msgbody->wr_ptr - msgbody->rd_ptr;
    //jdlen = ret_len >= 3000 ? 3000 : ret_len;
    //memcpy(debugger, msgbody->rd_ptr,dlen);
    //debug_rolling_log(debugger, dlen);
    send_tech_message(msgbody->rd_ptr, ret_len, subject->rd_ptr,
                      subject->wr_ptr - subject->rd_ptr);

    databuf_destroy(subject);
    databuf_destroy(msgbody);

    return ret_len;
}

int rom_to_hspt_account(char *rommsg, dart_fix_pos * xlateinfo,
                          char *mtype, databuf_t * fix,
                          size_t romfield, int *rejected,
                          dart_ex_order_obj * order);

int rom_to_hspt_side(char *rom, romindex_t * map, char *type,
                      databuf_t * fix, size_t field, int *rejected,
                      dart_ex_order_obj * order);
int rom_to_hspt_type(char *rom, romindex_t * map, char *type,
                      databuf_t * fix, size_t field, int *rejected,
                      dart_ex_order_obj * order);
void send_cancel_all();
void con_callBack(con_obj * co, int is_connected, char *log, int len);
void fix_server_connection_callBack(con_obj * co, int is_connected,
                                    char *log, int len);
void test_lac(con_obj * co);
void destroy_rom_queue(queue_t q);
int send_hspt_logon(struct parse_args *pa);
int set_hspt_exTag(char *mess, struct dart_fix_pos *positions,
                     char *mtype, databuf_t * output, size_t i,
                     int *rejected, dart_ex_order_obj * ob);

size_t create_hspt_fix_header(databuf_t * fixmsg,
                                const char *sendercompid,
                                size_t sendercomplen,
                                const char *targetcompid,
                                size_t targetcomplen,
                                const char *sendersubid,
                                size_t sendersublen,
                                const char *targetsubid,
                                size_t targetsublen,
                                unsigned long seq, time_t * now);

int hspt_symbol_hack(char *rommsg, dart_fix_pos * map,
                       char *type, databuf_t * fix,
                       size_t field, int *rejected,
                       dart_ex_order_obj * order);
int hspt_main()
{
    int ret;
    struct init_args *fix_args;
    ex_config *ex = NULL;
    struct tm *newtime;
    int keep_running = 1;
    queue_t rom_q;
    time_t long_time;
    rom_q = create_queue();
    pthread_mutex_lock(&config_lock);
    pthread_mutex_unlock(&config_lock);
    ex = init_config("hspot.dat", 16);
    fix_args = load_fix_config("hspot.dat", 16);
    load_rom_config(ex->dc, rom_q);
    fix_client_name = fix_args->target_comp;
    fix_client_len = fix_args->tc_len;
    init_date_str();
    fix_args->create_mess_header = create_hspt_fix_header;
    sender_comp_id = fix_args->sender_comp;
    sci_len = fix_args->sc_len;
    idgenerator = init_order_token_creator();
    ob = init_order_book(hspt_generate_id, get_can_send_orders,
                         def_rom2fix, set_hspt_defaults,
                         def_xlate_fixrom, ex->order_allocation,
                         ex->mod_name, ex->mod_name_len, fix_client_name,
                         fix_client_len, NULL, 0);
    opt_database_init(ex->db_user_name, ex->db_user_name_len,
                      ex->db_password, ex->db_password_len, ex->db_server,
                      ex->db_server_len, ex->dest_id, ex->dest_id_len, ob);

    build_map_from_file("FixToRomTradeCap", ob->fix_to_rom_trade_cap );
    ob->rom2fix[37] = def_rom2fix_no_op;
    ob->fix2rom[14] = hspt_fixrom_status_xlate;
    ob->fix2rom[15] = def_handle_ordids;
    ob->fix2rom[17] = def_add_rom_tag;
    ob->rom2fix[35] = def_rom2fix_no_op;
    ob->rom2fix[5] = hspt_symbol_hack;
    ob->rom2fix[38] = def_trans_open_close;
    fs_set_spread_book(ob);
    set_rom2fix_leg_book(ob);
    ob->rom2fix[17] = set_hspt_exTag;
    ob->rom2fix[50] = set_transacttime;
    ob->rom2fix[12] = rom_to_hspt_account;
    ob->rom2fix[4] = rom_to_hspt_side;
    ob->rom2fix[8] = rom_to_hspt_type;
    ob->fix2rom[15] = def_handle_ordids;
    module_name_len = ex->mod_name_len;
    module_name = calloc(module_name_len + 1, 1);
    memcpy(module_name, ex->mod_name, module_name_len);;
    start_dart_mail_service(ex->mail_server, ex->mail_server_len,
                            ex->biz_group, ex->b_len, ex->tech_group,
                            ex->t_len, ex->end_hour, ex->end_min, 7);
    firm_id =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "Firm ID ", 7,
                        &fid_len);
    customer_or_firm =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "Customer or Firm",
                        16, &cof_len);
    user =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "Username", 8,
                        &user_len);
    password =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "password", 8,
                        &pass_len);


    ret =
        start_dart_client_services(&from_rom_func, ex->mod_name,
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
                     fix_args, send_hspt_logon, build_fix_offsets,
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

            if (!get_is_hspt_connected()) {
                set_is_hspt_connected(1);
                fix_args->reset_time->create_new = 0;
                fix_args->reset_time->new_incoming = 0;
                fix_args->reset_time->new_outgoing = 0;
                ret =
                    dart_connect(&from_fix_func, ex->mod_name,
                                 ex->mod_name_len, ex->fix_port,
                                 ex->fix_ip, ex->fix_ip_len, ex->end_hour,
                                 ex->end_min, 7, ex->heart_beat_int, ob,
                                 fix_server_connection_callBack,
                                 parse_fix_message, fix_args,
                                 send_hspt_logon, build_fix_offsets,
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

int send_hspt_logon(struct parse_args *pa)
{
    char *mblk;
    mblk = calloc(1000, 1);
    if (mblk != NULL) {
        size_t len = sprintf(mblk,
                             "553=%s%c554=%s%c108=30%c98=0%c",
                             user,
                             FIX_FIELD_DELIM,
                             password,
                             FIX_FIELD_DELIM,
                             FIX_FIELD_DELIM,
                             FIX_FIELD_DELIM
                             );
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
    set_is_hspt_connected(is_connected);
    ob->can_send_orders = is_connected;
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


int hspt_symbol_hack(char *rommsg, dart_fix_pos * map,
                       char *type, databuf_t * fix,
                       size_t field, int *rejected,
                       dart_ex_order_obj * order)
{
    int result = 0;
    size_t len = map[field].end - map[field].beg;
    if (len > 0 && map[field].len > 0) {
        char *sym = rommsg + map[field].beg;
        long slash_off = 0;
        set_val_in_order_struct(sym, len, 5, order, type);
        result =
            fixmsg_addstr2(fix, map[field].name, map[field].len, sym,
                           len);
        
        slash_off = find_offset(sym, len, "/", 1);
        if(slash_off > 0) {
            result = fixmsg_addstr2(fix, "15",2, sym, slash_off);
        }
    }
    return result;
}


int rom_to_hspt_side(char *rom, romindex_t * map, char *type,
                      databuf_t * fix, size_t field, int *rejected,
                      dart_ex_order_obj * order)
{
    int result = 0;
    size_t len = map[field].end - map[field].beg;
    if (len > 0 && map[field].len > 0) {
        char *p = rom + map[field].beg;
        set_val_in_order_struct(p, len, field, order, type);
        result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
    } else {
        char*p = doa_get_side(order, &len);
        if(len > 0 && p != NULL) {
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
            free(p);
        }
    }
    return result;
}
int rom_to_hspt_type(char *rom, romindex_t * map, char *type,
                      databuf_t * fix, size_t field, int *rejected,
                      dart_ex_order_obj * order)
{
    int result = 0;
    size_t len = map[field].end - map[field].beg;
    if (len > 0 && map[field].len > 0) {
        char *p = rom + map[field].beg;
        set_val_in_order_struct(p, len, field, order, type);
        result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
    } else {
        char*p = doa_get_type(order, &len);
        if(len > 0 && p != NULL) {
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
            free(p);
        }
    }
    return result;
}

int rom_to_hspt_account(char *rommsg, dart_fix_pos * map,
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

int set_hspt_exTag(char *mess, struct dart_fix_pos *positions,
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

size_t create_hspt_fix_header(databuf_t * fixmsg,
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
    char* type_off = fixmsg->buffer;
    time(now);
    if ((t = gmtime_r(now, &tmval)) == 0) {
        perror("Calling gmtime_r");
    }
    type_off += 4;
    if(type_off[0] != 'D' && type_off[0] != 'F' && type_off[0] != 'G') {
        fixmsg_addstr2(fixmsg, "50", 2, firm_id, fid_len);
    }
    fixmsg_addstr2(fixmsg, "49", 2, sendercompid, sendercomplen);
    if (fixmsg_addstr2(fixmsg, "56", 2, targetcompid, targetcomplen) > -1
        && fixmsg_adduint(fixmsg, "34", 2, seq) > -1
        && fixmsg_addtime(fixmsg, "52", t) > -1) {
        if (databuf_unread(fixmsg) > 0 || databuf_realloc(fixmsg, 1) == 0) {
            *fixmsg->wr_ptr++ = '\x01';
            retval = databuf_unread(fixmsg);
        }
    }
    return retval;
}

