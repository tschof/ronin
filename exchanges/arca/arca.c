/**************************************************************************
 * Copyright (c) 2010 by DART Executions
 *
 * All Rights Reserved
 *************************************************************************/


#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/time.h>


#include "dart_constants.h"
#include "order_token_creator.h"
#include "config_loader.h"
#include "fixmsg.h"
#include "rom2fix.h"
#include "ex_config_helper.h"
#include "generic_fix_interface.h"
#include "ex_common.h"
#include "fix_handler.h"
#include "string_table.h"

static token_struct_t *idgenerator = 0;
static char fixserversection[] = "FIX SERVER";
static char *defroutinst = 0;
static char defroutinstkey[] = "DefRouteInst";
static char datestr[9] = { '\0' };
static int reg_hours_only = 0;

static hashmap_t clearing_accounts;
static char* amex_route = "504";
static int amex_len = 3;

static const char mapping_key_name[] = "ClearingFirmMap";
static strtable_t *clearing_id_table;

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

static char *arca_fix_generate_id(size_t * nlen)
{
    char *cl_ord_id = 0;
    if (nlen != 0) {
        int retval = 0;
        cl_ord_id = calloc(DART_EX_TAG_LEN, 1);
        memcpy(cl_ord_id, datestr, sizeof datestr - 1);
        if ((retval = create_order_token(idgenerator, cl_ord_id +
                                         (sizeof datestr - 1),
                                         DART_EX_TAG_LEN - 1)) > 0) {
            *nlen = (size_t) retval + (sizeof datestr - 1);
        } else {
            *nlen = 0;
        }
    }
    return cl_ord_id;
}

/*! \brief Sets session flags (P1,P2,P3) based on FIX order type.
 *
 *  \arg ordtype Pointer to FIX order type (not ROM), doesn't have to be
 *               null-terminated.
 *  \arg ordtypelen Length, in bytes, of order type; always use this length
 *                  since ordtype is not guaranteed to be null-terminated.
 *  \returns -1 on error 0 or greater on success indicating number of bytes
 *           written into the message.
 */
int arca_set_sessions(const char *ordtype, size_t ordtypelen,
                      databuf_t * fixmsg, char* dest, int dlen)
{
    int retval = -1;
    if(dlen == amex_len && strncmp(dest, amex_route , amex_len) == 0) {
        /* Core session only. */
        retval = fixmsg_addstr(fixmsg, "386", "1");
        if (retval > -1) {
            int nwritten = retval;
            if ((retval = fixmsg_addstr(fixmsg, "336", "P2")) > -1) {
                retval += nwritten;
            }
        }
    } else {
        if (ordtype != 0 && ordtypelen > 0 && fixmsg != 0) {
            if (ordtypelen == 1
                && (ordtype[0] == '1'
                    || ordtype[0] == '5'
                    || ordtype[0] == 'B' || ordtype[0] == 'P')) {
                /* Core session only. */
                retval = fixmsg_addstr(fixmsg, "386", "1");
                if (retval > -1) {
                    int nwritten = retval;
                    if ((retval = fixmsg_addstr(fixmsg, "336", "P2")) > -1) {
                        retval += nwritten;
                    }
                }
            } else {
                /* All sessions. */
                retval = fixmsg_addstr(fixmsg, "386", "3");
                if (retval > -1) {
                    int nwritten = retval;
                    if ((retval = fixmsg_addstr(fixmsg, "336", "P1")) > -1) {
                        nwritten += retval;
                        if ((retval = fixmsg_addstr(fixmsg, "336", "P2")) > -1) {
                            nwritten += retval;
                            if ((retval =
                                 fixmsg_addstr(fixmsg, "336", "P3")) > -1) {
                                retval += nwritten;
                            }
                        }
                    }
                }
            }
        }
    }
    return retval;
}
static void check_time_n_rej(char *beg_off, int len, char *l_fix_tag,
                             databuf_t * output, int *rejected,
                             char *mtype)
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
    if (cur_hour > 15 || (cur_hour == 15 && cur_min >= 59)) {
        int rej_len = 0;
        char *l_message =
            "MOC and LOC orders on ARCA cannot be replaced or cancelled or entered after 3:59 ET.";
        databuf_reset(output);
        if (mtype[0] == 'D') {
        } else {
            rej_len =
                build_cnl_rpl_rej(beg_off, len, l_fix_tag, output,
                                  l_message);
        }
        *rejected = 1;
    }

}

int arca_trans_ordtype(char *rom, romindex_t * map,
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
            case 'R':
                p = "P";
                n = 1;
                goto write_ordtype;
            default:
                break;
            }
        }
        if (n == 2) {
            if (p[0] == '1' && p[1] == '1') {
                if(fixtype[0] != 'D') {
                    check_time_n_rej(p, n, fixtype, fix, rejected, fixtype);
                }
                /* Translate ROM Limit on Close to FIX LOC (11=>B) */
                p = "B";
                n = 1;
            } else if (p[0] == '3' && p[1] == '7') {
                /* Arca Mid-Point Passive Liquidity Order */
                p = "2";
                n = 1;
                size_t nexecinst = map[57].end - map[57].beg;
                char *pexecinst = rom + map[57].beg;
                if (nexecinst > 1
                    || (nexecinst == 1 && pexecinst[0] != 'M')
                    || fixmsg_addstr_bylen(fix, "18", "M", 1) == -1) {
                    *rejected = 1;
                    retval = -1;
                }
            }
        }
      write_ordtype:
        retval = fixmsg_addstr_bylen(fix, map[romfield].name, p, n);
        if (retval > -1) {
            int dest_tif = 0;
            char* tif  = rom + map[ROM_TIF].beg;
            int tlen= map[ROM_TIF].end - map[ROM_TIF].beg;
            if(tlen <= 0) {
                size_t stlen = 0;
                tif = doa_get_tif(order, &stlen);
                dest_tif = stlen > 0 ? 1 : 0;
                tlen = (int)stlen;
            }
            if(tlen > 0 && tif[0] == '2') {
                //Do nothing;
            } else {
                if(reg_hours_only == 0) {
                    int should_delete = 0;
                    char *exchange = rom + map[13].beg;
                    size_t ex_len = map[13].end - map[13].beg;
                    if(ex_len > 0) {
                        doa_set_destination(order, exchange, ex_len);
                    } else {
                        exchange = doa_get_destination(order, &ex_len);
                        if(ex_len > 0) {
                            should_delete = 1;
                        }
                    }
                    retval = arca_set_sessions(p, n, fix, exchange, ex_len);
                    if(should_delete) {
                        free(exchange);
                    }
                } else {
                    if(tlen == 0 || (tlen == 1 && tif[0] == '0')) {
                        retval = fixmsg_addstr(fix, "386", "1");
                        retval = fixmsg_addstr(fix, "336", "P2");
                    } else if(tlen == 1 && tif[0] == '7') {
                        retval = fixmsg_addstr(fix, "386", "1");
                        retval = fixmsg_addstr(fix, "336", "P3");
                    }
                }
            }
            if(dest_tif) {
                free(tif);
            }
        }
        free(bookval);
    } else {
        retval = 0;
    }

    return retval;
}

int arca_translate_orderid(char *rom, romindex_t * map,
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

int arca_translate_clearing_id(char *rom, romindex_t * map,
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
            if (retval > -1) {
                /* Set SenderSubID (50) as clearing account
                   ID, only on New Order - Single. */
                int nwritten =
                    fixmsg_addstr_bylen(fix, "50", clearing_id, idlen);
                if (nwritten > -1) {
                    retval += nwritten;
                } else {
                    retval = -1;
                }
            }
        }

        free(bookval);

    } else {
        retval = 0;
    }

    return retval;
}
int arca_translate_dest(char *rom, romindex_t * map,
                                char *fixtype, databuf_t * fix,
                                size_t romfield, int *rejected,
                                dart_ex_order_obj * order)
{
    int retval = 1;
    int should_delete = 0;
    char *exchange = rom + map[romfield].beg;
    size_t ex_len = map[romfield].end - map[romfield].beg;
    if(ex_len > 0) {
        doa_set_destination(order, exchange, ex_len);
    } else {
        exchange = doa_get_destination(order, &ex_len);
        if(ex_len > 0) {
            should_delete = 1;
        }
    }
    if(ex_len == amex_len &&
            strncmp(exchange, amex_route, amex_len) == 0) {
            char* tif  = rom + map[ROM_TIF].beg;
            int tlen= map[ROM_TIF].end - map[ROM_TIF].beg;
            if(tlen <= 0) {
                size_t stlen = 0;
                tif = doa_get_tif(order, &stlen);
                tlen = (int)stlen;
            }
            if(tlen > 0 && tif[0] == '2') {
                retval = fixmsg_addstr_bylen(fix, "76", "POO", 3);
            } else {
                char *p = rom + map[ROM_TYPE].beg;
                size_t len = map[ROM_TYPE].end - map[ROM_TYPE].beg;
                if (len <= 0) {
                    p = doa_get_type(order, &len);
                }
                if(len == 1 && p[0] == '5') {
                    retval = fixmsg_addstr_bylen(fix, "76", "PO+", 3);
                } else if(len == 2 && p[0] == '1' && p[1] ==  '1') {
                    retval = fixmsg_addstr_bylen(fix, "76", "PO+", 3);
                } else {
                    retval = fixmsg_addstr_bylen(fix, "76", "PO+S", 4);
                }
            }
    }
    if(should_delete) {
        free(exchange);
    }
    return retval;
}
int arca_translate_instructions(char *rom, romindex_t * map,
                                char *fixtype, databuf_t * fix,
                                size_t romfield, int *rejected,
                                dart_ex_order_obj * order)
{
    int retval = 0;
    if (map[romfield].name != 0) {
        char *instr = rom + map[romfield].beg;
        if ((map[romfield].end - map[romfield].beg == 1)
            && instr[0] == 'A') {
            retval = fixmsg_addstr(fix, "9416", "A");
            if (retval > -1) {
                int nwritten = fixmsg_addstr(fix, "18", "6");
                if (nwritten > -1) {
                    retval += nwritten;
                } else {
                    retval = -1;
                }
            }
        } else if ((map[romfield].end - map[romfield].beg == 1)
                   && instr[0] == 'B') {
            retval = fixmsg_addstr(fix, "9417", "B");
            if (retval > -1) {
                int nwritten = fixmsg_addstr(fix, "18", "6");
                if (nwritten > -1) {
                    retval += nwritten;
                } else {
                    retval = -1;
                }
            }
        }
    }
    return retval;
}

int arca_translate_maxfloor(char *rom, romindex_t * map,
                            char *fixtype, databuf_t * fix,
                            size_t romfield, int *rejected,
                            dart_ex_order_obj * order)
{
    int retval = 0;
    if (map[romfield].name != 0) {
        char *maxfloorstr = rom + map[romfield].beg;
        long slen = map[romfield].end - map[romfield].beg;
        char buffer[32] = { '\0' };
        if (slen > 0 && slen < sizeof buffer) {
            memcpy(buffer, maxfloorstr, slen);
            buffer[slen] = '\0';
            {
                int maxfloor = atoi(buffer);
                if (maxfloor > 0) {
                    retval =
                        fixmsg_addstr(fix, map[romfield].name, buffer);
                }
            }
        }
    }
    return retval;
}

int arca_translate_minqty(char *rom, romindex_t * map,
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

int arca_translate_sectype(char *rom, romindex_t * map,
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

int arca_fixrom_symbol(char *fixmsg, dart_rom_pos * positions,
                       char mtype, char *status,
                       databuf_t * rom, size_t romfield,
                       int *rejtected, dart_ex_order_obj * order,
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


int arca_fixrom_status_xlate(char *fixmsg, dart_rom_pos * positions,
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
            /* If this is also an OPG order, then we will mark this
               order as being dead. */
            databuf_write(rom, "%s", "4");
        } else {
            len = determine_trans_type((int) romfield, beg_off, rom, len);
            status = fixmsg;
            status += positions[romfield].beg;
        }
    } else if (romfield == 14 && mtype == 'U') {
        len = set_status_for_cnrej(fixmsg, positions, status, rom, order);
    }
    return 1;
}

int clearing_id_table_mapping(char *rom, romindex_t * map, char *type,
                              databuf_t * fix, size_t field,
                              int *rejected, dart_ex_order_obj * order)
{
    int retval = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        char *ptr, *bookval = 0;
        if (len > 0) {
            ptr = rom + map[field].beg;
            set_val_in_order_struct(ptr, len, field, order, type);
        } else {
            ptr = bookval = doa_get_clr_id(order, &len);
        }
        if (len > 0 && ptr != 0) {
            /* Map the value. */
            str_t *val = lookup_str(clearing_id_table, ptr, len);
            if (val != 0 && val->len > 0) {
                retval =
                    fixmsg_addstr_bylen(fix, map[field].name, val->str,
                                        val->len);
            }
        }
        free(bookval);
    }
    return retval;
}

int arca_clearing_id_table_mapping(char *rom, romindex_t * map, char *type,
                                   databuf_t * fix, size_t field,
                                   int *rejected,
                                   dart_ex_order_obj * order)
{
    int retval =
        clearing_id_table_mapping(rom, map, type, fix, field, rejected,
                                  order);
    if (retval != -1) {
        size_t len = map[field].end - map[field].beg;
        if (len > 0) {
            char *ptr = rom + map[field].beg;
            retval = fixmsg_addstr_bylen(fix, "439", ptr, len);
        }
    }
    return retval;
}


/* int arca_clearing_id_table_mapping(char *rom, romindex_t * map, char *type, */
/*                                    databuf_t * fix, size_t field, */
/*                                    int *rejected, dart_ex_order_obj * order) */
/* { */
/*     int retval = 0; */
/*     if (map[field].name != 0) { */
/*         ssize_t len = map[field].end - map[field].beg; */
/*         if (len > 0) { */
/*             const char *ptr = rom + map[field].beg; */
/*             str_t *val = lookup_str(clearing_id_table, ptr, (size_t) len); */
/*             if (val != 0 && val->len > 0) { */
/*                 retval = fixmsg_addstr_bylen(fix, map[field].name, */
/*                                              val->str, val->len); */
/*             } */
/*         } */
/*     } */
/*     return retval; */
/* } */

/* These characters represent the logon profile "bits".  As of Version
   1.2 (March 7th, 2012) there are 7 bits.  Of those 7 bits, the
   defaults are okay for our purposes except for the first, Cancel on
   Disconnect.  If subsequent bits are to be specified, just add those
   and the intermediate "bits" to this array. */
#define CANCEL_ON_DISCONNECT_BIT 0
char logon_profile[] = {
    '1',                        /* Cancel on Disconnect */
};

static const size_t nbits = sizeof logon_profile / sizeof logon_profile[0];

static int arca_custom_init(ex_config * exchange_config, orderbook * pbook)
{
    int result = -1, len = 0;
    const char *cancel_on_disconnect_key_name = "CancelOnDisconnect";
    char *cancel_on_disconnect =
        get_val_for_tag(exchange_config->dc, fixserversection,
                        sizeof fixserversection - 1,
                        cancel_on_disconnect_key_name,
                        strlen(cancel_on_disconnect_key_name), &len);
    if (len > 0 && cancel_on_disconnect != 0) {
        if (len == 1 && cancel_on_disconnect[0] == '0') {
            logon_profile[CANCEL_ON_DISCONNECT_BIT] = '0';
        } else if (len != 1 || cancel_on_disconnect[0] != '1') {
            fprintf(stderr,
                    "invalid CancelOnDisconnect flag, must be '0' or "
                    "'1', defaulting to cancel on disconnect\n");
        }
    }
    printf("Cancel on disconnect = %c\n",
           logon_profile[CANCEL_ON_DISCONNECT_BIT]);
    free(cancel_on_disconnect);
    if (initialize_date_string() == 0) {
        idgenerator = init_order_token_creator();
        if (idgenerator != 0) {
            defroutinst =
                get_val_for_tag(exchange_config->dc, fixserversection,
                                sizeof fixserversection - 1,
                                defroutinstkey, sizeof defroutinstkey - 1,
                                &len);
            if (defroutinst == 0) {
                printf("Default Routing Instruction missing from "
                       "configuration, defaulting to 'ARCA'\n");
                defroutinst = "ARCA";
            }
            if ((result = def_init_translators(pbook, ROM_LEN)) != -1) {
                fprintf(stderr,
                        "Error initializing default translators\n");
            }

            int ret_len;
            char* lreg_hours_only =
                        get_val_for_tag(exchange_config->dc, "FIX SERVER", 10, "REG HOURS",
                                        9, &ret_len);
            if(ret_len > 0 && lreg_hours_only != 0x0) {
                reg_hours_only = 1;
            } else {
                reg_hours_only = 0;
            }

            clearing_accounts = load_clearing_ids("GSFlip.map");
            char* temp_amex = 
                get_val_for_tag(exchange_config->dc, "FIX SERVER", 10, "AMEX", 4, &ret_len);
            if(ret_len > 0) {
                amex_route  = temp_amex;
                amex_len = ret_len;
            }
            char *clearing_firm_mapping_rule
                = get_val_for_tag(exchange_config->dc, "FIX SERVER", 10,
                                  mapping_key_name,
                                  sizeof mapping_key_name - 1,
                                  &ret_len);
            if (clearing_firm_mapping_rule) {
                if ((clearing_id_table
                     =
                     parse_table_conf(clearing_firm_mapping_rule)) != 0) {
                    pbook->rom2fix[11] = arca_clearing_id_table_mapping;
                } else {
                    fprintf(stderr,
                            "Fatal error: cannot parse configuration rule for %s\n",
                            mapping_key_name);

                    return 1;
                }
                free(clearing_firm_mapping_rule);
            }


            /* The OrdType affects the session list so we'll do a
               custom OrdType translation and do the session
               translation at the same time. */
            pbook->rom2fix[5] = def_trans_symbol_and_suffix;
            pbook->rom2fix[8] = arca_trans_ordtype;
            pbook->rom2fix[16] = arca_translate_orderid;
            pbook->rom2fix[12] = arca_translate_clearing_id;
            pbook->rom2fix[13] = arca_translate_dest;
            pbook->rom2fix[15] = arca_translate_instructions;
            pbook->rom2fix[18] = arca_translate_minqty;
            pbook->rom2fix[34] = arca_translate_maxfloor;
            pbook->rom2fix[35] = arca_translate_sectype;

            pbook->fix2rom[5] = arca_fixrom_symbol;
            pbook->fix2rom[14] = arca_fixrom_status_xlate;
            pbook->fix2rom[26] = def_map_fix_text_to_rom;

            result = 0;
        }
    }

    return result;
}

/* Set HandlInst (21) to "1". */
static int arca_pre_translate(char *rom, romindex_t * map, char *type,
                              databuf_t * fix, size_t field, int *rejected,
                              dart_ex_order_obj * order)
{
    int nwritten = fixmsg_addstr(fix, "21", "1");
    if (nwritten != -1) {
        int tmp = fixmsg_addstr(fix, "57", "ARCA");
        if (tmp != -1) {
            nwritten += tmp;
        } else {
            nwritten = -1;
        }
    }
    return nwritten;
}

int arca_logon(struct parse_args *pa)
{
    struct databuf *buffer = databuf_alloc(512);
    if (buffer == 0) {
        return -1;
    }
    int retval = -1;
    databuf_write(buffer, "98=0%c", '\x01');
    databuf_write(buffer, "108=30%c", '\x01');
    if (databuf_write(buffer, "95=%zu%c", nbits, '\x01') != -1) {
        size_t i;
        for (i = 0; i < nbits; ++i) {
            if (databuf_write(buffer, "96=%c%c", logon_profile[i],
                              '\x01') == -1) {
                goto done;
            }
        }
    }
    retval = send_gen_logon(pa, buffer->rd_ptr, databuf_unread(buffer));
  done:
    databuf_destroy(buffer);
    return retval;
}

int generic_fix_interface_main(int argc, char *argv[],
                               struct fix_interface *handler)
{
    int result = -1;
    if (handler != 0) {
        handler->config_file = "arca.dat";
        handler->onstart = arca_custom_init;
        handler->pre_translate_func = arca_pre_translate;
        handler->id_generator = arca_fix_generate_id;
        handler->send_logon = arca_logon;
        result = 0;
    }
    return result;
}
