#include <string.h>
#include "rom2fix.h"
#include "ex_common.h"
#include "fixmsg.h"
#include "order_token_creator.h"
#include "dart_orderbook.h"
#include "fix_handler.h"

int rom2fix_add_translator(size_t field, fieldxlatefunc func,
                           orderbook * pbook)
{
    int retval = -1;
    if (field < ROM_LEN && func != 0)
        pbook->rom2fix[field] = func;
    return retval;
}

void set_option_rom2fix(orderbook * ob)
{
    ob->rom2fix[10] = def_trans_opt_capacity;
    ob->rom2fix[12] = def_opt_account_trans;
    ob->rom2fix[17] = def_rom2fix_no_op;
    ob->rom2fix[30] = def_trans_expire_my;
    ob->rom2fix[31] = def_trans_call_put;
    ob->rom2fix[32] = def_trans_strike;
    ob->rom2fix[35] = def_trans_sec_type;
    ob->rom2fix[36] = def_trans_mm_account;
    ob->rom2fix[37] = def_rom2fix_no_op;
    ob->rom2fix[38] = def_trans_open_close;
    ob->rom2fix[46] = def_trans_cmta;
    ob->rom2fix[62] = def_trans_expire_day;
}

int def_init_translators(orderbook * ob, size_t nfunc)
{
    int rv = 0;
    size_t i;
    for (i = 0; i < nfunc; ++i)
        ob->rom2fix[i] = def_rom2fix;
    if (nfunc < 57)
        rv = -1;
    ob->rom2fix[4] = def_trans_side;
    ob->rom2fix[8] = def_trans_ordtype;
    ob->rom2fix[10] = def_trans_capacity;
    ob->rom2fix[17] = def_field_noop;
    ob->rom2fix[50] = set_transacttime;
    ob->rom2fix[57] = def_trans_execinst;
    return rv;
}

int def_rom2fix_no_op(char *rom, romindex_t * map, char *type,
                      databuf_t * fix, size_t field, int *rejected,
                      dart_ex_order_obj * order)
{
    return 0;
}

int def_rom2fix(char *rom, romindex_t * map, char *type,
                databuf_t * fix, size_t field, int *rejected,
                dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        if (len > 0) {
            char *p = rom + map[field].beg;
            set_val_in_order_struct(p, len, field, order, type);
            result =
                fixmsg_addstr2(fix, map[field].name, map[field].len, p,
                               len);
        }
    }
    return result;
}

int def_opt_account_trans(char *rom, romindex_t * map, char *type,
                          databuf_t * fix, size_t field, int *rejected,
                          dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        if (len > 0) {
            char *p = rom + map[field].beg;
            if (len == 8 && (strncmp((p + 4), "1209", 4) == 0)) {
                len = 4;
            }
            set_val_in_order_struct(p, len, field, order, type);
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
        } else if (rom[0] != 'E') {
            char *p = doa_get_account(order, &len);
            if (len > 0 && p != NULL) {
                if (len == 8 && (strncmp((p + 4), "1209", 4) == 0)) {
                    len = 4;
                }
                result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
                free(p);
            }
        }
    }
    return result;
}

int def_trans_expire_my(char *rom, romindex_t * map, char *type,
                        databuf_t * fix, size_t field, int *rejected,
                        dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        if (len > 0) {
            char *p = rom + map[field].beg;
            set_val_in_order_struct(p, len, field, order, type);
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
        } else if (rom[0] != 'E' && map[field].name != NULL) {
            char *p = doa_get_expireMonYear(order, &len);
            if (len > 0 && p != NULL) {
                result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
                free(p);
            }
        }
    }
    return result;
}

int def_trans_opt_capacity(char *rom, romindex_t * map, char *type,
                           databuf_t * fix, size_t field, int *rejected,
                           dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        if (len > 0) {
            char *p = rom + map[field].beg;
            if (p[0] == 'S') {
                size_t mm_home_len = map[37].end - map[37].beg;
                char *mm_home = rom + map[37].beg;
                if (mm_home_len > 0 && (strncmp(mm_home, "21", 2) == 0)) {
                    p = "4";
                    len = 1;
                } else {
                    p = "5";
                    len = 1;
                }
            } else if(p[0] == 'X') {
                p = "8";
                len = 1;   
            } else {
                p = "1";
                len = 1;
            }
            set_val_in_order_struct(p, len, field, order, type);
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
        } else if (rom[0] != 'E') {
            char *p = doa_get_capacity(order, &len);
            if (len > 0 && p != NULL) {
                if (p[0] == 'S') {
                    result =
                        fixmsg_addstr_bylen(fix, map[field].name, "5", 1);
                } else if (p[0] == 'B') {
                    result =
                        fixmsg_addstr_bylen(fix, map[field].name, "1", 1);
                } else if(p[0] == 'X') {
                    result =
                        fixmsg_addstr_bylen(fix, map[field].name, "8", 1);
                } else {
                    result =
                        fixmsg_addstr_bylen(fix, map[field].name, p, len);
                }
                free(p);
            }
        }
    }
    return result;
}

int def_trans_expire_day(char *rom, romindex_t * map, char *type,
                         databuf_t * fix, size_t field, int *rejected,
                         dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        if (len > 0) {
            char *p = rom + map[field].beg;
            set_val_in_order_struct(p, len, field, order, type);
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
        } else if (rom[0] != 'E') {
            char *p = doa_get_expire_day(order, &len);
            if (len > 0 && p != NULL) {
                result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
                free(p);
            }
        }
    }
    return result;
}

int def_trans_call_put(char *rom, romindex_t * map, char *type,
                       databuf_t * fix, size_t field, int *rejected,
                       dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        char *p = rom + map[field].beg, *bookval = 0;
        size_t n = 0;
        if (map[field].end >= map[field].beg)
            n = map[field].end - map[field].beg;
        if (n > 0)
            doa_set_call_put(order, p, n);
        else
            p = bookval = doa_get_call_put(order, &n);
        if (n == 1) {
            if (p[0] == 'P')
                p = "0";
            else if (p[0] == 'C')
                p = "1";
            else if (p[0] != '0' && p[0] != '1') {
                result = -1;
                goto cleanup;
            }
            result = fixmsg_addstr_bylen(fix, map[field].name, p, n);
        }
      cleanup:
        free(bookval);
    }
    return result;
}

int def_trans_strike(char *rom, romindex_t * map, char *type,
                     databuf_t * fix, size_t field, int *rejected,
                     dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        if (len > 0) {
            char *p = rom + map[field].beg;
            set_val_in_order_struct(p, len, field, order, type);
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
        } else {
            char *p = doa_get_strike(order, &len);
            if (len > 0 && p != NULL) {
                result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
                free(p);
            }
        }
    }
    return result;
}

int def_trans_open_close(char *rom, romindex_t * map, char *type,
                         databuf_t * fix, size_t field, int *rejected,
                         dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        if (len > 0) {
            char *p = rom + map[field].beg;
            if (p[0] == '1') {
                p = "O";
            } else {
                p = "C";
            }
            len = 1;
            set_val_in_order_struct(p, len, field, order, type);
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
        } else if (rom[0] != 'E' && map[field].name != NULL) {
            char *p = doa_get_open_close(order, &len);
            if (len > 0 && p != NULL) {
                result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
                free(p);
            }
        }
    }
    return result;
}

int def_trans_sec_type(char *rom, romindex_t * map, char *type,
                       databuf_t * fix, size_t field, int *rejected,
                       dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        if (len > 0) {
            char *p = rom + map[field].beg;
            if (doa_get_is_spread(order)) {
                p = "MLEG";
                len = 4;
            } else if (p && strncmp(p, "O", 1) == 0) {
                p = "OPT";
                len = 3;
            } else if (p && strncmp(p, "F", 1) == 0) {
                p = "FUT";
                len = 3;
            } else if (p && strncmp(p, "E", 1) == 0) {
                p = "CS";
                len = 2;
            } else if (p && len == 1 && p[0] == 'X') {
                p = "FOR";
                len = 3;
            }
            set_val_in_order_struct(p, len, field, order, type);
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
        } else if (rom[0] != 'E') {
            char *p = doa_get_sec_type(order, &len);
            if (len > 0 && p != NULL) {
                if (doa_get_is_spread(order)) {
                    result =
                        fixmsg_addstr_bylen(fix, map[field].name, "MLEG",
                                            4);
                } else if (len == 1 && p[0] == 'O') {
                    result =
                        fixmsg_addstr_bylen(fix, map[field].name, "OPT",
                                            3);
                } else if (len == 1 && p[0] == 'F') {
                    result =
                        fixmsg_addstr_bylen(fix, map[field].name, "FUT",
                                            3);
                } else if (len == 1 && p[0] == 'E') {
                    result =
                        fixmsg_addstr_bylen(fix, map[field].name, "CS", 2);
                } else {
                    result =
                        fixmsg_addstr_bylen(fix, map[field].name, p, len);
                }
                free(p);
            }
        }
    }
    return result;
}

int def_trans_mm_account(char *rom, romindex_t * map, char *type,
                         databuf_t * fix, size_t field, int *rejected,
                         dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        if (len > 0) {
            char *p = rom + map[field].beg;
            set_val_in_order_struct(p, len, field, order, type);
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
        } else if (rom[0] != 'E') {
            char *p = doa_get_mm_account(order, &len);
            if (len > 0 && p != NULL) {
                result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
                free(p);
            }
        }
    }
    return result;
}

int def_trans_cmta(char *rom, romindex_t * map, char *type,
                   databuf_t * fix, size_t field, int *rejected,
                   dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[field].end - map[field].beg;
        if (len > 0) {
            char *p = rom + map[field].beg;
            set_val_in_order_struct(p, len, field, order, type);
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
        } else if (rom[0] != 'E') {
            char *p = doa_get_cmta(order, &len);
            if (len > 0 && p != NULL) {
                result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
                free(p);
            }
        }
    }
    return result;
}

/* If a LocateBroker field needs to be sent, the mapping can be done via the
 * standard mapping tables, it needn't be done in code and is not needed for
 * exchange destinations. */
int def_trans_side(char *rom, romindex_t * map, char *type,
                   databuf_t * fix, size_t field, int *rejected,
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

int def_trans_symbol_and_suffix(char *rom, romindex_t * map,
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
            memcpy(buffer, p, n);
            buffer[n] = '\0';
            if ((suffix = strchr(buffer, '/')) != 0) {
                *suffix++ = '\0';
            }
            result = fixmsg_addstr(fix, map[romfield].name, buffer);
            if (result > -1 && suffix != 0) {
                int nwritten = fixmsg_addstr(fix, "65", suffix);
                if (nwritten > -1) {
                    result += nwritten;
                }
            }
        } else {
            /* Reject the order. */
        }
        free(booksym);
    } else {
        result = 0;
    }
    return result;
}

int
def_trans_acct_and_store(char *rom, romindex_t * map, char *type,
                         databuf_t * fix, size_t field, int *rejected,
                         dart_ex_order_obj * order)
{
    int retval = 0;
    if (map[field].name != 0) {
        char *acct = rom + map[field].beg, *bookval = 0;
        long len = map[field].end - map[field].beg;
        size_t acctlen = 0;
        if (len > 0) {
            acctlen = (size_t) len;
            doa_set_account(order, acct, acctlen);
        } else {
            acct = bookval = doa_get_account(order, &acctlen);
        }
        if (acctlen > 0) {
            retval =
                fixmsg_addstr_bylen(fix, map[field].name, acct, acctlen);
        }
        free(bookval);
    }
    return retval;
}

int def_trans_ordtype(char *rom, romindex_t * map,
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
        }
        retval = fixmsg_addstr_bylen(fix, map[romfield].name, p, n);
        free(bookval);
    } else {
        retval = 0;
    }

    return retval;
}

int def_trans_capacity(char *rom, romindex_t * map, char *type,
                       databuf_t * fix, size_t field, int *rejected,
                       dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[10].end - map[10].beg;
        char *p = rom + map[10].beg;
        if (len > 0 && p[0] != '\0') {
            doa_set_capacity(order, p, len);
            result = fixmsg_addstr_bylen(fix, map[10].name, p, len);
        } else {
            p = doa_get_capacity(order, &len);
            if (p != 0 && len > 0) {
                result = fixmsg_addstr_bylen(fix, map[10].name, p, len);
                free(p);
            }
        }
    }
    return result;
}

int def_trans_execinst(char *rom, romindex_t * map, char *type,
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
            }
        }
    }
    return result;
}

int def_trans_account(char *rom, romindex_t * map,
                      char *fixtype, databuf_t * fix,
                      size_t romfield, int *rejected,
                      dart_ex_order_obj * order)
{
    int retval = 0;
    if (map[romfield].name != 0) {
        char *p = rom + map[romfield].beg;
        size_t len = map[romfield].end - map[romfield].beg;

        if (len > 0) {
            doa_set_account(order, p, len);
            retval = fixmsg_addstr_bylen(fix, map[romfield].name, p, len);
        } else {
            p = doa_get_account(order, &len);
            retval = fixmsg_addstr_bylen(fix, map[romfield].name, p, len);
            free(p);
        }
    }
    return retval;
}

int set_transacttime(char *rom, romindex_t * map, char *type,
                     databuf_t * fix, size_t field, int *rejected,
                     dart_ex_order_obj * order)
{
    struct tm *gmt = 0;
    time_t now = 0;
    time(&now);
    struct tm timestruct;
    gmt = gmtime_r(&now, &timestruct);
    return fixmsg_addtime(fix, "60", gmt);
}

int def_field_noop(char *rom, romindex_t * map, char *type,
                   databuf_t * fix, size_t field, int *rejected,
                   dart_ex_order_obj * order)
{
    return 0;
}

int def_add_header_fields(char *rommsg, struct dart_fix_pos *positions,
                          char *mtype, databuf_t * output,
                          size_t romfield, int *rejected,
                          dart_ex_order_obj * order)
{
    return fixmsg_addstr(output, "21", "1");    /* HandlInst must be 1 */
}

/**
 * This functions tests the callback into the main application.
 */
void from_rom_func(struct message *mess, void *b)
{
    databuf_t *output = databuf_alloc(mess->len * 8);
    orderbook *ob = (orderbook *) b;
    long total_len;
    struct message m1;
    char fix_type[3];
    int ft_len = 0;
    int rejected = 0;
    total_len = 0;

    dart_rolling_log_coming(mess, ob->module_name, ob->module_name_len);

    if (ob->is_connected()) {
        m1.name = mess->name;
        m1.name_len = mess->name_len;
        memset(fix_type, 0, 3);
        total_len =
            trans_rom_message(&m1, mess->data, mess->len, output,
                              fix_type, &ft_len, &rejected,
                              (orderbook *) ob, &rom2fix_translate_fields);
        if (rejected) {
            m1.data = output->buffer;
            m1.len = databuf_unread(output);
            send_message_to_roms(&m1, ob);
        } else if (total_len > 0) {
            m1.name = ob->fix_con_name;
            m1.name_len = ob->fix_con_len;
            m1.data = output->buffer;
            m1.len = databuf_unread(output);
            send_fix_message(&m1, fix_type, ft_len);
        }
    } else {
        char *l_message = "Not connected to Exchange.";
        rejected = 1;
        m1.name = mess->name;
        m1.name_len = mess->name_len;
        total_len = create_rom_reject(mess->data, output, l_message);
        if (total_len > 0) {
            m1.data = output->buffer;
            m1.len = databuf_unread(output);
            send_message_to_roms(&m1, ob);
        } else {
            databuf_destroy(output);
            return;
        }
    }
    databuf_destroy(output);
}

static void rec_in(struct message *mess)
{
    struct incoming_message *m1;
    m1 = create_incoming_message(mess->name, mess->name_len, 1);
    record_inbound_message(m1);
}

static char scftype(char rom_type, struct message *mess, int *send_on)
{
    switch (rom_type) {
    case 'E':
        rec_in(mess);
        *send_on = 1;
        return 'D';
    case 'R':
        rec_in(mess);
        *send_on = 1;
        return 'G';
    case 'C':
        rec_in(mess);
        *send_on = 1;
        return 'F';
    case 'A':
        rec_in(mess);
        *send_on = 0;
    default:
        *send_on = 0;
        return 'h';
    }
}

void from_rom_helper_func(void *r, struct message *mess, void *b)
{
    databuf_t *output = databuf_alloc(mess->len * 8);
    struct parse_con *pc = (struct parse_con *) b;
    orderbook *ob = (orderbook *) pc->book;
    dart_order_obj *rph = (dart_order_obj *) r;
    long total_len;
    struct message m1;
    char fix_type[3];
    int ft_len = 0;
    int rejected = 0;
    int send_on = 0;
    total_len = 0;

    dart_rolling_log_coming(mess, ob->module_name, ob->module_name_len);

    if (ob->is_connected()) {
        m1.name = mess->name;
        m1.name_len = mess->name_len;
        fix_type[0] = scftype(mess->data[0], mess, &send_on);
        fix_type[1] = '\0';
        ft_len = 1;
        if (send_on) {
            total_len = rom2fix_translate_fields(&m1, mess->data, output,
                                                 mess->len, fix_type,
                                                 rph->positions, &rejected,
                                                 ob, 0);
            if (rejected) {
                m1.data = output->buffer;
                m1.len = databuf_unread(output);
                send_message_to_roms(&m1, ob);
            } else if (total_len > 0) {
                m1.name = ob->fix_con_name;
                m1.name_len = ob->fix_con_len;
                m1.data = output->buffer;
                m1.len = databuf_unread(output);
                send_fix_message(&m1, fix_type, ft_len);
            }
        }
    } else {
        char *l_message = "Not connected to Exchange.";
        rejected = 1;
        m1.name = mess->name;
        m1.name_len = mess->name_len;
        total_len = create_rom_reject(mess->data, output, l_message);
        if (total_len > 0) {
            m1.data = output->buffer;
            m1.len = databuf_unread(output);
            send_message_to_roms(&m1, ob);
        } else {
            databuf_destroy(output);
            return;
        }
    }
    databuf_destroy(output);
}

long rom2fix_translate_fields(struct message *mess, char *rommsg,
                              databuf_t * output, long fix_len,
                              char *fix_type,
                              struct dart_fix_pos positions[ROM_LEN],
                              int *rejected, orderbook * book, int is_spread)
{
    int i = 1;
    /* Verify type is exactly one character */
    char mtype = (fix_type && fix_type[0] != '\0') ? fix_type[0] : '\0';
    dart_ex_order_obj *obj = NULL;
    int ret;
    char *beg_off;
    long len;
    //int is_spread = 0;
    if (mtype == '\0') {
        /* Unexpected message type, reject message */
    }
//    book->add_header_defs(rommsg, positions, fix_type, output, 0, rejected,
 //                         obj);
    if (*rejected != 0) {
        return databuf_unread(output);
    }
    //is_spread = (strncmp(fix_type, "AB", 2) == 0);
    if (mtype == 'D' || (is_spread && strncmp(fix_type, "AB", 2) == 0)) {
        size_t clordid_len = 0;
        char *cl_ord_id = book->generate_id(&clordid_len);
        obj =
            establish_order(rommsg, &book->mutex, book->cl_ord_id_map,
                            book->rom_ord_id_map, positions, mess,
                            cl_ord_id, clordid_len, book);
        book->add_header_defs(rommsg, positions, fix_type, output, 0, rejected,
                              obj);
        fixmsg_addstr(output, "11", cl_ord_id);
        if (is_spread) {
            doa_set_is_spread(obj, is_spread);
        }
        free(cl_ord_id);
    } else {
        beg_off = rommsg;
        beg_off += positions[17].beg;
        len = (positions[17].end - positions[17].beg);
        pthread_mutex_lock(&book->mutex);
        ret =
            get_map_entry(book->rom_ord_id_map, beg_off, len,
                          (void *) &obj);
        pthread_mutex_unlock(&book->mutex);
        if (ret > 0 && (mtype == 'F' || !doa_get_pending_replace(obj))) {
            book->add_header_defs(rommsg, positions, fix_type, output, 0, rejected,
                                  obj);
            size_t t_len = 0;
            char *l_type = doa_get_type(obj, &t_len);
            size_t clordid_len = 0;
            size_t rep_id_len = 0;
            char *rep_ord_id = book->generate_id(&rep_id_len);
            char *cl_ord_id = doa_get_cl_ord_id(obj, &clordid_len);
            fixmsg_addstr_bylen(output, "11", rep_ord_id, rep_id_len);
            doa_set_rep_ord_id(obj, rep_ord_id, rep_id_len);
            pthread_mutex_lock(&book->mutex);
            no_copy_insert(book->cl_ord_id_map, rep_ord_id, rep_id_len,
                           obj);
            pthread_mutex_unlock(&book->mutex);
            fixmsg_addstr_bylen(output, "41", cl_ord_id, clordid_len);

            free(l_type);
            free(rep_ord_id);
            free(cl_ord_id);
        } else if (ret > 0 && doa_get_pending_replace(obj)) {
            int rej_len = 0;
            char *l_message = "Order is pending Replace.";
            databuf_reset(output);
            rej_len =
                build_cnl_rpl_rej(beg_off, len, fix_type, output,
                                  l_message);
            *rejected = 1;
            return rej_len;
        } else {
            int rej_len = 0;
            char *l_message =
                mtype ==
                'F' ? "Unable to find rom tag too late to cancel?" :
                "Unable to find rom tag too late to replace?";
            databuf_reset(output);
            rej_len =
                build_cnl_rpl_rej(beg_off, len, fix_type, output,
                                  l_message);
            *rejected = 1;
            return rej_len;
        }
    }

    for (i = 1; i < ROM_LEN; ++i) {
        if (book->rom2fix[i] != 0) {
            if (book->rom2fix[i]
                (rommsg, positions, fix_type, output, i, rejected,
                 obj) == -1 || *rejected != 0) {
                char reject_reason[64] = { 0 };
                if (*rejected > 1) {
                    /* The output variable should be the error message */
                    const char *errmsgptr = output->rd_ptr;
                    size_t errmsglen = databuf_unread(output);
                    if (memchr(errmsgptr, ',', errmsglen) != 0 ||
                        memchr(errmsgptr, '\n', errmsglen) != 0) {
                        reject_reason[0] = '\0';
                    } else {
                        if (errmsglen >= (sizeof reject_reason - 1)) {
                            errmsglen = sizeof reject_reason - 1;
                        }
                        memcpy(reject_reason, errmsgptr, errmsglen);
                        reject_reason[errmsglen] = '\0';
                    }
                } else {
                    snprintf(reject_reason, sizeof reject_reason,
                             "Error processing ROM field %d", i);
                }
                databuf_reset(output);
                *rejected = 1;
                fprintf(stderr, "%s\n", reject_reason);
                return create_rom_reject(rommsg, output, reject_reason);
            }
        }
    }

    if (!*rejected) {
        databuf_memcpy(output, "\x01", 1);
    }

    return databuf_unread(output);
}
