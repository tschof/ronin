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

#include "dart_seq_request.h"
#include "debug_logger.h"

struct dart_seq_request {
    unsigned long (*parse_messages) (char *data,
                                     unsigned long size,
                                     unsigned long bytes_used,
                                     unsigned long *seq_num,
                                     void* book);
    char *name;
    void* p_helper;
    size_t name_len;
    int create_new;
    unsigned long new_incoming;
    unsigned long new_outgoing;
    char *fix_version;
    char *(*create_reset_pad) (unsigned long seq, char *version,
                               size_t * len);
    unsigned long incoming;
    unsigned long outgoing;
    pthread_mutex_t seq_mutex;
    pthread_cond_t seq_cond;
};

unsigned long dsr_get_new_incoming(sequence_req * sr)
{
    return sr->new_incoming;
}

unsigned long dsr_get_new_outgoing(sequence_req * sr)
{
    return sr->new_outgoing;
}

const char *dsr_get_name(sequence_req * sr, size_t * name_len)
{
    *name_len = sr->name_len;
    return sr->name;
}

int dsr_get_create_new(sequence_req * sr)
{
    return sr->create_new;
}

unsigned long dsr_parse_data(sequence_req * sr, char *data,
                             unsigned long size, unsigned long bytes_used,
                             unsigned long *seq_num)
{
    return sr->parse_messages(data, size, bytes_used, seq_num, sr->p_helper);
}

char *dsr_create_pad(sequence_req * sr, unsigned long seq, size_t * len)
{
    return sr->create_reset_pad(seq, sr->fix_version, len);
}
static int reset_day_offsets[7][7] =
{
    {7,6,5,4,3,2,1},
    {1,7,6,5,4,3,2},
    {2,1,7,6,5,4,3},
    {3,2,1,7,6,5,4},
    {4,3,2,1,7,6,5},
    {5,4,3,2,1,7,6},
    {6,5,4,3,2,1,7}    
};

int ll_before_reset_time(struct init_args* in, time_t now)
{
    int create_new = 0;
    struct tm *ti;
    struct tm lm, ll;
    ti = localtime_r(&now, &lm);
    ti = &lm;
    localtime_r(&in->reset_time->last_logon, &ll);
    int day_offset = reset_day_offsets[ti->tm_wday][ll.tm_wday];
    if(in->reset_time->reset_day == 7 && 
            (day_offset > 1 && day_offset < 7)) {
        create_new = 1;
    } else if(in->reset_time->reset_day == 7 || in->reset_time->reset_day == ti->tm_wday) {
        int sec_diff = ((lm.tm_hour * 3600) + (lm.tm_min * 60))  -
             ((in->reset_time->reset_hour * 3600) + (in->reset_time->reset_min * 60));
        time_t reset_time = now - sec_diff;
        create_new = (now >= reset_time && in->reset_time->last_logon < reset_time) ? 1 : 0;
	send_debug_message("In Sequence: createNew? %d,now %d, reset? %d, last logon? %d \n", create_new, now, reset_time, in->reset_time->last_logon);
    } else if(in->reset_time->reset_day != 7) {
        int reset_offset = reset_day_offsets[ti->tm_wday][in->reset_time->reset_day];        
        struct tm t;
        memset(&t, 0, sizeof(struct tm));
        t.tm_year = ti->tm_year;
        t.tm_mon = ti->tm_mon;
        t.tm_mday = ti->tm_mday - reset_offset;
        t.tm_hour = in->reset_time->reset_hour;
        t.tm_min = in->reset_time->reset_min;
        t.tm_sec = 0;
        t.tm_isdst = -1;
        time_t reset_time = mktime(&t);
        create_new = (now >= reset_time && in->reset_time->last_logon < reset_time) ? 1 : 0;
    }
    return create_new;
}

sequence_req *create_sequence_request(const char *name, size_t name_len,
                                      struct init_args* in,
                                      unsigned long (*p_messages) (char
                                                                   *data,
                                                                   unsigned
                                                                   long
                                                                   size,
                                                                   unsigned
                                                                   long
                                                                   bytes_used,
                                                                   unsigned
                                                                   long
                                                                   *seq_num,
                                                                   void* book),
                                      char *(*create_pad) (unsigned long
                                                           seq,
                                                           char *fix_ver,
                                                           size_t * len), void* p_helper)
{
    sequence_req *sr = malloc(sizeof(struct dart_seq_request));
    if (sr != NULL) {
        time_t now;
        time(&now);
        sr->name = calloc(name_len + 1, 1);
        memcpy(sr->name, name, name_len);
        sr->name_len = name_len;
        sr->create_new = in->reset_time->create_new;
        sr->new_incoming = in->reset_time->new_incoming;
        sr->new_outgoing = in->reset_time->new_outgoing;
        sr->incoming = 0;
        sr->outgoing = 0;
        sr->parse_messages = p_messages;
        sr->fix_version = in->fix_version;
        sr->create_reset_pad = create_pad;
        sr->p_helper = p_helper;
        if(in->reset_time->create_new == 0 && in->reset_time->last_logon != 0) {
            sr->create_new = ll_before_reset_time(in, now);
        }
        in->reset_time->last_logon = now;
        if (pthread_mutex_init(&sr->seq_mutex, NULL) != 0) {
            printf("Failed to initialize seq mutex. \n");
        }
        if (pthread_cond_init(&sr->seq_cond, NULL) != 0) {
            printf("Failed to initialize seq cond. \n");
        }
    }
    return sr;
}

void destroy_sequence_req(sequence_req * sr)
{
    if (sr) {
        if (pthread_mutex_destroy(&sr->seq_mutex) != 0) {
            printf("Failed to destroy seq reset seq mutex. \n");
        }
        if (pthread_cond_destroy(&sr->seq_cond) != 0) {
            printf("Failed to destroy seq reset cond. \n");
        }
        free(sr->name);
        free(sr);
        sr = NULL;
    }
}

unsigned long dsr_get_incoming(sequence_req * sr)
{
    return sr->incoming;
}

unsigned long dsr_get_outgoing(sequence_req * sr)
{
    return sr->outgoing;
}

void dsr_set_incoming(sequence_req * sr, unsigned long incoming)
{
    sr->incoming = incoming;
}

void dsr_set_outgoing(sequence_req * sr, unsigned long outgoing)
{
    sr->outgoing = outgoing;
}

void dsr_lock_con_mutex(sequence_req * sr)
{
    if (pthread_mutex_lock(&sr->seq_mutex) != 0) {
        printf("Failed to lock mutex in sequence_request \n");
    }
}

void dsr_wait_on_cond(sequence_req * sr)
{
    pthread_cond_wait(&sr->seq_cond, &sr->seq_mutex);
    pthread_mutex_unlock(&sr->seq_mutex);
}

void dsr_signal_cond(sequence_req * sr)
{
    pthread_mutex_lock(&sr->seq_mutex);
    pthread_cond_signal(&sr->seq_cond);
    pthread_mutex_unlock(&sr->seq_mutex);
}
