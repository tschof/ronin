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
#include <unistd.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "rom_handler.h"
#include "client_manager.h"
#include "parser_func.h"
#include "transaction_recorder.h"
#include "dart_seq_request.h"
#include "debug_logger.h"

static int local_sock_send(struct parse_con *pc, char *data,
                           unsigned long len)
{
    long sent_bytes;
    long bytes_to_send;
    char *d_off = data;
    long ret_code = 1;
    int failed_attempts = 0;
    sent_bytes = 0;
    bytes_to_send = (long) len;
    while (sent_bytes < len && ret_code >= 0) {
        ret_code = dart_send_message(pc->co, d_off, bytes_to_send);
        if (ret_code < 0) {
            ret_code = 0;
            ++failed_attempts;
            if (failed_attempts >= 128 || get_disconnect(pc->co)) {
                return 0;
            }
        }
        bytes_to_send -= ret_code;
        sent_bytes += ret_code;
        d_off += ret_code;
    }
    return 1;
}

static void send_logout(struct parse_con* pc, char* logout_reason)
{
    char* c_off = calloc(1, 128);
    int len = sprintf(c_off, "K,,,,,,,,,,,,,,,,,,,,,,,,,,%s,,,,,,,,,,,,,,,,,,,,,,\n", logout_reason);
    if(len > 0) {
        local_sock_send(pc, c_off, (unsigned long)len);
    }
    free(c_off);
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

/**
 * This needs to be altered so that we check username and password.
 */
static struct init_args *get_init_args(char *name, int name_len,
                                       char *pass, int pass_len,
                                       queue_t in_args)
{
    if (name != NULL && name_len > 0 && in_args != NULL) {
        element_t e;
        e = front(in_args);
        while (e != NULL) {
            struct init_args *init;
            init = (struct init_args *) e->d;
            if (init != NULL && name_len == init->name_len &&
                    pass_len == init->p_len &&
                (strncmp(name, init->name, name_len) == 0 &&
                 strncmp(pass, init->password, pass_len) == 0)) {
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

static char* get_password(const char* data, long dlen, int* len, int offset)
{
    char* d_off = (char*)data;
    char* c = d_off;
    int i = 0;
    for(; *c != '\n' && c - data < dlen; ++c) {
        if(*c == ',' && i == offset) {
            *len = c - d_off;
            return d_off;
        } else if(*c == ',') {
            d_off = c +1;
            ++i;
        }
    }
    if(c - d_off > 0) {
            *len = c - d_off;
            return d_off;
    }
    return NULL;
}


void handle_csv_logon(char type, const char *data, unsigned long len,
                      int *cut_con, struct parse_con *pc)
{
    if (type == 'L') {
        unsigned long cur_pos;
        int i;
        long ret_code;
        long nLen;
        char *ret_val = 0;
        char *d_off;
        char *name_ptr;
        int l_name_len = 0;
        long end;
        unsigned long last_rom_seq = 0;
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
                set_should_notify(pc->co, 0);
                return;
            }
        }
        ret_code = find_offset(d_off, (len - cur_pos), comma, 1);
        if (ret_code <= 0 && !pc->is_server) {
            char *l_name = (char *) get_name(pc->co);
            l_name_len = get_name_len(pc->co);
            name_ptr = calloc(l_name_len + 1, 1);
            if (name_ptr != NULL) {
                memcpy(name_ptr, l_name, l_name_len);
                nLen = l_name_len;
            } else {
                nLen = 0;
            }
            if (l_name) {
                free(l_name);
            }
        } else if (ret_code > 0) {
            /*Here we get the connection name */
            name_ptr = calloc(ret_code + 1, 1);
            l_name_len = ret_code + 1;
            if (name_ptr != NULL) {
                memcpy(name_ptr, d_off, ret_code);
                nLen = ret_code;
            } else {
                nLen = 0;
            }
        } else {
            *cut_con = 1;
            set_should_notify(pc->co, 0);
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
            int pass_len = 0;
            sequence_req *sr;
            char *mlbk;
            char *mlbk_off;
            char *send_name = 0;
            int time_len;
            unsigned long send_len;
            unsigned long send_seq;
            if (nLen > 0) {
                set_name(pc->co, name_ptr, nLen);
                char* pass = get_password(data, len, &pass_len, 7);
                if(pass_len >= 0) {
                /* Here we need to cut the connection if it does not
                 * conform with any of our init structs. */
                pc->in_args =
                    get_init_args(name_ptr, nLen, pass, pass_len, pc->in_args_list);
                }
            }
            if (pass_len == 0 || nLen == 0 || pc->in_args == NULL) {
                send_logout(pc, "Invalid Password");
                *cut_con = 1;
                set_should_notify(pc->co, 0);
                return;
            }
            if (check_is_connected(pc->co)) {
                send_logout(pc, "User is already connected.");
                *cut_con = 1;
                set_should_notify(pc->co, 0);
                return;
            }
            if (pc->in_args->sc_len > 0
                && pc->in_args->sender_comp != NULL) {
                send_name = pc->in_args->sender_comp;
            } else {
                send_name = pc->in_args->name;
            }
            if(last_rom_seq > 0) {
                /* We need to get our sequnces: */
                printf("Creating request: %s \n", name_ptr);
                sr = create_sequence_request(name_ptr, nLen,
                                             pc->in_args,
                                             build_offsets,
                                             create_rom_resend_pad);
                
                get_sequence_numbers(sr);
                printf("Got Sequences: %lu \n", dsr_get_incoming(sr));
                /* Add the sequences into the connection object */
                set_incoming_seq(pc->co, dsr_get_incoming(sr));
                set_outgoing_seq(pc->co, dsr_get_outgoing(sr));
                printf("Got Sequences: %lu \n", dsr_get_incoming(sr));
                destroy_sequence_req(sr);
                pc->in_args->reset_time->new_incoming = 0;
                pc->in_args->reset_time->new_outgoing = 0;
                send_seq = get_incoming_seq(pc->co) + 1;
            } else {
                send_seq = 1;
                set_incoming_seq(pc->co,1);
                set_outgoing_seq(pc->co,1);
            }
            send_len = 0;
            pc->in_args->reset_time->create_new = 0;
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
                    pc->init_con(pc->co);
                    pc->is_logged_on = 1;
                } else {
                    pc->is_logged_on = 0;
                    set_disconnect(pc->co, 1);
                    *cut_con = 1;
                    free(mlbk);
                    return;
                }
                free(mlbk);
        }
        pc->is_logged_on = 1;
        pc->connection_notice(pc->co, 2, NULL, 0);
        /* Now send resend if we need to. */
        out_seq = get_outgoing_seq(pc->co);
        if (last_rom_seq <= out_seq && last_rom_seq > 0) {
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
        logger = calloc(len + 1, 1);
        memcpy(logger, data, len);
        pc->connection_notice(pc->co, 1, logger, len);
        if (name_ptr) {
            free(name_ptr);
        }
        free(logger);
    }
}

int gen_rom_logon(struct parse_con *con, int seq_enabled)
{
    int ret_val = 1;
    sequence_req *sr;
    char *mlbk;
    char *mlbk_off;
    int time_len;
    unsigned long send_len;
    unsigned long send_seq;
    int l_name_len = get_name_len(con->co);
    char *l_name = (char *) get_name(con->co);
    sr = create_sequence_request(l_name, l_name_len,
                                 con->in_args
                                 , build_offsets,
                                 create_rom_resend_pad);
    get_sequence_numbers(sr);
    /* Add the sequences into the connection object */
    set_incoming_seq(con->co, dsr_get_incoming(sr));
    set_outgoing_seq(con->co, dsr_get_outgoing(sr));
    destroy_sequence_req(sr);
    con->in_args->reset_time->new_incoming = 0;
    con->in_args->reset_time->new_outgoing = 0;
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
            send_seq = get_incoming_seq(con->co) + 1;
            send_len +=
                sprintf(mlbk_off,
                        "%s,,,,,%s,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,%lu,,\n",
                        con->in_args->username, con->in_args->password,
                        send_seq);
        } else {
            send_len +=
                sprintf(mlbk_off,
                        "%s,,,,,%s,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n",
                        con->in_args->username, con->in_args->password);
        }
        if (local_sock_send(con, mlbk, send_len)) {
            con->init_con(con->co);
        } else {
            set_disconnect(con->co, 1);
            ret_val = 0;
        }
        free(mlbk);
    }
    if (l_name) {
        free(l_name);
    }
    return ret_val;
}

int init_rom_logon(struct parse_args *pa)
{
    return gen_rom_logon(pa->con, 1);
}

int init_rom_logon_no_seq(struct parse_args *pa)
{
    return gen_rom_logon(pa->con, 0);
}

int init_async_rom_logon(struct async_parse_args *pa)
{
    return gen_rom_logon(pa->con, 1);
}

unsigned long build_offsets(char *data, unsigned long size,
                            unsigned long byte_offset,
                            unsigned long *seq_num)
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
        ret_code = find_offset(d_off, size, eol, 1);
        while (ret_code >= 0) {
            ++cur_seq;
            if (beg_seq == cur_seq) {
                *beg_off = (byte_offset + read_bytes);
            }
            read_bytes += (size_t) ret_code;
            read_bytes += 1;
            d_off += (ret_code + 1);
            if (end_seq == cur_seq) {
                *end_off = (byte_offset + read_bytes);
            }
            ret_code = find_offset(d_off, (size - read_bytes), eol, 1);
        }
        *seq = cur_seq;
        return read_bytes;
    }
    return size;
}
static void check_password_change_mess(const char* data, unsigned long len,
                                    int* cut_con, struct parse_con * pc)
{
    int pass_len = 0;
    int nLen = 0;
    char* name_ptr = get_password(data, len, &nLen, 2);
    char* pass = get_password(data, len, &pass_len, 7);
    if(nLen > 0 && pass_len > 0) {
        /* Here we need to cut the connection if it does not
         * conform with any of our init structs. */
        pc->in_args =
            get_init_args(name_ptr, nLen, pass, pass_len, pc->in_args_list);
        if(pc->in_args == NULL) {
            *cut_con = 1;
        } else {
            set_name(pc->co, name_ptr, nLen);
            pc->init_con(pc->co);
            char* logger = calloc(len + 1, 1);
            memcpy(logger, data, len);
            pc->connection_notice(pc->co, 1, logger, len);
            free(logger);
        }
    } else {
        *cut_con = 1;
    }
}


unsigned long parse_rom_message(const char *raw_bytes, unsigned long len,
                                int *cut_con, struct parse_con *pc)
{
    unsigned long read_bytes = 0;
    if (len > 0) {
        char *d_off;
        long ret_code;
        char *ret_val;
        read_bytes = 0;
        ret_code = 0;
        d_off = (char *) raw_bytes;
        ret_val = memchr(d_off, '\n', len);
        while (ret_val) {
            char type;
            ret_code = (ret_val - d_off);
            read_bytes += 1 + (unsigned long) ret_code;
            if (read_bytes > len) {
                unsigned long ret_bytes = read_bytes - (ret_code + 1);
                return (ret_bytes < len ? ret_bytes : len);
            }
            type = d_off[0];
            if (type == 'L') {
                if(pc->is_logged_on != 1) {
                    handle_csv_logon(type, d_off, ret_code + 1, cut_con, pc);
                } else {
                    set_disconnect(pc->co, 1);
                    *cut_con = 1;
                }
            } else if(type == 'p') {
                check_password_change_mess(d_off, len, cut_con, pc);
                read_bytes = len;
            } else if (type == 'O') {
                *cut_con = 1;
                return 0;
            } else if (type == 'A') {
                char *name = (char *) get_name(pc->co);
                struct incoming_message *m1;
                m1 = create_incoming_message(name,
                                             get_name_len(pc->co), 1);

                record_inbound_message(m1);
                if (name) {
                    free(name);
                }
            } else if (pc->is_logged_on) {
                char *name = (char *) get_name(pc->co);
                struct message m;
                m.name = name;
                m.name_len = get_name_len(pc->co);
                m.len = ret_code + 1;
                m.data = d_off;
                pc->callback(&m, pc->book);
                if (type == 'S' || type == 'E' || type == 'R'
                    || type == 'C' || type == 'c' || type == 'P' 
                    || type == 'V') {
                    struct incoming_message *m1;
                    m1 = create_incoming_message(name,
                                                 get_name_len(pc->co), 1);
                    record_inbound_message(m1);
                }
                if (name) {
                    free(name);
                }
            } else {
                *cut_con = 1;
                return 0;
            }
            d_off = d_off + ret_code + 1;
            if (read_bytes < len) {
                ret_val = memchr(d_off, '\n', (len - read_bytes));
            } else {
                return (read_bytes == len ? read_bytes : len);
            }
        }
    } else {
        send_debug_message("Len is < 0 %lu \n", len);
        return 0;
    }
    return read_bytes;
}

/*static struct fix_tag_def **get_ftd_for_type(orderbook * ob, char type)
{
    switch (type) {
    case 'E':
        return ob->rom_to_fix_out;
    case 'C':
        return ob->rom_to_fix_cnl;
    case 'R':
        return ob->rom_to_fix_rpl;
    case 'S':
        return ob->rom_to_fix_status;
    default:
        return NULL;
    };
}*/

unsigned long ph_parse_rom_message(const char *raw_bytes,
                                   unsigned long len, int *cut_con,
                                   struct parse_con *pc)
{
    rom_parser_helper *rph = (rom_parser_helper *) pc->parser_helper;
    if (len > 0 && (raw_bytes[0] != 'L')) {
        rph->read_bytes = 0;
        rph->message_beg = (char *) raw_bytes;
        rph->d_off = (char *) raw_bytes;
        if (rph->mess_bytes > 0) {
            rph->d_off += rph->mess_bytes;
            rph->read_bytes += rph->mess_bytes;
        }
        rph->parse_v(len, rph, pc);
    } else if (raw_bytes[0] == 'L' ) {
        char *end = memchr(raw_bytes, '\n', len);
        if (end != NULL && (end - raw_bytes) <= len) {
            unsigned long log_len = (end - raw_bytes) + 1;
            if(pc->is_logged_on != 1) {
                handle_csv_logon('L', raw_bytes, log_len, cut_con, pc);
            } else {
                set_disconnect(pc->co, 1);
                *cut_con = 1;
            }
            if (log_len < len) {
                rph->read_bytes = log_len;
                rph->message_beg = (char *) raw_bytes + log_len;
                rph->d_off = (char *) raw_bytes + log_len;
                rph->parse_v(len - log_len, rph, pc);
            } else {
                rph->read_bytes = log_len;
            }
        }
    } else {
        send_debug_message("Len is < 0 %lu \n", len);
    }
    if(rph->mess_bytes > 0) {
        return rph->message_beg - raw_bytes;
    } else {
        return rph->read_bytes;
    }
}

void ph_build_dart_obj(long rom_len,
                              struct rom_parser_helper *rph, void *v)
{
    struct parse_con *pc = (struct parse_con *) v;
    long val_end = 0;
    long l_len  = 0;
    for(;rph->read_bytes < rom_len; ++rph->d_off, ++rph->read_bytes) {
        if (*rph->d_off == ',') {
            val_end = (rph->d_off - rph->message_beg);
            l_len = (val_end - rph->mess_bytes);
            rph->cur_doj->positions[rph->rom_tag].beg =
                rph->mess_bytes;
            rph->cur_doj->positions[rph->rom_tag].end =
                val_end;
            rph->cur_doj->positions[rph->rom_tag].val_len = l_len;
            ++rph->rom_tag;
            rph->mess_bytes += l_len + 1;
        } else if (*rph->d_off == '\n') {
            char *name = (char *) get_name(pc->co);
            struct message m;
            m.name = name;
            m.name_len = get_name_len(pc->co);
            m.len = rph->mess_bytes;
            m.data = rph->message_beg;
            databuf_memcpy(rph->cur_doj->data, rph->message_beg,
                           rph->mess_bytes);
            pc->p_callback(rph->cur_doj, &m, pc);

            rph->message_beg = rph->d_off + 1;
            rph->mess_bytes = 0;
            rph->rom_tag = 0;
            rph->cur_doj = (dart_order_obj *)
                get_obj(((orderbook *) pc->book)->ord_obj);
            free(name);
        }
    } 
}

