/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#include "transaction_recorder.h"
#include "hashmap.h"
#include "databuf.h"
#include "tm_to_str.h"
#include "dart_obj_factory.h"
#include "dart_allocator.h"
#include "gatekeeper.h"
#include "dart_constants.h"
#include "async_server.h"
#include "dbg_helper.h"
#include "order_obj_fact.h"
#include "connection_interface.h"
#include "parser_func.h"

#define INCOMING_SUFFIX "_incoming.log"
#define OUTGOING_SUFFIX "_outgoing.log"
#define SUFFIX_LEN 13
#define IN_SEQ_SIZE 50
#define ROLL_FILE_SIZE 50000000

#define NANO_MULT 100000

static pthread_mutex_t init_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_spinlock_t queue_mutex = 0;
static pthread_spinlock_t factory_mutex = 0;
static pthread_spinlock_t seq_queue_mutex = 0;
static queue_t incoming_queue;
static queue_t client_out_queue;
static queue_t seq_request;
static queue_t resend_requests;
static queue_t rolling_queue;
static queue_t oats_queue;
static queue_t raw_queue;
static queue_t clean_queue;
static hashmap_t dest_id_map;
static int is_init = 0;
static int keep_running = 1;
static databuf_t *in_seq_writer = 0;
static dof_t *byte_logger_fact = 0;
static oofact *trans_fact = 0;
static dallocator* incoming_creator = 0;

struct rec_args {
    int create_new;
    int timeout_value;
    hashmap_t out_file_map;
    hashmap_t in_file_map;
    hashmap_t out_seq_map;
    hashmap_t in_seq_map;
    hashmap_t rolling_map;
    gatekeeper *gk;
    void *oats;
    int (*oats_sender) (char *name, int name_len, databuf_t * data,
                        void *cb);
    char *directory;
    int dir_len;
};

struct rec_args *master_rec = 0;

struct rolling_log_file {
    int day;
    int file_ptr;
    char *file_name;
    long bytes_written;
};

void set_oats_vals(gatekeeper * gk, void *ci,
                   int (*oats_sender) (char *name, int name_len,
                                       databuf_t * data, void *cb))
{
    master_rec->gk = gk;
    master_rec->oats = ci;
    master_rec->oats_sender = oats_sender;
}

struct incoming_message *create_incoming_message(const char *name,
                                                 size_t name_len,
                                                 size_t num)
{
    pthread_spin_lock(&factory_mutex);
    struct incoming_message *im = get_stuff(incoming_creator);
    memcpy(im->name, name, name_len);
    im->name_len = name_len;
    im->num = num;
    pthread_spin_unlock(&factory_mutex);
    return im;
}

void init_incoming(void* mem)
{
    memset(mem, '\0', 64);
    struct incoming_message* im = mem;
    im->name = mem + 24;
}

void create_trans_databuffs(queue_t glob, queue_t q, int num)
{
    int i = 0;
    for (; i < num; ++i) {
        databuf_t *buff = databuf_alloc(1024);
        enqueue(q, buff);
    }
}

void clear_trans_db(void *db)
{
    databuf_t *buff = (databuf_t *) db;
    databuf_reset(buff);

}

void destroy_trans_db(void *d)
{
    databuf_t *buff = (databuf_t *) d;
    databuf_destroy(buff);
}

void destroy_incoming_message(struct incoming_message *im)
{
    pthread_spin_lock(&factory_mutex);
    if (im) {
        return_stuff(incoming_creator, im);
    }
    pthread_spin_unlock(&factory_mutex);
}

#define FILE_ERRCHECK(condition, fileptr)                                \
    if (!(condition)) {                                                  \
         int ferr = ferror(fileptr);                                     \
         fprintf(stderr, "%s:%d Error condition '%s' %d:%s\n",           \
                 __FILE__, __LINE__, #condition, ferr, strerror(ferr));  \
    }                                                                    \

static int record_client_out(queue_t q, hashmap_t map, hashmap_t seq_map,
                             struct rec_args *ra);
static long trans_writer(void *ptr,long nmemb,
                           int stream)
{
    char *w_off = (char *) ptr;
    long len_left = nmemb;
    long written = 0;
    long b_w = 0;

    while (written < nmemb && b_w >= 0) {
        b_w = write(stream, w_off, len_left);
        if(b_w >= 0 ) {
            written += b_w;
            w_off += b_w;
            len_left -= b_w;
        } else {
            printf("Error on stream in transwriter : %d. %d \n", errno,
                   stream);
        }
    }
    if(written > nmemb) {
        printf("Error on stream in transwriter : %ld. %ld \n", written,
               nmemb);
    }
    return written;
}

static int clean_logs(struct rec_args *ra)
{
    element_t e, temp;
    pthread_spin_lock(&seq_queue_mutex);
    e = dequeue(clean_queue);
    pthread_spin_unlock(&seq_queue_mutex);
    while (e != NULL) {
        sequence_req *m;
        m = (sequence_req *) e->d;
        size_t seq_num = 0;
        size_t local_name_len = 0;
        char *local_name = (char *) dsr_get_name(m, &local_name_len);
        int ret =
            get_map_entry(ra->out_seq_map, local_name, local_name_len,
                          (void *) &seq_num);
        if (ret == 0) {
            char *full_name = calloc(4096, 1);
            sprintf(full_name, "%s_incoming.log", local_name);
            remove(full_name);
            memset(full_name, '\0', 1000);
            sprintf(full_name, "%s_outgoing.log", local_name);
            remove(full_name);
            free(full_name);
        }
        destroy_sequence_req(m);
        temp = e;
        e = temp->next;
        pthread_spin_lock(&seq_queue_mutex);
        free_element(clean_queue, temp);
        pthread_spin_unlock(&seq_queue_mutex);
    }
    return 1;
}



static int initialize_incoming_log_file(const char *name, size_t name_len,
                                        sequence_req * sr,
                                        struct rec_args *ra)
{
    size_t namelen, in_len;
    char *in_suffix;
    struct rolling_log_file *rlf =
            calloc(sizeof(struct rolling_log_file), 1);
    char *in_file_name;
    char *file_off;
    databuf_reset(in_seq_writer);
    memset(in_seq_writer->buffer, '\0', IN_SEQ_SIZE);
    rlf->file_ptr = -1;
    in_len = 0;
    namelen = 0;
    namelen = name_len;
    in_suffix = INCOMING_SUFFIX;
    in_len += SUFFIX_LEN;
    in_file_name = calloc(1, ra->dir_len + in_len + namelen + 1);
    memcpy(in_file_name, ra->directory, ra->dir_len);
    file_off = in_file_name + ra->dir_len;
    memcpy(file_off, name, namelen);
    file_off += namelen;
    memcpy(file_off, in_suffix, in_len);
    rlf->file_ptr = open(in_file_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (rlf->file_ptr != -1 && (ra->create_new == 0)) {
        long size;
        size_t seq;
        size_t b_used;
        size = lseek(rlf->file_ptr, 0, SEEK_END);
        seq = 0;
        b_used = 0;
        if (size > 0) {
            lseek(rlf->file_ptr, 0, SEEK_SET);
            b_used =
                read(rlf->file_ptr, in_seq_writer->buffer, IN_SEQ_SIZE - 1);
            if (b_used <= 0) {
                printf("Error Initializing %s, setting sequence to 0, error: %d\n",
                       in_file_name, errno);
            } else {
                seq = atol(in_seq_writer->buffer);
            }
            b_used = 0;
            close(rlf->file_ptr);
            rlf->file_ptr = open(in_file_name, O_TRUNC | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
            if (rlf->file_ptr != -1) {
                size_t sprint_len;
                int ret_val = 0;
                ret_val = databuf_write(in_seq_writer, "%d", seq);
                sprint_len = ret_val >= 0 ? (size_t) ret_val : 0;
                trans_writer(in_seq_writer->buffer, sprint_len, 
                             rlf->file_ptr);
                no_copy_insert(ra->in_file_map, name, name_len,
                                   rlf);
            } else {
                printf("Second attempt to open file Write+ failed: %s\n",
                       in_file_name);
            }
        } else {
            size_t sprint_len;
            int ret_val = 0;
            printf
                ("0 Size file for: %s, Creating new file with sequence 0 \n",
                 in_file_name);
            close(rlf->file_ptr);
            rlf->file_ptr = open(in_file_name, O_TRUNC | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
            if (rlf->file_ptr > 0) {
                no_copy_insert(ra->in_file_map, name, name_len,
                                   rlf);
            } else {
                printf("Second attempt to open file Write+ failed: %s\n",
                       in_file_name);
            }
            b_used = 0;
            databuf_reset(in_seq_writer);
            memset(in_seq_writer->buffer, '\0', IN_SEQ_SIZE);
            ret_val = databuf_write(in_seq_writer, "%d", 0);
            sprint_len = ret_val >= 0 ? (size_t) ret_val : 0;
            trans_writer(in_seq_writer->buffer, sprint_len, rlf->file_ptr);
        }
        insert_map(ra->in_seq_map, name, name_len, &seq, sizeof(seq));
    } else if (ra->create_new) {
        size_t seq;
        size_t sprint_len;
        int ret_val = 0;
        seq = 0;
        rlf->file_ptr = open(in_file_name, O_TRUNC | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if (rlf->file_ptr > 0) {
            no_copy_insert(ra->in_file_map, name, name_len, rlf);
            databuf_reset(in_seq_writer);
            memset(in_seq_writer->buffer, '\0', IN_SEQ_SIZE);
            ret_val = databuf_write(in_seq_writer, "%d", 0);
            sprint_len = ret_val >= 0 ? (size_t) ret_val : 0;
            trans_writer(in_seq_writer->buffer, sprint_len, rlf->file_ptr);
            insert_map(ra->in_seq_map, name, name_len, &seq,
                           sizeof(seq));
        } else {
            printf("fopen failed for this: %s \n", in_file_name);
        }
    } else {
        printf("Could not create file: %s\n", in_file_name);
    }
    free(in_file_name);
    return 0;
}

#ifdef _LOG_NUM__
static int no_save_init(const char* name, size_t name_len,
        sequence_req* sr, struct rec_args* ra)
{
    struct rolling_log_file *rlf =
            calloc(sizeof(struct rolling_log_file), 1);
    size_t namelen, out_len;
    int map_ret = 0;
    char *out_suffix;
    char *out_file_name;
    char *file_off;
    databuf_reset(in_seq_writer);
    memset(in_seq_writer->buffer, '\0', IN_SEQ_SIZE);
    rlf->file_ptr = -1;
    out_len = 0;
    namelen = 0;
    namelen = name_len;
    out_suffix = OUTGOING_SUFFIX;
    out_len += SUFFIX_LEN;
    out_file_name = calloc(1, ra->dir_len + out_len + namelen + 1);
    memcpy(out_file_name, ra->directory, ra->dir_len);
    file_off = out_file_name + ra->dir_len;
    memcpy(file_off, name, namelen);
    file_off += namelen;
    memcpy(file_off, out_suffix, out_len);
    size_t seq = 0;
    if (ra->create_new) {
        rlf->file_ptr = open(out_file_name, O_TRUNC | O_CREAT |  O_RDWR, S_IRUSR | S_IWUSR);
        if (rlf->file_ptr > 0) {
            databuf_reset(in_seq_writer);
            memset(in_seq_writer->buffer, '\0', IN_SEQ_SIZE);
            int ret_val = databuf_write(in_seq_writer, "%d", 0);
            int sprint_len = ret_val >= 0 ? (size_t) ret_val : 0;
            trans_writer(in_seq_writer->buffer, sprint_len, rlf->file_ptr);
        }
    } else {
        rlf->file_ptr = open(out_file_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if (rlf->file_ptr > 0) {
            lseek(rlf->file_ptr, 0, SEEK_END);
            lseek(rlf->file_ptr, 0, SEEK_SET);
            int b_used =
                read(rlf->file_ptr, in_seq_writer->buffer, IN_SEQ_SIZE - 1);
            if (b_used <= 0) {
                printf("Error Initializing %s, setting sequence to 0, error: %d\n",
                       out_file_name, errno);
            } else {
                seq = atol(in_seq_writer->buffer);
            }
        }
    }
    if (rlf->file_ptr <= 0) {
        printf("Could not create file: %s\n", out_file_name);
    }
    map_ret =
        insert_map(ra->out_seq_map, name, name_len, &seq,
                   sizeof(size_t));
    map_ret =
        no_copy_insert(ra->out_file_map, name, name_len, rlf);
    free(out_file_name);
    return 0;
}
#else
static int set_sequence_queue(size_t * seq_num, int file,
                              sequence_req * sr)
{
    size_t bytes_read;
    size_t bytes_used;
    size_t total_bytes_read;
    size_t cum_bytes_used;
    size_t total_size;
    size_t prev_total_size;
    char *file_reader;
    char *wr_ptr;
    char *rd_ptr;
    size_t read_offset;
    size_t write_offset;
    file_reader = calloc(FILE_BUFFER_SIZE, 1);
    wr_ptr = file_reader;
    rd_ptr = file_reader;
    read_offset = 0;
    write_offset = 0;
    bytes_used = 0;
    bytes_read = 0;
    total_bytes_read = 0;
    cum_bytes_used = 0;
    total_size = FILE_BUFFER_SIZE;
    while ((bytes_read = read(file, wr_ptr, FILE_READ_CHUNK)) > 0) { //;(feof(file)) == 0) {
        
        total_bytes_read = bytes_read + (write_offset - read_offset);
        bytes_used =
            dsr_parse_data(sr, rd_ptr, total_bytes_read, cum_bytes_used,
                           seq_num);
        cum_bytes_used += bytes_used;
        if (bytes_used >= total_bytes_read) {
            write_offset = 0;
            read_offset = 0;
            wr_ptr = file_reader;
            rd_ptr = file_reader;
        } else {
            rd_ptr = rd_ptr + bytes_used;
            read_offset += bytes_used;
            write_offset += bytes_read;
            wr_ptr = wr_ptr + bytes_read;
        }
        if ((total_size - write_offset) <= FILE_READ_CHUNK) {
            char *temp;
            prev_total_size = (write_offset - read_offset) + 1;
            temp = calloc(prev_total_size, 1);
            memcpy(temp, rd_ptr, (write_offset - read_offset));
            free(file_reader);
            file_reader = NULL;
            total_size += FILE_BUFFER_SIZE;
            file_reader = calloc(total_size, 1);
            memcpy(file_reader, temp, prev_total_size);
            free(temp);
            temp = NULL;
            rd_ptr = file_reader;
            write_offset = (write_offset - read_offset);
            read_offset = 0;
            wr_ptr = file_reader + write_offset;
        }
    }
    free(file_reader);
    return 0;
}
static int initialize_outgoing_log_file(const char *name, size_t name_len,
                                        sequence_req * req,
                                        struct rec_args *ra)
{
    struct rolling_log_file *rlf =
            calloc(sizeof(struct rolling_log_file), 1);
    size_t namelen, out_len;
    char *out_suffix;
    char *out_file_name;
    char *file_off;
    databuf_reset(in_seq_writer);
    memset(in_seq_writer->buffer, '\0', IN_SEQ_SIZE);
    rlf->file_ptr = -1;
    out_len = 0;
    namelen = 0;
    namelen = name_len;
    out_suffix = OUTGOING_SUFFIX;
    out_len += SUFFIX_LEN;
    out_file_name = calloc(1, ra->dir_len + out_len + namelen + 1);
    memcpy(out_file_name, ra->directory, ra->dir_len);
    file_off = out_file_name + ra->dir_len;
    memcpy(file_off, name, namelen);
    file_off += namelen;
    memcpy(file_off, out_suffix, out_len);
    if (ra->create_new) {
        rlf->file_ptr = open(out_file_name, O_TRUNC | O_CREAT | O_APPEND | O_RDWR, S_IRUSR | S_IWUSR);
    } else {
        rlf->file_ptr = open(out_file_name, O_APPEND | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    }
    if (rlf->file_ptr > 0) {
        size_t seq = 0;
        no_copy_insert(ra->out_file_map, name, name_len, rlf);
        set_sequence_queue(&seq, rlf->file_ptr, req);
        insert_map(ra->out_seq_map, name, name_len, &seq,
                       sizeof(size_t));
    } else {
        printf("Could not create file: %s\n", out_file_name);
    }

    free(out_file_name);
    return 0;
}
#endif
static void reset_sequence_numbers(sequence_req * sr, struct rec_args *ra)
{
    size_t namelen, in_len, out_len;
    char *in_suffix, *out_suffix;
    struct rolling_log_file *in_rlf =
            calloc(sizeof(struct rolling_log_file), 1);
    struct rolling_log_file *out_rlf =
            calloc(sizeof(struct rolling_log_file), 1);
    char *in_file_name;
    char *out_file_name;
    char *file_off;
    size_t sr_name_len = 0;
    const char *sr_name = dsr_get_name(sr, &sr_name_len);
    unsigned long new_incoming = dsr_get_new_incoming(sr);
    unsigned long new_outgoing = dsr_get_new_outgoing(sr);
    databuf_reset(in_seq_writer);
    memset(in_seq_writer->buffer, '\0', IN_SEQ_SIZE);
    in_rlf->file_ptr = 0;
    out_rlf->file_ptr = 0;
    out_len = 0;
    in_len = 0;
    namelen = 0;
    namelen = sr_name_len;
    in_suffix = INCOMING_SUFFIX;
    in_len += SUFFIX_LEN;
    out_suffix = OUTGOING_SUFFIX;
    out_len += SUFFIX_LEN;
    in_file_name = calloc(1, ra->dir_len + in_len + namelen + 1);
    out_file_name = calloc(1, ra->dir_len + out_len + namelen + 1);
    memcpy(in_file_name, ra->directory, ra->dir_len);
    file_off = in_file_name + ra->dir_len;
    memcpy(file_off, sr_name, namelen);
    file_off +=  namelen;
    memcpy(file_off, in_suffix, in_len);
    if (new_incoming) {
        in_rlf->file_ptr = open(in_file_name, O_TRUNC | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if (in_rlf->file_ptr > 0) {
            size_t sprint_len;
            int ret_val = 0;
            unsigned long seq;
            seq = new_incoming;
            no_copy_insert(ra->in_file_map, sr_name, sr_name_len,
                           in_rlf );
            ret_val = databuf_write(in_seq_writer, "%lu", new_incoming);
            sprint_len = ret_val >= 0 ? (size_t) ret_val : 0;
            trans_writer(in_seq_writer->buffer, sprint_len, in_rlf->file_ptr);
            lseek(in_rlf->file_ptr, 0, SEEK_SET);
            insert_map(ra->in_seq_map, sr_name, sr_name_len, &seq,
                       sizeof(seq));
        } else {
            printf("Could not open in_file for seq reset.\n");
        }
    } else {
        initialize_incoming_log_file(sr_name, sr_name_len, sr, ra);
    }
    if (new_outgoing) {
#ifdef _LOG_NUM__
            memcpy(out_file_name, ra->directory, ra->dir_len);
            file_off = out_file_name + ra->dir_len;
            memcpy(file_off, sr_name, namelen);
            file_off += namelen;
            memcpy(file_off, out_suffix, out_len);
            out_rlf->file_ptr = open(out_file_name, O_TRUNC | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
            databuf_reset(in_seq_writer);
            databuf_write(in_seq_writer, "%lu", new_outgoing);
            trans_writer(in_seq_writer->buffer, databuf_unread(in_seq_writer), out_rlf->file_ptr);
            lseek(in_rlf->file_ptr, 0, SEEK_SET);
            insert_map(ra->out_seq_map, sr_name, sr_name_len, &new_outgoing,
                           sizeof(new_outgoing));
            no_copy_insert(ra->out_file_map, sr_name, sr_name_len,
                                   out_rlf);
#else
        memcpy(out_file_name, ra->directory, ra->dir_len);
        file_off = out_file_name + ra->dir_len;
        memcpy(file_off, sr_name, namelen);
        file_off += namelen;
        memcpy(file_off, out_suffix, out_len);
        out_rlf->file_ptr = open(out_file_name, O_TRUNC | O_CREAT | O_APPEND | O_RDWR, S_IRUSR | S_IWUSR);
        if (out_rlf->file_ptr > 0) {
            size_t seq_num = 0;
            size_t v_len = 0;
            size_t len_left;
            char *w_off;
            char *values = dsr_create_pad(sr, new_outgoing, &v_len);
            w_off = values;
            len_left = v_len;
            trans_writer(w_off, len_left, out_rlf->file_ptr);
            close(out_rlf->file_ptr);
            out_rlf->file_ptr = open(out_file_name, O_APPEND | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
            if (out_rlf->file_ptr > 0) {
                no_copy_insert(ra->out_file_map, sr_name, sr_name_len,
                               out_rlf);
                set_sequence_queue(&seq_num, out_rlf->file_ptr, sr);
                insert_map(ra->out_seq_map, sr_name, sr_name_len,
                                 &seq_num, sizeof(size_t));
            } else {
                printf("Second open of out file failed: %s!!!!\n",
                       out_file_name);
            }
            free(values);
        } else {
            printf("Could not open out_file for seq reset.\n");
        }
#endif
    } else {
#ifdef _LOG_NUM__
        no_save_init(sr_name, sr_name_len, sr, ra);
#else
        initialize_outgoing_log_file(sr_name, sr_name_len, sr, ra);
#endif
    }
    free(out_file_name);
    free(in_file_name);
}

static int grab_resend_messages(struct rec_args *ra)
{
    element_t e, temp;
    pthread_spin_lock(&seq_queue_mutex);
    e = dequeue(resend_requests);
    pthread_spin_unlock(&seq_queue_mutex);
    clean_logs(ra);
    while (e != NULL) {
        resend_req *rr = (resend_req *) e->d;
        if (rr) {
            long ret = 0;
            struct rolling_log_file *rlf;
            size_t name_len = 0;
            const char *name = drr_get_name(rr, &name_len);

            ret = (long) get_map_entry(ra->out_file_map, name, name_len,
                                       (void *) &rlf);
            if (ret == 0) {
                drr_signal_sleeper(rr);
            } else {
                drr_create_resend_out(rr, rlf->file_ptr);
                drr_signal_sleeper(rr);
            }
        }
        temp = e;
        e = temp->next;
        pthread_spin_lock(&seq_queue_mutex);
        free_element(resend_requests, temp);
        pthread_spin_unlock(&seq_queue_mutex);
    }
    return 0;
}

static int grab_incoming_messages(queue_t q, hashmap_t map,
                                  hashmap_t seq_map, struct rec_args *ra)
{
    element_t e, temp;
    databuf_reset(in_seq_writer);
    memset(in_seq_writer->buffer, '\0', IN_SEQ_SIZE);
    pthread_spin_lock(&queue_mutex);
    e = dequeue(q);
    pthread_spin_unlock(&queue_mutex);
    clean_logs(ra);
    while (e != NULL) {
        struct incoming_message *m;
        struct rolling_log_file *rlf;
        //int file_ptr;
        long init_in = 0;
        long *l_in = &init_in;
        int ret;
        size_t seq;
        seq = 0;
        databuf_reset(in_seq_writer);
        memset(in_seq_writer->buffer, '\0', IN_SEQ_SIZE);
        m = (struct incoming_message *) e->d;
        ret = get_map_entry(map, m->name, m->name_len, (void *) &rlf);
        if (ret > 0) {
            size_t w_len;
            int w_sp_len = 0;
            ret =
                get_map_entry(ra->in_seq_map, m->name, m->name_len,
                              (void *) &l_in);
            if (ret > 0) {
                seq += *l_in;
            } else {
                size_t b_used = 0;
                //lseek(file_ptr, 0, SEEK_SET);
                b_used =
                    read(rlf->file_ptr, in_seq_writer->buffer, (size_t) 
                          (size_t) (IN_SEQ_SIZE - 1));
                if (b_used <= 0) {
                    printf
                        ("INSERTING MESSAGE read error read this many bytes: %lu, and got this result: %lu for connection: %s seq: %lu \n",
                         b_used, seq, m->name, m->num);
                } else {
                    printf
                        ("Inserting this seq: %s, for this connection: %s, this many new seq: %lu\n",
                         in_seq_writer->buffer, m->name, m->num);
                    seq += atol(in_seq_writer->buffer);
                }
            }
            seq += m->num;
            w_sp_len = databuf_write(in_seq_writer, "%lu", seq);
            w_len = w_sp_len >= 0 ? (size_t) w_sp_len : 0;
            lseek(rlf->file_ptr, 0, SEEK_SET);
            trans_writer(in_seq_writer->buffer, w_len, rlf->file_ptr);
            remove_from_map(ra->in_seq_map, m->name, m->name_len);
            ret =
                insert_map(ra->in_seq_map, m->name, m->name_len, &seq,
                           sizeof(seq));
        } else {
            printf
                ("Error Could not find Incoming Sequence File for: %s \n",
                 m->name);
        }
        temp = e;
        e = temp->next;
        destroy_incoming_message(m);
        temp->d = NULL;
        pthread_spin_lock(&queue_mutex);
        free_element(q, temp);
        pthread_spin_unlock(&queue_mutex);
    }
    return 0;
}
static int fill_sequences(struct rec_args *ra)
{
    element_t e, temp;
    pthread_spin_lock(&seq_queue_mutex);
    e = dequeue(seq_request);
    pthread_spin_unlock(&seq_queue_mutex);
    clean_logs(ra);
    grab_incoming_messages(incoming_queue, ra->in_file_map,
                                   ra->in_seq_map, ra);
    record_client_out(client_out_queue, ra->out_file_map,
                              ra->out_seq_map, ra);
    while (e != NULL) {
        sequence_req *m;
        int ret;
        size_t init_in = 0;
        size_t *l_in = &init_in;
        size_t pad_seq = 0;
        size_t *seq_num = &pad_seq;
        const char *local_name = 0;
        size_t local_name_len = 0;
        unsigned long new_outgoing = 0;
        unsigned long new_incoming = 0;
        int create_new = 0;
        m = (sequence_req *) e->d;
        local_name = dsr_get_name(m, &local_name_len);
        new_outgoing = dsr_get_new_outgoing(m);
        new_incoming = dsr_get_new_incoming(m);
        create_new = dsr_get_create_new(m);
        if (new_outgoing || new_incoming) {
            ra->create_new = create_new;
            if (new_outgoing > 0) {
                remove_from_map(ra->out_seq_map, local_name,
                                local_name_len);
            }
            if (new_incoming > 0) {
                remove_from_map(ra->in_seq_map, local_name,
                                local_name_len);
            }
            reset_sequence_numbers(m, ra);
        }
        ret =
            get_map_entry(ra->out_seq_map, local_name, local_name_len,
                          (void *) &seq_num);
        if (ret == 0) {
            printf("Creating new seq for: %s \n", local_name);
            ra->create_new = dsr_get_create_new(m);     //m->create_new;
            initialize_incoming_log_file(local_name, local_name_len, m,
                                         ra);
#ifdef _LOG_NUM__
            no_save_init(local_name, local_name_len, m, ra);
#else
            initialize_outgoing_log_file(local_name, local_name_len, m,
                                         ra);
#endif
            ret =
                get_map_entry(ra->out_seq_map, local_name, local_name_len,
                              (void *) &seq_num);
        }
        pad_seq = *seq_num;
        dsr_set_outgoing(m, pad_seq);
        ret =
            get_map_entry(ra->in_seq_map, local_name, local_name_len,
                          (void *) &l_in);
        if (ret != 0) {
            dsr_set_incoming(m, *l_in);
        } else {
            dsr_set_incoming(m, 0);
        }
        dsr_signal_cond(m);
        temp = e;
        e = temp->next;
        pthread_spin_lock(&seq_queue_mutex);
        free_element(seq_request, temp);
        pthread_spin_unlock(&seq_queue_mutex);
    }
    return 0;
}

static void clean_up(queue_t q)
{
    element_t e, temp;
    pthread_spin_lock(&queue_mutex);
    e = dequeue(q);
    while (e != NULL) {
        temp = e;
        e = temp->next;
        free(temp->d);
        temp->d = NULL;
        free_element(q, temp);
    }
    pthread_spin_unlock(&queue_mutex);
}


int day_check(int day)
{
    time_t rawtime;
    struct tm *ti;
    struct tm lm;
    (void) time(&rawtime);
    ti = gmtime_r(&rawtime, &lm);
    ti = &lm;
    return ti->tm_mday == day;
}

static void reopen_log(struct rolling_log_file *rlf)
{

    int ret = 0;
    ret = close(rlf->file_ptr);
    if (ret != 0) {
        printf("Error closing this file: %s \n", rlf->file_name);
    }
    rlf->file_ptr = open(rlf->file_name, O_APPEND | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (rlf->file_ptr) {
    } else {
        printf("Unable to reopen: %s \n", rlf->file_name);
    }
}

static void internal_debug_roller(struct rec_args *ra, char *data, int len,
                                  char *prev, int prev_len)
{
    int ret = 0;
    struct rolling_log_file *rlf;
    ret = get_map_entry(ra->rolling_map, "DEBUG", 5, (void *) &rlf);
    if (ret > 0) {
        size_t len_left;
        char *w_off;
        long eof;
        len_left = len;
        eof = lseek(rlf->file_ptr, 0, SEEK_END);
        if (eof < ROLL_FILE_SIZE && day_check(rlf->day)) {
            w_off = data;
            if (trans_writer("ERROR-- ", 8, rlf->file_ptr) < 8) {
                reopen_log(rlf);
            }
            if (trans_writer(prev, prev_len, rlf->file_ptr) < prev_len) {
                reopen_log(rlf);
            }
            if (trans_writer(w_off, len_left, rlf->file_ptr) < len_left) {
                reopen_log(rlf);
                trans_writer(w_off, len_left, rlf->file_ptr);
            }
        }
    }
}

static void gen_rolling_log(struct rec_args* ra, hashmap_t rolling_map, char *file_name,
                            int file_name_len, char *data, int len)
{
    struct rolling_log_file *rlf;
    int ret = get_map_entry(rolling_map, file_name, file_name_len,
                               (void *) &rlf);
    if (ret > 0) {
        size_t len_left;
        char *w_off;
        long eof;
        len_left = len;
        eof = lseek(rlf->file_ptr, 0, SEEK_END);

        if (eof < 0) {
            printf("Error on stream after ftell: %d. %lu \n", errno,
                   (unsigned long) rlf->file_ptr);
        }
        if (eof < ROLL_FILE_SIZE && day_check(rlf->day)) {
            w_off = data;
            if (trans_writer(w_off, len_left, rlf->file_ptr) < len_left) {
                reopen_log(rlf);
                trans_writer(w_off, len_left, rlf->file_ptr);
            }
        } else {
            time_t rawtime;
            struct tm *ti;
            size_t year;
            int sprinter = 0;
            size_t month;
            databuf_t *mblk = databuf_alloc(file_name_len + 800);
            struct tm lm;
            close(rlf->file_ptr);
            printf("Closed file: %lu\n ",
                   (unsigned long) rlf->file_ptr);
            (void) time(&rawtime);
            ti = gmtime_r(&rawtime, &lm);
            ti = &lm;
            year = (size_t) (1900 + ti->tm_year);
            month = (size_t) (1 + ti->tm_mon);
            databuf_memcpy(mblk, file_name, file_name_len);
            sprinter =
                databuf_write(mblk,
                              "Log%04lu%02lu%02d.%02d.%02d.%02d.LOG",
                              year, month, ti->tm_mday,
                              ti->tm_hour, ti->tm_min, ti->tm_sec);
            free(rlf->file_name);
            rlf->file_name = calloc(1, ra->dir_len + sprinter + file_name_len + 1);
            memcpy(rlf->file_name, ra->directory, ra->dir_len);
            memcpy(rlf->file_name + ra->dir_len, mblk->buffer, databuf_unread(mblk));
            rlf->file_ptr = open(rlf->file_name, O_APPEND | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
            if (rlf->file_ptr) {
                rlf->day = ti->tm_mday;
                //setbuf(rlf->file_ptr, NULL);
                sprinter =
                    remove_from_map(rolling_map, file_name, file_name_len);
                (void) no_copy_insert(rolling_map, file_name,
                                      file_name_len, rlf);
                printf("Added file: %lu\n ",
                       (unsigned long) rlf->file_ptr);
                w_off = data;
                len_left = len;
                trans_writer(w_off, len_left, rlf->file_ptr);
            } else {
                printf("Unable to create this file: {%s}\n", file_name);
            }
            databuf_destroy(mblk);
        }
    } else {
        time_t rawtime;
        struct tm *ti;
        size_t year;
        size_t month;
        databuf_t *mblk = databuf_alloc(file_name_len + 80);
        size_t len_left;
        char *w_off = data;
        int sprinter = 0;
        struct tm lm;
        struct rolling_log_file *rlf =
            calloc(sizeof(struct rolling_log_file), 1);
        (void) time(&rawtime);
        ti = gmtime_r(&rawtime, &lm);
        ti = &lm;
        //printf("Opening this file: %s \n", file_name);
        year = (size_t) (1900 + ti->tm_year);
        month = (size_t) (1 + ti->tm_mon);
        databuf_memcpy(mblk, file_name, file_name_len);
        sprinter =
            databuf_write(mblk, "Log%04lu%02lu%02d.LOG",
                          year, month, ti->tm_mday);
        rlf->file_name = calloc(1, ra->dir_len + sprinter + file_name_len + 1);
        memcpy(rlf->file_name, ra->directory, ra->dir_len);
        memcpy(rlf->file_name + ra->dir_len, mblk->buffer, databuf_unread(mblk));
        rlf->file_ptr = open(rlf->file_name, O_APPEND | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if (rlf->file_ptr) {
            rlf->day = ti->tm_mday;
            no_copy_insert(rolling_map, file_name,
                                       file_name_len, rlf);
            //printf("Added file: %lu, name: %s\n ",
             //      (unsigned long) rlf->file_ptr, file_name);
            w_off = data;
            len_left = len;
            trans_writer(w_off, len_left, rlf->file_ptr);
        } else {
            printf("Unable to create this file: {%s}\n", file_name);
        }
        databuf_destroy(mblk);
    }
}

static void insert_rolling_log(struct rec_args *ra)
{
    element_t e, temp;
    pthread_spin_lock(&queue_mutex);
    e = dequeue(rolling_queue);
    pthread_spin_unlock(&queue_mutex);
    while (e != NULL) {
        struct rolling_log_message *m = NULL;
        m = (struct rolling_log_message *) e->d;
        if (m != NULL) {
            gen_rolling_log(ra, ra->rolling_map, m->file_name,
                            m->file_name_len, m->data, m->len);
            gen_rolling_log(ra, ra->rolling_map, "MasterAudit", 11,
                                m->data, m->len);
        }
        temp = e;
        e = temp->next;
        if (m) {
            free(m->data);
            free(m->name);
            free(m->file_name);
            free(m);
            m = 0;
        }
        temp->d = NULL;
        pthread_spin_lock(&queue_mutex);
        free_element(rolling_queue, temp);
        pthread_spin_unlock(&queue_mutex);
    }

}

static int tr_set_time_stamp(char *mblk)
{
    int retval = -1;
    struct timeb rawtime;
    ftime(&rawtime);
    struct tm lm;
    if (gmtime_r(&rawtime.time, &lm) != 0
        && tm_to_str_millis(&lm, rawtime.millitm, mblk,
                            TIMESTAMP_LEN_WITH_MILLIS) == 0) {
        retval = TIMESTAMP_LEN_WITH_MILLIS;
    }
    return retval;
}

static int tr_prepend_time(const char *wr_ptr)
{
    char *d_off = (char *) wr_ptr;
    int s_len = 0;
    int t_len = 0;
    s_len = tr_set_time_stamp(d_off);
    if (s_len > 0) {
        d_off += s_len;
        t_len += s_len;
    }
    memcpy(d_off, "  ", 2);
    d_off += 2;
    t_len += 2;
    return t_len;

}
static void pre_process_rolling_log(struct rec_args* ra, char* file_name,
                                    int file_name_len,
                                    databuf_t* buff, int len)
{
    char* end = "\n";
    char *wr_ptr = calloc(len + 100, 1);
    int i_len = tr_prepend_time(wr_ptr);
    memcpy((wr_ptr + i_len), buff->rd_ptr, len);
    memcpy(wr_ptr + (i_len + len), end, 1);
    gen_rolling_log(ra, ra->rolling_map, file_name, file_name_len,
                    wr_ptr, (i_len + len +1));
    free(wr_ptr);
}

static void restart_file(struct rec_args *ra, size_t write_ret,
                         size_t len_left, struct rolling_log_file* rlf,
                         char *name, int name_len, char *w_off)
{
    char *out_file_name = calloc(1, ra->dir_len + SUFFIX_LEN + name_len + 1);
    char *cp_off = out_file_name;
    memcpy(out_file_name, ra->directory, ra->dir_len);
    cp_off += ra->dir_len;
    memcpy(cp_off, name, name_len);
    cp_off += name_len;
    memcpy(cp_off, OUTGOING_SUFFIX, SUFFIX_LEN);
    remove_from_map(ra->out_file_map, name, name_len);
    close(rlf->file_ptr);
    rlf->file_ptr = open(out_file_name, O_APPEND | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (rlf->file_ptr > 0) {
        trans_writer(w_off + write_ret, (len_left - write_ret),
                     rlf->file_ptr);
        no_copy_insert(ra->out_file_map, name, name_len, rlf);
    }
}

#ifdef _LOG_NUM__
static void insert_seq_out(char* name, int name_len, long seq, 
        struct rec_args *ra)
{
    struct rolling_log_file *rlf;
    int ret = get_map_entry(ra->out_file_map, name, name_len, (void *) &rlf);
    if (ret <= 0) {
        size_t out_len;
        char *out_suffix;
        char *file_off;
        char *out_file_name;
        struct rolling_log_file *rlf =
            calloc(sizeof(struct rolling_log_file), 1);
        char er_mess[256];
        int er_len = sprintf
            (er_mess, "Recording Outgoing message: Error Could not find File for: %s \n",
             name);
        internal_debug_roller(ra, er_mess, er_len,
                              "NO QUEUE ", 9);
        out_len = 0;
        out_suffix = OUTGOING_SUFFIX;
        out_len += SUFFIX_LEN;
        out_file_name = calloc(1, ra->dir_len + out_len + name_len + 1);
        memcpy(out_file_name, ra->directory, ra->dir_len);
        file_off = out_file_name + ra->dir_len;
        memcpy(file_off, name, (name_len));
        file_off += name_len;
        memcpy(file_off, out_suffix, out_len);
        rlf->file_ptr = open(out_file_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if(rlf->file_ptr <= 0) {
            int er = errno;
            char emessage[256];
            int emlen = sprintf
                (emessage, "Unable to open file for: %s filename: %s, errno: %d, name len: %d \n",
                 name, out_file_name, er, name_len);
            internal_debug_roller(ra, emessage, emlen,
                              "NO QUEUE ", 9);
            free(out_file_name);
            return;
        }
        free(out_file_name);
    }
    databuf_reset(in_seq_writer);
    databuf_write(in_seq_writer, "%lu", seq);
    lseek(rlf->file_ptr, 0, SEEK_SET);
    trans_writer(in_seq_writer->buffer, databuf_unread(in_seq_writer), rlf->file_ptr);
    remove_from_map(ra->out_seq_map, name, name_len);
    insert_map(ra->out_seq_map, name, name_len, &seq,
                           sizeof(seq));
}
static long ts_get_sequence(char *mblk, size_t len)
{
    char* d_off = mblk;
    int l_len = (int) len;
    long found = 0;
    GET_OFF(d_off, l_len, "\00134=", 4, found);
    if(found) {
        found = atol(d_off + 4);
    }
    return found;
}
#endif
static void set_message_in_out_file(char *name, int name_len,
                                    databuf_t * data, hashmap_t map,
                                    hashmap_t seq_map, struct rec_args *ra)
{
    struct rolling_log_file *rlf;
    size_t ret;
    size_t l_seq = 0;
    size_t *old_seq = &l_seq;
    size_t write_ret = 0;
    ret = get_map_entry(map, name, name_len, (void *) &rlf);
    if (ret > 0) {
        size_t len_left;
        char *w_off;
        w_off = data->rd_ptr;
        len_left = databuf_unread(data);
        write_ret = trans_writer(w_off, len_left, rlf->file_ptr);
        if (write_ret < len_left) {
            restart_file(ra, write_ret, len_left, rlf, name,
                         name_len, w_off);
        }
    } else {
        size_t out_len;
        char *out_suffix;
        char *file_off;
        char *out_file_name;
        struct rolling_log_file *rlf =
            calloc(sizeof(struct rolling_log_file), 1);
        printf
            ("Recording Outgoing message: Error Could not find File for: %s \n",
             name);
        internal_debug_roller(ra, data->buffer, databuf_unread(data),
                              "NO QUEUE ", 9);
        out_len = 0;
        out_suffix = OUTGOING_SUFFIX;
        out_len += SUFFIX_LEN;
        out_file_name = calloc(1, ra->dir_len + out_len + name_len + 1);
        memcpy(out_file_name, ra->directory, ra->dir_len);
        file_off = out_file_name + ra->dir_len;
        memcpy(file_off, name, (name_len));
        file_off += name_len;
        memcpy(file_off, out_suffix, out_len);
        rlf->file_ptr = open(out_file_name, O_APPEND | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if (rlf->file_ptr > 0) {
            size_t len_left;
            //long tell_ret = 0;
            char *w_off;
            w_off = data->rd_ptr;
            len_left = databuf_unread(data);
            write_ret = trans_writer(w_off,  len_left, rlf->file_ptr);
            if (write_ret < len_left) {
                restart_file(ra, write_ret, len_left, rlf, name,
                             name_len, w_off);
            } else {
                //tell_ret = ftell(out_file);
                //len_left = tell_ret >= 0 ? (size_t) tell_ret : 0;
                ret =
                    no_copy_insert(ra->out_file_map, name,
                                   name_len, rlf);
            }
        } else {
            int er = errno;
            printf
                ("Unable to open file for: %s filename: %s, errno: %d, name len: %d \n",
                 name, out_file_name, er, name_len);
        }
        free(out_file_name);
    }
    ret = get_map_entry(seq_map, name, name_len, (void *) &old_seq);
    if (ret > 0) {
        size_t seq = *old_seq;
        ++seq;
        remove_from_map(seq_map, name, name_len);
        ret = insert_map(seq_map, name, name_len, &seq, sizeof(size_t));
    }
}

static int record_raw(queue_t q, hashmap_t map, hashmap_t seq_map,
                      struct rec_args *ra)
{
    element_t e, temp;
    pthread_spin_lock(&queue_mutex);
    e = dequeue(q);
    pthread_spin_unlock(&queue_mutex);
    while (e != NULL) {
        if (e->d != NULL) {
            databuf_t *buff = (databuf_t *) e->d;
            int name_len = 0;
            char inc_out = 0;
            memcpy(&inc_out, buff->rd_ptr, sizeof(char));
            buff->rd_ptr += sizeof(char);
            memcpy(&name_len, buff->rd_ptr, sizeof(int));
            buff->rd_ptr += sizeof(int);
            char *name = buff->rd_ptr;//buffer + sizeof(int);
            buff->rd_ptr += name_len;
            pre_process_rolling_log(ra, name, name_len,
                            buff, databuf_unread(buff));
            pre_process_rolling_log(ra, "MasterAudit", 11,
                            buff, databuf_unread(buff));
            if(inc_out) {
#ifdef _LOG_NUM__
                long seq = ts_get_sequence(buff->rd_ptr, databuf_unread(buff));
                insert_seq_out(name, name_len, seq,ra);
#else
                set_message_in_out_file(name, name_len,buff, ra->out_file_map,
                                        ra->out_seq_map, ra);
#endif
            }
            pthread_spin_lock(&factory_mutex);
            destroy_elem(byte_logger_fact, buff);
            pthread_spin_unlock(&factory_mutex);
        }
        temp = e;
        e = temp->next;
        temp->d = NULL;
        temp->next = 0;
        pthread_spin_lock(&queue_mutex);
        free_element(raw_queue, temp);
        pthread_spin_unlock(&queue_mutex);
    }
    return 1;
}

static int record_client_out(queue_t q, hashmap_t map, hashmap_t seq_map,
                             struct rec_args *ra)
{
    element_t e, temp;
    pthread_spin_lock(&queue_mutex);
    e = dequeue(q);
    pthread_spin_unlock(&queue_mutex);
    while (e != NULL) {
        struct dart_order_obj *m;
        if (e->d != NULL) {
            m = (struct dart_order_obj *) e->d;
            int sender_len = 0;
            char *name = get_value_from_copy(m, ROM_SENDER, &sender_len);
            pthread_spin_lock(&factory_mutex);
            databuf_t* data = (databuf_t *) get_obj(byte_logger_fact);
            set_copied_doo_in_buf(m, data);
            pthread_spin_unlock(&factory_mutex);
            if (sender_len > 0) {
                set_message_in_out_file(name,
                                        m->positions[ROM_SENDER].iov_len,
                                        data, map, seq_map, ra);
                pre_process_rolling_log(ra, name, m->positions[ROM_SENDER].iov_len,
                                        data, databuf_unread(data));
                pre_process_rolling_log(ra, "MasterAudit", 11,
                                data, databuf_unread(data));
            } else {
                internal_debug_roller(ra, "\n", 1, " ", 1);
                internal_debug_roller(ra, m->head->data->buffer,
                                      databuf_unread(m->head->data),
                                      "\nWe Could not find the Sender \n", 31);
            }
            pthread_spin_lock(&factory_mutex);
            reset_doj(trans_fact, m);
            destroy_elem(byte_logger_fact, data);
            pthread_spin_unlock(&factory_mutex);
        }
        temp = e;
        e = temp->next;
        m = NULL;
        temp->d = NULL;
        temp->next = 0;
        pthread_spin_lock(&queue_mutex);
        free_element(client_out_queue, temp);
        pthread_spin_unlock(&queue_mutex);
    }
    return 0;
}

static int send_oats_messages(queue_t q, hashmap_t map, hashmap_t seq_map,
                              struct rec_args *ra)
{
    element_t e, temp;
    pthread_spin_lock(&queue_mutex);
    e = dequeue(q);
    pthread_spin_unlock(&queue_mutex);
    while (e != NULL) {
        struct dart_order_obj *m;
        if (e->d != NULL) {
            m = (struct dart_order_obj *) e->d;
            databuf_t *data = 0;
            pthread_spin_lock(&factory_mutex);
            data = (databuf_t *) get_obj(byte_logger_fact);
            set_copied_doo_in_buf(m, data);
            pthread_spin_unlock(&factory_mutex);
            set_message_in_out_file("OATS", 4, data, map, seq_map, ra);
            if (master_rec->oats && ((con_interface*)master_rec->oats)->can_send_orders) {
                master_rec->oats_sender("OATS", 4, data, master_rec->oats);
            }
            pthread_spin_lock(&factory_mutex);
            reset_doj(trans_fact, m);
            destroy_elem(byte_logger_fact, data);
            pthread_spin_unlock(&factory_mutex);
        }
        temp = e;
        e = temp->next;
        m = NULL;
        temp->d = NULL;
        temp->next = 0;
        pthread_spin_lock(&queue_mutex);
        free_element(oats_queue, temp);
        pthread_spin_unlock(&queue_mutex);
    }
    return 0;
}

void stop_logging()
{
    pthread_mutex_lock(&init_mutex);
    keep_running = 0;
    pthread_mutex_unlock(&init_mutex);
}

static int should_run()
{
    int ret = 1;
    pthread_mutex_lock(&init_mutex);
    ret = keep_running;
    pthread_mutex_unlock(&init_mutex);
    return ret;
}
static void clean_seq_map(hashmap_t map)
{
    size_t *rlf;
    char *key = 0;
    int key_len = 0;
    hashmap_iterator_t hi = hashmap_iterator_begin(map);
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                                     (void *) &rlf);
    while (valid) {
        free(rlf);
        rlf = 0;
        valid = hashmap_iterator_next(map, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &rlf);
    }
    hashmap_iterator_destroy(hi);
    delete_map(map);
}
static void clean_pointers_from_map(hashmap_t map)
{
    struct rolling_log_file *rlf;
    char *key = 0;
    int key_len = 0;
    hashmap_iterator_t hi = hashmap_iterator_begin(map);
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                                     (void *) &rlf);
    while (valid) {
        free(rlf->file_name);
        free(rlf);
        rlf = 0;
        valid = hashmap_iterator_next(map, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &rlf);
    }
    hashmap_iterator_destroy(hi);
    delete_map(map);
}

void *record_messages(void *args)
{
    struct rec_args *ra = (struct rec_args *) args;
    keep_running = 1;
    int l_is_empty = 1;
    struct timespec sleep_val = {0};
    sleep_val.tv_sec = 0;
    sleep_val.tv_nsec = NANO_MULT;
    while (should_run()) {
        pthread_spin_lock(&seq_queue_mutex);
        l_is_empty = is_empty(clean_queue);
        pthread_spin_unlock(&seq_queue_mutex);
        if (!l_is_empty) {
            clean_logs(ra);
        }
        pthread_spin_lock(&seq_queue_mutex);
        l_is_empty = is_empty(seq_request);
        pthread_spin_unlock(&seq_queue_mutex);
        if (!l_is_empty) {
            fill_sequences(ra);
        }
        pthread_spin_lock(&seq_queue_mutex);
        l_is_empty = is_empty(resend_requests);
        pthread_spin_unlock(&seq_queue_mutex);
        if (!l_is_empty) {
            grab_resend_messages(ra);
        }
        pthread_spin_lock(&queue_mutex);
        l_is_empty = is_empty(oats_queue);
        pthread_spin_unlock(&queue_mutex);
        if (!l_is_empty) {
            send_oats_messages(oats_queue, ra->out_file_map,
                               ra->out_seq_map, ra);
        }
        pthread_spin_lock(&queue_mutex);
        l_is_empty = is_empty(raw_queue);
        pthread_spin_unlock(&queue_mutex);
        if (!l_is_empty) {
            record_raw(raw_queue, ra->out_file_map, ra->out_seq_map, ra);
        }
        pthread_spin_lock(&queue_mutex);
        l_is_empty = is_empty(client_out_queue);
        pthread_spin_unlock(&queue_mutex);
        if (!l_is_empty) {
            record_client_out(client_out_queue, ra->out_file_map,
                              ra->out_seq_map, ra);
        }
        pthread_spin_lock(&queue_mutex);
        l_is_empty = is_empty(incoming_queue);
        pthread_spin_unlock(&queue_mutex);
        if (!l_is_empty) {
            grab_incoming_messages(incoming_queue, ra->in_file_map,
                                   ra->in_seq_map, ra);
        }

        pthread_spin_lock(&queue_mutex);
        l_is_empty = is_empty(rolling_queue);
        pthread_spin_unlock(&queue_mutex);
        if (!l_is_empty) {
            insert_rolling_log(ra);
        }
        nanosleep(&sleep_val, NULL);
    }
    is_init = 0;
    clean_up(incoming_queue);
    clean_up(raw_queue);
    clean_up(client_out_queue);
    clean_up(oats_queue);
    clean_up(resend_requests);
    clean_up(seq_request);
    clean_up(rolling_queue);
    pthread_spin_lock(&queue_mutex);
    clean_seq_map(ra->in_seq_map);
    clean_seq_map(ra->out_seq_map);
    clean_pointers_from_map(ra->rolling_map);
    clean_pointers_from_map(ra->out_file_map);
    clean_pointers_from_map(ra->in_file_map);
    pthread_spin_unlock(&queue_mutex);
    pthread_spin_destroy(&queue_mutex);
    pthread_spin_destroy(&seq_queue_mutex);
    pthread_spin_destroy(&factory_mutex);
    free(ra);
    return NULL;
}

rec_args*  init_recorder(int allocation, int map_size, char* directory,
                         int dir_len)
{
    pthread_mutex_lock(&init_mutex);
    if (is_init) {
        //do nothing
    } else {
        incoming_queue = create_sized_queue(map_size);
        client_out_queue = create_sized_queue(map_size);
        resend_requests = create_sized_queue(map_size);
        rolling_queue = create_sized_queue(map_size);
        clean_queue = create_sized_queue(map_size);
        seq_request = create_sized_queue(map_size);
        in_seq_writer = databuf_alloc(IN_SEQ_SIZE);
        raw_queue = create_sized_queue(map_size);
        trans_fact = create_order_obj_factory(allocation);
        incoming_creator = create_mem_glob(64, init_incoming);
        byte_logger_fact = create_dart_obj_fact(allocation, destroy_trans_db,
                                                create_trans_databuffs,
                                                clear_trans_db);
        pthread_spin_init(&queue_mutex, PTHREAD_PROCESS_PRIVATE);
        pthread_spin_init(&seq_queue_mutex, PTHREAD_PROCESS_PRIVATE);
        pthread_spin_init(&factory_mutex, PTHREAD_PROCESS_PRIVATE);
        master_rec = calloc(sizeof(struct rec_args), 1);
        master_rec->create_new = 0;
        master_rec->timeout_value = 30;
        oats_queue = create_sized_queue(map_size);
        master_rec->gk = 0;
        master_rec->oats = 0;
        master_rec->oats_sender = 0;
        dest_id_map = create_map(map_size);
        master_rec->out_file_map = create_map(map_size);
        master_rec->in_file_map = create_map(map_size);
        master_rec->in_seq_map = create_map(map_size);
        master_rec->out_seq_map = create_map(map_size);
        master_rec->rolling_map = create_map(map_size);
        master_rec->directory = calloc(1, dir_len + 1);
        memcpy(master_rec->directory, directory, dir_len);
        master_rec->dir_len = dir_len;
        is_init = 1;
    }
    pthread_mutex_unlock(&init_mutex);
    return master_rec;
}

void reset_default_directory(struct rec_args* rc, char* dir, int dir_len)
{
    if(rc->dir_len > 0) {
        free(rc->directory);
    }
    rc->directory = calloc(1, dir_len + 1);
    memcpy(rc->directory, dir, dir_len);
    rc->dir_len = dir_len;
}

void record_inbound_message(struct incoming_message *m)
{
    if (is_init) {
        pthread_spin_lock(&queue_mutex);
        enqueue(incoming_queue, m);
        pthread_spin_unlock(&queue_mutex);
    } else {
        printf("Called before init: \n");
    }
}

void dart_rolling_log(struct rolling_log_message *m)
{
    if (is_init) {
        pthread_spin_lock(&queue_mutex);
        enqueue(rolling_queue, m);
        pthread_spin_unlock(&queue_mutex);
    } else {
        printf("Called before init: \n");
    }

}

void clean_seq(sequence_req * sr)
{
    if (is_init) {
        pthread_spin_lock(&seq_queue_mutex);
        enqueue(clean_queue, sr);
        pthread_spin_unlock(&seq_queue_mutex);
    }
}

void get_sequence_numbers(sequence_req * sr)
{
    if (is_init) {
        /* I need to grab the ready signal mutex before I signal to
         * ensure that the signal is not fired before I am waiting. */
        dsr_lock_con_mutex(sr);
        pthread_spin_lock(&seq_queue_mutex);
        enqueue(seq_request, sr);
        pthread_spin_unlock(&seq_queue_mutex);
        /* Now wait on for return. */
        dsr_wait_on_cond(sr);
    } else {
        printf("Called before init: \n");
    }
}

void get_resend_request(resend_req * rr)
{
    if (is_init) {
        /* I need to lock this mutex in order to ensure that I do not
         * miss the signal. */
        if (drr_lock_update_mutex(rr) == 0) {
            if (pthread_spin_lock(&seq_queue_mutex) == 0) {
                enqueue(resend_requests, rr);
                pthread_spin_unlock(&seq_queue_mutex);
                /* Now wait on for return. */
                drr_wait_on_cond(rr);
            } else {
                drr_unlock_update_mutex(rr);
            }
        }
    } else {
        printf("Called before init: \n");
    }

}

void record_raw_message(char *name, int name_len, char *bytes, int b_len, char rec_out_seq)
{
    if (is_init) {
        pthread_spin_lock(&factory_mutex);
        databuf_t *buff = (databuf_t *) get_obj(byte_logger_fact);
        pthread_spin_unlock(&factory_mutex);
        memcpy(buff->wr_ptr, &rec_out_seq, sizeof(char));
        buff->wr_ptr += sizeof(char);
        memcpy(buff->wr_ptr, &name_len, sizeof(int));
        buff->wr_ptr += sizeof(int);
        memcpy(buff->wr_ptr, name, name_len);
        buff->wr_ptr += name_len;
        databuf_memcpy(buff, bytes, b_len);
        pthread_spin_lock(&queue_mutex);
        enqueue(raw_queue, buff);
        pthread_spin_unlock(&queue_mutex);
    }
}

void record_client_outbound(dart_order_obj * doj)
{
    if (is_init) {
        pthread_spin_lock(&factory_mutex);
        dart_order_obj *ldoj = get_doj(trans_fact);
        rom_style_reset(ldoj);
        copy_dart_order_obj(doj, ldoj);
        pthread_spin_unlock(&factory_mutex);
        pthread_spin_lock(&queue_mutex);
        enqueue(client_out_queue, ldoj);
        pthread_spin_unlock(&queue_mutex);
    }
}

static int set_time_stamp(char *mblk)
{
    int retval = -1;
    struct timeb rawtime;
    ftime(&rawtime);
    struct tm lm;
    if (gmtime_r(&rawtime.time, &lm) != 0
        && tm_to_str_millis(&lm, rawtime.millitm, mblk,
                            TIMESTAMP_LEN_WITH_MILLIS) == 0) {
        retval = TIMESTAMP_LEN_WITH_MILLIS;
    }
    return retval;
}

#ifdef _SPREAD_N_READY_
static void insert_children(dart_order_obj* doj) 
{
    children* lch = doj->childs;
    while(lch != NULL) {
        dart_order_obj* child = (dart_order_obj*)lch->payload;
        pthread_spin_lock(&factory_mutex);
        dart_order_obj *lcdoj = get_doj(trans_fact);
        rom_style_reset(lcdoj);
        copy_dart_order_obj(child, lcdoj);
        pthread_spin_unlock(&factory_mutex);
        pthread_spin_lock(&queue_mutex);
        enqueue(oats_queue, lcdoj);
        pthread_spin_unlock(&queue_mutex);
        lch = lch->next;
    }
}
#endif

void send_oats_message(dart_order_obj * doj)
{
    if (is_init) {
        if (doj->positions[ROM_TIME].iov_len <= 0) {
            int len = set_time_stamp(doj->current->data->wr_ptr);
            reset_rom_field(doj, ROM_TIME, doj->current->data->wr_ptr,
                            len);
            doj->current->data->wr_ptr += len;
        } else {
            set_time_stamp((char *) doj->positions[ROM_TIME].iov_base);
        }
#ifdef _SPREAD_N_READY_
        pthread_spin_lock(&factory_mutex);
        dart_order_obj *ldoj = get_doj(trans_fact);
        rom_style_reset(ldoj);
        copy_dart_order_obj(doj, ldoj);
        pthread_spin_unlock(&factory_mutex);
        pthread_spin_lock(&queue_mutex);
        enqueue(oats_queue, ldoj);
        pthread_spin_unlock(&queue_mutex);
        if(doj->childs != 0) {
            insert_children(doj);
        }
#else
        pthread_spin_lock(&factory_mutex);
        dart_order_obj *ldoj = get_doj(trans_fact);
        rom_style_reset(ldoj);
        copy_dart_order_obj(doj, ldoj);
        pthread_spin_unlock(&factory_mutex);
        pthread_spin_lock(&queue_mutex);
        enqueue(oats_queue, ldoj);
        pthread_spin_unlock(&queue_mutex);
#endif
    }
}

void add_mapping(const char *dest_id, int dest_id_len, const char *name,
                 int name_len)
{
    insert_map(dest_id_map, dest_id, dest_id_len, name, name_len);
}
