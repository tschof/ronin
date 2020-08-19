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
#include "version.h"

/* Each project should put customized values and
   function declarations into this header file. */
#include "generic_fix_interface.h"

static pthread_mutex_t connection_lock = PTHREAD_MUTEX_INITIALIZER;

static char *fix_client_name;
static int fix_client_len;

static char *module_name = NULL;
static int module_name_len = 0;

static int is_con_now = 0;
static int is_genericfix_connected = 1;

static const int NUMORDERS = 150000;

int get_is_genericfix_connected()
{
    int ret = 0;
    pthread_mutex_lock(&connection_lock);
    ret = is_genericfix_connected;
    pthread_mutex_unlock(&connection_lock);
    return ret;
}

void set_is_genericfix_connected(int is)
{
    pthread_mutex_lock(&connection_lock);
    is_genericfix_connected = is;
    pthread_mutex_unlock(&connection_lock);
}

int generic_fix_initialize_translators(orderbook * pbook)
{
    int result = -1;
    if (pbook != 0) {
        size_t i;
        /* ROM to FIX */
        for (i = 0; i < ROM_LEN; ++i) {
            pbook->rom2fix[i] = def_rom2fix;
        }
        pbook->rom2fix[4] = def_trans_side;
        pbook->rom2fix[8] = def_trans_ordtype;
        pbook->rom2fix[10] = def_trans_capacity;
        pbook->rom2fix[50] = set_transacttime;
        pbook->rom2fix[57] = def_trans_execinst;

        /* FIX to ROM */
        pbook->fix2rom[63] = def_no_op;
        pbook->fix2rom[14] = def_fixrom_status_xlate;
        pbook->fix2rom[15] = def_handle_ordids;
        pbook->fix2rom[17] = def_add_rom_tag;
        pbook->fix2rom[14] = def_fixrom_status_xlate;

        result = 0;
    }
    return result;
}

int generic_fix_initialization(ex_config * exchange_config,
                               orderbook * pbook,
                               struct fix_interface *handler)
{
    int result = -1;
    if (handler != 0) {
        result = generic_fix_initialize_translators(pbook);
        if (result == 0) {
            database_init(&exchange_config->db,
                          exchange_config->dest_id,
                          create_queue(),
                          pbook->cl_ord_id_map, pbook->rom_ord_id_map,
                          pbook);
            result = handler->onstart(exchange_config, pbook);
        }
    }
    return result;
}


void send_cancel_all();
void con_callBack(con_obj * co, int is_connected, char *log, int len);
void fix_server_connection_callBack(con_obj * co, int is_connected,
                                    char *log, int len);
void test_lac(con_obj * co);
void destroy_rom_queue(queue_t q);
int send_genericfix_logon(struct parse_args *pa);
char *generic_fix_generate_id(size_t * nlen);

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
    int result = 0;
    struct fix_interface handler;

    /* About the first thing to do is to call the generic_fix_interface_main()
       function to pass control to the destination-specific code to setup the
       FIX destination interface handler. */
    memset(&handler, 0, sizeof handler);
    handler.config_file = "config.dat";
    if ((result = generic_fix_interface_main(argc, argv, &handler)) != 0) {
        fprintf(stderr, "Bailing out, init failed\n");
        return result;
    }
    rom_q = create_queue();
    ex = init_config(handler.config_file, handler.file_len);
    if ((fix_args =
         load_fix_config(handler.config_file, handler.file_len)) == 0) {
        fprintf(stderr, "Could not load config file\n");
        return 1;
    }
    load_rom_config(ex->dc, rom_q);
    fix_client_name = fix_args->target_comp;
    fix_client_len = fix_args->tc_len;

    printf("Initializing order book with %d orders\n", NUMORDERS);
    ob = init_order_book(handler.id_generator, get_is_genericfix_connected,
                         def_rom2fix, handler.pre_translate_func,
                         def_xlate_fixrom, NUMORDERS,
                         ex->mod_name, ex->mod_name_len, fix_client_name,
                         fix_client_len, NULL, 0);
    startup_allocator(ex->order_allocation);
    module_name_len = ex->mod_name_len;
    module_name = calloc(module_name_len + 1, 1);
    memcpy(module_name, ex->mod_name, module_name_len);;
    start_dart_mail_service(ex->mail_server, ex->mail_server_len,
                            ex->biz_group, ex->b_len, ex->tech_group,
                            ex->t_len, ex->end_hour, ex->end_min,
                            ex->end_day);
    if (generic_fix_initialization(ex, ob, &handler) == -1) {
        /* Bail out, something went wrong with the initialization. */
    }

    /* create_new determines whether the sequence numbers will be reset */
    ret =
        start_dart_client_services(&from_rom_func, ex->mod_name,
                                   ex->mod_name_len, ex->rom_port, rom_q,
                                   ex->rom_ip, ex->rom_ip_len,
                                   ex->end_hour, ex->end_min, ex->end_day,
                                   ex->heart_beat_int, ob, con_callBack,
                                   parse_rom_message, build_offsets,
                                   test_lac, create_rom_resend_pad, 0);
    ret =
        dart_connect(&from_fix_func, ex->mod_name, ex->mod_name_len,
                     ex->fix_port, ex->fix_ip, ex->fix_ip_len,
                     ex->end_hour, ex->end_min, ex->end_day,
                     ex->heart_beat_int, ob,
                     fix_server_connection_callBack, parse_fix_message,
                     fix_args, send_genericfix_logon, build_fix_offsets,
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
            if (!get_is_genericfix_connected()) {
                set_is_genericfix_connected(1);
                ret =
                    dart_connect(&from_fix_func, ex->mod_name,
                                 ex->mod_name_len, ex->fix_port,
                                 ex->fix_ip, ex->fix_ip_len, ex->end_hour,
                                 ex->end_min, ex->end_day,
                                 ex->heart_beat_int, ob,
                                 fix_server_connection_callBack,
                                 parse_fix_message, fix_args,
                                 send_genericfix_logon, build_fix_offsets,
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

int send_genericfix_logon(struct parse_args *pa)
{
    char *mblk;
    size_t len;
    mblk = calloc(600, 1);
    if (mblk != NULL) {
        len =
            sprintf(mblk, "98=0%c108=30%c95=1%c96=1%c", FIX_FIELD_DELIM,
                    FIX_FIELD_DELIM, FIX_FIELD_DELIM, FIX_FIELD_DELIM);
        if (len > 0) {
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
        set_is_genericfix_connected(is_connected);
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
