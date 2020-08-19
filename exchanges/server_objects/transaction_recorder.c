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
#include <sys/timeb.h>

#include "transaction_recorder.h"
#include "hashmap.h"
#include "client_manager.h"
#include "databuf.h"
#include "tm_to_str.h"
#include "dbg_helper.h"

#define INCOMING_SUFFIX "_incoming.log"
#define OUTGOING_SUFFIX "_outgoing.log"
#define SUFFIX_LEN 13
#define IN_SEQ_SIZE 50
#define ROLL_FILE_SIZE 50000000

static pthread_mutex_t init_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t cond_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t seq_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t logging_thread;
static queue_t incoming_queue;
static queue_t outgoing_queue;
static queue_t clean_queue;
static queue_t seq_request;
static queue_t resend_requests;
static queue_t rolling_queue;
static queue_t init_queue;
static int is_init = 0;
static int should_shutdown = 0;
static databuf_t *in_seq_writer = 0;

ssize_t retrieve_inbound_seqno_from_file(const char *name,
                                         size_t namelength)
{
    ssize_t seqno = -1;
    struct databuf *buf = databuf_alloc(4096);
    FILE *f = 0;
    if (databuf_write
        (buf, "%.*s%s", (int) namelength, name, "_incoming.log") == -1)
        goto done;
    if ((f = fopen(buf->rd_ptr, "a+")) == 0) {
        perror("opening inbound sequence file");
        goto done;
    }
    databuf_reset(buf);
    /* Read in the file contents */
    size_t nread;
    while ((nread = fread(buf->wr_ptr, 1, databuf_space_left(buf), f)) > 0) {
        buf->wr_ptr += nread;
        if (feof(f) != 0)
            break;
        if (databuf_reserve(buf, 512) == -1)
            goto done;
    }
    if (ferror(f) != 0)
        goto done;
    errno = 0;
    long int i = strtol(buf->rd_ptr, 0, 10);
    if (errno != 0)
        goto done;
    seqno = i;
  done:
    if (f != 0)
        fclose(f);
    databuf_destroy(buf);
    return seqno;
}

static int flush_inbound_seqno_to_file(const char *name, size_t namelength,
                                       size_t seqno, hashmap_t filemap)
{
    int retval = -1;
    FILE *f = 0;
    if (get_map_entry(filemap, name, namelength, (void *) &f) == 0) {
        /* No file pointer, must open the file and insert into the
         * map. */
        struct databuf *filename = databuf_alloc(512);
        databuf_write(filename, "%.*s%s", (int) namelength, name,
                      "_incoming.log");
        f = fopen(filename->rd_ptr, "w+");
        if (f == 0) {
            perror("creating inbound sequence file");
            return -1;
        }
        databuf_destroy(filename);
        if (no_copy_insert(filemap, name, namelength, f) == 0) {
            fprintf(stderr, "%s:%d error inserting FILE* into file map\n",
                    __FILE__, __LINE__);
            return -1;
        }
        setbuf(f, 0);
    }
    rewind(f);
    char tmpbuf[64];
    struct databuf seqnum;
    databuf_attach(&seqnum, tmpbuf, sizeof tmpbuf);
    databuf_write(&seqnum, "%lu", seqno);
    size_t nwritten;
    while ((nwritten =
            fwrite(seqnum.rd_ptr, sizeof seqnum.rd_ptr[0],
                   databuf_unread(&seqnum), f)) < databuf_unread(&seqnum)
           && ferror(f) == 0)
        seqnum.rd_ptr += nwritten;
    if (ferror(f) == 0)
        retval = 0;
    rewind(f);
    return retval;
}

struct rec_args {
    int create_new;
    int timeout_value;
    queue_t in_mess;
    queue_t out_mess;
    queue_t roll_mess;
    app_end_time *t;
    hashmap_t out_file_map;
    hashmap_t in_file_map;
    hashmap_t out_seq_map;
    hashmap_t in_seq_map;
    hashmap_t rolling_map;
    char *name;
    int name_len;
};

struct rolling_log_file {
    int day;
    FILE *file_ptr;
    char *file_name;
};

struct incoming_message *create_incoming_message(const char *name,
                                                 size_t name_len,
                                                 size_t num)
{
    struct incoming_message *im = malloc(sizeof(struct incoming_message));
    im->name = calloc(name_len + 1, 1);
    memcpy(im->name, name, name_len);
    im->name_len = name_len;
    im->num = num;
    return im;
}

/* The high bit on the size_t is set to indicate that this value is
 * simply the new inbound sequence number.  With the bit cleared the
 * number is used as an increment.  Either way the range of possible
 * values is equivalent to the max size of a signed long int instead
 * of an unsigned long int. */
#define INCOMING_MESSAGE_REPLACE_VALUE_FLAG 0x8000000000000000

struct incoming_message *create_incoming_message_replace_value(const char
                                                               *name,
                                                               size_t
                                                               namelength,
                                                               size_t
                                                               value)
{
    struct incoming_message *msg =
        create_incoming_message(name, namelength,
                                value);
    msg->num |= INCOMING_MESSAGE_REPLACE_VALUE_FLAG;
    return msg;
}

void set_incoming_message_name(struct incoming_message *im,
                               const char *name, size_t name_len)
{
    if (im) {
        if (im->name) {
            free(im->name);
        }
        im->name = calloc(name_len + 1, 1);
        memcpy(im->name, name, name_len);
        im->name_len = name_len;
    }
}

void destroy_incoming_message(struct incoming_message *im)
{
    if (im) {
        if (im->name) {
            free(im->name);
        }
        free(im);
    }
}

static size_t trans_writer(void *ptr, size_t size, size_t nmemb,
                           FILE * stream)
{
    char *w_off = (char *) ptr;
    size_t len_left = nmemb;
    size_t written = 0;
    size_t b_w = 0;
    int ret_val = 0;

    if (ferror(stream)) {
        printf("Error on stream before write: %d. %lu \n", errno,
               (unsigned long) stream);
        clearerr(stream);
    }

    while (written < nmemb && !ferror(stream)) {
        b_w = fwrite(w_off, size, len_left, stream);
        written += b_w;
        if (written < nmemb) {
            w_off += b_w;
            len_left -= b_w;
        }
    }
    ret_val = written;
    if (ferror(stream)) {
        printf("Error on stream after write: %lu %lu \n", written, nmemb);
        clearerr(stream);
    }
    return ret_val;
}

static int set_sequence_queue(size_t * seq_num, FILE * file,
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
    while ((feof(file)) == 0) {
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
        bytes_read = fread(wr_ptr, 1, FILE_READ_CHUNK, file);
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
    }
    fseek(file, 0, SEEK_CUR);
    clearerr(file);
    free(file_reader);
    return 0;
}

static int initialize_incoming_log_file(const char *name, size_t name_len,
                                        sequence_req * sr,
                                        struct rec_args *ra)
{
    char *in_suffix;
    FILE *in_file = 0, *temp_in_file;
    size_t map_ret = 0;
    int create_new = dsr_get_create_new(sr);
    struct databuf *filename = databuf_alloc(64);
    databuf_write(filename, "%.*s%s", (int) name_len, name,
                  INCOMING_SUFFIX);
    databuf_reset(in_seq_writer);
    memset(in_seq_writer->buffer, '\0', IN_SEQ_SIZE);
    in_suffix = INCOMING_SUFFIX;
    temp_in_file = fopen(filename->rd_ptr, "ab+");
    if (temp_in_file != NULL && (create_new == 0)) {
        long size;
        size_t seq;
        size_t b_used;
        fseek(temp_in_file, 0, SEEK_SET);
        fseek(temp_in_file, 0, SEEK_END);
        size = ftell(temp_in_file);
        seq = 0;
        b_used = 0;
        if (size > 0) {
            fseek(temp_in_file, 0, SEEK_SET);
            b_used =
                fread(in_seq_writer->buffer, 1, IN_SEQ_SIZE - 1,
                      temp_in_file);
            if (b_used < (IN_SEQ_SIZE - 1) && (ferror(temp_in_file))) {
                printf("Error Initializing %s, setting sequence to 0\n",
                       filename->rd_ptr);
            } else {
                seq = atol(in_seq_writer->buffer);
            }
            b_used = 0;
            fseek(temp_in_file, 0, SEEK_CUR);
            fclose(temp_in_file);
            in_file = fopen(filename->rd_ptr, "wb+");
            if (in_file != NULL) {
                size_t sprint_len;
                int ret_val = 0;
                setbuf(in_file, NULL);
                fseek(in_file, 0, SEEK_SET);
                ret_val = databuf_write(in_seq_writer, "%d", seq);
                sprint_len = ret_val >= 0 ? (size_t) ret_val : 0;
                trans_writer(in_seq_writer->buffer, sprint_len, 1,
                             in_file);
                fseek(in_file, 0, SEEK_SET);
                map_ret =
                    no_copy_insert(ra->in_file_map, name, name_len,
                                   in_file);
            } else {
                printf("Second attempt to open file Write+ failed: %s\n",
                       filename->rd_ptr);
            }
        } else {
            size_t sprint_len;
            int ret_val = 0;
            printf
                ("0 Size file for: %s, Creating new file with sequence 0 \n",
                 filename->rd_ptr);
            fclose(temp_in_file);
            in_file = fopen(filename->rd_ptr, "wb+");
            if (in_file != NULL) {
                setbuf(in_file, NULL);
                map_ret =
                    no_copy_insert(ra->in_file_map, name, name_len,
                                   in_file);
            } else {
                printf("Second attempt to open file Write+ failed: %s\n",
                       filename->rd_ptr);
            }
            b_used = 0;
            fseek(in_file, 0, SEEK_SET);
            databuf_reset(in_seq_writer);
            memset(in_seq_writer->buffer, '\0', IN_SEQ_SIZE);
            ret_val = databuf_write(in_seq_writer, "%d", 0);
            sprint_len = ret_val >= 0 ? (size_t) ret_val : 0;
            trans_writer(in_seq_writer->buffer, sprint_len, 1, in_file);
            fseek(in_file, 0, SEEK_SET);
        }
        map_ret =
            insert_map(ra->in_seq_map, name, name_len, &seq, sizeof(seq));
    } else if (create_new) {
        printf("Create new file: %s \n", name);
        size_t seq = 0;
        size_t sprint_len;
        int ret_val = 0;
        in_file = fopen(filename->rd_ptr, "wb+");
        if (in_file != NULL) {
            setbuf(in_file, NULL);
            map_ret =
                no_copy_insert(ra->in_file_map, name, name_len, in_file);
            fseek(in_file, 0, SEEK_SET);
            databuf_reset(in_seq_writer);
            memset(in_seq_writer->buffer, '\0', IN_SEQ_SIZE);
            ret_val = databuf_write(in_seq_writer, "%d", 1);    /* Why is this 1? */
            sprint_len = ret_val >= 0 ? (size_t) ret_val : 0;
            trans_writer(in_seq_writer->buffer, sprint_len, 1, in_file);
            fseek(in_file, 0, SEEK_SET);
            map_ret =
                insert_map(ra->in_seq_map, name, name_len, &seq,
                           sizeof(seq));
        } else {
            printf("fopen failed for this: %s \n", filename->rd_ptr);
        }
    } else {
        printf("Could not create file: %s\n", filename->rd_ptr);
    }
    databuf_destroy(filename);
    return 0;
}

static int initialize_outgoing_log_file(const char *name, size_t name_len,
                                        sequence_req * req,
                                        struct rec_args *ra)
{
    size_t namelen, out_len;
    size_t map_ret = 0;
    char *out_suffix;
    FILE *out_file;
    char *out_file_name;
    char *file_off;
    databuf_reset(in_seq_writer);
    memset(in_seq_writer->buffer, '\0', IN_SEQ_SIZE);
    out_file = NULL;
    out_len = 0;
    namelen = 0;
    namelen = name_len;
    out_suffix = OUTGOING_SUFFIX;
    out_len += SUFFIX_LEN;
    out_file_name = malloc(out_len + namelen + 1);
    memset(out_file_name, '\0', namelen + out_len + 1);
    memcpy(out_file_name, name, namelen);
    int create_new = dsr_get_create_new(req);
    file_off = out_file_name + namelen;
    memcpy(file_off, out_suffix, out_len);
    if (create_new) {
        out_file = fopen(out_file_name, "wb+");
    } else {
        out_file = fopen(out_file_name, "ab+");
    }
    if (out_file != NULL) {
        size_t seq = 0;
        setbuf(out_file, NULL);
        map_ret =
            no_copy_insert(ra->out_file_map, name, name_len, out_file);
        fseek(out_file, 0, SEEK_SET);
        set_sequence_queue(&seq, out_file, req);
        map_ret =
            insert_map(ra->out_seq_map, name, name_len, &seq,
                       sizeof(size_t));
        printf("Initialize outgoing created file: %s, %lu create new: %d \n", name, name_len, create_new);
    } else {
        printf("Could not create file: %s\n", out_file_name);
    }
    free(out_file_name);
    return 0;
}

static void reset_sequence_numbers(sequence_req * sr, struct rec_args *ra)
{
    size_t namelen, in_len, out_len;
    size_t map_ret = 0;
    char *in_suffix, *out_suffix;
    FILE *in_file, *out_file;
    char *in_file_name;
    char *out_file_name;
    char *file_off;
    size_t sr_name_len = 0;
    const char *sr_name = dsr_get_name(sr, &sr_name_len);
    unsigned long new_incoming = dsr_get_new_incoming(sr);
    unsigned long new_outgoing = dsr_get_new_outgoing(sr);
    databuf_reset(in_seq_writer);
    memset(in_seq_writer->buffer, '\0', IN_SEQ_SIZE);
    in_file = NULL;
    out_file = NULL;
    out_len = 0;
    in_len = 0;
    namelen = 0;
    namelen = sr_name_len;
    in_suffix = INCOMING_SUFFIX;
    in_len += SUFFIX_LEN;
    out_suffix = OUTGOING_SUFFIX;
    out_len += SUFFIX_LEN;
    in_file_name = malloc(in_len + namelen + 1);
    out_file_name = malloc(out_len + namelen + 1);
    memset(in_file_name, '\0', namelen + in_len + 1);
    memset(out_file_name, '\0', namelen + out_len + 1);
    memcpy(in_file_name, sr_name, namelen);
    memcpy(out_file_name, sr_name, namelen);
    file_off = in_file_name + namelen;
    memcpy(file_off, in_suffix, in_len);
    if (new_incoming && new_incoming < MAX_SEQ_RESET_NUM) {
        in_file = fopen(in_file_name, "wb+");
        if (in_file != NULL) {
            size_t sprint_len;
            int ret_val = 0;
            unsigned long seq;
            seq = new_incoming;
            setbuf(in_file, NULL);
            map_ret =
                no_copy_insert(ra->in_file_map, sr_name, sr_name_len,
                               in_file);
            fseek(in_file, 0, SEEK_SET);
            ret_val = databuf_write(in_seq_writer, "%lu", new_incoming);
            sprint_len = ret_val >= 0 ? (size_t) ret_val : 0;
            trans_writer(in_seq_writer->buffer, sprint_len, 1, in_file);
            fseek(in_file, 0, SEEK_SET);
            map_ret =
                insert_map(ra->in_seq_map, sr_name, sr_name_len, &seq,
                           sizeof(seq));
        } else {
            printf("Could not open in_file for seq reset.\n");
        }
    } else {
        initialize_incoming_log_file(sr_name, sr_name_len, sr, ra);
    }
    if (new_outgoing && new_outgoing < MAX_SEQ_RESET_NUM) {
        file_off = out_file_name + namelen;
        memcpy(file_off, out_suffix, out_len);
        out_file = fopen(out_file_name, "wb+");
        if (out_file != NULL) {
            size_t seq_num = 0;
            size_t v_len = 0;
            size_t len_left;
            char *w_off;
            char *values = dsr_create_pad(sr, new_outgoing, &v_len);
            setbuf(out_file, NULL);
            w_off = values;
            len_left = v_len;

            trans_writer(w_off, 1, len_left, out_file);
            fclose(out_file);
            out_file = NULL;
            out_file = fopen(out_file_name, "ab+");
            if (out_file != NULL) {
                setbuf(out_file, NULL);
                map_ret =
                    no_copy_insert(ra->out_file_map, sr_name, sr_name_len,
                                   out_file);
                fseek(out_file, 0, SEEK_SET);
                set_sequence_queue(&seq_num, out_file, sr);
                map_ret = insert_map(ra->out_seq_map, sr_name, sr_name_len,
                                     &seq_num, sizeof(size_t));
                printf("Inserted into out seq map in reset_seq: %s, %lu \n", sr_name, sr_name_len);
            } else {
                printf("Second open of out file failed: %s!!!!\n",
                       out_file_name);
            }
            free(values);
        } else {
            printf("Could not open out_file for seq reset.\n");
        }
    } else {
        initialize_outgoing_log_file(sr_name, sr_name_len, sr, ra);
    }
    free(out_file_name);
    free(in_file_name);
}

static int grab_resend_messages(struct rec_args *ra)
{
    element_t e, temp;
    pthread_mutex_lock(&seq_queue_mutex);
    e = dequeue(resend_requests);
    pthread_mutex_unlock(&seq_queue_mutex);
    while (e != NULL) {
        resend_req *rr = (resend_req *) e->d;
        if (rr) {
            long ret = 0;
            FILE *file_ptr = NULL;
            size_t name_len = 0;
            const char *name = drr_get_name(rr, &name_len);

            ret = (long) get_map_entry(ra->out_file_map, name, name_len,
                                       (void *) &file_ptr);
            if (ret == 0) {
                drr_signal_sleeper(rr);
            } else {
                drr_create_resend_out(rr, file_ptr);
                drr_signal_sleeper(rr);
            }
        }
        temp = e;
        e = temp->next;
        pthread_mutex_lock(&seq_queue_mutex);
        free_element(resend_requests, temp);
        pthread_mutex_unlock(&seq_queue_mutex);
    }
    return 0;
}

static int clean_logs(struct rec_args *ra)
{
    element_t e, temp;
    pthread_mutex_lock(&seq_queue_mutex);
    e = dequeue(clean_queue);
    pthread_mutex_unlock(&seq_queue_mutex);
    while (e != NULL) {
        sequence_req *m;
        m = (sequence_req *) e->d;
        size_t seq_num = 0;
        size_t local_name_len = 0;
        char *local_name = (char *) dsr_get_name(m, &local_name_len);
        printf("Pulled this off the queue: %s, name len? %lu \n",
                local_name, local_name_len);
        int ret =
            get_map_entry(ra->out_seq_map, local_name, local_name_len,
                          (void *) &seq_num);
        if (ret <= 0) {
            //do nothing
        } else {
            printf("Found this file: %s, Len: %d \n", local_name, ret);
            remove_from_map(ra->in_seq_map, local_name, local_name_len);
            remove_from_map(ra->in_file_map, local_name, local_name_len);
            remove_from_map(ra->out_seq_map, local_name, local_name_len);
            remove_from_map(ra->out_file_map, local_name, local_name_len);
        }
        char *full_name = calloc(4096, 1);
        sprintf(full_name, "%s_incoming.log", local_name);
        int r = remove(full_name);
        if (r != 0) {
            //int er = errno;
            //printf("Error removing this file: %d, %s \n", er, full_name);
        }
        memset(full_name, '\0', 1000);
        sprintf(full_name, "%s_outgoing.log", local_name);
        r = remove(full_name);
        if (r != 0) {
            int er = errno;
            printf("Error removing this file: %d, %s \n", er, full_name);
        }
        insert_map(ra->out_seq_map, local_name, local_name_len,
                                 &seq_num, sizeof(size_t));
        insert_map(ra->in_seq_map, local_name, local_name_len,
                                 &seq_num, sizeof(size_t));
        free(full_name);
        destroy_sequence_req(m);
        temp = e;
        e = temp->next;
        pthread_mutex_lock(&seq_queue_mutex);
        free_element(clean_queue, temp);
        pthread_mutex_unlock(&seq_queue_mutex);
    }
    return 1;
}

static size_t figure_new_incoming_sequence_number(size_t existing,
                                                  size_t update_value)
{
    if (update_value & INCOMING_MESSAGE_REPLACE_VALUE_FLAG)
        existing = update_value;
    else
        existing += update_value;
    return existing;
}

static int grab_incoming_messages(queue_t q, hashmap_t map,
                                  hashmap_t seq_map, struct rec_args *ra)
{
    element_t e, temp;
    int remove_ret = 0;
    pthread_mutex_lock(&queue_mutex);
    e = dequeue(q);
    pthread_mutex_unlock(&queue_mutex);
    clean_logs(ra);
    while (e != NULL) {
        struct incoming_message *m;
        FILE *file_ptr;
        long init_in = 0;
        long *l_in = &init_in;
        size_t ret;
        size_t seq;
        seq = 0;
        file_ptr = NULL;
        databuf_reset(in_seq_writer);
        memset(in_seq_writer->buffer, '\0', IN_SEQ_SIZE);
        m = (struct incoming_message *) e->d;
        ret = get_map_entry(map, m->name, m->name_len, (void *) &file_ptr);
        if (ret > 0) {
            /* Initial incoming sequence file load */
            size_t w_len;
            int w_sp_len = 0;
            ret =
                get_map_entry(ra->in_seq_map, m->name, m->name_len,
                              (void *) &l_in);
            if (ret > 0) {
                seq += *l_in;
            } else {
                size_t b_used = 0;
                fseek(file_ptr, 0, SEEK_SET);
                b_used =
                    fread(in_seq_writer->buffer, (size_t) 1,
                          (size_t) (IN_SEQ_SIZE - 1), file_ptr);
                if (b_used < (size_t) (IN_SEQ_SIZE - 1)
                    && (ferror(file_ptr))) {
                    printf
                        ("INSERTING MESSAGE read error read this many bytes: "
                         "%lu, and got this result: %lu for connection: "
                         "%s seq: %lu \n", b_used, seq, m->name, m->num);
                } else {
                    printf
                        ("Inserting this seq: %s, for this connection: "
                         "%s, this many new seq: %lu\n",
                         in_seq_writer->buffer, m->name, m->num);
                    seq += atol(in_seq_writer->buffer);
                }
            }

            /* Modify the sequence number */
            seq = figure_new_incoming_sequence_number(seq, m->num);

            /* Flush to disk */
            fseek(file_ptr, 0, SEEK_SET);
            databuf_reset(in_seq_writer);
            w_sp_len = databuf_write(in_seq_writer, "%lu", seq);
            w_len = w_sp_len >= 0 ? (size_t) w_sp_len : 0;
            trans_writer(in_seq_writer->buffer, w_len, 1, file_ptr);
            fseek(file_ptr, 0, SEEK_SET);

            /* Add new value to map */
            remove_ret =
                remove_from_map(ra->in_seq_map, m->name, m->name_len);
            ret =
                insert_map(ra->in_seq_map, m->name, m->name_len, &seq,
                           sizeof(seq));
        } else {
            /* No record of this incoming sequence file. Find the
             * existing sequnce number in the incoming sequence
             * file. */
            ssize_t seqno =
                retrieve_inbound_seqno_from_file(m->name, m->name_len);
            if (seqno > -1) {
                seqno = figure_new_incoming_sequence_number(seqno, m->num);
                remove_ret =
                    remove_from_map(ra->in_seq_map, m->name, m->name_len);
                insert_map (ra->in_seq_map, m->name, m->name_len, &seqno,
                            sizeof seqno);
                    /* Open the incoming sequnce number file for writing,
                     * store the FILE* and flush the file for the first
                     * time for good measure. */
                    if (flush_inbound_seqno_to_file
                        (m->name, m->name_len, seqno,
                         ra->in_file_map) == -1)
                        fprintf(stderr, "Failed to flush to map\n");
            }
        }
        temp = e;
        e = temp->next;
        destroy_incoming_message(m);
        temp->d = NULL;
        pthread_mutex_lock(&queue_mutex);
        free_element(q, temp);
        pthread_mutex_unlock(&queue_mutex);
    }
    return 0;
}

static int fill_sequences(struct rec_args *ra)
{
    element_t e, temp;
    pthread_mutex_lock(&seq_queue_mutex);
    e = dequeue(seq_request);
    pthread_mutex_unlock(&seq_queue_mutex);
    clean_logs(ra);
    grab_incoming_messages(incoming_queue, ra->in_file_map,
                                   ra->in_seq_map, ra);
    while (e != NULL) {
        sequence_req *m;
        size_t ret;
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
        printf("Create new in fill sequences: %d, new outgoing, %lu, new incoming %lu Name: %s \n",
                 create_new, new_outgoing, new_incoming, local_name);
        ra->create_new = dsr_get_create_new(m);
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
        if (ret == 0 || ra->create_new) {
            printf("About to initialize: %d, %s \n", ra->create_new, local_name);
            if(create_new) {
                remove_from_map(ra->out_seq_map, local_name,
                                local_name_len);
                remove_from_map(ra->in_seq_map, local_name,
                                local_name_len);
            }
            initialize_incoming_log_file(local_name, local_name_len, m,
                                         ra);
            initialize_outgoing_log_file(local_name, local_name_len, m,
                                         ra);
            ret =
                get_map_entry(ra->out_seq_map, local_name, local_name_len,
                              (void *) &seq_num);
        }
        printf("Fill sequences: %s, %lu \n", local_name,local_name_len);
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
        pthread_mutex_lock(&seq_queue_mutex);
        free_element(seq_request, temp);
        pthread_mutex_unlock(&seq_queue_mutex);
    }
    return 0;
}

static void clean_up(queue_t q)
{
    element_t e, temp;
    pthread_mutex_lock(&queue_mutex);
    e = dequeue(q);
    while (e != NULL) {
        temp = e;
        e = temp->next;
        free(temp->d);
        temp->d = NULL;
        free_element(q, temp);
    }
    pthread_mutex_unlock(&queue_mutex);
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
    ret = fclose(rlf->file_ptr);
    if (ret != 0) {
        printf("Error closing this file: %s \n", rlf->file_name);
    }
    rlf->file_ptr = fopen(rlf->file_name, "ab+");
    if (rlf->file_ptr) {
        //do nothing
    } else {
        printf("Unable to reopen: %s \n", rlf->file_name);
    }
}

static void insert_rolling_log(struct rec_args *ra)
{
    element_t e, temp;
    size_t write_ret = 0;
    pthread_mutex_lock(&queue_mutex);
    e = dequeue(rolling_queue);
    pthread_mutex_unlock(&queue_mutex);
    while (e != NULL) {
        struct rolling_log_message *m = NULL;
        size_t ret = 0;
        struct rolling_log_file *rlf;
        m = (struct rolling_log_message *) e->d;
        temp = NULL;
        if (m == NULL) {
            temp = e;
            e = temp->next;
            pthread_mutex_lock(&queue_mutex);
            free_element(rolling_queue, temp);
            pthread_mutex_unlock(&queue_mutex);
            printf("NULL structure in the queue!!\n");
            continue;
        }
        ret =
            get_map_entry(ra->rolling_map, m->file_name, m->file_name_len,
                          (void *) &rlf);
        if (ret > 0) {
            size_t written;
            size_t len_left;
            char *w_off;
            long eof;
            written = 0;
            len_left = m->len;
            fseek(rlf->file_ptr, 0, SEEK_END);
            eof = ftell(rlf->file_ptr);

            if (ferror(rlf->file_ptr)) {
                printf("Error on stream after ftell: %d. %lu \n", errno,
                       (unsigned long) rlf->file_ptr);
                clearerr(rlf->file_ptr);
            }
            if (eof < ROLL_FILE_SIZE && day_check(rlf->day)) {
                w_off = m->data;
                if (trans_writer(w_off, 1, len_left, rlf->file_ptr) <
                    len_left) {
                    reopen_log(rlf);
                    trans_writer(w_off, 1, len_left, rlf->file_ptr);
                }
                if (trans_writer(" --end \n", 1, 8, rlf->file_ptr) < 8) {
                    reopen_log(rlf);
                }
            } else {
                time_t rawtime;
                struct tm *ti;
                size_t year;
                int sprinter = 0;
                size_t month;
                databuf_t *mblk = databuf_alloc(m->file_name_len + 80);
                struct tm lm;
                fclose(rlf->file_ptr);
                printf("Closed file: %lu, name: %s\n ",
                       (unsigned long) rlf->file_ptr, m->file_name);
                (void) time(&rawtime);
                ti = gmtime_r(&rawtime, &lm);
                ti = &lm;
                year = (size_t) (1900 + ti->tm_year);
                month = (size_t) (1 + ti->tm_mon);
                sprinter =
                    databuf_write(mblk,
                                  "%sLog%04lu%02lu%02d.%02d.%02d.%02d.LOG",
                                  m->file_name, year, month, ti->tm_mday,
                                  ti->tm_hour, ti->tm_min, ti->tm_sec);
                free(rlf->file_name);
                rlf->file_name = calloc(sprinter + 1, 1);
                memcpy(rlf->file_name, mblk->buffer, sprinter);
                rlf->file_ptr = fopen(rlf->file_name, "ab+");
                if (rlf->file_ptr) {
                    rlf->day = ti->tm_mday;
                    setbuf(rlf->file_ptr, NULL);
                    sprinter =
                        remove_from_map(ra->rolling_map, m->file_name,
                                        m->file_name_len);
                    (void) no_copy_insert(ra->rolling_map, m->file_name,
                                          m->file_name_len, rlf);
                    printf("Added file: %lu, name: %s\n ",
                           (unsigned long) rlf->file_ptr, m->file_name);
                    w_off = m->data;
                    len_left = m->len;
                    trans_writer(w_off, 1, len_left, rlf->file_ptr);
                    trans_writer(" --end \n", 1, 8, rlf->file_ptr);
                    fflush(rlf->file_ptr);
                } else {
                    printf("Unable to create this file: {%s}\n",
                           m->file_name);
                }
                databuf_destroy(mblk);
            }
        } else {
            time_t rawtime;
            struct tm *ti;
            size_t year;
            size_t month;
            databuf_t *mblk = databuf_alloc(m->file_name_len + 80);
            size_t len_left;
            char *w_off = m->data;
            int sprinter = 0;
            struct tm lm;
            struct rolling_log_file *rlf =
                malloc(sizeof(struct rolling_log_file));
            (void) time(&rawtime);
            ti = gmtime_r(&rawtime, &lm);
            ti = &lm;
            printf("Opening this file: %s \n", m->file_name);
            year = (size_t) (1900 + ti->tm_year);
            month = (size_t) (1 + ti->tm_mon);
            sprinter =
                databuf_write(mblk, "%sLog%04lu%02lu%02d.LOG",
                              m->file_name, year, month, ti->tm_mday);
            rlf->file_name = calloc(sprinter + 1, 1);
            memcpy(rlf->file_name, mblk->buffer, sprinter);
            rlf->file_ptr = fopen(rlf->file_name, "ab+");
            if (rlf->file_ptr) {
                rlf->day = ti->tm_mday;
                setbuf(rlf->file_ptr, NULL);
                write_ret = no_copy_insert(ra->rolling_map, m->file_name,
                                           m->file_name_len, rlf);
                printf("Added file: %lu, name: %s\n ",
                       (unsigned long) rlf->file_ptr, m->file_name);
                w_off = m->data;
                len_left = m->len;
                trans_writer(w_off, 1, len_left, rlf->file_ptr);
                trans_writer(" --end \n", 1, 8, rlf->file_ptr);
                //write_ret = fwrite("\n", 1, 1, file_2);
                fflush(rlf->file_ptr);
            } else {
                printf("Unable to create this file: {%s}\n", m->file_name);
            }
            databuf_destroy(mblk);
        }
        temp = e;
        e = temp->next;
        if (m) {
            free(m->name);
            free(m->data);
            free(m->file_name);
            free(m);
            m = 0;
        }
        temp->d = NULL;
        pthread_mutex_lock(&queue_mutex);
        free_element(rolling_queue, temp);
        pthread_mutex_unlock(&queue_mutex);
    }
}

static void restart_file(struct rec_args *ra, size_t write_ret,
                         size_t len_left, FILE * file_ptr,
                         struct message *m, char *w_off)
{
    char *out_file_name = calloc(SUFFIX_LEN + m->name_len + 1, 1);
    char *cp_off = out_file_name;
    memcpy(out_file_name, m->name, m->name_len);
    cp_off += m->name_len;
    memcpy(cp_off, OUTGOING_SUFFIX, SUFFIX_LEN);
    remove_from_map(ra->out_file_map, m->name, m->name_len);
    fclose(file_ptr);
    file_ptr = fopen(out_file_name, "ab+");
    if (file_ptr) {
        setbuf(file_ptr, NULL);
        trans_writer(w_off + write_ret, 1, (len_left - write_ret),
                     file_ptr);
        no_copy_insert(ra->out_file_map, m->name, m->name_len, file_ptr);
    }

}

/**
 * If incoming is 1 then incoming otherwise outgoing.  we free the
 * memory allocated for the message.
 */
static int grab_messages(queue_t q, hashmap_t map, hashmap_t seq_map,
                         struct rec_args *ra)
{
    element_t e, temp;
    pthread_mutex_lock(&queue_mutex);
    e = dequeue(q);
    pthread_mutex_unlock(&queue_mutex);
    clean_logs(ra);
    while (e != NULL) {
        struct message *m;
        FILE *file_ptr;
        size_t ret;
        size_t l_seq = 0;
        size_t *old_seq = &l_seq;
        size_t write_ret = 0;
        if (e->d != NULL) {
            m = (struct message *) e->d;
            file_ptr = NULL;
            temp = NULL;
            ret =
                get_map_entry(map, m->name, m->name_len,
                              (void *) &file_ptr);
            if (ret > 0) {
                size_t written;
                size_t len_left;
                char *w_off;

                written = 0;
                w_off = m->data;
                len_left = m->len;
                write_ret = trans_writer(w_off, 1, len_left, file_ptr);
                if (write_ret < len_left) {
                    restart_file(ra, write_ret, len_left, file_ptr, m,
                                 w_off);
                }
            } else {
                size_t out_len;
                char *out_suffix;
                char *file_off;
                FILE *out_file;
                char *out_file_name;
                printf
                    ("Recording Outgoing message: "
                     "Error Could not find File for: %s \n", m->name);
                out_file = NULL;
                out_len = 0;
                out_suffix = OUTGOING_SUFFIX;
                out_len += SUFFIX_LEN;
                out_file_name = calloc(out_len + m->name_len + 1, 1);
                memcpy(out_file_name, m->name, (m->name_len));
                file_off = out_file_name;
                file_off += m->name_len;
                memcpy(file_off, out_suffix, out_len);
                out_file = fopen(out_file_name, "ab+");
                if (out_file) {
                    size_t written;
                    size_t len_left;
                    long tell_ret = 0;
                    char *w_off;
                    setbuf(out_file, NULL);
                    written = 0;
                    w_off = m->data;
                    len_left = m->len;
                    write_ret = trans_writer(w_off, 1, len_left, out_file);
                    if (write_ret < len_left) {
                        restart_file(ra, write_ret, len_left, out_file, m,
                                     w_off);
                    } else {
                        tell_ret = ftell(out_file);
                        len_left = tell_ret >= 0 ? (size_t) tell_ret : 0;
                        ret =
                            no_copy_insert(ra->out_file_map, m->name,
                                           m->name_len, out_file);
                    }
                } else {
                    int er = errno;
                    printf
                        ("Unable to open file for: "
                         "%s filename: %s, errno: %d, name len: %lu \n",
                         m->name, out_file_name, er, m->name_len);
                }
                free(out_file_name);
            }
            ret =
                get_map_entry(seq_map, m->name, m->name_len,
                              (void *) &old_seq);
            if (ret > 0) {
                size_t seq = *old_seq;
                ++seq;
                remove_from_map(seq_map, m->name, m->name_len);
                ret = insert_map(seq_map, m->name, m->name_len, &seq,
                                 sizeof(size_t));
            } else {
                printf("Could not find queue for %s in grab messages!\n",
                       m->name);
            }
            free(m->name);
            free(m->data);
            free(m);
        }
        temp = e;
        e = temp->next;
        m = NULL;
        temp->d = NULL;
        temp->next = 0;
        pthread_mutex_lock(&queue_mutex);
        free_element(outgoing_queue, temp);
        pthread_mutex_unlock(&queue_mutex);
    }
    return 0;
}

static void *record_messages(void *args)
{
    struct rec_args *ra = (struct rec_args *) args;
    time_t long_time;
    struct timespec timeout;
    struct tm *newtime;
    int status;
    int keep_running;
    int l_is_empty = 1;
    struct tm l_new;
    keep_running = 1;
    while (keep_running) {
        timeout.tv_sec = time(NULL) + 2;
        timeout.tv_nsec = 0;
        pthread_mutex_lock(&seq_queue_mutex);
        l_is_empty = is_empty(clean_queue);
        pthread_mutex_unlock(&seq_queue_mutex);
        if (!l_is_empty) {
            clean_logs(ra);
        }
        pthread_mutex_lock(&seq_queue_mutex);
        l_is_empty = is_empty(seq_request);
        pthread_mutex_unlock(&seq_queue_mutex);
        if (!l_is_empty) {
            fill_sequences(ra);
        }
        pthread_mutex_lock(&seq_queue_mutex);
        l_is_empty = is_empty(resend_requests);
        pthread_mutex_unlock(&seq_queue_mutex);
        if (!l_is_empty) {
            grab_resend_messages(ra);
        }
        /* Locking the condition so that we do not miss messages. */
        pthread_mutex_lock(&queue_mutex);
        l_is_empty = is_empty(outgoing_queue);
        pthread_mutex_unlock(&queue_mutex);
        if (!l_is_empty) {
            grab_messages(outgoing_queue, ra->out_file_map,
                          ra->out_seq_map, ra);
        }
        pthread_mutex_lock(&queue_mutex);
        l_is_empty = is_empty(incoming_queue);
        pthread_mutex_unlock(&queue_mutex);
        if (!l_is_empty) {
            grab_incoming_messages(incoming_queue, ra->in_file_map,
                                   ra->in_seq_map, ra);
        }
        status = pthread_mutex_lock(&cond_mutex);
        if (status != 0) {
            printf("Failed to get lock!\n");
        }
        status =
            pthread_cond_timedwait(&queue_cond, &cond_mutex, &timeout);
        pthread_mutex_unlock(&cond_mutex);
        if (status == ETIMEDOUT) {
            (void) time(&long_time);
            newtime = localtime_r(&long_time, &l_new);
            newtime = &l_new;
            if (newtime->tm_hour == ra->t->end_hour
                && newtime->tm_min >= ra->t->end_min
                && newtime->tm_min < (ra->t->end_min + 5)
                && ((newtime->tm_wday == ra->t->end_day)
                    || (ra->t->end_day == 7))) {
                printf("Time to end, transaction recorder: %d min %d \n",
                       ra->t->end_hour, ra->t->end_min);
                keep_running = 0;
                is_init = 0;
                should_shutdown = 1;
            }
        }
        pthread_mutex_lock(&queue_mutex);
        l_is_empty = is_empty(incoming_queue);
        pthread_mutex_unlock(&queue_mutex);
        if (!l_is_empty) {
            grab_incoming_messages(incoming_queue, ra->in_file_map,
                                   ra->in_seq_map, ra);
        }
        pthread_mutex_lock(&queue_mutex);
        l_is_empty = is_empty(outgoing_queue);
        pthread_mutex_unlock(&queue_mutex);
        if (!l_is_empty) {
            grab_messages(outgoing_queue, ra->out_file_map,
                          ra->out_seq_map, ra);
        }
        pthread_mutex_lock(&seq_queue_mutex);
        l_is_empty = is_empty(clean_queue);
        pthread_mutex_unlock(&seq_queue_mutex);
        if (!l_is_empty) {
            clean_logs(ra);
        }
        pthread_mutex_lock(&seq_queue_mutex);
        l_is_empty = is_empty(resend_requests);
        pthread_mutex_unlock(&seq_queue_mutex);
        if (!l_is_empty) {
            grab_resend_messages(ra);
        }
        pthread_mutex_lock(&seq_queue_mutex);
        l_is_empty = is_empty(seq_request);
        pthread_mutex_unlock(&seq_queue_mutex);
        if (!l_is_empty) {
            fill_sequences(ra);
        }
        pthread_mutex_lock(&queue_mutex);
        l_is_empty = is_empty(rolling_queue);
        pthread_mutex_unlock(&queue_mutex);
        if (!l_is_empty) {
            insert_rolling_log(ra);
        }
    }
    if (pthread_mutex_destroy(&init_mutex) != 0) {
        printf("Failed to destroy init mutex . \n");
    }
    if (pthread_mutex_destroy(&seq_queue_mutex) != 0) {
        printf("Failed to destroy seq queue mutex. \n");
    }
    if (pthread_cond_destroy(&queue_cond) != 0) {
        printf("Failed to destroy queueu cond. \n");
    }
    clean_up(incoming_queue);
    clean_up(outgoing_queue);
    clean_up(resend_requests);
    clean_up(seq_request);
    clean_up(init_queue);
    clean_up(ra->in_mess);
    clean_up(ra->out_mess);
    clean_up(rolling_queue);
    delete_map(ra->out_file_map);
    delete_map(ra->in_file_map);
    delete_map(ra->in_seq_map);
    delete_map(ra->out_seq_map);
    delete_map(ra->rolling_map);
    if (pthread_mutex_destroy(&queue_mutex) != 0) {
        printf("Failed to destroy queue mutex. \n");
    }
    if (pthread_mutex_destroy(&cond_mutex) != 0) {
        printf("Failed to destroy seq reset seq mutex. \n");
    }
    free(ra);
    return NULL;
}
int start_recorder(struct rec_init *init)
{
    int is_started = -1;
    pthread_mutex_lock(&init_mutex);
    if (is_init == 0 && should_shutdown == 0) {
        struct rec_args *ra = malloc(sizeof *ra);
        pthread_attr_t thread_attr;
        printf("****STARTING TRANSACTION RECORDER***\n");
        incoming_queue = create_queue();
        outgoing_queue = create_queue();
        resend_requests = create_queue();
        rolling_queue = create_queue();
        clean_queue = create_queue();
        seq_request = create_queue();
        init_queue = create_queue();
        in_seq_writer = databuf_alloc(IN_SEQ_SIZE);

        ra->create_new = init->create_new;
        ra->timeout_value = (int) init->timeout_value;
        ra->in_mess = incoming_queue;
        ra->out_mess = outgoing_queue;
        ra->roll_mess = rolling_queue;
        ra->t = init->t;
        ra->out_file_map = create_map(200);
        ra->in_file_map = create_map(200);
        ra->in_seq_map = create_map(200);
        ra->out_seq_map = create_map(200);
        ra->rolling_map = create_map(200);

        is_init = 1;
        if (pthread_attr_init(&thread_attr) != 0) {
            printf("Pthread attr init failed!\n");
            is_started = -1;
        }
        if (pthread_attr_setdetachstate
            (&thread_attr, PTHREAD_CREATE_DETACHED) != 0) {
            printf("Thread set detached failed!\n");
            is_started = -1;
        }

        pthread_create(&logging_thread, &thread_attr, record_messages, ra);
        is_started = 0;
    }
    pthread_mutex_unlock(&init_mutex);
    return is_started;
}

void record_inbound_message(struct incoming_message *m)
{
    if (is_init) {
        pthread_mutex_lock(&queue_mutex);
        enqueue(incoming_queue, m);
        pthread_mutex_unlock(&queue_mutex);
        pthread_mutex_lock(&cond_mutex);
        pthread_cond_signal(&queue_cond);
        pthread_mutex_unlock(&cond_mutex);
    } else {
        printf("Called before init: \n");
    }
}

void record_outbound_message(struct message *m)
{
    if (is_init) {
        pthread_mutex_lock(&queue_mutex);
        enqueue(outgoing_queue, m);
        pthread_mutex_unlock(&queue_mutex);
        pthread_mutex_lock(&cond_mutex);
        pthread_cond_signal(&queue_cond);
        pthread_mutex_unlock(&cond_mutex);
    } else {
        printf("Called before init: \n");
    }
}

int dart_rolling_log(struct rolling_log_message *m)
{
    int result = 0;
    if (is_init) {
        pthread_mutex_lock(&queue_mutex);
        enqueue(rolling_queue, m);
        pthread_mutex_unlock(&queue_mutex);
        pthread_mutex_lock(&cond_mutex);
        pthread_cond_signal(&queue_cond);
        pthread_mutex_unlock(&cond_mutex);
    } else {
        fprintf(stderr, "%s called before logging system initialized\n",
                __FUNCTION__);
        result = -1;
    }
    return result;
}

void clean_seq(sequence_req * sr)
{
    if (is_init) {
        pthread_mutex_lock(&seq_queue_mutex);
        enqueue(clean_queue, sr);
        pthread_mutex_unlock(&seq_queue_mutex);
        pthread_mutex_lock(&cond_mutex);
        pthread_cond_signal(&queue_cond);
        pthread_mutex_unlock(&cond_mutex);
    } else {
        printf("Clean seq Called before init: \n");
    }

}

void get_sequence_numbers(sequence_req * sr)
{
    if (is_init) {
        /* I need to grab the ready signal mutex before I signal to
         * ensure that the signal is not fired before. I am
         * waiting. */
        dsr_lock_con_mutex(sr);
        pthread_mutex_lock(&seq_queue_mutex);
        enqueue(seq_request, sr);
        pthread_mutex_unlock(&seq_queue_mutex);
        pthread_mutex_lock(&cond_mutex);
        /* Signal the other thread */
        pthread_cond_signal(&queue_cond);
        pthread_mutex_unlock(&cond_mutex);
        /* Wait on for return */
        dsr_wait_on_cond(sr);
    } else {
        printf("Called before init: \n");
    }
}

void get_resend_request(resend_req * rr)
{
    if (is_init) {
        /*I need to lock this mutex in order to
         * ensure that I do not miss the signal.*/
        if (drr_lock_update_mutex(rr) == 0) {
            if (pthread_mutex_lock(&seq_queue_mutex) == 0) {
                enqueue(resend_requests, rr);
                pthread_mutex_unlock(&seq_queue_mutex);
                if (pthread_mutex_lock(&cond_mutex) == 0) {
                    pthread_cond_signal(&queue_cond);
                    pthread_mutex_unlock(&cond_mutex);
                }
                /* Wait on for return */
                drr_wait_on_cond(rr);
            } else {
                drr_unlock_update_mutex(rr);
            }
        }
    } else {
        printf("Called before init: \n");
    }
}
