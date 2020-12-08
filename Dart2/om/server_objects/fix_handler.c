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
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "fix_handler.h"
#include "dart_constants.h"
#include "ex_config_helper.h"
#include "parser_func.h"
#include "transaction_recorder.h"
#include "hashmap.h"
#include "dart_seq_request.h"
#include "tm_to_str.h"
#include "debug_logger.h"
#include "fix_trans.h"
#include "version.h"
#include "hmac_helper.h"

#define DEF_FIX_SIZE 1400
#define HEADER_TAIL_LEN 200
#define CHAR_BIT 8

#ifdef _CHECK_SEQ__
typedef struct fix_resend_req {
    long beg;
    long end;
}frr;
typedef struct fix_resend_handler {
    long beg_pending;
    long end_pending;
    queue_t resends;
    queue_t sent_resends;
}frh;
#endif
static int create_timestamp(char *mblk, int len, time_t * now)
{
    time_t rawtime;
    struct tm *ti;
    int retval = -1;
    time(&rawtime);
    struct tm lm;
    ti = gmtime_r(&rawtime, &lm);
    ti = &lm;
    *now = rawtime;
    if (len > TIMESTAMP_LEN && tm_to_str(ti, mblk, len) == 0) {
        mblk += TIMESTAMP_LEN;
        *mblk = '\x01';
        retval = TIMESTAMP_LEN + 1;
    }

    return retval;
}

static int iov_calc_check_sum(const struct iovec *iov, int iovcnt)
{
    int sum = 0;
    int i;
    const char *c;
    int j = 0;
    for (; j < iovcnt; ++j) {
        c = iov[j].iov_base;
        for (i = 0; i < iov[j].iov_len; ++i) {
            sum = (sum + *c);
            ++c;
        }
    }
    sum = sum % 256;
    return sum;
}

void dart_send_fix_message(ofp* obj, con_interface* con)
{
    pthread_spin_lock(&con->send_mutex);
    time_t now;
    struct tm tmval;
    struct tm *t = &tmval;
    time(&now);
    if ((t = gmtime_r(&now, &tmval)) == 0) {
        perror("Calling gmtime_r");
    }
    trans_t* fix_t = (trans_t*)con->parser;
    set_time_tag(fix_t, obj,&tmval, 52); 
    set_num_val(fix_t, obj, 34, con->seq);
    set_tag_nine(fix_t, obj);                      
    set_check_sum(fix_t, obj);
    con->last_sent = now;
    databuf_t* b = get_message(obj);
    ci_no_lock_sender(con, b);
    ++con->seq;
    pthread_spin_unlock(&con->send_mutex);
   
    record_raw_message(con->session_conf->name,
                       con->session_conf->name_len,
                       b->buffer, databuf_unread(b), 0x1);
}

void send_cme_logon(ofp* obj, con_interface* con)
{
    pthread_spin_lock(&con->send_mutex);
    time_t now;
    struct tm tmval;
    struct tm *t = &tmval;
    time(&now);
    if ((t = gmtime_r(&now, &tmval)) == 0) {
        perror("Calling gmtime_r");
    }
    trans_t* fix_t = (trans_t*)con->parser;
    set_time_tag(fix_t, obj,&tmval, 52); 
    set_num_val(fix_t, obj, 34, con->seq);
    /*Now we need to add goofy cme tags*/ 
    databuf_t * cmessage = databuf_alloc(1024);
    set_cannonical_str(fix_t, obj, cmessage);
    unsigned char md[2048];
    char* dkey;
    int keylen = decode_base64((uint8_t*)con->session_conf->secretkey,
            con->session_conf->secretlen, &dkey);
    uint32_t md_len = 0;
    unsigned char* result = dart_hmac_sha256(dkey, keylen,
        (unsigned char*)cmessage->rd_ptr, databuf_unread(cmessage), md,
        &md_len);
    if(md_len > 0 && result != NULL) {
        char* buffer;
        int elen = encode_base64(md, md_len, &buffer); 
        set_num_val(fix_t, obj, 354, con->session_conf->accesslen);
        set_fix_val(fix_t, obj, 355, con->session_conf->accesskey,
                con->session_conf->accesslen);
        set_fix_val(fix_t, obj, 1400, "CME-1-SHA-256", 13);
        set_num_val(fix_t, obj, 1401, elen);
        set_fix_val(fix_t, obj, 1402, buffer, elen);
        free (buffer);
    }
    /*Done with cme tags*/
    set_tag_nine(fix_t, obj);                      
    set_check_sum(fix_t, obj);
    con->last_sent = now;
    databuf_t* b = get_message(obj);
    ci_no_lock_sender(con, b);
    ++con->seq;
    pthread_spin_unlock(&con->send_mutex);
   
    record_raw_message(con->session_conf->name,
                       con->session_conf->name_len,
                       b->buffer, databuf_unread(b), 0x1);
    if(keylen > 0) {
        free(dkey);
    }
}
static long get_sequence(char *mblk, size_t len)
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
/**
 *
 */
unsigned long build_fix_offsets(char *data, unsigned long size,
        unsigned long byte_offset,
        unsigned long *seq_num, void* b)
{
    if (size > 0 && size > (FIX_BEG_MESSAGE_LEN + 9)) {
        char* d_off = data;
        unsigned long len =  size;
        int byte_len = 0;
        int roll_back = len;
        int jump = 0;
        do {
            d_off += FIX_BEG_MESSAGE_LEN;
            len -= FIX_BEG_MESSAGE_LEN;
            if(d_off[0] == '=') {
                d_off += 1;
                len -= 1;
            }
            byte_len = atoi(d_off);
            NEXT_ONE(d_off, len);
            jump = (byte_len + FIX_CHECKSUM_LEN + 1);
            if(len >= jump) {
                d_off += jump;
                len -= jump;
                ++*seq_num;
                roll_back = len;
            } else {
                break;
            }
        } while(len > (FIX_BEG_MESSAGE_LEN + 9));
        return size - roll_back;
    }
    return size;
}
size_t find_fix_offsets(char *data, size_t * seq_off, size_t size,
        size_t byte_offset, size_t beg_seq, size_t end_seq,
        size_t * beg_off, size_t * end_off)
{
    char* d_off = data;
    int len = (int) size;
    int byte_len = 0;
    int roll_back = len;
    int jump = 0;
    do {
        d_off += FIX_BEG_MESSAGE_LEN;
        len -= FIX_BEG_MESSAGE_LEN;
        if(d_off[0] == '=') {
            d_off += 1;
            len -= 1;
        }
        byte_len = atoi(d_off);
        NEXT_ONE(d_off, len);
        jump = (byte_len + FIX_CHECKSUM_LEN + 1);
        if(len >= jump) {
            d_off += jump;
            len -= jump;
            ++*seq_off;
            if(*seq_off == beg_seq) { *beg_off = byte_offset + (size - roll_back);} 
            if(*seq_off == end_seq) {
               *end_off = byte_offset + (size - len); //end of last message.
               roll_back = len;
               break;
            }
            roll_back = len;
        } else {
            break;
        }
    } while(len > (FIX_BEG_MESSAGE_LEN + 9));
    return ((roll_back >= 0) ? size - roll_back : size);
}

static void request_resend(con_interface* pc, 
        unsigned long start, unsigned long end)
{
    trans_t* fix_t = (trans_t*)pc->parser;
    ofp* test = get_fixy_message(fix_t, 0x32);
    set_num_val(fix_t, test, 7, start);
    set_num_val(fix_t, test, 16, end);
    dart_send_fix_message(test, pc); 
    destroy_fixy_message(fix_t, test);
}

char *create_fix_resend_pad(unsigned long seq, char *fix_version,
        size_t * len)
{
    int local_len = 0;
    int m_len = 0;
    unsigned long l_seq = 1;
    char *f_mess = calloc(200, 1);
    char *ret_val = calloc(200 * seq, 1);
    char *d_off;
    if (ret_val != NULL) {
        d_off = ret_val;
        while (l_seq <= seq) {
            m_len = sprintf(f_mess, "35=4%c34=%lu%c10=023%c",
                    FIX_FIELD_DELIM,
                    l_seq, FIX_FIELD_DELIM, FIX_FIELD_DELIM);

            local_len = sprintf(d_off, "8=%s%c9=%d%c", 
                    fix_version, FIX_FIELD_DELIM,
                    (m_len - FIX_CHECKSUM_LEN),
                    FIX_FIELD_DELIM);
            if (local_len > 0) {
                d_off += local_len;
                ++l_seq;
                *len += local_len;
            }
            memcpy(d_off, f_mess, m_len);
            d_off += m_len;
            *len += m_len;
        }
        free(f_mess);
        return ret_val;
    } else {
        free(f_mess);
        return NULL;
    }
}
int send_fix_logon(async_parse_args* pc)
{
    con_interface* pa = (con_interface*)pc->ds_book;
    pa->ap = pc;
    pa->sock = pc->sock;
    sequence_req* sr = create_sequence_request(pa->ap->in_args->name,//sender_comp,
                                 pa->ap->in_args->name_len,//sc_len,
                                 pa->ap->in_args,
                                 build_fix_offsets,
                                 create_fix_resend_pad, pa);
    get_sequence_numbers(sr);
    /* Add the sequences into the connection object */
    pc->in_seq = dsr_get_incoming(sr);
    pc->out_seq = dsr_get_outgoing(sr) + 1;
    pa->seq = dsr_get_outgoing(sr) + 1;
    destroy_sequence_req(sr);
    trans_t* fix_t = (trans_t*)pa->parser;
    ofp* test = get_fixy_message(fix_t, 0x41);
    if(pa->ap->in_args->secretlen > 0 &&
            pa->ap->in_args->accesslen > 0) {
        send_cme_logon(test, pa);
    } else {
        dart_send_fix_message(test, pa); 
    }
    destroy_fixy_message(fix_t, test);
    return 0;
}

static struct init_args *validate_target_comp(char *name, size_t name_len,
        queue_t in_args)
{
    if (name != NULL && name_len > 0 && in_args != NULL) {
        element_t e;
        e = front(in_args);
        while (e != NULL) {
            struct init_args *init;
            init = (struct init_args *) e->d;
            if (init != NULL) {
                if (init->tc_len == name_len
                        && (strncmp(name, init->target_comp, name_len) == 0)) {
                    return init;
                }
                e = e->next;
            } else {
                send_debug_message("Struct was null element was not: %s\n",
                        name);
                e = e->next;
            }
        }
        send_debug_message("Element was null for: %s\n", name);
        return NULL;
    } else {
        send_debug_message("Name was null or name_len was 0 or less.\n");
        return NULL;
    }
}
#ifdef _CHECK_SEQ__

static void pop_resend(frh* fhelp, con_interface* con, async_parse_args* pc)
{
    element_t e = pop_front(fhelp->resends);
    frr* f = (frr*)e->d; 
    request_resend((con_interface*)pc->ds_book, f->beg, f->end);
    fhelp->beg_pending = f->beg;
    fhelp->end_pending = f->end;
    free_element(fhelp->resends, e);
    free(f);
}
static void cme_resend_breakup(con_interface* con, async_parse_args* pc, int seq)
{
    int expected = pc->in_seq + 1;
    int rem = seq - expected;
    if(pc->phelp == 0x0) {
       frh* fhelp = calloc(1, sizeof(frh));
       fhelp->resends = create_sized_queue(8); 
       fhelp->sent_resends = create_sized_queue(8); 
       pc->phelp = fhelp;
    }
    frh* fhelp = (frh*)pc->phelp;
    do {
        frr* f = calloc(1, sizeof(frr));
        f->beg = expected;
        f->end = expected + 2400;
        enqueue(fhelp->resends, f);
        expected += 2401;
        rem -= 2400;
    } while(rem > 2400);
    if(rem > 0) {
        frr* f = calloc(1, sizeof(frr));
        f->beg = expected;
        f->end = seq;
        enqueue(fhelp->resends, f);
    }
    pop_resend(fhelp, con, pc);
}

static void cme_small_resend_req(con_interface* con, async_parse_args* pc,
                                 int expected, int seq)
{
    if(pc->phelp == 0x0) {
       frh* fhelp = calloc(1, sizeof(frh));
       fhelp->resends = create_sized_queue(8); 
       fhelp->sent_resends = create_sized_queue(8); 
       pc->phelp = fhelp;
    }
    frh* fhelp = (frh*)pc->phelp;
    if(fhelp->end_pending > 0) {
        frr* f = calloc(1, sizeof(frr));
        f->beg = expected;
        f->end = seq;
        enqueue(fhelp->resends, f);
    } else {
        fhelp->beg_pending = expected;
        fhelp->end_pending = seq;
        request_resend((con_interface*)pc->ds_book, expected, seq);
    }
}
#endif

static void handle_fix_logon(con_interface* con, async_parse_args* pc, 
                             char* logon, int l_len, int* cut_con)
{
    trans_t* fix_t = (trans_t*)con->parser;
    dart_order_obj* rej = (dart_order_obj *) get_gk_obj(pc->gk);
    translate_fix_msg(fix_t,  logon, l_len, 
                       rej, 0x41);
    struct init_args* ia = validate_target_comp(getpval(rej, ROM_TRADER), 
                                                getplen(rej, ROM_TRADER),
                                                pc->in_args_list);
    if(ia) {
        long expected = pc->in_seq + 1;
        long sent = atoi(getpval(rej,ROM_SEQ));
        if(sent > expected) {
#ifdef _CHECK_SEQ__
            long gap = sent - expected;
            if(gap > 2500) {
                cme_resend_breakup(con, pc, (sent - 1));
            } else {
                cme_small_resend_req(con, pc, expected, (sent -1));
            }
#else
            request_resend(con, expected, 0);
#endif
        } else if(expected > sent) {
            char reason[100];
            int r_len = sprintf(reason, "Seq too low: got: %ld expected %ld",
                                sent, expected);
            send_logout(con, reason, r_len);
            *cut_con = 1; 
            return;
        }
        con->time_wrapper =  add_timer(pc->ass, con->connection_conf->heart_beat_int,
                                       0, 1,0, con,send_heart_beat);
        pc->connection_notice(pc, 1, NULL, 0);
    } else {
        *cut_con = 1;
    }
    
}

void send_logout(con_interface * fco, char* message, int m_len)
{
    trans_t* fix_t = (trans_t*)fco->parser;
    ofp* test = get_fixy_message(fix_t, 0x35);
    set_fix_val(fix_t, test, 58, message, m_len);
    dart_send_fix_message(test, fco); 
    destroy_fixy_message(fix_t, test);
}

static void test_req_response(char* message, int m_len, con_interface * fco)
{

    char *d_off = message;
    int found = 0;
    long test_req = m_len;
    GET_OFF(d_off, test_req, "\001112=", 5, found);
    if (found) {
        d_off += 5;
        char *m_off = d_off;
        NEXT_ONE(m_off, test_req);
        if((test_req = m_off - d_off) <= m_len && m_off[0] == FIX_FIELD_DELIM) {
            trans_t* fix_t = (trans_t*)fco->parser;
            ofp* test = get_fixy_message(fix_t, 0x30);
            set_fix_val(fix_t, test, 112, d_off, test_req);
            dart_send_fix_message(test, fco); 
            destroy_fixy_message(fix_t, test);
        }
    }
}

static void reset_seq_number(char* message, int m_len, async_parse_args *pc,
        int *cut_con)
{
    char* d_off = message;
    int found = 0;
    int l_len = m_len;
    {
        GET_OFF(d_off, l_len, "\00136=", 4, found);
    }
    if(found) {
        long n_seq = atol(d_off + 4);
        long c_seq = pc->in_seq;
        long diff = (n_seq -1) - c_seq;
        if(diff > 0) {
            struct incoming_message *m1 =
                create_incoming_message(pc->in_args->name,//sender_comp,
                                        pc->in_args->name_len, /*sc_len,*/diff);
            record_inbound_message(m1);
            pc->in_seq = n_seq;
        } 
    }
}
#ifdef _CHECK_SEQ__
static void cme_reset_seq(char* message, int m_len, async_parse_args* pc,
                          int* cut_con)
{
    if(pc->phelp == 0x0) {
       frh* fhelp = calloc(1, sizeof(frh));
       fhelp->resends = create_sized_queue(8); 
       fhelp->sent_resends = create_sized_queue(8); 
       pc->phelp = fhelp;
    }
    frh* fhelp = (frh*)pc->phelp;
    if(fhelp->end_pending > 0) {
        char* d_off = message;
        int found = 0;
        int l_len = m_len;
        {
            GET_OFF(d_off, l_len, "\00136=", 4, found);
        }
        if(found) {
            long n_seq = atol(d_off + 4);
            if(n_seq >= fhelp->end_pending) {
                long diff = (fhelp->end_pending + 1) - fhelp->beg_pending;
                diff = diff == 0 ? 1 : diff;
                struct incoming_message *m1 =
                    create_incoming_message(pc->in_args->name,//sender_comp,
                                            pc->in_args->name_len, /*sc_len,*/diff);
                record_inbound_message(m1);
                pc->in_seq += diff;//(n_seq - 1);
                //send_debug_message("Setting inbound current: %ld, diff: %ld \n", pc->in_seq, diff);
                if(is_empty(fhelp->resends)) {
                    fhelp->end_pending = 0;
                    fhelp->beg_pending = 0;
                } else {
                    pop_resend(fhelp, (con_interface*)pc->ds_book, pc);
                }
            } else {
                //pc->in_seq = n_seq;
                long diff = n_seq - fhelp->beg_pending;
                if(diff > 0) {
                    struct incoming_message *m1 =
                        create_incoming_message(pc->in_args->name,//sender_comp,
                                                pc->in_args->name_len, /*sc_len,*/diff);
                    //send_debug_message("Setting inbound current: %ld, diff: %ld, %ld \n", pc->in_seq, diff, n_seq);
                    record_inbound_message(m1);
                }
                fhelp->beg_pending += diff;
                pc->in_seq += diff;// = pc->in_seq;
            } 
        }
    } else {
        reset_seq_number(message, m_len, pc, cut_con);
    }
}
#endif

static void create_gap_fill(struct async_parse_args *pc, 
        unsigned long beg_seq, long seq, con_interface* con)
{
    trans_t* fix_t = (trans_t*)con->parser;
    ofp* obj = get_fixy_message(fix_t, 0x34);
    set_num_val(fix_t, obj, 34, beg_seq);
    pthread_spin_lock(&con->send_mutex);
    time_t now;
    struct tm tmval;
    struct tm *t = &tmval;
    time(&now);
    if ((t = gmtime_r(&now, &tmval)) == 0) {
        perror("Calling gmtime_r");
    }
    set_time_tag(fix_t, obj,&tmval, 52); 
    set_time_tag(fix_t, obj,&tmval, 122); 
    if(seq == -1) {
        set_num_val(fix_t, obj, 36, con->seq);
    } else {
        set_num_val(fix_t, obj, 36, (unsigned long)seq);
    }
    set_tag_nine(fix_t, obj);                      
    set_check_sum(fix_t, obj);
    con->last_sent = now;
    databuf_t* db = get_message(obj);
    ci_no_lock_sender(con, db);
    pthread_spin_unlock(&con->send_mutex);
    record_raw_message(pc->in_args->name,
                       pc->in_args->name_len, db->rd_ptr,
                       databuf_unread(db), 0x0);
    destroy_fixy_message(fix_t, obj);
}

static void resend_message(char *start, long sLen,
        struct async_parse_args *pc, long seq)
{
    int found = 0;
    char* d_off = start;
    long len = sLen;
    {
        GET_OFF(d_off, len, "\00152=", 4, found);
    }
    if(found) {
        char* t_one = d_off + 4;
        {
            GET_OFF(d_off, len, "\00110=", 4, found);
        }
        if(found) {
            long m_len = (d_off - t_one) +1;
            char time [32];
            time_t t;
            int time_len = create_timestamp(time, 32, &t);
            struct iovec iov[6];
            char header[128];
            char time_2[128];
            int time_2_len = sprintf(time_2, "43=Y%c97=Y%c122=", FIX_FIELD_DELIM, FIX_FIELD_DELIM);
            char tail[16];
            unsigned long total_len = m_len + ((t_one - start) - 1) + time_len + time_2_len;
            int h_len = sprintf(header, "8=%s%c9=%ld", pc->in_args->fix_version,
                            FIX_FIELD_DELIM, total_len);
            iov[0].iov_base = header;
            iov[0].iov_len = h_len;
            iov[1].iov_base = start;
            iov[1].iov_len = (t_one - start);
            iov[2].iov_base = time;
            iov[2].iov_len = time_len;
            iov[3].iov_base = time_2;
            iov[3].iov_len = time_2_len;
            iov[4].iov_base = t_one;
            iov[4].iov_len = m_len;
            size_t check_sum_len = iov_calc_check_sum(iov, 5);
            int tail_len = sprintf(tail, "10=%03lu%c", check_sum_len,'\x01');
            iov[5].iov_base = tail;
            iov[5].iov_len = tail_len;
            ci_raw_iovec_sender(iov, 6, pc->in_args->target_comp,
                                  pc->in_args->tc_len, pc->ds_book, pc->sock);
        }
    }
}

void parse_resend_mess(char* output, size_t out_len,
        struct async_parse_args *pc, long beg_seq, long end)
{
    int found = 0;
    char* d_off = output;
    long len = out_len;
    long seq = beg_seq;
    con_interface *ci = 0;
    if (pc->ds_book == NULL) {
        found = get_obj_by_client_id(pc->gk, pc->in_args->target_comp,
                                     pc->in_args->tc_len, (void **) &ci);
        pc->ds_book = ci;
    } else {
        ci = (con_interface*)pc->ds_book;
        found = 1;
    }
    if(found) {
        do {
            GET_OFF(d_off, len, "\00135=", 4, found);
            if(found) {
                char* val = d_off + 4;
                seq = get_sequence(d_off, len);
                switch(val[0]) {
                    default:
                        //do nothing we will gap fill later.
                        break;
                    case 'C':
                        if(val[1] == 'G') {
                            if(beg_seq != seq) {
                                create_gap_fill(pc, beg_seq, seq, ci);
                            }
                            resend_message(d_off, len, pc, seq);
                            beg_seq = seq + 1;
                        }
                        break;
                    case 'c':
                    case '8':
                    case 'F':
                        if(beg_seq != seq) {
                            create_gap_fill(pc, beg_seq, seq, ci);
                        }
                        resend_message(d_off, len, pc, seq);
                        beg_seq = seq + 1;
                        break;
                };
            }
            d_off += 4;
            len -= 4;
        } while(found);
        if(beg_seq <= seq) {
            long end_val = end == 0 ? -1: end;
            if(end != 0) {
                end_val = end == beg_seq ? end + 1 : end;
            }
            create_gap_fill(pc, beg_seq, end_val, ci);
        }
    }
}

#ifdef _CHECK_SEQ__

static int cme_should_send_gap_fill(long new_start,long new_end, con_interface* ci,
        async_parse_args *pc)
{
    if(pc->phelp == 0x0) {
       frh* fhelp = calloc(1, sizeof(frh));
       fhelp->resends = create_sized_queue(8); 
       fhelp->sent_resends = create_sized_queue(8); 
       pc->phelp = fhelp;
    } 
    frh* fhelp = (frh*)pc->phelp;
    if(is_empty(fhelp->sent_resends)) {
        //do nothing 
    } else {
        element_t e;
        e = front(fhelp->sent_resends);
        while (e != NULL) {
            frr *init;
            init = (frr *) e->d;
            if (init != NULL && init->beg == new_start &&
                    init->end == new_end) {
                return 0;
            } else {
                e = e->next;
            }
        }
    }
    frr* f = calloc(1, sizeof(frr));
    f->beg = new_start;
    f->end = new_end;
    enqueue(fhelp->sent_resends, f);
    return 1;
}
static void cme_handle_resend_request(char* message, int m_len, struct async_parse_args *pc)
{
    char* d_off = message;
    con_interface* ci = (con_interface*)pc->ds_book;
    int found = 0;
    int l_len = m_len;
    {
        GET_OFF(d_off, l_len, "\0017=", 3, found);
    }
    if(found) {
        long start = atol(d_off + 3);
        long end = 0;
        {
            GET_OFF(d_off, l_len, "\00116=", 4, found);
            if(found) {
                end = atol(d_off + 4);
            }
        }
        if(cme_should_send_gap_fill(start, end, ci, pc)) { 
            create_gap_fill(pc, start, -1, ci);
        }
    }
}
#else
static void handle_resend_request(char* message, int m_len, struct async_parse_args *pc)
{
    char* d_off = message;
    int found = 0;
    int l_len = m_len;
    {
        GET_OFF(d_off, l_len, "\0017=", 3, found);
    }
    if(found) {
        long start = atol(d_off + 3);
        GET_OFF(d_off, l_len, "\00116=", 4, found);
        if(found) {
            long end = atol(d_off + 4);
            resend_req* rr = create_resend_request(pc->in_args->name,
                                                   pc->in_args->name_len,
                                                   start, end, find_fix_offsets);
            get_resend_request(rr);
            size_t out_len = 0;
            char* output = (char*)drr_get_resend_output(rr, &out_len);
            if(out_len > 0) {
                parse_resend_mess(output, out_len, pc, start, end);
            }
            destroy_resend_req(rr);
        }
    }
}

#endif

#ifdef _LOG_NUM__
static void cme_process_reject(char* message, int m_len, struct async_parse_args * pc, short type)
{
    con_interface* con = (con_interface*)pc->ds_book;
    trans_t* fix_t = (trans_t*)con->parser;
    dart_order_obj* rej = (dart_order_obj *) get_gk_obj(pc->gk);
    translate_fix_msg(fix_t, message, m_len, rej, type);
    int len1 = rej->positions[70].iov_len;
    int len2 = rej->positions[73].iov_len;
    if(len1 > 0 || len2 > 0) {
        pc->obj_callback(rej, pc);
    } 
}
#else

static void parse_rejected_message(char* message, int  m_len, struct async_parse_args *pc,
        char *rr_output, size_t rr_out_len)
{
    char* d_off = rr_output;
    int len = rr_out_len;
    int found = 0;
    d_off += FIX_BEG_MESSAGE_LEN;
    GET_OFF(d_off, len, "\00135=", 4, found);
    if(found) {
        d_off += 4;
        char* end = d_off;
        NEXT_ONE(end, len);
        short stype = end - d_off > 1 ?((d_off[1] << CHAR_BIT) + d_off[0]) :
                    ((0x0 << CHAR_BIT) +  d_off[0]);
        con_interface* con = (con_interface*)pc->ds_book;
        trans_t* fix_t = (trans_t*)con->parser;
        /**
         * We use the New Order special type here to translate the "order"
         * all we want is the clordid.  we will also take the text from the
         * low level reject.
         */
        dart_order_obj* rej = (dart_order_obj *) get_gk_obj(pc->gk);
        translate_fix_msg(fix_t,  rr_output, rr_out_len, 
                       rej, 0x44);
        switch(stype) {
            case 0x44:
                rej->type = 0x38;
                reset_rom_field(rej, FIX_STATUS, "8", 1);
                break;
            case 0x46:
                rej->type = 0x39;
                reset_rom_field(rej, FIX_CNL_TYPE, "1", 1);
                break;
            case 0x47:
                rej->type = 0x39;
                reset_rom_field(rej, FIX_CNL_TYPE, "2", 1);
                break;
            case 0x4341:
                rej->type = 0x39;
                reset_rom_field(rej, FIX_CNL_TYPE, "2", 1);
                break;
            case 0x69:
                rej->type = stype;
                break;
            default:
                rej->type = 0x38;
                reset_rom_field(rej, FIX_STATUS, "8", 1);
                break;
        }
        translate_fix_msg(fix_t, message, m_len, rej, stype);
        pc->obj_callback(rej, pc);
        destroy_gk_obj(pc->gk, rej);
    }
}
static void process_reject(char* message, int m_len, struct async_parse_args *pc)
{
    char *d_off = message;
    int len = m_len;
    int found = 0;
    GET_OFF(d_off, len, "\00145=", 4, found);
    if (found) {
        char *output = NULL;
        size_t out_len = 0;
        resend_req *rr;
        long beg_res = atoi(d_off + 4);
        rr = create_resend_request(pc->in_args->name, pc->in_args->name_len, beg_res, beg_res,
                find_fix_offsets);
        get_resend_request(rr);
        output = (char *) drr_get_resend_output(rr, &out_len);
        if (out_len > 0) {
            parse_rejected_message(message, m_len, pc, output, out_len);
        }
        destroy_resend_req(rr);
    }
}
#endif
static int handle_admin_message(char* message, int m_len, int* cut_con,
        struct async_parse_args* pc, short type)
{
    int ret = 1;
    switch(type) {
        case 0x41:
             handle_fix_logon((con_interface*)pc->ds_book, pc, 
                             message, m_len, cut_con);
           //logon
            break;
        case 0x32:
            //resend
#ifdef _CHECK_SEQ__
            cme_handle_resend_request(message, m_len, pc);
#else
            handle_resend_request(message, m_len, pc);
#endif
            break;
        case 0x31:
            //test_req
            test_req_response(message, m_len, (con_interface*)pc->ds_book);            
            break;
        case 0x35:
            //Logout
            *cut_con = 1;
            break;
        case 0x34:
            //seq reset
            ret = 0;
#ifdef _CHECK_SEQ__
            cme_reset_seq(message, m_len, pc, cut_con);
#else
            reset_seq_number(message, m_len, pc, cut_con);
#endif
            break;
        case 0x6A:
        case 0x33:
            //Biz and Session Rej
#ifdef _LOG_NUM__
            cme_process_reject(message, m_len, pc, 0x33);
#else
            process_reject(message, m_len, pc);
#endif
            break;
        default:
            break;
    }
    return ret;
}

#ifdef _CHECK_SEQ__
static int ok_req_resend(struct async_parse_args* pc, int seq) 
{
    if(pc->phelp == 0x0) {
        //send_debug_message("HELP WAS NULL!!!!!:%ld \n", pc->in_seq);
        return 1;
    } 
    frh* fhelp = (frh*)pc->phelp;
    if(fhelp->end_pending == 0) {
        //send_debug_message("NO PENDING END!!!!!:%ld, got: %d \n", pc->in_seq, seq);
        return 1;
    } else {
        return 0;
    }
}
static void check_seq(struct async_parse_args* pc, char* message, int len)
{
    int seq = get_sequence(message, len);
    int expected = pc->in_seq + 1;
    if(seq > expected && ((seq - expected) <= 2500) && ok_req_resend(pc, seq)) {
        //send_debug_message("about to request small reqest!!!!!:%ld,%ld \n", pc->in_seq, expected);
        cme_small_resend_req((con_interface*)pc->ds_book, pc, expected, (seq -1));
    } else if(seq > expected && ok_req_resend(pc, seq)) {
        cme_resend_breakup((con_interface*)pc->ds_book, pc, (seq -1));
    } 
    if(pc->phelp != 0x0) {
        frh* fhelp = (frh*)pc->phelp;
        if(seq <= fhelp->end_pending
                && seq > fhelp->beg_pending) {
            //send_debug_message("Incrementing BEG: %ld in_seq:%ld \n", fhelp->beg_pending, pc->in_seq);
            fhelp->beg_pending++;
            if(fhelp->beg_pending >= fhelp->end_pending) {
                fhelp->beg_pending = 0;
                fhelp->end_pending = 0;
                if(is_empty(fhelp->resends)) {
                    //do nothing
                } else {
                    pop_resend(fhelp, (con_interface*)pc->ds_book, pc);
                }
            }
        }
    }
}
#endif

static int get_fix_header_len(struct async_parse_args* pc)
{
    if(pc && pc->in_args) {
        return (pc->in_args->fv_len == 7 ? FIX_BEG_MESSAGE_LEN : 13);
    } else {
        return FIX_BEG_MESSAGE_LEN;
    }
}
/*
 * t_len > 1 ?((type[1] << CHAR_BIT) + type[0]) :
 * ((0x0 << CHAR_BIT) +  type[0]);
 * return (a << CHAR_BIT) + b;
 */
void parse_fix_message(int *cut_con, struct async_parse_args *apa)
{
    sock_recv_manager* pc = apa->con;
    long len = (pc->wr_ptr - pc->rd_ptr);
    int beg_len = get_fix_header_len(apa);
    if(len > beg_len) {
        int should_parse = 1;
        do {
            int rec_in = 1;
            char* d_off = pc->rd_ptr;
            d_off += beg_len;
            len -= beg_len;
            pc->rd_ptr += beg_len;
            int byte_len = atoi(d_off);
            long plen = len;
            NEXT_ONE(d_off, plen);
            int sn_len = (d_off - pc->rd_ptr);
            if(d_off[0] == FIX_FIELD_DELIM &&
                    (sn_len + 1+ byte_len + FIX_CHECKSUM_LEN <= len)) {
                d_off += 4;
                char* t_end = d_off;
                NEXT_ONE(t_end, plen);
                int t_len = t_end - d_off;
                unsigned short stype = 0;
                if(t_len > 1) {
                    stype = ((d_off[1] << CHAR_BIT) + d_off[0]);
                } else {
                    stype = ((0x0 << CHAR_BIT) +  d_off[0]);
                }
                pc->rd_ptr += sn_len + 1;
                switch(stype) {
                    case 0x30:
                    case 0x31:
                    case 0x32:
                    case 0x33:
                    case 0x34:
                    case 0x35:
                    case 0x41:
                    case 0x6A:
                        rec_in = handle_admin_message(pc->rd_ptr, byte_len, cut_con, apa, stype);
                        break;
                    default:
                        {
                            dart_order_obj* cur_doj = (dart_order_obj *) get_gk_obj(apa->gk);
                            con_interface* con = (con_interface*)apa->ds_book;
                            trans_t* t = (trans_t*)con->parser;
                            cur_doj->type = stype;
                            translate_fix_msg(t, pc->rd_ptr, byte_len, 
                                              cur_doj, stype);
                            //We dangerously stash the buflen here even though the buffer is empty
                            size_t buf_len = cur_doj->current->data->buflen;
                            cur_doj->current->data->buflen = byte_len;
                            apa->obj_callback(cur_doj, apa);
                            //Need to put back the 'real' buflen
                            cur_doj->current->data->buflen = buf_len;
                            destroy_gk_obj(apa->gk, cur_doj);
                        }
                }
                record_raw_message(apa->in_args->name,
                                   apa->in_args->name_len, pc->rd_ptr, (byte_len + 7), 0x0);
                if(!*cut_con && rec_in) {
#ifdef _CHECK_SEQ__
                    if(stype != 0x41) {
                        check_seq(apa, pc->rd_ptr, byte_len);
                    }
#endif
                    struct incoming_message *m1 = 
                        create_incoming_message(apa->in_args->name,
                                                apa->in_args->name_len, /*tc_len,*/1);
                    apa->in_seq++;
                    record_inbound_message(m1);
                    time_t now = 0;
                    time(&now);
                    con_interface* con = (con_interface*)apa->ds_book;
                    con->last_recv = now;
                }
                sn_len += 1 + byte_len + FIX_CHECKSUM_LEN;
                len -= sn_len;
                pc->rd_ptr += byte_len + FIX_CHECKSUM_LEN;
            } else {
                pc->rd_ptr -= beg_len;
                should_parse = 0;
            }
        } while(should_parse && len > 0);
    }
}
void send_test_req(con_interface * fco)
{
    trans_t* fix_t = (trans_t*)fco->parser;
    ofp* test = get_fixy_message(fix_t, 0x31);
    set_fix_val(fix_t, test, 112, "TEST", 4);
    dart_send_fix_message(test, fco); 
    destroy_fixy_message(fix_t, test);
}
void* send_heart_beat(void * v)
{
    time_t now = 0;
    time(&now);
    con_interface *fco = (con_interface *) v;
    if(fco->connected) {
        char buff[256];
        read(fco->time_wrapper->sock,buff, 256); 
        long diff = now - fco->last_sent;
        if(diff >= fco->connection_conf->heart_beat_int) {
            trans_t* fix_t = (trans_t*)fco->parser;
            ofp* hb = get_fixy_message(fix_t, 0x30);
            dart_send_fix_message(hb, fco); 
            destroy_fixy_message(fix_t, hb);
        }
        diff = diff >= fco->connection_conf->heart_beat_int ?
                      fco->connection_conf->heart_beat_int :
                      (fco->connection_conf->heart_beat_int - diff);
        add_back_rep_timer_adjust(fco->ass,fco->time_wrapper,diff);
        if(now - fco->last_recv > (fco->connection_conf->heart_beat_int * 2)) {
            send_test_req(fco);
        } else if(now - fco->last_recv > (fco->connection_conf->heart_beat_int *4)) {
            fco->ap->disconnect = 1;
        }
    }
    return NULL;
}
