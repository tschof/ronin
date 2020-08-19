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

#include "dart_connection_object.h"
#include "transaction_recorder.h"
#include "debug_logger.h"

struct dart_con_obj {
    char *name;
    size_t name_len;
    int sock;
    int disconnect;
    int been_removed;
    int should_notify;
    pthread_mutex_t con_name_mtx;
    unsigned long in_seq;
    unsigned long out_seq;
};

con_obj *create_empty_con_obj()
{
    con_obj *co = malloc(sizeof(struct dart_con_obj));
    if (co != NULL) {
        co->name = NULL;
        co->name_len = 0;
        co->sock = -1;
        co->disconnect = 0;
        co->in_seq = 0;
        co->out_seq = 0;
        co->been_removed = 0;
        co->should_notify = 1;
        if (pthread_mutex_init(&co->con_name_mtx, NULL) != 0) {
            printf("Failed to init con_name_mtx .\n");
        }
    }
    return co;
}

void destroy_con_obj(con_obj * co)
{
    if (co) {
        if (pthread_mutex_lock(&co->con_name_mtx) != 0) {
            printf("Failed to lock mtx in destroy.\n");
        }
        co->disconnect = 1;
        if (co->name_len > 0 && co->name) {
            free(co->name);
            co->name_len = 0;
            co->name = 0;
        }
        if (pthread_mutex_unlock(&co->con_name_mtx) != 0) {
            printf("failed to unlock mtx in destroy.\n");
        }
        if (pthread_mutex_destroy(&co->con_name_mtx) != 0) {
            printf("failed to destroy mutex in destroy. \n");
        }
        free(co);
        co = 0;
    }
}

con_obj *create_con_obj(const char *name, size_t name_len, int sock,
                        int disconnect, unsigned long in_seq,
                        unsigned long out_seq)
{
    con_obj *co = malloc(sizeof(struct dart_con_obj));
    if (co != NULL) {
        co->name = calloc(name_len + 1, 1);
        if (co->name == NULL) {
            return NULL;
        }
        co->name_len = name_len;
        memcpy(co->name, name, name_len);
        co->sock = sock;
        co->disconnect = disconnect;
        co->in_seq = in_seq;
        co->out_seq = out_seq;
        co->been_removed = 0;
        co->should_notify = 1;
        if (pthread_mutex_init(&co->con_name_mtx, NULL) != 0) {
            printf("Failed to init con_name_mtx .\n");
        }
    }
    return co;
}

void set_sock(con_obj * co, int sock)
{
    if (pthread_mutex_lock(&co->con_name_mtx) != 0) {
        printf("Failed to lock mtx in set_sock.\n");
    }
    co->sock = sock;
    if (pthread_mutex_unlock(&co->con_name_mtx) != 0) {
        printf("Failed to unlock mtx in set sock.\n");
    }
}

int get_sock(con_obj * co)
{
    int ret = 0;
    if (pthread_mutex_lock(&co->con_name_mtx) != 0) {
        printf("Failed to lock mtx in get_sock.\n");
    }
    ret = co->sock;
    if (pthread_mutex_unlock(&co->con_name_mtx) != 0) {
        printf("Failed to lock mtx in get_sock.\n");
    }
    return ret;
}

void set_outgoing_seq(con_obj * co, unsigned long out_seq)
{
    if (pthread_mutex_lock(&co->con_name_mtx) != 0) {
        printf("Failed to lock mtx in set_outgoing.\n");
    }
    co->out_seq = out_seq;
    if (pthread_mutex_unlock(&co->con_name_mtx) != 0) {
        printf("Failed to unlock mtx in set_outgoing.\n");
    }
}

void set_incoming_seq(con_obj * co, unsigned long in_seq)
{
    if (pthread_mutex_lock(&co->con_name_mtx) != 0) {
        printf("Failed to lock mtx in set_incoming.\n");
    }
    co->in_seq = in_seq;
    if (pthread_mutex_unlock(&co->con_name_mtx) != 0) {
        printf("Failed to unlock mtx in set_incoming.\n");
    }
}

void set_disconnect(con_obj * co, int disconnect)
{
    if (pthread_mutex_lock(&co->con_name_mtx) != 0) {
        printf("Failed to lock mtx in set_disconnect.\n");
    }
    co->disconnect = disconnect;
    if (pthread_mutex_unlock(&co->con_name_mtx) != 0) {
        printf("Failed to unlock mtx in set_disconnect.\n");
    }
}

unsigned long get_outgoing_seq(con_obj * co)
{
    unsigned long ret = 0;
    if (pthread_mutex_lock(&co->con_name_mtx) != 0) {
        printf("Failed to lock mtx in get_outgoing.\n");
    }
    ret = co->out_seq;
    if (pthread_mutex_unlock(&co->con_name_mtx) != 0) {
        printf("Failed to unlock mtx in get_outgoing.\n");
    }
    return ret;
}

void set_removed(con_obj * co, int removed)
{
    if (pthread_mutex_lock(&co->con_name_mtx) != 0) {
        printf("Failed to lock mtx in set_removed.\n");
    }
    co->been_removed = removed;
    if (pthread_mutex_unlock(&co->con_name_mtx) != 0) {
        printf("Failed to unlock mtx in set_removed.\n");
    }
}

int get_is_removed(con_obj * co)
{
    int ret = 0;
    if (pthread_mutex_lock(&co->con_name_mtx) != 0) {
        printf("Failed to lock mtx in get_removed.\n");
    }
    ret = co->been_removed;
    if (pthread_mutex_unlock(&co->con_name_mtx) != 0) {
        printf("Failed to unlock mtx in get_removed.\n");
    }
    return ret;
}

void set_should_notify(con_obj * co, int s)
{
    if (pthread_mutex_lock(&co->con_name_mtx) != 0) {
        printf("Failed to lock mtx in set_should_notify.\n");
    }
    co->should_notify = s;
    if (pthread_mutex_unlock(&co->con_name_mtx) != 0) {
        printf("Failed to unlock mtx in set_should_notify.\n");
    }
}

int get_should_notify(con_obj * co)
{
    int ret = 0;
    if (pthread_mutex_lock(&co->con_name_mtx) != 0) {
        printf("Failed to lock mtx in get_should_notify.\n");
    }
    ret = co->should_notify;
    if (pthread_mutex_unlock(&co->con_name_mtx) != 0) {
        printf("Failed to unlock mtx in get_should_notify.\n");
    }
    return ret;
}

unsigned long get_incoming_seq(con_obj * co)
{
    unsigned long ret = 0;
    if (pthread_mutex_lock(&co->con_name_mtx) != 0) {
        printf("Failed to lock mtx in get_incoming.\n");
    }
    ret = co->in_seq;
    if (pthread_mutex_unlock(&co->con_name_mtx) != 0) {
        printf("Failed to unlock mtx in get_incoming.\n");
    }
    return ret;
}

int get_disconnect(con_obj * co)
{
    int ret = 0;
    if (pthread_mutex_lock(&co->con_name_mtx) != 0) {
        printf("Failed to lock mtx in get_disconnect.\n");
    }
    ret = co->disconnect;
    if (pthread_mutex_unlock(&co->con_name_mtx) != 0) {
        printf("Failed to unlock mtx in get_disconnect.\n");
    }
    return ret;
}

const char *get_name(con_obj * co)
{
    char *ret = 0;
    if (pthread_mutex_lock(&co->con_name_mtx) != 0) {
        printf("Failed to lock mtx in get_name.\n");
    }
    ret = calloc(co->name_len + 1, 1);
    memcpy(ret, co->name, co->name_len);
    if (pthread_mutex_unlock(&co->con_name_mtx) != 0) {
        printf("Failed to unlock mtx in get_name.\n");
    }
    return ret;
}

size_t get_name_len(con_obj * co)
{
    size_t ret = 0;
    if (pthread_mutex_lock(&co->con_name_mtx) != 0) {
        printf("Failed to lock mtx in get_name_len.\n");
    }
    ret = co->name_len;
    if (pthread_mutex_unlock(&co->con_name_mtx) != 0) {
        printf("Failed to unlock mtx in get_name_len.\n");
    }
    return ret;
}

void increment_incoming_seq(con_obj * co)
{
    if (pthread_mutex_lock(&co->con_name_mtx) != 0) {
        printf("Failed to lock mtx in inc_incomming.\n");
    }
    co->in_seq++;
    if (pthread_mutex_unlock(&co->con_name_mtx) != 0) {
        printf("Failed to unlock mtx in inc_incoming.\n");
    }
}

void increment_outgoing_seq(con_obj * co)
{
    if (pthread_mutex_lock(&co->con_name_mtx) != 0) {
        printf("Failed to lock mtx in inc_out.\n");
    }
    co->out_seq++;
    if (pthread_mutex_unlock(&co->con_name_mtx) != 0) {
        printf("Failed to unlock mtx in inc_out.\n");
    }
}

void set_name(con_obj * co, const char *name, size_t name_len)
{
    if (pthread_mutex_lock(&co->con_name_mtx) != 0) {
        printf("Failed to lock mtx in set_name.\n");
    }
    co->name = calloc(name_len + 1, 1);
    memcpy(co->name, name, name_len);
    co->name_len = name_len;
    if (pthread_mutex_unlock(&co->con_name_mtx) != 0) {
        printf("Failed to unlock mtx in set_name.\n");
    }
}
