
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
#include "rex_allocator.h"
#include "parser_func.h"
#include "rex_constants.h"
#include "rex_seq_request.h"
#include "config_loader.h"
#include "ex_config_helper.h"
#include "dart_handler.h"
#include "mailer.h"
#include "nyse_seq_generator.h"
#include "connection_interface.h"
#include "connection_factory.h"
#include "utpdirect.h"
#include "utp_object.h"
#include "log_mnger.h"
#include "dart_order_fact.h"


struct utp_parser {
    otl* logger;
    login_info* logon;
    con_int* pbp;
    rex_con* rc;
    nyse_seq_struct* nss;
    databuf_t* mess_to_send;
    pthread_spinlock_t send_mutex;
    call_back_wrapper* recon_wrap;
    char* on_behalf;
    int oblen;
    int in_seq;
    int out_seq;
    int is_logged_on;
};


nyse_seq_struct* get_seq_struct(utpp* uparse)
{
    return uparse->nss;
}

static int utp_sender(utpp* up, databuf_t* buff)
{
    int bytes_sent = 0;
    if(pthread_spin_lock(&up->send_mutex) == 0) {
        int disconnect = 0;
        int len = databuf_unread(buff);
        bytes_sent = gen_send(up->rc, buff->rd_ptr, len,
                                  &disconnect);
        if(bytes_sent < len && !disconnect) {
            databuf_memcpy(up->mess_to_send, buff->rd_ptr + bytes_sent,
                           (len - bytes_sent));
            add_sender(up->rc->p, up->rc);
        } else if(disconnect) {
            up->rc->disconnect = disconnect;
            up->is_logged_on = 0;
        }
        pthread_spin_unlock(&up->send_mutex);
    } 
    return bytes_sent;
}
void utp_write_callback(void* cb)
{
    rex_con* rc = (rex_con*)cb;
    utpp* up = (utpp*)rc->parser;
    int blen = 0;
    int disconnect = 0;
    pthread_spin_lock(&up->send_mutex);
    blen = databuf_unread(up->mess_to_send);
    int bytes_sent = gen_send(up->rc, up->mess_to_send->rd_ptr, blen,
                                  &disconnect);
    if(bytes_sent >= 0) {
        up->mess_to_send->rd_ptr += bytes_sent;
        if(bytes_sent < blen && up->rc->disconnect <= 0) {
            pthread_spin_unlock(&up->send_mutex);
            add_sender(rc->p, rc);
            return;
        } 
    } else if(disconnect) {
        up->rc->disconnect = disconnect;
        up->is_logged_on = 0;
    }
    pthread_spin_unlock(&up->send_mutex);
}

static void create_seq_struct(con_int* con, utpp* uparse, int seq)
{
    char *branch_code = NULL;
    int branch_code_int = 25;
    int ret_len = 0;
    branch_code =
        get_val_for_tag(con->ex->dc, "SERVER", 6, "Branch Code", 11,
                &ret_len);
    if (branch_code != NULL) {
        branch_code_int = atoi(branch_code);
        free(branch_code);
    }
    char *reserved_ids = get_val_for_tag(con->ex->dc, "SERVER", 6,
            "Reserved Branch Codes", 21,
            &ret_len);
    uparse->nss = start_seq_generator(reserved_ids,
            ret_len, "NYSEOrderids.map", 16,
            branch_code_int + seq);
    if (reserved_ids) {
        free(reserved_ids);
    }
}
static utpp* create_utp_parser(otl* logger, con_int* pbp, login_info* u)
{
    utpp* fp = 0;
    int ret = 0;
    REX_ALLOC(fp, 8, sizeof(struct utp_parser), ret);
    fp->logger = logger;
    fp->pbp = pbp;
    fp->logon = u;
    fp->nss = 0;
    pthread_spin_init(&fp->send_mutex, PTHREAD_PROCESS_PRIVATE);
    fp->is_logged_on = 0;
    pbp->cparser = fp;
    ret = 0;
    fp->on_behalf = 
        get_val_for_tag(pbp->ex->dc, "SERVER", 6, "OnBehalfOfCompId", 16,
                &ret);
    fp->oblen = ret;
    return fp;
}
static void parse_utpdirect(int *cut_con, rex_con *rc);

static void send_hb(utpp *con)
{
    con_int *ob = (con_int *) con->pbp;
    databuf_t *buff = get_databuf(ob->gk);
    short mt = htons(0x001);
    short ml = htons(8);
    databuf_memcpy(buff, &mt, 2);
    databuf_memcpy(buff, &ml, 2);
    *buff->wr_ptr++ = '\0';
    *buff->wr_ptr++ = '\0';
    *buff->wr_ptr++ = '\0';
    *buff->wr_ptr++ = '\0';
    utp_sender(con, buff);
    destroy_gk_db(ob->gk, buff);
}


void send_utp_logon(void* dd)
{
    utpp *pa = (utpp*)dd;
    con_int *ob = (con_int *) pa->pbp;
    pa->in_seq = 1;
    ob->sock = pa->rc->sock;//read_wrapper->sock;
    sequence_req *sr = create_sequence_request(
                                 pa->logon->name, pa->logon->nlen,
                                 pa->logon,ob->ex->version,
                                 build_offsets,
                                 create_rom_resend_pad, ob);
    get_sequence_numbers(sr, ob->o);
    if(pa->nss == 0) {
        create_seq_struct(ob, pa, dsr_get_incoming(sr));
    }
    //add the sequences intor the connection object;
    int temp_in = dsr_get_incoming(sr);
    if(temp_in == 0) {
        temp_in = -1;
    }
    send_debug_message(ob->o, "%d, incoming messages recorded.\n", temp_in);
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
    databuf_memcpy(logon, pa->logon->name, pa->logon->nlen);
    int cp_len = pa->logon->nlen;
    for (; cp_len < 44; ++cp_len) {
        *logon->wr_ptr++ = '\0';
    }
    *logon->wr_ptr++ = '1';
    *logon->wr_ptr++ = '\0';
    *logon->wr_ptr++ = '\0';
    *logon->wr_ptr++ = '\0';
    utp_sender(pa, logon);
    databuf_destroy(logon);
}
int build_utp_offset(rex_con *apa)
{
    return 0;
}

int on_utp_enter_order(con_int * ci, rcntr * risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    int total_len = 0;
    utpp* uparse = (utpp*)ci->cparser;
    databuf_t *buff = (databuf_t *) get_databuf(ci->gk);
    set_rom_field(doj, ROM_ON_BEHALF,uparse->on_behalf, uparse->oblen);
    memset(buff->buffer, '\0', 84);
    total_len = create_new_order_message(buff, doj, uparse->out_seq, ci);
    if (total_len > 0) {
        total_len = utp_sender(uparse, buff);
        record_raw_message(ci->o, uparse->logon->name, uparse->logon->nlen,buff->buffer,
                databuf_unread(buff), 0x0);
    }
    destroy_gk_db(ci->gk, buff);
    return 1;
}

int on_utp_cancel_order(con_int * ci, rcntr * risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    int total_len = 0;
    utpp* uparse = (utpp*)ci->cparser;
    databuf_t *buff = get_databuf(ci->gk);
    memset(buff->buffer, '\0', 92);
    total_len = create_cancel_message(buff, doj, uparse->out_seq, ci);
    if (total_len > 0) {
        total_len = utp_sender(uparse, buff);
        record_raw_message(ci->o, uparse->logon->name, uparse->logon->nlen,buff->buffer,
                databuf_unread(buff), 0x0);
    }
    destroy_gk_db(ci->gk, buff);
    return 1;
}

int on_utp_replace_order(con_int * ci, rcntr * risk)
{
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    int total_len = 0;
    utpp* uparse = (utpp*)ci->cparser;
    databuf_t *buff = (databuf_t *) get_databuf(ci->gk);
    memset(buff->buffer, '\0', 104);
    total_len = create_replace_message(buff, doj, uparse->out_seq, ci);
    if (total_len > 0) {
        total_len = utp_sender(uparse, buff);
        record_raw_message(ci->o, uparse->logon->name, uparse->logon->nlen,buff->buffer,
                databuf_unread(buff), 0x0);
    }
    destroy_gk_db(ci->gk, buff);
    return 1;
}
void utp_cleansing_rinse(con_int *ci)
{
    if(ci && ci->cparser) {
        utpp* uparse = (utpp*)ci->cparser;
        destroy_seq_struct(uparse->nss);
        databuf_destroy(uparse->mess_to_send);
        free(uparse);
        free(ci);
    }
}

static void utp_on_connect(void* pass_back, int is_connected,
                           dart_order_obj* log)
{
    utpp* uparse = (utpp*)pass_back;
    con_int* ci = uparse->pbp;
    ci->update_oats(ci, is_connected);
    switch(is_connected) {
        case 0:
            ci->can_send_orders = 0;
            ci->connected = 0;
            reload_config(ci->ex);
            if(uparse->recon_wrap == 0) {
                uparse->recon_wrap = add_timer(ci->p,0,
                                               0,30,0,
                                               uparse->rc,
                                               reconnect);
            } else {
                add_recon_timer(ci->p, uparse->recon_wrap);
            }
            break;
        case 1:
            ci->can_send_orders = 1;
            ci->connected = 1;
            break;
        case 2:
            ci->can_send_orders = 0;
            ci->connected = 1;
            break;
    }
}

void create_connection(con_int * ci, char *filename, int name_len,
        gatekeeper * gk)
{
    ci->cl_ord_id_map = get_initial_order_map(gk, ci->mod_name,
                                              ci->mod_name_len);
    ci->sbm = 0;
    ci->tg = 0;
    ci->on_enter_order = on_utp_enter_order;
    ci->on_replace_order = on_utp_replace_order;
    ci->on_cancel_order = on_utp_cancel_order;
    ci->cleansing_rinse = utp_cleansing_rinse;
    ci->can_send_orders = 0;
    add_int_orderids(ci);
    login_info* li = build_logon_info_from_config(ci->ex);
    utpp* uparse = create_utp_parser(ci->o, ci, li);
    rex_con* rc = create_rex_con(ci->p, ci->ex, uparse, uparse,li,
                                  utp_write_callback, 
                                  utp_on_connect,
                                  parse_utpdirect, NULL);
    rc->parser = uparse;
    rc->connect = send_utp_logon;
    uparse->rc = rc;
    ci->rc = rc;
    rex_connect(ci->p, rc);
}

int get_obj_by_id(con_int * ob, char *clordid, int len,
        rcntr ** obj)
{
    int ret = 0;
    pthread_spin_lock(&ob->mutex);
    ret = get_map_entry(ob->cl_ord_id_map, clordid, len, (void **) obj);
    pthread_spin_unlock(&ob->mutex);
    return ret;
}

static void build_fill_utp_not_found_mail(con_int * ob, char *d_off,
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

void build_bust(con_int * ob, char *d_off, databuf_t * buff)
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
static void utp_return_out(con_int* ci, rcntr* cnt)
{
    dart_order_obj* doj = (dart_order_obj*)cnt->raw;
    if(doj->sender != NULL && ((con_int*)doj->sender)->on_out) {
        ((con_int*)doj->sender)->on_out((con_int*)doj->sender,
            cnt);
    } else {
        con_int* sender = 0;
        get_obj_by_client_id(ci->gk, 
                cnt->owner,cnt->olen, (void**)&sender);        
        if(sender && sender->on_update) {
            doj->sender = sender;
            sender->on_out(sender, cnt);
        }
    }   
}
static void utp_return_rej(con_int* ci, rcntr* cnt)
{
    dart_order_obj* doj = (dart_order_obj*)cnt->raw;
    if(doj->sender != NULL && ((con_int*)doj->sender)->on_rej) {
        ((con_int*)doj->sender)->on_rej((con_int*)doj->sender,
            cnt);
    } else {
        con_int* sender = 0;
        get_obj_by_client_id(ci->gk, 
                cnt->owner,cnt->olen, (void**)&sender);        
        if(sender && sender->on_rej) {
            doj->sender = sender;
            sender->on_rej(sender, cnt);
        }
    }   
}
static void utp_return_fill(con_int* ci, rcntr* cnt)
{
    dart_order_obj* doj = (dart_order_obj*)cnt->raw;
    if(doj->sender != NULL && ((con_int*)doj->sender)->on_update) {
        ((con_int*)doj->sender)->on_fill((con_int*)doj->sender,
            cnt);
    } else {
        con_int* sender = 0;
        get_obj_by_client_id(ci->gk, 
                cnt->owner,cnt->olen, (void**)&sender);        
        if(sender && sender->on_fill) {
            doj->sender = sender;
            sender->on_fill(sender, cnt);
        } 
    }   
}
static void utp_return_to_sender(con_int* ci, rcntr* cnt)
{
    dart_order_obj* doj = (dart_order_obj*)cnt->raw;
    if(doj->sender != NULL && ((con_int*)doj->sender)->on_update) {
        ((con_int*)doj->sender)->on_update((con_int*)doj->sender,
            cnt);
    } else {
        con_int* sender = 0;
        get_obj_by_client_id(ci->gk, 
                cnt->owner,cnt->olen, (void**)&sender);        
        if(sender && sender->on_update) {
            doj->sender = sender;
            sender->on_update(sender, cnt);
        } 
    }   
}

static void parse_utpdirect(int *cut_con, rex_con *rc)
{
    short ord_type = 0;
    utpp* pc = (utpp*)rc->parser;
    char* d_off = rc->con->rd_ptr;
    long len = (rc->con->wr_ptr - rc->con->rd_ptr);
    unsigned long bytes_used = 0;
    unsigned short mess_len = 0;
    long len_left = (long) len;
    int should_parse = 1;
    int add_seq = 0;
    //struct message mess;
    con_int*ob = (con_int*) pc->pbp;
    dart_order_obj *obj = 0;
    rcntr* cnt = 0x0;
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
                    int found = get_obj_by_id(ob, id, id_len, &cnt);
                    if (found) {
                        obj = (dart_order_obj*)cnt->raw;
                        parse_order_ack(obj, d_off, ob);
                        utp_return_to_sender(ob, cnt);
                    }
                    record_raw_message(ob->o, pc->logon->name, pc->logon->nlen,
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
                    record_raw_message(ob->o, pc->logon->name, pc->logon->nlen,
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
                    int found = get_obj_by_id(ob, id, id_len, &cnt);
                    if (found) {
                        obj = (dart_order_obj*)cnt->raw;
                        parse_ur_out(obj, d_off, ob);
                        utp_return_out(ob, cnt);
                    }
                    record_raw_message(ob->o, pc->logon->name, pc->logon->nlen,
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
                    int found = get_obj_by_id(ob, id, id_len, &cnt);
                    if (found) {
                        obj = (dart_order_obj*)cnt->raw;
                        parse_cancel_rep(obj, d_off, ob);
                        utp_return_to_sender(ob, cnt);
                    }
                    record_raw_message(ob->o, pc->logon->name, pc->logon->nlen,
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
                    int found = get_obj_by_id(ob, id, id_len, &cnt);
                    if (found) {
                        obj = (dart_order_obj*)cnt->raw;
                        parse_replaced(obj, d_off, ob);
                        utp_return_to_sender(ob, cnt);
                    }
                    record_raw_message(ob->o, pc->logon->name, pc->logon->nlen,
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
                    int found = get_obj_by_id(ob, id, id_len, &cnt);
                    if (found) {
                        obj = (dart_order_obj*)cnt->raw;
                        parse_rej(obj, d_off, ob);
                        utp_return_rej(ob, cnt);
                    }
                    record_raw_message(ob->o, pc->logon->name, pc->logon->nlen,
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
                    int found = get_obj_by_id(ob, id, id_len, &cnt);
                    if (found) {
                        parse_filled(obj, d_off, ob);
                        utp_return_fill(ob, cnt);
                    } else {
                        databuf_t *buff = get_databuf(ob->gk);
                        build_fill_utp_not_found_mail(ob, d_off, buff);
                        destroy_gk_db(ob->gk, buff);
                    }
                    record_raw_message(ob->o, pc->logon->name, pc->logon->nlen,
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
                    record_raw_message(ob->o, pc->logon->name, pc->logon->nlen,
                            d_off, mess_len, 0x0);
                    d_off += mess_len;
                    bytes_used += mess_len;
                } else {
                    should_parse = 0;
                }
                break;
            case 0x00C1:
                if ((len_left = (len_left - mess_len)) >= 0) {
                    record_raw_message(ob->o, pc->logon->name, pc->logon->nlen,
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
                    record_raw_message(ob->o, pc->logon->name, pc->logon->nlen,
                        d_off, len, 0x0);
                send_debug_message(ob->o, "Logon Rejected: %s \n", d_off + 18);
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
                    record_raw_message(ob->o, pc->logon->name, pc->logon->nlen,
                        d_off, mess_len,0x0);
                bytes_used += mess_len;
                d_off += mess_len;
                ob->can_send_orders = 1;
                rc->connection_notice(pc, 1, NULL);
                break;
            default:
                bytes_used +=  mess_len;
                d_off += mess_len;
                    record_raw_message(ob->o, pc->logon->name, pc->logon->nlen,
                        d_off, mess_len, 0x0);
                //should_parse = 0;
                break;
        }
        if (add_seq) {
            add_seq = 0;
            record_inbound_message(ob->o,1, pc->logon->name, pc->logon->nlen);
        }
    }
    rc->con->rd_ptr += bytes_used;
}
