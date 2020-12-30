/*************************************************************************
 * Copyright (c) 2013 by DART executions, LLC
 *
 * All Rights Reserved
 * ***********************************************************************/
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>

#include "fix_parser.h"
#include "rex_allocator.h"
#include "polling_server.h"
#include "rex_constants.h"
#include "fix_trans.h"
#include "log_mnger.h"
#include "databuf.h"
#include "call_back_wrapper.h"
#include "parser_func.h"


struct fix_parser {
    otl* logger;
    dartfact* objs;
    trans_t* fixtrans;
    void* pbp;
    void (*callback)(void* pbp, dart_order_obj* doj);    
    call_back_wrapper* hb_timer;
    call_back_wrapper* recon_wrap;
    login_info* user;
    rex_con* rc;
    databuf_t* mess_to_send;
    pthread_spinlock_t send_mutex;
    int in_seq;
    int out_seq;
    time_t last_sent;
    time_t last_recv;
    int header_len;
    int is_connected;
};

void clean_fix_parser(fixp* fp)
{
    if(fp && fp->user) {
        if(fp->hb_timer) {
            close(fp->hb_timer->sock);
            destroy_call_back_wrapper(fp->hb_timer);
        }
        if(fp->recon_wrap) {
            close(fp->hb_timer->sock);
            destroy_call_back_wrapper(fp->recon_wrap);
        }
        if(fp->mess_to_send) {
            databuf_destroy(fp->mess_to_send);     
        }
        pthread_spin_destroy(&fp->send_mutex);
        free(fp);
    }
}

void* get_fs_pass_back_pointer(fixp* fp)
{
    return fp->pbp;
}
void set_fs_pass_back_pointer(fixp* fp, void* pbp)
{
    fp->pbp = pbp;
}

void set_fp_header_len(fixp* fp, int header_len)
{
    fp->header_len = header_len;
}

int get_fp_header_len(fixp* fp)
{
    return fp->header_len;
}

fixp* create_fix_parser(void* pass_back_pointer,
                        void (*call_back)(void* pbp, dart_order_obj* doj),
                        dartfact* d, otl* o, trans_t* t, int num_obj)
{
    fixp* fp = 0;
    int ret = 0;
    REX_ALLOC(fp, 8, sizeof(struct fix_parser), ret);
    fp->logger = o;
    fp->pbp = pass_back_pointer;
    fp->callback = call_back;
    fp->objs = d;//
    fp->fixtrans = t;
    fp->user = 0;
    pthread_spin_init(&fp->send_mutex, PTHREAD_PROCESS_PRIVATE);
    fp->mess_to_send = databuf_alloc(1024);
    fp->header_len = FIX_BEG_MESSAGE_LEN;
    return fp;
}
fixp* clone_fix_parser(fixp* orig)
{
    fixp* fp = 0;
    int ret = 0;
    REX_ALLOC(fp, 8, sizeof(struct fix_parser), ret);
    fp->logger = orig->logger;
    fp->pbp = orig->pbp;
    fp->callback = orig->callback;
    fp->objs = orig->objs;//
    fp->fixtrans = orig->fixtrans;
    fp->user = 0;
    pthread_spin_init(&fp->send_mutex, PTHREAD_PROCESS_PRIVATE);
    fp->mess_to_send = databuf_alloc(1024);
    fp->header_len = orig->header_len;
    return fp;
}
void copy_pbp_and_cbp(fixp* fp, fixp* orig)
{
    fp->pbp = orig->pbp;
    fp->callback = orig->callback;
}

ofp* get_message_for_type(fixp* fp, short type)
{
   return get_fixy_message(fp->fixtrans, type);
}

void return_fix_message(fixp* fp, ofp* p)
{
    destroy_fixy_message(fp->fixtrans, p);
}
trans_t* get_trans(fixp* fp)
{
    return fp->fixtrans;
}
                        
dart_order_obj* trans_fix_msg(fixp* fp, char* fix_msg, int len, short type)
{
    dart_order_obj* doj = get_doj(fp->objs);//doj->dalloc = fp->objs;
    translate_fix_msg(fp->fixtrans, fix_msg, len, doj, type);
    return doj;
}
void no_create_trans(fixp* fp,  char* fix_msg, 
                     int flen, dart_order_obj* doj,short type)
{
    translate_fix_msg(fp->fixtrans, fix_msg, flen,doj, type);
}
void no_create_copy_trans(fixp* fp, char* fix_msg, int len,
                          dart_order_obj* doj, short type)
{
    trans_n_copy(fp->fixtrans, fix_msg, len, doj, type);
}

void return_dart_obj(fixp* fp, dart_order_obj* doj)
{
   reset_doj(fp->objs, doj);
}

void fp_callback(fixp* fp, dart_order_obj* doj)
{
    fp->callback(fp->pbp, doj);
}

void set_log_info(fixp* fp, login_info* in)
{
    fp->user = in;
}

void set_rex_con(fixp* fp, rex_con* rc)
{
    fp->rc = rc;
}
rex_con* get_fs_rex_con(fixp* fp)
{
    return fp->rc;
}
void set_in_seq(fixp* fp, int in)
{
    fp->in_seq = in;
}
void inc_in_seq_n_set_recv(fixp* fp)
{
    fp->in_seq++;
    time_t now = 0;
    time(&now);
    fp->last_recv = now;
}

void set_out_seq(fixp* fp, int out)
{
    fp->out_seq = out;
}

int get_in_seq(fixp* fp)
{
    return fp->in_seq;
}

int get_out_seq(fixp* fp)
{
    return fp->out_seq;
}

static int s_wrapper(int sock,fixp* fp, char* data, int len, int* disconnect)
{
    int sent = 0;
    int ret = 0;
    do {
        ret = send(sock, data, len, MSG_DONTWAIT | MSG_NOSIGNAL);
        if(ret < 0) {
            ret = errno;
            if(ret == EAGAIN || ret == EWOULDBLOCK || ret == EINTR ||
                    ret == 0) {
                ret = 0;
            } else {
                send_debug_message(fp->logger, "Got this error on send: %d, killing con \n",
                        ret);
                ret = -1;
                *disconnect = 1;
            }
        } else {
            data += ret;
            sent += ret;
            len -= ret;
        }
    } while(len > 0 && ret > 0);
    return sent;
}

static void fp_send(fixp* fp, databuf_t* buff)
{
    int bytes_sent = 0;
    char* bytes = 0;
    int blen = 0;
    if(databuf_unread(fp->mess_to_send) <= 0) {
        bytes = buff->rd_ptr;
        blen = databuf_unread(buff);
    } else {
        databuf_memcpy(fp->mess_to_send, buff->rd_ptr, databuf_unread(buff));
        bytes = fp->mess_to_send->rd_ptr;
        blen = databuf_unread(fp->mess_to_send);     
    }
    bytes_sent = s_wrapper(fp->rc->sock, fp, bytes, blen, &fp->rc->disconnect);
    if(bytes_sent < blen && fp->rc->disconnect <= 0) {
        if(databuf_unread(fp->mess_to_send) <= 0) {
            buff->rd_ptr += bytes_sent;
            databuf_memcpy(fp->mess_to_send,buff->rd_ptr, databuf_unread(buff));
        } else {
            fp->mess_to_send->rd_ptr += bytes_sent;
        }
        add_sender(fp->rc->p, fp->rc);
    }
}

void fp_write_callback(void* cb)
{
    rex_con* rc = (rex_con*)cb;
    fixp* fp = (fixp*)rc->parser;
    pthread_spin_lock(&fp->send_mutex);
    char* data = fp->mess_to_send->rd_ptr;
    int blen = databuf_unread(fp->mess_to_send);
    int bytes_sent = s_wrapper(rc->sock, fp, data, blen, &rc->disconnect);
    fp->mess_to_send->rd_ptr += bytes_sent;
    pthread_spin_unlock(&fp->send_mutex);
    if(bytes_sent < blen && fp->rc->disconnect <= 0) {
        add_sender(rc->p, rc);
    } 
}


void fp_send_fix_message(ofp* obj, fixp* con)
{
    pthread_spin_lock(&con->send_mutex);
    time_t now;
    struct tm tmval;
    struct tm *t = &tmval;
    time(&now);
    if ((t = gmtime_r(&now, &tmval)) == 0) {
        perror("Calling gmtime_r");
    }
    set_time_tag(obj,&tmval, 52); 
    set_num_val(obj, 34, con->out_seq);
    set_tag_nine(obj);                      
    set_check_sum(obj);
    con->last_sent = now;
    databuf_t* b = get_message(obj);
    fp_send(con, b);
    ++con->out_seq;
    pthread_spin_unlock(&con->send_mutex);
   
    record_raw_message(con->logger, con->user->name,
                       con->user->nlen,
                       b->buffer, databuf_unread(b), 0x1);
}

void send_gap_fill(fixp* fp, long beg_seq, long seq)
{
    ofp* obj = get_fixy_message(fp->fixtrans, 0x34);
    set_num_val(obj, 34, beg_seq);
    pthread_spin_lock(&fp->send_mutex);
    time_t now;
    struct tm tmval;
    struct tm *t = &tmval;
    time(&now);
    if ((t = gmtime_r(&now, &tmval)) == 0) {
        perror("Calling gmtime_r");
    }
    set_time_tag(obj,&tmval, 52); 
    if(seq == -1) {
        set_num_val(obj, 36, fp->out_seq);
    } else {
        set_num_val(obj, 36, (unsigned long)seq);
    }
    set_tag_nine(obj);                      
    set_check_sum(obj);
    fp->last_sent = now;
    fp_send(fp, get_message(obj));
    pthread_spin_unlock(&fp->send_mutex);
    destroy_fixy_message(fp->fixtrans, obj);
}

void send_logout(fixp * fco, char* message, int m_len)
{
    ofp* test = get_fixy_message(fco->fixtrans, 0x35);
    set_fix_val(test, 58, message, m_len);
    fp_send_fix_message(test, fco); 
    destroy_fixy_message(fco->fixtrans, test);
}
void request_resend(fixp* fp, 
        unsigned long start, unsigned long end)
{
    ofp* test = get_fixy_message(fp->fixtrans, 0x32);
    set_num_val(test, 7, start);
    set_num_val(test, 16, end);
    fp_send_fix_message(test, fp); 
    destroy_fixy_message(fp->fixtrans, test);
}

int validate_sender_comp(fixp* fp, dart_order_obj* doj)
{
    int is_valid = 0;
    if(fp->user) {
        if(getplen(doj, ROM_TRADER) == fp->user->nlen &&
                (strncmp(fp->user->name,
                         getpval(doj, ROM_TRADER), fp->user->nlen) == 0)) {
            is_valid = 1;
        }
    }
    return is_valid;
}

void set_fix_parser_con(fixp* fp, int is_connected)
{
    fp->is_connected = is_connected;
}
void send_test_req(fixp * fp)
{
    ofp* test = get_fixy_message(fp->fixtrans, 0x31);
    set_fix_val(test, 112, "TEST", 4);
    fp_send_fix_message(test, fp); 
    destroy_fixy_message(fp->fixtrans, test);
}

void send_heart_beat(void * v)
{
    fixp *fco = (fixp *) v;
    if(fco->is_connected) {
        time_t now = 0;
        time(&now);
        char buff[256];
        read(fco->hb_timer->sock,buff, 256); 
        long diff = now - fco->last_sent;
        if(diff >= fco->rc->conf->heart_beat_int) {
            ofp* hb = get_fixy_message(fco->fixtrans, 0x30);
            fp_send_fix_message(hb, fco); 
            destroy_fixy_message(fco->fixtrans, hb);
        }
        diff = diff >= fco->rc->conf->heart_beat_int ?
                      fco->rc->conf->heart_beat_int :
                      (fco->rc->conf->heart_beat_int - diff);
        add_back_rep_timer_adjust(fco->rc->p,fco->hb_timer,diff);
        if(now - fco->last_recv > (fco->rc->conf->heart_beat_int * 2)) {
            send_test_req(fco);
        }
    }
}

void start_hb_timer(fixp* fp)
{
    fp->hb_timer =  add_timer(fp->rc->p, fp->rc->conf->heart_beat_int,
                                       0, 1,0, fp,send_heart_beat);
}

void respond_to_tr(fixp* fp, char* d_off, int test_req)
{
    ofp* test = get_fixy_message(fp->fixtrans, 0x30);
    set_fix_val(test, 112, d_off, test_req);
    fp_send_fix_message(test, fp); 
    destroy_fixy_message(fp->fixtrans, test);
}


void send_iovec(fixp* fp, struct iovec* iov, int iovcnt)
{
    if(pthread_spin_lock(&fp->send_mutex) == 0) {
        int bytes_sent = 0;
        if(databuf_unread(fp->mess_to_send) <= 0) {
            int err = 0;
            int iov_unsent = 0;
            bytes_sent = ci_send_bunches(iov, iovcnt, fp->rc->sock,
                                         &err, &iov_unsent);
            if(err == 0) {
                copy_iovecs_to_buff(iov, iov_unsent, fp->mess_to_send,
                                    bytes_sent);
                if(fp->rc->p) {
                    add_sender(fp->rc->p, fp->rc);
                }
            }
        } else {
            copy_iovecs_to_buff(iov, iovcnt, fp->mess_to_send,
                    bytes_sent);
            pthread_spin_unlock(&fp->send_mutex);
            fp_write_callback(fp->rc);
            return;
        }
        pthread_spin_unlock(&fp->send_mutex);
    }
}
static void fs_timer_shutdown(void* w)
{
}
void fix_reconnect(fixp* dp)
{
    if(dp->recon_wrap == 0) {
        dp->recon_wrap = create_call_back_wrapper(dp->rc,
                            reconnect,fs_timer_shutdown, -1);
    } 
    add_recon_timer(dp->rc->p, dp->recon_wrap);
}
void reload_fix_config(fixp* fp)
{
    reload_config(fp->rc->conf);
}

