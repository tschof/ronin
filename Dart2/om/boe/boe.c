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
#include "debug_logger.h"
#include "boe_seq.h"
#include "boe_parser.h"
#include "rom_to_boe.h"
#include "service_bureau.h"

#define MICRO_HOUR 3600000000
#define MICRO_MIN 60000000
#define MICRO_SEC 1000000
void parse_boe(int *cut_con, struct async_parse_args *pc);

#pragma pack(push, 1)
struct boe_logon {
    unsigned short som;
    unsigned short len;
    unsigned char type;
    unsigned char mu;
    unsigned int seq;
    char sender_sub_id[4];
    char username[4];
    char password[10];
    unsigned char num_params;
};
#pragma pack(pop)

void create_boe_logon(databuf_t* buff, async_parse_args *pa,
                      void* con)
{
    memset(buff->buffer, 0, 256);
    bsm* seqm = (bsm*)con;
    struct boe_logon* ord  =  (struct boe_logon*)buff->buffer;
    buff->wr_ptr += sizeof(struct boe_logon);
    ord->som = 0xBABA;
    ord->type = 0x37;
    memcpy(ord->sender_sub_id, pa->in_args->sender_comp, pa->in_args->sc_len);
    memcpy(ord->username, pa->in_args->username, pa->in_args->un_len);
    memcpy(ord->password, pa->in_args->password, pa->in_args->p_len);
    ord->num_params += add_unit_seq_param_group(buff, seqm); 

    /*Now we add the message optional bitfield requests*/
    unsigned char param_group_type = 0x81;
    unsigned short param_len = 0x5;
    unsigned char type = 0x25;
    unsigned char bits = 0x0;
    /*First we have the Order Ack message*/
    databuf_memcpy(buff, &param_len, 2);
    databuf_memcpy(buff, &param_group_type, 1);
    databuf_memcpy(buff, &type, 1);
    databuf_memcpy(buff, &bits, 1);
    ord->num_params += 1;
    /*Now the Reject response message*/
    databuf_memcpy(buff, &param_len, 2);
    databuf_memcpy(buff, &param_group_type, 1);
    type = 0x26;
    databuf_memcpy(buff, &type, 1);
    databuf_memcpy(buff, &bits, 1);
    ord->num_params += 1;
    /*Order Modified*/
    type = 0x27;
    param_len = 13;
    bits = 0x8;
    unsigned char om1 = 0x0;
    unsigned char om2 = 0x0;
    unsigned char om3 = 0x0;
    unsigned char om4 = 0x0;
    unsigned char om5 = 0x0;
    unsigned char om6 = 0x0;
    unsigned char om7 = 0x0;
    unsigned char om8 = 0x0;
    databuf_memcpy(buff, &param_len, 2);
    databuf_memcpy(buff, &param_group_type, 1);
    databuf_memcpy(buff, &type, 1);
    databuf_memcpy(buff, &bits, 1);
    SET_BIT(0x1, 0x1, om1);
    SET_BIT(0x1, 0x2, om1);
    SET_BIT(0x1, 0x4, om1);
    SET_BIT(0x1, 0x8, om1);
    SET_BIT(0x1, 0x10, om1);
    SET_BIT(0x1, 0x20, om1);
    SET_BIT(0x1, 0x40, om1);
    databuf_memcpy(buff, &om1, 1);
    databuf_memcpy(buff, &om2, 1);
    SET_BIT(0x1, 0x8, om3);
    SET_BIT(0x1, 0x10, om3);
    SET_BIT(0x1, 0x40, om3);
    SET_BIT(0x1, 0x80, om3);
    databuf_memcpy(buff, &om3, 1);
    databuf_memcpy(buff, &om4, 1);
    SET_BIT(0x1, 0x1, om5);
    SET_BIT(0x1, 0x2, om5);
    SET_BIT(0x1, 0x10, om5);
    SET_BIT(0x1, 0x20, om5);
    SET_BIT(0x1, 0x80, om5);
    databuf_memcpy(buff, &om5, 1);
    databuf_memcpy(buff, &om6, 1);
    databuf_memcpy(buff, &om7, 1);
    SET_BIT(0x1, 0x4, om8);
    SET_BIT(0x1, 0x8, om8);
    SET_BIT(0x1, 0x10, om8);
    SET_BIT(0x1, 0x20, om8);
    databuf_memcpy(buff, &om8, 1);
    ord->num_params += 1;
    /*End Order Modified*/
    /*Order Cancelled*/
    type = 0x2A;
    param_len = 0x5;
    bits = 0x0;
    databuf_memcpy(buff, &param_len, 2);
    databuf_memcpy(buff, &param_group_type, 1);
    databuf_memcpy(buff, &type, 1);
    databuf_memcpy(buff, &bits, 1);
    ord->num_params += 1;
    /*End Cancelled*/
    /*Cancel Rejected*/
    type = 0x2B;
    databuf_memcpy(buff, &param_len, 2);
    databuf_memcpy(buff, &param_group_type, 1);
    databuf_memcpy(buff, &type, 1);
    databuf_memcpy(buff, &bits, 1);
    ord->num_params += 1;
    /*Cancel Rejected end*/
    /*Execution start*/
    type = 0x2C;
    bits = 0x8;
    param_len = 13;
    databuf_memcpy(buff, &param_len, 2);
    databuf_memcpy(buff, &param_group_type, 1);
    databuf_memcpy(buff, &type, 1);
    databuf_memcpy(buff, &bits, 1);
    om1 = 0x0;
    databuf_memcpy(buff, &om1, 1);
    om2 = 0x0;
    databuf_memcpy(buff, &om2, 1);
    om3 = 0x0;
    databuf_memcpy(buff, &om3, 1);
    om4 = 0x0;
    databuf_memcpy(buff, &om4, 1);
    om5 = 0x0;
    databuf_memcpy(buff, &om5, 1);
    om6 = 0x0;
    databuf_memcpy(buff, &om6, 1);
    om7 = 0x0;
    databuf_memcpy(buff, &om7, 1);
    om8 = 0x0;
    SET_BIT(0x1, 0x1, om8);
    databuf_memcpy(buff, &om8, 1);
    ord->num_params += 1;
    /*End Execution*/
    ord->len = (databuf_unread(buff) - 2);
}
int send_boe_logon(struct async_parse_args *pa)
{
    con_interface *ob = (con_interface *) pa->ds_book;
    ob->ap = pa;
    bsm* seq_master = create_seq_master();
    databuf_t *buff = get_databuf(ob->gk);
    memset(buff->buffer, '\0', 256);
    ob->sock = pa->read_wrapper->sock;
    sequence_req *sr = create_sequence_request(pa->in_args->name, pa->in_args->name_len,
                                               pa->in_args,
                                               parse_boe_outgoing,
                                               create_rom_resend_pad, seq_master);
    get_sequence_numbers(sr);
    /* Add the sequences into the connection object */
    pa->in_seq = dsr_get_incoming(sr);
    destroy_sequence_req(sr);
    create_boe_logon(buff, pa, seq_master);
    int ret =
        ci_async_send(ob->fix_con_name, ob->fix_con_len, buff, ob);
    destroy_gk_db(ob->gk, buff);
    return ret;
}


static int should_send_hb(con_interface* ci)
{
    time_t now = 0;
    time(&now);
    return (now - ci->last_sent >= 1);
}

void* send_boe_hb(void* pc)
{
    con_interface *ob = (con_interface *) pc;
    char buff[256];
    read(ob->time_wrapper->sock,buff, 256); 
    if(should_send_hb(ob)) {
        databuf_t *buff = get_databuf(ob->gk);
        memset(buff->buffer, '\0', 10);
        *buff->wr_ptr++ = 0xBA;
        *buff->wr_ptr++ = 0xBA;
        short len = 8;
        databuf_memcpy(buff, &len, 2);
        *buff->wr_ptr++ = 0x03;
        *buff->wr_ptr++ = 0x0;
        buff->wr_ptr += 4;
        ci_async_send(ob->fix_con_name, ob->fix_con_len, buff, ob);
        destroy_gk_db(ob->gk, buff);
    }
    add_back_rep_timer(ob->ass,ob->time_wrapper);
    return NULL;
}

void build_fill_not_found_mail(con_interface * ob, char *d_off,
                               int len)
{
    char *title = "Unmatched fill from BOE";
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
    char *title = 0;
    int t_len = 0;
    long corrected_price = 0;
    memcpy(&corrected_price, d_off + 76, 8);
    if(corrected_price == 0) {
        title = "Bust from BOE";
        t_len = 14;
    } else {
        title = "Correction from BOE";
        t_len = 19; 
    }
    databuf_t *buff = get_databuf(ob->gk);
    char* clord_id = d_off + 18;
    long ord_id = 0;
    memcpy(&ord_id, d_off + 38, 8);
    long exec_id = 0;
    memcpy(&exec_id, d_off + 46, 8);

    char side = d_off[54];
    char liq = d_off[55];
    //char* clear_account = d_off + 60;
    char* str_side = "Buy";
    if(side == '2') {
        str_side = "Sell";
    } else if(side == '5') {
        str_side = "Sell Short";
    }
    databuf_write(buff, "%s with liquidity code of: %c \n", str_side, liq);
    databuf_write(buff, "Clordid: %s \nBATS ID: %l \nExecution ID: %l\n", clord_id, ord_id, exec_id);
    int last_shares = 0;
    memcpy(&last_shares, d_off + 54, 4);
    databuf_write(buff, "Last shares: %d \n", last_shares);
    /*Last price*/
    long last_price = 0;
    memcpy(&last_price, d_off + 68, 8);
    double lp = ((double) last_price / 10000.0) + .0000001;
    databuf_write(buff, "The previous price: %f  \n", lp);
    /*Corrected price*/
    double dcp = ((double) corrected_price / 10000.0) + .0000001;
    databuf_write(buff, "The new price: %f  \n", dcp);
    /*Now send it off to relay*/
    send_biz_message(buff->buffer, databuf_unread(buff), title, t_len); 
}

void start_boe_hb(struct async_parse_args* pc, con_interface* ci)
{
    ci->time_wrapper =  add_timer(pc->ass, 1, 0, 1,
                             0, ci,
                             send_boe_hb);
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
int on_boe_enter_order(con_interface * ci, dart_order_obj * doj,
                      struct async_parse_args *ap)
{
    int total_len = 0;
    databuf_t *buff = (databuf_t *) get_databuf(ci->gk);
    total_len = create_boe_new_order_message(buff, doj, ci->seq, ci);
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
void on_boe_replace_order(con_interface * ci, dart_order_obj * doj)
{
    int total_len = 0;
    databuf_t *buff = (databuf_t *) get_databuf(ci->gk);
    total_len = create_boe_replace_message(buff, doj, ci->seq, ci);
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
int on_boe_cancel_order(con_interface * ci, dart_order_obj * doj)
{ 
    int total_len = 0;
    databuf_t *buff = (databuf_t *) get_databuf(ci->gk);
    total_len = create_boe_cancel_message(buff, doj, ci->seq, ci);
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
int build_boe_offset(struct async_parse_args *apa)
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
                        send_boe_logon, build_boe_offset,
                        on_boe_enter_order, on_boe_cancel_order,
                        on_boe_replace_order, gk, (char *) ex->dest_id,
                        ex->dest_id_len);
    con->client_mess_parse = parse_boe;
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
        con->parser = create_boe_time_keeper("bats_suffix.map");
    } else {
        con->parser = create_boe_time_keeper(suffix_map_name);
    } 
    add_int_orderids(con);
}

static void parse_seq_packet(con_interface* ci, async_parse_args *pc,
                             char* message, size_t mess_len, char mtype, 
                             short mlen, long bu)
{
    record_raw_message(ci->fix_con_name, ci->fix_con_len,
                       (char*)message, mess_len, 0x1);
    char* clordid = (char*)message + 18;
    int clordid_len = 0;
    {
        FIND_NULL(clordid, clordid_len, 20);
    }
    int ord_done = 0;
    dart_order_obj* obj = 0;
    int found = get_obj_by_id(ci, clordid, clordid_len, &obj);
    if (found && lock_n_check(obj)) {
        char* counter =(char*) message;
        switch(mtype) {
            case 0x25:
            case 0x28:
                ord_done = boe_parse_order_ack(obj, counter, ci);
                ci->ci_obj_callback(obj, ci);
                unlock(obj);
                break;
            case 0x26: //rejected
                ord_done = boe_parse_rej(obj, message, ci);
                ci->on_reject(ci, obj);
                unlock(obj);
                break;
            case 0x27: //modified
                ord_done = boe_parse_replaced(obj, message, ci);
                ci->on_replaced(ci, obj);
                unlock(obj);
                break;
            case 0x29: //replace rejected
                ord_done = boe_parse_rep_rej(obj, message, ci);
                ci->on_reject(ci, obj);
                unlock(obj);
                break;
            case 0x2A:  //cancelled
                ord_done = boe_parse_ur_out(obj, message, ci);
                ci->on_out(ci, obj);
                unlock(obj);
                destroy_gk_obj(pc->gk, obj);
                break;
            case 0x2B: //cancel rejected
                ord_done = boe_parse_cancel_rej(obj, message, ci);
                ci->on_reject(ci, obj);
                unlock(obj);
                break;
            case 0x2C: //execution
                ord_done = boe_parse_filled(obj, message, ci);
                ci->on_fill(ci, obj);
                unlock(obj);
                if (ord_done) {
                    destroy_gk_obj(pc->gk, obj);
                }
                break;
            case 0x2D: //Trade cancel or correct
                build_bust_mail(ci, message, mess_len);
                unlock(obj);
                break;
            default:
                send_debug_message("Matched this order: %s, with unknown type: %c \n",
                                    clordid,mtype);
                unlock(obj);
                break;

        };
    } else {
        char bust = 0x2D;
        if(mtype == bust) {
            build_bust_mail(ci, message, mess_len);
        }
        send_debug_message("Unable to match this order: %s, with type: %c and length: %d, %d: %lu \n",
                                    clordid,mtype, mess_len, mlen, bu);
    }
}
#define BOE_LOGOUT 0x08

void parse_boe(int *cut_con, struct async_parse_args *pc)
{
    con_interface *ob = (con_interface *) pc->ds_book;
    char* d_off = pc->con->rd_ptr;
    long len = (pc->con->wr_ptr - pc->con->rd_ptr);
    unsigned long bytes_used = 0;
    unsigned short mess_len = 0;
    unsigned char mtype = 0x0;
    do {
        memcpy(&mtype, d_off + 4, 1);
        memcpy(&mess_len, d_off + 2, 2);
        mess_len += 2;
        if(bytes_used + mess_len > len || mess_len == 2) {
            break;
        } 
        if(mtype == 0x08) {
            *cut_con = 1;
            send_debug_message("End of session signaled by boe.\n");
        } else if(mtype == 0x24) {
            mtype = d_off[10];
            if(mtype == 0x41) {
                start_boe_hb(pc, ob);
                ob->can_send_orders = 1;
                pc->connection_notice(pc, 1, NULL, 0);
                send_debug_message("Logon Accepted mess_len: %d, len: %d .\n", mess_len, len);
            } else {
                switch(mtype) {
                    case 'N':
                        send_debug_message(
                                "Logon Rejected: %c = Not Authorized, invalid username and/or password \n",
                                d_off[10]);
                        break;
                    case 'D':
                        send_debug_message("Logon Rejected: %c = Session not available. \n", d_off[10]);
                        break;
                    case 'B':
                        send_debug_message("Logon Rejected: %c = Session in use. \n", d_off[10]);
                        break;
                    case 'S':
                        send_debug_message("Logon Rejected: %c = Session invalid. \n", d_off[10]);
                        break;
                    case 'Q':
                        send_debug_message(
                                "Logon Rejected: %c = Sequence ahead in Login message. \n",
                                d_off[10]);
                        break;
                    default:
                        send_debug_message("Logon Rejected: %s. \n", d_off + 10);
                        break;
                };
            }
        } else if(mtype == 0x09) {
            //printf("Got a Heartbeat!!!!\n");
        } else if(mtype == 0x13) {
            send_debug_message("Replay Complete mess_len: %d, len: %d .\n", mess_len, len);
        } else {
            parse_seq_packet(ob, pc, d_off, mess_len, mtype, len, bytes_used);
        }
        bytes_used += mess_len;
        d_off += mess_len ;
    } while(bytes_used < len);
    pc->con->rd_ptr += bytes_used;
}

