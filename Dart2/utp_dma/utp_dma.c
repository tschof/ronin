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
#include "mailer.h"
#include "parser_func.h"
#include "rex_constants.h"
#include "config_loader.h"
#include "rex_seq_request.h"
#include "ex_config_helper.h"
#include "balance_checker.h"
#include "polling_server.h"
#include "connection_factory.h"
#include "connection_interface.h"
#include "rex_obj_factory.h"
#include "utp_dma.h"
#include "utp.h"
#include "utp_oats_parser.h"

static void parse_ss_utpdirect(int *cut_con, rex_con* rc);


typedef struct dma_utp_parser {
    otl* logger;
    login_info* logon;
    con_int* pbp;
    rex_con* rc;
    databuf_t* mess_to_send;
    pthread_spinlock_t send_mutex;
    call_back_wrapper* recon_wrap;
    int in_seq;
    int out_seq;
    int is_logged_on;
}dutp;

int get_utp_field_len(char *start, int max_len)
{
    char *end = start + (max_len - 1);
    while (end[0] == 0 && (end -start) >= 0) {
        --end;
    }
    if(end - start == 0 && start[0] == 0) {
        return 0;
    } else {
        return (end - start) + 1;
    }
}
int utp_get_obj_by_id(con_int * ob, void *clordid, int len,
                  rcntr ** obj)
{
    int ret = 0;
    pthread_spin_lock(&ob->mutex);
    ret = get_map_entry(ob->cl_ord_id_map, clordid, len, (void **) obj);
    pthread_spin_unlock(&ob->mutex);
    return ret;
}
static void client_side_remove_from_maps(con_int* ci, char *id, int id_len,
        rcntr* cnt)
{
    pthread_spin_lock(&ci->mutex);
    remove_from_map(ci->cl_ord_id_map, id, id_len);
    pthread_spin_unlock(&ci->mutex);
    con_int* dest = 0;
    get_destination(ci->gk, 0, 0, (void **) &dest);
    if(dest) {
        pthread_spin_lock(&dest->mutex);
        remove_from_map(dest->cl_ord_id_map, id, id_len);
        pthread_spin_unlock(&dest->mutex);
    }
}
static void utp_remove_from_maps(con_int * ci, char *id, int id_len, rcntr* cnt)
{
    pthread_spin_lock(&ci->mutex);
    remove_from_map(ci->cl_ord_id_map, id, id_len);
    pthread_spin_unlock(&ci->mutex);
   
    con_int* sender = 0;
    int worked = get_obj_by_client_id(ci->gk,
                                      cnt->owner,cnt->olen,
                                      (void**)&sender);
    if(worked) {
        pthread_spin_lock(&sender->mutex);
        remove_from_map(sender->cl_ord_id_map, id, id_len);
        pthread_spin_unlock(&sender->mutex);
    }
    return_cnt(ci->gk, cnt);
}

static void direct_send(con_int* ci, dutp* up, void* data, int len)
{
    int bytes_sent = 0;
    if(pthread_spin_lock(&up->send_mutex) == 0) {
        int disconnect = 0;
        bytes_sent = gen_send(up->rc, data, len,
                                  &disconnect);
        if(bytes_sent < len && !disconnect) {
            databuf_memcpy(up->mess_to_send, data + bytes_sent,
                           (len - bytes_sent));
            add_sender(up->rc->p, up->rc);
        } else if(disconnect) {
            up->rc->disconnect = disconnect;
            up->is_logged_on = 0;
        }
        pthread_spin_unlock(&up->send_mutex);
    } 
}
static void utpd_send(con_int* ci, rcntr* risk)
{
    dutp* up = (dutp*)ci->cparser;
    direct_send(ci, up, risk->raw, risk->rlen);
}

static int utpd_wrapper(con_int* ci, rcntr* risk)
{
    utpd_send(ci, risk);
    return 1;
}
/*static void eurex_cancel_all_orders(con_int* ci)
{
    databuf_t* db = get_databuf(ci->gk);
    rcntr* cnt = 0;
    hashmap_iterator_t hi = hashmap_iterator_begin(ci->cl_ord_id_map);
    char *key = 0;
    int key_len = 0;
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                                     (void *) &cnt);
    while (valid && cnt) {
        on_eurex_cancel_order(ci, cnt);
        valid = hashmap_iterator_next(ci->cl_ord_id_map, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &cnt);
    }
    hashmap_iterator_destroy(hi);
    destroy_gk_db(ci->gk, db);
}*/
void utpd_cleansing_rinse(con_int *ci)
{
    if(ci && ci->cparser) {
        dutp* uparse = (dutp*)ci->cparser;
        databuf_destroy(uparse->mess_to_send);
        free(uparse);
        free(ci);
    }
}
static int utp_dma_sender(dutp* up, databuf_t* buff)
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
static void send_hb(dutp *con)
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
    utp_dma_sender(con, buff);
    destroy_gk_db(ob->gk, buff);
}


void send_utp_logon(void* dd)
{
    dutp *pa = (dutp*)dd;
    con_int *ob = (con_int *) pa->pbp;
    pa->in_seq = 1;
    ob->sock = pa->rc->sock;//read_wrapper->sock;
    sequence_req *sr = create_sequence_request(
                                 pa->logon->name, pa->logon->nlen,
                                 pa->logon,ob->ex->version,
                                 build_offsets,
                                 create_rom_resend_pad, ob);
    get_sequence_numbers(sr, ob->o);
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
    utp_dma_sender(pa, logon);
    databuf_destroy(logon);
}

void send_utp_logout(dutp* pa, con_int* ob)
{
    pa->in_seq = 1;
    ob->sock = pa->rc->sock;//read_wrapper->sock;
    pa->in_seq = 0;
    databuf_t *logon = databuf_alloc(61);
    short type = htons(0x0141);
    short len = htons(60);
    int temp_in = 0;
    int in = htonl(temp_in);
    databuf_memcpy(logon, &type, 2);
    databuf_memcpy(logon, &len, 2);
    *logon->wr_ptr++ = '\0';
    *logon->wr_ptr++ = '\0';
    *logon->wr_ptr++ = '\0';
    *logon->wr_ptr++ = '\0';
    databuf_memcpy(logon, &in, 4);
    *logon->wr_ptr++ = '\0';
    *logon->wr_ptr++ = '\0';
    *logon->wr_ptr++ = '\0';
    *logon->wr_ptr++ = '\0';
    short rej = 5;
    databuf_memcpy(logon, &rej, 2);
    int cp_len = 0;
    for (; cp_len < 40; ++cp_len) {
        *logon->wr_ptr++ = '\0';
    }
    *logon->wr_ptr++ = '\0';
    *logon->wr_ptr++ = '\0';
    utp_dma_sender(pa, logon);
    databuf_destroy(logon);
}

static dutp* create_dma_utp_parser(otl* logger, con_int* pbp, login_info* u)
{
    int ret = 0;
    dutp* fp = 0;
    REX_ALLOC(fp, 8, sizeof(struct dma_utp_parser), ret);
    fp->logger = logger;
    fp->pbp = pbp;
    fp->logon = u;
    fp->mess_to_send = databuf_alloc(1024);
    pthread_spin_init(&fp->send_mutex, PTHREAD_PROCESS_PRIVATE);
    fp->is_logged_on = 0;
    pbp->cparser = fp;
    return fp;
}
void utp_dma_write_callback(void* cb)
{
    rex_con* rc = (rex_con*)cb;
    dutp* up = (dutp*)rc->parser;
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

static void set_new_connection(dart_order_obj* log, con_int* ci)
{
    rex_con* rc = ci->rc;
    dutp* udb = create_dma_utp_parser(ci->o, ci, rc->logon);
    udb->rc = rc;
    ci->cparser = udb;
    ci->sock = rc->sock;
    ci->is_server = 1;
    ci->logon = rc->logon;
    rc->con_pass_back = ci;
    ci->logon->cancel_on_dis = 1;
    ci->should_buffer = 1;
    ci->can_send_orders = 1;
    ci->connected = 1;
    ci->update_oats(ci, 1);
}
static void utp_listener_connect(void* pass_back, int is_connected,
                           dart_order_obj* log)
{
    con_int* ab = (con_int*)pass_back;
    if (is_connected == 1 && log != NULL) {
        ab->update_oats(ab, is_connected);
    } else if(is_connected == 0) {
        ab->update_oats(ab, is_connected);
    }
}
static void utp_client_connect(void* pass_back, int is_connected,
                           dart_order_obj* log)
{
    dutp* uparse = (dutp*)pass_back;
    con_int* ci = uparse->pbp;
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
    ci->update_oats(ci, is_connected);
}

static void no_parse_send(con_int* ci, char* message, int len)
{
    con_int* sender = 0;
    int worked = get_obj_by_client_id(ci->gk, NULL,0, (void**)&sender);        
    if(worked) {
        dutp* up = (dutp*)sender->cparser;
        direct_send(sender, up, message, len);
    }
}
static void ord_ack_send(con_int* ci, void* d_off, int len)
{
    utpack* ack = d_off;
    int clordid_len = get_utp_field_len(ack->clordid, 17);
    if(clordid_len > 0) {
        rcntr* cnt = 0;
        utp_get_obj_by_id(ci, ack->clordid, clordid_len, &cnt);
        if(cnt) {
            con_int* sender = 0;
            int worked = get_obj_by_client_id(ci->gk,
                                              cnt->owner,cnt->olen,
                                              (void**)&sender);
            if(worked) {
                dutp* up = (dutp*)sender->cparser;
                direct_send(sender, up, d_off, len);
            }
            int cplen = cnt->olen <= 5 ? cnt->olen : 5;
            memcpy(ack->target, cnt->owner, cplen);
        }
    }
}
static void parse_out_message(con_int* ci, void* d_off, int mlen)
{
    utpout* cnl = d_off;
    int clordid_len = get_utp_field_len(cnl->orig_clordid, 17);
    if(clordid_len > 0) {
        rcntr* cnt = 0;
        utp_get_obj_by_id(ci, cnl->orig_clordid, clordid_len, &cnt);
        if(cnt) {
            cnt->raw = d_off;
            cnt->rlen = mlen;
            ci->on_out(ci, cnt);
            int cplen = cnt->olen <= 5 ? cnt->olen : 5;
            memcpy(cnl->target, cnt->owner, cplen);
            utp_remove_from_maps(ci, cnl->orig_clordid, clordid_len, cnt);
        }
    }
}

static int parse_rej_message(con_int* ci, void* d_off, int mlen)
{
    int ret = 0;
    utprej* rej = d_off;
    int clordid_len = get_utp_field_len(rej->clordid, 17);
    if(clordid_len > 0) {
        rcntr* cnt = 0;
        utp_get_obj_by_id(ci, rej->clordid, clordid_len, &cnt);
        if(cnt) {
            con_int* sender = 0;
            int worked = get_obj_by_client_id(ci->gk,
                                              cnt->owner,cnt->olen,
                                              (void**)&sender);
            switch (rej->rej_msg_type) {
                case '1':
                    ci->on_rej(ci, cnt);
                    break;
                default:
                    break;
            }
            if(worked) {
                dutp* up = (dutp*)sender->cparser;
                direct_send(sender, up, d_off, mlen);
            }
            int cplen = cnt->olen <= 5 ? cnt->olen : 5;
            memcpy(rej->target, cnt->owner, cplen);
            if(rej->rej_msg_type == '1') {
                utp_remove_from_maps(ci, rej->clordid, clordid_len, cnt);
            }
        }
    }
    return ret;
}
   /* send_biz_message(buff->buffer, databuf_unread(buff), title, t_len);
}*/
static void parse_fill_message(con_int* ci, void* d_off, int mlen)
{
    utpfill* ufill = d_off;
    int clordid_len = get_utp_field_len(ufill->clordid, 17);
    int last = ufill->last;
    last = ntohl(last);
    if(clordid_len > 0) {
        rcntr* cnt = 0;
        utp_get_obj_by_id(ci, ufill->clordid, clordid_len, &cnt);
        if(cnt) {
            cnt->raw = d_off;
            cnt->rlen = mlen;
            cnt->last = last;
            cnt->cum += last;
            if(cnt->cum < cnt->size && ufill->leaves == 0) {
                ufill->leaves = cnt->size - cnt->cum;
            }
            ci->on_fill(ci, cnt);
            int cplen = cnt->olen <= 5 ? cnt->olen : 5;
            memcpy(ufill->target, cnt->owner, cplen);
            if(ufill->leaves == 0 && cnt->cum >= cnt->size) {
                utp_remove_from_maps(ci, ufill->clordid, clordid_len, cnt);
            }
        } else {
            /*TODO need to build fill not found mail*/
        }
    }
}
static void parse_vfill_message(con_int* ci, void* d_off, int mlen)
{
    utpvfill* ufill = d_off;
    int clordid_len = get_utp_field_len(ufill->clordid, 17);
    int last = ufill->last;
    last = ntohl(last);
    if(clordid_len > 0) {
        rcntr* cnt =  0;
        utp_get_obj_by_id(ci, ufill->clordid, clordid_len, &cnt);
        if(cnt) {
            cnt->raw = d_off;
            cnt->rlen = mlen;
            cnt->last = last;
            cnt->cum += last;
            if(cnt->cum < cnt->size && ufill->leaves == 0) {
                ufill->leaves = cnt->size - cnt->cum;
            }
            ci->on_fill(ci, cnt);
            int cplen = cnt->olen <= 5 ? cnt->olen : 5;
            memcpy(ufill->target, cnt->owner, cplen);
            if(ufill->leaves == 0 && cnt->cum >= cnt->size) {
                utp_remove_from_maps(ci, ufill->clordid, clordid_len, cnt);
            }
        } else {
            /*TODO need to build fill not found mail*/
        }
    }
}
void parse_con_utpdirect(int *cut_con, rex_con *rc)
{
    short ord_type = 0;
    dutp* pc = (dutp*)rc->parser;
    char* d_off = rc->con->rd_ptr;
    long len = (rc->con->wr_ptr - rc->con->rd_ptr);
    unsigned long bytes_used = 0;
    unsigned short mess_len = 0;
    long len_left = (long) len;
    int should_parse = 1;
    int add_seq = 0;
    con_int*ob = (con_int*) pc->pbp;
    while (should_parse && bytes_used < len) {
        memcpy(&ord_type, d_off, 2);
        ord_type = ntohs(ord_type);
        memcpy(&mess_len, d_off + 2, 2);
        mess_len = ntohs(mess_len);
        if(mess_len > len_left) {
            goto PDONE;
        }
        switch (ord_type) {
            case 0x0091:
                add_seq = 1;
                ord_ack_send(ob, d_off, mess_len);
                break;
            case 0x00A1:
                add_seq = 1;
                goto NO_LOGGIN;
                break;
            case 0x00D1:
                add_seq = 1;
                parse_out_message(ob, d_off, mess_len);
                break;
            case 0x00B1:
                add_seq = 1;
                break;
            case 0x00E1:
                add_seq = 1;
                break;
            case 0x00F1:
                add_seq = 1;
                parse_rej_message(ob, d_off, mess_len);
                break;
            case 0x0081:
                add_seq = 1;
                parse_fill_message(ob, d_off, mess_len);
                break;
            case 0x0101:
                add_seq = 1;
                no_parse_send(ob, d_off, mess_len);
                break;
            case 0x00C1:
                add_seq = 1;
                parse_vfill_message(ob, d_off, mess_len);
                break;
            case 0x0141:
                bytes_used = mess_len;
                should_parse = 0;
                    record_raw_message(ob->o, pc->logon->name, pc->logon->nlen,
                        d_off, len, 0x0);
                send_debug_message(ob->o, "Logon Rejected: %s \n", d_off + 18);
                ob->can_send_orders = 0;
                *cut_con = 1;
                goto PDONE;
                break;
            case 0x0011:
                send_hb(pc);
                goto NO_LOGGIN;
                break;
            case 0x0001:
                goto NO_LOGGIN;
                break;
            case 0x0021:
                record_raw_message(ob->o, pc->logon->name, pc->logon->nlen,
                        d_off, mess_len,0x0);
                ob->can_send_orders = 1;
                rc->connection_notice(pc, 1, NULL);
                goto NO_LOGGIN;
                break;
            default:
                    record_raw_message(ob->o, pc->logon->name, pc->logon->nlen,
                        d_off, mess_len, 0x0);
                    goto NO_LOGGIN;
                break;
        }
       record_dma_message(ob->o, "OATS", 4,  
                       d_off, mess_len,
                       utp_cs_parse_oats,
                       ob);
NO_LOGGIN:
        bytes_used += mess_len;
        d_off += mess_len;
        len_left -= mess_len;
        if (add_seq) {
            add_seq = 0;
            record_inbound_message(ob->o,1, pc->logon->name, pc->logon->nlen);
        }
    }
PDONE:
    rc->con->rd_ptr += bytes_used;
}

static void init_logon(dutp* dp, rex_con* rc, char* message, int mlen, int* cut_con)
{
    con_int *ob = (con_int *) dp->pbp;
    char* logname = message + 12;
    int log_len = get_utp_field_len(logname, 12);
    dart_order_obj* doj = get_gk_obj(ob->gk);
    set_rom_field(doj, ROM_PRICE, logname, log_len);
    set_rom_field(doj, ROM_TRADER, logname, log_len);
    login_info* li = validate_logon(ob, doj);
    if(check_is_connected(ob->p, logname,log_len)) {
        *cut_con = 1;
        return;
    }
    if(li) {
        rc->logon = li;
        con_int* ci = clone_interface(ob, doj);
        ci->rc = rc;
        ci->logon = li;
        set_new_connection(doj, ci);
        rc->parser = ci->cparser;
        int bytes_sent = gen_send(rc, message, mlen,
                                  cut_con);
        if(bytes_sent < mlen) {
            /*This is a logon so if we are getting stuffed give up.*/
            *cut_con = 1;
        }
    } else {
        *cut_con = 1;
        //send_utp_logout(dp, ob);
    }
}

static rcntr*  trans_new_ord(con_int* ci, void* d_off, int mlen)
{
    rcntr* cnt = get_cnt(ci->gk);
    cnt->raw = d_off;
    cnt->rlen = mlen;
    utpno* new_ord = d_off;
    cnt->cum = 0;
    cnt->last = 0;
    cnt->size = new_ord->qty;
    cnt->price = new_ord->price;
    cnt->size = ntohl(cnt->size);
    cnt->price = ntohl(cnt->price);
    char* sym = new_ord->sym;
    char* clordid = new_ord->clordid;
    int clordid_len = get_utp_field_len(new_ord->clordid, 17);
    int sym_len = get_utp_field_len(sym, 11);
    memcpy(cnt->sym, sym, sym_len);
    cnt->slen = sym_len;
    cnt->sec_type = 'E';
    cnt->side = new_ord->side;
    memcpy(cnt->owner, ci->logon->name, ci->logon->nlen);
    cnt->olen = ci->logon->nlen;
    no_copy_insert(ci->cl_ord_id_map, clordid, clordid_len, cnt);
    con_int* dest = 0;
    get_destination(ci->gk, 0, 0, (void **) &dest);
    if(dest && !dest->is_blocked && dest->connected) {
        no_copy_insert(dest->cl_ord_id_map, clordid, clordid_len, cnt);
    }
    return cnt;
}
static void create_n_send_rej(con_int* ci, dutp* up, char rr,
                              void* d_off, int mess_len, rcntr* cnt)
{
    void* mem_one = calloc(1, 116);
    utprej* rej = mem_one;
    rej->len = 116;
    rej->len = htons(rej->len);
    rej->type = 0x00F1;
    rej->type = htons(rej->type);
    rej->reason = (short)3000;
    rej->reason = htons(rej->reason);
    rej->rej_msg_type = rr;
    if(cnt->err_len > 0) {
        int tlen = cnt->err_len > 40 ? 40 : cnt->err_len;
        memcpy(rej->text, cnt->errStr, tlen);
    }
    if(rr == '1') {
        utpno* nord = d_off;
        memcpy(rej->account,nord->account, 5);
        int clordid_len = get_utp_field_len(nord->clordid, 17);
        memcpy(rej->clordid,nord->clordid, clordid_len);
        memcpy(rej->orig_clordid,nord->clordid, clordid_len);
        memcpy(rej->target, nord->sender_sub, 5);
    } else {
        utpcnl* nord = d_off;
        memcpy(rej->account,nord->account, 5);
        int clordid_len = get_utp_field_len(nord->clordid, 17);
        memcpy(rej->clordid,nord->clordid, clordid_len);
        clordid_len = get_utp_field_len(nord->orig_clordid, 17);
        memcpy(rej->orig_clordid,nord->orig_clordid, clordid_len);
        memcpy(rej->target, nord->sender_sub, 5);
    }
    direct_send(ci,  up, mem_one, 116);
    record_dma_message(ci->o, "OATS", 4,  
                       mem_one, 116,
                       utp_cs_parse_oats,
                       ci);
    free(mem_one);
}

static void no_risk_rej(char* rej, int rlen, con_int* ob,
        char* d_off, int dlen, dutp* dp, char rr)
{
    record_dma_message(ob->o, "OATS", 4,  
                       d_off, dlen,
                       utp_cs_parse_oats,
                       ob);
    rcntr* cnt = get_cnt(ob->gk);
    memcpy(cnt->errStr, rej, rlen);
    cnt->err_len = 20;
    create_n_send_rej(ob, dp, rr, d_off, dlen, cnt);
    return_cnt(ob->gk, cnt);
}

static void parse_ss_utpdirect(int* cut_con, rex_con* rc)
{
    short ord_type = 0;
    char* d_off = rc->con->rd_ptr;
    long len = (rc->con->wr_ptr - rc->con->rd_ptr);
    unsigned long bytes_used = 0;
    unsigned short mess_len = 0;
    long len_left = (long) len;
    int worked = 0;
    dutp* dp = (dutp*)rc->parser;
    dp->rc = rc;
    con_int *ob = (con_int *) dp->pbp;
    do {
        memcpy(&ord_type, d_off, 2);
        ord_type = ntohs(ord_type);
        memcpy(&mess_len, d_off + 2, 2);
        mess_len = ntohs(mess_len);
        if(mess_len > len_left) {
            goto PARSE_DONE;
        }
        switch (ord_type) {
            case 0x0021:
                init_logon(dp, rc, d_off, mess_len, cut_con);
            default:
                goto NO_RECORD;
            case 0x0041:
                {
                    if (ob && !ob->is_blocked && ob->connected) {
                        rcntr* cnt = trans_new_ord(ob, d_off, mess_len);
                        worked = ob->on_enter_order(ob, cnt);
                        utpno* new_ord = (void*)d_off;
                        int cplen = cnt->olen <= 5 ? cnt->olen : 5;
                        memcpy(new_ord->sender_sub, cnt->owner, cplen);
                        if(worked < 0) {
                            record_dma_message(ob->o, "OATS", 4,  
                                           d_off, mess_len,
                                           utp_cs_parse_oats,
                                           ob);
                            create_n_send_rej(ob, dp, '1', d_off, mess_len, cnt);
                            int clordid_len = get_utp_field_len(new_ord->clordid, 17);
                            client_side_remove_from_maps(ob, new_ord->clordid,
                                                 clordid_len,  cnt);
                            goto NO_RECORD;
                        }
                    } else {
                        int cplen = ob->logon->nlen <= 5 ? ob->logon->nlen : 5;
                        utpno* new_ord = (void*)d_off;
                        memcpy(new_ord->sender_sub, ob->logon->name, cplen);
                        record_dma_message(ob->o, "OATS", 4,  
                                           d_off, mess_len,
                                           utp_cs_parse_oats,
                                           ob);
                        if(ob->is_blocked) {
                            no_risk_rej("blocked", 7, ob,
                                        d_off, mess_len, dp, '1');
                        } else {
                            no_risk_rej("Not connected", 13, ob,
                                        d_off, mess_len, dp, '1');
                        }
                        goto NO_RECORD;
                    }
                }
                break;
            case 0x0061:
                {
                    utpcnl* cnl = (void*)d_off;
                    int clordid_len = get_utp_field_len(cnl->orig_clordid, 17);
                    rcntr* cnt = 0;
                    utp_get_obj_by_id(ob, cnl->orig_clordid, clordid_len, &cnt);
                    if(cnt) {
                        int worked = 1;
                        cnt->raw = d_off;
                        cnt->rlen = mess_len;
                        worked = ob->on_cancel_order(ob, cnt);
                        int cplen = cnt->olen <= 5 ? cnt->olen : 5;
                        memcpy(cnl->sender_sub, cnt->owner, cplen);
                        if(worked <= 0) {
                            record_dma_message(ob->o, "OATS", 4,  
                                           d_off, mess_len,
                                           utp_cs_parse_oats,
                                           ob);
                            create_n_send_rej(ob, dp, '2', d_off, mess_len, cnt);
                            goto NO_RECORD;
                        }
                    } else {
                        int cplen = ob->logon->nlen <= 5 ? ob->logon->nlen : 5;
                        memcpy(cnl->sender_sub, ob->logon->name, cplen);
                        no_risk_rej("Could not find order", 20, ob,
                                    d_off, mess_len, dp, '2');
                        goto NO_RECORD;
                    }
                }
                break;
        };
        record_dma_message(ob->o, "OATS", 4,  
                       d_off, mess_len,
                       utp_cs_parse_oats,
                       ob);
NO_RECORD:
        len_left -= mess_len;
        d_off += mess_len;
        bytes_used += mess_len;
    } while(bytes_used < len);
PARSE_DONE:
    rc->con->rd_ptr += bytes_used;
}

void utpd_reset_seq(con_int* ci, int out, int in)
{
    /*TODO Add reset to dart_parser*/
}
void create_connection(con_int * ci, char *filename, int name_len,
        gatekeeper * gk)
{
    ci->cl_ord_id_map = get_initial_order_map(gk, ci->mod_name,
                                              ci->mod_name_len);
    ci->sbm = 0;
    ci->tg = 0;
    if(ci->is_server) {
        ci->cl_ord_id_map = get_initial_order_map(gk, ci->mod_name,
                                              ci->mod_name_len);
        ci->sbm = 0;
        ci->tg = 0;
        ci->gk = gk;
        ci->send_to_con = utpd_send;
        ci->reset_seq = utpd_reset_seq;
        ci->cleansing_rinse = utpd_cleansing_rinse;
        ci->can_send_orders = 0;
        ci->on_rej = utpd_send;
        ci->on_replaced = utpd_send;
        ci->on_out = utpd_send;
        ci->on_fill = utpd_send;
        ci->on_update = utpd_send;
        add_int_orderids(ci);
        dutp* uparse = create_dma_utp_parser(ci->o, ci, NULL);
        rex_con* rc = add_listener(ci->p, ci->ex, uparse, ci,
                                         utp_dma_write_callback, 
                                         utp_listener_connect,
                                         parse_ss_utpdirect,
                                         validate_logon);
        ci->cparser = uparse;
        ci->rc = rc;
    } else {
        ci->on_enter_order = utpd_wrapper;
        ci->on_replace_order = utpd_wrapper;
        ci->on_cancel_order = utpd_wrapper;
        ci->cleansing_rinse = utpd_cleansing_rinse;
        ci->can_send_orders = 0;
        add_int_orderids(ci);
        login_info* li = build_logon_info_from_config(ci->ex);
        dutp* uparse = create_dma_utp_parser(ci->o, ci, li);
        rex_con* rc = create_rex_con(ci->p, ci->ex, uparse, uparse,li,
                                      utp_dma_write_callback, 
                                      utp_client_connect,
                                      parse_con_utpdirect, NULL);
        rc->parser = uparse;
        rc->connect = send_utp_logon;
        uparse->rc = rc;
        ci->rc = rc;
        rex_connect(ci->p, rc);
    }
}
