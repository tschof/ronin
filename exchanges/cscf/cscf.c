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

static char *cscf_fix_generate_id(size_t * nlen)
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

static int cscf_translate_orderid(char *rom, romindex_t * map,
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

static int cscf_translate_currency(char *rom, romindex_t * map,
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

static int cscf_translate_short_lender(char *rom, romindex_t * map,
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

static int cscf_fixrom_symbol(char *fixmsg, dart_rom_pos * positions,
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

static int cscf_custom_init(ex_config * exchange_config, orderbook * pbook)
{
    int result = -1;

    idgenerator = init_order_token_creator();
    if (idgenerator != 0) {
        if ((result = def_init_translators(pbook, ROM_LEN)) == -1) {
            fprintf(stderr, "Error initializing default translators\n");
        }

        pbook->rom2fix[5] = def_trans_symbol_and_suffix;
        pbook->rom2fix[16] = cscf_translate_orderid;
        pbook->rom2fix[56] = cscf_translate_currency;
        pbook->rom2fix[58] = cscf_translate_short_lender;

        pbook->fix2rom[5] = cscf_fixrom_symbol;
        pbook->fix2rom[26] = def_map_fix_text_to_rom;

        result = 0;
    }

    return result;
}

/* Set HandlInst (21) to "1". */
static int cscf_pre_translate(char *rom, romindex_t * map, char *type,
                              databuf_t * fix, size_t field, int *rejected,
                              dart_ex_order_obj * order)
{
    return fixmsg_addstr(fix, "21", "1");
}

int generic_fix_interface_main(int argc, char *argv[],
                               struct fix_interface *handler)
{
    int result = -1;
    if (handler != 0) {
        handler->config_file = "cscf.dat";
        handler->onstart = cscf_custom_init;
        handler->pre_translate_func = cscf_pre_translate;
        handler->id_generator = cscf_fix_generate_id;
        result = 0;
    }
    return result;
}
