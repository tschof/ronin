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

static token_struct_t *idgenerator = 0;
static char datestr[9] = { '\0' };

static char fixserversection[] = "FIX SERVER";
static char *defroutinst = 0;
static char defroutinstkey[] = "DefRouteInst";
static char execinstkey[] = "ExecInst";
static char *execinst = 0;

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

static char *box_fix_generate_id(size_t * nlen)
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

int box_map_capacity(char *rom, romindex_t * map,
                     char *fixtype, databuf_t * fix,
                     size_t romfield, int *rejected,
                     dart_ex_order_obj * order)
{
    int result = -1;
    if (map[romfield].name == 0) {
        result = 0;
        goto done;
    }
    const char *capacity = rom + map[romfield].beg;
    ssize_t ncapacity = map[romfield].end - map[romfield].beg;
    if (ncapacity == 0) {
        *rejected = 1;
        goto done;
    }
    char combined_ordtype = '3';
    if (ncapacity == 1) {
        switch (capacity[0]) {
        case 'C':
        case 'A':
            combined_ordtype = '1';
            capacity = "C";
            break;
        case 'P':
        case 'R':
        case 'B':
            capacity = "F";
            break;
        case 'S':
            capacity = "X";
            break;
        }
    }
    if ((result = fixmsg_addstr_bylen(fix, map[romfield].name,
                                      capacity, ncapacity)) != -1) {
        int ntemp = fixmsg_addstr_bylen(fix, "7906", &combined_ordtype, 1);
        if (ntemp != -1) {
            result += ntemp;
        } else {
            *rejected = 1;
            result = -1;
        }
    }
  done:
    return result;
}

static int box_custom_init(ex_config * exchange_config, orderbook * pbook)
{
    int result = -1;
    idgenerator = init_order_token_creator();
    if (idgenerator != 0) {
        if (initialize_date_string() != -1) {
            int len = 0;
            defroutinst =
                get_val_for_tag(exchange_config->dc, fixserversection,
                                sizeof fixserversection - 1,
                                defroutinstkey, sizeof defroutinstkey - 1,
                                &len);
            if (defroutinst == 0) {
                printf("Default Routing Instruction missing from "
                       "configuration, defaulting to 'BOX'\n");
                defroutinst = "BOX";
            }
            execinst =
                get_val_for_tag(exchange_config->dc, fixserversection,
                                sizeof fixserversection - 1,
                                execinstkey, sizeof execinstkey - 1, &len);
            set_option_rom2fix(pbook);
            pbook->rom2fix[10] = box_map_capacity;
            result = 0;
        }
    }

    return result;
}

static int box_pre_translate(char *rom, romindex_t * map, char *type,
                             databuf_t * fix, size_t field,
                             int *rejected, dart_ex_order_obj * order)
{
    int result = 0;
    if (type[0] != 'F') {
        result = fixmsg_addstr_bylen(fix, "21", "1", 1);
    }
    return result;
}

int generic_fix_interface_main(int argc, char *argv[],
                               struct fix_interface *handler)
{
    int result = -1;
    if (handler != 0) {
        if (argc < 2) {
            fprintf(stderr, "usage: box conf-file\n");
            goto done;
        }
        handler->config_file = argv[1];
        handler->file_len = strlen(argv[1]);
        handler->onstart = box_custom_init;
        handler->pre_translate_func = box_pre_translate;
        handler->id_generator = box_fix_generate_id;
        result = 0;
    }
  done:
    return result;
}
