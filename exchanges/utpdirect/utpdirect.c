
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

#include "client_manager.h"
#include "message_queue.h"
#include "parser_func.h"
#include "dart_constants.h"
#include "config_loader.h"
#include "ex_common.h"
#include "ex_config_helper.h"
#include "rom_handler.h"
#include "rom2fix.h"
#include "mailer.h"
#include "nyse_seq_generator.h"
#include "utp_object.h"
#include "debug_logger.h"

static int utp_send_message(orderbook * ob, char *message, size_t len,
                            char *name, int name_len)
{
    long len_sent = 0;
    long ret_val = 0;
    int failed_attempts = 0;
    char *mess_off = message;
    long t_len = (long) len;
    if (pthread_mutex_lock(&ob->send_mutex) == 0) {
        while (len_sent < t_len && len_sent >= 0 && failed_attempts < 40) {
            ret_val = linux_only_dart_send(name, name_len, mess_off, len);
            if (ret_val < 0) {
                ++failed_attempts;
                ret_val = 0;
            }
            mess_off += ret_val;
            len -= ret_val;
            len_sent += ret_val;
        }
        if (pthread_mutex_unlock(&ob->send_mutex) != 0) {
        }
    }
    return len_sent;
}

static int utp_send_iovec(orderbook * ob, struct iovec *iov, int iovcnt,
                          const char *name, int name_len)
{
    int err = 0;
    int vec_sent = 0;
    int sock = get_sock_for_con(name, name_len);
    if (pthread_mutex_lock(&ob->send_mutex) == 0) {
        if (sock >= 0) {
            while (err == 0) {
                send_bunches(iov, iovcnt, sock, &err, &vec_sent);
            }
            if (err < 0) {
                send_debug_message("Kicking off this connection: %s \n",
                                   name);
                kick_off_connection((char *) name, name_len);
            }
        }
        if (pthread_mutex_unlock(&ob->send_mutex) != 0) {
        }
    }
    return err;
}

static void utp_send_wrapper(orderbook * ob, databuf_t * buff,
                             dart_order_obj * doj, const char *name,
                             int name_len)
{
    struct iovec iov[8];
    iov[0].iov_base = "S,";
    iov[0].iov_len = 2;
    iov[1].iov_base = doj->data->buffer + doj->positions[1].beg;
    iov[1].iov_len = ((doj->positions[4].end) - doj->positions[1].beg);
    iov[2].iov_base = ",,";
    iov[2].iov_len = 2;
    iov[3].iov_base = doj->data->buffer + doj->positions[6].beg;
    iov[3].iov_len = (doj->positions[6].end - doj->positions[6].beg);
    iov[4].iov_base = ",";
    iov[4].iov_len = 1;
    iov[5].iov_base = doj->data->buffer + doj->positions[7].beg;
    iov[5].iov_len = (doj->positions[7].end - doj->positions[7].beg);
    iov[6].iov_base = ",,,,,,,";
    iov[6].iov_len = 7;
    iov[7].iov_base = buff->buffer;
    iov[7].iov_len = databuf_unread(buff);
    struct iovec *t = iov;
    utp_send_iovec(ob, t, 8, name, name_len);
    dart_rolling_log_going_iovec(iov, 8, name, name_len, ob->module_name,
                                 ob->module_name_len);
}

static void utp_rec_in(char *name, int name_len)
{
    struct incoming_message *m1;
    m1 = create_incoming_message(name, name_len, 1);
    record_inbound_message(m1);
}

static void send_hb(struct parse_con *con)
{
    orderbook *ob = (orderbook *) con->book;
    databuf_t *buff = (databuf_t *) get_obj(ob->databufs);
    short mt = htons(0x001);
    short ml = htons(8);
    databuf_memcpy(buff, &mt, 2);
    databuf_memcpy(buff, &ml, 2);
    *buff->wr_ptr++ = '\0';
    *buff->wr_ptr++ = '\0';
    *buff->wr_ptr++ = '\0';
    *buff->wr_ptr++ = '\0';
    utp_send_message(ob, buff->buffer, databuf_unread(buff),
                     ob->fix_con_name, ob->fix_con_len);
    destroy_elem(ob->databufs, buff);
}

int send_utp_logon(struct parse_args *pa)
{
    orderbook *ob = (orderbook *) pa->con->book;
    ob->seq = 1;
    struct parse_con *con = pa->con;
    int l_name_len = get_name_len(con->co);
    char *l_name = (char *) get_name(con->co);
    sequence_req *sr = create_sequence_request(l_name, l_name_len,
                                               con->in_args
                                               , build_offsets,
                                               create_rom_resend_pad);
    get_sequence_numbers(sr);
    /* Add the sequences into the connection object */
    set_incoming_seq(con->co, dsr_get_incoming(sr));
    destroy_sequence_req(sr);
    databuf_t *logon = databuf_alloc(61);
    short type = htons(0x0021);
    short len = htons(60);
    int in = htonl(get_incoming_seq(con->co));
    databuf_memcpy(logon, &type, 2);
    databuf_memcpy(logon, &len, 2);
    *logon->wr_ptr++ = '\0';
    *logon->wr_ptr++ = '\0';
    *logon->wr_ptr++ = '\0';
    *logon->wr_ptr++ = '\0';
    databuf_memcpy(logon, &in, 4);
    databuf_memcpy(logon, con->in_args->username, con->in_args->un_len);
    int cp_len = con->in_args->un_len;
    for (; cp_len < 44; ++cp_len) {
        *logon->wr_ptr++ = '\0';
    }
    *logon->wr_ptr++ = '1';
    *logon->wr_ptr++ = '\0';
    *logon->wr_ptr++ = '\0';
    *logon->wr_ptr++ = '\0';
    if (l_name) {
        free(l_name);
    }
    pa->con->init_con(pa->con->co);
    utp_send_message(ob, logon->buffer, databuf_unread(logon),
                     ob->fix_con_name, ob->fix_con_len);
    databuf_destroy(logon);
    return 1;
}

void create_databuffs(queue_t glob, queue_t q, int num)
{
    int i = 0;
    for (; i < num; ++i) {
        databuf_t *buff = databuf_alloc(500);
        enqueue(q, buff);
    }
}

void clear_db(void *db)
{
    databuf_t *buff = (databuf_t *) db;
    databuf_reset(buff);
}

void destroy_db(void *d)
{
    databuf_t *buff = (databuf_t *) d;
    databuf_destroy(buff);
}

orderbook *init_utp(const char *config, struct init_args *fix_args,
                    ex_config * ex, int (*get_is_utp_connected) ())
{
    int ret_len = 0;
    char *branch_code = NULL;
    int branch_code_int = 25;
    branch_code =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "Branch Code", 11,
                        &ret_len);
    if (branch_code != NULL) {
        branch_code_int = atoi(branch_code);
    }
    char *reserved_ids = get_val_for_tag(fix_args->dc, "FIX SERVER", 10,
                                         "Reserved Branch Codes", 21,
                                         &ret_len);
    start_seq_generator(reserved_ids, ret_len, "NYSEOrderids.map", 16,
                        branch_code_int, fix_args->reset_time->create_new,
                        ex->end_hour, ex->end_min);
    orderbook *ob = no_order_mng_book(next_order_id, get_is_utp_connected,
                                      def_rom2fix, def_rom2fix,
                                      def_xlate_fixrom,
                                      ex->order_allocation,
                                      ex->mod_name, ex->mod_name_len,
                                      fix_args->target_comp,
                                      fix_args->tc_len, NULL, 0);
    dof_t *objects = create_dart_obj_fact(ex->order_allocation,
                                          destroy_doo,
                                          create_dart_obj_func,
                                          clean_doo);
    dof_t *buffs = create_dart_obj_fact(100, destroy_db,
                                        create_databuffs,
                                        clear_db);
    ob->ord_obj = objects;
    ob->databufs = buffs;
    ob->can_send_orders = 0;
    ob->in_args = fix_args;
    utp_database_init(&ex->db, ex->dest_id, ob);
    return ob;
}

int get_obj_by_id(orderbook * ob, char *clordid, int len,
                  dart_order_obj ** obj)
{
    int ret = 0;
    pthread_mutex_lock(&ob->mutex);
    ret = get_map_entry(ob->cl_ord_id_map, clordid, len, (void **) obj);
    pthread_mutex_unlock(&ob->mutex);
    return ret;
}

void build_fill_not_found_mail(orderbook * ob, char *d_off,
                               databuf_t * buff)
{
    int last_shares = 0;
    int last_price = 0;
    double rom_price = 0;
    char scale = '4';
    memcpy(&last_shares, d_off + 20, 4);
    last_shares = ntohl(last_shares);
    memcpy(&last_price, d_off + 24, 4);
    last_price = ntohl(last_price);
    memcpy(&scale, d_off + 28, 1);
    rom_price = utp_to_rom_price(last_price, scale);
    char *title = "Unmatched fill from NYSE UTP DIRECT";
    int t_len = 35;
    char *side = 0;
    if (d_off[30] == '1') {
        side = "Buy";
    } else {
        side = "Sell";
    }
    databuf_write(buff, "%s of %d shares @ %f.\n", side, last_shares,
                  rom_price);
    int account_len = find_utp_field_len(d_off + 79, 10);
    int exec_len = find_utp_field_len(d_off + 69, 10);
    int clordid_len = find_utp_field_len(d_off + 99, 17);
    databuf_write(buff, "Billing indicator: %c\nOrder id: ", d_off[31]);
    databuf_memcpy(buff, d_off + 99, clordid_len);
    databuf_memcpy(buff, "\nAccount: ", 10);
    databuf_memcpy(buff, d_off + 79, account_len);
    databuf_memcpy(buff, "\nExec ID: ", 10);
    databuf_memcpy(buff, d_off + 69, exec_len);
    send_biz_message(buff->buffer, databuf_unread(buff), title, t_len);
}

void build_bust(orderbook * ob, char *d_off, databuf_t * buff)
{
    int last_shares = 0;
    int last_price = 0;
    double rom_price = 0;
    char scale = '4';
    char *title = 0;
    int t_len = 0;
    memcpy(&last_shares, d_off + 12, 4);
    last_shares = ntohl(last_shares);
    memcpy(&last_price, d_off + 16, 4);
    last_price = ntohl(last_price);
    memcpy(&scale, d_off + 20, 1);
    rom_price = utp_to_rom_price(last_price, scale);
    if (d_off[22] == '1') {
        title = "Bust from NYSE UTP DIRECT";
        t_len = 25;
    } else {
        title = "Correction from NYSE UTP DIRECT";
        t_len = 31;
    }
    databuf_write(buff, "%d shares @ %f.\n", last_shares, rom_price);
    int account_len = find_utp_field_len(d_off + 66, 10);
    int exec_len = find_utp_field_len(d_off + 46, 10);
    int clordid_len = find_utp_field_len(d_off + 76, 17);
    databuf_memcpy(buff, "\nOrder id: ", 11);
    databuf_memcpy(buff, d_off + 76, clordid_len);
    databuf_memcpy(buff, "\nAccount: ", 10);
    databuf_memcpy(buff, d_off + 66, account_len);
    databuf_memcpy(buff, "\nExec ID: ", 10);
    databuf_memcpy(buff, d_off + 46, exec_len);
    send_biz_message(buff->buffer, databuf_unread(buff), title, t_len);
}

unsigned long parse_utpdirect(const char *message,
                              unsigned long len,
                              int *cut_con, struct parse_con *pc)
{
    short ord_type = 0;
    char *d_off = (char *) message;
    unsigned long bytes_used = 0;
    long len_left = (long) len;
    int should_parse = 1;
    int add_seq = 0;
    struct message mess;
    orderbook *ob = (orderbook *) pc->book;
    dart_order_obj *obj = 0;
    while (should_parse && bytes_used < len) {
        memcpy(&ord_type, d_off, 2);
        ord_type = ntohs(ord_type);
        switch (ord_type) {
        case 0x0091:
            if ((len_left = (len_left - 56)) >= 0) {
                add_seq = 1;
                char *id = d_off + 36;
                int id_len = find_utp_field_len(id, 17);
                int sender_len = 0;
                int found = get_obj_by_id(ob, id, id_len, &obj);
                if (found) {
                    databuf_t *buff = (databuf_t *) get_obj(ob->databufs);
                    parse_order_ack(obj, buff, d_off, ob);
                    char *sender =
                        get_rom_field(obj, ROM_SENDER, &sender_len);
                    utp_send_wrapper(ob, buff, obj, sender, sender_len);
                    destroy_elem(ob->databufs, buff);
                }
                d_off += 56;
                bytes_used += 56;
            } else {
                should_parse = 0;
            }
            break;
        case 0x00A1:
            if ((len_left = (len_left - 56)) >= 0) {
                add_seq = 1;
                databuf_t *buff = (databuf_t *) get_obj(ob->databufs);
                char *id = d_off + 36;
                int id_len = find_utp_field_len(id, 17);
                databuf_memcpy(buff, "Cancel Pending received for: ", 29);
                databuf_memcpy(buff, id, id_len);
                *buff->wr_ptr++ = '\n';
                mess.data = buff->buffer;
                mess.len = databuf_unread(buff);
                mess.name = ob->fix_con_name;
                mess.name_len = ob->fix_con_len;
                dart_rolling_log_coming(&mess, ob->module_name,
                                        ob->module_name_len);
                destroy_elem(ob->databufs, buff);
                d_off += 56;
                bytes_used += 56;
            } else {
                should_parse = 0;
            }
            break;
        case 0x00D1:
            if ((len_left = (len_left - 56)) >= 0) {
                add_seq = 1;
                char *id = d_off + 37;
                int id_len = find_utp_field_len(id, 17);
                int sender_len = 0;
                int found = get_obj_by_id(ob, id, id_len, &obj);
                if (found) {
                    databuf_t *buff = (databuf_t *) get_obj(ob->databufs);
                    parse_ur_out(obj, buff, d_off, ob);
                    char *sender =
                        get_rom_field(obj, ROM_SENDER, &sender_len);
                    utp_send_wrapper(ob, buff, obj, sender, sender_len);
                    destroy_elem(ob->databufs, buff);
                    destroy_elem(ob->ord_obj, obj);
                }
                d_off += 56;
                bytes_used += 56;
            } else {
                should_parse = 0;
            }
            break;
        case 0x00B1:
            if ((len_left = (len_left - 56)) >= 0) {
                add_seq = 1;
                char *id = d_off + 36;
                int id_len = find_utp_field_len(id, 17);
                int sender_len = 0;
                int found = get_obj_by_id(ob, id, id_len, &obj);
                if (found) {
                    databuf_t *buff = (databuf_t *) get_obj(ob->databufs);
                    parse_cancel_rep(obj, buff, d_off, ob);
                    char *sender =
                        get_rom_field(obj, ROM_SENDER, &sender_len);
                    utp_send_wrapper(ob, buff, obj, sender, sender_len);
                    destroy_elem(ob->databufs, buff);
                }
                d_off += 56;
                bytes_used += 56;
            } else {
                should_parse = 0;
            }
            break;
        case 0x00E1:
            if ((len_left = (len_left - 60)) >= 0) {
                add_seq = 1;
                char *id = d_off + 41;
                int id_len = find_utp_field_len(id, 17);
                int sender_len = 0;
                int found = get_obj_by_id(ob, id, id_len, &obj);
                if (found) {
                    databuf_t *buff = (databuf_t *) get_obj(ob->databufs);
                    parse_replaced(obj, buff, d_off, ob);
                    char *sender =
                        get_rom_field(obj, ROM_SENDER, &sender_len);
                    utp_send_wrapper(ob, buff, obj, sender, sender_len);
                    destroy_elem(ob->databufs, buff);
                }
                d_off += 60;
                bytes_used += 60;
            } else {
                should_parse = 0;
            }
            break;
        case 0x00F1:
            if ((len_left = (len_left - 116)) >= 0) {
                add_seq = 1;
                char rej_type = '0';
                int offset = 56;
                memcpy(&rej_type, (d_off + 18), 1);
                if (rej_type == '1') {
                    offset = 39;
                }
                char *id = d_off + offset;
                int id_len = find_utp_field_len(id, 17);
                int sender_len = 0;
                int found = get_obj_by_id(ob, id, id_len, &obj);
                if (found) {
                    databuf_t *buff = (databuf_t *) get_obj(ob->databufs);
                    parse_rej(obj, buff, d_off, ob);
                    char *sender =
                        get_rom_field(obj, ROM_SENDER, &sender_len);
                    utp_send_wrapper(ob, buff, obj, sender, sender_len);
                    destroy_elem(ob->databufs, buff);
                }
                d_off += 116;
                bytes_used += 116;
            } else {
                should_parse = 0;
            }
            break;
        case 0x0081:
            if ((len_left = (len_left - 116)) >= 0) {
                add_seq = 1;
                char *id = d_off + 99;
                int id_len = find_utp_field_len(id, 17);
                int sender_len = 0;
                int found = get_obj_by_id(ob, id, id_len, &obj);
                if (found) {
                    databuf_t *buff = (databuf_t *) get_obj(ob->databufs);
                    int clean = parse_filled(obj, buff, d_off, ob);
                    char *sender =
                        get_rom_field(obj, ROM_SENDER, &sender_len);
                    utp_send_wrapper(ob, buff, obj, sender, sender_len);
                    destroy_elem(ob->databufs, buff);
                    if (clean) {
                        destroy_elem(ob->ord_obj, obj);
                    }
                } else {
                    databuf_t *buff = (databuf_t *) get_obj(ob->databufs);
                    build_fill_not_found_mail(ob, d_off, buff);
                    destroy_elem(ob->databufs, buff);
                }
                d_off += 116;
                bytes_used += 116;
            } else {
                should_parse = 0;
            }
            break;
        case 0x0101:
            if ((len_left = (len_left - 96)) >= 0) {
                databuf_t *buff = (databuf_t *) get_obj(ob->databufs);
                build_bust(ob, d_off, buff);
                destroy_elem(ob->databufs, buff);
                d_off += 96;
                bytes_used += 96;
            } else {
                should_parse = 0;
            }
            break;
        case 0x00C1:
            if ((len_left = (len_left - 216)) >= 0) {
                d_off += 216;
                bytes_used += 216;
            } else {
                should_parse = 0;
            }
            break;
        case 0x0141:
            bytes_used = len;
            should_parse = 0;
            send_debug_message("Logon Rejected: %s \n", d_off + 18);
            ob->can_send_orders = 0;
            *cut_con = 1;
            break;
        case 0x0011:
            bytes_used += 8;
            send_hb(pc);
            break;
        case 0x0001:
            bytes_used += 8;
            break;
        case 0x0021:
            bytes_used += 60;
            ob->can_send_orders = 1;
            pc->connection_notice(pc->co, 1, NULL, 0);
            break;
        default:
            bytes_used = len;
            should_parse = 0;
            break;
        }
        if (add_seq) {
            add_seq = 0;
            utp_rec_in(ob->fix_con_name, ob->fix_con_len);
        }
    }
    return bytes_used;
}

int get_obj_by_rom_tag(orderbook * ob, dart_order_obj * rph,
                       dart_order_obj ** obj)
{
    int ret = 0;
    char *rom_id = rph->data->buffer + rph->positions[ROM_TAG].beg;
    int rom_id_len =
        rph->positions[ROM_TAG].end - rph->positions[ROM_TAG].beg;
    pthread_mutex_lock(&ob->mutex);
    ret =
        get_map_entry(ob->rom_ord_id_map, rom_id, rom_id_len,
                      (void **) obj);
    pthread_mutex_unlock(&ob->mutex);
    return ret;
}

static void log_outgoing(orderbook * ob, dart_order_obj * ute,
                         char *message, int mess_len)
{
    struct message mess;
    int len = 0;
    char *clordid = get_rom_field(ute, ROM_CLORDID, &len);
    databuf_t *buff = (databuf_t *) get_obj(ob->databufs);
    databuf_memcpy(buff, message, mess_len);
    databuf_memcpy(buff, clordid, len);
    *buff->wr_ptr++ = '\n';
    mess.data = buff->buffer;
    mess.len = databuf_unread(buff);
    mess.name = ob->fix_con_name;
    mess.name_len = ob->fix_con_len;
    dart_rolling_log_going(&mess, ob->module_name, ob->module_name_len);
    destroy_elem(ob->databufs, buff);
}

void utp_rom_helper_func(void *r, struct message *mess, void *b)
{
    struct parse_con *pc = (struct parse_con *) b;
    orderbook *ob = (orderbook *) pc->book;
    dart_order_obj *rph = (dart_order_obj *) r;
    long total_len = 0;
    struct message m1;
    int found = 0;
    char *l_message = 0;
    dart_order_obj *ute = 0;
    databuf_t *buff = (databuf_t *) get_obj(ob->databufs);
    dart_rolling_log_coming(mess, ob->module_name, ob->module_name_len);

    if (ob->can_send_orders) {
        switch (mess->data[0]) {
        case 'E':
            set_rom_field(rph, ROM_SENDER, mess->name, mess->name_len);
            set_rom_field(rph, ROM_ON_BEHALF, ob->in_args->sender_comp,
                          ob->in_args->sc_len);
            memset(buff->buffer, '\0', 84);
            total_len = create_new_order_message(buff, rph, ob->seq, ob);
            if (total_len > 0) {
                utp_send_message(ob, buff->buffer, 84,
                                 ob->fix_con_name, ob->fix_con_len);
                log_outgoing(ob, rph, "New Order id: ", 14);
                ob->seq++;
            }
            break;
        case 'C':
            found = get_obj_by_rom_tag(ob, rph, &ute);
            if (found) {
                memset(buff->buffer, '\0', 92);
                total_len = create_cancel_message(buff, ute, ob->seq, ob);
                if (total_len > 0) {
                    utp_send_message(ob, buff->buffer, 92,
                                     ob->fix_con_name, ob->fix_con_len);
                    log_outgoing(ob, ute, "Cancel id: ", 11);
                    ob->seq++;
                }
            }
            destroy_elem(ob->ord_obj, rph);
            break;
        case 'R':
            found = get_obj_by_rom_tag(ob, rph, &ute);
            if (found) {
                ute->rep = rph;
                memset(buff->buffer, '\0', 104);
                total_len = create_replace_message(buff, ute, ob->seq, ob);
                if (total_len > 0) {
                    utp_send_message(ob, buff->buffer, 104,
                                     ob->fix_con_name, ob->fix_con_len);
                    log_outgoing(ob, ute, "Replace id: ", 12);
                    ob->seq++;
                }
            }
            break;
        default:
            break;
        }
        if (total_len <= 0) {
            l_message = "Message faild validation.";
        }
    } else {
        l_message = "Not connected to Exchange.";
    }
    if (total_len <= 0) {
        databuf_reset(buff);
        total_len = create_rom_reject(mess->data, buff, l_message);
        if (total_len > 0) {
            utp_send_message(ob, buff->buffer, databuf_unread(buff),
                             mess->name, mess->name_len);
        }
        total_len = databuf_unread(buff);
        m1.name = mess->name;
        m1.name_len = mess->name_len;
        m1.data = buff->buffer;
        m1.len = total_len;
        dart_rolling_log_going(&m1, ob->module_name, ob->module_name_len);
    }
    destroy_elem(ob->databufs, buff);
}
