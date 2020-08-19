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
static char fixserversection[] = "FIX SERVER";
static char *defroutinst = 0;
static int def_route_len = 0;
static char defroutinstkey[] = "DefRouteInst";
static char execinstkey[] = "ExecInst";
static char *execinst = 0;
static hashmap_t clearing_accounts;

static char *nomura_fix_generate_id(size_t * nlen)
{
    char *cl_ord_id = 0;
    if (nlen != 0) {
        int retval = 0;
        cl_ord_id = calloc(DART_EX_TAG_LEN, 1);
        if ((retval = create_order_token(idgenerator, cl_ord_id,
                                         DART_EX_TAG_LEN - 1)) > 0)
            *nlen = (size_t) retval;
        else
            *nlen = 0;
    }
    return cl_ord_id;
}

hashmap_t load_clearing_ids(const char* filename)
{
    hashmap_t h = create_map(16);
    gen_build_clear_id_map(filename, h);
    return h;
}
/* This function only translates FIX tag 58 (Text) fields that have at
 * least one non-numeric character. For some reason Nomura sends a
 * timestamp in the text field. We only want to send actual text
 * strings back in ROM field 26.
 */
static int nomura_fix2rom_trans_text(char *fixmsg,
                                     dart_rom_pos * positions, char mtype,
                                     char *status, databuf_t * rom,
                                     size_t romfield, int *rejected,
                                     dart_ex_order_obj * order,
                                     pthread_mutex_t * mutex,
                                     hashmap_t ord_map)
{
    int retval = 0;
    const char *text = fixmsg + positions[romfield].beg;
    const size_t textlen =
        positions[romfield].end - positions[romfield].beg;
    if (textlen > 0) {
        size_t i;
        int alldigits = 1;
        for (i = 0; i < textlen; ++i) {
            if (isdigit(text[i]) == 0) {
                alldigits = 0;
                break;
            }
        }
        if (alldigits == 0) {
            /* Copy the output. */
            retval = databuf_memcpy(rom, text, textlen);
        }
    }
    return retval;
}

static int insti_account(char *rom, romindex_t * map, char *type,
                         databuf_t * fix, size_t field, int *rejected,
                         dart_ex_order_obj * order)
{
    int retval = 0;
    if (map[field].name != 0) {
        char *acct = rom + map[field].beg, *bookval = 0;
        long len = map[field].end - map[field].beg;
        size_t acctlen = 0;
        if (len > 0 && rom[0] == 'E') {
            char* clid = rom + map[11].beg;
            int clid_len = map[11].end - map[11].beg;
            if((clid_len == 3 && strncmp("695", clid, 3) == 0) ||
                     (clid_len == 4 && strncmp("0695", clid, 4) == 0)) {
                char newacc[30];
                memset(newacc, '\0', 30);
                memcpy(newacc, "ABN_", 4);
                memcpy(newacc + 4, acct, len);
                doa_set_account(order, newacc, (size_t)(len + 4));
                retval =
                    fixmsg_addstr_bylen(fix, map[field].name, newacc, (len + 4));
            } else if((clid_len >= 3 && (strncmp("005", clid, 3) == 0)) ||
                      (clid_len == 4 && strncmp("0005", clid, 4) == 0)) { 
                char* rom_val= 0;
                int found =
                    find_n_fill(clearing_accounts, acct, len, (void *) &rom_val);
                if(found) {
                    len = found;
                    acct = rom_val;
                }
                char newacc[30];
                memset(newacc, '\0', 30);
                memcpy(newacc, "GSC_", 4);
                memcpy(newacc + 4, acct, len);
                doa_set_account(order, newacc, (size_t)(len + 4));
                retval =
                    fixmsg_addstr_bylen(fix, map[field].name, newacc, (len + 4));
            } else {
                acctlen = (size_t) len;
                doa_set_account(order, acct, acctlen);
                retval =
                    fixmsg_addstr_bylen(fix, map[field].name, acct, acctlen);
            }
        } else {
            acct = bookval = doa_get_account(order, &acctlen);
            if (acctlen > 0) {
                retval =
                    fixmsg_addstr_bylen(fix, map[field].name, acct, acctlen);
            }
        }
        free(bookval);
    }
    return retval;
}
static int nomura_translate_short_lender(char *rom, romindex_t * map,
                                       char *fixtype, databuf_t * fix,
                                       size_t romfield, int *rejected,
                                       dart_ex_order_obj * order)
{
    int result = 0;
    if (map[romfield].name != 0) {
        char *p = rom + map[romfield].beg;
        size_t len = map[romfield].end - map[romfield].beg;
        if (len > 0) {
            char *p = rom + map[romfield].beg;
            if((len == 3 && strncmp("695", p, 3) == 0) ||
                    (len == 4 && strncmp("0695", p, 4) == 0)) {
                p = "ABNA";
                len = 4;
            }
            doa_set_short_locate(order,p, len);
            result =
                fixmsg_addstr2(fix, map[romfield].name, map[romfield].len, p,
                               len);
        } else {
            p = doa_get_short_locate(order, &len);
            if(len > 0) {
                fixmsg_addstr_bylen(fix, "5700", p, len);
                free(p);
            }
        }
    }
    return result;
}
static int nomura_translate_end(char *rom, romindex_t * map,
                                       char *fixtype, databuf_t * fix,
                                       size_t romfield, int *rejected,
                                       dart_ex_order_obj * order)
{
    int result = 0;
    char *p = rom + map[romfield].beg;
    size_t len = map[romfield].end - map[romfield].beg;
    if(len > 0) {
        doa_set_end_time(order, p, len);
            result =
                fixmsg_addstr2(fix, map[romfield].name, map[romfield].len, p,
                               len);
    } else if(rom[0] != 'E' && rom[0] != 'C') {
        p = doa_get_end_time(order);
        if(p != NULL) {
            result =
                fixmsg_addstr2(fix, map[romfield].name, map[romfield].len, p,
                               17);
        }
    }
    return result;
}

static int nomura_translate_begin(char *rom, romindex_t * map,
                                       char *fixtype, databuf_t * fix,
                                       size_t romfield, int *rejected,
                                       dart_ex_order_obj * order)
{
    int result = 0;
    char *p = rom + map[romfield].beg;
    size_t len = map[romfield].end - map[romfield].beg;
    if(len > 0) {
        doa_set_begin_time(order, p, len);
            result =
                fixmsg_addstr2(fix, map[romfield].name, map[romfield].len, p,
                               len);
    } else if(rom[0] != 'E' && rom[0] != 'C') {
        p = doa_get_begin_time(order);
        if(p != NULL) {
            result =
                fixmsg_addstr2(fix, map[romfield].name, map[romfield].len, p,
                               17);
        }
    }
    return result;
}

static int nomura_translate_instructions(char *rom, romindex_t * map,
                                       char *fixtype, databuf_t * fix,
                                       size_t romfield, int *rejected,
                                       dart_ex_order_obj * order)
{
    int result = 0;
    char *p = rom + map[romfield].beg;
    size_t len = map[romfield].end - map[romfield].beg;
    char* algotype = 0;
    size_t alen = 0;
    if(rom[0] == 'E') {
        algotype = rom + map[73].beg;
        alen = map[73].end - map[73].beg;
    } else if(rom[0] != 'E' && alen <= 0) {
        algotype = doa_get_dest_route(order, &alen);    
    }
    if (len > 0 && alen <= 0) {
        result = fixmsg_addstr_bylen(fix, "100", p, len);
        doa_set_dest_route(order, p, len);
    } else if(rom[0] == 'E' && alen <= 0) {
        result = fixmsg_addstr(fix, "100", defroutinst);
        doa_set_dest_route(order, defroutinst, def_route_len);
    } else if(alen <= 0) {
        p = doa_get_dest_route(order, &len);
        if(len > 0) {
            fixmsg_addstr_bylen(fix, "100", p, len);
            free(p);
        }
    } else if(rom[0] == 'E' && alen > 0) {
        char* wap = rom + map[73].beg;
        if(wap[0] == '1' && alen == 1) {
            result = fixmsg_addstr_bylen(fix, "100", "TWAPG", 5);
            doa_set_dest_route(order, "TWAPG", 5);
        } else if(wap[0] == '3') {
            result = fixmsg_addstr_bylen(fix, "100", "VWAPX3", 6);
            doa_set_dest_route(order, "VWAPX3", 6);
        } else {
            if(alen == 2 && wap[0] == '1' && wap[1] == '0') {
                result = fixmsg_addstr_bylen(fix, "100", "NHAWKG", 6);
                doa_set_dest_route(order, "NHAWKG", 6);
            } else {
                result = fixmsg_addstr_bylen(fix, "100", "VWAPG", 5);
                doa_set_dest_route(order, "VWAPG", 5);
            }
            if(len > 0) {
                char* off = memchr(p, '=', len);
                if(off) {
                    if (len >= 3 && p[0] == 'M' && p[1] == 'V' && p[2] == 'P') {
                        /* MVP is Maximum Volume Percentage. */
                        off += 1;
                        double d = atof(off);
                        int i = 0;
                        if (d < 1.0) {
                            d *= 100.0;
                        }
                        i = (int) d;
                        if (i != 0) {
                            result = databuf_write(fix, "%c9244=%d", '\x01', (int) d);
                        }
                        return result;
                    } 
                }
            }
        }
        if(len > 0) {
            result = fixmsg_addstr_bylen(fix, "9245", p, len);
        }
    } else if(rom[0] != 'E' && alen > 0){
        fixmsg_addstr_bylen(fix, "100", algotype, alen);
    }
    return result;
}
static int nomura_custom_init(ex_config * exchange_config,
                              orderbook * pbook)
{
    int result = -1;
    idgenerator = init_order_token_creator();
    if (idgenerator != 0) {
        int len = 0;
        defroutinst =
            get_val_for_tag(exchange_config->dc, fixserversection,
                            sizeof fixserversection - 1,
                            defroutinstkey, sizeof defroutinstkey - 1,
                            &def_route_len);
        if (defroutinst == 0) {
            printf("Default Routing Instruction missing from "
                   "configuration, defaulting to 'NOMURA'\n");
            defroutinst = "APOG";
        }
        execinst =
            get_val_for_tag(exchange_config->dc, fixserversection,
                            sizeof fixserversection - 1,
                            execinstkey, sizeof execinstkey - 1, &len);

        pbook->rom2fix[5] = def_trans_symbol_and_suffix;
        pbook->rom2fix[8] = def_trans_ordtype;
        pbook->rom2fix[17] = def_field_noop;
        pbook->rom2fix[15] = nomura_translate_instructions;
        pbook->rom2fix[58] = nomura_translate_short_lender;
        pbook->rom2fix[12] = insti_account;
        pbook->rom2fix[77] = nomura_translate_begin;
        pbook->rom2fix[76] = nomura_translate_end;
        pbook->fix2rom[26] = nomura_fix2rom_trans_text;

        clearing_accounts = load_clearing_ids("GSFlip.map");

        result = 0;
    }

    return result;
}

/* Set SenderSubID (50) with first 4 chars of clearing account; set
   HandlInst (21) to "1". */
static int nomura_pre_translate(char *rom, romindex_t * map, char *type,
                                databuf_t * fix, size_t field,
                                int *rejected, dart_ex_order_obj * order)
{
    int ret = fixmsg_addstr(fix, "21", "1");
    return  ret;
}

static const char configfile[] = "nomura.dat";

int generic_fix_interface_main(int argc, char *argv[],
                               struct fix_interface *handler)
{
    int result = -1;
    if (handler != 0) {
        handler->config_file = configfile;
        handler->file_len = sizeof configfile - 1;
        handler->onstart = nomura_custom_init;
        handler->pre_translate_func = nomura_pre_translate;
        handler->id_generator = nomura_fix_generate_id;
        result = 0;
    }
    return result;
}
