
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
#include "ad_parser.h"
#include "order_token_creator.h"
#include "debug_logger.h"

#define MICRO_HOUR 3600000000
#define MICRO_MIN 60000000
#define MICRO_SEC 1000000
static char *fake_client_id(size_t * i)
{
    return NULL;
}

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

static int ad_send_message(orderbook * ob, char *message, size_t len,
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

static int ad_send_iovec(orderbook * ob, struct iovec *iov, int iovcnt,
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

static void ad_send_wrapper(orderbook * ob, databuf_t * buff,
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
    ad_send_iovec(ob, t, 8, name, name_len);
    dart_rolling_log_going_iovec(iov, 8, name, name_len, ob->module_name,
                                 ob->module_name_len);
}

static void ad_rec_in(char *name, int name_len)
{
    struct incoming_message *m1;
    m1 = create_incoming_message(name, name_len, 1);
    record_inbound_message(m1);
}

static void send_hb(struct parse_con *con)
{
    orderbook *ob = (orderbook *) con->book;
    databuf_t *buff = (databuf_t *) get_obj(ob->databufs);
    memset(buff->buffer, '\0', 12);
    unsigned char var = 0x01;
    short ml = htons(12);
    *buff->wr_ptr++ = '0';
    databuf_memcpy(buff, &var, 1);
    databuf_memcpy(buff, &ml, 2);
    buff->wr_ptr = buff->buffer + 11;
    *buff->wr_ptr++ = '\n';
    ad_send_message(ob, buff->buffer, databuf_unread(buff),
                    ob->fix_con_name, ob->fix_con_len);
    destroy_elem(ob->databufs, buff);
}

int send_ad_logon(struct parse_args *pa)
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
    //add the sequences intor the connection object;
    set_incoming_seq(con->co, dsr_get_incoming(sr));
    destroy_sequence_req(sr);
    databuf_t *logon = databuf_alloc(49);
    memset(logon->buffer, '\0', 49);
    ob->seq = get_incoming_seq(con->co);
    ob->seq += create_ad_clordid_offset();
    int test_in = get_incoming_seq(con->co);
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
    databuf_memcpy(logon, con->in_args->username, con->in_args->un_len);
    logon->wr_ptr = logon->buffer + 46;
    databuf_memcpy(logon, &var, 1);
    *logon->wr_ptr++ = '\n';
    if (l_name) {
        free(l_name);
    }
    pa->con->init_con(pa->con->co);
    ad_send_message(ob, logon->buffer, 48, ob->fix_con_name,
                    ob->fix_con_len);
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

orderbook *init_ad(const char *config, struct init_args *fix_args,
                   ex_config * ex, int (*get_is_ad_connected) ())
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
    orderbook *ob = no_order_mng_book(fake_client_id, get_is_ad_connected,
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
    ob->in_args = fix_args;//copy_init_args(fix_args);
    ad_database_init(&ex->db, ex->dest_id, ob);
    return ob;
}

int get_obj_by_id(orderbook * ob, void *clordid, int len,
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
    unsigned int clordid = 0;
    memcpy(&clordid, d_off + 12, 4);
    clordid = ntohl(clordid);
    databuf_write(buff, "\nExec ID:%lu \nCL Ord ID: %d\nArca ID: %lu\n",
                  exec_id, clordid, order_id);
    databuf_write(buff, "Billing indicator: %c\n", d_off[33]);
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
    unsigned int clordid = 0;
    memcpy(&clordid, d_off + 12, 4);
    clordid = ntohl(clordid);
    databuf_write(buff, "\nExec ID:%lu \nCL Ord ID: %d\n", exec_id,
                  clordid);
    send_biz_message(buff->buffer, databuf_unread(buff), title, t_len);
}

unsigned long parse_addirect(const char *message,
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
        switch (d_off[0]) {
        case 'a':
            if ((len_left = (len_left - 32)) >= 0) {
                add_seq = 1;
                unsigned int id = 0;
                memcpy(&id, d_off + 12, 4);
                id = ntohl(id);
                int sender_len = 0;
                int found = get_obj_by_id(ob, &id, 4, &obj);
                if (found) {
                    databuf_t *buff = (databuf_t *) get_obj(ob->databufs);
                    parse_order_ack(obj, buff, d_off, ob);
                    char *sender =
                        get_rom_field(obj, ROM_SENDER, &sender_len);
                    ad_send_wrapper(ob, buff, obj, sender, sender_len);
                    destroy_elem(ob->databufs, buff);
                }
                d_off += 32;
                bytes_used += 32;
            } else {
                should_parse = 0;
            }
            break;
        case '6':
            if ((len_left = (len_left - 32)) >= 0) {
                add_seq = 1;
                databuf_t *buff = (databuf_t *) get_obj(ob->databufs);
                unsigned int id = 0;
                memcpy(&id, d_off + 12, 4);
                id = ntohl(id);
                databuf_memcpy(buff, "Cancel Pending received for: ", 29);
                databuf_memcpy(buff, &id, 4);
                *buff->wr_ptr++ = '\n';
                mess.data = buff->buffer;
                mess.len = databuf_unread(buff);
                mess.name = ob->fix_con_name;
                mess.name_len = ob->fix_con_len;
                dart_rolling_log_coming(&mess, ob->module_name,
                                        ob->module_name_len);
                destroy_elem(ob->databufs, buff);
                d_off += 32;
                bytes_used += 32;
            } else {
                should_parse = 0;
            }
            break;
        case '4':
            if ((len_left = (len_left - 32)) >= 0) {
                add_seq = 1;
                unsigned int id = 0;
                memcpy(&id, d_off + 12, 4);
                id = ntohl(id);
                int sender_len = 0;
                int found = get_obj_by_id(ob, &id, 4, &obj);
                if (found) {
                    databuf_t *buff = (databuf_t *) get_obj(ob->databufs);
                    parse_ur_out(obj, buff, d_off, ob);
                    char *sender =
                        get_rom_field(obj, ROM_SENDER, &sender_len);
                    ad_send_wrapper(ob, buff, obj, sender, sender_len);
                    destroy_elem(ob->databufs, buff);
                    destroy_elem(ob->ord_obj, obj);
                }
                d_off += 32;
                bytes_used += 32;
            } else {
                should_parse = 0;
            }
            break;
        case 'E':
            if ((len_left = (len_left - 32)) >= 0) {
                add_seq = 1;
                unsigned int id = 0;
                memcpy(&id, d_off + 12, 4);
                id = ntohl(id);
                int sender_len = 0;
                int found = get_obj_by_id(ob, &id, 4, &obj);
                if (found) {
                    databuf_t *buff = (databuf_t *) get_obj(ob->databufs);
                    parse_cancel_rep(obj, buff, d_off, ob);
                    char *sender =
                        get_rom_field(obj, ROM_SENDER, &sender_len);
                    ad_send_wrapper(ob, buff, obj, sender, sender_len);
                    destroy_elem(ob->databufs, buff);
                }
                d_off += 32;
                bytes_used += 32;
            } else {
                should_parse = 0;
            }
            break;
        case '5':
            if ((len_left = (len_left - 32)) >= 0) {
                add_seq = 1;
                unsigned int id = 0;
                memcpy(&id, d_off + 12, 4);
                id = ntohl(id);
                int sender_len = 0;
                int found = get_obj_by_id(ob, &id, 4, &obj);
                if (found) {
                    databuf_t *buff = (databuf_t *) get_obj(ob->databufs);
                    parse_replaced(obj, buff, d_off, ob);
                    char *sender =
                        get_rom_field(obj, ROM_SENDER, &sender_len);
                    ad_send_wrapper(ob, buff, obj, sender, sender_len);
                    destroy_elem(ob->databufs, buff);
                }
                d_off += 32;
                bytes_used += 32;
            } else {
                should_parse = 0;
            }
            break;
        case '8':
            if ((len_left = (len_left - 64)) >= 0) {
                add_seq = 1;
                unsigned int id = 0;
                memcpy(&id, d_off + 16, 4);
                id = ntohl(id);
                int sender_len = 0;
                int found = get_obj_by_id(ob, &id, 4, &obj);
                if (!found) {
                    memcpy(&id, d_off + 12, 4);
                    id = ntohl(id);
                    found = get_obj_by_id(ob, &id, 4, &obj);
                }
                if (found) {
                    databuf_t *buff = (databuf_t *) get_obj(ob->databufs);
                    parse_rej(obj, buff, d_off, ob);
                    char *sender =
                        get_rom_field(obj, ROM_SENDER, &sender_len);
                    ad_send_wrapper(ob, buff, obj, sender, sender_len);
                    destroy_elem(ob->databufs, buff);
                }
                d_off += 64;
                bytes_used += 64;
            } else {
                should_parse = 0;
            }
            break;
        case '2':
            if ((len_left = (len_left - 48)) >= 0) {
                add_seq = 1;
                short length = 0;
                memcpy(&length, d_off + 2, 2);
                length = ntohs(length);
                unsigned int id = 0;
                memcpy(&id, d_off + 12, 4);
                id = ntohl(id);
                int sender_len = 0;
                int found = get_obj_by_id(ob, &id, 4, &obj);
                if (found) {
                    int clean = 0;
                    databuf_t *buff = (databuf_t *) get_obj(ob->databufs);
                    if (length == 48) {
                        clean = parse_filled(obj, buff, d_off, ob);
                    } else {
                        clean = parse_ex_filled(obj, buff, d_off, ob);
                    }
                    char *sender =
                        get_rom_field(obj, ROM_SENDER, &sender_len);
                    ad_send_wrapper(ob, buff, obj, sender, sender_len);
                    destroy_elem(ob->databufs, buff);
                    if (clean) {
                        destroy_elem(ob->ord_obj, obj);
                    }
                } else {
                    databuf_t *buff = (databuf_t *) get_obj(ob->databufs);
                    build_fill_not_found_mail(ob, d_off, buff);
                    destroy_elem(ob->databufs, buff);
                }
                d_off += length;
                bytes_used += length;
            } else {
                should_parse = 0;
            }
            break;
        case 'C':
            if ((len_left = (len_left - 40)) >= 0) {
                databuf_t *buff = (databuf_t *) get_obj(ob->databufs);
                build_bust(ob, d_off, buff);
                destroy_elem(ob->databufs, buff);
                d_off += 40;
                bytes_used += 40;
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
            bytes_used += 60;
            should_parse = 0;
            send_debug_message("Logon Rejected: %s \n", d_off + 18);
            ob->can_send_orders = 0;
            *cut_con = 1;
            break;
        case 'A':
            bytes_used += 48;
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
            ad_rec_in(ob->fix_con_name, ob->fix_con_len);
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

void ad_rom_helper_func(void *r, struct message *mess, void *b)
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
        switch(mess->data[0]) {
            case 'E':
                set_rom_field(rph, ROM_SENDER, mess->name, mess->name_len);
                set_rom_field(rph, ROM_ON_BEHALF, ob->in_args->sender_comp, ob->in_args->sc_len);
                int dis_len = 0;
                get_rom_field(rph, 34, &dis_len);
                if(dis_len <= 0) {
                    memset(buff->buffer, '\0', 84);
                    total_len =  create_new_order_message(buff, rph, ob->seq, ob);
                    if(total_len > 0) {
                        ad_send_message(ob, buff->buffer, 76,
                                         ob->fix_con_name, ob->fix_con_len);
                        log_outgoing(ob, rph, "New Order id: ",14);
                        //ob->seq++;
                    }
                } else {
                    memset(buff->buffer, '\0', 168);
                    total_len = create_var_3_message(buff, rph, ob->seq, ob);
                    if(total_len > 0) {
                        ad_send_message(ob, buff->buffer, 164,
                                         ob->fix_con_name, ob->fix_con_len);
                        log_outgoing(ob, rph, "New Order id: ",14);
                        //ob->seq++;
                    }
                }
            break;
        case 'C':
            found = get_obj_by_rom_tag(ob, rph, &ute);
            if (found) {
                memset(buff->buffer, '\0', 92);
                total_len = create_cancel_message(buff, ute, ob->seq, ob);
                if (total_len > 0) {
                    ad_send_message(ob, buff->buffer, 72,
                                    ob->fix_con_name, ob->fix_con_len);
                    log_outgoing(ob, ute, "Cancel id: ", 11);
                    //ob->seq++;
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
                    ad_send_message(ob, buff->buffer, 88,
                                    ob->fix_con_name, ob->fix_con_len);
                    log_outgoing(ob, ute, "Replace id: ", 12);
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
            ad_send_message(ob, buff->buffer, databuf_unread(buff),
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
