/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include "nyse_order_tracker.h"
#include "nyse_seq_generator.h"
#include "hashmap.h"
#include "dart_constants.h"
#include "parser_func.h"
#include "mailer.h"
#include "ex_common.h"
#include "dart_order_allocator.h"
#include "rom2fix.h"
#include "fix_rom_trans.h"
#include "fix_handler.h"
#include "fixmsg.h"

#define CLORDID_LOC 15
static char *on_behalf_comp_id;
static int on_behalf_comp_id_len = 0;
static char *dquote_route;
static int dquote_route_len = 0;
static char *dquote_mne;
static int dquote_mne_len = 0;
static char *equote_mne;
static int equote_mne_len = 0;
static char *dquote_clear;
static int dquote_clear_len = 0;
static hashmap_t dquote_routes;
static hashmap_t clearing_ids;
static hashmap_t clearing_accounts;
static hashmap_t dquote_mne_map;
static hashmap_t dquote_clr_map;

int nyse_instruction_check(char *rom, romindex_t * map, char *type,
                databuf_t * fix, size_t field, int *rejected,
                dart_ex_order_obj * order);

int nyse_reduction_check(char *rommsg, struct dart_fix_pos *positions,
                          char *mtype, databuf_t * output,
                          size_t i, int *rejected,
                          dart_ex_order_obj * order);
static void nyse_add_to_map(hashmap_t map, char *buffer, long len)
{
    char *off;
    long c_offset = 0;
    off = strchr(buffer, ',');
    if (off) {
        char *tag;
        char* val;
        char *val_off;
        c_offset = off - buffer;
        val = calloc(10,1);
        tag = calloc(c_offset + 1, 1);
        memcpy(tag, buffer, c_offset);

        val_off = buffer;
        val_off += c_offset + 1;
        int val_len = len - (c_offset + 1);
        memcpy(val, val_off, val_len); 
        insert_map(map, tag, c_offset, val, val_len);
        free(val);
        free(tag);
    } else {
        printf("Could not find a comma in this mess: %s \n", buffer);
    }
}

int build_clear_id_map(const char *filename, hashmap_t map)
{
    char *buffer;
    long size;
    size = 0;
    buffer = NULL;
    buffer = get_file_contents(filename, &size);
    if (size > 0) {
        long cur_pos = 0;
        char eol[2] = { '\n', '\0' };
        long end_pos = 0;
        int should_parse = 1;
        char *d_off = buffer;
        while (cur_pos < size && should_parse) {
            end_pos = find_offset(d_off, (size - cur_pos), eol, 1);
            if (end_pos > 0) {
                nyse_add_to_map(map, d_off, end_pos);
                d_off += end_pos + 1;
                cur_pos += end_pos + 1;
            } else {
                should_parse = 0;
            }
        }
        free(buffer);
    } else {
        return 1;
    }
    return 0;
}

hashmap_t load_clearing_ids(const char* filename)
{
    hashmap_t h = create_map(5);
    build_clear_id_map(filename, h);
    return h;
}

hashmap_t load_dquote_mne_clr(const char* filename)
{
    hashmap_t h = create_map(5);
    build_clear_id_map(filename, h);
    return h;
}
hashmap_t load_dquote_clr_clr(const char* filename)
{
    hashmap_t h = create_map(5);
    build_clear_id_map(filename, h);
    return h;
}
hashmap_t load_dquote_routes(const char* filename)
{
    hashmap_t h = create_map(5);
    build_clear_id_map(filename, h);
    return h;
}
int nyse_handle_ordids(char* fix_mess, dart_rom_pos* positions,
                       char mtype, char *status,databuf_t * out_mess,
                       size_t romfield, int* rejected, dart_ex_order_obj * obj, 
                       pthread_mutex_t * map_mutex,
                       hashmap_t cl_ord_id_map)
{
    char *beg_off = 0;
    int len = 0;
    int ret_len = 0;
    size_t doa_len = 0;
    if (strncmp("4", status, 1) == 0 ||
        strncmp("6", status, 1) == 0 || strncmp("5", status, 1) == 0) {
        beg_off = (char *) fix_mess;
        beg_off += positions[CLORDID_LOC].beg;
        len = positions[CLORDID_LOC].end - positions[CLORDID_LOC].beg;
        int pending_cnl = doa_get_pending_cancel(obj); 
        if('5' == status[0] && pending_cnl) {
            doa_set_pending_cancel(obj, 0);
            char *cur_cl_ord_id = doa_get_cl_ord_id(obj, &doa_len);
            databuf_memcpy(out_mess, cur_cl_ord_id, doa_len);
            free(cur_cl_ord_id);
        } else {
            databuf_memcpy(out_mess, beg_off, len);
            ret_len = len;     
            if (strncmp("5", status, 1) == 0) {
                char *cur_cl_ord_id = doa_get_cl_ord_id(obj, &doa_len);
                beg_off = (char *) fix_mess;
                beg_off += positions[CLORDID_LOC].beg;
                len = positions[CLORDID_LOC].end - positions[CLORDID_LOC].beg;

                if (strncmp(beg_off, cur_cl_ord_id, len) != 0) {
                    pthread_mutex_lock(map_mutex);
                    remove_from_map(cl_ord_id_map, cur_cl_ord_id, doa_len);
                    pthread_mutex_unlock(map_mutex);
                    doa_set_cl_ord_id(obj, beg_off, len);
                }
                free(cur_cl_ord_id);
            } 
        }
    } else {
        beg_off = (char *) fix_mess;
        beg_off += positions[CLORDID_LOC].beg;
        len = positions[CLORDID_LOC].end - positions[CLORDID_LOC].beg;
        databuf_memcpy(out_mess, beg_off, len);
        ret_len = len;
    }
    return ret_len;
}
static const struct dart_fix_pos df_initializer = { 0, 0, 0, 0 };
static void field_pos_init(struct dart_fix_pos dfpos[ROM_LEN],
                           struct fix_tag_def *ftdefs[ROM_LEN])
{
    size_t i;
    for (i = 0; i < ROM_LEN; ++i) {
        dfpos[i] = df_initializer;
        if (ftdefs[i] != 0) {
            dfpos[i].name = ftdefs[i]->name;
            dfpos[i].len = ftdefs[i]->len;
        }
    }
}
long nyse_trans_rom_message(struct message *mess, char *rom_message,
                       long rom_len, databuf_t * output,
                       char *ret_fix_type, int *ft_len, int *rejected,
                       orderbook * ob,
                       long (*rom_to_fix_override) (struct message * mess,
                                                    char *fix_message,
                                                    databuf_t * output,
                                                    long fix_len,
                                                    char *fix_type,
                                                    struct dart_fix_pos
                                                    positions[ROM_LEN],
                                                    int *rejected,
                                                    orderbook * b, int is_spread))
{

    long ret_val;
    struct dart_fix_pos l_positions[ROM_LEN];
    char fix_type;
    ret_val = 0;

        fix_type = rom_message[0];
        switch (fix_type) {
        case 'E':
            fix_type = 'D';
            field_pos_init(l_positions, ob->rom_to_fix_out);
            build_rom_pos2(rom_message, rom_len, ob->rom_to_fix_out, l_positions, 0);
            ret_val =  rom_to_fix_override(mess, rom_message, output, rom_len,
                               &fix_type, l_positions, rejected, ob, 0);
            break;
        case 'R':
            fix_type = 'G';
            field_pos_init(l_positions, ob->rom_to_fix_rpl);
            build_rom_pos2(rom_message, rom_len, ob->rom_to_fix_rpl, l_positions, 0);
            
            int len = l_positions[25].end - l_positions[25].beg;
            if (len > 0) {
                fix_type = 'F';
            }
            ret_val =  rom_to_fix_override(mess, rom_message, output, rom_len,
                               &fix_type, l_positions, rejected, ob, 0);
            break;
        case 'C':
            fix_type = 'F';
            field_pos_init(l_positions, ob->rom_to_fix_cnl);
            build_rom_pos2(rom_message, rom_len, ob->rom_to_fix_cnl, l_positions, 0);
            ret_val =  rom_to_fix_override(mess, rom_message, output, rom_len,
                               &fix_type, l_positions, rejected, ob, 0);
            break;
        default:
            *rejected = 1;
            break;
        }
    ret_fix_type[0] = fix_type;
    ret_fix_type[1] = '\0';
    *ft_len = 1;
    return ret_val;
}
/**
 * This functions tests the callback into the main application.
 */
void nyse_from_rom_func(struct message *mess, void *b)
{
    databuf_t *output = databuf_alloc(mess->len * 8);
    orderbook *ob = (orderbook *) b;
    long total_len;
    struct message m1;
    char fix_type[3];
    int ft_len = 0;
    int rejected = 0;
    total_len = 0;

    dart_rolling_log_coming(mess, ob->module_name, ob->module_name_len);

    if (ob->is_connected()) {
        m1.name = mess->name;
        m1.name_len = mess->name_len;
        memset(fix_type, 0, 3);
        total_len =
            nyse_trans_rom_message(&m1, mess->data, mess->len, output,
                              fix_type, &ft_len, &rejected,
                              (orderbook *) ob, &rom2fix_translate_fields);
        if (rejected) {
            m1.data = output->buffer;
            m1.len = databuf_unread(output);
            send_message_to_roms(&m1, ob);
        } else if (total_len > 0) {
            m1.name = ob->fix_con_name;
            m1.name_len = ob->fix_con_len;
            m1.data = output->buffer;
            m1.len = databuf_unread(output);
            send_fix_message(&m1, fix_type, ft_len);
        }
    } else {
        char *l_message = "Not connected to Exchange.";
        rejected = 1;
        m1.name = mess->name;
        m1.name_len = mess->name_len;
        total_len = create_rom_reject(mess->data, output, l_message);
        if (total_len > 0) {
            m1.data = output->buffer;
            m1.len = databuf_unread(output);
            send_message_to_roms(&m1, ob);
        } else {
            databuf_destroy(output);
            return;
        }
    }
    databuf_destroy(output);
}
static long nyse_set_status_for_cnrej(char *fix_message,
                          struct dart_rom_pos positions[],
                          char *status, databuf_t * output,
                          dart_ex_order_obj* obj)
{
    long len = 0;
    char *cc_off = NULL;
    status = fix_message;
    status += positions[14].beg;
    cc_off = fix_message;
    if (positions[63].beg != positions[63].end) {
        cc_off += positions[63].beg;
        if (cc_off[0] == '1') {
            if(doa_get_pending_cancel(obj)) {
                databuf_memcpy(output, "20", 2);
                doa_set_pending_cancel(obj, 0);
            } else {
                databuf_memcpy(output, "14", 2);
            }
            len = 2;
        } else {
            doa_set_pending_replace(obj, 0);
            databuf_memcpy(output, "20", 2);
            len = 2;
        }
    }
    return len;
}
static long nyse_translate_status(char *value, databuf_t * output, long value_len)
{
    long len = value_len;
    if (strncmp("C", value, 1) == 0) {
        databuf_memcpy(output, "12", 2);
        len = 2;
    } else if (strncmp("13", value, 2) == 0) {
        databuf_memcpy(output, "0", 1);
        len = 1;
    } else if (strncmp("0", value, 1) == 0) {
        databuf_memcpy(output, "13", 2);
        len = 2;
    } else if (strncmp("12", value, 2) == 0) {
        databuf_memcpy(output, "C", 1);
        len = 1;
    } else if (strncmp("21", value, 2) == 0) {
        databuf_memcpy(output, "E", 1);
        len = 1;
    } else if (strncmp("E", value, 1) == 0) {
        databuf_memcpy(output, "21", 2);
        len = 2;
    } else if(len == 1 && strncmp("3", value, 1) == 0) {
        databuf_memcpy(output, "4", 1);
    } else {
        databuf_memcpy(output, value, value_len);
    }
    return len;
}
int nyse_fixrom_status_xlate(char *fixmsg, dart_rom_pos * positions,
                            char mtype, char *status,
                            databuf_t * rom, size_t romfield,
                            int *rejtected, dart_ex_order_obj * order,
                            pthread_mutex_t * mutex, hashmap_t ord_map)
{
    char *beg_off = fixmsg;
    size_t len = (positions[romfield].end - positions[romfield].beg);
    beg_off += positions[romfield].beg;
    if (romfield == 14 && mtype != 'U') {
        len = nyse_translate_status(beg_off, rom, len);
        status = fixmsg;
        status += positions[romfield].beg;
    } else if (romfield == 14 && mtype == 'U') {
        len = nyse_set_status_for_cnrej(fixmsg, positions, status, rom, order);
    }
    return 1;
}
void init_nyse_tracker(char *obo_comp_id, int obo_len,
                       struct database_conf *dbconf, const char *destID,
                       int d_len, orderbook * ob, struct init_args* in)
{
    queue_t q = create_queue();
    on_behalf_comp_id = calloc(obo_len + 1, 1);
    memcpy(on_behalf_comp_id, obo_comp_id, obo_len);
    on_behalf_comp_id_len = obo_len;
    clearing_ids = load_clearing_ids("IDtomnemonic.map");
    clearing_accounts = load_clearing_ids("GSFlip.map");
    dquote_mne_map = load_dquote_mne_clr("dquote_mne.map");
    dquote_clr_map = load_dquote_clr_clr("dquote_clr.map");
    dquote_routes = load_dquote_routes("dquote_routes.map");
    int ret_len = 0;
    dquote_route =
        get_val_for_tag(in->dc, "FIX SERVER", 10, "DQUOTE ROUTE",
                        12, &ret_len);
    dquote_route_len = ret_len;
    dquote_mne =
        get_val_for_tag(in->dc, "FIX SERVER", 10, "DQUOTE MNE",
                        10, &ret_len);
    dquote_mne_len = ret_len;
    equote_mne =
        get_val_for_tag(in->dc, "FIX SERVER", 10, "EQUOTE MNE",
                        10, &ret_len);
    equote_mne_len = ret_len;
    if(equote_mne_len <=0) {
        equote_mne = "DTR";
        equote_mne_len = 3;
    }
    dquote_clear =
        get_val_for_tag(in->dc, "FIX SERVER", 10, "DQUOTE CLEAR",
                        12, &ret_len);
    dquote_clear_len = ret_len;
    ob->rom2fix[4] = nyse_side_override;
    ob->rom2fix[5] = nyse_symbol_override;
    ob->rom2fix[12] = nyse_account_override;
    ob->rom2fix[57] = def_trans_execinst;
    ob->rom2fix[6] = nyse_reduction_check;
    ob->rom2fix[8] = set_nyse_type_param;
    ob->rom2fix[10] = set_nyse_capacity;
    ob->rom2fix[50] = set_transacttime;
    ob->rom2fix[17] = set_nyse_exTag;
    ob->rom2fix[15] = nyse_instruction_check;
    ob->rom2fix[63] = set_nyse_display;
    ob->fix2rom[63] = def_no_op;
    ob->fix2rom[14] = nyse_fixrom_status_xlate;
    ob->fix2rom[15] = nyse_handle_ordids;
    ob->fix2rom[26] = def_map_fix_text_to_rom;
    ob->fix2rom[17] = def_add_rom_tag;
    database_init(dbconf, destID, q, ob->cl_ord_id_map, ob->rom_ord_id_map,
                  ob);
}

int set_nyse_exTag(char *mess, struct dart_fix_pos *positions, char *mtype,
                   databuf_t * output, size_t i,
                   int *rejected, dart_ex_order_obj * ob)
{
    if (mtype[0] == 'D') {
    } else {
        size_t cl_ord_id_len = 0;
        char *cl_ord_id = doa_get_cl_ord_id(ob, &cl_ord_id_len);
        databuf_write(output, "%c37=%s", FIX_FIELD_DELIM, cl_ord_id);
        free(cl_ord_id);
    }
    return 1;
}

int set_nyse_display(char *rommsg, struct dart_fix_pos *positions,
                     char *mtype, databuf_t * output,
                     size_t i, int *rejected, dart_ex_order_obj * order)
{
    if (positions[i].beg == positions[i].end) {
        //do nothing
    } else {
        char *beg_off = rommsg;
        beg_off += positions[i].beg;
        if (beg_off[0] == 'I') {
            databuf_write(output, "%c9487=CO", FIX_FIELD_DELIM);
        } else if(beg_off[0] == 'S') {
            databuf_write(output, "%c9487=SOC", FIX_FIELD_DELIM);
        }
    }
    return 1;
}

int nyse_side_override(char *rommsg, struct dart_fix_pos *positions,
                       char *mtype, databuf_t * output,
                       size_t i, int *rejected, dart_ex_order_obj * order)
{
    if (positions[i].beg == positions[i].end) {
        size_t side_len = 0;
        char *side = doa_get_side(order, &side_len);
        databuf_write(output, "%c54=%s", FIX_FIELD_DELIM, side);
        free(side);
    } else {
        set_rom_val_in_fix(output, rommsg, positions, i,
                           mtype, order, set_val_in_order_struct);
    }
    return 1;
}


int nyse_symbol_override(char *rommsg, struct dart_fix_pos *positions,
                         char *mtype, databuf_t * output,
                         size_t i, int *rejected,
                         dart_ex_order_obj * order)
{
    if (positions[i].beg == positions[i].end) {
        size_t sym_len = 0;
        char *symbol = doa_get_symbol(order, &sym_len);
        translate_rom_symbol(output, symbol, sym_len);
        free(symbol);
    } else {
        char *beg_off = rommsg;
        size_t len = (positions[5].end - positions[5].beg);
        beg_off += positions[5].beg;
        translate_rom_symbol(output, beg_off, len);
        doa_set_symbol(order, beg_off, len);
    }
    return 1;
}
int nyse_reduction_check(char *rommsg, struct dart_fix_pos *positions,
                          char *mtype, databuf_t * output,
                          size_t i, int *rejected,
                          dart_ex_order_obj * order)
{
    int len = positions[25].end - positions[25].beg;
    if (len > 0 && mtype[0] == 'F') {
        doa_set_pending_cancel(order, 1);
        char* cxlqty = rommsg + positions[25].beg;
        char* new_qty = rommsg + positions[i].beg;
        int n_len = positions[i].end - positions[i].beg;
        unsigned int qty = (unsigned int)doa_get_order_qty(order);
        fixmsg_adduint(output, "38",2, qty);
        fixmsg_addstr_bylen(output, "9428",
                       cxlqty,len);
        fixmsg_addstr_bylen(output, "9429", new_qty, n_len);
    } else {
        return def_rom2fix(rommsg, positions, mtype,
                        output, i, rejected,order);
    }
    return 1;
}

/*
*/
int nyse_account_override(char *rommsg, struct dart_fix_pos *positions,
                          char *mtype, databuf_t * output,
                          size_t i, int *rejected,
                          dart_ex_order_obj * order)
{

    if (positions[i].beg == positions[i].end) {
        size_t acc_len = 0;
        char *account = doa_get_account(order, &acc_len);
        databuf_write(output, "%c1=%s", FIX_FIELD_DELIM, account);
        free(account);
    } else {
        char *p = rommsg + positions[i].beg;
        size_t len = positions[i].end - positions[i].beg;
        char* rom_val= 0;
        int found =
            find_n_fill(clearing_accounts, p, len, (void *) &rom_val);
        if(found) {
            len = found;
            p = rom_val;
        }
        doa_set_account(order, p, len);
        fixmsg_addstr_bylen(output, "1", p, len);
    }
    return 1;
}
int nyse_clr_id(char *rom, romindex_t * map, char *type,
                databuf_t * fix, size_t field, int *rejected,
                dart_ex_order_obj * order)
{
    int result = 0;
    char* p = 0;
    size_t len = map[field].end - map[field].beg;
    if (len > 0) {
        p = rom + map[field].beg;
        set_val_in_order_struct(p, len, field, order, type);
    } else {
        p = doa_get_clr_id(order, &len);
        if(len > 0) {
            char* rom_val= 0;
            int found =
                find_n_fill(clearing_ids, p, len, (void *) &rom_val);
            if(found) {
                databuf_write(fix, "%c115=", FIX_FIELD_DELIM);
                databuf_memcpy(fix, rom_val, found);
            } else {
                databuf_write(fix, "%c115=%s", FIX_FIELD_DELIM, on_behalf_comp_id);
            }
        }
    }
    return result;
}
int nyse_instruction_check(char *rom, romindex_t * map, char *type,
                databuf_t * fix, size_t field, int *rejected,
                dart_ex_order_obj * order)
{
    int result = 1;
    char* dq_off = rom;
    dq_off += map[field].beg;
    size_t len = map[field].end - map[field].beg;
    if(len == 2 && strncmp(dq_off, "DQ", 2) == 0 && dquote_mne_len <= 0) {
        result = -1;
    }
    return result;
}

int set_nyse_defaults(char *rommsg, struct dart_fix_pos *positions,
                      char *mtype, databuf_t * output,
                      size_t romfield, int *rejected,
                      dart_ex_order_obj * order)
{
    char* dq_off = rommsg;
    dq_off += positions[ROM_INSTR].beg;
    int len = positions[ROM_INSTR].end - positions[ROM_INSTR].beg;
    char* clear_id = rommsg + positions[ROM_CLR_ID].beg;
    size_t clear_id_len = positions[ROM_CLR_ID].end - positions[ROM_CLR_ID].beg;
    char* rom_val = rommsg + positions[ROM_CLR_ACC].beg;
    size_t rom_val_len = positions[ROM_CLR_ACC].end - positions[ROM_CLR_ACC].beg;
    if(rom_val_len <= 0) {
        rom_val = doa_get_account(order, &rom_val_len);
    }
    char* route = rommsg + positions[13].beg;
    int rlen = positions[13].end - positions[13].beg;
    if(rlen > 0) {
        size_t glen = (size_t)rlen;
       doa_set_destination(order, route, glen); 
    } else {
        size_t glen = 0;
       route = doa_get_destination(order, &glen);
      rlen = glen; 
    }
    char* fb_name = 0;
    int fb_nlen = 0;
    if(rlen > 0) {
            fb_nlen =
                find_n_fill(dquote_routes, route, rlen, (void *) &fb_name);
            if(fb_nlen) {
                dq_off = "DQ";
                len = 2;
            }
    }
    if(len == 2 && strncmp(dq_off, "DQ", 2) == 0 && dquote_mne_len > 0) {
        char* ldquote_mne = 0;
            int found =
                find_n_fill(dquote_mne_map, rom_val, rom_val_len, (void *) &ldquote_mne);
            if(found) {
                char lld[10];
                memset(lld, '\0', 10);
                memcpy(lld, ldquote_mne, found);
                databuf_write(output, "%c57=4", FIX_FIELD_DELIM);
                databuf_write(output, "%c115=%s", FIX_FIELD_DELIM, lld);
                doa_set_clr_id(order, lld,found);
                if(fb_nlen <= 0) {
                databuf_write(output, "%c143=R2", FIX_FIELD_DELIM);
                } else {
                databuf_write(output, "%c143=", FIX_FIELD_DELIM);
                databuf_memcpy(output, fb_name, fb_nlen);
                }
                char* ldquote_clear = 0;
                found = 
                    find_n_fill(dquote_clr_map, rom_val, rom_val_len, (void *) &ldquote_clear);
                if(found) {
                memset(lld, '\0', 10);
                memcpy(lld, ldquote_clear, found);
                    databuf_write(output, "%c439=%s", FIX_FIELD_DELIM, lld);
                    doa_set_cmta(order, lld, found);
                }
            } else {
                databuf_write(output, "%c57=4", FIX_FIELD_DELIM);
                databuf_write(output, "%c115=%s", FIX_FIELD_DELIM, dquote_mne);
                doa_set_clr_id(order, dquote_mne,dquote_mne_len);
                if(fb_nlen <= 0) {
                    databuf_write(output, "%c143=R2", FIX_FIELD_DELIM);
                } else {
                    databuf_write(output, "%c143=", FIX_FIELD_DELIM);
                    databuf_memcpy(output, fb_name, fb_nlen);
                }
                databuf_write(output, "%c439=%s", FIX_FIELD_DELIM, dquote_clear);
                doa_set_cmta(order, dquote_clear, dquote_clear_len);
            }
    } else if(dquote_route_len > 0 && dquote_route_len == rlen &&
              strncmp(route, dquote_route, rlen) == 0) {
        char* ldquote_mne = 0;
            int found =
                find_n_fill(dquote_mne_map, rom_val, rom_val_len, (void *) &ldquote_mne);
            if(found) {
                char lld[10];
                memset(lld, '\0', 10);
                memcpy(lld, ldquote_mne, found);
                databuf_write(output, "%c57=4", FIX_FIELD_DELIM);
                databuf_write(output, "%c115=%s", FIX_FIELD_DELIM, lld);
                doa_set_clr_id(order, lld,found);
                if(fb_nlen <= 0) {
                    databuf_write(output, "%c143=R2", FIX_FIELD_DELIM);
                } else {
                    databuf_write(output, "%c143=", FIX_FIELD_DELIM);
                    databuf_memcpy(output, fb_name, fb_nlen);
                }
                char* ldquote_clear = 0;
                found = 
                    find_n_fill(dquote_clr_map, rom_val, rom_val_len, (void *) &ldquote_clear);
                if(found) {
                    memset(lld, '\0', 10);
                    memcpy(lld, ldquote_clear, found);
                    databuf_write(output, "%c439=%s", FIX_FIELD_DELIM, lld);
                    doa_set_cmta(order, lld, found);
                }
            } else {
                databuf_write(output, "%c57=4", FIX_FIELD_DELIM);
                databuf_write(output, "%c115=%s", FIX_FIELD_DELIM, dquote_mne);
                doa_set_clr_id(order, dquote_mne,dquote_mne_len);
                if(fb_nlen <= 0) {
                    databuf_write(output, "%c143=R2", FIX_FIELD_DELIM);
                } else {
                    databuf_write(output, "%c143=", FIX_FIELD_DELIM);
                    databuf_memcpy(output, fb_name, fb_nlen);
                }
                databuf_write(output, "%c439=%s", FIX_FIELD_DELIM, dquote_clear);
                doa_set_cmta(order, dquote_clear, dquote_clear_len);
            }
    } else if(len == 2 && strncmp(dq_off, "EQ", 2) == 0 && equote_mne_len > 0) {
        databuf_write(output, "%c57=4", FIX_FIELD_DELIM);
        databuf_write(output, "%c115=%s", FIX_FIELD_DELIM, equote_mne);
        doa_set_clr_id(order, equote_mne,equote_mne_len);
        databuf_write(output, "%c143=R5", FIX_FIELD_DELIM);
        databuf_write(output, "%c439=%s", FIX_FIELD_DELIM, dquote_clear);
        doa_set_cmta(order, dquote_clear, dquote_clear_len);
    } else if(clear_id_len > 0) {
        char* rom_val= 0;
        int found =
            find_n_fill(clearing_ids, clear_id, clear_id_len, (void *) &rom_val);
        if(found) {
            databuf_write(output, "%c115=", FIX_FIELD_DELIM);
            databuf_memcpy(output, rom_val, found);
            set_val_in_order_struct(rom_val, found, 11, order, mtype);
        } else {
            databuf_write(output, "%c115=%s", FIX_FIELD_DELIM, on_behalf_comp_id);
            set_val_in_order_struct(on_behalf_comp_id, on_behalf_comp_id_len, 11, order, mtype);
        }
    } else if(mtype[0] != 'E' && order != NULL) {
        clear_id = doa_get_clr_id(order, &clear_id_len);
        if(clear_id_len > 0) {
            databuf_write(output, "%c115=%s", FIX_FIELD_DELIM, clear_id);
            if(dquote_mne_len > 0 && strncmp(clear_id, dquote_mne, clear_id_len) == 0) {
                databuf_write(output, "%c57=4", FIX_FIELD_DELIM);
                if(fb_nlen <= 0) {
                    databuf_write(output, "%c143=R2", FIX_FIELD_DELIM);
                } else {
                    databuf_write(output, "%c143=", FIX_FIELD_DELIM);
                    databuf_memcpy(output, fb_name, fb_nlen);
                }
                size_t cmta_len = 0;
                char* cmta = doa_get_cmta(order, &cmta_len);
                if(len > 0) {
                    databuf_write(output, "%c439=%s", FIX_FIELD_DELIM, cmta);
                }
            }
        }
        
    }
    databuf_write(output, "%c20=0", FIX_FIELD_DELIM);
    databuf_write(output, "%c21=1", FIX_FIELD_DELIM);
    databuf_write(output, "%c207=N", FIX_FIELD_DELIM);
    databuf_write(output, "%c167=CS", FIX_FIELD_DELIM);
    return 1;
}

int set_nyse_capacity(char *b_off,
                      struct dart_fix_pos positions[ROM_LEN],
                      char *mtype, databuf_t * d_off,
                      size_t romfield, int *rejected,
                      dart_ex_order_obj * obj)
{
    char *beg_off;
    long len;
    int s_len;
    int ret_len = 0;
    beg_off = b_off;
    beg_off += positions[10].beg;
    len = (positions[10].end - positions[10].beg);
    if (len > 0) {
        s_len =
            databuf_write(d_off, "%c%s=", FIX_FIELD_DELIM,
                          positions[10].name);
        ret_len += s_len;
        if (strncmp("A", beg_off, 1) == 0) {
            databuf_memcpy(d_off, "A", 1);
            len = 1;
            doa_set_capacity(obj, "A", 1);
        } else {
            databuf_memcpy(d_off, beg_off, len);
            doa_set_capacity(obj, beg_off, len);
        }
        ret_len += len;
    } else if (mtype[0] != 'D') {
        size_t cap_len = 0;
        char *l_cap = doa_get_capacity(obj, &cap_len);
        databuf_write(d_off, "%c47=%s", FIX_FIELD_DELIM, l_cap);
        free(l_cap);
    } else {
        databuf_write(d_off, "%c47=%s", FIX_FIELD_DELIM, "A");
    }
    return ret_len;
}

static void check_time_n_rej(char *beg_off, int len, char *l_fix_tag,
                             databuf_t * output, int *rejected,
                             char *mtype)
{
    struct tm *new_time;
    time_t long_time;
    int cur_hour = 0;
    int cur_min = 0;
    time(&long_time);
    struct tm lm;
    new_time = localtime_r(&long_time, &lm);
    cur_hour = lm.tm_hour;
    cur_min = lm.tm_min;
    //This works only in the eastern time zone:
    if (cur_hour > 15 || (cur_hour == 15 && cur_min >= 50)) {
        int rej_len = 0;
        char *l_message =
            "MOC and LOC orders on the NYSE cannot be replaced or cancelled or entered after 3:45 ET.";
        databuf_reset(output);
        if (mtype[0] == 'D') {
        } else {
            rej_len =
                build_cnl_rpl_rej(beg_off, len, l_fix_tag, output,
                                  l_message);
        }
        *rejected = 1;
    }

}

int set_nyse_type_param(char *b_off, struct dart_fix_pos *positions,
                        char *mtype, databuf_t * d_off,
                        size_t rom_off, int *rejected,
                        dart_ex_order_obj * obj)
{
    char *beg_off;
    char *disp_off = b_off;
    long len;
    int s_len;
    int ret_len = 0;
    beg_off = b_off;
    beg_off += positions[8].beg;
    len = (positions[8].end - positions[8].beg);
    int disp_len = positions[63].end - positions[63].beg;
    disp_off += positions[63].beg;
    if (len > 0) {
        s_len =
            databuf_write(d_off, "%c%s=", FIX_FIELD_DELIM,
                          positions[8].name);
        ret_len += s_len;
        if (strncmp("2", beg_off, 1) == 0) {
            databuf_memcpy(d_off, beg_off, len);
            doa_set_type(obj, beg_off, len);
        } else if (strncmp("11", beg_off, 2) == 0 && disp_len > 0
                   && strncmp(disp_off, "I", 1) == 0) {
            databuf_memcpy(d_off, "2", 1);
            len = 1;
            doa_set_type(obj, "2", 1);
        } else if (strncmp("11", beg_off, 2) == 0) {
            databuf_memcpy(d_off, "B", 1);
            len = 1;
            doa_set_type(obj, "B", 1);
            if (mtype[0] != 'D') {
                check_time_n_rej(beg_off, len, mtype, d_off, rejected, mtype);
            }
        } else if (strncmp("10", beg_off, 2) == 0) {
            databuf_memcpy(d_off, "A", 1);
            len = 1;
            doa_set_type(obj, "A", 1);
            if (mtype[0] != 'D') {
                check_time_n_rej(beg_off, len, mtype, d_off, rejected, mtype);
            }
        } else if (strncmp("5", beg_off, 1) == 0) {
            databuf_memcpy(d_off, beg_off, len);
            doa_set_type(obj, beg_off, len);
            if (mtype[0] != 'D') {
                check_time_n_rej(beg_off, len, mtype, d_off, rejected, mtype);
            }
        } else {
            databuf_memcpy(d_off, beg_off, len);
            doa_set_type(obj, beg_off, len);
        }
        ret_len += len;
    } else if (mtype[0] != 'D') {
        size_t cap_len = 0;
        char *l_cap = doa_get_type(obj, &cap_len);
        if (l_cap && (strncmp("B", l_cap, 1) == 0 ||
                      strncmp("A", l_cap, 1) == 0 ||
                      strncmp("5", l_cap, 1) == 0)) {
            check_time_n_rej(b_off, len, mtype, d_off, rejected, mtype);
        }
        free(l_cap);
    }
    return ret_len;
}
