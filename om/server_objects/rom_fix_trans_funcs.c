/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rom_fix_trans_funcs.h"
#include "dart_constants.h"


void set_exec_time(trans_t* t, ofp* obj)
{
    time_t now;
    struct tm tmval;
    struct tm *tmm = &tmval;
    time(&now);
    if ((tmm = gmtime_r(&now, &tmval)) == 0) {
        perror("Calling gmtime_r");
    }
    set_time_tag(t, obj,&tmval, 60);
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
    } else if(val_len > 1) {
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
void remove_from_maps(con_interface * ci, char *id, int id_len)
{
    pthread_spin_lock(&ci->mutex);
    remove_from_map(ci->cl_ord_id_map, id, id_len);
    pthread_spin_unlock(&ci->mutex);
}
void fix_return(con_interface* a, dart_order_obj * doj)
{
    if (doj->sender != NULL) {
        ((con_interface *) doj->sender)->ci_iovec_sender(doj->positions,
            DART_SEND_LEN,
            doj->positions
            [ROM_SENDER].iov_base,
            doj->positions
            [ROM_SENDER].iov_len,
            ((con_interface *)
             doj->sender),
            ((con_interface *)
             doj->
             sender)->sock);
        if(((con_interface*)doj->sender)->do_oats_balance) {
            increment_ex_tag(doj);
            send_oats_message(doj);
        }
        record_client_outbound(doj);
    }
}
int get_obj_by_id(con_interface * ob, void *clordid, int len,
        dart_order_obj ** obj)
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
        con_interface* ob)
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
    if(rep->positions[ROM_STOP].iov_len > 0) {
        fill_in_rom_field(ute, ROM_STOP, rep->positions[ROM_STOP].iov_base,
                rep->positions[ROM_STOP].iov_len);
    }
    if(rep->positions[ROM_TIF].iov_len > 0) {
        fill_in_rom_field(ute, ROM_TIF, rep->positions[ROM_TIF].iov_base,
                rep->positions[ROM_TIF].iov_len);
    }
    pthread_spin_lock(&ob->mutex);
    remove_from_map(ob->cl_ord_id_map,
                    ute->positions[ROM_CLORDID].iov_base,
                    ute->positions[ROM_CLORDID].iov_len);
    fill_in_rom_field(ute, ROM_CLORDID, ute->positions[ROM_PENDING_ID].iov_base,
                   ute->positions[ROM_PENDING_ID].iov_len);
    no_copy_insert(ob->cl_ord_id_map, getpval(ute, ROM_CLORDID), getplen(ute, ROM_CLORDID),
                   ute);
    //destroy_gk_obj(ob->gk, rep);
    destroy_gk_db(ob->gk, itoa_buff);
    ute->rep = 0;
    pthread_spin_unlock(&ob->mutex);
}
void handle_cnl_rpl_rej(dart_order_obj* doj, con_interface* ci)
{
    if(is_spread(doj)) {
        if(doj->positions[ROM_STASH_SYM].iov_len > 0) {
            fill_in_rom_field(doj, ROM_SYM, getpval(doj, ROM_STASH_SYM),
                          getplen(doj, ROM_STASH_SYM));
        }
    }
    dart_order_obj *ute = 0;
    int found = get_obj_by_id(ci, doj->positions[FIX_CLORDID].iov_base,
            doj->positions[FIX_CLORDID].iov_len, &ute);
    if(found == 0) {
        found = get_obj_by_id(ci, doj->positions[FIX_ORIG_ID].iov_base,
                              doj->positions[FIX_ORIG_ID].iov_len, &ute);
    }
    if (found && lock_n_check(ute)) {
        if(getplen(doj, FIX_CNL_TYPE) > 0 && ((char*)getpval(doj,FIX_CNL_TYPE))[0] == '2') {
            set_rom_field(ute, ROM_STATUS, "20", 2);
        } else {
            set_rom_field(ute, ROM_STATUS, "14", 2);
        }
        set_rom_field(ute, 0, ROM_ZERO_STATUS, 1);
        if(getplen(doj, FIX_TEXT) > 0) {
            databuf_t *out_mess = get_databuf(ci->gk);
            clean_text(ute, out_mess, 
                        ROM_TEXT, getpval(doj, FIX_TEXT),
                        getplen(doj, FIX_TEXT));
            destroy_gk_db(ci->gk, out_mess);
        }
        if(is_spread(ute)) {
            set_rom_field(ute, ROM_SPREAD, "5", 1);
        }
        ci->on_reject(ci, ute);
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
            set_fix_val(t, fix_spread, 608, "OP", 2);
        } else {
            set_fix_val(t, fix_spread, 608, "OC", 2);
        }
    } else {
        set_fix_val(t, fix_spread, 608, "ES", 2);
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
        set_fix_val(t, fix_spread, 611, lmd, 8);
    }
}
static void set_leg_strike(trans_t* t, dart_order_obj* obj, ofp* fix_spread)
{
    if(getplen(obj, ROM_STRIKE) > 0) {
        set_fix_val(t, fix_spread, 612, getpval(obj, ROM_STRIKE),
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
            set_fix_val(t, fix_spread, 564, "O", 1);
        } else {
            set_fix_val(t, fix_spread, 564, "C", 1);
        }
    }
}

void apply_spread_fields(trans_t* t, dart_order_obj* obj, ofp* fix_spread)
{
    set_fix_val(t, fix_spread, 600, getpval(obj, ROM_SYM), getplen(obj, ROM_SYM));
    set_cfi_code(t, obj, fix_spread);
    set_leg_mat_date(t, obj, fix_spread);
    set_leg_strike(t, obj, fix_spread);
    set_leg_pos_effect(t, obj, fix_spread);
    set_fix_val(t, fix_spread, 624, getpval(obj, ROM_SIDE), getplen(obj, ROM_SIDE));
    set_fix_val(t, fix_spread, 623, getpval(obj, ROM_ORIG_SHARES), getplen(obj, ROM_ORIG_SHARES));
}

void build_children(con_interface* ci)
{
    pthread_spin_lock(&ci->mutex);
    hashmap_iterator_t hi = hashmap_iterator_begin(ci->cl_ord_id_map);
    dart_order_obj *current = 0;
    char *key = 0;
    int key_len = 0;
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                                     (void *) &current);
    while (valid && current) {
        if (lock_n_check(current)) {
            if(current->positions[ROM_OM_PARENT].iov_len > 0) {
                dart_order_obj* parent = 0;
                int found = get_map_entry(ci->cl_ord_id_map,
                              current->positions[ROM_OM_PARENT].iov_base,
                              current->positions[ROM_OM_PARENT].iov_len,
                              (void **) &parent);
                if(found) {
                    add_child_obj(parent, current);
                }
            }
            unlock(current);
        }
        valid = hashmap_iterator_next(ci->cl_ord_id_map, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &current);
    }
    pthread_spin_unlock(&ci->mutex);
    hashmap_iterator_destroy(hi);
}
int reset_fields_for_fill(dart_order_obj * orig,
        dart_order_obj * doj, con_interface* ob,
        void (*next_step) (struct connection_interface *,
                                           dart_order_obj * doj))
{
    fill_in_rom_field(orig, ROM_LAST_SHARES,
            doj->positions[FIX_LAST_SHARES].iov_base,
            doj->positions[FIX_LAST_SHARES].iov_len);
    fill_in_rom_field(orig, ROM_EXEC_PRICE,
            doj->positions[FIX_EXEC_PRICE].iov_base,
            doj->positions[FIX_EXEC_PRICE].iov_len);
    if(getplen(doj, FIX_EXEC_TAG) > 0) {
        fill_in_rom_field(orig, EXEC_ID,
                doj->positions[FIX_EXEC_TAG].iov_base,
                doj->positions[FIX_EXEC_TAG].iov_len);
    }
    if(getplen(doj, FIX_EXEC_TIME) > 0) {
        fill_in_rom_field(orig, EXEC_TIME,
                doj->positions[FIX_EXEC_TIME].iov_base,
                doj->positions[FIX_EXEC_TIME].iov_len);
    }
    if(getplen(doj, FIX_LIQ_FLAG) > 0) {
        fill_in_rom_field(orig, ROM_LIQ,
                doj->positions[FIX_LIQ_FLAG].iov_base,
                doj->positions[FIX_LIQ_FLAG].iov_len);
    }
    if(getplen(doj, FIX_CUM) > 0) {
        fill_in_rom_field(orig, ROM_CUM, getpval(doj, FIX_CUM),
                          getplen(doj, FIX_CUM));
    }
    if(getplen(doj, FIX_LEAVES) > 0) {
        fill_in_rom_field(orig, ROM_LEAVES, getpval(doj, FIX_LEAVES),
                          getplen(doj, FIX_LEAVES));
    }
    

    double d = strtod(doj->positions[FIX_EXEC_PRICE].iov_base, NULL);
    d = d * 10000;
    d += .0000001;
    long rblp = (long) d;
    fill_in_rom_field(orig, ROM_BIN_LAST_PRICE, &rblp, sizeof(long));
    int ls = strtol(doj->positions[FIX_LAST_SHARES].iov_base, NULL, 10);
    int qty = strtol(doj->positions[FIX_SHARES].iov_base, NULL, 10);
    fill_in_rom_field(orig, ROM_BIN_LAST_SHARES, &ls, sizeof(int));
    int cum = 0;
    int leaves = 0;
    if (orig->positions[ROM_BIN_CUM].iov_len > 0) {
        memcpy(&cum, orig->positions[ROM_BIN_CUM].iov_base, 4);
        cum += ls;
        fill_in_rom_field(orig, ROM_BIN_CUM, &cum, sizeof(int));

    } else {
        fill_in_rom_field(orig, ROM_BIN_CUM, &ls, sizeof(int));
        cum = ls;
    }
    leaves = qty - cum;
    databuf_t *itoa_buff = get_databuf(ob->gk);
    dart_itoa(orig, leaves, ROM_LEAVES, itoa_buff);
    dart_itoa(orig, cum, ROM_CUM, itoa_buff);
    /**
     * Here we see if we need to send parent back to ROM/users.
     */
    /*if(is_spread(orig)) {
        char* mark = getpval(orig, ROM_LEG_MARK);
        if(getplen(orig, ROM_LEG_MARK) > 0 && mark[0] == '1') {
            dart_order_obj* parent = 0;
            int found = get_obj_by_id(ob, doj->positions[FIX_CLORDID].iov_base,
                    doj->positions[FIX_CLORDID].iov_len, &parent);
            if(found == 0) {
                found = get_obj_by_id(ob, doj->positions[FIX_ORIG_ID].iov_base,
                                      doj->positions[FIX_ORIG_ID].iov_len, &parent);
            }
        }
    } */
    destroy_gk_db(ob->gk, itoa_buff);
    return leaves;
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
void send_leg_rej_updates(dart_order_obj* doj,
                                 con_interface* ci)
{
    if(doj->childs != 0) {
        children* lch = doj->childs;
        while(lch != NULL) {
            dart_order_obj* ldoj = (dart_order_obj*)lch->payload;
            set_rom_field(ldoj, 0, ROM_ZERO_STATUS, 1);
            set_rom_field(ldoj, ROM_STATUS, "8", 1);
            remove_from_maps(ci, getpval(ldoj, ROM_CLORDID),
                    getplen(ldoj, ROM_CLORDID)); 
            ci->on_reject(ci, ldoj);
            lch = lch->next;
        }
    } 
}
void set_spread_rep_fields(dart_order_obj* orig, 
                                  dart_order_obj* fix,
                                  con_interface* ci)
{
    char *off = getpval(fix, FIX_SHARES);
    int slen = getplen(fix, FIX_SHARES);
    char* price = getpval(fix, FIX_PRICE);
    int plen = getplen(fix, FIX_PRICE);
    if(slen >= 1 && off[0] != ',') {
        fill_in_rom_field(orig, ROM_SHARES, off,slen);
                
    }
    if(plen >= 1 && price[0] != ',') {
        fill_in_rom_field(orig, ROM_PRICE, price,plen);
    }
    if(getplen(fix, FIX_CUM) > 0) {
        fill_in_rom_field(orig, ROM_CUM, getpval(fix, FIX_CUM),
                          getplen(fix, FIX_CUM));
    }
    if(getplen(fix, FIX_LEAVES) > 0) {
        fill_in_rom_field(orig, ROM_LEAVES, getpval(fix, FIX_LEAVES),
                          getplen(fix, FIX_LEAVES));
    }
    if(getplen(fix, FIX_LAST_SHARES) > 0) {
        fill_in_rom_field(orig, ROM_LAST_SHARES,
                fix->positions[FIX_LAST_SHARES].iov_base,
                fix->positions[FIX_LAST_SHARES].iov_len);
    } else {
        fill_in_rom_field(orig, ROM_LAST_SHARES,"0", 1);
    }
    if(getplen(fix, FIX_EXEC_PRICE) > 0) {
        fill_in_rom_field(orig, ROM_EXEC_PRICE,
                fix->positions[FIX_EXEC_PRICE].iov_base,
                fix->positions[FIX_EXEC_PRICE].iov_len);
    } else {
        fill_in_rom_field(orig, ROM_EXEC_PRICE,"0", 1);
    }
}
