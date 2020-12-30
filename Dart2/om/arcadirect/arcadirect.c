/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

#include "message_queue.h"
#include "parser_func.h"
#include "dart_constants.h"
#include "config_loader.h"
#include "ex_config_helper.h"
#include "rom_handler.h"
#include "connection_interface.h"
#include "mailer.h"
#include "ad_parser.h"
#include "debug_logger.h"

#define MICRO_HOUR 3600000000
#define MICRO_MIN 60000000
#define MICRO_SEC 1000000

void parse_addirect(int *cut_con, struct async_parse_args *pc);

static int create_ad_clordid_offset()
{
    time_t rawtime;
    struct tm *ti;
    struct timeval tv;
    int ret_code = 0;
    long long dart_sec = 0;
    long long off_time = 0;
    struct tm lm;
    ret_code = gettimeofday(&tv, NULL);
    time(&rawtime);
    ti = gmtime_r(&rawtime, &lm);
    ti = &lm;
    dart_sec =
        (ti->tm_hour * MICRO_HOUR) + (ti->tm_min * MICRO_MIN) +
        (ti->tm_sec * MICRO_SEC);
    off_time = tv.tv_usec - dart_sec;
    ret_code = (int) tv.tv_usec - off_time;
    return ret_code;
}

static void ad_rec_in(char *name, int name_len)
{
    struct incoming_message *m1;
    m1 = create_incoming_message(name, name_len, 1);
    record_inbound_message(m1);
}

static void send_hb(struct async_parse_args *con)
{
    con_interface *ob = (con_interface *) con->ds_book;
    databuf_t *buff = get_databuf(ob->gk);
    memset(buff->buffer, '\0', 12);
    unsigned char var = 0x01;
    short ml = htons(12);
    *buff->wr_ptr++ = '0';
    databuf_memcpy(buff, &var, 1);
    databuf_memcpy(buff, &ml, 2);
    buff->wr_ptr = buff->buffer + 11;
    *buff->wr_ptr++ = '\n';
    ci_async_send(ob->fix_con_name, ob->fix_con_len, buff, ob);
    destroy_gk_db(ob->gk, buff);
}

int send_ad_logon(struct async_parse_args *pa)
{
    con_interface *ob = (con_interface *) pa->ds_book;
    ob->seq = 1;
    ob->ap = pa;
    ob->sock = pa->read_wrapper->sock;
    //struct async_parser *con = pa->con;
    sequence_req *sr = create_sequence_request(pa->in_args->name, pa->in_args->name_len,
                                               pa->in_args,
                                               build_offsets,
                                               create_rom_resend_pad, ob);
    get_sequence_numbers(sr);
    /* Add the sequences into the connection object */
    pa->in_seq = dsr_get_incoming(sr);
    destroy_sequence_req(sr);
    databuf_t *logon = databuf_alloc(49);
    memset(logon->buffer, '\0', 49);
    ob->seq = pa->in_seq;
    ob->seq += create_ad_clordid_offset();
    int test_in = pa->in_seq;
    send_debug_message("%d, incoming messages recorded.\n", test_in);
    unsigned char var = 0x01;
    short len = htons(48);
    int in = 0;
    if (test_in == 0) {
        in = htonl(-1);
    } else {
        in = htonl(test_in);
    }
    *logon->wr_ptr++ = 'A';
    databuf_memcpy(logon, &var, 1);
    databuf_memcpy(logon, &len, 2);
    *logon->wr_ptr++ = '\0';
    *logon->wr_ptr++ = '\0';
    *logon->wr_ptr++ = '\0';
    *logon->wr_ptr++ = '\0';
    databuf_memcpy(logon, &in, 4);
    databuf_memcpy(logon, pa->in_args->username, pa->in_args->un_len);
    logon->wr_ptr = logon->buffer + 46;
    databuf_memcpy(logon, &var, 1);
    *logon->wr_ptr++ = '\n';
    int ret =
        ci_async_send(ob->fix_con_name, ob->fix_con_len, logon, ob);
    databuf_destroy(logon);
    return ret;
}

int get_obj_by_id(con_interface * ob, void *clordid, int len,
                  dart_order_obj ** obj)
{
    int ret = 0;
    pthread_spin_lock(&ob->mutex);
    ret = get_map_entry(ob->cl_ord_id_map, clordid, len, (void **) obj);
    pthread_spin_unlock(&ob->mutex);
    return ret;
}

int build_ad_offset(struct async_parse_args *apa)
{
    return 0;
}

int on_ad_enter_order(con_interface * ci, dart_order_obj * doj,
                      struct async_parse_args *ap)
{
    int total_len = 0;
    databuf_t *buff = (databuf_t *) get_databuf(ci->gk);
    set_rom_field(doj, ROM_ON_BEHALF, ci->session_conf->sender_comp,
                  ci->session_conf->sc_len);
    memset(buff->buffer, '\0', 256);
    total_len = create_new_order_message(buff, doj, ci->seq, ci);
    if (total_len > 0) {
        int b_len = databuf_unread(buff);
        total_len = ci_async_send(ci->fix_con_name, ci->fix_con_len, buff, ci);
        if(total_len > 0) {
            buff->rd_ptr -= total_len;
        }
        if(b_len != total_len) {
            printf("Lens diff: %d, total: %d \n", b_len, total_len); 
        }
        record_raw_message(ci->fix_con_name, ci->fix_con_len, buff->rd_ptr,
                           databuf_unread(buff),0x0);
    }
    destroy_gk_db(ci->gk, buff);
    return 1;
}

int on_ad_cancel_order(con_interface * ci, dart_order_obj * doj)
{
    int total_len = 0;
    databuf_t *buff = (databuf_t *) get_databuf(ci->gk);
    memset(buff->buffer, '\0', 256);
    total_len = create_cancel_message(buff, doj, ci->seq, ci);
    if (total_len > 0) {
        total_len = ci_async_send(ci->fix_con_name, ci->fix_con_len, buff, ci);
        if(total_len > 0) {
            buff->rd_ptr -= total_len;
        }
        record_raw_message(ci->fix_con_name, ci->fix_con_len, buff->rd_ptr,
                           databuf_unread(buff), 0x0);
    }
    destroy_gk_db(ci->gk, buff);
    return 1;
}

void on_ad_replace_order(con_interface * ci, dart_order_obj * doj)
{
    int total_len = 0;
    databuf_t *buff = (databuf_t *) get_databuf(ci->gk);
    memset(buff->buffer, '\0', 256);
    total_len = create_replace_message(buff, doj, ci->seq, ci);
    if (total_len > 0) {
        total_len = ci_async_send(ci->fix_con_name, ci->fix_con_len, buff, ci);
        if(total_len > 0) {
            buff->rd_ptr -= total_len;
        }
        record_raw_message(ci->fix_con_name, ci->fix_con_len, buff->rd_ptr,
                           databuf_unread(buff), 0x0);
    }
    destroy_gk_db(ci->gk, buff);
}

void create_connection(con_interface * con, char *filename, int name_len,
                       gatekeeper * gk)
{

    ex_config *ex = init_config(filename, name_len);
    struct init_args *fix_args = load_fix_config(filename, name_len);
    reset_con_interface(con, 8192,
                        (char *) ex->mod_name, ex->mod_name_len,
                        fix_args->target_comp, fix_args->tc_len,
                        send_ad_logon, build_ad_offset,
                        on_ad_enter_order, on_ad_cancel_order,
                        on_ad_replace_order, gk, (char *) ex->dest_id,
                        ex->dest_id_len);
    con->client_mess_parse = parse_addirect;
    con->connection_conf = ex;
    con->can_send_orders = 0;
    con->session_conf = fix_args;
    con->tg = 0;
    add_int_orderids(con);
}

void build_fill_not_found_mail(con_interface * ob, char *d_off,
                               databuf_t * buff)
{
    int last_shares = 0;
    int last_price = 0;
    double rom_price = 0;
    char scale = '4';
    memcpy(&last_shares, d_off + 32, 4);
    last_shares = ntohl(last_shares);
    memcpy(&last_price, d_off + 36, 4);
    last_price = ntohl(last_price);
    memcpy(&scale, d_off + 40, 1);
    rom_price = ad_to_rom_price(last_price, scale);
    char *title = "Unmatched fill from ARCA DIRECT";
    int t_len = 35;
    char *side = 0;
    if (d_off[30] == '1') {
        side = "Buy";
    } else {
        side = "Sell";
    }
    databuf_write(buff, "%s of %d shares @ %f.\n", side, last_shares,
                  rom_price);
    unsigned long order_id = 0;
    memcpy(&order_id, d_off + 16, 8);
    unsigned long exec_id = 0;
    memcpy(&exec_id, d_off + 24, 8);
    exec_id = ntohl(exec_id);
    int clordid = 0;
    memcpy(&clordid, d_off + 12, 4);
    clordid = ntohl(clordid);
    databuf_write(buff, "\nExec ID:%lu \nCL Ord ID: %d\nArca ID: %lu\n",
                  exec_id, clordid, order_id);
    databuf_write(buff, "Billing indicator: %c\n", d_off[33]);
    send_biz_message(buff->buffer, databuf_unread(buff), title, t_len);
}

void build_bust(con_interface * ob, char *d_off, databuf_t * buff)
{
    int last_shares = 0;
    int last_price = 0;
    double rom_price = 0;
    char scale = '4';
    char *title = 0;
    int t_len = 0;
    memcpy(&last_shares, d_off + 24, 4);
    last_shares = ntohl(last_shares);
    memcpy(&last_price, d_off + 28, 4);
    last_price = ntohl(last_price);
    memcpy(&scale, d_off + 32, 1);
    rom_price = ad_to_rom_price(last_price, scale);
    if (d_off[34] == '1') {
        title = "Bust from ARCA DIRECT";
        t_len = 25;
    } else {
        title = "Correction from ARCA DIRECT";
        t_len = 31;
    }
    databuf_write(buff, "%d shares @ %f.\n", last_shares, rom_price);
    unsigned long exec_id = 0;
    memcpy(&exec_id, d_off + 16, 8);
    exec_id = ntohl(exec_id);
    int clordid = 0;
    memcpy(&clordid, d_off + 12, 4);
    clordid = ntohl(clordid);
    databuf_write(buff, "\nExec ID:%lu \nCL Ord ID: %d\n", exec_id,
                  clordid);
    send_biz_message(buff->buffer, databuf_unread(buff), title, t_len);
}

void parse_addirect(int *cut_con, struct async_parse_args *pc)
{
    short ord_type = 0;
    unsigned long bytes_used = 0;
    int should_parse = 1;
    int add_seq = 0;
    //struct message mess;
    con_interface *ob = (con_interface *) pc->ds_book;
    char* d_off = pc->con->rd_ptr;
    long len = (pc->con->wr_ptr - pc->con->rd_ptr);
    long len_left = (long) len;
    dart_order_obj *obj = 0;
    while (should_parse && bytes_used < len) {
        switch (d_off[0]) {
        case 'a':
            if ((len_left = (len_left - ACK_V1)) >= 0) {
                add_seq = 1;
                int id = 0;
                memcpy(&id, d_off + 24, 4);
                id = ntohl(id);
                int found = get_obj_by_id(ob, &id, 4, &obj);
                if (found && lock_n_check(obj)) {
                    parse_order_ack(obj, d_off, ob);
                    ob->ci_obj_callback(obj, ob);
                    unlock(obj);
                }
                record_raw_message(ob->fix_con_name, ob->fix_con_len,
                                   d_off, ACK_V1,0x0);
                d_off += ACK_V1;
                bytes_used += ACK_V1;
            } else {
                should_parse = 0;
            }
            break;
        case '6':
            if ((len_left = (len_left - CNL_ACK_V1)) >= 0) {
                add_seq = 1;
                record_raw_message(ob->fix_con_name, ob->fix_con_len,
                                   d_off, CNL_ACK_V1, 0x0);
                d_off += CNL_ACK_V1;
                bytes_used += CNL_ACK_V1;
            } else {
                should_parse = 0;
            }
            break;
        case '4':
            if ((len_left = (len_left - KILLED_V1)) >= 0) {
                add_seq = 1;
                int id = 0;
                memcpy(&id, d_off + 24, 4);
                id = ntohl(id);
                int found = get_obj_by_id(ob, &id, 4, &obj);
                if (found && lock_n_check(obj)) {
                    parse_ur_out(obj, d_off, ob);
                    ob->on_out(ob, obj);
                    unlock(obj);
                    destroy_gk_obj(ob->gk, obj);
                }
                record_raw_message(ob->fix_con_name, ob->fix_con_len,
                                   d_off, KILLED_V1, 0x0);
                d_off += KILLED_V1;
                bytes_used += KILLED_V1;
            } else {
                should_parse = 0;
            }
            break;
        case 'E':
            if ((len_left = (len_left - CNL_RPL_ACK_V1)) >= 0) {
                add_seq = 1;
                int id = 0;
                memcpy(&id, d_off + 24, 4);
                id = ntohl(id);
                int found = get_obj_by_id(ob, &id, 4, &obj);
                if (found && lock_n_check(obj)) {
                    parse_cancel_rep(obj, d_off, ob);
                    ob->ci_obj_callback(obj, ob);
                    unlock(obj);
                }
                record_raw_message(ob->fix_con_name, ob->fix_con_len,
                                   d_off, CNL_RPL_ACK_V1, 0x0);
                d_off += CNL_RPL_ACK_V1;
                bytes_used += CNL_RPL_ACK_V1;
            } else {
                should_parse = 0;
            }
            break;
        case '5':
            if ((len_left = (len_left - RPL_V1)) >= 0) {
                add_seq = 1;
                int id = 0;
                memcpy(&id, d_off + 24, 4);
                id = ntohl(id);
                int found = get_obj_by_id(ob, &id, 4, &obj);
                if (found && lock_n_check(obj)
                    && lock_n_check((dart_order_obj *) obj->rep)) {
                    parse_replaced(obj, d_off, ob);
                    ob->on_replaced(ob, obj);
                    unlock((dart_order_obj *) obj->rep);
                    destroy_gk_obj(ob->gk, obj->rep);
                    obj->rep = 0;
                    unlock(obj);
                }
                record_raw_message(ob->fix_con_name, ob->fix_con_len,
                                   d_off, RPL_V1, 0x0);
                d_off += RPL_V1;
                bytes_used += RPL_V1;
            } else {
                should_parse = 0;
            }
            break;
        case '8':
            if ((len_left = (len_left - CNL_RPL_REJ_V1)) >= 0) {
                add_seq = 1;
                int id = 0;
                memcpy(&id, d_off + 24, 4);
                id = ntohl(id);
                int found = get_obj_by_id(ob, &id, 4, &obj);
                if (!found) {
                    memcpy(&id, d_off + 28, 4);
                    id = ntohl(id);
                    found = get_obj_by_id(ob, &id, 4, &obj);
                }
                if (found && lock_n_check(obj)) {
                    int ret = parse_rej(obj, d_off, ob);
                    if (ret == 3) {
                        lock_n_check((dart_order_obj *) obj->rep);
                    }
                    ob->on_reject(ob, obj);
                    if (ret == 3) {
                        unlock((dart_order_obj *) obj->rep);
                        destroy_gk_obj(ob->gk, obj->rep);
                    } else if (ret == 1) {
                        unlock(obj);
                        destroy_gk_obj(ob->gk, obj);
                    } else {
                        unlock(obj);
                    }
                }
                record_raw_message(ob->fix_con_name, ob->fix_con_len,
                                   d_off, CNL_RPL_REJ_V1,0x0);
                d_off += CNL_RPL_REJ_V1;
                bytes_used += CNL_RPL_REJ_V1;
            } else {
                should_parse = 0;
            }
            break;
        case '2':
            if ((len_left = (len_left - FILL_V1)) >= 0) {
                add_seq = 1;
                short length = 0;
                memcpy(&length, d_off + 2, 2);
                length = ntohs(length);
                int id = 0;
                memcpy(&id, d_off + 24, 4);
                id = ntohl(id);
                int found = get_obj_by_id(ob, &id, 4, &obj);
                if (found && lock_n_check(obj)) {
                    int clean = 0;
                    if (length == FILL_V1) {
                        clean = parse_filled(obj, d_off, ob);
                    } else {
                        clean = parse_ex_filled(obj, d_off, ob);
                    }
                    ob->on_fill(ob, obj);
                    unlock(obj);
                    if (clean) {
                        destroy_gk_obj(ob->gk, obj);
                    }
                } else {
                    databuf_t *buff = get_databuf(ob->gk);
                    build_fill_not_found_mail(ob, d_off, buff);
                    destroy_gk_db(ob->gk, buff);
                }
                record_raw_message(ob->fix_con_name, ob->fix_con_len,
                                   d_off, length, 0x0);
                d_off += length;
                bytes_used += length;
            } else {
                should_parse = 0;
            }
            break;
        case 'C':
            if ((len_left = (len_left - BUST_COR_V1)) >= 0) {
                databuf_t *buff = get_databuf(ob->gk);
                build_bust(ob, d_off, buff);
                destroy_gk_db(ob->gk, buff);
                record_raw_message(ob->fix_con_name, ob->fix_con_len,
                                   d_off, BUST_COR_V1, 0x0);
                d_off += BUST_COR_V1;
                bytes_used += BUST_COR_V1;
            } else {
                should_parse = 0;
            }
            break;
        case '1':
            bytes_used += 12;
            send_hb(pc);
            break;
        case '0':
            bytes_used += 12;
            break;
        case 'L':
            memcpy(&ord_type, d_off + 2, 2);
            ord_type = ntohs(ord_type);
            record_raw_message(ob->fix_con_name, ob->fix_con_len, d_off,
                               60, 0x0);
            bytes_used += 60;
            should_parse = 0;
            send_debug_message("Logon Rejected: %s \n", d_off + 18);
            ob->can_send_orders = 0;
            *cut_con = 1;
            break;
        case 'A':
            record_raw_message(ob->fix_con_name, ob->fix_con_len, d_off,
                               48, 0x0);
            bytes_used += 48;
            ob->can_send_orders = 1;
            pc->connection_notice(pc, 1, NULL, 0);
            break;
        default:
            bytes_used = len;
            should_parse = 0;
            break;
        }
        if (add_seq) {
            add_seq = 0;
            ad_rec_in(ob->fix_con_name, ob->fix_con_len);
        }
    }
    pc->con->rd_ptr += bytes_used;
}
