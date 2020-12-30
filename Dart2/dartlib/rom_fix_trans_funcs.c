/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rom_fix_trans_funcs.h"
#include "rex_constants.h"


void set_exec_time(trans_t* t, ofp* obj)
{
    time_t now;
    struct tm tmval;
    struct tm *tmm = &tmval;
    time(&now);
    if ((tmm = gmtime_r(&now, &tmval)) == 0) {
        perror("Calling gmtime_r");
    }
    set_time_tag(obj,&tmval, 60);
}
char* translate_pending_status(char val)
{
    switch(val) {
        case 'D':
            return "E";
        default:
            return "S";
    }
}

char* get_rom_status_for_fix(char* val, int val_len, int* ret_len)
{
    switch(val[0]) {
        case 'D':
            *ret_len = 1;
            return "0";
        case 'G':
            *ret_len = 2;
            return "27";
        case 'F':
            *ret_len = 2;
            return "26";
            break;
        case '0':
            *ret_len = 2;
            return "13";
        default:
            *ret_len = val_len;
            return val;
    }
}
char translate_put_call(char val)
{
    switch(val) {
        case 'P':
            return '0';
        case '0':
            return 'P';
        case '1':
            return 'C';
        case 'C':
            return '1';
        default:
            return '0';
    }
}

char translate_open_close(char val)
{
    switch(val) {
        case '1':
            return 'O';
        case 'O':
            return '1';
        case '0':
            return 'C';
        case 'C':
            return '0';
        default:
            return '0';
    }
}

int trans_sec_type(char* val, int val_len, char* output)
{
    int len = 0;
    if(val_len == 1) {
        switch(val[0]) {
            case 'E':
                memcpy(output, "CS", 2);
                len = 2;
                break;
            case 'F':
                memcpy(output, "FUT", 3);
                len = 3;
                break;
            case 'O':
                memcpy(output, "OPT", 3);
                len = 3;
                break;
            case 'X':
                memcpy(output, "FOR", 3);
                len = 3;
                break;
            default:
                len = 0;
                break;
        }
    } else {
        if(strncmp("CS", val, 2) == 0) {
            memcpy(output, "E", 1);
            len = 1;
        } else if(strncmp("FUT", val, 3) == 0) {
            memcpy(output, "F", 1);
            len = 1;
        } else if(strncmp("OPT", val, 3) == 0) {
            memcpy(output, "O", 1);
            len = 1;
        } else if(strncmp("FOR", val, 3) == 0) {
            memcpy(output, "X", 1);
            len = 1;
        }
    }
    return len;
}
void remove_from_maps(con_int * ci, char *id, int id_len)
{
    pthread_spin_lock(&ci->mutex);
    remove_from_map(ci->cl_ord_id_map, id, id_len);
    pthread_spin_unlock(&ci->mutex);
}
int get_obj_by_id(con_int * ob, void *clordid, int len,
        rcntr ** obj)
{
    int ret = 0;
    pthread_spin_lock(&ob->mutex);
    ret = get_map_entry(ob->cl_ord_id_map, clordid, len, (void **) obj);
    pthread_spin_unlock(&ob->mutex);
    return ret;
}
void set_parent_fill_fields(dart_order_obj* child, dart_order_obj* parent,
                            dart_order_obj* ex_mess, int last_shares, int cum,
                            databuf_t* itoa_buff)
{
        set_rom_field(parent, 0, ROM_ZERO_STATUS, 1);
        set_rom_field(parent, ROM_STATUS, getpval(child, ROM_STATUS), getplen(child, ROM_STATUS));
        fill_in_rom_field(parent, ROM_EXEC_PRICE, getpval(parent, ROM_PRICE),
                          getplen(parent, ROM_PRICE));
        char atoi_buff[50];
        memset(atoi_buff, '\0', 50);
        memcpy(atoi_buff, getpval(child, ROM_SHARES), getplen(child, ROM_SHARES));
        int ratio = atoi(atoi_buff);
        if(ratio > 1) {
            int bin_leaves = 0;
            int bin_last = last_shares/ratio;
            int lcum = cum/ratio;
            if(getplen(parent, ROM_BIN_QTY) >0) {
                int bin_shares = 0;
                 memcpy(&bin_shares, getpval(parent, ROM_BIN_QTY), sizeof(int));
                 bin_leaves = bin_shares - lcum;
            } else {
                memset(atoi_buff, '\0', 50);
                memcpy(atoi_buff, getpval(parent, ROM_SHARES), getplen(parent, ROM_SHARES));
                int bin_shares = atoi(atoi_buff);
                fill_in_rom_field(parent, ROM_BIN_QTY, &bin_shares, sizeof(int));
                bin_leaves = bin_shares - lcum;
            }
            fill_in_rom_field(parent, ROM_BIN_LAST_SHARES, &bin_last, sizeof(int));
            fill_in_rom_field(parent, ROM_BIN_CUM, &lcum, sizeof(int));
            dart_itoa(parent, bin_leaves, ROM_LEAVES, itoa_buff);
            dart_itoa(parent, bin_last, ROM_LAST_SHARES, itoa_buff);
            dart_itoa(parent, lcum, ROM_CUM, itoa_buff);

        } else {
            fill_in_rom_field(parent, ROM_LEAVES, getpval(child, ROM_LEAVES), 
                              getplen(child, ROM_LEAVES));
            fill_in_rom_field(parent, ROM_CUM, getpval(child, ROM_CUM),
                              getplen(child, ROM_CUM));
            fill_in_rom_field(parent, ROM_LAST_SHARES, getpval(child, ROM_LAST_SHARES),
                              getplen(child, ROM_LAST_SHARES));
            if(getplen(ex_mess, FIX_CUM) > 0) {
                int cum = atoi(getpval(ex_mess, FIX_CUM));
                fill_in_rom_field(parent, ROM_BIN_CUM, &cum, sizeof(int));
            } 
        }
}
void set_rep_fields(dart_order_obj * ute, dart_order_obj * rep, 
        con_int* ob)
{
    char *off = rep->positions[ROM_SHARES].iov_base;
    fill_in_rom_field(ute, ROM_SHARES, off,
            rep->positions[ROM_SHARES].iov_len);
    int l_q = strtol(off, NULL, 10);
    fill_in_rom_field(ute, ROM_BIN_QTY, &l_q, 4);
    if (rep->positions[ROM_BIN_PRICE].iov_base != NULL) {
        memcpy(ute->positions[ROM_BIN_PRICE].iov_base,
                rep->positions[ROM_BIN_PRICE].iov_base, 4);
    }
    fill_in_rom_field(ute, ROM_PRICE, rep->positions[ROM_PRICE].iov_base,
            rep->positions[ROM_PRICE].iov_len);
    // Setting leaves
    int l_cum = 0;
    if(ute->positions[ROM_BIN_CUM].iov_base != NULL) {
        memcpy(&l_cum, ute->positions[ROM_BIN_CUM].iov_base, 4);
    }
    int leaves = l_q - l_cum;
    databuf_t *itoa_buff = get_databuf(ob->gk);
    dart_itoa(ute, leaves, ROM_LEAVES, itoa_buff);
    // End leaves
    pthread_spin_lock(&ob->mutex);
    remove_from_map(ob->cl_ord_id_map,
                    ute->positions[ROM_CLORDID].iov_base,
                    ute->positions[ROM_CLORDID].iov_len);
    fill_in_rom_field(ute, ROM_CLORDID, ute->positions[ROM_PENDING_ID].iov_base,
                   ute->positions[ROM_PENDING_ID].iov_len);
    no_copy_insert(ob->cl_ord_id_map, getpval(ute, ROM_CLORDID), getplen(ute, ROM_CLORDID),
                   ute);
    destroy_gk_obj(ob->gk, rep);
    destroy_gk_db(ob->gk, itoa_buff);
    ute->rep = 0;
    pthread_spin_unlock(&ob->mutex);
}
void handle_cnl_rpl_rej(dart_order_obj* doj, con_int* ci)
{
    rcntr *cnt = 0;
    int found = get_obj_by_id(ci, doj->positions[FIX_CLORDID].iov_base,
            doj->positions[FIX_CLORDID].iov_len, &cnt);
    if(found == 0) {
        found = get_obj_by_id(ci, doj->positions[FIX_ORIG_ID].iov_base,
                              doj->positions[FIX_ORIG_ID].iov_len, &cnt);
    }
    if (found) {
        if(cnt->rlen == sizeof(dart_order_obj)) {
            dart_order_obj* ute = (dart_order_obj*)cnt->raw;
            if(getplen(doj, FIX_CNL_TYPE) > 0 && ((char*)getpval(doj,FIX_CNL_TYPE))[0] == '2') {
                set_rom_field(ute, ROM_STATUS, "20", 2);
            } else {
                set_rom_field(ute, ROM_STATUS, "14", 2);
            }
            set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
            if(getplen(doj, FIX_TEXT) > 0) {
                fill_in_rom_field(ute, ROM_TEXT, getpval(doj, FIX_TEXT),
                                  getplen(doj, FIX_TEXT));
            }
            ci->on_rej(ci, &ute->risk);
        }
    }
}

int is_spread(dart_order_obj * doj)
{
    return doj->positions[ROM_SPREAD].iov_len > 0;
}
static void set_cfi_code(trans_t* t, dart_order_obj* obj, ofp* fix_spread)
{
    char* sectype = getpval(obj, ROM_SEC_TYPE);
    if(getplen(obj, ROM_SEC_TYPE) > 0 && sectype[0] == 'O') {
        char* cp = getpval(obj, ROM_P_C);
        if(getplen(obj, ROM_P_C) > 0 && cp[0] == 'P') {
            set_fix_val(fix_spread, 608, "OP", 2);
        } else {
            set_fix_val(fix_spread, 608, "OC", 2);
        }
    } else {
        set_fix_val(fix_spread, 608, "ES", 2);
    }
}
static void set_leg_mat_date(trans_t* t, dart_order_obj* obj, ofp* fix_spread)
{
    char* matmy = getpval(obj, ROM_EXPIRE_DATE);
    char* mday = getpval(obj, ROM_EXPIRE_DAY);
    if(getplen(obj, ROM_EXPIRE_DATE) > 0 && getplen(obj, ROM_EXPIRE_DAY) > 0) {
        char lmd[8];
        memcpy(lmd, matmy, 6);
        memcpy(lmd + 6, mday, 2);
        set_fix_val(fix_spread, 611, lmd, 8);
    }
}
static void set_leg_strike(trans_t* t, dart_order_obj* obj, ofp* fix_spread)
{
    if(getplen(obj, ROM_STRIKE) > 0) {
        set_fix_val(fix_spread, 612, getpval(obj, ROM_STRIKE),
                    getplen(obj, ROM_STRIKE));
    }
}
static void set_leg_pos_effect(trans_t* t, dart_order_obj* obj, ofp* fix_spread)
{
    char* sectype = getpval(obj, ROM_SEC_TYPE);
    if(getplen(obj, ROM_SEC_TYPE) > 0 && getplen(obj, ROM_O_C) > 0 
               && sectype[0] == 'O') {
        char* oc = getpval(obj, ROM_O_C);
        if(oc[0] == '1') {
            set_fix_val(fix_spread, 564, "O", 1);
        } else {
            set_fix_val(fix_spread, 564, "C", 1);
        }
    }
}

void apply_spread_fields(trans_t* t, dart_order_obj* obj, ofp* fix_spread)
{
    set_fix_val(fix_spread, 600, getpval(obj, ROM_SYM), getplen(obj, ROM_SYM));
    set_cfi_code(t, obj, fix_spread);
    set_leg_mat_date(t, obj, fix_spread);
    set_leg_strike(t, obj, fix_spread);
    set_leg_pos_effect(t, obj, fix_spread);
    set_fix_val(fix_spread, 624, getpval(obj, ROM_SIDE), getplen(obj, ROM_SIDE));
    set_fix_val(fix_spread, 623, getpval(obj, ROM_SHARES), getplen(obj, ROM_SHARES));
}

static int get_rom_num_for_tag(char* tag, int tlen)
{
    int ret = 0;
    if(tlen == 2) {
        if(tag[0] =='5' && tag[1] == '2') {
            printf("Found time!!!!!!\n");
            return ROM_TIME;
        } else if(tag[0] == '3' && tag [1] == '5') {
            printf("Found echo!!!!!!\n");
            return ROM_ECHO;
        } else if(tag[0] == '5' && tag [1] == '4') {
            printf("Found Side!!!!!!\n");
            return ROM_SIDE;
        } else if(tag[0] == '3' && tag [1] == '8') {
            printf("Found Shares!!!!!!\n");
            return ROM_SHARES;
        } else if(tag[0] == '4' && tag [1] == '4') {
            printf("Found price!!!!!!\n");
            return ROM_PRICE;
        } else if(tag[0] == '1' && tag [1] == '4') {
            printf("Found cum!!!!!!\n");
            return ROM_CUM;
        } else if(tag[0] == '5' && tag [1] == '5') {
            printf("Found Sym!!!!!!\n");
            return ROM_SYM;
        }
    }
    return ret;
}

static void re_parse_message(dart_order_obj* doj, char* raw_off, int len)
{
    char* tag = raw_off;
    char* val = raw_off;
    char* end = raw_off;
    char* bend = raw_off + len;
    do {
        end = memchr(tag, '=', bend - tag);
        if(end) {
            int rom_num = get_rom_num_for_tag(tag, end - tag);
            if(rom_num > 0) {
                val = end + 1;
                end = memchr(val, FIX_FIELD_DELIM, bend - val);
                if(end) {
                    reset_rom_field(doj, rom_num, val, end - val);
                }
            }
            ++end;
            tag = end;
        }
    } while(end);
}

void validate_offsets_client(dart_order_obj* doj, char* raw_off, int len)
{
    if(doj->positions[ROM_ECHO].iov_len <= 0 ||
            doj->positions[ROM_TIME].iov_len <= 0 ||
            doj->positions[ROM_SIDE].iov_len <= 0 ||
            doj->positions[ROM_SHARES].iov_len <=0 ||
            doj->positions[ROM_PRICE].iov_len <= 0)
    {
        printf("Missing Fields!!!!!!\n");
        re_parse_message(doj, raw_off, len);
    }
}

void validate_offsets(dart_order_obj* doj, char* raw_off, int len)
{
    if(doj->positions[ROM_ECHO].iov_len <= 0 ||
            doj->positions[ROM_TIME].iov_len <= 0 ||
            doj->positions[ROM_SIDE].iov_len <= 0 ||
            doj->positions[ROM_SHARES].iov_len <=0 ||
            doj->positions[ROM_PRICE].iov_len <= 0 ||
            doj->positions[ROM_CUM].iov_len <=0)
    {
        printf("Missing Fields!!!!!!\n");
        re_parse_message(doj, raw_off, len);
    }
}

void get_cp_len_off(dart_order_obj* doj, ofp* fix_obj)
{
    int len = 0;
    char* type_off = doj->positions[ROM_ECHO].iov_base;
    type_off -= 3;
    char* time_off = doj->positions[ROM_TIME].iov_base;
    time_off += doj->positions[ROM_TIME].iov_len + 1;
    len = doj->ex_mess - (time_off - type_off); 
    databuf_t* db = get_message(fix_obj); 
    databuf_memcpy(db, time_off, len);     
}

static void switch_ord_ids(dart_order_obj* doj)
{
    if(doj->positions[ROM_NEW_ID].iov_len > 0) {
        char temp[128];
        memset(temp, '\0', 128);
        memcpy(temp, doj->positions[ROM_NEW_ID].iov_base,
                doj->positions[ROM_NEW_ID].iov_len);
        int len = doj->positions[ROM_NEW_ID].iov_len;
        set_rom_field(doj, ROM_NEW_ID, doj->positions[ROM_CLORDID].iov_base,
                      doj->positions[ROM_CLORDID].iov_len);
        set_rom_field(doj, ROM_CLORDID, temp, len);
    }

}
void clean_text(dart_order_obj* doj, databuf_t* out_mess, 
        int rom_num, char* raw_text, int rlen)
{
    char* d_off = raw_text;
    char* comma = memchr(d_off, ',', rlen);
    if(comma == 0x0) {
        set_rom_field(doj, rom_num, raw_text, rlen);
        return;
    }
    while(comma && (comma - raw_text) < rlen) {
        databuf_memcpy(out_mess, d_off, (comma - d_off));
        databuf_memcpy(out_mess, " ", 1); 
        d_off = ++comma;
        comma = memchr(d_off, ',', rlen);
    }
    set_rom_field(doj, rom_num, out_mess->rd_ptr, databuf_unread(out_mess));
}

void set_defaults_for_dma(dart_order_obj* doj, ex_config* ex)
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
    char* def_trader = get_config_val(ex, "SERVER", 6, 
                              "Trader", 6, &ret_len);
    if(ret_len > 0) {
        set_rom_field(doj, ROM_TRADER, def_trader, ret_len);
        set_rom_field(doj, ROM_TRADE_FOR, def_trader, ret_len);
    }
    switch_ord_ids(doj);
}
