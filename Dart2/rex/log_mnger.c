/********************************************************************************
 *
 * Copyright (c) 2015 DART Executions LLC
 *
 * ****************************************************************************/
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <sys/fcntl.h>
#include <dirent.h>


#include "log_mnger.h"
#include "hashmap.h"
#include "databuf.h"
#include "message_queue.h"
#include "rex_seq_request.h"
#include "rex_resend_request.h"
#include "rex_allocator.h"
#include "rex_obj_factory.h"
#include "tm_to_str.h"
#include "dart_order_fact.h"
#include "dart_handler.h"
#include "rex_constants.h"

#define DEBUG 1
#define OUT 2
#define RAW 3
#define SEQ 4
#define RESEND 5
#define IN 6
#define ROLL 7
#define CLEAN 8
#define DART 9
#define DMA 10
#define AUDIT 11
#define OAT 0

#define IN_SUFF "_incoming.log"
#define OUT_SUFF "_outgoing.log"
#define SUFF_LEN 13


typedef struct file_struct {
    char name[64];
    size_t nlen;
    int in_fd;
    int out_fd;
    int roll_fd;
    int in_seq;
    int out_seq;
    int cur_day;
} fst;


typedef struct otl_wrapper {
    char name[64];
    void* payload;
    size_t nlen;
    short nmessages;
    short type;
    char inc_out_seq;
}otl_wrapper;

typedef struct dma_wrapper {
    void (*parse_dma)(void* p, char* data, int dlen, dart_order_obj* db);
    void* p;
    char* dstore;
    int dlen;
    int max;
    time_t time_sec;
    int time_micro;
}dma_wrapper;

typedef struct audit_wrapper {
    void (*parse_dma)(void* p, char* raw, int rlen,
        dart_order_obj* db, time_t sec, int micro);
    void* p;
    databuf_t* orig;
    time_t sec;
    int micro;
} audit_wrapper;

struct off_thread_logger {
    char* directory;
    queue_t in_q;
    hashmap_t file_map;
    dallocator* otl_wrap_fact;
    dallocator* dma_wrap_fact;
    dallocator* audit_wrap_fact;
    dartfact* df;
    dof_t* dbuff_fact;
    pthread_spinlock_t queue_mut;
    int thr;
    short dir_len;
    short should_run;
};

static int set_time_stamp(char* offset)
{
    int ret = 0;
    struct timeb rawtime;
    ftime(&rawtime);
    struct tm lm;
    gmtime_r(&rawtime.time, &lm);
    tm_to_str_millis(&lm, rawtime.millitm, offset,
            TIMESTAMP_LEN_WITH_MILLIS);
    ret = TIMESTAMP_LEN_WITH_MILLIS;
    return ret;
}

static void otl_file_writer(char* bytes, int nbytes,
        int fd)
{
    char* off = bytes;
    int b_w = 0;
    do {
        b_w = write(fd, off, nbytes);
        if(b_w > 0) {
            nbytes -= b_w;
            off += b_w;
        }
    } while(nbytes > 0 && b_w > 0);
}

static int day_check(fst* o)
{
    time_t rawtime;
    struct tm lm;
    time(&rawtime);
    gmtime_r(&rawtime, &lm);
    if(lm.tm_mday != o->cur_day) {
        o->cur_day = lm.tm_mday;
        return 1;
    }
    return 0;
}

static int prepend_time(char* wr_ptr)
{
    int slen = set_time_stamp(wr_ptr);
    wr_ptr += slen;
    memcpy(wr_ptr, "  ", 2);
    wr_ptr += 2;
    return slen + 2;
}

static void open_rolling_log(otl* o, fst* f, int too_big)
{
    time_t rawtime;
    struct tm *ti;
    int month;
    int year;
    struct tm lm;
    time(&rawtime);
    ti = gmtime_r(&rawtime, &lm);
    ti = &lm;
    year = 1900 + ti->tm_year;
    month = 1 + ti->tm_mon;
    char filename[1024];
    if(too_big) {
        sprintf(filename, "%s%sLOG%04d%02d%02d.%02d.%02d.%02d.LOG",o->directory, f->name,
                year, month, ti->tm_mday, ti->tm_hour, ti->tm_min, ti->tm_sec);
    } else {
        sprintf(filename, "%s%sLOG%04d%02d%02d.LOG",o->directory, f->name,
                year, month, ti->tm_mday);
    }
    f->roll_fd = open(filename,
                        O_CREAT | O_APPEND | O_RDWR ,  S_IRUSR | S_IWUSR);
    f->cur_day = ti->tm_mday;
}


static void init_otl_wrap(void* mem)
{
    memset(mem, '\0', sizeof(struct otl_wrapper));
}

static void init_dma_wrap(void* mem)
{
    memset(mem, '\0', REX_PG_SIZE);
    dma_wrapper* dma = mem;
    dma->dstore = mem + sizeof(dma_wrapper);
    dma->max = REX_PG_SIZE - sizeof(dma_wrapper);
}
static void init_audit_wrap(void* mem)
{
    memset(mem, '\0', sizeof(struct audit_wrapper));
}

static void create_logging_db(queue_t q, int num)
{
    do {
        databuf_t *buff = databuf_alloc(1024);
        enqueue(q, buff);
        --num;
    } while(num > 0);
}

static void clear_logging_db(void* db)
{
    databuf_reset((databuf_t*)db);
}

static void destroy_logging_db(void* d)
{
    databuf_t * buff = (databuf_t*)d;
    free(buff->buffer);
    free(buff);
}
void stop_logging(otl* o)
{
    o->should_run = 0;
}

static void get_in_seq(sequence_req* seq, fst* f)
{
    if(lseek(f->in_fd, 0, SEEK_SET) == 0) {
        char lseq_reader[128];
        memset(lseq_reader, '\0', 128);
        int b = read(f->in_fd, lseq_reader, 128);
        if(b <= 0) {
            printf("Error getting seq for: %s, error: %d\n", f->name, errno);
            f->in_seq = 0;
        } else {
            f->in_seq = atol(lseq_reader);
        }
    }
}

static void get_out_seq(sequence_req* seq, fst* f)
{
    lseek(f->out_fd, 0, SEEK_SET);
    char* file_reader = calloc(1,16384);
    char* rd_ptr = file_reader;
    char* wr_ptr = file_reader;
    long read_chunk = 16384;
    long bytes_read = read(f->out_fd, wr_ptr, read_chunk);
    long total_bytes_read = 0;
    long cum_used = 0;
    unsigned long sequence = 0;
    if(bytes_read <= 0) {
        f->out_seq = 0;
        goto CLEAN_SEQ;
    }
    do {
        total_bytes_read = bytes_read;
        total_bytes_read += wr_ptr - rd_ptr;
        long bytes_used = dsr_parse_data(seq, rd_ptr, total_bytes_read, 
                cum_used, &sequence);
        cum_used += bytes_used;
        f->out_seq = sequence;
        if(bytes_used >= total_bytes_read) {
            wr_ptr = file_reader;
            rd_ptr = file_reader;
            read_chunk = 16384;
        } else {
            wr_ptr += bytes_read;
            rd_ptr += bytes_used;
            long wr_off = wr_ptr - rd_ptr;
            memcpy(file_reader, rd_ptr, wr_off);
            read_chunk = (16384 - wr_off);
            rd_ptr = file_reader;
            wr_ptr = file_reader;
            wr_ptr += wr_off;
        }
        
    } while((bytes_read = read(f->out_fd, wr_ptr, read_chunk)) > 0) ;
CLEAN_SEQ:
    lseek(f->out_fd, 0, SEEK_END);
    free(file_reader);
}

static void reset_in_seq(fst* f, char* directory)
{
    char filename[512];
    int flen = sprintf(filename, "%s%s%s", directory, f->name,IN_SUFF);
    if(flen > 0) {
        f->in_fd = open(filename,
                        O_CREAT | O_RDWR | O_TRUNC , S_IRUSR | S_IWUSR);
        char sseq[128];
        memset(sseq, '\0', 128);
        int sslen = sprintf(sseq, "%d",f->in_seq);
        lseek(f->in_fd, 0, SEEK_SET);
        otl_file_writer(sseq, sslen, f->in_fd);
    }
}

static void reset_out_seq(fst* f, char* directory, sequence_req* seq)
{
    char filename[512];
    sprintf(filename, "%s%s%s", directory, f->name,OUT_SUFF);
    f->out_fd = open(filename,
            O_CREAT | O_RDWR | O_TRUNC | O_APPEND , S_IRUSR | S_IWUSR);
    size_t vlen = 0;
    char* values = dsr_create_pad(seq, f->out_seq, &vlen);
    if(vlen > 0) {
        otl_file_writer(values, vlen, f->out_fd);
    }
}

static void just_open(fst* f, char* directory)
{
    char filename[512];
    sprintf(filename, "%s%s%s",directory, f->name,IN_SUFF);
    f->in_fd = open(filename,
                O_CREAT | O_RDWR ,  S_IRUSR | S_IWUSR);
    sprintf(filename, "%s%s%s", directory,f->name,OUT_SUFF);
    f->out_fd = open(filename,
                O_APPEND | O_CREAT | O_RDWR , S_IRUSR | S_IWUSR);
}

static fst* open_outgoing(otl* o, otl_wrapper* wrapper)
{
    int ret = 0x0;
    fst* f = 0x0;
    REX_ALLOC(f, 8,sizeof(struct file_struct), ret);
    memcpy(f->name, wrapper->name, wrapper->nlen);
    f->nlen = wrapper->nlen;
    ret = no_copy_insert(o->file_map, f->name, f->nlen,
            f);
    char filename[512];
    sprintf(filename, "%s%s%s", o->directory,f->name,OUT_SUFF);
    f->out_fd = open(filename,
                O_APPEND | O_CREAT | O_RDWR , S_IRUSR | S_IWUSR);
    return f;
}

static void soft_seq_open(fst* f, char* directory, sequence_req* seq)
{
    unsigned long newin = dsr_get_new_incoming(seq);
    unsigned long newout = 0;
    newout = dsr_get_new_outgoing(seq);
    if(newin > 0) {
        f->in_seq = newin;
        reset_in_seq(f, directory);
    } else {
        char filename[512];
        /*First set the in coming sequence file*/
        sprintf(filename, "%s%s%s",directory, f->name,IN_SUFF);
        f->in_fd = open(filename,
                O_CREAT | O_RDWR ,  S_IRUSR | S_IWUSR);
    }
    if(newout > 0) {
        f->out_seq = newout;
        reset_out_seq(f, directory, seq);
    } else {
        /*Now set the out file*/
        char filename[512];
        sprintf(filename, "%s%s%s", directory,f->name,OUT_SUFF);
        f->out_fd = open(filename,
                O_APPEND | O_CREAT | O_RDWR , S_IRUSR | S_IWUSR);
    }
}

static void trunc_seq_open(fst* f, char* directory, sequence_req* seq)
{
    unsigned long newin = dsr_get_new_incoming(seq);
    unsigned long newout = dsr_get_new_outgoing(seq);
    if(newin > 0) {
        f->in_seq = newin;
        reset_in_seq(f, directory);
    } else {
        char filename[512];
        /*First set the in coming sequence file*/
        sprintf(filename, "%s%s%s", directory, f->name,IN_SUFF);
        f->in_fd = open(filename,
                O_CREAT | O_RDWR | O_TRUNC , S_IRUSR | S_IWUSR);
        f->in_seq = 0; 
    }
    if(newout > 0) {
        f->out_seq = newout;
        reset_out_seq(f, directory, seq);
    } else {
        char filename[512];
        /*Now set the out file*/
        sprintf(filename, "%s%s%s", directory, f->name,OUT_SUFF);
        f->out_fd = open(filename,
                O_CREAT | O_RDWR | O_TRUNC | O_APPEND , S_IRUSR | S_IWUSR);
        f->out_seq = 0;
    }
}

static void clean_log(otl* o, otl_wrapper* otl)
{
    fst* f = 0;
    int ret = get_map_entry(o->file_map, otl->name,
                otl->nlen, (void*)&f);
    if(ret <= 0) {
        REX_ALLOC(f, 8,sizeof(struct file_struct), ret);
        memcpy(f->name, otl->name, otl->nlen);
        f->nlen = otl->nlen;
        ret = no_copy_insert(o->file_map, f->name, f->nlen,
                f);
    } 
    char filename[512];
    int flen = sprintf(filename, "%s%s%s", o->directory,
                  otl->name,OUT_SUFF);
    if(flen > 0) {
        remove(filename);
    }
    flen = sprintf(filename, "%s%s%s", o->directory,
                  otl->name,IN_SUFF);
    if(flen > 0) {
        remove(filename);
    }
}

static void roll_dis_baby(otl* o, otl_wrapper* otl)
{
    fst* f = 0;
    int ret = get_map_entry(o->file_map, otl->name,
                 otl->nlen, (void*)&f);
    if(ret > 0) {
        if(f->roll_fd == 0) {
            open_rolling_log(o, f,0);
        } else if(day_check(f)) {
            close(f->roll_fd);
            open_rolling_log(o, f, 1);
        }
    } else {
        REX_ALLOC(f, 8,sizeof(struct file_struct), ret);
        memcpy(f->name, otl->name, otl->nlen);
        f->nlen = otl->nlen;
        ret = no_copy_insert(o->file_map, f->name, f->nlen,
                f);
        open_rolling_log(o, f, 0);
    }
    databuf_t* d = (databuf_t*)otl->payload;
    otl_file_writer(d->buffer, databuf_unread(d),
                    f->roll_fd);
    pthread_spin_lock(&o->queue_mut);
    destroy_elem(o->dbuff_fact, d);
    pthread_spin_unlock(&o->queue_mut);
}

static void log_outbound_message(otl* o, otl_wrapper* otl)
{
    fst* f = 0;
    databuf_t* d = (databuf_t*)otl->payload;
    int ret = get_map_entry(o->file_map, otl->name,
                 otl->nlen, (void*)&f);
    if(ret > 0) {
        f->out_seq++;
        if(f->out_fd == 0) {
            just_open(f, o->directory);
        }
        otl_file_writer(d->buffer, databuf_unread(d),
                        f->out_fd);
    }
    pthread_spin_lock(&o->queue_mut);
    destroy_elem(o->dbuff_fact, d);
    pthread_spin_unlock(&o->queue_mut);
}

static void rewind_n_log_out(otl* o, otl_wrapper* otl)
{
    fst* f = 0;
    databuf_t* d = (databuf_t*)otl->payload;
    int ret = get_map_entry(o->file_map, otl->name,
                 otl->nlen, (void*)&f);
    if(ret > 0) {
        f->out_seq++;
        if(f->out_fd == 0) {
            char filename[512];
            sprintf(filename, "%s%s%s",o->directory, f->name,IN_SUFF);
            f->in_fd = open(filename,
                        O_CREAT | O_RDWR ,  S_IRUSR | S_IWUSR);
            sprintf(filename, "%s%s%s", o->directory,f->name,OUT_SUFF);
            f->out_fd = open(filename,
                        O_CREAT | O_RDWR , S_IRUSR | S_IWUSR);
        }
    } else {
        int ret = 0x0;
        f = 0x0;
        REX_ALLOC(f, 8,sizeof(struct file_struct), ret);
        memcpy(f->name, otl->name, otl->nlen);
        f->nlen = otl->nlen;
        ret = no_copy_insert(o->file_map, f->name, f->nlen,
                f);
        char filename[512];
        sprintf(filename, "%s%s%s", o->directory,f->name,OUT_SUFF);
        f->out_fd = open(filename,
                    O_CREAT | O_RDWR , S_IRUSR | S_IWUSR);
    }
    lseek(f->out_fd, 0, SEEK_SET);
    otl_file_writer(d->buffer, databuf_unread(d),
                        f->out_fd);
    lseek(f->out_fd, 0, SEEK_SET);
    pthread_spin_lock(&o->queue_mut);
    destroy_elem(o->dbuff_fact, d);
    pthread_spin_unlock(&o->queue_mut);
}

static void record_raw(otl* o, otl_wrapper* otl) 
{
    fst* f = 0;
    databuf_t* d = (databuf_t*)otl->payload;
    int ret = get_map_entry(o->file_map, otl->name,
                 otl->nlen, (void*)&f);
    if(ret > 0) {
        //do nothing we found it
    } else {
        int ret = 0x0;
        f = 0x0;
        REX_ALLOC(f, 8,sizeof(struct file_struct), ret);
        memcpy(f->name, otl->name, otl->nlen);
        f->nlen = otl->nlen;
        ret = no_copy_insert(o->file_map, f->name, f->nlen,
                f);
        char filename[512];
        sprintf(filename, "%s%s%s", o->directory,f->name,OUT_SUFF);
        f->out_fd = open(filename,
                    O_CREAT | O_RDWR , S_IRUSR | S_IWUSR);
    }
    pthread_spin_lock(&o->queue_mut);
    databuf_t* db = get_obj(o->dbuff_fact);
    pthread_spin_unlock(&o->queue_mut);
    int tlen = prepend_time(db->wr_ptr);
    if((db->wr_ptr - db->buffer) < tlen) {
        db->wr_ptr = db->buffer + tlen;
    }
    if(f->roll_fd == 0) {
        open_rolling_log(o, f, 0);
    } else if(day_check(f)) {
        close(f->roll_fd);
        open_rolling_log(o, f, 1);
    }
    databuf_memcpy(db, d->rd_ptr, databuf_unread(d));
    databuf_memcpy(db, "\n", 1);
    otl_file_writer(db->buffer, databuf_unread(db),
                    f->roll_fd);
    pthread_spin_lock(&o->queue_mut);
    destroy_elem(o->dbuff_fact, db);
    pthread_spin_unlock(&o->queue_mut);
    if(otl->inc_out_seq) {
        log_outbound_message(o, otl);
    } else {
        pthread_spin_lock(&o->queue_mut);
        destroy_elem(o->dbuff_fact, d);
        pthread_spin_unlock(&o->queue_mut);
    }
}

static void dma_roller_helper(fst* f, otl* o, char* data, int dlen)
{
    if(f->roll_fd == 0) {
        open_rolling_log(o, f,0);
    } else if(day_check(f)) {
        close(f->roll_fd);
        open_rolling_log(o, f, 1);
    }
    pthread_spin_lock(&o->queue_mut);
    databuf_t* db = get_obj(o->dbuff_fact);
    pthread_spin_unlock(&o->queue_mut);
    int tlen = prepend_time(db->wr_ptr);
    if((db->wr_ptr - db->buffer) < tlen) {
        db->wr_ptr = db->buffer + tlen;
    }
    databuf_memcpy(db, data, dlen);
    databuf_memcpy(db, "\n", 1);
    otl_file_writer(db->buffer, databuf_unread(db),
                    f->roll_fd);
    pthread_spin_lock(&o->queue_mut);
    destroy_elem(o->dbuff_fact, db);
    pthread_spin_unlock(&o->queue_mut);
}

static void log_dma(otl* o, otl_wrapper* otl)
{
    fst* f = 0;
    int ret = get_map_entry(o->file_map, otl->name,
                 otl->nlen, (void*)&f);
    dma_wrapper* doj = (dma_wrapper*)otl->payload;
    pthread_spin_lock(&o->queue_mut);
    dart_order_obj* copy = get_doj(o->df);
    dart_style_reset(copy);
    databuf_t* db = get_obj(o->dbuff_fact);
    pthread_spin_unlock(&o->queue_mut);
    char t_str[32];
    memset(t_str, '\0', 32);
    struct tm lm;
    gmtime_r(&doj->time_sec, &lm);
    if(tm_to_str_millis(&lm, (doj->time_micro/1000),
                     t_str, 32) == 0) {
        set_rom_field(copy, ROM_TIME, t_str, 21);
    }
    doj->parse_dma(doj->p, doj->dstore, doj->dlen, copy);
    set_copied_doo_in_buf(copy, db);
    dartp* dp = (dartp*)copy->dest;          
    if(get_logged_on(dp)) {
        send_raw_csv(dp, db->rd_ptr ,databuf_unread(db));
    }
    if(ret > 0) {
        otl_file_writer(db->buffer, databuf_unread(db),
                        f->out_fd);
    }
    dma_roller_helper(f,o,doj->dstore, doj->dlen);
    pthread_spin_lock(&o->queue_mut);
    init_dma_wrap(doj);
    return_stuff(o->dma_wrap_fact, doj);
    reset_doj(o->df, copy);
    destroy_elem(o->dbuff_fact, db);
    pthread_spin_unlock(&o->queue_mut);
}

static void log_audit_trail(otl* o, otl_wrapper* otl)
{
    audit_wrapper* doj = (audit_wrapper*)otl->payload;
    pthread_spin_lock(&o->queue_mut);
    dart_order_obj* copy = get_doj(o->df);
    dart_style_reset(copy);
    databuf_t* db = get_obj(o->dbuff_fact);
    pthread_spin_unlock(&o->queue_mut);
    fst* f = 0;
    int ret = get_map_entry(o->file_map, otl->name,
                 otl->nlen, (void*)&f);
    if(ret > 0) {
        /*Do nothing*/
    } else {
        REX_ALLOC(f, 8,sizeof(struct file_struct), ret);
        memcpy(f->name, otl->name, otl->nlen);
        f->nlen = otl->nlen;
        open_rolling_log(o, f, 0);
        ret = no_copy_insert(o->file_map, f->name, f->nlen,
                f);
        doj->parse_dma(doj->p, 0, 0, copy,
            doj->sec, doj->micro);
        set_copied_doo_in_buf(copy, db);
        otl_file_writer(db->buffer, databuf_unread(db),
                    f->roll_fd);
        dart_style_reset(copy);
    }
    //do stuff.
    doj->parse_dma(doj->p, doj->orig->rd_ptr, 
        databuf_unread(doj->orig), copy,
        doj->sec, doj->micro);
    set_copied_doo_in_buf(copy, db);

    otl_file_writer(db->buffer, databuf_unread(db),
                    f->roll_fd);
    pthread_spin_lock(&o->queue_mut);
    destroy_elem(o->dbuff_fact, doj->orig);
    init_audit_wrap(doj);
    return_stuff(o->audit_wrap_fact, doj);
    reset_doj(o->df, copy);
    destroy_elem(o->dbuff_fact, db);
    pthread_spin_unlock(&o->queue_mut);
}

static void log_dart(otl* o, otl_wrapper* otl)
{
    fst* f = 0;
    int ret = get_map_entry(o->file_map, otl->name,
                 otl->nlen, (void*)&f);
    dart_order_obj* doj = (dart_order_obj*)otl->payload;
    if(ret > 0) {
        //do nothing
    } else {
        f = open_outgoing(o, otl);
    }
    pthread_spin_lock(&o->queue_mut);
    databuf_t* db = get_obj(o->dbuff_fact);
    pthread_spin_unlock(&o->queue_mut);
    set_copied_doo_in_buf(doj, db);
    otl_file_writer(db->buffer, databuf_unread(db),
                    f->out_fd);
    if(otl->type == OAT) {
        dartp* dp = (dartp*)doj->dest;          
        send_raw_csv(dp, db->rd_ptr, databuf_unread(db));
    }
    dma_roller_helper(f,o,db->buffer, databuf_unread(db));
    pthread_spin_lock(&o->queue_mut);
    destroy_elem(o->dbuff_fact, db);
    pthread_spin_unlock(&o->queue_mut);
        
    pthread_spin_lock(&o->queue_mut);
    reset_doj(o->df, doj);
    pthread_spin_unlock(&o->queue_mut);
}

static void on_thread_fill_sequence(otl* o, otl_wrapper* otl)
{
    sequence_req* seq = (sequence_req*)otl->payload;
    fst* ptr = 0;
    int ret =  get_map_entry(o->file_map, otl->name, otl->nlen,
                             (void*)&ptr);
    if(ret <= 0) {
        REX_ALLOC(ptr, 8,sizeof(struct file_struct), ret);
        memcpy(ptr->name, otl->name, otl->nlen);
        ptr->nlen = otl->nlen;
        ret = no_copy_insert(o->file_map, ptr->name, ptr->nlen,
                ptr);
    } else {
        close(ptr->in_fd);
        close(ptr->out_fd);
    }
    int create_new = dsr_get_create_new(seq);
    if(create_new) {
        trunc_seq_open(ptr, o->directory, seq);
    } else {
        soft_seq_open(ptr, o->directory, seq);
        get_in_seq(seq, ptr);
        get_out_seq(seq, ptr);
    }
    dsr_set_outgoing(seq, ptr->out_seq);
    dsr_set_incoming(seq, ptr->in_seq);
}

static void fill_sequence(otl* o, otl_wrapper* otl)
{
    on_thread_fill_sequence(o, otl);
    sequence_req* seq = (sequence_req*)otl->payload;
    dsr_signal_cond(seq);
}

static void on_thr_gen_resend(otl* o, otl_wrapper* otl)
{
    resend_req* rr = (resend_req*)otl->payload;
    fst* f = 0;
    int ret = get_map_entry(o->file_map, otl->name, otl->nlen,
                            (void*)&f);
    if(ret > 0) {
        //do nothing
    } else {
        REX_ALLOC(f, 8,sizeof(struct file_struct), ret);
        memcpy(f->name, otl->name, otl->nlen);
        f->nlen = otl->nlen;
        char filename[512];
        sprintf(filename, "%s%s%s",o->directory, f->name,IN_SUFF);
        f->in_fd = open(filename,
                    O_CREAT | O_RDWR ,  S_IRUSR | S_IWUSR);
        sprintf(filename, "%s%s%s", o->directory,f->name,OUT_SUFF);
        f->out_fd = open(filename,
                    O_CREAT | O_RDWR , S_IRUSR | S_IWUSR);
        no_copy_insert(o->file_map, f->name, f->nlen,f);
    }
    drr_create_resend_out(rr, f->out_fd);
}

static void gen_resend(otl* o, otl_wrapper* otl)
{
    resend_req* rr = (resend_req*)otl->payload;
    on_thr_gen_resend(o,otl);
    drr_signal_sleeper(rr);
}
static void update_incoming(otl* o, otl_wrapper* otl)
{
    fst* f = 0;
    int ret = get_map_entry(o->file_map, otl->name, otl->nlen,
                            (void*)&f);
    if(ret > 0) {
        f->in_seq += otl->nmessages;
        char sseq[128];
        memset(sseq, '\0', 128);
        int sslen = sprintf(sseq, "%d",f->in_seq);
        if(f->in_fd == 0) {
            just_open(f, o->directory);
        }
        lseek(f->in_fd, 0, SEEK_SET);
        otl_file_writer(sseq, sslen, f->in_fd);
    }
}

void set_thr_id(otl* o, int id)
{
    o->thr = id;
}
otl* create_otl(char* dir, int dir_len)
{
    otl* o = 0;
    if(opendir(dir)) {
        int ret = 0;
        REX_ALLOC(o, 8,sizeof(struct off_thread_logger), ret);
        o->in_q = create_sized_queue(128);
        o->thr = 0;
        o->file_map = create_map(128);
        o->otl_wrap_fact = create_mem_glob(sizeof(otl_wrapper), init_otl_wrap);
        o->audit_wrap_fact = create_mem_glob(sizeof(audit_wrapper), init_audit_wrap);
        o->dma_wrap_fact = create_mem_glob(32 * REX_PG_SIZE, init_dma_wrap);
        o->dbuff_fact = create_rex_obj_fact(128, destroy_logging_db,
                                            create_logging_db,
                                            clear_logging_db);
        o->df = create_dart_order_factory(128);
        pthread_spin_init(&o->queue_mut, PTHREAD_PROCESS_PRIVATE);
        o->directory = calloc(dir_len + 1, 1);
        memcpy(o->directory, dir, dir_len);
        o->dir_len = dir_len;
        o->should_run = 1;
    }
    return o;
}
static void clean_file_map(hashmap_t map)
{
    fst* f = 0;
    char *key = 0;
    int key_len = 0;
    hashmap_iterator_t hi = hashmap_iterator_begin(map);
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                                     (void *) &f);
    while (valid) {
        free(f);
        f = 0;
        valid = hashmap_iterator_next(map, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &f);
    }
    hashmap_iterator_destroy(hi);
    delete_map(map);
}


void log_check(otl* o)
{
    element_t e, temp;
    pthread_spin_lock(&o->queue_mut);
    int isempty = is_empty(o->in_q);
    if(!isempty) {
        e = dequeue(o->in_q);
        pthread_spin_unlock(&o->queue_mut);
        while(e != NULL) {
            otl_wrapper* otl = (otl_wrapper*)e->d;
            switch(otl->type) {
                case IN:
                    update_incoming(o, otl);
                    break;
                case OUT:
                    rewind_n_log_out(o, otl);
                    break;
                case ROLL:
                    roll_dis_baby(o, otl);
                    break;
                case SEQ:
                    fill_sequence(o, otl);
                    break;
                case RESEND:
                    gen_resend(o, otl);
                    break;
                case RAW:
                    record_raw(o, otl);
                    break;
                case DEBUG:
                    roll_dis_baby(o, otl);
                    break;
                case CLEAN:
                    clean_log(o, otl);
                    break;
                case OAT:
                case DART:
                    log_dart(o, otl);
                    break;
                case DMA:
                    log_dma(o, otl);
                    break;
                case AUDIT:
                    log_audit_trail(o, otl);
                    break;
            }
            pthread_spin_lock(&o->queue_mut);
            return_stuff(o->otl_wrap_fact, otl);    
            temp = e;
            e = temp->next;
            free_element(o->in_q, temp);
            pthread_spin_unlock(&o->queue_mut);
        }
    } else {
        pthread_spin_unlock(&o->queue_mut);
    }
}
void start_logging(void* v)
{
    otl* o = (otl*)v;
    struct timespec sleep_val = {0};
    sleep_val.tv_sec = 0;
    sleep_val.tv_nsec = 100000;
    do {
        element_t e, temp;
        pthread_spin_lock(&o->queue_mut);
        int isempty = is_empty(o->in_q);
        if(!isempty) {
            e = dequeue(o->in_q);
            pthread_spin_unlock(&o->queue_mut);
            while(e != NULL) {
                otl_wrapper* otl = (otl_wrapper*)e->d;
                switch(otl->type) {
                    case IN:
                        update_incoming(o, otl);
                        break;
                    case OUT:
                        rewind_n_log_out(o, otl);
                        break;
                    case ROLL:
                        roll_dis_baby(o, otl);
                        break;
                    case SEQ:
                        fill_sequence(o, otl);
                        break;
                    case RESEND:
                        gen_resend(o, otl);
                        break;
                    case RAW:
                        record_raw(o, otl);
                        break;
                    case DEBUG:
                        roll_dis_baby(o, otl);
                        break;
                    case CLEAN:
                        clean_log(o, otl);
                        break;
                    case OAT:
                    case DART:
                        log_dart(o, otl);
                        break;
                    case DMA:
                        log_dma(o, otl);
                        break;
                    case AUDIT:
                        log_audit_trail(o, otl);
                        break;
                }
                pthread_spin_lock(&o->queue_mut);
                return_stuff(o->otl_wrap_fact, otl);    
                temp = e;
                e = temp->next;
                free_element(o->in_q, temp);
                pthread_spin_unlock(&o->queue_mut);
            }
        } else {
            pthread_spin_unlock(&o->queue_mut);
        }
        nanosleep(&sleep_val, NULL);
    } while(o->should_run);
    /*
    pthread_spinlock_t queue_mut;
    */
    destroy_mem_glob(o->otl_wrap_fact);
    destroy_mem_glob(o->dma_wrap_fact);
    destroy_mem_glob(o->audit_wrap_fact);
    clean_fact(o->df);
    destroy_rex_obj_fact(o->dbuff_fact);
    clean_file_map(o->file_map);
    destroy_queue(o->in_q); 
}

static void reset_otl(otl_wrapper* otl)
{
    if(otl) {
        memset(otl->name, '\0', 64);
        otl->payload = 0;
        otl->nlen = 0;
        otl->nmessages = 0;
        otl->type = 0x0;
        otl->inc_out_seq = 0;
    }
}

void record_inbound_message(otl* o, int n_messages, char* name, int nlen)
{
    if(o->should_run) {
        pthread_spin_lock(&o->queue_mut);
        otl_wrapper* otl = get_stuff(o->otl_wrap_fact);
        reset_otl(otl);
        pthread_spin_unlock(&o->queue_mut);
        memcpy(otl->name, name, nlen);
        otl->nlen = nlen;
        otl->type = IN;
        otl->inc_out_seq = 0;
        otl->nmessages = n_messages;
        pthread_spin_lock(&o->queue_mut);
        enqueue(o->in_q, otl);
        pthread_spin_unlock(&o->queue_mut);
    }
}

void record_raw_message(otl* o, char* name, int nlen, char* raw, int rlen, char inc_out_seq)
{
    if(o->should_run) {
        pthread_spin_lock(&o->queue_mut);
        otl_wrapper* otl = get_stuff(o->otl_wrap_fact);
        reset_otl(otl);
        databuf_t* db = get_obj(o->dbuff_fact);
        pthread_spin_unlock(&o->queue_mut);
        memcpy(otl->name, name, nlen);
        otl->nlen = nlen;
        otl->type = RAW;
        otl->inc_out_seq = inc_out_seq;
        otl->nmessages = 0;
        otl->payload = db;
        databuf_memcpy(db, raw, rlen);
        pthread_spin_lock(&o->queue_mut);
        enqueue(o->in_q, otl);
        pthread_spin_unlock(&o->queue_mut);
    }
}

void rewind_raw_outbound(otl* o, char* name, int nlen, char* raw, int rlen)
{
    if(o->should_run) {
        pthread_spin_lock(&o->queue_mut);
        otl_wrapper* otl = get_stuff(o->otl_wrap_fact);
        reset_otl(otl);
        databuf_t* db = get_obj(o->dbuff_fact);
        databuf_reset(db);
        pthread_spin_unlock(&o->queue_mut);
        memcpy(otl->name, name, nlen);
        otl->nlen = nlen;
        otl->type = OUT;
        otl->inc_out_seq = 1;
        otl->nmessages = 0;
        otl->payload = db;
        databuf_memcpy(db, raw, rlen);
        pthread_spin_lock(&o->queue_mut);
        enqueue(o->in_q, otl);
        pthread_spin_unlock(&o->queue_mut);
    }
}

void record_dma_message(otl* o, char* name, int nlen, char* raw, int rlen,
                        void (*pdma)(void* p, char* d, int dlen, dart_order_obj* doj),
                        void* p)
{
    if(o->should_run) {
        pthread_spin_lock(&o->queue_mut);
        otl_wrapper* otl = get_stuff(o->otl_wrap_fact);
        reset_otl(otl);
        dma_wrapper* db = get_stuff(o->dma_wrap_fact);
        pthread_spin_unlock(&o->queue_mut);
        memcpy(otl->name, name, nlen);
        otl->nlen = nlen;
        otl->type = DMA;
        otl->inc_out_seq = 0;
        otl->nmessages = 0;
        otl->payload = db;
        db->parse_dma = pdma;
        struct timeval tv;
        gettimeofday(&tv, NULL);
        db->time_sec  = tv.tv_sec;
        db->time_micro = tv.tv_usec;
        memcpy(db->dstore, raw, rlen);
        db->dlen = rlen;
        db->p = p;
        pthread_spin_lock(&o->queue_mut);
        enqueue(o->in_q, otl);
        pthread_spin_unlock(&o->queue_mut);
    }
}

void record_audit_trail(otl* o, char* raw, int rlen,
                        void (*pdma)(void* p, char* orig, int olen, dart_order_obj* doj,
                          time_t sec, int micro),
                        void* p)
{
  if(o->should_run) {
        pthread_spin_lock(&o->queue_mut);
        otl_wrapper* otl = get_stuff(o->otl_wrap_fact);
        reset_otl(otl);
        audit_wrapper* db = get_stuff(o->audit_wrap_fact);
        memcpy(otl->name, "AUDIT_TRAIL", 11);
        otl->nlen = 11;
        otl->type = AUDIT;
        otl->inc_out_seq = 0;
        otl->nmessages = 0;
        otl->payload = db;
        db->parse_dma = pdma;
        struct timeval tv;
        gettimeofday(&tv, NULL);
        db->sec  = tv.tv_sec;
        db->micro = tv.tv_usec;
        db->orig = get_obj(o->dbuff_fact);
        databuf_memcpy(db->orig, raw, rlen);
        db->p = p;
        enqueue(o->in_q, otl);
        pthread_spin_unlock(&o->queue_mut);
  }
}

void record_rolling_log(otl* o, char* name, int nlen, char* raw, int rlen, char inc_out_seq)
{
    if(o->should_run) {
        pthread_spin_lock(&o->queue_mut);
        otl_wrapper* otl = get_stuff(o->otl_wrap_fact);
        reset_otl(otl);
        databuf_t* db = get_obj(o->dbuff_fact);
        pthread_spin_unlock(&o->queue_mut);
        memcpy(otl->name, name, nlen);
        otl->nlen = nlen;
        otl->type = ROLL;
        otl->inc_out_seq = inc_out_seq;
        otl->nmessages = 0;
        otl->payload = db;
        int tlen = prepend_time(db->wr_ptr);
        if((db->wr_ptr - db->buffer) < tlen) {
            db->wr_ptr = db->buffer + tlen;
        }
        databuf_memcpy(db, raw, rlen);
        pthread_spin_lock(&o->queue_mut);
        enqueue(o->in_q, otl);
        pthread_spin_unlock(&o->queue_mut);
    }
}

void reset_seq(otl* o, char* name, int nlen)
{
    if(o->should_run) {
        pthread_spin_lock(&o->queue_mut);
        otl_wrapper* otl = get_stuff(o->otl_wrap_fact);
        reset_otl(otl);
        pthread_spin_unlock(&o->queue_mut);
        otl->payload = 0;
        memcpy(otl->name, name, nlen);
        otl->nlen = nlen;
        otl->type = CLEAN;
        otl->inc_out_seq = 0; 
        pthread_spin_lock(&o->queue_mut);
        enqueue(o->in_q, otl);
        pthread_spin_unlock(&o->queue_mut);
    }
}

void get_sequence_numbers(void* v, otl* o)
{
    if(o->should_run) {
        sequence_req* sr = (sequence_req*)v;
        int sf = pthread_self();
        if(sf == o->thr) {
            otl_wrapper* otl = get_stuff(o->otl_wrap_fact);
            reset_otl(otl);
            const char* lname = dsr_get_name(sr, &otl->nlen);
            memcpy(otl->name, lname, otl->nlen) ;
            otl->type = SEQ;
            otl->inc_out_seq = 0;
            otl->nmessages = 0;
            otl->payload = sr;
            on_thread_fill_sequence(o, otl);
        } else {
            dsr_lock_con_mutex(sr);
            pthread_spin_lock(&o->queue_mut);
            otl_wrapper* otl = get_stuff(o->otl_wrap_fact);
            reset_otl(otl);
            const char* lname = dsr_get_name(sr, &otl->nlen);
            memcpy(otl->name, lname, otl->nlen) ;
            otl->type = SEQ;
            otl->inc_out_seq = 0;
            otl->nmessages = 0;
            otl->payload = sr;
            enqueue(o->in_q, otl);
            pthread_spin_unlock(&o->queue_mut);
            dsr_wait_on_cond(sr);
        }
    }
}

void get_resend_request(void* v, otl* o)
{
    if(o->should_run) {
        resend_req* rr = (resend_req*)v;
        int sf = pthread_self();
        if(sf == o->thr) {
                otl_wrapper* otl = get_stuff(o->otl_wrap_fact);
                reset_otl(otl);
                const char* lname = drr_get_name(rr, &otl->nlen);
                memcpy(otl->name, lname, otl->nlen) ;
                otl->type = RESEND;
                otl->inc_out_seq = 0;
                otl->nmessages = 0;
                otl->payload = rr;
                on_thr_gen_resend(o, otl);
        } else {
            if(drr_lock_update_mutex(rr) == 0) {
                if(pthread_spin_lock(&o->queue_mut) == 0) {
                    otl_wrapper* otl = get_stuff(o->otl_wrap_fact);
                    reset_otl(otl);
                    const char* lname = drr_get_name(rr, &otl->nlen);
                    memcpy(otl->name, lname, otl->nlen) ;
                    otl->type = RESEND;
                    otl->inc_out_seq = 0;
                    otl->nmessages = 0;
                    otl->payload = rr;
                    enqueue(o->in_q, otl);
                    pthread_spin_unlock(&o->queue_mut);
                    drr_wait_on_cond(rr);
                } else {
                    drr_unlock_update_mutex(rr);
                }
            }
        }
    }
}
void record_dart_message(otl* o, dart_order_obj* doj)
{
    if(o->should_run) {
        pthread_spin_lock(&o->queue_mut);
        otl_wrapper* otl = get_stuff(o->otl_wrap_fact);
        reset_otl(otl);
        dart_order_obj* copy = get_doj(o->df);
        copy_dart_order_obj(doj, copy);
        memcpy(otl->name,getpval(doj,ROM_SENDER),getplen(doj,ROM_SENDER));
        otl->nlen = getplen(doj,ROM_SENDER);
        otl->type = DART;
        otl->inc_out_seq = 0;
        otl->nmessages = 0;
        otl->payload = copy;
        enqueue(o->in_q, otl);
        pthread_spin_unlock(&o->queue_mut);
    }
}

void record_dart_message_for_sender(otl* o, dart_order_obj* doj,
                             char* sender, int slen, void* dp)
{
    if(o->should_run) {
        pthread_spin_lock(&o->queue_mut);
        otl_wrapper* otl = get_stuff(o->otl_wrap_fact);
        reset_otl(otl);
        dart_order_obj* copy = get_doj(o->df);
        copy_dart_order_obj(doj, copy);
        copy->dest = dp;
        memcpy(otl->name,sender,slen);
        otl->nlen = slen;
        otl->type = OAT;
        otl->inc_out_seq = 0;
        otl->nmessages = 0;
        otl->payload = copy;
        enqueue(o->in_q, otl);
        pthread_spin_unlock(&o->queue_mut);
    }
}

void send_debug_message(otl* o, const char* formatstr, ...)
{
    if(o->should_run) {
        pthread_spin_lock(&o->queue_mut);
        otl_wrapper* otl = get_stuff(o->otl_wrap_fact);
        reset_otl(otl);
        databuf_t* db = get_obj(o->dbuff_fact);
        pthread_spin_unlock(&o->queue_mut);
        memcpy(otl->name, "DEBUG", 5);
        otl->nlen = 5;
        otl->type = DEBUG;
        otl->inc_out_seq = 0;
        otl->nmessages = 0;
        otl->payload = db;
        int tlen = prepend_time(db->wr_ptr);
        if((db->wr_ptr - db->buffer) < tlen) {
            db->wr_ptr = db->buffer + tlen;
        }
        va_list argp;
        va_start(argp, formatstr);
        int result = vsnprintf(db->wr_ptr, databuf_space_left(db), 
                               formatstr, argp);
        va_end(argp);
        if(result > 0 && result < databuf_space_left(db)) { 
            db->wr_ptr = db->wr_ptr + result;
            pthread_spin_lock(&o->queue_mut);
            enqueue(o->in_q, otl);
            pthread_spin_unlock(&o->queue_mut);
        } else {
            pthread_spin_lock(&o->queue_mut);
            return_stuff(o->otl_wrap_fact, otl);
            destroy_elem(o->dbuff_fact, db);
            pthread_spin_unlock(&o->queue_mut);
        }
    }
}

