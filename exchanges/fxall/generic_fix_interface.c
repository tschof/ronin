/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

#include "client_manager.h"
#include "message_queue.h"
#include "parser_func.h"
#include "dart_constants.h"
#include "config_loader.h"
#include "ex_common.h"
#include "ex_config_helper.h"
#include "dart_orderbook.h"
#include "rom_handler.h"
#include "rom2fix.h"
#include "mailer.h"
#include "fix_handler.h"
#include "order_token_creator.h"
#include "timecalc.h"
#include "fixmsg.h"

/* Each project should put customized values and
   function declarations into this header file. */
#include "generic_fix_interface.h"

static pthread_mutex_t connection_lock = PTHREAD_MUTEX_INITIALIZER;

static char *fix_client_name;
static int fix_client_len;

static char *module_name = NULL;
static int module_name_len = 0;

static int is_con_now = 0;
static int is_genericfix_connected = 1;

int get_is_genericfix_connected()
{
    int ret = 0;
    pthread_mutex_lock(&connection_lock);
    ret = is_genericfix_connected;
    pthread_mutex_unlock(&connection_lock);
    return ret;
}

void set_is_genericfix_connected(int is)
{
    pthread_mutex_lock(&connection_lock);
    is_genericfix_connected = is;
    pthread_mutex_unlock(&connection_lock);
}

int generic_fix_initialize_translators(orderbook * pbook)
{
    int result = -1;
    if (pbook != 0) {
        size_t i;
        /* ROM to FIX */
        for (i = 0; i < ROM_LEN; ++i) {
            pbook->rom2fix[i] = def_rom2fix;
        }
        pbook->rom2fix[4] = def_trans_side;
        pbook->rom2fix[8] = def_trans_ordtype;
        pbook->rom2fix[10] = def_trans_capacity;
        pbook->rom2fix[50] = set_transacttime;
        pbook->rom2fix[57] = def_trans_execinst;

        /* FIX to ROM */
        pbook->fix2rom[63] = def_no_op;
        pbook->fix2rom[14] = def_fixrom_status_xlate;
        pbook->fix2rom[15] = def_handle_ordids;
        pbook->fix2rom[17] = def_add_rom_tag;
        pbook->fix2rom[14] = def_fixrom_status_xlate;

        result = 0;
    }
    return result;
}

int generic_fix_initialization(ex_config * exchange_config,
                               orderbook * pbook,
                               struct fix_interface *handler)
{
    int result = -1;
    if (handler != 0) {
        result = generic_fix_initialize_translators(pbook);
        if (result == 0) {
            database_init(exchange_config->db_user_name,
                          exchange_config->db_user_name_len,
                          exchange_config->db_password,
                          exchange_config->db_password_len,
                          exchange_config->db_server,
                          exchange_config->db_server_len,
                          exchange_config->dest_id,
                          exchange_config->dest_id_len, create_queue(),
                          pbook->cl_ord_id_map, pbook->rom_ord_id_map,
                          pbook);
            result = handler->onstart(exchange_config, pbook);
        }
    }
    return result;
}


void send_cancel_all();
void con_callBack(con_obj * co, int is_connected, char *log, int len);
void fix_server_connection_callBack(con_obj * co, int is_connected,
                                    char *log, int len);
void test_lac(con_obj * co);
void destroy_rom_queue(queue_t q);
int send_genericfix_logon(struct parse_args *pa);
char *generic_fix_generate_id(size_t * nlen);

static unsigned int ack_copy_fields[] = {
    37,                         /* OrderID */
    11,                         /* ClOrdID */
    17,                         /* ExecID */
    55,                         /* Symbol */
    64,                         /* FutSettDate */
    15,                         /* Currency */
    54,                         /* Side */
    38,                         /* OrderQty */
    382,                        /* NoContraBrokers */
    375,                        /* ContraBroker */
    32,                         /* LastQty */
    31,                         /* LastPx */
    14,                         /* CumQty */
    194,                        /* LastSpotRate */
};

static const size_t num_ack_copy_fields =
    sizeof ack_copy_fields / sizeof ack_copy_fields[0];

int intcompare(const void *left, const void *right)
{
    const int *l = left, *r = right;
    if (*l < *r) {
        return -1;
    } else if (*l > *r) {
        return +1;
    } else {
        return 0;
    }
}

static int shouldcopy(const char *tag)
{
    int retval = -1;
    if (tag == 0 || tag[0] == '\0') {
        errno = EINVAL;
        goto done;
    }
    int tagnum = atoi(tag);
    void *result = bsearch(&tagnum, ack_copy_fields, num_ack_copy_fields,
                           sizeof ack_copy_fields[0], intcompare);
    retval = result == 0 ? 0 : 1;
  done:
    return retval;
}

static int copy_execreport_fields(char *fixmsg, long fixmsglen, databuf_t * ack)
{
    int retval = -1;
    databuf_t *fixcpy = databuf_alloc(256);
    if (fixcpy == 0 || ack == 0) {
        goto done;
    }
    if (databuf_memcpy(fixcpy, fixmsg, fixmsglen) > -1) {
        if (fixmsglen != databuf_unread(fixcpy)) {
            goto done;
        }
        char *endptr, *startptr = fixcpy->rd_ptr, *tag;
        while ((tag = strtok_r(startptr, "=", &endptr)) != 0) {
            if (startptr != 0) {
                startptr = 0;
            }
            const char *val = strtok_r(startptr, "\x01", &endptr);
            if (val == 0) {
                /* If we have a tag without a value,
                   we must have a malformed message */
                retval = -1;
                break;
            }
            retval = 0;
            if (shouldcopy(tag) != 0 && fixmsg_addstr(ack, tag, val) < 0) {
                retval = -1;
                goto done;
            }
        }
    }
    databuf_destroy(fixcpy);
  done:
    return retval;
}

long fxall_fix_to_rom_override(struct message *mess, char *fix_message,
                               databuf_t * output, long fix_len,
                               char rom_type,
                               struct dart_rom_pos positions[ROM_LEN],
                               int *rejected, orderbook * ob)
{
    dart_ex_order_obj *obj = 0;
    size_t doa_len = 0;
    if (rom_type == 'U') {
        (void) databuf_write(output, "S,");
    } else {
        (void) databuf_write(output, "%c,", rom_type);
    }
    if (!get_order_object
        (fix_message, fix_len, ob->cl_ord_id_map, positions, &ob->mutex,
         rom_type, &obj, mess, output, ob)) {
        return 0;
    }
    mess->name = doa_get_sender_name(obj, &doa_len);
    mess->name_len = doa_len;
    char *status = fix_message + positions[14].beg;
    int i;
    for (i = 1; i < ROM_LEN; ++i) {
        if (ob->fix2rom[i] != 0) {
            if (ob->fix2rom[i] (fix_message, positions, rom_type,
                                status, output, i, rejected,
                                obj, &ob->mutex,
                                ob->cl_ord_id_map) == -1) {
                fprintf(stderr, "%s: Processing %d failed\n",
                        __FUNCTION__, i);
            }
        }
        databuf_memcpy(output, ",", 1);
    }
    databuf_memcpy(output, "\n", 1);

    /* Specific functionality for FXall. If this is an execution
       send back an Execution Report Acknowledgement. */
    if (status[0] == '1' || status[0] == '2' || status[0] == 'F') {
        databuf_t *ack = databuf_alloc(256);
        if (copy_execreport_fields(fix_message, fix_len, ack) > -1) {
            (void) databuf_memcpy(ack, "\x01", 1);
            struct message ackmsg = {
                .name = ob->fix_con_name,
                .name_len = ob->fix_con_len,
                .data = ack->buffer,
                .len = databuf_unread(ack),
            };
            (void) send_fix_message(&ackmsg, "BN", 2);
        }
    }

    if (status[0] == 'A') {
        return -1;
    }

    check_n_clean(status, ob->cl_ord_id_map, ob->rom_ord_id_map, obj,
                  &ob->mutex, rom_type, ob, fix_message, fix_len, positions);
    return databuf_unread(output);
}

void fxall_from_fix_func(struct message *mess, void *ob)
{
    struct databuf *output = databuf_alloc(mess->len * 5);
    struct message m1 = { 0 };
    int rejected = -1;
    orderbook *book = (orderbook *) ob;
    const long total_len =
        trans_fix_message(&m1, mess->data, mess->len, output,
                          &rejected, (orderbook *) ob,
                          fxall_fix_to_rom_override);
    if (total_len < 1) {
        free(m1.name);
        databuf_destroy(output);
        return;
    }

    m1.data = output->buffer;
    m1.len = databuf_unread(output);

    send_message_to_roms(&m1, (orderbook*)ob);

    struct message log = {
        .name = m1.name,
        .name_len = m1.name_len,
        .data = m1.data,
        .len = m1.len,
    };
    dart_rolling_log_going(&log, book->module_name, book->module_name_len);
    databuf_destroy(output);
    free(m1.name);
}

int main(int argc, char **argv)
{
    int ret;
    /* int fix_create_new = -1; */
    struct init_args *fix_args;
    ex_config *ex = NULL;
    struct tm *newtime;
    int keep_running = 1;
    queue_t rom_q;
    time_t long_time;
    orderbook *ob = NULL;
    int result = 0;
    struct fix_interface handler;

    /* About the first thing to do is to call the generic_fix_interface_main()
       function to pass control to the destination-specific code to setup the
       FIX destination interface handler. */
    memset(&handler, 0, sizeof handler);
    handler.config_file = "config.dat";
    if ((result = generic_fix_interface_main(argc, argv, &handler)) != 0) {
        fprintf(stderr, "Bailing out, init failed\n");
        return result;
    }
    rom_q = create_queue();
    ex = init_config(handler.config_file, handler.file_len);
    fix_args = load_fix_config(handler.config_file, handler.file_len);
    load_rom_config(ex->dc, rom_q);
    fix_client_name = fix_args->target_comp;
    fix_client_len = fix_args->tc_len;

    ob = init_order_book(handler.id_generator, get_is_genericfix_connected,
                         def_rom2fix, handler.pre_translate_func,
                         def_xlate_fixrom, 50000,
                         ex->mod_name, ex->mod_name_len, fix_client_name,
                         fix_client_len, NULL, 0);
    startup_allocator(ex->order_allocation);
    module_name_len = ex->mod_name_len;
    module_name = calloc(module_name_len + 1, 1);
    memcpy(module_name, ex->mod_name, module_name_len);;
    start_dart_mail_service(ex->mail_server, ex->mail_server_len,
                            ex->biz_group, ex->b_len, ex->tech_group,
                            ex->t_len, ex->end_hour, ex->end_min,
                            ex->end_day);

    /* Sort the ack fields */
    qsort(ack_copy_fields, num_ack_copy_fields,
          sizeof ack_copy_fields[0], intcompare);

    if (generic_fix_initialization(ex, ob, &handler) == -1) {
        fprintf(stderr, "Fatal initialization error, exiting\n");
        return 1;
    }

    ret =
        start_dart_client_services(&from_rom_func, ex->mod_name,
                                   ex->mod_name_len, ex->rom_port, rom_q,
                                   ex->rom_ip, ex->rom_ip_len,
                                   ex->end_hour, ex->end_min, ex->end_day,
                                   ex->heart_beat_int, ob, con_callBack,
                                   parse_rom_message, build_offsets,
                                   test_lac, create_rom_resend_pad, 0);


    ret =
        dart_connect(&fxall_from_fix_func, ex->mod_name, ex->mod_name_len,
                     ex->fix_port, ex->fix_ip, ex->fix_ip_len,
                     ex->end_hour, ex->end_min, ex->end_day,
                     ex->heart_beat_int, ob,
                     fix_server_connection_callBack, parse_fix_message,
                     fix_args, send_genericfix_logon, build_fix_offsets,
                     low_activity_sig, 1);

    while (keep_running) {
        time(&long_time);
        struct tm lm;
        newtime = localtime_r(&long_time, &lm);
        newtime = &lm;
        if (newtime->tm_hour == ex->end_hour
            && newtime->tm_min >= ex->end_min) {
            keep_running = 0;
            printf("Shutting down. end_hour %d end_min %d now %d\n",
                   ex->end_hour, ex->end_min, newtime->tm_hour);
        } else {
            sleep(25);
            if (!get_is_genericfix_connected()) {
                set_is_genericfix_connected(1);
                ret =
                    dart_connect(&from_fix_func, ex->mod_name,
                                 ex->mod_name_len, ex->fix_port,
                                 ex->fix_ip, ex->fix_ip_len, ex->end_hour,
                                 ex->end_min, ex->end_day,
                                 ex->heart_beat_int, ob,
                                 fix_server_connection_callBack,
                                 parse_fix_message, fix_args,
                                 send_genericfix_logon, build_fix_offsets,
                                 low_activity_sig, 1);
            }
        }
    }
    clean_dart_oo_pool(ob->mgr);
    free_init_args(fix_args);
    destroy_ex_config(ex);
    destroy_rom_queue(rom_q);
    destroy_queue(rom_q);
    return 0;
}

int send_genericfix_logon(struct parse_args *pa)
{
    char *mblk = calloc(600, 1);
    size_t len = 0;
    struct init_args *fixconfig = 0;
    if (pa != 0 && pa->con != 0) {
        fixconfig = pa->con->in_args;
    }
    if (fixconfig != 0) {
        if (mblk != NULL) {
            char *pbuf = mblk;
            len =
                sprintf(mblk, "98=0%c108=30%c", FIX_FIELD_DELIM,
                        FIX_FIELD_DELIM);
            if (len > 0) {
                pbuf += len;
                if (fixconfig->username != 0) {
                    len =
                        sprintf(pbuf, "553=%.*s%c",
                                (int) fixconfig->un_len,
                                fixconfig->username, FIX_FIELD_DELIM);
                    if (len > 0) {
                        pbuf += len;
                    } else {
                        fprintf(stderr,
                                "Error writing username to login message "
                                "buffer (%d) %s\n",
                                errno, strerror(errno));
                    }
                }
                if (fixconfig->password != 0) {
                    len =
                        sprintf(pbuf, "554=%.*s%c", (int) fixconfig->p_len,
                                fixconfig->password, FIX_FIELD_DELIM);
                    if (len > 0) {
                        pbuf += len;
                    } else {
                        fprintf(stderr,
                                "Error writing password to login message "
                                "buffer (%d) %s\n",
                                errno, strerror(errno));
                    }
                }
                send_gen_logon(pa, mblk, pbuf - mblk);
            }
            free(mblk);
            return 0;
        } else {
            fprintf(stderr, "Memory alloation error (%d) %s\n", errno,
                    strerror(errno));
        }
    } else {
        fprintf(stderr,
                "Configuration error, cannot find FIX arguments\n");
    }
    return -1;
}

void destroy_rom_queue(queue_t q)
{
    element_t e, temp;
    e = dequeue(q);
    while (e != NULL) {
        temp = e;
        e = temp->next;
        free_init_args((struct init_args *) temp->d);
        free(temp);
    }
}

void fix_server_connection_callBack(con_obj * co, int is_connected,
                                    char *log, int len)
{
    printf("Info about this FIX Connection: %s, Is it connected {%d} ? \n",
           get_name(co), is_connected);
    if (is_connected) {
    } else {
        fix_connection_callback(co, is_connected);
        set_is_genericfix_connected(is_connected);
    }
}

void test_lac(con_obj * co)
{
    printf("Low activity reported on this connection: %s \n",
           get_name(co));
}

void con_callBack(con_obj * co, int is_connected, char *log, int len)
{
    printf("Info about this Connection: %s, Is it connected {%d} ? \n",
           get_name(co), is_connected);
    is_con_now = is_connected;
}
