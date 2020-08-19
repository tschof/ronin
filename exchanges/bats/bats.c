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
#include "dart_constants.h"
#include "ex_config_helper.h"
#include "generic_fix_interface.h"
#include "ex_common.h"

static token_struct_t *idgenerator = 0;
static char fixserversection[] = "FIX SERVER";
//static char clearingidkey[] = "ClearingID";
static hashmap_t clearing_ids;
static char *clearing_acct_tag = 0;
static char clearing_acct_tag_key[] = "ClearingAcctTag";
static char *defroutinst = 0;
static char defroutinstkey[] = "DefRouteInst";


hashmap_t load_clearing_ids(const char* filename)
{
    hashmap_t h = create_map(5);
    gen_build_clear_id_map(filename, h);
    return h;
}
static int bats_trans_capacity(char *rom, romindex_t * map, char *type,
                       databuf_t * fix, size_t field, int *rejected,
                       dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        size_t len = map[10].end - map[10].beg;
        char *p = rom + map[10].beg;
        if (len > 0 && p[0] != '\0') {
            switch(p[0]) {
                case 'B':
                    p = "J";
                    len = 1;
                    break;
                case 'S':
                    p = "N";
                    len = 1;
                    break;
                case 'X':
                    p = "U";
                    len = 1;
                    break;
            }
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
static char *bats_fix_generate_id(size_t * nlen)
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
int bats_trans_account(char *rom, romindex_t * map,
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
            if(len > 4 && strncmp("CRON", p, 4) == 0) {
                len = len -4;
                p += 4;
            }
            retval = fixmsg_addstr_bylen(fix, "440", p, len);
        } else {
            p = doa_get_account(order, &len);
            int rewind = 0;
            retval = fixmsg_addstr_bylen(fix, map[romfield].name, p, len);
            if(len > 4 && strncmp("CRON", p, 4) == 0) {
                len = len -4;
                p += 4;
                rewind = 1;
            } 
            retval = fixmsg_addstr_bylen(fix, "440", p, len);
            if(rewind) {
                p -= 4;
            }
            free(p);
        }
    }
    return retval;
}
static int bats_trans_max_floor(char *rom, romindex_t * map, char *type,
                                   databuf_t * fix, size_t field,
                                   int *rejected,
                                   dart_ex_order_obj * order)
{
    int result = 0;
    size_t len = map[field].end - map[field].beg;
    char *p = rom + map[field].beg;
    if (len == 1 && strncmp(p, "0", 1) == 0) {
        result = fixmsg_addstr(fix, "9479", "I");
    } else if(len > 0) {
        result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
    }
    return result;
}

static int bats_trans_instructions(char *rom, romindex_t * map, char *type,
                                   databuf_t * fix, size_t field,
                                   int *rejected,
                                   dart_ex_order_obj * order)
{
    int result = -1;
    size_t len = map[field].end - map[field].beg;
    char *p = rom + map[field].beg;
    if (len == 0) {
        p = defroutinst;
        /* The only mapping for order type is for Peg orders; pegs are
           indicated by an execution instruction (ROM field 57). */
        if ((map[57].end - map[57].beg) == 1) {
            switch (*(rom + map[57].beg)) {
            case 'P':
            case 'M':
            case 'R':
            case 'm':
            case 'Q':
            case 'L':
            case 'U':
                p = "P";
                len = 1;
                break;
            }
        }
        result = fixmsg_addstr(fix, "9303", p);
    } else if(len == 1) {
        if (p[0] == 'A') {
            result = fixmsg_addstr(fix, "9303", defroutinst);
            if (result > -1) {
                result = fixmsg_addstr(fix, "9479", "V");
            }
        } else if (p[0] == 'I') {
            result = fixmsg_addstr(fix, "9303", defroutinst);
        } else {
            result = fixmsg_addstr_bylen(fix, "9303", p, len);
        }
    } else if(len >= 4) {
        char *dash = memchr(p, '-', len);
        if (dash == 0) {
            result = fixmsg_addstr_bylen(fix, "9303", p, len);
        } else if(p[0] == '-') {
            result = fixmsg_addstr_bylen(fix, "9400", p + 1, len -1);
        } else if(dash != 0 && (p[1] == 'D' || p[1] == 'L')) {
            result = fixmsg_addstr_bylen(fix, "9303", p, 2);
            if(p[2] == 'R') {
                char* rdm = p+ 2;
                result = fixmsg_addstr2(fix, "9350", 4, rdm, 3);//dash - p);
                char *end = p + len;
                rdm += 4;
                fixmsg_addstr2(fix, "9400", 4, rdm, end - rdm);
            } else {
                char* rdm = p+ 3;
                char *end = p + len;
                fixmsg_addstr2(fix, "9400", 4, rdm, end - rdm);
            }
        } else {
            result = fixmsg_addstr_bylen(fix, "9303", p, 1);
            if(p[1] == 'R') {
                char* rdm = p+ 1;
                result = fixmsg_addstr2(fix, "9350", 4, rdm, 3);//dash - p);
                char *end = p + len;
                rdm += 4;
                fixmsg_addstr2(fix, "9400", 4, rdm, end - rdm);
            } else {
                char *end = p + len;
                p = dash + 1;
                if (p < end) {
                    result =
                        fixmsg_addstr2(fix, "9400", 4, p, end - p);
                }
            }
        }
    } else {
        result = fixmsg_addstr_bylen(fix, "9303", p, len);
    }
    return result;
}

static int bats_set_clearingfirm(char *rom, romindex_t * map, char *type,
                                 databuf_t * fix, size_t field,
                                 int *rejected, dart_ex_order_obj * order)
{
    int result = 0;
    char* p = 0;
    size_t len = map[field].end - map[field].beg;
    if (len > 0) {
        p = rom + map[field].beg;
        set_val_in_order_struct(p, len, field, order, type);
    } else {
        p = doa_get_clr_id(order, &len);
    }
    if(len > 0) {
        char* rom_val= 0;
        int found =
            find_n_fill(clearing_ids, p, len, (void *) &rom_val);
        if(found) {
            databuf_write(fix, "%c%s=", FIX_FIELD_DELIM, clearing_acct_tag);
            databuf_memcpy(fix, rom_val, found);
        } 
    }
    return result;
    //return fixmsg_addstr(fix, clearing_acct_tag, clearingfirm);
}

static int bats_transcmssymbol(char *rom, romindex_t * map, char *type,
                               databuf_t * fix, size_t field,
                               int *rejected, dart_ex_order_obj * order)
{
    int result = -1;
    size_t len = map[field].end - map[field].beg;
    char *p = rom + map[field].beg;
    if (len > 0 && p[0] != '\0') {
        doa_set_symbol(order, p, len);
    } else {
        p = doa_get_symbol(order, &len);
    }

    if (p != 0 && len > 0 && p[0] != '\0') {
        const char *pslash = memchr(p, '/', len);
        if (pslash == 0) {
            result = fixmsg_addstr_bylen(fix, map[field].name, p, len);
        } else {
            const int symlen = pslash - p;
            const int suffixlen = len - symlen - 1;
            result = fixmsg_addstr_bylen(fix, map[field].name, p, symlen);
            if (result > -1)
                result =
                    fixmsg_addstr_bylen(fix, "65", pslash + 1, suffixlen);
        }
        if (p != (rom + map[field].beg)) {
            free(p);
        }
    }
    return result;
}

/* The main difference from the default translation for
   ExecInst is that BATS routes default to w, Do Not DRT. */
static int bats_trans_execinst(char *rom, romindex_t * map, char *type,
                               databuf_t * fix, size_t field,
                               int *rejected, dart_ex_order_obj * order)
{
    int result = -1;
    if (type[0] == 'D') {
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
        } else {
            result = 0;
        }
    } else {
        /* ExecInst (FIX tag 18) is not needed on Cancel (35=F) or
           Cancel/Replace (35=G) messages. */
        result = 0;
    }
    return result;
}
static void oc_check_time_n_rej(char *beg_off, int len, char *l_fix_tag,
                             databuf_t * output, int *rejected,
                             char *mtype, int oc)
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
    if (oc == 1 && (cur_hour > 15 || (cur_hour == 15 && cur_min >= 55))) {
        int rej_len = 0;
        char *l_message =
            "MOC and LOC orders on BATS cannot be replaced or cancelled after 3:55 ET.";
        databuf_reset(output);
        if (mtype[0] == 'D') {
        } else {
            rej_len =
                build_cnl_rpl_rej(beg_off, len, l_fix_tag, output,
                                  l_message);
        }
        *rejected = 1;
    } else if(oc == 0 && cur_hour >= 9 && cur_min >= 28) {
        int rej_len = 0;
        char *l_message =
            "MOO and LOO orders on BATS cannot be replaced or cancelled after 9:28 ET.";
        databuf_reset(output);
        rej_len =
            build_cnl_rpl_rej(beg_off, len, l_fix_tag, output,
                              l_message);
        *rejected = 1;
    }

}
int bats_trans_tif(char *rom, romindex_t * map,
                       char *fixtype, databuf_t * fix,
                       size_t field, int *rejected,
                       dart_ex_order_obj * order)
{
    int result = 0;
    if (map[field].name != 0) {
        char *p = rom + map[field].beg;
        size_t len = map[field].end - map[field].beg;
        if (len > 0) {
            if(len == 1 && p[0] == '2' && fixtype[0] != 'D') {
                oc_check_time_n_rej(p, len, fixtype, fix, rejected, fixtype, 0);
            }
            char* type = rom + map[8].beg;
            size_t tlen = map[8].end - map[8].beg;
            if(tlen == 2 && strncmp(type, "11", 2) == 0) {
                p = "7";
                len = 1;
            } else {
               p = rom + map[field].beg;
            }
            set_val_in_order_struct(p, len, field, order, fixtype);
            result =
                fixmsg_addstr2(fix, map[field].name, map[field].len, p,
                               len);
        } else {
            p = doa_get_tif(order, &len);
            if(len == 1 && p[0] == '2' && fixtype[0] != 'D') {
                oc_check_time_n_rej(p, len, fixtype, fix, rejected, fixtype, 0);
            }
        }
    }
    return result;
}

int bats_trans_ordtype(char *rom, romindex_t * map,
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
        if(n == 2 && strncmp(p, "11", 2) == 0) {
            if(fixtype[0] != 'D') {
                oc_check_time_n_rej(p, n, fixtype, fix, rejected, fixtype, 1);
            }
            p = "2";
            n = 1;
        }

        /* The only mapping for order type is for Peg orders; pegs are
           indicated by an execution instruction (ROM field 57). */
        if ((map[57].end - map[57].beg) == 1) {
            switch (*(rom + map[57].beg)) {
            case 'P':
            case 'M':
            case 'R':
            case 'm':
            case 'Q':
            case 'L':
            case 'U':
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

static int bats_custom_init(ex_config * exchange_config, orderbook * pbook)
{
    int result = -1;
    int is_options = 0;
    idgenerator = init_order_token_creator();
    if (idgenerator != 0) {
        int len = 0;
        clearing_ids = load_clearing_ids("IDtomnemonic.map");
        /*clearingfirm =
            get_val_for_tag(exchange_config->dc, "FIX SERVER", 10,
                            clearingidkey, sizeof clearingidkey - 1, &len);
        if (clearingfirm == 0) {
            fprintf(stderr,
                    "No clearing firm specified in configuration\n");
        } else {
            result = 0;
        }*/
        char* options =  
            get_val_for_tag(exchange_config->dc, "FIX SERVER", 10,
                           "Options" , 7, &len);
        if(len > 0) {
            is_options = atoi(options);
        }
        clearing_acct_tag =
            get_val_for_tag(exchange_config->dc, "FIX SERVER", 10,
                            clearing_acct_tag_key,
                            sizeof clearing_acct_tag_key - 1, &len);
        if (clearing_acct_tag == 0) {
            clearing_acct_tag = "115";
            fprintf(stderr, "Clearing Account Tag not specified in "
                    "configuration, defaulting to %s\n",
                    clearing_acct_tag);
        }

        defroutinst =
            get_val_for_tag(exchange_config->dc, fixserversection,
                            sizeof fixserversection - 1, defroutinstkey,
                            sizeof defroutinstkey - 1, &len);
        if (defroutinst == 0) {
            printf("Default Routing Instruction missing from "
                   "configuration, defaulting to 'B'\n");
            defroutinst = "B";
        }
        if(is_options) {
            set_option_rom2fix(pbook);
        } else {
            pbook->rom2fix[12] = bats_trans_account;
        }
        pbook->rom2fix[5] = bats_transcmssymbol;
        pbook->rom2fix[8] = bats_trans_ordtype;
        pbook->rom2fix[9] = bats_trans_tif;
        pbook->rom2fix[11] = bats_set_clearingfirm;
        pbook->rom2fix[15] = bats_trans_instructions;
        pbook->rom2fix[34] = bats_trans_max_floor;
        pbook->rom2fix[17] = def_field_noop;
        pbook->rom2fix[57] = bats_trans_execinst;
        pbook->rom2fix[10] = bats_trans_capacity;
    }

    return result;
}

static int bats_pre_translate(char *rom, romindex_t * map,
                              char *type, databuf_t * fix,
                              size_t field, int *rejected,
                              dart_ex_order_obj * order)
{
    int result = -1;

    /* HandlInst (21) is the only field we must always add to BATS
     * orders. */
    result = fixmsg_addstr(fix, "21", "1");
    return result;
}

int generic_fix_interface_main(int argc, char *argv[],
                               struct fix_interface *handler)
{
    int result = -1;
    if (handler != 0) {
        handler->config_file = "bats.dat";
        handler->file_len = 8;
        handler->onstart = bats_custom_init;
        handler->pre_translate_func = bats_pre_translate;
        handler->id_generator = bats_fix_generate_id;
        result = 0;
    }
    return result;
}
