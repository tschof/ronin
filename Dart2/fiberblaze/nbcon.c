#include <signal.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

#include "nbcon.h"
#include "mailer.h"
#include "ex_config_helper.h"
#include "utp_oats_parser.h"
#include "oats.h"
#include "connection_interface.h"
#include "polling_server.h"
#include "rex_seq_request.h"
#include "rex_allocator.h"
#include "connection_factory.h"
#include "direct_router.h"
#include "balance_checker.h"
#include "lifecycle_master.h"
#include "log_mnger.h"
#include "rex_thr_pool.h"
#include "dart_handler.h"
#include "fb.h"

struct dart_nb_parser {
    otl* logger;
    login_info* logon;
    con_int* nyse;
    dnbcon* dnbc;
    gatekeeper* gk;
    thr_pool_t* thr;
    databuf_t* mess_to_send;
    pthread_spinlock_t send_mutex;
    call_back_wrapper* recon_wrap;
    int in_seq;
    int out_seq;
    int sock;
    int is_logged_on;
    int should_run_recv;
    struct fb_context *nyse_ctx;
    volatile uint8_t cur_bucket;
    volatile uint32_t* sendReg;
};

struct dart_nb_cntr_structs {
    jp_limits_checker* jp;
    gatekeeper* gk;
    ex_config* mc;
    con_int* oats;
    pserver* ass;
    cycle_master* cm;
    dnbparse* nbp;
};

void send_nb_logon(void* dd);
void run_nb_recv_loop(void* v);
static void nb_direct_send(con_int* ci, dnbparse* dnbp, void* data, int len)
{
    pthread_spin_lock(&dnbp->send_mutex);
    memcpy((struct fb_bucket*)fb_get_bucket(dnbp->nyse_ctx, dnbp->cur_bucket), data, len);
    *(dnbp->sendReg) = fb_get_bucket_hwaddr(dnbp->nyse_ctx, dnbp->cur_bucket) | len;
    if(++dnbp->cur_bucket == FB_NO_BUCKETS) { 
        while( *(dnbp->sendReg) != 0 )
            ;
        dnbp->cur_bucket = 0;
    }
    pthread_spin_unlock(&dnbp->send_mutex);
}
static void nb_send_helper(con_int* ci, rcntr* risk)
{
    dnbparse* up = (dnbparse*)ci->cparser;
    nb_direct_send(ci, up, risk->raw, risk->rlen);
}

static void nb_sender(dnbparse* up, databuf_t* buff)
{
    nb_direct_send(up->nyse, up, buff->rd_ptr, databuf_unread(buff));
}
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
int utp_get_obj_by_id(con_int * ob, void *clordid, int len,
                  rcntr ** obj)
{
    int ret = 0;
    pthread_spin_lock(&ob->mutex);
    ret = get_map_entry(ob->cl_ord_id_map, clordid, len, (void **) obj);
    pthread_spin_unlock(&ob->mutex);
    return ret;
}
static dnbparse* create_nb_parser(otl* logger, con_int* pbp, login_info* u, dnbcon* dnbc)
{
    int ret = 0;
    dnbparse* fp = 0;
    REX_ALLOC(fp, 8, sizeof(struct dart_nb_parser), ret);
    fp->logger = logger;
    fp->nyse = pbp;
    fp->logon = u;
    fp->dnbc = dnbc;
    fp->gk = dnbc->cntr->gk;
    fp->mess_to_send = databuf_alloc(1024);
    pthread_spin_init(&fp->send_mutex, PTHREAD_PROCESS_PRIVATE);
    fp->is_logged_on = 0;
    pbp->cparser = fp;
    return fp;
}

static void send_hb(dnbparse *con)
{
    databuf_t *buff = get_databuf(con->gk);
    short mt = htons(0x001);
    short ml = htons(8);
    databuf_memcpy(buff, &mt, 2);
    databuf_memcpy(buff, &ml, 2);
    *buff->wr_ptr++ = '\0';
    *buff->wr_ptr++ = '\0';
    *buff->wr_ptr++ = '\0';
    *buff->wr_ptr++ = '\0';
    nb_sender(con, buff);
    destroy_gk_db(con->gk, buff);
}

static void nb_on_out(dnbparse* ci, void* d_off, int mlen)
{
    utpout* cnl = d_off;
    int clordid_len = get_utp_field_len(cnl->orig_clordid, 17);
    if(clordid_len > 0) {
        rcntr* cnt = 0;
        utp_get_obj_by_id(ci->nyse, cnl->orig_clordid, clordid_len, &cnt);
        if(cnt) {
            cnt->raw = d_off;
            cnt->rlen = mlen;
            jp_limits_checker *jlc =
                (jp_limits_checker *) get_balance_struct(ci->gk);
            update_balance(jlc, cnt);
            ci->dnbc->on_out(cnl);
            int cplen = cnt->olen <= 5 ? cnt->olen : 5;
            memcpy(cnl->target, cnt->owner, cplen);
            utp_remove_from_maps(ci->nyse, cnl->orig_clordid, clordid_len, cnt);
        }
    }
}


void from_oats_on_fill(con_int* ci, rcntr * risk)
{
}
void from_oats_on_rej(con_int* ci, rcntr * risk)
{
}
void from_oats_on_out(con_int* ci, rcntr * risk)
{
}

static int nb_on_rej(dnbparse* ci, void* d_off, int mlen)
{
    int ret = 0;
    utprej* rej = d_off;
    int clordid_len = get_utp_field_len(rej->clordid, 17);
    if(clordid_len > 0) {
        rcntr* cnt = 0;
        utp_get_obj_by_id(ci->nyse, rej->clordid, clordid_len, &cnt);
        if(cnt) {
            switch (rej->rej_msg_type) {
                case '1':
                    {
                        jp_limits_checker *jlc =
                            (jp_limits_checker *) get_balance_struct(ci->gk);
                        update_balance(jlc, cnt);
                    }
                    break;
                default:
                    break;
            }
            ci->dnbc->on_rej(rej);
            int cplen = cnt->olen <= 5 ? cnt->olen : 5;
            memcpy(rej->target, cnt->owner, cplen);
            if(rej->rej_msg_type == '1') {
                utp_remove_from_maps(ci->nyse, rej->clordid, clordid_len, cnt);
            }
        }
    }
    return ret;
}
   /* send_biz_message(buff->buffer, databuf_unread(buff), title, t_len);
}*/
static void nb_on_fill(dnbparse* ci, void* d_off, int mlen)
{
    utpfill* ufill = d_off;
    int clordid_len = get_utp_field_len(ufill->clordid, 17);
    int last = ufill->last;
    last = ntohl(last);
    if(clordid_len > 0) {
        rcntr* cnt = 0;
        utp_get_obj_by_id(ci->nyse, ufill->clordid, clordid_len, &cnt);
        if(cnt) {
            cnt->raw = d_off;
            cnt->rlen = mlen;
            cnt->last = last;
            cnt->cum += last;
            if(cnt->cum < cnt->size && ufill->leaves == 0) {
                ufill->leaves = cnt->size - cnt->cum;
            }
            jp_limits_checker *jlc =
                (jp_limits_checker *) get_balance_struct(ci->gk);
            update_balance_fill(jlc, cnt);
            ci->dnbc->on_fill(ufill);
            int cplen = cnt->olen <= 5 ? cnt->olen : 5;
            memcpy(ufill->target, cnt->owner, cplen);
            if(ufill->leaves == 0 && cnt->cum >= cnt->size) {
                utp_remove_from_maps(ci->nyse, ufill->clordid, clordid_len, cnt);
            }
        } else {
            /*TODO need to build fill not found mail*/
        }
    }
}
static void parse_vfill_message(dnbparse* ci, void* d_off, int mlen)
{
    utpvfill* ufill = d_off;
    int clordid_len = get_utp_field_len(ufill->clordid, 17);
    int last = ufill->last;
    last = ntohl(last);
    if(clordid_len > 0) {
        rcntr* cnt =  0;
        utp_get_obj_by_id(ci->nyse, ufill->clordid, clordid_len, &cnt);
        if(cnt) {
            cnt->raw = d_off;
            cnt->rlen = mlen;
            cnt->last = last;
            cnt->cum += last;
            if(cnt->cum < cnt->size && ufill->leaves == 0) {
                ufill->leaves = cnt->size - cnt->cum;
            }
            jp_limits_checker *jlc =
                (jp_limits_checker *) get_balance_struct(ci->gk);
            update_balance_fill(jlc, cnt);
            ci->dnbc->on_vfill(ufill);
            //ci->on_fill(ci, cnt);
            int cplen = cnt->olen <= 5 ? cnt->olen : 5;
            memcpy(ufill->target, cnt->owner, cplen);
            if(ufill->leaves == 0 && cnt->cum >= cnt->size) {
                utp_remove_from_maps(ci->nyse, ufill->clordid, clordid_len, cnt);
            }
        } else {
            /*TODO need to build fill not found mail*/
        }
    }
}

static void nb_fb_connect(void* dnbp);
static void fb_client_connect(dnbparse* uparse, int is_connected,
        dart_order_obj* log)
{
    con_int* ci = uparse->nyse;
    switch(is_connected) {
        case 0:
            uparse->should_run_recv = 0;
            ci->can_send_orders = 0;
            ci->connected = 0;
            uparse->is_logged_on = 0;
            reload_config(ci->ex);
            if(uparse->recon_wrap == 0) {
                uparse->recon_wrap = add_timer(ci->p,0,
                        0,30,0,
                        uparse,
                        nb_fb_connect);
            } else {
                add_recon_timer(ci->p, uparse->recon_wrap);
            }
            break;
        case 1:
        case 2:
            ci->can_send_orders = 0;
            ci->connected = 1;
            uparse->is_logged_on = 1;
            break;
    }
    ci->update_oats(ci, is_connected);
    uparse->dnbc->on_connect(is_connected);
}
static void nb_fb_connect(void* v)
{
    dnbparse* dnbp = (dnbparse*)v;
    con_int* ci = dnbp->nyse;
    dnbp->should_run_recv = 1;
    if ( fb_init(&dnbp->nyse_ctx, NULL) ) {
            fprintf(stderr, "Can't initaliaze card! Is the driver loaded?\n");
        dnbp->should_run_recv = 0;
    }

    if (fb_allocate_tcp_channel( dnbp->nyse_ctx, 0, -1)) {
        fprintf(stderr, "Can't allocate tcp channel\n");
        dnbp->should_run_recv = 0;
        fb_destroy( dnbp->nyse_ctx);
    }
    dnbp->sendReg = fb_send_register(dnbp->nyse_ctx );
    dnbp->cur_bucket = 0;
    struct in_addr ip;
    inet_aton(ci->ex->ip, &ip);
    if ( fb_connect(dnbp->nyse_ctx, ip,ci->ex->port , 0, 0) ) {
        fprintf(stderr, "Can't connect to port 2000. Are the loopback module inserted?\n");
        fb_client_connect(dnbp, 0, NULL);
        dnbp->should_run_recv = 0;
        fb_destroy( dnbp->nyse_ctx);
    }
    if(dnbp->should_run_recv) {
        start_job(ci->p, run_nb_recv_loop, dnbp);
        send_nb_logon(dnbp);
    }
}
uint16_t parse_con_nb(int *cut_con,dnbparse  *rc,
        const uint8_t *c_off,uint16_t len)
{
    char* d_off = (char*)c_off;
    short ord_type = 0;
    uint16_t bytes_used = 0;
    unsigned short mess_len = 0;
    uint16_t len_left = len;
    int should_parse = 1;
    int add_seq = 0;
    con_int*ob = (con_int*) rc->nyse;
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
                {
                    add_seq = 1;
                    utpack* ack = (utpack*)d_off;
                    int cplen = ob->logon->nlen <= 5 ? ob->logon->nlen : 5;
                    memcpy(ack->target, ob->logon->name, cplen);
                    rc->dnbc->on_ack(ack);
                }
                break;
            case 0x00A1:
                add_seq = 1;
                goto NO_LOGGIN;
                break;
            case 0x00D1:
                add_seq = 1;
                nb_on_out(rc, d_off, mess_len);
                break;
            case 0x00B1:
                add_seq = 1;
                break;
            case 0x00E1:
                add_seq = 1;
                break;
            case 0x00F1:
                add_seq = 1;
                nb_on_rej(rc, d_off, mess_len);
                break;
            case 0x0081:
                add_seq = 1;
                nb_on_fill(rc, d_off, mess_len);
                break;
            case 0x0101:
                add_seq = 1;
                //no_parse_send(ob, d_off, mess_len);
                break;
            case 0x00C1:
                add_seq = 1;
                parse_vfill_message(rc, d_off, mess_len);
                break;
            case 0x0141:
                bytes_used = mess_len;
                should_parse = 0;
                record_raw_message(ob->o, rc->logon->name, rc->logon->nlen,
                        d_off, len, 0x0);
                send_debug_message(ob->o, "Logon Rejected: %s \n", d_off + 18);
                ob->can_send_orders = 0;
                fb_client_connect(rc, 0, NULL);
                *cut_con = 1;
                goto PDONE;
                break;
            case 0x0011:
                send_hb(rc);
                goto NO_LOGGIN;
                break;
            case 0x0001:
                goto NO_LOGGIN;
                break;
            case 0x0021:
                record_raw_message(ob->o, rc->logon->name, rc->logon->nlen,
                        d_off, mess_len,0x0);
                ob->can_send_orders = 1;
                rc->is_logged_on = 1;
                fb_client_connect(rc, 1, NULL);
                goto NO_LOGGIN;
                break;
            default:
                record_raw_message(ob->o, rc->logon->name, rc->logon->nlen,
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
            record_inbound_message(ob->o,1, rc->logon->name, rc->logon->nlen);
        }
    }
PDONE:
    return bytes_used;
    //rc->con->rd_ptr += bytes_used;
}
void run_nb_recv_loop(void* v)
{
    dnbparse* dnbp = (dnbparse*)v;
    fb_packet *incomingPacket = 0;
    fb_packet *prev = 0;
    uint64_t total_recv = 0;
    while (dnbp->should_run_recv) {
        incomingPacket = fb_busy_poll_for_next_packet(dnbp->nyse_ctx, prev);
        if (incomingPacket->len > 0)
        {
            uint16_t payload_length = fb_get_tcp_payload_length( incomingPacket ) ;
            total_recv += payload_length;
            if (payload_length > FB_BUCKET_SIZE )
                payload_length = FB_BUCKET_SIZE;
            int cut_con = 0; 
            const uint8_t* data = fb_get_tcp_payload(incomingPacket);
            uint16_t used = parse_con_nb(&cut_con,dnbp,data, payload_length);
            if(used < payload_length) {
                databuf_memcpy(dnbp->mess_to_send,data + used, payload_length - used) ;
            }
            // Copy the received data to bucket 0
            //memcpy((void*)echo_bucket->data, fb_get_tcp_payload( incomingPacket ), payload_length );
            // Echo back the received data
            // [11:0] payload len ; [31:12] address for 4k boundary (PAGE_SIZE)
            // Tell the FPGA that this packet has been processed
            if( prev ) {
                fb_update_recv_ptr(dnbp->nyse_ctx, prev);
            }
            if(cut_con) {
                dnbp->should_run_recv = 0;
            }
        }

        prev = incomingPacket;
    }
}

void send_nb_logon(void* dd)
{
    dnbparse *pa = (dnbparse*)dd;
    pa->in_seq = 1;
    pa->nyse->sock = pa->sock;//read_wrapper->sock;
    sequence_req *sr = create_sequence_request(
            pa->logon->name, pa->logon->nlen,
            pa->logon,pa->nyse->ex->version,
            build_offsets,
            create_rom_resend_pad, pa->nyse);
    get_sequence_numbers(sr, pa->nyse->o);
    //add the sequences intor the connection object;
    int temp_in = dsr_get_incoming(sr);
    if(temp_in == 0) {
        temp_in = -1;
    }
    send_debug_message(pa->nyse->o, "%d, incoming messages recorded.\n", temp_in);
    pa->in_seq = dsr_get_incoming(sr);
    destroy_sequence_req(sr);
    databuf_t *logon = databuf_alloc(60);
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
    databuf_memcpy(logon, pa->nyse->mod_name, pa->nyse->mod_name_len);
    int cp_len = pa->nyse->mod_name_len;
    for (; cp_len < 44; ++cp_len) {
        *logon->wr_ptr++ = '\0';
    }
    *logon->wr_ptr++ = '1';
    *logon->wr_ptr++ = '\0';
    *logon->wr_ptr++ = '\0';
    *logon->wr_ptr++ = '\0';
    nb_sender(pa, logon);
    databuf_destroy(logon);
}

void nb_log_on_to_oats(con_int* ab, int is_connected)
{ 
    if(ab->mod_name_len > 0) {
        send_debug_message(ab->o, "Connection: %s, is connected: %d ?\n", ab->mod_name,
                is_connected);
        if(is_connected) {
            gk_add_connection(ab->gk, ab);
        } 
    }
    if(ab && ab->logon && ab->logon->nlen > 0) {
        set_obj_by_client_id(ab->gk, ab->logon->name, ab->logon->nlen, ab);
        send_debug_message(ab->o, "Connection: %s, is connected: %d ?\n", ab->logon->name,
                is_connected);
    }
    con_int* oats = get_compliance_con(ab->gk);
    if(oats) {
        //send_oats_update(oats, is_connected, ab->mod_name, ab->mod_name_len, 1, ab);
        if(ab && ab->rc && ab->rc->logon) {
            send_oats_update(oats, is_connected, ab->rc->logon->name, ab->rc->logon->nlen, 1, ab);
        }
    }
}
void nb_log_to_oats_dest(con_int* ab, int is_connected)
{
    if(ab->mod_name_len > 0) {
        send_debug_message(ab->o, "Connection: %s,%s is connected: %d ?\n", ab->mod_name,
                ab->ex->dest_id, is_connected);
    }
    if(is_connected) {
        gk_add_connection(ab->gk, ab);
    }
    con_int* oats = get_compliance_con(ab->gk);
    if(oats) {
        send_oats_update(oats, is_connected, ab->ex->dest_id, ab->ex->dest_id_len, 0, ab);
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
uint8_t send_new_order(dnbcon* dnb, utpno* uno)
{
    uint8_t worked = 1;
    if(!dnb->dnbp->nyse->is_blocked && dnb->dnbp->nyse->connected) {
        rcntr* cnt = trans_new_ord(dnb->dnbp->nyse, uno, sizeof(utpno));
        jp_limits_checker *jlc =
            (jp_limits_checker *) get_balance_struct(dnb->cntr->gk);
        int allowed = update_balance_new(jlc, cnt);
        if (allowed) {
            nb_send_helper(dnb->dnbp->nyse, cnt);
            int cplen = cnt->olen <= 5 ? cnt->olen : 5;
            memcpy(uno->sender_sub, cnt->owner, cplen);
            record_dma_message(dnb->dnbp->nyse->o, "OATS", 4,  
                       (char*)uno, sizeof(utpno),
                       utp_cs_parse_oats,
                       dnb->dnbp->nyse);
        } else {
            worked =  -1;
            send_debug_message(dnb->dnbp->nyse->o, "Order Rejected: %s \n", uno->clordid);
        }
    } else {
        worked =  -1;
        send_debug_message(dnb->dnbp->nyse->o,
                "Order Rejected: Are we connected? %d  blocked? %d \n",
               dnb->dnbp->nyse->connected,
               dnb->dnbp->nyse->is_blocked);
    }
    return worked;
}
uint8_t send_cancel(dnbcon* dnb, utpcnl* cnl)
{
    uint8_t allowed = 1;
    int clordid_len = get_utp_field_len(cnl->orig_clordid, 17);
    rcntr* cnt = 0;
    utp_get_obj_by_id(dnb->dnbp->nyse, cnl->orig_clordid, clordid_len, &cnt);
    if(cnt) {
        jp_limits_checker *jlc =
            (jp_limits_checker *) get_balance_struct(dnb->cntr->gk);
        int allowed = check_trader_msg_rate(jlc, cnt->owner, cnt->olen, 'E');
        cnt->raw = cnl;
        cnt->rlen = sizeof(utpcnl);
        if(dnb->dnbp->nyse->connected && allowed) {
            nb_send_helper(dnb->dnbp->nyse, cnt);
            int cplen = cnt->olen <= 5 ? cnt->olen : 5;
            memcpy(cnl->sender_sub, cnt->owner, cplen);
            record_dma_message(dnb->dnbp->nyse->o, "OATS", 4,  
                       (char*)cnl, sizeof(utpcnl),
                       utp_cs_parse_oats,
                       dnb->dnbp->nyse);
        } else {
            allowed = -1;
            send_debug_message(dnb->dnbp->nyse->o,
                    "Order Rejected: %s orig: %s, connected? %d, blocked? %d\n",
                    cnl->clordid,
                    cnl->orig_clordid,
                    dnb->dnbp->nyse->connected,
                    dnb->dnbp->nyse->is_blocked);
        }
    } else {
        allowed = -1;
        send_debug_message(dnb->dnbp->nyse->o,
                "Could not find orig order, CNL Rejected: %s orig: %s, \n",
                cnl->clordid,
                cnl->orig_clordid);
    }
    return allowed; 
}

static con_int* create_oats(char* filename, int flen,
        dnbcntr* cntr)
{
    con_int* oats = create_empty_interface();
    oats->ex = init_config(filename, flen);
    oats->mod_name = calloc(oats->ex->ulen + 1, 1);
    memcpy(oats->mod_name, oats->ex->username, oats->ex->ulen);
    oats->mod_name_len = oats->ex->ulen;
    oats->o = get_logger(cntr->ass);
    oats->p = cntr->ass;
    oats->gk = cntr->gk;
    oats->update_oats = nb_log_on_to_oats;
    oats->on_cancel_all = 0x0;
    oats->from_oats_update = 0x0;
    create_oats_connection(oats, filename, flen, cntr->gk);
    oats->on_fill = from_oats_on_fill;
    oats->on_rej = from_oats_on_rej;
    oats->on_out = from_oats_on_out;
    return oats;
}
login_info* nb_build_logon_info_from_config(ex_config* ex, 
        char* user, int ulen)
{

    login_info* li = 0;
    int ret = 0;
    REX_ALLOC(li, 8, sizeof(login_info), ret);
    memcpy(li->name, user, ulen);
    memcpy(li->password, user, ulen);
    li->plen = ulen;
    li->last_logon = ex->last_logon;
    li->nlen = ulen;
    li->reset_hour = ex->reset_hour;
    li->reset_min = ex->reset_min;
    li->reset_day  = ex->reset_day;
    li->create_new = ex->create_new;
    li->new_incoming = ex->new_incoming;
    li->new_outgoing = ex->new_outgoing;
    return li;
}
static dnbparse* create_nyseb_con(dnbcon* dnbc, dnbcntr* cntr,
        char* config, int cfglen, char* user, int ulen)
{
    con_int* ci = create_empty_interface();
    ci->ex = init_config(config, cfglen);
    ci->mod_name = calloc(ci->ex->ulen + 1, 1);
    memcpy(ci->mod_name, ci->ex->username, ci->ex->ulen);
    ci->mod_name_len = ci->ex->ulen;
    ci->o = get_logger(cntr->ass);
    ci->gk = cntr->gk;
    ci->p = cntr->ass;
    ci->update_oats = nb_log_to_oats_dest;
    ci->can_send_orders = 0;
    ci->cl_ord_id_map = get_initial_order_map(cntr->gk, ci->mod_name,
            ci->mod_name_len);
    ci->sbm = 0;
    ci->tg = 0;

    add_int_orderids(ci);
    login_info* li = nb_build_logon_info_from_config(ci->ex, user, ulen);
    dnbparse* dnbp = create_nb_parser(ci->o, ci, li, dnbc);
    ci->logon = li;
    ci->cparser = dnbp;
    dnbc->dnbp = dnbp;
    dnbp->should_run_recv = 1;
    struct fb_context* ctx = 0;
    if ( fb_init(&ctx, NULL) ) {
            fprintf(stderr, "Can't initaliaze card! Is the driver loaded?\n");
    }
    dnbp->nyse_ctx = ctx;
    if (fb_allocate_tcp_channel( dnbp->nyse_ctx, 0, -1)) {
        fprintf(stderr, "Can't allocate tcp channel\n");
        fb_destroy( dnbp->nyse_ctx);
    }
    dnbp->sendReg = fb_send_register(dnbp->nyse_ctx );
    dnbp->cur_bucket = 0;
    struct in_addr ip;
    inet_aton(ci->ex->ip, &ip);
    if ( fb_connect(dnbp->nyse_ctx, ip,ci->ex->port , 0, 0) ) {
        fprintf(stderr, "Can't connect to port %s, %d\n", ci->ex->ip, ci->ex->port);
        fb_client_connect(dnbp, 0, NULL);
        dnbp->should_run_recv = 0;
        fb_destroy( dnbp->nyse_ctx);
    } else {
       dnbp->nyse->connected = 1;
    }
    return dnbp;
}

dnbcon* start_dart(char* password, int plen,
        char* user, int ulen,
        char* config, int cfglen,
        void(*on_rej)(utprej* doj),
        void(*on_out)(utpout* doj),
        void(*on_fill)(utpfill* doj),
        void(*on_vfill)(utpvfill* doj),
        void(*on_ack)(utpack* doj),
        void(*on_cnl_ack)(utpcnlack* doj),
        void(*on_connect)(uint8_t is_connected))
{
    dnbcon* dbc = malloc(sizeof(dnbcon));
    dbc->cntr = malloc(sizeof(dnbcntr));
    dbc->cntr->mc = init_config(config, cfglen);

    int md_len = 0;
    char* mdsdat = get_config_val(dbc->cntr->mc, "SERVER", 6,
            "mds", 3, &md_len);
    int utp_len = 0;
    char* utpdat = get_config_val(dbc->cntr->mc, "SERVER", 6,
            "utp", 3, &utp_len);
    int oats_len = 0;
    char* oatdat = get_config_val(dbc->cntr->mc, "SERVER", 6,
            "oats", 4, &oats_len);
    time_t now;
    time(&now);
    char* router = "dest_route_tbl.map";
    dbc->cntr->ass = start_lib_base_services(dbc->cntr->mc);
    dbc->cntr->jp = create_jp_limits_checker(dbc->cntr->mc,
            get_logger(dbc->cntr->ass),
            mdsdat, md_len);
    sbrouter* dd = create_direct_router(get_logger(dbc->cntr->ass),
            dbc->cntr->mc->mod_name,
            dbc->cntr->mc->mod_name_len);
    dbc->cntr->mc->create_new = ll_before_reset_time(dbc->cntr->mc->reset_day,
            dbc->cntr->mc->reset_hour,
            dbc->cntr->mc->reset_min,
            dbc->cntr->mc->last_logon, now);

    dbc->cntr->gk = create_dd_gatekeeper(dd, get_logger(dbc->cntr->ass),dbc->cntr->ass,
            dbc->cntr->mc->mod_name, dbc->cntr->mc->mod_name_len,
            dbc->cntr->jp, dbc->cntr->mc->create_new,
            router);
    dbc->cntr->cm = create_gen_life_master(dbc->cntr->ass, dbc->cntr->mc);
    load_orders(dbc->cntr->jp, dbc->cntr->gk, dbc->cntr->mc->destinations);
    start_dart_mail_service(dbc->cntr->mc->mail_server, dbc->cntr->mc->mail_server_len,
            dbc->cntr->mc->biz_group, dbc->cntr->mc->b_len,
            dbc->cntr->mc->tech_group, dbc->cntr->mc->t_len,
            dbc->cntr->mc->end_hour, dbc->cntr->mc->end_min,
            dbc->cntr->mc->end_day);
    dbc->cntr->oats = create_oats(oatdat, oats_len, dbc->cntr);
    dbc->on_rej = on_rej;
    dbc->on_out = on_out;
    dbc->on_fill = on_fill;
    dbc->on_ack = on_ack;
    dbc->on_cnl_ack = on_cnl_ack;
    dbc->on_connect = on_connect;
    dbc->dnbp = create_nyseb_con(dbc, dbc->cntr,utpdat, utp_len, user, ulen);
    con_int* ci = create_empty_interface();
    ci->can_send_orders = 1;
    ci->connected = 1;
    ci->sock = 0;
    send_oats_update(dbc->cntr->oats, 1, user, ulen, 1, ci);
    if(dbc->dnbp->should_run_recv) {
        start_job(dbc->cntr->ass, run_nb_recv_loop, dbc->dnbp);
        send_nb_logon(dbc->dnbp);
    }
    return dbc;
}
