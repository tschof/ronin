/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

#include "rex_resend_request.h"

#define RESEND_REQ_FILE_BUFF_SIZE 32768
#define RESEND_REQ_FILE_READ_CHUNK 16384

struct rex_resend_req {
    char *name;
    size_t name_len;
    size_t beg;
    size_t end;
    char *output;
    size_t len;
    void* chunk_parser;
    pthread_mutex_t seq_mutex;
    pthread_cond_t seq_cond;
     size_t(*parse_resend_req) (char *data, size_t * seq_off,
                                size_t size, size_t bytes_used,
                                size_t beg_seq, size_t end_seq,
                                size_t * beg_off, size_t * end_off);
     int return_chunk;
     int chunk_request_size;
     int (*chunk_return)(char* output, int out_len, void* chunk_parser);

};

resend_req *create_chunky_resend(const char* name,
                                 size_t name_len, 
                                 size_t chunk_request_size,void* cp,
                                 int (*chunk_ret)(char* output, int out_len, void* cp))
{
    resend_req *rr = malloc(sizeof(struct rex_resend_req));
    if (rr != NULL) {
        rr->name = calloc(name_len + 1, 1);
        memcpy(rr->name, name, name_len);
        rr->name_len = name_len;
        rr->beg = 0;
        rr->end = 0;
        rr->output = calloc(1, chunk_request_size + 1);
        rr->len = chunk_request_size;
        rr->parse_resend_req = 0;
        if (pthread_mutex_init(&rr->seq_mutex, NULL) != 0) {
            printf("Failed to init seq_mutex resend.\n");
        }
        if (pthread_cond_init(&rr->seq_cond, NULL) != 0) {
            printf("Failed to init seq cond resend.\n");
        }
        rr->return_chunk = 1;
        rr->chunk_request_size = chunk_request_size;
        rr->chunk_return = chunk_ret;
        rr->chunk_parser = cp;
    }
    return rr;
}
resend_req *create_resend_request(const char *name,
                                  size_t name_len,
                                  size_t beg, size_t end,
                                  size_t(*parse_req) (char *data,
                                                      size_t * seq_off,
                                                      size_t size,
                                                      size_t bytes_used,
                                                      size_t beg_seq,
                                                      size_t end_seq,
                                                      size_t * beg_off,
                                                      size_t * end_off))
{
    resend_req *rr = malloc(sizeof(struct rex_resend_req));
    if (rr != NULL) {
        rr->name = calloc(name_len + 1, 1);
        memcpy(rr->name, name, name_len);
        rr->name_len = name_len;
        rr->beg = beg;
        rr->end = end;
        rr->output = NULL;
        rr->len = 0;
        rr->parse_resend_req = parse_req;
        if (pthread_mutex_init(&rr->seq_mutex, NULL) != 0) {
            printf("Failed to init seq_mutex resend.\n");
        }
        if (pthread_cond_init(&rr->seq_cond, NULL) != 0) {
            printf("Failed to init seq cond resend.\n");
        }
        rr->return_chunk = 0;
        rr->chunk_request_size = 0;
        rr->chunk_return = 0;
        rr->chunk_parser = 0;
    }
    return rr;
}

void destroy_resend_req(resend_req * rr)
{
    if (rr) {
        if (pthread_mutex_destroy(&rr->seq_mutex) != 0) {
            printf("Failed to destroy resend request mutex.\n");
        }
        if (pthread_cond_destroy(&rr->seq_cond) != 0) {
            printf("Failed to destroy resend condition \n");
        }
        if (rr->output) {
            free(rr->output);
            rr->output = NULL;
            rr->len = 0;
        }
        free(rr);
        rr = NULL;
    }
}

const char *drr_get_name(resend_req * rr, size_t * len)
{
    *len = rr->name_len;
    return rr->name;
}

void drr_signal_sleeper(resend_req * rr)
{
    if (pthread_mutex_lock(&rr->seq_mutex) == 0) {
        if (pthread_cond_signal(&rr->seq_cond) == 0) {
            pthread_mutex_unlock(&rr->seq_mutex);
        }
    }
}

const char *drr_get_resend_output(resend_req * rr, size_t * len)
{
    *len = rr->len;
    return rr->output;
}

int drr_lock_update_mutex(resend_req * rr)
{
    return pthread_mutex_lock(&rr->seq_mutex);
}

void drr_unlock_update_mutex(resend_req * rr)
{
    pthread_mutex_unlock(&rr->seq_mutex);
}

void drr_wait_on_cond(resend_req * rr)
{
    if (pthread_cond_wait(&rr->seq_cond, &rr->seq_mutex) == 0) {
        pthread_mutex_unlock(&rr->seq_mutex);
    }
}

void drr_create_resend_out(resend_req * rr, int fp)
{
    if(fp <= 0) {
        return;
    }
    size_t bytes_read = 0;
    size_t bytes_used = 0;
    size_t total_bytes_read = 0;
    size_t cum_bytes_used = 0;
    size_t total_size = 0;
    size_t prev_total_size = 0;
    char *file_reader = calloc(RESEND_REQ_FILE_BUFF_SIZE, 1);
    char *wr_ptr = file_reader;
    char *rd_ptr = file_reader;
    size_t read_offset = 0;
    size_t write_offset = 0;
    int should_parse = 1;
    size_t beg_off = 0;
    size_t end_off = 0;
    size_t cp_len = 0;
    size_t seq = 0;
    size_t read_bytes = 0;
    lseek(fp, 0, SEEK_SET);
    if(rr->return_chunk && rr->chunk_return) {
        do {
            bytes_read = read(fp, wr_ptr, rr->len);
            should_parse = rr->chunk_return(wr_ptr, bytes_read, rr->chunk_parser);
        } while(should_parse);
        goto done_with_resend;
    } 
    //long offset = lseek(fp, 0, SEEK_END);
    total_size = RESEND_REQ_FILE_BUFF_SIZE;
    while (should_parse) { // && !feof(fp)) {
        if ((total_size - write_offset) <= RESEND_REQ_FILE_READ_CHUNK) {
            char *temp;
            prev_total_size = (write_offset - read_offset);
            temp = calloc(prev_total_size, 1);
            memcpy(temp, rd_ptr, prev_total_size);
            free(file_reader);
            file_reader = NULL;
            total_size += RESEND_REQ_FILE_BUFF_SIZE;
            file_reader = calloc(total_size, 1);
            memcpy(file_reader, temp, prev_total_size);
            free(temp);
            temp = NULL;
            rd_ptr = file_reader;
            wr_ptr = file_reader;
            wr_ptr += prev_total_size;
            write_offset = prev_total_size;
            read_offset = 0;
        }
        bytes_read = read(fp, wr_ptr, RESEND_REQ_FILE_READ_CHUNK);
        total_bytes_read = bytes_read + (write_offset - read_offset);
        bytes_used = rr->parse_resend_req(rd_ptr, &seq, total_bytes_read,
                                          cum_bytes_used, rr->beg, rr->end,
                                          &beg_off, &end_off);
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
        if (end_off > 0 || (rr->end == 0 && (seq >= rr->beg))) {
            should_parse = 0;
        }
        if(bytes_read <= 0) {
            should_parse = 0;
        }
    }
    if (end_off > beg_off) {
        cp_len = end_off - beg_off;
    } else if(end_off == 0 && beg_off == 0 && rr->beg > 1) {
        rr->output = 0;
        rr->len = 0;
        goto done_with_resend;
    } else {
        end_off = lseek(fp, 0, SEEK_END);
        cp_len = end_off - beg_off;
    }

    rr->output = calloc(1, (cp_len + 1));
    lseek(fp, beg_off, SEEK_SET);
    read_bytes = read(fp, rr->output, cp_len);
    while (read_bytes < cp_len) {
        char *off = rr->output;
        size_t b_left = cp_len - read_bytes;
        size_t r_bytes = 0;
        off += read_bytes;
        r_bytes = read(fp, off, b_left);
        if(r_bytes <= 0) {
            break;
        }
        read_bytes += r_bytes;
    }
    rr->len = read_bytes;
    //lseek(fp, offset, SEEK_SET);
done_with_resend:
    end_off = lseek(fp, 0, SEEK_END);
    free(file_reader);
}
