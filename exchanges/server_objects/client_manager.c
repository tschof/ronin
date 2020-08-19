/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <pthread.h>
#include "client_manager.h"
#include "hashmap.h"
#include "transaction_recorder.h"
#include "server_socket.h"
#include "connector.h"
#include "fix_rom_trans.h"
#include "dart_connection_object.h"
#include "tm_to_str.h"
#include "dbg_helper.h"
#include "debug_logger.h"
#include "async_server.h"
#include "version.h"
#include <unistd.h>

static pthread_mutex_t client_man_init = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t client_map_guard = PTHREAD_MUTEX_INITIALIZER;
static hashmap_t con_ptr;
static hashmap_t pa_map;
static con_obj *init_con_obj;
static int client_init = 0;

static void init_client_man()
{
    pthread_mutex_lock(&client_man_init);
    if (client_init) {
        //do nothing.
    } else {
        client_init = 1;
        con_ptr = create_map(MAXCONNECTIONS);
        pa_map = create_map(MAXCONNECTIONS);
        init_con_obj = create_empty_con_obj();
    }
    pthread_mutex_unlock(&client_man_init);
}

unsigned long gen_build_off(char *data, unsigned long size,
                            unsigned long byte_offset,
                            unsigned long *seq_num)
{
    return size;
}

char *gen_resend_pad(unsigned long seq, char *rom, size_t * len)
{
    return NULL;
}

static void reset_seq(struct init_args *in_args)
{
    printf("Resetting this: %s \n", in_args->name);
    sequence_req *sr =
        create_sequence_request(in_args->name, in_args->name_len,
                                in_args,
                                gen_build_off,
                                gen_resend_pad);
    clean_seq(sr);
}

/*@null@*/
struct init_args *copy_init_args(struct init_args *in)
{
    struct init_args *cp = malloc(sizeof(struct init_args));
    if (cp) {
        cp->name = calloc(in->name_len + 1, 1);
        if (cp->name) {
            memcpy(cp->name, in->name, in->name_len);
            cp->name_len = in->name_len;
        } else {
            cp->name_len = 0;
        }
        if (in->sc_len > 0) {
            cp->sender_comp = calloc(in->sc_len + 1, 1);
            if (cp->sender_comp) {
                memcpy(cp->sender_comp, in->sender_comp, in->sc_len);
                cp->sc_len = in->sc_len;
            } else {
                cp->sc_len = 0;
            }
        } else {
            cp->sc_len = 0;
            cp->sender_comp = NULL;
        }
        if (in->tc_len > 0) {
            cp->target_comp = calloc(in->tc_len + 1, 1);
            if (cp->target_comp) {
                memcpy(cp->target_comp, in->target_comp, in->tc_len);
                cp->tc_len = in->tc_len;
            } else {
                cp->tc_len = 0;
            }
        } else {
            cp->tc_len = 0;
            cp->target_comp = NULL;
        }
        if (in->ts_len > 0) {
            cp->target_sub = calloc(in->ts_len + 1, 1);
            if (cp->target_sub && in->target_sub && in->ts_len) {
                memcpy(cp->target_sub, in->target_sub, in->ts_len);
                cp->ts_len = in->ts_len;
            } else {
                cp->ts_len = 0;
            }
        } else {
            cp->ts_len = 0;
            cp->target_sub = NULL;
        }
        if (in->ss_len > 0) {
            cp->sender_sub = calloc(in->ss_len + 1, 1);
            if (cp->sender_sub && in->sender_sub && in->ss_len) {
                memcpy(cp->sender_sub, in->sender_sub, in->ss_len);
                cp->ss_len = in->ss_len;
            } else {
                cp->ss_len = 0;
            }
        } else {
            cp->sender_sub = NULL;
            cp->ss_len = 0;
        }
        if (in->un_len > 0) {
            cp->username = calloc(in->un_len + 1, 1);
            if (cp->username) {
                memcpy(cp->username, in->username, in->un_len);
                cp->un_len = in->un_len;
            } else {
                cp->un_len = 0;
            }
        } else {
            cp->un_len = 0;
            cp->username = NULL;
        }
        if (in->p_len > 0) {
            cp->password = calloc(in->p_len + 1, 1);
            if (cp->password) {
                memcpy(cp->password, in->password, in->p_len);
                cp->p_len = in->p_len;
            } else {
                cp->p_len = 0;
            }
        } else {
            cp->password = NULL;
            cp->p_len = 0;
        }
        if (in->fv_len > 0) {
            cp->fix_version = calloc(in->fv_len + 1, 1);
            if (cp->fix_version) {
                memcpy(cp->fix_version, in->fix_version, in->fv_len);
                cp->fv_len = in->fv_len;
            } else {
                cp->fv_len = 0;
            }
        } else {
            cp->fv_len = 0;
            cp->fix_version = NULL;
        }
        cp->reset_time = calloc(1,sizeof(struct seq_reset_time));
        if (cp->reset_time) {
            cp->reset_time->reset_hour = in->reset_time->reset_hour;
            cp->reset_time->reset_min = in->reset_time->reset_min;
            cp->reset_time->reset_day = in->reset_time->reset_day;
            cp->reset_time->create_new = in->reset_time->create_new;
            cp->reset_time->new_incoming = in->reset_time->new_incoming;
            cp->reset_time->new_outgoing = in->reset_time->new_outgoing;
            cp->reset_time->last_logon = in->reset_time->last_logon;
        }
        if (in->log_outgoing > 1 || in->log_outgoing < 0) {
            in->log_outgoing = 1;
        }
        if (in->cancel_only == 1) {
            cp->cancel_only = 1;
        } else {
            cp->cancel_only = 0;
        }
        cp->log_outgoing = in->log_outgoing;
        cp->create_mess_header = in->create_mess_header;
        if (cp->log_outgoing && cp->reset_time->create_new) {
            reset_seq(cp);
            cp->reset_time->create_new = 0;
            in->reset_time->create_new = 0;
        }
        return cp;
    } else {
        return NULL;
    }
}

int compare_times(int h, int m)
{
    time_t rawtime;
    struct tm *ti;
    time(&rawtime);
    struct tm lm;
    ti = localtime_r(&rawtime, &lm);
    ti = &lm;
    if (ti->tm_hour == h && (ti->tm_min >= m && (m + 5) >= ti->tm_min)) {
        return 1;
    } else {
        return 0;
    }
}

int dart_connect_2(void (*client_callback) (struct message * m,
                                            void *ob),
                   const char *mod_name, size_t mod_len, uint16_t port,
                   const char *ip, size_t ip_len, int end_hour,
                   int end_min, int end_day, size_t timeout_val, void *ob,
                   void (*connection_callback) (con_obj * co,
                                                int is_connected,
                                                char *log, int len),
                   unsigned long (*client_mess_parse) (const char *message,
                                                       unsigned long len,
                                                       int *cut_con,
                                                       struct parse_con *
                                                       pc),
                   struct init_args *args,
                   int (*init_con) (struct parse_args * pa),
                   unsigned long (*seq_parse) (char *data,
                                               unsigned long size,
                                               unsigned long bo,
                                               unsigned long *seq),
                   void (*low_activity_pulse) (con_obj * obj),
                   int check_activity, void *parser_helper,
                   void *(*copy_parser_helper) (void *ph),
                   void (*p_callback) (void *ph, struct message * m,
                                       void *ob))
{
    struct rec_init *rec;
    struct parse_con *pcon;
    con_obj *co;
    struct parse_args *pa;
    struct client_con_set *ccs;
    queue_t in_q;
    struct init_args *l_args;
    app_end_time *t = malloc(sizeof(struct app_end_time));
    if (ip == NULL || args == NULL || client_callback == NULL ||
        connection_callback == NULL || client_mess_parse == NULL ||
        init_con == NULL || seq_parse == NULL
        || low_activity_pulse == NULL) {
        return -1;
    }

    if (args->log_outgoing > 1 || args->log_outgoing < 0) {
        args->log_outgoing = 1;
    } else if (args->log_outgoing == 0) {
        printf("Not logging outgoing!!! %d\n", args->log_outgoing);
    }
    init_client_man();
    rec = malloc(sizeof(struct rec_init));
    if (rec == NULL) {
        return -1;
    }
    t->end_hour = end_hour;
    t->end_min = end_min;
    t->end_day = end_day;
    rec->create_new = args->reset_time->create_new;
    rec->timeout_value = timeout_val;
    rec->t = t;
    rec->def_name = 0;
    rec->def_name_len = 0;
    start_recorder(rec);
    free(rec);
    pcon = malloc(sizeof(struct parse_con));
    if (pcon == NULL) {
        return -1;
    }
    pcon->send_ptr = send_message;
    pcon->callback = (*client_callback);
    pcon->record_mess = record_inbound_message;
    pcon->init_con = add_connection;
    pcon->remove_con = remove_connection;
    pcon->connection_notice = connection_callback;
    pcon->timeout_val = timeout_val;
    pcon->is_logged_on = 0;
    pcon->book = ob;
    pcon->t = t;
    pcon->parser_helper = parser_helper;
    pcon->copy_parser_helper = copy_parser_helper;
    pcon->p_callback = p_callback;
    if (mod_len > 0 && mod_len < 100) {
        pcon->mod_name = calloc(mod_len + 1, 1);
        memcpy(pcon->mod_name, mod_name, mod_len);
        pcon->mod_len = mod_len;
    } else {
        pcon->mod_len = 0;
        pcon->mod_name = NULL;
    }
    in_q = create_sized_queue(80);
        send_debug_message("Connect to This: %s, Create new? %d Log outgoing? %d Reset_addr: %d \n",
                               args->name, args->reset_time->create_new, args->log_outgoing, args->reset_time);
    l_args = copy_init_args(args);
    enqueue(in_q, l_args);
    pcon->in_args_list = in_q;
    pcon->in_args = l_args;
    co = create_con_obj(l_args->name, l_args->name_len, -1, 0, 1, 1);
    if (co == NULL) {
        free(pcon);
        return -1;
    }
    pcon->co = co;
    pa = malloc(sizeof(struct parse_args));
    if (pa == NULL) {
        destroy_con_obj(co);
        free(pcon);
        return -1;
    }
    pa->parse = (*client_mess_parse);
    pa->seq_parse = (*seq_parse);
    pa->low_activity_pulse = (*low_activity_pulse);
    pa->t = t;
    pa->con = pcon;
    pa->check_activity = check_activity;
    add_config(pcon->in_args);
    ccs = malloc(sizeof(struct client_con_set));
    if (ccs == NULL) {
        destroy_con_obj(co);
        free(pcon);
        return -1;
    }
    ccs->pa = pa;
    ccs->port = port;
    ccs->host = calloc(ip_len + 1, 1);
    memcpy((char *) ccs->host, ip, ip_len);
    ccs->host_len = ip_len;
    ccs->init_connection = (*init_con);
    if (ip_len > 0 && (memchr(ccs->host, '.', ip_len) == 0)) {
        ccs->use_ud = 1;
    } else {
        ccs->use_ud = 0;
    }
    connect_dart(ccs);
    return 0;
}

int dart_connect(void (*client_callback) (struct message * m,
                                          void *ob),
                 const char *mod_name, size_t mod_len, uint16_t port,
                 const char *ip, size_t ip_len, int end_hour,
                 int end_min, int end_day, size_t timeout_val, void *ob,
                 void (*connection_callback) (con_obj * co,
                                              int is_connected,
                                              char *log, int len),
                 unsigned long (*client_mess_parse) (const char *message,
                                                     unsigned long len,
                                                     int *cut_con,
                                                     struct parse_con *
                                                     pc),
                 struct init_args *args,
                 int (*init_con) (struct parse_args * pa),
                 unsigned long (*seq_parse) (char *data,
                                             unsigned long size,
                                             unsigned long bo,
                                             unsigned long *seq),
                 void (*low_activity_pulse) (con_obj * obj),
                 int check_activity)
{
    return dart_connect_2(client_callback, mod_name, mod_len, port,
                          ip, ip_len, end_hour, end_min, end_day,
                          timeout_val, ob, connection_callback,
                          client_mess_parse, args, init_con,
                          seq_parse, low_activity_pulse, check_activity,
                          NULL, NULL, NULL);
}

queue_t set_parse_args_from_list(queue_t args,
                                        char *(*create_reset_pad) (unsigned
                                                                   long
                                                                   seq,
                                                                   char
                                                                   *ver,
                                                                   size_t *
                                                                   len))
{
    element_t e;
    queue_t ret_val;
    ret_val = create_sized_queue(80);
    e = front(args);
    while (e != NULL) {
        struct init_args *init;
        init = (struct init_args *) e->d;
        if (init != NULL) {
            struct init_args *l_args = copy_init_args(init);
            enqueue(ret_val, l_args);
            add_config(l_args);
            e = e->next;
        } else {
            printf
                ("Client Manager set parse args. "
                 "Struct was null element was not:\n");
            e = e->next;
        }
    }
    return ret_val;
}

int start_dart_client_services(void (*fn_ptr) (struct message * m,
                                               void *ob),
                               const char *mod_name, size_t mod_len,
                               uint16_t port, queue_t args, const char *ip,
                               size_t ip_len, int end_hour,
                               int end_min, int end_day,
                               size_t timeout_val, void *ob,
                               void (*connection_callback) (con_obj * co,
                                                            int
                                                            is_connected,
                                                            char *log,
                                                            int len),
                               unsigned long (*mess_parse) (const char
                                                            *message,
                                                            unsigned long
                                                            len,
                                                            int *cut_con,
                                                            struct
                                                            parse_con *
                                                            pc),
                               unsigned long (*seq_parse) (char *data,
                                                           unsigned long
                                                           size,
                                                           unsigned long
                                                           bo,
                                                           unsigned long
                                                           *seq),
                               void (*low_activity_pulse) (con_obj * obj),
                               char *(*create_reset_pad) (unsigned long
                                                          seq, char *ver,
                                                          size_t * len),
                               int check_activity)
{
    return start_dart_client_services_2(fn_ptr, mod_name, mod_len, port,
                                        args, ip, ip_len, end_hour,
                                        end_min, end_day, timeout_val, ob,
                                        connection_callback, mess_parse,
                                        seq_parse, low_activity_pulse,
                                        create_reset_pad, check_activity,
                                        NULL, NULL, NULL);
}

int start_dart_client_services_2(void (*fn_ptr) (struct message * m,
                                                 void *ob),
                                 const char *mod_name, size_t mod_len,
                                 uint16_t port, queue_t args,
                                 const char *ip, size_t ip_len,
                                 int end_hour, int end_min, int end_day,
                                 size_t timeout_val, void *ob,
                                 void (*connection_callback) (con_obj * co,
                                                              int
                                                              is_connected,
                                                              char *log,
                                                              int len),
                                 unsigned long (*mess_parse) (const char
                                                              *message,
                                                              unsigned long
                                                              len,
                                                              int *cut_con,
                                                              struct
                                                              parse_con *
                                                              pc),
                                 unsigned long (*seq_parse) (char *data,
                                                             unsigned long
                                                             size,
                                                             unsigned long
                                                             bo,
                                                             unsigned long
                                                             *seq),
                                 void (*low_activity_pulse) (con_obj *
                                                             obj),
                                 char *(*create_reset_pad) (unsigned long
                                                            seq, char *ver,
                                                            size_t * len),
                                 int check_activity, void *parser_helper,
                                 void *(*copy_parser_helper) (void *ph),
                                 void (*p_callback) (void *ph,
                                                     struct message * m,
                                                     void *ob))
{
    struct rec_init *rec;
    struct parse_con *pcon;
    con_obj *co;
    struct parse_args *pa = NULL;
    struct server_con_set *scs;
    queue_t l_args;
    int rec_ret = 0;
    app_end_time *t = malloc(sizeof(struct app_end_time));
    if (fn_ptr == NULL || connection_callback == NULL ||
        mess_parse == NULL || seq_parse == NULL
        || low_activity_pulse == NULL || args == NULL) {
        return -1;
    }
    init_client_man();
    rec = malloc(sizeof(struct rec_init));
    if (rec == NULL) {
        return -1;
    }
    t->end_hour = end_hour;
    t->end_min = end_min;
    t->end_day = end_day;
    rec->t = t;
    rec->create_new = 0;
    rec->timeout_value = timeout_val;
    rec_ret = start_recorder(rec);
    free(rec);
    l_args = set_parse_args_from_list(args, (*create_reset_pad));

    pcon = malloc(sizeof(struct parse_con));
    if (pcon == NULL) {
        return -1;
    }
    pcon->send_ptr = send_message;
    pcon->callback = (*fn_ptr);
    pcon->record_mess = record_inbound_message;
    pcon->init_con = add_connection;
    pcon->remove_con = remove_connection;
    pcon->connection_notice = connection_callback;
    pcon->timeout_val = timeout_val;
    pcon->is_logged_on = 0;
    pcon->in_args_list = l_args;
    pcon->in_args = NULL;
    pcon->is_server = 1;
    pcon->t = t;
    pcon->parser_helper = parser_helper;
    pcon->copy_parser_helper = copy_parser_helper;
    pcon->p_callback = p_callback;
    if (mod_len > 0 && mod_len < 100) {
        pcon->mod_name = calloc(mod_len + 1, 1);
        memcpy(pcon->mod_name, mod_name, mod_len);
        pcon->mod_len = mod_len;
    } else {
        pcon->mod_len = 0;
        pcon->mod_name = NULL;
    }
    co = create_empty_con_obj();
    if (co == NULL) {
        free(pcon);
        return -1;
    }
    pcon->book = ob;
    pcon->co = co;

    pa = malloc(sizeof(struct parse_args));
    if (pa == NULL) {
        destroy_con_obj(co);
        free(pcon);
        return -1;
    }
    pa->parse = (*mess_parse);
    pa->seq_parse = (*seq_parse);
    pa->low_activity_pulse = (*low_activity_pulse);
    pa->t = t;
    pa->con = pcon;
    pa->check_activity = check_activity;

    scs = malloc(sizeof(struct server_con_set));
    if (scs == NULL) {
        destroy_con_obj(co);
        free(pcon);
        free(pa);
        return -1;
    }
    scs->pa = pa;
    scs->port = port;
    if (ip == NULL) {
        scs->ip = NULL;
        scs->ip_len = 0;
    } else {
        scs->ip = calloc(ip_len + 1, 1);
        memcpy(scs->ip, ip, ip_len);
        scs->ip_len = ip_len;
    }
    if (ip_len > 0 && (memchr(scs->ip, '.', ip_len) == 0)) {
        start_ud_binder((void *) scs);
        scs->use_ud = 1;
    } else {
        start_server_socket((void *) scs);
        scs->use_ud = 0;
    }

    return 0;
}

async_server_con_set *async_start_dart_client_services_2(void (*fn_ptr)
                                                         (struct message *
                                                          m, void *ob),
                                                         const char
                                                         *mod_name,
                                                         size_t mod_len,
                                                         uint16_t port,
                                                         queue_t args,
                                                         const char *ip,
                                                         size_t len,
                                                         int end_hour,
                                                         int end_min,
                                                         int end_day,
                                                         size_t
                                                         timeout_val,
                                                         void *ob,
                                                         void
                                                         (*connection_callback)
                                                         (con_obj * co,
                                                          int is_connected,
                                                          char *log,
                                                          int len),
                                                         unsigned
                                                         long (*mess_parse)
                                                         (const char
                                                          *message,
                                                          unsigned long
                                                          len,
                                                          int *cut_con,
                                                          struct parse_con
                                                          * pc),
                                                         unsigned
                                                         long (*seq_parse)
                                                         (char *data,
                                                          unsigned long
                                                          size,
                                                          unsigned long bo,
                                                          unsigned long
                                                          *seq),
                                                         void
                                                         (*low_activity_pulse)
                                                         (con_obj * obj),
                                                         char
                                                         *
                                                         (*create_reset_pad)
                                                         (unsigned long
                                                          seq, char *ver,
                                                          size_t * len),
                                                         int
                                                         check_activity,
                                                         int num_threads,
                                                         int time_out,
                                                         int *cpus,
                                                         int set_affinity,
                                                         void
                                                         *write_pass_back,
                                                         void
                                                         (*write_callback)
                                                         (const char *name,
                                                          int name_len,
                                                          void *pass_back),
                                                         void
                                                         *parser_helper,
                                                         void
                                                         *copy_parser_helper
                                                         (void *ph),
                                                         void (*p_callback)
                                                          (void *ph,
                                                           struct message *
                                                           m, void *ob))
{
    struct rec_init *rec;
    queue_t l_args;
    int rec_ret = 0;
    app_end_time *t = malloc(sizeof(struct app_end_time));
    if (fn_ptr == NULL || connection_callback == NULL ||
        mess_parse == NULL || seq_parse == NULL
        || low_activity_pulse == NULL || args == NULL) {
        return NULL;
    }
    init_client_man();
    rec = malloc(sizeof(struct rec_init));
    if (rec == NULL) {
        return NULL;
    }
    t->end_hour = end_hour;
    t->end_min = end_min;
    t->end_day = end_day;
    rec->t = t;
    rec->create_new = 0;
    rec->timeout_value = timeout_val;
    rec_ret = start_recorder(rec);
    free(rec);
    l_args = set_parse_args_from_list(args, (*create_reset_pad));
    return start_async_server((*fn_ptr), mod_name, mod_len, port, l_args,
                              ip, len, end_hour, end_min, end_day,
                              timeout_val, ob, (*connection_callback),
                              (*mess_parse), (*seq_parse),
                              (*low_activity_pulse), (*create_reset_pad),
                              check_activity, num_threads, time_out, cpus,
                              set_affinity, write_pass_back,
                              (*write_callback), parser_helper,
                              (*copy_parser_helper), (*p_callback));
}

async_server_con_set *async_start_dart_client_services(void (*fn_ptr)
                                                       (struct message *
                                                        m, void *ob),
                                                       const char
                                                       *mod_name,
                                                       size_t mod_len,
                                                       uint16_t port,
                                                       queue_t args,
                                                       const char *ip,
                                                       size_t len,
                                                       int end_hour,
                                                       int end_min,
                                                       int end_day,
                                                       size_t timeout_val,
                                                       void *ob,
                                                       void
                                                       (*connection_callback)
                                                       (con_obj * co,
                                                        int is_connected,
                                                        char *log,
                                                        int len),
                                                       unsigned
                                                       long (*mess_parse)
                                                       (const char
                                                        *message,
                                                        unsigned long len,
                                                        int *cut_con,
                                                        struct parse_con *
                                                        pc),
                                                       unsigned
                                                       long (*seq_parse)
                                                       (char *data,
                                                        unsigned long
                                                        size,
                                                        unsigned long bo,
                                                        unsigned long
                                                        *seq),
                                                       void
                                                       (*low_activity_pulse)
                                                       (con_obj * obj),
                                                       char
                                                       *(*create_reset_pad)
                                                       (unsigned long seq,
                                                        char *ver,
                                                        size_t * len),
                                                       int check_activity,
                                                       int num_threads,
                                                       int time_out,
                                                       int *cpus,
                                                       int set_affinity,
                                                       void
                                                       *write_pass_back,
                                                       void
                                                       (*write_callback)
                                                       (const char *name,
                                                        int name_len,
                                                        void *pass_back))
{
    return async_start_dart_client_services_2(fn_ptr, mod_name, mod_len,
                                              port, args, ip, len,
                                              end_hour, end_min, end_day,
                                              timeout_val, ob,
                                              connection_callback,
                                              mess_parse, seq_parse,
                                              low_activity_pulse,
                                              create_reset_pad,
                                              check_activity, num_threads,
                                              time_out, cpus, set_affinity,
                                              write_pass_back,
                                              write_callback, NULL, NULL,
                                              NULL);
}

int async_dart_connect_2(void (*client_callback) (struct message * m,
                                                  void *ob),
                         const char *mod_name, size_t mod_len,
                         uint16_t port, const char *ip, size_t ip_len,
                         int end_hour, int end_min, int end_day,
                         size_t timeout_val, void *ob,
                         void (*connection_callback) (con_obj * co,
                                                      int is_connected,
                                                      char *log, int len),
                         unsigned long (*client_mess_parse) (const char
                                                             *message,
                                                             unsigned long
                                                             len,
                                                             int *cut_con,
                                                             struct
                                                             parse_con *
                                                             pc),
                         struct init_args *args,
                         int (*init_con) (struct async_parse_args * pa),
                         unsigned long (*seq_parse) (char *data,
                                                     unsigned long size,
                                                     unsigned long bo,
                                                     unsigned long *seq),
                         void (*low_activity_pulse) (con_obj * obj),
                         int check_activity, async_server_con_set * ascs,
                         void *write_pass_back,
                         void (*write_callback) (const char *name,
                                                 int name_len,
                                                 void *pass_back),
                         void *parser_helper,
                         void *(*copy_parser_helper) (void *ph),
                         void (*p_callback) (void *ph, struct message * m,
                                             void *ob))
{
    struct rec_init *rec;
    struct init_args *l_args;
    app_end_time *t = malloc(sizeof(struct app_end_time));
    if (ip == NULL || args == NULL || client_callback == NULL ||
        connection_callback == NULL || client_mess_parse == NULL ||
        init_con == NULL || seq_parse == NULL
        || low_activity_pulse == NULL) {
        return -1;
    }

    if (args->log_outgoing > 1 || args->log_outgoing < 0) {
        args->log_outgoing = 1;
    } else if (args->log_outgoing == 0) {
    }
    init_client_man();
    rec = malloc(sizeof(struct rec_init));
    if (rec == NULL) {
        return -1;
    }
    t->end_hour = end_hour;
    t->end_min = end_min;
    t->end_day = end_day;
    rec->create_new = args->reset_time->create_new;
    rec->timeout_value = timeout_val;
    rec->t = t;
    start_recorder(rec);
    free(rec);
    send_debug_message("Connect to This: %s, Create new? %d \n",
                               args->name, args->reset_time->create_new);
    l_args = copy_init_args(args);
    return async_connection(ascs, (*client_callback), mod_name, mod_len,
                            port, ip, ip_len, end_hour, end_min, end_day,
                            timeout_val, ob, (*connection_callback),
                            (*client_mess_parse), l_args, (*init_con),
                            (*seq_parse), (*low_activity_pulse),
                            check_activity, write_pass_back,
                            (*write_callback), parser_helper,
                            (*copy_parser_helper), (*p_callback));
}

int async_dart_connect(void (*client_callback) (struct message * m,
                                                void *ob),
                       const char *mod_name, size_t mod_len, uint16_t port,
                       const char *ip, size_t ip_len, int end_hour,
                       int end_min, int end_day, size_t timeout_val,
                       void *ob, void (*connection_callback) (con_obj * co,
                                                              int
                                                              is_connected,
                                                              char *log,
                                                              int len),
                       unsigned long (*client_mess_parse) (const char
                                                           *message,
                                                           unsigned long
                                                           len,
                                                           int *cut_con,
                                                           struct parse_con
                                                           * pc),
                       struct init_args *args,
                       int (*init_con) (struct async_parse_args * pa),
                       unsigned long (*seq_parse) (char *data,
                                                   unsigned long size,
                                                   unsigned long bo,
                                                   unsigned long *seq),
                       void (*low_activity_pulse) (con_obj * obj),
                       int check_activity, async_server_con_set * ascs,
                       void *write_pass_back,
                       void (*write_callback) (const char *name,
                                               int name_len,
                                               void *pass_back))
{
    return async_dart_connect_2(client_callback, mod_name, mod_len, port,
                                ip, ip_len, end_hour, end_min, end_day,
                                timeout_val, ob, connection_callback,
                                client_mess_parse, args, init_con,
                                seq_parse, low_activity_pulse,
                                check_activity, ascs, write_pass_back,
                                write_callback, NULL, NULL, NULL);
}

void async_kick_off_connection(async_server_con_set * ascs, char *name,
                               size_t name_len)
{
    con_obj *temp = init_con_obj;
    int res;
    if (pthread_mutex_lock(&client_map_guard) == 0) {
        res = get_map_entry(con_ptr, name, name_len, (void *) &temp);
        if (res > 0) {
            remove_from_map(con_ptr, name, name_len);
            set_disconnect(temp, 1);
            set_removed(temp, 1);
            pthread_mutex_unlock(&client_map_guard);
            disconnect_by_name(ascs, name, name_len, get_sock(temp));
        } else {
            pthread_mutex_unlock(&client_map_guard);
        }
    }
}

int queue_async_send(async_server_con_set * set, const char *name,
                     int name_len, void *pb)
{
    con_obj *l_obj = init_con_obj;
    int res = 0;
    int return_code;
    return_code = 0;
    if (pthread_mutex_lock(&client_map_guard) == 0) {
        res = get_map_entry(con_ptr, name, name_len, (void *) &l_obj);
    }
    if (pthread_mutex_unlock(&client_map_guard) != 0) {
        printf("failed to unlock client map guard.\n");
    }
    if (res > 0 && get_disconnect(l_obj) == 0) {
        return_code = 1;
        add_sender(set, pb, get_sock(l_obj));

    } else {
        printf("Could not find connection for this: %s \n", name);
        return_code = -1;
    }
    return return_code;

}

int linux_only_dart_send(const char *name, int name_len, const char *data,
                         size_t len)
{
    con_obj *l_obj = init_con_obj;
    int res = 0;
    int return_code = 0;
    int sock = -1;
    if (pthread_mutex_lock(&client_map_guard) == 0) {
        res = get_map_entry(con_ptr, name, name_len, (void *) &l_obj);
        if (res > 0 && get_disconnect(l_obj) == 0) {
            sock = get_sock(l_obj);
        } else {
            return_code = -1;
        }
        pthread_mutex_unlock(&client_map_guard);
        if (sock != -1) {
            int error;
            return_code =
                send(sock, data, len, MSG_DONTWAIT | MSG_NOSIGNAL);
            if (return_code < 0) {
                error = errno;
                if (error == EAGAIN || error == EWOULDBLOCK
                    || error == ENOBUFS || error == EINTR || error == 0) {
                    return_code = 0;
                } else {
                    return_code = -1;
                    set_disconnect(l_obj, 1);
                }
            }
        }
    }
    return return_code;
}

int get_sock_for_con(const char *name, int name_len)
{

    con_obj *l_obj = init_con_obj;
    int res = 0;
    int sock = -1;
    if (pthread_mutex_lock(&client_map_guard) == 0) {
        res = get_map_entry(con_ptr, name, name_len, (void *) &l_obj);
        if (res > 0 && get_disconnect(l_obj) == 0) {
            sock = get_sock(l_obj);
        } else {
            send_debug_message("Could not find connection for this: %s \n",
                               name);
        }
        if (pthread_mutex_unlock(&client_map_guard) != 0) {
            send_debug_message
                ("failed to unlock client map guard.  For this: %s \n",
                 name);
        }
    }
    return sock;
}

int send_bunches(struct iovec *iov, int iovcnt, int sock, int *err,
                 int *vec_sent)
{
    ssize_t ret_code = 0;
    size_t bytes_written = 0;
    struct iovec *iov_end = iov + iovcnt;
    int num_sent = 0;
    while (iovcnt > 0 && (ret_code = writev(sock, iov, iovcnt)) >= 0) {
        bytes_written += (size_t) ret_code;
        while (iov != iov_end) {
            if (iov->iov_len <= ret_code) {
                ret_code -= iov->iov_len;
                ++iov;
                --iovcnt;
                ++num_sent;
            } else {
                iov->iov_len -= ret_code;
                iov->iov_base += ret_code;
                break;
            }
        }
    }
    if (ret_code < 0) {
        int error = errno;
        if (error == EAGAIN || error == EWOULDBLOCK || error == ENOBUFS
            || error == EINTR || error == 0) {
            *err = 0;
        } else {
            *err = -1;
        }
    } else {
        *err = 1;
    }
    *vec_sent = num_sent;
    return bytes_written;
}

int dart_send_message(con_obj * co, char *data, size_t len)
{
    int return_code = -1;
    int error;
    return_code =
        send(get_sock(co), data, len, MSG_DONTWAIT | MSG_NOSIGNAL);
    if (return_code < 0) {
        error = errno;
        if (error == EAGAIN || error == EWOULDBLOCK || error == ENOBUFS
            || error == EINTR || error == 0) {
            return_code = 0;
        } else {
            send_debug_message
                ("Got this error on send: %d , we will now disconnect.\n",
                 error);
            return_code = -1;
            set_disconnect(co, 1);
        }
    }
    return return_code;
}

int send_message(struct message *mess)
{
    con_obj *l_obj = init_con_obj;
    int res = 0;
    int return_code;
    return_code = 0;
    if (pthread_mutex_lock(&client_map_guard) == 0) {
        res =
            get_map_entry(con_ptr, mess->name, mess->name_len,
                          (void *) &l_obj);
    }
    if (pthread_mutex_unlock(&client_map_guard) != 0) {
        printf("failed to unlock client map guard.\n");
    }
    if (res > 0 && get_disconnect(l_obj) == 0) {
        return_code = dart_send_message(l_obj, mess->data, mess->len);
    } else {
        send_debug_message("Could not find connection for this: %s \n",
                           mess->name);
        return_code = -1;
    }
    return return_code;
}

void record_sent_message(const struct message *mess)
{
    struct message *temp;
    temp = malloc(sizeof(struct message));
    if (temp != NULL) {
        temp->data = calloc(mess->len + 1, 1);
        memcpy(temp->data, mess->data, mess->len);
        temp->name = calloc(mess->name_len + 1, 1);
        memcpy(temp->name, mess->name, mess->name_len);
        temp->name_len = mess->name_len;
        temp->len = mess->len;
        record_outbound_message(temp);
    }
}

void record_sent_message_iovec(struct iovec *iov, int iovcnt,
                               const char *name, int name_len)
{
    struct message *temp;
    temp = malloc(sizeof(struct message));
    if (temp != NULL) {
        temp->data = calloc(500, 1);
        temp->len = 0;
        char *d_off = temp->data;
        struct iovec *l_iov = iov;
        while (l_iov) {
            memcpy(d_off, l_iov->iov_base, l_iov->iov_len);
            temp->len += l_iov->iov_len;
            d_off += l_iov->iov_len;
            ++l_iov;
        }
        temp->name = calloc(name_len + 1, 1);
        memcpy(temp->name, name, name_len);
        temp->name_len = name_len;
        record_outbound_message(temp);
    }
}

static int set_time_stamp(char *mblk, size_t nlen)
{
    int retval = -1;
    struct timeval now;
    gettimeofday(&now, 0);
    struct tm lm;
    if (gmtime_r(&now.tv_sec, &lm) != 0
        && tm_to_str_micros(&lm, now.tv_usec, mblk, nlen) == 0) {
        retval = TIMESTAMP_LEN_WITH_MICROS;
    }
    return retval;
}

static void send_to_rolling_log(const struct message *m, char *d,
                                size_t d_len, const char *log_name,
                                size_t log_name_len)
{
    struct rolling_log_message *m1 =
        malloc(sizeof(struct rolling_log_message));
    char *d_off = NULL;
    int s_len = 0;
    size_t t_len = 0;
    if (m1 != NULL) {
        const size_t ndatalen = m->len + 100 + m->name_len + d_len;
        m1->data = calloc(ndatalen, 1);
        d_off = m1->data;
        m1->name = calloc(m->name_len + 1, 1);
        m1->name_len = m->name_len;
        s_len = set_time_stamp(d_off, ndatalen);
        if (s_len > 0) {
            d_off += s_len;
            t_len += s_len;
        }
        memcpy(d_off, d, d_len);
        d_off += d_len;
        t_len += d_len;
        memcpy(d_off, m->name, m->name_len);
        d_off += m->name_len;
        t_len += m->name_len;
        memcpy(d_off, "  ", 2);
        d_off += 2;
        t_len += 2;
        memcpy(d_off, m->data, m->len);
        t_len += m->len;
        memcpy(m1->name, m->name, m->name_len);
        m1->len = t_len;
        m1->file_name = calloc(log_name_len + 1, 1);
        memcpy(m1->file_name, log_name, log_name_len);
        m1->file_name_len = log_name_len;
        dart_rolling_log(m1);
    }
}

void dart_rolling_log_going_iovec(struct iovec *iov, int iovcnt,
                                  const char *name, int name_len,
                                  const char *log_name, int log_name_len)
{
    struct rolling_log_message *logmsg = malloc(sizeof *logmsg);
    logmsg->name = strndup(name, name_len);
    logmsg->name_len = name_len;
    struct databuf *data = databuf_alloc(512);
    int timestamplength =
        set_time_stamp(data->wr_ptr, databuf_space_left(data));
    if (timestamplength > -1)
        data->wr_ptr += timestamplength;
    databuf_write(data, "--To-- %.*s  ", name_len, name);
    int i;
    for (i = 0; i < iovcnt; ++i)
        databuf_memcpy(data, iov[i].iov_base, iov[i].iov_len);
    /* Use the buffer allocated by the databuffer. */
    logmsg->data = data->buffer;
    logmsg->len = databuf_unread(data);
    logmsg->file_name = strndup(log_name, log_name_len);
    logmsg->file_name_len = log_name_len;
    /* Now that we are done with the databuf and the underlying buffer
     * will be disposed of in the transaction recorder thread, we
     * simply need to get rid of the databuf structure that is also
     * allocated on the heap. */
    free(data);
    dart_rolling_log(logmsg);
}

void dart_rolling_log_coming(const struct message *mess,
                             const char *log_name, size_t log_name_len)
{
    send_to_rolling_log(mess, "--From-- ", 9, log_name, log_name_len);
}

void dart_rolling_log_going(const struct message *mess,
                            const char *log_name, size_t log_name_len)
{
    send_to_rolling_log(mess, "--To-- ", 7, log_name, log_name_len);
}

int check_is_connected(con_obj * co)
{
    int res = 0;
    if (pthread_mutex_lock(&client_map_guard) == 0) {
        char *name = (char *) get_name(co);
        res = contains_key(con_ptr, name, get_name_len(co));
        if (res > 0) {
            set_disconnect(co, 1);
            set_removed(co, 1);
        }
        free(name);
    }
    if (pthread_mutex_unlock(&client_map_guard) != 0) {
        printf
            ("failed to unlock client map guard mutex in add connection.\n");
    }
    return res;
}

int add_connection(con_obj * co)
{
    con_obj *temp = init_con_obj;
    int res;
    if (pthread_mutex_lock(&client_map_guard) == 0) {
        char *name = (char *) get_name(co);
        temp = NULL;
        res =
            get_map_entry(con_ptr, name, get_name_len(co), (void *) &temp);
        if (res > 0) {
            set_disconnect(temp, 1);
            remove_from_map(con_ptr, name, get_name_len(co));
            set_removed(temp, 1);
        }
        no_copy_insert(con_ptr, name, get_name_len(co), co);
        free(name);
    }
    if (pthread_mutex_unlock(&client_map_guard) != 0) {
        printf
            ("failed to unlock client map guard mutex in add connection.\n");
    }
    return 0;
}

struct init_args *find_config(char *name, size_t name_len)
{
    struct init_args stack_pa;
    struct init_args *pa = &stack_pa;
    int res;
    if (pthread_mutex_lock(&client_map_guard) == 0) {
        pa = NULL;
        res = get_map_entry(pa_map, name, name_len, (void *) &pa);
        if (res <= 0) {
            printf("Could not find pa struct for: %s\n", name);
            pa = NULL;
        }
        if (pthread_mutex_unlock(&client_map_guard) != 0) {
            printf("failed to unlock cmg in find_config.\n");
        }
    }
    return pa;
}

void kick_off_connection(char *name, size_t name_len)
{
    con_obj *temp = init_con_obj;
    int res;
    if (pthread_mutex_lock(&client_map_guard) == 0) {
        temp = NULL;
        res = get_map_entry(con_ptr, name, name_len, (void *) &temp);
        if (res > 0) {
            set_disconnect(temp, 1);
            remove_from_map(con_ptr, name, name_len);
            set_removed(temp, 1);
        }
    }
    if (pthread_mutex_unlock(&client_map_guard) != 0) {
        printf("unable to unlock cmg in kick off connection.\n");
    }
}

void remove_connection(con_obj * co)
{
    if (pthread_mutex_lock(&client_map_guard) == 0) {
        if (get_name_len(co) > 0) {
            char *name = (char *) get_name(co);
            remove_from_map(con_ptr, name, get_name_len(co));
            set_removed(co, 1);
            free(name);
        } else {
            printf
                ("Received a null name for a connection we wished to remove.\n");
        }
    }
    if (pthread_mutex_unlock(&client_map_guard) != 0) {
        printf("Unable to unlock mutex in remove connection.\n");
    }
}

void add_config(struct init_args *pa)
{
    struct init_args stack_tep;
    struct init_args *temp = &stack_tep;
    int res;
    if (pa->name_len > 0) {
        if (pthread_mutex_lock(&client_map_guard) == 0) {
            temp = NULL;
            res =
                get_map_entry(pa_map, pa->name, pa->name_len,
                              (void *) &temp);
            if (res > 0) {
                remove_from_map(pa_map, pa->name, pa->name_len);
            }
            no_copy_insert(pa_map, pa->name, pa->name_len, pa);
        }
        if (pthread_mutex_unlock(&client_map_guard) != 0) {
            printf("Unable to unlock cmg in add config. \n");
        }
    } else {
        printf("Name is invalid on add_config.\n");
    }
}

int remove_config(char *name, size_t name_len)
{
    int ret_code;
    ret_code = -1;
    if (pthread_mutex_lock(&client_map_guard) == 0) {
        ret_code = remove_from_map(pa_map, name, name_len);
    }
    if (pthread_mutex_unlock(&client_map_guard) != 0) {
        printf("Unable to unlock cmg in remove config.\n");
    }
    return ret_code;
}

int update_config(struct init_args *pa)
{
    struct init_args stack_temp;
    struct init_args *temp = &stack_temp;
    int res = -1;
    if (pthread_mutex_lock(&client_map_guard) == 0) {
        temp = NULL;
        res =
            get_map_entry(pa_map, pa->name, pa->name_len, (void *) &temp);
        if (res > 0) {
            remove_from_map(pa_map, pa->name, pa->name_len);
        }
        res = 1;
        no_copy_insert(pa_map, pa->name, pa->name_len, pa);
    }
    if (pthread_mutex_unlock(&client_map_guard) != 0) {
        printf("Unable to unlock cmg in update config. \n");
    }
    return res;
}
