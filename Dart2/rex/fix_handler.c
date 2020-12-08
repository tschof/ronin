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
#include "rex_constants.h"
#include "ex_config_helper.h"
#include "parser_func.h"
#include "log_mnger.h"
#include "hashmap.h"
#include "rex_seq_request.h"
#include "rex_resend_request.h"
#include "sock_recv_manager.h"
#include "tm_to_str.h"
#include "fix_parser.h"
#include "fix_parser_factory.h"


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
    if (size > 0) {
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

void send_fix_logon(void* v)
{
    fixp* pa = (fixp*)v;
    rex_con* rc = get_fs_rex_con(pa);
    sequence_req* sr = create_sequence_request(rc->logon->name,
                                 rc->logon->nlen,
                                 rc->logon,rc->conf->version,
                                 build_fix_offsets,
                                 create_fix_resend_pad, pa);
    get_sequence_numbers(sr, rc->o);
    /* Add the sequences into the connection object */
    set_in_seq(pa, dsr_get_incoming(sr));
    set_out_seq(pa, dsr_get_outgoing(sr) + 1);
    destroy_sequence_req(sr);
    ofp* test = get_message_for_type(pa, 0x41);
    fp_send_fix_message(test, pa); 
    return_fix_message(pa, test);
}

static void handle_fix_server_logon(rex_con* pc, fixp* fp,
                             char* logon, int l_len, int* cut_con)
{
    dart_order_obj* rej = trans_fix_msg(fp, logon, l_len, 0x41);
    char* user = (char*)getpval(rej, ROM_TRADER);
    int nLen = getplen(rej, ROM_TRADER); 
    pc->logon = pc->is_valid(get_fs_pass_back_pointer(fp),rej);
    if(pc->logon == 0) {
        *cut_con = 1; 
        pc->removed = 1;
        return;
    }
    if (check_is_connected(pc->p, user, nLen)) {
        *cut_con = 1;
        pc->removed = 1;
        return;
    }
    sequence_req* sr = create_sequence_request(user, nLen,
                                 pc->logon,pc->conf->version,
                                 build_fix_offsets,
                                 create_fix_resend_pad, pc);
    get_sequence_numbers(sr, pc->o);
    /* Add the sequences into the connection object */
    long send_seq = dsr_get_incoming(sr) + 1;
    long out_seq = dsr_get_outgoing(sr);
    destroy_sequence_req(sr);
    fixp* lfp = clone_fix_parser(fp);
    pc->parser = lfp;
    set_rex_con(lfp, pc);
    set_log_info(lfp, pc->logon);
    long sent = atoi(getpval(rej,ROM_SEQ));
    set_in_seq(lfp, send_seq);
    set_out_seq(lfp, out_seq + 1);
    if(send_seq > sent) {
        char reason[100];
        int r_len = sprintf(reason, "Seq too low: got: %ld expected %ld",
                            sent, send_seq);
        send_logout(lfp, reason, r_len);
        *cut_con = 1; 
        clean_fix_parser(lfp);
        return;
    }
    ofp* slog = get_message_for_type(lfp, 0x41);
    fp_send_fix_message(slog, lfp); 
    return_fix_message(lfp, slog);
    start_hb_timer(lfp);
    pc->connection_notice(lfp, 1, rej);
    if(sent > send_seq) {
        request_resend(lfp, send_seq, 0);
    }
}


static void handle_fix_logon(rex_con* pc, fixp* fp,
                             char* logon, int l_len, int* cut_con)
{
    dart_order_obj* rej = trans_fix_msg(fp, logon, l_len, 0x41);
    int allowed = validate_sender_comp(fp, rej);
    if(pc->logon && allowed) {
        long expected = get_in_seq(fp) + 1;
        long sent = atoi(getpval(rej,ROM_SEQ));
        if(sent > expected) {
            request_resend(fp, expected, 0);
        } else if(expected > sent) {
            char reason[100];
            int r_len = sprintf(reason, "Seq too low: got: %ld expected %ld",
                                sent, expected);
            send_logout(fp, reason, r_len);
            *cut_con = 1; 
            return;
        }
        pc->connection_notice(fp, 1, rej);
        start_hb_timer(fp);
    } else {
        *cut_con = 1;
    }
    return_dart_obj(fp, rej); 
}

static void test_req_response(char* message, int m_len, fixp * fco)
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
            respond_to_tr(fco, d_off, test_req);
        }
    }
}

static void reset_seq_number(char* message, int m_len, rex_con *pc,
        int *cut_con)
{
    char* d_off = message;
    int found = 0;
    int l_len = m_len;
    {
        GET_OFF(d_off, l_len, "\00136=", 4, found);
    }
    if(found) {
        fixp* fp = (fixp*)pc->parser;
        long n_seq = atol(d_off + 4);
        long c_seq = get_in_seq(fp);//pc->in_seq;
        long diff = n_seq - c_seq;
        if(diff > 0) {
            record_inbound_message(pc->o, diff, pc->logon->name, 
                                   pc->logon->nlen);
            set_in_seq(fp, n_seq);
        } 
    }
}

static void resend_message(char *start, long sLen,
        rex_con *pc, long seq)
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
            int h_len = sprintf(header, "8=%s%c9=%ld", pc->conf->version,
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
            fixp* fp = (fixp*)pc->parser;
            send_iovec(fp, iov, 6);
        }
    }
}

static void parse_resend_mess(char* output, size_t out_len,
        rex_con *pc, long beg_seq, long end)
{
    int found = 0;
    char* d_off = output;
    long len = out_len;
    long seq = beg_seq;
    fixp *ci = (fixp*)pc->parser;
    if(pc->parser != 0) {
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
                                send_gap_fill(ci, beg_seq, seq);
                            }
                            resend_message(d_off, len, pc, seq);
                            beg_seq = seq + 1;
                        }
                        break;
                    case 'c':
                    case '8':
                    case 'F':
                        if(beg_seq != seq) {
                            send_gap_fill(ci, beg_seq, seq);
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
            end_val = end == beg_seq ? end + 1 : end_val;
            send_gap_fill(ci, beg_seq, end_val);
        }
    }
}

static void handle_resend_request(char* message, int m_len, rex_con *pc)
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
            resend_req* rr = create_resend_request(pc->logon->name,
                                                   pc->logon->nlen,
                                                   start, end, find_fix_offsets);
            get_resend_request(rr, pc->o);
            size_t out_len = 0;
            char* output = (char*)drr_get_resend_output(rr, &out_len);
            if(out_len > 0) {
                parse_resend_mess(output, out_len, pc, start, end);
            }
            destroy_resend_req(rr);
        }
    }
}

static void parse_rejected_message(char* message, int  m_len, rex_con *pc,
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
        fixp* fp = (fixp*)pc->parser;
        /**
         * We use the New Order special type here to translate the "order"
         * all we want is the clordid.  we will also take the text from the
         * low level reject.
         */
        dart_order_obj* rej = trans_fix_msg(fp, rr_output, rr_out_len,0x44);
        switch(stype) {
            case 0x44:
                rej->type = 0x3;
                reset_rom_field(rej, FIX_STATUS, "8", 1);
                reset_rom_field(rej, ROM_STATUS, "8", 1);
                break;
            case 0x46:
                rej->type = 0x3;
                reset_rom_field(rej, FIX_CNL_TYPE, "1", 1);
                reset_rom_field(rej, ROM_STATUS, "14", 2);
                break;
            case 0x47:
                rej->type = 0x3;
                reset_rom_field(rej, FIX_CNL_TYPE, "2", 1);
                reset_rom_field(rej, ROM_STATUS, "20", 2);
                break;
            case 0x69:
                rej->type = stype;
                break;
        }
        no_create_trans(fp, message, m_len, rej, stype);

        //We dangerously stash the buflen here even though the buffer is empty
        size_t buf_len = rej->current->data->buflen;
        rej->current->data->buflen = m_len;
        rej->ex_mess = m_len;
        fp_callback(fp, rej);
        rej->current->data->buflen = buf_len;
        return_dart_obj(fp, rej);
    }
}

static void process_reject(char* message, int m_len, rex_con *pc)
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
        rr = create_resend_request(pc->logon->name, pc->logon->nlen, beg_res, beg_res,
                find_fix_offsets);
        get_resend_request(rr, pc->o);
        output = (char *) drr_get_resend_output(rr, &out_len);
        if (out_len > 0) {
            parse_rejected_message(message, m_len, pc, output, out_len);
        }
        destroy_resend_req(rr);
    }
}
static int handle_admin_message(char* message, int m_len, int* cut_con,
        rex_con* pc, short type)
{
    int ret = 1;
    switch(type) {
        case 0x41:
            if(pc->is_server) {
                handle_fix_server_logon(pc, (fixp*)pc->parser, message, m_len, cut_con);
            } else {
             handle_fix_logon(pc, (fixp*)pc->parser,
                             message, m_len, cut_con);
            }
           //logon
            break;
        case 0x32:
            //resend
            handle_resend_request(message, m_len, pc);
            break;
        case 0x31:
            //test_req
            test_req_response(message, m_len, (fixp*)pc->parser);            
            break;
        case 0x35:
            //Logout
            *cut_con = 1;
            pc->disconnect = 1;
            if(pc->is_server) {
                clean_fix_parser((fixp*)pc->parser);
            }
            break;
        case 0x34:
            //seq reset
            ret = 0;
            reset_seq_number(message, m_len, pc, cut_con);
            break;
        case 0x6A:
        case 0x33:
            //Biz and Session Rej
            process_reject(message, m_len, pc);
            break;
        default:
            break;
    }
    return ret;
}
/*
 * t_len > 1 ?((type[1] << CHAR_BIT) + type[0]) :
 * ((0x0 << CHAR_BIT) +  type[0]);
 * return (a << CHAR_BIT) + b;
 */
void parse_fix_message(int *cut_con, rex_con *apa)
{
    sock_mnger* pc = apa->con;
    long len = (pc->wr_ptr - pc->rd_ptr);
    long debug_len = (pc->wr_ptr - pc->rd_ptr);
    long used = 0;
    fixp* fp = (fixp*)apa->parser;
    int hlen = get_fp_header_len(fp);
    char* r_off = pc->rd_ptr;
    if(len > (hlen + 6)) {
        int should_parse = 1;
        do {
            int rec_in = 1;
            len -= hlen; 
            r_off += hlen;
            int byte_len = atoi(r_off);
            char* d_off = memchr(r_off, FIX_FIELD_DELIM, len);
            if(d_off) {
                int sn_len = (d_off - r_off) + 1;
                long bod_len = (sn_len + byte_len + FIX_CHECKSUM_LEN);
                if(bod_len <= len) {
                    d_off += 4;
                    char* t_end = memchr(d_off, FIX_FIELD_DELIM, (len - 4));
                    int t_len = t_end - d_off;
                    unsigned short stype = 0;
                    if(t_len > 1) {
                        stype = ((d_off[1] << CHAR_BIT) + d_off[0]);
                    } else {
                        stype = ((0x0 << CHAR_BIT) +  d_off[0]);
                    }
                    r_off += sn_len;
                    len -= sn_len;
                    used += (hlen +bod_len);
                    switch(stype) {
                        case 0x30:
                        case 0x31:
                        case 0x32:
                        case 0x33:
                        case 0x34:
                        case 0x35:
                        case 0x41:
                        case 0x6A:
                            rec_in = handle_admin_message(r_off, byte_len, cut_con, apa, stype);
                            break;
                        default:
                            {
                                dart_order_obj* doj = trans_fix_msg(fp, r_off, byte_len, 
                                                                    stype);
                                doj->type = stype;
                                //We dangerously stash the buflen here even though the buffer is empty
                                size_t buf_len = doj->current->data->buflen;
                                doj->current->data->buflen = byte_len;
                                doj->ex_mess = byte_len;
                                fp_callback(fp, doj);
                                //Need to put back the 'real' buflen
                                doj->current->data->buflen = buf_len;
                                return_dart_obj(fp, doj);
                            }
                    }
                    if(apa->logon) {
                        record_raw_message(apa->o, apa->logon->name,
                                       apa->logon->nlen, r_off, (byte_len + 7), 0x0);
                    } else {
                        send_debug_message(apa->o, "Logon faild on: %d \n", apa->sock);
                    }
                    if(!*cut_con && rec_in) {
                        record_inbound_message(apa->o, 1, apa->logon->name,  apa->logon->nlen);
                        inc_in_seq_n_set_recv(fp);
                    }
                    sn_len = byte_len + FIX_CHECKSUM_LEN;
                    len -= sn_len;
                    r_off += sn_len;
                } else {
                    r_off -= hlen;
                    should_parse = 0;
                }
            } else {
                should_parse = 0;
            }
        } while(should_parse && (len > (hlen + 6)) && used < debug_len);
    }
    pc->rd_ptr += used;
}
