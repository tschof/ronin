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
#include "debug_logger.h"
#include "ebs_display_fact.h"
static pthread_mutex_t connection_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t config_lock = PTHREAD_MUTEX_INITIALIZER;
static hashmap_t display_factors;

static token_struct_t *idgenerator = 0;
static char datestr[9] = { '\0' };

static char *fix_client_name;
static int fix_client_len;

static char *module_name = NULL;        // = databuf_alloc(100);
static int module_name_len = 0;
static  orderbook *ob = NULL;       // init_order_book(next_order_id, def_rom2fix,50000);
static int is_ebs_connected = 1;
static char *sender_comp_id = 0;
static int sci_len = 0;
static char *firm_id = 0;
static int fid_len = 0;
static char *customer_type_code = 0;
static int ctc_len = 0;
static char *customer_or_firm = 0;
static int cof_len = 0;
static char *user = 0;
static int user_len = 0;
static char *password = 0;
static int pass_len = 0;
static char* new_pass = 0;
static int new_pass_len = 0;
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

static char *ebs_generate_id(size_t * nlen)
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

int get_is_ebs_connected()
{
    int ret = 0;
    pthread_mutex_lock(&connection_lock);
    ret = is_ebs_connected;
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

void set_is_ebs_connected(int is)
{
    pthread_mutex_lock(&connection_lock);
    is_ebs_connected = is;
    pthread_mutex_unlock(&connection_lock);
}

int set_ebs_defaults(char *rommsg, struct dart_fix_pos *positions,
                        char *mtype, databuf_t * output,
                        size_t romfield, int *rejected,
                        dart_ex_order_obj * order)
{
    databuf_write(output, "%c461=RCSXXX", FIX_FIELD_DELIM);
    databuf_write(output, "%c63=0", FIX_FIELD_DELIM);
    return 1;
}


long ebs_send_alert_for_message(char *fix_message,
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

int rom_to_ebs_account(char *rommsg, dart_fix_pos * xlateinfo,
                          char *mtype, databuf_t * fix,
                          size_t romfield, int *rejected,
                          dart_ex_order_obj * order);
int rom_to_ebs_tif(char *rom, romindex_t * map, char *type,
                      databuf_t * fix, size_t field, int *rejected,
                      dart_ex_order_obj * order);

int rom_to_ebs_side(char *rom, romindex_t * map, char *type,
                      databuf_t * fix, size_t field, int *rejected,
                      dart_ex_order_obj * order);
int rom_to_ebs_type(char *rom, romindex_t * map, char *type,
                      databuf_t * fix, size_t field, int *rejected,
                      dart_ex_order_obj * order);
int rom_to_ebs_price(char *rommsg, dart_fix_pos * xlateinfo,
                        char *mtype, databuf_t * fix,
                        size_t romfield, int *rejected,
                        dart_ex_order_obj * order);
void send_cancel_all();
void con_callBack(con_obj * co, int is_connected, char *log, int len);
void fix_server_connection_callBack(con_obj * co, int is_connected,
                                    char *log, int len);
void test_lac(con_obj * co);
void destroy_rom_queue(queue_t q);
int send_ebs_logon(struct parse_args *pa);
int set_ebs_exTag(char *mess, struct dart_fix_pos *positions,
                     char *mtype, databuf_t * output, size_t i,
                     int *rejected, dart_ex_order_obj * ob);

size_t create_ebs_fix_header(databuf_t * fixmsg,
                                const char *sendercompid,
                                size_t sendercomplen,
                                const char *targetcompid,
                                size_t targetcomplen,
                                const char *sendersubid,
                                size_t sendersublen,
                                const char *targetsubid,
                                size_t targetsublen,
                                unsigned long seq, time_t * now);

long ebs_trans_fix_message(struct message *mess, char *fix_message,
                       long fix_len, databuf_t * output,
                       int *rejected, orderbook * ob
                                                    );

int ebs_fixrom_status_xlate(char *fixmsg, dart_rom_pos * positions,
                            char mtype, char *status,
                            databuf_t * rom, size_t romfield,
                            int *rejtected, dart_ex_order_obj * order,
                            pthread_mutex_t * mutex, hashmap_t ord_map);
void ebs_from_fix_func(struct message *mess, void *ob);
long ebs_fix_to_rom_override(struct message *mess, char *fix_message,
                             databuf_t * output, long fix_len,
                             char rom_type,
                             struct dart_rom_pos positions[ROM_LEN],
                             int *rejected, orderbook * ob, char* type_off);
int ebs_main()
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
    display_factors = load_display_factors("ebs_pairs.map");
    pthread_mutex_unlock(&config_lock);
    ex = init_config("ebs.dat", 16);
    fix_args = load_fix_config("ebs.dat", 16);
    load_rom_config(ex->dc, rom_q);
    fix_client_name = fix_args->target_comp;
    fix_client_len = fix_args->tc_len;
    init_date_str();
    fix_args->reset_time->create_new = 1;
    fix_args->create_mess_header = create_fix_header;
    sender_comp_id = fix_args->sender_comp;
    sci_len = fix_args->sc_len;
    idgenerator = init_order_token_creator();
    ob = init_order_book(ebs_generate_id, get_can_send_orders,
                         def_rom2fix, set_ebs_defaults,
                         def_xlate_fixrom, ex->order_allocation,
                         ex->mod_name, ex->mod_name_len, fix_client_name,
                         fix_client_len, NULL, 0);
    opt_database_init(ex->db_user_name, ex->db_user_name_len,
                      ex->db_password, ex->db_password_len, ex->db_server,
                      ex->db_server_len, ex->dest_id, ex->dest_id_len, ob);

    build_map_from_file("FixToRomTradeCap", ob->fix_to_rom_trade_cap );
    ob->rom2fix[37] = def_rom2fix_no_op;
    ob->fix2rom[14] = ebs_fixrom_status_xlate;
    ob->fix2rom[15] = def_handle_ordids;
    ob->fix2rom[17] = def_add_rom_tag;
    ob->rom2fix[35] = def_rom2fix_no_op;
    ob->rom2fix[38] = def_trans_open_close;
    fs_set_spread_book(ob);
    set_rom2fix_leg_book(ob);
    ob->rom2fix[7] = rom_to_ebs_price;
    ob->rom2fix[17] = set_ebs_exTag;
    ob->rom2fix[50] = set_transacttime;
    ob->rom2fix[12] = rom_to_ebs_account;
    ob->rom2fix[9] = rom_to_ebs_tif;
    ob->rom2fix[4] = rom_to_ebs_side;
    ob->rom2fix[8] = rom_to_ebs_type;
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
    customer_type_code =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10,
                        "Customer Type Code", 18, &ctc_len);
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
        dart_connect(&ebs_from_fix_func, ex->mod_name, ex->mod_name_len,
                     ex->fix_port, ex->fix_ip, ex->fix_ip_len,
                     ex->end_hour, ex->end_min, 7, ex->heart_beat_int, ob,
                     fix_server_connection_callBack, parse_fix_message,
                     fix_args, send_ebs_logon, build_fix_offsets,
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
            if (!get_is_ebs_connected()) {
                set_is_ebs_connected(1);
                fix_args->reset_time->create_new = 1;
                fix_args->reset_time->new_incoming = 1;
                fix_args->reset_time->new_outgoing = 1;
                ret =
                    dart_connect(&ebs_from_fix_func, ex->mod_name,
                                 ex->mod_name_len, ex->fix_port,
                                 ex->fix_ip, ex->fix_ip_len, ex->end_hour,
                                 ex->end_min, 7, ex->heart_beat_int, ob,
                                 fix_server_connection_callBack,
                                 parse_fix_message, fix_args,
                                 send_ebs_logon, build_fix_offsets,
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

int send_ebs_logon(struct parse_args *pa)
{
    char *mblk;
    mblk = calloc(1000, 1);
    if (mblk != NULL) {
        size_t len = sprintf(mblk,
                             "108=1%c98=0%c1137=FIX5.0%c",
                             FIX_FIELD_DELIM,
                             FIX_FIELD_DELIM, FIX_FIELD_DELIM
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

void send_application_logon(con_obj* co)
{
    struct message* m = malloc(sizeof(struct message));
    m->name_len = get_name_len(co);
    m->name = calloc(m->name_len +1 , 1);
    memcpy(m->name, get_name(co), m->name_len);
    size_t id_len = 0;
    char* userid = ebs_generate_id(&id_len);
    m->data = calloc(800,1);
    m->len = sprintf(m->data,
            "1129=1.4%c923=%s%c924=1%c553=%s%c554=%s%c5976=2%c5977=AutoCancelDuplSession%c5978=Y%c5977=PriceCheck%c5978=N%c"
            ,FIX_FIELD_DELIM,
             userid,
             FIX_FIELD_DELIM,
             FIX_FIELD_DELIM,
             user,
             FIX_FIELD_DELIM,
             password,
             FIX_FIELD_DELIM,
             FIX_FIELD_DELIM,
             FIX_FIELD_DELIM,
             FIX_FIELD_DELIM,
             FIX_FIELD_DELIM,
             FIX_FIELD_DELIM
            );
    send_fix_message(m, "BE", 2);
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
        send_application_logon(co);
    }
    fix_connection_callback(co, is_connected);
    set_is_ebs_connected(is_connected);
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


int rom_to_ebs_price(char *rommsg, dart_fix_pos * map,
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
        if (sym_len > 0) {
            sym = rommsg + map[5].beg;
        } else {
            sym = doa_get_symbol(order, &sym_len);
            delete_sym = 1;
        }
        set_val_in_order_struct(p, len, field, order, type);
        pthread_mutex_lock(&config_lock);
        found =
            find_n_fill(display_factors, sym, sym_len, (void *) &rom_val);
        pthread_mutex_unlock(&config_lock);
        char dstr[50];
        double cmep = 0;
        memcpy(dstr, p, len);
        cmep = atof(dstr);
        cmep += 1e-10;
        if(found) {
            rVal = *rom_val;
        } 
        switch(rVal) {
            case 2:
                databuf_write(fix, "%c%s=%.2f", FIX_FIELD_DELIM, map[field].name,
                              cmep);
                break;
            case 3:
                databuf_write(fix, "%c%s=%.3f", FIX_FIELD_DELIM, map[field].name,
                              cmep);
                break;
            case 4:
                databuf_write(fix, "%c%s=%.4f", FIX_FIELD_DELIM, map[field].name,
                              cmep);
                break;
            case 5:
                databuf_write(fix, "%c%s=%.5f", FIX_FIELD_DELIM, map[field].name,
                              cmep);
                break;
            default:
                databuf_write(fix, "%c%s=%.2f", FIX_FIELD_DELIM, map[field].name,
                              cmep);
                break;
        }
        if (delete_sym) {
            free(sym);
        }
    }
    return result;
}

int rom_to_ebs_tif(char *rom, romindex_t * map, char *type,
                      databuf_t * fix, size_t field, int *rejected,
                      dart_ex_order_obj * order)
{
    int result = 0;
    size_t len = map[field].end - map[field].beg;
    if (len > 0 && map[field].len > 0) {
        char *p = rom + map[field].beg;
        set_val_in_order_struct(p, len, field, order, type);
        if(p[0] == '0') {
            result =
                fixmsg_addstr2(fix, map[field].name, map[field].len, "1", 1);
        } else {
            result =
                fixmsg_addstr2(fix, map[field].name, map[field].len, "3", 1);
        }
    } else {
        char* p = doa_get_tif(order, &len);
        if(len > 0 && p != NULL) {
            if(p[0] == '0') {
                result =
                    fixmsg_addstr2(fix, map[field].name, map[field].len, "1", 1);
            } else {
                result =
                    fixmsg_addstr2(fix, map[field].name, map[field].len, "3", 1);
            }
            free(p);
        }
    }
    return result;
}


int rom_to_ebs_side(char *rom, romindex_t * map, char *type,
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
int rom_to_ebs_type(char *rom, romindex_t * map, char *type,
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

int rom_to_ebs_account(char *rommsg, dart_fix_pos * map,
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

void send_md_request_message(orderbook* ob, struct message* message)
{
    struct message m;
    char send_buff[3200];
    m.len = sprintf(send_buff, "%c146=25%c55=AUD/NZD%c461=RCSXXX%c63=0%c55=CHF/JPY%c461=RCSXXX%c63=0%c55=EUR/HUF%c461=RCSXXX%c63=0%c55=EUR/AUD%c461=RCSXXX%c63=0%c55=GBP/JPY%c461=RCSXXX%c63=0%c55=GBP/AUD%c461=RCSXXX%c63=0%c55=EUR/CZK%c461=RCSXXX%c63=0%c55=EUR/JPY%c461=RCSXXX%c63=0%c55=USD/JPY%c461=RCSXXX%c63=0%c55=AUD/USD%c461=RCSXXX%c63=0%c55=EUR/CHF%c461=RCSXXX%c63=0%c55=EUR/GBP%c461=RCSXXX%c63=0%c55=EUR/USD%c461=RCSXXX%c63=0%c55=GBP/USD%c461=RCSXXX%c63=0%c55=NZD/USD%c461=RCSXXX%c63=0%c55=USD/CAD%c461=RCSXXX%c63=0%c55=USD/CHF%c461=RCSXXX%c63=0%c55=AUD/JPY%c461=RCSXXX%c63=0%c55=AUD/USD%c461=RCSXXX%c63=0%c55=EUR/NOK%c461=RCSXXX%c63=0%c55=EUR/SEK%c461=RCSXXX%c63=0%c55=GBP/CHF%c461=RCSXXX%c63=0%c55=USD/MXN%c461=RCSXXX%c63=0%c55=USD/NOK%c461=RCSXXX%c63=0%c55=USD/SEK%c461=RCSXXX%c63=0%c262=2012020643866%c263=Z%c264=0%c265=1%c267=1%c269=*%c",
            FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,
            FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,
            FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,
            FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,
            FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,
            FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,
            FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,
            FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,
            FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,
            FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,
            FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,
            FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,
            FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,FIX_FIELD_DELIM,
            FIX_FIELD_DELIM,FIX_FIELD_DELIM);
        m.data = send_buff;
        m.name = fix_client_name;
        m.name_len = fix_client_len;
        send_fix_message(&m, "V", 1);

}
void handle_user_response(char* type_off, int fix_len, orderbook* ob, struct message* message)
{
    char* user_status = strstr(type_off, "926=");
    if(user_status && ((user_status - type_off) < fix_len)) {
        if(strncmp(user_status +4, "1000", 4) == 0) {
            char send_buff[600];
            struct message m;
            new_pass = calloc(pass_len +1, 1);
            new_pass_len = pass_len;
            memcpy(new_pass, "0", 1);
            memcpy((new_pass +1), (password +1), (pass_len -1));
            m.len = sprintf(send_buff, "%c923=ZZ444%c924=3%c553=%s%c554=%s%c925=%s%c",
                              FIX_FIELD_DELIM,FIX_FIELD_DELIM, FIX_FIELD_DELIM, user, FIX_FIELD_DELIM,
                              password, FIX_FIELD_DELIM, new_pass, FIX_FIELD_DELIM);
            m.data = send_buff;
            m.name = fix_client_name;
            m.name_len = fix_client_len;
            send_fix_message(&m, "BE", 2);
        } else if(strncmp(user_status +4, "5", 1) ==0) {

        } else if(strncmp(user_status +4, "1", 1) == 0) {
            send_md_request_message(ob, message);
        }
    }
}

int set_ebs_exTag(char *mess, struct dart_fix_pos *positions,
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
long ebs_translate_status(char *value, databuf_t * output, long value_len,char* fix_msg,
                          dart_rom_pos* positions, dart_ex_order_obj* ord)
{
    long len = value_len;
    if (strncmp("C", value, 1) == 0) {
        databuf_memcpy(output, "12", 2);
        len = 2;
    } else if (strncmp("13", value, 2) == 0) {
        databuf_memcpy(output, "0", 1);
        len = 1;
    } else if (strncmp("0", value, 1) == 0) {
        databuf_memcpy(output, "13", 2);
        len = 2;
    } else if (strncmp("12", value, 2) == 0) {
        databuf_memcpy(output, "C", 1);
        len = 1;
    } else if (strncmp("21", value, 2) == 0) {
        databuf_memcpy(output, "E", 1);
        len = 1;
    } else if (strncmp("E", value, 1) == 0) {
        databuf_memcpy(output, "21", 2);
        len = 2;
    } else if(strncmp("Z", value, 1) == 0) {
        int last_shares = atoi((fix_msg + positions[42].beg));
        int known_cum = doa_get_cum_shares(ord);
        int known_leaves = doa_get_leaves_shares(ord);
        known_cum += last_shares;
        known_leaves -= last_shares;
        if(known_leaves <= 0) {
            databuf_memcpy(output, "2", 1);
        } else {
            databuf_memcpy(output, "1", 1);
        }
        len = 1;
    } else if(strncmp("F", value, 1) == 0) {
        databuf_memcpy(output, "2", 1);
        len = 1;
    } else if(strncmp("2", value, 1) == 0) {
        databuf_memcpy(output, "0", 1);
        len = 1;
    } else {
        databuf_memcpy(output, value, value_len);
    }
    return len;
}
int ebs_fixrom_status_xlate(char *fixmsg, dart_rom_pos * positions,
                            char mtype, char *status,
                            databuf_t * rom, size_t romfield,
                            int *rejtected, dart_ex_order_obj * order,
                            pthread_mutex_t * mutex, hashmap_t ord_map)
{
    char *beg_off = fixmsg;
    size_t len = (positions[romfield].end - positions[romfield].beg);
    beg_off += positions[romfield].beg;
    if (romfield == 14 && mtype != 'U') {
        len = ebs_translate_status(beg_off, rom, len, fixmsg, positions,order);
        status = fixmsg;
        status += positions[romfield].beg;
    } else if (romfield == 14 && mtype == 'U') {
        len = set_status_for_cnrej(fixmsg, positions, status, rom);
    }
    return 1;
}

void ebs_from_fix_func(struct message *mess, void *ob)
{
    struct databuf *output = databuf_alloc(mess->len * 5);
    long total_len;
    struct message m1;
    struct message log;
    int rejected;
    //char* type_offset = mess->data + 3;
    orderbook *book = (orderbook *) ob;
    m1.name = NULL;
    m1.name_len = 0;
    total_len = 0;
    total_len =
        ebs_trans_fix_message(&m1, mess->data, mess->len, output,
                          &rejected, (orderbook *) ob
                          );
    if (total_len == 0) {
        if (m1.name) {
            free(m1.name);
        }
        databuf_destroy(output);
        return;
    }

    m1.data = output->buffer;
    m1.len = databuf_unread(output);
    if(book->is_async) {
        exchange_async_rom_sender(&m1, ob);
    } else {
        send_message_to_roms(&m1, ob);
    }
    log.name = mess->name;
    log.name_len = mess->name_len;
    log.data = mess->data;
    log.len = mess->len;
    log.name = m1.name;
    log.name_len = m1.name_len;
    log.data = m1.data;
    log.len = m1.len;
    dart_rolling_log_going(&log, book->module_name, book->module_name_len);
    databuf_destroy(output);
    if (m1.name) {
        free(m1.name);
    }
}
static hashmap_t get_fix_trans_map(char *type, char *rom_type,
                                   orderbook * ob)
{
    char *end = strchr(type, FIX_FIELD_DELIM);
    if (end != NULL && (end - type) > 1) {
        if (strncmp(type, "AE", 2) == 0) {
            *rom_type = 'S';
            return ob->fix_to_rom_trade_cap;
        } else if (strncmp(type, "AC", 2) == 0) {
            *rom_type = 'R';
            return ob->fix_to_rom_map_rpl;
        } else {
            *rom_type = 'S';
            return ob->fix_to_rom_map_exe;
        }
    } else {
        return get_map_for_type(type[0], rom_type, ob);
    }

}

void ebs_check_n_clean(char *status, hashmap_t cl_ord_id_map,
                   hashmap_t rom_id_map, dart_ex_order_obj * obj,
                   pthread_mutex_t * map_mutex, char rom_type,
                   orderbook * ob, char* type_off)
{
    if (status) {
        int leaves = doa_get_leaves_shares(obj);
        if (strncmp("4", status, 1) == 0 ||
            (strncmp("8", status, 1) == 0 && rom_type != 'U')) {
            size_t ord_id_len = 0;
            size_t rom_id_len = 0;
            size_t rep_id_len = 0;
            char *ord_id = doa_get_cl_ord_id(obj, &ord_id_len);
            char *rom_id = doa_get_rom_id(obj, &rom_id_len);
            char *rep_id = doa_get_rep_ord_id(obj, &rep_id_len);
            pthread_mutex_lock(map_mutex);
            remove_from_map(cl_ord_id_map, ord_id, ord_id_len);
            remove_from_map(rom_id_map, rom_id, rom_id_len);
            if (rep_id != 0) {
                remove_from_map(cl_ord_id_map, rep_id, rep_id_len);
            }
            pthread_mutex_unlock(map_mutex);
            destroy_dart_order_obj(ob->mgr, obj);       //free(obj);
            free(ord_id);
            free(rom_id);
            free(rep_id);
        } else if (strncmp("2", status, 1) == 0 && leaves == 0 &&
                   !doa_get_pending_replace(obj)
                   && !doa_get_is_spread(obj)
                   && (strncmp("AE", type_off, 2) != 0)) {
            size_t ord_id_len = 0;
            size_t rom_id_len = 0;
            char *ord_id = doa_get_cl_ord_id(obj, &ord_id_len);
            char *rom_id = doa_get_rom_id(obj, &rom_id_len);
            pthread_mutex_lock(map_mutex);
            remove_from_map(cl_ord_id_map, ord_id, ord_id_len);
            remove_from_map(rom_id_map, rom_id, rom_id_len);
            pthread_mutex_unlock(map_mutex);
            destroy_dart_order_obj(ob->mgr, obj);       //free(obj);
            free(ord_id);
            free(rom_id);
        }
    }
}


long ebs_fix_to_rom_override(struct message *mess, char *fix_message,
                             databuf_t * output, long fix_len,
                             char rom_type,
                             struct dart_rom_pos positions[ROM_LEN],
                             int *rejected, orderbook * ob, char* type_off)
{
    int i = 1;
    int s_len = 0;
    char *status = NULL;
    dart_ex_order_obj *obj = NULL;
    size_t doa_len = 0;
    if (rom_type == 'U') {
        s_len = databuf_write(output, "S,");
    } else {
        s_len = databuf_write(output, "%c,", rom_type);
    }
    if (!get_order_object
        (fix_message, fix_len, ob->cl_ord_id_map, positions, &ob->mutex,
         rom_type, &obj, mess, output, ob)) {
        return 0;
    }
    mess->name = doa_get_sender_name(obj, &doa_len);
    mess->name_len = doa_len;
    status = fix_message;
    status += positions[14].beg;
    if(strncmp("AE", type_off, 2) == 0 &&
       (strncmp("2", status, 1) == 0 ||
        strncmp("0", status, 1) == 0)) {
        return 0;
    }
    for (i = 1; i < ROM_LEN; ++i) {
        if (ob->fix2rom[i] != 0) {
            if (ob->fix2rom[i] (fix_message, positions, rom_type,
                                status, output, i, rejected,
                                obj, &ob->mutex,
                                ob->cl_ord_id_map) == -1) {
                fprintf(stderr, "%s: Processing %d failed\n",
                        __FUNCTION__, i);
            }
        }
        databuf_memcpy(output, ",", 1);
    }
    databuf_memcpy(output, "\n", 1);
    ebs_check_n_clean(status, ob->cl_ord_id_map, ob->rom_ord_id_map, obj,
                  &ob->mutex, rom_type, ob, type_off);
    if(strncmp("AE", type_off, 2) == 0 && 
       strncmp("Z", status, 1) != 0) {
        char* last = fix_message;
        int last_len = positions[42].end - positions[42].beg;
        last += positions[42].beg;
        if(last_len > 0 && strncmp("0", last, 1) == 0) {
            ebs_send_alert_for_message(fix_message,
                                        fix_len,
                                        positions);

        }
                
        databuf_reset(output);
    } else if(strncmp("F", status, 1) == 0) {
        databuf_reset(output);
    }
    return databuf_unread(output);
}

long ebs_trans_fix_message(struct message *mess, char *fix_message,
                       long fix_len, databuf_t * output,
                       int *rejected, orderbook * ob
                                                    )
{
    char type[4] = { '3', '5', FIX_FIELD_VALUE_DELIM, '\0' };
    char *d_off;
    long ret_code;
    char *ret_val;
    char* type_off = fix_message;
    long len;
    long ret_len;
    len = fix_len;
    d_off = fix_message;
    ret_len = 0;
    ret_code = 0;
    ret_val = 0;
    ret_val = strstr(d_off, type);
    type_off +=  (ret_val - d_off) + 3;
    if (ret_val && strncmp("BF", type_off , 2) != 0) {
        struct dart_rom_pos l_positions[ROM_LEN];
        int i = 0;
        char rom_type = 'S';
        hashmap_t trans_map;
        ret_code = (ret_val - d_off);
        for (i = 0; i < ROM_LEN; ++i) {
            l_positions[i].beg = 0;
            l_positions[i].end = 0;
        }
        d_off += ret_code + 3;
        trans_map = get_fix_trans_map(d_off, &rom_type, ob);
        len -= (ret_code + 3);
        build_dart_positions(l_positions, d_off, len, trans_map);
        ret_len += ebs_fix_to_rom_override(mess, fix_message, output, fix_len,
                                       rom_type, l_positions,
                                       rejected, ob, type_off);
    } else if( strncmp("BF", type_off, 2) == 0) {
       handle_user_response(type_off, fix_len, ob, mess); 
       ret_len = 0;
    }
    return ret_len;
}
