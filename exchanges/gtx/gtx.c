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

static char *gtx_fix_generate_id(size_t * nlen)
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

static int gtx_translate_ordtype(char *rom, romindex_t * map,
                                 char *fixtype, databuf_t * fix,
                                 size_t romfield, int *rejected,
                                 dart_ex_order_obj * order)
{
    int retval = -1;
    if (map[romfield].name != 0) {
        char *p = rom + map[romfield].beg;
        size_t len = map[romfield].end - map[romfield].beg;
        int delete_p = 0;

        /* The only mapping for order type is for Peg orders; pegs are
           indicated by an execution instruction (ROM field 57). */
        if ((map[57].end - map[57].beg) == 1) {
            switch (*(rom + map[57].beg)) {
            case 'P':
            case 'M':
            case 'R':
                p = "P";
                len = 1;
                break;
            }
        } else if (len == 1 && p[0] == '5') {
            /* Market on close */
            p = "1";
        } else if (len == 2 && p[0] == '1' && p[1] == '1') {
            /* Limit on close */
            p = "2";
            len = 1;
        }

        if (len < 1) {
            p = doa_get_type(order, &len);
            delete_p = 1;
        } else {
            doa_set_type(order, p, len);
        }

        retval = fixmsg_addstr_bylen(fix, map[romfield].name, p, len);

        if (delete_p) {
            free(p);
        }
    } else {
        retval = 0;
    }
    return retval;
}

static int gtx_translate_tif(char *rom, romindex_t * map,
                             char *fixtype, databuf_t * fix,
                             size_t romfield, int *rejected,
                             dart_ex_order_obj * order)
{
    int result = -1;
    if (map[romfield].name != 0) {
        const char *p = rom + map[romfield].beg;
        size_t len = map[romfield].end - map[romfield].beg;
        const char *ordtype = rom + map[8].beg;
        size_t ordtypelen = map[8].end - map[8].beg;
        if (ordtypelen == 1 && ordtype[0] == '5') {
            /* Market on close */
            p = "7";
        } else if (ordtypelen == 2 && ordtype[0] == '1'
                   && ordtype[1] == '1') {
            /* Limit on close */
            p = "7";
            len = 1;
        }

        if (len < 1) {
            p = doa_get_tif(order, &len);
        } else {
            doa_set_tif(order, p, len);
        }

        result = fixmsg_addstr_bylen(fix, map[romfield].name, p, len);
    } else {
        result = 0;
    }

    return result;
}

static int gtx_translate_instructions(char *rom, romindex_t * map,
                                      char *fixtype, databuf_t * fix,
                                      size_t romfield, int *rejected,
                                      dart_ex_order_obj * order)
{
    int result = -1;
    const char *p = rom + map[romfield].beg;
    size_t len = map[romfield].end - map[romfield].beg;
    if (len > 0) {
        const char *dash = memchr(p, '-', len);
        if (dash == 0) {
            result = fixmsg_addstr_bylen(fix, "76", p, len);
        } else {
            result = fixmsg_addstr_bylen(fix, "76", p, dash - p);
            if (result != -1) {
                const char *end = p + len;
                p = dash + 1;
                if (p < end) {
                    int nwritten =
                        fixmsg_addstr_bylen(fix, "9355", p, end - p);
                    if (nwritten != -1) {
                        result += nwritten;
                    } else {
                        result = -1;
                    }
                }
            }
        }
    } else {
        result = 0;
    }
    return result;
}

static int gtx_translate_exdest(char *rom, romindex_t * map,
                                char *fixtype, databuf_t * fix,
                                size_t romfield, int *rejected,
                                dart_ex_order_obj * order)
{
    int result = 0;
    const char *p = rom + map[romfield].beg;
    size_t len = map[romfield].end - map[romfield].beg;
    if ((map[57].end - map[57].beg) == 1) {
        /* All peg orders must be GTX-only */
        switch (*(rom + map[57].beg)) {
        case 'P':
        case 'M':
        case 'R':
            p = "GTX";
            len = 4;
            break;
        }
    } else if (((map[15].end - map[15].beg)) == 0) {
        /* Translate the ROM EX_DEST field if the INSTRUCTIONS field is empty,
           otherwise the INSTRUCTIONS will be parsed and translated. */
        if (len == 1) {
            if (p[0] == 'N' || p[0] == 'A') {
                p = "DOTI";
                len = 4;
            } else if (p[0] == 'O') {
                p = "STGY";
                len = 4;
            }
        } else if (len == 7 && strncmp("Direct+", p, 7) == 0) {
            p = "DOTI";
            len = 4;
        } else {
            p = defroutinst;
            len = strlen(defroutinst);
        }

        {
            const char *tif = rom + map[9].beg;
            const size_t tiflen = map[9].end - map[9].beg;
            if (tiflen == 1 && tif[0] == '2') {
                /* TIF is OPG */
                result = fixmsg_addstr(fix, "9355", "O");
            }
        }
    }

    if (result != -1 && len > 0) {
        int nwritten =
            fixmsg_addstr_bylen(fix, map[romfield].name, p, len);
        if (nwritten != -1) {
            result += nwritten;
        }
    }

    return result;
}

static int gtx_translate_maxfloor(char *rom, romindex_t * map,
                                  char *fixtype, databuf_t * fix,
                                  size_t romfield, int *rejected,
                                  dart_ex_order_obj * order)
{
    int result = -1;
    const char *p = rom + map[romfield].beg;
    const size_t len = map[romfield].end - map[romfield].beg;
    if (len != 0) {
        if (len == 1 && p[0] == '0') {
            result = fixmsg_addstr(fix, "9140", "N");
        } else {
            result = fixmsg_addstr_bylen(fix, map[romfield].name, p, len);
        }
    } else {
        result = 0;
    }
    return result;
}

static int gtx_set_execinst(char *rom, romindex_t * map, char *fixtype,
                            databuf_t * fix, size_t romfield,
                            int *rejected, dart_ex_order_obj * order)
{
    int result = 0;
    if (map[romfield].name != 0 && execinst != 0) {
        result = fixmsg_addstr(fix, map[romfield].name, execinst);
    }
    return result;
}

static int gtx_trans_side(char *rom, romindex_t * map,
                          char *fixtype, databuf_t * fix,
                          size_t romfield, int *rejected,
                          dart_ex_order_obj * order)
{
    int result = -1;
    if (map[romfield].name != 0) {
        result =
            def_trans_side(rom, map, fixtype, fix, romfield, rejected,
                           order);
    } else {
        result = 0;
    }
    return result;
}

static int gtx_trans_price(char *rom, romindex_t * map,
                           char *fixtype, databuf_t * fix,
                           size_t romfield, int *rejected,
                           dart_ex_order_obj * order)
{
    int result = -1;
    if (map[romfield].name != 0) {
        result =
            def_rom2fix(rom, map, fixtype, fix, romfield, rejected, order);
    } else {
        result = 0;
    }
    return result;
}

int
gtx_trans_status(char *fixmsg, dart_rom_pos * xlateinfo, char mtype,
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
                char *leavesqtystr = fixmsg + xlateinfo[49].beg;
                signed long int leavesqtylen =
                    xlateinfo[49].end - xlateinfo[49].beg;
                if (leavesqtylen > 0) {
                    int leavesqty = atoi(leavesqtystr);
                    if (leavesqty == 0) {
                        status = "2";
                    } else {
                        status = "1";
                    }
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


static int gtx_custom_init(ex_config * exchange_config, orderbook * pbook)
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
                   "configuration, defaulting to 'GTX'\n");
            defroutinst = "GTX";
        }
        execinst =
            get_val_for_tag(exchange_config->dc, fixserversection,
                            sizeof fixserversection - 1, execinstkey,
                            sizeof execinstkey - 1, &len);

        pbook->rom2fix[4] = gtx_trans_side;     /*def_trans_side; */
        pbook->rom2fix[7] = gtx_trans_price;
        pbook->rom2fix[8] = gtx_translate_ordtype;
        pbook->rom2fix[9] = gtx_translate_tif;
        pbook->rom2fix[10] = def_field_noop;
        pbook->rom2fix[12] = def_field_noop;    /*gtx_translate_clearing_account; */
        pbook->rom2fix[15] = gtx_translate_instructions;
        pbook->rom2fix[17] = def_field_noop;
        pbook->rom2fix[29] = gtx_translate_exdest;
        pbook->rom2fix[34] = gtx_translate_maxfloor;
        pbook->rom2fix[57] = gtx_set_execinst;

        pbook->fix2rom[14] = gtx_trans_status;

        result = 0;
    }

    return result;
}

/* Set SenderSubID (50) with first 4 chars of clearing account;
   set HandlInst (21) to "1". */
static int gtx_pre_translate(char *rom, romindex_t * map, char *type,
                             databuf_t * fix, size_t field, int *rejected,
                             dart_ex_order_obj * order)
{
    int result = -1;
    if (type[0] == 'D' && type[1] == '\0') {
        result = fixmsg_addstr(fix, "21", "1");
    } else {
        result = 0;
    }
    return result;
}

int generic_fix_interface_main(int argc, char *argv[],
                               struct fix_interface *handler)
{
    int result = -1;
    if (handler != 0) {
        handler->config_file = "gtx.dat";
        handler->onstart = gtx_custom_init;
        handler->pre_translate_func = gtx_pre_translate;
        handler->id_generator = gtx_fix_generate_id;
        result = 0;
    }
    return result;
}
