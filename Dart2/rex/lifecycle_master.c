/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <sys/signalfd.h>
#include <sys/timerfd.h>
#include <time.h>
#include "lifecycle_master.h"
#include "call_back_wrapper.h"
#include "log_mnger.h"

struct lifecycle_master {
    pserver* asc;
    ex_config* mc;
    call_back_wrapper* sig_wrapper;
    call_back_wrapper* eod_wrapper;
};

void process_terminated(void* v)
{
    cycle_master* cm = (cycle_master*)v;
    otl* o = get_logger(cm->asc);
    send_debug_message(o, "Life cycle master destroy lifer signal captured.\n");
}

void timer_end(void* v)
{
    cycle_master* cm = (cycle_master*)v;
    otl* o = get_logger(cm->asc);
    send_debug_message(o, "Life cycle master shutdown starting.\n");
    stop_logging(o);
    stop_system(cm->asc);
}

void destroy_lifer(void* v)
{
    cycle_master* cm = (cycle_master*)v;
    otl* o = get_logger(cm->asc);
    send_debug_message(o, "Life cycle master destroy lifer signal captured shutting down.\n");
    stop_logging(o);
    stop_system(cm->asc);
}

static call_back_wrapper* create_signal_handler(cycle_master* cm,
                                 void (*destroy_me)(void* v))
{
    int fd;
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGTERM);
    if(sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
        perror("could not block sigs.");
    }
    fd = signalfd(-1, &mask, SFD_NONBLOCK);
    if(fd < 0) {
        perror("failed to build fd for signal handling.");
    }    
    call_back_wrapper* cbw = create_call_back_wrapper(cm,
                                                      process_terminated,
                                                      destroy_me, fd);
    return cbw;
}

#define SEC_PER_DAY 86400
#define SEC_PER_HOUR 3600
#define SEC_PER_MIN 60

int get_sec_till_end_time(int end_hour, int end_min, int end_day,
                          struct tm* newtime)
{
    if(end_day == 7) {
        if(end_hour <= newtime->tm_hour) {
            int sec_till_eod = ((24 - newtime->tm_hour)*SEC_PER_MIN) - newtime->tm_min;
            sec_till_eod *= SEC_PER_MIN;
            int sec_on_last = (end_hour * SEC_PER_MIN) + end_min;
            sec_on_last *= SEC_PER_MIN;
            return (sec_till_eod + sec_on_last);
        } else {
            int min_to_hour =  ((end_hour - newtime->tm_hour) * 60) - newtime->tm_min; 
            if(min_to_hour < 0) {
                min_to_hour = abs(end_min - newtime->tm_min);
            } else {
                min_to_hour += end_min;
            }
            return (min_to_hour * SEC_PER_MIN);
        }
    } else {
        int full_day_secs = (end_day - newtime->tm_wday) -1;
        if(full_day_secs > 0) {
            full_day_secs *= SEC_PER_DAY;
        } else {
            full_day_secs = 0;
        }
        int sec_till_eod = 0;
        int sec_on_last = 0;
        if(end_day == newtime->tm_wday) {
            sec_till_eod = ((end_hour - newtime->tm_hour) * 60) - newtime->tm_min;
            if(sec_till_eod < 0) {
                sec_till_eod = abs(end_min - newtime->tm_min);
            } else {
                sec_till_eod += end_min;
            }
            sec_till_eod *= SEC_PER_MIN;
        } else {
            sec_till_eod = ((24 - newtime->tm_hour)*60) - newtime->tm_min;
            sec_till_eod *= SEC_PER_MIN;
            sec_on_last = (end_hour * 60) + end_min;
            sec_on_last *= 60;
        }
        return (sec_on_last + sec_till_eod + full_day_secs);
    }
}

static void create_timer(pserver* asc, 
                         ex_config* mc,
                         cycle_master* cm,
                         void (*eod_clean_up)(void* v))
{

    time_t now = 0;
    struct tm *newtime;
    time(&now);
    struct tm lm;
    newtime = localtime_r(&now, &lm);
    newtime = &lm;
    int sec_till_drop = get_sec_till_end_time(mc->end_hour, mc->end_min,
                                              mc->end_day, newtime); 
    cm->eod_wrapper =  add_timer(asc, 0, 0, sec_till_drop,
                             0, cm,
                             eod_clean_up);
}

void destroy_life_cycle_master(cycle_master* cm)
{
    destroy_call_back_wrapper(cm->sig_wrapper);
    destroy_call_back_wrapper(cm->eod_wrapper);
    free(cm);
}

cycle_master* create_lifecycle_master(pserver* asc, 
                                      void (*destroy_me)(void* v),
                                      void (*eod_clean_up) (void* v),
                                      ex_config* mc)
{
    cycle_master* m = malloc(sizeof(struct lifecycle_master));
    m->asc = asc;
    m->mc = mc;
    m->sig_wrapper = create_signal_handler(m, destroy_me);
    add_signal_listener(asc, m->sig_wrapper);
    create_timer(asc, mc, m, eod_clean_up);
    return m;
}

cycle_master* create_gen_life_master(pserver* asc, ex_config* mc)
{
    cycle_master* m = malloc(sizeof(struct lifecycle_master));
    m->asc = asc;
    m->mc = mc;
    m->sig_wrapper = create_signal_handler(m, destroy_lifer);
    add_signal_listener(asc, m->sig_wrapper);
    create_timer(asc, mc, m, timer_end);
    return m;
}

