/**************************************************************************
 * Copyright (c) 2010 by DART Executions
 *
 * All Rights Reserved
 *************************************************************************/


#include <string.h>
#include "order_token_creator.h"
#include "config_loader.h"
#include "fixmsg.h"
#include "rom2fix.h"
#include "ex_config_helper.h"
#include "generic_fix_interface.h"
#include "ex_common.h"
#include <errno.h>
#include <ctype.h>

static token_struct_t *idgenerator = 0;

static char *csalgo_fix_generate_id(size_t * nlen)
{
    char *clordid = 0;
    if (nlen != 0) {
        int retval = 0;
        clordid = calloc(DART_EX_TAG_LEN, 1);
        if (clordid != 0) {
            if ((retval =
                 create_order_token(idgenerator, clordid,
                                    DART_EX_TAG_LEN - 1)) > 0) {
                *nlen = (size_t) retval;
            } else {
                *nlen = 0;
            }
        }
    }
    return clordid;
}

static int csalgo_translate_clearing_account(char *rom, romindex_t * map,
                                             char *fixtype,
                                             databuf_t * fix,
                                             size_t romfield,
                                             int *rejected,
                                             dart_ex_order_obj * order)
{
    int result = 0;
    if (map[romfield].name != 0) {
        char *p = rom + map[romfield].beg;
        size_t len = map[romfield].end - map[romfield].beg;
        if (len == 0) {
            p = doa_get_account(order, &len);
        } else {
            doa_set_account(order, p, len);
        }
        result =
            fixmsg_addstr_bylen(fix, map[romfield].name, p, (int) len);
        if ((p != 0) && (p != (rom + map[romfield].beg))) {
            free(p);
        }
    }
    return result;
}

typedef int (*algo_map_function) (char *rom, char *key, size_t keylen,
                                  char *val, size_t vallen,
                                  databuf_t * fix, int *rejected);

static int csalgo_map_algo_parameters(char *rom, char *key, size_t keylen,
                                      char *val, size_t vallen,
                                      databuf_t * fix, int *rejected)
{
    int retval = 0;
    if (keylen == 3 && key[0] == 'M' && key[1] == 'V' && key[2] == 'P') {
        /* MVP is Maximum Volume Percentage. */
        double d = atof(val);
        int i = 0;
        if (d < 1.0) {
            d *= 100.0;
        }
        i = (int) d;
        if (i != 0) {
            retval = databuf_write(fix, "%c6064=%d", '\x01', (int) d);
        }
    } else {
        retval = -1;
        /* Nothing else is currently supported, reject the order. */
        if (rejected != 0 && databuf_reset(fix) == 0) {
            *rejected = 1;
            retval =
                (int) create_rom_reject(rom, fix,
                                        "Unsupported algo parameter");
        }
    }
    return retval;
}

static int parse_algo_instructions(char *rom, char *p, size_t len,
                                   databuf_t * fix, int *rejected,
                                   algo_map_function mapfunc)
{
    int retval = 0;
    if (p != 0 && len > 0) {
        /* This ROM field can map to multiple FIX fields so the
           map[romfield].name from the configuration is ignored. */

        /* The instructions field is comprised of a
           space-separated tag=value pairs. */

        enum instr_parse_states { start, key, keyend, valstart, val };
        char *end = p + len;
        char *pkey = 0, *pval = 0;
        size_t keylen = 0, vallen = 0;
        enum instr_parse_states state = start;
        for (; p <= end; ++p) {
            switch (state) {
            case start:
                if (isspace(*p) == 0) {
                    state = key;
                    pkey = p;
                }
                break;
            case key:
                if (*p == '=') {
                    state = valstart;
                    keylen = p - pkey;
                } else if (isspace(*p) != 0) {
                    state = keyend;
                    keylen = p - pkey;
                }
                break;
            case keyend:
                if (isspace(*p) != 0) {
                    state = valstart;
                }
                break;
            case valstart:
                if (isspace(*p) == 0) {
                    pval = p;
                    state = val;
                }
                break;
            case val:
                if (isspace(*p) != 0 || p == end) {
                    int nwritten;
                    state = start;
                    vallen = p - pval;
                    nwritten = mapfunc(rom, pkey, keylen, pval,
                                       vallen, fix, rejected);
                    if (nwritten > -1) {
                        retval += nwritten;
                    } else {
                        retval = -1;
                        goto on_error;
                    }
                }
                break;
            default:
                retval = -1;
                goto on_error;
            }
        }
    }

  on_error:
    return retval;
}

static int csalgo_translate_instructions(char *rom, romindex_t * map,
                                         char *fixtype, databuf_t * fix,
                                         size_t romfield, int *rejected,
                                         dart_ex_order_obj * order)
{
    int retval = 0;
    if (map[romfield].name != 0) {
        retval = parse_algo_instructions(rom, rom + map[romfield].beg,
                                         map[romfield].end -
                                         map[romfield].beg, fix, rejected,
                                         csalgo_map_algo_parameters);
    }
    return retval;
}

static int csalgo_translate_orderid(char *rom, romindex_t * map,
                                    char *fixtype, databuf_t * fix,
                                    size_t romfield, int *rejected,
                                    dart_ex_order_obj * order)
{
    int retval = 0;
    if (map[romfield].name != 0) {
        if (fixtype != 0 && fixtype[0] != 'D') {
            size_t exchtaglen;
            char *exchange_tag = doa_get_exchange_tag(order, &exchtaglen);
            if (exchtaglen > 0 && exchange_tag != 0) {
                retval =
                    fixmsg_addstr(fix, map[romfield].name, exchange_tag);
            }
            free(exchange_tag);
        }
    }
    return retval;
}

static int csalgo_maxvolpct_alternate_mapping(char *rom, romindex_t * map,
                                              char *fixtype,
                                              databuf_t * fix,
                                              size_t romfield,
                                              int *rejected,
                                              dart_ex_order_obj * order)
{
    int retval = 0;
    if (map[romfield].name != 0) {
        signed long int mfieldlen = map[romfield].end - map[romfield].beg;
        if (mfieldlen > 0) {
            size_t n = (size_t) mfieldlen;
            char *p = rom + map[romfield].beg;
            retval =
                csalgo_map_algo_parameters(rom, "MVP", 3, p, n, fix,
                                           rejected);
        }
    }
    return retval;
}

static int csalgo_translate_currency(char *rom, romindex_t * map,
                                     char *fixtype, databuf_t * fix,
                                     size_t romfield, int *rejected,
                                     dart_ex_order_obj * order)
{
    int retval = 0;
    if (map[romfield].name != 0) {
        signed long int msg_field_len =
            map[romfield].end - map[romfield].beg;
        char *p = rom + map[romfield].beg;
        size_t n = 0;
        if (msg_field_len > 0) {
            n = (size_t) msg_field_len;
        } else {
            p = "USD";
            n = 3;
        }
        retval = fixmsg_addstr_bylen(fix, map[romfield].name, p, (int) n);
    }
    return retval;
}

static int csalgo_translate_short_lender(char *rom, romindex_t * map,
                                         char *fixtype, databuf_t * fix,
                                         size_t romfield, int *rejected,
                                         dart_ex_order_obj * order)
{
    int retval = 0;
    if (map[romfield].name != 0) {
        char *lender = rom + map[romfield].beg, *bookval = 0;
        signed long int tmplen = map[romfield].end - map[romfield].beg;
        size_t len = 0;
        if (tmplen > 0) {
            len = (size_t) tmplen;
        }
        if (len > 0) {
            doa_set_short_locate(order, lender, len);
        } else {
            lender = bookval = doa_get_short_locate(order, &len);
        }
        if (len > 0) {
            if ((len == 3 && lender[0] == '5' && lender[1] == '0'
                 && lender[2] == '1')
                || (len == 4 && lender[0] == '0' && lender[1] == '5'
                    && lender[2] == '0' && lender[3] == '1')) {
                lender = "GSCO";
                len = 4;
            }
            retval =
                fixmsg_addstr_bylen(fix, map[romfield].name, lender,
                                    (int) len);
        }
        free(bookval);
    }
    return retval;
}

static int csalgo_translate_algotype(char *rom, romindex_t * map,
                                     char *fixtype, databuf_t * fix,
                                     size_t romfield, int *rejected,
                                     dart_ex_order_obj * order)
{
    int retval = 0;
    if (map[romfield].name != 0) {
        signed long int field_len = map[romfield].end - map[romfield].beg;
        char *p = rom + map[romfield].beg;
        if (field_len == 1 && p[0] == '2') {
            /* This is a VWAP, let it through. */
            retval = fixmsg_addstr(fix, map[romfield].name, "1");
        } else if (field_len < 1) {
            /* Empty ALGOTYPE field, only algorithm
               orders allowed this destination. */
            retval =
                (int) create_rom_reject(rom, fix,
                                        "Only ALGO orders allowed this destination");
            *rejected = 1;
        } else {
            /* Invalid/unsupported ALGOTYPE. */
            retval =
                (int) create_rom_reject(rom, fix,
                                        "Invalid ALGO TYPE; only VWAP allowed");
            *rejected = 1;
        }
    }
    return retval;
}

static int csalgo_test_datetime(char *rom, romindex_t * map, char *type,
                                databuf_t * fix, size_t field,
                                int *rejected, dart_ex_order_obj * order)
{
    int result = 0;
    size_t len = map[field].end - map[field].beg;
    if (len > 0) {
        char *p = rom + map[field].beg;
        set_val_in_order_struct(p, len, (int) field, order, type);
        result = fixmsg_addstr_bylen(fix, map[field].name, p, (int) len);
    }
    return result;
}

static int csalgo_fixrom_symbol(char *fixmsg, dart_rom_pos * positions,
                                char mtype, char *status,
                                databuf_t * rom, size_t romfield,
                                int *rejected, dart_ex_order_obj * order,
                                pthread_mutex_t * mutex, hashmap_t ord_map)
{
    int retval = -1;
    size_t nlen = 0;
    char *symbol = doa_get_symbol(order, &nlen);
    if (symbol != 0 && nlen > 0) {
        retval = databuf_write(rom, "%.*s", (int) nlen, symbol);
        free(symbol);
    }
    return retval;
}


static int csalgo_custom_init(ex_config * exchange_config,
                              orderbook * pbook)
{
    int result = -1;

    idgenerator = init_order_token_creator();
    if (idgenerator != 0) {
        if ((result = def_init_translators(pbook, ROM_LEN)) == -1) {
            fprintf(stderr, "Error initializing default translators\n");
        }

        /* pbook->rom2fix[ 0] = csalgo_reject_cancel_replace; */
        pbook->rom2fix[5] = def_trans_symbol_and_suffix;
        pbook->rom2fix[12] = csalgo_translate_clearing_account;
        pbook->rom2fix[15] = csalgo_translate_instructions;
        pbook->rom2fix[16] = csalgo_translate_orderid;
        pbook->rom2fix[26] = csalgo_maxvolpct_alternate_mapping;
        pbook->rom2fix[56] = csalgo_translate_currency;
        pbook->rom2fix[58] = csalgo_translate_short_lender;
        pbook->rom2fix[73] = csalgo_translate_algotype;
        pbook->rom2fix[77] = csalgo_test_datetime;

        pbook->fix2rom[5] = csalgo_fixrom_symbol;
        pbook->fix2rom[26] = def_map_fix_text_to_rom;

        result = 0;
    }

    return result;
}

/* Set HandlInst (21) to "1". */
static int csalgo_pre_translate(char *rom, romindex_t * map, char *type,
                                databuf_t * fix, size_t field,
                                int *rejected, dart_ex_order_obj * order)
{
    int retval = -1;
    if (type[0] == 'G') {
        *rejected = 1;
        if (databuf_reset(fix) == 0) {
            *rejected = 1;
            retval =
                (int) create_rom_reject(rom, fix,
                                        "Cancel/Replace not "
                                        "allowed this destination");
        }
    } else {
        retval = fixmsg_addstr(fix, "57", "execsvcs");
        if (retval > -1) {
            int nwritten = fixmsg_addstr(fix, "21", "1");
            if (nwritten > -1) {
                retval += nwritten;
            }
        }
    }
    return retval;
}

int generic_fix_interface_main(int argc, char *argv[],
                               struct fix_interface *handler)
{
    int result = -1;
    if (handler != 0) {
        handler->config_file = "csalgo.dat";
        handler->onstart = csalgo_custom_init;
        handler->pre_translate_func = csalgo_pre_translate;
        handler->id_generator = csalgo_fix_generate_id;
        result = 0;
    }
    return result;
}
