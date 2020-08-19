/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#include "transaction_recorder.h"
#include "dart_fix_connection_object.h"
#include "dart_connection_object.h"
#include "fix_handler.h"

struct dart_fix_connection_obj {
    char *sender_compid;
    size_t sc_len;
    char *sendersubid;
    size_t ss_len;
    char *target_compid;
    size_t tc_len;
    char *targetsubid;
    size_t ts_len;
    unsigned long in_seq;
    unsigned long out_seq;
    app_end_time *t;
    int tickle;
    int disconnect;
    size_t timeout_val;
    pthread_mutex_t fix_seq_blocker;
    pthread_mutex_t fix_admin_blocker;
    pthread_mutex_t fix_admin_cond_mut;
    pthread_cond_t fix_admin_cond;
    pthread_mutex_t fix_ts_blocker;
    char *fix_version;
    size_t fv_len;
    char *mod_name;
    size_t mod_len;
    con_obj *obj;
    time_t last_mess_sec;
    time_t last_mess_rec;
    int log_outgoing;
    int pending_resend;
     size_t(*create_mess_header) (databuf_t * fixmsg,
                                  const char *sendercompid,
                                  size_t sendercomplen,
                                  const char *targetcompid,
                                  size_t targetcomplen,
                                  const char *sendersubid,
                                  size_t sendersublen,
                                  const char *targetsubid,
                                  size_t targetsublen,
                                  unsigned long seq, time_t * t);
};

void dfc_set_timeout(fix_con_obj * fco, size_t tv)
{
    pthread_mutex_lock(&fco->fix_ts_blocker);
    fco->timeout_val = tv;
    pthread_mutex_unlock(&fco->fix_ts_blocker);
}

void dfc_get_timeout(fix_con_obj * fco, size_t * tv)
{
    pthread_mutex_lock(&fco->fix_ts_blocker);
    *tv = fco->timeout_val;
    pthread_mutex_unlock(&fco->fix_ts_blocker);
}

const char *dfc_get_mod_name(fix_con_obj * fco, size_t * len)
{
    *len = fco->mod_len;
    return fco->mod_name;
}

const char *dfc_get_target_comp_id(fix_con_obj * fco, size_t * len)
{
    *len = fco->tc_len;
    return fco->target_compid;
}

const char *dfc_get_sender_comp_id(fix_con_obj * fco, size_t * len)
{
    *len = fco->sc_len;
    return fco->sender_compid;
}

const char *dfc_get_sendersubid(fix_con_obj * fco, size_t * len)
{
    *len = fco->ss_len;
    return fco->sendersubid;
}

const char *dfc_get_targetsubid(fix_con_obj * fco, size_t * len)
{
    *len = fco->ts_len;
    return fco->targetsubid;
}

int dfc_get_disconnected(fix_con_obj * fco)
{
    return fco->disconnect;
}

void dfc_set_last_mess_rec(fix_con_obj * fco, time_t rawtime)
{
    pthread_mutex_lock(&fco->fix_ts_blocker);
    fco->last_mess_rec = rawtime;
    pthread_mutex_unlock(&fco->fix_ts_blocker);
}

time_t dfc_get_last_mess_rec(fix_con_obj * fco)
{
    time_t ret_t = 0;
    pthread_mutex_lock(&fco->fix_ts_blocker);
    ret_t = fco->last_mess_rec;
    pthread_mutex_unlock(&fco->fix_ts_blocker);
    return ret_t;
}

void dfc_set_last_mess_sec(fix_con_obj * fco, time_t rawtime)
{
    pthread_mutex_lock(&fco->fix_ts_blocker);
    fco->last_mess_sec = rawtime;
    pthread_mutex_unlock(&fco->fix_ts_blocker);
}

time_t dfc_get_last_mess_sec(fix_con_obj * fco)
{
    time_t ret_t = 0;
    pthread_mutex_lock(&fco->fix_ts_blocker);
    ret_t = fco->last_mess_sec;
    pthread_mutex_unlock(&fco->fix_ts_blocker);
    return ret_t;
}

void dfc_get_end_values(fix_con_obj * fco, int *hour, int *min, int *day)
{
    pthread_mutex_lock(&fco->fix_ts_blocker);
    *hour = fco->t->end_hour;
    *min = fco->t->end_min;
    *day = fco->t->end_day;
    pthread_mutex_unlock(&fco->fix_ts_blocker);
}

int dfc_get_socket(fix_con_obj * fco)
{
    if (fco->obj) {
        return get_sock(fco->obj);
    } else {
        return -1;
    }
}

con_obj *dfc_get_con_obj(fix_con_obj * fco)
{
    return fco->obj;
}

const char *dfc_get_fix_version(fix_con_obj * fco, size_t * len)
{
    *len = fco->fv_len;
    return fco->fix_version;
}

fix_con_obj *create_fix_con_obj(const char *sender_comp, size_t sc_len,
                                const char *target_comp, size_t tc_len,
                                const char *sendersub, size_t sendersublen,
                                const char *targetsub, size_t targetsublen,
                                unsigned long in_seq,
                                unsigned long out_seq, app_end_time * t,
                                int tickle, int log_outgoing,
                                size_t timeout_val, char *fix_version,
                                size_t fv_len, char *mod_name,
                                size_t mod_len, con_obj * obj,
                                size_t(*create_mess_header) (databuf_t *
                                                             fixmsg,
                                                             const char
                                                             *sendercompid,
                                                             size_t
                                                             sendercomplen,
                                                             const char
                                                             *targetcompid,
                                                             size_t
                                                             targetcomplen,
                                                             const char
                                                             *sendersubid,
                                                             size_t
                                                             sendersublen,
                                                             const char
                                                             *targetsubid,
                                                             size_t
                                                             targetsublen,
                                                             unsigned long
                                                             seq,
                                                             time_t * t))
{
    fix_con_obj *fco = malloc(sizeof(struct dart_fix_connection_obj));
    if (fco != NULL) {
        fco->sender_compid = calloc(sc_len + 1, 1);
        memcpy(fco->sender_compid, sender_comp, sc_len);
        fco->sc_len = sc_len;

        fco->sendersubid = calloc(sendersublen + 1, 1);
        memcpy(fco->sendersubid, sendersub, sendersublen);
        fco->ss_len = sendersublen;

        fco->target_compid = calloc(tc_len + 1, 1);
        memcpy(fco->target_compid, target_comp, tc_len);
        fco->tc_len = tc_len;

        fco->targetsubid = calloc(targetsublen + 1, 1);
        memcpy(fco->targetsubid, targetsub, targetsublen);
        fco->ts_len = targetsublen;
        fco->log_outgoing = log_outgoing;
        fco->in_seq = in_seq;
        fco->out_seq = out_seq;
        fco->tickle = 0;
        fco->disconnect = 0;
        fco->timeout_val = timeout_val;
        if (fv_len > 0) {
            fco->fix_version = calloc(fv_len + 1, 1);
            memcpy(fco->fix_version, fix_version, fv_len);
            fco->fv_len = fv_len;
        } else {
            fco->fix_version = NULL;
            fco->fv_len = 0;
        }
        fco->last_mess_sec = 0;
        fco->last_mess_rec = 0;
        fco->t = t;
        if (mod_len > 0 && mod_len < 50) {
            fco->mod_name = calloc(mod_len + 1, 1);
            memcpy(fco->mod_name, mod_name, mod_len);
            fco->mod_len = mod_len;
        } else {
            fco->mod_name = NULL;
            fco->mod_len = 0;
        }
        if (pthread_mutex_init(&fco->fix_seq_blocker, NULL) != 0) {
            printf("Failed to init fix_seq_blocker.\n");
        }
        if (pthread_mutex_init(&fco->fix_admin_blocker, NULL) != 0) {
            printf("Failed to init fix_admin_blocker mutex.\n");
        }
        if (pthread_mutex_init(&fco->fix_admin_cond_mut, NULL) != 0) {
            printf("Failed to init the fix_admin_cond_mut.\n");
        }
        if (pthread_cond_init(&fco->fix_admin_cond, NULL) != 0) {
            printf("Failed to init the fix_admin_cond.\n");
        }
        if (pthread_mutex_init(&fco->fix_ts_blocker, NULL) != 0) {
            printf("Failed to init the fix TS blocker. \n");
        }
        fco->obj = obj;
        fco->create_mess_header = create_mess_header;
        fco->pending_resend = 0;
    }
    return fco;
}

int dfc_get_pending_reset(fix_con_obj* obj)
{
    return obj->pending_resend;
}
void dfc_set_pending_reset(fix_con_obj* obj, int pr)
{
    obj->pending_resend = pr;
}

int dfc_get_log_outgoing(fix_con_obj * obj)
{
    return obj->log_outgoing;
}

void dfc_set_log_outgoing(fix_con_obj * obj, int log)
{
    obj->log_outgoing = log;
}

size_t dfc_create_fix_header(fix_con_obj * connobj, databuf_t * fixmsg,
                             const char *sendercompid,
                             size_t sendercomplen,
                             const char *targetcompid,
                             size_t targetcomplen, const char *sendersubid,
                             size_t sendersublen, const char *targetsubid,
                             size_t targetsublen, unsigned long seq,
                             time_t * t)
{
    return connobj->create_mess_header(fixmsg, sendercompid, sendercomplen,
                                       targetcompid, targetcomplen,
                                       sendersubid, sendersublen,
                                       targetsubid, targetsublen, seq, t);
}

void destroy_fix_con_obj(fix_con_obj * obj)
{
    if (obj) {
        if (obj->sender_compid) {
            free(obj->sender_compid);
            obj->sender_compid = NULL;
        }
        if (obj->target_compid) {
            free(obj->target_compid);
            obj->target_compid = NULL;
        }
        if (obj->fv_len > 0) {
            free(obj->fix_version);
            obj->fix_version = NULL;
        }
        if (obj->mod_len > 0) {
            free(obj->mod_name);
            obj->mod_name = NULL;
        }
        free(obj);
        obj = NULL;
    }
}

void dfc_set_con_obj(fix_con_obj * fco, con_obj * co)
{
    if (fco) {
        if (pthread_mutex_lock(&fco->fix_seq_blocker) == 0) {
            fco->obj = co;
        }
        if (pthread_mutex_unlock(&fco->fix_seq_blocker) != 0) {
            printf
                ("Failed to unlock fix_seq_blocker in fix_con_obj set_con_obj.\n");
        }
    }
}

void dfc_set_mod_name(fix_con_obj * fco, const char *mod_name,
                      size_t mod_len)
{
    if (mod_len > 0 && mod_len < 50) {
        if(fco->mod_len == mod_len && strncmp(fco->mod_name, mod_name, mod_len) == 0) {
            //do nothing we are already set
            return;
        } else {
            fco->mod_name = calloc(mod_len + 1, 1);
            memcpy(fco->mod_name, mod_name, mod_len);
            fco->mod_len = mod_len;
        }
    } else {
        fco->mod_name = NULL;
        fco->mod_len = 0;
    }
}

void dfc_set_incoming_seq(fix_con_obj * fco, unsigned long in_seq)
{
    fco->in_seq = in_seq;
}

void dfc_set_outgoing_seq(fix_con_obj * fco, unsigned long out_seq)
{
    fco->out_seq = out_seq;
}

unsigned long dfc_get_outgoing_seq(fix_con_obj * fco)
{
    return fco->out_seq;
}

unsigned long dfc_get_incoming_seq(fix_con_obj * fco)
{
    return fco->in_seq;
}

void dfc_increment_outgoing_seq(fix_con_obj * fco)
{
    fco->out_seq++;
}

void dfc_increment_incoming_seq(fix_con_obj * fco)
{
    fco->in_seq++;
}

void dfc_set_tickle(fix_con_obj * fco, int tickle)
{
    pthread_mutex_lock(&fco->fix_ts_blocker);
    fco->tickle = tickle;
    pthread_mutex_unlock(&fco->fix_ts_blocker);
}

int dfc_get_tickle(fix_con_obj * fco)
{
    int tick = 0;
    pthread_mutex_lock(&fco->fix_ts_blocker);
    tick = fco->tickle;
    pthread_mutex_unlock(&fco->fix_ts_blocker);
    return tick;
}

void dfc_set_disconnect(fix_con_obj * fco, int disconnect)
{
    pthread_mutex_lock(&fco->fix_ts_blocker);
    fco->disconnect = disconnect;
    pthread_mutex_unlock(&fco->fix_ts_blocker);
}

int dfc_get_disconnect(fix_con_obj * fco)
{
    int dis = 0;
    pthread_mutex_lock(&fco->fix_ts_blocker);
    dis = fco->disconnect;
    pthread_mutex_unlock(&fco->fix_ts_blocker);
    return dis;
}

void dfc_lock_seq_blocker(fix_con_obj * fco)
{
    if (pthread_mutex_lock(&fco->fix_seq_blocker) != 0) {
        printf("Failed to lock fix_seq_blocker.");
    }
}

void dfc_unlock_seq_blocker(fix_con_obj * fco)
{
    if (pthread_mutex_unlock(&fco->fix_seq_blocker) != 0) {
        printf
            ("Failed to unlock fix_seq_blocker in fix_admin_master cleanup.\n");
    }
}

void dfc_send_tickle(fix_con_obj * fco)
{
    dfc_set_tickle(fco, 1);
    pthread_mutex_lock(&fco->fix_admin_cond_mut);
    pthread_cond_signal(&fco->fix_admin_cond);
    pthread_mutex_unlock(&fco->fix_admin_cond_mut);
}

void dfc_kill_connection(fix_con_obj * fco)
{
    dfc_set_disconnect(fco, 1); //fco->disconnect = 1;
    printf("Signalling thread dead.\n");
    if (pthread_mutex_trylock(&fco->fix_admin_cond_mut) == 0) {
        pthread_cond_broadcast(&fco->fix_admin_cond);
        pthread_mutex_unlock(&fco->fix_admin_cond_mut);
    }
    printf("Done with signal.\n");
}

void dfc_run_admin_loop(fix_con_obj * fco)
{
    time_t now = 0;
    struct timespec timeout;
    struct tm *newtime;
    int status;
    int keep_running;
    time_t l_mess_sec = 0;
    size_t l_timeout_val = 0;
    int l_end_hour = 0;
    int l_end_min = 0;
    int l_end_day = 7;
    dfc_get_end_values(fco, &l_end_hour, &l_end_min, &l_end_day);
    dfc_get_timeout(fco, &l_timeout_val);
    keep_running = 1;
    while (keep_running) {
        l_mess_sec = dfc_get_last_mess_sec(fco);
        timeout.tv_sec = l_mess_sec + l_timeout_val;
        timeout.tv_nsec = 0;
        status = pthread_mutex_lock(&fco->fix_admin_cond_mut);
        if (status != 0) {
            printf("Failed to get lock!\n");
        }
        if (dfc_get_disconnect(fco)) {
            keep_running = 0;
            pthread_exit(NULL);
            printf("Dead Admin.\n");
            return;
        }
        if (dfc_get_tickle(fco)) {
            dfc_set_tickle(fco, 0);
            send_test_req(fco);
        }
        status =
            pthread_cond_timedwait(&fco->fix_admin_cond,
                                   &fco->fix_admin_cond_mut, &timeout);
        pthread_mutex_unlock(&fco->fix_admin_cond_mut);
        if (status == ETIMEDOUT) {
            int diff_result = 0;
            time(&now);
            struct tm lm;
            newtime = localtime_r(&now, &lm);
            newtime = &lm;
            if (newtime->tm_hour == l_end_hour
                && newtime->tm_min >= l_end_min
                && newtime->tm_min < (l_end_min + 5)
                && ((newtime->tm_wday == l_end_day) || (l_end_day == 7))) {
                printf
                    ("Time to end FIX admin loop end hour: %d end min %d!!\n",
                     l_end_hour, l_end_min);
                keep_running = 0;
            }
            l_mess_sec = dfc_get_last_mess_sec(fco);
            diff_result = now - l_mess_sec;
            if (diff_result > 0
                && (diff_result >= l_timeout_val || l_mess_sec == 0)) {
                send_heart_beat(fco);
            }
        }
        if (dfc_get_disconnect(fco)) {
            keep_running = 0;
            printf("Killing da Tread.\n");
            break;
        }
        if (dfc_get_tickle(fco)) {
            dfc_set_tickle(fco, 0);
            send_test_req(fco);
        }
    }
    printf("Dead Admin.\n");
    pthread_exit(NULL);
}
