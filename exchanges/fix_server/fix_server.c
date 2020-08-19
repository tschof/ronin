/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "version.h"
#include "client_manager.h"
#include "message_queue.h"
#include "parser_func.h"
#include "rom_handler.h"
#include "fix_handler.h"
#include "fix_rom_trans.h"
#include "dart_constants.h"
#include "config_loader.h"
#include "fix_admin_master.h"
#include "ex_common.h"
#include "dart_orderbook.h"
#include "complex_parser.h"

static char *rom_client_name;
static int rom_client_len;
static char *fix_client_name;
static int fix_client_len;
static hashmap_t clearing_ids;

static int is_con_now = 1;
static int cancel_on_disconnect = 1;

static char *module_name;
static int module_name_len;
static int end_hour = 0;
static int end_min = 0;

char *fix_server_id_gen(size_t * len)
{
    return NULL;
}
hashmap_t load_clearing_ids(const char* filename)
{
    hashmap_t h = create_map(5);
    gen_build_clear_id_map(filename, h);
    return h;
}
static void create_tag_sixty(databuf_t * output)
{
    time_t rawtime;
    struct tm *ti;
    struct tm lm;
    int year;
    int month;
    databuf_write(output, "%c60=", FIX_FIELD_DELIM);
    time(&rawtime);
    ti = gmtime_r(&rawtime, &lm);
    ti = &lm;
    year = 1900 + ti->tm_year;
    month = 1 + ti->tm_mon;
    databuf_write(output,
                  "%04d%02d%02d-%02d:%02d:%02d", year,
                  month, ti->tm_mday, ti->tm_hour, ti->tm_min, ti->tm_sec);
}

long rapid_rom_to_fix_override(struct message *mess, char *fix_message,
                                 databuf_t * output, long fix_len,
                                 char *fix_type,
                                 struct dart_fix_pos positions[],
                                 int *rejected, orderbook * ob, int is_spread)
{

    char end_ascii_one[2] = { FIX_FIELD_DELIM, '\0' };
    int i = 1;
    int should_parse = 1;
    int s_len;
    char *status_offset;
    int status_len = 0;
    status_offset = NULL;
    status_offset = fix_message;
    status_offset += positions[14].beg;
    status_len = (positions[14].end - positions[14].beg);
    if (strncmp("14", status_offset, 2) != 0 &&
        strncmp("20", status_offset, 2) != 0 &&
        strncmp("29", status_offset, 2) != 0 &&
        strncmp("30", status_offset, 2) != 0) {
        if (*fix_type == '8') {
            char *s_off;
            s_off = fix_message;
            s_off += positions[14].beg;
            if (strncmp("5", s_off, 1) == 0 || strncmp("4", s_off, 1) == 0) {
                positions[3].name = "41";
                positions[60].name = "11";
            }
        }
        while (i < ROM_LEN && should_parse) {
            if (positions[i].beg == positions[i].end
                && positions[i].name != NULL) {
                if (*fix_type == '8' && (i == 44 || i == 42)) {
                    char *tag_type = NULL;
                    tag_type = i == 42 ? "32=0" : "31=0";
                    s_len =
                        databuf_write(output,
                                      "%c%s", FIX_FIELD_DELIM, tag_type);
                } else if (i == 50) {
                    create_tag_sixty(output);
                } else if (i == 60) {
                    char *l_beg = fix_message;
                    long len = 0;
                    l_beg += positions[3].beg;
                    len = (positions[3].end - positions[3].beg);
                    s_len =
                        databuf_write(output, "%c%s=", FIX_FIELD_DELIM,
                                      positions[i].name);
                    databuf_memcpy(output, l_beg, len);
                }
            } else if (positions[i].name != NULL) {

                char *beg_off;
                long len;
                beg_off = fix_message;
                beg_off += positions[i].beg;
                len = (positions[i].end - positions[i].beg);
                s_len =
                    databuf_write(output, "%c%s=", FIX_FIELD_DELIM,
                                  positions[i].name);
                if (i != 14 && i != 31 && i != 38 && i != 35 && i != 12 && i != 66) {
                    databuf_memcpy(output, beg_off, len);
                } else if( i == 35) {
                    char* cplx = fix_message;
                    cplx += positions[66].beg;
                    int cplx_len = positions[66].end - positions[66].beg;
                    if(cplx_len > 0 && (cplx[0] == '5' || cplx[0] == '3')) {
                        databuf_memcpy(output, "MLEG", 4);
                    } else {
                        len = determine_trans_type(i, beg_off, output, len);
                    }
                } else if (i == 31 || i == 38 || i == 35) {
                    len = determine_trans_type(i, beg_off, output, len);
                } else if (i == 12) {
                    char* account = fix_message + positions[22].beg;
                    int acc_len = positions[22].end - positions[22].beg;
                    if(acc_len > 0) {
                        char* rom_val= 0;
                        int found =
                            find_n_fill(clearing_ids, account, acc_len, (void *) &rom_val);
                        if(found) {
                            databuf_memcpy(output, rom_val, found);
                        }  else {
                            databuf_memcpy(output, beg_off, len);
                        }
                    } else {
                        databuf_memcpy(output, beg_off, len);
                    }
                } else if(i == 66 && len > 0) {
                    if(beg_off[0] == '6') {
                        databuf_memcpy(output, "2", 1);
                    } else if(beg_off[0] == '5' || beg_off[0] == '3') {
                        databuf_memcpy(output, "3", 1);
                    } else {
                        databuf_memcpy(output, "1", 1);
                    }
                } else {
                    s_len = determine_trans_type(14, beg_off, output, len);
                    s_len =
                        databuf_write(output, "%c150=", FIX_FIELD_DELIM);
                    s_len = determine_trans_type(14, beg_off, output, len);
                    s_len =
                        databuf_write(output, "%c20=0", FIX_FIELD_DELIM);
                }
            }
            ++i;
        }
    } 
    databuf_memcpy(output, end_ascii_one, 1);
    return databuf_unread(output);
}
struct init_args *create_rom_init_args();
void send_cancel_all();
void con_callBack(con_obj * co, int is_connected, char *log, int len);
void fix_server_connection_callBack(con_obj * co, int is_connected,
                                    char *log, int len);
void fs_from_rom_func(struct message *mess, void *ob);
void fs_from_fix_func(struct message *mess, void *ob);
void test_lac(con_obj * co);

int main(int argc, char **argv)
{
    int ret;

    struct init_args *rom_args;
    struct init_args *fix_args;
    char *fix_ip;
    int fix_ip_len = 0;
    int ret_len = 0;
    char *fix_port = NULL;
    short fport = 0;
    char *rom_port = NULL;
    short rport = 0;
    char *rom_addr = NULL;
    int rom_addr_len = 0;
    struct tm *newtime;
    int keep_running = 1;
    queue_t fix_q;
    time_t long_time;
    char *cancel_choice;
    char *end_str;
    char *hb_int_str;
    int hb_int = 20;
    orderbook *ob = def_init_order_book();
    fix_args = load_fix_config("FixConfig.dat", 13);
    printf("Loaded fix config\n");
    if (fix_args == NULL) {
        printf("Fix Args null problem with configuration.\n");
    }
    rom_args = create_rom_init_args(fix_args->dc);

    fix_q = create_queue();
    enqueue(fix_q, fix_args);
    rom_client_name = rom_args->name;
    rom_client_len = rom_args->name_len;
    fix_client_name = fix_args->target_comp;
    fix_client_len = fix_args->tc_len;
    clearing_ids = load_clearing_ids("IDtomnemonic.map");
    cancel_choice =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "Cancel Choice",
                        13, &ret_len);
    if (cancel_choice != NULL) {
        cancel_on_disconnect = atoi(cancel_choice);
    }
    module_name =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "Mod Name", 8,
                        &ret_len);
    module_name_len = ret_len;
    if (module_name == NULL) {
        module_name = calloc(10, 1);
        memcpy(module_name, "FixServer", 9);
        module_name_len = 9;
    }
    fix_ip =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "Fix IP", 6,
                        &ret_len);
    fix_ip_len = ret_len;
    fix_port =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "Fix Port", 8,
                        &ret_len);
    fport = (short) atoi(fix_port);
    rom_port =
        get_val_for_tag(fix_args->dc, "ROM SERVER", 10, "Rom Port", 8,
                        &ret_len);
    if (rom_port) {
        rport = (short) atoi(rom_port);
    } else {
        rport = 8008;
    }
    rom_addr =
        get_val_for_tag(fix_args->dc, "ROM SERVER", 10, "Rom Host", 8,
                        &ret_len);
    rom_addr_len = ret_len;

    hb_int_str =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "Heartbeat Int",
                        13, &ret_len);
    if (hb_int_str != NULL) {
        hb_int = atoi(hb_int_str);
    }

    end_str =
        get_val_for_tag(fix_args->dc, "TIMES", 5, "End Hour", 8, &ret_len);
    if (end_str != NULL) {
        end_hour = atoi(end_str);
    } else {
        end_hour = 00;
    }
    end_str = NULL;
    end_str =
        get_val_for_tag(fix_args->dc, "TIMES", 5, "End Min", 7, &ret_len);
    if (end_str != NULL) {
        end_min = atoi(end_str);
    } else {
        end_min = 00;
    }

    fs_set_spread_book(ob);
    ret =
        start_dart_client_services(&fs_from_fix_func, module_name,
                                   module_name_len, fport, fix_q, fix_ip,
                                   fix_ip_len, end_hour, end_min, 7,
                                   hb_int, ob,
                                   &fix_server_connection_callBack,
                                   &parse_fix_message, &build_fix_offsets,
                                   &low_activity_sig,
                                   &create_fix_resend_pad, 1);
    ret =
        dart_connect(&fs_from_rom_func, module_name, module_name_len,
                     rport, rom_addr, rom_addr_len, end_hour, end_min, 7,
                     hb_int, ob, &con_callBack, parse_rom_message,
                     rom_args, &init_rom_logon, &build_offsets, &test_lac,
                     0);

    while (keep_running) {
        time(&long_time);
        newtime = localtime(&long_time);
        if (newtime->tm_hour == end_hour && newtime->tm_min >= end_min) {
            keep_running = 0;
            printf("Done with our test!!!!end_hour %d end_min %d now %d\n",
                   end_hour, end_min, newtime->tm_hour);
        } else {
            sleep(25);
            if (!is_con_now) {
                is_con_now = 1;
                dart_connect(&fs_from_rom_func, module_name,
                             module_name_len, rport, rom_addr,
                             rom_addr_len, end_hour, end_min, 7, hb_int,
                             ob, &con_callBack, parse_rom_message,
                             rom_args, &init_rom_logon, &build_offsets,
                             &test_lac, 0);
            }
        }
    }
    return 0;
}

void fix_server_connection_callBack(con_obj * co, int is_connected,
                                    char *log, int len)
{
    printf("Info about this FIX Connection: %s, Is it connected {%d} ? \n",
           get_name(co), is_connected);
    if (is_connected) {
    } else {
        if (cancel_on_disconnect) {
            fix_connection_callback(co, is_connected);
            send_cancel_all();
        }
        //remove_fix_con(co);
    }
    // 
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
static void fs_send_message_to_roms(struct message *mess, orderbook * ob)
{
    long len_sent = 0;
    long ret_val = 0;
    int failed_attempts = 0;
    long t_len = mess->len;
    char *orig_data = mess->data;
    char *mess_off = mess->data;
    if (pthread_mutex_lock(&ob->send_mutex) == 0) {
        while (len_sent < t_len && len_sent >= 0 && failed_attempts < 10) {
            ret_val = send_message(mess);
            if (ret_val < 0) {
                ++failed_attempts;
                ret_val = 0;
            }
            mess_off += ret_val;
            mess->len -= ret_val;
            mess->data = mess_off;
            len_sent += ret_val;
        }
        if (pthread_mutex_unlock(&ob->send_mutex) != 0) {
        }
    }
    mess->len = t_len;
    mess->data = orig_data;
    record_sent_message(mess);
}

void send_cancel_all()
{
    struct message m;
    char *output;
    int len;
    output = calloc(300, 1);
    m.name = rom_client_name;
    m.name_len = rom_client_len;
    len =
        sprintf(output, "c,,%s,,,,,,,,,,,,,,,,,,,,,,,,,,\n",
                fix_client_name);
    m.data = output;
    m.len = len;
    send_message(&m);
    free(output);
}

void fs_from_fix_func(struct message *mess, void *ob)
{
    databuf_t *output = databuf_alloc(mess->len * 5);
    long total_len;
    struct message m1;
    int rejected;
    struct message log;
    total_len = 0;
    m1.name = rom_client_name;
    m1.name_len = rom_client_len;
    total_len =
        trans_fix_message(&m1, mess->data, mess->len, output,
                          &rejected, (orderbook *) ob,
                          fix_rom_spread_override);
    m1.data = output->buffer;
    m1.len = databuf_unread(output);

    fs_send_message_to_roms(&m1, ob);
    log.name = rom_client_name;
    log.name_len = rom_client_len;
    log.data = output->buffer;
    log.len = databuf_unread(output);
    dart_rolling_log_going(&log, module_name, module_name_len);
    databuf_destroy(output);
}

/**
 * This functions tests the callback into the main application.
 */
void fs_from_rom_func(struct message *mess, void *ob)
{
    databuf_t *output = databuf_alloc(mess->len * 5);
    long total_len;
    struct message m1;
    struct message log;
    char fix_type[3];
    int ft_len = 0;
    int rejected = 0;
    total_len = 0;
    total_len =
        trans_rom_message(&m1, mess->data, mess->len, output,
                          fix_type, &ft_len, &rejected, (orderbook *) ob,
                          &rapid_rom_to_fix_override);

    log.name = mess->name;
    log.name_len = mess->name_len;
    log.data = mess->data;
    log.len = mess->len;
    dart_rolling_log_coming(&log, module_name, module_name_len);
    if (rejected) {
        //do nothing
    } else {
        m1.name = fix_client_name;
        m1.name_len = fix_client_len;
        m1.data = output->buffer;
        m1.len = databuf_unread(output);
        send_fix_message(&m1, fix_type, 1);
    }
    databuf_destroy(output);
}

struct init_args *create_rom_init_args(dart_config * dc)
{
    struct init_args *in_args;
    struct seq_reset_time *rt;
    int ret_len;
    char *reset_h;
    char *reset_m;
    char *last_run_date;
    char *new_incoming;
    char *new_outgoing;
    char *end_day_str = 0;
    int end_day_int = 0;
    time_t rawtime;
    struct tm *ti;
    char *now;
    int year;
    int month;
    int len;
    ret_len = 0;
    in_args = malloc(sizeof(struct init_args));
    in_args->dc = dc;           //initialize_config("FixConfig.dat", 13);
    in_args->name =
        get_val_for_tag(in_args->dc, "ROM SERVER", 10,
                        "Rom Connection Name", 19, &ret_len);
    in_args->name_len = ret_len;
    in_args->sender_comp = NULL;
    in_args->sc_len = 0;
    in_args->target_comp = NULL;
    in_args->tc_len = 0;
    in_args->target_sub = NULL;
    in_args->ts_len = 0;
    in_args->sender_sub = NULL;
    in_args->ss_len = 0;
    in_args->fix_version = NULL;
    in_args->fv_len = 0;
    in_args->username =
        get_val_for_tag(in_args->dc, "ROM SERVER", 10, "Rom User Name", 13,
                        &ret_len);
    in_args->un_len = ret_len;
    in_args->password = in_args->username;
    in_args->p_len = in_args->un_len;
    rt = calloc(sizeof(struct seq_reset_time), 1);
    reset_h =
        get_val_for_tag(in_args->dc, "TIMES", 5, "Reset Hour", 10,
                        &ret_len);
    if (reset_h != NULL) {
        rt->reset_hour = atoi(reset_h);
    } else {
        rt->reset_hour = 0;
    }
    reset_m =
        get_val_for_tag(in_args->dc, "TIMES", 5, "Reset Min", 9, &ret_len);
    if (reset_m != NULL) {
        rt->reset_min = atoi(reset_m);
    } else {
        rt->reset_min = 0;
    }
    new_incoming =
        get_val_for_tag(in_args->dc, "ROM SERVER", 10, "Reset Incoming",
                        14, &ret_len);
    if (new_incoming != NULL) {
        rt->new_incoming = atoi(new_incoming);
    } else {
        rt->new_incoming = 0;
    }
    new_outgoing =
        get_val_for_tag(in_args->dc, "ROM SERVER", 10, "Reset Outgoing",
                        14, &ret_len);
    if (new_outgoing != NULL) {
        rt->new_outgoing = atoi(new_outgoing);
    } else {
        rt->new_outgoing = 0;
    }

    end_day_str =
        get_val_for_tag(in_args->dc, "TIMES", 5, "End Day", 7, &ret_len);
    if (end_day_str != NULL) {
        end_day_int = atoi(end_day_str);
    } else {
        end_day_int = 7;
    }
    rt->reset_day = end_day_int;
    rt->last_logon = 0;
    last_run_date =
        get_val_for_tag(in_args->dc, "TIMES", 5, "Last Run Day", 12,
                        &ret_len);
    time(&rawtime);
    ti = localtime(&rawtime);
    now = calloc(9, 1);
    year = 1900 + ti->tm_year;
    month = 1 + ti->tm_mon;
    len = sprintf(now, "%04d%02d%02d", year, month, ti->tm_mday);
    if (strncmp(now, last_run_date, 8) == 0 && len <= 8) {
        rt->create_new = 0;
    } else if (ti->tm_wday == end_day_int || end_day_int == 7) {
        rt->create_new = 1;
    }
    in_args->reset_time = rt;
    in_args->log_outgoing = 1;
    return in_args;
}
