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
static hashmap_t clearing_accounts;
static int is_canada = 0;
static hashmap_t algomapping = 0;

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
hashmap_t load_clearing_ids(const char* filename)
{
    hashmap_t h = create_map(16);
    gen_build_clear_id_map(filename, h);
    return h;
}

static char *knight_fix_generate_id(size_t * nlen)
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

int
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
int knight_translate_tif(char *rom, romindex_t * map,
                                 char *fixtype, databuf_t * fix,
                                 size_t romfield, int *rejected,
                                 dart_ex_order_obj * order)
{
    int sfree = 0;
    int ret = 0;
    char* dest = rom + map[13].beg;
    int len = map[13].end - map[13].beg;
    if(len <= 0) {
        size_t dlen = 0;
        dest = doa_get_destination(order, &dlen);
        len = (int)dlen;
        sfree = len > 0 ? 1 : 0;
    }
    if(len > 0 && strncmp(dest, "278", 3) == 0) {
        set_val_in_order_struct("5", 1, romfield, order, fixtype);
        ret = 
            fixmsg_addstr2(fix, map[romfield].name, map[romfield].len, "5",
                               1);
    } else {
        ret =  def_rom2fix(rom, map, fixtype,
                fix, romfield, rejected,order);
    }
    if(sfree) {
        free(dest);
    }
    return ret;
}

int knight_translate_clearing_id(char *rom, romindex_t * map,
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
            char* rom_val= 0;
            int found =
                find_n_fill(clearing_accounts, clearing_id, rom_field_len, (void *) &rom_val);
            if(found) {
                rom_field_len = found;
                clearing_id = rom_val;
            }
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
int knight_can_sym(char *rom, romindex_t * map,
                          char *fixtype, databuf_t * fix,
                          size_t romfield, int *rejected,
                          dart_ex_order_obj * order)
{
    int result = -1;
    /* Is this field mapped? */
    if (map[romfield].name != 0) {
        char *p = map[romfield].beg + rom, *booksym = 0;
        size_t n = 0;
        long msgsymlen = map[romfield].end - map[romfield].beg;
        char buffer[64];
        if (msgsymlen > 0) {
            n = (size_t) msgsymlen;
            doa_set_symbol(order, p, n);
        } else {
            p = booksym = doa_get_symbol(order, &n);
        }
        if (n < sizeof buffer) {
            char *suffix = 0;
            char* slash = 0;
            memcpy(buffer, p, n);
            buffer[n] = '\0';
            if ((suffix = strrchr(buffer, '.')) != 0) {
                *suffix++ = '\0';
                fixmsg_addstr(fix, "100", suffix);
            }
            if ((slash = strchr(buffer, '/')) != 0) {
                *slash++ = '\0';
                fixmsg_addstr(fix, "65", slash);
            }
            result = fixmsg_addstr(fix, map[romfield].name, buffer);
        } else {
            /* Reject the order. */
        }
        free(booksym);
    } else {
        result = 0;
    }
    return result;
}

int knight_store_exchange(char *rom, romindex_t * map,
                          char *fixtype, databuf_t * fix,
                          size_t romfield, int *rejected,
                          dart_ex_order_obj * order)
{
    if (map[13].beg != map[13].end) {
        doa_set_destination(order, rom + map[13].beg,
                            map[13].end - map[13].beg);
    }
    char* dest = rom + map[13].beg;
    int len = map[13].end - map[13].beg;
    if(len > 0 && (strncmp(dest, "278", 3) == 0 ||
                   strncmp(dest, "378", 3) == 0 ||
                   strncmp(dest, "678", 3) == 0)) {
        fixmsg_addstr(fix, "57", "ALGO");
        fixmsg_addstr(fix, "6101", "FAN");
    } else {
        size_t dlen = 0;
        char* p = doa_get_destination(order, &dlen);
        if(dlen > 0) {
            if(strncmp(p, "278", 3) == 0 ||
               strncmp(p, "378", 3) == 0 ||
               strncmp(dest, "678", 3) == 0) {
                fixmsg_addstr(fix, "57", "ALGO");
                fixmsg_addstr(fix, "6101", "FAN");
            }
            free(p);
        }
    }
    return 0;
}

typedef int (*algo_map_function) (char *rom, char *key, size_t keylen,
                                  char *val, size_t vallen,
                                  databuf_t * fix, int *rejected);

static int knight_map_algo_parameters(char *rom, char *key, size_t keylen,
                                      char *val, size_t vallen,
                                      databuf_t * fix, int *rejected)
{
    int retval = 0;
    void *result = 0;
    char errmsg[256] = { 0 };
    if (algomapping != 0) {
        int fixtaglen;
        if ((fixtaglen = get_map_entry(algomapping, key, (int)
                                       keylen, &result)) > 0) {
            char fixtagbuf[16] = { 0 };
            if (fixtaglen > (sizeof fixtagbuf - 1)) {
                retval = -1;
                sprintf(errmsg, "Internal error");
                goto done;
            }
            memcpy(fixtagbuf, result, fixtaglen);
            retval = fixmsg_addstr_bylen(fix, fixtagbuf, val, vallen);
        } else {
            *rejected = 1;
            sprintf(errmsg, "Unsupported algo parameter: %.*s",
                    (int) keylen, key);
            errmsg[sizeof errmsg - 1] = '\0';
        }
    } else {
        *rejected = 1;
        sprintf(errmsg, "Algo mapping not initialized");
        errmsg[sizeof errmsg - 1] = '\0';
    }

    if (*rejected != 0) {
        if (errmsg[0] == '\0') {
            sprintf(errmsg, "Error processing algo fields");
            errmsg[sizeof errmsg - 1] = '\0';
        }
        databuf_reset(fix);
        retval = (int) create_rom_reject(rom, fix, errmsg);
    }

  done:
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

static int knight_translate_instructions(char *rom, romindex_t * map,
                                         char *fixtype, databuf_t * fix,
                                         size_t romfield, int *rejected,
                                         dart_ex_order_obj * order)
{
    int retval = 0;
    if (map[romfield].name != 0) {
        retval = parse_algo_instructions(rom, rom + map[romfield].beg,
                                         map[romfield].end -
                                         map[romfield].beg, fix, rejected,
                                         knight_map_algo_parameters);
    }
    return retval;
}

int
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
knight_translate_algotype(char *rom, romindex_t * map,
                          char *fixtype, databuf_t * fix,
                          size_t romfield, int *rejected,
                          dart_ex_order_obj * order)
{
    int retval = 0;
    if (map[romfield].name != 0) {
        char *p = rom + map[romfield].beg;
        ptrdiff_t plen = map[romfield].end - map[romfield].beg;
        if (plen == 1) {
            const char *algoname = 0;
            switch (p[0]) {
            case '4':
                algoname = "arrival-price";
                break;
            case '1':
                algoname = "TWAP";
                break;
            case '2':
                algoname = "VWAP";
                break;
            case '5':
                algoname = "CATCH";
                break;
            case '6':
                algoname = "OPPORTUNISTIC";
                break;
            case '7':
                algoname = "COVERT";
                break;
            case '8':
                algoname = "FAN";
                break;
            }
            if (algoname != 0) {
                retval = fixmsg_addstr(fix, "6101", algoname);
            }
        }
    }
    return retval;
}

int knight_fixrom_symbol(char *fixmsg, dart_rom_pos * positions,
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

int knight_fixrom_status_xlate(char *fixmsg, dart_rom_pos * positions,
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

struct algofields_t {
    const char *str, *fixtag;
};
static const struct algofields_t algofields[] = {
    {"oo", "6206"},
    {"OO", "6206"},
    {"oc", "6207"},
    {"OC", "6207"},
    {"iwould", "6208"},
    {"IWOULD", "6208"},
    {"mvp", "6209"},
    {"MVP", "6209"},
    {"style", "6210"},
    {"STYLE", "6210"},
    {"10b-18", "6212"},
    {"10B-18", "6212"},
    {"msp", "6258"},
    {"MSP", "6258"},
    {"ds", "6260"},
    {"DS", "6260"},
    {"dmq", "6261"},
    {"DMQ", "6261"},
    {"dur", "6263"},
    {"DUR", "6263"},
    {"ARA", "6285"},
    {"ara", "6285"},
    {"MV", "6288"},
    {"mv", "6288"},
    {"STY", "6287"},
    {"sty", "6287"},
};

static int knight_initalgomappings(void)
{
    int retval = -1;
    size_t i, nmappings = sizeof algofields / sizeof algofields[0];
    algomapping = create_map(256);
    if (algomapping == 0) {
        goto done;
    }
    for (i = 0; i < nmappings; ++i) {
        if (insert_map
            (algomapping, algofields[i].str, strlen(algofields[i].str),
             algofields[i].fixtag, strlen(algofields[i].fixtag)) == 0) {
            break;
        }
    }
    retval = 0;
  done:
    return retval;
}

const char *onbehalf = 0;
int onbehalf_length = 0;
const char *exdest = 0;
int exdest_length = 0;
static char fixserversection[] = "FIX SERVER";

static int knight_custom_init(ex_config * exchange_config,
                              orderbook * pbook)
{
    int result = -1;

    if (initialize_date_string() == 0) {
        idgenerator = init_order_token_creator();
        if (idgenerator != 0) {
            if ((result = def_init_translators(pbook, ROM_LEN)) == -1) {
                fprintf(stderr,
                        "Error initializing default translators\n");
            }

            if ((result = knight_initalgomappings()) < 0) {
                (void) fprintf(stderr,
                               "Error initializing algo mappings\n");
            }

            pbook->rom2fix[16] = knight_translate_orderid;
            pbook->rom2fix[12] = knight_translate_clearing_id;
            pbook->rom2fix[13] = knight_store_exchange;
            pbook->rom2fix[9] = knight_translate_tif;
            pbook->rom2fix[15] = knight_translate_instructions;
            pbook->rom2fix[58] = knight_translate_short_lender;
            pbook->rom2fix[73] = knight_translate_algotype;
            pbook->fix2rom[26] = def_map_fix_text_to_rom;

            result = 0;
        }
    }

    clearing_accounts = load_clearing_ids("GSFlip.map");
    const char *onbehalfsection = "OnBehalfOfCompID";
    onbehalf =
        get_val_for_tag(exchange_config->dc, fixserversection,
                        sizeof fixserversection - 1, onbehalfsection,
                        strlen(onbehalfsection), &onbehalf_length);
    const char *exdestsection = "ExDest";
    exdest =
        get_val_for_tag(exchange_config->dc, fixserversection,
                        sizeof fixserversection - 1, exdestsection,
                        strlen(exdestsection), &exdest_length);
    const char* isCanada = "Canadian";
    int wlen = 0;
    char* what = get_val_for_tag(exchange_config->dc, fixserversection,
                        sizeof fixserversection - 1, isCanada,
                        strlen(isCanada), &wlen);
    if(wlen > 0 && what[0] == '1') {
        is_canada = 1;
        pbook->rom2fix[5] = knight_can_sym;
    }
    return result;
}

#define CSV_ALGOTYPE 73
#define csv_field_len(map, idx) ((map)[(idx)].end - (map)[(idx)].beg)

/* Set HandlInst (21) to "1". */
static int knight_pre_translate(char *rom, romindex_t * map, char *type,
                                databuf_t * fix, size_t field,
                                int *rejected, dart_ex_order_obj * order)
{
    int result = -1;
    if ((result = fixmsg_addstr(fix, "21", "1")) == -1) {
        goto done;
    }

    ssize_t algotypelen = csv_field_len(map, CSV_ALGOTYPE);
    if (algotypelen == 1) {
        const char *algotypeptr = rom + map[CSV_ALGOTYPE].beg;
        if (algotypeptr[0] != '0'
            && fixmsg_addstr(fix, "57", "ALGO") == -1) {
            goto done;
        }
    }
    if(is_canada) {
        fixmsg_addstr_bylen(fix, "57", "ALGO", 4);
        fixmsg_addstr_bylen(fix, "15", "CAD", 3);
        fixmsg_addstr(fix, "6101", "FAN");
        //fixmsg_addstr(fix, "22", "4");
    }

    int nwritten;
    if (onbehalf_length > 0 && onbehalf != 0) {
        if ((nwritten =
             fixmsg_addstr_bylen(fix, "115", onbehalf,
                                 onbehalf_length)) != -1) {
            result = 0;
        } else {
            result = -1;
            goto done;
        }
    }

    if (exdest_length > 0 && exdest != 0) {
        if ((nwritten =
             fixmsg_addstr_bylen(fix, "100", exdest,
                                 exdest_length)) != -1) {
            result = 0;
        } else {
            result = -1;
            goto done;
        }
    }

  done:
    return result;
}

int generic_fix_interface_main(int argc, char *argv[],
                               struct fix_interface *handler)
{
    int result = -1;
    if (handler != 0) {
        handler->config_file = "knight.dat";
        handler->onstart = knight_custom_init;
        handler->pre_translate_func = knight_pre_translate;
        handler->id_generator = knight_fix_generate_id;
        result = 0;
    }
    return result;
}
