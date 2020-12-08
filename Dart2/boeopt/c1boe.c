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
#include "rex_seq_request.h"
#include "rex_allocator.h"
#include "risk_cntr_structs.h"
#include "config_loader.h"
#include "ex_config_helper.h"
#include "dart_handler.h"
#include "connection_interface.h"
#include "connection_factory.h"
#include "mailer.h"
#include "log_mnger.h"
#include "boe_seq.h"
#include "rom_to_boe.h"
#include "service_bureau.h"
#include "rom_fix_trans_funcs.h"
#include "boe_utils.h"
#include "boe_master.h"

#define MICRO_HOUR 3600000000
#define MICRO_MIN 60000000
#define MICRO_SEC 1000000

static void parse_seq_packet(boem *pc,
        char* message, size_t mess_len, char mtype, 
        short mlen, long bu);


boem* create_boe_parser(otl* logger, con_int* pbp, login_info* u)
{
    int ret = 0;
    boem* fp = 0;
    REX_ALLOC(fp, 8, sizeof(struct boe_master), ret);
    fp->o = logger;
    fp->ci = pbp;
    fp->logon = u;
    fp->gk = pbp->gk;
    fp->tg = generic_creation_func();
    fp->mess_to_send = databuf_alloc(1024);
    pthread_spin_init(&fp->send_mutex, PTHREAD_PROCESS_PRIVATE);
    fp->is_logged_on = 0;
    pbp->cparser = fp;
    int ret_len = 0;
    char* temp = 0;
    temp = get_config_val(pbp->ex, "SERVER", 6,
            "Username", 8, &ret_len);
    fp->ulen = ret_len;
    fp->username = malloc(ret_len);
    memcpy(fp->username, temp, ret_len);
    temp= get_config_val(pbp->ex, "SERVER", 6,
            "Password", 8, &ret_len);
    fp->plen = ret_len;
    fp->password = malloc(ret_len);
    memcpy(fp->password, temp, ret_len);
    temp= get_config_val(pbp->ex, "SERVER", 6,
            "Sender Comp ID", 14, &ret_len);
    fp->sslen = ret_len;
    fp->sendsub = malloc(ret_len);
    memcpy(fp->sendsub, temp, ret_len);
    temp = get_config_val(pbp->ex, "FIX SERVER", 10,
            "idtomnemonic", 9, &ret_len);
    char* filename = get_val_for_tag(pbp->ex->dc, "SERVER", 6, "PCODE LOC", 9,
            &ret);
    if(ret > 0) {
        fp->prod_codes = create_service_bureau_matcher(filename);
    } else {
        fp->prod_codes = create_service_bureau_matcher("CFE_Dump.map");
    }
    fp->rom_to_cfe_simp = create_service_bureau_matcher("rom_to_cfe.map");
    if(ret_len <=0 ) {
        pbp->sbm = create_service_bureau_matcher("IDtomnemonic.map");
    } else {
        pbp->sbm = create_service_bureau_matcher(temp);
    }
    temp = get_config_val(pbp->ex, "FIX SERVER", 10,
            "nontph", 6, &ret_len);
    if(ret_len <= 0) {
        temp = "nontphaccounts.map";
    }
    fp->sbm = create_service_bureau_matcher(temp);
    time_t now;
    time(&now);
    u->create_new = ll_before_reset_time(u->reset_day, u->reset_hour,
            u->reset_min, u->last_logon, now);
    fp->bseq = create_seq_master(fp->o, pbp->ex->dest_id, pbp->ex->dest_id_len, u->create_new);
    fp->parse_seq_packet = parse_seq_packet;
    return fp;
}

static void create_boe_logon(databuf_t* buff,
        boem* con)
{
    memset(buff->buffer, 0, 256);
    struct boe_logon* ord  =  (struct boe_logon*)buff->buffer;
    buff->wr_ptr += sizeof(struct boe_logon);
    ord->som = 0xBABA;
    ord->type = 0x37;
    memcpy(ord->sender_sub_id, con->sendsub, con->sslen);
    memcpy(ord->username, con->username, con->ulen);
    memcpy(ord->password, con->password, con->plen);
    ord->num_params += add_unit_seq_param_group(buff, con->bseq); 
    unsigned char om1 = 0x0;
    unsigned char om2 = 0x0;
    unsigned char om3 = 0x0;
    unsigned char om4 = 0x0;
    unsigned char om5 = 0x0;
    unsigned char om6 = 0x0;
    unsigned char om7 = 0x0;
    unsigned char om8 = 0x0;
    unsigned char om9 = 0x0;
    unsigned char om10 = 0x0;
    unsigned char om11 = 0x0;
    unsigned char om12 = 0x0;
    unsigned char om13 = 0x0;
    unsigned char om14 = 0x0;
    unsigned char param_group_type = 0x81;
    /*Execution start*/
    unsigned char type = 0x2C;
    unsigned char bits = 13;
    unsigned short param_len = 18;
    databuf_memcpy(buff, &param_len, 2);
    databuf_memcpy(buff, &param_group_type, 1);
    databuf_memcpy(buff, &type, 1);
    databuf_memcpy(buff, &bits, 1);
    SET_BIT(0x1, 0x1, om1);
    databuf_memcpy(buff, &om1, 1);
    SET_BIT(0x1, 0x1, om2);
    databuf_memcpy(buff, &om2, 1);
    databuf_memcpy(buff, &om3, 1);
    SET_BIT(0x1, 0x1, om4);
    SET_BIT(0x1, 0x2, om4);
    SET_BIT(0x1, 0x4, om4);
    databuf_memcpy(buff, &om4, 1);
    databuf_memcpy(buff, &om5, 1);
    databuf_memcpy(buff, &om6, 1);
    databuf_memcpy(buff, &om7, 1);
    SET_BIT(0x1, 0x1, om8);
    databuf_memcpy(buff, &om8, 1);
    databuf_memcpy(buff, &om9, 1);
    databuf_memcpy(buff, &om10, 1);
    databuf_memcpy(buff, &om11, 1);
    databuf_memcpy(buff, &om12, 1);
    SET_BIT(0x1, 0x80, om13);
    databuf_memcpy(buff, &om13, 1);
    ord->num_params += 1;
    /*End Execution*/
    /*Order Restated*/
    type = 0x28;
    bits = 0x5;
    param_len = 10;
    databuf_memcpy(buff, &param_len, 2);
    databuf_memcpy(buff, &param_group_type, 1);
    databuf_memcpy(buff, &type, 1);
    databuf_memcpy(buff, &bits, 1);
    om1 = 0x0;
    databuf_memcpy(buff, &om1, 1);
    om2 = 0x0;
    databuf_memcpy(buff, &om2, 1);
    om3 = 0x0;
    SET_BIT(0x1, 0x40, om3);
    databuf_memcpy(buff, &om3, 1);
    om4 = 0x0;
    databuf_memcpy(buff, &om4, 1);
    om5 = 0x0;
    SET_BIT(0x1, 0x2, om5);
    databuf_memcpy(buff, &om5, 1);
    ord->num_params += 1;
    /*Order restated End*/
    /*Complex Instrument Accepted */
    type = 0x4d;
    bits = 14;
    param_len = 19;
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
    databuf_memcpy(buff, &om8, 1);
    om9 = 0x0;
    databuf_memcpy(buff, &om9, 1);
    om10 = 0x0;
    databuf_memcpy(buff, &om10, 1);
    om11 = 0x0;
    databuf_memcpy(buff, &om11, 1);
    om12 = 0x0;
    databuf_memcpy(buff, &om12, 1);
    om13 = 0x0;
    databuf_memcpy(buff, &om13, 1);
    SET_BIT(0x1, 0x1, om14);
    SET_BIT(0x1, 0x2, om14);
    SET_BIT(0x1, 0x4, om14);
    databuf_memcpy(buff, &om14, 1);
    ord->num_params += 1;
    /*Complex Instrument Accepted End*/
    ord->len = (databuf_unread(buff) - 2);
}
void send_boe_logon(void* c)
{
    boem* bm = (boem*)c;
    databuf_t* buff = get_databuf(bm->gk);
    create_boe_logon(buff, bm);
    boe_direct_send(bm, buff->rd_ptr, databuf_unread(buff));
    destroy_gk_db(bm->gk, buff);
}


static void remove_order_details(con_int* ci, char* clordid, int clordid_len)
{
    pthread_spin_lock(&ci->mutex);
    remove_from_map(ci->cl_ord_id_map, clordid, clordid_len);
    pthread_spin_unlock(&ci->mutex);
}


void build_fill_not_found_mail(con_int * ob, char *d_off,
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
void build_bust_mail(con_int* ob, char* d_off, int len)
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


static int get_order_details(con_int* ci, char* clordid, int clordid_len, rcntr** cnt)
{
    pthread_spin_lock(&ci->mutex);
    /*START LOCK*/
    int ret = get_map_entry(ci->cl_ord_id_map, clordid, clordid_len, (void**)cnt);
    /*END LOCK*/
    pthread_spin_unlock(&ci->mutex);

    return ret;
}
int on_boe_enter_order(con_int * ci, rcntr* cnt)
{
    int ret = 1;
    if(cnt->rlen == sizeof(dart_order_obj)) {
        boem* bm = (boem*)ci->cparser;
        int total_len = 0;
        databuf_t *buff = (databuf_t *) get_databuf(ci->gk);
        dart_order_obj* doj = (dart_order_obj*)cnt->raw;
        if(is_spread(doj)) {
            if(doj->num_childs <= 0 && doj->positions[ROM_SEC_DEF].iov_len > 0 && 
                    doj->positions[ROM_O_C].iov_len > 1) {
                total_len = create_ex_boe_cmplx_ord(buff, cnt, ci, bm->tg, ++bm->seq);
            } else if(doj->num_childs > 0) {
                total_len = create_boe_cmplx_instrument(buff, cnt, ci,bm->tg, ++bm->seq);
            } else {
                set_rom_field(doj, ROM_TEXT, "Invalid Spread", 14);
            }
        } else {
            total_len = create_boe_new_order_message(buff, cnt,ci,bm->tg, ++bm->seq);
        }
        if (total_len > 0) {
            boe_direct_send(bm, buff->rd_ptr, databuf_unread(buff));
            record_raw_message(bm->o, ci->mod_name, ci->mod_name_len, buff->rd_ptr,
                    databuf_unread(buff), 0x0);
        } else if(total_len <= 0) {
            ret = -1;
        }
        destroy_gk_db(ci->gk, buff);
    }
    return ret;
}
int on_boe_replace_order(con_int * ci, rcntr* cnt)
{
    int ret = 1;
    if(cnt->rlen == sizeof(dart_order_obj)) {
        boem* bm = (boem*)ci->cparser;
        int total_len = 0;
        databuf_t *buff = (databuf_t *) get_databuf(ci->gk);
        total_len = create_boe_replace_message(buff, cnt, ci, bm->tg, ++bm->seq);
        if (total_len > 0) {
            boe_direct_send(bm,buff->rd_ptr,databuf_unread(buff));
            record_raw_message(bm->o, ci->mod_name, ci->mod_name_len, buff->rd_ptr,
                    databuf_unread(buff), 0x0);
        } else {
            ret = -1;
        }
        destroy_gk_db(ci->gk, buff);
    }
    return ret;
}
int on_boe_cancel_order(con_int * ci, rcntr * cnt)
{ 
    int ret = 1;
    if(cnt->rlen == sizeof(dart_order_obj)) {
        boem* bm = (boem*)ci->cparser;
        int total_len = 0;
        databuf_t *buff = (databuf_t *) get_databuf(ci->gk);
        total_len = create_boe_cancel_message(buff, cnt, ci, ++bm->seq);
        if (total_len > 0) {
            boe_direct_send(bm,buff->rd_ptr, databuf_unread(buff));
            record_raw_message(bm->o, ci->mod_name, ci->mod_name_len, buff->rd_ptr,
                    databuf_unread(buff), 0x0);
        } else {
            ret = -1;
        }
        destroy_gk_db(ci->gk, buff);
    }
    return ret;
}

static void c1_on_connect(void* pass_back, int is_connected,
        dart_order_obj* log)
{
    boem* uparse = (boem*)pass_back;
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
            ci->can_send_orders = 1;
            ci->connected = 1;
            break;
        case 2:
            ci->can_send_orders = 0;
            ci->connected = 1;
            break;
        case -1:
            send_boe_logout(uparse);
            is_connected = 0;
            ci->can_send_orders = 0;
            ci->connected = 0;
            /*Add logout request message*/
            break;
    }
    ci->update_oats(ci, is_connected);
}
void boe_write_callback(void* cb)
{
    rex_con* rc = (rex_con*)cb;
    boem* up = (boem*)rc->parser;
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
static void boe_cancel_all_orders(con_int* ci)
{
}
void boe_cleansing_rinse(con_int* ci)
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
    ci->on_enter_order = on_boe_enter_order;
    ci->on_replace_order = on_boe_replace_order;
    ci->on_cancel_order = on_boe_cancel_order;
    ci->cleansing_rinse = boe_cleansing_rinse;
    ci->on_cancel_all = boe_cancel_all_orders;
    ci->can_send_orders = 0;
    login_info* li = build_logon_info_from_config(ci->ex);
    boem* otp = create_boe_parser(ci->o, ci, li);
    rex_con* rc = create_rex_con(ci->p, ci->ex, otp, otp,li,
            boe_write_callback, 
            c1_on_connect,
            parse_boe, NULL);
    rc->parser = otp;
    rc->connect = send_boe_logon;
    otp->rc = rc;
    ci->rc = rc;
    rex_connect(ci->p, rc);
}

static con_int* get_sender(boem* bm, dart_order_obj* doj, 
        rcntr* obj)
{
    con_int* sender = 0x0;
    if(doj->sender != 0x0) {
        sender = (con_int*) doj->sender;
    } else {
        send_debug_message(bm->o, "We are looking up this owner:%s, len: %d, %lu\n",
                obj->owner, obj->olen, obj);
        get_obj_by_client_id(bm->ci->gk,
                obj->owner,obj->olen, (void**)&sender);        
    }
    return sender;
}

static void log_for_sender(con_int* us, con_int* sender, rcntr* obj)
{ 
    dart_order_obj* doj = (dart_order_obj*)obj->raw;
    record_dart_message(us->o, doj);
}
static void send_back_spread_legs(con_int* ci, dart_order_obj* daddy,
        void(*back)(con_int* lci, rcntr* risk), rcntr* cnt, char* status, int slen)
{

    children* lch = daddy->childs;
    if(lch) {
        do {
            dart_order_obj* ldoj = (dart_order_obj*)lch->payload;
            cnt->raw = ldoj;
            set_rom_field(ldoj, 0, ROM_ZERO_STATUS, 1);
            set_rom_field(ldoj, ROM_STATUS, status, slen);
            back(ci, cnt);
            lch = lch->next;
        } while(lch);
        cnt->raw = daddy;
    }
}
static void boe_parse_cmplx_rej(rcntr * cnt, char *ack, con_int* ob,
        char* clordid, int clordid_len, boem* bm)
{
    dart_order_obj* ute = (dart_order_obj*)cnt->raw;
    databuf_t *itoa_buff = get_databuf(ob->gk);
    set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    set_rom_field(ute, ROM_STATUS, "8", 1);
    char* text = ack + 39;
    int tlen = 0;
    FIND_NULL(text, tlen, 60);
    clean_text(ute, itoa_buff, 
            ROM_TEXT, text, tlen);
    children* lch = ute->childs;
    do {
        dart_order_obj* ldoj = (dart_order_obj*)lch->payload;
        set_rom_field(ldoj, 0, ROM_ZERO_STATUS, 1);
        set_rom_field(ldoj, ROM_STATUS, "8", 1);
        lch = lch->next;
    }while(lch);
    destroy_gk_db(ob->gk, itoa_buff);
    con_int* sender = get_sender(bm,ute,cnt);
    if(sender && sender->on_rej) {
        send_back_spread_legs(sender, ute, sender->on_rej, cnt, "8", 1);
        ute->childs = 0x0;
        sender->on_rej(sender, cnt);
    }else if(sender) {
        log_for_sender(ob,sender, cnt); 
    }
    remove_order_details(ob, clordid, clordid_len);
}

static void parse_seq_packet(boem *pc,
        char* message, size_t mess_len, char mtype, 
        short mlen, long bu)
{
    con_int* ci = pc->ci;
    record_raw_message(pc->o, ci->mod_name, ci->mod_name_len,
            (char*)message, mess_len, 0x0);
    uint8_t munit = 0x0;
    uint32_t seq = 0x0;
    memcpy(&munit, message + 5, 1);
    memcpy(&seq, message + 6, 4);
    add_seq(pc->bseq, munit, seq);
    char* clordid = (char*)message + 18;
    int clordid_len = 0;
    {
        FIND_NULL(clordid, clordid_len, 20);
    }
    int ord_done = 0;
    rcntr* obj = 0;
    con_int* sender = 0;
    if(get_order_details(pc->ci, clordid, clordid_len, &obj)) {
        if(obj->raw != 0x0) {
            dart_order_obj* doj = (dart_order_obj*)obj->raw;
            switch(mtype) {
                case 0x25:
                    ord_done = boe_parse_order_ack(obj, message, ci);
                    sender = get_sender(pc,doj,obj);
                    if(sender && sender->on_update) {
                        sender->on_update(sender, obj);
                        if(doj->num_childs > 0) {
                            send_back_spread_legs(sender, doj, sender->on_update, obj, "13", 2);
                        }
                    } else if(sender) {
                        log_for_sender(ci, sender, obj); 
                    }

                    break;
                case 0x28://Restated
                    ord_done = boe_parse_order_restated(obj, message, ci);
                    if(ord_done) {
                        remove_order_details(pc->ci, clordid, clordid_len);
                        if(sender && sender->on_out) {
                            if(doj->num_childs > 0) {
                                send_back_spread_legs(sender, doj, sender->on_out, obj, "4", 1);
                                doj->childs = 0x0;
                            }
                            sender->on_out(sender, obj);
                        } else if(sender) {
                            log_for_sender(ci, sender, obj); 
                        } else {
                            send_debug_message(ci->o, "No sender for order:%s, rcntr: %lu\n",
                                    clordid, obj);
                        }
                    }
                    break;
                case 0x26: //rejected
                    ord_done = boe_parse_rej(obj, message, ci);
                    sender = get_sender(pc,doj,obj);
                    remove_order_details(pc->ci, clordid, clordid_len);
                    if(sender && sender->on_rej) {
                        if(doj->num_childs > 0) {
                            send_back_spread_legs(sender, doj, sender->on_rej, obj, "8", 1);
                            doj->childs = 0x0;
                        }
                        sender->on_rej(sender, obj);
                    }else if(sender) {
                        log_for_sender(ci,sender, obj); 
                    }

                    break;
                case 0x27: //modified
                    ord_done = boe_parse_replaced(obj, message, ci);
                    sender = get_sender(pc,doj,obj);
                    if(sender && sender->on_replaced) {
                        sender->on_replaced(sender, obj);
                        if(doj->num_childs > 0) {
                            send_back_spread_legs(sender, doj, sender->on_replaced, obj, "5", 1);
                        }
                    }
                    break;
                case 0x29: //replace rejected
                    ord_done = boe_parse_rep_rej(obj, message, ci);
                    sender = get_sender(pc,doj,obj);
                    if(sender && sender->on_rej) {
                        sender->on_rej(sender, obj);
                    }
                    break;
                case 0x2A:  //cancelled
                    ord_done = boe_parse_ur_out(obj, message, ci);
                    sender = get_sender(pc,doj,obj);
                    if(ord_done) {
                        remove_order_details(pc->ci, clordid, clordid_len);
                    }
                    if(sender && sender->on_out) {
                        if(doj->num_childs > 0) {
                            send_back_spread_legs(sender, doj, sender->on_out, obj, "4", 1);
                            doj->childs = 0x0;
                        }
                        sender->on_out(sender, obj);
                    } else if(sender) {
                        log_for_sender(ci, sender, obj); 
                    } else {
                        send_debug_message(ci->o, "No sender for order:%s, rcntr: %lu\n",
                                clordid, obj);
                    }

                    break;
                case 0x2B: //cancel rejected
                    ord_done = boe_parse_cancel_rej(obj, message, ci);
                    sender = get_sender(pc,doj,obj);
                    if(sender && sender->on_rej) {
                        sender->on_rej(sender, obj);
                    }
                    break;
                case 0x2C: //execution
                    ord_done = boe_parse_filled(obj, message, ci);
                    sender = get_sender(pc,doj,obj);
                    if (ord_done == -1) {
                        send_debug_message(ci->o, "We are removing this order:%s, cplx len: %d\n",
                                clordid, doj->positions[ROM_CPLX_TYPE].iov_len);
                        remove_order_details(pc->ci, clordid, clordid_len);
                    } 
                    if(doj->num_childs > 0 && ord_done < 13) {
                        int i = 0;
                        children* lch = doj->childs;
                        do {
                            dart_order_obj* ldoj = (dart_order_obj*)lch->payload;
                            obj->raw = ldoj;
                            set_rom_field(ldoj, ROM_CPLX_TYPE, "6", 1);
                            if(i == ord_done && sender && sender->on_fill) {
                                sender->on_fill(sender, obj);
                                break;
                            }
                            ++i;
                            lch = lch->next;
                        } while(lch);
                        obj->raw = doj;
                    } else {
                        if(ord_done == -2 ) {
                            send_debug_message(ci->o, "Got -2 removing this order:%s, cplx len: %d\n",
                                    clordid, doj->positions[ROM_CPLX_TYPE].iov_len);
                            remove_order_details(pc->ci, clordid, clordid_len);
                            if(sender && sender->on_fill) {
                                sender->on_fill(sender, obj);
                                set_rom_field(doj, ROM_STATUS, "4", 1);
                                set_rom_field(doj, ROM_LEAVES, "0", 1);
                                sender->on_out(sender, obj);
                            } else if(sender) {
                                log_for_sender(ci,sender, obj); 
                            }
                        } else {
                            if(sender && sender->on_fill) {
                                sender->on_fill(sender, obj);
                            } else if(sender) {
                                log_for_sender(ci,sender, obj); 
                            }
                        }
                    }
                    break;
                case 0x2D: //Trade cancel or correct
                    build_bust_mail(ci, message, mess_len);
                    break;
                case 0x4D:
                    {
                        databuf_t *buff = (databuf_t *) get_databuf(ci->gk);
                        boem* bm = (boem*)ci->cparser;
                        int total_len = create_boe_cmplx_order(buff, obj,
                                ci, bm->tg, bm->seq++, message);
                        if (total_len > 0) {
                            boe_direct_send(bm, buff->rd_ptr, databuf_unread(buff));
                            record_raw_message(bm->o, ci->mod_name, ci->mod_name_len, buff->rd_ptr,
                                    databuf_unread(buff), 0x0);
                        } else if(total_len <= 0) {
                            send_debug_message(ci->o, "Failed to create cplx We are removing this order:%s, cplx len: %d\n",
                                    clordid, doj->positions[ROM_CPLX_TYPE].iov_len);
                            sender = get_sender(pc,doj,obj);
                            if(sender && sender->on_rej) {
                                if(doj->num_childs > 0) { 
                                    send_back_spread_legs(sender, doj, sender->on_rej, obj, "8", 1);
                                    doj->childs = 0x0;
                                }
                                sender->on_rej(sender, obj);
                            }
                        }
                    }
                    break;
                case 0x4E:
                    boe_parse_cmplx_rej(obj, message, ci, clordid, clordid_len, pc);
                    break;
                default:
                    send_debug_message(ci->o,"Matched this order: %s, with unknown type: %c \n",
                            clordid,mtype);
                    break;

            };
        }
    } else {
        char bust = 0x2D;
        if(mtype == bust) {
            build_bust_mail(ci, message, mess_len);
        }
        send_debug_message(ci->o,"Unable to match this order: %s, with type: %c and length: %d, %d: %lu \n",
                clordid,mtype, mess_len, mlen, bu);
        if(mtype == 'I') {
            uint8_t num_bit_fields = *(message + 47);
            uint8_t bit4 = *(message + 52);
            uint8_t bit5 = *(message + 59);
            uint32_t is_present = 0x0;
            IS_BIT_SET(is_present, bit4, 0x3);
            send_debug_message(ci->o, "TAS restate num_bit_fields? %d what about cur bit: %d clear %d, do we have shares: %d\n",
                    num_bit_fields, bit4, bit5, is_present);
        }
    }
}

