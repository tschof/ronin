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
#include "fix_handler.h"
#include "dart_constants.h"
#include <errno.h>
#include <ctype.h>

static token_struct_t *idgenerator = 0;
static char datestr[9] = { '\0' };

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

static char *nsdqopt_fix_generate_id(size_t * nlen)
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

static char get_security_exchange(char *mm_home)
{
    int ihome = strtol(mm_home, NULL, 10);
    switch (ihome) {
    case 42:
        return '1';
    case 35:
        return 'X';
    case 22:
        return 'W';
    case 21:
        return 'Y';
    case 34:
        return 'N';
    case 44:
        return 'B';
    case 5:
        return 'Q';
    case 261:
        return 'C';
    default:
        return 'Z';
    }
}
static int nsdqopt_translate_capacity(char *rom, romindex_t * map,
                                      char *fixtype, databuf_t * fix,
                                      size_t romfield, int *rejected,
                                      dart_ex_order_obj * order)
{
    int retval = 0;
    if (map[romfield].name != 0) {
        char *pfield = rom + map[romfield].beg, *bookval = 0;
        size_t nfield = map[romfield].end - map[romfield].beg;
        if (nfield == 0) {
            pfield = bookval = doa_get_capacity(order, &nfield);
        } else {
            doa_set_capacity(order, pfield, nfield);
        }
        if (nfield == 1) {
            char capacity = pfield[0];
            if (capacity == 'B') {
                pfield = "J";
            } else if (capacity == 'S') {
                pfield = "O";
            }
            retval =
                fixmsg_addstr_bylen(fix, map[romfield].name, pfield,
                                    nfield);
            if (retval != -1 && (pfield[0] == 'M' || pfield[0] == 'O')) {
                pfield = rom + map[36].beg;
                nfield = map[36].end - map[36].beg;
                char* mm_home = rom + map[ROM_MM_HOME].beg;
                size_t mm_home_len = map[ROM_MM_HOME].end - map[ROM_MM_HOME].beg;
                if(mm_home_len > 0) {
                    char sec_exch = get_security_exchange(mm_home);
                    doa_set_mm_home(order, &sec_exch, 1);
                    retval = fixmsg_addstr_bylen(fix, "207", &sec_exch, 1);
                }
                if (nfield > 0) {
                    int nclearingacct =
                        fixmsg_addstr_bylen(fix, "440", pfield, nfield);
                    if (nclearingacct != -1) {
                        retval += nclearingacct;
                    } else {
                        retval = nclearingacct;
                    }
                }
            }
        }
        free(bookval);
    }
    return retval;
}

static int nsdqopt_translate_minqty(char *rom, romindex_t * map,
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

static int nsdqopt_fixrom_symbol(char *fixmsg, dart_rom_pos * positions,
                                 char mtype, char *status,
                                 databuf_t * rom, size_t romfield,
                                 int *rejtected, dart_ex_order_obj * order,
                                 pthread_mutex_t * mutex,
                                 hashmap_t ord_map)
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

int nsdqopt_trans_expire_day(char *rom, romindex_t * map, char *type,
                             databuf_t * fix, size_t field, int *rejected,
                             dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        char *p = rom + map[field].beg;
        if (len > 0) {
            set_val_in_order_struct(p, len, field, order, type);
        } else {
            p = doa_get_expire_day(order, &len);
        }
        if (len > 0) {
            if (len == 1) {
                /* A single-digit year, prefix with a zero. */
                result =
                    databuf_write(fix, "%c%s=0%c", '\x01', map[field].name,
                                  p[0]);
            } else {
                result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
            }
        } else {
            /* The field is mapped, but not present in the message AND
             * not present in our order object. */
            result = -1;
        }
    }
    return result;
}

static int nsdqopt_translate_open_close(char *rom, romindex_t * map, char *type,
                                        databuf_t * fix, size_t field, int *rejected,
                                        dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        char *pfield = rom + map[field].beg;
        size_t nfield = map[field].end - map[field].beg;
        if (nfield == 1) {
            if (pfield[0] == '1') {
                pfield = "O";
            } else {
                pfield = "C";
            }
            result = fixmsg_addstr_bylen(fix, map[field].name, pfield, nfield);
        }
    }
    return result;
}

static int nsdqopt_translate_account(char *rom, romindex_t * map, char *type,
                                     databuf_t * fix, size_t field, int *rejected,
                                     dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        char *pfield = rom + map[field].beg, *bookval = 0;
        size_t nfield = map[field].end - map[field].beg;
        if (nfield == 0) {
            pfield = bookval = doa_get_account(order, &nfield);
        } else {
            doa_set_account(order, pfield, nfield);
        }
        if (nfield > 0) {
            if (nfield == 8 && (strncmp((pfield + 4), "1209", 4) == 0)) {
                nfield = 4;
            }
            result = fixmsg_addstr_bylen(fix, map[field].name, pfield, nfield);
        }
        free(bookval);
    }
    return result;
}

static char *clientID = 0;
static const char *section = "FIX SERVER";
static const char *clientID_key = "ClientID";
static char *execBroker = 0;
static int execBrokerLength = 0;
static const char *execBrokerKey = "ExecBroker";

static int nsdqopt_custom_init(ex_config * exchange_config,
                               orderbook * pbook)
{
    if ((idgenerator = init_order_token_creator()) == 0) {
        return -1;
    }

    if (initialize_date_string() == -1) {
        return -1;
    }

    /* We need to send an MPID in tag 109. This is configurable. */
    int length;
    clientID =
        get_val_for_tag(exchange_config->dc, section, strlen(section),
                        clientID_key, strlen(clientID_key), &length);
    if (length == 0 || clientID == 0)
        return -1;

    execBroker =
        get_val_for_tag(exchange_config->dc, section, strlen(section),
                        execBrokerKey, strlen(execBrokerKey),
                        &execBrokerLength);
    if (execBroker == 0 || execBrokerLength == 0) {
        printf("Defaulting ExecBroker to NSDQ (76=NSDQ)\n");
        execBroker = "NSDQ";
        execBrokerLength = 4;
    }

    pbook->rom2fix[18] = nsdqopt_translate_minqty;

    set_option_rom2fix(pbook);
    pbook->rom2fix[10] = nsdqopt_translate_capacity;
    pbook->rom2fix[12] = nsdqopt_translate_account;
    pbook->rom2fix[36] = def_rom2fix_no_op;
    pbook->rom2fix[38] = nsdqopt_translate_open_close;

    /* This override ensures that the day is two-digit. */
    pbook->rom2fix[62] = nsdqopt_trans_expire_day;

    pbook->fix2rom[5] = nsdqopt_fixrom_symbol;
    pbook->fix2rom[26] = def_map_fix_text_to_rom;

    return 0;
}

/* Set HandlInst (21) to "1". */
static int nsdqopt_pre_translate(char *rom, romindex_t * map, char *type,
                                 databuf_t * fix, size_t field,
                                 int *rejected, dart_ex_order_obj * order)
{
    int result = fixmsg_addstr_bylen(fix, "21", "1", 1);
    if (result != -1 && type[0] == 'D') {
        int nwritten = fixmsg_addstr(fix, "109", clientID);
        if (nwritten != -1) {
            result += nwritten;
        }
    }
    return result;
}

const char FIX_DELIMITER = '\x01';

int standard_fix_logon(struct parse_args *pa)
{
    struct databuf *buffer = databuf_alloc(512);
    databuf_write(buffer, "98=0%c", FIX_DELIMITER);
    databuf_write(buffer, "108=30%c", FIX_DELIMITER);
    int retval =
        send_gen_logon(pa, buffer->rd_ptr, databuf_unread(buffer));
    databuf_destroy(buffer);
    return retval;
}

int generic_fix_interface_main(int argc, char *argv[],
                               struct fix_interface *handler)
{
    int result = -1;
    if (handler != 0) {
        handler->config_file = "nsdqopt.dat";
        handler->onstart = nsdqopt_custom_init;
        handler->pre_translate_func = nsdqopt_pre_translate;
        handler->id_generator = nsdqopt_fix_generate_id;
        handler->send_logon = standard_fix_logon;
        result = 0;
    }
    return result;
}
