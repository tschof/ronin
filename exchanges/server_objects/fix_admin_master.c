/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "message_queue.h"
#include "hashmap.h"
#include "client_manager.h"
#include "fix_handler.h"
#include "fix_admin_master.h"
#include "dart_fix_connection_object.h"

static void *run_fa_loop(void *args)
{
    fix_con_obj *fco = (fix_con_obj *) args;
    dfc_run_admin_loop(fco);
    return NULL;
}

void run_fix_admin_loop(fix_con_obj * rec)
{
    int status;
    pthread_attr_t thread_attr;
    pthread_t fix_admin_thread;
    dfc_set_tickle(rec, 0);
    dfc_set_disconnect(rec, 0);
    status = pthread_attr_init(&thread_attr);
    if (status != 0) {
        printf("Pthread attr init failed!\n");
    }
    status = pthread_attr_setdetachstate(&thread_attr,
                                         PTHREAD_CREATE_DETACHED);
    if (status != 0) {
        printf("Thread set detached failed!\n");
    }

    pthread_create(&fix_admin_thread, &thread_attr, run_fa_loop, rec);
}

void tickle_connection(fix_con_obj * fco)
{
    dfc_send_tickle(fco);
}

void kill_connection(fix_con_obj * fco)
{
    dfc_kill_connection(fco);
}
