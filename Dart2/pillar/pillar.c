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
#include "service_bureau.h"
#include "pillar.h"
#include "pillar_server.h"
#include "suffix_map.h"
#include "pillar_test.h"

/**
 * @param f is the flag that determines if we set or clear bit.
 * @param m the bit mask
 * @param w the value to modify
 * if (f) w |= m else w &= ~m
 */
#define SET_BIT(f, m, w)            \
    w ^= (-f ^ w) & m;              \

/**
 * @param r the return value
 * @param b the char to set
 * @param bit the bit you are lookin fer.
 */
#define IS_BIT_SET(r, b, bit)          \
    r = b & (0x1 << bit);              \

typedef struct pillar_parser {
    otl* logger;
    login_info* logon;
    con_int* pbp;
    rex_con* rc;
    databuf_t* mess_to_send;
    databuf_t* msg_maker;
    pthread_spinlock_t send_mutex;
    call_back_wrapper* recon_wrap;
    call_back_wrapper* hb_timer;
    char* mpid;
    suffixm* clr_accs;
    hashmap_t symbols;
    int in_seq;
    uint64_t out_seq;
    uint64_t wr_str_id;
    uint64_t rd_str_id;
    uint64_t ref_str_id;
    uint64_t clordid;
    uint32_t sess_num;
    uint8_t is_logged_on;
    uint8_t throttle;
}pillp;


int get_pill_field_len(char *start, int max_len)
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
static int pillar_itoa(uint64_t value, databuf_t* db)
{
    char *str = db->wr_ptr;
    int radix = 10;
    static char dig[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    unsigned int n = 0;
    uint64_t v = 0;
    char *p, *q;
    char c;
    v = value;
    do {
        str[n++] = dig[v % radix];
        v /= radix;
    } while (v);
    for (p = str, q = p + (n - 1); p < q; ++p, --q) {
        c = *p;
        *p = *q;
        *q = c;
    }
    return n;
}
void  pillar_seq_msg_id_to_str(psmid id,databuf_t* db)
{
    int len = pillar_itoa(id.id, db);
    db->wr_ptr += len;
    len = pillar_itoa(id.seq, db);
    db->wr_ptr += len;
}
int64_t rom_to_pillar_price(char *price, size_t len)
{
    if(len > 0) {
        double d = strtod(price, NULL);
        d += .000000001;
        d = d * 100000000;
        return (int64_t) d;
    } else {
        return 0; 
    }
}

static void set_clordid_for_map(con_int* ci, dart_order_obj* doj, 
        pillp* ep, int rom_num, rcntr* risk)
{
        pthread_spin_lock(&ci->mutex);
        databuf_t* db = get_databuf(ci->gk);
        uint64_t lclordid = ep->clordid;
        set_rom_field(doj, rom_num, (char*)&ep->clordid, 8);
        ++ep->clordid;
        int len = pillar_itoa(lclordid, db);
        db->wr_ptr += len;
        no_copy_insert(ci->cl_ord_id_map,
                db->rd_ptr, len,(void*)risk);
        destroy_gk_db(ci->gk, db);
        pthread_spin_unlock(&ci->mutex);
}

static int get_clordid_from_map(con_int* ci, uint64_t clordid,
        rcntr** cnt)
{
        pthread_spin_lock(&ci->mutex);
        databuf_t* db = get_databuf(ci->gk);
        int len = pillar_itoa(clordid, db);
        db->wr_ptr += len;
        int ret = get_map_entry(ci->cl_ord_id_map,
                db->rd_ptr, len, (void**)cnt);
        destroy_gk_db(ci->gk, db);
        pthread_spin_unlock(&ci->mutex);
        return ret;
} 

static void pillar_remove_from_maps(con_int * ci, uint64_t id)
{
    pthread_spin_lock(&ci->mutex);
    databuf_t* db = get_databuf(ci->gk);
    int len = pillar_itoa(id, db);
    db->wr_ptr += len;
    remove_from_map(ci->cl_ord_id_map, db->rd_ptr, len);
    destroy_gk_db(ci->gk, db);
    pthread_spin_unlock(&ci->mutex);
}

int pillar_to_rom_price(int64_t price, databuf_t* val)
{
    double dprice = (double) price;
    dprice = (dprice/100000000.0);
    if(dprice > 0) {
        dprice += .000000001;
    } else {
        dprice -= .000000001;
    }
    return databuf_write(val, "%f", dprice);
}

int pillar_to_rom_time(uint64_t eur_time, databuf_t* buff)
{
    if(eur_time > 0) {
    struct tm t;
    time_t epoch_time = eur_time/1000000000;
    long nanos = eur_time % epoch_time;
    nanos /= 1000; //We are only going to display micros.
    gmtime_r(&epoch_time, &t);
    return databuf_write(buff, "%04d%02d%02d-%02d:%02d:%02d.%06d",
            (t.tm_year + 1900), (t.tm_mon + 1), t.tm_mday,
            t.tm_hour, t.tm_min, t.tm_sec, nanos);
    } else {
        struct tm t;
        time_t epoch_time;
        time(&epoch_time);
        gmtime_r(&epoch_time, &t);
        return databuf_write(buff, "%04d%02d%02d-%02d:%02d:%02d.%06d",
            (t.tm_year + 1900), (t.tm_mon + 1), t.tm_mday,
            t.tm_hour, t.tm_min, t.tm_sec, 0);
    }
}
#define EMICRO_SEC 1000000
static uint64_t get_pillar_time()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((tv.tv_usec + (tv.tv_sec * EMICRO_SEC)) * 1000);
}

static pillp* create_pillar_parser(otl* logger, con_int* pbp, login_info* u)
{
    int ret = 0;
    pillp* fp = 0;
    REX_ALLOC(fp, 8, sizeof(struct pillar_parser), ret);
    fp->logger = logger;
    fp->pbp = pbp;
    fp->logon = u;
    int ret_len = 0;
    char* tmpid = get_config_val(pbp->ex, "SERVER", 6,
            "MPID", 4, &ret_len);
    if(ret_len == 0) {
        fp->mpid = "DART";
    } else {
        fp->mpid = calloc(1,8);
        memcpy(fp->mpid, tmpid, 4);
    }
    fp->mess_to_send = databuf_alloc(1024);
    fp->msg_maker = databuf_alloc(1024);
    pthread_spin_init(&fp->send_mutex, PTHREAD_PROCESS_PRIVATE);
    fp->symbols = create_map(2048);
    fp->wr_str_id = 0;
    fp->clordid = get_pillar_time();
    fp->rd_str_id = 0;
    fp->sess_num = 0;
    fp->ref_str_id = 0;
    fp->is_logged_on = 0;
    fp->out_seq = 1;
    pbp->cparser = fp;
    return fp;
}
static int pillar_translate_rom_symbol(char *output, char *symbol, size_t sym_len)
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
static uint32_t get_symbol(pillp* p, char* sym, int sym_len)
{
    uint32_t* getter = 0;
    int ret = get_map_entry(p->symbols, sym, sym_len,
                            (void**)&getter);
    if(ret) {
        return *getter;
    } else {
        return 0;
    }
}
static void pillar_direct_send(con_int* ci, pillp* up, void* data, int len)
{
    int bytes_sent = 0;
    if(pthread_spin_lock(&up->send_mutex) == 0) {
        int disconnect = 0;
        if(databuf_unread(up->mess_to_send) <= 0) {
            bytes_sent = gen_send(up->rc, data, len,
                    &disconnect);
        } else {
            databuf_memcpy(up->mess_to_send, data, len);
            bytes_sent = gen_send(up->rc, up->mess_to_send->rd_ptr, databuf_unread(up->mess_to_send),
                    &disconnect);
            if(bytes_sent > 0) {
                up->mess_to_send->rd_ptr += bytes_sent;
            }
            if(databuf_unread(up->mess_to_send) >0 ) {
                pthread_spin_unlock(&up->send_mutex);
                add_sender(up->rc->p, up->rc);
                return;
            }
            pthread_spin_unlock(&up->send_mutex);
            return;
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
void pillar_write_callback(void* cb)
{
    rex_con* rc = (rex_con*)cb;
    pillp* up = (pillp*)rc->parser;
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
static void pillar_send(con_int* ci, rcntr* risk)
{
    pillp* up = (pillp*)ci->cparser;
    pillar_direct_send(ci, up, risk->raw, risk->rlen);
}


static int pillar_wrapper(con_int* ci, rcntr* risk)
{
    pillar_send(ci, risk);
    return 1;
}

uint64_t set_pillar_bitfields(dart_order_obj* doj)
{
    uint64_t bitfield = 0;
    uint64_t on = 0x1;
    char* rside = doj->positions[ROM_SIDE].iov_base;
    char* rcap = doj->positions[ROM_CAP].iov_base;
    if(doj->positions[ROM_SIDE].iov_len > 0 &&
            rside[0] == '5') {
        bitfield |= on << 17UL;
    }
    if(doj->positions[ROM_CAP].iov_len > 0 &&
            rcap[0] == 'A') {
        bitfield |= on << 22UL;
    } else {
        bitfield |= on << 24UL;
    }
    char* type = doj->positions[ROM_TYPE].iov_base;
    int typelen = doj->positions[ROM_TYPE].iov_len;
    char* tif = doj->positions[ROM_TIF].iov_base;
    int tlen = doj->positions[ROM_TIF].iov_len;
    int inst_len = doj->positions[ROM_INSTR].iov_len;
    char* instr = doj->positions[ROM_INSTR].iov_base;
    int exlen = doj->positions[ROM_EXEC_INST].iov_len;
    if(typelen == 2) {
        bitfield |= on <<29UL;
        if(type[0] == '1' && type[1] == '2') {
            bitfield |= on <<31UL;
            bitfield |= on <<32UL;
        } else if(type[0] == '1') {
            bitfield |= on <<33UL;
        } else {
            bitfield |= on <<31UL;
        }
    } else {
        if(typelen == 1 && type[0] == '5') {
            bitfield |= on <<29UL;
            bitfield |= on <<33UL;
        } else if(typelen == 1 && type[0] == '1') {
            bitfield |= on <<29UL;
            bitfield |= on <<31UL;
        } else if(tlen == 1 && tif[0] == '2') {
            bitfield |= on <<29UL;
            bitfield |= on <<31UL;
            bitfield |= on <<32UL;
        } else if (tlen == 1 && tif[0] == '3') {
            bitfield |= on <<29UL;
            bitfield |= on <<30UL;
            bitfield |= on <<32UL;
        } else if(inst_len == 1 && instr[0] == '2') {
            bitfield |= on <<29UL;
            bitfield |= on <<30UL;
            bitfield |= on <<31UL;
        } else if((inst_len >0 && inst_len <=4) || exlen > 0) {
            bitfield |= on <<29UL;
            bitfield |= on <<31UL;
        } else {
            bitfield |= on <<29UL;
            bitfield |= on <<30UL;
            bitfield |= on <<31UL;
        }
    }
    /*Proactive route*/
    if(inst_len == 0 && exlen == 0) {
        //bitfield |= on <<34UL;
    }
    char* dest = doj->positions[ROM_DESTID].iov_base;
    int destlen = doj->positions[ROM_DESTID].iov_len;
    char* exinst = doj->positions[ROM_EXEC_INST].iov_base;
    /*Self trade cancel both*/
    bitfield |= on <<38UL;
    if(inst_len > 0) {
        if(instr[0] == 'A' && inst_len == 1) {
            bitfield |= on <<44UL;
            bitfield |= on <<48UL;
        } else if(instr[0] == 'I' && inst_len == 1) {
            //bitfield |= on <<44UL;
            bitfield |= on <<51UL;
            /*Non routable*/
        } else if(inst_len == 2 && instr[0] == 'N' &&
                instr[1] == 'R') {
            bitfield |= on <<44UL;
            /*PMA and PMU primary after and primary until*/
        } else if(inst_len == 3 && instr[2] == 'U') {
            bitfield |= on <<44UL;
            bitfield |= on <<46UL;
        } else if(inst_len == 3 && instr[2] == 'A') {
            bitfield |= on <<45UL;
            bitfield |= on <<46UL;
            /*Directed + routable = PO+S*/
        } else if(inst_len == 4 && instr[2] == '+') {
            bitfield |= on <<46UL;
            /*Directed Primary only = DP*/
        } else if(inst_len == 2 && instr[0] == 'P' &&
                instr [1] == 'O') {
            bitfield |= on <<44UL;
            bitfield |= on <<45UL;
            /*Both Primary until and after = BPMA*/
        } else if(inst_len == 4 && instr[0] == 'B') {
            bitfield |= on <<44UL;
            bitfield |= on <<45UL;
            bitfield |= on <<46UL;
        } else if(inst_len == 1 && instr[0] == '2') {
            bitfield |= on <<48UL;
            bitfield |= on <<49UL;
        } else if(exlen == 1 && exinst[0] == '1') {
            bitfield |= on <<45UL;
        }
    } else if(exlen > 0) {
        if(exinst[0] == 'A' && exlen == 1) {
            bitfield |= on <<48UL;
        } else if(exinst[0] == 'I' && exlen == 1) {
            bitfield |= on <<51UL;
        } else if(exinst[0] == 'P') {
            bitfield |= on <<53UL;
            bitfield |= on <<54UL;
        } else if(exinst[0] == 'R') {
            bitfield |= on <<52UL;
            bitfield |= on <<54UL;
        } else if(exinst[0] == 'M') {
            bitfield |= on <<52UL;
            bitfield |= on <<53UL;
            bitfield |= on <<54UL;
        } else if(exinst[0] == '8') {
            bitfield |= on <<55UL;
        } 
    } else if(destlen == 3 && dest[0] == '5'
            && dest[1] == '0' && (dest[2] == '3' || dest[2] == '4')) {
            bitfield |= on <<44UL;
            bitfield |= on <<45UL;
    }
    if((typelen ==2 && type[0] == '9')) {
        bitfield |= on <<58UL;
    } else if(typelen == 1 ) {
        if(type[0] == '1' || type[0] == '5') {
            bitfield |= on <<56UL;
        } else if(type[0] == '2') {
            bitfield |= on <<57UL;
        } else if(type[0] == '7') {
            bitfield |= on <<56UL;
            bitfield |= on <<57UL;
        } else {
            bitfield |= on <<57UL;
        }
    } else if(typelen == 2) {
        if(doj->positions[ROM_PRICE].iov_len > 0) {
            bitfield |= on <<57UL;
        } else {
            bitfield |= on <<56UL;
        }
    }
    char* side = doj->positions[ROM_SIDE].iov_base;
    int sidelen = doj->positions[ROM_SIDE].iov_len;
    if(sidelen > 0) {
        if(side[0] == '1') {
            bitfield |= on <<60UL;
        } else if(side[0] == '2') {
            bitfield |= on <<61UL;
        } else if(side[0] == '5') {
            bitfield |= on <<60UL;
            bitfield |= on <<61UL;
        }
    }
    return bitfield;
}

static void send_no_lock(pillp* p, char* data, int len)
{
    int disconnect = 0;
    int bytes_sent = 0;
    if(databuf_unread(p->mess_to_send) <= 0) {
        bytes_sent = gen_send(p->rc, data, len,
                &disconnect);
    } else {
        databuf_memcpy(p->mess_to_send, data, len);
        bytes_sent = gen_send(p->rc, p->mess_to_send->rd_ptr, databuf_unread(p->mess_to_send),
                &disconnect);
        if(bytes_sent > 0) {
            p->mess_to_send->rd_ptr += bytes_sent;
        }
        if(databuf_unread(p->mess_to_send) >0 ) {
            add_sender(p->rc->p, p->rc);
        }
        return;
    }
    if(bytes_sent < len && !disconnect) {
        databuf_memcpy(p->mess_to_send, data + bytes_sent,
                (len - bytes_sent));
        add_sender(p->rc->p, p->rc);
    } else if(disconnect) {
        p->rc->disconnect = disconnect;
        p->is_logged_on = 0;
    }
}
/**
 *
 *
 */
static int pillar_doj_enter(con_int* ci, rcntr* risk)
{
    int sent = 1;
    pillp* ep = (pillp*)ci->cparser;
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    if(pthread_spin_lock(&ep->send_mutex) == 0) {
        databuf_reset(ep->msg_maker);
        psm* seqmsg = (psm*)ep->msg_maker->wr_ptr;
        ep->msg_maker->wr_ptr += sizeof(psm);
        seqmsg->header.type = 0x0905;
        seqmsg->timestamp = get_pillar_time();
        seqmsg->seq_msg_id.id = ep->wr_str_id;
        seqmsg->seq_msg_id.seq = ep->out_seq;
        
        set_clordid_for_map(ci, doj, ep,
                ROM_BIN_CLORDID, risk);
        pillneword* neword = (pillneword*)ep->msg_maker->wr_ptr;
        memset(ep->msg_maker->wr_ptr, '\0', sizeof(pillneword));
        memcpy(&neword->clordid, doj->positions[ROM_BIN_CLORDID].iov_base, 
                doj->positions[ROM_BIN_CLORDID].iov_len);
        ++ep->out_seq;
        neword->header.type = 0x0240;
        neword->symbol = get_symbol(ep, doj->positions[ROM_SYM].iov_base,
                                 doj->positions[ROM_SYM].iov_len);
        int mpid_len = 0;
        char* on_behalf = get_mpid_for_clr_acct(ci->sbm,
                getpval(doj, ROM_CLR_ID),
                getplen(doj, ROM_CLR_ID),
                &mpid_len);
        if(mpid_len > 0) {
            memcpy(&neword->mpid, on_behalf,mpid_len);
        } else {
            memcpy(&neword->mpid, ep->mpid, 4);
        }
        neword->bitfield_instr = set_pillar_bitfields(doj);
        neword->price = rom_to_pillar_price(doj->positions[ROM_PRICE].iov_base,
                                            doj->positions[ROM_PRICE].iov_len);
        neword->qty = (uint32_t)risk->size;
        if(doj->positions[ROM_CLR_ACC].iov_len > 0) {
            if(ep->clr_accs != 0x0) {
                int slen = 0;
                const char* bclr = get_suffix(ep->clr_accs,
                        doj->positions[ROM_CLR_ACC].iov_base,
                        doj->positions[ROM_CLR_ACC].iov_len, &slen);
                if(slen > 0 && slen <= 8) {
                    memcpy(&neword->user_data, bclr, slen);
                } else {
                    int cplen = doj->positions[ROM_CLR_ACC].iov_len > 8 ?
                        8 : doj->positions[ROM_CLR_ACC].iov_len;
                    memcpy(&neword->user_data, doj->positions[ROM_CLR_ACC].iov_base,
                            cplen);
                }
            } else {
                int cplen = doj->positions[ROM_CLR_ACC].iov_len > 8 ?
                    8 : doj->positions[ROM_CLR_ACC].iov_len;
                memcpy(&neword->user_data, doj->positions[ROM_CLR_ACC].iov_base,
                        cplen);
            }
        } 
        /*We need to advance the wr_ptr */
        if(doj->positions[ROM_MAX_FLOOR].iov_len > 0 ||
                doj->positions[ROM_EXPIRE_TIME].iov_len > 0) {
            ep->msg_maker->wr_ptr += 65;//sizeof(pillneword);
            pillequityadd* pilleadd = (pillequityadd*)ep->msg_maker->wr_ptr;
            memset(pilleadd, '\0', sizeof(pillequityadd));
            pilleadd->header.length = 41;
            pilleadd->header.type = 0x0241;
            if(doj->positions[ROM_MAX_FLOOR].iov_len > 0) {
                char floor[32];
                memset(&floor, '\0', 32);
                memcpy(&floor, doj->positions[ROM_MAX_FLOOR].iov_base,
                        doj->positions[ROM_MAX_FLOOR].iov_len);
                pilleadd->maxfloor = atoi(floor);
            }
            if(doj->positions[ROM_EXPIRE_TIME].iov_len > 0) {
                char floor[32];
                memset(&floor, '\0', 32);
                memcpy(&floor, doj->positions[ROM_EXPIRE_TIME].iov_base,
                        doj->positions[ROM_EXPIRE_TIME].iov_len);
                pilleadd->effectivetime= atoi(floor);
            }
            neword->header.length = 106;
            seqmsg->header.length = 138;
            ep->msg_maker->wr_ptr += sizeof(pillequityadd);
        } else { 
            seqmsg->header.length = 97;
            neword->header.length = 65;
            ep->msg_maker->wr_ptr += 65;//sizeof(pillneword);
        }
        send_no_lock(ep, ep->msg_maker->rd_ptr, databuf_unread(ep->msg_maker));
        pthread_spin_unlock(&ep->send_mutex);
    } 
    return sent;
}

static void pillar_cnl_all(con_int* ci)
{
   /* pillp* ep = (pillp*)ci->cparser;
    if(pthread_spin_lock(&ep->send_mutex) == 0) {
        databuf_reset(ep->msg_maker);
        psm* seqmsg = (psm*)ep->msg_maker->wr_ptr;
        ep->msg_maker->wr_ptr += sizeof(psm);
        seqmsg->header.type = 0x0905;
        seqmsg->header.length = 63;
        seqmsg->timestamp = get_pillar_time();
        seqmsg->seq_msg_id.id = ep->wr_str_id;
        seqmsg->seq_msg_id.seq = ep->out_seq;
        uint64_t lclordid = ep->clordid;
        ++ep->clordid;
        pillmasscancel* pmc = (pillmasscancel*)ep->msg_maker->wr_ptr;
        memset(ep->msg_maker->wr_ptr, '\0', sizeof(pillmasscancel));
        memcpy(&pmc->clordid, &lclordid, 8);
        ++ep->out_seq;
        pmc->header.type = 0x0281;
        pmc->header.length = 31;
        memcpy(&pmc->mpid, ep->mpid, 4);
        pmc->bulk_cancel_type = 2;
        ep->msg_maker->wr_ptr += 31;
        send_no_lock(ep, ep->msg_maker->rd_ptr, databuf_unread(ep->msg_maker));
        pthread_spin_unlock(&ep->send_mutex);
    }*/
}
static int pillar_doj_cancel(con_int* ci, rcntr* risk)
{
    int sent = 1;
    pillp* ep = (pillp*)ci->cparser;
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
                                
    if(pthread_spin_lock(&ep->send_mutex) == 0) {
        databuf_reset(ep->msg_maker);
        psm* seqmsg = (psm*)ep->msg_maker->wr_ptr;
        ep->msg_maker->wr_ptr += sizeof(psm);
        seqmsg->header.type = 0x0905;
        seqmsg->header.length = 60;
        seqmsg->timestamp = get_pillar_time();
        seqmsg->seq_msg_id.id = ep->wr_str_id;
        seqmsg->seq_msg_id.seq = ep->out_seq;
        ++ep->out_seq;
        set_clordid_for_map(ci, doj, ep,
                ROM_BIN_PENDING_ID, risk);
        pillcancel* cnl = (pillcancel*)ep->msg_maker->wr_ptr;
        memset(ep->msg_maker->wr_ptr, '\0', sizeof(pillcancel));
        cnl->header.type = 0x0280;
        cnl->header.length = 28;
        memcpy(&cnl->mpid, ep->mpid, 4);
        if(doj->positions[ROM_BIN_CLORDID].iov_len > 0) {
            memcpy(&cnl->orig_ord_key, (char*)doj->positions[ROM_BIN_CLORDID].iov_base, 8);
        }
        if(doj->positions[ROM_BIN_PENDING_ID].iov_len > 0) {
            memcpy(&cnl->clordid, (char*)doj->positions[ROM_BIN_PENDING_ID].iov_base, 8);
        }
        cnl->symbol = get_symbol(ep, doj->positions[ROM_SYM].iov_base,
                                 doj->positions[ROM_SYM].iov_len);
        int mpid_len = 0;
        char* on_behalf = get_mpid_for_clr_acct(ci->sbm,
                getpval(doj, ROM_CLR_ID),
                getplen(doj, ROM_CLR_ID),
                &mpid_len);
        if(mpid_len > 0) {
            memcpy(&cnl->mpid, on_behalf,mpid_len);
        } else {
            memcpy(&cnl->mpid, ep->mpid, 4);
        }
        /*We need to advance the wr_ptr */
        ep->msg_maker->wr_ptr += 28;
        send_no_lock(ep, ep->msg_maker->rd_ptr, databuf_unread(ep->msg_maker));
        pthread_spin_unlock(&ep->send_mutex);
    } 
    return sent;
}
static int pillar_doj_replace(con_int* ci, rcntr* risk)
{
    int sent = 1;
    pillp* ep = (pillp*)ci->cparser;
    dart_order_obj* doj = (dart_order_obj*)risk->raw;
    dart_order_obj* rep = (dart_order_obj*)doj->rep;
    if(pthread_spin_lock(&ep->send_mutex) == 0) {
        databuf_reset(ep->msg_maker);
        psm* seqmsg = (psm*)ep->msg_maker->wr_ptr;
        ep->msg_maker->wr_ptr += sizeof(psm);
        seqmsg->header.type = 0x0905;
        seqmsg->header.length = 97;
        seqmsg->timestamp = get_pillar_time();
        seqmsg->seq_msg_id.id = ep->wr_str_id;
        seqmsg->seq_msg_id.seq = ep->out_seq;
        set_clordid_for_map(ci, doj, ep, ROM_BIN_PENDING_ID, risk);
        pillneword* neword = (pillneword*)ep->msg_maker->wr_ptr;
        memset(ep->msg_maker->wr_ptr, '\0', sizeof(pillneword));
        memcpy(&neword->orig_ord_key, (char*)doj->positions[ROM_BIN_CLORDID].iov_base, 8);
        memcpy(&neword->clordid, (char*)doj->positions[ROM_BIN_PENDING_ID].iov_base, 8);
        ++ep->out_seq;
        neword->header.type = 0x0240;
        neword->header.length = 65;
        neword->symbol = get_symbol(ep, doj->positions[ROM_SYM].iov_base,
                                 doj->positions[ROM_SYM].iov_len);
        //memcpy(&neword->mpid, ep->mpid, 4);

        int mpid_len = 0;
        char* on_behalf = get_mpid_for_clr_acct(ci->sbm,
                getpval(doj, ROM_CLR_ID),
                getplen(doj, ROM_CLR_ID),
                &mpid_len);
        if(mpid_len > 0) {
            memcpy(&neword->mpid, on_behalf,mpid_len);
        } else {
            memcpy(&neword->mpid, ep->mpid, 4);
        }
        neword->bitfield_instr = set_pillar_bitfields(doj);
        if(rep->positions[ROM_PRICE].iov_len > 0) {
            neword->price = rom_to_pillar_price(rep->positions[ROM_PRICE].iov_base,
                                                rep->positions[ROM_PRICE].iov_len);
        } else {
            neword->price = rom_to_pillar_price(doj->positions[ROM_PRICE].iov_base,
                                                doj->positions[ROM_PRICE].iov_len);
        }
        if(rep->positions[ROM_PRICE].iov_len > 0) {
            neword->qty = (uint32_t)rep->risk.size;
        } else {
            neword->qty = (uint32_t)risk->size;
        }
        
        if(doj->positions[ROM_CLR_ACC].iov_len > 0) {
            if(ep->clr_accs != 0x0) {
                int slen = 0;
                const char* bclr = get_suffix(ep->clr_accs,
                        doj->positions[ROM_CLR_ACC].iov_base,
                        doj->positions[ROM_CLR_ACC].iov_len, &slen);
                if(slen > 0 && slen <= 8) {
                    memcpy(&neword->user_data, bclr, slen);
                } else {
                    int cplen = doj->positions[ROM_CLR_ACC].iov_len > 8 ?
                        8 : doj->positions[ROM_CLR_ACC].iov_len;
                    memcpy(&neword->user_data, doj->positions[ROM_CLR_ACC].iov_base,
                            cplen);
                }
            } else {
                int cplen = doj->positions[ROM_CLR_ACC].iov_len > 8 ?
                    8 : doj->positions[ROM_CLR_ACC].iov_len;
                memcpy(&neword->user_data, doj->positions[ROM_CLR_ACC].iov_base,
                        cplen);
            }
        } 
        /*We need to advance the wr_ptr */
        ep->msg_maker->wr_ptr += 65;//sizeof(pillneword);
        send_no_lock(ep, ep->msg_maker->rd_ptr, databuf_unread(ep->msg_maker));
        pthread_spin_unlock(&ep->send_mutex);
    } 
    return sent;
}
static void send_pill_hb(void * v)
{
    pillp *fco = (pillp *) v;
    con_int* ci = fco->pbp;
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
        pillar_direct_send(ci, fco, hb->rd_ptr, 4);
        destroy_gk_db(ci->gk, hb);
        add_back_rep_timer_adjust(fco->rc->p,fco->hb_timer,1);
    }
}

static void manual_pill_hb(pillp* pc, con_int* ci)
{
    databuf_t* hb = get_databuf(ci->gk);
    uint16_t type = 0x0204;
    uint16_t len = 4;
    databuf_memcpy(hb, &type, 2);
    databuf_memcpy(hb, &len, 2);
    pillar_direct_send(ci, pc, hb->rd_ptr, 4);
    destroy_gk_db(ci->gk, hb);
}
void start_pill_hb_timer(pillp* fp)
{
    fp->hb_timer =  add_timer(fp->rc->p,1,
            0, 1,0, fp,send_pill_hb);
}

static void send_pillar_open(con_int* ci, uint64_t stream_id,
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
    pillp* pill = ci->cparser;
    pillar_direct_send(ci, pill, &open, 30);
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
static void send_pillar_logon(void* v)
{
    pillp* pill = (pillp*)v;
    con_int* ci = pill->pbp;
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
    memcpy(&login.username, ci->logon->name, ci->logon->nlen);
    memcpy(&login.password, ci->logon->password, ci->logon->plen);
    memcpy(&login.version, "1.1", 3);
    int ret_len = 0;
    char* mic = get_config_val(ci->ex, "SERVER", 6,
            "MIC", 3, &ret_len);
    if(ret_len == 4) {
        memcpy(&login.mic, mic, 4);
    } else {
        memcpy(&login.mic, "ARCX", 4);
    }
    pillar_direct_send(ci, pill, &login, 76);
}
int get_pillar_field_len(char *start, int max_len)
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
static void build_symbol_map(pill_ref_data* prd, pillp* p)
{
    int len = get_pillar_field_len(prd->nyse_sym, 24);
    if(len > 0) {
        char tsym[24];
        memset(tsym, '\0', 24);
        int tlen = pillar_translate_rom_symbol(tsym, prd->nyse_sym, len);
        uint32_t* usym = malloc(sizeof(uint32_t));
        memcpy(usym, &prd->symbol, sizeof(uint32_t));
        no_copy_insert(p->symbols, tsym, tlen,
                (void*)usym);
        char debug[25];
        memset(debug, '\0', 25);
        memcpy(debug, prd->nyse_sym, 24);
        send_debug_message(p->logger,
            "Mapped this symbol ID: %ld, char symbol %s len %d, our sym: %s\n",
            prd->symbol, debug, len, tsym);
    }
}
static void log_for_sender(con_int* us, con_int* sender, rcntr* obj)
{ 
    dart_order_obj* doj = (dart_order_obj*)obj->raw;
    record_dart_message(us->o, doj);
}

static void log_mpid_config(mpid_config* pmpidc, pillp* p) 
{
    send_debug_message(p->logger, 
            "Got a MPID config, status: %d, MPID: %s, User: %s \n",
            pmpidc->status, pmpidc->mpid, pmpidc->username);
}
static void pillar_return_out(con_int* ci, rcntr* cnt)
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
        } else if(sender) {
            log_for_sender(ci, sender, cnt);
        }
    }   
}
static void pillar_return_rej(con_int* ci, rcntr* cnt)
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
        }else if(sender) {
            log_for_sender(ci, sender, cnt);
        }

    }   
}
static void pillar_return_fill(con_int* ci, rcntr* cnt)
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
        } else if(sender) {
            log_for_sender(ci, sender, cnt);
        }
 
    }   
}
static void pillar_return_to_sender(con_int* ci, rcntr* cnt)
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
        } else if(sender) {
            log_for_sender(ci, sender, cnt);
        }
 
    }   
}
/*
 *
 */
static void log_session_config_ack(char* d_off, pillp* p)
{
    pillsessack* psack = (pillsessack*)d_off;
    send_debug_message(p->logger, 
            "Session Acknowledged: %s, mic: %s, c on d? %u throt pref: %u throt win: %u thresh: %u sym: %u max_ord: %u self trade: %u ack status: %u\n",
            psack->username, psack->mic, psack->cancel_on_disconnect,
            psack->throttle_pref, psack->throttle_window, psack->throttle_thresh,
            psack->symbol_eligiblity, psack->max_order_qty, psack->self_trade_prev,
            psack->ack_status);
}
/**
 *
 */
static void parse_ack(pillordack* poack, pillp* p, con_int* ci)
{
            /*send_debug_message(p->logger,
                    "ACK MESSAGE:clordid %lu ORIG: %lu qty: %ld price %lu leavs: %ld\n",
                    poack->clordid, poack->orig_clordid, poack->qty,
                    poack->price, poack->leaves);*/
    databuf_t* db = get_databuf(ci->gk);
    rcntr* cnt = 0;
    int ret = get_clordid_from_map(ci, poack->clordid, &cnt);
    if(ret <= 0) {
            send_debug_message(p->logger,
                    "Could not find:clordid %lu qty: %ld price %lu leavs: %ld\n",
                    poack->clordid, poack->qty,
                    poack->price, poack->leaves);
            goto PACKDONE;
    }
    if(ret && cnt->rlen == sizeof(dart_order_obj)) {
        dart_order_obj* doj = (dart_order_obj*)cnt->raw;
        set_rom_field(doj, 0, "S", 1);
        switch(poack->acktype) {
            case 1:
                set_rom_field(doj, ROM_STATUS, "13",2);
                break;
            case 9:
            case 8:
                {
                    dart_order_obj* rep = (dart_order_obj*)doj->rep;
                    pillar_remove_from_maps(ci, poack->orig_clordid);
                    fill_in_rom_field(doj, ROM_BIN_CLORDID,
                            doj->positions[ROM_BIN_PENDING_ID].iov_base,
                            doj->positions[ROM_BIN_PENDING_ID].iov_len);
                    set_rom_field(doj, ROM_STATUS, "5",1);
                    if(rep->positions[ROM_PRICE].iov_len > 0) {
                        set_rom_field(doj, ROM_PRICE, rep->positions[ROM_PRICE].iov_base, 
                                rep->positions[ROM_PRICE].iov_len); 
                        cnt->price = rep->risk.price;
                    }
                    if(rep->positions[ROM_SHARES].iov_len > 0) {
                        set_rom_field(doj, ROM_SHARES, rep->positions[ROM_SHARES].iov_base, 
                                rep->positions[ROM_SHARES].iov_len); 
                        cnt->size = rep->risk.size;
                    }
                    unsigned_dart_itoa(doj, 
                            poack->leaves, ROM_LEAVES,db);
                    databuf_reset(db);
                    unsigned_dart_itoa(doj, 
                            poack->order_id, ROM_EX_TAG,db);
                    databuf_reset(db);
                }
                break;
            case 12:
            case 11:
                set_rom_field(doj, ROM_STATUS, "4",1);
                break;
            default:
                send_debug_message(p->logger,
                        "Status: %u ID:clordid %lu qty: %ld price %lu leaves: %ld\n",
                        poack->acktype, poack->clordid, poack->qty,
                        poack->price, poack->leaves);
                break;
        };
        databuf_reset(db);
        int clordid_len = pillar_itoa(poack->clordid, db);
        set_rom_field(doj, ROM_CLORDID, db->rd_ptr, clordid_len);
        databuf_reset(db);
        int len = pillar_to_rom_time(poack->timestamp, db); 
        set_rom_field(doj, EXEC_TIME, db->rd_ptr, len);
        pillar_return_to_sender(ci, cnt);
    }
PACKDONE:
    destroy_gk_db(ci->gk, db);
}
static void parse_pill_exe(pillexerpt* pillexe, pillp* p, con_int* ci)
{
        /*send_debug_message(p->logger,
                "EXE MESSAGE cum: %u ID:clordid %lu qty: %ld price %lu leaves: %ld\n",
                pillexe->cum, pillexe->clordid, pillexe->lastqty,
                pillexe->last_price, pillexe->leaves);*/
    databuf_t* db = get_databuf(ci->gk);
    rcntr* cnt = 0;
    int ret = get_clordid_from_map(ci, pillexe->clordid, &cnt);
    if(ret && cnt->rlen == sizeof(dart_order_obj)) {
       dart_order_obj* doj = (dart_order_obj*)cnt->raw; 
       set_rom_field(doj, 0, "S", 1);
       int len = pillar_to_rom_time(pillexe->timestamp, db); 
       set_rom_field(doj, EXEC_TIME, db->rd_ptr, len);
       databuf_reset(db);
       u64_dart_itoa(doj, pillexe->order_id, ROM_EX_TAG,db);
       databuf_reset(db);
       u64_dart_itoa(doj, pillexe->dealid, EXEC_ID,db);
       databuf_reset(db);
       pillar_to_rom_price(pillexe->last_price, db);
       set_rom_field(doj, ROM_EXEC_PRICE, db->rd_ptr, databuf_unread(db));
       databuf_reset(db);
       unsigned_dart_itoa(doj, pillexe->leaves,
                                ROM_LEAVES, db);
       databuf_reset(db);
       unsigned_dart_itoa(doj, pillexe->cum, ROM_CUM, db);
       databuf_reset(db);
       unsigned_dart_itoa(doj, pillexe->lastqty, ROM_LAST_SHARES, db);
       databuf_reset(db);
       len = get_pill_field_len(pillexe->liq, 4);
       set_rom_field(doj, ROM_LIQ, pillexe->liq, len);
       cnt->cum = pillexe->cum;
       cnt->last = pillexe->lastqty;
        if(cnt->size <= cnt->cum) {
            set_rom_field(doj, ROM_STATUS, "2", 1);
        } else {
            set_rom_field(doj, ROM_STATUS, "1", 1);
        }
       pillar_return_fill(ci, cnt);
    } else {
        /**
         * DEBU 
         */
        send_debug_message(p->logger,
                "Could not find fill cum: %u ID:clordid %lu qty: %ld price %lu leaves: %ld\n",
                pillexe->cum, pillexe->clordid, pillexe->lastqty,
                pillexe->last_price, pillexe->leaves);
    }
    destroy_gk_db(ci->gk, db);
}

static void parse_pill_out(pillmodcnlack* pillcnl, pillp* p, con_int* ci)
{
            /*send_debug_message(p->logger,
                    "OUT MESSAGE Status: %u ID:clordid %lu ref: %lu, qty: %ld price %lu leaves: %ld reason: %d\n",
                    pillcnl->acktype, pillcnl->orig_clordid, pillcnl->refclordid, pillcnl->qty,
                    pillcnl->price, pillcnl->leaves, pillcnl->reason);*/
            
    databuf_t* db = get_databuf(ci->gk);
    rcntr* cnt = 0;
    int ret = get_clordid_from_map(ci, pillcnl->orig_clordid, &cnt);
    if(ret <= 0) {
        ret = get_clordid_from_map(ci, pillcnl->refclordid, &cnt);
        if(ret <= 0) {
            send_debug_message(p->logger,
                    "Could not find ord Status: %u ID:clordid %lu ref: %lu, qty: %ld price %lu leaves: %ld\n",
                    pillcnl->acktype, pillcnl->orig_clordid, pillcnl->refclordid, pillcnl->qty,
                    pillcnl->price, pillcnl->leaves);
            goto PILLOUTDONE;
        }
    }
    if (ret && cnt->rlen == sizeof(dart_order_obj)) {
        dart_order_obj* doj = (dart_order_obj*)cnt->raw;
        set_rom_field(doj, 0, "S", 1);
        int len = pillar_to_rom_time(pillcnl->timestamp, db); 
        set_rom_field(doj, EXEC_TIME, db->rd_ptr, len);
        switch(pillcnl->acktype) {
            case 5: 
            case 4: 
                {
                    set_rom_field(doj, ROM_STATUS, "6",1);
                    pillar_return_to_sender(ci, cnt);
                    goto PILLOUTDONE;
                }
                break;
            case 11:
            case 12:
                set_rom_field(doj, ROM_STATUS, "4",1);
                pillar_remove_from_maps(ci, pillcnl->orig_clordid);
                if(doj->positions[ROM_BIN_PENDING_ID].iov_len > 0 &&
                        pillcnl->reason == 99) {
                    pillar_remove_from_maps(ci, pillcnl->refclordid);
                }
                break;
            case 13:
            case 14:
            case 15:
            case 16:
                {
                    if(pillcnl->leaves > 0) {
                        set_rom_field(doj, ROM_STATUS, "6",1);
                        pillar_return_to_sender(ci, cnt);
                        goto PILLOUTDONE;
                    } else {
                        set_rom_field(doj, ROM_STATUS, "4",1);
                        pillar_remove_from_maps(ci, pillcnl->orig_clordid);
                        if(doj->positions[ROM_BIN_PENDING_ID].iov_len > 0 &&
                                pillcnl->reason == 99) {
                            pillar_remove_from_maps(ci, pillcnl->refclordid);
                        }
                    }
                }
                break;
            case 7:
                set_rom_field(doj, ROM_STATUS, "21",2);
                pillar_return_to_sender(ci, cnt);
                goto PILLOUTDONE;
                break;
            case 8:
            case 9:
                set_rom_field(doj, ROM_STATUS, "5",1);
                pillar_return_to_sender(ci, cnt);
                goto PILLOUTDONE;
               break; 
            default:
                send_debug_message(p->logger,
                        "Status: %u ID:clordid %lu qty: %ld price %lu leaves: %ld\n",
                        pillcnl->acktype, pillcnl->orig_clordid, pillcnl->qty,
                        pillcnl->price, pillcnl->leaves);
                goto PILLOUTDONE;
                break;
        };
        pillar_return_out(ci, cnt);
    }
PILLOUTDONE:
    destroy_gk_db(ci->gk, db);
}

static void parse_pill_cnl_rej(pillcnlreprej* pillrej, pillp* p, con_int* ci)
{
            /*    send_debug_message(p->logger, "Reject ID: %lu Type: %u, reason: %u, ID: %lu \n",
                       pillrej->clordid, pillrej->rejtype,pillrej->reason, pillrej->clordid); */
    rcntr* cnt = 0;
    int ret = get_clordid_from_map(ci, pillrej->clordid, &cnt);
    if(ret && cnt->rlen == sizeof(dart_order_obj)) {
        dart_order_obj* doj = (dart_order_obj*)cnt->raw;
        set_rom_field(doj, 0, "S", 1);
        databuf_t* db = get_databuf(ci->gk);
        switch (pillrej->rejtype) {
            case 3:
                set_rom_field(doj, ROM_STATUS, "14", 2);
                break;
            case 1:
                {
                    if(doj->positions[ROM_STATUS].iov_len == 2) {
                        if(strncmp("27",
                                    doj->positions[ROM_STATUS].iov_base, 2) == 0) {
                            set_rom_field(doj, ROM_STATUS, "20", 2);
                        } else {
                            set_rom_field(doj, ROM_STATUS, "8", 1);
                            pillar_remove_from_maps(ci, pillrej->clordid);
                        }
                    } else {
                        set_rom_field(doj, ROM_STATUS, "8", 1);
                        pillar_remove_from_maps(ci, pillrej->clordid);
                    }
                break;
                }
            default:
                send_debug_message(p->logger, "Found Reject Type: %u, reason: %u, ID: %lu \n",
                        pillrej->rejtype,pillrej->reason, pillrej->clordid); 
                break;
        };
        int len = pillar_to_rom_time(pillrej->timestamp, db); 
        set_rom_field(doj, EXEC_TIME, db->rd_ptr, len);
        databuf_reset(db);
        databuf_write(db, "Pillar Reject: %u", pillrej->reason);
        set_rom_field(doj, ROM_TEXT, db->rd_ptr, databuf_unread(db));
        pillar_return_rej(ci, cnt);
        destroy_gk_db(ci->gk, db);
    }
}

static void parse_pill_priority(pillpriorack* pillprior, pillp* p, con_int* ci)
{
        send_debug_message(p->logger, "Priority change: %lu, %d \n", pillprior->clordid, pillprior->acktype);
    rcntr* cnt = 0;
    int ret = get_clordid_from_map(ci, pillprior->clordid, &cnt);
    if(ret) {
    }
}

static void send_bust_mail(pillbust* pillb, pillp* p, con_int* ci)
{
        send_debug_message(p->logger,
                "BUST Dealid: %u ID:clordid %lu qty: %ld price %lu leaves: %lu Reason: %u\n",
                pillb->dealid, pillb->clordid, pillb->lastqty,
                pillb->last_price, pillb->order_id, pillb->reason);
}
void parse_payload(char* d_off, uint32_t len_left, pillp* p, con_int* ci)
{
    uint16_t ord_type = 0;
    uint16_t len = 0;
    memcpy(&ord_type, d_off, 2);
    memcpy(&len, d_off + 2, 2);
    switch(ord_type) {
        case PILL_ORD_ACK:
            parse_ack((pillordack*)d_off, p, ci);
            break; 
        case PILL_EXE:
            parse_pill_exe((pillexerpt*)d_off, p, ci);
            break;
        case PILL_MOD_OUT_ACK:
            parse_pill_out((pillmodcnlack*)d_off, p, ci);
            break; 
        case PILL_CNL_REP_REJ:
            parse_pill_cnl_rej((pillcnlreprej*)d_off, p, ci);
            break; 
        case PILL_PRI_UPDATE:
            parse_pill_priority((pillpriorack*)d_off, p, ci);
            break; 
        case PILL_BUST:
            send_bust_mail((pillbust*)d_off, p, ci);
            break; 
        case 0x0232:
            build_symbol_map((pill_ref_data*)d_off, p);
            break;
        case 0x0272:
            log_mpid_config((mpid_config*)d_off, p);
            break;
        case 0x0221:
            log_session_config_ack(d_off, p);
            break;
        default:
            send_debug_message(p->logger, "Unknown: %d, %d \n", ord_type, len);
            break;
    }
}

uint8_t parse_stream_id(uint32_t id, uint8_t des_type, pillp* pc)
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
void parse_con_pillar(int *cut_con, rex_con *rc)
{
    uint16_t ord_type = 0;
    uint16_t mess_len = 0;
    pillp* pc = (pillp*)rc->parser;
    con_int *ci = (con_int *) pc->pbp;
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
                            manual_pill_hb(pc, ci);
                            start_pill_hb_timer(pc);
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
                            uint8_t is_set = parse_stream_id(r2.value, 13, pc);
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
                                send_pillar_open(ci, str->stream_id,
                                        next_seq, str->access);
                                pc->in_seq = next_seq;
                            }
                            is_set = parse_stream_id(r2.value, 15, pc);
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
                                send_pillar_open(ci, str->stream_id,
                                        str->next_seq, ret_len);
                            }
                            is_set = parse_stream_id(r2.value, 33, pc);
                            if(is_set && pc->ref_str_id == 0) {
                                pc->ref_str_id = str->stream_id;
                                send_pillar_open(ci, str->stream_id,
                                        1, str->access);
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
                        parse_payload(d_off + 32, len_left, pc, ci);
                        memcpy(&pc->in_seq, d_off + 12, 8);
                        record_inbound_message(pc->logger, 1, ci->logon->name, ci->logon->nlen);
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
             goto DONE_PARSIN_PILLS;
        }
    } while(len_left > 4);
DONE_PARSIN_PILLS:
    rc->con->rd_ptr += bytes_used;
}
void parse_ss_pillar(int* cut_con, rex_con* rc)
{
}
static void pillar_listener_connect(void* pass_back, int is_connected,
        dart_order_obj* log)
{
    con_int* ab = (con_int*)pass_back;
    if (is_connected == 1 && log != NULL) {
        ab->update_oats(ab, is_connected);
    } else if(is_connected == 0) {
        ab->update_oats(ab, is_connected);
    }
}
static void pillar_client_connect(void* pass_back, int is_connected,
        dart_order_obj* log)
{
    pillp* uparse = (pillp*)pass_back;
    con_int* ci = uparse->pbp;
    switch(is_connected) {
        case 0:
            ci->can_send_orders = 0;
            ci->connected = 0;
            ci->sock = 0;
            uparse->wr_str_id = 0;
            uparse->rd_str_id = 0;
            uparse->ref_str_id = 0;
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
            ci->sock = uparse->rc->sock;
            ci->can_send_orders = 1;
            ci->connected = 1;
            break;
        case 2:
            ci->sock = uparse->rc->sock;
            ci->can_send_orders = 0;
            ci->connected = 1;
            break;
        case -1:
            is_connected = 0;
            ci->can_send_orders = 0;
            ci->connected = 0;
            break;
    }
    ci->update_oats(ci, is_connected);
}

static hashmap_t recopy_order_id_map(con_int* ci)
{
    hashmap_t db_map = get_initial_order_map(ci->gk, ci->ex->dest_id,
            ci->ex->dest_id_len);
    hashmap_t real_map = create_map_with_hash(ci->ex->order_allocation, hashfuncOAT);
    hashmap_iterator_t hi = hashmap_iterator_begin(db_map);
    rcntr* cnt = 0;
    char *key = 0;
    int key_len = 0;
    int valid = hashmap_iterator_dereference(hi, &key,
                                             &key_len,
                                             (void *) &cnt);
    while (valid) {
        no_copy_insert(real_map, key, key_len, (void*)cnt);
        valid = hashmap_iterator_next(db_map, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &cnt);
    }
    hashmap_iterator_destroy(hi);
    remove_from_init_orders(ci->gk, ci->mod_name, ci->mod_name_len);
    return real_map;
}

void create_connection(con_int * ci, char *filename, int name_len,
        gatekeeper * gk)
{
    ci->gk = gk;
    ci->cl_ord_id_map = recopy_order_id_map(ci);
    ci->sbm = 0;
    ci->tg = 0;
    if(ci->is_server) {
        ci->sbm = 0;
        ci->tg = 0;
        ci->gk = gk;
        ci->send_to_con = pillar_send;
        ci->can_send_orders = 0;
        ci->on_rej = pillar_send;
        ci->on_replaced = pillar_send;
        ci->on_out = pillar_send;
        ci->on_fill = pillar_send;
        ci->on_update = pillar_send;
        add_int_orderids(ci);
        pillp* pp = create_pillar_parser(ci->o, ci, NULL);
        rex_con* rc = add_listener(ci->p, ci->ex, pp, ci,
                pillar_write_callback, 
                pillar_listener_connect,
                parse_ss_pillar,
                validate_logon);
        ci->cparser = pp;
        ci->rc = rc;
    } else {
        int ret_len = 0;
        get_config_val(ci->ex, "SERVER", 6,
                "PILLAR DIRECT", 13, &ret_len);
        if(ret_len > 0) {
            ci->on_enter_order = pillar_wrapper;
            ci->on_replace_order = pillar_wrapper;
            ci->on_cancel_order = pillar_wrapper;
            //ci->on_cancel_all = pillar_direct_cnl_all;
        } else {
            ci->on_enter_order = pillar_doj_enter;
            ci->on_replace_order = pillar_doj_replace;
            ci->on_cancel_order = pillar_doj_cancel;
            ci->on_cancel_all = pillar_cnl_all;
        }
        //ci->cleansing_rinse = pillar_cleansing_rinse;
        ci->can_send_orders = 0;
        add_int_orderids(ci);
        login_info* li = build_logon_info_from_config(ci->ex);
        ci->logon = li;
        pillp* pp = create_pillar_parser(ci->o, ci, li);
        ret_len = 0;
        char* clear_map = get_config_val(ci->ex, "SERVER", 6,
            "GSFlip.map", 10, &ret_len);
        if(ret_len > 0) {
            pp->clr_accs = create_suffix_map(clear_map);
        } else {
            pp->clr_accs = 0x0;
        }
        clear_map = get_config_val(ci->ex, "SERVER", 6,
                "idtomnemonic", 9, &ret_len);
        if(ret_len <=0 ) {
            ci->sbm = create_service_bureau_matcher("IDtomnemonic.map");
        } else {
            ci->sbm = create_service_bureau_matcher(clear_map);
        }
        rex_con* rc = create_rex_con(ci->p, ci->ex, pp, pp,li,
                pillar_write_callback, 
                pillar_client_connect,
                parse_con_pillar, NULL);
        rc->parser = pp;
        rc->connect = send_pillar_logon;
        pp->rc = rc;
        ci->rc = rc;
        rex_connect(ci->p, rc);
    }
}
