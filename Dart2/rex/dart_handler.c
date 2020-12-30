/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <netinet/tcp.h>

#include "dart_handler.h"
#include "parser_func.h"
#include "log_mnger.h"
#include "dart_order_fact.h"
#include "rex_seq_request.h"
#include "rex_resend_request.h"
#include "rex_constants.h"
#include "rex_allocator.h"

#define DART_LEN 200

struct dart_parser {
    otl* logger;
    dartfact* objs;
    void* pbp;
    void (*callback)(void* pbp, dart_order_obj* doj);    
    void (*dp_con_notice)(dartp* dp,
            int is_connected, dart_order_obj* logon);
    login_info* user;
    rex_con* rc;
    databuf_t* mess_to_send;
    pthread_spinlock_t send_mutex;
    call_back_wrapper* recon_wrap;
    int in_seq;
    int out_seq;
    int is_logged_on;
    int resend;
    int num_obj;
    int is_server;
};

static struct iovec* def_pos = 0;
static int needs_init = 1;
static void init_def_pos()
{
   if(needs_init) {
       needs_init = 0;
       int ret = 0;
       REX_ALLOC(def_pos, 8,(DART_LEN *  sizeof(struct iovec)), ret);
       def_pos[161].iov_base = "\n";
       def_pos[161].iov_len = 1;
       int rom_tag = 0;
       do {
           ++rom_tag;
           def_pos[rom_tag].iov_base = ",";
           def_pos[rom_tag].iov_len = 1;
           ++rom_tag;
       } while(rom_tag < 160);
   } 
}

static void dartp_reset(dart_order_obj* doj)
{
    init_def_pos();
    memcpy(doj->positions, def_pos, (DART_LEN * sizeof(struct iovec)));
    doj->in_use = 0;
    doj->ex_mess = 0;
    doj->rep = 0;
    db_node_t* db = doj->head;
    do {
        databuf_reset(db->data);
        db = db->next;
    } while(db != NULL);
   doj->current = doj->head;
   def_pos[161].iov_base = "\n";
   def_pos[161].iov_len = 1;
   doj->dest = 0;
   doj->sender = 0;
   doj->childs = 0;
   doj->num_childs = 0;
}

dartp* create_dart_parser(void* pass_back_pointer,void (*dp_con_notice)(dartp* dp,
                                 int is_connected, dart_order_obj* logon),
                        void (*call_back)(void* pbp, dart_order_obj* doj),
                        dartfact* d, otl* o, int num_obj, int resend, int is_server)
{
    dartp* fp = 0;
    int ret = 0;
    REX_ALLOC(fp, 8, sizeof(struct dart_parser), ret);
    fp->logger = o;
    fp->pbp = pass_back_pointer;
    fp->callback = call_back;
    fp->dp_con_notice = dp_con_notice;
    fp->objs = d;// create_dart_order_factory(num_obj);
    fp->user = 0;
    pthread_spin_init(&fp->send_mutex, PTHREAD_PROCESS_PRIVATE);
    fp->is_logged_on = 0;
    fp->num_obj = num_obj;
    fp->resend = resend;
    fp->is_server = is_server;
    fp->mess_to_send = databuf_alloc(1024);
    return fp;
}

void clean_dart_parser(dartp* dp)
{
    free(dp->mess_to_send);
    free(dp);
}

void set_callback(dartp* dp, void(*call_back)(void* pbp, dart_order_obj* doj)) 
{
    dp->callback = call_back;
}

static dart_order_obj* dp_get_doj(dartp* dp)
{
    dart_order_obj* ret = get_doj(dp->objs);
    dartp_reset(ret);
    return ret;
}
void reset_pass_back_point(dartp* dp, void* pbp)
{
    dp->pbp = pbp;
}

void* get_pass_back_point(dartp* dp)
{
    return dp->pbp;
}

void dart_set_rex_con(dartp* dp, rex_con* rc)
{
    dp->rc = rc;
}
rex_con* get_rex_con(dartp* dp)
{
    return dp->rc;
}
int get_logged_on(dartp* dp)
{
    return dp->is_logged_on;
}

void set_logged_on(dartp* dp, int is_logged_on)
{
    dp->is_logged_on = is_logged_on;
}

void dp_return_doj(dartp* dp, dart_order_obj* doj)
{
    reset_doj(dp->objs, doj);
}

static int dart_send_message(dartp* dp, rex_con* co, char *data, size_t len)
{
    int error = 0;
    int return_code = -1;
    return_code =
        send(co->sock, data, len, MSG_DONTWAIT | MSG_NOSIGNAL);
    error = errno;
    if (return_code < 0) {
        if (error == EAGAIN || error == EWOULDBLOCK || error == ENOBUFS
            || error == EINTR || error == 0) {
            return_code = 0;
        } else {
            co->disconnect = 1;
        }
    }
    return return_code;
}

static int local_sock_send(dartp* dp, rex_con * co, char *data, unsigned long len)
{
    long sent_bytes;
    long bytes_to_send;
    char *d_off = data;
    long ret_code = 1;
    int failed_attempts = 0;
    sent_bytes = 0;
    bytes_to_send = (long) len;
    pthread_spin_lock(&dp->send_mutex);
    while (sent_bytes < len && ret_code >= 0) {
        ret_code = dart_send_message(dp, co, d_off, bytes_to_send);
        if (ret_code == 0) {
            ++failed_attempts;
            if (failed_attempts >= 32 || co->disconnect) {
                pthread_spin_unlock(&dp->send_mutex);
                return 0;
            }
        }
        bytes_to_send -= ret_code;
        sent_bytes += ret_code;
        d_off += ret_code;
    }
    pthread_spin_unlock(&dp->send_mutex);
    return sent_bytes;
}
void dart_write_callback(void* cb)
{
    rex_con* rc = (rex_con*)cb;
    dartp* dp = (dartp*)rc->parser;
    pthread_spin_lock(&dp->send_mutex);
    char* data = dp->mess_to_send->rd_ptr;
    int blen = databuf_unread(dp->mess_to_send);
    int bytes_sent = dart_send_message(dp, rc, data, blen);
    dp->mess_to_send->rd_ptr += bytes_sent;
    pthread_spin_unlock(&dp->send_mutex);
    if(bytes_sent < blen && dp->rc->disconnect <= 0) {
        add_sender(rc->p, rc);
    } 
}
void send_raw_csv(dartp* dp, char* data, int len)
{
    int bytes_sent = 0;
    int mts = 0;
    pthread_spin_lock(&dp->send_mutex);
    mts = databuf_unread(dp->mess_to_send);
    if(mts <= 0) {
        bytes_sent = dart_send_message(dp, dp->rc, data, len);
        if(bytes_sent < len) {
            databuf_memcpy(dp->mess_to_send,
                       data + bytes_sent,
                       len - bytes_sent);
            if(dp->rc->p) {
                pthread_spin_unlock(&dp->send_mutex);
                add_sender(dp->rc->p, dp->rc);
                return;
            }
        }
    } else {
        databuf_memcpy(dp->mess_to_send, data, len);
        pthread_spin_unlock(&dp->send_mutex);
        dart_write_callback(dp->rc);
        return;
    }
    pthread_spin_unlock(&dp->send_mutex);
}
void send_dart_message(dartp* dp, dart_order_obj* doj)
{
    if(pthread_spin_lock(&dp->send_mutex) == 0) {
        int bytes_sent = 0;
        if(databuf_unread(dp->mess_to_send) <= 0) {
            int err = 0;
            int iov_unsent = 0;
            bytes_sent = ci_send_bunches(doj->positions, DART_SEND_LEN, dp->rc->sock,
                                         &err, &iov_unsent);
            if(err == 0) {
                copy_iovecs_to_buff(doj->positions, iov_unsent, dp->mess_to_send,
                                    bytes_sent);
                if(dp->rc->p) {
                    add_sender(dp->rc->p, dp->rc);
                }
            }
        } else {
            copy_iovecs_to_buff(doj->positions, DART_SEND_LEN, dp->mess_to_send,
                    bytes_sent);
            pthread_spin_unlock(&dp->send_mutex);
            dart_write_callback(dp->rc);
            return;
        }
        pthread_spin_unlock(&dp->send_mutex);
    }
}


static int create_rom_time(char *mlbk)
{
    time_t rawtime;
    struct tm *ti;
    time(&rawtime);
    struct tm lm;
    ti = gmtime_r(&rawtime, &lm);
    ti = &lm;
    return sprintf(mlbk, "%02d%02d%02d,", ti->tm_hour, ti->tm_min,
                   ti->tm_sec);
}

char *create_rom_resend_pad(unsigned long seq, char *rom, size_t * len)
{
    size_t local_len = 0;
    unsigned long l_seq = 1;
    char *ret_val = calloc(15 * seq, 1);
    char *d_off;
    if (ret_val != NULL) {
        d_off = ret_val;
        while (l_seq <= seq) {
            local_len = sprintf(d_off, "A,,,,,,,,,\n");
            d_off += local_len;
            ++l_seq;
            *len += local_len;
        }
        return ret_val;
    } else {
        return NULL;
    }
}

char *pad_rom_resend(char *rom, size_t len)
{
    size_t bytes_read = 0;
    char *ret_str = calloc(len + 1, 1);
    char *c_off = ret_str;
    char *ret_val = rom;
    char *d_off = rom;
    ret_val = strchr(d_off, '\n');
    while (ret_val && bytes_read < len) {
        if (d_off[0] == 'C') {
            int l = (ret_val - d_off) + 1;
            memcpy(c_off, d_off, l);
            c_off += l;
        } else {
            memcpy(c_off, "A,,,,,,,,,,,,\n", 14);
            c_off += 14;
        }
        bytes_read = (ret_val - d_off);
        d_off = ret_val + 1;
        ret_val = strchr(d_off, '\n');
    }
    return ret_str;
}

void send_dart_logon_for_user(dartp* dp, char* name, int nlen)
{
    rex_con* rc = dp->rc;
    sequence_req *sr;
    char mlbk[512];
    char *mlbk_off;
    int time_len;
    int seq_enabled = 1;
    unsigned long send_len;
    unsigned long send_seq;
    sr = create_sequence_request(name, nlen,
                                 rc->logon,rc->conf->version,
                                 build_offsets,
                                 create_rom_resend_pad, rc);
    get_sequence_numbers(sr, rc->o);
    /* Add the sequences into the connection object */
    send_seq = dsr_get_incoming(sr) + 1;
    dp->in_seq =  send_seq;
    dp->out_seq =  (dsr_get_outgoing(sr) + 1);
    destroy_sequence_req(sr);
    send_len = 0;
    mlbk_off = mlbk;
    memcpy(mlbk_off, "L,", 2);
    mlbk_off += 2;
    send_len += 2;
    time_len = create_rom_time(mlbk_off);
    mlbk_off += time_len;
    send_len += time_len;
    if (seq_enabled) {
        //send_seq = get_incoming_seq(pa->co) + 1;
        send_len +=
            sprintf(mlbk_off,
                    "%s,,,,,%s,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,%lu,,\n",
                    rc->logon->name, rc->conf->password,
                    send_seq);
    } else {
        send_len +=
            sprintf(mlbk_off,
                    "%s,,,,,%s,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n",
                    rc->logon->name, rc->conf->password);
    }
    if (local_sock_send(dp, rc, mlbk, send_len)) {
    } else {
        rc->disconnect = 1;
    }
    send_debug_message(rc->o, "Sent Logon:, %s \n", mlbk);
}
void send_dart_logon(void* p)
{
    dartp* dp = (dartp*)p;
    rex_con* rc = dp->rc;
    sequence_req *sr;
    char mlbk[512];
    char *mlbk_off;
    int time_len;
    int seq_enabled = 1;
    unsigned long send_len;
    unsigned long send_seq;
    sr = create_sequence_request(rc->logon->name, rc->logon->nlen,
                                 rc->logon,rc->conf->version,
                                 build_offsets,
                                 create_rom_resend_pad, rc);
    get_sequence_numbers(sr, rc->o);
    /* Add the sequences into the connection object */
    send_seq = dsr_get_incoming(sr) + 1;
    dp->in_seq =  send_seq;
    dp->out_seq =  (dsr_get_outgoing(sr) + 1);
    destroy_sequence_req(sr);
    send_len = 0;
    mlbk_off = mlbk;
    memcpy(mlbk_off, "L,", 2);
    mlbk_off += 2;
    send_len += 2;
    time_len = create_rom_time(mlbk_off);
    mlbk_off += time_len;
    send_len += time_len;
    if (seq_enabled) {
        //send_seq = get_incoming_seq(pa->co) + 1;
        send_len +=
            sprintf(mlbk_off,
                    "%s,,,,,%s,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,%lu,,\n",
                    rc->logon->name, rc->conf->password,
                    send_seq);
    } else {
        send_len +=
            sprintf(mlbk_off,
                    "%s,,,,,%s,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n",
                    rc->logon->name, rc->conf->password);
    }
    if (local_sock_send(dp, rc, mlbk, send_len)) {
    } else {
        rc->disconnect = 1;
    }
    send_debug_message(rc->o, "Sent Logon:, %s \n", mlbk);
}

unsigned long build_offsets(char *data, unsigned long size,
                            unsigned long byte_offset,
                            unsigned long *seq_num, void* ba)
{
    if (size > 0) {
        char eol[2] = { '\n', '\0' };
        char *d_off;
        unsigned long read_bytes;
        long ret_code;
        unsigned long l_seq;
        read_bytes = 0;
        ret_code = 0;

        l_seq = *seq_num;
        d_off = data;

        ret_code = find_offset(d_off, size, eol, 1);
        while (ret_code >= 0) {
            read_bytes += (unsigned long) ret_code;
            read_bytes += 1;
            d_off = d_off + ret_code + 1;
            ++l_seq;
            ret_code = find_offset(d_off, (size - read_bytes), eol, 1);
        }
        *seq_num = l_seq;
        return read_bytes;
    }
    return size;
}

size_t find_rom_offsets(char *data, size_t * seq,
                        size_t size, size_t byte_offset,
                        size_t beg_seq, size_t end_seq, size_t * beg_off,
                        size_t * end_off)
{
    if (size > 0) {
        char eol[2] = { '\n', 0 };
        char *d_off = data;
        size_t read_bytes = 0;
        long ret_code = 0;
        size_t cur_seq = *seq;
        //TODO I changed the dart_resend object to start a seq 0 rather than one.
        //I would like to change this to a do while loop that increments on the
        //fist run.
        ++cur_seq;
        ret_code = find_offset(d_off, size, eol, 1);
        while (ret_code >= 0) {
            if (beg_seq == cur_seq) {
                *beg_off = (byte_offset + read_bytes);
            }
            read_bytes += (size_t) ret_code;
            read_bytes += 1;
            d_off += (ret_code + 1);
            if (end_seq == cur_seq) {
                *end_off = (byte_offset + read_bytes);
            }
            ++cur_seq;
            ret_code = find_offset(d_off, (size - read_bytes), eol, 1);
        }
        *seq = cur_seq;
        return read_bytes;
    }
    return size;
}

void handle_async_csv_logon(char type, const char *data, unsigned long len,
                            int *cut_con, rex_con *pc, dartp* dp)
{
        dart_order_obj* cur_doj = dp_get_doj(dp);
        dartp* ldp = 0;
        databuf_memcpy(cur_doj->current->data, data,
                                   len);
        ph_build_iovec(cur_doj);

        send_debug_message(pc->o, "Received Logon:, %s \n", data);
        char* user = getpval(cur_doj, ROM_TRADER);
        int nLen = getplen(cur_doj, ROM_TRADER);
        /* Now send Logon. */
        if (pc->is_server) {
            sequence_req *sr;
            char mlbk[512];
            char *mlbk_off;
            int time_len;
            unsigned long send_len;
            unsigned long send_seq;
            unsigned long out_seq = 0;
            if (nLen > 0) {
                /* Here we need to cut the connection if it does not
                 * conform with any of our init structs. */
                pc->logon = pc->is_valid(pc->con_pass_back,cur_doj);
            }
            if (nLen == 0 || pc->logon == NULL) {
                pc->removed = 1;
                *cut_con = 1;
                dp_return_doj(dp, cur_doj);
                return;
            }
            if (check_is_connected(pc->p, user, nLen)) {
                *cut_con = 1;
                pc->removed = 1;
                dp_return_doj(dp, cur_doj);
                return;
            }
            /*Create the parser if it does not exist*/
            ldp = create_dart_parser(pc->con_pass_back,dp->dp_con_notice, dp->callback, 
                                              dp->objs,pc->o,
                                              pc->conf->order_allocation, 1, 1);
            pc->parser = ldp;
            ldp->rc = pc; 
            /* We need to get our sequnces: */
            sr = create_sequence_request(user, nLen,
                                         pc->logon,pc->conf->version,
                                         build_offsets,
                                         create_rom_resend_pad, pc);
            get_sequence_numbers(sr, pc->o);
            /* Add the sequences into the connection object */
            send_seq = dsr_get_incoming(sr) + 1;//set_incoming_seq(pc->co, dsr_get_incoming(sr));
            out_seq = dsr_get_outgoing(sr);//set_incoming_seq(pc->co, dsr_get_incoming(sr));
            destroy_sequence_req(sr);
            send_len = 0;
            mlbk_off = mlbk;
            memcpy(mlbk_off, "L,", 2);
            mlbk_off += 2;
            send_len += 2;
            time_len = create_rom_time(mlbk_off);
            mlbk_off += time_len;
            send_len += time_len;
            send_len +=
                sprintf(mlbk_off,
                        "%s,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,%lu,,\n",
                        pc->conf->mod_name, send_seq);
            if (local_sock_send(ldp, pc, mlbk, send_len)) {
                dartp* pp = (dartp*)pc->parser;
                pp->in_seq = send_seq;
                pp->out_seq = out_seq;
                pp->user = pc->logon;
                send_debug_message(pc->o, "Sent Logon:, %s \n", mlbk);
            } else {
                pc->disconnect =1;
                *cut_con = 1;
                dp_return_doj(dp, cur_doj);
                return;
            }
        } else {
            ldp = dp;
        }
        ((dartp*)pc->parser)->is_logged_on = 1;
        ldp->dp_con_notice(ldp, 2, cur_doj);
        /* Now send resend if we need to. */
        int out_seq = ((dartp*)pc->parser)->out_seq;
        int last_rom_seq = 0;
        if(getplen(cur_doj, 144) > 0) {
            last_rom_seq = atoi(getpval(cur_doj, 144));
        }
        if (last_rom_seq <= out_seq && out_seq > 1 && last_rom_seq > 0) {
            char *output = NULL;
            size_t out_len = 0;
            resend_req *rr = create_resend_request(getpval(cur_doj, ROM_TRADER),
                                                   getplen(cur_doj, ROM_TRADER),
                                                   last_rom_seq, 0,
                                                   find_rom_offsets);
            get_resend_request(rr, pc->o);
            output = (char *) drr_get_resend_output(rr, &out_len);
            if (out_len > 0 && ((dartp*)pc->parser)->resend) {
                int send_ret = 1;
                send_ret = local_sock_send(ldp, pc, output, out_len);
                if (send_ret <= 0) {
                    *cut_con = 1;
                }
            }
            destroy_resend_req(rr);
        }
        ldp->dp_con_notice(ldp, 1, cur_doj);
        dp_return_doj(dp, cur_doj);
}


void ph_build_iovec(dart_order_obj * doj)
{
    long len = databuf_unread(doj->current->data);
    char *c = doj->current->data->buffer;
    char *d_off = c;
    int i = 0;
    for (; *c != '\n' && (c - doj->current->data->buffer) < len; ++c) {
        if (*c == ',') {
            doj->positions[i].iov_len = c - d_off;
            doj->positions[i].iov_base = d_off;
            d_off = c + 1;
            i += 2;
        }
    }
    if (c - d_off > 0) {
        doj->positions[i].iov_len = c - d_off;
        doj->positions[i].iov_base = d_off;
    }
}

#ifdef _SPREAD_N_READY_

#define ROM_SPREAD_SEP "!#!"

static void ph_build_non_fix_rom_pos(char *raw_bytes, long rom_len, rex_con *rc, dartp* dp)
{
    sock_mnger* rph = rc->con;
    long len_left;
    long val_end;
    char *vend_ptr = 0;
    val_end = 0;
    len_left = rom_len;
    vend_ptr = memchr(rph->rd_ptr, '\n', len_left);
    while (vend_ptr) {
        val_end = (vend_ptr - rph->rd_ptr);
        len_left -= (val_end + 1);
        if(rph->rd_ptr[0] == 'H') {
            local_sock_send(dp, rc, "H,\n", 3);
            rph->rd_ptr += val_end + 1;
            vend_ptr = memchr(rph->rd_ptr, '\n', len_left);
        } else {
            dart_order_obj* cur_doj = (dart_order_obj *) dp_get_doj(dp);
            char* leg_off = strstr(rph->rd_ptr, ROM_SPREAD_SEP);
            if(leg_off == NULL || ((leg_off - rph->rd_ptr) > val_end)) {
                int copied = databuf_memcpy(cur_doj->current->data, rph->rd_ptr,
                                           val_end + 1);
                if(copied >= 0) {
                    ph_build_iovec(cur_doj);
                    set_rom_field(cur_doj, ROM_SENDER, rc->logon->name,
                                  rc->logon->nlen);
                    if (rc->logon->nlen > 0) {
                        record_raw_message(rc->o, rc->logon->name, rc->logon->nlen, rph->rd_ptr, val_end + 1, 0x0);
                        record_inbound_message(rc->o,1, rc->logon->name, rc->logon->nlen);
                    }
                    dp->callback(dp->pbp, cur_doj);
                    rph->rd_ptr += val_end + 1;
                    vend_ptr = memchr(rph->rd_ptr, '\n', len_left);
                } else if(rc->logon->nlen > 0) {
                    send_debug_message(rc->o, "Unable to parse this message, %lu \n", val_end);
                    record_raw_message(rc->o, rc->logon->name, rc->logon->nlen, rph->rd_ptr, val_end + 1, 0x0);
                    rph->rd_ptr += val_end + 1;
                }
            } else {
                if (rc->logon->nlen > 0) {
                    record_raw_message(rc->o, rc->logon->name, rc->logon->nlen, rph->rd_ptr, val_end + 1, 0x0);
                    record_inbound_message(rc->o,1, rc->logon->name, rc->logon->nlen);
                }
                /*We have found the spread sep*/
                long slen_left = val_end;
                char* sprd_beg = rph->rd_ptr;
                long sprd_len = leg_off - sprd_beg;
                databuf_memcpy(cur_doj->current->data, sprd_beg,
                                           sprd_len + 1);
                ph_build_iovec(cur_doj);
                set_rom_field(cur_doj, ROM_SENDER, rc->logon->name,
                                  rc->logon->nlen);
                set_rom_field(cur_doj, ROM_CPLX_TYPE, "3", 1);
                sprd_len += 3;
                sprd_beg += sprd_len;
                slen_left -= sprd_len;
                /*Loop through and collect children*/
                while(slen_left > 4) {
                    leg_off = strstr(sprd_beg, ROM_SPREAD_SEP);
                    if((leg_off == NULL && slen_left > 0) ||
                        (leg_off - sprd_beg > slen_left)) {
                        sprd_len = slen_left;
                    } else {
                        sprd_len = leg_off - sprd_beg;
                    }
                    dart_order_obj* child_doj = (dart_order_obj *) dp_get_doj(dp);
                    databuf_memcpy(child_doj->current->data, sprd_beg,
                                           sprd_len + 1);
                    ph_build_iovec(child_doj);
                    set_rom_field(child_doj, ROM_CPLX_TYPE, "6", 1);
                    set_rom_field(child_doj, ROM_SENDER, rc->logon->name,
                                  rc->logon->nlen);
                    add_child_obj(cur_doj, child_doj);
                    if(cur_doj->num_childs == 1) {
                        set_rom_field(child_doj, ROM_LEG_MARK, "1", 1);
                    } else {
                        set_rom_field(child_doj, ROM_LEG_MARK, "0", 1);
                    }
                    sprd_len += 3;
                    sprd_beg += sprd_len;
                    slen_left -= sprd_len;
                }
                /*DONE WITH SPREADS*/
                dp->callback(dp->pbp, cur_doj);
                rph->rd_ptr += val_end + 1;
                vend_ptr = memchr(rph->rd_ptr, '\n', len_left);
            }
        }
    }
}

#else

static void ph_build_non_fix_rom_pos(char *raw_bytes, long rom_len,  rex_con *rc, dartp* dp)
{
    sock_mnger* rph = rc->con;
    long len_left;
    long val_end;
    char *vend_ptr = 0;
    val_end = 0;
    len_left = rom_len;
    vend_ptr = memchr(rph->rd_ptr, '\n', len_left);
    while (vend_ptr) {
        val_end = (vend_ptr - rph->rd_ptr);
        len_left -= (val_end + 1);
        if(rph->rd_ptr[0] == 'H') {
            local_sock_send(dp, rc, "H,\n", 3);
            rph->rd_ptr += val_end + 1;
            vend_ptr = memchr(rph->rd_ptr, '\n', len_left);
        } else {
            dart_order_obj* cur_doj = dp_get_doj(dp);
            int copied = databuf_memcpy(cur_doj->current->data, rph->rd_ptr,
                                       val_end + 1);
            if(copied >= 0) {
                ph_build_iovec(cur_doj);
                set_rom_field(cur_doj, ROM_SENDER, rc->logon->name,
                              rc->logon->nlen);
                if (rc->logon->nlen > 0) {
                    record_raw_message(rc->o, rc->logon->name, rc->logon->nlen,
                                       rph->rd_ptr, val_end + 1, 0x0);
                    record_inbound_message(rc->o, 1, rc->logon->name,
                                           rc->logon->nlen);
                }
                dp->callback(dp->pbp, cur_doj);
                rph->rd_ptr += val_end + 1;
                vend_ptr = memchr(rph->rd_ptr, '\n', len_left);
            } else if(rc->logon->nlen > 0) {
                send_debug_message(rc->o, "Unable to parse this message, %lu \n", val_end);
                record_raw_message(rc->o, rc->logon->name, rc->logon->nlen, rph->rd_ptr, val_end + 1, 0x0);
                rph->rd_ptr += val_end + 1;
            }
        }
    }
}
#endif

void ph_parse_rom_message(int *cut_con, rex_con *apa)
{
    sock_mnger* pc = apa->con;
    long len = (pc->wr_ptr - pc->rd_ptr);
    char* raw_bytes = pc->rd_ptr;
    dartp* dp = (dartp*)apa->parser;
    if (len > 0 && apa->parser != NULL && dp->is_logged_on) {
        dartp* dp = (dartp*)apa->parser;
        ph_build_non_fix_rom_pos(pc->rd_ptr, len, apa, dp);
    } else if (len > 0 && raw_bytes[0] == 'L') {
        char *end = memchr(raw_bytes, '\n', len);
        if (end != NULL && (end - raw_bytes) <= len) {
            unsigned long log_len = (end - raw_bytes) + 1;
            handle_async_csv_logon('L', raw_bytes, log_len, cut_con, apa, dp);
            pc->rd_ptr += log_len;
            if(*cut_con) {
                send_debug_message(apa->o, "Failed Logon, cut con. %lu \n", len);
            }
            if (log_len < len) {
                ph_build_non_fix_rom_pos(pc->rd_ptr, len - log_len, apa, dp);
            } 
        }
    } else if(len > 0 && raw_bytes[0] == 'K') {
        *cut_con = 1;
    } else if(len > 0 && !dp) {
        *cut_con = 1;
        send_debug_message(apa->o, "sending data without being logged on, cut con. %lu \n", len);
    }
}
void reload_dartp_config(dartp* dp)
{
    reload_config(dp->rc->conf);
}


static void dp_timer_shutdown(void* w)
{
}
void dart_reconnect(dartp* dp)
{
    dp->is_logged_on = 0;
    if(dp->recon_wrap == 0) {
        dp->recon_wrap = create_call_back_wrapper(dp->rc,
                            reconnect,dp_timer_shutdown, -1);
    } 
    add_recon_timer(dp->rc->p, dp->recon_wrap);
}
int get_sock(dartp* dp)
{
    return dp->rc->sock;
}
