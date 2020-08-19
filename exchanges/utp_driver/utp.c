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
#include "rom_handler.h"
#include "config_loader.h"
#include "ex_common.h"
#include "nyse_seq_generator.h"
#include "ex_config_helper.h"
#include "dart_orderbook.h"
#include "mailer.h"
#include "utpdirect.h"
#include "utp_object.h"
#include "debug_logger.h"
#include "version.h"

int utp_is_connected = 0;

int get_is_utp_connected()
{
    return utp_is_connected;
}

void place_holder_from_utp(struct message *mess, void *ob)
{
}

void place_holder_from_rom_func(struct message *mess, void *b)
{
}

unsigned long build_utp_offsets(char *data, unsigned long size,
                                unsigned long byte_offset,
                                unsigned long *seq_num)
{
    return size;
}

void low_activity_sig(con_obj * co)
{
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

void utp_connection_callBack(con_obj * co, int is_connected,
                             char *log, int len)
{
    utp_is_connected = is_connected;
    char *name = (char *) get_name(co);
    send_debug_message
        ("Info about this FIX Connection: %s, Is it connected {%d} ? \n",
         name, is_connected);
    if (name) {
        free(name);
    }
}

void test_lac(con_obj * co)
{
    char *name = (char *) get_name(co);
    send_debug_message("Low activity reported on this connection: %s \n",
                       name);
    if (name) {
        free(name);
    }
}

void con_callBack(con_obj * co, int is_connected, char *log, int len)
{
    char *name = (char *) get_name(co);
    send_debug_message
        ("Info about this Connection: %s, Is it connected {%d} ? \n", name,
         is_connected);
    if (name) {
        free(name);
    }
}

int main(int argc, char **argv)
{
    struct init_args *fix_args;
    ex_config *ex = NULL;
    struct tm *newtime;
    int keep_running = 1;
    int ret = 0;
    queue_t rom_q;
    time_t long_time;
    orderbook *ob = NULL;
    rom_q = create_queue();
    ex = init_config("utp.dat", 14);
    fix_args = load_fix_config("utp.dat", 14);
    load_rom_config(ex->dc, rom_q);
    rom_parser_helper *rph = create_rph(ph_build_dart_obj);
    ob = init_utp("utp.dat", fix_args, ex, (&get_is_utp_connected));
    start_dart_mail_service(ex->mail_server, ex->mail_server_len,
                            ex->biz_group, ex->b_len, ex->tech_group,
                            ex->t_len, ex->end_hour, ex->end_min, 7);
    ret =
        start_dart_client_services_2(&place_holder_from_rom_func,
                                     ex->mod_name, ex->mod_name_len,
                                     ex->rom_port, rom_q, ex->rom_ip,
                                     ex->rom_ip_len, ex->end_hour,
                                     ex->end_min, 7, ex->heart_beat_int,
                                     ob, con_callBack,
                                     ph_parse_rom_message, build_offsets,
                                     test_lac, create_rom_resend_pad, 0,
                                     rph, copy_rom_parser_helper,
                                     utp_rom_helper_func);
    ret =
        dart_connect(&place_holder_from_utp, ex->mod_name,
                     ex->mod_name_len, ex->fix_port, ex->fix_ip,
                     ex->fix_ip_len, ex->end_hour, ex->end_min, 7,
                     ex->heart_beat_int, ob, utp_connection_callBack,
                     parse_utpdirect, fix_args, send_utp_logon,
                     build_utp_offsets, low_activity_sig, 1);

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
            if (!utp_is_connected) {
                utp_is_connected = 1;
                ret =
                    dart_connect(&place_holder_from_utp, ex->mod_name,
                                 ex->mod_name_len, ex->fix_port,
                                 ex->fix_ip, ex->fix_ip_len, ex->end_hour,
                                 ex->end_min, 7, ex->heart_beat_int, ob,
                                 utp_connection_callBack,
                                 parse_utpdirect, fix_args,
                                 send_utp_logon, build_utp_offsets,
                                 low_activity_sig, 1);
            }
        }
    }
    free_init_args(fix_args);
    destroy_ex_config(ex);
    destroy_rom_queue(rom_q);
    destroy_queue(rom_q);
    return 0;
}
