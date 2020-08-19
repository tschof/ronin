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
#include "version.h"
#include <pthread.h>
#include "basicstring.h"
#include "symbol-map.h"
#include "perrchk.h"

static token_struct_t *idgenerator = 0;
static char fixserversection[] = "FIX SERVER";
static synch_symbolmap_t *symbolmapper = 0;

static char *liffe_us_fix_generate_id(size_t * nlen)
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

/* This function only translates FIX tag 58 (Text) fields that have at
 * least one non-numeric character. For some reason Liffe_Us sends a
 * timestamp in the text field. We only want to send actual text
 * strings back in ROM field 26.
 */
static int liffe_us_fix2rom_trans_text(char *fixmsg,
                                       dart_rom_pos * positions,
                                       char mtype
                                       __attribute__ ((unused)),
                                       char *status
                                       __attribute__ ((unused)),
                                       databuf_t * rom, size_t romfield,
                                       int *rejected
                                       __attribute__ ((unused)),
                                       dart_ex_order_obj * order
                                       __attribute__ ((unused)),
                                       pthread_mutex_t * mutex
                                       __attribute__ ((unused)),
                                       hashmap_t ord_map
                                       __attribute__ ((unused)))
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

static int liffeus_getsymboltosend(string_t symbol, databuf_t * buf)
{
    int map_ok = -1;
    databuf_reset(buf);
    string_t root, expiration, mapped_root;
    if (parse_futures_symbol(symbol, &root, &expiration) == 0 &&
        synchronized_lookup_symbol(symbolmapper, root, &mapped_root) == 0
        && databuf_memcpy(buf, mapped_root.ptr, mapped_root.len) != -1
        && databuf_memcpy(buf, expiration.ptr, expiration.len) != -1) {
        map_ok = 0;
    }
    return map_ok;
}

/* We are requiring that the symbol is already a RIC code. Goldman
   requires that we send it both as the symbol (55) and as the
   SecurityID (48) along with an IDSource (22) of 5 to indicate RIC
   symbology. */
static int liffe_us_map_symbol(char *rom,
                               romindex_t * map,
                               char *fixtype __attribute__ ((unused)),
                               databuf_t * fix,
                               size_t romfield,
                               int *rejected __attribute__ ((unused)),
                               dart_ex_order_obj * order
                               __attribute__ ((unused)))
{
    int retval = -1;
    ssize_t symlength = map[romfield].end - map[romfield].beg;
    if (symlength > 0) {
        string_t input_symbol
            = string_init(map[romfield].beg + rom, (size_t) symlength);
        char scratch_space[64];
        databuf_t buf;
        if (databuf_attach(&buf, scratch_space, sizeof scratch_space) ==
            -1) {
            goto done;
        }
        if ((symbolmapper == 0 ||
             liffeus_getsymboltosend(input_symbol, &buf) != 0)) {
            if (databuf_memcpy(&buf, input_symbol.ptr, input_symbol.len) ==
                -1) {
                goto done;
            }
        }
        if (fixmsg_addstr_bylen
            (fix, "55", buf.rd_ptr, databuf_unread(&buf)) != -1
            && fixmsg_addstr_bylen(fix, "48", buf.rd_ptr,
                                   databuf_unread(&buf)) != -1
            && fixmsg_addstr_bylen(fix, "22", "5", 1) != -1) {
            retval = 0;
        }
    }
  done:
    return retval;
}

int liffe_us_map_account(char *rom, romindex_t * map,
                         char *fixtype __attribute__ ((unused)),
                         databuf_t * fix, size_t romfield,
                         int *rejected __attribute__ ((unused)),
                         dart_ex_order_obj * order)
{
    int retval = -1;
    ssize_t acctlen = map[romfield].end - map[romfield].beg;
    const char *acctptr = rom + map[romfield].beg;
    char *bookval = 0;
    if (acctlen > 0) {
        doa_set_account(order, acctptr, (size_t) acctlen);
    } else {
        acctptr = bookval = doa_get_account(order, (size_t *) & acctlen);
    }
    retval =
        fixmsg_addstr_bylen(fix, map[romfield].name, acctptr,
                            (int) acctlen);
    free(bookval);
    return retval;
}

int liffe_us_translate_sectype(char *rom, romindex_t * map, char *fixtype
                               __attribute__ ((unused)), databuf_t * fix,
                               size_t romfield, int *rejected,
                               dart_ex_order_obj * order
                               __attribute__ ((unused)))
{
    int result = -1;
    ssize_t typelen = map[romfield].end - map[romfield].beg;
    const char *romtype = rom + map[romfield].beg;
    if (typelen == 1 && romtype[0] == 'F') {
        result = fixmsg_addstr(fix, "167", "FUT");
    } else {
        *rejected = 2;
        databuf_reset(fix);
        databuf_write(fix, "Invalid security type: only futures allowed");
    }
    return result;
}

long liffe_set_status_for_cnrej(char *fix_message,
                                struct dart_rom_pos positions[],
                                char *status, databuf_t * output)
{
    long int status_len = positions[14].end - positions[14].beg;
    status = fix_message + positions[14].beg;
    if (status_len != 1 || status[0] != '8') {
        ssize_t cxl_rej_response_to_len =
            positions[63].end - positions[63].beg;
        if (cxl_rej_response_to_len == 1) {
            const char *cxl_rej_response_to_ptr =
                fix_message + positions[63].beg;
            status = "20";
            status_len = 2;
            if (cxl_rej_response_to_ptr[0] == '1') {
                status = "14";
            }
        }
    }
    return databuf_memcpy(output, status, status_len);
}

int liffe_us_fixrom_status_xlate(char *fixmsg, dart_rom_pos * positions,
                                 char mtype, char *status,
                                 databuf_t * rom, size_t romfield,
                                 int *rejected __attribute__ ((unused)),
                                 dart_ex_order_obj * order
                                 __attribute__ ((unused)),
                                 pthread_mutex_t * mutex
                                 __attribute__ ((unused)), hashmap_t
                                 __attribute__ ((unused)) ord_map)
{
    status = fixmsg + positions[romfield].beg;
    long len = positions[romfield].end - positions[romfield].beg;
    if (romfield == 14) {
        if (mtype != 'U') {
            len = determine_trans_type(romfield, status, rom, len);
        } else {
            len =
                liffe_set_status_for_cnrej(fixmsg, positions, status, rom);
        }
    }
    return 1;
}

static const char *symbol_mapping_key_name = "SymbolMappingFile";
static const char *ucode = 0;

static int liffe_us_custom_init(ex_config * exchange_config,
                                orderbook * pbook)
{
    int result = -1;
    idgenerator = init_order_token_creator();
    if (idgenerator != 0) {
        int len;
        char *mapfile =
            get_val_for_tag(exchange_config->dc, fixserversection,
                            sizeof fixserversection - 1,
                            symbol_mapping_key_name,
                            (int) strlen(symbol_mapping_key_name),
                            &len);
        if (len > 0) {
            if ((symbolmapper =
                 create_synchronized_symbolmap(mapfile)) == 0) {
                fprintf(stderr, "Cannot load from file: %s\n", mapfile);
            }
            free(mapfile);
        }

        ucode =
            get_val_for_tag(exchange_config->dc, fixserversection,
                            sizeof fixserversection - 1, "ucode", 5, &len);

        pbook->rom2fix[5] = liffe_us_map_symbol;
        pbook->rom2fix[8] = def_trans_ordtype;
        pbook->rom2fix[12] = liffe_us_map_account;
        pbook->rom2fix[17] = def_field_noop;
        pbook->rom2fix[35] = liffe_us_translate_sectype;
        pbook->fix2rom[14] = liffe_us_fixrom_status_xlate;
        pbook->fix2rom[26] = liffe_us_fix2rom_trans_text;

        result = 0;
    }

    return result;
}

static int liffe_us_pre_translate(char *rom
                                  __attribute__ ((unused)),
                                  romindex_t * map
                                  __attribute__ ((unused)), char *type
                                  __attribute__ ((unused)),
                                  databuf_t * fix, size_t field
                                  __attribute__ ((unused)), int *rejected
                                  __attribute__ ((unused)),
                                  dart_ex_order_obj * order
                                  __attribute__ ((unused)))
{
    if (ucode) {
        fixmsg_addstr(fix, "50", ucode);
    }
    fixmsg_addstr(fix, "21", "1");
    return 0;
}

static const char configfile[] = "liffe-us.dat";

int generic_fix_interface_main(int argc
                               __attribute__ ((unused)), char *argv[]
                               __attribute__ ((unused)),
                               struct fix_interface *handler)
{
    int result = -1;
    if (handler != 0) {
        handler->config_file = configfile;
        handler->file_len = sizeof configfile - 1;
        handler->onstart = liffe_us_custom_init;
        handler->pre_translate_func = liffe_us_pre_translate;
        handler->id_generator = liffe_us_fix_generate_id;
        result = 0;
    }
    return result;
}
