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
static char datestr[9] = { '\0' };

static int
initialize_date_string(void)
{
    int retval = -1;
    time_t now = time(&now);
    struct tm *now_tm = gmtime(&now);
    if (now_tm != 0) {
        int nwritten = sprintf(datestr, "%04d%02d%02d",
                               now_tm->tm_year + 1900,
                               now_tm->tm_mon + 1,
                               now_tm->tm_mday);
        if (nwritten <= sizeof datestr) {
            retval = 0;
        }
    }
    return retval;
}

static char*
knight_fix_generate_id(size_t * nlen)
{
    char *clordid = 0;
    if (nlen != 0) {
        int retval = 0;
        size_t total_clordid_len = DART_EX_TAG_LEN + sizeof datestr;
        clordid = calloc(total_clordid_len, 1);
        if (clordid != 0) {
            memcpy(clordid, datestr, 8);
            if ((retval =
                 create_order_token(idgenerator, clordid + 8,
                                    total_clordid_len - 9)) > 0) {
                *nlen = (size_t) retval + 8;
            } else {
                *nlen = 0;
            }
        }
    }
    return clordid;
}

static int
knight_translate_capacity(char *rom, romindex_t * map, char *fixtype,
                          databuf_t * fix, size_t romfield,
                          int *rejected, dart_ex_order_obj * order)
{
    int retval = -1;
    if (map[romfield].name != 0) {
        char *sec_type_ptr = rom + map[35].beg;
        long sec_type_len = map[35].end - map[35].beg;

        char *capacity_ptr = rom + map[romfield].beg;
        long capacity_len = map[romfield].end - map[romfield].beg;

        char *p = 0, *bookval = 0;
        size_t n = 0;

        if (capacity_len > 0) {
            doa_set_capacity(order, capacity_ptr, (size_t) capacity_len);
            p = capacity_ptr;
            n = capacity_len;
        } else {
            p = bookval = doa_get_capacity(order, &n);
        }

        if (sec_type_len == 1 && sec_type_ptr[0] == 'O'
            && capacity_len == 1 && capacity_ptr[0] == 'B') {
            p = "F";
            n = 1;
        }

        retval = fixmsg_addstr_bylen(fix, map[romfield].name, p, n);

        free(bookval);
    } else {
        /* Not mapping capacity */
        retval = 0;
    }
    return retval;
}

static int
knight_translate_orderid(char *rom, romindex_t * map,
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

static int
knight_translate_clearing_id(char *rom, romindex_t *map,
                             char *fixtype, databuf_t *fix,
                             size_t romfield, int *rejected,
                             dart_ex_order_obj *order)
{
    int retval = -1;
    if (map[romfield].name != 0) {
        char *clearing_id = rom + map[romfield].beg, *bookval = 0;
        long rom_field_len = map[romfield].end - map[romfield].beg;
        size_t idlen = 0;
        if (rom_field_len > 0) {
            idlen = (size_t) rom_field_len;
            doa_set_account(order, clearing_id, idlen);
        } else {
            bookval = clearing_id = doa_get_account(order, &idlen);
        }

        if (idlen > 0 && clearing_id != 0) {
            retval = fixmsg_addstr_bylen(fix, map[romfield].name,
                                         clearing_id, idlen);
        }

        free(bookval);

    } else {
        retval = 0;
    }

    return retval;
}

static int
knight_store_exchange(char *rom, romindex_t * map,
                      char *fixtype, databuf_t * fix,
                      size_t romfield, int *rejected,
                      dart_ex_order_obj * order)
{
    if (map[13].beg != map[13].end) {
        doa_set_destination(order, rom + map[13].beg,
                            map[13].end - map[13].beg);
    }
    return 0;
}

static int
knight_translate_short_lender(char *rom, romindex_t * map,
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
            retval =
                fixmsg_addstr_bylen(fix, map[romfield].name, lender, len);
        }
        free(bookval);
    }
    return retval;
}

static int
knight_custom_init(ex_config *exchange_config, orderbook *pbook)
{
    int result = -1;
    if (initialize_date_string() == 0) {
        idgenerator = init_order_token_creator();
        if (idgenerator != 0) {
            if ((result = def_init_translators(pbook, ROM_LEN)) == -1) {
                fprintf(stderr,
                        "Error initializing default translators\n");
            }
            set_option_rom2fix(pbook);
            pbook->rom2fix[16] = knight_translate_orderid;
            pbook->rom2fix[10] = knight_translate_capacity;
            pbook->rom2fix[12] = knight_translate_clearing_id;
            pbook->rom2fix[13] = knight_store_exchange;
            pbook->rom2fix[58] = knight_translate_short_lender;
            pbook->fix2rom[26] = def_map_fix_text_to_rom;
            result = 0;
        }
    }

    return result;
}

/* Set HandlInst (21) to "1". */
static int
knight_pre_translate(char *rom, romindex_t * map, char *type,
                     databuf_t * fix, size_t field,
                     int *rejected, dart_ex_order_obj * order)
{
    return fixmsg_addstr(fix, "21", "1");
}

int generic_fix_interface_main(int argc, char *argv[],
                               struct fix_interface *handler)
{
    int result = -1;
    if (handler != 0) {
        handler->config_file = "knightopt.dat";
        handler->file_len = strlen(handler->config_file);
        handler->onstart = knight_custom_init;
        handler->pre_translate_func = knight_pre_translate;
        handler->id_generator = knight_fix_generate_id;
        result = 0;
    }
    return result;
}
