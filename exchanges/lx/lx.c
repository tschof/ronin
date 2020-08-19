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
static char *defroutinst = 0;
static char defroutinstkey[] = "DefRouteInst";
static char execinstkey[] = "ExecInst";
static char *execinst = 0;

static char *lx_fix_generate_id(size_t * nlen)
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

int
lx_trans_acct_and_store(char *rom, romindex_t * map, char *type,
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
            if (retval > -1) {
                int nwritten =
                    fixmsg_addstr_bylen(fix, "116", acct, acctlen);
                if (nwritten > -1) {
                    retval += nwritten;
                }
            }
        }
        free(bookval);
    }
    return retval;
}

/*!
 * \brief Translate Side from ROM to FIX, virtually identical
 *        default processing except it doesn't send 114=N.
 */
int lx_trans_side(char *rom, romindex_t * map, char *type,
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
        }
        free(bookval);
    } else {
        result = 0;
    }

    return result;
}

static int lx_custom_init(ex_config * exchange_config, orderbook * pbook)
{
    int result = -1;
    idgenerator = init_order_token_creator();
    if (idgenerator != 0) {
        int len = 0;
        defroutinst =
            get_val_for_tag(exchange_config->dc, fixserversection,
                            sizeof fixserversection - 1, defroutinstkey,
                            sizeof defroutinstkey - 1, &len);
        if (defroutinst == 0) {
            printf("Default Routing Instruction missing from "
                   "configuration, defaulting to 'LX'\n");
            defroutinst = "LX";
        }
        execinst =
            get_val_for_tag(exchange_config->dc, fixserversection,
                            sizeof fixserversection - 1, execinstkey,
                            sizeof execinstkey - 1, &len);

        pbook->rom2fix[4] = lx_trans_side;
        pbook->rom2fix[5] = def_trans_symbol_and_suffix;
        pbook->rom2fix[8] = def_trans_ordtype;
        pbook->rom2fix[12] = lx_trans_acct_and_store;
        pbook->rom2fix[17] = def_field_noop;

        result = 0;
    }

    return result;
}

/* Set SenderSubID (50) with first 4 chars of clearing account;
   set HandlInst (21) to "1". */
static int lx_pre_translate(char *rom, romindex_t * map, char *type,
                            databuf_t * fix, size_t field, int *rejected,
                            dart_ex_order_obj * order)
{
    int result = -1;
    if (type[0] == 'D' && type[1] == '\0') {
        result = fixmsg_addstr(fix, "21", "1");
    } else {
        result = 0;
    }
    {
        int nwritten = fixmsg_addstr(fix, "100", "83");
        if (nwritten > -1) {
            result += nwritten;
        } else {
            result = -1;
        }
    }
    return result;
}

int generic_fix_interface_main(int argc, char *argv[],
                               struct fix_interface *handler)
{
    int result = -1;
    if (handler != 0) {
        handler->config_file = "lx.dat";
        handler->onstart = lx_custom_init;
        handler->pre_translate_func = lx_pre_translate;
        handler->id_generator = lx_fix_generate_id;
        result = 0;
    }
    return result;
}
