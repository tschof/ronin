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

static token_struct_t *idgenerator = 0;
static char fixserversection[] = "FIX SERVER";
static const char acctkey[] = "Account";
static char *configacct = 0;
static size_t configacctlen = 0;

static char *fxall_fix_generate_id(size_t * nlen)
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

static int fxall_translate_account(char *rom, romindex_t * map,
                                   char *fixtype, databuf_t * fix,
                                   size_t romfield, int *rejected,
                                   dart_ex_order_obj * order)
{
    if (map[romfield].name == 0)
        return 0;
    char *acct = configacct, *storedval = 0;
    size_t nacct = configacctlen;
    if (nacct == 0 || acct == 0) {
        acct = rom + map[romfield].beg;
        ptrdiff_t tmp = map[romfield].end - map[romfield].beg;
        if (tmp > -1)
            nacct = (size_t) tmp;
        else
            nacct = 0;
    }
    if (acct != 0 && nacct > 0)
        doa_set_account(order, acct, nacct);
    else
        storedval = acct = doa_get_account(order, &nacct);
    int retval = fixmsg_addstr_bylen(fix, map[romfield].name, acct, nacct);
    free(storedval);
    return retval;
}

static int fxall_trans_symbol(char *rom, romindex_t * map,
                              char *fixtype, databuf_t * fix,
                              size_t romfield, int *rejected,
                              dart_ex_order_obj * order)
{
    int retval = -1;
    if (map[romfield].name != 0) {
        char *sym = rom + map[romfield].beg;
        size_t nsym = map[romfield].end - map[romfield].beg;
        if (nsym > 0) {
            size_t i;
            char *slash = 0;
            for (i = 0; i < nsym; ++i) {
                if (sym[i] == '/') {
                    slash = &(sym[i]);
                    break;
                }
            }
            if (slash != 0) {
                if ((retval =
                     fixmsg_addstr_bylen(fix, "15", sym,
                                         (int) (slash - sym))) > -1) {
                    retval =
                        fixmsg_addstr_bylen(fix, map[romfield].name, sym,
                                            (int) nsym);
                }
            }
        }
    }
    return retval;
}

int
fxall_trans_status(char *fixmsg, dart_rom_pos * xlateinfo, char mtype,
                   char *status, databuf_t * rom, size_t romfield,
                   int *rejected, dart_ex_order_obj * order,
                   pthread_mutex_t * mutex, hashmap_t map)
{
    int result = -1;
    if (mtype == 'S') {
        char *msgstatus = fixmsg + xlateinfo[romfield].beg;
        signed long int msgstatuslen =
            xlateinfo[romfield].end - xlateinfo[romfield].beg;
        if (msgstatuslen > 0) {
            if (msgstatuslen == 1 && msgstatus[0] == 'F') {
                /* Map from FIX 4.3 ExecType of F to a FIX 4.2 ExecType of
                   1 (Partial Fill) or 2 (Fill) to satisfy the rest of the
                   framework and ROM. */
                /* ROM field 3 is an arbitrary place to stash the ordstatus,
                   we do not really intend that ROM field to be sent along. */
                char *ordstatus = fixmsg + xlateinfo[3].beg;
                signed long int statlen =
                    xlateinfo[3].end - xlateinfo[3].beg;
                if (statlen > 0) {
                    status = ordstatus;
                    result = 0;
                }
            } else {
                result = 0;
            }
        }
    } else {
        result = 0;
    }

    if (result == 0) {
        if (romfield == 14 && mtype != 'U') {
            result = determine_trans_type(romfield, status, rom, 1);
        } else if (romfield == 14 && mtype == 'U') {
            result = set_status_for_cnrej(fixmsg, xlateinfo, status, rom);
        }
    }

    return result;
}

static int fxall_custom_init(ex_config * exchange_config,
                             orderbook * pbook)
{
    int result = -1;
    idgenerator = init_order_token_creator();
    if (idgenerator != 0) {
        int len = 0;
        configacct = get_val_for_tag(exchange_config->dc, fixserversection,
                                     sizeof fixserversection - 1,
                                     acctkey, sizeof acctkey - 1, &len);
        if (len > 0) {
            configacctlen = (size_t) len;
        }

        if (def_init_translators(pbook, ROM_LEN) > -1) {
            pbook->rom2fix[4] = def_trans_side;
            pbook->rom2fix[5] = fxall_trans_symbol;
            pbook->rom2fix[10] = def_field_noop;
            pbook->rom2fix[12] = fxall_translate_account;
            pbook->rom2fix[17] = def_field_noop;

            pbook->fix2rom[14] = fxall_trans_status;

            result = 0;
        }
    }
    return result;
}

/* Set SenderSubID (50) with first 4 chars of clearing account;
   set HandlInst (21) to "1". */
static int fxall_pre_translate(char *rom, romindex_t * map, char *type,
                               databuf_t * fix, size_t field,
                               int *rejected, dart_ex_order_obj * order)
{
    int result = -1;
    if (type[0] == 'D' && type[1] == '\0') {
        result = fixmsg_addstr(fix, "21", "1");
    } else {
        result = 0;
    }
    return result;
}

static const char configfile[] = "fxall.dat";

int generic_fix_interface_main(int argc, char *argv[],
                               struct fix_interface *handler)
{
    int result = -1;
    if (handler != 0) {
        handler->config_file = configfile;
        handler->file_len = sizeof configfile - 1;
        handler->onstart = fxall_custom_init;
        handler->pre_translate_func = fxall_pre_translate;
        handler->id_generator = fxall_fix_generate_id;
        result = 0;
    }
    return result;
}
