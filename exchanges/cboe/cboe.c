/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

#include "client_manager.h"
#include "message_queue.h"
#include "parser_func.h"
#include "dart_constants.h"
#include "config_loader.h"
#include "ex_common.h"
#include "ex_config_helper.h"
#include "dart_orderbook.h"
#include "dart_order_allocator.h"
#include "rom_handler.h"
#include "rom2fix.h"
#include "mailer.h"
#include "fix_handler.h"
#include "fixmsg.h"
#include "version.h"
#include "cboe_seq_generator.h"
#include "complex_parser.h"
#include "options_common.h"
#include "debug_logger.h"
#define CLORDID_LOC 15
static pthread_mutex_t connection_lock = PTHREAD_MUTEX_INITIALIZER;
static char *cboe_cum_str = 0;

static char *fix_client_name;
static int fix_client_len;
static char *sender_sub_id = 0;
static char *target_sub_id = 0;
static int target_sub_len = 0;
static char *username = 0;
static int user_len = 0;
static char *module_name = NULL;        // = databuf_alloc(100);
static int module_name_len = 0;
static char *exec_broker = 0;
static int exec_broker_len = 0;
static char *tradin_session = 0;
static int tradin_sess_len = 0;
static char *mm_dest = 0;
static int mm_dest_len = 0;
static int show_acc = 0;
static int is_dart = 0;

static int is_con_now = 0;
static int is_cboe_connected = 1;
void cboe_from_fix_func(struct message *mess, void *ob);
int cboe_trans_mm_account(char *rom, romindex_t * map, char *type,
                          databuf_t * fix, size_t field, int *rejected,
                          dart_ex_order_obj * order);
int cboe_trans_capacity(char *rom, romindex_t * map, char *type,
                        databuf_t * fix, size_t field, int *rejected,
                        dart_ex_order_obj * order);

int cboe_handle_ordids(char *fixmsg, dart_rom_pos * positions,
                       char mtype, char *status,
                       databuf_t * rom, size_t romfield,
                       int *rejtected, dart_ex_order_obj * order,
                       pthread_mutex_t * mutex, hashmap_t ord_map);

int rom_to_cboe_price(char *rommsg, dart_fix_pos * map,
                      char *mtype, databuf_t * fix,
                      size_t field, int *rejected,
                      dart_ex_order_obj * order)
{
    size_t len = map[field].end - map[field].beg;
    char *p = rommsg + map[field].beg;
    if (len > 0 && doa_get_is_user_defined(order)) {
        char m = p[0];
        if (m == '-') {
            fixmsg_addstr2(fix, map[field].name, map[field].len, p + 1,
                           len - 1);
            set_val_in_order_struct(p + 1, len + 1, field, order, mtype);
        } else {
            char sub_p[100];
            memset(sub_p, '\0', 100);
            memcpy(sub_p, p, len);
            databuf_write(fix, "%c%s=-%s", FIX_FIELD_DELIM,
                          map[field].name, sub_p);
        }
    } else if (len > 0) {
        fixmsg_addstr2(fix, map[field].name, map[field].len, p, len);
    }
    return 1;
}

int rom_to_cboe_side(char *rommsg, dart_fix_pos * map,
                     char *mtype, databuf_t * fix,
                     size_t field, int *rejected,
                     dart_ex_order_obj * order)
{
    size_t len = map[field].end - map[field].beg;
    char *p = rommsg + map[field].beg;
    if (len > 0 && doa_get_is_user_defined(order)) {
        char m = p[0];
        if (m == '2') {
            fixmsg_addstr2(fix, map[field].name, map[field].len, "1", 1);
            set_val_in_order_struct("1", 1, field, order, mtype);
        } else {
            fixmsg_addstr2(fix, map[field].name, map[field].len, p, len);
        }
    } else if (len > 0) {
        fixmsg_addstr2(fix, map[field].name, map[field].len, p, len);
    } else {
        p = doa_get_side(order, &len);
        if (len > 0) {
            fixmsg_addstr2(fix, map[field].name, map[field].len, p, len);
        }
    }
    return 1;
}

int cboe_to_rom_last_price(char *fixmsg, dart_rom_pos * xlateinfo,
                      char mtype, char *status,
                      databuf_t * rom, size_t field,
                      int *rejtected,
                      dart_ex_order_obj * order,
                      pthread_mutex_t * mutex, hashmap_t map)
{
    size_t len = xlateinfo[field].end - xlateinfo[field].beg;
    size_t leg_len = xlateinfo[66].end - xlateinfo[66].beg;
    char *p = fixmsg + xlateinfo[field].beg;
    if (len > 0 && leg_len <= 0 && doa_get_is_user_defined(order)) {
        char m = p[0];
        if (m == '-') {
            databuf_memcpy(rom, p + 1, len - 1);
        } else {
            char sub_p[100];
            memset(sub_p, '\0', 100);
            memcpy(sub_p, p, len);
            databuf_write(rom, "-%s", sub_p);
        }
    } else if (len > 0) {
        databuf_memcpy(rom, p, len);
    }
    return 1;
}
char get_month_code(char* ex_month) {
    if(ex_month[0] == '1') {
        switch(ex_month[1]) {
            case '0': 
                return 'V';
            case '1':
                return 'X';
            case '2':
                return 'Z';
        };
    } else {
        switch(ex_month[1]) {
            case '1': 
                return 'F';
            case '2':
                return 'G';
            case '3':
                return 'H';
            case '4':
                return 'J';
            case '5':
                return 'K';
            case '6':
                return 'M';
            case '7':
                return 'N';
            case '8':
                return 'Q';
            case '9':
                return 'U';
        }
    }
    return 'R';
}
int cboe_handle_spread_sym(char *fixmsg, dart_rom_pos * xlateinfo,
                      char mtype, char *status,
                      databuf_t * rom, size_t field,
                      int *rejtected,
                      dart_ex_order_obj * order,
                      pthread_mutex_t * mutex, hashmap_t map)
{
    size_t leg_len = xlateinfo[66].end - xlateinfo[66].beg;
    char *p = fixmsg + xlateinfo[66].beg;
    char* sym = fixmsg + xlateinfo[5].beg;
    size_t sym_len = xlateinfo[5].end - xlateinfo[5].beg;
    if (leg_len > 0 && sym_len > 0 && doa_get_is_spread(order)
            && !doa_get_is_user_defined(order)) {
        if(p[0] == '3') {
            databuf_memcpy(rom, sym, sym_len);
        } else if(status[0] == '1' || status[0] == '2') {
            size_t exp_len = xlateinfo[30].end - xlateinfo[30].beg;
            char* expire = fixmsg + xlateinfo[30].beg;
            if(exp_len == 6) {
                char new_sym [16];
                memset(new_sym, '\0', 16);
                memcpy(new_sym, sym, sym_len);
                char mc = get_month_code(expire + 4);
                memcpy(new_sym + sym_len,&mc, 1); 
                memcpy(new_sym + sym_len + 1, expire + 3, 1);
                databuf_memcpy(rom, new_sym, sym_len +2);
            }            
        }
    } else if(sym_len > 0) {
        size_t len = 0;
        char* rom_sym = doa_get_rom_symbol(order, &len);
        if(len > 0) {
            databuf_memcpy(rom, rom_sym, len);
            free(rom_sym);
        } else {
            databuf_memcpy(rom, sym, sym_len);
        }
    }
    return 1;
}
int cboe_handle_spread(char *fixmsg, dart_rom_pos * xlateinfo,
                      char mtype, char *status,
                      databuf_t * rom, size_t field,
                      int *rejtected,
                      dart_ex_order_obj * order,
                      pthread_mutex_t * mutex, hashmap_t map)
{
    size_t len = xlateinfo[field].end - xlateinfo[field].beg;
    //size_t leg_len = xlateinfo[66].end - xlateinfo[66].beg;
    char *p = fixmsg + xlateinfo[field].beg;
    if (len > 0 && doa_get_is_spread(order)) {
        if(p[0] == '3') {
            databuf_memcpy(rom, "5", 1);
        } else {
            databuf_memcpy(rom, "6", 1);
        }
    }
    return 1;
}
int cboe_to_rom_price(char *fixmsg, dart_rom_pos * xlateinfo,
                      char mtype, char *status,
                      databuf_t * rom, size_t field,
                      int *rejtected,
                      dart_ex_order_obj * order,
                      pthread_mutex_t * mutex, hashmap_t map)
{
    size_t len = xlateinfo[field].end - xlateinfo[field].beg;
    //size_t leg_len = xlateinfo[66].end - xlateinfo[66].beg;
    char *p = fixmsg + xlateinfo[field].beg;
    if (len > 0 && doa_get_is_user_defined(order)) {
        char m = p[0];
        if (m == '-') {
            databuf_memcpy(rom, p + 1, len - 1);
        } else {
            char sub_p[100];
            memset(sub_p, '\0', 100);
            memcpy(sub_p, p, len);
            databuf_write(rom, "-%s", sub_p);
        }
    } else if (len > 0) {
        databuf_memcpy(rom, p, len);
    }
    return 1;
}

int cboe_tradefor(char *rom, romindex_t * map, char *type,
                  databuf_t * fix, size_t field, int *rejected,
                  dart_ex_order_obj * order)
{
    int ret = 0;
    if (map[field].name != 0) {
        char *symbol = rom + map[field].beg;
        size_t sym_len = map[field].end - map[field].beg;
        if (sym_len > 0) {
            doa_set_trade_for(order, symbol, sym_len);
            ret =
                fixmsg_addstr_bylen(fix, map[field].name, symbol, sym_len);
        } else {
            symbol = doa_get_trade_for(order, &sym_len);
            if (symbol) {
                ret =
                    fixmsg_addstr_bylen(fix, map[field].name, symbol,
                                        sym_len);
                free(symbol);
            }
        }
    }
    return ret;
}
int cfe_type(char *rom, romindex_t * map, char *mtype,
                      databuf_t * fix, size_t field, int *rejected,
                      dart_ex_order_obj * order)
{
    int ret = -1;
    char *type = rom + map[field].beg;
    size_t type_len = map[field].end - map[field].beg;
    if (type_len > 0 && type[0] != '3') {
        doa_set_type(order, type, type_len);
        ret =
            fixmsg_addstr_bylen(fix, "40", type, type_len);
    } else { 
        type = doa_get_type(order, &type_len);
        if (type) {
            ret =
                fixmsg_addstr_bylen(fix, "40", type,
                                    type_len);
            free(type);
        }
    }
    return ret;
}
int cboe_tif_trans(char *rom, romindex_t * map,
                             char *mtype, databuf_t * fix,
                             size_t field, int *rejected,
                             dart_ex_order_obj * order)
{
    if (map[field].name != 0) {
        char* sym = rom + map[5].beg;
        int len = map[5].end - map[5].beg;
        if(len >= 3 && strncmp("VXT", sym, 3) == 0) {
                return 
                    fixmsg_addstr2(fix, map[field].name, map[field].len, "0",
                                   1);
        } else {
            return def_rom2fix(rom, map, mtype,
                    fix, field, rejected,order);
        }
    }
    return 0;
}

int cboe_handle_auction(char *rom, romindex_t * map, char *type,
                      databuf_t * fix, size_t field, int *rejected,
                      dart_ex_order_obj * order)
{
    int ret = 0;
    char *symbol = rom + map[field].beg;
    size_t sym_len = map[field].end - map[field].beg;
    if (sym_len > 0) {
        ret =
            fixmsg_addstr_bylen(fix, "9384", "14", 2);
        ret =
            fixmsg_addstr_bylen(fix, "9385", symbol, sym_len);
    } 
    return ret;
}
int cboe_sec_def(char *rom, romindex_t * map, char *type,
                      databuf_t * fix, size_t field, int *rejected,
                      dart_ex_order_obj * order)
{
    int ret = 0;
    char *symbol = rom + map[field].beg;
    size_t sym_len = map[field].end - map[field].beg;
    size_t leg_len = map[66].end - map[66].beg;
    if (sym_len > 0 && leg_len > 0 && !doa_get_is_user_defined(order)) {
        doa_set_sec_def(order, symbol, sym_len);
        ret =
            fixmsg_addstr_bylen(fix, "48", symbol, sym_len);
    } else if(doa_get_is_spread(order) && !doa_get_is_user_defined(order)) {
        symbol = doa_get_sec_def(order, &sym_len);
        if (symbol) {
            ret =
                fixmsg_addstr_bylen(fix, "48", symbol,
                                    sym_len);
            free(symbol);
        }
    }
    return ret;
}
int cboe_opt_symbol_trans(char *rom, romindex_t * map, char *type,
                      databuf_t * fix, size_t field, int *rejected,
                      dart_ex_order_obj * order)
{
    int result = 0;
    int free_it = 0;
    char *symbol = rom + map[field].beg;
    size_t sym_len = map[field].end - map[field].beg;
    char* sec_def = rom + map[55].beg;
    size_t secdlen = map[55].end - map[55].beg;
    char* underscore = 0x0;
    if(secdlen > 0) {
        underscore = memchr(sec_def, '_', secdlen);
    }
    if(underscore != 0x0) {
        doa_set_rom_symbol(order, symbol, sym_len);
        sym_len = underscore - sec_def;
        symbol = sec_def;
        doa_set_symbol(order, symbol, sym_len);
        result = fixmsg_addstr2(fix, map[field].name, map[field].len,
                                symbol, sym_len);
    } else {
        if(sym_len <= 0) {
            symbol = doa_get_symbol(order, &sym_len);
            free_it = 1;
        } else {
            doa_set_rom_symbol(order, symbol, sym_len);
            doa_set_symbol(order, symbol, sym_len);
        }
        if(sym_len > 0) {
            char *pslash = memchr(symbol, '/', sym_len);
            if (pslash == 0) {
                result = fixmsg_addstr2(fix, map[field].name, map[field].len,
                                        symbol, sym_len);
            } else {
                char buffer[64];
                const int leftlen = pslash - symbol;
                memcpy(buffer, symbol, leftlen);
                memcpy(buffer + leftlen, ".", 1);
                memcpy(buffer + leftlen + 1, symbol + leftlen +1, (sym_len -(leftlen + 1)));
                result = fixmsg_addstr2(fix, map[field].name, map[field].len,
                                        buffer, sym_len);

            }
            if(free_it) {
                free(symbol);
            }
        }
    }
    return result;
}
/**
 * This function will strip the year and month code off a symbol.
 * This cannot be used for option on future symbols that include
 * spaces, strikes and so forth.
 */
int cboe_get_base_sym_len(const char *symbol, int sym_len)
{
    int len = get_base_sym_len(symbol, sym_len);
    int end = len - 1;
    /*while (symbol[end] >= 0x30 && symbol[end] <= 0x39) {
        --end;
    } */
    return end + 1;
}
int cboe_symbol_trans(char *rom, romindex_t * map, char *type,
                      databuf_t * fix, size_t field, int *rejected,
                      dart_ex_order_obj * order)
{
    int ret = 0;
    if (map[field].name != 0) {
        char *symbol = rom + map[field].beg;
        size_t sym_len = map[field].end - map[field].beg;
        size_t leg_len = map[66].end - map[66].beg;
        char* sec_def = rom + map[55].beg;
        size_t sec_len = map[55].end - map[55].beg;
        if(sec_len > 0 && leg_len <= 0) {
            doa_set_rom_symbol(order, symbol, sym_len);
            sec_len = cboe_get_base_sym_len(sec_def, sec_len);
            doa_set_symbol(order, sec_def, sec_len);
            ret =
                fixmsg_addstr_bylen(fix, map[field].name, sec_def, sec_len);
        } else if (rom[0] == 'E' && sym_len > 0) {
            doa_set_rom_symbol(order, symbol, sym_len);
            sym_len = cboe_get_base_sym_len(symbol, sym_len);
            doa_set_symbol(order, sec_def, sec_len);
            ret =
                fixmsg_addstr_bylen(fix, map[field].name, symbol, sym_len);
        } else {
            symbol = doa_get_symbol(order, &sym_len);
            if (symbol) {
                ret =
                    fixmsg_addstr_bylen(fix, map[field].name, symbol,
                                        sym_len);
                free(symbol);
            }
        }
    }
    return ret;
}

static void recalc_cboe_cum(dart_ex_order_obj * order)
{
    int cur_cum = doa_get_temp_cum(order);
    doa_add_to_offset_cum(order, cur_cum);
    doa_set_temp_cum(order, 0);
}

static void check_leg_pending(char *fixmsg, dart_rom_pos * positions,
                              dart_ex_order_obj * order)
{
    if (doa_get_is_spread(order)) {
        char *beg_off = fixmsg;
        int len = positions[17].end - positions[17].beg;
        beg_off += positions[17].beg;
        if (len > 0) {
            int pend_rep =
                doa_get_is_pending_rep_for_leg(order, beg_off, len);
            if (pend_rep) {
                doa_decrement_is_pending_rep_for_leg(order,beg_off, len);
            }
        }
    }
}

int cboe_fixrom_status_xlate(char *fixmsg, dart_rom_pos * positions,
                             char mtype, char *status,
                             databuf_t * rom, size_t romfield,
                             int *rejtected, dart_ex_order_obj * order,
                             pthread_mutex_t * mutex, hashmap_t ord_map)
{

    if (status[0] == '5') {
        databuf_memcpy(rom, "5", 1);
        //recalc_cboe_cum(order);
        //check_leg_pending(fixmsg, positions, rom, order);
        size_t len = positions[66].end - positions[66].beg;
        char* beg_off = fixmsg;
        beg_off += positions[66].beg;
        if(!doa_get_is_spread(order)) {
            recalc_cboe_cum(order);
        } else if (doa_get_is_spread(order) && len > 0 && beg_off[0] == '3') {
            recalc_cboe_cum(order);
        }
        return 1;
    }
    if(status[0] == '3') {
        char* sym = fixmsg + positions[5].beg;
        int len = positions[5].end - positions[5].beg;
        if(len >= 3 && strncmp("VXT", sym, 3) == 0) {
            databuf_memcpy(rom, "4", 1);
            return 1;
        }
        size_t tif_len = 0;
        char* tif = doa_get_tif(order, &tif_len);
        if(tif_len > 0 && tif[0] != '1') {
            databuf_memcpy(rom, "4", 1);
            free(tif);
            return 1;
        } 
        if(tif_len > 0 && tif) {
            free(tif);
        }
    }
    char *beg_off = fixmsg;
    size_t len = (positions[romfield].end - positions[romfield].beg);
    beg_off += positions[romfield].beg;
    if (romfield == 14 && mtype != 'U') {
        len = determine_trans_type(romfield, beg_off, rom, len);
        status = fixmsg;
        status += positions[romfield].beg;
    } else if (romfield == 14 && mtype == 'U') {
        len = set_status_for_cnrej(fixmsg, positions, status, rom, order);
    }
    return 1;
}

int cboe_trans_expire_my(char *rom, romindex_t * map, char *type,
                         databuf_t * fix, size_t field, int *rejected,
                         dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0 && !doa_get_is_spread(order)) {
        size_t len = map[field].end - map[field].beg;
        if (len > 0) {
            char *p = rom + map[field].beg;
            set_val_in_order_struct(p, len, field, order, type);
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
        } else if (rom[0] != 'E' && map[field].name != NULL) {
            char *p = doa_get_expireMonYear(order, &len);
            if (len > 0 && p != NULL) {
                result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
                free(p);
            }
        }
    }
    return result;
}
int cboe_trans_abn_exec_broker(char *rom, romindex_t * map, char *type,
                           databuf_t * fix, size_t field, int *rejected,
                           dart_ex_order_obj * order)
{
    size_t len = map[field].end - map[field].beg;
    char *p = rom + map[field].beg;
    char *cmta = rom + map[46].beg;
    size_t cmta_len = map[46].end - map[46].beg;
    if (len > 0) {
        doa_set_clr_id(order, p, len);
        if(cmta_len > 0) {
            doa_set_cmta(order, cmta, cmta_len);
        }
        if(cmta_len <= 0) {
            *fix->wr_ptr++ =FIX_FIELD_DELIM;
            databuf_write(fix, "76=%s:", exec_broker);
            databuf_memcpy(fix, p, len);
        } else {
            *fix->wr_ptr++ =FIX_FIELD_DELIM;
            databuf_write(fix, "76=%s:", exec_broker);
            databuf_memcpy(fix, cmta, cmta_len);
        }
    } else {
        p = doa_get_clr_id(order, &len);
        cmta = doa_get_cmta(order, &cmta_len); 
        if(len > 0 && cmta_len == 0) {
            databuf_write(fix, "%c76=%s:%s", FIX_FIELD_DELIM, exec_broker, p);
        } else if(cmta_len > 0) {
            databuf_write(fix, "%c76=%s:%s", FIX_FIELD_DELIM, exec_broker, cmta);
        }
        if(p) {
            free(p);
        }
        if (cmta) {
            free(cmta);
        }
    }
    return 1;
}

int cboe_trans_exec_broker(char *rom, romindex_t * map, char *type,
                           databuf_t * fix, size_t field, int *rejected,
                           dart_ex_order_obj * order)
{
    size_t len = map[field].end - map[field].beg;
    char *p = rom + map[field].beg;
    char *cmta = rom + map[46].beg;
    size_t cmta_len = map[46].end - map[46].beg;
    if (len > 0) {
        doa_set_clr_id(order, p, len);
        doa_set_cmta(order, cmta, cmta_len);
        //if(len > 0 && cmta_len == 0) {
            *fix->wr_ptr++ =FIX_FIELD_DELIM;
            databuf_write(fix, "76=%s:", exec_broker);
            databuf_memcpy(fix, p, len);
        /*} else if(cmta_len > 0) {
            *fix->wr_ptr++ =FIX_FIELD_DELIM;
            databuf_write(fix, "76=%s:", exec_broker);
            databuf_memcpy(fix, cmta, cmta_len);
        }*/
    } else {
        p = doa_get_clr_id(order, &len);
        cmta = doa_get_cmta(order, &cmta_len); 
        //if(len > 0 && cmta_len == 0) {
            databuf_write(fix, "%c76=%s:%s", FIX_FIELD_DELIM, exec_broker, p);
       // } else if(cmta_len > 0) {
         //   databuf_write(fix, "%c76=%s:%s", FIX_FIELD_DELIM, exec_broker, cmta);
       // }
        if(p) {
            free(p);
        }
        if (cmta) {
            free(cmta);
        }
    }
    return 1;
}

static void cboe_update_shares_for_replace(char *dart_val, size_t len,
                               dart_ex_order_obj * dart_obj)
{
    if (len > 0) {
        int l_qty = atoi(dart_val);
        int delta = doa_get_delta(dart_obj);
        int known_leaves = doa_get_leaves_shares(dart_obj);
        doa_set_leaves_shares(dart_obj, (known_leaves + delta));
        doa_set_order_qty(dart_obj, l_qty);
        send_debug_message("QTY: %d, delta %d, leaves: %d, set leaves to this: %d \n",
                l_qty, delta, known_leaves, (known_leaves + delta));
    }
}
int cboe_trans_shares(char *rom, romindex_t * map, char *type,
                      databuf_t * fix, size_t field, int *rejected,
                      dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        if (len > 0 && rom[0] != 'R') {
            char *p = rom + map[field].beg;
            set_val_in_order_struct(p, len, field, order, type);
            result =
                fixmsg_addstr2(fix, map[field].name, map[field].len, p,
                               len);
        } else if (len > 0) {
            int cboe_cum = doa_get_offset_cum(order);
            char *p = rom + map[field].beg;
            int new_shares = strtol(p, NULL, 10);
            new_shares -= cboe_cum;
            int len = dart_itoa(new_shares, cboe_cum_str, 10);
            set_val_in_order_struct(p, len, field, order, type);
            result =
                fixmsg_addstr2(fix, map[field].name, map[field].len,
                               cboe_cum_str, len);
        }
    }
    return result;
}

int cboe_trans_mat_day(char *rom, romindex_t * map, char *type,
                       databuf_t * fix, size_t field, int *rejected,
                       dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        char *p = rom + map[field].beg;
        if (len == 1) {
            char val[3];
            val[0] = '0';
            val[1] = p[0];
            val[2] = '\0';
            set_val_in_order_struct(val, 2, field, order, type);
            result = fixmsg_addstr_bylen(fix, map[field].name, val, 2);
        } else if (len > 0) {
            set_val_in_order_struct(p, len, field, order, type);
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
        } else {
            p = doa_get_expire_day(order, &len);
            if (len > 0 && p != NULL) {
                result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
                free(p);
            }
        }
    }
    return result;
}

int get_is_cboe_connected()
{
    int ret = 0;
    pthread_mutex_lock(&connection_lock);
    ret = is_cboe_connected;
    pthread_mutex_unlock(&connection_lock);
    return ret;
}

void set_is_cboe_connected(int is)
{
    pthread_mutex_lock(&connection_lock);
    is_cboe_connected = is;
    pthread_mutex_unlock(&connection_lock);
}

int set_cboe_defaults(char *rommsg, struct dart_fix_pos *positions,
                      char *mtype, databuf_t * output,
                      size_t romfield, int *rejected,
                      dart_ex_order_obj * order)
{
    size_t c_len =positions[66].end - positions[66].beg; 
    if(c_len > 0 && order) {
        doa_set_is_spread(order, 1);
        char* p = rommsg + positions[66].beg;
        doa_set_is_user_defined(order, p[0]);
    }
    //databuf_write(output, "%c57=%s", FIX_FIELD_DELIM, target_sub_id);
    databuf_write(output, "%c21=2", FIX_FIELD_DELIM);
    databuf_write(output, "%c386=1", FIX_FIELD_DELIM);
    databuf_write(output, "%c336=%s", FIX_FIELD_DELIM, tradin_session);
    return 1;
}

int set_cboe_transacttime(char *rom, romindex_t * map, char *type,
                          databuf_t * fix, size_t field, int *rejected,
                          dart_ex_order_obj * order)
{
    if (map[field].name != 0) {
        struct tm *gmt = 0;
        time_t now = 0;
        time(&now);
        struct tm timestruct;
        gmt = gmtime_r(&now, &timestruct);
        return fixmsg_addtime(fix, "60", gmt);
    } else {
        return 0;
    }
}

int trans_cboe_mleg_put_call(char *rom, dart_fix_pos * map,
                             char *mtype, databuf_t * fix,
                             size_t field, int *rejected,
                             dart_ex_order_obj * order,
                             dart_leg_obj * l_obj)
{
    int clean_up = 0;
    int len = map[field].end - map[field].beg;
    char *put_call = rom + map[field].beg;
    if (len > 0) {
        doa_set_leg_call_put(order, put_call, len, l_obj);
    } else {
        put_call = doa_get_leg_call_put(order, l_obj, &len);
        if (put_call) {
            len = 1;
            clean_up = 1;
        }
    }
    if (put_call != 0) {
        databuf_write(fix, "%c%s=", FIX_FIELD_DELIM, "613");
        if (put_call[0] == 'C') {
            databuf_memcpy(fix, "1", 1);
        } else {
            databuf_memcpy(fix, "0", 1);
        }
    }
    if (clean_up) {
        free(put_call);
    }
    if (rom[0] == 'E') {
        l_obj->is_pending_rep = 0;
    } else if (rom[0] == 'R') {
        l_obj->is_pending_rep = 2;
    }

    return 1;
}
int cfe_spread_tag(char *fixmsg, dart_rom_pos * positions,
                       char mtype, char *status,
                       databuf_t * rom, size_t romfield,
                       int *rejtected, dart_ex_order_obj * order,
                       pthread_mutex_t * mutex, hashmap_t ord_map)
{
    add_rom_id_to_mess(rom, order);
    return 1;
}

int cboe_last_shares_xlate(char *fixmsg, dart_rom_pos * positions,
                     char mtype, char *status,
                     databuf_t * rom, size_t romfield,
                     int *rejtected, dart_ex_order_obj * order,
                     pthread_mutex_t * mutex, hashmap_t ord_map)
{
    if(!doa_get_is_spread(order)) {
        char *beg_off = fixmsg;
        size_t len = (positions[romfield].end - positions[romfield].beg);
        beg_off += positions[romfield].beg;
        set_val_in_order_struct(beg_off, len, romfield, order, &mtype);
        if (!determine_trans_type(romfield, beg_off, rom, len)) {
            databuf_memcpy(rom, beg_off, len);
        }
    } else {
        size_t plen = positions[66].end - positions[66].beg;
        char* p_off = fixmsg;
        p_off += positions[66].beg;
        char *beg_off = fixmsg;
        size_t len = (positions[romfield].end - positions[romfield].beg);
        beg_off += positions[romfield].beg;
        if (doa_get_is_spread(order) && plen > 0 && p_off[0] == '3') {
            set_val_in_order_struct(beg_off, len, romfield, order, &mtype);
        }
        if (!determine_trans_type(romfield, beg_off, rom, len)) {
            databuf_memcpy(rom, beg_off, len);
        }
    }
    return 1;
}
int from_cboe_tif_trans(char *fixmsg, dart_rom_pos * positions,
                     char mtype, char *status,
                     databuf_t * rom, size_t romfield,
                     int *rejtected, dart_ex_order_obj * order,
                     pthread_mutex_t * mutex, hashmap_t ord_map)
{
    char* sym = fixmsg + positions[5].beg;
    int len = positions[5].end - positions[5].beg;
    if(len >= 3 && strncmp("VXT", sym, 3) == 0) {
        databuf_memcpy(rom, "1", 1);
    } else {
        return def_xlate_fixrom(fixmsg,  positions,
                    mtype, status,
                    rom, romfield,
                    rejtected, order,
                    mutex, ord_map);
    }
    return 1;
}
int cboe_shares_xlate_fixrom(char *fixmsg, dart_rom_pos * positions,
                     char mtype, char *status,
                     databuf_t * rom, size_t romfield,
                     int *rejtected, dart_ex_order_obj * order,
                     pthread_mutex_t * mutex, hashmap_t ord_map)
{
    char *beg_off = fixmsg;
    size_t len = (positions[romfield].end - positions[romfield].beg);
    beg_off += positions[romfield].beg;
    if (status[0] == '5') {
        int l_qty = atoi(beg_off);
        int delta = doa_get_delta(order);
        int known_leaves = doa_get_leaves_shares(order);
        doa_set_leaves_shares(order, (known_leaves + delta));
        doa_set_order_qty(order, l_qty);
        send_debug_message("Set shares to this: %d status: %c \n", l_qty, status[0]);
    } else {
        size_t plen = positions[66].end - positions[66].beg;
        char* p_off = fixmsg;
        p_off += positions[66].beg;
        if(!doa_get_is_spread(order)) {
            cboe_update_shares_for_replace(beg_off, len, order);//, &mtype);
        } else if (doa_get_is_spread(order) && plen > 0 && p_off[0] == '3') {
            cboe_update_shares_for_replace(beg_off, len, order);//, &mtype);
            send_debug_message("Updated replace spread status %c \n", status[0]);
        }
    }
    if (!determine_trans_type(romfield, beg_off, rom, len)) {
        databuf_memcpy(rom, beg_off, len);
    }
    return 1;
}
int cboe_trans_mleg_sec_type(char *rom, dart_fix_pos * map,
                        char *mtype, databuf_t * fix,
                        size_t field, int *rejected,
                        dart_ex_order_obj * order, dart_leg_obj * l_obj)
{
    int clean_up = 0;
    int len = map[field].end - map[field].beg;
    char *put_call = rom + map[field].beg;
    if (len > 0) {
        doa_set_leg_sec_type(order, put_call, len, l_obj);
    } else if( len <= 0 && rom[0] == 'E') {
        doa_set_leg_sec_type(order, "OPT", 3, l_obj);        
    } else {
        put_call = doa_get_leg_sec_type(order, l_obj, &len);
        if(put_call) {
           clean_up = 1;
        }
    }
    if (len > 0) {
        databuf_write(fix, "%c%s=", FIX_FIELD_DELIM, "609");
        if(put_call[0] == 'O') {
            databuf_memcpy(fix, "OPT", 3);
        } else if(put_call[0] == 'E') {
            *rejected = 1;
            databuf_reset(fix);
            create_rom_reject(rom, fix,
                                      "Equity Legs not allowed");
            return -1;//databuf_memcpy(fix, "CS", 2);
        } else {
            databuf_memcpy(fix, "FUT", 3);
        }
    } else {
        databuf_write(fix, "%c%s=OPT", FIX_FIELD_DELIM, map[field].name);
    }

    if (clean_up) {
        free(put_call);
    }

    return 1;
}

void set_cboe_rom2fix_leg_book(orderbook * ob)
{
    ob->r2fleg[1].legfunc = trans_cboe_mleg_put_call;
    ob->r2fleg[1].rom_tag = 31;
    ob->r2fleg[4].legfunc = trans_mleg_open_close;
    ob->r2fleg[4].rom_tag = 38;
    ob->r2fleg[3].legfunc = trans_mleg_stike;
    ob->r2fleg[3].rom_tag = 32;
    ob->r2fleg[2].legfunc = trans_leg_mat_date;
    ob->r2fleg[2].rom_tag = 30;
    ob->r2fleg[0].legfunc = cboe_trans_mleg_sec_type;
    ob->r2fleg[0].rom_tag = 35;
    ob->r2fleg[7].legfunc = trans_mleg_symbol;
    ob->r2fleg[7].rom_tag = 5;
    ob->r2fleg[6].legfunc = trans_mleg_side;
    ob->r2fleg[6].rom_tag = 4;
    ob->r2fleg[5].legfunc = trans_mleg_ratio;
    ob->r2fleg[5].rom_tag = 6;
}

void send_cancel_all();
void con_callBack(con_obj * co, int is_connected, char *log, int len);
void fix_server_connection_callBack(con_obj * co, int is_connected,
                                    char *log, int len);
void test_lac(con_obj * co);
void destroy_rom_queue(queue_t q);
int send_cboe_logon(struct parse_args *pa);

int main(int argc, char **argv)
{
    int ret_len = 0;
    struct init_args *fix_args;
    ex_config *ex = NULL;
    struct tm *newtime;
    char *reserved_ids = NULL;
    char *branch_code = NULL;
    int branch_code_int = 25;
    int keep_running = 1;
    cboe_cum_str = calloc(100, 1);
    queue_t rom_q;
    time_t long_time;
    orderbook *ob = NULL;       // init_order_book(next_order_id, def_rom2fix,50000);
    rom_q = create_queue();
    ex = init_config("cboe.dat", 14);
    fix_args = load_fix_config("cboe.dat", 14);
    load_rom_config(ex->dc, rom_q);
    fix_client_name = fix_args->target_comp;
    fix_client_len = fix_args->tc_len;
    ob = init_order_book(next_order_id, get_is_cboe_connected,
                         def_rom2fix, set_cboe_defaults,
                         def_xlate_fixrom, ex->order_allocation,
                         ex->mod_name, ex->mod_name_len, fix_client_name,
                         fix_client_len, NULL, 0);
    ob->no_fix_43 = 1;
    ob->id_last = 1;
    branch_code =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "Branch Code", 11,
                        &branch_code_int);
    if (branch_code != NULL) {
        branch_code_int = atoi(branch_code);
    }

    char *acc_teller =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "ACCOUNT", 7,
                        &ret_len);
    if (acc_teller != NULL) {
        show_acc = atoi(acc_teller);
    }
    char* dart_teller =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "DART", 4, &ret_len);
    if(ret_len > 0 && dart_teller != NULL) {
        is_dart = atoi(dart_teller);
    } else {
        is_dart = 0;
    }
    tradin_session =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "Trading Session",
                        15, &tradin_sess_len);
    mm_dest =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "MM Dest", 7,
                        &mm_dest_len);
    if (mm_dest_len <= 0) {
        mm_dest = "22";
        mm_dest_len = 2;
    }
    exec_broker =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "Exec Broker", 11,
                        &exec_broker_len);

    reserved_ids =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10,
                        "Reserved Branch Codes", 21, &ret_len);
    start_seq_generator(reserved_ids, ret_len, "CBOEOrderids.map", 16,
                        branch_code_int, fix_args, //fix_args->reset_time->create_new,
                        ex->end_hour, ex->end_min);
    opt_database_init(&ex->db, ex->dest_id, ob, 1);
    set_option_rom2fix(ob);
    set_option_fix_to_rom(ob);
    fs_set_spread_book(ob);
    set_cboe_rom2fix_leg_book(ob);
    ob->fix2rom[14] = cboe_fixrom_status_xlate;
    ob->rom2fix[10] = cboe_trans_capacity;
    ob->rom2fix[43] = cboe_trans_shares;
    if(show_acc) {
        ob->rom2fix[11] = cboe_trans_abn_exec_broker;
    } else {
        ob->rom2fix[11] = cboe_trans_exec_broker;
    }

    
    ob->rom2fix[50] = set_cboe_transacttime;
    ob->rom2fix[30] = cboe_trans_expire_my;
    ob->fix2rom[15] = cboe_handle_ordids;//def_handle_ordids;
    ob->fix2rom[6] = cboe_shares_xlate_fixrom;
    ob->fix2rom[42] = cboe_last_shares_xlate;
    ob->rom2fix[62] = cboe_trans_mat_day;
    ob->rom2fix[55] = cboe_sec_def;
    ob->fix2rom[66] = cboe_handle_spread;
    ob->rom2fix[67] = cboe_handle_auction;
    if (strncmp("CFE_MAIN", tradin_session, 8) == 0) {
        ob->fix2rom[5] = cboe_handle_spread_sym;
        ob->rom2fix[5] = cboe_symbol_trans;
        ob->rom2fix[24] = cboe_tradefor;
        ob->rom2fix[8] = cfe_type;
        ob->fix2rom[17] = cfe_spread_tag;
        ob->rom2fix[9] = cboe_tif_trans;
        ob->fix2rom[9] = from_cboe_tif_trans;
    } else {
        ob->rom2fix[5] = cboe_opt_symbol_trans;
    }
    ob->fix2rom[44] = cboe_to_rom_last_price;
    ob->rom2fix[4] = rom_to_cboe_side;
    ob->fix2rom[7] = cboe_to_rom_price;
    ob->rom2fix[7] = rom_to_cboe_price;
    module_name_len = ex->mod_name_len;
    module_name = calloc(module_name_len + 1, 1);
    memcpy(module_name, ex->mod_name, module_name_len);;
    start_dart_mail_service(ex->mail_server, ex->mail_server_len,
                            ex->biz_group, ex->b_len, ex->tech_group,
                            ex->t_len, ex->end_hour, ex->end_min, 7);


    sender_sub_id =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "senderSubId", 11,
                        &ret_len);
    if (ret_len <= 0) {
        sender_sub_id = "DEGS";
    }
    target_sub_id =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "targetSubId", 11,
                        &target_sub_len);
    username =
        get_val_for_tag(fix_args->dc, "FIX SERVER", 10, "username", 8,
                        &user_len);
    ret_len =
        start_dart_client_services(&complex_from_rom_func, ex->mod_name,
                                   ex->mod_name_len, ex->rom_port, rom_q,
                                   ex->rom_ip, ex->rom_ip_len,
                                   ex->end_hour, ex->end_min, 7,
                                   ex->heart_beat_int, ob, con_callBack,
                                   parse_rom_message, build_offsets,
                                   test_lac, create_rom_resend_pad, 0);
    ret_len =
        dart_connect(&cboe_from_fix_func, ex->mod_name, ex->mod_name_len,
                     ex->fix_port, ex->fix_ip, ex->fix_ip_len,
                     ex->end_hour, ex->end_min, 7, ex->heart_beat_int, ob,
                     fix_server_connection_callBack, parse_fix_message,
                     fix_args, send_cboe_logon, build_fix_offsets,
                     low_activity_sig, 1);

        time(&long_time);
        struct tm lm;
        newtime = localtime_r(&long_time, &lm);
        newtime = &lm;
        int tzone = 1;
        if(newtime->tm_zone[0] == 'C') {
            tzone = 0;
        }
        int cur_day = newtime->tm_mday;
    while (keep_running) {
        time(&long_time);
        newtime = localtime_r(&long_time, &lm);
        newtime = &lm;
        if (newtime->tm_hour == ex->end_hour
            && newtime->tm_min >= ex->end_min
            && newtime->tm_min < (ex->end_min + 5)) {
            keep_running = 0;
            printf("Done with our test!!!!end_hour %d end_min %d now %d\n",
                   ex->end_hour, ex->end_min, newtime->tm_hour);
        } else {
            if(cur_day != newtime->tm_mday) {
                if(tzone == 1 && newtime->tm_hour - tzone == 0) {
                    set_time_str(newtime);
                    cur_day = newtime->tm_mday;
                } else if(tzone == 0) {
                    set_time_str(newtime);
                    cur_day = newtime->tm_mday;
                }
            }
            sleep(25);

            if (!get_is_cboe_connected()) {
                set_is_cboe_connected(1);
                ret_len =
                    dart_connect(&cboe_from_fix_func, ex->mod_name,
                                 ex->mod_name_len, ex->fix_port,
                                 ex->fix_ip, ex->fix_ip_len, ex->end_hour,
                                 ex->end_min, 7, ex->heart_beat_int, ob,
                                 fix_server_connection_callBack,
                                 parse_fix_message, fix_args,
                                 send_cboe_logon, build_fix_offsets,
                                 low_activity_sig, 1);
            }
        }
    }
    clean_dart_oo_pool(ob->mgr);
    free_init_args(fix_args);
    destroy_ex_config(ex);
    destroy_rom_queue(rom_q);
    destroy_queue(rom_q);
    return 0;
}

int send_cboe_logon(struct parse_args *pa)
{
    char *mblk;
    mblk = calloc(600, 1);
    if (mblk != NULL) {
        size_t len =
            sprintf(mblk, "50=%s%c57=%s%c21070=0%c98=0%c108=30%c", sender_sub_id,
                    FIX_FIELD_DELIM, target_sub_id, FIX_FIELD_DELIM,FIX_FIELD_DELIM,
                    FIX_FIELD_DELIM, FIX_FIELD_DELIM);
        if (len <= 0) {
            send_logon(pa);
        } else {
            send_gen_logon(pa, mblk, len);
        }
        free(mblk);
        return 0;
    }
    return -1;
}

int cboe_trans_mm_account(char *rom, romindex_t * map, char *type,
                          databuf_t * fix, size_t field, int *rejected,
                          dart_ex_order_obj * order)
{
    int result = 0;
    size_t len = map[field].end - map[field].beg;
    char *p = rom + map[field].beg;
    //char* mm_home = rom + map[ROM_MM_HOME].beg;
    size_t mm_home_len = map[ROM_MM_HOME].end - map[ROM_MM_HOME].beg;
    if (len > 0) {
        set_val_in_order_struct(p, len, field, order, type);
        if (mm_home_len > 0) {
        } else {
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
        }
    } else if (rom[0] != 'E') {
        char *p = doa_get_mm_account(order, &len);
        size_t clen = 0;
        char *c = doa_get_capacity(order, &clen);
        if ((len > 0 && p != NULL) && (clen > 0 && c[0] != '6')) {
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
            free(p);
        }
    }
    return result;
}

int cboe_trans_capacity(char *rom, romindex_t * map, char *type,
                        databuf_t * fix, size_t field, int *rejected,
                        dart_ex_order_obj * order)
{
    int result = 0;
    char *mm_home = 0;
    size_t mm_home_len = 0;
    char *mm_acc = 0;
    size_t mm_acc_len = 0;
    char *instr = 0;
    size_t instr_len = 0;
    char *trader = 0;
    size_t trader_len = 0;
    char *tradefor = 0;
    size_t tradefor_len = 0;
    size_t len = map[10].end - map[10].beg;
    char *p = rom + map[10].beg;
    char *clear_acc = rom + map[12].beg;
    size_t clear_len = map[12].end - map[12].beg;
    if (len > 0 && (p[0] == 'B' || p[0] == 'A' || p[0] == 'X' || p[0] == 'C') && rom[0] == 'E') {
        if (p[0] == 'A' && is_dart) {
            result = fixmsg_addstr_bylen(fix, "47", "C", 1);
        } else if(p[0] == 'A') {
            result = fixmsg_addstr_bylen(fix, "47", "O", 1);
        } else if(p[0] == 'X') {
            result = fixmsg_addstr_bylen(fix, "47", "W", 1);
        } else if (p[0] == 'C') {
            result = fixmsg_addstr_bylen(fix, "47", "C", 1);
        } else {
            result = fixmsg_addstr_bylen(fix, "47", "J", 1);
        }
        doa_set_account(order, clear_acc, clear_len);
        if (clear_len == 8 && (strncmp((clear_acc + 4), "1209", 4) == 0)) {
            clear_len = 4;
        }
        result = fixmsg_addstr_bylen(fix, "440", clear_acc, clear_len);
        *fix->wr_ptr++ = FIX_FIELD_DELIM;
        databuf_memcpy(fix, "9324=", 5);
        databuf_memcpy(fix, clear_acc, clear_len);
        databuf_memcpy(fix, "        N", 9);
        doa_set_capacity(order, p, len);
    } else if (len > 0 && rom[0] == 'E') {
        mm_home = rom + map[ROM_MM_HOME].beg;
        mm_home_len = map[ROM_MM_HOME].end - map[ROM_MM_HOME].beg;
        mm_acc = rom + map[36].beg;
        mm_acc_len = map[36].end - map[36].beg;
        instr_len = map[15].end - map[15].beg;
        instr = rom + map[15].beg;
        trader = rom + map[2].beg;
        trader_len = map[2].end - map[2].beg;
        tradefor = rom + map[24].beg;
        tradefor_len = map[24].end - map[24].beg;
        doa_set_capacity(order, p, len);
        doa_set_account(order, clear_acc, clear_len);
        doa_set_trader(order, trader, trader_len);
        doa_set_mm_account(order, mm_acc, mm_acc_len);
        doa_set_mm_home(order, mm_home, mm_home_len);
        result = fixmsg_addstr_bylen(fix, "440", clear_acc, clear_len);
        if (instr_len > 0 && strncmp("INCROWD", instr, instr_len) == 0) {
            doa_set_mm_home(order, mm_home, mm_home_len);
            result = fixmsg_addstr_bylen(fix, "47", "I", 1);
            *fix->wr_ptr++ = FIX_FIELD_DELIM;
            databuf_memcpy(fix, "9324=I:", 7);
        } else if (strncmp(mm_home, mm_dest, mm_dest_len) == 0) {
            doa_set_mm_home(order, mm_home, mm_home_len);
            result = fixmsg_addstr_bylen(fix, "47", "M", 1);
            *fix->wr_ptr++ = FIX_FIELD_DELIM;
            databuf_memcpy(fix, "9324=M:", 7);
        } else {
            doa_set_mm_home(order, mm_home, mm_home_len);
            result = fixmsg_addstr_bylen(fix, "47", "N", 1);
            *fix->wr_ptr++ = FIX_FIELD_DELIM;
            databuf_memcpy(fix, "9324=N:", 7);
        }
        if (clear_len == 8 && (strncmp((clear_acc + 4), "1209", 4) == 0)) {
            clear_len = 4;
        }
        //*fix->wr_ptr++ =FIX_FIELD_DELIM;
        databuf_memcpy(fix, mm_acc, mm_acc_len);
        *fix->wr_ptr++ = ' ';
        databuf_memcpy(fix, clear_acc, clear_len);
        *fix->wr_ptr++ = 'N';
        databuf_memcpy(fix, tradefor, tradefor_len);
    } else {
        p = doa_get_capacity(order, &len);
        clear_acc = doa_get_account(order, &clear_len);
        if (p != 0 && (p[0] == 'B' || p[0] == 'A' || p[0] == 'X')) {
            if (p[0] == 'A') {
                result = fixmsg_addstr_bylen(fix, "47", "O", 1);
            } else if(p[0] == 'X') {
                result = fixmsg_addstr_bylen(fix, "47", "W", 1);
            } else {
                result = fixmsg_addstr_bylen(fix, "47", "J", 1);
            }
            if (clear_len == 8
                && (strncmp((clear_acc + 4), "1209", 4) == 0)) {
                clear_len = 4;
            }
            result = fixmsg_addstr_bylen(fix, "440", clear_acc, clear_len);
            databuf_write(fix, "%c9324=%- 3s        N", FIX_FIELD_DELIM,
                          clear_acc);
        } else if (p != 0 && len > 0) {
            mm_home = doa_get_mm_home(order, &mm_home_len);
            trader = doa_get_trader(order, &trader_len);
            mm_acc = doa_get_mm_account(order, &mm_acc_len);
            result = fixmsg_addstr_bylen(fix, "440", clear_acc, clear_len);
            if (clear_len == 8
                && (strncmp((clear_acc + 4), "1209", 4) == 0)) {
                clear_len = 4;
            }
            if (instr_len > 0 && strncmp("INCROWD", instr, 7) == 0) {
                result = fixmsg_addstr_bylen(fix, "47", "I", 1);
                databuf_write(fix, "%c9324=I:%- 3s %- 5sN%- 3s",
                              FIX_FIELD_DELIM, mm_acc, clear_acc, tradefor);
            } else if (mm_home_len > 0
                       && strncmp(mm_home, mm_dest, mm_dest_len) == 0) {
                result = fixmsg_addstr_bylen(fix, "47", "M", 1);
                databuf_write(fix, "%c9324=M:%- 3s %- 5sN%- 3s",
                              FIX_FIELD_DELIM, mm_acc, clear_acc, tradefor);
            } else {
                result = fixmsg_addstr_bylen(fix, "47", "N", 1);
                databuf_write(fix, "%c9324=N:%- 3s %- 5sN%- 3s",
                              FIX_FIELD_DELIM, mm_acc, clear_acc, tradefor);
            }

            if (mm_home) {
                free(mm_home);
            }
            if (trader) {
                free(trader);
            }
            if (tradefor) {
                free(tradefor);
            }
            if (mm_acc) {
                free(mm_acc);
            }
        }
        if (p) {
            free(p);
        }
        if (clear_acc) {
            free(clear_acc);
        }
    }
    return result;
}

void destroy_rom_queue(queue_t q)
{
    element_t e, temp;
    e = dequeue(q);
    while (e != NULL) {
        temp = e;
        e = temp->next;
        free_init_args((struct init_args *) temp->d);
        free(temp);
    }
}

void fix_server_connection_callBack(con_obj * co, int is_connected,
                                    char *log, int len)
{
    printf("Info about this FIX Connection: %s, Is it connected {%d} ? \n",
           get_name(co), is_connected);
    if (is_connected) {
    } else {
        fix_connection_callback(co, is_connected);
        set_is_cboe_connected(is_connected);
    }
}

void test_lac(con_obj * co)
{
    printf("Low activity reported on this connection: %s \n",
           get_name(co));
}

void con_callBack(con_obj * co, int is_connected, char *log, int len)
{
    printf("Info about this Connection: %s, Is it connected {%d} ? \n",
           get_name(co), is_connected);
    is_con_now = is_connected;
}

void cboe_check_n_clean(char *status, hashmap_t cl_ord_id_map,
                        hashmap_t rom_id_map, dart_ex_order_obj * obj,
                        pthread_mutex_t * map_mutex, char rom_type,
                        orderbook * ob, char *fix_message, int fix_len,
                        struct dart_rom_pos positions[ROM_LEN])
{
    if (status) {
        int leaves = doa_get_leaves_shares(obj);
        if ((strncmp("4", status, 1) == 0 ||
             (strncmp("8", status, 1) == 0))
            && !doa_get_is_spread(obj) && !doa_get_pending_replace(obj)
            && rom_type != 'U') {
            size_t ord_id_len = 0;
            size_t rom_id_len = 0;
            size_t rep_id_len = 0;
            char *ord_id = doa_get_cl_ord_id(obj, &ord_id_len);
            char *rom_id = doa_get_rom_id(obj, &rom_id_len);
            char *rep_id = doa_get_rep_ord_id(obj, &rep_id_len);
            pthread_mutex_lock(map_mutex);
            remove_from_map(cl_ord_id_map, ord_id, ord_id_len);
            remove_from_map(rom_id_map, rom_id, rom_id_len);
            if (rep_id != 0) {
                remove_from_map(cl_ord_id_map, rep_id, rep_id_len);
            }
            pthread_mutex_unlock(map_mutex);
            destroy_dart_order_obj(ob->mgr, obj);
            free(ord_id);
            free(rom_id);
            free(rep_id);
        } else if (strncmp("2", status, 1) == 0 && leaves == 0 &&
                   !doa_get_pending_replace(obj)
                   && !doa_get_is_spread(obj) && rom_type != 'U') {
            size_t ord_id_len = 0;
            size_t rom_id_len = 0;
            char *ord_id = doa_get_cl_ord_id(obj, &ord_id_len);
            char *rom_id = doa_get_rom_id(obj, &rom_id_len);
            pthread_mutex_lock(map_mutex);
            remove_from_map(cl_ord_id_map, ord_id, ord_id_len);
            remove_from_map(rom_id_map, rom_id, rom_id_len);
            pthread_mutex_unlock(map_mutex);
            destroy_dart_order_obj(ob->mgr, obj);
            free(ord_id);
            free(rom_id);
        } else if (rom_type == 'U') {
            char *rep_id = fix_message;
            rep_id += positions[15].beg;
            int rep_id_len = (positions[15].end - positions[15].beg);
            //size_t rep_id_len = 0;
            //char *rep_id = doa_get_rep_ord_id(obj, &rep_id_len);
            pthread_mutex_lock(map_mutex);
            if (rep_id != 0 && rep_id_len > 0) {
                remove_from_map(cl_ord_id_map, rep_id, rep_id_len);
            }
            pthread_mutex_unlock(map_mutex);
        }
    }
}

long cboe_fix_to_rom_override(struct message *mess, char *fix_message,
                              databuf_t * output, long fix_len,
                              char rom_type,
                              struct dart_rom_pos positions[ROM_LEN],
                              int *rejected, orderbook * ob)
{
    int i = 1;
    int s_len = 0;
    char *status = NULL;
    dart_ex_order_obj *obj = NULL;
    size_t doa_len = 0;
    if (rom_type == 'U') {
        s_len = databuf_write(output, "S,");
    } else {
        s_len = databuf_write(output, "%c,", rom_type);
    }
    if (!get_order_object
        (fix_message, fix_len, ob->cl_ord_id_map, positions, &ob->mutex,
         rom_type, &obj, mess, output, ob)) {
        return 0;
    }
    mess->name = doa_get_sender_name(obj, &doa_len);
    mess->name_len = doa_len;
    status = fix_message;
    status += positions[14].beg;
    int is_pending_rep = doa_get_pending_replace(obj);
    if(doa_get_is_spread(obj) && status[0] == 'E') {
        return 0;
    }
    if (is_pending_rep && rom_type != 'U') {
        if (status[0] == '0') {
            status = "5";
            if(doa_get_is_spread(obj) && !doa_get_is_user_defined(obj)) {
                //do nothing
            } else if(doa_get_is_spread(obj)) {
                check_leg_pending(fix_message, positions,obj);
                if(doa_get_are_any_legs_pending_rep(obj) == 0) {
                    doa_set_pending_replace(obj, 0);
                    char* beg_off = fix_message;
                    beg_off += positions[15].beg;
                    int len = positions[15].end - positions[15].beg;
                    doa_set_cl_ord_id(obj, beg_off, len);
                }
            } else {
                doa_decrement_pending_replace(obj);
            }
        } else if(status[0] == 'E') {
            return 0;
        } else if (status[0] == '4') {
            char* beg_off = fix_message;
            beg_off += positions[15].beg;
            int len = positions[15].end - positions[15].beg;
            if(doa_get_is_spread(obj) && !doa_get_is_user_defined(obj)) {
                doa_set_pending_replace(obj, 0);
                pthread_mutex_lock(&ob->mutex);
                remove_from_map(ob->cl_ord_id_map, beg_off, len);
                pthread_mutex_unlock(&ob->mutex);
            } else if(doa_get_is_spread(obj)) {
                check_leg_pending(fix_message, positions,obj);
                if(doa_get_are_any_legs_pending_rep(obj) == 0) {
                    pthread_mutex_lock(&ob->mutex);
                    remove_from_map(ob->cl_ord_id_map, beg_off, len);
                    pthread_mutex_unlock(&ob->mutex);
                    doa_set_pending_replace(obj, 0);
                }
            } else {
                doa_decrement_pending_replace(obj);
                if(doa_get_pending_replace(obj) <= 0) {
                    pthread_mutex_lock(&ob->mutex);
                    remove_from_map(ob->cl_ord_id_map, beg_off, len);
                    pthread_mutex_unlock(&ob->mutex);
                }
            }
            return 0;
        }
    }
    for (i = 1; i < ROM_LEN; ++i) {
        if (ob->fix2rom[i] != 0) {
            if (ob->fix2rom[i] (fix_message, positions, rom_type,
                                status, output, i, rejected,
                                obj, &ob->mutex,
                                ob->cl_ord_id_map) == -1) {
                fprintf(stderr, "%s: Processing %d failed\n",
                        __FUNCTION__, i);
            }
            if (*rejected != 0) {
                /* We do not support sending a DK (Don't Know) and
                 * most FIX sessions would not handle it anyway.  The
                 * best we can do is drop the resulting message. */
                fprintf(stderr,
                        "%s:%d Message processing failed, not sending to ROM\n",
                        __FILE__, __LINE__);
                return 0;
            }
        }
        databuf_memcpy(output, ",", 1);
    }
    databuf_memcpy(output, "\n", 1);
    cboe_check_n_clean(status, ob->cl_ord_id_map, ob->rom_ord_id_map, obj,
                       &ob->mutex, rom_type, ob, fix_message, fix_len,
                       positions);
    return databuf_unread(output);
}

void cboe_from_fix_func(struct message *mess, void *ob)
{
    struct databuf *output = databuf_alloc(mess->len * 5);
    long total_len;
    struct message m1;
    struct message log;
    int rejected = 0;
    orderbook *book = (orderbook *) ob;
    m1.name = NULL;
    m1.name_len = 0;
    total_len = 0;
    total_len =
        trans_fix_message(&m1, mess->data, mess->len, output,
                          &rejected, (orderbook *) ob,
                          &cboe_fix_to_rom_override);
    if (total_len == 0) {
        if (m1.name) {
            free(m1.name);
        }
        databuf_destroy(output);
        return;
    }

    m1.data = output->buffer;
    m1.len = databuf_unread(output);
    if (book->is_async) {
        exchange_async_rom_sender(&m1, ob);
    } else {
        send_message_to_roms(&m1, (orderbook *) ob);
    }
    log.name = mess->name;
    log.name_len = mess->name_len;
    log.data = mess->data;
    log.len = mess->len;
    log.name = m1.name;
    log.name_len = m1.name_len;
    log.data = m1.data;
    log.len = m1.len;
    dart_rolling_log_going(&log, book->module_name, book->module_name_len);
    databuf_destroy(output);
    if (m1.name) {
        free(m1.name);
    }
}

int cboe_handle_ordids(char *fix_mess, dart_rom_pos * positions,
                       char mtype, char *status,
                       databuf_t * rom, size_t romfield,
                       int *rejtected, dart_ex_order_obj * order,
                       pthread_mutex_t * mutex, hashmap_t ord_map)
{
    char *beg_off = 0;
    size_t len = 0;
    size_t ret_len = 0;
    size_t doa_len = 0;
    if (strncmp("4", status, 1) == 0 ||
        strncmp("6", status, 1) == 0 || strncmp("5", status, 1) == 0) {
        beg_off = (char *) fix_mess;
        beg_off += positions[CLORDID_LOC].beg;
        len = positions[CLORDID_LOC].end - positions[CLORDID_LOC].beg;
        databuf_memcpy(rom, beg_off, len);
        ret_len = len;
        if (strncmp("5", status, 1) == 0) {
            char *cur_cl_ord_id = doa_get_cl_ord_id(order, &doa_len);
            beg_off = (char *) fix_mess;
            beg_off += positions[CLORDID_LOC].beg;
            len = positions[CLORDID_LOC].end - positions[CLORDID_LOC].beg;

            if (strncmp(beg_off, cur_cl_ord_id, len) != 0) {
                //pthread_mutex_lock(map_mutex);
                //remove_from_map(cl_ord_id_map, cur_cl_ord_id, doa_len);
                //pthread_mutex_unlock(map_mutex);
                doa_set_cl_ord_id(order, beg_off, len);
            }
            free(cur_cl_ord_id);
        }
    } else {
        beg_off = (char *) fix_mess;
        beg_off += positions[CLORDID_LOC].beg;
        len = positions[CLORDID_LOC].end - positions[CLORDID_LOC].beg;
        databuf_memcpy(rom, beg_off, len);
        ret_len = len;
    }
    return ret_len;
}
