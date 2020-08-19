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
#include "nyse_seq_generator.h"
#include "ex_config_helper.h"
#include "dart_orderbook.h"
#include "rom_handler.h"
#include "rom2fix.h"
#include "mailer.h"
#include "fix_handler.h"
#include "nyse_order_tracker.h"
static pthread_mutex_t connection_lock = PTHREAD_MUTEX_INITIALIZER;

static char *fix_client_name;
static int fix_client_len;

static char *module_name = NULL;        // = databuf_alloc(100);
static int module_name_len = 0;

static int is_nyse_connected = 1;

int get_is_nyse_connected()
{
    int ret = 0;
    pthread_mutex_lock(&connection_lock);
    ret = is_nyse_connected;
    pthread_mutex_unlock(&connection_lock);
    return ret;
}

void set_is_nyse_connected(int is)
{
    pthread_mutex_lock(&connection_lock);
    is_nyse_connected = is;
    pthread_mutex_unlock(&connection_lock);
}

void send_cancel_all();
void con_callBack(con_obj * co, int is_connected, char *log, int len);
void fix_server_connection_callBack(con_obj * co, int is_connected,
                                    char *log, int len);
void test_lac(con_obj * co);
void destroy_rom_queue(queue_t q);
int send_nyse_logon(struct parse_args *pa);

int main(int argc, char **argv)
{
    int ret;
    struct init_args *fix_args;
    ex_config *ex = NULL;
    int ret_len = 0;
    char *reserved_ids = NULL;
    char *branch_code = NULL;
    int branch_code_int = 25;
    struct tm *newtime;
    int keep_running = 1;
    queue_t rom_q;
    time_t long_time;
    char *on_behalf_comp_id;
    int obc_len;
    orderbook *ob = NULL;       // init_order_book(next_order_id, def_rom2fix,50000);
    rom_q = create_queue();
    ex = init_config("NYSEConfig.dat", 14);
    fix_args = load_fix_config("NYSEConfig.dat", 14);
    load_rom_config(ex->dc, rom_q);
    fix_client_name = fix_args->target_comp;
    fix_client_len = fix_args->tc_len;

    ob = init_order_book(next_order_id, get_is_nyse_connected,
                         def_rom2fix, set_nyse_defaults,
                         def_xlate_fixrom, ex->order_allocation,
                         ex->mod_name, ex->mod_name_len, fix_client_name,
                         fix_client_len, NULL, 0);
    module_name_len = ex->mod_name_len;
    module_name = calloc(module_name_len + 1, 1);
    memcpy(module_name, ex->mod_name, module_name_len);;
    start_dart_mail_service(ex->mail_server, ex->mail_server_len,
                            ex->biz_group, ex->b_len, ex->tech_group,
                            ex->t_len, ex->end_hour, ex->end_min, 7);
    branch_code =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "Branch Code", 11,
                        &ret_len);
    if (branch_code != NULL) {
        branch_code_int = atoi(branch_code);
    }

    on_behalf_comp_id =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "OnBehalfOfCompId",
                        16, &ret_len);
    obc_len = ret_len;
    reserved_ids =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10,
                        "Reserved Branch Codes", 21, &ret_len);
    start_seq_generator(reserved_ids, ret_len, "NYSEOrderids.map", 16,
                        branch_code_int, fix_args,
                        ex->end_hour, ex->end_min);

    init_nyse_tracker(on_behalf_comp_id, obc_len, &ex->db, ex->dest_id,
                      ex->dest_id_len, ob, fix_args);

    ret =
        start_dart_client_services(&nyse_from_rom_func, ex->mod_name,
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
                     fix_args, send_nyse_logon, build_fix_offsets,
                     low_activity_sig, 1);

    while (keep_running) {
        sleep(25);
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
            if (!get_is_nyse_connected()) {
                set_is_nyse_connected(1);
                ret =
                    dart_connect(&from_fix_func, ex->mod_name,
                                 ex->mod_name_len, ex->fix_port,
                                 ex->fix_ip, ex->fix_ip_len, ex->end_hour,
                                 ex->end_min, 7, ex->heart_beat_int, ob,
                                 fix_server_connection_callBack,
                                 parse_fix_message, fix_args,
                                 send_nyse_logon, build_fix_offsets,
                                 low_activity_sig, 1);
            }
        }
    }
    clean_dart_oo_pool(ob->mgr);
    free(reserved_ids);
    free(branch_code);
    free(on_behalf_comp_id);
    free_init_args(fix_args);
    destroy_ex_config(ex);
    destroy_rom_queue(rom_q);
    destroy_queue(rom_q);
    return 0;
}

int send_nyse_logon(struct parse_args *pa)
{
    char *mblk;
    mblk = calloc(600, 1);
    if (mblk != NULL) {
        size_t len =
            sprintf(mblk, "98=0%c108=30%c95=1%c96=1%c", FIX_FIELD_DELIM,
                    FIX_FIELD_DELIM, FIX_FIELD_DELIM, FIX_FIELD_DELIM);
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
    char *name = (char *) get_name(co);
    printf("Info about this FIX Connection: %s, Is it connected {%d} ? \n",
           name, is_connected);
    fix_connection_callback(co, is_connected);
    set_is_nyse_connected(is_connected);
    if (name) {
        free(name);
    }
}

void test_lac(con_obj * co)
{
    char *name = (char *) get_name(co);
    printf("Low activity reported on this connection: %s \n", name);
    if (name) {
        free(name);
    }
}

void con_callBack(con_obj * co, int is_connected, char *log, int len)
{
    char *name = (char *) get_name(co);
    printf("Info about this Connection: %s, Is it connected {%d} ? \n",
           name, is_connected);
    if (name) {
        free(name);
    }
}
