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
#include "fix_admin_master.h"
#include "dart_constants.h"
#include "client_manager.h"
#include "parser_func.h"
#include "transaction_recorder.h"
#include "hashmap.h"
#include "dart_seq_request.h"
#include "tm_to_str.h"
#include "fixmsg.h"
#include "debug_logger.h"

#define DEF_FIX_SIZE 1400
#define HEADER_TAIL_LEN 200

static pthread_mutex_t init_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t seq_mutex = PTHREAD_MUTEX_INITIALIZER;
static hashmap_t con_ptr;
static fix_con_obj *init_con_obj = NULL;
static int is_init = 0;

static void init_map()
{
    if (pthread_mutex_lock(&init_mutex) == 0) {
        if (is_init) {
            //do nothing
        } else {
            is_init = 1;
            con_ptr = create_map(20);
            init_con_obj =
                create_fix_con_obj("DODO", 4, "DODO", 4, 0, 0, 0, 0, 0, 0,
                                   0, 0, 1, 0, "FIX.4.2", 7, "DODO", 4,
                                   NULL, create_fix_header);
        }
    }
    if (pthread_mutex_unlock(&init_mutex) != 0) {
        send_debug_message("Failed to unlock mutex in init_map.\n");
    }
}

static int calc_check_sum(const char *begin, int len)
{
    int sum = 0;
    int i;
    const char *c;
    c = begin;
    for (i = 0; i < len; ++i) {
        sum = (sum + *c);
        ++c;
    }
    sum = sum % 256;
    return sum;
}


static int add_fix_con(fix_con_obj * co)
{
    fix_con_obj *temp = init_con_obj;
    int res;
    size_t tcid_len = 0;
    const char *tcid = dfc_get_target_comp_id(co, &tcid_len);
    if (pthread_mutex_lock(&seq_mutex) == 0) {
        temp = NULL;
        res = get_map_entry(con_ptr, tcid, tcid_len, (void *) &temp);
        if (res > 0) {
            send_debug_message("Found an FCO with this name: %s\n", tcid);
            kill_connection(temp);
            send_debug_message("Memory Leak doh, %s \n", tcid);
        } else {
            no_copy_insert(con_ptr, tcid, tcid_len, co);
        }
    }
    if (pthread_mutex_unlock(&seq_mutex) != 0) {
        send_debug_message("Failed to ulock mutex add_fix_con.\n");
    }
    return 0;
}

static fix_con_obj *get_fix_con_obj(const char *name, int name_len)
{
    fix_con_obj *ret = init_con_obj;
    int res;
    if (pthread_mutex_lock(&seq_mutex) == 0) {
        res = get_map_entry(con_ptr, name, name_len, (void *) &ret);
        if (res <= 0) {
            ret = NULL;
        }
    }
    if (pthread_mutex_unlock(&seq_mutex) != 0) {
        send_debug_message
            ("Failled to unlock mutex in get_fix_con_obj.\n");
    }
    return ret;
}

int remove_fix_con_obj(fix_con_obj * fco)
{
    int res;
    res = pthread_mutex_lock(&seq_mutex);
    if (res == 0) {
        size_t tc_id_len = 0;
        const char *tc_id = dfc_get_target_comp_id(fco, &tc_id_len);
        res = remove_from_map(con_ptr, tc_id, tc_id_len);
    }
    if (pthread_mutex_unlock(&seq_mutex) != 0) {
        send_debug_message
            ("Failed to unlock mutex in remove_fix_con_obj.\n");
    }
    return res;
}

/**
 * This function will find the first instance of FIX_FIELD_VALUE_DELIM
 * and then find the next instance of a FIX_FIELD_DELIM and then
 * calloc a new string with the value.
 */
static char *copy_tag_value(char *mblk, size_t len, size_t * tag_len)
{
    char *ret_value;
    char *d_off;
    d_off = mblk;
    ret_value = strchr(d_off, FIX_FIELD_VALUE_DELIM);
    if (ret_value) {
        d_off += (ret_value - d_off) + 1;
        ret_value = strchr(d_off, FIX_FIELD_DELIM);
        if (ret_value) {
            char *val = NULL;
            int l_len = ret_value - d_off;
            if (l_len > 0) {
                val = calloc(l_len + 1, 1);
            }
            if (l_len < 0 || val == NULL) {
                return NULL;
            }
            *tag_len = l_len;
            memcpy(val, d_off, *tag_len);
            return val;
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

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

size_t create_fix_header(databuf_t * fixmsg,
                         const char *sendercompid,
                         size_t sendercompid_len,
                         const char *targetcompid,
                         size_t targetcompid_len,
                         const char *sendersubid,
                         size_t sendersubid_len,
                         const char *targetsubid,
                         size_t targetsubid_len,
                         unsigned long seq, time_t * now)
{
    size_t retval = 0;
    struct tm tmval;
    struct tm *t = &tmval;
    time(now);
    if ((t = gmtime_r(now, &tmval)) == 0) {
        perror("Calling gmtime_r");
    }
    if (fixmsg_addstr2(fixmsg, "49", 2, sendercompid, sendercompid_len) >
        -1
        && fixmsg_addstr2(fixmsg, "56", 2, targetcompid,
                          targetcompid_len) > -1
        && fixmsg_addstr2(fixmsg, "50", 2, sendersubid,
                          sendersubid_len) > -1
        && fixmsg_addstr2(fixmsg, "57", 2, targetsubid,
                          targetsubid_len) > -1
        && fixmsg_adduint(fixmsg, "34", 2, seq) > -1
        && fixmsg_addtime(fixmsg, "52", t) > -1) {
        if (databuf_unread(fixmsg) > 0 || databuf_realloc(fixmsg, 1) == 0) {
            *fixmsg->wr_ptr++ = '\x01';
            retval = databuf_unread(fixmsg);
        }
    }
    return retval;
}

static void record_sent_message2(const struct iovec *iov, int iovcnt,
                                 const char *target_comp_id, int tc_len,
                                 size_t total_len, fix_con_obj * fco)
{
    struct message *temp;
    temp = malloc(sizeof(struct message));
    if (temp != NULL) {
        int i = 0;
        char *d_off = NULL;
        char *name = calloc(tc_len + 1, 1);
        temp->data = calloc(total_len + 1, 1);
        d_off = temp->data;
        memcpy(name, target_comp_id, tc_len);
        temp->name = name;
        temp->name_len = tc_len;
        for (; i < iovcnt; ++i) {
            memcpy(d_off, iov[i].iov_base, iov[i].iov_len);
            d_off += iov[i].iov_len;
        }
        temp->len = total_len;
        size_t mod_len = 0;
        const char *mod_name = dfc_get_mod_name(fco, &mod_len);
        if (mod_len > 0) {
            struct message m1;
            m1.name = (char *) target_comp_id;
            m1.name_len = tc_len;
            m1.data = temp->data;
            m1.len = total_len;
            dart_rolling_log_going(&m1, mod_name, mod_len);
        }
        record_outbound_message(temp);
    }
}

static int lin_send_func(struct iovec *iov, int iovcnt,
                         fix_con_obj * fco, size_t total_len)
{
    ssize_t ret_code = 0;
    size_t bytes_written = 0;
    int sock = dfc_get_socket(fco);
    int reset = 0;
    if (sock >= 0) {
        while (ret_code >= 0 && bytes_written < total_len) {
            ret_code = writev(sock, iov, iovcnt);
            bytes_written = (ret_code >= 0)
                ? bytes_written + ret_code : bytes_written;
            if (bytes_written < total_len && ret_code >= 0) {
                reset = 1;
                int move_ptr = 1;
                size_t acc_len = 0;
                while (move_ptr) {
                    acc_len += iov->iov_len;
                    if (acc_len > ret_code) {
                        size_t snt = (ret_code - (acc_len - iov->iov_len));
                        size_t off = iov->iov_len - snt;
                        iov->iov_len = off;
                        iov->iov_base = (char *) iov->iov_base + snt;
                        move_ptr = 0;
                    } else {
                        ++iov;
                        --iovcnt;
                    }
                }
            } else if (ret_code < 0) {
                int error = 0;
                error = errno;
                if (error == EAGAIN || error == EWOULDBLOCK
                    || error == ENOBUFS || error == EINTR || error == 0) {
                    ret_code = 0;
                } else {
                    con_obj *co = dfc_get_con_obj(fco);
                    send_debug_message("Got this error on send: %d \n",
                                       error);
                    ret_code = -1;
                    set_disconnect(co, 1);
                }
            }
        }
    }
    return reset;
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

static int dart_write_v(char *data, size_t datalen, fix_con_obj * fco,
                        char *type, size_t typelen)
{
    char header[HEADER_TAIL_LEN];
    char user_header[HEADER_TAIL_LEN];
    char tail[HEADER_TAIL_LEN];

    int ret_code = -1;
    size_t sc_len = 0;
    size_t tc_len = 0;
    size_t ss_len = 0;
    size_t ts_len = 0;
    size_t fv_len = 0;
    time_t now;

    const char *target_comp_id = dfc_get_target_comp_id(fco, &tc_len);
    struct iovec iov[4];
    int reset = 0;


    dfc_lock_seq_blocker(fco);

    databuf_t usrhdrbuf;
    if (databuf_attach(&usrhdrbuf, user_header, sizeof user_header) == 0) {
        const char *sendercompid = dfc_get_sender_comp_id(fco, &sc_len);
        const char *sendersubid = dfc_get_sendersubid(fco, &ss_len);
        const char *targetsubid = dfc_get_targetsubid(fco, &ts_len);
        size_t outgoing_seq = dfc_get_outgoing_seq(fco);
        if (fixmsg_addstr2(&usrhdrbuf, "35", 2, type, typelen) < 0
            || dfc_create_fix_header(fco, &usrhdrbuf,
                                     sendercompid, sc_len,
                                     target_comp_id, tc_len,
                                     sendersubid, ss_len,
                                     targetsubid, ts_len,
                                     outgoing_seq, &now) == 0) {
            ret_code = -1;
            goto on_error;
        }
    }

    databuf_t hdrbuf;
    const char *fixver = dfc_get_fix_version(fco, &fv_len);
    if (databuf_attach(&hdrbuf, header, sizeof header) < 0
        || databuf_write(&hdrbuf, "%s=%s", "8", fixver) < 0
        || fixmsg_adduint(&hdrbuf, "9", 1,
                          databuf_unread(&usrhdrbuf) + datalen - 1) < 0) {
        ret_code = -1;
        goto on_error;
    }

    dfc_set_last_mess_sec(fco, now);

    iov[0].iov_base = header;
    iov[0].iov_len = databuf_unread(&hdrbuf);
    iov[1].iov_base = user_header;
    iov[1].iov_len = databuf_unread(&usrhdrbuf);
    iov[2].iov_base = data;
    iov[2].iov_len = datalen;

    size_t check_sum_len = iov_calc_check_sum(iov, 3);
    databuf_t tailbuf;
    if (databuf_attach(&tailbuf, tail, sizeof tail) < 0
        || databuf_write(&tailbuf, "10=%03lu%c", check_sum_len,
                         '\x01') < 0) {
        ret_code = -1;
        goto on_error;
    }

    iov[3].iov_base = tail;
    iov[3].iov_len = databuf_unread(&tailbuf);

    size_t totallen = databuf_unread(&hdrbuf)
        + databuf_unread(&usrhdrbuf)
        + datalen + databuf_unread(&tailbuf);
    reset = lin_send_func(iov, 4, fco, totallen);
    dfc_increment_outgoing_seq(fco);
    if (dfc_get_log_outgoing(fco)) {
        iov[0].iov_base = header;
        iov[0].iov_len = databuf_unread(&hdrbuf);
        iov[1].iov_base = user_header;
        iov[1].iov_len = databuf_unread(&usrhdrbuf);
        iov[2].iov_base = data;
        iov[2].iov_len = datalen;
        iov[3].iov_base = tail;
        iov[3].iov_len = databuf_unread(&tailbuf);
        record_sent_message2(iov, 4, target_comp_id, tc_len, totallen,
                             fco);
    }

  on_error:
    dfc_unlock_seq_blocker(fco);
    return ret_code;
}

static int fix_internal_send(char *data, int len, fix_con_obj * fco)
{
    int ret_code = 0;
    int len_sent = 0;
    long bytes_to_send = len;
    char *s_off = data;
    int failed_send = 0;
    int sock = dfc_get_socket(fco);
    if (sock >= 0) {
        while (len_sent < len) {
            ret_code = dart_send_message(dfc_get_con_obj(fco),
                                         s_off, bytes_to_send);
            if (ret_code >= 0) {
                len_sent += ret_code;
            } else {
                ++failed_send;
                len_sent += 0;
                if (failed_send >= 1) {
                    send_debug_message
                        ("Giving up sending fix message: "
                         "total_bytes: %d bytes_sent %d \n",
                         len, len_sent);
                    return ret_code;
                }
                ret_code = 0;
            }
            if (len_sent < len) {
                bytes_to_send -= ret_code;
                s_off += ret_code;
            }
        }
    }
    return ret_code;
}

static int polish_n_send(char *data, size_t d_len, fix_con_obj * fco,
                         char *type, size_t type_len)
{
    return dart_write_v(data, d_len, fco, type, type_len);
}

static long get_sequence(char *mblk, size_t len)
{
    char *ret_code;
    char *d_off;
    char sq[5] =
        { FIX_FIELD_DELIM, '3', '4', FIX_FIELD_VALUE_DELIM, '\0' };
    d_off = mblk;
    ret_code = strstr(d_off, sq);
    if (ret_code) {
        unsigned long res;
        d_off += (ret_code - d_off) + 4;
        res = 0;
        res = atol(d_off);
        return res;
    } else {
        return 0;
    }
}

unsigned long build_fix_offsets(char *data, unsigned long size,
                                unsigned long byte_offset,
                                unsigned long *seq_num)
{
    if (size > 0) {
        char eol[5] = { FIX_FIELD_DELIM, '1', '0',
            FIX_FIELD_VALUE_DELIM, '\0'
        };
        char *d_off;
        unsigned long read_bytes;
        long ret_code;
        unsigned long l_seq;
        read_bytes = 0;
        ret_code = 0;
        l_seq = *seq_num;
        d_off = data;
        ret_code = find_offset(d_off, size, eol, 5);
        while (ret_code >= 0) {
            if ((read_bytes + ret_code + FIX_EOM_OFFSET) > size) {
                *seq_num = l_seq;
                return read_bytes;
            }
            read_bytes += (unsigned long) ret_code;
            read_bytes += FIX_EOM_OFFSET;
            d_off = d_off + ret_code + FIX_EOM_OFFSET;
            ++l_seq;
            ret_code = find_offset(d_off, (size - read_bytes), eol, 5);
        }
        *seq_num = l_seq;
        return read_bytes;
    }
    return size;
}

size_t find_fix_offsets(char *data, size_t * seq_off, size_t size,
                        size_t byte_offset,
                        size_t beg_seq, size_t end_seq, size_t * beg_off,
                        size_t * end_off)
{
    size_t read_bytes = 0;
    if (size > FIX_BEG_MESSAGE_LEN) {
        int byte_len = 0;
        size_t mess_len = 0;
        size_t seq = *seq_off;
        seq = seq == 0 ? 1 : seq;
        char *d_off = data;
        char *ret_code = NULL;
        int should_parse = 1;
        size_t len_left = size;
        size_t real_off = byte_offset;
        while (should_parse && (len_left > FIX_BEG_MESSAGE_LEN)) {
            if (seq == beg_seq) {
                *beg_off = real_off;
            }
            d_off += FIX_BEG_MESSAGE_LEN;
            read_bytes += FIX_BEG_MESSAGE_LEN;
            len_left -= FIX_BEG_MESSAGE_LEN;
            real_off += FIX_BEG_MESSAGE_LEN;
            if (read_bytes < size) {
                byte_len = atoi(d_off);
                ret_code = strchr(d_off, FIX_FIELD_DELIM);
                if (ret_code) {
                    mess_len = (ret_code - d_off) + 1 + byte_len +
                        FIX_CHECKSUM_LEN;
                    if (mess_len <= len_left) {
                        d_off += mess_len;
                        read_bytes += mess_len;
                        len_left -= mess_len;
                        real_off += mess_len;
                        if (seq == end_seq) {
                            *end_off = real_off;
                        } else if (end_seq == 0 && seq >= beg_seq) {
                            should_parse = 0;
                        }
                        ++seq;
                    } else {
                        should_parse = 0;
                        read_bytes -= FIX_BEG_MESSAGE_LEN;
                    }
                } else {
                    should_parse = 0;
                    read_bytes -= FIX_BEG_MESSAGE_LEN;
                }
            } else {
                should_parse = 0;
                read_bytes -= FIX_BEG_MESSAGE_LEN;
            }
        }
        *seq_off = seq;
    }
    return read_bytes;
}

static void request_resend2(char *name, size_t n_len,
                            unsigned long start, unsigned long end)
{
    char *builder;
    char *d_off;
    size_t total_len;
    fix_con_obj *fco;
    size_t len;
    total_len = 0;
    builder = calloc(50, 1);
    if (builder == NULL) {
        send_debug_message("Could not malloc builder!!!!\n");
        return;
    }
    d_off = builder;
    fco = NULL;
    fco = get_fix_con_obj(name, n_len);
    if (fco != NULL) {
        len =
            sprintf(d_off, "7=%lu%c16=%lu%c", start, FIX_FIELD_DELIM, end,
                    FIX_FIELD_DELIM);
        if (len > 0) {
            total_len += len;
            d_off += len;
        }
        polish_n_send(builder, total_len, fco, "2", 1);
        free(builder);

    } else {
        send_debug_message
            ("Could not find fix connection for: "
             "%s while trying to request a resend.\n", name);
    }

}

static void request_resend(char *name, size_t n_len,
                           unsigned long request_start)
{
    char *builder;
    char *d_off;
    size_t total_len;
    fix_con_obj *fco;
    size_t len;
    total_len = 0;
    builder = calloc(50, 1);
    if (builder == NULL) {
        send_debug_message("Could not malloc builder\n");
        return;
    }
    d_off = builder;
    fco = NULL;
    fco = get_fix_con_obj(name, n_len);
    if (fco != NULL) {
        len =
            sprintf(d_off, "7=%lu%c16=0%c", request_start, FIX_FIELD_DELIM,
                    FIX_FIELD_DELIM);
        if (len > 0) {
            total_len += len;
            d_off += len;
        }
        polish_n_send(builder, total_len, fco, "2", 1);
        free(builder);

    } else {
        send_debug_message
            ("Could not find fix connection for: "
             "%s while trying to request a resend.\n", name);
    }
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

            local_len = sprintf(d_off, "8=%s%c9=%d%c",  //"8=FIX.4.2%c9=%d%c",
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

fix_con_obj *build_fix_con_obj(struct parse_args * pa)
{
    sequence_req *sr;
    fix_con_obj *fco;
    char *name = (char *) get_name(pa->con->co);
    init_map();
    sr = create_sequence_request(name,
                                 get_name_len(pa->con->co),
                                 pa->con->in_args,
                                 build_fix_offsets, create_fix_resend_pad);
    if (sr == NULL) {
        return NULL;
    }
    get_sequence_numbers(sr);
    set_incoming_seq(pa->con->co, dsr_get_incoming(sr));
    set_outgoing_seq(pa->con->co, dsr_get_outgoing(sr) + 1);
    destroy_sequence_req(sr);
    pa->con->in_args->reset_time->new_incoming = 0;
    pa->con->in_args->reset_time->new_outgoing = 0;

    fco = get_fix_con_obj(name, get_name_len(pa->con->co));
    if (fco == NULL) {
        fco =
            create_fix_con_obj(pa->con->in_args->sender_comp,
                               pa->con->in_args->sc_len,
                               name,
                               get_name_len(pa->con->co),
                               pa->con->in_args->sender_sub,
                               pa->con->in_args->ss_len,
                               pa->con->in_args->target_sub,
                               pa->con->in_args->ts_len,
                               get_incoming_seq(pa->con->co),
                               get_outgoing_seq(pa->con->co), pa->t,
                               0, pa->con->in_args->log_outgoing,
                               pa->con->timeout_val,
                               pa->con->in_args->fix_version,
                               pa->con->in_args->fv_len, pa->con->mod_name,
                               pa->con->mod_len, pa->con->co,
                               pa->con->in_args->create_mess_header);
        if (fco == NULL) {
            return NULL;
        }
        add_fix_con(fco);
    } else {
        kill_connection(fco);
        dfc_set_con_obj(fco, pa->con->co);
        dfc_set_mod_name(fco, pa->con->mod_name, pa->con->mod_len);
        dfc_set_incoming_seq(fco, get_incoming_seq(pa->con->co));
        dfc_set_outgoing_seq(fco, get_outgoing_seq(pa->con->co));
        dfc_set_tickle(fco, 0);
        dfc_set_disconnect(fco, 0);
    }
    if (name) {
        free(name);
    }
    return fco;
}

static fix_con_obj *con_build_fix_con_obj(struct parse_con *con,
                                          app_end_time * t)
{
    sequence_req *sr;
    fix_con_obj *fco;
    char *name = (char *) get_name(con->co);
    init_map();
    sr = create_sequence_request(name,
                                 get_name_len(con->co),
                                 con->in_args,
                                 build_fix_offsets, create_fix_resend_pad);
    if (sr == NULL) {
        return NULL;
    }
    get_sequence_numbers(sr);
    set_incoming_seq(con->co, dsr_get_incoming(sr));
    set_outgoing_seq(con->co, dsr_get_outgoing(sr) + 1);
    destroy_sequence_req(sr);
    con->in_args->reset_time->new_incoming = 0;
    con->in_args->reset_time->new_outgoing = 0;

    fco = get_fix_con_obj(name, get_name_len(con->co));
    if (fco == NULL) {
        fco =
            create_fix_con_obj(con->in_args->sender_comp,
                               con->in_args->sc_len,
                               name,
                               get_name_len(con->co),
                               con->in_args->sender_sub,
                               con->in_args->ss_len,
                               con->in_args->target_sub,
                               con->in_args->ts_len,
                               get_incoming_seq(con->co),
                               get_outgoing_seq(con->co), t,
                               0, con->in_args->log_outgoing,
                               con->timeout_val, con->in_args->fix_version,
                               con->in_args->fv_len, con->mod_name,
                               con->mod_len, con->co,
                               con->in_args->create_mess_header);
        if (fco == NULL) {
            return NULL;
        }
        add_fix_con(fco);
    } else {
        kill_connection(fco);
        dfc_set_con_obj(fco, con->co);
        dfc_set_mod_name(fco, con->mod_name, con->mod_len);
        dfc_set_incoming_seq(fco, get_incoming_seq(con->co));
        dfc_set_outgoing_seq(fco, get_outgoing_seq(con->co));
        dfc_set_tickle(fco, 0);
        dfc_set_disconnect(fco, 0);
    }
    if (name) {
        free(name);
    }
    return fco;
}

int send_con_logon(struct parse_con *con, app_end_time * t, char *tag_list,
                   int len)
{
    fix_con_obj *fco;
    fco = con_build_fix_con_obj(con, t);
    if (fco != NULL) {
        con->init_con(con->co);
        if (len > 0) {
            polish_n_send(tag_list, len, fco, "A", 1);
        }
        return 0;
    }
    return -1;
}

int send_gen_logon(struct parse_args *pa, char *tag_list, int len)
{
    fix_con_obj *fco;
    fco = build_fix_con_obj(pa);
    if (fco != NULL) {
        pa->con->init_con(pa->con->co);
        if (len > 0) {
            polish_n_send(tag_list, len, fco, "A", 1);
        }
        return 0;
    }
    return -1;
}

int send_logon(struct parse_args *pa)
{
    fix_con_obj *fco;
    char *mblk;
    size_t len;
    fco = build_fix_con_obj(pa);
    mblk = calloc(DEF_FIX_SIZE, 1);
    if (fco != NULL && mblk != NULL) {
        len =
            sprintf(mblk, "98=0%c108=30%c", FIX_FIELD_DELIM,
                    FIX_FIELD_DELIM);
        pa->con->init_con(pa->con->co);
        if (len > 0) {
            polish_n_send(mblk, len, fco, "A", 1);
        }
        free(mblk);
        return 0;
    }
    return -1;
}

static struct init_args *validate_target_comp(char *name, size_t name_len,
                                              queue_t in_args)
{
    if (name != NULL && name_len > 0 && in_args != NULL) {
        element_t e;
        e = front(in_args);     //dequeue(in_args);
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

static int handle_fix_logon(struct message *m, int *cut_con,
                            struct parse_con *pc,
                            struct incoming_message *m1)
{
    char *d_off;
    long ret_code;
    int should_record = 1;
    char fix_send[5] =
        { FIX_FIELD_DELIM, '4', '9', FIX_FIELD_VALUE_DELIM, '\0' };
    ret_code = 0;
    d_off = m->data;
    ret_code = find_offset(d_off, m->len, fix_send, 5);
    if (ret_code >= 0) {
        char *name;
        size_t n_len;
        d_off += ret_code;
        n_len = 0;
        name = copy_tag_value(d_off, (m->len - ret_code), &n_len);
        pc->in_args = validate_target_comp(name, n_len, pc->in_args_list);
        if (pc->in_args != NULL) {
            unsigned long other_seq;
            unsigned long our_seq = 0;
            fix_con_obj *fco = get_fix_con_obj(name, n_len);
            init_map();
            m->name = name;
            m->name_len = n_len;
            set_incoming_message_name(m1, name, n_len);
            other_seq = get_sequence(m->data, m->len);
            set_name(pc->co, name, n_len);
            /* Check if we need to send a logon response */
            if (pc->is_server) {
                int create_new = 0;
                sequence_req *sr;
                size_t len;
                char *mblk;
                create_new =
                    fco ==
                    NULL ? (pc->in_args->reset_time->create_new) : 0;
                pc->in_args->reset_time->create_new = create_new;
                sr = create_sequence_request(name, n_len,
                                             pc->in_args,
                                             build_fix_offsets,
                                             create_fix_resend_pad);
                //If we changed to clean up we now need to reset it 
                //for a possible relogon.
                pc->in_args->reset_time->create_new = 0;
                if (sr == NULL) {
                    send_debug_message("Unable to allocate sr.\n");
                    return 0;
                }
                get_sequence_numbers(sr);
                /* Since we have already had a logon today we no
                 * longer want to reset the sequences. Add the
                 * sequences into the connection object. */
                set_incoming_seq(pc->co, dsr_get_incoming(sr));
                set_outgoing_seq(pc->co, dsr_get_outgoing(sr) + 1);
                if (create_new || fco == NULL) {
                    fco = create_fix_con_obj(pc->in_args->sender_comp,
                                             pc->in_args->sc_len,
                                             name, n_len,
                                             pc->in_args->sender_sub,
                                             pc->in_args->ss_len,
                                             pc->in_args->target_sub,
                                             pc->in_args->ts_len,
                                             get_incoming_seq(pc->co),
                                             get_outgoing_seq(pc->co),
                                             pc->t,
                                             0, pc->in_args->log_outgoing,
                                             pc->timeout_val,
                                             pc->in_args->fix_version,
                                             pc->in_args->fv_len,
                                             pc->mod_name,
                                             pc->mod_len, pc->co,
                                             pc->
                                             in_args->create_mess_header);
                    if (fco == NULL) {
                        send_debug_message("Unable to allocate fco.\n");
                        return 0;
                    }
                }
                dfc_set_timeout(fco, pc->timeout_val);
                dfc_set_tickle(fco, 0);
                dfc_set_disconnect(fco, 0);
                dfc_set_con_obj(fco, pc->co);
                dfc_set_mod_name(fco, pc->mod_name, pc->mod_len);
                destroy_sequence_req(sr);
                mblk = calloc(DEF_FIX_SIZE, 1);
                if (mblk == NULL) {
                    return 0;
                }
                len =
                    sprintf(mblk, "98=0%c108=30%c", FIX_FIELD_DELIM,
                            FIX_FIELD_DELIM);
                pc->init_con(pc->co);
                if (len > 0) {
                    polish_n_send(mblk, len, fco, "A", 1);
                    add_fix_con(fco);
                    dfc_set_disconnect(fco, 0);
                }
                free(mblk);
                /* We need to reset this value for the init_args. */
                pc->in_args->reset_time->new_incoming = 0;
                pc->in_args->reset_time->new_outgoing = 0;
            }
            run_fix_admin_loop(fco);
            pc->connection_notice(pc->co, 1, NULL, 0);
            pc->is_logged_on = 1;
            our_seq = get_incoming_seq(pc->co);
            if (other_seq > our_seq && our_seq > 0) {
                should_record = 0;
                request_resend(name, n_len, our_seq);
            } else if (other_seq < our_seq) {
                char reason[100];
                size_t reason_len;
                fix_con_obj *fco;
                reason_len = 0;
                reason_len =
                    sprintf(reason,
                            "58=Seq to low: got %ld expected %lu%c",
                            other_seq, get_incoming_seq(pc->co),
                            FIX_FIELD_DELIM);
                fco = NULL;
                fco = get_fix_con_obj(m->name, m->name_len);
                if (fco != NULL && reason_len > 0) {
                    polish_n_send(reason, reason_len, fco, "5", 1);
                }
                *cut_con = 1;
                pc->connection_notice(pc->co, 0, NULL, 0);
                pc->is_logged_on = 0;
                should_record = 0;
            }
        } else {
            send_debug_message
                ("Unable pull sender comp ID from this: %s \n", d_off);
            *cut_con = 1;
            should_record = 0;
        }
        if (name) {
            free(name);
        }
    } else {
        send_debug_message
            ("Unable to find senderCompId in fix message: %s \n", m->data);
        *cut_con = 1;
        should_record = 0;
    }
    return should_record;
}

int send_logout(fix_con_obj * fco)
{
    char reason[100];
    size_t reason_len;
    reason_len = 0;
    reason_len = sprintf(reason, "58=Bye%c", FIX_FIELD_DELIM);
    if (fco != NULL && reason_len > 0) {
        polish_n_send(reason, reason_len, fco, "5", 1);
    }
    return 1;
}

void test_req_response(struct message *m, fix_con_obj * fco)
{
    char *builder;
    char *d_off;
    size_t total_len;
    char fix_test_req_id[6] = { FIX_FIELD_DELIM, '1', '1', '2',
        FIX_FIELD_VALUE_DELIM, '\0'
    };
    long test_req;
    total_len = 0;
    builder = calloc(400, 1);
    if (builder == NULL) {
        return;
    }
    d_off = builder;
    test_req = find_offset(m->data, m->len, fix_test_req_id, 6);
    if (test_req >= 0) {
        char *m_off;
        long eot;
        char fix_delim_str[2] = { FIX_FIELD_DELIM, '\0' };
        m_off = m->data;
        m_off += test_req + 1;
        eot =
            find_offset(m_off, (m->len - (test_req + 1)), fix_delim_str,
                        2);
        if (eot >= 0) {
            long req_len = eot + 1;
            memcpy(d_off, m_off, req_len);
            total_len += req_len;
            polish_n_send(builder, total_len, fco, "0", 1);
            free(builder);
        } else {
            free(builder);
            send_debug_message("could not find end of test request: %s \n",
                               m->data);
        }
    }
}

static void reset_seq_number(struct message *m, struct parse_con *pc,
                             int *cut_con)
{
    char *d_off;
    long res;
    char new_seq[5] =
        { FIX_FIELD_DELIM, '3', '6', FIX_FIELD_VALUE_DELIM, '\0' };
    char gap_fill_flag[] = { '1', '2', '3', '=', 'Y', '\x1', '\0' };
    d_off = m->data;
    res = find_offset(d_off, m->len, new_seq, 5);
    if (res >= 0) {
        unsigned long n_seq;
        unsigned long m_seq = get_sequence(m->data, m->len);
        unsigned long l_seq = get_incoming_seq(pc->co);
        d_off += res + 4;
        n_seq = 0;
        n_seq = atol(d_off);
        if (n_seq >= m_seq && n_seq > l_seq) {
            struct incoming_message *m1;
            long diff = n_seq - (m_seq - 1);
            long temp_diff = n_seq - l_seq;
            diff = temp_diff > diff ? diff : temp_diff;
            m1 = create_incoming_message(m->name, m->name_len, diff);
            if (m1 != NULL) {
                size_t i = 0;
                record_inbound_message(m1);
                l_seq += diff;
                for (i = 0; i < diff; ++i) {
                    increment_incoming_seq(pc->co);
                }
            }
        } else if (n_seq < m_seq
                   && find_offset(m->data, m->len, gap_fill_flag,
                                  sizeof gap_fill_flag) != -1) {
            /* Because of a bad FIX session partner, one which will go
               unnamed (<*loud cough*>Belzberg!), we will ignore any
               SequenceReset messages that tell us to set our sequence
               lower than it currently is.  Despite it being
               completely unnecessary, this particular unnamed entity
               insists on sending us one of these before fulfilling a
               ResendRequest.  We detect this particular situation if
               123=Y is not found.
             */
            struct incoming_message *m1;
            char reason[100];
            int reason_len;
            fix_con_obj *fco;
            reason_len = 0;
            reason_len =
                sprintf(reason, "58=Reset Seq Problem: %ld > %ld%c", m_seq,
                        n_seq, FIX_FIELD_DELIM);
            fco = NULL;
            fco = get_fix_con_obj(m->name, m->name_len);
            if (fco != NULL && reason_len > 0) {
                polish_n_send(reason, reason_len, fco, "5", 1);
            }

            *cut_con = 1;
            m1 = create_incoming_message(m->name, m->name_len, 1);
            if (m1 != NULL) {
                record_inbound_message(m1);
                increment_incoming_seq(pc->co);
            }
        }
    } else {
        send_debug_message("Not a valid Sequence Reset: %s Message: %s",
                           m->name, m->data);
    }

    {
        unsigned long m_seq = get_sequence(m->data, m->len);
        unsigned long last_seq = get_incoming_seq(pc->co);
        if (m_seq > last_seq) {
            unsigned long diff = m_seq - last_seq;
            unsigned long end = 0;
            if (diff > 2500) {
                unsigned long resend_start = last_seq;
                unsigned long resend_end = last_seq + 2000;
                request_resend2(m->name, m->name_len, resend_start,
                                resend_end);
                resend_start += 2000;
                resend_end += 2000;
                if (resend_end > m_seq) {
                    resend_end = 0;
                }
            } else {
                request_resend2(m->name, m->name_len, last_seq, end);
            }
        }
    }
}

static long create_gap_fill(char *output, long out_len,
                            struct parse_con *pc, unsigned long seq)
{
    char *builder;
    char *d_off;
    char *t_off;
    long len;
    long total_len;
    long output_len;
    time_t now;
    databuf_t fixhdr;
    output_len = 0;
    len = 0;
    total_len = 0;
    builder = calloc(500, 1);
    if (builder == NULL) {
        return 0;
    }
    d_off = builder;
    t_off = output;
    len = sprintf(d_off, "%c35=4", FIX_FIELD_DELIM);
    d_off += len;
    total_len += len;
    if (databuf_attach(&fixhdr, d_off, (out_len - total_len)) == 0) {
        len =
            pc->in_args->create_mess_header(&fixhdr,
                                            pc->in_args->sender_comp,
                                            pc->in_args->sc_len,
                                            pc->in_args->target_comp,
                                            pc->in_args->tc_len,
                                            pc->in_args->sender_sub,
                                            pc->in_args->ss_len,
                                            pc->in_args->target_sub,
                                            pc->in_args->ts_len, seq,
                                            &now);
    }
    if (len <= 0) {
        send_debug_message
            ("Zero len from fix header: %s, %lu, %s, %lu, %s, %lu, %s, %lu, %lu \n",
             pc->in_args->sender_comp, pc->in_args->sc_len,
             pc->in_args->target_comp, pc->in_args->tc_len,
             pc->in_args->sender_sub, pc->in_args->ss_len,
             pc->in_args->target_sub, pc->in_args->ts_len, seq);
    }
    total_len += len;
    d_off += len;
    len =
        sprintf(d_off, "43=Y%c97=Y%c122=", FIX_FIELD_DELIM,
                FIX_FIELD_DELIM);
    total_len += len;
    d_off += len;
    len = create_timestamp(d_off, (out_len - total_len), &now);
    total_len += len;
    d_off += len;
    len =
        sprintf(d_off, "123=Y%c36=%lu%c", FIX_FIELD_DELIM, (seq + 1),
                FIX_FIELD_DELIM);
    total_len += len;
    d_off += len;
    len =
        sprintf(output, "8=%s%c9=%ld", pc->in_args->fix_version,
                FIX_FIELD_DELIM, (total_len - 1));
    output += len;
    output_len += len;
    if (out_len > total_len) {
        int check_sum;
        memcpy(output, builder, total_len);
        output += total_len;
        output_len += total_len;
        check_sum = calc_check_sum(t_off, output_len);
        len = sprintf(output, "10=%03d%c", check_sum, FIX_FIELD_DELIM);
        output_len += len;
        output += len;
    } else {
        send_debug_message
            ("We ran out of space in create_gap_fill. "
             "out_len: %lu total_len %lu {%s} \n",
             out_len, total_len, output);
        free(builder);
        return -1;
    }
    free(builder);
    return output_len;
}

static long resend_message(char *start, long sLen, char *output,
                           long out_len, struct parse_con *pc, long seq)
{
    char fix_time_tag[5] =
        { FIX_FIELD_DELIM, '5', '2', FIX_FIELD_VALUE_DELIM, '\0' };
    char *builder;
    char *d_off;
    char *orig_out;
    char *s_off;
    long len;
    long total_len;
    long output_len;
    long ts_off;
    time_t now;
    databuf_t fixhdr;
    orig_out = output;
    output_len = 0;
    len = 0;
    total_len = 0;
    builder = calloc(500, 1);
    if (builder == NULL) {
        send_debug_message
            ("Could not allocate builder in resend_message!!!\n");
        return 0;
    }
    d_off = builder;
    s_off = start;
    len = sprintf(d_off, "35=%c", start[0]);
    total_len += len;
    d_off += len;
    if (databuf_attach(&fixhdr, d_off, out_len - total_len) == 0) {
        len = pc->in_args->create_mess_header(&fixhdr,
                                              pc->in_args->sender_comp,
                                              pc->in_args->sc_len,
                                              pc->in_args->target_comp,
                                              pc->in_args->tc_len,
                                              pc->in_args->sender_sub,
                                              pc->in_args->ss_len,
                                              pc->in_args->target_sub,
                                              pc->in_args->ts_len,
                                              seq, &now);
    }
    total_len += len;
    d_off += len;
    len =
        sprintf(d_off, "43=Y%c97=Y%c122=", FIX_FIELD_DELIM,
                FIX_FIELD_DELIM);
    total_len += len;
    d_off += len;
    ts_off = find_offset(s_off, sLen, fix_time_tag, 5);
    if (ts_off >= 0) {
        long time_len;
        int check_sum;
        int beg_mess_len = 0;
        s_off += (ts_off + 4);
        time_len = sLen - (ts_off + 4);
        memcpy(d_off, s_off, time_len);
        d_off += time_len;
        total_len += time_len;
        beg_mess_len =
            sprintf(output, "8=%s%c9=%ld%c", pc->in_args->fix_version,
                    FIX_FIELD_DELIM, (total_len), FIX_FIELD_DELIM);
        output += beg_mess_len;
        output_len += beg_mess_len;
        //orig_out += beg_mess_len;
        memcpy(output, builder, total_len);
        output += total_len;
        output_len += total_len;
        check_sum = calc_check_sum(orig_out, (output_len));
        len = sprintf(output, "10=%03d%c", check_sum, FIX_FIELD_DELIM);
        output_len += len;
        output += len;
        free(builder);
    } else {
        send_debug_message("Unable to find original sending time. \n");
        free(builder);
        return 0;
    }
    return output_len;
}

static long determine_resend(char *d_off, long dLen, char *output,
                             long out_len, struct parse_con *pc)
{
    char fix_msg_type[5] =
        { FIX_FIELD_DELIM, '3', '5', FIX_FIELD_VALUE_DELIM, '\0' };
    long type_off = 0;
    char *t_off = d_off;
    long total_len;
    unsigned long seq;
    long ret_val;
    total_len = 0;
    ret_val = 0;
    type_off = find_offset(t_off, dLen, fix_msg_type, 5);
    if (type_off >= 0) {
        char val;
        t_off += (type_off + 4);
        val = t_off[0];
        seq = get_sequence(t_off, dLen);
        switch (val) {
        case '8':
        case 'F':
            ret_val =
                resend_message(t_off, (dLen - (type_off + 4)), output,
                               out_len, pc, seq);
            total_len += ret_val;
            break;
        default:
            ret_val = create_gap_fill(output, out_len, pc, seq);
            if (ret_val > 0) {
                total_len += ret_val;
            } else {
                send_debug_message
                    ("Ran out of space in gap fill sent this many %ld. \n",
                     out_len);
                return total_len;
            }
        };
    } else {
        send_debug_message
            ("Could not find message type flag in resend parse.");
    }
    return total_len;
}

static void parse_resend_mess(char *output, size_t out_len,
                              struct parse_con *pc, struct message *m)
{
    char fix_end_msg[5] =
        { FIX_FIELD_DELIM, '1', '0', FIX_FIELD_VALUE_DELIM, '\0' };
    fix_con_obj *fco;
    long cur_off;
    long cur_len;
    long end_pos;
    char *d_off;
    long len = out_len;
    long total_len = 0;
    char *final_resend;
    size_t size;
    int should_parse = 1;
    char *f_resend;
    fco = NULL;
    d_off = output;
    end_pos = 0;
    cur_off = 0;
    cur_len = 0;
    size = out_len * 20;
    final_resend = calloc(size, 1);
    if (final_resend == NULL) {
        send_debug_message("unable to allocate resend message. %lu \n",
                           size);
        return;
    }
    f_resend = final_resend;
    while (should_parse && len > 5) {
        end_pos = find_offset(d_off, len, fix_end_msg, 5);
        if (end_pos > 0) {
            long l_len;
            cur_len = end_pos + 1;
            end_pos += 8;
            len -= end_pos;
            l_len = determine_resend(d_off, cur_len, f_resend, size, pc);
            total_len += l_len;
            cur_off += end_pos;
            d_off += end_pos;
            f_resend += l_len;
            size -= l_len;
        } else {
            should_parse = 0;
        }
    }
    fco = get_fix_con_obj(pc->in_args->target_comp, pc->in_args->tc_len);
    if (fco != NULL) {
        dfc_lock_seq_blocker(fco);
        fix_internal_send(final_resend, total_len, fco);
        dfc_unlock_seq_blocker(fco);
    }
    free(final_resend);
}

static void gap_all(struct message *m, struct parse_con *pc,
                    fix_con_obj * fco)
{

    char fix_begin_res[4] =
        { FIX_FIELD_DELIM, '7', FIX_FIELD_VALUE_DELIM, '\0' };
    long res_code = 0;
    char *d_off;
    databuf_t fixhdr;
    d_off = m->data;
    res_code = find_offset(d_off, m->len, fix_begin_res, 4);
    if (res_code >= 0) {
        long beg_res = 0;
        char *output = calloc(500, 1);
        char *builder = calloc(500, 1);
        size_t output_len = 0;
        size_t len = 0;
        size_t total_len = 0;
        char *t_off = output;
        char *c_off = output;
        int check_sum = 0;
        size_t seq = dfc_get_outgoing_seq(fco);
        time_t now;
        d_off += res_code + 3;
        beg_res = atoi(d_off);
        d_off = builder;
        if (builder == NULL) {
            return;
        }
        len = sprintf(d_off, "%c35=4", FIX_FIELD_DELIM);
        d_off += len;
        total_len += len;
        if (databuf_attach(&fixhdr, d_off, (500 - total_len)) == 0) {
            len = dfc_create_fix_header(fco, &fixhdr,
                                        pc->in_args->sender_comp,
                                        pc->in_args->sc_len,
                                        pc->in_args->target_comp,
                                        pc->in_args->tc_len,
                                        pc->in_args->sender_sub,
                                        pc->in_args->ss_len,
                                        pc->in_args->target_sub,
                                        pc->in_args->ts_len,
                                        beg_res, &now);

        }
        total_len += len;
        d_off += len;
        len =
            sprintf(d_off, "43=Y%c97=Y%c122=", FIX_FIELD_DELIM,
                    FIX_FIELD_DELIM);
        total_len += len;
        d_off += len;
        len = create_timestamp(d_off, (500 - total_len), &now);
        total_len += len;
        d_off += len;
        len =
            sprintf(d_off, "123=Y%c36=%lu%c", FIX_FIELD_DELIM, (seq),
                    FIX_FIELD_DELIM);
        total_len += len;
        d_off += len;
        len =
            sprintf(output, "8=%s%c9=%ld", pc->in_args->fix_version,
                    FIX_FIELD_DELIM, (total_len - 1));
        //output += len;
        c_off += len;
        output_len += len;
        memcpy(c_off, builder, total_len);
        c_off += total_len;
        output_len += total_len;
        check_sum = calc_check_sum(t_off, output_len);
        len = sprintf(c_off, "10=%03d%c", check_sum, FIX_FIELD_DELIM);
        output_len += len;
        c_off += len;
        free(builder);
        dfc_lock_seq_blocker(fco);
        fix_internal_send(output, output_len, fco);
        dfc_unlock_seq_blocker(fco);
        free(output);
    }
}

static void handle_resend_request(struct message *m, struct parse_con *pc)
{
    char fix_begin_res[4] =
        { FIX_FIELD_DELIM, '7', FIX_FIELD_VALUE_DELIM, '\0' };
    char fix_end_res[5] =
        { FIX_FIELD_DELIM, '1', '6', FIX_FIELD_VALUE_DELIM, '\0' };
    long res_code = 0;
    char *d_off;
    d_off = m->data;
    res_code = find_offset(d_off, m->len, fix_begin_res, 4);
    if (res_code >= 0) {
        long beg_res = 0;
        long end_res = 0;
        resend_req *rr;
        char *output = NULL;
        size_t out_len = 0;
        d_off += res_code + 3;
        beg_res = atoi(d_off);
        d_off = m->data;
        res_code = find_offset(d_off, m->len, fix_end_res, 5);
        if (res_code >= 0) {
            d_off += res_code + 4;
            end_res = atoi(d_off);
        } else {
            send_debug_message
                ("Unable to find end sequence!!! %s in message: %s ",
                 m->name, m->data);
        }
        rr = create_resend_request(m->name, m->name_len, beg_res, end_res,
                                   find_fix_offsets);
        get_resend_request(rr);
        output = (char *) drr_get_resend_output(rr, &out_len);
        if (out_len > 0) {
            parse_resend_mess(output, out_len, pc, m);
        }
        destroy_resend_req(rr);
    } else {
        send_debug_message
            ("Unable to find Begin sequence!!! %s in message: %s ",
             m->name, m->data);
    }
}

static int insert_value_tag(const char *message_to_search, size_t ms_len,
                            const char *search_tag, size_t st_len,
                            char *output, size_t out_len, int offset)
{
    char *d_off = output;
    char *m_off = (char *) message_to_search;
    size_t total_len = 0;
    char fix_delim_str[2] = { FIX_FIELD_DELIM, '\0' };
    long ret_code = 0;
    ret_code = find_offset(message_to_search, ms_len, search_tag, st_len);
    if (ret_code >= 0) {
        m_off += ret_code + offset;
        ret_code =
            find_offset(m_off, (ms_len - (ret_code + 1)), fix_delim_str,
                        1);
        if (ret_code >= 0) {
            total_len = ret_code + 1;
            memcpy(d_off, m_off, total_len);
        }
    }
    return total_len;
}

static void parse_rejected_message(struct message *m, struct parse_con *pc,
                                   char *rr_output, size_t rr_out_len)
{
    char cl_ord_id[5] =
        { FIX_FIELD_DELIM, '1', '1', FIX_FIELD_VALUE_DELIM, '\0' };
    char orig_cl_ord[5] =
        { FIX_FIELD_DELIM, '4', '1', FIX_FIELD_VALUE_DELIM, '\0' };
    char text[5] =
        { FIX_FIELD_DELIM, '5', '8', FIX_FIELD_VALUE_DELIM, '\0' };
    char type[5] =
        { FIX_FIELD_DELIM, '3', '5', FIX_FIELD_VALUE_DELIM, '\0' };
    char offtype[6] =
        { FIX_FIELD_DELIM, '3', '7', '1', FIX_FIELD_VALUE_DELIM, '\0' };
    char rea[6] =
        { FIX_FIELD_DELIM, '3', '7', '3', FIX_FIELD_VALUE_DELIM, '\0' };
    char rea2[6] =
        { FIX_FIELD_DELIM, '3', '7', '9', FIX_FIELD_VALUE_DELIM, '\0' };
    char mtype[6] =
        { FIX_FIELD_DELIM, '3', '7', '2', FIX_FIELD_VALUE_DELIM, '\0' };
    char bizRea[6] =
        { FIX_FIELD_DELIM, '3', '8', '0', FIX_FIELD_VALUE_DELIM, '\0' };
    char quoteID[6] =
        { FIX_FIELD_DELIM, '1', '1', '7', FIX_FIELD_VALUE_DELIM, '\0' };
    char *output = calloc(500, 1);
    if (output != NULL) {
        char *o_off = output;
        char *r_off = rr_output;
        long ret_code = 0;
        size_t total_len = 0;
        size_t s_len = 0;
        struct message m1;
        m1.name = m->name;
        m1.name_len = m->name_len;
        ret_code = find_offset(r_off, rr_out_len, type, 4);
        if (ret_code >= 0) {
            char val;
            r_off += ret_code + 4;
            val = r_off[0];
            switch (val) {
            case 'F':
                s_len =
                    sprintf(o_off, "35=9%c39=0%c434=1%c", FIX_FIELD_DELIM,
                            FIX_FIELD_DELIM, FIX_FIELD_DELIM);
                o_off += s_len;
                total_len += s_len;
                break;
            case 'G':
                s_len =
                    sprintf(o_off, "35=9%c39=0%c434=2%c", FIX_FIELD_DELIM,
                            FIX_FIELD_DELIM, FIX_FIELD_DELIM);
                o_off += s_len;
                total_len += s_len;
                break;
            case 'i':
                s_len = insert_value_tag(m->data, m->len, type,4, o_off, total_len, 0);
                o_off += s_len;
                total_len += s_len;
                s_len =
                    insert_value_tag(rr_output, rr_out_len, quoteID, 4,
                                     o_off, total_len,1);
                o_off += s_len;
                total_len += s_len;
                break;
            default:
                s_len =
                    sprintf(o_off, "35=8%c39=8%c150=8%c", FIX_FIELD_DELIM,
                            FIX_FIELD_DELIM, FIX_FIELD_DELIM);
                o_off += s_len;
                total_len += s_len;
                break;
            }
            s_len =
                insert_value_tag(rr_output, rr_out_len, cl_ord_id, 4,
                                 o_off, total_len, 1);
            o_off += s_len;
            total_len += s_len;
            s_len =
                insert_value_tag(rr_output, rr_out_len, orig_cl_ord, 4,
                                 o_off, total_len, 1);
            o_off += s_len;
            total_len += s_len;
            s_len =
                insert_value_tag(m->data, m->len, text, 4, o_off,
                                 total_len, 1);
            o_off += s_len;
            total_len += s_len;
            s_len =
                insert_value_tag(m->data, m->len, rea, 5, o_off,
                                 total_len, 1);
            o_off += s_len;
            total_len += s_len;
            s_len = insert_value_tag(m->data, m->len, rea2, 5, o_off, total_len, 1);
            o_off += s_len;
            total_len += s_len;
            s_len = insert_value_tag(m->data, m->len, mtype, 5, o_off, total_len, 1);
            o_off += s_len;
            total_len += s_len;
            s_len = insert_value_tag(m->data, m->len, bizRea, 5, o_off, total_len, 1);
            o_off += s_len;
            total_len += s_len;
            s_len = insert_value_tag(m->data, m->len, offtype,5, o_off, total_len,1);
            o_off += s_len;
            total_len += s_len;
            m1.data = output;
            m1.len = total_len;
            pc->callback(&m1, pc->book);
        }
        free(output);
    }
}

static void process_reject(struct message *m, struct parse_con *pc)
{
    char fix_ref_seq[5] =
        { FIX_FIELD_DELIM, '4', '5', FIX_FIELD_VALUE_DELIM, '\0' };
    long res_code = 0;
    char *d_off;
    d_off = m->data;
    res_code = find_offset(d_off, m->len, fix_ref_seq, 4);
    if (res_code >= 0) {
        long beg_res;
        long end_res = 0;
        char *output = NULL;
        size_t out_len = 0;
        resend_req *rr;
        d_off += res_code + 4;
        beg_res = atoi(d_off);
        d_off = m->data;
        end_res = beg_res;
        rr = create_resend_request(m->name, m->name_len, beg_res, end_res,
                                   find_fix_offsets);
        get_resend_request(rr);
        output = (char *) drr_get_resend_output(rr, &out_len);
        if (out_len > 0) {
            parse_rejected_message(m, pc, output, out_len);
        }
        destroy_resend_req(rr);
    } else {
        send_debug_message
            ("Unable to find Begin sequence!!! %s in message: %s ",
             m->name, m->data);
    }
}
/*
static void check_seq(struct parse_con* pc, struct message* m)
{
    fix_con_obj *fco;
    char *name = (char *) get_name(pc->co);
    fco = get_fix_con_obj(name, get_name_len(pc->co));
    if (fco != NULL) {
        long our_seq = get_incoming_seq(pc->co);
        long mess_seq = get_sequence(m->data, m->len);
        dfc_set_incoming_seq(fco, mess_seq);
        if (mess_seq > our_seq && our_seq > 0 && ((mess_seq - our_seq) < 2500)) {
            request_resend(name, get_name_len(pc->co), our_seq);
        } else if(mess_seq > our_seq && our_seq > 0) {
            request_resend2(name, get_name_len(pc->co), our_seq, (our_seq + 2400));
        }
    }
    if(name) {
        free(name);
    }
}
*/
static void handle_admin_message(struct message *m, int *cut_con,
                                 struct parse_con *pc, char type)
{
    struct incoming_message *m1;
    fix_con_obj *fco;
    int logon_ret = 0;
    fco = NULL;
    m1 = create_incoming_message(m->name, m->name_len, 1);
    if (m1 == NULL) {
        send_debug_message
            ("Unable to allocate incoming message struct.\n");
        return;
    }
    switch (type) {
    case FIX_LOGON:
        logon_ret = handle_fix_logon(m, cut_con, pc, m1);
        if (logon_ret) {
            record_inbound_message(m1);
            increment_incoming_seq(pc->co);
        } else {
            destroy_incoming_message(m1);
        }
        break;
    case FIX_RESEND:
        if (pc->in_args->log_outgoing) {
            handle_resend_request(m, pc);
        } else {
            fco =
                get_fix_con_obj(pc->in_args->target_comp,
                                pc->in_args->tc_len);
            if (fco != NULL) {
                gap_all(m, pc, fco);
            }

        }
        /*fco =
            get_fix_con_obj(pc->in_args->target_comp,
                            pc->in_args->tc_len);
        if (fco != NULL) {
            gap_all(m, pc, fco);
        }*/
        record_inbound_message(m1);
        increment_incoming_seq(pc->co);
        break;
    case FIX_TEST_REQ:
        fco =
            get_fix_con_obj(pc->in_args->target_comp, pc->in_args->tc_len);
        if (fco != NULL) {
            test_req_response(m, fco);
        }
        record_inbound_message(m1);
        increment_incoming_seq(pc->co);
        break;
    case FIX_LOGOUT:
        *cut_con = 1;
        break;
    case FIX_SEQ_RESET:
        reset_seq_number(m, pc, cut_con);
        destroy_incoming_message(m1);   //free(m1);
        break;
    case FIX_BIZ_REJECT:
    case FIX_REJECT:
        if (pc->in_args->log_outgoing) {
            process_reject(m, pc);
        }
        record_inbound_message(m1);
        increment_incoming_seq(pc->co);
        break;
    default:
        record_inbound_message(m1);
        increment_incoming_seq(pc->co);
        //check_seq(pc, m);
        break;
    }
}

void low_activity_sig(con_obj * co)
{
    fix_con_obj *fco;
    char *name = (char *) get_name(co);
    fco = get_fix_con_obj(name, get_name_len(co));
    if (fco != NULL) {
        time_t now = 0;
        time(&now);
        time_t last_rec = dfc_get_last_mess_rec(fco);
        int diff = now - last_rec;
        if (last_rec == 0 || diff < 300) {
            tickle_connection(fco);
        } else {
            kill_connection(fco);
            dfc_set_con_obj(fco, NULL);
            dfc_set_last_mess_rec(fco, 0);
        }
    }
    if (name) {
        free(name);
    }
}

void fix_connection_callback(con_obj * co, int is_connected)
{
    fix_con_obj *fco;
    char *name = (char *) get_name(co);
    fco = get_fix_con_obj(name, get_name_len(co));
    if (fco != NULL && !is_connected) {
        kill_connection(fco);
        dfc_set_con_obj(fco, NULL);
        dfc_set_last_mess_rec(fco, 0);
    }
    if (name) {
        free(name);
    }
}


static void set_rec_mess(con_obj * co)
{
    fix_con_obj *fco;
    char *name = (char *) get_name(co);
    fco = get_fix_con_obj(name, get_name_len(co));
    if (fco != NULL) {
        time_t rawtime = 0;
        time(&rawtime);
        dfc_set_last_mess_rec(fco, rawtime);
    }
    if (name) {
        free(name);
    }
}

static void determine_msg_type(char *mlbk, unsigned long len, int *cut_con,
                               struct parse_con *pc)
{
    char *ret_code = mlbk;
    if (len > 4) {
        struct message m;
        char *name = (char *) get_name(pc->co);
        unsigned long n_len = get_name_len(pc->co);
        m.name = name;
        m.name_len = n_len;
        m.len = len;
        m.data = mlbk;
        dart_rolling_log_coming(&m, pc->mod_name, pc->mod_len);
        ret_code += 3;
        set_rec_mess(pc->co);
        if (ret_code[0] == FIX_ORDER_S || ret_code[0] == FIX_REPLACE_REQ ||
            ret_code[0] == FIX_CANCEL_REQ || ret_code[0] == FIX_EXEC
            || ret_code[0] == FIX_CANCEL_REJECT
            || strncmp(ret_code, "AB", 2) == 0
            || strncmp(ret_code, "AC", 2) == 0
            || strncmp(ret_code, "BF", 2) == 0
            || strncmp(ret_code, "BE", 2) == 0
            || strncmp(ret_code, "AR", 2) == 0
            || strncmp(ret_code, "AE", 2) == 0
            || strncmp(ret_code, "CI", 2) == 0
            || ret_code[0] == 'b'
            || ret_code[0] == 'd'
            || ret_code[0] == 'Q' || ret_code[0] == 'r') {
            struct incoming_message *m1;
            //check_seq(pc, &m);
            if (pc->is_logged_on) {
                increment_incoming_seq(pc->co);
                pc->callback(&m, pc->book);

                m1 = create_incoming_message(name,
                                             get_name_len(pc->co), 1);
                if (m1 != NULL) {
                    record_inbound_message(m1);
                }
            } else {
                *cut_con = 1;
            }
        } else {
            handle_admin_message(&m, cut_con, pc, ret_code[0]);
        }
        if (name) {
            free(name);
        }
    } else {
        send_debug_message("Unable to determine type of message: %s !!\n",
                           mlbk);
    }
}

unsigned long parse_fix_message(const char *rd_ptr, unsigned long len,
                                int *cut_con, struct parse_con *pc)
{
    unsigned long read_bytes = 0;
    if (len > FIX_BEG_MESSAGE_LEN) {
        int byte_len = 0;
        long mess_len = 0;
        char *ret_code;
        long len_left = len;
        int should_parse = 1;
        char *d_off;
        d_off = (char *) rd_ptr;
        while (should_parse && len_left > 0 && read_bytes < len) {
            d_off += FIX_BEG_MESSAGE_LEN;
            read_bytes += FIX_BEG_MESSAGE_LEN;
            len_left -= FIX_BEG_MESSAGE_LEN;
            if (read_bytes < len) {
                byte_len = atoi(d_off);
                ret_code = memchr(d_off, FIX_FIELD_DELIM, len_left);
                if (ret_code) {
                    mess_len =
                        (ret_code - d_off) + 1 + byte_len +
                        FIX_CHECKSUM_LEN;
                    if (mess_len <= len_left && mess_len > 0) {
                        d_off += (ret_code - d_off) + 1;
                        determine_msg_type(d_off,
                                           (byte_len + FIX_CHECKSUM_LEN),
                                           cut_con, pc);
                        d_off += byte_len + FIX_CHECKSUM_LEN;
                        read_bytes += mess_len;
                        len_left -= mess_len;
                    } else {
                        should_parse = 0;
                        read_bytes -= FIX_BEG_MESSAGE_LEN;
                    }
                } else {
                    send_debug_message
                        ("Could not find a ASCII TAG 1: "
                         "%lu, len_left: %ld, read_bytes: "
                         "%lu, d_off %s  \n",
                         len, len_left, read_bytes, d_off);
                    should_parse = 0;
                    read_bytes -= FIX_BEG_MESSAGE_LEN;
                }
            } else {
                should_parse = 0;
                read_bytes -= FIX_BEG_MESSAGE_LEN;
            }
        }
    } else {
        send_debug_message("Len is < 12 %lu \n", len);
    }
    return read_bytes;
}

int send_test_req(fix_con_obj * fco)
{
    char *sender;
    char *d_off;
    size_t len;
    int ret_code;
    sender = calloc(400, 1);
    if (sender == NULL) {
        return -1;
    }
    d_off = sender;
    ret_code = -1;
    if (fco != NULL) {
        len = sprintf(d_off, "112=TEST%c", FIX_FIELD_DELIM);
        polish_n_send(sender, len, fco, "1", 1);
        free(sender);
        ret_code = 1;
    } else {
        free(sender);
    }
    return ret_code;
}

int send_heart_beat(fix_con_obj * fco)
{
    int ret_code;
    ret_code = -1;
    if (fco != NULL) {
        polish_n_send(NULL, 0, fco, "0", 1);
        ret_code = 1;
    }
    return ret_code;
}

int send_fix_message(struct message *m, char *type, size_t type_len)
{
    fix_con_obj *l_obj;
    l_obj = NULL;
    l_obj = get_fix_con_obj(m->name, m->name_len);
    if (l_obj != NULL) {
        char *send_off;
        size_t send_len;
        send_off = m->data;
        send_len = m->len;
        if (send_off[0] == FIX_FIELD_DELIM) {
            send_off += 1;
            send_len -= 1;
        }
        polish_n_send(send_off, send_len, l_obj, type, type_len);
    } else if (l_obj == NULL) {
        struct init_args *in_arg;
        send_debug_message
            ("Could not find connection for this name: %s \n", m->name);
        init_map();
        in_arg = NULL;
        in_arg = find_config(m->name, m->name_len);
        if (in_arg != NULL) {
            char *send_off;
            size_t send_len;
            sequence_req *sr;
            app_end_time *t = malloc(sizeof(struct app_end_time));

            sr = create_sequence_request(m->name, m->name_len,
                                         in_arg,
                                         build_fix_offsets,
                                         create_fix_resend_pad);
            if (sr == NULL) {
                return -1;
            }
            get_sequence_numbers(sr);
            if (in_arg->reset_time != NULL) {
                t->end_hour = in_arg->reset_time->reset_hour;
                t->end_min = in_arg->reset_time->reset_min;
            } else {
                t->end_hour = 23;
                t->end_min = 58;
                t->end_day = 7;
            }
            l_obj = create_fix_con_obj(in_arg->sender_comp, in_arg->sc_len,
                                       in_arg->target_comp, in_arg->tc_len,
                                       in_arg->sender_sub, in_arg->ss_len,
                                       in_arg->target_sub, in_arg->ts_len,
                                       dsr_get_incoming(sr),
                                       dsr_get_outgoing(sr) + 1, t,
                                       0, in_arg->log_outgoing, 0,
                                       in_arg->fix_version, in_arg->fv_len,
                                       NULL, 0, NULL,
                                       in_arg->create_mess_header);
            if (l_obj == NULL) {
                return -1;
            }
            destroy_sequence_req(sr);
            add_fix_con(l_obj);
            send_off = m->data;
            send_len = m->len;
            if (send_off[0] == FIX_FIELD_DELIM) {
                send_off += 1;
                send_len -= 1;
            }
            polish_n_send(send_off, send_len, l_obj, type, type_len);
        } else {
            send_debug_message("Got a NULL init_args for: %s\n", m->name);
            return -1;
        }
    } else {
        char *send_off;
        size_t send_len;
        send_off = m->data;
        send_len = m->len;
        if (send_off[0] == FIX_FIELD_DELIM) {
            send_off += 1;
            send_len -= 1;
        }
        polish_n_send(send_off, send_len, l_obj, type, type_len);
    }
    return 0;
}
