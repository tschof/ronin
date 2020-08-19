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
#include "parser_func.h"
#include "ex_config_helper.h"
#include "generic_fix_interface.h"
#include "ex_common.h"
#include <errno.h>
#include <ctype.h>

static token_struct_t *idgenerator = 0;
static char datestr[9] = { '\0' };
static hashmap_t algomapping = 0;

static char *ucode = 0;
//We read this out of the dat file to see if we need to
//support CAN and Brazillian order flow.
static hashmap_t redi_route_codes;
static hashmap_t redi_currency;
static hashmap_t redi_eleven_seven;
static hashmap_t redi_one_four_five;
static const char *fixsrv_section_name = "FIX SERVER";
static const char *ucode_key_name = "ucode";
static const char *majic_sonar_route = "485";
static void redi_add_to_map(hashmap_t map, char *buffer, long len)
{
    char *off;
    long c_offset = 0;
    off = strchr(buffer, ',');
    if (off) {
        char *tag;
        char *val_off;
        c_offset = off - buffer;
        tag = calloc(c_offset + 1, 1);
        memcpy(tag, buffer, c_offset);

        val_off = buffer;
        val_off += c_offset + 1;
        insert_map(map, tag, c_offset, val_off,len - (c_offset + 1));
        free(tag);
    } else {
        printf("Could not find a comma in this mess: %s \n", buffer);
    }
}
static hashmap_t clearing_accounts;
static void redi_add_to_map2(hashmap_t map, char *buffer, long len)
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
                redi_add_to_map2(map, d_off, end_pos);
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

struct algofields_t {
    const char *str, *fixtag;
};
static const struct algofields_t algofields[] = {
    {"AU", "8036"},
    {"au", "8036"},
    {"wouldp", "8210"},
    {"WOULDP", "8210"},
    {"WOULDS", "8301"},
    {"woulds", "8301"},
    {"wouldpr", "8057"},
    {"WOULDPR", "8057"},
    {"mvp", "111"},
    {"MVP", "111"},
    {"style", "8042"},
    {"STYLE", "8042"},
    {"wouldref", "8083"},
    {"WOULDREF", "8083"},
    {"STY", "8042"},
    {"SOS", "8032"},
    {"sty", "8042"},
};

static int redi_initalgomappings(void)
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


int  redi_build_map_from_file(const char *filename, hashmap_t map)
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
                redi_add_to_map(map, d_off, end_pos);
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

static char *fix_generate_id_with_date(size_t * nlen)
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

static int set_short_locate_information(char *rom, romindex_t * map,
                                        char *fixtype, databuf_t * fix,
                                        size_t romfield, int *rejected,
                                        dart_ex_order_obj * order)
{
    int retval = 0;
    if (map[romfield].name != 0) {
        ssize_t locatelen = map[romfield].end - map[romfield].beg;
        char *locateptr = rom + map[romfield].beg, *locate_from_book = 0;
        if (locateptr > 0) {
            doa_set_short_locate(order, locateptr, locatelen);
        } else {
            locateptr = locate_from_book =
                doa_get_short_locate(order, (size_t *) & locatelen);
        }
        if (locatelen > 0) {
            if ((locatelen == 3 && locateptr[0] == '5'
                 && locateptr[1] == '0' && locateptr[2] == '1')
                || (locatelen == 4 && locateptr[0] == '0'
                    && locateptr[1] == '5' && locateptr[2] == '0'
                    && locateptr[3] == '1')) {
                retval = fixmsg_addstr(fix, map[romfield].name, "GSCO");
            } else {
                retval =
                    fixmsg_addstr_bylen(fix, map[romfield].name, locateptr,
                                        (int) locatelen);
            }
        }
        free(locate_from_book);
    }
    return retval;
}

static int
redi_translate_side_and_shortsell_fields(char *rom,
                                         romindex_t * map,
                                         char *type,
                                         databuf_t * fix,
                                         size_t field,
                                         int *rejected,
                                         dart_ex_order_obj * order)
{
    int result = -1;
    if (map[field].name != 0) {
        size_t n = 0;
        long len = map[field].end - map[field].beg;
        char *side = rom + map[field].beg, *bookval = 0;
        if (len > 0) {
            n = (size_t) len;
            set_val_in_order_struct(side, n, field, order, type);
        } else {
            side = bookval = doa_get_side(order, &n);
        }
        if (side != 0 && n != 0) {
            result = fixmsg_addstr_bylen(fix, map[field].name, side, n);
            if (n == 1 && side[0] == '5') {
                int nwritten = fixmsg_addstr(fix, "114", "N");
                if (nwritten > -1) {
                    result += nwritten;
                    if ((nwritten =
                         set_short_locate_information(rom, map, type, fix,
                                                      58, rejected,
                                                      order)) > -1) {
                        result += nwritten;
                    } else {
                        result = -1;
                    }
                } else {
                    result = -1;
                }
            }
        }
        free(bookval);
    } else {
        result = 0;
    }

    return result;
}

static int redi_translate_clearing_account(char *rom, romindex_t * map,
                                           char *fixtype, databuf_t * fix,
                                           size_t romfield, int *rejected,
                                           dart_ex_order_obj * order)
{
    int result = 0;
    if (map[romfield].name != 0) {
        char *p = rom + map[romfield].beg;
        size_t len = map[romfield].end - map[romfield].beg;
        size_t id_len = map[11].end - map[11].beg;
        char* id = rom + map[11].beg;
        int del_id = 0;
        if (len == 0) {
            p = doa_get_account(order, &len);
        } else {
            doa_set_account(order, p, len);
        }
        if(id_len > 0) {
            id = doa_get_clr_id(order, &id_len);
            if(id_len > 0) {
                del_id = 1;
            }
        } else {
            doa_set_clr_id(order, id, id_len);
        }
        if(id_len >= 3 && 
                (strncmp(id, "695", 3) == 0 ||
                 strncmp(id, "396", 3) == 0 ||
                 strncmp(id, "813", 3) == 0 ||
                 strncmp(id, "0813", 3) == 0 ||
                 strncmp(id, "0695", 3) == 0||
                 strncmp(id, "0396", 3) == 0)) {
            char coll[40];
            memset(coll, '\0', 40);
            memcpy(coll, "ABN", 3);
            if(strncmp(id, "695", 3) == 0 || strncmp(id, "0695", 4) == 0 ||
                    strncmp(id, "813", 3) == 0) {
                memcpy(coll + 3, "DCHF", 4);
            } else {
                memcpy(coll + 3, "DRTL", 4);
            }
            memcpy(coll + 7, "-", 1);
            memcpy(coll + 8, p, len);
            result =
                fixmsg_addstr(fix, map[romfield].name, coll);
                fixmsg_addstr_bylen(fix, "115", "DCHF", 4);
        } else { 
            char* rom_val= 0;
            int found =
                find_n_fill(clearing_accounts, p, len, (void *) &rom_val);
            if(found) {
                len = found;
                p = rom_val;
            }
            doa_set_account(order, p, len);
            result =
                fixmsg_addstr_bylen(fix, map[romfield].name, p, (int) len);
                fixmsg_addstr_bylen(fix, "115", "DEGS", 4);
        }
        if(del_id) {
            free(id);
        }
    }
    return result;
}
typedef int (*algo_map_function) (char *rom, char *key, size_t keylen,
                                  char *val, size_t vallen,
                                  databuf_t * fix, int *rejected);

static int redi_map_algo_parameters(char *rom, char *key, size_t keylen,
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

static int redi_translate_instructions(char *rom, romindex_t * map,
                                         char *fixtype, databuf_t * fix,
                                         size_t romfield, int *rejected,
                                         dart_ex_order_obj * order)
{
    int retval = 0;
    if (map[romfield].name != 0) {
        retval = parse_algo_instructions(rom, rom + map[romfield].beg,
                                         map[romfield].end -
                                         map[romfield].beg, fix, rejected,
                                         redi_map_algo_parameters);
    }
    return retval;
}

static int redi_translate_orderid(char *rom, romindex_t * map,
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

static int redi_maxvolpct_alternate_mapping(char *rom, romindex_t * map,
                                            char *fixtype, databuf_t * fix,
                                            size_t romfield, int *rejected,
                                            dart_ex_order_obj * order)
{
    int retval = 0;
    if (map[romfield].name != 0) {
        signed long int mfieldlen = map[romfield].end - map[romfield].beg;
        if (mfieldlen > 0) {
            size_t n = (size_t) mfieldlen;
            char *p = rom + map[romfield].beg;
            retval =
                redi_map_algo_parameters(rom, "MVP", 3, p, n, fix,
                                         rejected);
        }
    }
    return retval;
}

static int redi_translate_currency(char *rom, romindex_t * map,
                                   char *fixtype, databuf_t * fix,
                                   size_t field, int *rejected,
                                   dart_ex_order_obj * order)
{
    int retval = 0;
    if (map[field].name != 0) {
        size_t len = map[13].end - map[13].beg;
        if(len > 0) {
            char* p = rom + map[13].beg;
            char* route = 0;
            int found = find_n_fill(redi_currency, p, len, (void *) &route);
            if(found) {
                retval = fixmsg_addstr_bylen(fix, map[field].name, route, found);
            }
        } else {
            char* route = 0;
            char* p = doa_get_dest_route(order, &len);
            int found = find_n_fill(redi_currency, p, len, (void *) &route);
            if(found > 0) {
                retval = fixmsg_addstr_bylen(fix, map[field].name, route, found);
            }
        }
    }
    return retval;
}

static int redi_translate_algotype(char *rom, romindex_t * map,
                                   char *fixtype, databuf_t * fix,
                                   size_t romfield, int *rejected,
                                   dart_ex_order_obj * order)
{
    char* route = rom + map[13].beg;
    int rlen = map[13].end - map[13].beg;
    if(rlen > 0) {
        size_t glen = (size_t)rlen;
       doa_set_destination(order, route, glen); 
    } else {
        size_t glen = 0;
       route = doa_get_destination(order, &glen);
      rlen = glen; 
    }
    int retval = 0;
    if(rlen == 3 && strncmp(route, majic_sonar_route, 3) == 0) {
        retval = fixmsg_addstr(fix, "8031", "SOR");
        fixmsg_addstr(fix, "100", ";");
        //fixmsg_addstr(fix, "8036", "7");
    } else {
        if (map[romfield].name != 0) {
            signed long int field_len = map[romfield].end - map[romfield].beg;
            char *p = rom + map[romfield].beg;
            if (field_len == 1 && p[0] == '2') {
                /* This is a VWAP, let it through. */
                retval = fixmsg_addstr(fix, map[romfield].name, "VWAP");
                fixmsg_addstr(fix, "100", "6");
            } else if(field_len == 1 && p[0] == '1') {
                retval = fixmsg_addstr(fix, map[romfield].name, "TWAP");
                fixmsg_addstr(fix, "100", "6");
            } else if(field_len == 1 && p[0] == '9') {
                retval = fixmsg_addstr(fix, map[romfield].name, "SOR");
                fixmsg_addstr(fix, "100", ";");
            } else if(field_len == 1 && p[0] == '4') {
                retval = fixmsg_addstr(fix, map[romfield].name, "SONAR");
                fixmsg_addstr(fix, "100", "6");
            }
        }
    }
    return retval;
}

static int redi_test_datetime(char *rom, romindex_t * map, char *type,
                              databuf_t * fix, size_t field, int *rejected,
                              dart_ex_order_obj * order)
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

static int redi_fixrom_symbol(char *fixmsg, dart_rom_pos * positions,
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

int redi_trans_routes(char *rom, romindex_t * map, char *type,
                        databuf_t * fix, size_t field, int *rejected,
                        dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        if (len > 0) {
            char *p = rom + map[field].beg;
            doa_set_dest_route(order, p, len);
            set_val_in_order_struct(p, len, field, order, type);
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
        } else {
            len = map[13].end - map[13].beg;
            if(len > 0) {
                char* p = rom + map[13].beg;
                char* route = 0;
                int found = find_n_fill(redi_route_codes, p, len, (void *) &route);
                if(found) {
                    result = fixmsg_addstr_bylen(fix, map[field].name, route, found);
                    doa_set_dest_route(order, p, len);
                }
                found = find_n_fill(redi_one_four_five, p, len, (void*) &route);
                if(found) {
                    result = fixmsg_addstr_bylen(fix, "145", route, found);
                }
                found = find_n_fill(redi_eleven_seven, p, len, (void*) &route);
                if(found) {
                    result = fixmsg_addstr_bylen(fix, "11007", route, found);
                }
                /**
                 *  fixmsg_addstr(fix, "145", "1") > -1
                 */
            } else {
                char* route = 0;
                char* p = doa_get_dest_route(order, &len);
                int found = find_n_fill(redi_route_codes, p, len, (void *) &route);
                if(found > 0) {
                    result = fixmsg_addstr_bylen(fix, map[field].name, route, found);
                }
                found = find_n_fill(redi_one_four_five, p, len, (void*) &route);
                if(found) {
                    result = fixmsg_addstr_bylen(fix, "145", route, found);
                }
                found = find_n_fill(redi_eleven_seven, p, len, (void*) &route);
                if(found) {
                    result = fixmsg_addstr_bylen(fix, "11007", route, found);
                }
            }
        }
    }
    return result;
}

static int redi_custom_init(ex_config * exchange_config, orderbook * pbook)
{
    int result = -1;

    idgenerator = init_order_token_creator();
    if (idgenerator != 0) {
        if (initialize_date_string() != -1) {
            if ((result = def_init_translators(pbook, ROM_LEN)) == -1) {
                fprintf(stderr,
                        "Error initializing default translators\n");
            }

            /* Load the ucode (Goldman account number) needed for each order. */
            int ucode_len;
            ucode =
                get_val_for_tag(exchange_config->dc, fixsrv_section_name,
                                strlen(fixsrv_section_name),
                                ucode_key_name, strlen(ucode_key_name),
                                &ucode_len);
            if (ucode == 0 && ucode_len == 0) {
                return -1;
            }

            if ((result = redi_initalgomappings()) < 0) {
                (void) fprintf(stderr,
                               "Error initializing algo mappings\n");
            }
            clearing_accounts = load_clearing_ids("GSFlip.map");
            redi_route_codes = create_map(16);
            redi_build_map_from_file("REDI_ROUTES", redi_route_codes);
            redi_currency = create_map(16);
            redi_build_map_from_file("REDI_CURRENCY", redi_currency);
            redi_one_four_five = create_map(16);
            redi_build_map_from_file("REDI_145", redi_one_four_five);
            redi_eleven_seven = create_map(16);
            redi_build_map_from_file("REDI_11007", redi_eleven_seven);
            /* pbook->rom2fix[ 0] = redi_reject_cancel_replace; */
            pbook->rom2fix[4] = redi_translate_side_and_shortsell_fields;
            pbook->rom2fix[29] = redi_trans_routes;
            pbook->rom2fix[5] = def_trans_symbol_and_suffix;
            pbook->rom2fix[12] = redi_translate_clearing_account;
            pbook->rom2fix[15] = redi_translate_instructions;
            pbook->rom2fix[16] = redi_translate_orderid;
            pbook->rom2fix[56] = redi_translate_currency;
            pbook->rom2fix[58] = 0;     /* Do not translate */
            /* * 
             * We will key off algo dest config param in order to
             * inable this translation.
             * */
            pbook->rom2fix[26] = redi_maxvolpct_alternate_mapping;
            pbook->rom2fix[73] = redi_translate_algotype;
            pbook->rom2fix[77] = redi_test_datetime;
            pbook->fix2rom[5] = redi_fixrom_symbol;
            pbook->fix2rom[26] = def_map_fix_text_to_rom;

            result = 0;
        }
    }

    return result;
}

/* Set HandlInst (21) to "1". */
static int redi_pre_translate(char *rom, romindex_t * map, char *type,
                              databuf_t * fix, size_t field, int *rejected,
                              dart_ex_order_obj * order)
{
    /**
     *
            && fixmsg_addstr(fix, "100", "6") > -1
     */
    int retval = -1;
        if (fixmsg_addstr(fix, "21", "1") > -1
            && fixmsg_addstr(fix, "145", "1") > -1
            && fixmsg_addstr(fix, "50", ucode)) {
            retval = 0;
        } else {
            retval = -1;
        }
    return retval;
}

int generic_fix_interface_main(int argc, char *argv[],
                               struct fix_interface *handler)
{
    int result = -1;
    if (handler != 0) {
        handler->config_file = "redi.dat";
        handler->onstart = redi_custom_init;
        handler->pre_translate_func = redi_pre_translate;
        handler->id_generator = fix_generate_id_with_date;
        result = 0;
    }
    return result;
}
