#include <string.h>
#include "order_token_creator.h"
#include "config_loader.h"
#include "fixmsg.h"
#include "rom2fix.h"
#include "ex_config_helper.h"
#include "generic_fix_interface.h"
#include "ex_common.h"
#include "suffixmap.h"
#include "destmap.h"
#include <errno.h>

static token_struct_t *idgenerator = 0;
static struct destmap_t *destmap = 0;

int blz_translate_symbol(char *rom, romindex_t * map, char *fixtype,
                         databuf_t * fix, size_t field, int *rejected,
                         dart_ex_order_obj * order)
{
    int result = -1;
    char *sym = rom + map[field].beg;
    size_t symlen = map[field].end - map[field].beg;
    if (symlen > 0) {
        char buf[24];
        if (symlen < sizeof buf) {
            char *p = 0;
            memcpy(buf, sym, symlen);
            buf[symlen] = '\0';
            if ((p = strstr(buf, ".TO")) != 0) {
                *p = '\0';
            } else if ((p = strstr(buf, ".TV")) != 0) {
                *p = '\0';
            }
            /* Handle the case in which there is a suffix in
               addition to the .TO suffix, e.g. SU.B.TO */
            p = strchr(buf, '.');
            if (p == 0) {
                result = fixmsg_addstr(fix, map[field].name, buf);
            } else {
                result =
                    fixmsg_addstr_bylen(fix, map[field].name, sym,
                                        p - buf);
                if (result != -1) {
                    result += fixmsg_addstr(fix, "65", p);
                }
            }
        } else {
            fprintf(stderr,
                    "symbol length exceeds internal buffer size\n");
        }
    }

    return result;
}

int blz_translate_ordtype(char *rom, romindex_t * map, char *fixtype,
                          databuf_t * fix, size_t field, int *rejected,
                          dart_ex_order_obj * order)
{
    int result = -1;

    char *p = rom + map[field].beg;
    char *bookval = 0;

    size_t n = 0;
    signed long msg_val_len = map[field].end - map[field].beg;

    if (msg_val_len > 0) {
        n = (size_t) msg_val_len;
        doa_set_type(order, p, n);
    } else {
        /* Get from the book */
        p = bookval = doa_get_type(order, &n);
    }

    if (n > 0) {
        if (n == 2 && p[0] == '1' && p[1] == '1') {
            p = "B";
            n = 1;
        } else {
            const char *execinst = rom + map[57].beg;
            signed long execinstlen = map[57].end - map[57].beg;
            if (execinstlen == 1) {
                switch (*execinst) {
                case 'P':
                case 'M':
                case 'R':
                    p = "P";
                    n = 1;
                    break;
                }
            }
        }

        result = fixmsg_addstr_bylen(fix, map[field].name, p, n);
    }

    /* bookval is either 0 or points to malloc'd memory from
     * doa_get_type */
    free(bookval);

    return result;
}

int blz_translate_capacity(char *rom, romindex_t * map, /*@unused@ */
                           char *type, databuf_t * fix, size_t field,
                           /*@unused@ */ int *rejected,
                           dart_ex_order_obj * order)
{
    int result = -1;
    char *p = rom + map[field].beg;
    char *bookcap = 0;
    signed long int msglen = map[field].end - map[field].beg;
    size_t n = 0;
    if (msglen > 0) {
        n = (size_t) msglen;
        doa_set_capacity(order, p, n);
    } else {
        p = bookcap = doa_get_capacity(order, &n);
    }

    if (n == 1 && p[0] == 'A') {
        result = fixmsg_addstr(fix, map[field].name, "CL");
        result = 0;
    } else if (rejected != 0) {
        *rejected = 1;
        result = databuf_reset(fix);
        if (result != -1) {
            result = (int) create_rom_reject(rom, fix, "invalid capacity");
        }
    }

    free(bookcap);
    return result;
}


int blz_trans_clearing(char *rom, romindex_t * map,     /*@unused@ */
                       char *type, databuf_t * fix, size_t field,       /*@unused@ */
                       int *rejected, dart_ex_order_obj * order)
{
    int result = -1;

    char *p = 0;
    size_t n = 0;

    char *romfield = rom + map[field].beg;
    ptrdiff_t romfieldlen = map[field].end - map[field].beg;

    char *bookacct = 0;
    size_t bookacctlen = 0;

    if (romfieldlen > 0) {
        doa_set_account(order, romfield, romfieldlen);
        p = romfield;
        n = (size_t) romfieldlen;
    } else {
        bookacct = doa_get_account(order, &bookacctlen);
        if (bookacct != 0 && bookacctlen > 0) {
            p = bookacct;
            n = bookacctlen;
        }
    }

    if (n > 0 && p != 0 && p[0] != '\0') {
        /* The last 4 characters of the clearing
           account go into the mapped field */
        if (n > 4) {
            p += n - 4;
            n = 4;
        }
        result = fixmsg_addstr_bylen(fix, map[field].name, p, (int) n);
    }

    free(bookacct);

    return result;
}

/* There is a special setup here in that the destination (ROM field
   13) will determine the routing instructions (FIX tag 113) value on
   the outbound.  The mapping is determined by the DestMap
   configuration setting in the INI file. */
static int blz_map_routing_instructions(char *rom, romindex_t * map,
                                        char *type, databuf_t * fix,
                                        size_t field, int *rejected,
                                        dart_ex_order_obj * order)
{
    int result = -1;
    if (map[field].name != 0) {
        const char *p = rom + map[field].beg;
        size_t n = map[field].end - map[field].beg;
        char buf[16];
        if ((n > 0) && (n < (sizeof buf))) {
            char *exdest = 0;
            memset(buf, 0, sizeof buf);
            memcpy(buf, p, n);
            buf[n] = '\0';
            result = destmap_find(destmap, buf, &exdest);
            if (result == 0) {
                result = fixmsg_addstr(fix, map[field].name, exdest);
            } else {
                /* Okay if not found in destmap, it defaults at BLZ. */
                result = 0;
            }
        }

        if (result < 0) {
            result = -1;
            databuf_reset(fix);
            (void) create_rom_reject(rom, fix,
                                     "DART: No routing instructions");
            *rejected = 1;
        }
    } else {
        result = 0;
    }
    return result;
}

int blz_map_short_locate(char *rom, romindex_t * map,   /*@unused@ */
                         char *type, databuf_t * fix, size_t field,     /*@unused@ */
                         int *rejected, dart_ex_order_obj * order)
{
    int result = -1;
    if (map[field].name != 0) {
        char *p = rom + map[field].beg;
        char *book_locate = 0;
        signed long int msglen = map[field].end - map[field].beg;
        size_t n = 0;
        if (msglen > 0) {
            n = (size_t) msglen;
            doa_set_short_locate(order, p, n);
        } else {
            p = book_locate = doa_get_short_locate(order, &n);
        }

        if (p != 0) {
            result = fixmsg_addstr_bylen(fix, map[field].name, p, n);
        } else {
            result = 0;
        }

        free(book_locate);
    } else {
        result = 0;
    }

    return result;
}

long blz_set_status_for_cnrej(char *fix_message,
                              struct dart_rom_pos positions[],
                              char *status, databuf_t * output,
                              dart_ex_order_obj * order)
{
    long result = -1;
    char *cc_off = fix_message + positions[63].beg;
    status = fix_message + positions[14].beg;
    if (positions[63].beg != positions[63].end) {
        if (cc_off[0] == '1') {
            result = databuf_write(output, "14");
        } else {
            result = databuf_write(output, "20");
        }
    } else {
        /* For some silly destinations that refuse to send back FIX
           tag 434 (mapped from ROM tag 63), we compare the
           OrigClOrdID with the replacement ClOrdID, if they are the
           same we are getting a reject Cancel/Replace, if not we must
           be getting a reject on a Cancel. */
        size_t orig_cl_ord_id_len = positions[60].end - positions[60].beg;
        if (orig_cl_ord_id_len != 0) {
            size_t replace_id_len;
            char *replace_id = doa_get_rep_ord_id(order, &replace_id_len);
            if (replace_id != 0) {
                char *orig_cl_ord_id = fix_message + positions[60].beg;
                if (replace_id_len == orig_cl_ord_id_len &&
                    strncmp(replace_id, orig_cl_ord_id,
                            replace_id_len) == 0) {
                    /* This is a reject of a replace */
                    result = databuf_write(output, "20");
                } else {
                    /* A Cancel must have been rejected */
                    result = databuf_write(output, "14");
                }
                free(replace_id);
            }
        }
    }
    return result;
}

int blz_fixrom_status_xlate(char *fixmsg, dart_rom_pos * positions,
                            char mtype, char *status,
                            databuf_t * rom, size_t romfield,
                            int *rejtected, dart_ex_order_obj * order,
                            pthread_mutex_t * mutex, hashmap_t ord_map)
{
    char *beg_off = fixmsg;
    size_t len = (positions[romfield].end - positions[romfield].beg);
    beg_off += positions[romfield].beg;
    if (romfield == 14 && mtype != 'U') {
        len = determine_trans_type(romfield, beg_off, rom, len);
        status = fixmsg;
        status += positions[romfield].beg;
    } else if (romfield == 14 && mtype == 'U') {
        len =
            blz_set_status_for_cnrej(fixmsg, positions, status, rom,
                                     order);
    }
    return 1;
}

int is_shortsale_exempt(char *rom, romindex_t * map, char *type,
                        databuf_t * fix, size_t field, int *rejected,
                        dart_ex_order_obj * order)
{
    int retval = -1;
    /* The determination as to whether the order is shortsale exempt
       or just shortsale is determined by the value of the short
       lender. A short lender of "ETFX" indicates exempt; any other
       value results in a regular short sale. */
    char *shortlender = 0, *bookval = 0;
    size_t lender_len = 0;
    if (map[58].name != 0) {
        long tmplen = map[58].end - map[58].beg;
        shortlender = rom + map[58].beg;
        if (tmplen > 0) {
            lender_len = (size_t) tmplen;
        }
    }

    if (lender_len == 0) {
        /* Either the locate lender was not mapped for this message
           type or the short locate lender was not specified in this
           message; the backup plan is to look in the book for the
           short lender stored there. */
        shortlender = bookval = doa_get_short_locate(order, &lender_len);
    }

    if (lender_len == 4
        && shortlender[0] == 'E'
        && shortlender[1] == 'T'
        && shortlender[2] == 'F' && shortlender[3] == 'X') {
        retval = 1;
    } else {
        retval = 0;
    }

    free(bookval);
    return retval;
}

/*  If a LocateBroker field needs to be sent, the mapping can be done
 *  via the standard mapping tables, it needn't be done in code and is
 *  not needed for exchange destinations.
 *
 *  In order to send Short Sale Exempt orders to Canadian exchanges
 *  (allowed for short sales on the down tick for ETFs for instance),
 *  we inspect the short lender; if it is ETFX and the side is short
 *  sale exempt, then we send the order as short sale exempt (54=6).
 */
static int
blz_translate_side(char *rom, romindex_t * map, char *type,
                   databuf_t * fix, size_t field, int *rejected,
                   dart_ex_order_obj * order)
{
    int result = 0;
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
            if (n == 1 && side[0] == '5') {
                result = fixmsg_addstr(fix, "114", "N");
                if (result > -1) {
                    int is_exempt =
                        is_shortsale_exempt(rom, map, type, fix, field,
                                            rejected, order);
                    if (is_exempt == 1) {
                        side = "6";
                        n = 1;
                    } else if (is_exempt == -1) {
                        result = -1;
                    }
                }
            }

            if (result > -1) {
                int nwritten =
                    fixmsg_addstr_bylen(fix, map[field].name, side, n);
                if (nwritten > -1) {
                    result += nwritten;
                } else {
                    result = -1;
                }
            }
        } else {
            result = -1;
        }
        free(bookval);
    }

    return result;
}

static int blz_custom_init(ex_config * exchange_config, orderbook * pbook)
{
    int result = -1;
    idgenerator = init_order_token_creator();
    if (idgenerator != 0) {
        pbook->rom2fix[4] = blz_translate_side;
        pbook->rom2fix[5] = blz_translate_symbol;
        pbook->rom2fix[8] = blz_translate_ordtype;
        pbook->rom2fix[10] = blz_translate_capacity;
        /* TIF mapping? If we're doing Limit on Close there should be
           a TIF mapping in addition to an order type mapping. */
        pbook->rom2fix[12] = blz_trans_clearing;
        pbook->rom2fix[13] = blz_map_routing_instructions;
        pbook->rom2fix[17] = def_field_noop;
        pbook->rom2fix[58] = blz_map_short_locate;

        pbook->fix2rom[14] = blz_fixrom_status_xlate;
        pbook->fix2rom[15] = def_handle_ordids;
        pbook->fix2rom[17] = def_add_rom_tag;
        pbook->fix2rom[26] = def_map_fix_text_to_rom;
        pbook->fix2rom[63] = def_no_op;

        /* Initialize the Destination->ExDest mapping mechanism. */
        {
            int nlen = -1;
            char *mapconfig =
                get_val_for_tag(exchange_config->dc, "BLZ", 3, "DestMap",
                                7, &nlen);
            if (mapconfig != 0 && nlen > 0) {
                destmap = destmap_fromstr(mapconfig);
                if (destmap == 0) {
                    fprintf(stderr, "Error parsing DestMap\n");
                    exit(1);
                }
            } else {
                fprintf(stderr, "Fatal error! Cannot find [BLZ] DestMap "
                        "entry in configuration!\n");
                exit(1);
            }
        }

        result = 0;
    }
    return result;
}

static int blz_pre_translate(char *rom, romindex_t * map,
                             char *type, databuf_t * fix,
                             size_t field, int *rejected,
                             dart_ex_order_obj * order)
{
    return fixmsg_addstr(fix, "21", "1");
}

static char *blz_fix_generate_id(size_t * nlen)
{
    char *cl_ord_id = 0;
    if (nlen != 0) {
        int retval = 0;
        cl_ord_id = calloc(DART_EX_TAG_LEN, 1);
        if ((retval = create_order_token(idgenerator, cl_ord_id,
                                         DART_EX_TAG_LEN - 1)) > 0) {
            *nlen = (size_t) retval;
        } else {
            *nlen = 0;
        }
    }
    return cl_ord_id;
}

int generic_fix_interface_main(int argc, char *argv[],
                               struct fix_interface *handler)
{
    int result = -1;
    if (handler != 0) {
        handler->config_file = "blz.dat";
        handler->onstart = blz_custom_init;
        handler->pre_translate_func = blz_pre_translate;
        handler->id_generator = blz_fix_generate_id;
        result = 0;
    }
    return result;
}
