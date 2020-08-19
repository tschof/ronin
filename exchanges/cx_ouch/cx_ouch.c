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
#include "rom_handler.h"
#include "rom2fix.h"
#include "mailer.h"
#include "cx_object.h"
#include "cx_ouch_helper.h"

static pthread_mutex_t connection_lock = PTHREAD_MUTEX_INITIALIZER;
static int cx_connected = 1;
static cx_obj *cx = 0;
static char *module_name = 0;
static int module_name_len = 0;
int get_cx_connected()
{
    int ret = 0;
    pthread_mutex_lock(&connection_lock);
    ret = cx_connected;
    pthread_mutex_unlock(&connection_lock);
    return ret;
}

void set_cx_connected(int is)
{
    pthread_mutex_lock(&connection_lock);
    cx_connected = is;
    pthread_mutex_unlock(&connection_lock);
}

void destroy_rom_queue(queue_t q);
int send_cx_logon(struct parse_args *pa);
void from_cx_func(struct message *mess, void *ob);
void send_cx_to_rom(char *message, int len, char *rom, int rom_len);
unsigned long parse_cx_ouch(const char *message,
                            unsigned long len,
                            int *cut_con, struct parse_con *pc);
void cx_connection_callBack(con_obj * co, int is_connected, char *log,
                            int len);
void con_callback(con_obj * co, int is_connected, char *log, int len);
void cx_lac(con_obj * co);
void cx_low_activity_pulse(con_obj * obj);
int main(int argc, char **argv)
{
    int ret;
    struct init_args *fix_args;
    ex_config *ex = NULL;
    struct tm *newtime;
    int keep_running = 1;
    queue_t rom_q;
    time_t long_time;
    rom_q = create_queue();
    ex = get_config("CXOUCHConfig.dat", 16, "OUCH SERVER", 11);
    fix_args = load_cx_config("CXOUCHConfig.dat", 16);
    load_rom_config(ex->dc, rom_q);

    module_name_len = ex->mod_name_len;
    module_name = calloc(module_name_len + 1, 1);
    memcpy(module_name, ex->mod_name, module_name_len);;
    cx = create_cx_obj(fix_args->username, fix_args->un_len,
                       fix_args->password, fix_args->p_len,
                       0, ex->end_hour, ex->end_min, 0,
                       module_name, module_name_len, 0, send_cx_to_rom);
    start_dart_mail_service(ex->mail_server, ex->mail_server_len,
                            ex->biz_group, ex->b_len, ex->tech_group,
                            ex->t_len, ex->end_hour, ex->end_min,
                            ex->end_day);
    run_cx_admin_loop(cx);
    ret =
        start_dart_client_services(&to_cx_func, ex->mod_name,
                                   ex->mod_name_len, ex->rom_port, rom_q,
                                   ex->rom_ip, ex->rom_ip_len,
                                   ex->end_hour, ex->end_min, ex->end_day,
                                   ex->heart_beat_int, cx, con_callback,
                                   parse_rom_message, build_offsets,
                                   cx_lac, create_rom_resend_pad, 0);
    ret =
        dart_connect(&from_cx_func, ex->mod_name, ex->mod_name_len,
                     ex->fix_port, ex->fix_ip, ex->fix_ip_len,
                     ex->end_hour, ex->end_min, ex->end_day,
                     ex->heart_beat_int, cx, cx_connection_callBack,
                     parse_cx_ouch, fix_args, send_cx_logon,
                     build_cx_offsets, cx_low_activity_pulse, 1);

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
            if (!get_cx_connected()) {
                set_cx_connected(1);
                ret =
                    dart_connect(&from_cx_func, ex->mod_name,
                                 ex->mod_name_len, ex->fix_port,
                                 ex->fix_ip, ex->fix_ip_len, ex->end_hour,
                                 ex->end_min, ex->end_day,
                                 ex->heart_beat_int, cx,
                                 cx_connection_callBack, parse_cx_ouch,
                                 fix_args, send_cx_logon, build_cx_offsets,
                                 cx_low_activity_pulse, 1);
            }
        }
    }
    destroy_cx_obj(cx);
    free_init_args(fix_args);
    destroy_ex_config(ex);
    destroy_rom_queue(rom_q);
    destroy_queue(rom_q);
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

void from_cx_func(struct message *mess, void *ob)
{
}

void send_cx_to_rom(char *message, int len, char *rom, int rom_len)
{
    struct message mess;
    mess.data = message;
    mess.name = rom;
    mess.name_len = rom_len;
    mess.len = len;
    cx_send_message_to_roms(&mess);
    dart_rolling_log_going(&mess, module_name, module_name_len);
}

unsigned long parse_cx_ouch(const char *message,
                            unsigned long len,
                            int *cut_con, struct parse_con *pc)
{
    return frame_cx_mess(cx, message, len, cut_con, pc);
}

void cx_connection_callBack(con_obj * co, int is_connected, char *log,
                            int len)
{
    printf("Info about this CX Connection: %s, Is it connected {%d} ? \n",
           get_name(co), is_connected);
    set_cx_connected(is_connected);
}

void con_callback(con_obj * co, int is_connected, char *log, int len)
{
    printf("Info about this Connection: %s, Is it connected {%d} ? \n",
           get_name(co), is_connected);
}

int send_cx_logon(struct parse_args *pc)
{
    set_cx_con_obj(cx, pc->con->co);
    send_seq_cx_message(cx, 'A', 0);
    return 0;
}

void cx_low_activity_pulse(con_obj * obj)
{
}

void cx_lac(con_obj * co)
{
}
