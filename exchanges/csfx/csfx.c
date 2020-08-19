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
#include "fix_handler.h"

static token_struct_t *idgenerator = 0;
static char fixserversection[] = "FIX SERVER";
static char *account = 0;
static char *client_id = 0;
static char account_key[] = "Account";
static char client_id_key[] = "ClientID";
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

static char *fix_generate_id_with_date(size_t * nlen)
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

static char *csfx_fix_generate_id(size_t * nlen)
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

/* This function only translates FIX tag 58 (Text) fields that have at least
 * one non-numeric character. For some reason Csfx sends a timestamp in the
 n* text field. We only want to send actual text strings back in ROM field 26.
 */
static int csfx_fix2rom_trans_text(char *fixmsg,
                                   dart_rom_pos * positions, char mtype,
                                   char *status, databuf_t * rom,
                                   size_t romfield, int *rejected,
                                   dart_ex_order_obj * order,
                                   pthread_mutex_t * mutex,
                                   hashmap_t ord_map)
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

int csfx_trans_ordtype(char *rom, romindex_t * map,
                       char *fixtype, databuf_t * fix,
                       size_t romfield, int *rejected,
                       dart_ex_order_obj * order)
{
    int retval = -1;
    if (map[romfield].name != 0) {
        char *p = rom + map[romfield].beg, *bookval = 0;
        ssize_t len = map[romfield].end - map[romfield].beg;
        size_t n = 0;
        if (len > 0) {
            n = (size_t) len;
            doa_set_type(order, p, n);
        } else {
            p = bookval = doa_get_type(order, &n);
        }

        /* Our ROM order type of limit maps to 'D', "Previously Quoted", since
           the order is actually "hitting" a quote delivered by Credit Suisse. */
        if (n == 1 && p[0] == '2') {
            p = "D";
            n = 1;
        }

        retval = fixmsg_addstr_bylen(fix, map[romfield].name, p, n);
        free(bookval);
    } else {
        retval = 0;
    }

    return retval;
}

int csfx_trans_symbol(char *rom, romindex_t * map,
                      char *fixtype, databuf_t * fix,
                      size_t romfield, int *rejected,
                      dart_ex_order_obj * order)
{
    int retval = -1;
    if (map[romfield].name != 0) {
        char *p = rom + map[romfield].beg, *bookval = 0;
        ssize_t len = map[romfield].end - map[romfield].beg;
        size_t n = 0;
        if (len > 0) {
            n = (size_t) len;
            doa_set_symbol(order, p, n);
        } else {
            p = bookval = doa_get_symbol(order, &n);
        }

        /* In our system the side and quantity always apply to the base
           currency. For Credit Suisse we must set the Currency field to the base
           currency to signify this. Here we find the base currency and set the
           field. */
        void *slashp = memchr(p, '/', n);
        if ((retval =
             fixmsg_addstr_bylen(fix, map[romfield].name, p, n)) != -1) {
            retval =
                fixmsg_addstr_bylen(fix, "15", p,
                                    (const char *) slashp - p);
        }
        free(bookval);
    } else {
        retval = 0;
    }

    return retval;
}

static int csfx_custom_init(ex_config * exchange_config, orderbook * pbook)
{
    int result = -1;
    if (initialize_date_string() == 0) {
        idgenerator = init_order_token_creator();
        if (idgenerator != 0) {
            int len = 0;
            if ((account =
                 get_val_for_tag(exchange_config->dc, fixserversection,
                                 sizeof fixserversection - 1, account_key,
                                 sizeof account_key - 1, &len)) == 0) {
                printf("[%s] %s mapping missing\n", fixserversection,
                       account_key);
                return -1;
            }
            if ((client_id =
                 get_val_for_tag(exchange_config->dc, fixserversection,
                                 sizeof fixserversection - 1,
                                 client_id_key, sizeof client_id_key - 1,
                                 &len)) == 0) {
                printf("[%s] %s key missing\n", fixserversection,
                       client_id_key);
            }

            pbook->rom2fix[3] = def_field_noop;
            pbook->rom2fix[5] = csfx_trans_symbol;
            pbook->rom2fix[8] = csfx_trans_ordtype;
            pbook->rom2fix[17] = def_field_noop;
            pbook->rom2fix[35] = def_trans_sec_type;

            pbook->fix2rom[26] = csfx_fix2rom_trans_text;

            result = 0;
        }
    }
    return result;
}

/* Set SenderSubID (50) with first 4 chars of clearing account;
   set HandlInst (21) to "1". */
static int csfx_pre_translate(char *rom, romindex_t * map, char *type,
                              databuf_t * fix, size_t field,
                              int *rejected, dart_ex_order_obj * order)
{
    int retval = -1;
    if (fixmsg_addstr(fix, "21", "1") != -1
        && account != 0 && fixmsg_addstr(fix, "1", account) != -1
        && client_id != 0 && fixmsg_addstr(fix, "109", client_id) != -1) {
        retval = 0;
    }
    return retval;
}

void csfx_on_session_status_changed(con_obj * connection,
                                    int is_established, char *log, int len)
{
    /* Call the default functionality. */
    default_on_session_status_changed(connection, is_established, log,
                                      len);

    /* For Credit Suisse FX, we must not do a Session Status Request. */
    if (is_established != 0) {
        databuf_t *msgbuf = databuf_alloc(256);
        if (msgbuf != 0) {
            size_t idlen;
            char *id = csfx_fix_generate_id(&idlen);
            if (fixmsg_addstr(msgbuf, "263", "1") != -1
                && fixmsg_addstr_bylen(msgbuf, "335", id, idlen) != -1
                && databuf_write(msgbuf, "%c", '\x01')) {
                struct message *m = malloc(sizeof *m);
                m->name = (char *) get_name(connection);
                m->name_len = get_name_len(connection);
                m->data = msgbuf->rd_ptr;
                m->len = databuf_unread(msgbuf);
                (void) send_fix_message(m, "g", 1u);
                (void) databuf_destroy(msgbuf);
            }
            free(id);
        }
    }
}

static const char configfile[] = "csfx.dat";

int generic_fix_interface_main(int argc, char *argv[],
                               struct fix_destination_interface *handler)
{
    int result = -1;
    if (handler != 0) {
        handler->config_file_name = configfile;
        handler->file_name_length = sizeof configfile - 1;
        handler->on_start = csfx_custom_init;
        handler->on_session_status_change = csfx_on_session_status_changed;
        handler->pre_translate_func = csfx_pre_translate;
        handler->id_generator = fix_generate_id_with_date;
        result = 0;
    }
    return result;
}
