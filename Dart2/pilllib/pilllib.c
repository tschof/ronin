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


#include "mailer.h"
#include "ex_config_helper.h"
#include "pillar_oats_parser.h"
#include "pilllib.h"
#include "oats.h"
#include "rb_tree.h"
#include "connection_interface.h"
#include "mds_client.h"
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
#include "rex_constants.h"


#define EMICRO_SEC 1000000
static uint64_t get_pillar_time()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((tv.tv_usec + (tv.tv_sec * EMICRO_SEC)) * 1000);
}
typedef struct pmdupdate {
    short exempt;
    long bid;
    long offer;
}pmdupdate;


struct pillar_parser {
    otl* logger;
    login_info* logon;
    con_int* pill;
    rex_con* rc;
    pillcon* dnbc;
    gatekeeper* gk;
    databuf_t* mess_to_send;
    databuf_t* msg_maker;
    pthread_spinlock_t send_mutex;
    pthread_spinlock_t mds_mutex;
    call_back_wrapper* recon_wrap;
    call_back_wrapper* hb_timer;
    int sock;
    char* mpid;
    char* user;
    char* password;
    char* mic;
    databuf_t* data;
    jp_trader_info* trader;
    jp_limits* trader_lim;
    jp_limits* group_lim;
    hashmap_t orders;
    hashmap_t pill_to_collar;
    int in_seq;
    uint64_t out_seq;
    uint64_t wr_str_id;
    uint64_t rd_str_id;
    uint64_t ref_str_id;
    uint64_t ref_seq;
    uint64_t clordid;
    uint32_t sess_num;
    uint8_t is_logged_on;
    uint8_t throttle;
    int ulen;
    int plen;
    int miclen;
    hashmap_t rom_to_pill;
    hashmap_t symbols;
    uint8_t check_eq;
};

struct pillar_cntr_structs {
    jp_limits_checker* jp;
    gatekeeper* gk;
    ex_config* mc;
    con_int* oats;
    pserver* ass;
    cycle_master* cm;
    pillparse* nbp;
    mdsclient* mdc;
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
static int check_price(pillparse* dnbp, pillneword* no,
        rcntr* rc)
{
    int ret = 1;
    if(dnbp->check_eq) {
        pthread_spin_lock(&dnbp->mds_mutex);
        pmdupdate* temp = 0x0;
        uint32_t isim = no->symbol;
        int found = get_map_entry(dnbp->pill_to_collar, (const char*)&isim,
                sizeof(uint32_t), (void**)&temp);
        if(found){
            if(rc->side == '1') {
                ret = no->price < temp->offer;
            } else {
                ret = no->price > temp->bid;
            }
            if(ret == 0) {
                rc->err_len = sprintf(rc->errStr, "Price out of bounds");
            }
        }
        pthread_spin_unlock(&dnbp->mds_mutex);
    }
    return ret;
}

static int add_to_limits(jp_limits_checker* jplc, pillparse* dnbp, rcntr* rc)
{
    int allowed = 1;
    long dollars = rc->price * rc->size;
    jp_limits* jp = dnbp->trader_lim;
    jp_limits* gjp = dnbp->group_lim;
    if(is_blocked(dnbp->trader)) {
        return 0;
    }
    if(allowed == 1) {
        allowed = local_check_msg_rate(jp);
        if(allowed == 0) {
            rc->err_len = sprintf(rc->errStr, "Exceeds equity Message rate");
        }
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
    long a = rc->price % EQ_DIV;
    if(rc->price > EQ_MULT && a > 0) {
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

static void remove_from_limits(pillparse* dnbp, rcntr* rc, int order,
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

static void plsend_pillar_logon(void* dd);
static void pl_direct_send(con_int* ci, pillparse* up, void* data, int len)
{
    int bytes_sent = 0;
    if(pthread_spin_lock(&up->send_mutex) == 0) {
        int disconnect = 0;
        if(databuf_unread(up->mess_to_send) == 0) {
            bytes_sent = gen_send(up->rc, data, len,
                    &disconnect);
        } else {
            databuf_memcpy(up->mess_to_send, data,
                    len);
            bytes_sent = gen_send(up->rc, up->mess_to_send->rd_ptr,
                    databuf_unread(up->mess_to_send), &disconnect);
        }
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
static void pl_no_lock_send(con_int* ci, pillparse* up, void* data, int len)
{
    int bytes_sent = 0;
    int disconnect = 0;
    if(databuf_unread(up->mess_to_send) == 0) {
        bytes_sent = gen_send(up->rc, data, len,
                &disconnect);
    } else {
        databuf_memcpy(up->mess_to_send, data,
                len);
        bytes_sent = gen_send(up->rc, up->mess_to_send->rd_ptr,
                databuf_unread(up->mess_to_send), &disconnect);
    }
    if(bytes_sent < len && !disconnect) {
        databuf_memcpy(up->mess_to_send, data + bytes_sent,
                (len - bytes_sent));
        add_sender(up->rc->p, up->rc);
    } else if(disconnect) {
        up->rc->disconnect = disconnect;
        up->is_logged_on = 0;
    }
}

static void process_mds_update(void* cbo, char* sym, int slen, long bid, long offer)
{
    pillparse* pill = (pillparse*)cbo;
    int* psym = 0x0;
    int len = get_map_entry(pill->rom_to_pill, sym, slen, (void**)&psym);
    if(len > 0) {
        int isym = *psym;
        pmdupdate* temp = 0x0;
        pthread_spin_lock(&pill->mds_mutex);
        if(find_n_fill(pill->pill_to_collar,
                    (const char*)&isym, sizeof(uint32_t), (void**)&temp)) {
            temp->bid = bid;
            temp->offer = offer;
        } else {
            temp = calloc(1, sizeof(pmdupdate));
            temp->exempt = 0;
            temp->bid = bid;
            temp->offer = offer;
            no_copy_insert(pill->pill_to_collar, (const char*)&isym,
                    sizeof(uint32_t),temp);
        }
        pthread_spin_unlock(&pill->mds_mutex);
        char dsym[32];
        memset(dsym, '\0', 32);
        memcpy(dsym, sym, slen);
        send_debug_message(pill->logger,
                "MDS insert sym: %s, bid: %ld, offer: %ld, isym: %ld \n",
                dsym, bid, offer, isym);
    }
}
static void process_mds_con(void* me, int is_connected)
{
    pillparse* pill = (pillparse*)me;
    if(!is_connected) {
        pill->check_eq = 0;
    } else {
        pill->check_eq = 1;
    }
    send_debug_message(pill->logger,
            "MDS con update: Will pill check: %d, val: %d\n",
            pill->check_eq, is_connected);
}

static void connect_to_mds(pillparse* pp)
{
    if(pp->dnbc->cntr->mdc== 0x0) {
        char* mod_name = pp->dnbc->cntr->mc->mod_name;
        int mlen = pp->dnbc->cntr->mc->mod_name_len;
        ex_config* mdsconfig = create_config_from_db(pp->dnbc->cntr->jp, mod_name, mlen, "3466");
        if(mdsconfig && mdsconfig->ip_len > 0) {
            mdsconfig->username = mod_name;
            mdsconfig->ulen = mlen;
            send_debug_message(pp->logger, "Starting MDS IP: %s \n", mdsconfig->ip);
            pp->dnbc->cntr->mdc  = create_mds_client(mdsconfig, pp->logger,pp, 
                    process_mds_update,process_mds_con);
        }
    } else {
        send_debug_message(pp->logger, "MDS Already started MDS IP: %lu \n",pp->dnbc->cntr->mdc );
    }
}
static void pdpillar_remove_from_maps(con_int* ci, pillparse * pp, uint64_t id, rcntr* cnt)
{
    pthread_spin_lock(&ci->mutex);
    remove_from_map(pp->orders, (const char*) &id, sizeof(uint64_t));
    pthread_spin_unlock(&ci->mutex);
    return_cnt(ci->gk, cnt);
}
int  pill_get_cnt(con_int* ob, pillparse * pp, uint64_t clordid,rcntr** cnt )
{
    pthread_spin_lock(&ob->mutex);
    int ret = get_map_entry(pp->orders, (const char*)&clordid, sizeof(uint64_t),
            (void**)cnt);
    pthread_spin_unlock(&ob->mutex);
    return ret;
}
static pillparse* create_pillar_parser(otl* logger, con_int* pbp, login_info* u, pillcon* dnbc)
{
    int ret = 0;
    pillparse* fp = 0;
    REX_ALLOC(fp, 8, sizeof(struct pillar_parser), ret);
    fp->logger = logger;
    fp->pill = pbp;
    fp->logon = u;
    fp->dnbc = dnbc;
    fp->gk = dnbc->cntr->gk;
    fp->mess_to_send = databuf_alloc(1024);
    pthread_spin_init(&fp->send_mutex, PTHREAD_PROCESS_PRIVATE);
    pthread_spin_init(&fp->mds_mutex, PTHREAD_PROCESS_PRIVATE);
    fp->is_logged_on = 0;
    fp->data = databuf_alloc(2048);
    fp->msg_maker = databuf_alloc(512);
    pbp->cparser = fp;
    fp->symbols = create_map_with_hash(2048, murmerhash);
    fp->pill_to_collar = create_map_with_hash(2048, murmerhash);
    fp->rom_to_pill = create_map(2048);
    fp->wr_str_id = 0;
    fp->clordid = get_pillar_time();
    fp->rd_str_id = 0;
    fp->sess_num = 0;
    fp->ref_str_id = 0;
    fp->ref_seq = 0;
    fp->is_logged_on = 0;
    fp->out_seq = 1;
    fp->orders = create_map_with_hash(2048, murmerhash);
    return fp;
}


static void plsend_pill_hb(void * v)
{
    pillparse *fco = (pillparse *) v;
    con_int* ci = fco->pill;
    if(ci->connected) {
        char buff[256];
        int r = read(fco->hb_timer->sock,buff, 256); 
	if(r < 0) {
		printf("Error pillar parse: %d\n ", r); 
	}
        /*SEND HB*/
        databuf_t* hb = get_databuf(ci->gk);
        uint16_t type = 0x0204;
        uint16_t len = 4;
        databuf_memcpy(hb, &type, 2);
        databuf_memcpy(hb, &len, 2);
        pl_direct_send(ci, fco, hb->rd_ptr, 4);
        destroy_gk_db(ci->gk, hb);
        add_back_rep_timer_adjust(fco->rc->p,fco->hb_timer,1);
    }
}

static void plmanual_pill_hb(pillparse* pc, con_int* ci)
{
    databuf_t* hb = get_databuf(ci->gk);
    uint16_t type = 0x0204;
    uint16_t len = 4;
    databuf_memcpy(hb, &type, 2);
    databuf_memcpy(hb, &len, 2);
    pl_direct_send(ci, pc, hb->rd_ptr, 4);
    destroy_gk_db(ci->gk, hb);
}
void plstart_pill_hb_timer(pillparse* fp)
{
    fp->hb_timer =  add_timer(fp->rc->p,1,
            0, 1,0, fp,plsend_pill_hb);
}

static void plsend_pillar_open(con_int* ci, uint64_t stream_id,
        uint64_t start_seq, uint8_t access)
{
    pillopen open;
    open.header.type = 0x0205;
    open.header.length = 30;
    open.stream_id = stream_id;
    open.start_seq = start_seq;
    open.end_seq = 0xffffffffffffffff;
    open.access = access;
    open.mode = 0x0;
    pillparse* pill = ci->cparser;
    pl_direct_send(ci, pill, &open, 30);
}

char *create_pillar_resend_pad(unsigned long seq, char *rom, size_t * len)
{
    return NULL;
}
static uint64_t pillar_build_offsets(char *data, unsigned long size,
                            unsigned long byte_offset,
                            unsigned long *seq_num, void* ba)
{
    return 0;
}
void pl_write_callback(void* cb)
{
    rex_con* rc = (rex_con*)cb;
    pillparse* up = (pillparse*)rc->parser;
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
static void plsend_pillar_logon(void* v)
{
    pillparse* pill = (pillparse*)v;
    con_int* ci = pill->pill;
    sequence_req *sr = create_sequence_request(ci->logon->name, ci->logon->nlen,
                                 ci->logon,"1.1",
                                 pillar_build_offsets,
                                 create_pillar_resend_pad, ci);
    get_sequence_numbers(sr, ci->o);
    /* Add the sequences into the connection object */
    pill->in_seq = dsr_get_incoming(sr);
    pillog login;
    memset(&login, ' ', sizeof(pillog));
    login.header.type = 0x0201;
    login.header.length = 0x4c;
    memcpy(&login.username, pill->user, pill->ulen);
    memcpy(&login.password, pill->password, pill->plen);
    memcpy(&login.version, "1.1", 3);
    memcpy(&login.mic, pill->mic, 4);
    pl_direct_send(ci, pill, &login, 76);
}
int pdget_pillar_field_len(char *start, int max_len)
{
    char *end = start + (max_len - 1);
    while (end[0] == 0x20 && (end -start) >= 0) {
        --end;
    }
    if(end - start == 0 && start[0] == 0x20) {
        return 0;
    } else {
        return (end - start) + 1;
    }
}
static int pdpillar_translate_rom_symbol(char *output, char *symbol, size_t sym_len)
{
    char *slash_off = 0;
    int ret_len = 0;
    char *off = output;
    slash_off = memchr(symbol, ' ', sym_len);   //find_offset(symbol, sym_len, "/", 1);
    if (slash_off == 0) {
        memcpy(off, symbol, sym_len);
        ret_len = sym_len;
    } else {
        int ex_len = (sym_len - ((slash_off + 1) - symbol));
        int l_len = slash_off - symbol;
        memcpy(off, symbol, l_len);
        off += l_len;
        *off++ = '/';
        memcpy(off, slash_off + 1, ex_len);
        ret_len = l_len + 1 + ex_len;
        *slash_off = '/';
    }
    return ret_len;
}
static void pdbuild_symbol_map(pill_ref_data* prd, pillparse* p)
{
    int len = pdget_pillar_field_len(prd->nyse_sym, 24);
    if(len > 0) {
        char tsym[24];
        memset(tsym, '\0', 24);
        int tlen = pdpillar_translate_rom_symbol(tsym, prd->nyse_sym, len);
        insert_map(p->symbols, (const char*)&prd->symbol, sizeof(uint32_t),
                (void*)tsym, tlen);
        insert_map(p->rom_to_pill, tsym, tlen,
                (void*)&prd->symbol, sizeof(uint32_t));
        char debug[25];
        memset(debug, '\0', 25);
        memcpy(debug, prd->nyse_sym, 24);
        send_debug_message(p->logger,
            "Mapped this symbol ID: %ld, char symbol %s len %d, our sym: %s\n",
            prd->symbol, debug, len, tsym);
    }
}

static void pdlog_mpid_config(mpid_config* pmpidc, pillparse* p) 
{
    send_debug_message(p->logger, 
            "Got a MPID config, status: %d, MPID: %s, User: %s \n",
            pmpidc->status, pmpidc->mpid, pmpidc->username);
}

static void pdpill_on_out(pillparse* pill, void* d_off, int mlen)
{
    con_int* ci = pill->pill;
    pillmodcnlack* cnl = d_off;
    rcntr* cnt = 0x0;
    int found = pill_get_cnt(ci, pill, cnl->orig_clordid, &cnt);
    if(found) {
        cnt->raw = d_off;
        cnt->rlen = mlen;
        int64_t cur_size = cnt->size - cnt->cum;
        remove_from_limits(pill, cnt, 1, (cur_size * cnt->price), cur_size);
        pdpillar_remove_from_maps(ci,pill, cnl->orig_clordid, cnt);
    }
    pill->dnbc->on_out(cnl);
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
static void pdlog_session_config_ack(char* d_off, pillparse* p)
{
    pillsessack* psack = (pillsessack*)d_off;
    send_debug_message(p->logger, 
            "Session Acknowledged: %s, mic: %s, c on d? %u throt pref: %u throt win: %u thresh: %u sym: %u max_ord: %u self trade: %u ack status: %u\n",
            psack->username, psack->mic, psack->cancel_on_disconnect,
            psack->throttle_pref, psack->throttle_window, psack->throttle_thresh,
            psack->symbol_eligiblity, psack->max_order_qty, psack->self_trade_prev,
            psack->ack_status);
}

static int pdpill_on_rej(pillparse* ci, void* d_off, int mlen)
{
    int ret = 0;
    pillcnlreprej* rej = d_off;
    rcntr* cnt = 0;
    int found = pill_get_cnt(ci->pill, ci, rej->clordid, &cnt);
    if(found) {
        int64_t cur_size = cnt->size - cnt->cum;
        switch (rej->rejtype) {
            case '1':
                {
                    remove_from_limits(ci, cnt, 1,(cur_size * cnt->price), cur_size);
                }
                break;
            default:
                break;
        }
        if(rej->rejtype == 1) {
            pdpillar_remove_from_maps(ci->pill,ci, rej->clordid, cnt);
        }
    }
    ci->dnbc->on_rej(rej);
    return ret;
}
/* send_biz_message(buff->buffer, databuf_unread(buff), title, t_len);
   }*/
static void pdpill_on_fill(pillparse* ci, void* d_off, int mlen)
{
    pillexerpt* ufill = d_off;
    long last = ufill->lastqty;
    last = last;
    rcntr* cnt = 0;
    int found = pill_get_cnt(ci->pill, ci, ufill->clordid, &cnt);
    if(found) {
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
        if(ufill->leaves == 0 && cnt->cum >= cnt->size) {
            pdpillar_remove_from_maps(ci->pill, ci, ufill->clordid,cnt);
        }
    } else {
        ci->dnbc->on_fill(ufill);
        /*TODO need to build fill not found mail*/
    }
}

static void pdparse_pill_priority(pillpriorack* pillprior, pillparse* p, con_int* ci)
{
        send_debug_message(p->logger, "Priority change: %lu, %d \n", pillprior->clordid, pillprior->acktype);
}

static void pdsend_bust_mail(pillbust* pillb, pillparse* p, con_int* ci)
{
        send_debug_message(p->logger,
                "BUST Dealid: %u ID:clordid %lu qty: %ld price %lu leaves: %lu Reason: %u\n",
                pillb->dealid, pillb->clordid, pillb->lastqty,
                pillb->last_price, pillb->order_id, pillb->reason);
}

static void send_pd_to_oats(pillparse* rc, char* d_off, int mess_len)
{
    record_dma_message(rc->logger, "OATS", 4,  
            d_off, mess_len,
            pillar_cs_parse_oats,
            rc->pill);
}
void plparse_payload(char* d_off, uint32_t len_left, pillparse* p, con_int* ci)
{
    uint16_t ord_type = 0;
    uint16_t len = 0;
    memcpy(&ord_type, d_off, 2);
    memcpy(&len, d_off + 2, 2);
    switch(ord_type) {
        case PILL_ORD_ACK:
            p->dnbc->on_ack((pillordack*)d_off);
            send_pd_to_oats(p,d_off, len);
            break; 
        case PILL_EXE:
            pdpill_on_fill(p, d_off, len);
            send_pd_to_oats(p,d_off, len);
            break;
        case PILL_MOD_OUT_ACK:
            pdpill_on_out(p, d_off, len);
            send_pd_to_oats(p,d_off, len);
            break; 
        case PILL_CNL_REP_REJ:
            pdpill_on_rej(p, d_off, len);
            send_pd_to_oats(p,d_off, len);
            break; 
        case PILL_PRI_UPDATE:
            pdparse_pill_priority((pillpriorack*)d_off, p, ci);
            break; 
        case PILL_BUST:
            pdsend_bust_mail((pillbust*)d_off, p, ci);
            break; 
        case 0x0232:
            pdbuild_symbol_map((pill_ref_data*)d_off, p);
            p->dnbc->on_def((pill_ref_data*)d_off);
            break;
        case 0x0272:
            pdlog_mpid_config((mpid_config*)d_off, p);
            break;
        case 0x0221:
            pdlog_session_config_ack(d_off, p);
            break;
        default:
            send_debug_message(p->logger, "Unknown: %d, %d \n", ord_type, len);
            break;
    }
}
static void pill_client_connect(void* pass_back, int is_connected,
        dart_order_obj* log)
{
    pillparse* uparse = (pillparse*)pass_back;
    con_int* ci = uparse->pill;
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
                        reconnect);
            } else {
                add_recon_timer(ci->p, uparse->recon_wrap);
            }
            break;
        case 1:
        case 2:
            ci->can_send_orders = 1;
            ci->connected = 1;
            uparse->is_logged_on = 1;
            break;
    }
    ci->update_oats(ci, is_connected);
    uparse->dnbc->on_connect(is_connected);
    send_oats_update(uparse->dnbc->cntr->oats, 1, ci->ex->username, ci->ex->ulen, 1, ci);
}
uint8_t pdparse_stream_id(uint32_t id, uint8_t des_type, pillparse* pc)
{
    uint8_t sub_id = id & 0xff;
    uint16_t user = (id >> 8) & 0xffff;
    uint8_t stype = (id >>24) & 0xff;
    send_debug_message(pc->logger,
            "Parse Stream Available ID: %ld, %ld, stype: %ld ,sub_id: %ld\n",
            id, user, stype, sub_id);
    if(des_type  == stype ) {
        return 1;
    }
    return 0;
}
void plparse_pill(int *cut_con, rex_con *rc)
{
    uint16_t ord_type = 0;
    uint16_t mess_len = 0;
    pillparse* pc = (pillparse*)rc->parser;
    con_int *ci = (con_int *) pc->pill;
    char* d_off = rc->con->rd_ptr;
    int64_t len = (rc->con->wr_ptr - rc->con->rd_ptr);
    int64_t bytes_used = 0;
    int64_t len_left = len;
    do {
        memcpy(&ord_type, d_off, 2);
        memcpy(&mess_len, d_off + 2, 2);
        if(mess_len <= len_left) {
            switch(ord_type) {
                case 0x0202:
                    {
                        plr* lr = (plr*)d_off; 
                        if(lr->status == 0) {              
                            ci->connected = 1;
                            plmanual_pill_hb(pc, ci);
                            plstart_pill_hb_timer(pc);
                            rc->connection_notice(pc, 1, NULL);
                        } else {
                            send_debug_message(pc->logger,
                                    "Login Failed with error code: %d \n",
                                    lr->status);
                            rc->connection_notice(pc, 0, NULL);
                        }
                    }
                    break;
                case 0x0203:
                    {
                        /*Stream available*/
                        if(pc->rd_str_id == 0 || pc->wr_str_id == 0 ||
                                pc->ref_str_id == 0) {
                            psavail* str = (psavail*)d_off;
                            psid r2; 
                            memcpy(&r2, &str->stream_id, 8);
                            uint64_t sess_num = r2.sess & 0xffffff;
                            send_debug_message(pc->logger,"Stream: %lu, Cast ID: %ld, cast val: %ld sess_num: %d \n",
                                   str->stream_id,  r2.sess, r2.value, sess_num);
                            uint8_t is_set = pdparse_stream_id(r2.value, 13, pc);
                            if(is_set && pc->rd_str_id == 0 &&
                                    pc->wr_str_id != 0) {
                                if(sess_num == pc->sess_num) {
                                    pc->rd_str_id = str->stream_id;
                                }
                            send_debug_message(pc->logger,
                                   "Sequences our seq: %lu, Stream seq: %lu, Stream: %lu, Cast ID: %ld, cast val: %ld sess_num: %d \n",
                                   pc->in_seq, str->next_seq, str->stream_id,  r2.sess, r2.value, sess_num);
                                uint64_t next_seq = 
                                    (pc->in_seq > 0 &&
                                     pc->in_seq < str->next_seq) ? pc->in_seq : str->next_seq;
                                plsend_pillar_open(ci, str->stream_id,
                                        next_seq, str->access);
                                pc->in_seq = next_seq;
                            }
                            is_set = pdparse_stream_id(r2.value, 15, pc);
                            if(is_set && pc->wr_str_id == 0) {
                                pc->wr_str_id = str->stream_id;
                                if(pc->sess_num == 0) {
                                    pc->sess_num = sess_num;
                                }
                                pc->clordid += str->stream_id;
                                pc->out_seq = str->next_seq;
                                int ret_len = 0;
                                get_config_val(ci->ex, "SERVER", 6,
                                        "Throttle", 8, &ret_len);
                                if(ret_len > 0) {
                                    ret_len = 0x6;
                                } else {
                                    ret_len = 0x2;
                                }
                                plsend_pillar_open(ci, str->stream_id,
                                        str->next_seq, ret_len);
                            }
                            is_set = pdparse_stream_id(r2.value, 33, pc);
                            if(is_set && pc->ref_str_id == 0) {
                                pc->ref_str_id = str->stream_id;
                                plsend_pillar_open(ci, str->stream_id,
                                        1, str->access);
                                pc->ref_seq = str->next_seq;
                                send_debug_message(pc->logger, 
                                        "Ref Stream: next seq: %lu, %d \n", str->next_seq, str->access);
                            }
                        }
                    }
                    break;
                case 0x0206:
                    {
                        /*Open Response*/
                        popen_resp* resp = (popen_resp*)d_off;
                        if(resp->status == 0x0) {
                            pc->is_logged_on = 1;
                            send_debug_message(pc->logger,
                                    "Open response for %lu, our read: %lu, write %lu, ref: %lu \n",
                                    resp->stream_id, pc->rd_str_id,
                                    pc->wr_str_id, pc->ref_str_id);
                        } else {
                            if(pc->wr_str_id == resp->stream_id) { 
                                pc->wr_str_id = 0x0;
                                send_debug_message(pc->logger,
                                        "Open response for our write stream with Failed status: %d \n",
                                        resp->status);
                            } else if(pc->rd_str_id == resp->stream_id) {
                                pc->rd_str_id = 0x0;
                                send_debug_message(pc->logger,
                                        "Open response for our read stream with Failed status: %d \n",
                                        resp->status);
                            } else {
                                send_debug_message(pc->logger,
                                        "Open response for an unknown stream with Failed status: %d , %lu \n",
                                        resp->status, resp->stream_id);
                            }
                        }                     
                    }
                    break;
                case 0x0208:
                    {
                        /*Close Reponse*/
                        pclose_resp* resp = (pclose_resp*)d_off;
                        if(resp->stream_id == pc->rd_str_id) {
                            pc->rd_str_id = 0x0;
                            send_debug_message(pc->logger,
                                    "Close response for our read stream with status: %d \n",
                                    resp->status);
                        } else if(resp->stream_id == pc->wr_str_id) {
                            pc->wr_str_id = 0x0;
                            send_debug_message(pc->logger,
                                    "Close response for our write stream with status: %d \n",
                                    resp->status);
                        } else {
                            send_debug_message(pc->logger,
                                    "Close response for an unknown stream with status: %d \n",
                                    resp->status);
                        }
                    }
                    break;
                case 0x0905:
                    {
                        plparse_payload(d_off + 32, len_left, pc, ci);
                        psm* seqmsg = (psm*)d_off;
                        if(seqmsg->seq_msg_id.id == pc->rd_str_id) {
                            memcpy(&pc->in_seq, d_off + 12, 8);
                            record_inbound_message(pc->logger, 1, ci->logon->name, ci->logon->nlen);
                        } else if(seqmsg->seq_msg_id.id == pc->ref_str_id) {
                            send_debug_message(pc->logger,
                                "REF ID: %lu Seq: %lu \n",
                                seqmsg->seq_msg_id.id, seqmsg->seq_msg_id.seq);
                            if(seqmsg->seq_msg_id.seq >= (pc->ref_seq - 1)) {
                                connect_to_mds(pc); 
                            }
                        } 
                    }
                    break;
                default:
                        send_debug_message(pc->logger,
                                "Unknown message:ord_type: %lu mess len: %lu used: %lu ,left: %lu\n",
                                ord_type, mess_len, bytes_used, len_left);
                    break;
            }
            bytes_used += mess_len;
            len_left -= mess_len;
            d_off += mess_len;
        } else {
             goto DONE_PARSIN_PLPILLS;
        }
    } while(len_left > 4);
DONE_PARSIN_PLPILLS:
    rc->con->rd_ptr += bytes_used;
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


static rcntr*  trans_new_ord(con_int* ci, pillparse* pp, pillneword* new_ord, int mlen)
{
    rcntr* cnt = get_cnt(ci->gk);
    cnt->raw = new_ord;
    cnt->rlen = mlen;
    cnt->cum = 0;
    cnt->last = 0;
    cnt->size = new_ord->qty;
    cnt->price = new_ord->price;
    cnt->sym = 0x0;
    cnt->slen = 0;
    cnt->sec_type = 'E';
    uint8_t side = get_bitfield_instruction(new_ord->bitfield_instr,
            60, 64);
    switch(side) {
        case 1:
            cnt->side = '1';
            break;
        case 2:
            cnt->side = '2';
            break;
        case 3:
            cnt->side = '5';
            break;
        default:
            break;
    };
    memcpy(cnt->owner, ci->logon->name, ci->logon->nlen);
    cnt->olen = ci->logon->nlen;
    pthread_spin_lock(&ci->mutex);
    uint64_t new_id = new_ord->clordid;
    no_copy_insert(pp->orders, (const char*)&new_id, sizeof(uint64_t), cnt);
    pthread_spin_unlock(&ci->mutex);
    return cnt;
}
int8_t send_pill_new_order(pillcon* dnb, pillneword* uno)
{
    int8_t worked = 1;
    if(!dnb->dnbp->pill->is_blocked && dnb->dnbp->pill->connected) {
        rcntr* cnt = trans_new_ord(dnb->dnbp->pill, dnb->dnbp, uno, sizeof(pillneword));
        if (check_price(dnb->dnbp, uno, cnt) && add_to_limits(dnb->cntr->jp, dnb->dnbp, cnt)) {
            pthread_spin_lock(&dnb->dnbp->send_mutex);
            pillparse* ep = dnb->dnbp;
            //add seqmsg header
            databuf_reset(ep->msg_maker);
            psm* seqmsg = (psm*)ep->msg_maker->wr_ptr;
            ep->msg_maker->wr_ptr += sizeof(psm);
            seqmsg->header.type = 0x0905;
            seqmsg->header.length = 97;
            seqmsg->timestamp = get_pillar_time();
            seqmsg->seq_msg_id.id = ep->wr_str_id;
            seqmsg->seq_msg_id.seq = ep->out_seq;
            ++ep->out_seq;
            //added
            databuf_memcpy(ep->msg_maker,uno, sizeof(pillneword) - 4); 
            pl_no_lock_send(dnb->dnbp->pill, ep, ep->msg_maker->rd_ptr, 
                    databuf_unread(ep->msg_maker));
            record_dma_message(dnb->dnbp->pill->o, "OATS", 4,  
                    ep->msg_maker->rd_ptr, sizeof(pillneword) + sizeof(psm),
                    pillar_cs_parse_oats,
                    dnb->dnbp->pill);
            pthread_spin_unlock(&dnb->dnbp->send_mutex);
        } else {
            worked =  -1;
            send_debug_message(dnb->dnbp->pill->o, "Order Rejected: %d \n", uno->clordid);
            send_debug_message(dnb->dnbp->pill->o, "Failed Limits: %s \n", cnt->errStr);
        }
    } else {
        worked =  -1;
        send_debug_message(dnb->dnbp->pill->o,
                "Order Rejected: Are we connected? %d  blocked? %d \n",
                dnb->dnbp->pill->connected,
                dnb->dnbp->pill->is_blocked);
    }
    return worked;
}
int8_t send_pill_cancel(pillcon* dnb, pillcancel* cnl)
{
    int8_t allowed = 1;
    rcntr* cnt = 0;
    int found = pill_get_cnt(dnb->dnbp->pill, dnb->dnbp, cnl->orig_ord_key,
            &cnt);
    if(found) {
        cnt->raw = cnl;
        cnt->rlen = sizeof(pillcancel);
        if(dnb->dnbp->pill->connected) {
            pthread_spin_lock(&dnb->dnbp->send_mutex);
            pillparse* ep = dnb->dnbp;
            //add seqmsg header
            databuf_reset(ep->msg_maker);
            psm* seqmsg = (psm*)ep->msg_maker->wr_ptr;
            ep->msg_maker->wr_ptr += sizeof(psm);
            seqmsg->header.type = 0x0905;
            seqmsg->header.length = 60;
            seqmsg->timestamp = get_pillar_time();
            seqmsg->seq_msg_id.id = ep->wr_str_id;
            seqmsg->seq_msg_id.seq = ep->out_seq;
            ++ep->out_seq;
            //added
            databuf_memcpy(ep->msg_maker, cnl, sizeof(pillcancel));
            pl_no_lock_send(dnb->dnbp->pill, ep, ep->msg_maker->rd_ptr, 
                    databuf_unread(ep->msg_maker));
            record_dma_message(dnb->dnbp->pill->o, "OATS", 4,  
                    ep->msg_maker->rd_ptr, sizeof(pillcancel) + sizeof(psm),
                    pillar_cs_parse_oats,
                    dnb->dnbp->pill);
            pthread_spin_unlock(&dnb->dnbp->send_mutex);
        } else {
            allowed = -1;
            send_debug_message(dnb->dnbp->pill->o,
                    "Order Rejected: %d orig: %d, connected? %d, blocked? %d\n",
                    cnl->clordid,
                    cnl->orig_ord_key,
                    dnb->dnbp->pill->connected,
                    dnb->dnbp->pill->is_blocked);
        }
    } else {
        allowed = -1;
        send_debug_message(dnb->dnbp->pill->o,
                "Could not find orig order, CNL Rejected: %d orig: %d, \n",
                cnl->clordid,
                cnl->orig_ord_key);
    }
    return allowed; 
}

static con_int* create_oats(char* filename, int flen,
        pillcntr* cntr)
{
    con_int* oats = create_empty_interface();
    int oats_len = 0;
    char* oatnum = get_config_val(cntr->mc, "SERVER", 6,
            "oats", 4, &oats_len);
    oats->ex = create_config_from_db(cntr->jp, filename, flen, oatnum);
    oats->ex->last_logon = cntr->mc->last_logon;
    set_rom_name_from_num(cntr->jp, oats->ex,
        filename, flen);
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


static void insert_ords_in_tree(con_int* con, pillparse* pp)
{
    hashmap_iterator_t hi = hashmap_iterator_begin(con->cl_ord_id_map);
    rcntr* cnt = 0;
    dart_order_obj *buff = 0;
    queue_t q = create_queue();
    element_t e, temp;
    char *key = 0;
    int key_len = 0;
    int valid = hashmap_iterator_dereference(hi, &key,
                                             &key_len,
                                             (void *) &cnt);
    while (valid) {
        enqueue(q, cnt);
        valid = hashmap_iterator_next(con->cl_ord_id_map, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &cnt);
    }
    hashmap_iterator_destroy(hi);

    e = dequeue(q);
    char ord_num[64];
    while (e) {
        cnt = (rcntr*)e->d;
        buff = (dart_order_obj *) cnt->raw;
        memset(ord_num, '\0', 64);
        memcpy(ord_num, buff->positions[ROM_CLORDID].iov_base,
               buff->positions[ROM_CLORDID].iov_len);
        uint64_t new_id = strtoul(ord_num, NULL, 10);
        no_copy_insert(pp->orders, (const char*)&new_id, sizeof(uint64_t), cnt);
        temp = e;
        e = temp->next;
        temp->d = NULL;
        free_element(q, temp);
    }
    destroy_queue(q);
}
static pillparse* create_pill_con(pillcon* dnbc, pillcntr* cntr,
        char* config, int cfglen, char* user, int ulen, con_int* ci)
{
    int utp_len = 0;
    char* utpnum = get_config_val(cntr->mc, "SERVER", 6,
            "Dest ID", 7, &utp_len);
    ci->ex = create_config_from_db(cntr->jp, config, cfglen, utpnum);
    ci->ex->last_logon = cntr->mc->last_logon;
    ci->mod_name = calloc(cfglen + 1, 1);
    ci->mod_name_len = 0;//;
    memcpy(ci->mod_name, config, cfglen);
    ci->mod_name_len = cfglen;
    ci->ex->dest_id = calloc(utp_len + 1, 1);
    memcpy(ci->ex->dest_id, utpnum, utp_len);
    ci->ex->dest_id_len = utp_len;
    ci->o = get_logger(cntr->ass);
    ci->gk = cntr->gk;
    ci->p = cntr->ass;
    ci->update_oats = nb_log_to_oats_dest;
    ci->can_send_orders = 0;
    ci->cl_ord_id_map = get_initial_order_map(cntr->gk, ci->mod_name,
            ci->mod_name_len);
    ci->sbm = 0;
    ci->tg = 0;
    ci->ex->username = calloc(1, ulen + 1);
    memcpy(ci->ex->username, user, ulen);
    ci->ex->ulen = ulen;
    ci->ex->dc = cntr->mc->dc;

    login_info* li = nb_build_logon_info_from_config(ci->ex, user, ulen);
    pillparse* dnbp = create_pillar_parser(ci->o, ci, li, dnbc);
    int ret_len = 0;
    char* tmpid = get_config_val(cntr->mc, "SERVER", 6,
            "MPID", 4, &ret_len);
    if(ret_len == 0) {
        dnbp->mpid = "DART";
    } else {
        dnbp->mpid = calloc(1,8);
        memcpy(dnbp->mpid, tmpid, 4);
    }
    dnbp->mic = get_config_val(cntr->mc, "SERVER", 6,
            "MIC", 3, &ret_len);
    dnbp->miclen = ret_len;

    dnbp->user = cntr->mc->username;
    dnbp->ulen = cntr->mc->ulen;
    dnbp->password = cntr->mc->password;
    dnbp->plen = cntr->mc->plen;
    insert_ords_in_tree(ci, dnbp);
    dnbp->trader_lim = get_equity_limits_for_trader(cntr->jp,
            user, ulen);
    ci->ph = dnbp->symbols;
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
    add_destination(cntr->gk, ci->ex->dest_id, ci->ex->dest_id_len,
                            ci);
        rex_con* rc = create_rex_con(cntr->ass, ci->ex, dnbp, dnbp,li,
                pl_write_callback, 
                pill_client_connect,
                plparse_pill, NULL);
        rc->parser = dnbp;
        rc->connect = plsend_pillar_logon;
        dnbp->rc = rc;
        ci->rc = rc;
    return dnbp;
}
void shutdown_pillar(pillparse* cntr)
{
}

pillcon* start_pillar(char* password, int plen,
        char* user, int ulen,
        char* config, int cfglen,
        void(*on_rej)(pillcnlreprej* doj),
        void(*on_out)(pillmodcnlack* doj),
        void(*on_fill)(pillexerpt* doj),
        void(*on_ack)(pillordack* doj),
        void(*on_def)(pill_ref_data* doj),
        void(*on_connect)(uint8_t is_connected))
{
    int mod_name_len = 0;
    pillcon* dbc = malloc(sizeof(pillcon));
    dbc->cntr = malloc(sizeof(pillcntr));
    dbc->cntr->mc = init_config(config, cfglen);
    dbc->cntr->mc->mod_name = get_mod_name_for_user(dbc->cntr->mc,
            user, &mod_name_len);
    if(mod_name_len <= 0) {
        destroy_ex_config(dbc->cntr->mc);
        free(dbc->cntr);
        free(dbc);
        return NULL;
    }
    if(access("/var/log/ptpd2.log", F_OK) == 0) {
        dbc->on_rej = on_rej;
        dbc->on_out = on_out;
        dbc->on_fill = on_fill;
        dbc->on_ack = on_ack;
        dbc->on_def = on_def;
        dbc->on_connect = on_connect;
        time_t now;
        time(&now);
        dbc->cntr->ass = start_lib_base_services(dbc->cntr->mc);
        dbc->cntr->jp = create_jp_limits_checker(dbc->cntr->mc,
                get_logger(dbc->cntr->ass),0x0, 0);
                //dbc->cntr->mc->mod_name, dbc->cntr->mc->mod_name_len);
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
        dbc->cntr->oats = create_oats(dbc->cntr->mc->mod_name, 
                dbc->cntr->mc->mod_name_len, dbc->cntr);
        con_int* ci = create_empty_interface();
        dbc->dnbp = create_pill_con(dbc, dbc->cntr,dbc->cntr->mc->mod_name,
                dbc->cntr->mc->mod_name_len, user, ulen, ci);
        send_oats_update(dbc->cntr->oats, 1, user, ulen, 1, dbc->dnbp->pill);
        rex_connect(dbc->cntr->ass, dbc->dnbp->rc);
        //pill_td_connect(dbc->dnbp);
    } else {
        printf("PTP not detected!!! %d \n\n", errno);
        destroy_ex_config(dbc->cntr->mc);
        free(dbc->cntr);
        free(dbc);
        return NULL;
    }
    return dbc;
}
