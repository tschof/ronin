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
#include <unistd.h>
#include <netinet/tcp.h>
#include "rom_handler.h"
#include "parser_func.h"
#include "transaction_recorder.h"
#include "dart_seq_request.h"
#include "debug_logger.h"
#include "dart_constants.h"
#include "async_server.h"

static int dart_send_message(async_parse_args* co, char *data, size_t len)
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

static int local_sock_send(async_parse_args * co, char *data, unsigned long len)
{
    long sent_bytes;
    long bytes_to_send;
    char *d_off = data;
    long ret_code = 1;
    int failed_attempts = 0;
    sent_bytes = 0;
    bytes_to_send = (long) len;
    while (sent_bytes < len && ret_code >= 0) {
        ret_code = dart_send_message(co, d_off, bytes_to_send);
        if (ret_code < 0) {
            ret_code = 0;
            ++failed_attempts;
            if (failed_attempts >= 128 || co->disconnect) {
                return 0;
            }
        }
        bytes_to_send -= ret_code;
        sent_bytes += ret_code;
        d_off += ret_code;
    }
    return 1;
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

/* This needs to be altered so that we check username and password. */
static struct init_args *get_init_args(char *name, int name_len,
                                       queue_t in_args)
{
    if (name != NULL && name_len > 0 && in_args != NULL) {
        element_t e;
        e = front(in_args);
        while (e != NULL) {
            struct init_args *init;
            init = (struct init_args *) e->d;
            if (init != NULL && init->name_len == name_len && name_len > 0 &&
                strncmp(name, init->name, name_len) == 0) {
                return init;
            } else {
                e = e->next;
            }
        }
        send_debug_message("Element was null for: %s\n", name);
    } else {
        send_debug_message("Name was null or name_len was 0 or less.\n");
    }
    return NULL;
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
int init_async_rom_logon(struct async_parse_args *pa)
{
    int ret_val = 1;
    sequence_req *sr;
    char *mlbk;
    char *mlbk_off;
    int time_len;
    int seq_enabled = 1;
    unsigned long send_len;
    unsigned long send_seq;
    sr = create_sequence_request(pa->in_args->name, pa->in_args->name_len,
                                 pa->in_args,
                                 build_offsets,
                                 create_rom_resend_pad, pa);
    get_sequence_numbers(sr);
    /* Add the sequences into the connection object */
    send_seq = dsr_get_incoming(sr) + 1;
    pa->out_seq = dsr_get_outgoing(sr);
    destroy_sequence_req(sr);
    send_len = 0;
    //This 'should' be big enough.
    mlbk = malloc(550);
    if (mlbk != NULL) {
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
                        pa->in_args->username, pa->in_args->password,
                        send_seq);
        } else {
            send_len +=
                sprintf(mlbk_off,
                        "%s,,,,,%s,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n",
                        pa->in_args->username, pa->in_args->password);
        }
        if (local_sock_send(pa, mlbk, send_len)) {
        } else {
            pa->disconnect = 1;
            ret_val = 0;
        }
        free(mlbk);
    }
    return ret_val;
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
        char *d_off = data;
        size_t read_bytes = 0;
        char* ret_code = 0;
        size_t cur_seq = *seq;
        do {
            ret_code = memchr(d_off, '\n', (size - read_bytes));
            if(ret_code == 0x0) {
                break;
            }
            ++cur_seq;
            if (beg_seq == cur_seq) {
                *beg_off = (byte_offset + read_bytes);
            }
            read_bytes += (size_t) (ret_code - d_off);
            read_bytes += 1;
            d_off = (ret_code + 1);
            if (end_seq == cur_seq) {
                *end_off = (byte_offset + read_bytes);
            }
        } while(read_bytes < size);
        *seq = cur_seq;
        return read_bytes;
    }
    return size;
}

void handle_async_csv_logon(char type, const char *data, unsigned long len,
                            int *cut_con, struct async_parse_args *pc)
{
    if (type == 'L') {
        unsigned long cur_pos;
        int i;
        long ret_code;
        long nLen;
        char *ret_val = 0;
        char *d_off;
        char *name_ptr;
        long end;
        unsigned long last_rom_seq;
        unsigned long out_seq = 0;
        char comma[2] = { ',', '\0' };
        char *logger;
        cur_pos = 0;
        d_off = (char *) data;
        for (i = 0; i < 2; ++i) {
            ret_val = strchr(d_off, ',');
            if (ret_val) {
                ret_code = (ret_val - d_off);
                d_off += (ret_code + 1);
                cur_pos += 1 + (unsigned long) ret_code;
            } else {
                *cut_con = 1;
                return;
            }
        }
        ret_code = find_offset(d_off, (len - cur_pos), comma, 1);
        if (!pc->is_server) {
            nLen = pc->in_args->name_len;
            name_ptr =pc->in_args->name;
        } else if (ret_code > 0) {
            /*Here we get the connection name */
            name_ptr = d_off;
            nLen = ret_code;
        } else {
            *cut_con = 1;
            return;
        }
        /* Now find the ROM last sequence received. */
        for (i = 2; i < 72; ++i) {
            ret_val = strchr(d_off, ',');
            if (ret_val) {
                ret_code = (ret_val - d_off);
                d_off = d_off + ret_code + 1;
                cur_pos += 1 + (unsigned long) ret_code;
            } else {
                break;
            }
        }
        if (len > cur_pos && i >= 71) {
            end = find_offset(d_off, (len - cur_pos), comma, 1);
        } else {
            end = 0;
        }
        if (end > 0) {
            last_rom_seq = atol(d_off);
        } else {
            send_debug_message
                ("could not find rom_seq, {%s} total data = {%s}\n", d_off,
                 data);
            last_rom_seq = 0;
        }
        /* Now send Logon. */
        if (pc->is_server) {
            sequence_req *sr;
            char *mlbk;
            char *mlbk_off;
            char *send_name = 0;
            int time_len;
            unsigned long send_len;
            unsigned long send_seq;
            if (nLen > 0) {
                /* Here we need to cut the connection if it does not
                 * conform with any of our init structs. */
                pc->in_args =
                    get_init_args(name_ptr, nLen, pc->in_args_list);
            }
            if (nLen == 0 || pc->in_args == NULL) {
                pc->removed = 1;
                *cut_con = 1;
                return;
            }
            if (check_is_connected(pc->ass, name_ptr, nLen)) {
                *cut_con = 1;
                pc->removed = 1;
                return;
            }
            if (pc->in_args->sc_len > 0
                && pc->in_args->sender_comp != NULL) {
                send_name = pc->in_args->sender_comp;
            } else {
                send_name = pc->in_args->name;
            }
            
            /* We need to get our sequnces: */
            sr = create_sequence_request(name_ptr, nLen,
                                         pc->in_args,
                                         build_offsets,
                                         create_rom_resend_pad, pc);
            get_sequence_numbers(sr);
            /* Add the sequences into the connection object */
            send_seq = dsr_get_incoming(sr) + 1;//set_incoming_seq(pc->co, dsr_get_incoming(sr));
            pc->out_seq = dsr_get_outgoing(sr);
            destroy_sequence_req(sr);
            send_len = 0;
            /* This 'should' be big enough. */
            mlbk = malloc(550);
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
                        send_name, send_seq);
            if (local_sock_send(pc, mlbk, send_len)) {
                pc->is_logged_on = 1;
            send_debug_message
                ("Sent this logon{%s}\n", mlbk);
            } else {
                pc->disconnect =1;
                *cut_con = 1;
                free(mlbk);
                return;
            }
            free(mlbk);
        }
        pc->is_logged_on = 1;
        logger = calloc(len + 1, 1);
        memcpy(logger, data, len);
        pc->connection_notice(pc, 2, logger, len);
        /* Now send resend if we need to. */
        out_seq = pc->out_seq;
        if (last_rom_seq <= out_seq && out_seq > 1 && last_rom_seq > 0) {
            char *output = NULL;
            size_t out_len = 0;
            resend_req *rr = create_resend_request(name_ptr,
                                                   nLen,
                                                   last_rom_seq, 0,
                                                   find_rom_offsets);
            get_resend_request(rr);
            output = (char *) drr_get_resend_output(rr, &out_len);
            if (out_len > 0) {
                int send_ret = 1;
                if(pc->in_args == 0x0) {
                    pc->in_args =
                        get_init_args(name_ptr, nLen, pc->in_args_list);
                }
                if (pc->in_args != 0 && pc->in_args->cancel_only) {

                } else {
                    send_ret = local_sock_send(pc, output, out_len);
                }
                if (send_ret <= 0) {
                    *cut_con = 1;
                }
            }
            destroy_resend_req(rr);
        }
        pc->connection_notice(pc, 1, logger, len);
        free(logger);
    }
}

void ph_parse_rom_message(int *cut_con,
                                   struct async_parse_args *apa)
{
    sock_recv_manager* pc = apa->con;
    long len = (pc->wr_ptr - pc->rd_ptr);
    char* raw_bytes = pc->rd_ptr;
    if (len > 0 && apa->is_logged_on) {
        apa->parse_v(pc->rd_ptr, len, apa);
    } else if (len > 0 && raw_bytes[0] == 'L') {
        char *end = memchr(raw_bytes, '\n', len);
        if (end != NULL && (end - raw_bytes) <= len) {
            unsigned long log_len = (end - raw_bytes) + 1;
            send_debug_message("Logon received.%s %lu \n", raw_bytes, log_len);
            handle_async_csv_logon('L', raw_bytes, log_len, cut_con, apa);
            pc->rd_ptr += log_len;
            if (log_len < len) {
                apa->parse_v(pc->rd_ptr, len - log_len, apa);
            } 
        }
    } else if(len > 0 && !apa->is_logged_on) {
        *cut_con = 1;
        send_debug_message("sending data without being logged on, cut con. %lu \n", len);
    }
}

void ph_build_iovec(dart_order_obj * doj)
{
    long len = databuf_unread(doj->current->data);
    char *c = doj->current->data->buffer;
    char *d_off = c;
    int i = 0;
    for (; *c != '\n'&& i < ROM_LEN && (c - doj->current->data->buffer) < len; ++c) {
        if (*c == ',') {
            doj->positions[i].iov_len = c - d_off;
            doj->positions[i].iov_base = d_off;
            d_off = c + 1;
            i += 2;
        }
    }
    if (c - d_off > 0 && i < ROM_LEN) {
        doj->positions[i].iov_len = c - d_off;
        doj->positions[i].iov_base = d_off;
    }
}

#ifdef _SPREAD_N_READY_

#define ROM_SPREAD_SEP "!#!"

void ph_build_non_fix_rom_pos(char *raw_bytes, long rom_len, void *v)
{
    struct async_parse_args *apa = (struct async_parse_args *) v;
    sock_recv_manager* rph = apa->con;
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
            local_sock_send(apa, "H,\n", 3);
            rph->rd_ptr += val_end + 1;
            vend_ptr = memchr(rph->rd_ptr, '\n', len_left);
        } else {
            dart_order_obj* cur_doj = (dart_order_obj *) get_gk_obj(apa->gk);
            char* leg_off = strstr(rph->rd_ptr, ROM_SPREAD_SEP);
            if(leg_off == NULL || ((leg_off - rph->rd_ptr) > val_end)) {
                int copied = databuf_memcpy(cur_doj->current->data, rph->rd_ptr,
                                           val_end + 1);
                if(copied >= 0) {
                    ph_build_iovec(cur_doj);
                    set_rom_field(cur_doj, ROM_SENDER, apa->in_args->name,
                                  apa->in_args->name_len);
                    if (apa->in_args->name_len > 0) {
                        record_raw_message(apa->in_args->name, apa->in_args->name_len, rph->rd_ptr, val_end + 1, 0x0);
                        struct incoming_message *m1;
                        m1 = create_incoming_message(apa->in_args->name, apa->in_args->name_len, 1);
                        record_inbound_message(m1);
                    }
                    apa->obj_callback(cur_doj, apa);
                    rph->rd_ptr += val_end + 1;
                    vend_ptr = memchr(rph->rd_ptr, '\n', len_left);
                } else if(apa->in_args->name_len > 0) {
                    send_debug_message("Unable to parse this message, %lu \n", val_end);
                    record_raw_message(apa->in_args->name, apa->in_args->name_len, rph->rd_ptr, val_end + 1, 0x0);
                    rph->rd_ptr += val_end + 1;
                    vend_ptr = memchr(rph->rd_ptr, '\n', len_left);
                }
            } else {
                if (apa->in_args->name_len > 0) {
                    record_raw_message(apa->in_args->name, apa->in_args->name_len, rph->rd_ptr, val_end + 1, 0x0);
                    struct incoming_message *m1;
                    m1 = create_incoming_message(apa->in_args->name, apa->in_args->name_len, 1);
                    record_inbound_message(m1);
                }
                /*We have found the spread sep*/
                long slen_left = val_end;
                char* sprd_beg = rph->rd_ptr;
                long sprd_len = leg_off - sprd_beg;
                databuf_memcpy(cur_doj->current->data, sprd_beg,
                                           sprd_len + 1);
                ph_build_iovec(cur_doj);
                set_rom_field(cur_doj, ROM_SENDER, apa->in_args->name,
                                  apa->in_args->name_len);
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
                    dart_order_obj* child_doj = (dart_order_obj *) get_gk_obj(apa->gk);
                    databuf_memcpy(child_doj->current->data, sprd_beg,
                                           sprd_len + 1);
                    ph_build_iovec(child_doj);
                    set_rom_field(child_doj, ROM_CPLX_TYPE, "6", 1);
                    set_rom_field(child_doj, ROM_SENDER, apa->in_args->name,
                                  apa->in_args->name_len);
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
                apa->obj_callback(cur_doj, apa);
                rph->rd_ptr += val_end + 1;
                vend_ptr = memchr(rph->rd_ptr, '\n', len_left);
            }
        }
    }
}

#else

void ph_build_non_fix_rom_pos(char *raw_bytes, long rom_len, void *v)
{
    struct async_parse_args *apa = (struct async_parse_args *) v;
    sock_recv_manager* rph = apa->con;
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
            local_sock_send(apa, "H,\n", 3);
            rph->rd_ptr += val_end + 1;
            vend_ptr = memchr(rph->rd_ptr, '\n', len_left);
        } else {
            dart_order_obj* cur_doj = (dart_order_obj *) get_gk_obj(apa->gk);
            int copied = databuf_memcpy(cur_doj->current->data, rph->rd_ptr,
                                       val_end + 1);
            if(copied >= 0) {
                ph_build_iovec(cur_doj);
                set_rom_field(cur_doj, ROM_SENDER, apa->in_args->name,
                              apa->in_args->name_len);
                if (apa->in_args->name_len > 0) {
                    record_raw_message(apa->in_args->name, apa->in_args->name_len, rph->rd_ptr, val_end + 1, 0x0);
                    char mtype = rph->rd_ptr[0];
                    struct incoming_message *m1;
                    switch(mtype) {
                        case 'c':
                        case 'C':
                        case 'R':
                        case 'E':
                        case 'S':
                        m1 = create_incoming_message(apa->in_args->name, apa->in_args->name_len, 1);
                        record_inbound_message(m1);
                        break;
                        default:
                        break;
                    }
                }
                apa->obj_callback(cur_doj, apa);
                rph->rd_ptr += val_end + 1;
                vend_ptr = memchr(rph->rd_ptr, '\n', len_left);
            } else if(apa->in_args->name_len > 0) {
                send_debug_message("Unable to parse this message, %lu \n", val_end);
                record_raw_message(apa->in_args->name, apa->in_args->name_len, rph->rd_ptr, val_end + 1, 0x0);
                rph->rd_ptr += val_end + 1;
                vend_ptr = memchr(rph->rd_ptr, '\n', len_left);
            }
        }
    }
}
#endif
