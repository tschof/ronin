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
#include "rex_constants.h"
#include "ex_config_helper.h"
#include "dart_handler.h"
#include "connection_interface.h"
#include "connection_factory.h"
#include "dart_handler.h"
#include "rex_seq_request.h"
#include "rex_allocator.h"
#include "risk_cntr_structs.h"
#include "rom_fix_trans_funcs.h"
#include "mailer.h"
#include "log_mnger.h"
#include "suffix_map.h"
#include "rb_tree.h"
#include "otto.h"
#include "otto_parser.h"

#define MICRO_HOUR 3600000000
#define MICRO_MIN 60000000
#define MICRO_SEC 1000000


struct otto_parser {
    otl* logger;
    login_info* logon;
    con_int* ci;
    rex_con* rc;
    gatekeeper* gk;
    token_gen* tg;
    databuf_t* mess_to_send;
    databuf_t* sym_maker;
    pthread_spinlock_t send_mutex;
    call_back_wrapper* recon_wrap;
    call_back_wrapper* hb_timer;
    hashmap_t prod_codes;
    rb_tree_t* instruments;
    FILE* ifile;
    char* firmid;
    char* mpid;
    int fidlen;
    int mpidlen;
    int is_logged_on;
    time_t last_sent;
    int in_seq;
};

static void otto_create_inst_file(const char* filename, ottop* op)
{
    op->ifile = fopen(filename, "w+");
        send_debug_message(op->logger,
                "Filename:%s, was created: %d,\n", filename, op->ifile);
}
static char* otto_get_file_contents(const char* filename, long* size, ottop* otp)
{
    long res = 0;
    char* buff = 0;
    int ret = 0;
    otp->ifile = fopen(filename, "r+");
    if(otp->ifile != 0) {
        fseek(otp->ifile, 0, SEEK_END);
        *size = ftell(otp->ifile);
        rewind(otp->ifile);
        REX_ALLOC(buff, 8, *size + 1, ret);
        res = fread(buff, 1, *size, otp->ifile);
        if(res != *size) {
            free(buff);
            buff = 0;
        }
    } 
    return buff;
}

static void set_instrument_id(ottop* op, char* value, int val_len, int tag)
{
    char* rom_sym = calloc(1, val_len + 1);
    memcpy(rom_sym, value, val_len);
    uint32_t* data = malloc(sizeof(uint32_t));
    memcpy(data, &tag, sizeof(uint32_t));
    rb_tree_no_delete_insert(op->instruments, data, rom_sym);
}

static void otto_build_map_from_file(const char* filename, hashmap_t map, ottop* op)
{
    char* buffer = 0;
    long size = 0;
    buffer = otto_get_file_contents(filename, &size, op);
    if(size > 0) {
        char* start = buffer;
        char* comma = buffer;
        char* end = buffer;
        int tag_len = 0;
        int val = 0;
        do {
            if(*end == ',') {
                tag_len = end - start;
                comma = end;
            } else if(*end == '\n') {
                val = atoi(comma + 1);
                insert_map(map, start, tag_len, &val, sizeof(int));
                set_instrument_id(op, start, tag_len, val);
                start = end + 1;
            }
            ++end;
            --size;
        } while(size > 0);
    }
}
static int otto_key_compare(const void* a, const void *b)
{
    if(*(int*)a > *(int*)b) {
        return 1;
    }
    if(*(int*)a < *(int*)b) {
        return -1;
    } 
    return 0;
}
static void otto_int_print(const void* a)
{
    printf("%d", *(int*)a);
}

static void otto_info_print(const void* a)
{
    printf("Printing RC Cola.\n");
}
static void otto_key_destroy(void* a)
{
    free((int*)a);
}
static void otto_info_destroy(void* a)
{
   ottosid* rc = (ottosid*)a;
   free(rc);
}

static int get_order_details(con_int* ci, char* clordid, int clordid_len, rcntr** cnt)
{
    pthread_spin_lock(&ci->mutex);
    /*START LOCK*/
    int ret = get_map_entry(ci->cl_ord_id_map, clordid, clordid_len, (void**)cnt);
    /*END LOCK*/
    pthread_spin_unlock(&ci->mutex);
    return ret;
}
static void remove_order_details(con_int* ci, char* clordid, int clordid_len)
{
    pthread_spin_lock(&ci->mutex);
    remove_from_map(ci->cl_ord_id_map, clordid, clordid_len);
    pthread_spin_unlock(&ci->mutex);
}
uint32_t get_instrumentid(char* sym, int slen,
                          char* expire, int elen,
                          char* strike, int tlen,
                          char* exday, int edlen,
                          char put_call, ottop* otp)
{
    uint32_t* instrumentid = 0;  
    uint32_t ret = 0;
    int len = get_map_entry(otp->prod_codes, sym, slen, (void**)&instrumentid);
    if(len > 0) {
        ret = *instrumentid;
    }
    return ret;
}

uint32_t get_instrument_with_sec_def(ottop* otp, char* sec_def, int slen)
{
    uint32_t* instrumentid = 0;  
    uint32_t ret = 0;
    int len = get_map_entry(otp->prod_codes, sec_def, slen, (void**)&instrumentid);
    if(len > 0) {
        ret = *instrumentid;
    }
    return ret;
}
static void write_inst_to_fiel(ottop* otp, ottosid* osid, uint64_t strike)
{
    databuf_write(otp->sym_maker, ",%d\n", osid->instrumentid);
    int len = databuf_unread(otp->sym_maker);
    int written = 0;
    if(otp->ifile != 0x0) {
        written = fwrite(otp->sym_maker->rd_ptr, 1,len, otp->ifile);
    }
    if(written < len && otp->logger) {
        send_debug_message(otp->logger,
                "Write fail: %s %02d%02d %02d %c %08d %d\n",
                osid->productname, osid->expiryear, osid->expirmon,
                osid->expirday, osid->optiontype, strike/1000,
                osid->instrumentid);
    }
    if(otp->ifile != 0x0) {
        fflush(otp->ifile);
    }
}
void create_instrument_sym(ottop* otp, ottosid* osid)
{
    int prod_len = 5;/*We have 13 character space for product*/
    ouch_find_token_len(osid->securitysym, prod_len);
    if(prod_len > 0) {
        databuf_reset(otp->sym_maker);
        databuf_memcpy(otp->sym_maker, osid->securitysym, prod_len);
        do {
            databuf_memcpy(otp->sym_maker, "_", 1);
            ++prod_len;
        } while(prod_len < 6);
        int64_t strike = Swap8Bytes(osid->strikeprice);
        databuf_write(otp->sym_maker, "%02d%02d%02d%c%08d", osid->expiryear,
                osid->expirmon, osid->expirday, osid->optiontype,
                strike/1000);
        uint32_t* data = malloc(sizeof(uint32_t));
        memcpy(data, &osid->instrumentid, sizeof(uint32_t));
        no_copy_insert(otp->prod_codes, otp->sym_maker->rd_ptr, 
               databuf_unread(otp->sym_maker), data); 
        set_instrument_id(otp, otp->sym_maker->rd_ptr,
                databuf_unread(otp->sym_maker), osid->instrumentid);
        write_inst_to_fiel(otp, osid, strike);
    }
}

void set_rom_symbol(ottop* otp, uint32_t ottosym, dart_order_obj* doj)
{
    char* rom_sym = rb_tree_get_val(otp->instruments, &ottosym);
    if(rom_sym ) {
        fill_in_rom_field(doj, ROM_SEC_DEF, rom_sym, 21);
        char* base = memchr(rom_sym, '_', 8);
        if(base) {
            fill_in_rom_field(doj, ROM_SYM, rom_sym, (base - rom_sym));
        }
        do {
            ++base;
        } while (base && *base == '_');
        if(base) {
            char expire[8];
            memset(expire, '\0', 8);
            memcpy(expire, "20", 2);
            memcpy(expire + 2, base, 4);
            fill_in_rom_field(doj, ROM_EXPIRE_DATE, expire, 6);
            base += 4;
            fill_in_rom_field(doj, ROM_EXPIRE_DAY, base, 2);
            base += 2;
            fill_in_rom_field(doj, ROM_P_C, base, 1);
            base += 1;
            char striker[32];
            memset(striker, '\0', 32);
            memcpy(striker, base, 8);
            int istrike = atoi(striker);
            double div = 1000.0;
            double dstrike = ((double)istrike)/div;
            dstrike += .0000001;
            char sstrike[32];
            memset(sstrike, '\0', 32);
            int strike_len = sprintf(sstrike, "%f", dstrike);
            fill_in_rom_field(doj, ROM_STRIKE, sstrike, strike_len);
        }
    }
}

static void otto_direct_send(ottop* up, void* data, int len)
{
    int bytes_sent = 0;
    if(pthread_spin_lock(&up->send_mutex) == 0) {
        time(&up->last_sent);
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
void parse_otto(int *cut_con, rex_con *pc);
void send_otto_logon(void *pa)
{
    ottop* ep = (ottop*)pa;
    con_int *ob = (con_int *) ep->ci;
    databuf_t *buff = get_databuf(ob->gk);
    memset(buff->buffer, '\0', 256);
    time_t now;
    time(&now);
    int build_secs = ll_before_reset_time(ep->logon->reset_day,
                                          ep->logon->reset_hour,
                                          ep->logon->reset_min,
                                          ep->logon->last_logon,
                                          now);
    int ret = 0;
    char* filename = get_val_for_tag(ob->ex->dc, "SERVER", 6, "PCODE LOC", 9,
            &ret);
    if(build_secs) {
        if(ret <= 0) {
            filename = "OUCH_INSTR_MAP.log";
        }
        otto_create_inst_file(filename,ep);
        ep->in_seq = 1;
    } else {
        if(ret > 0) {
            otto_build_map_from_file(filename, ep->prod_codes, ep);
        }
        /*sequence_req *sr = create_sequence_request(ob->mod_name, ob->mod_name_len,
                                                   ep->logon,NULL, 
                                                   build_offsets,
                                                   create_rom_resend_pad, ep);
        get_sequence_numbers(sr, ep->logger);
        ep->in_seq = dsr_get_incoming(sr);
        destroy_sequence_req(sr);*/
        ep->in_seq = 0;
    }
    
    short mess_len = 47;
    mess_len = htons(mess_len);
    databuf_memcpy(buff, &mess_len, 2);
    databuf_memcpy(buff, "L", 1);
    databuf_write(buff, "%-6s", ep->logon->name);
    databuf_write(buff, "%-10s", ep->logon->password);
    databuf_write(buff, "%-10s", " ");
    databuf_write(buff, "%-20d", ep->in_seq);
    buff->wr_ptr = buff->buffer + 49;
    otto_direct_send(ep, buff->rd_ptr, databuf_unread(buff));
    destroy_gk_db(ob->gk, buff);
}
void send_otto_logout(void* pa)
{
    ottop* ep = (ottop*)pa;
    con_int *ob = (con_int *) ep->ci;
    databuf_t *buff = get_databuf(ob->gk);
    memset(buff->buffer, '\0', 5);
    uint16_t len = 1;
    len = htons(len);
    databuf_memcpy(buff, &len, 2);
    databuf_memcpy(buff, "O", 1);
    otto_direct_send(ep, buff->rd_ptr, databuf_unread(buff));
    destroy_gk_db(ob->gk, buff);
}
void just_send_hb(ottop* fco)
{
    databuf_t *buff = get_databuf(fco->gk);
    memset(buff->buffer, '\0', 5);
    unsigned char var = 'R';
    short ml = htons(1);
    databuf_memcpy(buff, &ml, 2);
    databuf_memcpy(buff, &var, 1);
    otto_direct_send(fco, buff->rd_ptr, databuf_unread(buff)); 
    destroy_gk_db(fco->gk, buff);
}
void send_otto_hb(void* pc)
{
    ottop *fco = (ottop *) pc;
    if(fco->is_logged_on) {
        long diff = 1;
        /*SEND HB*/
        databuf_t *buff = get_databuf(fco->gk);
        memset(buff->buffer, '\0', 5);
        unsigned char var = 'R';
        short ml = htons(1);
        databuf_memcpy(buff, &ml, 2);
        databuf_memcpy(buff, &var, 1);
        otto_direct_send(fco, buff->rd_ptr, databuf_unread(buff)); 
        destroy_gk_db(fco->gk, buff);
        add_back_rep_timer_adjust(fco->rc->p,fco->hb_timer,diff);
    }
}
void start_otto_hb_timer(ottop* fp)
{
    fp->hb_timer =  add_timer(fp->rc->p,1,
            0, 1,0, fp,send_otto_hb);
}

void build_fill_not_found_mail(con_int * ob, char *d_off,
                               int len)
{
/*    char *title = "Unmatched fill from OUCH";
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
    send_biz_message(buff->buffer, databuf_unread(buff), title, t_len); */
}
void build_bust_mail(con_int* ob, char* d_off, int len)
{
  /*  char *title = "Bust from OUCH";
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
    send_biz_message(buff->buffer, databuf_unread(buff), title, t_len); */
}

static int set_order_details(con_int* ci, char* clordid, int clordid_len, rcntr* cnt)
{
    pthread_spin_lock(&ci->mutex);
    /*START LOCK*/
    int ret = no_copy_insert(ci->cl_ord_id_map, clordid, clordid_len, cnt);
    /*END LOCK*/
    pthread_spin_unlock(&ci->mutex);
    return ret;
}
void otto_package_packet(ottop* otp, char* data, int dlen)
{
    databuf_t *buff = (databuf_t *) get_databuf(otp->gk);
    uint16_t len = htons(dlen + 1);
    databuf_memcpy(buff, &len, 2);
    databuf_memcpy(buff, "U", 1);
    databuf_memcpy(buff, data, dlen);
    otto_direct_send(otp, buff->rd_ptr, databuf_unread(buff));
    destroy_gk_db(otp->ci->gk, buff);
}

int create_cmplx_strategy(ottop* top, dart_order_obj* doj, con_int* ci, rcntr* cnt)
{
    int ret_val = 0;
    ottocmplx* ocmp = 0;
    REX_ALLOC(ocmp, 8, sizeof(ottocmplx), ret_val);   
    ocmp->type = 'S';
    memset(ocmp->firmid, ' ', 4);
    memcpy(ocmp->firmid, top->firmid, top->fidlen);
    memset(ocmp->clrequestid, ' ',16);
    int clordid_len = create_in_place_ord_token(top->tg, ocmp->clrequestid, 16);
    if(doj->positions[ROM_SYM].iov_len > 0) {
        memset(ocmp->productname, ' ', 13);
        memcpy(ocmp->productname, doj->positions[ROM_SYM].iov_base,
                doj->positions[ROM_SYM].iov_len);
    } else if(doj->positions[ROM_UNDER].iov_len > 0) {
        memset(ocmp->productname, ' ', 13);
        memcpy(ocmp->productname, doj->positions[ROM_UNDER].iov_base,
                doj->positions[ROM_UNDER].iov_len);
    } else {
        ret_val = -1;
        goto NOCMPLX;
    }
    ocmp->numlegs = doj->num_childs;
    databuf_t* sender = (databuf_t*)get_databuf(top->gk);
    databuf_memcpy(sender, ocmp, sizeof(ottocmplx)); 
    uint8_t child_num = 0;
    children* lch = doj->childs;
    do {
        ottolegdef* old = (ottolegdef*)sender->wr_ptr;
        dart_order_obj* ldoj = (dart_order_obj*)lch->payload;
        char* type = ldoj->positions[ROM_SEC_TYPE].iov_base;
        if(type[0] == 'E') {
            old->legtype = 'S';
            old->leginstrumentid = 0;
        } else {
            old->legtype = 'O';
            if(ldoj->positions[ROM_SEC_DEF].iov_len > 0) {
                old->leginstrumentid = get_instrument_with_sec_def(top,
                        ldoj->positions[ROM_SEC_DEF].iov_base,
                        ldoj->positions[ROM_SEC_DEF].iov_len);
                set_rom_field(ldoj, ROM_EXCHANGE_SYM,&old->leginstrumentid,
                        sizeof(old->leginstrumentid));
            }
        }
        databuf_t* ordiddb = get_databuf(ci->gk);
        dart_itoa(ldoj, child_num, ROM_CLORDID, ordiddb);
        set_rom_field(ldoj, ROM_BIN_CLORDID, &child_num, sizeof(uint8_t));
        destroy_gk_db(ci->gk, ordiddb);
        
        char* side = ldoj->positions[ROM_SIDE].iov_base;
        if(side[0] == '1') {
            old->legside = 'B';
        } else {
            old->legside = 'S';
        }
        char shares[10];
        memset(&shares, '\0', 10);
        memcpy(&shares, ldoj->positions[ROM_SHARES].iov_base,
                ldoj->positions[ROM_SHARES].iov_len);
        old->legratio = atoi(shares);
        ldoj->risk.size = old->legratio * cnt->size;
        old->legratio = htons(old->legratio);
        sender->wr_ptr += sizeof(ottolegdef);
        lch = lch->next;
        ++child_num;
    }while(lch);
    otto_package_packet(top, sender->rd_ptr, databuf_unread(sender));
    set_order_details(ci, ocmp->clrequestid, clordid_len, cnt);
    destroy_gk_db(top->gk, sender);
NOCMPLX:
    free(ocmp);
    return ret_val;
}

static void reset_leaves(dart_order_obj* doj) 
{
    if(doj->positions[ROM_SHARES].iov_len > 0) {
        set_rom_field(doj, ROM_LEAVES,
                doj->positions[ROM_SHARES].iov_base,
                doj->positions[ROM_SHARES].iov_len);
    }
}
int on_otto_enter_order(con_int * ci, rcntr* cnt)
{
    int total_len = 0;
    if(cnt->rlen == sizeof(dart_order_obj)) {
        dart_order_obj* doj = (dart_order_obj*)cnt->raw;
        ottop* otp = (ottop*)ci->cparser;
        reset_leaves(doj);
        if(doj->num_childs <= 0) {
            ottono* ono = create_otto_new_order_message(cnt,otp,ci,otp->tg);
            if (ono) {
                memcpy(ono->firmid, otp->firmid, otp->fidlen);
                otto_package_packet(otp, (char*)ono, sizeof(ottono));
                free(ono);
            } else {
                total_len = -1;
            }
        } else {
            total_len = create_cmplx_strategy(otp, doj, ci, cnt);
        }
    } 
    return total_len;
}
int on_otto_replace_order(con_int * ci,rcntr* cnt)
{
    int total_len = 0;
    if(cnt->rlen == sizeof(dart_order_obj)) {
        ottop* otp = (ottop*)ci->cparser;
        ottorep* otr = create_otto_replace_message(cnt, otp, ci, otp->tg);
        if(otr) {
            memcpy(otr->firmid, otp->firmid, otp->fidlen);
            otto_package_packet(otp, (char*)otr, sizeof(ottorep));
            free(otr);
        } else {
            total_len = -1;
        }
    }
    return total_len;
}
int on_otto_cancel_order(con_int * ci, rcntr* cnt)
{ 
    int total_len = 0;
    if(cnt->rlen == sizeof(dart_order_obj)) {
        ottop* otp = (ottop*)ci->cparser;
        ottocnl* otc = create_otto_cancel_message(cnt, otp, ci, otp->tg);
        if(otc) {
            memcpy(otc->firmid, otp->firmid, otp->fidlen);
            otto_package_packet(otp, (char*)otc, sizeof(ottocnl));
            free(otc);
        } else {
            total_len = -1;
        }
    }
    return total_len;
}
void send_cmplx_order(ottop* otp,ottocmplxresp *ocr)
{
    int clordid_len = 16;
    rcntr* cnt = 0;
    ouch_find_token_len(ocr->clrequestid, clordid_len);
    if(get_order_details(otp->ci, ocr->clrequestid, clordid_len, &cnt)) {
        dart_order_obj* doj = (dart_order_obj*)cnt->raw;
        set_rom_field(doj, ROM_EXCHANGE_SYM, &ocr->instrumentid,
                sizeof(ocr->instrumentid)); 
        ottono* ono = create_otto_new_cmplx(cnt,otp,otp->ci,otp->tg,
                ocr->instrumentid, otp->mpid, otp->mpidlen);
        if (ono) {
            memcpy(ono->firmid, otp->firmid, otp->fidlen);
            otto_package_packet(otp, (char*)ono, sizeof(ottono));
            free(ono);
        }
        send_debug_message(otp->logger,
                "Created Complex Intrument: %lu,request ID: %lu \n",
                ntohl(ocr->instrumentid), ocr->clrequestid);
    } else {
        send_debug_message(otp->logger,
                "Failed to find Complex Intrument: %lu,ID: %lu \n",
                ntohl(ocr->instrumentid), ocr->clrequestid);
    }
}
int build_otto_offset(rex_con *apa)
{
    return 0;
}

ottop* create_empty_otto_parser()
{
    int ret = 0;
    ottop* fp = 0;
    REX_ALLOC(fp, 8, sizeof(struct otto_parser), ret);
    fp->logger = NULL;
    fp->ci = NULL;
    fp->logon = NULL;
    fp->gk = NULL;
    fp->tg = generic_creation_func();
    fp->prod_codes = create_map(16383);
    fp->mess_to_send = databuf_alloc(1024);
    fp->sym_maker = databuf_alloc(512);
    pthread_spin_init(&fp->send_mutex, PTHREAD_PROCESS_PRIVATE);
    fp->is_logged_on = 0;
    fp->instruments = create_rb_tree(otto_key_compare,
            otto_key_destroy,
            otto_info_destroy,
            otto_int_print,
            otto_info_print);
    return fp;
}
ottop* create_otto_parser(otl* logger, con_int* pbp, login_info* u)
{
    int ret = 0;
    ottop* fp = 0;
    REX_ALLOC(fp, 8, sizeof(struct otto_parser), ret);
    fp->logger = logger;
    fp->ci = pbp;
    fp->logon = u;
    fp->gk = pbp->gk;
    fp->tg = generic_creation_func();
    fp->prod_codes = create_map(16383);
    fp->mess_to_send = databuf_alloc(1024);
    fp->sym_maker = databuf_alloc(512);
    pthread_spin_init(&fp->send_mutex, PTHREAD_PROCESS_PRIVATE);
    fp->is_logged_on = 0;
    pbp->cparser = fp;
    int ret_len = 0;
    char* temp = 0;
    temp = get_config_val(pbp->ex, "SERVER", 6,
            "Firm ID", 7, &ret_len);
    fp->fidlen = ret_len;
    fp->firmid = malloc(ret_len);
    memcpy(fp->firmid, temp, ret_len);
    temp= get_config_val(pbp->ex, "SERVER", 6,
            "mpid", 7, &ret_len);
    fp->mpidlen = ret_len;
    fp->mpid = malloc(ret_len);
    memcpy(fp->mpid, temp, ret_len);
    fp->instruments = create_rb_tree(otto_key_compare,
            otto_key_destroy,
            otto_info_destroy,
            otto_int_print,
            otto_info_print);
    return fp;
}

/**
 * 
 */
static void otto_cancel_all_orders(con_int* ci)
{
    ottop* otp = (ottop*)ci->cparser;
    int ret = 0;
    ottomcnl* omcnl = 0;
    REX_ALLOC(omcnl, 8, sizeof(ottomcnl), ret);
    omcnl->type = 'U';
    omcnl->instrumenttype = 'A';
    memset(omcnl->clrequestid, ' ',16);
    create_in_place_ord_token(otp->tg, omcnl->clrequestid, 16);
    memset(omcnl->firmid, ' ',4);
    memcpy(omcnl->firmid, otp->firmid, otp->fidlen);
    omcnl->scope = 'F';
    memset(omcnl->underlyingsym, ' ',13);
    otto_package_packet(otp, (char*)omcnl, sizeof(ottomcnl));
    free(omcnl);
}
static void otto_on_connect(void* pass_back, int is_connected,
                           dart_order_obj* log)
{
    ottop* uparse = (ottop*)pass_back;
    con_int* ci = uparse->ci;
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
	    {
		    time_t now;
		    time(&now);
		    uparse->logon->last_logon = now;
	    }
            ci->can_send_orders = 1;
            ci->connected = 1;
            break;
        case 2:
            ci->can_send_orders = 0;
            ci->connected = 1;
            break;
        case -1:
            otto_cancel_all_orders(ci);
            send_otto_logout(ci->cparser);
            is_connected = 0;
            ci->can_send_orders = 0;
            ci->connected = 0;
    }
    ci->update_oats(ci, is_connected);
}

void otto_write_callback(void* cb)
{
    rex_con* rc = (rex_con*)cb;
    ottop* up = (ottop*)rc->parser;
    pthread_spin_lock(&up->send_mutex);
    int blen = databuf_unread(up->mess_to_send);
    time(&up->last_sent);
    int disconnect = 0;
    int bytes_sent = gen_send(up->rc, up->mess_to_send->rd_ptr, blen,
                &disconnect);
    up->mess_to_send->rd_ptr += bytes_sent;
    pthread_spin_unlock(&up->send_mutex);
    if(bytes_sent < blen && up->rc->disconnect <= 0) {
        add_sender(rc->p, rc);
    } else if(disconnect) {
        up->rc->disconnect = disconnect;
        up->is_logged_on = 0;
    } 
}
void otto_cleansing_rinse(con_int* ci)
{
}
void create_connection(con_int * ci, char *filename, int name_len,
                       gatekeeper * gk)
{
/**/
    ci->cl_ord_id_map = get_initial_order_map(gk, ci->ex->dest_id,
                                              ci->ex->dest_id_len);
    ci->sbm = 0;
    ci->tg = 0;
    ci->on_enter_order = on_otto_enter_order;
    ci->on_replace_order = on_otto_replace_order;
    ci->on_cancel_order = on_otto_cancel_order;
    ci->cleansing_rinse = otto_cleansing_rinse;
    ci->on_cancel_all = otto_cancel_all_orders;
    ci->can_send_orders = 0;
    add_int_orderids(ci);
    login_info* li = build_logon_info_from_config(ci->ex);
    ottop* otp = create_otto_parser(ci->o, ci, li);
    rex_con* rc = create_rex_con(ci->p, ci->ex, otp, otp,li,
                                  otto_write_callback, 
                                  otto_on_connect,
                                  parse_otto, NULL);
    rc->parser = otp;
    rc->connect = send_otto_logon;
    otp->rc = rc;
    ci->rc = rc;
    rex_connect(ci->p, rc);
}

static void send_event_code_mail(con_int* ci, ottosysevent* osys)
{
    char *title = "System message from OUCH";
    int t_len = 24;
    databuf_t *buff = get_databuf(ci->gk);
    switch(osys->eventcode) {
        case 'O':
            databuf_write(buff, "Start of Messages %c \n", osys->eventcode);
            break;
        case 'S':
            databuf_write(buff, "Start of System Hours %c \n", osys->eventcode);
            break;
        case 'Q':
            databuf_write(buff, "Start of Opening Process %c \n", osys->eventcode);
            break;
        case 'N':
            databuf_write(buff, "End of Normal Hours %c \n", osys->eventcode);
            break;
        case 'L':
            databuf_write(buff, "End of late Hours %c \n", osys->eventcode);
            break;
        case 'E':
            databuf_write(buff, "End of system Hours %c \n", osys->eventcode);
            break;
        case 'C':
            databuf_write(buff, "End of Messages %c \n", osys->eventcode);
            break;
    };
    send_biz_message(buff->buffer, databuf_unread(buff), title, t_len); 
    destroy_gk_db(ci->gk, buff);
}
/**
 */
void check_trade_details(ottop* otp, ottotd* otd)
{
    char clordid[32];
    otd->orderid = ntohl(otd->orderid);
    otd->matchid = ntohl(otd->matchid);
    otd->exeprice = Swap8Bytes(otd->exeprice);
    otd->lastshares = ntohl(otd->lastshares);
    otd->occaccount = ntohl(otd->occaccount);
    memset(clordid, '\0', 32);
    memcpy(clordid, otd->clordid, 16);
    if(otd->transtype == 'B' || 
            otd->transtype == 'C') {
        char* title = 0;
        int t_len = 0;
        if(otd->transtype == 'B') {
            title = "Bust from OTTO";
            t_len = 14;
        } else {
            title = "Trade Modified by OTTO";
            t_len = 22;
        }
        databuf_t *buff = get_databuf(otp->ci->gk);
        databuf_write(buff,
            "Trade Details: type: %c, source: %c, OrdID: %d,match ID: %d, %s, %c, price: %ld, qty: %d \n", 
            otd->transtype, otd->eventsource,
            otd->orderid, otd->matchid, clordid,otd->side,
            otd->exeprice, otd->lastshares);
        send_biz_message(buff->buffer, databuf_unread(buff), title, t_len); 
        destroy_gk_db(otp->ci->gk, buff);
    }

    send_debug_message(otp->logger,
            "Trade Details: type: %c, source: %c, OrdID: %d,match ID: %d, %s, %c, price: %ld, qty: %d \n", 
            otd->transtype, otd->eventsource,
            otd->orderid, otd->matchid, clordid,otd->side,
            otd->exeprice, otd->lastshares);

}

void send_trading_action_message(ottop* otp, ottointam* otinm)
{
    send_debug_message(otp->logger, "Action Message, product: %lu, instrument: %lu is: %c \n",
            otinm->productid, otinm->instrumentid, 
            otinm->tradingstate);
}
void log_complex_resp(ottop* otp, ottocmplxresp* ocr)
{
    send_debug_message(otp->logger, "Complex Instrument: %lu,request id : %s \n",
            ocr->instrumentid, ocr->clrequestid); 
}
void log_complex_inst(ottop* otp, ottocid* ocid)
{
    send_debug_message(otp->logger, "Complex Intrument: %lu,product name: %s instrumentid: %lu, num legs:%lu \n",
            ocid->productid, ocid->productname, 
            ntohl(ocid->instrumentid), ocid->numlegs);
}
static void send_back_spread_legs(con_int* ci, dart_order_obj* daddy,
        void(*back)(con_int* lci, rcntr* risk), rcntr* cnt)
{

    children* lch = daddy->childs;
    do {
        dart_order_obj* ldoj = (dart_order_obj*)lch->payload;
        set_rom_field(ldoj, 0, "S", 1);
        set_rom_field(ldoj, ROM_STATUS, daddy->positions[ROM_STATUS].iov_base,
                daddy->positions[ROM_STATUS].iov_len);
        cnt->raw = ldoj;
        back(ci, cnt);
        lch = lch->next;
    } while(lch);
    cnt->raw = daddy;
}
static void parse_seq_packet(ottop *otp, const char* message, size_t mess_len)
{
    size_t clordid_len= 16;
    rcntr *obj = 0;
    dart_order_obj* doj = 0;
    switch(message[0]) {
        case 'a':
            {
                ottooa* oak = (ottooa*)message;
                ouch_find_token_len(oak->clordid, clordid_len);
                if(get_order_details(otp->ci, oak->clordid, clordid_len, &obj)) {
                    doj = (dart_order_obj*)obj->raw;
                    if(lock_n_check(doj)) {
                        databuf_t *buff = get_databuf(otp->ci->gk);
                        update_order_ack(obj, oak,buff);
                        if(doj->sender != 0x0) {
                            con_int* ct = (con_int*) doj->sender;
                            ct->on_update(ct, obj);
                            if(doj->num_childs > 0) {
                                send_back_spread_legs(ct, doj, ct->on_update, obj);
                            }
                        } else {
                            con_int* sender = 0;
                            get_obj_by_client_id(otp->ci->gk,
                                    obj->owner,obj->olen, (void**)&sender);        
                            if(sender && sender->on_update) {
                                sender->on_update(sender, obj);
                                if(doj->num_childs > 0) {
                                    send_back_spread_legs(sender, doj, sender->on_update, obj);
                                }
                            }
                        }

                        destroy_gk_db(otp->ci->gk, buff);
                        unlock(doj);
                    }
                }
            }
            break;
        case 'b':
            {
                ottooashorty* oak = (ottooashorty*)message;
                ouch_find_token_len(oak->clordid, clordid_len);
                if(get_order_details(otp->ci, oak->clordid, clordid_len, &obj)) {
                    doj = (dart_order_obj*)obj->raw;
                    if(lock_n_check(doj)) {
                        update_short_order_ack(obj, oak);
                        if(doj->sender != 0x0) {
                            con_int* ct = (con_int*) doj->sender;
                            ct->on_update(ct, obj);
                            if(doj->num_childs > 0) {
                                send_back_spread_legs(ct, doj, ct->on_update, obj);
                            }
                        } else {
                            con_int* sender = 0;
                            get_obj_by_client_id(otp->ci->gk,
                                    obj->owner,obj->olen, (void**)&sender);        
                            if(sender && sender->on_update) {
                                sender->on_update(sender, obj);
                                if(doj->num_childs > 0) {
                                    send_back_spread_legs(sender, doj, sender->on_update, obj);
                                }
                            }
                        }

                        unlock(doj);
                    }
                }
            }
            break;
        case 'r':
            {
                ottorepl* orpl = (ottorepl*)message;
                ouch_find_token_len(orpl->origclordid, clordid_len);
                if(get_order_details(otp->ci, orpl->origclordid, clordid_len, &obj)) {
                    doj = (dart_order_obj*)obj->raw;
                    if(lock_n_check(doj) &&
                            lock_n_check((dart_order_obj *) doj->rep)) {
                        parse_replaced(obj, orpl, otp->ci);
                        unlock((dart_order_obj *) doj->rep);
                        unlock(doj);
                        if(doj->sender != 0x0) {
                            con_int* ct = (con_int*) doj->sender;
                            ct->on_replaced(ct, obj);
                            if(doj->num_childs > 0) {
                                send_back_spread_legs(ct, doj, ct->on_replaced, obj);
                            }
                        } else {
                            con_int* sender = 0;
                            get_obj_by_client_id(otp->ci->gk,
                                    obj->owner,obj->olen, (void**)&sender);        
                            if(sender && sender->on_replaced) {
                                sender->on_replaced(sender, obj);
                                if(doj->num_childs > 0) {
                                    send_back_spread_legs(sender, doj, sender->on_replaced, obj);
                                }
                            }
                        }

                    }
                }
            }
            break;
        case 'c':
            {
                ottocnled* ocnl = (ottocnled*)message;
                ouch_find_token_len(ocnl->clordid, clordid_len);
                if(get_order_details(otp->ci, ocnl->clordid, clordid_len, &obj)) {
                    doj = (dart_order_obj*)obj->raw;
                    if(lock_n_check(doj)) {
                        parse_ur_out(obj, ocnl);
                        unlock(doj);
                        if(doj->sender != 0x0) {
                            con_int* ct = (con_int*) doj->sender;
                            ct->on_out(ct, obj);
                            if(doj->num_childs > 0) {
                                send_back_spread_legs(ct, doj, ct->on_out, obj);
                            }
                        } else {
                            con_int* sender = 0;
                            get_obj_by_client_id(otp->ci->gk,
                                    obj->owner,obj->olen, (void**)&sender);        
                            if(sender && sender->on_out) {
                                sender->on_out(sender, obj);
                                if(doj->num_childs > 0) {
                                    send_back_spread_legs(sender, doj, sender->on_out, obj);
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 'e':
            {
                ottoexe* oexe = (ottoexe*)message;
                ouch_find_token_len(oexe->clordid, clordid_len);
                if(get_order_details(otp->ci, oexe->clordid, clordid_len, &obj)) {
                    doj = (dart_order_obj*)obj->raw;
                    if(lock_n_check(doj)) {
                        databuf_t *buff = get_databuf(otp->ci->gk);
                        rcntr* rcnt = parse_filled(obj, oexe, buff, otp);
                        unlock(doj);
                        if(rcnt) {
                            if(doj->sender != 0x0) {
                                con_int* ct = (con_int*) doj->sender;
                                ct->on_fill(ct, rcnt);
                                if(doj->num_childs > 0) {
                                    send_back_spread_legs(ct, doj, ct->on_fill, obj);
                                }
                            } else {
                                con_int* sender = 0;
                                get_obj_by_client_id(otp->ci->gk,
                                        obj->owner,obj->olen, (void**)&sender);        
                                if(sender && sender->on_fill) {
                                    sender->on_fill(sender, obj);
                                    if(doj->num_childs > 0) {
                                        send_back_spread_legs(sender, doj, sender->on_fill, obj);
                                    }
                                }
                            }
                        }
                        destroy_gk_db(otp->ci->gk, buff);
                    }
                } else {
                    build_fill_not_found_mail(otp->ci, (char*)message, mess_len);
                }
            }
            break;
        case 't':
            {
                ottotd* otd = (ottotd*)message;
                check_trade_details(otp, otd);
            }
            break;
        case 'i':
            {
                ottointam* otinm = (ottointam*)message;
                send_trading_action_message(otp, otinm);
            }
            break;
        case 'z':
            {
                ottosysevent* osys = (ottosysevent*)message;
                send_event_code_mail(otp->ci, osys);
            }
            break;
        case 'o':
            {
                ottosid* osid = (ottosid*)message;
                create_instrument_sym(otp, osid);
                just_send_hb(otp);
                //int64_t strike = Swap8Bytes(osid->strikeprice);
                /*send_debug_message(otp->logger,
                "Write DIS: %s %s %02d%02d %02d %c %08d %d Size comp: %d, %d\n",
                osid->productname, osid->securitysym, osid->expiryear, osid->expirmon,
                osid->expirday, osid->optiontype, strike/1000,
                osid->instrumentid, sizeof(ottosid), mess_len);*/
            }
            break;
        case 'd':
            {
                ottocmplxresp* ocr = (ottocmplxresp*)message;
                send_cmplx_order(otp, ocr);
                log_complex_resp(otp, ocr);
                break;
            }
        case 's':
            {
                ottocid* ocid = (ottocid*)message;
                log_complex_inst(otp, ocid);
            }
            break;
        case 'j':
            {
                ottorej* rej = (ottorej*)message;
                ouch_find_token_len(rej->clordid, clordid_len);
                if(get_order_details(otp->ci, rej->clordid, clordid_len, &obj)) {
                    doj = (dart_order_obj*)obj->raw;
                    if(doj && lock_n_check(doj)) {
                        int ret = parse_rej(obj, rej);
                        if (ret == 3) {
                            unlock((dart_order_obj *) doj->rep);
                            unlock(doj);
                        } else {
                            unlock(doj);
                        }
                        char* status = doj->positions[ROM_STATUS].iov_base;
                        if(status[0] == '8') {
                            remove_order_details(otp->ci, rej->clordid, clordid_len);
                        }
                        if(doj->sender != 0x0) {
                            con_int* ct = (con_int*) doj->sender;
                            ct->on_rej(ct, obj);
                            if(doj->num_childs > 0) {
                                send_back_spread_legs(ct, doj, ct->on_rej, obj);
                            }
                        } else {
                            con_int* sender = 0;
                            get_obj_by_client_id(otp->ci->gk,
                                    obj->owner,obj->olen, (void**)&sender);        
                            if(sender && sender->on_rej) {
                                sender->on_rej(sender, obj);
                                if(doj->num_childs > 0) {
                                    send_back_spread_legs(sender, doj, sender->on_rej, obj);
                                }
                            }
                        }
                    }
                }
            }
            break;
    };
}
void parse_otto(int *cut_con, rex_con *pc)
{
    ottop* otp = (ottop*)pc->parser;
    char* d_off = pc->con->rd_ptr;
    int64_t len = (pc->con->wr_ptr - pc->con->rd_ptr);
    uint64_t bytes_used = 0;
    uint16_t mess_len = 0;
    do {
        memcpy(&mess_len, d_off, 2);
        mess_len = ntohs(mess_len);
        if(bytes_used + mess_len + 2 > len || mess_len == 0) {
            break;
        } 
        d_off += 2;
        switch(d_off[0]) {
            case 'S':
                parse_seq_packet(otp, d_off + 1, mess_len -1 );
                record_inbound_message(otp->logger, 1, otp->ci->mod_name, otp->ci->mod_name_len);
                break;
            case '+':
                send_debug_message(otp->logger, "DEBUG message: %s \n", d_off);
                break;
            case 'A':
                just_send_hb(otp);
                start_otto_hb_timer(otp);
                otp->ci->can_send_orders = 1;
                otp->is_logged_on = 1;
                pc->connection_notice(otp, 1, NULL);
                break;
            case 'J':
                if(d_off[1] == 'A') {
                    send_debug_message(otp->logger, 
                            "Logon Rejected: %c = Not Authorized, invalid username and/or password \n", d_off[1]);
                } else {
                    send_debug_message(otp->logger,
                            "Logon Rejected: %c = Session not available. \n", d_off[1]);
                }
                break;
            case 'H':
                break;
            case 'Z':
                *cut_con = 1;
                send_debug_message(otp->logger, 
                        "End of session signaled by otto.\n");
                break;
        };
        bytes_used += (mess_len + 2);
        d_off += mess_len;
    } while(bytes_used < len);
    pc->con->rd_ptr += bytes_used;
}

