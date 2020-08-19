#ifndef __ROM2FIX_H__
#define __ROM2FIX_H__

#include <stdlib.h>
#include "databuf.h"
#include "client_manager.h"
#include "dart_orderbook.h"


int rom2fix_add_translator(size_t field, fieldxlatefunc func,
                           orderbook * ob);

#define DECLARE_TRANS_FUNC(func) \
    int func(char *rom, romindex_t *map, char* type, \
                           databuf_t *fix, size_t field, int *rejected, \
                           dart_ex_order_obj *order) \

long rom2fix_translate_fields(struct message *mess,
                              char *rommsg,
                              databuf_t * output,
                              long fix_len,
                              char *fix_type,
                              struct dart_fix_pos
                              positions[ROM_LEN],
                              int *rejected, orderbook * ob,
                              int is_spread);

DECLARE_TRANS_FUNC(def_rom2fix);
DECLARE_TRANS_FUNC(def_trans_side);
DECLARE_TRANS_FUNC(def_trans_ordtype);
DECLARE_TRANS_FUNC(def_trans_capacity);
DECLARE_TRANS_FUNC(def_trans_execinst);
DECLARE_TRANS_FUNC(def_trans_account);
DECLARE_TRANS_FUNC(set_transacttime);
DECLARE_TRANS_FUNC(def_field_noop);
DECLARE_TRANS_FUNC(def_trans_cmta);
DECLARE_TRANS_FUNC(def_trans_mm_account);
DECLARE_TRANS_FUNC(def_trans_sec_type);
DECLARE_TRANS_FUNC(def_trans_open_close);
DECLARE_TRANS_FUNC(def_trans_strike);
DECLARE_TRANS_FUNC(def_trans_call_put);
DECLARE_TRANS_FUNC(def_trans_expire_day);
DECLARE_TRANS_FUNC(def_trans_expire_my);
DECLARE_TRANS_FUNC(def_rom2fix_no_op);
DECLARE_TRANS_FUNC(def_opt_account_trans);
DECLARE_TRANS_FUNC(def_trans_opt_capacity);
DECLARE_TRANS_FUNC(def_trans_symbol_and_suffix);
DECLARE_TRANS_FUNC(def_trans_acct_and_store);

int def_init_translators(orderbook * ob, size_t nfunc);

void from_rom_func(struct message *mess, void *ob);

void set_option_rom2fix(orderbook * ob);
void from_rom_helper_func(void *r, struct message *mess, void *b);
#define min(x,y) (((int) (x) < (int) (y)) ? (x) : (y))

#endif/*__ROM2FIX_H__*/
