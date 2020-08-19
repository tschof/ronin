
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
#include "mailer.h"
#include "nyse_seq_generator.h"
#include "connection_interface.h"
#include "utp_object.h"
#include "debug_logger.h"


void parse_utpdirect(int *cut_con, struct async_parse_args *pc);



static void utp_rec_in(char *name, int name_len)
{
    struct incoming_message *m1;
    m1 = create_incoming_message(name, name_len, 1);
    record_inbound_message(m1);
}

static void send_hb(struct async_parse_args *con)
{
    con_interface *ob = (con_interface *) con->ds_book;
    databuf_t *buff = get_databuf(ob->gk);
    short mt = htons(0x001);
    short ml = htons(8);
    databuf_memcpy(buff, &mt, 2);
    databuf_memcpy(buff, &ml, 2);
    *buff->wr_ptr++ = '\0';
    *buff->wr_ptr++ = '\0';
    *buff->wr_ptr++ = '\0';
    *buff->wr_ptr++ = '\0';
    ci_async_send(ob->fix_con_name, ob->fix_con_len, buff, ob);
    //utp_send_message(ob, buff->buffer, databuf_unread(buff),
    //                 ob->fix_con_name, ob->fix_con_len);
    destroy_gk_db(ob->gk, buff);
}

static void create_seq_struct(con_interface* con, int seq)
{
    char *branch_code = NULL;
    int branch_code_int = 25;
    int ret_len = 0;
    branch_code =
        get_val_for_tag(con->connection_conf->dc, "FIX SERVER", 10, "Branch Code", 11,
                &ret_len);
    if (branch_code != NULL) {
        branch_code_int = atoi(branch_code);
        free(branch_code);
    }
    char *reserved_ids = get_val_for_tag(con->connection_conf->dc, "FIX SERVER", 10,
            "Reserved Branch Codes", 21,
            &ret_len);
    nyse_seq_struct* nyss = start_seq_generator(reserved_ids,
            ret_len, "NYSEOrderids.map", 16,
            branch_code_int + seq);
    con->parser = nyss;
    if (reserved_ids) {
        free(reserved_ids);
    }
}

int send_utp_logon(struct async_parse_args *pa)
{
    con_interface *ob = (con_interface *) pa->ds_book;
    ob->seq = 1;
    ob->ap = pa;
    ob->sock = pa->read_wrapper->sock;
    sequence_req *sr = create_sequence_request(pa->in_args->name, pa->in_args->name_len,
            pa->in_args,
            build_offsets,
            create_rom_resend_pad, ob);
    get_sequence_numbers(sr);
    if(ob->parser == 0) {
        create_seq_struct(ob, dsr_get_incoming(sr));
    }
    //add the sequences intor the connection object;
    int temp_in = dsr_get_incoming(sr);
    if(temp_in == 0) {
        temp_in = -1;
    }
    send_debug_message("%d, incoming messages recorded.\n", temp_in);
    pa->in_seq = dsr_get_incoming(sr);
    destroy_sequence_req(sr);
    databuf_t *logon = databuf_alloc(61);
    short type = htons(0x0021);
    short len = htons(60);
    int in = htonl(temp_in);
    databuf_memcpy(logon, &type, 2);
    databuf_memcpy(logon, &len, 2);
    *logon->wr_ptr++ = '\0';
    *logon->wr_ptr++ = '\0';
    *logon->wr_ptr++ = '\0';
    *logon->wr_ptr++ = '\0';
    databuf_memcpy(logon, &in, 4);
    databuf_memcpy(logon, pa->in_args->username, pa->in_args->un_len);
    int cp_len = pa->in_args->un_len;
    for (; cp_len < 44; ++cp_len) {
        *logon->wr_ptr++ = '\0';
    }
    *logon->wr_ptr++ = '1';
    *logon->wr_ptr++ = '\0';
    *logon->wr_ptr++ = '\0';
    *logon->wr_ptr++ = '\0';
    int ret =
        ci_async_send(ob->fix_con_name, ob->fix_con_len, logon, ob);
    databuf_destroy(logon);
    return ret;
}
int build_utp_offset(struct async_parse_args *apa)
{
    return 0;
}

int on_utp_enter_order(con_interface * ci, dart_order_obj * doj,
        struct async_parse_args *ap)
{
    int total_len = 0;
    databuf_t *buff = (databuf_t *) get_databuf(ci->gk);
    set_rom_field(doj, ROM_ON_BEHALF, ci->session_conf->sender_comp,
            ci->session_conf->sc_len);
    memset(buff->buffer, '\0', 84);
    total_len = create_new_order_message(buff, doj, ci->seq, ci);
    if (total_len > 0) {
        total_len = ci_async_send(ci->fix_con_name, ci->fix_con_len, buff, ci);
        if(total_len > 0) {
            buff->rd_ptr -= total_len;
        }
        record_raw_message(ci->fix_con_name, ci->fix_con_len, buff->buffer,
                databuf_unread(buff), 0x0);
    }
    destroy_gk_db(ci->gk, buff);
    return 1;
}

int on_utp_cancel_order(con_interface * ci, dart_order_obj * doj)
{
    int total_len = 0;
    databuf_t *buff = get_databuf(ci->gk);
    memset(buff->buffer, '\0', 92);
    total_len = create_cancel_message(buff, doj, ci->seq, ci);
    if (total_len > 0) {
        total_len = ci_async_send(ci->fix_con_name, ci->fix_con_len, buff, ci);
        if(total_len > 0) {
            buff->rd_ptr -= total_len;
        }
        record_raw_message(ci->fix_con_name, ci->fix_con_len, buff->buffer,
                databuf_unread(buff), 0x0);
    }
    destroy_gk_db(ci->gk, buff);
    return 1;
}

void on_utp_replace_order(con_interface * ci, dart_order_obj * doj)
{
    int total_len = 0;
    databuf_t *buff = (databuf_t *) get_databuf(ci->gk);
    memset(buff->buffer, '\0', 104);
    total_len = create_replace_message(buff, doj, ci->seq, ci);
    if (total_len > 0) {
        total_len = ci_async_send(ci->fix_con_name, ci->fix_con_len, buff, ci);
        if(total_len > 0) {
            buff->rd_ptr -= total_len;
        }
        record_raw_message(ci->fix_con_name, ci->fix_con_len, buff->buffer,
                databuf_unread(buff), 0x0);
    }
    destroy_gk_db(ci->gk, buff);
}
void utp_cleansing_rinse(struct connection_interface *ci)
{
    if(ci && ci->parser) {
        destroy_seq_struct((struct nyse_sequence_struct*)ci->parser);
        ci_cleansing_rinse(ci);
    }
}

void create_connection(con_interface * con, char *filename, int name_len,
        gatekeeper * gk)
{
    ex_config *ex = init_config(filename, name_len);
    struct init_args *fix_args = load_fix_config(filename, name_len);
    reset_con_interface(con, 8192,
            (char *) ex->mod_name, ex->mod_name_len,
            fix_args->target_comp, fix_args->tc_len,
            send_utp_logon, build_utp_offset,
            on_utp_enter_order, on_utp_cancel_order,
            on_utp_replace_order, gk, (char *) ex->dest_id,
            ex->dest_id_len);
    con->client_mess_parse = parse_utpdirect;
    con->connection_conf = ex;
    con->can_send_orders = 0;
    con->session_conf = fix_args;
    con->tg = 0;
    con->parser =0;
    con->cleansing_rinse = utp_cleansing_rinse;
}

int get_obj_by_id(con_interface * ob, char *clordid, int len,
        dart_order_obj ** obj)
{
    int ret = 0;
    pthread_spin_lock(&ob->mutex);
    ret = get_map_entry(ob->cl_ord_id_map, clordid, len, (void **) obj);
    pthread_spin_unlock(&ob->mutex);
    return ret;
}

void build_fill_not_found_mail(con_interface * ob, char *d_off,
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

void build_bust(con_interface * ob, char *d_off, databuf_t * buff)
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

void parse_utpdirect(int *cut_con, struct async_parse_args *pc)
{
    short ord_type = 0;
    char* d_off = pc->con->rd_ptr;
    long len = (pc->con->wr_ptr - pc->con->rd_ptr);
    unsigned long bytes_used = 0;
    unsigned short mess_len = 0;
    long len_left = (long) len;
    int should_parse = 1;
    int add_seq = 0;
    //struct message mess;
    con_interface *ob = (con_interface *) pc->ds_book;
    dart_order_obj *obj = 0;
    while (should_parse && bytes_used < len) {
        memcpy(&ord_type, d_off, 2);
        ord_type = ntohs(ord_type);
        memcpy(&mess_len, d_off + 2, 2);
        mess_len = ntohs(mess_len);
        switch (ord_type) {
            case 0x0091:
                if ((len_left = (len_left - mess_len)) >= 0) {
                    add_seq = 1;
                    char *id = d_off + 36;
                    int id_len = find_utp_field_len(id, 17);
                    int found = get_obj_by_id(ob, id, id_len, &obj);
                    if (found && lock_n_check(obj)) {
                        parse_order_ack(obj, d_off, ob);
                        ob->ci_obj_callback(obj, ob);
                        unlock(obj);
                    }
                    record_raw_message(ob->fix_con_name, ob->fix_con_len,
                            d_off, mess_len, 0x0);
                    d_off += mess_len;//56;
                    bytes_used += mess_len;//56;
                } else {
                    should_parse = 0;
                }
                break;
            case 0x00A1:
                if ((len_left = (len_left - mess_len)) >= 0) {
                    add_seq = 1;
                    record_raw_message(ob->fix_con_name, ob->fix_con_len,
                            d_off, mess_len, 0x0);
                    d_off += mess_len;
                    bytes_used += mess_len;
                } else {
                    should_parse = 0;
                }
                break;
            case 0x00D1:
                if ((len_left = (len_left - mess_len)) >= 0) {
                    add_seq = 1;
                    char *id = d_off + 37;
                    int id_len = find_utp_field_len(id, 17);
                    int found = get_obj_by_id(ob, id, id_len, &obj);
                    if (found && lock_n_check(obj)) {
                        parse_ur_out(obj, d_off, ob);
                        ob->on_out(ob, obj);
                        unlock(obj);
                        destroy_gk_obj(ob->gk, obj);
                    }
                    record_raw_message(ob->fix_con_name, ob->fix_con_len,
                            d_off, mess_len, 0x0);
                    d_off += mess_len;
                    bytes_used += mess_len;
                } else {
                    should_parse = 0;
                }
                break;
            case 0x00B1:
                if ((len_left = (len_left - mess_len)) >= 0) {
                    add_seq = 1;
                    char *id = d_off + 36;
                    int id_len = find_utp_field_len(id, 17);
                    int found = get_obj_by_id(ob, id, id_len, &obj);
                    if (found && lock_n_check(obj)) {
                        parse_cancel_rep(obj, d_off, ob);
                        ob->ci_obj_callback(obj, ob);
                        unlock(obj);
                    }
                    record_raw_message(ob->fix_con_name, ob->fix_con_len,
                            d_off, mess_len, 0x0);
                    d_off += mess_len;
                    bytes_used += mess_len;
                } else {
                    should_parse = 0;
                }
                break;
            case 0x00E1:
                if ((len_left = (len_left - mess_len)) >= 0) {
                    add_seq = 1;
                    char *id = d_off + 41;
                    int id_len = find_utp_field_len(id, 17);
                    int found = get_obj_by_id(ob, id, id_len, &obj);
                    if (found && lock_n_check(obj)) {
                        parse_replaced(obj, d_off, ob);
                        ob->on_replaced(ob, obj);
                        unlock(obj);
                    }
                    record_raw_message(ob->fix_con_name, ob->fix_con_len,
                            d_off, mess_len, 0x0);
                    d_off += mess_len;
                    bytes_used += mess_len;
                } else {
                    should_parse = 0;
                }
                break;
            case 0x00F1:
                if ((len_left = (len_left - mess_len)) >= 0) {
                    add_seq = 1;
                    char rej_type = '0';
                    int offset = 56;
                    memcpy(&rej_type, (d_off + 18), 1);
                    if (rej_type == '1') {
                        offset = 39;
                    }
                    char *id = d_off + offset;
                    int id_len = find_utp_field_len(id, 17);
                    int found = get_obj_by_id(ob, id, id_len, &obj);
                    if (found && lock_n_check(obj)) {
                        int rej = parse_rej(obj, d_off, ob);
                        ob->on_reject(ob, obj);
                        unlock(obj);
                        if (rej == 1) {
                            destroy_gk_obj(ob->gk, obj);
                        }
                    }
                    record_raw_message(ob->fix_con_name, ob->fix_con_len,
                            d_off, mess_len, 0x0);
                    d_off += mess_len;
                    bytes_used += mess_len;
                } else {
                    should_parse = 0;
                }
                break;
            case 0x0081:
                if ((len_left = (len_left - mess_len)) >= 0) {
                    add_seq = 1;
                    char *id = d_off + 99;
                    int id_len = find_utp_field_len(id, 17);
                    int found = get_obj_by_id(ob, id, id_len, &obj);
                    if (found && lock_n_check(obj)) {
                        int clean = parse_filled(obj, d_off, ob);
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
                            d_off, mess_len, 0x0);
                    d_off += mess_len;
                    bytes_used += mess_len;
                } else {
                    should_parse = 0;
                }
                break;
            case 0x0101:
                if ((len_left = (len_left - mess_len)) >= 0) {
                    databuf_t *buff = get_databuf(ob->gk);
                    build_bust(ob, d_off, buff);
                    destroy_gk_db(ob->gk, buff);
                    record_raw_message(ob->fix_con_name, ob->fix_con_len,
                            d_off, mess_len, 0x0);
                    d_off += mess_len;
                    bytes_used += mess_len;
                } else {
                    should_parse = 0;
                }
                break;
            case 0x00C1:
                if ((len_left = (len_left - mess_len)) >= 0) {
                    record_raw_message(ob->fix_con_name, ob->fix_con_len,
                            d_off, mess_len, 0x0);
                    d_off += mess_len;
                    bytes_used += mess_len;
                } else {
                    should_parse = 0;
                }
                break;
            case 0x0141:
                bytes_used = mess_len;
                should_parse = 0;
                record_raw_message(ob->fix_con_name, ob->fix_con_len, d_off,
                        len, 0x0);
                send_debug_message("Logon Rejected: %s \n", d_off + 18);
                ob->can_send_orders = 0;
                *cut_con = 1;
                break;
            case 0x0011:
                bytes_used += mess_len;
                send_hb(pc);
                d_off += mess_len;
                break;
            case 0x0001:
                bytes_used += mess_len;
                d_off += mess_len;
                break;
            case 0x0021:
                record_raw_message(ob->fix_con_name, ob->fix_con_len, d_off,
                        mess_len,0x0);
                bytes_used += mess_len;
                d_off += mess_len;
                ob->can_send_orders = 1;
                pc->connection_notice(pc, 1, NULL, 0);
                break;
            default:
                bytes_used +=  mess_len;
                d_off += mess_len;
                record_raw_message(ob->fix_con_name, ob->fix_con_len, d_off,
                        mess_len, 0x0);
                //should_parse = 0;
                break;
        }
        if (add_seq) {
            add_seq = 0;
            utp_rec_in(ob->fix_con_name, ob->fix_con_len);
        }
    }
    pc->con->rd_ptr += bytes_used;
}
