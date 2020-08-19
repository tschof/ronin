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
#include "dart_constants.h"
#include "config_loader.h"
#include "ex_config_helper.h"
#include "rom_handler.h"
#include "connection_interface.h"
#include "mailer.h"
#include "ouch_parser.h"
#include "debug_logger.h"
#include "suffix_map.h"

#define MICRO_HOUR 3600000000
#define MICRO_MIN 60000000
#define MICRO_SEC 1000000
void parse_ouch(int *cut_con, struct async_parse_args *pc);
int send_ouch_logon(struct async_parse_args *pa)
{
    con_interface *ob = (con_interface *) pa->ds_book;
    ob->ap = pa;
    databuf_t *buff = get_databuf(ob->gk);
    memset(buff->buffer, '\0', 256);
    ob->sock = pa->read_wrapper->sock;
    sequence_req *sr = create_sequence_request(pa->in_args->name, pa->in_args->name_len,
                                               pa->in_args,
                                               build_offsets,
                                               create_rom_resend_pad, pa);
    get_sequence_numbers(sr);
    /* Add the sequences into the connection object */
    pa->in_seq = dsr_get_incoming(sr);
    destroy_sequence_req(sr);
    short mess_len = 47;
    mess_len = htons(mess_len);
    databuf_memcpy(buff, &mess_len, 2);
    databuf_memcpy(buff, "L", 1);
    databuf_write(buff, "%-6s", pa->in_args->username);
    databuf_write(buff, "%-10s", pa->in_args->password);
    databuf_write(buff, "%-10s", " ");
    databuf_write(buff, "%-20d", pa->in_seq);
    buff->wr_ptr = buff->buffer + 49;
    int ret =
        ci_async_send(ob->fix_con_name, ob->fix_con_len, buff, ob);
    destroy_gk_db(ob->gk, buff);
    return ret;
}
static void ouch_count_incoming(char *name, int name_len)
{
    struct incoming_message *m1;
    m1 = create_incoming_message(name, name_len, 1);
    record_inbound_message(m1);
}


static int should_send_hb(con_interface* ci)
{
    time_t now = 0;
    time(&now);
    return (now - ci->last_sent > 1);
}

void* send_ouch_hb(void* pc)
{
    con_interface *ob = (con_interface *) pc;
    char buff[256];
    read(ob->time_wrapper->sock,buff, 256); 
    if(should_send_hb(ob)) {
        databuf_t *buff = get_databuf(ob->gk);
        memset(buff->buffer, '\0', 5);
        unsigned char var = 'R';
        short ml = htons(1);
        databuf_memcpy(buff, &ml, 2);
        databuf_memcpy(buff, &var, 1);
        ci_async_send(ob->fix_con_name, ob->fix_con_len, buff, ob);
        destroy_gk_db(ob->gk, buff);
    }
    add_back_rep_timer(ob->ass,ob->time_wrapper);
    return NULL;
}

void build_fill_not_found_mail(con_interface * ob, char *d_off,
                               int len)
{
    char *title = "Unmatched fill from OUCH";
    int t_len = 24;
    databuf_t *buff = get_databuf(ob->gk);
    int shares = 0;
    int price = 0;
    long match = 0;
    char token[15];
    memset(token, '\0', 15);
    memcpy(token, d_off + 9, 14);
    memcpy(&shares, d_off + 23, 4);
    memcpy(&price, d_off + 27, 4);
    memcpy(&match, d_off + 32, 8);
    shares = ntohl(shares);
    price = ntohl(price);
    databuf_write(buff, "\nExec ID:%lu \nCL Ord ID: %d\n shares: %lu \n price: %d \n",
                  match, token, shares, price);
    databuf_write(buff, "Billing indicator: %c\n", d_off[31]);
    send_biz_message(buff->buffer, databuf_unread(buff), title, t_len); 
}
void build_bust_mail(con_interface* ob, char* d_off, int len)
{
    char *title = "Bust from OUCH";
    int t_len = 14;
    databuf_t *buff = get_databuf(ob->gk);
    char token[15];
    memset(token, '\0', 15);
    memcpy(token, d_off + 9, 14);
    switch(d_off[31]) {
        case 'E':
            databuf_write(buff, "This trade was ruled erroneous:\n %s\n", token);
            break;
        case 'C':
            databuf_write(buff, "Consent, two parties mutually agreed to bust this trade:\n %s\n", token);
            break;
        case 'S':
            databuf_write(buff, "This trade was manually broken by a NASDAQ Supervisory terminal:\n %s\n", token);
            break;
        case 'X':
            databuf_write(buff, "This trade was broken by an external third party:\n %s\n", token);
            break;
        default:
            databuf_write(buff, "This trade was broken for an unknown reason: \n%s\n", token);
    }
    send_biz_message(buff->buffer, databuf_unread(buff), title, t_len); 
}

void start_ouch_hb(struct async_parse_args* pc, con_interface* ci)
{
    ci->time_wrapper =  add_timer(pc->ass, 1, 0, 1,
                             0, ci,
                             send_ouch_hb);
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
int on_ouch_enter_order(con_interface * ci, dart_order_obj * doj,
                      struct async_parse_args *ap)
{
    int total_len = 0;
    databuf_t *buff = (databuf_t *) get_databuf(ci->gk);
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
                           databuf_unread(buff), 0x0);
    } else if(total_len <= 0) {
        reset_rom_field(doj, ROM_STATUS, "8", 1);
        reset_rom_field(doj, 0, ROM_ZERO_STATUS, 1);
        ci->on_reject(ci, doj);
    }
    destroy_gk_db(ci->gk, buff);
    return 1;
}
void on_ouch_replace_order(con_interface * ci, dart_order_obj * doj)
{
    int total_len = 0;
    databuf_t *buff = (databuf_t *) get_databuf(ci->gk);
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
int on_ouch_cancel_order(con_interface * ci, dart_order_obj * doj)
{ 
    int total_len = 0;
    databuf_t *buff = (databuf_t *) get_databuf(ci->gk);
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
    return 0;
}
int build_ouch_offset(struct async_parse_args *apa)
{
    return 0;
}


void create_connection(con_interface * con, char *filename, int name_len,
                       gatekeeper * gk)
{
    ex_config *ex = init_config(filename, name_len);
    struct init_args *fix_args = load_fix_config(filename, name_len);
    reset_con_interface(con, 8192,
                        (char *) ex->mod_name, ex->mod_name_len,
                        fix_args->target_comp, fix_args->tc_len,
                        send_ouch_logon, build_ouch_offset,
                        on_ouch_enter_order, on_ouch_cancel_order,
                        on_ouch_replace_order, gk, (char *) ex->dest_id,
                        ex->dest_id_len);
    con->client_mess_parse = parse_ouch;
    con->connection_conf = ex;
    con->can_send_orders = 0;
    con->session_conf = fix_args;
    con->tg = generic_creation_func();
    int ret_len = 0;
    char* directory = get_val_for_tag(ex->dc, "FIX SERVER", 10,
            "idtomnemonic", 9, &ret_len);
    if(ret_len <=0 ) {
        con->sbm = create_service_bureau_matcher("IDtomnemonic.map");
    } else {
        con->sbm = create_service_bureau_matcher(directory);
    }
    char* suffix_map_name = get_val_for_tag(ex->dc, "FIX SERVER", 10,
            "suffix_map", 10, &ret_len);
    if(ret_len <= 0) {
        con->parser = create_suffix_map("inet_suffix.map");
    } else {
        con->parser = create_suffix_map(suffix_map_name);
    } 
    add_int_orderids(con);
}

static int get_real_token_len(char* message)
{
    char* d_off = message + 9;
    char* end = message + 23;
    do {                                
        --end;                            
    } while(d_off != end && *(end) == ' '); 
    return (end - d_off) + 1;    
}

static void parse_seq_packet(con_interface* ci, async_parse_args *pc, const char* message, size_t mess_len)
{
    char* d_off = (char*)message;
    size_t len = 0;
    dart_order_obj *obj = 0;
//    ouch_find_token_len(d_off, len);
    len = get_real_token_len((char*)message);
    if(get_obj_by_id(ci,((char*) message) + 9, len, &obj)) {
        len = mess_len;
        d_off = (char*)message;
        switch(message[0]) {
            case 'A':
                if(lock_n_check(obj)) {
                    int ret = parse_order_ack(obj, d_off, ci);
                    if(ret) {
                        ci->on_out(ci, obj);
                        unlock(obj);
                        destroy_gk_obj(ci->gk, obj);
                    } else {
                        ci->ci_obj_callback(obj, ci);
                        unlock(obj);
                    }
                }
                break;
            case 'U':
                if(lock_n_check(obj) &&
                        lock_n_check((dart_order_obj *) obj->rep)) {
                    int ret = parse_replaced(obj, d_off, ci);
                    ci->on_replaced(ci, obj);
                    unlock((dart_order_obj *) obj->rep);
                    destroy_gk_obj(ci->gk, obj->rep);
                    obj->rep = 0;
                    unlock(obj);
                    if(ret) {
                        destroy_gk_obj(ci->gk, obj);
                    }
                }
                break;
            case 'C':
                if(lock_n_check(obj)) {
                    parse_ur_out(obj, d_off, ci);
                    ci->on_out(ci, obj);
                    unlock(obj);
                    destroy_gk_obj(ci->gk, obj);
                }
                break;
            case 'E':
                if(lock_n_check(obj)) {
                    int ret = parse_filled(obj, d_off, ci);
                    ci->on_fill(ci, obj);
                    unlock(obj);
                    if(ret) {
                        destroy_gk_obj(ci->gk, obj);
                    }
                }
                break;
            case 'J':
                if(lock_n_check(obj)) {
                    int ret = parse_rej(obj, d_off, ci);
                    ci->on_reject(ci, obj);
                    if (ret == 3) {
                        unlock((dart_order_obj *) obj->rep);
                        destroy_gk_obj(ci->gk, obj->rep);
                        unlock(obj);
                    } else {
                        unlock(obj);
                        destroy_gk_obj(ci->gk, obj);
                    }
                }
                break;
            case 'P':
                break;
            case 'M':
                break;
            case 'T':
                break;
            case 'I':
                if(lock_n_check(obj)) {
                    parse_cnl_rej(obj, d_off, ci);
                    ci->on_reject(ci, obj);
                    unlock(obj);
                }
                break;
            case 'B':
                build_bust_mail(ci, (char*)message, mess_len);
                break;
            case 'D':
                break;
            case 'S':
                if(d_off[9] == 'S') {
                    send_debug_message("Start of the Day. Send away! \n");
                } else {
                    send_debug_message("End of Day. No mas!!! \n");
                }
                break;

        };
    } else if(message[0] == 'E') {
        build_fill_not_found_mail(ci, (char*)message, mess_len);
    } else if(message[0] == 'B') {
        build_bust_mail(ci, (char*)message, mess_len);
    }
    
    record_raw_message(ci->fix_con_name, ci->fix_con_len,
                       (char*)message, mess_len, 0x0);
}
void parse_ouch(int *cut_con, struct async_parse_args *pc)
{
    con_interface *ob = (con_interface *) pc->ds_book;
    char* d_off = pc->con->rd_ptr;
    long len = (pc->con->wr_ptr - pc->con->rd_ptr);
    unsigned long bytes_used = 0;
    unsigned short mess_len = 0;
    do {
        memcpy(&mess_len, d_off, 2);
        mess_len = ntohs(mess_len);
        if(bytes_used + mess_len + 2 > len || mess_len == 0) {
            break;
        } 
        d_off += 2;
        switch(d_off[0]) {
            case 'S':
                parse_seq_packet(ob, pc, d_off + 1, mess_len -1 );
                ouch_count_incoming(ob->fix_con_name, ob->fix_con_len);
                break;
            case '+':
                send_debug_message("DEBUG message: %s \n", d_off);
                break;
            case 'A':
                start_ouch_hb(pc, ob);
                ob->can_send_orders = 1;
                pc->connection_notice(pc, 1, NULL, 0);
                break;
            case 'J':
                if(d_off[1] == 'A') {
                    send_debug_message("Logon Rejected: %c = Not Authorized, invalid username and/or password \n", d_off[1]);
                } else {
                    send_debug_message("Logon Rejected: %c = Session not available. \n", d_off[1]);
                }
                break;
            case 'H':
                break;
            case 'Z':
                *cut_con = 1;
                send_debug_message("End of session signaled by ouch.\n");
                break;
        };
        bytes_used += (mess_len + 2);
        d_off += mess_len;
    } while(bytes_used < len);
    pc->con->rd_ptr += bytes_used;
}

