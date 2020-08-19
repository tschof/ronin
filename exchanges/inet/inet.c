/**************************************************************************
 * Copyright (c) 2010 by DART Executions
 *
 * All Rights Reserved
 *************************************************************************/

#include "order_token_creator.h"
#include "config_loader.h"
#include "fixmsg.h"
#include "rom2fix.h"
#include "ex_config_helper.h"
#include "generic_fix_interface.h"
#include "ex_common.h"
#include "suffixmap.h"
#include "stringutil.h"
#include "dart_constants.h"
#include <errno.h>
#include <string.h>
#include <ctype.h>

static token_struct_t *idgenerator = 0;
static char fixserversection[] = "FIX SERVER";
static char *defroutinst = 0;
static hashmap_t clearing_ids;
static hashmap_t clearing_accounts;
static char defroutinstkey[] = "DefRouteInst";
static suffixmap_t *suffixmap = 0;
//static char clearingfirmkey[] = "ClearingFirmName";

static void oc_check_time_n_rej(char *beg_off, int len, char *l_fix_tag,
                             databuf_t * output, int *rejected,
                             char *mtype, int oc)
{
    struct tm *new_time;
    int cur_hour = 0;
    int cur_min = 0;
    struct tm lm;
    time_t long_time;
    time(&long_time);
    new_time = localtime_r(&long_time, &lm);
    cur_hour = lm.tm_hour;
    cur_min = lm.tm_min;
    //This works only in the eastern time zone:
    if (oc == 1 &&  (cur_hour >= 15 && cur_min >= 50)) {
        int rej_len = 0;
        char *l_message =
            "MOC and LOC orders on NASDAQ cannot be replaced or cancelled after 3:50 ET.";
        databuf_reset(output);
        if (mtype[0] == 'D') {
        } else {
            rej_len =
                build_cnl_rpl_rej(beg_off, len, l_fix_tag, output,
                                  l_message);
        }
        *rejected = 1;
    } else if(oc == 0 && (cur_hour == 9 && cur_min >= 28) &&
              (cur_hour == 9 && cur_min < 30)) {
        int rej_len = 0;
        char *l_message =
            "MOO and LOO orders on NASDAQ cannot be replaced or cancelled after 9:28 ET.";
        databuf_reset(output);
        rej_len =
            build_cnl_rpl_rej(beg_off, len, l_fix_tag, output,
                              l_message);
        *rejected = 1;
    }

}
static char *inet_fix_generate_id(size_t * nlen)
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

/* Looks for a forward-slash character which separates a symbol from
 * its suffix.  If there is one, the symbol is split with the left
 * side going into FIX tag 55 and the right into FIX tag 65 after
 * being translated through the suffix map.  If there is none, the
 * whole thing is put in FIX tag 55.
 */
static int inet_translate_symbol(char *rom, romindex_t * map, char *type,
                                 databuf_t * fix, size_t field,
                                 int *rejected, dart_ex_order_obj * order)
{
    int result = -1;
    char *origsymbol = rom + map[field].beg;
    size_t symlen = map[field].end - map[field].beg;
    if (symlen != 0) {
        set_val_in_order_struct(origsymbol, symlen, field, order, type);
    } else {
        origsymbol = doa_get_symbol(order, &symlen);
    }

    if (symlen > 0 && origsymbol != 0) {
        char *pslash = memchr(origsymbol, '/', symlen);
        if (pslash == 0) {
            result = fixmsg_addstr2(fix, map[field].name, map[field].len,
                                    origsymbol, symlen);
        } else {
            const int leftlen = pslash - origsymbol;
            const char *newsuffix = 0;
            if (lookupsuffix2
                (suffixmap, pslash + 1, symlen - leftlen - 1,
                 &newsuffix) == 0) {
                result =
                    databuf_write(fix, "%c%s=%.*s%s", '\x01',
                                  map[field].name, leftlen, origsymbol,
                                  newsuffix);
            }
        }
        result = 0;
    }

    if (origsymbol != (rom + map[field].beg)) {
        free(origsymbol);
    }

    return result;
}

static int allow_market_maker_pegs = 0;

static int inet_translate_ordtype(char *rom, romindex_t * map,
                                  char *fixtype, databuf_t * fix,
                                  size_t romfield, int *rejected,
                                  dart_ex_order_obj * order)
{
    int retval = -1;
    char *p = rom + map[romfield].beg;
    size_t len = map[romfield].end - map[romfield].beg;
    int delete_p = 0;

    /* The only mapping for order type is for Peg orders; pegs are
       indicated by an execution instruction (ROM field 57). */
    if ((map[57].end - map[57].beg) == 1) {
        switch (*(rom + map[57].beg)) {
        case 'Q':
            if (!allow_market_maker_pegs) {
                databuf_reset(fix);
                databuf_write(fix,
                              "Market maker pegs not allowed (ExecInst=Q)");
                *rejected = 2;
                return -1;
            }
        case 'P':
        case 'M':
        case 'R':
            p = "P";
            len = 1;
            break;
        }
    } else if (len == 1 && p[0] == '5') {
        /* Market on close */
        p = "C";
        doa_set_sec_def(order, p, 1);
        p = "1";
    } else if (len == 2 && p[0] == '1' && p[1] == '1') {
        /* Limit on close */
        p = "C";
        doa_set_sec_def(order, p, 1);
        p = "2";
        len = 1;
    }

    if (len < 1) {
        p = doa_get_type(order, &len);
        delete_p = 1;
    } else {
        doa_set_type(order, p, len);
    }

    retval =
        fixmsg_addstr2(fix, map[romfield].name, map[romfield].len, p, len);

    if (delete_p) {
        free(p);
    }

    return retval;
}

static int inet_translate_tif(char *rom, romindex_t * map,
                              char *fixtype, databuf_t * fix,
                              size_t romfield, int *rejected,
                              dart_ex_order_obj * order)
{
    int result = -1;
    char *p = rom + map[romfield].beg;
    size_t len = map[romfield].end - map[romfield].beg;
    char *ordtype = rom + map[8].beg;
    size_t ordtypelen = map[8].end - map[8].beg;
    if (ordtypelen == 1 && ordtype[0] == '5') {
        /* Market on close */
        p = "C";
        doa_set_sec_def(order, p, 1);
        p = "7";
    } else if (ordtypelen == 2 && ordtype[0] == '1' && ordtype[1] == '1') {
        /* Limit on close */
        p = "C";
        doa_set_sec_def(order, p, 1);
        p = "7";
        len = 1;
    }

    if (len < 1) {
        p = doa_get_tif(order, &len);
    } else {
        doa_set_tif(order, p, len);
        if(len == 1 && p[0] == '2') {
            doa_set_sec_def(order, "O", 1);
        }
    }

    result =
        fixmsg_addstr2(fix, map[romfield].name, map[romfield].len, p, len);

    if(fixtype[0] != 'D') {
        len = 0;
        p = 0;
        p = doa_get_sec_def(order, &len);
        if(len == 1 && p[0] == 'O') {
            oc_check_time_n_rej((rom + map[romfield].beg), len, fixtype, fix, rejected, fixtype, 0);
        } else if(len == 1 && p[0] == 'C') {
            oc_check_time_n_rej((rom + map[romfield].beg), len, fixtype, fix, rejected, fixtype, 1);
        }
        if(len > 0 && p != NULL) {
            free(p);
        }
    }
    return result;
}

static int inet_translate_instructions(char *rom, romindex_t * map,
                                       char *fixtype, databuf_t * fix,
                                       size_t romfield, int *rejected,
                                       dart_ex_order_obj * order)
{
    int result = -1;
    char *p = rom + map[romfield].beg;
    size_t len = map[romfield].end - map[romfield].beg;
    if (len > 0) {
        char *dash = memchr(p, '-', len);
        if (dash == 0) {
            result = fixmsg_addstr2(fix, "76", 2, p, len);
        } else {
            result = fixmsg_addstr2(fix, "76", 2, p, dash - p);
            if (result != -1) {
                char *end = p + len;
                p = dash + 1;
                if (p < end) {
                    int nwritten =
                        fixmsg_addstr2(fix, "9355", 4, p, end - p);
                    if (nwritten != -1) {
                        result += nwritten;
                    } else {
                        result = -1;
                    }
                    if(p[0] == 'C') {
                        doa_set_sec_def(order, p, 1);
                    } else if(p[0] == 'O') {
                        doa_set_sec_def(order, p, 1);
                    }
                    if(result >=0 && fixtype[0] != 'D' && p[0] == 'O') {
                        oc_check_time_n_rej((rom + map[romfield].beg), len, fixtype, fix, rejected, fixtype, 0);
                    } else if(result >=0 && fixtype[0] != 'D' && p[0] == 'C') {
                        oc_check_time_n_rej((rom + map[romfield].beg), len, fixtype, fix, rejected, fixtype, 1);
                    }
                }
            }
        }
    } else {
        result = 0;
        if(fixtype[0] != 'D') {
            len = 0;
            p = doa_get_sec_def(order, &len);
            if(len == 1 && p[0] == 'O') {
                oc_check_time_n_rej((rom + map[romfield].beg), len, fixtype, fix, rejected, fixtype, 0);
            } else if(len == 1 && p[0] == 'C') {
                oc_check_time_n_rej((rom + map[romfield].beg), len, fixtype, fix, rejected, fixtype, 1);
            }
            if(len > 0 && p != NULL) {
                free(p);
            }
        }
    }
    return result;
}

int inet_trans_execinst(char *rom, romindex_t * map, char *type,
                       databuf_t * fix, size_t field, int *rejected,
                       dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        char *p = rom + map[field].beg;
        if (len > 0 && p != 0 && p[0] != '\0') {
            if (memchr(p, 'f', len) != 0) {
                databuf_reset(fix);
                *rejected = 1;
                result =
                    create_rom_reject(rom, fix,
                                      "ISO Sweep orders not allowed");
            } else {
                result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
                int inst_len = map[15].end - map[15].beg;
                if(inst_len == 0) {
                    char* inst = 0;
                    switch (p[0]) {
                    case 'P':
                    case 'M':
                    case 'R':
                        inst = "INET";
                        inst_len = 4;
                        break;
                    }
                    if(inst_len > 0) {
                        fixmsg_addstr2(fix, "76",
                                      2, inst, inst_len);
                    }
                }
            }
        }
    }
    return result;
}
static int inet_translate_exdest(char *rom, romindex_t * map,
                                 char *fixtype, databuf_t * fix,
                                 size_t romfield, int *rejected,
                                 dart_ex_order_obj * order)
{
    int result = 0;
    const char *p = rom + map[romfield].beg;
    size_t len = map[romfield].end - map[romfield].beg;
    if ((map[57].end - map[57].beg) == 1) {
        /* All peg orders must be INET-only */
        switch (*(rom + map[57].beg)) {
        case 'P':
        case 'M':
        case 'R':
            p = "INET";
            len = 4;
            break;
        }
    } else if (((map[15].end - map[15].beg)) == 0) {
        /* Translate the ROM EX_DEST field if the INSTRUCTIONS field
           is empty, otherwise the INSTRUCTIONS will be parsed and
           translated. */
        if (len == 1) {
            if (p[0] == 'N' || p[0] == 'A') {
                p = "DOTI";
                len = 4;
            } else if (p[0] == 'O') {
                p = "STGY";
                len = 4;
            }
        } else if (len == 7 && strncmp("Direct+", p, 7) == 0) {
            p = "DOTI";
            len = 4;
        } else {
            p = defroutinst;
            len = strlen(defroutinst);
        }

        {
            const char *tif = rom + map[9].beg;
            const size_t tiflen = map[9].end - map[9].beg;
            if (tiflen == 1 && tif[0] == '2') {
                /* TIF is OPG */
                result = fixmsg_addstr2(fix, "9355", 4, "O", 1);
            }
        }
    }

    if (result != -1 && len > 0) {
        int nwritten = fixmsg_addstr2(fix, map[romfield].name,
                                      map[romfield].len, p, len);
        if (nwritten != -1) {
            result += nwritten;
        }
    }

    return result;
}

static int inet_translate_maxfloor(char *rom, romindex_t * map,
                                   char *fixtype, databuf_t * fix,
                                   size_t romfield, int *rejected,
                                   dart_ex_order_obj * order)
{
    int result = 0;
    const size_t nmaxfloor = map[romfield].end - map[romfield].beg;
    if (nmaxfloor != 0) {
        const char *pmaxfloor = rom + map[romfield].beg;
        if (nmaxfloor == 1 && pmaxfloor[0] == '0') {
            result = fixmsg_addstr2(fix, "9140", 4, "N", 1);
        } else {
            result =
                fixmsg_addstr2(fix, map[romfield].name, map[romfield].len,
                               pmaxfloor, nmaxfloor);
        }
    } else {
        /* Midpoint peg orders must also be hidden (9140=N) */
        const size_t nexecinst = map[57].end - map[57].beg;
        if (nexecinst == 1) {
            int disp = map[63].end - map[63].beg;
            const char *pexecinst = rom + map[57].beg;
            if (pexecinst[0] == 'M' && disp <= 0) {
                result = fixmsg_addstr2(fix, "9140", 4, "N", 1);
            }
        }
    }
    return result;
}

static int inet_translate_abnamro_clearing_account(char *rom,
                                                   romindex_t * map,
                                                   char *fixtype,
                                                   databuf_t * fix,
                                                   size_t romfield,
                                                   int *rejected,
                                                   dart_ex_order_obj *
                                                   order)
{
    int result = -1;
    char *p = rom + map[romfield].beg;
    size_t len = map[romfield].end - map[romfield].beg;
    int nwrite = 0;
    if (len == 0) {
        p = doa_get_account(order, &len);
    } else {
        doa_set_account(order, p, len);
    }
    nwrite = min(4, len);
    if (nwrite > 0) {
        result = fixmsg_addstr2(fix, "50", 2, p + len - nwrite, nwrite);
        if (result != -1) {
            int nwritten = fixmsg_addstr2(fix, map[romfield].name,
                                          map[romfield].len, p, len);
            if (nwritten > -1) {
                result += nwritten;
            }
        }
    }
    if ((p != 0) && (p != (rom + map[romfield].beg))) {
        free(p);
    }
    return result;
}

static int inet_translate_gs_clearing_account(char *rom, romindex_t * map,
                                           char *fixtype, databuf_t * fix,
                                           size_t romfield, int *rejected,
                                           dart_ex_order_obj * order)
{
    int result = -1;
    char *p = rom + map[romfield].beg;
    size_t len = map[romfield].end - map[romfield].beg;
    int nwrite = 0;
    int delete = 0;
    if (len == 0) {
        p = doa_get_account(order, &len);
        delete = 1;
    } else {
        char* rom_val= 0;
        int found =
            find_n_fill(clearing_accounts, p, len, (void *) &rom_val);
        if(found) {
            len = found;
            p = rom_val;
        }
        doa_set_account(order, p, len);
    }
    nwrite = min(4, len);
    if (nwrite > 0) {
        result = fixmsg_addstr2(fix, "50", 2, p, nwrite);
        if (result != -1) {
            int nwritten = fixmsg_addstr2(fix, map[romfield].name,
                                          map[romfield].len, p, len);
            if (nwritten > -1) {
                result += nwritten;
            }
        }
    }
    if (delete) {
        free(p);
    }
    return result;
}
static int inet_translate_clearing_account(char *rom, romindex_t * map,
                                           char *fixtype, databuf_t * fix,
                                           size_t romfield, int *rejected,
                                           dart_ex_order_obj * order)
{
    int result = 0;
    char* p = 0;
    size_t len = map[11].end - map[11].beg;
    if (len > 0) {
        p = rom + map[11].beg;
        doa_set_clr_id(order, p, len);
    } else {
        p = doa_get_clr_id(order, &len);
    }
    if(len > 0) {
        char* rom_val= 0;
        int found =
            find_n_fill(clearing_ids, p, len, (void *) &rom_val);
        if(found) {
            databuf_write(fix, "%c109=", FIX_FIELD_DELIM);
            databuf_memcpy(fix, rom_val, found);
        } 

        if(strncmp("695", p, 3) == 0 ||
                (len == 4 && strncmp("0695", p, 4) == 0)) {
            inet_translate_abnamro_clearing_account(rom, map, fixtype, fix,
                                                    romfield, rejected, order);
        } else if(strncmp("501", p, 3) == 0 || (strncmp("005", p, 3) == 0) ||
                (len == 4 && strncmp("0501", p, 4) == 0) ||
                 (len == 4 && strncmp("0005", p, 4) == 0)) {
            inet_translate_gs_clearing_account(rom, map, fixtype, fix,
                                               romfield, rejected, order);
        } else {
            char* acct = rom + map[romfield].beg;
            int should_del = 0;
            len = map[romfield].end - map[romfield].beg;
            if (len == 0) {
                acct = doa_get_account(order, &len);
                should_del = 1;
            } else {
                doa_set_account(order, acct, len);
            }
            if(len > 0) {
                result = fixmsg_addstr2(fix, "50", 2, acct, len);
                result = fixmsg_addstr2(fix, map[romfield].name,
                                        map[romfield].len, acct, len);
            }
            if(should_del) {
                free(acct);
            }
        }
    }
    return result;
}

int translate_reject_codes(char *fixmsg, dart_rom_pos * positions,
                           char mtype, char *status,
                           databuf_t * rom, size_t romfield,
                           int *rejected, dart_ex_order_obj * order,
                           pthread_mutex_t * mutex, hashmap_t ord_map)
{
    ssize_t ntext = positions[romfield].end - positions[romfield].beg;
    if (ntext == 0) {
        return 0;
    } else if (ntext < 0) {
        return -1;
    }
    char *ptext = fixmsg + positions[romfield].beg;
    filtercommas(ptext, ntext, rom, " ", 1);
    if (ntext == 1) {
        /* If the text is a single character, then it's probably a
         * standard cancel or reject code. The full text needs to
         * looked-up. Determine whether this message is in response to
         * a reject or a cancel. */
        char *pstatus = fixmsg + positions[14].beg;
        ssize_t nstatus =
            positions[romfield].end - positions[romfield].beg;
        if (nstatus == 1) {
            char reason = ptext[0];
            ptext = 0;
            if (pstatus[0] == '4') {
                /* Order is canceled */
                switch (reason) {
                case 'U':
                    ptext = "User requested cancel";
                    break;
                case 'I':
                    ptext = "Immediate or cancel order";
                    break;
                case 'T':
                    ptext = "Timeout";
                    break;
                case 'S':
                    ptext = "Supervisory cancel";
                    break;
                case 'D':
                    ptext = "Regulatory restriction";
                    break;
                case 'Q':
                    ptext = "Self match protection";
                    break;
                }
            } else if (pstatus[0] == '8') {
                /* Order is rejected */
                switch (reason) {
                case 'Y':
                    ptext = "LocateReqd tag not set to N";
                    break;
                case 'C':
                    ptext = "Rash is not yet open";
                    break;
                case 'I':
                    ptext = "Invalid side";
                    break;
                case 'E':
                    ptext = "Invalid peg type";
                    break;
                case 'L':
                    ptext = "Invalid firm";
                    break;
                case 'Z':
                    ptext = "Order size more than maximum shares allowed";
                    break;
                case 'O':
                    ptext = "Unknown error";
                    break;
                case 'F':
                    ptext = "Invalid marketability value";
                    break;
                case 'B':
                    ptext = "Quote not available for pegged order";
                    break;
                case 'P':
                    ptext = "Pegging is not allowed";
                    break;
                case 'X':
                    ptext = "Order does not have valid price";
                    break;
                case 'R':
                    ptext = "Routing not enabled";
                    break;
                case 'G':
                    ptext =
                        "Specified routing destination is temporarily down";
                    break;
                case 'J':
                    ptext = "Internal NASDAQ error";
                    break;
                case 'N':
                    ptext =
                        "Routing destination does not support order attribute";
                    break;
                case 'D':
                    ptext = "Bad reserve value";
                    break;
                case 'T':
                    ptext = "Port is in test only mode";
                    break;
                case 'H':
                    ptext = "Stock is halted";
                    break;
                case 'S':
                    ptext = "Invalid stock";
                    break;
                case 'Q':
                    ptext = "Invalid order qty";
                    break;
                case 'K':
                    ptext = "Invalid min qty";
                    break;
                case 'W':
                    ptext = "Routing destination not supported";
                    break;
                case 'A':
                    ptext = "Advanced functionality not supported";
                    break;
                case 'U':
                    ptext = "Reject resent orders";
                    break;
                case 'V':
                    ptext = "Invalid order type";
                    break;
                }
            }
        }
        if (ptext != 0 && databuf_write(rom, ":%s", ptext) == -1) {
            return -1;
        }
    }
    return 0;
}

static const char mmpegflagkey[] = "AllowMarketMakerPeg";

static int inet_custom_init(ex_config * exchange_config, orderbook * pbook)
{
    int result = -1;
    /* Initialize suffix map */
    if (buildsuffixmap(&suffixmap, "RomToCMSSuffix.map") != 0) {
        fprintf(stderr, "Error loading suffix map: %s\n", strerror(errno));
        return -1;
    }

    idgenerator = init_order_token_creator();
    if (idgenerator != 0) {
        int len = 0;
        defroutinst =
            get_val_for_tag(exchange_config->dc, fixserversection,
                            sizeof fixserversection - 1, defroutinstkey,
                            sizeof defroutinstkey - 1, &len);
        if (defroutinst == 0) {
            printf("Default Routing Instruction missing from "
                   "configuration, defaulting to 'INET'\n");
            defroutinst = "INET";
        }

        clearing_ids = load_clearing_ids("IDtomnemonic.map");
        clearing_accounts = load_clearing_ids("GSFlip.map");
        char *mmpegflagstr = get_val_for_tag(exchange_config->dc,
                                             fixserversection,
                                             sizeof fixserversection - 1,
                                             mmpegflagkey,
                                             sizeof mmpegflagkey - 1,
                                             &len);
        if (len > 0 && mmpegflagstr != 0) {
            int state = 1;
            const char *p;
            for (p = mmpegflagstr; *p != 0; ++p) {
                if (isspace(*p)) {
                    continue;
                }
                switch (state) {
                case 1:
                    if (*p == 'Y' || *p == 'y') {
                        allow_market_maker_pegs = 1;
                    } else if (*p == 'N' || *p == 'n') {
                        allow_market_maker_pegs = 0;
                    } else {
                        fprintf(stderr,
                                "Bad value '%s' for key %s, only Y/N allowed\n",
                                mmpegflagstr, mmpegflagkey);
                        free(mmpegflagstr);
                        return -1;
                    }
                    state = 2;
                    break;
                case 2:
                    if (!isspace(*p)) {
                        fprintf(stderr,
                                "Bad value '%s' for key %s, only Y/N allowed\n",
                                mmpegflagstr, mmpegflagkey);
                        free(mmpegflagstr);
                        return -1;
                    }
                    break;
                }
            }
            free(mmpegflagstr);
        } else {
            allow_market_maker_pegs = 0;
        }

        pbook->rom2fix[4] = def_trans_side;
        pbook->rom2fix[5] = inet_translate_symbol;
        pbook->rom2fix[8] = inet_translate_ordtype;
        pbook->rom2fix[9] = inet_translate_tif;
        pbook->rom2fix[12] = inet_translate_clearing_account;
        pbook->rom2fix[15] = inet_translate_instructions;
        //pbook->rom2fix[29] = inet_translate_exdest;
        pbook->rom2fix[34] = inet_translate_maxfloor;
        pbook->rom2fix[57] = inet_trans_execinst;

        pbook->fix2rom[26] = translate_reject_codes;
/*
        {
            char *clearing_firm_name = get_val_for_tag(exchange_config->dc,
                                                       fixserversection,
                                                       sizeof
                                                       fixserversection -
                                                       1,
                                                       clearingfirmkey,
                                                       sizeof
                                                       clearingfirmkey - 1,
                                                       &len);
            if (clearing_firm_name != 0 && len > 0
                && (strstr(clearing_firm_name, "abn") != 0
                    || strstr(clearing_firm_name, "ABN") != 0)) {
                (void)
                    printf
                    ("Using ABN AMRO clearing account translation\n");
                pbook->rom2fix[12] =
                    inet_translate_abnamro_clearing_account;
            } else {
                (void)
                    printf
                    ("Using Goldman Sachs clearing account translation\n");
            }

            free(clearing_firm_name);
        }
*/
        result = 0;
    }

    return result;
}

/* Set SenderSubID (50) with first 4 chars of clearing account; set
   HandlInst (21) to "1". */
static int inet_pre_translate(char *rom, romindex_t * map,
                              char *type, databuf_t * fix,
                              size_t field, int *rejected,
                              dart_ex_order_obj * order)
{
    return fixmsg_addstr2(fix, "21", 2, "1", 1);
}

int generic_fix_interface_main(int argc, char *argv[],
                               struct fix_interface *handler)
{
    int result = -1;
    if (handler != 0) {
        handler->config_file = "inet.dat";
        handler->file_len = 8;
        handler->onstart = inet_custom_init;
        handler->pre_translate_func = inet_pre_translate;
        handler->id_generator = inet_fix_generate_id;
        result = 0;
    }
    return result;
}
