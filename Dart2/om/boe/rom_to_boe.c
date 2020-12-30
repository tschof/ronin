/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>

#include "rom_to_boe.h"
#include "boe_parser.h"
#include "dart_constants.h"
#include "dart_order_obj.h"
#include "common_structs.h"
#include "debug_logger.h"

static unsigned int null_int = 0x0;

int rom_to_boe_price(char *price, size_t len)
{
    double d = strtod(price, NULL);
    d = d * 10000;
    if(d > 0) {
        d += .00001;
    } else {
        d -= .00001;
    }
    return (int) d;
}
/**
 * For BATS we  translate one ROM field in to possibly three BOE fields.
 * The format for a ROM INSTR field for BATS goes like this:
 * XXYYY-ZZZZZ Where the X fields refer to the RoutingInst, this can be one
 * possibly 2 characters long(the only eligible second characters are 'D' and 'L'.
 * The Y chars refer to the RouteDelivery method, this can be absent but if present
 * it must be three characters long and either RTI or RTF.
 * Next we demand a dash before the Z's which mark the begining of the RoutStrategy.
 * We do not validate any of these fields and require the exchange to reject.
 */
static void trans_extra_routes(databuf_t* buff, dart_order_obj* rph,
        boeno* ord)
{
    char* inst = getpval(rph, ROM_INSTR);
    int full_len = getplen(rph, ROM_INSTR);
    if(inst[1] == 'D' || inst[1] == 'L') {
        inst += 2;
        if(inst[0] == '-') {
            SET_BIT(0x1, 0x4, ord->bf6); /* We have a route strategy */
            inst++;
            full_len -= 3;
            databuf_memcpy(buff, inst, full_len);
            buff->wr_ptr += (6 - full_len);
        } else if(full_len >= 9) {
            char* rdeliv = inst;
            inst += 4;
            full_len -= 6; 
            SET_BIT(0x1, 0x4, ord->bf6); /* We have a route strategy */
            databuf_memcpy(buff, inst, full_len);
            buff->wr_ptr += (6 - full_len);
            SET_BIT(0x1, 0x8, ord->bf6); /* We have a route delivery method */
            databuf_memcpy(buff, rdeliv, 3);
        }
    } else if(inst[1] == '-') {
        inst += 1; 
        full_len -= 2;
        SET_BIT(0x1, 0x4, ord->bf6); /* We have a route strategy */
        databuf_memcpy(buff, inst, full_len);
        buff->wr_ptr += (6 - full_len);
    } else {
        char* dash = memchr(inst, '-', full_len);
        if(dash) {
            dash += 1;
            SET_BIT(0x1, 0x4, ord->bf6); /* We have a route strategy */
            int strat_len = full_len - (dash  - inst); 
            databuf_memcpy(buff, dash, strat_len);
            buff->wr_ptr += (6 - strat_len);
            SET_BIT(0x1, 0x8, ord->bf6); /* We have a route delivery method */
            inst += 1;
            databuf_memcpy(buff, inst, 3);
        } else {
            SET_BIT(0x1, 0x4, ord->bf6); /* We have a route strategy */
            databuf_memcpy(buff, inst, full_len);
            buff->wr_ptr += (6 - full_len);
        }
    }
}
static void trans_ord_instr(databuf_t* buff, dart_order_obj* rph)
{
    if(getplen(rph, ROM_EXEC_INST) > 0 && getplen(rph, ROM_INSTR) <= 0) {
        char* ex_inst = getpval(rph, ROM_EXEC_INST);
        switch(ex_inst[0]) {
            case 'L':
            case 'M':
            case 'm':
            case 'P':
            case 'Q':
            case 'R':
            case 'U':
                databuf_memcpy(buff, "P", 1);
                buff->wr_ptr += 3;
                return;
        }
        databuf_memcpy(buff, "B", 1);
        buff->wr_ptr += 3;
        return;
    }
    if(getplen(rph, ROM_INSTR) == 1) {
        char* inst = getpval(rph, ROM_INSTR);
        if(inst[0] == 'I' || inst[0] == 'A') {
            databuf_memcpy(buff, "P", 1);
        } else {
            databuf_memcpy(buff, inst, 1);
        }
        buff->wr_ptr += 3;
    } else if(getplen(rph, ROM_INSTR) > 1) {
        char* inst = getpval(rph, ROM_INSTR);
        if(inst[1] == 'D' || inst[1] == 'L') {
            databuf_memcpy(buff, inst, 2);
            buff->wr_ptr += 2;
        } else {
            databuf_memcpy(buff, inst, 1);
            buff->wr_ptr += 3;
        }  
    } else {
        databuf_memcpy(buff, "R", 1);
        buff->wr_ptr += 3;
    }
}

int trans_mat_ymd(char* year_mon, int ymlen, char* day, int dlen)
{
    char packy[16];
    memset(packy, '\0', 16);
    memcpy(packy, year_mon, ymlen);
    if(dlen == 2) {
        memcpy(packy + ymlen, day, 2);
    } else {
        memcpy(packy + ymlen, "0", 1);
        memcpy(packy + ymlen + 1, day, 1);
    }
    return atoi(packy);
}
static char* trans_ord_type(char* rom_type, int rtlen , char* ex_inst, int ex_len)
{
    if(rtlen == 1 && ex_len <= 0) {
        if(rom_type[0] == '5') {
            return "1";
        }
        return rom_type;
    } else if(ex_len > 0) {
        switch(ex_inst[0]) {
            case 'L':
            case 'M':
            case 'm':
            case 'P':
            case 'Q':
            case 'R':
            case 'U':
                return "P";
                break;
            default:
                return rom_type;
        }
    } else {
        if(rtlen == 2 && (strncmp("11", rom_type, 2) == 0) ) {
            return "2";
        }
        if(rtlen == 2 && ((strncmp("12", rom_type, 2) == 0)||
                    (strncmp("10", rom_type, 2) == 0))) {
            return "1";
        }
        return rom_type;
    }
}

static char* trans_ord_tif(char* rom_type, int rtlen, char* ex_inst, int ex_len,
        char* rom_tif, int tif_len)
{
    if(rtlen >= 2) {
        if(rtlen == 2 && ((strncmp("11", rom_type, 2) == 0) ||
                    (strncmp("10", rom_type, 2) == 0))) {
            return "7";
        }
        if(rtlen == 2 && strncmp("12", rom_type, 2) == 0) {
            return "2";
        }
    }
    if(tif_len > 0) {
        if(rom_tif[0] == '7') {
            return "5";
        } 
        return rom_tif;
    } else {
        return "0";
    }
}

int create_boe_new_order_message(databuf_t * buff, void *v, int seq,
        con_interface * ob)
{
    memset(buff->buffer, 0, 256);
    boeno* ord  =  (boeno*)buff->buffer;
    buff->wr_ptr += sizeof(struct boe_new_ord);
    ord->type = 0x38;
    dart_order_obj *rph = (dart_order_obj *) v;
    btk* b = (btk*)ob->parser;
    ord->som = 0xBABA;
    int ord_id_len = create_multiday_in_place_ord_token(ob->tg,
            ord->ord_id, 20);
    memcpy(&ord->side, getpval(rph, ROM_SIDE), 1);
    unsigned int net_qty =
        strtol(rph->positions[ROM_SHARES].iov_base, NULL, 10);
    set_rom_field(rph, ROM_BIN_QTY, &net_qty, sizeof(int));
    set_rom_field(rph, ROM_BIN_CUM, &null_int, sizeof(int));
    set_rom_field(rph, ROM_LEAVES, rph->positions[ROM_SHARES].iov_base,
            rph->positions[ROM_SHARES].iov_len);
    ord->qty = net_qty;
    ord->nnobfields = 6; /*We always have 6 bit fields some may be 0*/
    SET_BIT(0x1, 0x1, ord->bf1); /*All new orders have clearing firm*/
    int mpid_len = 0;
    char* on_behalf = get_mpid_for_clr_acct(ob->sbm,
            getpval(rph, ROM_CLR_ID),
            getplen(rph, ROM_CLR_ID),
            &mpid_len);
    if(mpid_len > 0) {
        databuf_memcpy(buff, on_behalf,mpid_len);
    } else {
        databuf_memcpy(buff, getpval(rph, ROM_CLR_ID),getplen(rph, ROM_CLR_ID));
    }
    SET_BIT(0x1, 0x2, ord->bf1); /*All new orders have the clearing account*/
    char* clearing_account = getpval(rph, ROM_CLR_ACC);
    mpid_len = getplen(rph, ROM_CLR_ACC);
    if(mpid_len >= 7 && strncmp("CRON", clearing_account, 4) == 0) {
        int off = mpid_len > 7 ? 4 : 3;
        clearing_account += off;
        databuf_memcpy(buff, clearing_account,4);/*We only copy 4 anyway.*/
    } else {
        int fclr_len = 0;
        char* bclr =  (char*)boe_get_clr_account(b, clearing_account, mpid_len,
                   &fclr_len);
        if(fclr_len >= 7 && strncmp("74X5", bclr, 4) == 0) {
            int off = fclr_len > 7 ? 4 : 3;
            char* temp = bclr + off;
            databuf_memcpy(buff, temp,4);/*We only copy 4 anyway.*/
        } else {
            databuf_memcpy(buff, bclr,4);/*We only copy 4 anyway.*/
        }
    }
    if(getplen(rph, ROM_PRICE) > 0) {
        SET_BIT(0x1, 0x4, ord->bf1); /* We have a price*/
        if(getplen(rph, ROM_BIN_PRICE) <= 0) {
            unsigned long np = rom_to_boe_price(rph->positions[ROM_PRICE].iov_base,
                    rph->positions[ROM_PRICE].iov_len);
            set_rom_field(rph, ROM_BIN_PRICE, &np, sizeof(long));
            databuf_memcpy(buff, (char *) &np, sizeof(long));
        } else  {
            long np = 0;
            memcpy(&np, getpval(rph, ROM_BIN_PRICE), getplen(rph, ROM_BIN_PRICE));
            databuf_memcpy(buff,(char*)&np, sizeof(long));
        }
    }
    if(getplen(rph, ROM_EXEC_INST) > 0) {
        char* exec = getpval(rph, ROM_EXEC_INST);
        if(exec[0] == 'f') {
            return -1;/*Intermarket sweeps are not allowed*/
        }
        SET_BIT(0x1, 0x8, ord->bf1); /* We have execution instructions*/
        databuf_memcpy(buff, exec, 1);
    }

    SET_BIT(0x1, 0x10, ord->bf1); /* DART requires an order type*/
    databuf_memcpy(buff, trans_ord_type(getpval(rph, ROM_TYPE), getplen(rph, ROM_TYPE),
                getpval(rph, ROM_EXEC_INST), getplen(rph, ROM_EXEC_INST)), 1);
    if(getplen(rph, ROM_TIF) > 0) {
        SET_BIT(0x1, 0x20, ord->bf1); /* We have a time in force*/
        databuf_memcpy(buff, trans_ord_tif(getpval(rph, ROM_TYPE), getplen(rph, ROM_TYPE),
                    getpval(rph, ROM_EXEC_INST), getplen(rph, ROM_EXEC_INST),
                    getpval(rph, ROM_TIF), getplen(rph, ROM_TIF)), 1);
    } 
    if(getplen(rph, ROM_MIN_QTY) > 0) {
        SET_BIT(0x1, 0x40, ord->bf1); /* We have a Min qty*/
        unsigned int min = atoi(getpval(rph, ROM_MIN_QTY));
        databuf_memcpy(buff, &min, 4);
    }
    if(getplen(rph, ROM_MAX_FLOOR) > 0) {
        SET_BIT(0x1, 0x80, ord->bf1); /* We have a Max Floor*/
        unsigned int max = atoi(getpval(rph, ROM_MAX_FLOOR));
        databuf_memcpy(buff, &max, 4);
    }
    if(getplen(rph, ROM_SYM) > 0) {
        SET_BIT(0x1, 0x1, ord->bf2); /* We have a Symbol*/
        char* symbol = getpval(rph, ROM_SYM);
        char* slash = memchr(getpval(rph, ROM_SYM), '/', getplen(rph, ROM_SYM));
        if(slash != NULL) {
            //SET_BIT(0x1, 0x2, ord->bf2); /* We have a Symbol suffix*/
            int sym_len = slash - symbol;
            int suf_len = getplen(rph, ROM_SYM) - (sym_len + 1);
            int fsufflen = 0;
            const char* bsuffix =  boe_get_suffix(b, (slash + 1), suf_len,
                       &fsufflen);
            databuf_memcpy(buff, symbol, sym_len);
            databuf_memcpy(buff, bsuffix, fsufflen);
            buff->wr_ptr += (8 - (sym_len + fsufflen));
        } else {
            databuf_memcpy(buff, getpval(rph, ROM_SYM), getplen(rph, ROM_SYM));
            buff->wr_ptr += (8 - getplen(rph, ROM_SYM));
        }
    }
    if(getplen(rph, ROM_CAP) > 0) {
        SET_BIT(0x1, 0x40, ord->bf2); /* We have a capacity*/
        databuf_memcpy(buff, getpval(rph, ROM_CAP), 1);
    }
    if(getplen(rph, ROM_INSTR) > 0 ||
            getplen(rph, ROM_EXEC_INST) > 0) {
        SET_BIT(0x1, 0x80, ord->bf2); /* We have a route*/
        trans_ord_instr(buff, rph);
    }
    if(getplen(rph, ROM_CLR_ACC) > 0) {
        SET_BIT(0x1, 0x1, ord->bf3); /*We have an account*/
        int acc_len = getplen(rph, ROM_CLR_ACC);
        clearing_account = getpval(rph, ROM_CLR_ACC);
        if(acc_len > 4 && strncmp("CRON", clearing_account, 4) == 0) {
            databuf_memcpy(buff, clearing_account, acc_len);
            buff->wr_ptr += (16 - acc_len);
        } else {
            int fclr_len = 0;
            const char* bclr =  boe_get_clr_account(b, clearing_account, acc_len,
                       &fclr_len);
            databuf_memcpy(buff, bclr, fclr_len);
            buff->wr_ptr += (16 - fclr_len);
        }
    }
    if(getplen(rph, ROM_DISP_INSTR) > 0) {
        SET_BIT(0x1, 0x2, ord->bf3); /*We have a display instruction*/
        databuf_memcpy(buff, getpval(rph, ROM_DISP_INSTR), 1);
    }
    if(getplen(rph, ROM_PEG_DIFF) > 0) {
        SET_BIT(0x1, 0x10, ord->bf3); /*We have a peg difference*/
        long np = rom_to_boe_price(rph->positions[ROM_PEG_DIFF].iov_base,
                rph->positions[ROM_PEG_DIFF].iov_len);
        databuf_memcpy(buff, &np, sizeof(long));
    }
    if(getplen(rph, ROM_DESC) == 3) {
        SET_BIT(0x1, 0x20, ord->bf3); /*We have a prevent match code*/
        databuf_memcpy(buff, getpval(rph, ROM_DESC), 3);
    }
    if(getplen(rph, ROM_SHORT_LEND) > 0) {
        SET_BIT(0x1, 0x40, ord->bf3); /*We have a short lender*/
        databuf_memcpy(buff, "N", 1);
    }
    if(getplen(rph, ROM_EXPIRE_TIME) > 0) {
        SET_BIT(0x1, 0x80, ord->bf3); /*We have an expire time */
        long gtd = convert_to_nanosec_since_1970(getpval(rph, ROM_EXPIRE_TIME));
        databuf_memcpy(buff, &gtd, sizeof(long));
    }
    if(getplen(rph, ROM_STOP) > 0) {
        SET_BIT(0x1, 0x2, ord->bf6); /*We have a stop price */
        unsigned long np = rom_to_boe_price(rph->positions[ROM_STOP].iov_base,
                rph->positions[ROM_STOP].iov_len);
        databuf_memcpy(buff, &np, sizeof(long));
    }
    if(getplen(rph, ROM_INSTR) > 2) {
        /*This check is for the ROUTE strategy and delivery fields*/
        trans_extra_routes(buff, rph, ord);
    }
    ord->len = databuf_unread(buff) - 2;    
    /*Since ROM_CLORDID and instructions are the same field we need to wait until 
     * we have used the instructions before we set the id.*/
    set_rom_field(rph, ROM_CLORDID, ord->ord_id, ord_id_len);    
    no_copy_insert(ob->cl_ord_id_map, getpval(rph,ROM_CLORDID),
            getplen(rph,ROM_CLORDID),rph);
    return databuf_unread(buff);
}
int create_boe_cancel_message(databuf_t * buff, void *v, int seq,
        con_interface * ob)
{
    memset(buff->buffer, 0, 256);
    boecnl* ord  =  (boecnl*)buff->buffer;
    buff->wr_ptr += sizeof(struct boe_cnl_ord);
    ord->type = 0x39;
    dart_order_obj *rph = (dart_order_obj *) v;
    ord->som = 0xBABA;
    ord->len = databuf_unread(buff) - 2; 
    memcpy(ord->orig_id, getpval(rph, ROM_CLORDID),
            getplen(rph, ROM_CLORDID));   
    ord->ncnlbfields = 0;
    return databuf_unread(buff);
}
int create_boe_replace_message(databuf_t * buff, void *v, int seq,
        con_interface * ob)
{
    memset(buff->buffer, 0, 256);
    boerpl* ord  =  (boerpl*)buff->buffer;
    buff->wr_ptr += sizeof(struct boe_rpl_ord);
    btk* b = (btk*)ob->parser;
    ord->type = 0x3A;
    dart_order_obj *rph = (dart_order_obj *) v;
    ord->som = 0xBABA;
    memcpy(ord->orig_id, getpval(rph, ROM_CLORDID),
            getplen(rph, ROM_CLORDID));   
    int ord_id_len = create_multiday_in_place_ord_token(ob->tg,
            ord->ord_id, 20);
    set_rom_field(rph, ROM_PENDING_ID, ord->ord_id, ord_id_len);    
    no_copy_insert(ob->cl_ord_id_map, getpval(rph,ROM_PENDING_ID),
            getplen(rph,ROM_PENDING_ID),rph);
    ord->nrplbfields = 2;
    /* Since we have a service bureau ports we need a clearing firm.*/
    SET_BIT(0x1, 0x1, ord->bf1);
    char* on_behalf = getpval(rph, ROM_CLR_ACC);
    int mpid_len = getplen(rph, ROM_CLR_ACC);
    if(mpid_len >= 7 && strncmp("CRON", on_behalf, 4) == 0) {
        int off = mpid_len > 7 ? 4 : 3;
        on_behalf += off;
        databuf_memcpy(buff, on_behalf,4);/*We only copy 4 anyway.*/
    }else {
        int fclr_len = 0;
        char* bclr =  (char*)boe_get_clr_account(b, on_behalf, mpid_len,
                   &fclr_len);
        if(fclr_len >= 7 && strncmp("74X5", bclr, 4) == 0) {
            int off = fclr_len > 7 ? 4 : 3;
            char* temp = bclr + off;
            databuf_memcpy(buff, temp,4);/*We only copy 4 anyway.*/
        } else {
            databuf_memcpy(buff, bclr,4);/*We only copy 4 anyway.*/
        }
    }

    /* All replaces MUST have a price and qty*/
    SET_BIT(0x1, 0x4, ord->bf1);
    SET_BIT(0x1, 0x8, ord->bf1);
    dart_order_obj *rep = (dart_order_obj *) rph->rep;
    if(rep->positions[ROM_SHARES].iov_len > 0 &&
            rep->positions[ROM_BIN_QTY].iov_len <= 0) {
        unsigned int net_qty =
            strtol(rep->positions[ROM_SHARES].iov_base, NULL, 10);
        set_rom_field(rep, ROM_BIN_QTY, &net_qty, sizeof(int));
        databuf_memcpy(buff, getpval(rep, ROM_BIN_QTY), getplen(rep, ROM_BIN_QTY));
    } else if(getplen(rep, ROM_BIN_QTY) > 0) {
        databuf_memcpy(buff, getpval(rep, ROM_BIN_QTY), getplen(rep, ROM_BIN_QTY));
    } else {
        databuf_memcpy(buff, getpval(rph, ROM_BIN_QTY), getplen(rph, ROM_BIN_QTY));
    }
    if(rep->positions[ROM_PRICE].iov_len > 0 && rep->positions[ROM_BIN_PRICE].iov_len <= 0) {
        unsigned long np = rom_to_boe_price(rep->positions[ROM_PRICE].iov_base,
                rep->positions[ROM_PRICE].iov_len);
        set_rom_field(rep, ROM_BIN_PRICE, &np, sizeof(long));
        databuf_memcpy(buff, (char *) &np, sizeof(long));
    } else if(getplen(rep, ROM_BIN_PRICE) > 0) {
        unsigned long np = 0;
        memcpy(&np, rep->positions[ROM_BIN_PRICE].iov_base,
                rep->positions[ROM_BIN_PRICE].iov_len);
        databuf_memcpy(buff, (char *) &np, sizeof(long));
    } else {
        unsigned long np = 0;
        memcpy(&np, rph->positions[ROM_BIN_PRICE].iov_base,
                rph->positions[ROM_BIN_PRICE].iov_len);
        databuf_memcpy(buff, (char *) &np, sizeof(long));
    }
    if(rep->positions[ROM_TYPE].iov_len > 0) {
        if(rph->positions[ROM_TYPE].iov_len == rep->positions[ROM_TYPE].iov_len) {
            char* orig_type = getpval(rph, ROM_TYPE);
            char* rep_type = getpval(rep, ROM_TYPE);
            if(strncmp(orig_type, rep_type, getplen(rep, ROM_TYPE)) != 0) {
                SET_BIT(0x1, 0x10, ord->bf1);/*Type changed*/
                databuf_memcpy(buff, trans_ord_type(getpval(rep, ROM_TYPE),
                               getplen(rep, ROM_TYPE),
                               getpval(rep, ROM_EXEC_INST),
                               getplen(rep, ROM_EXEC_INST)), 1);
            }
        } else {
            SET_BIT(0x1, 0x10, ord->bf1);/*Type changed*/
            databuf_memcpy(buff, trans_ord_type(getpval(rep, ROM_TYPE),
                           getplen(rep, ROM_TYPE),
                           getpval(rep, ROM_EXEC_INST),
                           getplen(rep, ROM_EXEC_INST)), 1);
        }

    }
    if(getplen(rep, ROM_EXEC_INST) > 0) {
        char* exec = getpval(rep, ROM_EXEC_INST);
        if(exec[0] == 'f') {
            return -1;/*Intermarket sweeps are not allowed*/
        }
        SET_BIT(0x1, 0x40, ord->bf1); /* We have execution instructions*/
        databuf_memcpy(buff, exec, 1);
    }
    if(getplen(rep, ROM_SIDE) > 0) {
        if(getplen(rph, ROM_SIDE) == getplen(rep, ROM_SIDE)) {
            char* orig_side = getpval(rph, ROM_SIDE);
            char* rep_side = getpval(rep, ROM_SIDE);
            if(strncmp(orig_side, rep_side, getplen(rep, ROM_SIDE)) != 0) {
                SET_BIT(0x1, 0x80, ord->bf1); /* We have a new side */
                databuf_memcpy(buff, getpval(rep, ROM_SIDE), 1);
            }
        } else {
            SET_BIT(0x1, 0x80, ord->bf1); /* We have a new side */
            databuf_memcpy(buff, getpval(rep, ROM_SIDE), 1);
        }
    }
    if(getplen(rep, ROM_MAX_FLOOR) > 0) {
        SET_BIT(0x1, 0x1, ord->bf2); /* We have a Max Floor*/
        unsigned int max = atoi(getpval(rep, ROM_MAX_FLOOR));
        databuf_memcpy(buff, &max, 4);
    }
    if(getplen(rep, ROM_STOP) > 0) {
        SET_BIT(0x1, 0x2, ord->bf2); /*We have a stop price */
        unsigned long np = rom_to_boe_price(rep->positions[ROM_STOP].iov_base,
                rep->positions[ROM_STOP].iov_len);
        databuf_memcpy(buff, &np, sizeof(long));
    }
    ord->len = databuf_unread(buff) - 2; 
    return databuf_unread(buff);
}

static void set_time(dart_order_obj* ute, char* mess, con_interface* ob) 
{
    long tt = 0;
    memcpy(&tt, (mess + 10), 8);
    btk* b = (btk*)ob->parser;
    char itoa_buff[64];
    int date_len = boe_to_rom_time(b, tt, itoa_buff);
    fill_in_rom_field(ute, EXEC_TIME, itoa_buff, date_len);
}

static void set_ordid(dart_order_obj* ute, char* mess, con_interface* ob)
{
    long bordid = 0;
    memcpy(&bordid, mess + 38, 8);
    databuf_t *itoa_buff = get_databuf(ob->gk);
    dart_itoa(ute, bordid,ROM_EX_TAG, itoa_buff);
    destroy_gk_db(ob->gk, itoa_buff);
}
int boe_parse_order_ack(dart_order_obj * ute, char *ack, con_interface * ob)
{
    set_time(ute, ack, ob);
    reset_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    reset_rom_field(ute, ROM_STATUS, "13", 2);
    set_ordid(ute, ack, ob);
    return 1;
}
int boe_parse_cancel_rej(dart_order_obj * ute, char *ack, con_interface * ob)
{
    reset_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    reset_rom_field(ute, ROM_STATUS, "14", 2);
    set_time(ute, ack, ob);
    char* text = ack + 39;
    int tlen = 0;
    FIND_NULL(text, tlen, 60);
    fill_in_rom_field(ute, ROM_TEXT, text, tlen);
    return 1;
}
int boe_parse_ur_out(dart_order_obj * ute, char *ack, con_interface * ob)
{
    set_time(ute, ack, ob);
    reset_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    reset_rom_field(ute, ROM_STATUS, "4", 1);
    pthread_spin_lock(&ob->mutex);
    remove_from_map(ob->cl_ord_id_map,
                    ute->positions[ROM_CLORDID].iov_base,
                    ute->positions[ROM_CLORDID].iov_len);
    pthread_spin_unlock(&ob->mutex);
    return 1;
}
int boe_parse_rep_rej(dart_order_obj * ute, char *ack, con_interface * ob)
{
    reset_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    reset_rom_field(ute, ROM_STATUS, "20", 2);
    set_time(ute, ack, ob);
    pthread_spin_lock(&ob->mutex);
    remove_from_map(ob->cl_ord_id_map,
                    ute->positions[ROM_PENDING_ID].iov_base,
                    ute->positions[ROM_PENDING_ID].iov_len);
    pthread_spin_unlock(&ob->mutex);
    dart_order_obj *rep = (dart_order_obj *) ute->rep;
    char* text = ack + 39;
    int tlen = 0;
    FIND_NULL(text, tlen, 60);
    fill_in_rom_field(ute, ROM_TEXT, text, tlen);
    destroy_gk_obj(ob->gk, rep);
    ute->rep = 0;
    return 1;
}
int boe_parse_replaced(dart_order_obj * ute, char *ack, con_interface * ob)
{
    int ret_val = 0;
    reset_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    reset_rom_field(ute, ROM_STATUS, "5", 1);
    set_time(ute, ack, ob);
    set_ordid(ute, ack, ob);

    char num_bit_fields = *(ack + 47);
    char bit_one = *(ack + 48);
    char bit_three = *(ack + 50);
    char bit_eight = *(ack + 55);
    unsigned int is_present = 0;
    if(num_bit_fields < 8) {
        send_debug_message("We received fewer than 8 bit fields on a replace: %d\n",
                            num_bit_fields);
    }
    dart_order_obj *rep = (dart_order_obj *) ute->rep;
    databuf_t *buff = get_databuf(ob->gk);
    IS_BIT_SET(is_present, bit_three, 6);
    if(rep && is_present) {
        if(rep->positions[ROM_SHARES].iov_len > 0) {
            fill_in_rom_field(ute, ROM_SHARES, rep->positions[ROM_SHARES].iov_base,
                              rep->positions[ROM_SHARES].iov_len);
            int l_q = strtol(rep->positions[ROM_SHARES].iov_base, NULL, 10);
            memcpy(ute->positions[ROM_BIN_QTY].iov_base, &l_q, 4);
            //setting leaves
            int l_cum = 0;
            memcpy(&l_cum, ute->positions[ROM_BIN_CUM].iov_base, 4);
            int leaves = l_q - l_cum;
            dart_itoa(ute, leaves, ROM_LEAVES, buff);
            //End leaves
        }
    } else {
        send_debug_message("We did not see the share bit three as set: %#010x\n",
                            bit_three);
    }
    IS_BIT_SET(is_present, bit_one, 2);
    if(rep && is_present) {
        if(rep->positions[ROM_BIN_PRICE].iov_len > 0) { 
            memcpy(ute->positions[ROM_BIN_PRICE].iov_base,
                   rep->positions[ROM_BIN_PRICE].iov_base, getplen(rep, ROM_BIN_PRICE));
        }
        if(rep->positions[ROM_PRICE].iov_len > 0) {
            fill_in_rom_field(ute, ROM_PRICE, rep->positions[ROM_PRICE].iov_base,
                              rep->positions[ROM_PRICE].iov_len);
        }
    } else {
        send_debug_message("We did not see the share bit one as set: %#010x\n",
                            bit_one);
    }

    if(rep) {
        pthread_spin_lock(&ob->mutex);
        remove_from_map(ob->cl_ord_id_map,
                        ute->positions[ROM_CLORDID].iov_base,
                        ute->positions[ROM_CLORDID].iov_len);
        pthread_spin_unlock(&ob->mutex);
        /*Save off the old position, we are going to perform a switcharoo.*/
        void* old_ord_off = getpval(ute, ROM_CLORDID);
        reset_rom_field(ute, ROM_CLORDID, ute->positions[ROM_PENDING_ID].iov_base,
                        ute->positions[ROM_PENDING_ID].iov_len);
        reset_rom_field(ute, ROM_PENDING_ID, old_ord_off, getplen(ute,ROM_PENDING_ID));
        IS_BIT_SET(is_present, bit_one, 0x0);
        if(is_present && getplen(rep, ROM_SIDE) > 0) {
            fill_in_rom_field(ute, ROM_SIDE, getpval(rep, ROM_SIDE), 
                              getplen(rep, ROM_SIDE));
        }
        IS_BIT_SET(is_present, bit_one, 0x1);
        if(is_present && getplen(rep, ROM_PEG_DIFF) > 0) {
            fill_in_rom_field(ute, ROM_PEG_DIFF, getpval(rep, ROM_PEG_DIFF), 
                              getplen(rep, ROM_PEG_DIFF));
        }
        IS_BIT_SET(is_present, bit_one, 0x3);
        if(is_present && getplen(rep, ROM_EXEC_INST) > 0) {
            fill_in_rom_field(ute, ROM_EXEC_INST, getpval(rep, ROM_EXEC_INST), 
                              getplen(rep, ROM_EXEC_INST));
        }
        IS_BIT_SET(is_present, bit_one, 5);
        if(is_present && getplen(rep, ROM_TIF) > 0) {
            fill_in_rom_field(ute, ROM_TIF, getpval(rep, ROM_TIF),
                              getplen(rep, ROM_TIF));
        }
        IS_BIT_SET(is_present, bit_one, 6);
        if(is_present && getplen(rep, ROM_MIN_QTY) > 0) {
            fill_in_rom_field(ute, ROM_MIN_QTY, getpval(rep, ROM_MIN_QTY),
                              getplen(rep, ROM_MIN_QTY));
        }
        IS_BIT_SET(is_present, bit_three, 0x3);
        if(is_present && getplen(rep, ROM_DISP_INSTR) > 0) {
            fill_in_rom_field(ute, ROM_DISP_INSTR, getpval(rep, ROM_DISP_INSTR),
                              getplen(rep, ROM_DISP_INSTR));
        }
        IS_BIT_SET(is_present, bit_three, 4);
        if(is_present && getplen(rep, ROM_MAX_FLOOR) > 0) {
            fill_in_rom_field(ute, ROM_MAX_FLOOR, getpval(rep, ROM_MAX_FLOOR),
                              getplen(rep, ROM_MAX_FLOOR));
        }
        IS_BIT_SET(is_present, bit_eight, 0x2);
        if(is_present && getplen(rep, ROM_STOP) > 0) {
            fill_in_rom_field(ute, ROM_STOP, getpval(rep, ROM_STOP),
                              getplen(rep, ROM_STOP));
        }
        unlock(rep);
        destroy_gk_obj(ob->gk, rep);
    }
    destroy_gk_db(ob->gk, buff);
    return ret_val;
}
int boe_parse_rej(dart_order_obj * ute, char *ack, con_interface * ob)
{
    int ret_val = 0;
    reset_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    reset_rom_field(ute, ROM_STATUS, "8", 1);
    set_time(ute, ack, ob);
    char* text = ack + 39;
    int text_len = 0;
    FIND_NULL(text, text_len, 60);
    fill_in_rom_field(ute, ROM_TEXT, text, text_len);
    return ret_val;
}
unsigned int boe_set_ex_id(dart_order_obj * doj, unsigned long value,
                                int rom_tag,  databuf_t* db)
{
    char *str = db->buffer;
    int radix = 10;
    static char dig[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    unsigned int n = 0;
    unsigned long v = 0;
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
    fill_in_rom_field(doj, rom_tag, db->buffer,n);
    return n;
}
int boe_parse_filled(dart_order_obj * ute, char *ack, con_interface * ob)
{
    char num_bit_fields = *(ack + 69);
    char bit_eight = *(ack + 77);
    unsigned int is_present = 0;
    int ret_val = 0;
    set_time(ute, ack, ob);
    reset_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
    databuf_t *buff = get_databuf(ob->gk);
    long execid = 0;
    memcpy(&execid, ack + 38, 8);
    boe_set_ex_id(ute, execid, EXEC_ID, buff);
    databuf_reset(buff);
    int last_shares = 0;
    memcpy(&last_shares, ack + 46, 4);
    int leaves_shares = 0;
    memcpy(&leaves_shares, ack + 58, 4);
    int l_cum = 0;
    int qty = 0;
    memcpy(&qty, ute->positions[ROM_BIN_QTY].iov_base, 4);
    memcpy(&l_cum, ute->positions[ROM_BIN_CUM].iov_base, 4);
    l_cum += last_shares;
    fill_in_rom_field(ute, ROM_BIN_LAST_SHARES, &last_shares, sizeof(int));
    fill_in_rom_field(ute, ROM_BIN_CUM, &l_cum, 4);
    if (l_cum >= qty) {
        reset_rom_field(ute, ROM_STATUS, "2", 1);
        pthread_spin_lock(&ob->mutex);
        remove_from_map(ob->cl_ord_id_map,
                        ute->positions[ROM_CLORDID].iov_base,
                        ute->positions[ROM_CLORDID].iov_len);
        if(getplen(ute, ROM_PENDING_ID) > 0) {
            remove_from_map(ob->cl_ord_id_map,
                            ute->positions[ROM_PENDING_ID].iov_base,
                            ute->positions[ROM_PENDING_ID].iov_len);
        }
        pthread_spin_unlock(&ob->mutex);
        ret_val = 1;
    } else {
        reset_rom_field(ute, ROM_STATUS, "1", 1);
    }
    dart_itoa(ute, l_cum, ROM_CUM, buff);
    databuf_reset(buff);
    dart_itoa(ute, last_shares, ROM_LAST_SHARES, buff);
    databuf_reset(buff);
    dart_itoa(ute, leaves_shares, ROM_LEAVES, buff);
    databuf_reset(buff);

    long last_px = 0;//(long)*(ack + 50);
    memcpy(&last_px, ack + 50, 8);
    fill_in_rom_field(ute, ROM_BIN_LAST_PRICE, &last_px, sizeof(int));
    double lp = ((double) last_px / 10000.0) + .0000001;
    int dlen = sprintf(buff->buffer, "%f", lp);
    fill_in_rom_field(ute, ROM_EXEC_PRICE, buff->buffer, dlen);
    
    IS_BIT_SET(is_present, bit_eight, 0x0);
    if(is_present && num_bit_fields >= 8) {
        char* liq = ack + 78; /*The next two bytes are the FeeCode*/
        if(*(liq + 1) == 0) {
            fill_in_rom_field(ute, ROM_LIQ, liq, 1);
        } else {
            fill_in_rom_field(ute, ROM_LIQ, liq, 2);
        }
    } else {
        char* liq = ack + 62; /*The next two bytes are the liq*/
        if(*(liq + 1) == 0) {
            fill_in_rom_field(ute, ROM_LIQ, liq, 1);
        } else {
            fill_in_rom_field(ute, ROM_LIQ, liq, 2);
        }
    }
    destroy_gk_db(ob->gk, buff);
    return ret_val;
}

