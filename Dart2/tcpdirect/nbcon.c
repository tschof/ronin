#include <signal.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <zf/zf.h>

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




struct dart_nb_parser {
    otl* logger;
    login_info* logon;
    con_int* nyse;
    dnbcon* dnbc;
    gatekeeper* gk;
    databuf_t* mess_to_send;
    pthread_spinlock_t send_mutex;
    call_back_wrapper* recon_wrap;
    int in_seq;
    int out_seq;
    int sock;
    int is_logged_on;
    databuf_t* data;
    jp_trader_info* trader;
    jp_limits* trader_lim;
    jp_limits* group_lim;
    struct zf_attr* zfatter;
    struct zf_stack* zack;
    struct zft_handle* zandle;
    struct zft* tcp;
    struct iovec* iov;
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
static int local_check_msg_rate(jp_limits* jp)
{
    ++jp->nmes;
    if(jp->nmes >= jp->mrate) {
        struct timeval tv;
        int ret_val = gettimeofday(&tv, 0);
        if(ret_val == 0) {
            unsigned long cur_micro = tv.tv_sec * 1000000;
            cur_micro += tv.tv_usec;
            if(cur_micro - jp->toff < 1000000) {
                return 0;
            } else {
                jp->toff = cur_micro;
                jp->nmes = 0;
            }
        } 
    }
    return 1;
}


static int add_to_limits(jp_limits_checker* jplc, dnbparse* dnbp, rcntr* rc)
{
    int allowed = 1;
    long dollars = rc->price * rc->size;
    jp_limits* jp = dnbp->trader_lim;
    jp_limits* gjp = dnbp->group_lim;
    if(is_blocked(dnbp->trader)) {
        return 0;
    }
    allowed = local_check_msg_rate(jp);
    if(allowed == 0) {
        rc->err_len = sprintf(rc->errStr, "Exceeds equity Message rate");
    }
    if ((jp->max_ord < (jp->open_ords + 1)) || (gjp->max_ord < gjp->open_ords + 1)) {
        rc->err_len = sprintf(rc->errStr, "Exceeds equity Max Open Orders");
        allowed = 0;
    }
    if ((jp->max_share < (jp->open_shares + rc->size)) ||
            (gjp->max_share < (gjp->open_shares + rc->size))) {
        rc->err_len = sprintf(rc->errStr, "Exceeds equity Max Open Shares" );
        allowed = 0;
    }
    if ((jp->max_doll < labs(dollars)) 
            || (gjp->max_doll < labs(dollars))) {
        rc->err_len = sprintf(rc->errStr, "Exceeds equity Max Dollar per Order" );
        allowed = 0;
    }
    if(rc->side == '1') {
        if ((jp->bp < (jp->cur_balance_buy + dollars)) ||
                (gjp->bp < (gjp->cur_balance_buy + dollars))) {
            rc->err_len = sprintf(rc->errStr, "Exceeds equity Max Balance" );
            allowed = 0;
        }
    } else {
        if ((jp->bp < (jp->cur_balance_sell + dollars)) ||
                (gjp->bp < (gjp->cur_balance_sell + dollars))) {
            rc->err_len = sprintf(rc->errStr, "Exceeds equity Max Balance");
            allowed = 0;
        }
    }
    if ((jp->max_shares_ord < rc->size) ||( gjp->max_shares_ord < rc->size)) {
        rc->err_len = sprintf(rc->errStr, "Exceeds Max Shares per Order");
        allowed = 0;
    }
    long a = rc->price % 100;
    if(rc->price > 10000 && a > 0) {
        rc->err_len = sprintf(rc->errStr, "Price has incorrect decimalization");
        allowed = 0;
    }
    if (is_exempt(dnbp->trader)) {
        allowed = 1;
    } else if (allowed == 0) {
        return allowed;
    }
    if(rc->side == '1') {
        jp->cur_balance_buy += dollars;
        gjp->cur_balance_buy += dollars;
    } else {
        jp->cur_balance_sell += dollars;
        gjp->cur_balance_sell += dollars;
    }
    /*printf("Balance update add: %ld, %ld, %ld ,price %ld,size %d\n", jp->cur_balance_buy, jp->cur_balance_sell, dollars,
      rc->price , rc->size);*/
    jp->open_ords += 1;
    gjp->open_ords += 1;
    jp->open_shares += rc->size;
    gjp->open_shares += rc->size;
    return allowed;
}

static void remove_from_limits(dnbparse* dnbp, rcntr* rc, int order,
        long dollars, int shares)
{
    jp_limits* jp = dnbp->trader_lim;
    jp_limits* gjp = dnbp->group_lim;
    if(rc->side == '1') {
        jp->cur_balance_buy -= dollars;
        gjp->cur_balance_buy -= dollars;
    } else {
        jp->cur_balance_sell -= dollars;
        gjp->cur_balance_sell -= dollars;
    }
    jp->open_ords -= order;
    gjp->open_ords -= order;
    jp->open_shares -= shares;
    gjp->open_shares -= shares;
    /*printf("Balance update remove: %ld, %ld, %ld,price %ld,size %d \n", jp->cur_balance_buy, jp->cur_balance_sell, dollars,
      rc->price , rc->size);*/
}

static void run_nb_recv_loop(dnbparse* dnbp);
static void send_nb_logon(void* dd);
static int nb_direct_send(con_int* ci, dnbparse* dnbp, void* data, int len)
{
    int error = 0;
    void* ld = data;
    int llen = len;
    int ts = 0;
    do {
        error = zft_send_single(dnbp->tcp,ld, llen,0);
        if(error <= 0) {
            send_debug_message(ci->o, "Failed to send %d \n", error); 
            break;
        } else {
            ts += error;
            ld += error;
            llen -= error;
        }
    }while(ts < len);
    zf_reactor_perform(dnbp->zack);
    return ts;
}
static int nb_send_helper(con_int* ci, rcntr* risk)
{
    int sent = 1;
    dnbparse* up = (dnbparse*)ci->cparser;
    if(databuf_unread(up->mess_to_send)) {
        send_debug_message(ci->o, "Have mess to send %d \n", databuf_unread(up->mess_to_send)); 
        databuf_memcpy(up->mess_to_send, risk->raw,
                risk->rlen);
        int total = nb_direct_send(ci, up, up->mess_to_send->rd_ptr,
                databuf_unread(up->mess_to_send));
        if(total < databuf_unread(up->mess_to_send)) {
            sent = 0;
        }
        up->mess_to_send->rd_ptr += total;
    } else {
        int total = nb_direct_send(ci, up, risk->raw, risk->rlen);
        if(total < risk->rlen) {
            sent = 0;
            databuf_memcpy(up->mess_to_send, risk->raw + total,
                    risk->rlen - total);
        }
    }
    return sent;
}

static void nb_sender(dnbparse* up, databuf_t* buff)
{
    if(databuf_unread(up->mess_to_send)) {
        send_debug_message(up->nyse->o, "Have mess to send %d \n", databuf_unread(up->mess_to_send)); 
        databuf_memcpy(up->mess_to_send, buff->rd_ptr,
                databuf_unread(buff));
        int total = nb_direct_send(up->nyse, up, up->mess_to_send->rd_ptr,
                databuf_unread(up->mess_to_send));
        up->mess_to_send->rd_ptr += total;
    } else {
        int total = nb_direct_send(up->nyse, up, buff->rd_ptr, databuf_unread(buff));
        if(total >= 0 && total < databuf_unread(buff)) {
            databuf_memcpy(up->mess_to_send, buff->rd_ptr + total,
                    databuf_unread(buff) - total);
        }
    }
}
int8_t flush_dart(dnbparse* up)
{
    pthread_spin_lock(&up->send_mutex);
    int total = nb_direct_send(up->nyse, up, up->mess_to_send->rd_ptr,
            databuf_unread(up->mess_to_send));
    up->mess_to_send->rd_ptr += total;
    pthread_spin_unlock(&up->send_mutex);
    return total;
}
void warm_dart(dnbparse* dnbp, void* data, int len)
{
    if(databuf_unread(dnbp->mess_to_send) > 0) {
        flush_dart(dnbp);
    } else {
        pthread_spin_lock(&dnbp->send_mutex);
        int ret = zft_send_single_warm(dnbp->tcp, data, len);
        if(ret == -EAGAIN) {
            zf_reactor_perform(dnbp->zack);
        }
        pthread_spin_unlock(&dnbp->send_mutex);
    }
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
    fp->data = databuf_alloc(1024);
    fp->iov = calloc(sizeof(struct iovec), 10);
    fp->zfatter = 0;
    fp->zack = 0;
    fp->zandle = 0;
    fp->tcp = calloc(1, sizeof(struct zft)); 
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
    int clordid_len = 8;//get_utp_field_len(cnl->orig_clordid, 17);
    rcntr* cnt = 0;
    utp_get_obj_by_id(ci->nyse, cnl->orig_clordid, clordid_len, &cnt);
    if(cnt) {
        cnt->raw = d_off;
        cnt->rlen = mlen;
        int64_t cur_size = cnt->size - cnt->cum;
        remove_from_limits(ci, cnt, 1, (cur_size * cnt->price), cur_size);
        int cplen = cnt->olen <= 5 ? cnt->olen : 5;
        memcpy(cnl->target, cnt->owner, cplen);
        utp_remove_from_maps(ci->nyse, cnl->orig_clordid, clordid_len, cnt);
    }
    ci->dnbc->on_out(cnl);
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
    int clordid_len = 8;//get_utp_field_len(rej->clordid, 17);
    rcntr* cnt = 0;
    utp_get_obj_by_id(ci->nyse, rej->clordid, clordid_len, &cnt);
    if(cnt) {
        int64_t cur_size = cnt->size - cnt->cum;
        switch (rej->rej_msg_type) {
            case '1':
                {
                    remove_from_limits(ci, cnt, 1,(cur_size * cnt->price), cur_size);
                }
                break;
            default:
                break;
        }
        int cplen = cnt->olen <= 5 ? cnt->olen : 5;
        memcpy(rej->target, cnt->owner, cplen);
        if(rej->rej_msg_type == '1') {
            utp_remove_from_maps(ci->nyse, rej->clordid, clordid_len, cnt);
        }
    }
    ci->dnbc->on_rej(rej);
    return ret;
}
/* send_biz_message(buff->buffer, databuf_unread(buff), title, t_len);
   }*/
static void nb_on_fill(dnbparse* ci, void* d_off, int mlen)
{
    utpfill* ufill = d_off;
    int clordid_len = 8;//get_utp_field_len(ufill->clordid, 17);
    long last = ufill->last;
    last = ntohl(last);
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
        int num_ords = ufill->leaves == 0 ? 1: 0;
        remove_from_limits(ci, cnt,num_ords,(last * cnt->price), last);
        ci->dnbc->on_fill(ufill);
        int cplen = cnt->olen <= 5 ? cnt->olen : 5;
        memcpy(ufill->target, cnt->owner, cplen);
        if(ufill->leaves == 0 && cnt->cum >= cnt->size) {
            utp_remove_from_maps(ci->nyse, ufill->clordid, clordid_len, cnt);
        }
    } else {
        ci->dnbc->on_fill(ufill);
        /*TODO need to build fill not found mail*/
    }
}
static void parse_vfill_message(dnbparse* ci, void* d_off, int mlen)
{
    utpvfill* ufill = d_off;
    int clordid_len = 8;//get_utp_field_len(ufill->clordid, 17);
    long last = ufill->last;
    last = ntohl(last);
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
        int num_ords = ufill->leaves == 0 ? 1: 0;
        remove_from_limits(ci, cnt,num_ords,(last * cnt->price), last);
        ci->dnbc->on_vfill(ufill);
        //ci->on_fill(ci, cnt);
        int cplen = cnt->olen <= 5 ? cnt->olen : 5;
        memcpy(ufill->target, cnt->owner, cplen);
        if(ufill->leaves == 0 && cnt->cum >= cnt->size) {
            utp_remove_from_maps(ci->nyse, ufill->clordid, clordid_len, cnt);
        }
    } else {
        ci->dnbc->on_vfill(ufill);
        /*TODO need to build fill not found mail*/
    }
}

static void nb_td_connect(void* dnbp);
static void fb_client_connect(dnbparse* uparse, int is_connected,
        dart_order_obj* log)
{
    con_int* ci = uparse->nyse;
    switch(is_connected) {
        case 0:
            ci->can_send_orders = 0;
            ci->connected = 0;
            uparse->is_logged_on = 0;
            reload_config(ci->ex);
            if(uparse->recon_wrap == 0) {
                uparse->recon_wrap = add_timer(ci->p,0,
                        0,30,0,
                        uparse,
                        nb_td_connect);
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
static void nb_td_connect(void* v)
{
    char portstr[6];
    dnbparse* dnbp = (dnbparse*)v;
    con_int* ci = dnbp->nyse;
    pthread_spin_lock(&dnbp->send_mutex);
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME;
    snprintf(portstr, sizeof(portstr), "%d", ci->ex->port);
    int adder_ret = getaddrinfo(ci->ex->ip, portstr, &hints, &res);
    if(adder_ret != 0) {
        send_debug_message(ci->o, "Connection Failed on getaddrinfo: %d, %s: %d\n",
                ci->ex->port, ci->ex->ip, adder_ret); 
    }
    if ( zft_connect(dnbp->zandle, res->ai_addr, res->ai_addrlen,&dnbp->tcp)!= 0 ) {
        fprintf(stderr,
                "Can't connect to port %d, error: %d \n",
                ci->ex->port, errno);
        dnbp->dnbc->on_connect(0);
    }
    while(zf_reactor_perform(dnbp->zack) == 0);
    send_nb_logon(dnbp);
    while(zf_reactor_perform(dnbp->zack) == 0);
    pthread_spin_unlock(&dnbp->send_mutex);
    run_non_block_recv_loop(dnbp);
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
static void run_nb_recv_loop(dnbparse* dnbp)
{
    int cut_con = 0;
    int block = 0;
    struct zft_msg zcr;
    zf_reactor_perform(dnbp->zack);
    do {
        zcr.iovcnt = 1;
        zft_zc_recv(dnbp->tcp,&zcr, 0);
        if(zcr.iovcnt == 0) {
            return;
        } 
        if(databuf_unread(dnbp->data) || zcr.iovcnt > 1) {
            int i = 0;
            for(;i < zcr.iovcnt; ++i) {
                databuf_memcpy(dnbp->data, zcr.iov[i].iov_base,
                        zcr.iov[i].iov_len);
            }
            uint16_t used = parse_con_nb(&cut_con,dnbp,(uint8_t*)dnbp->data->rd_ptr,
                    databuf_unread(dnbp->data));
            dnbp->data->rd_ptr += used;
        } else {
            uint16_t used = parse_con_nb(&cut_con,dnbp,zcr.iov[0].iov_base,
                    zcr.iov[0].iov_len);
            if(used < zcr.iov[0].iov_len) {
                databuf_memcpy(dnbp->data, zcr.iov[0].iov_base + used,
                        (zcr.iov[0].iov_len - used));
            }
        }
        block = zft_zc_recv_done(dnbp->tcp, &zcr);
    }while(block > 0 || zcr.pkts_left);
    if(block == -ECONNRESET || block == -EPIPE ||
            block == -ETIMEDOUT) {
        dnbp->nyse->can_send_orders = 0;
        dnbp->is_logged_on = 0;
        zft_free(dnbp->tcp);
        fb_client_connect(dnbp, 0, NULL);
    }
}

void run_non_block_recv_loop(dnbparse* dnbp)
{
    pthread_spin_lock(&dnbp->send_mutex);
    run_nb_recv_loop(dnbp);
    pthread_spin_unlock(&dnbp->send_mutex);
}

static void send_nb_logon(void* dd)
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
    cnt->size = ntohl(cnt->size);
    cnt->price = new_ord->price;
    cnt->price = ntohl(cnt->price);
    char* clordid = new_ord->clordid;
    int clordid_len = 8;//get_utp_field_len(new_ord->clordid, 17);
    int sym_len = get_utp_field_len(new_ord->sym, 11);
    cnt->sym = new_ord->sym;
    cnt->slen = sym_len;
    cnt->sec_type = 'E';
    cnt->side = new_ord->side;
    memcpy(cnt->owner, ci->logon->name, ci->logon->nlen);
    cnt->olen = ci->logon->nlen;
    pthread_spin_lock(&ci->mutex);
    no_copy_insert(ci->cl_ord_id_map, clordid, clordid_len, cnt);
    con_int* dest = 0;
    get_destination(ci->gk, 0, 0, (void **) &dest);
    if(dest && !dest->is_blocked && dest->connected) {
        no_copy_insert(dest->cl_ord_id_map, clordid, clordid_len, cnt);
    }
    pthread_spin_unlock(&ci->mutex);
    return cnt;
}
int8_t send_new_order(dnbcon* dnb, utpno* uno)
{
    int8_t worked = 1;
    if(!dnb->dnbp->nyse->is_blocked && dnb->dnbp->nyse->connected) {
        rcntr* cnt = trans_new_ord(dnb->dnbp->nyse, uno, sizeof(utpno));
        if (add_to_limits(dnb->cntr->jp, dnb->dnbp, cnt)) {
            pthread_spin_lock(&dnb->dnbp->send_mutex);
            worked = nb_send_helper(dnb->dnbp->nyse, cnt);
            int cplen = cnt->olen <= 5 ? cnt->olen : 5;
            memcpy(uno->sender_sub, cnt->owner, cplen);
            record_dma_message(dnb->dnbp->nyse->o, "OATS", 4,  
                    (char*)uno, sizeof(utpno),
                    utp_cs_parse_oats,
                    dnb->dnbp->nyse);
            pthread_spin_unlock(&dnb->dnbp->send_mutex);
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
int8_t send_cancel(dnbcon* dnb, utpcnl* cnl)
{
    int8_t allowed = 1;
    int clordid_len = 8;//get_utp_field_len(cnl->orig_clordid, 17);
    rcntr* cnt = 0;
    utp_get_obj_by_id(dnb->dnbp->nyse, cnl->orig_clordid, clordid_len, &cnt);
    if(cnt) {
        cnt->raw = cnl;
        cnt->rlen = sizeof(utpcnl);
        if(dnb->dnbp->nyse->connected) {
            pthread_spin_lock(&dnb->dnbp->send_mutex);
            allowed = nb_send_helper(dnb->dnbp->nyse, cnt);
            int cplen = cnt->olen <= 5 ? cnt->olen : 5;
            memcpy(cnl->sender_sub, cnt->owner, cplen);
            record_dma_message(dnb->dnbp->nyse->o, "OATS", 4,  
                    (char*)cnl, sizeof(utpcnl),
                    utp_cs_parse_oats,
                    dnb->dnbp->nyse);
            pthread_spin_unlock(&dnb->dnbp->send_mutex);
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
        char* config, int cfglen, char* user, int ulen, con_int* ci)
{
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
    dnbp->trader_lim = get_equity_limits_for_trader(cntr->jp,
            user, ulen);
    dnbp->group_lim = get_equity_limits_for_clraccnt(cntr->jp, user, ulen);
    if(dnbp->group_lim == 0x0) {
        dnbp->group_lim = calloc(1,sizeof(struct jp_limits));
        dnbp->group_lim->bp = dnbp->trader_lim->bp;
        dnbp->group_lim->max_doll = dnbp->trader_lim->max_doll;
        dnbp->group_lim->max_ord = dnbp->trader_lim->max_ord;
        dnbp->group_lim->max_shares_ord= dnbp->trader_lim->max_shares_ord;
        dnbp->group_lim->max_share= dnbp->trader_lim->max_share;
        dnbp->group_lim->cur_balance_buy= dnbp->trader_lim->cur_balance_buy;
        dnbp->group_lim->cur_balance_sell= dnbp->trader_lim->cur_balance_sell;
        dnbp->group_lim->open_ords= dnbp->trader_lim->open_ords;
        dnbp->group_lim->open_shares= dnbp->trader_lim->open_shares;
        dnbp->group_lim->mrate= dnbp->trader_lim->mrate;
        dnbp->group_lim->nmes= dnbp->trader_lim->nmes;
        dnbp->group_lim->toff= dnbp->trader_lim->toff;
    }
    dnbp->trader =  jp_get_trader(cntr->jp, user, ulen);
    ci->logon = li;
    ci->cparser = dnbp;
    dnbc->dnbp = dnbp;
    int spin_count = 128;
    int md_len = 0;
    char* interface = get_config_val(cntr->mc, "SERVER", 6,
            "interface", 9, &md_len);
    if(md_len <= 0) {
        fprintf(stderr, "You must specify an interface in the glob.dat file?, %d\n", md_len);
        return NULL;
    }
    char* csc = get_config_val(cntr->mc, "SERVER", 6,
            "spin count", 10, &md_len);
    if(md_len > 0) {
        spin_count = atoi(csc);
    } 
    zf_init();
    int walloc = zf_attr_alloc(&dnbp->zfatter);
    if (walloc < 0) {
        fprintf(stderr, "Can't alloc zf_atter! Is the driver loaded?, %d\n", walloc);
    }
    zf_attr_set_str(dnbp->zfatter, "interface", interface);
    zf_attr_set_int(dnbp->zfatter, "tcp_delayed_ack", 0);
    zf_attr_set_int(dnbp->zfatter, "reactor_spin_count", spin_count);
    walloc = zf_stack_alloc(dnbp->zfatter, &dnbp->zack);
    if (walloc < 0) {
        fprintf(stderr, "Can't alloc zf_stack! Is the driver loaded?, %d\n", walloc);
    }
    walloc = zft_alloc(dnbp->zack, dnbp->zfatter, &dnbp->zandle); 
    if (walloc < 0) {
        fprintf(stderr, "Can't alloc zf_stack! Is the driver loaded?, %d\n", walloc);
    }
    return dnbp;
}
void shutdown_dart(dnbparse* cntr)
{
    zft_shutdown_tx(cntr->tcp);
    zft_free(cntr->tcp);
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
    dbc->cntr->ass = start_lib_base_services(dbc->cntr->mc);
    dbc->cntr->jp = create_jp_limits_checker(dbc->cntr->mc,
            get_logger(dbc->cntr->ass),
            mdsdat, md_len);
    if(validate_user(dbc->cntr->jp, user, ulen, user, ulen)) {
        sbrouter* dd = create_direct_router(get_logger(dbc->cntr->ass),
                dbc->cntr->mc->mod_name,
                dbc->cntr->mc->mod_name_len);
        dbc->cntr->mc->create_new = ll_before_reset_time(dbc->cntr->mc->reset_day,
                dbc->cntr->mc->reset_hour,
                dbc->cntr->mc->reset_min,
                dbc->cntr->mc->last_logon, now);

        dbc->cntr->gk = create_dd_gatekeeper(dd, get_logger(dbc->cntr->ass),dbc->cntr->ass,
                dbc->cntr->mc->mod_name, dbc->cntr->mc->mod_name_len,
                dbc->cntr->jp, dbc->cntr->mc->create_new);
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
        con_int* ci = create_empty_interface();
        dbc->dnbp = create_nyseb_con(dbc, dbc->cntr,utpdat, utp_len, user, ulen, ci);
        send_oats_update(dbc->cntr->oats, 1, user, ulen, 1, dbc->dnbp->nyse);
        nb_td_connect(dbc->dnbp);
    } else {
        otl* o = get_logger(dbc->cntr->ass);
        send_debug_message(o, "Invalid USER, %s. \n", user);
        free(dbc->cntr);
        free(dbc);
        dbc = 0x0;
    }
    return dbc;
}
