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
#include "rom2fix.h"
#include "ex_config_helper.h"
#include "dart_orderbook.h"
#include "rom_handler.h"
#include "rom2fix.h"
#include "mailer.h"
#include "order_token_creator.h"
#include "dart_obj_factory.h"
#include "dart_order_obj.h"
#include "fix_handler.h"

static pthread_mutex_t connection_lock = PTHREAD_MUTEX_INITIALIZER;

static token_struct_t *idgenerator = 0;
static char *fix_client_name;
static int fix_client_len;
static orderbook *ob = NULL;       // init_order_book(next_order_id, def_rom2fix,50000);
static char *module_name = NULL;        // = databuf_alloc(100);
static int module_name_len = 0;

static int is_con_now = 0;
static int is_async_connected = 1;
int get_is_async_connected()
{
    int ret = 0;
    pthread_mutex_lock(&connection_lock);
    ret = is_async_connected;
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

void set_is_async_connected(int is)
{
    pthread_mutex_lock(&connection_lock);
    is_async_connected = is;
    ob->can_send_orders = is;
    pthread_mutex_unlock(&connection_lock);
}

char *async_next_order_id(size_t * len)
{
    char *cl_ord_id = 0;
    if (len != 0) {
        int retval = 0;
        cl_ord_id = calloc(DART_EX_TAG_LEN, 1);
        if ((retval = create_order_token(idgenerator, cl_ord_id,
                                         DART_EX_TAG_LEN - 1)) > 0) {
            *len = (size_t) retval;
        } else {
            *len = 0;
        }
    }
    return cl_ord_id;
}

int set_async_defaults(char *rommsg, struct dart_fix_pos *positions,
                       char *mtype, databuf_t * output,
                       size_t romfield, int *rejected,
                       dart_ex_order_obj * order)
{
    databuf_write(output, "%c167=CS", FIX_FIELD_DELIM);
    return 1;
}

size_t test_create_rom_reject(char *data, databuf_t * output,
                              const char *message)
{
    int i = 0;
    size_t cur_off = 0;
    char *ret_val = NULL;
    size_t ret_code = 0;
    size_t end_pos;
    char *mess_off = data;
    for (i = 0; i < 3; ++i) {
        ret_val = strchr(mess_off, ',');
        if (ret_val) {
            ret_code = (ret_val - mess_off);
            mess_off += (ret_code + 1);
            cur_off += 1 + ret_code;
        } else {
            printf("Failed to properly parse this rom message: %s", data);
            return 0;
        }
    }
    ret_val = strchr(mess_off, ',');
    if (ret_val) {
        size_t bytes_written = 0;
        size_t ret_bytes = 0;
        //char *ret_status = "8";

        char *ret_status = NULL;
        if (data[0] == 'R') {
            ret_status = "20";
        } else if (data[0] == 'C') {
            ret_status = "14";
        } else {
            ret_status = "8";
        }
        end_pos = (ret_val - mess_off);
        bytes_written = databuf_memcpy(output, "S,,,", 4);
        ret_bytes += bytes_written;
        databuf_memcpy(output, mess_off, end_pos);
        ret_bytes += end_pos;
        bytes_written =
            databuf_write(output,
                          ",,,,,,,,,,,%s,,,,,,,,,,,,%s,,,,,,,,,,,,,,,,,,,,\n",
                          ret_status, message);
        ret_bytes += bytes_written;
        return ret_bytes;
    } else {
        printf("Failed to find comma after rom tag: %s", data);
        return 0;
    }
}


void create_databuffs(queue_t glob, queue_t q, int num)
{
    int i = 0;
    for(; i< num; ++i) {
        databuf_t* buff = databuf_alloc(500);
        enqueue(q, buff);
    }
}
void clear_db(void* db)
{
    databuf_t* buff = (databuf_t*)db;
    databuf_reset(buff);
}


void destroy_db(void* d)
{
    databuf_t* buff = (databuf_t*)d;
    databuf_destroy(buff);
}
void async_from_rom_func(const char* name, int name_len,void *b)
{
    //char debugger[300];
    //int d_len = sprintf(debugger, "Write callback in async server %s  value: %s.",name, (char*)b);
    //debug_rolling_log(debugger, d_len);
}

void send_cancel_all();
void con_callBack(con_obj * co, int is_connected, char *log, int len);
void fix_server_connection_callBack(con_obj * co, int is_connected,
                                    char *log, int len);
void test_lac(con_obj * co);
void destroy_rom_queue(queue_t q);
int send_async_logon(struct async_parse_args *pa);

int main(int argc, char **argv)
{
    struct init_args *fix_args;
    ex_config *ex = NULL;
    int ret_len = 0;
    char *reserved_ids = NULL;
    char *branch_code = NULL;
    int branch_code_int = 25;
    struct tm *newtime;
    int keep_running = 1;
    char* callback_name = calloc(20, 1);
    memcpy(callback_name, "ASYNC", 5);
    queue_t rom_q;
    time_t long_time;
    char *on_behalf_comp_id;
    int obc_len;
    int my_cpus[] = {1,2,3};
    int * cpu = malloc(3 * sizeof(int));
    rom_parser_helper* rph = create_rph(ph_build_dart_obj);
    memcpy(cpu, &my_cpus, (3 * sizeof(int)));   
    rom_q = create_queue();
    ex = init_config("NYSEConfig.dat", 14);
    fix_args = load_fix_config("NYSEConfig.dat", 14);
    load_rom_config(ex->dc, rom_q);
    fix_client_name = fix_args->target_comp;
    fix_client_len = fix_args->tc_len;
    async_server_con_set *ascs = 0;
    idgenerator = init_order_token_creator();
    ob = init_order_book(async_next_order_id, get_can_send_orders,
                         def_rom2fix, set_async_defaults,
                         def_xlate_fixrom, ex->order_allocation,
                         ex->mod_name, ex->mod_name_len, fix_client_name,
                         fix_client_len, NULL, 1);
    ob->fix2rom[17] = def_add_rom_tag;
    ob->fix2rom[15] = def_handle_ordids;
    ob->rom2fix[37] = def_rom2fix_no_op;
    ob->fix2rom[15] = def_handle_ordids;
      dof_t* objects =  create_dart_obj_fact(ex->order_allocation,
                                             destroy_doo,
                                             create_dart_obj_func,
                                             clean_doo);
      dof_t* buffs =  create_dart_obj_fact(100, destroy_db,
                                           create_databuffs,
                                           clear_db);
      ob->ord_obj = objects;
      ob->databufs = buffs;
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

    ascs =
        async_start_dart_client_services_2(&from_rom_func, ex->mod_name,
                                         ex->mod_name_len, ex->rom_port,
                                         rom_q, ex->rom_ip, ex->rom_ip_len,
                                         ex->end_hour, ex->end_min, 7,
                                         ex->heart_beat_int, ob,
                                         con_callBack, ph_parse_rom_message,
                                         build_offsets, test_lac,
                                         create_rom_resend_pad, 0, 3, 0, cpu, 1,ob,
                                         exchange_async_write_callback,
                                         rph, copy_rom_parser_helper, from_rom_helper_func);
    ob->async_serv_con = ascs;

    async_dart_connect(&from_fix_func, ex->mod_name, ex->mod_name_len,
                       ex->fix_port, ex->fix_ip, ex->fix_ip_len,
                       ex->end_hour, ex->end_min, 7, ex->heart_beat_int,
                       ob, fix_server_connection_callBack,
                       parse_fix_message, fix_args, send_async_logon,
                       build_fix_offsets, low_activity_sig, 1, ascs ,ob, exchange_async_write_callback);

    add_async_listener(ascs, &from_rom_func, ex->mod_name,
                                         ex->mod_name_len, 2001,
                                         rom_q, NULL, 0,
                                         ex->end_hour, ex->end_min, 7,
                                         ex->heart_beat_int, ob,
                                         con_callBack, ph_parse_rom_message,
                                         build_offsets, test_lac,
                                         create_rom_resend_pad,0, ob,
                                         exchange_async_write_callback,
                                         rph, copy_rom_parser_helper, from_rom_helper_func);
    
    add_async_listener(ascs, &from_rom_func, ex->mod_name,
                                         ex->mod_name_len, ex->rom_port,
                                         rom_q, NULL, 0,
                                         ex->end_hour, ex->end_min, 7,
                                         ex->heart_beat_int, ob,
                                         con_callBack, ph_parse_rom_message,
                                         build_offsets, test_lac,
                                         create_rom_resend_pad,0, ob,
                                         exchange_async_write_callback,
                                         rph, copy_rom_parser_helper, from_rom_helper_func);
    add_async_listener(ascs, &from_rom_func, ex->mod_name,
                                         ex->mod_name_len, 2000,
                                         rom_q, "DODO", 4,
                                         ex->end_hour, ex->end_min, 7,
                                         ex->heart_beat_int, ob,
                                         con_callBack, ph_parse_rom_message,
                                         build_offsets, test_lac,
                                         create_rom_resend_pad,0, ob,
                                         exchange_async_write_callback,
                                         rph, copy_rom_parser_helper, from_rom_helper_func);

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
            if (!get_is_async_connected()) {
                set_is_async_connected(1);
                fix_args->reset_time->new_incoming = 0;
                fix_args->reset_time->new_outgoing = 0;
                async_dart_connect(&from_fix_func, ex->mod_name,
                                   ex->mod_name_len, ex->fix_port,
                                   ex->fix_ip, ex->fix_ip_len,
                                   ex->end_hour, ex->end_min, 7,
                                   ex->heart_beat_int, ob,
                                   fix_server_connection_callBack,
                                   parse_fix_message, fix_args,
                                   send_async_logon, build_fix_offsets,
                                   low_activity_sig, 1, ascs ,ob, exchange_async_write_callback);
            }
            sleep(25);
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

int send_async_logon(struct async_parse_args *pa)
{
    char *mblk;
    mblk = calloc(600, 1);
    if (mblk != NULL) {
        size_t len =
            sprintf(mblk, "98=0%c108=30%c95=1%c96=1%c", FIX_FIELD_DELIM,
                    FIX_FIELD_DELIM, FIX_FIELD_DELIM, FIX_FIELD_DELIM);
        if (len <= 0) {
            return -1;
        } else {
            send_con_logon(pa->con, pa->t, mblk, len);
        }
        free(mblk);
        return 1;
    }
    return 0;
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
    set_is_async_connected(is_connected);
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
