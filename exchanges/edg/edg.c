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
#include "dart_constants.h"
#include "ex_common.h"
#include <errno.h>
#include <ctype.h>

static token_struct_t *idgenerator = 0;
static char fixserversection[] = "FIX SERVER";
static char *defroutinst = 0;
static char defroutinstkey[] = "DefRouteInst";
static char execinstkey[] = "ExecInst";
static char *execinst = 0;
static hashmap_t clearing_ids;

static char datestr[9] = { '\0' };

hashmap_t load_clearing_ids(const char* filename)
{
    hashmap_t h = create_map(5);
    gen_build_clear_id_map(filename, h);
    return h;
}
static int initialize_date_string(void)
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

static char *edg_fix_generate_id(size_t * nlen)
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

int edg_trans_ordtype(char *rom, romindex_t * map,
                      char *fixtype, databuf_t * fix,
                      size_t romfield, int *rejected,
                      dart_ex_order_obj * order)
{
    int retval = -1;
    if (map[romfield].name != 0) {
        char *p = rom + map[romfield].beg, *bookval = 0;
        long len = map[romfield].end - map[romfield].beg;
        size_t n = 0;
        if (len > 0) {
            n = (size_t) len;
            doa_set_type(order, p, n);
        } else {
            p = bookval = doa_get_type(order, &n);
        }

        /* The only mapping for order type is for Peg orders; pegs are
           indicated by an execution instruction (ROM field 57). */
        if ((map[57].end - map[57].beg) == 1) {
            switch (*(rom + map[57].beg)) {
            case 'P':
            case 'M':
            case 'R':
                p = "P";
                n = 1;
                break;
            }
        } else if (n == 2 && p[0] == '1' && p[1] == '1') {
            /* Translate ROM Limit on Close to FIX LOC (11=>B) */
            p = "B";
            n = 1;
        }
        retval = fixmsg_addstr_bylen(fix, map[romfield].name, p, n);
        free(bookval);
    } else {
        retval = 0;
    }

    return retval;
}

int set_extended_hours_eligibility(char *fixtype, databuf_t * fix)
{
    int retval = 0;
    if (fixtype[0] == 'D' || fixtype[0] == 'G') {
        retval = fixmsg_addstr(fix, "9206", "B");
    }
    return retval;
}

int edg_translate_orderid(char *rom, romindex_t * map,
                          char *fixtype, databuf_t * fix,
                          size_t romfield, int *rejected,
                          dart_ex_order_obj * order)
{
    int retval = 0;
    char* ty = rom + map[8].beg;
    long ty_len = map[8].end - map[8].beg;
    if(ty_len > 0 && (ty[0] != '3' && ty[0] != '4')) { 
        set_extended_hours_eligibility(fixtype, fix);
    }
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

int edg_translate_clearing_id(char *rom, romindex_t * map,
                              char *fixtype, databuf_t * fix,
                              size_t romfield, int *rejected,
                              dart_ex_order_obj * order)
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

int edg_translate_instructions(char *rom, romindex_t * map,
                               char *fixtype, databuf_t * fix,
                               size_t romfield, int *rejected,
                               dart_ex_order_obj * order)
{
    int retval = 0;
    if (map[romfield].name != 0) {
        char *instr = rom + map[romfield].beg;
        long field_len = map[romfield].end - map[romfield].beg;
        if (field_len == 1 && instr[0] == 'A') {
            retval = fixmsg_addstr(fix, "9303", "P");
        } else if (field_len == 1 && instr[0] == 'N') {
            retval = fixmsg_addstr(fix, "9303", "N");
        } else if (field_len > 0) {
            retval = fixmsg_addstr_bylen(fix, map[romfield].name,
                                         instr, (size_t) field_len);
        }
    }
    return retval;
}

int edg_translate_minqty(char *rom, romindex_t * map,
                         char *fixtype, databuf_t * fix,
                         size_t romfield, int *rejected,
                         dart_ex_order_obj * order)
{
    int retval = 0;
    /* Only send MinQty on New Order - Single. */
    if (map[romfield].name != 0 && fixtype != 0 && fixtype[0] == 'D') {
        char *minqty = rom + map[romfield].beg;
        long len = map[romfield].end - map[romfield].beg;
        size_t minqtylen = 0;
        if (len > 0) {
            int is_zero = 1;
            size_t i = 0;
            minqtylen = (size_t) len;
            for (i = 0; i < minqtylen && isdigit(minqty[i]) != 0; ++i) {
                if (minqty[i] != '0') {
                    is_zero = 0;
                    break;
                }
            }
            if (!is_zero) {
                /* Only set non-zero MinQty values. */
                retval =
                    fixmsg_addstr_bylen(fix, map[romfield].name, minqty,
                                        minqtylen);
            }
        }
    }
    return retval;
}

int edg_translate_sectype(char *rom, romindex_t * map,
                          char *fixtype, databuf_t * fix,
                          size_t romfield, int *rejected,
                          dart_ex_order_obj * order)
{
    int retval = 0;
    if (map[romfield].name != 0) {
        char *sectype = rom + map[romfield].beg;
        long diff = map[romfield].end - map[romfield].beg;
        if (diff > 0) {
            size_t sectypelen = (size_t) diff;
            if (sectypelen == 1 && sectype[0] == 'E') {
                sectype = "CS";
                sectypelen = 2;
            }
            retval = fixmsg_addstr_bylen(fix, map[romfield].name,
                                         sectype, sectypelen);
        }
    }
    return retval;
}

int edg_translate_short_lender(char *rom, romindex_t * map,
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
                fixmsg_addstr_bylen(fix, map[romfield].name, lender, len);
        }
        free(bookval);
    }
    return retval;
}


int edg_fixrom_symbol(char *fixmsg, dart_rom_pos * positions,
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

int edg_fixrom_status_xlate(char *fixmsg, dart_rom_pos * positions,
                            char mtype, char *status,
                            databuf_t * rom, size_t romfield,
                            int *rejtected, dart_ex_order_obj * order,
                            pthread_mutex_t * mutex, hashmap_t ord_map)
{
    char *beg_off = fixmsg;
    size_t len = (positions[romfield].end - positions[romfield].beg);
    beg_off += positions[romfield].beg;
    if (romfield == 14 && mtype != 'U') {
        if (len == 1 && beg_off[0] == '3') {
            /* If this is also an OPG order, then we will mark this order as
               being dead. */
            size_t len;
            char *ordtype = doa_get_type(order, &len);
            if (len == 1 && ordtype != 0 && ordtype[0] == '2') {
                databuf_write(rom, "%s", "4");
            }
            free(ordtype);
        } else {
            len = determine_trans_type(romfield, beg_off, rom, len);
            status = fixmsg;
            status += positions[romfield].beg;
        }
    } else if (romfield == 14 && mtype == 'U') {
        len = set_status_for_cnrej(fixmsg, positions, status, rom, order);
    }
    return 1;
}


static int edg_custom_init(ex_config * exchange_config, orderbook * pbook)
{
    int result = -1;

    if (initialize_date_string() == 0) {
        idgenerator = init_order_token_creator();
        if (idgenerator != 0) {
            int len = 0;
            defroutinst =
                get_val_for_tag(exchange_config->dc, fixserversection,
                                sizeof fixserversection - 1,
                                defroutinstkey, sizeof defroutinstkey - 1,
                                &len);
            if (defroutinst == 0) {
                printf("Default Routing Instruction missing from "
                       "configuration, defaulting to 'EDG'\n");
                defroutinst = "EDG";
            }
            execinst =
                get_val_for_tag(exchange_config->dc, fixserversection,
                                sizeof fixserversection - 1, execinstkey,
                                sizeof execinstkey - 1, &len);

            if ((result = def_init_translators(pbook, ROM_LEN)) == -1) {
                fprintf(stderr,
                        "Error initializing default translators\n");
            }

            clearing_ids = load_clearing_ids("IDtomnemonic.map");
            /* The OrdType affects the session list so we'll do a custom OrdType
               translation and do the session translation at the same time. */
            pbook->rom2fix[5] = def_trans_symbol_and_suffix;
            pbook->rom2fix[16] = edg_translate_orderid;
            pbook->rom2fix[12] = edg_translate_clearing_id;
            pbook->rom2fix[15] = edg_translate_instructions;
            pbook->rom2fix[18] = edg_translate_minqty;
            pbook->rom2fix[35] = edg_translate_sectype;
            pbook->rom2fix[58] = edg_translate_short_lender;

            pbook->fix2rom[5] = edg_fixrom_symbol;
            pbook->fix2rom[14] = edg_fixrom_status_xlate;
            pbook->fix2rom[26] = def_map_fix_text_to_rom;

            result = 0;
        }
    }

    return result;
}

/* Set HandlInst (21) to "1". */
static int edg_pre_translate(char *rom, romindex_t * map, char *type,
                             databuf_t * fix, size_t field, int *rejected,
                             dart_ex_order_obj * order)
{
    char* p = 0;
    size_t len = map[11].end - map[11].beg;
    int delete_me = 0;
    if (len > 0) {
        p = rom + map[11].beg;
        doa_set_clr_id(order, p, len);
    } else {
        p = doa_get_clr_id(order, &len);
        delete_me = 1;
    }
    if(len > 0) {
        char* rom_val= 0;
        int found =
            find_n_fill(clearing_ids, p, len, (void *) &rom_val);
        if(found) {
            databuf_write(fix, "%c50=", FIX_FIELD_DELIM);
            databuf_memcpy(fix, rom_val, found);
        }
        if(delete_me) {
            free(p);
        }
    } 
    return fixmsg_addstr(fix, "21", "1");
}

int generic_fix_interface_main(int argc, char *argv[],
                               struct fix_interface *handler)
{
    int result = -1;
    if (handler != 0) {
        handler->config_file = "edg.dat";
        handler->onstart = edg_custom_init;
        handler->pre_translate_func = edg_pre_translate;
        handler->id_generator = edg_fix_generate_id;
        result = 0;
    }
    return result;
}
