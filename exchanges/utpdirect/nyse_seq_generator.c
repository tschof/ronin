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
#include <semaphore.h>

#include "hashmap.h"
#include "databuf.h"
#include "dart_order_allocator.h"

#define NYSE_SEQ_LEN 4
#define NYSE_SEQ_BASE 26
#define NYSE_MAX_BRANCH_COMBO 18250
#define NYSE_MIN_BRANCH_COMBO 26
#define NYSE_MAX_SEQ 9999
#define NYSE_SEQ_SIZE 20

static pthread_mutex_t nyse_seq_init = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t seq_protect = PTHREAD_MUTEX_INITIALIZER;
static pthread_t nyse_seq_thread;
static sem_t nyse_sem;

static int nyse_is_init = 0;
static hashmap_t reserved_id_map;
static char time_str[9];

static char *nyse_seq_writer;

struct nyse_seq_struct {
    int sequence;
    int branch_code_seq;
    char branch_code[NYSE_SEQ_LEN];
    FILE *map_file;
    int end_hour;
    int end_min;
};

static struct nyse_seq_struct *nyse_seq;

static const char baseChars[26] = {
    'A', 'B', 'C', 'D', 'E', 'F',
    'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
    'W', 'X', 'Y', 'Z'
};

static void write_val_to_file(int seq, FILE * map_file)
{
    int b_to_write;
    char *e_off;
    int written = 0;
    int result;
    memset(nyse_seq_writer, '\0', NYSE_SEQ_SIZE);
    result = sprintf(nyse_seq_writer, "%d", seq);
    fseek(map_file, 0, SEEK_SET);
    e_off = nyse_seq_writer;
    b_to_write = result;
    while (written < result) {
        written += fwrite(e_off, 1, b_to_write, map_file);
        e_off += written;
        b_to_write -= written;
    }
    fseek(map_file, 0, SEEK_SET);
}

static void res_id_breakdown(const char *res_ids, int rid_len)
{
    long cur_off;
    long len_left;
    long val_end;
    char *vend_ptr = NULL;
    char *d_off = NULL;
    int should_parse = 1;
    d_off = (char *) res_ids;
    cur_off = 0;
    val_end = 0;
    len_left = rid_len;
    while (should_parse && len_left > 0) {
        vend_ptr = strchr(d_off, ',');
        if (vend_ptr != NULL) {
            val_end = vend_ptr - d_off;
            if (insert_map
                (reserved_id_map, d_off, val_end, d_off, val_end)) {
            } else {
                printf("Insertion failed in res_id_breakdown.\n");
            }
            d_off += val_end + 1;
        } else {
            should_parse = 0;
        }
    }
}

static int is_reserved_branch_code(const char *b, int len)
{
    return contains_key(reserved_id_map, b, len);
}

static void *run_seq_file_updater(void *ra)
{
    struct nyse_seq_struct *nss = (struct nyse_seq_struct *) ra;
    time_t long_time;
    struct timespec timeout;
    struct tm *newtime;
    int keep_running;
    int status;
    int l_seq_branch = 0;
    keep_running = 1;
    while (keep_running) {
        timeout.tv_sec = time(NULL) + 3;
        timeout.tv_nsec = 0;
        status = sem_timedwait(&nyse_sem, &timeout);
        if (status == 0) {
        } else if (status == ETIMEDOUT || status == EINVAL) {
            printf
                ("Could not get the semaphore before timeout or bad sem: %d.\n",
                 status);
        } else if (status == EDEADLK) {
            printf("Deadlock Detected in nyse_seq_generator.\n");
        }
        pthread_mutex_lock(&seq_protect);
        l_seq_branch = nyse_seq->branch_code_seq;
        pthread_mutex_unlock(&seq_protect);
        write_val_to_file(l_seq_branch, nss->map_file);
        time(&long_time);
        struct tm lm;
        newtime = localtime_r(&long_time, &lm);
        if (lm.tm_hour == nss->end_hour && lm.tm_min >= nss->end_min) {
            keep_running = 0;
        }
    }
    fclose(nss->map_file);
    free(nss);
    return NULL;
}

static void reverse_bc()
{
    char first;
    char third;
    if (nyse_seq->branch_code[2] != '\0') {
        first = nyse_seq->branch_code[0];
        third = nyse_seq->branch_code[2];
        nyse_seq->branch_code[0] = third;
        nyse_seq->branch_code[2] = first;
    } else {
        third = nyse_seq->branch_code[1];
        first = nyse_seq->branch_code[0];
        nyse_seq->branch_code[0] = third;
        nyse_seq->branch_code[1] = first;
    }
}

static void set_branch_code()
{
    int remainder = 0;
    int pos = 0;
    int value = nyse_seq->branch_code_seq;
    memset(nyse_seq->branch_code, '\0', 4);
    while (value > 0 && pos < 3) {
        remainder = value % NYSE_SEQ_BASE;
        value = ((value - remainder) / NYSE_SEQ_BASE);
        nyse_seq->branch_code[pos] = baseChars[remainder];
        ++pos;
    }
    reverse_bc();
}

static void increment_branch_code()
{
    ++nyse_seq->branch_code_seq;
    if (nyse_seq->branch_code_seq > NYSE_MAX_BRANCH_COMBO ||
        nyse_seq->branch_code_seq < NYSE_MIN_BRANCH_COMBO) {
        nyse_seq->branch_code_seq = NYSE_MIN_BRANCH_COMBO;
    }
    set_branch_code();
}

void start_seq_generator(const char *reserved_ids, int rid_len,
                         const char *name, int name_len, int seq,
                         int create_new, int end_hour, int end_min)
{
    pthread_mutex_lock(&nyse_seq_init);
    if (nyse_is_init) {

    } else {
        pthread_attr_t thread_attr;
        FILE *map_file;
        long result;
        time_t rawtime;
        struct tm *ti;
        int year;
        int month;
        int status;
        int status_seq = 1;
        time(&rawtime);
        seq = seq < NYSE_MIN_BRANCH_COMBO ? NYSE_MIN_BRANCH_COMBO : seq;
        seq = seq > NYSE_MAX_BRANCH_COMBO ? NYSE_MIN_BRANCH_COMBO : seq;
        struct tm lm;
        ti = gmtime_r(&rawtime, &lm);
        ti = &lm;
        year = 1900 + ti->tm_year;
        month = 1 + ti->tm_mon;
        memset(time_str, '\0', 9);
        sprintf(time_str, "%02d%02d%04d", month, ti->tm_mday, year);
        nyse_seq_writer = calloc(NYSE_SEQ_SIZE, 1);
        sem_init(&nyse_sem, 0, 0);
        nyse_is_init = 1;
        reserved_id_map = create_map(50);
        map_file = NULL;
        res_id_breakdown(reserved_ids, rid_len);
        if (create_new) {
            map_file = fopen(name, "w+");
        } else {
            map_file = fopen(name, "r+");
            if (map_file == NULL) {
                create_new = 1;
                map_file = fopen(name, "w+");
            }
        }
        if (map_file != NULL && !create_new) {
            fseek(map_file, 0, SEEK_SET);
            result = fread(nyse_seq_writer, 1, NYSE_SEQ_SIZE, map_file);
            if (result < NYSE_SEQ_SIZE && (ferror(map_file))) {
                printf("Error Initializing %s, setting sequence to 0\n",
                       name);
            } else {
                seq = atoi(nyse_seq_writer);
                seq =
                    seq <
                    NYSE_MIN_BRANCH_COMBO ? NYSE_MIN_BRANCH_COMBO : seq;
                seq =
                    seq >
                    NYSE_MAX_BRANCH_COMBO ? NYSE_MIN_BRANCH_COMBO : seq;
                status_seq = 9999;
            }
        } else if (map_file != NULL) {
            write_val_to_file(seq, map_file);
        } else {
            printf("Unable to open this order id file: %s\n", name);
            pthread_mutex_unlock(&nyse_seq_init);
            return;
        }
        nyse_seq = malloc(sizeof(struct nyse_seq_struct));
        if (nyse_seq) {
            nyse_seq->map_file = map_file;
            nyse_seq->sequence = status_seq;
            nyse_seq->branch_code_seq = seq;
            memset(nyse_seq->branch_code, '\0', NYSE_SEQ_LEN);
            nyse_seq->end_hour = end_hour;
            nyse_seq->end_min = end_min;
            set_branch_code();
            status = pthread_attr_init(&thread_attr);
            if (status != 0) {
                printf("Pthread attr init failed!\n");
                pthread_mutex_unlock(&nyse_seq_init);
                return;
            }
            status = pthread_attr_setdetachstate(&thread_attr,
                                                 PTHREAD_CREATE_DETACHED);
            if (status != 0) {
                printf("Thread set detached failed!\n");
                pthread_mutex_unlock(&nyse_seq_init);
                return;
            }
            pthread_create(&nyse_seq_thread, &thread_attr,
                           run_seq_file_updater, nyse_seq);
        } else {
            printf("Failed to malloc seq struct.\n");
            pthread_mutex_unlock(&nyse_seq_init);
            return;
        }
    }
    pthread_mutex_unlock(&nyse_seq_init);
}

char *next_order_id(size_t * len)
{
    char *buffer = calloc(DART_EX_TAG_LEN, 1);
    pthread_mutex_lock(&seq_protect);
    if (nyse_seq->sequence > NYSE_MAX_SEQ) {
        nyse_seq->sequence = 1;
        increment_branch_code();
        while (is_reserved_branch_code(nyse_seq->branch_code, 3)) {
            increment_branch_code();
        }
    }
    *len =
        sprintf(buffer, "%s %04d/%s", nyse_seq->branch_code,
                nyse_seq->sequence, time_str);

    ++nyse_seq->sequence;
    pthread_mutex_unlock(&seq_protect);
    sem_post(&nyse_sem);
    return buffer;
}

int in_place_order_id(char *buff)
{
    int ret = 0;
    pthread_mutex_lock(&seq_protect);
    if (nyse_seq->sequence > NYSE_MAX_SEQ) {
        nyse_seq->sequence = 1;
        increment_branch_code();
        while (is_reserved_branch_code(nyse_seq->branch_code, 3)) {
            increment_branch_code();
        }
    }
    ret =
        sprintf(buff, "%s %04d/%s", nyse_seq->branch_code,
                nyse_seq->sequence, time_str);
    ++nyse_seq->sequence;
    pthread_mutex_unlock(&seq_protect);
    sem_post(&nyse_sem);
    pthread_mutex_unlock(&seq_protect);
    return ret;
}
