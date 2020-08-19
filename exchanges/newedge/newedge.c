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
#include "destmap.h"

static token_struct_t *idgenerator = 0;
static char fixserversection[] = "FIX SERVER";
static synch_symbolmap_t *symbolmapper = 0;
static struct destmap *destmap = 0;

static char *newedge_fix_generate_id(size_t * nlen)
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
 * least one non-numeric character. For some reason Newedge sends a
 * timestamp in the text field. We only want to send actual text
 * strings back in ROM field 26.
 */
static int newedge_fix2rom_trans_text(char *fixmsg,
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

static int newedge_getsymboltosend(string_t symbol, databuf_t * buf)
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

static int newedge_translate_destination(char *rom,
                                         romindex_t * map,
                                         char *fixtype
                                         __attribute__ ((unused)),
                                         databuf_t * fix, size_t romfield,
                                         int *rejected
                                         __attribute__ ((unused)),
                                         dart_ex_order_obj * order
                                         __attribute__ ((unused)))
{
    int retval = 0;
    if (fixtype[0] == 'D') {
        /* Newedge has an issue with us sending ExDestination on
         * Cancel/Replace and Cancel messages, only send on New Order
         * - Single. */
        ssize_t ndest = map[romfield].end - map[romfield].beg;
        if (ndest > 0) {
            char *pdest = rom + map[romfield].beg, *exdest = 0;
            long destnum = strtol(pdest, 0, 10);
            if (destmap && destmap_findnum(destmap, destnum, &exdest) == 0
                && exdest != 0) {
                retval = fixmsg_addstr(fix, "100", exdest);
            }
        }
    }
    return retval;
}

static int newedge_map_symbol(char *rom,
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
             newedge_getsymboltosend(input_symbol, &buf) != 0)) {
            if (databuf_memcpy(&buf, input_symbol.ptr, input_symbol.len) ==
                -1) {
                goto done;
            }
        }
        if (fixmsg_addstr_bylen
            (fix, "55", buf.rd_ptr, databuf_unread(&buf)) != -1) {
            retval = 0;
        }
    }
  done:
    return retval;
}

int newedge_translate_sectype(char *rom, romindex_t * map, char *fixtype
                              __attribute__ ((unused)), databuf_t * fix,
                              size_t romfield, int *rejected,
                              dart_ex_order_obj * order
                              __attribute__ ((unused)))
{
    int result = -1;
    if (fixtype[0] == 'D') {
        ssize_t typelen = map[romfield].end - map[romfield].beg;
        const char *romtype = rom + map[romfield].beg;
        if (typelen == 1 && romtype[0] == 'F') {
            result = fixmsg_addstr(fix, "167", "FUT");
        } else if (typelen == 1 && romtype[0] == 'O') {
            result = fixmsg_addstr(fix, "167", "OPT");
        } else {
            *rejected = 2;
            databuf_reset(fix);
            databuf_write(fix,
                          "Invalid security type");
        }
    } else {
        result = 0;
    }
    return result;
}

static const char *symbol_mapping_key_name = "SymbolMappingFile";

static int newedge_custom_init(ex_config * exchange_config,
                               orderbook * pbook)
{
    int result = -1;
    idgenerator = init_order_token_creator();
    if (idgenerator != 0) {
        int mapfilelen;
        char *mapfile =
            get_val_for_tag(exchange_config->dc, fixserversection,
                            sizeof fixserversection - 1,
                            symbol_mapping_key_name,
                            (int) strlen(symbol_mapping_key_name),
                            &mapfilelen);
        if (mapfilelen > 0) {
            if ((symbolmapper =
                 create_synchronized_symbolmap(mapfile)) == 0) {
                fprintf(stderr, "Cannot load from file: %s\n", mapfile);
            }
            free(mapfile);
        }

        /* Newedge can accept orders sent to multiple destinations,
         * routed through Newedge.  In our system we create a
         * different DestID for each destination, setting all of their
         * DestRoutID to this Newedge interface.  We then switch on
         * the DestID to determine the ExDestination (100) FIX tag to
         * send to Newedge which then directs them to send to the
         * proper destination.  This mapping can change over time and
         * is therefore configurable. */
        int ndestmap = 0;
        char *pdestmap =
            get_val_for_tag(exchange_config->dc, fixserversection,
                            sizeof fixserversection - 1,
                            "DestMap", 7, &ndestmap);
        if (ndestmap <= 0) {
            fprintf(stderr, "Must specify a DestMap key in config\n");
            return -1;
        } else if ((destmap = destmap_fromstr(pdestmap)) == 0) {
            fprintf(stderr, "Invalid destmap: '%s'", pdestmap);
            free(pdestmap);
            return -1;
        } else {
            free(pdestmap);
        }

	set_option_rom2fix(pbook);

        pbook->rom2fix[5] = newedge_map_symbol;
        pbook->rom2fix[8] = def_trans_ordtype;
        pbook->rom2fix[13] = newedge_translate_destination;
        pbook->rom2fix[17] = def_field_noop;
        pbook->rom2fix[35] = newedge_translate_sectype;
        pbook->fix2rom[26] = newedge_fix2rom_trans_text;

        result = 0;
    }

    return result;
}

/* Set SenderSubID (50) with first 4 chars of clearing account; set
   HandlInst (21) to "1". */
static int newedge_pre_translate(char *rom
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
    if (type[0] != 'F') {
        fixmsg_addstr(fix, "21", "1");
    }
    return 0;
}

static const char configfile[] = "newedge.dat";

int generic_fix_interface_main(int argc
                               __attribute__ ((unused)), char *argv[]
                               __attribute__ ((unused)),
                               struct fix_interface *handler)
{
    int result = -1;
    if (handler != 0) {
        handler->config_file = configfile;
        handler->file_len = sizeof configfile - 1;
        handler->onstart = newedge_custom_init;
        handler->pre_translate_func = newedge_pre_translate;
        handler->id_generator = newedge_fix_generate_id;
        result = 0;
    }
    return result;
}
