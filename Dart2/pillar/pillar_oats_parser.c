/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <arpa/inet.h>

#include "pillar_oats_parser.h"
#include "databuf.h"
#include "connection_interface.h"
#include "dart_handler.h"
#include "rex_constants.h"
#include "pillar.h"
#include "pillar_server.h"

uint8_t get_bitfield_instruction(uint64_t bits, uint64_t start, uint64_t end)
{
    uint64_t mask = ((1UL << (end - start)) - 1UL) << start;
    uint64_t value = mask & bits;
    uint8_t v = value >> start;
    return v;
}
void trans_side(uint8_t iside, dart_order_obj* doj)
{
    switch(iside) {
        case 1:
            set_rom_field(doj, ROM_SIDE, "1", 1);
            break;
        case 2:
            set_rom_field(doj, ROM_SIDE, "2", 1);
            break;
        case 3:
            set_rom_field(doj, ROM_SIDE, "5", 1);
            if(doj->positions[ROM_FIRM].iov_len > 0) {
                set_rom_field(doj, ROM_SHORT_LEND, doj->positions[ROM_FIRM].iov_base,
                        doj->positions[ROM_FIRM].iov_len);
            }
            break;
        default:
            break;
    };
}
void trans_ex_ex(uint8_t exinst, dart_order_obj* doj)
{
    switch(exinst) {
        case 0:
        default:
            break;
        case 1:
            set_rom_field(doj, ROM_EXEC_INST, "A", 1);
            break;
        case 8:
            set_rom_field(doj, ROM_EXEC_INST, "I", 1);
            break;
        case 3:
            set_rom_field(doj, ROM_ECHO, "PO", 2);
            break;
        case 4:
            set_rom_field(doj, ROM_ECHO, "PO+S", 4);
            break;
        case 5:
            set_rom_field(doj, ROM_ECHO, "PMU", 3);
            break;
        case 6:
            set_rom_field(doj, ROM_ECHO, "PMA", 3);
            break;
        case 7:
            set_rom_field(doj, ROM_ECHO, "BPMA", 4);
            break;
    };
}
void trans_exec(uint8_t exinst, dart_order_obj* doj)
{
    switch(exinst) {
        case 0:
        case 1:
        case 2:
        default:
            break;
        case 5:
        case 6:
        case 10:
            set_rom_field(doj, ROM_EXEC_INST, "P", 1);
           break;

    };
}

void trans_type(uint8_t itype, dart_order_obj* doj)
{
    switch(itype) {
        case 1:
            set_rom_field(doj, ROM_TYPE, "1", 1);
            break;
          case 2:
            set_rom_field(doj, ROM_TYPE, "2", 1);
            break;
          case 3:
            set_rom_field(doj, ROM_TYPE, "3", 1);
            break;
          case 4:
            set_rom_field(doj, ROM_TYPE, "90", 2);
            break;
    };
}
static void trans_self_trade_type(uint8_t stt, dart_order_obj* doj)
{
    switch(stt) {
        default :
            break;
        case 1:
            set_rom_field(doj, ROM_DESC, "No STP", 6);
            break;
        case 2:
            set_rom_field(doj, ROM_DESC, "CNL New", 7);
            break;
        case 3:
            set_rom_field(doj, ROM_DESC, "CNL OLD", 7);
            break;
        case 4:
            set_rom_field(doj, ROM_DESC, "CNL ALL", 7);
            break;
        case 5:
            set_rom_field(doj, ROM_DESC, "CNL DEC", 7);
            break;
    };
}
static void trans_tif(uint8_t tif, dart_order_obj* doj)
{
    switch(tif) {
        case 1:
            set_rom_field(doj, ROM_TIF, "0", 1);
            break;
        case 2:
            set_rom_field(doj, ROM_TIF, "3", 1);
            break;
        case 3:
            set_rom_field(doj, ROM_TIF, "2", 1);
            break;
        case 4:
            set_rom_field(doj, ROM_TYPE, "10", 2);
            /*Intentionally no break to give us the "Day" tif */
        default:
            set_rom_field(doj, ROM_TIF, "0", 1);
            break;
    };
}
static void trans_cap(uint8_t cap, dart_order_obj* doj)
{
    switch(cap) {
        case 1:
        default:
            set_rom_field(doj, ROM_CAP, "A", 1);
            break;
        case 3:
        case 2:
            set_rom_field(doj, ROM_CAP, "P", 1);
            break;
        case 4:
            set_rom_field(doj, ROM_CAP, "E", 1);
            break;
    };
}
static void trans_locate(uint8_t locate, dart_order_obj* doj)
{
    if(locate == 1) {
        int clen = getplen(doj, ROM_FIRM);
        if(clen > 0) {
            char* clr_id = getpval(doj, ROM_FIRM);
            set_rom_field(doj,ROM_SHORT_LEND, clr_id, clen );
        }
    }
}

static void pill_set_om_ex_tag(dart_order_obj* doj, con_int* ci, uint64_t id)
{
    int dtaglen = 0;
    char* dom_tag = get_dom_tag(ci->gk, &dtaglen);
    if(dtaglen > 0) {
        databuf_t* db = get_databuf(ci->gk);
        databuf_write(db, "%s%s%lu-%lu", ci->ex->username, dom_tag,
                id, get_dom_num(ci->gk));
        set_rom_field(doj, ROM_EXECUTION_TAG, db->rd_ptr, databuf_unread(db));
        destroy_gk_db(ci->gk, db);   
    }
}

static void pill_set_om_tag(dart_order_obj* doj, con_int* ci, uint64_t id)
{
    int dtaglen = 0;
    char* dom_tag = get_dom_tag(ci->gk, &dtaglen);
    if(dtaglen > 0) {
        databuf_t* db = get_databuf(ci->gk);
        databuf_write(db, "%s%s%lu", ci->ex->username, dom_tag,
                id);
        set_rom_field(doj, ROM_TAG, db->rd_ptr, databuf_unread(db));
        destroy_gk_db(ci->gk, db);   
    }
}

static void parse_da_bit(uint64_t bit, dart_order_obj* doj)
{
    uint8_t side = get_bitfield_instruction(bit,
            60, 64);
    trans_side(side, doj);
    side = get_bitfield_instruction(bit, 56, 60);
    trans_type(side, doj);
    side = get_bitfield_instruction(bit, 52, 56);
    trans_exec(side, doj);
    side = get_bitfield_instruction(bit, 48, 52);
    trans_ex_ex(side, doj);
    side = get_bitfield_instruction(bit, 37, 40);
    trans_self_trade_type(side, doj);
    side = get_bitfield_instruction(bit, 31, 34);
    trans_tif(side, doj);
    side = get_bitfield_instruction(bit, 22, 25);
    trans_cap(side, doj);
    side = get_bitfield_instruction(bit, 17, 18);
    trans_locate(side, doj);
}
/**
 * Getting the side 
    uint8_t side = get_bitfield_instruction(new_ord->bitfield_instr,
            60, 64);
 *
 */
static void parse_pill_cnl(pillcancel* pc, con_int* ci,
        dart_order_obj* doj) 
{
    set_rom_field(doj, 0, "S", 1);
    databuf_t* db = get_databuf(ci->gk);
    u64_dart_itoa(doj, pc->clordid, ROM_PENDING_ID,db);
    databuf_reset(db);
    u64_dart_itoa(doj, pc->orig_ord_key, ROM_CLORDID,db);
    databuf_reset(db);
    u64_dart_itoa(doj, pc->orig_ord_key, ROM_CLIENT_ID,db);
    databuf_reset(db);
    set_rom_field(doj, ROM_STATUS, "26", 2);
    pill_set_om_tag(doj, ci, pc->orig_ord_key);
    pill_set_om_ex_tag(doj, ci, pc->orig_ord_key);
    destroy_gk_db(ci->gk, db);   
}

static void get_rom_symbol(con_int* ci, dart_order_obj* doj,
        uint32_t symbol)
{
    char* ret;
    hashmap_t symbols = (hashmap_t)ci->ph;
    int found = get_map_entry(symbols, (const char*)&symbol, 4, (void**)&ret);
    if(found) {
        set_rom_field(doj, ROM_SYM, ret, found);
    }
}
static int find_udata_len(char *start, int max_len)
{
    char *end = start + (max_len - 1);
    while (end[0] == 0 && (end -start) >= 0) {
        --end;
    }
    if(end - start == 0) {
        return 0;
    } else {
        return (end - start) + 1;
    }
}
/**
 *
    uint32_t symbol;
    char mpid[4];
    uint32_t mmid;
    char mpsubid;
    uint64_t clordid;
    uint64_t orig_ord_key;
    uint64_t bitfield_instr;
    uint64_t price;
    uint32_t qty;
    uint32_t min_qty;
    char user_data[8];
    uint32_t optional;
 */
static void parse_pill_new_ord_rep(pillneword* pnw, con_int* ci,
        dart_order_obj* doj,uint64_t timestamp)
{
    databuf_t* db = get_databuf(ci->gk);
    set_rom_field(doj, 0, "E", 1);
    set_rom_field(doj, ROM_STATUS, "0", 1);
    u64_dart_itoa(doj, pnw->qty, ROM_SHARES,db);
    databuf_reset(db);
    u64_dart_itoa(doj, pnw->min_qty, ROM_MIN_QTY,db);
    databuf_reset(db);
    u64_dart_itoa(doj, pnw->clordid, ROM_CLORDID,db);
    databuf_reset(db);
    u64_dart_itoa(doj, pnw->clordid, ROM_CLIENT_ID,db);
    databuf_reset(db);
    pillar_to_rom_price(pnw->price, db);
    set_rom_field(doj, ROM_PRICE, db->rd_ptr, databuf_unread(db));
    databuf_reset(db);
    parse_da_bit(pnw->bitfield_instr, doj);
    int len = pillar_to_rom_time(timestamp, db);
    set_rom_field(doj, EXEC_TIME, db->rd_ptr, len);
    get_rom_symbol(ci, doj, pnw->symbol);
    int cplen = find_udata_len(pnw->user_data, 8);
    set_rom_field(doj, ROM_CLR_ACC, pnw->user_data, cplen); 
    pill_set_om_tag(doj, ci, pnw->clordid);
    pill_set_om_ex_tag(doj, ci, pnw->clordid);
    destroy_gk_db(ci->gk, db);   
}
static void op_parse_pill_cnl_rej(pillcnlreprej* pillrej, con_int* ci,
        dart_order_obj* doj)
{
    databuf_t* db = get_databuf(ci->gk);
    set_rom_field(doj, 0, "S", 1);
    int len = pillar_to_rom_time(pillrej->timestamp, db); 
    set_rom_field(doj, EXEC_TIME, db->rd_ptr, len);
    u64_dart_itoa(doj, pillrej->clordid, ROM_CLORDID,db);
    databuf_reset(db);
    u64_dart_itoa(doj, pillrej->clordid, ROM_CLIENT_ID,db);
    databuf_reset(db);
    get_rom_symbol(ci, doj, pillrej->symbol);
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
                    }
                } else {
                    set_rom_field(doj, ROM_STATUS, "8", 1);
                }
            break;
            }
        default:
            send_debug_message(ci->o, "Found Reject Type: %u, reason: %u, ID: %lu \n",
                    pillrej->rejtype,pillrej->reason, pillrej->clordid); 
            break;
    };
    if(pillrej->reason == 9999) {
        databuf_write(db, "RoPro Failed Limits or price check");
    } else {
        databuf_write(db, "Pillar Reject: %u", pillrej->reason);
    }
    set_rom_field(doj, ROM_TEXT, db->rd_ptr, databuf_unread(db));
    pill_set_om_tag(doj, ci, pillrej->clordid);
    pill_set_om_ex_tag(doj, ci, pillrej->clordid);
    destroy_gk_db(ci->gk, db);   
}
/**
 *
    uint64_t order_id;
    uint64_t refclordid;
    uint64_t orig_clordid;
 *
 */
static void op_parse_pill_out(pillmodcnlack* pmca, con_int* ci,
        dart_order_obj* doj)
{
    databuf_t* db = get_databuf(ci->gk);
    set_rom_field(doj, 0, "S", 1);
    int len = pillar_to_rom_time(pmca->timestamp, db); 
    set_rom_field(doj, EXEC_TIME, db->rd_ptr, len);
    get_rom_symbol(ci, doj, pmca->symbol);
    u64_dart_itoa(doj, pmca->qty, ROM_SHARES,db);
    databuf_reset(db);
    u64_dart_itoa(doj, pmca->refclordid, ROM_PENDING_ID,db);
    databuf_reset(db);
    u64_dart_itoa(doj, pmca->orig_clordid, ROM_CLORDID,db);
    databuf_reset(db);
    u64_dart_itoa(doj, pmca->orig_clordid, ROM_CLIENT_ID,db);
    databuf_reset(db);
    u64_dart_itoa(doj, pmca->leaves, ROM_LEAVES,db);
    databuf_reset(db);
    u64_dart_itoa(doj, pmca->order_id, ROM_EX_TAG,db);
    databuf_reset(db);
    pillar_to_rom_price(pmca->price, db);
    set_rom_field(doj, ROM_PRICE, db->rd_ptr, databuf_unread(db));
    databuf_reset(db);
    int cplen = find_udata_len(pmca->user_data, 8);
    set_rom_field(doj, ROM_CLR_ACC, pmca->user_data, cplen); 
    switch(pmca->acktype) {
        case 5: 
        case 4: 
            {
                set_rom_field(doj, ROM_STATUS, "6",1);
            }
            break;
        case 11:
        case 12:
            set_rom_field(doj, ROM_STATUS, "4",1);
            break;
        case 13:
        case 14:
        case 15:
        case 16:
            {
                if(pmca->leaves > 0) {
                    set_rom_field(doj, ROM_STATUS, "6",1);
                } else {
                    set_rom_field(doj, ROM_STATUS, "4",1);
                }
            }
            break;
        case 7:
            set_rom_field(doj, ROM_STATUS, "21",2);
            break;
        case 8:
        case 9:
            set_rom_field(doj, ROM_STATUS, "5",1);
           break; 
        default:
            send_debug_message(ci->o,
                    "Status: %u ID:clordid %lu qty: %ld price %lu leaves: %ld\n",
                    pmca->acktype, pmca->orig_clordid, pmca->qty,
                    pmca->price, pmca->leaves);
            break;
    };
    pill_set_om_tag(doj, ci, pmca->orig_clordid);
    pill_set_om_ex_tag(doj, ci, pmca->orig_clordid);
    destroy_gk_db(ci->gk, db);   
}
/**
 *
    uint64_t timestamp;
    uint32_t symbol;
    char mpid[4];
    uint64_t order_id;
    uint64_t  clordid;
    uint64_t dealid;
    uint64_t last_price;
    uint32_t leaves;
    uint32_t cum;
    uint32_t lastqty;
    char liq[4];
    char displiq[4];
    uint8_t locate;
    uint8_t participanttype;
    uint16_t reason;
    char user_data[8];
 *
 */
static void op_parse_pill_exe(pillexerpt* pillexe, con_int* ci,
        dart_order_obj* doj)
{
    databuf_t* db = get_databuf(ci->gk);
    set_rom_field(doj, 0, "S", 1);
    if(pillexe->leaves == 0) {
        set_rom_field(doj, ROM_STATUS, "2", 1);
    } else {
        set_rom_field(doj, ROM_STATUS, "1", 1);
    }
    int len = pillar_to_rom_time(pillexe->timestamp, db); 
    set_rom_field(doj, EXEC_TIME, db->rd_ptr, len);
    u64_dart_itoa(doj, pillexe->lastqty, ROM_LAST_SHARES,db);
    databuf_reset(db);
    u64_dart_itoa(doj, pillexe->leaves, ROM_LEAVES,db);
    databuf_reset(db);
    u64_dart_itoa(doj, pillexe->cum, ROM_CUM,db);
    databuf_reset(db);
    u64_dart_itoa(doj, pillexe->clordid, ROM_CLORDID,db);
    databuf_reset(db);
    u64_dart_itoa(doj, pillexe->clordid, ROM_CLIENT_ID,db);
    databuf_reset(db);
    u64_dart_itoa(doj, pillexe->order_id, ROM_EX_TAG,db);
    databuf_reset(db);
    u64_dart_itoa(doj, pillexe->dealid, EXEC_ID,db);
    databuf_reset(db);
    pillar_to_rom_price(pillexe->last_price, db);
    set_rom_field(doj, ROM_EXEC_PRICE, db->rd_ptr, databuf_unread(db));
    databuf_reset(db);
    int cplen = find_udata_len(pillexe->user_data, 8);
    set_rom_field(doj, ROM_CLR_ACC, pillexe->user_data, cplen); 
    get_rom_symbol(ci, doj, pillexe->symbol);
    len = find_udata_len(pillexe->liq, 4);
    set_rom_field(doj, ROM_LIQ, pillexe->liq, len);
    pill_set_om_tag(doj, ci, pillexe->clordid);
    pill_set_om_ex_tag(doj, ci, pillexe->clordid);
    destroy_gk_db(ci->gk, db);   
}
/**
 *
    uint32_t leaves;
    uint64_t working_price;
    uint8_t wadisplay;
    char pre_liq_ind[4];
    uint16_t reason;
    uint8_t acktype;
    uint8_t bitfieldflow;
    char user_data[8];
 */
static void op_parse_ack(pillordack* poack, con_int* ci, 
        dart_order_obj* doj)
{
    databuf_t* db = get_databuf(ci->gk);
    set_rom_field(doj, 0, "S", 1);
    set_rom_field(doj, ROM_STATUS, "13", 2);
    int len = pillar_to_rom_time(poack->timestamp, db); 
    set_rom_field(doj, EXEC_TIME, db->rd_ptr, len);
    parse_da_bit(poack->bitfield_instr, doj);
    get_rom_symbol(ci, doj, poack->symbol);
    u64_dart_itoa(doj, poack->qty, ROM_SHARES,db);
    databuf_reset(db);
    u64_dart_itoa(doj, poack->min_qty, ROM_MIN_QTY,db);
    databuf_reset(db);
    u64_dart_itoa(doj, poack->clordid, ROM_CLORDID,db);
    databuf_reset(db);
    u64_dart_itoa(doj, poack->clordid, ROM_CLIENT_ID,db);
    databuf_reset(db);
    u64_dart_itoa(doj, poack->leaves, ROM_LEAVES,db);
    databuf_reset(db);
    u64_dart_itoa(doj, poack->order_id, ROM_EX_TAG,db);
    databuf_reset(db);
    pillar_to_rom_price(poack->price, db);
    set_rom_field(doj, ROM_PRICE, db->rd_ptr, databuf_unread(db));
    databuf_reset(db);
    int cplen = find_udata_len(poack->user_data, 8);
    set_rom_field(doj, ROM_CLR_ACC, poack->user_data, cplen); 
    pill_set_om_tag(doj, ci, poack->clordid);
    pill_set_om_ex_tag(doj, ci, poack->clordid);
    destroy_gk_db(ci->gk, db);   
}
static void set_defaults(dart_order_obj* doj, ex_config* ex)
{
    int ret_len = 0;
    char* dest_id = get_config_val(ex, "SERVER", 6,
                          "Dest ID", 7, &ret_len);
    if(ret_len > 0) {
        set_rom_field(doj, ROM_DESTID, dest_id, ret_len);
        set_rom_field(doj, ROM_DEST, dest_id, ret_len);
    }
    char* clr_id = get_config_val(ex, "SERVER", 6,
                            "CLR ID",6, &ret_len);
    if(ret_len > 0) {
        set_rom_field(doj, ROM_CLR_ID, clr_id, ret_len);
                
    }
    char* firm = get_config_val(ex, "SERVER", 6,
                            "FIRM",4, &ret_len);
    if(ret_len > 0) {
        set_rom_field(doj, ROM_FIRM, firm, ret_len);
    }
    if(ex->ulen > 0) {
        set_rom_field(doj, ROM_TRADER, ex->username, ex->ulen);
        set_rom_field(doj, ROM_TRADE_FOR, ex->username, ex->ulen);
        set_rom_field(doj, ROM_LOCAL_ACC, ex->username, ex->ulen);
        set_rom_field(doj, ROM_OWNER, ex->username, ex->ulen);
    }
}

static void parse_outgoing(char* d, con_int* ci, dart_order_obj* doj)
{
    psm* seq = (psm*)d;
    char* v = d + 32;
    uint16_t ord_type = 0;
    uint16_t len = 0;
    memcpy(&ord_type, v, 2);
    memcpy(&len, v + 2, 2);
    switch(ord_type){
        case 0x0240:
            parse_pill_new_ord_rep((pillneword*)v,ci,doj, seq->timestamp);
           break;
        case 0x0280:
            parse_pill_cnl((pillcancel*)v, ci,doj); 
            break;
    };
}
void pillar_cs_parse_oats(void* p, char* v, int dlen, dart_order_obj* doj)
{
    con_int* ci = (con_int*)p;
    con_int* oat = get_compliance_con(ci->gk);
    dartp* dp = (dartp*)oat->cparser;
    doj->dest = dp;
    set_defaults(doj, ci->ex);
    uint16_t ord_type = 0;
    uint16_t len = 0;
    memcpy(&ord_type, v, 2);
    memcpy(&len, v + 2, 2);
    switch(ord_type) {
        case PILL_ORD_ACK:
            op_parse_ack((pillordack*)v, ci, doj);
            break; 
        case PILL_EXE:
            op_parse_pill_exe((pillexerpt*)v, ci, doj);
            break;
        case PILL_MOD_OUT_ACK:
            op_parse_pill_out((pillmodcnlack*)v, ci, doj);
            break; 
        case PILL_CNL_REP_REJ:
            op_parse_pill_cnl_rej((pillcnlreprej*)v, ci, doj);
            break;
        case 0x0905:
            parse_outgoing(v, ci, doj);
            break;
        default:
            send_debug_message(ci->o, "Oats Parse Unknown: %d, %d \n", ord_type, len);
            break;
    }
}
