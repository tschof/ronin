/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <assert.h>
#include <errno.h>

#include "ex_common.h"
#include "config_loader.h"
#include "dart_constants.h"
#include "mailer.h"
#include "parser_func.h"
#include "databuf.h"
#include "db_layer.h"
#include "fix_handler.h"
#include "debug_logger.h"

#define CLORDID_LOC 15
#define LEG_ID 17

static int check_for_bust(struct message* mess, char* fix_message,
                          databuf_t* output, long fix_len,
                          char rom_type, 
                          struct dart_rom_pos positions[ROM_LEN],
                          orderbook* ob)
{
    char *beg_off = fix_message;
    int len = 0;
    beg_off += positions[75].beg;
    len = positions[75].end - positions[75].beg;
    if (len > 0) {
        int transType = atoi(beg_off);
        if(transType == 1 || transType == 2) {
            if (ob != 0 && ob->module_name != 0 && ob->module_name_len > 0) {
                mess->name = calloc(ob->module_name_len + 1, 1);
                memcpy(mess->name, ob->module_name, ob->module_name_len);
            } else {
                mess->name = calloc(10,1);
                memcpy(mess->name, "EXCH", 4);
            }
            return send_alert_for_message(mess, fix_message,
                                          output, fix_len,
                                          rom_type, positions);
        } else {
            return 0;
        }
    } else { 
        return 0;
    }
}

void from_fix_func(struct message *mess, void *ob)
{
    struct databuf *output = databuf_alloc(mess->len * 5);
    long total_len;
    struct message m1;
    struct message log;
    int rejected = 0;
    orderbook *book = (orderbook *) ob;
    m1.name = NULL;
    m1.name_len = 0;
    total_len = 0;
    total_len =
        trans_fix_message(&m1, mess->data, mess->len, output,
                          &rejected, (orderbook *) ob,
                          &def_fix_to_rom_override);
    if (total_len == 0) {
        if (m1.name) {
            free(m1.name);
        }
        databuf_destroy(output);
        return;
    }

    m1.data = output->buffer;
    m1.len = databuf_unread(output);
    if (book->is_async) {
        exchange_async_rom_sender(&m1, ob);
    } else {
        send_message_to_roms(&m1, (orderbook *) ob);
    }
    log.name = mess->name;
    log.name_len = mess->name_len;
    log.data = mess->data;
    log.len = mess->len;
    log.name = m1.name;
    log.name_len = m1.name_len;
    log.data = m1.data;
    log.len = m1.len;
    dart_rolling_log_going(&log, book->module_name, book->module_name_len);
    databuf_destroy(output);
    if (m1.name) {
        free(m1.name);
    }
}

void database_init(struct database_conf *dbconf, const char *destid,
                   queue_t q, hashmap_t cl_ord_id_map,
                   hashmap_t rom_ord_id_map, orderbook * ob)
{
    int res = 0;
    element_t e, temp;
    e = NULL;
    temp = NULL;
    res = get_db_objects(dbconf, q, destid);
    e = dequeue(q);
    while (e != NULL) {
        struct dart_ex_result_set *ders;
        dart_ex_order_obj *obj;
        databuf_t *buff = databuf_alloc(500);
        char *ord_id = NULL;
        char *rom_id = NULL;
        int rom_id_len = 0;
        size_t rom_inst_len = 0;
        char *rom_inst = NULL;
        int s_len = 0;
        size_t len = 0;
        if (buff == NULL) {
            printf("Calloc failed in init_nyse_tracker.\n");
            return;
        }

        ders = NULL;
        ders = (struct dart_ex_result_set *) e->d;
        if (ders != NULL) {
            obj = get_empty_dart_order_obj(ob->mgr);
            doa_set_order_qty(obj, ders->qty);
            doa_set_cl_ord_id(obj, ders->orderTag, strlen(ders->orderTag));
            rom_id_len = strlen(ders->omTag);
            doa_set_rom_id(obj, ders->omTag, rom_id_len);
            rom_inst =
                build_rom_name(ders->omTag, (size_t) rom_id_len,
                               &rom_inst_len);
            if (rom_inst != NULL) {
                doa_set_sender_name(obj, rom_inst, rom_inst_len);
                free(rom_inst);
            }
            doa_set_exchange_tag(obj, ders->exchangeTag,
                                 strlen(ders->exchangeTag));
            doa_set_leaves_shares(obj, ders->leavesQTY);
            doa_set_cum_shares(obj, ders->cumQty);
            doa_set_delta(obj, 0);
            doa_set_pending_replace(obj, 0);
            databuf_reset(buff);
            s_len = databuf_write(buff, "%f", ders->price);
            doa_set_price(obj, buff->buffer, s_len);
            databuf_reset(buff);
            s_len = databuf_write(buff, "%d", ders->side);
            doa_set_side(obj, buff->buffer, s_len);
            databuf_reset(buff);
            doa_set_symbol(obj, ders->symbol, strlen(ders->symbol));
            s_len = databuf_write(buff, "%d", ders->orderType);
            doa_set_type(obj, buff->buffer, s_len);
            databuf_reset(buff);
            doa_set_capacity(obj, ders->capacity, strlen(ders->capacity));
            s_len = databuf_write(buff, "%d", ders->tif);
            doa_set_tif(obj, buff->buffer, s_len);
            ord_id = doa_get_cl_ord_id(obj, &len);
            no_copy_insert(cl_ord_id_map, ord_id, len, obj);
            len = 0;
            rom_id = doa_get_rom_id(obj, &len);
            doa_set_account(obj, ders->clearingAccount,
                            strlen(ders->clearingAccount));
            doa_set_short_locate(obj, ders->short_lender,
                                 strlen(ders->short_lender));
            doa_set_destination(obj, ders->destination,
                                strlen(ders->destination));

            doa_set_sec_type(obj, ders->secType, 1);
            databuf_reset(buff);
            doa_set_is_spread(obj, ders->cplxtype);
            databuf_reset(buff);
            no_copy_insert(rom_ord_id_map, rom_id, len, obj);
        }
        if (ord_id != NULL) {
            free(ord_id);
        }
        if (rom_id != NULL) {
            free(rom_id);
        }
        temp = e;
        e = temp->next;
        if (ders != NULL) {
            free(ders);
        }
        temp->d = NULL;
        free(temp);
        temp = NULL;
        databuf_destroy(buff);
    }
    destroy_queue(q);
}

size_t build_cnl_rpl_rej(char *beg_off, size_t len, char *l_fix_tag,
                         databuf_t * output, const char *message)
{
    size_t bytes_written = 0;
    size_t ret_bytes = 0;
    char *ret_status =
        (l_fix_tag[0] == 'G'
         || (strncmp(l_fix_tag, "AC", 2) == 0)) ? "20" : "14";
    bytes_written =
        databuf_write(output, "S,,,,,,,,,,,,,,%s,,,", ret_status);
    ret_bytes += bytes_written;
    databuf_memcpy(output, beg_off, len);
    ret_bytes += len;
    bytes_written =
        databuf_write(output, ",,,,,,,,,%s,,,,,,,,,,,,,,,,,,,,\n",
                      message);
    ret_bytes += bytes_written;
    return ret_bytes;
}

static size_t create_spread_head_generic(dart_ex_order_obj * obj, char *status, int len,
                                        const char *fix_mess, int fix_len,
                                        struct dart_rom_pos positions[],
                                        pthread_mutex_t * map_mutex,
                                        hashmap_t cl_ord_id_map, databuf_t * output)
{
    char* id = (char*)fix_mess;
    int ratio = 0;
    id += positions[LEG_ID].beg;
    int id_len = (positions[LEG_ID].end - positions[LEG_ID].beg);
    int is_chosen = doa_get_is_chosen_n_ratio(obj, id, id_len, &ratio);
    if(is_chosen) {
        doa_set_current_status(obj, status, len);
        size_t s_len = 0;
        char *rom_id = doa_get_rom_id(obj, &s_len);
        char *clord = doa_get_cl_ord_id(obj, &s_len);
        databuf_memcpy(output, ",,,,,,,,,,,,,", 13);
        databuf_memcpy(output, status, len);
        databuf_memcpy(output, ",", 1);
        handle_clordids(status, output,
                        fix_mess, positions, map_mutex, cl_ord_id_map, obj);

        databuf_write(output, ",,%s,,,,,,,,,,,,,,,,,,,,,,,,,,,\n", rom_id);
        if(status[0] == '5') {
            id = (char*)fix_mess;
            id += positions[6].beg;
            int q = atoi(id);
            if(q > 0) {
                q = q / ratio;
                doa_set_spread_head_qty(obj,q);
            }
        }
        free(rom_id);
        free(clord);
        return 1;
    } else {
        return 0;
    }
}
static size_t create_spread_head_fill(dart_ex_order_obj * obj, char *status, int len,
                                        const char *fix_mess, int fix_len,
                                        struct dart_rom_pos positions[],
                                        pthread_mutex_t * map_mutex,
                                        hashmap_t cl_ord_id_map, databuf_t * output)
{
    char* id = (char*)fix_mess;
    int ratio = 0;
    id += positions[LEG_ID].beg;
    int id_len = (positions[LEG_ID].end - positions[LEG_ID].beg);
    int is_chosen = doa_get_is_chosen_n_ratio(obj, id, id_len, &ratio);
    if(is_chosen) {
        char* last_off = (char*)fix_mess;
        last_off += positions[42].beg;
        int last_shares = atoi(last_off);
        if(last_shares % ratio  == 0) {
            last_shares /= ratio;
            size_t s_len = 0;
            char *rom_id = doa_get_rom_id(obj, &s_len);
            char *clord = doa_get_cl_ord_id(obj, &s_len);
            int known_cum = doa_get_offset_cum(obj);
            int known_leaves = doa_get_spread_head_qty(obj);
            known_leaves -= known_cum;
            known_cum += last_shares;
            known_leaves -= last_shares;
            doa_set_offset_cum(obj, known_cum);
            databuf_memcpy(output, ",,,,,,,,,,,,,", 13);
            databuf_memcpy(output, status, len);
            databuf_memcpy(output, ",", 1);
            handle_clordids(status, output,
                            fix_mess, positions, map_mutex, cl_ord_id_map, obj);

            databuf_write(output, ",,%s,,,,,,,,,,,,,,,,,,,,,,,,,%d,,,,,,%d,%d,,,,,,,,,,\n",
                          rom_id, last_shares, known_cum, known_leaves);
            free(rom_id);
            free(clord);
            return 1;
        } else if(len > 0 && status[0] == '2') {
            int known_cum = doa_get_offset_cum(obj);
            int totalqty = doa_get_spread_head_qty(obj);
            int known_leaves = totalqty - known_cum;
            doa_set_offset_cum(obj, known_cum + known_leaves);
            last_shares = known_leaves;
            size_t s_len = 0;
            char *rom_id = doa_get_rom_id(obj, &s_len);
            databuf_memcpy(output, ",,,,,,,,,,,,,", 13);
            databuf_memcpy(output, status, len);
            databuf_memcpy(output, ",", 1);
            handle_clordids(status, output,
                            fix_mess, positions, map_mutex, cl_ord_id_map, obj);

            databuf_write(output, ",,%s,,,,,,,,,,,,,,,,,,,,,,,,,%d,,,,,,%d,%d,,,,,,,,,,\n",
                          rom_id, last_shares, known_cum, 0);
            free(rom_id);
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}
static size_t create_spread_head_out(dart_ex_order_obj * obj, char *status, int len,
                                        const char *fix_mess, int fix_len,
                                        struct dart_rom_pos positions[],
                                        pthread_mutex_t * map_mutex,
                                        hashmap_t cl_ord_id_map, databuf_t * output)
{
    int pending_cancel = doa_get_pending_cancel(obj);
    if(pending_cancel == 1) {
        doa_set_current_status(obj, status, len);
        size_t s_len = 0;
        char *rom_id = doa_get_rom_id(obj, &s_len);
        char *clord = doa_get_cl_ord_id(obj, &s_len);
        databuf_memcpy(output, ",,,,,,,,,,,,,", 13);
        databuf_memcpy(output, status, len);
        databuf_memcpy(output, ",", 1);
        handle_clordids(status, output,
                        fix_mess, positions, map_mutex, cl_ord_id_map, obj);

        databuf_write(output, ",,%s,,,,,,,,,,,,,,,,,,,,,,,,,,,\n", rom_id);
        free(rom_id);
        free(clord);
        return 1;
    } else {
        doa_decrement_pending_cancel(obj);
        return 0;
    }
}

int update_spread_head(dart_ex_order_obj * obj, char *status, int len,
                       char rom_type, const char *fix_mess, int fix_len,
                       struct dart_rom_pos positions[],
                       pthread_mutex_t * map_mutex,
                       hashmap_t cl_ord_id_map, databuf_t * output)
{
    int cur_len = 0;
    char *cur_head_stat = doa_get_current_status(obj, &cur_len);
    if ((len == cur_len && strncmp(cur_head_stat, status, len) == 0) ||
        (strncmp(status, "8", 1) == 0
         && (positions[LEG_ID].end - positions[LEG_ID].beg <= 0))) {
        return 0;
    }
    if(rom_type == 'U' || strncmp("0", status, len) == 0) {
        cur_len = create_spread_head_generic(obj, "13", 2, fix_mess, fix_len,
                                  positions, map_mutex, cl_ord_id_map, output);
        if(cur_len == 0) {
            return 0;
        }
    } else if(strncmp(status, "4", 1) == 0) {
        cur_len = create_spread_head_out(obj, "4", 1, fix_mess, fix_len,
                                         positions, map_mutex, cl_ord_id_map, output);
        if(cur_len == 0) {
            return 0;
        }
    } else if(len == 1 && (strncmp(status, "1", 1) == 0 ||
                   strncmp(status, "2", 1) == 0)) {
        cur_len = create_spread_head_fill(obj, status, len, fix_mess, fix_len,
                                         positions, map_mutex, cl_ord_id_map, output);
        if(cur_len == 0) {
            return 0;
        }
    } else if(strncmp("E", status, len) == 0) {
        cur_len = create_spread_head_generic(obj, "21", 2, fix_mess, fix_len,
                                  positions, map_mutex, cl_ord_id_map, output);
        if(cur_len == 0) {
            return 0;
        }
    } else {
        cur_len = create_spread_head_generic(obj, status, len, fix_mess, fix_len,
                                  positions, map_mutex, cl_ord_id_map, output);
        if(cur_len == 0) {
            return 0;
        }
    }
    if (rom_type == 'U') {
        cur_len = databuf_write(output, "S,");
    } else {
        cur_len = databuf_write(output, "%c,", rom_type);
    }
    return cur_len;
}

char *build_rom_name(const char *om_tag, size_t om_tag_len,
                     size_t * r_name_len)
{
    char *ret = calloc(15, 1);
    char *r_off = ret;
    if (om_tag_len > 0 && ret != NULL) {
        char *first_off = NULL;
        char *sec_off = NULL;
        char *d_off = (char *) om_tag;
        first_off = strchr(om_tag, '-');
        if (first_off != NULL && (first_off - om_tag) < om_tag_len) {
            d_off = first_off;
            d_off += 1;
            sec_off = strchr(d_off, '-');
            if (sec_off != NULL && (sec_off - om_tag) < om_tag_len) {
                int len = 0;
                sec_off -= 7;
                len = sec_off - d_off;
                memcpy(r_off, "ROM", 3);
                r_off += 3;
                memcpy(r_off, d_off, len);
                *r_name_len = (3 + len);
            }
        }
    }
    return ret;
}

size_t add_rom_id_to_mess(databuf_t * out_mess, dart_ex_order_obj * obj)
{
    size_t doa_len = 0;
    char *l_rom_id = doa_get_rom_id(obj, &doa_len);
    databuf_memcpy(out_mess, l_rom_id, doa_len);
    free(l_rom_id);
    return doa_len;
}

size_t clean_text(databuf_t * out_mess, const char *fix_mess,
                  size_t fix_len, int idx, struct dart_rom_pos positions[])
{
    size_t len = positions[idx].end - positions[idx].beg;
    const char *d_off = fix_mess + positions[idx].beg;
    char *comma = strchr(d_off, ',');
    if (comma != NULL && (comma - d_off) < len) {
        size_t l_len = comma - d_off;
        size_t t_len = l_len + 1;
        while (comma != NULL && t_len < len) {
            databuf_memcpy(out_mess, d_off, (l_len));
            databuf_memcpy(out_mess, " ", 1);
            d_off = comma + 1;
            comma = strchr(d_off, ',');
            l_len = comma != NULL ? (comma - d_off) : 0;
            t_len += (l_len == 0 ? 0 : l_len + 1);
        }
        if (t_len < len) {
            databuf_memcpy(out_mess, d_off, (len - t_len));
        }
    } else {
        databuf_memcpy(out_mess, d_off, len);
    }
    return len;
}


int check_exe_inst(struct dart_fix_pos positions[],
                   const char *rom_message)
{
    const char *exec_inst = rom_message + positions[57].beg;
    if (exec_inst[0] == 'f') {
        return 0;
    } else {
        return 1;
    }
}

dart_ex_order_obj *establish_order(char *fix_message,
                                   pthread_mutex_t * mut,
                                   hashmap_t cl_ord_id_map,
                                   hashmap_t rom_ord_id_map,
                                   struct dart_fix_pos positions[],
                                   struct message * mess,
                                   char *cl_ord_id, size_t clordid_len,
                                   orderbook * ob)
{
    size_t r_id_len = 0;
    char *r_id = NULL;
    char *beg_off;
    long len = 0;
    size_t cp_len = 0;
    dart_ex_order_obj *obj = NULL;
    beg_off = fix_message;
    beg_off += positions[17].beg;
    len = (positions[17].end - positions[17].beg);
    cp_len = len > (DART_EX_TAG_LEN - 1) ? (DART_EX_TAG_LEN - 1) : len;
    obj = get_empty_dart_order_obj(ob->mgr);
    doa_set_sender_name(obj, mess->name, mess->name_len);
    doa_set_rom_id(obj, beg_off, cp_len);
    r_id = doa_get_rom_id(obj, &r_id_len);
    pthread_mutex_lock(mut);
    doa_set_cl_ord_id(obj, cl_ord_id, clordid_len);
    no_copy_insert(cl_ord_id_map, cl_ord_id, clordid_len, obj);
    no_copy_insert(rom_ord_id_map, r_id, r_id_len, obj);
    pthread_mutex_unlock(mut);
    free(r_id);
    return obj;
}

long set_status_for_cnrej(char *fix_message,
                          struct dart_rom_pos positions[],
                          char *status, databuf_t * output,
                          dart_ex_order_obj* obj)
{
    long len = 0;
    char *cc_off = NULL;
    status = fix_message;
    status += positions[14].beg;
    cc_off = fix_message;
    if (positions[63].beg != positions[63].end) {
        cc_off += positions[63].beg;
        if (cc_off[0] == '1') {
            databuf_memcpy(output, "14", 2);
            len = 2;
        } else {
            doa_set_pending_replace(obj, 0);
            databuf_memcpy(output, "20", 2);
            len = 2;
        }
    }
    return len;
}

size_t handle_clordids(char *status, databuf_t * out_mess,
                       const char *fix_mess,
                       struct dart_rom_pos positions[],
                       pthread_mutex_t * map_mutex,
                       hashmap_t cl_ord_id_map, dart_ex_order_obj * obj)
{
    char *beg_off = 0;
    size_t len = 0;
    size_t ret_len = 0;
    size_t doa_len = 0;
    if (strncmp("4", status, 1) == 0 ||
        strncmp("6", status, 1) == 0 || strncmp("5", status, 1) == 0) {
        beg_off = (char *) fix_mess;
        beg_off += positions[CLORDID_LOC].beg;
        len = positions[CLORDID_LOC].end - positions[CLORDID_LOC].beg;
        databuf_memcpy(out_mess, beg_off, len);
        ret_len = len;
        if (strncmp("5", status, 1) == 0) {
            char *cur_cl_ord_id = doa_get_cl_ord_id(obj, &doa_len);
            beg_off = (char *) fix_mess;
            beg_off += positions[CLORDID_LOC].beg;
            len = positions[CLORDID_LOC].end - positions[CLORDID_LOC].beg;

            if (strncmp(beg_off, cur_cl_ord_id, len) != 0) {
                pthread_mutex_lock(map_mutex);
                remove_from_map(cl_ord_id_map, cur_cl_ord_id, doa_len);
                pthread_mutex_unlock(map_mutex);
                doa_set_cl_ord_id(obj, beg_off, len);
            }
            free(cur_cl_ord_id);
        }
    } else {
        beg_off = (char *) fix_mess;
        beg_off += positions[CLORDID_LOC].beg;
        len = positions[CLORDID_LOC].end - positions[CLORDID_LOC].beg;
        databuf_memcpy(out_mess, beg_off, len);
        ret_len = len;
    }
    return ret_len;
}

int get_order_object(char *fix_message, size_t fix_len,
                     hashmap_t cl_ord_id_map,
                     struct dart_rom_pos positions[],
                     pthread_mutex_t * map_mutex, char rom_type,
                     dart_ex_order_obj ** obj, struct message *mess,
                     databuf_t * output, orderbook * ob)
{
    if(check_for_bust(mess, fix_message, output, fix_len,
                      rom_type, positions, ob) == 0) {
        char *beg_off = NULL;
        size_t len = 0;
        int ret = 0;
        beg_off = fix_message;
        beg_off += positions[CLORDID_LOC].beg;
        len = (positions[CLORDID_LOC].end - positions[CLORDID_LOC].beg);
        pthread_mutex_lock(map_mutex);
        ret = get_map_entry(cl_ord_id_map, beg_off, len, (void **) obj);
        pthread_mutex_unlock(map_mutex);
        if (ret > 0) {
            return 1;
        } else {
            beg_off = fix_message;
            beg_off += positions[60].beg;
            len = (positions[60].end - positions[60].beg);
            pthread_mutex_lock(map_mutex);
            ret = get_map_entry(cl_ord_id_map, beg_off, len, (void **) obj);
            pthread_mutex_unlock(map_mutex);
            if (ret <= 0 && rom_type != 'U') {
                if (ob != 0 && ob->module_name != 0 && ob->module_name_len > 0) {
                    mess->name = calloc(ob->module_name_len + 1, 1);
                    memcpy(mess->name, ob->module_name, ob->module_name_len);
                } else {
                    const char *sendercompid = strstr(fix_message, "49=");
                    if (sendercompid != 0) {
                        const char *sendercompid_endptr;
                        sendercompid += 3;
                        sendercompid_endptr = strchr(sendercompid, '\x01');
                        if (sendercompid_endptr != 0
                            && sendercompid_endptr > sendercompid) {
                            const size_t sendercompid_len =
                                sendercompid_endptr - sendercompid;
                            if (sendercompid_len > 0) {
                                mess->name = calloc(sendercompid_len + 1, 1);
                                memcpy(mess->name, sendercompid,
                                       sendercompid_len);
                            }
                        }
                    }
                }
                int stat_len = positions[14].end - positions[14].beg;
                char *stat = fix_message + positions[14].beg;
                if (stat_len > 0 && (stat[0] == '1' || stat[0] == '2')) {
                    send_alert_for_message(mess, fix_message, output, fix_len,
                                           rom_type, positions);
                }
                return 0;
            } else if (ret <= 0) {
                return 0;
            }
            return 1;
        }
    } else {
        return 0;
    }

}

void check_n_clean(char *status, hashmap_t cl_ord_id_map,
                   hashmap_t rom_id_map, dart_ex_order_obj * obj,
                   pthread_mutex_t * map_mutex, char rom_type,
                   orderbook * ob, char *fix_message, int fix_len,
                   struct dart_rom_pos positions[ROM_LEN])
{
    if (status) {
        int leaves = doa_get_leaves_shares(obj);
        if ((strncmp("4", status, 1) == 0 ||
             (strncmp("8", status, 1) == 0))
            && !doa_get_is_spread(obj) && !doa_get_pending_replace(obj)
            && rom_type != 'U') {
            size_t ord_id_len = 0;
            size_t rom_id_len = 0;
            size_t rep_id_len = 0;
            char *ord_id = doa_get_cl_ord_id(obj, &ord_id_len);
            char *rom_id = doa_get_rom_id(obj, &rom_id_len);
            char *rep_id = doa_get_rep_ord_id(obj, &rep_id_len);
            pthread_mutex_lock(map_mutex);
            remove_from_map(cl_ord_id_map, ord_id, ord_id_len);
            remove_from_map(rom_id_map, rom_id, rom_id_len);
            if (rep_id != 0) {
                remove_from_map(cl_ord_id_map, rep_id, rep_id_len);
            }
            pthread_mutex_unlock(map_mutex);
            destroy_dart_order_obj(ob->mgr, obj);
            free(ord_id);
            free(rom_id);
            free(rep_id);
        } else if (strncmp("2", status, 1) == 0 && leaves == 0 &&
                   !doa_get_pending_replace(obj)
                   && !doa_get_is_spread(obj) && rom_type != 'U') {
            size_t ord_id_len = 0;
            size_t rom_id_len = 0;
            char *ord_id = doa_get_cl_ord_id(obj, &ord_id_len);
            char *rom_id = doa_get_rom_id(obj, &rom_id_len);
            pthread_mutex_lock(map_mutex);
            remove_from_map(cl_ord_id_map, ord_id, ord_id_len);
            remove_from_map(rom_id_map, rom_id, rom_id_len);
            pthread_mutex_unlock(map_mutex);
            destroy_dart_order_obj(ob->mgr, obj);
            free(ord_id);
            free(rom_id);
        } else if (rom_type == 'U') {
            char *rep_id = fix_message;
            rep_id += positions[CLORDID_LOC].beg;
            int rep_id_len =
                (positions[CLORDID_LOC].end - positions[CLORDID_LOC].beg);
            //size_t rep_id_len = 0;
            //char *rep_id = doa_get_rep_ord_id(obj, &rep_id_len);
            pthread_mutex_lock(map_mutex);
            if (rep_id != 0 && rep_id_len > 0) {
                remove_from_map(cl_ord_id_map, rep_id, rep_id_len);
            }
            pthread_mutex_unlock(map_mutex);
        }
    }
}

void set_val_in_order_struct( /*@unique@ */ char *dart_val, size_t len,
                             int offset,
                             dart_ex_order_obj * dart_obj, char *type)
{
    if (type[0] == 'D' || strncmp(type, "AB", 2) == 0) {
        switch (offset) {
        default:
            break;
        case 4:
            doa_set_side(dart_obj, dart_val, len);
            break;
        case 5:
            doa_set_symbol(dart_obj, dart_val, len);
            break;
        case 6:
            set_shares_fields(dart_val, len, dart_obj);
            break;
        case 7:
            doa_set_price(dart_obj, dart_val, len);
            break;
        case 8:
            doa_set_type(dart_obj, dart_val, len);
            break;
        case 9:
            doa_set_tif(dart_obj, dart_val, len);
            break;
        case 10:
            doa_set_capacity(dart_obj, dart_val, len);
            break;
        case 11:
            doa_set_clr_id(dart_obj, dart_val, len);
            break;
        case 12:
            doa_set_account(dart_obj, dart_val, len);
            break;
        case 24:
            doa_set_trade_for(dart_obj, dart_val, len);
            break;
        case 19:
            doa_set_stop_price(dart_obj, dart_val, len);
            break;
        case 30:
            doa_set_expireMonYear(dart_obj, dart_val, len);
            break;
        case 46:
            doa_set_cmta(dart_obj, dart_val, len);
            break;
        case 36:
            doa_set_mm_account(dart_obj, dart_val, len);
            break;
        case 37:
            doa_set_mm_home(dart_obj, dart_val, len);
            break;
        case 38:
            doa_set_open_close(dart_obj, dart_val, len);
            break;
        case 31:
            doa_set_call_put(dart_obj, dart_val, len);
            break;
        case 32:
            doa_set_strike(dart_obj, dart_val, len);
            break;
        case 35:
            doa_set_sec_type(dart_obj, dart_val, len);
            break;
        case 55:
            doa_set_sec_def(dart_obj, dart_val, len);
        case 62:
            doa_set_expire_day(dart_obj, dart_val, len);
            break;
        };
    } else if (type[0] == 'G' || strncmp(type, "AC", 2) == 0) {
        switch (offset) {
        default:
            break;
        case 43:
            set_replace_delta(dart_val, len, dart_obj);
            break;
        case 7:
            doa_set_price(dart_obj, dart_val, len);
            break;
        case 9:
            doa_set_tif(dart_obj, dart_val, len);
            break;
        case 19:
            doa_set_stop_price(dart_obj, dart_val, len);
            break;
        };
    } else if (type[0] == 'S') {
        switch (offset) {
        default:
            break;
        case 16:
            doa_set_exchange_tag(dart_obj, dart_val, len);
            break;
        case 42:
            update_shares_for_fill(dart_val, len, dart_obj);
            break;
        case 6:
            if (doa_get_pending_replace(dart_obj)) {
                update_shares_for_replace(dart_val, len, dart_obj);
            }
        };
    }
}

size_t translate_rom_symbol(databuf_t * d_off, char *symbol,
                            size_t sym_len)
{
    char *slash_off = 0;
    size_t s_len;
    size_t ret_len = 0;
    slash_off = memchr(symbol, '/', sym_len);
    if (slash_off == 0) {
        s_len = databuf_write(d_off, "%c55=", FIX_FIELD_DELIM);
        ret_len += s_len;
        databuf_memcpy(d_off, symbol, sym_len);
        ret_len += sym_len;
    } else {
        int beg_len = slash_off - symbol;
        char *e_off = slash_off + 1;
        size_t ex_len = (sym_len - ((slash_off + 1) - symbol));
        s_len = databuf_write(d_off, "%c55=", FIX_FIELD_DELIM);
        ret_len += s_len;
        databuf_memcpy(d_off, symbol, beg_len);
        ret_len += beg_len;
        s_len = databuf_write(d_off, "%c65=", FIX_FIELD_DELIM);
        ret_len += s_len;
        databuf_memcpy(d_off, e_off, ex_len);
        ret_len += ex_len;
    }
    return ret_len;
}

int set_rom_val_in_fix(databuf_t * d_off, char *b_off,
                       struct dart_fix_pos positions[ROM_LEN], int offset,
                       char *type, dart_ex_order_obj * obj,
                       void (*set_val_in_order_struct) (char *b,
                                                        size_t len,
                                                        int off,
                                                        dart_ex_order_obj *
                                                        obj, char *type))
{
    char *beg_off;
    long len;
    int s_len;
    int ret_len = 0;
    beg_off = b_off;
    beg_off += positions[offset].beg;
    len = (positions[offset].end - positions[offset].beg);
    s_len =
        databuf_write(d_off, "%c%s=", FIX_FIELD_DELIM,
                      positions[offset].name);
    ret_len += s_len;
    databuf_memcpy(d_off, beg_off, len);
    ret_len += len;
    set_val_in_order_struct(beg_off, len, offset, obj, type);
    return ret_len;
}

int set_execution_time(databuf_t * d_off)
{
    time_t rawtime;
    struct tm *ti;
    int year;
    int month;
    int s_len;
    int ret_len = 0;
    time(&rawtime);
    struct tm lm;
    ti = gmtime_r(&rawtime, &lm);
    ti = &lm;
    year = 1900 + ti->tm_year;
    month = 1 + ti->tm_mon;
    s_len =
        databuf_write(d_off, "%c60=%04d%02d%02d-%02d:%02d:%02d",
                      FIX_FIELD_DELIM, year, month, ti->tm_mday,
                      ti->tm_hour, ti->tm_min, ti->tm_sec);
    ret_len += s_len;
    return ret_len;
}
long send_alert_for_message(struct message *mess, char *fix_message,
                            databuf_t * output, long fix_len,
                            char rom_type,
                            struct dart_rom_pos positions[ROM_LEN])
{
    char *beg_off = fix_message;
    int len = 0;
    long ret_len = 0;
    databuf_t *subject = databuf_alloc(256);
    databuf_t *msgbody = databuf_alloc(256);

    beg_off += positions[75].beg;
    len = positions[75].end - positions[75].beg;
    if (len > 0) {
        int transType = atoi(beg_off);
        if (transType == 1) {
            databuf_write(subject, "Busted Trade from: %s ", mess->name);
        } else if (transType == 2) {
            databuf_write(subject, "Correction sent from: %s ",
                          mess->name);
        } else {
            beg_off = fix_message;
            beg_off += positions[14].beg;
            if (strncmp(beg_off, "2", 1) == 0
                || strncmp(beg_off, "1", 1) == 0) {
                databuf_write(subject, "Unmatched message from: %s ",
                              mess->name);
            } else {
                return 0;
            }
        }
    } else {
        databuf_write(subject, "Unmatched message from: %s", mess->name);
    }

    len = positions[CLORDID_LOC].end - positions[CLORDID_LOC].beg;
    databuf_write(msgbody, "Message Values:\nClOrdID = %.*s\n", len,
                  fix_message + positions[CLORDID_LOC].beg);
    if ((len = positions[60].end - positions[60].beg) > 0)
        databuf_write(msgbody, "OrigOrdID = %.*s\n", len,
                      fix_message + positions[60].beg);
    if ((len = positions[7].end - positions[7].beg) > 0)
        databuf_write(msgbody, "Price = %.*s\n", len,
                      fix_message + positions[7].beg);
    if ((len = positions[44].end - positions[44].beg) > 0)
        databuf_write(msgbody, "Last Price = %.*s\n", len,
                      fix_message + positions[44].beg);
    if ((len = positions[42].end - positions[42].beg) > 0)
        databuf_write(msgbody, "Last Shares = %.*s\n", len,
                      fix_message + positions[42].beg);
    if ((len = positions[6].end - positions[6].beg) > 0)
        databuf_write(msgbody, "Shares = %.*s\n", len,
                      fix_message + positions[6].beg);
    databuf_write(msgbody, "Raw Message:\n%.*s\n", (int) fix_len,
                  fix_message);

    ret_len = msgbody->wr_ptr - msgbody->rd_ptr;
    send_tech_message(msgbody->rd_ptr, ret_len, subject->rd_ptr,
                      subject->wr_ptr - subject->rd_ptr);

    databuf_destroy(subject);
    databuf_destroy(msgbody);

    return ret_len;
}

void update_shares_for_fill(char *dart_val, size_t len,
                            dart_ex_order_obj * dart_obj)
{
    int last_shares = atoi(dart_val);
    if (last_shares > 0 && len > 0) {
        int known_cum = doa_get_cum_shares(dart_obj);
        int known_leaves = doa_get_leaves_shares(dart_obj);
        known_cum += last_shares;
        known_leaves -= last_shares;
        doa_set_cum_shares(dart_obj, known_cum);
        doa_add_to_temp_cum(dart_obj, last_shares);
        doa_set_leaves_shares(dart_obj, known_leaves);
    }
}

void update_shares_for_replace(char *dart_val, size_t len,
                               dart_ex_order_obj * dart_obj)
{
    if (len > 0) {
        int l_qty = atoi(dart_val);
        int delta = doa_get_delta(dart_obj);
        int known_leaves = doa_get_leaves_shares(dart_obj);
        doa_set_leaves_shares(dart_obj, (known_leaves + delta));
        doa_set_order_qty(dart_obj, l_qty);
        doa_set_pending_replace(dart_obj, 0);
    }
}

void set_shares_fields(char *dart_val, size_t len,
                       dart_ex_order_obj * dart_obj)
{
    int l_qty = atoi(dart_val);
    doa_set_order_qty(dart_obj, l_qty);
    doa_set_leaves_shares(dart_obj, l_qty);
    doa_set_cum_shares(dart_obj, 0);
    doa_set_offset_cum(dart_obj, 0);
    doa_set_temp_cum(dart_obj, 0);
    doa_set_delta(dart_obj, 0);
    doa_set_pending_replace(dart_obj, 0);
}

void set_replace_delta(char *dart_val, size_t len,
                       dart_ex_order_obj * dart_obj)
{
    int rep_shares = atoi(dart_val);
    int leaves = doa_get_leaves_shares(dart_obj);
    int cum = doa_get_cum_shares(dart_obj);
    int delta = rep_shares - (leaves + cum);
    doa_set_delta(dart_obj, delta);
    doa_set_pending_replace(dart_obj, 2);
}

void exchange_async_write_callback(const char *name, int name_len,
                                   void *cb)
{
    orderbook *ob = (orderbook *) cb;
    if (pthread_mutex_lock(&ob->send_mutex) == 0) {
        databuf_t *mess_to_send = NULL;
        int ret = get_map_entry(ob->send_buffs, name, name_len,
                                (void *) &mess_to_send);
        if (ret == 0) {
            pthread_mutex_unlock(&ob->send_mutex);
            send_debug_message("Could not find buff for this: %s \n",
                               name);
            return;
        }
        int unread = databuf_unread(mess_to_send);
        int total_sent = 0;
        int keep_trying = 1;
        while (total_sent < unread && keep_trying) {
            int len_sent =
                linux_only_dart_send(name, name_len, mess_to_send->rd_ptr,
                                     databuf_unread(mess_to_send));
            if (len_sent == 0) {
                queue_async_send(ob->async_serv_con, name, name_len, ob);
                keep_trying = 0;
            } else if (len_sent < 0) {
                keep_trying = 0;
                //We got an error and the socket is closing.
                databuf_reset(mess_to_send);
            } else {
                total_sent += len_sent;
                mess_to_send->rd_ptr += len_sent;
            }
        }
        if (total_sent >= unread) {
            databuf_reset(mess_to_send);
        }
        if (pthread_mutex_unlock(&ob->send_mutex) != 0) {
            send_debug_message
                ("failed to unlock client map guard in async callback.  For this: %s \n",
                 name);
        }
    }
}

static void copy_iovecs_to_buff(struct iovec *iov, int iovcnt,
                                databuf_t * buff)
{
    while (iovcnt > 0) {
        databuf_memcpy(buff, iov->iov_base, iov->iov_len);
        ++iov;
        --iovcnt;
    }
}

void exchange_async_rom_sender(struct message *mess, orderbook * ob)
{
    struct iovec iov;
    iov.iov_base = mess->data;
    iov.iov_len = mess->len;
    int sock = get_sock_for_con(mess->name, mess->name_len);
    if (sock >= 0) {
        async_iovec_sender(&iov, 1, mess->name, mess->name_len, ob, sock);
    }
    record_sent_message(mess);
}

void async_iovec_sender(struct iovec *iov, int iovcnt, char *name,
                        int name_len, orderbook * ob, int sock)
{
    if (pthread_mutex_lock(&ob->send_mutex) == 0) {
        databuf_t *mess_to_send = NULL;
        int ret = get_map_entry(ob->send_buffs, name, name_len,
                                (void *) &mess_to_send);
        int err = 0;
        int vec_sent = 0;
        if (ret == 0 || databuf_unread(mess_to_send) <= 0) {
            send_bunches(iov, iovcnt, sock, &err, &vec_sent);
            if (err == 0) {
                if (ret > 0) {
                    copy_iovecs_to_buff(iov, (iovcnt - vec_sent),
                                        mess_to_send);
                } else {
                    mess_to_send = databuf_alloc(3000);
                    copy_iovecs_to_buff(iov, (iovcnt - vec_sent),
                                        mess_to_send);
                    no_copy_insert(ob->send_buffs, name, name_len,
                                   mess_to_send);
                    send_debug_message
                        ("Just inserted map for this: %s, orig:  %lu , num to sent: %d \n",
                         ((char *) iov->iov_base), iovcnt,
                         (iovcnt - vec_sent));
                }
                queue_async_send(ob->async_serv_con, name, name_len, ob);
            }
            if (pthread_mutex_unlock(&ob->send_mutex) != 0) {
                send_debug_message
                    ("failed to unlock client map guard in ron sender.  For this: %s \n",
                     name);
            }
        } else {
            copy_iovecs_to_buff(iov, (iovcnt - vec_sent), mess_to_send);
            if (pthread_mutex_unlock(&ob->send_mutex) != 0) {
                send_debug_message
                    ("failed to unlock client map guard in rom sender.  For this: %s \n",
                     name);
            }
            exchange_async_write_callback(name, name_len, ob);
        }
    }
}

void send_message_to_roms(struct message *mess, orderbook * ob)
{
    long len_sent = 0;
    long ret_val = 0;
    int failed_attempts = 0;
    long t_len = mess->len;
    char *orig_data = mess->data;
    char *mess_off = mess->data;
    if (pthread_mutex_lock(&ob->send_mutex) == 0) {
        while (len_sent < t_len && len_sent >= 0 && failed_attempts < 10) {
            ret_val = send_message(mess);
            if (ret_val < 0) {
                ++failed_attempts;
                ret_val = 0;
            }
            mess_off += ret_val;
            mess->len -= ret_val;
            mess->data = mess_off;
            len_sent += ret_val;
        }
        if (pthread_mutex_unlock(&ob->send_mutex) != 0) {
        }
    }
    mess->len = t_len;
    mess->data = orig_data;
    record_sent_message(mess);
}

size_t create_rom_reject(char *data, databuf_t * output,
                         const char *message)
{
    int i = 0;
    size_t cur_off = 0;
    char *ret_val = NULL;
    size_t ret_code = 0;
    size_t end_pos;
    char *mess_off = data;
    for (i = 0; i < 17; ++i) {
        ret_val = strchr(mess_off, ',');
        if (ret_val) {
            ret_code = (ret_val - mess_off);
            mess_off += (ret_code + 1);
            cur_off += 1 + ret_code;
        } else {
            printf("Failed to properly parse this rom message: %s", data);
            return 0;
        }
    }
    ret_val = strchr(mess_off, ',');
    if (ret_val) {
        size_t bytes_written = 0;
        size_t ret_bytes = 0;

        char *ret_status = NULL;
        if (data[0] == 'R') {
            ret_status = "20";
        } else if (data[0] == 'C') {
            ret_status = "14";
        } else {
            ret_status = "8";
        }
        end_pos = (ret_val - mess_off);
        bytes_written = databuf_write(output,
                                      "S,,,,,,,,,,,,,,%s,,,", ret_status);
        ret_bytes += bytes_written;
        databuf_memcpy(output, mess_off, end_pos);
        ret_bytes += end_pos;
        bytes_written =
            databuf_write(output, ",,,,,,,,,%s,,,,,,,,,,,,,,,,,,,,\n",
                          message);
        ret_bytes += bytes_written;
        return ret_bytes;
    } else {
        printf("Failed to find comma after rom tag: %s", data);
        return 0;
    }
}

static struct seq_reset_time *clone_rt(struct seq_reset_time *rt)
{
    struct seq_reset_time *ret = calloc(1,sizeof(struct seq_reset_time));
    ret->reset_hour = rt->reset_hour;
    ret->reset_min = rt->reset_min;
    ret->reset_day = rt->reset_day;
    ret->last_logon = rt->last_logon;
    ret->new_incoming = rt->new_incoming;
    ret->new_outgoing = rt->new_outgoing;
    ret->create_new = rt->create_new;
    return ret;
}

void load_rom_config(dart_config * dc, queue_t rom_queue)
{
    struct seq_reset_time *rt;
    int ret_len;
    char *reset_h;
    char *reset_m;
    char *last_run_date;
    char *new_incoming;
    char *new_outgoing;
    time_t rawtime;
    struct tm *ti;
    char *now;
    int year;
    int month;
    int len;
    int i;
    ret_len = 0;
    rt = calloc(1,sizeof(struct seq_reset_time));
    reset_h = get_val_for_tag(dc, "TIMES", 5, "Reset Hour", 10, &ret_len);
    if (reset_h != NULL) {
        rt->reset_hour = (size_t) atoi(reset_h);
        free(reset_h);
    } else {
        rt->reset_hour = 0;
    }
    reset_m = get_val_for_tag(dc, "TIMES", 5, "Reset Min", 9, &ret_len);
    if (reset_m != NULL) {
        rt->reset_min = (size_t) atoi(reset_m);
        free(reset_m);
    } else {
        rt->reset_min = 0;
    }
    new_incoming =
        get_val_for_tag(dc, "ROM SERVER", 10, "Reset Incoming", 14,
                        &ret_len);
    if (new_incoming != NULL) {
        rt->new_incoming = atoi(new_incoming);
        free(new_incoming);
    } else {
        rt->new_incoming = 0;
    }
    new_outgoing =
        get_val_for_tag(dc, "ROM SERVER", 10, "Reset Outgoing", 14,
                        &ret_len);
    if (new_outgoing != NULL) {
        rt->new_outgoing = atoi(new_outgoing);
        free(new_outgoing);
    } else {
        rt->new_outgoing = 0;
    }

    last_run_date =
        get_val_for_tag(dc, "TIMES", 5, "Last Run Day", 12, &ret_len);
    time(&rawtime);
    struct tm lm;
    ti = localtime_r(&rawtime, &lm);
    ti = &lm;
    now = calloc(9, 1);
    year = 1900 + ti->tm_year;
    month = 1 + ti->tm_mon;
    len = sprintf(now, "%04d%02d%02d", year, month, ti->tm_mday);
    if (last_run_date && strncmp(now, last_run_date, 8) == 0 && len <= 8) {
        rt->create_new = 0;
    } else {
        rt->create_new = 1;
    }
    rt->reset_day = 7;
    rt->last_logon = 0;
    for (i = 0; i < 90; ++i) {
        struct init_args *in_args;
        in_args = calloc(1,sizeof(struct init_args));
        in_args->name = calloc(6, 1);
        ret_len = sprintf(in_args->name, "ROM%d", i);
        in_args->name_len = ret_len;
        in_args->sender_comp = NULL;
        in_args->sc_len = 0;
        in_args->target_comp = NULL;
        in_args->tc_len = 0;
        in_args->target_sub = NULL;
        in_args->ts_len = 0;
        in_args->sender_sub = NULL;
        in_args->ss_len = 0;
        in_args->fix_version = NULL;
        in_args->fv_len = 0;
        in_args->log_outgoing = 1;
        in_args->username = calloc(6, 1);
        ret_len = sprintf(in_args->username, "ROM%d", i);
        in_args->un_len = ret_len;
        in_args->password = calloc(6, 1);
        ret_len = sprintf(in_args->password, "ROM%d", i);
        in_args->p_len = ret_len;
        in_args->reset_time = clone_rt(rt);
        in_args->cancel_only = 0;
        in_args->dc = dc;
        enqueue(rom_queue, in_args);
    }
    free(now);
    free(last_run_date);
}

void free_init_args(struct init_args *init)
{
    if (init) {
        if (init->name) {
            free(init->name);
        }
        if (init->password) {
            free(init->password);
        }
        if (init->username) {
            free(init->username);
        }
        if (init->sender_sub) {
            free(init->sender_sub);
        }
        if (init->target_sub) {
            free(init->target_sub);
        }
        if (init->target_comp) {
            free(init->target_comp);
        }
        if (init->sender_comp) {
            free(init->sender_comp);
        }
        if (init->fix_version) {
            free(init->fix_version);
        }

        free(init);
        init = NULL;
    }
}

struct init_args *load_fix_config(const char *filename, int file_len)
{
    struct init_args *in_args;
    int ret_len;
    struct seq_reset_time *rt;
    char *reset_h;
    char *reset_m;
    char *reset_day;
    int day_of_reset = 7;
    char *last_run_date;
    time_t rawtime;
    char *new_incoming;
    char *new_outgoing;
    struct tm *ti;
    char *now;
    int year;
    int month;
    int len;
    in_args = calloc(1,sizeof(struct init_args));
    ret_len = 0;
    in_args->dc = initialize_config(filename, file_len);
    if (in_args->dc == NULL) {
        fprintf(stderr, "No config file found: %s\n", filename);
        return 0;
    }
    in_args->name =
        get_val_for_tag(in_args->dc, "FIX SERVER", 10, "Target Comp ID",
                        14, &ret_len);
    in_args->name_len = ret_len;
    in_args->sender_comp =
        get_val_for_tag(in_args->dc, "FIX SERVER", 10, "Sender Comp ID",
                        14, &ret_len);
    in_args->sc_len = ret_len;
    in_args->target_comp =
        get_val_for_tag(in_args->dc, "FIX SERVER", 10, "Target Comp ID",
                        14, &ret_len);
    in_args->tc_len = ret_len;
    in_args->target_sub =
        get_val_for_tag(in_args->dc, "FIX SERVER", 10, "Target Sub ID", 13,
                        &ret_len);
    in_args->ts_len = ret_len;
    in_args->sender_sub =
        get_val_for_tag(in_args->dc, "FIX SERVER", 10, "Sender Sub ID", 13,
                        &ret_len);
    in_args->ss_len = ret_len;
    in_args->username =
        get_val_for_tag(in_args->dc, "FIX SERVER", 10, "Username", 8,
                        &ret_len);
    in_args->un_len = ret_len;
    in_args->password =
        get_val_for_tag(in_args->dc, "FIX SERVER", 10, "Password", 8,
                        &ret_len);
    in_args->p_len = ret_len;

    in_args->fix_version =
        get_val_for_tag(in_args->dc, "FIX SERVER", 10, "Fix Version", 11,
                        &ret_len);
    if (in_args->fix_version == NULL) {
        in_args->fix_version = calloc(8, 1);
        memcpy(in_args->fix_version, "FIX.4.2", 7);
        in_args->fv_len = 7;
    } else {
        in_args->fv_len = ret_len;
    }

    rt = calloc(1, sizeof(struct seq_reset_time));

    reset_h =
        get_val_for_tag(in_args->dc, "TIMES", 5, "Reset Hour", 10,
                        &ret_len);
    if (reset_h != NULL) {
        rt->reset_hour = atoi(reset_h);
        free(reset_h);
    } else {
        rt->reset_hour = 0;
    }
    reset_m =
        get_val_for_tag(in_args->dc, "TIMES", 5, "Reset Min", 9, &ret_len);
    if (reset_m != NULL) {
        rt->reset_min = atoi(reset_m);
        free(reset_m);
    } else {
        rt->reset_min = 0;
    }
    reset_day =
        get_val_for_tag(in_args->dc, "TIMES", 5, "Reset Day", 9, &ret_len);
    if (ret_len > 0 && reset_day != NULL) {
        day_of_reset = atoi(reset_day);
    }
    new_incoming =
        get_val_for_tag(in_args->dc, "FIX SERVER", 10, "Reset Incoming",
                        14, &ret_len);
    if (new_incoming != NULL) {
        rt->new_incoming = atoi(new_incoming);
        free(new_incoming);
    } else {
        rt->new_incoming = 0;
    }
    new_outgoing =
        get_val_for_tag(in_args->dc, "FIX SERVER", 10, "Reset Outgoing",
                        14, &ret_len);
    if (new_outgoing != NULL) {
        rt->new_outgoing = atoi(new_outgoing);
        free(new_outgoing);
    } else {
        rt->new_outgoing = 0;
    }

    last_run_date =
        get_val_for_tag(in_args->dc, "TIMES", 5, "Last Run Day", 12,
                        &ret_len);
    time(&rawtime);
    struct tm lm;
    ti = localtime_r(&rawtime, &lm);
    ti = &lm;
    now = calloc(15, 1);
    year = 1900 + ti->tm_year;
    month = 1 + ti->tm_mon;
    len = sprintf(now, "%04d%02d%02d%02d%02d%02d", year, month, ti->tm_mday,
            ti->tm_hour, ti->tm_min, ti->tm_sec);
    rt->create_new = 0;
    if(ret_len  == 14) {
        rt->last_logon = convert_to_sec_since_1970(last_run_date);
    } else {
        rt->last_logon = 0;
        rt->create_new = 1;
    }
    rt->reset_day = day_of_reset;
    in_args->log_outgoing = 1;
    in_args->reset_time = rt;
    in_args->create_mess_header = create_fix_header;
    in_args->cancel_only = 0;
    set_val_for_tag(in_args->dc, "TIMES", 5, "Last Run Day", 12, now, 14);
    free(now);
    free(last_run_date);
    return in_args;
}

int def_fixrom_status_xlate(char *fixmsg, dart_rom_pos * positions,
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
        len = set_status_for_cnrej(fixmsg, positions, status, rom, order);
    }
    return 1;
}

int def_add_rom_tag(char *fixmsg, dart_rom_pos * positions,
                    char mtype, char *status,
                    databuf_t * rom, size_t romfield,
                    int *rejtected, dart_ex_order_obj * order,
                    pthread_mutex_t * mutex, hashmap_t ord_map)
{
    add_rom_id_to_mess(rom, order);
    return 1;
}


int def_map_fix_text_to_rom(char *fixmsg, dart_rom_pos * positions,
                            char mtype, char *status,
                            databuf_t * rom, size_t romfield,
                            int *rejtected, dart_ex_order_obj * order,
                            pthread_mutex_t * mutex, hashmap_t ord_map)
{
    int err = 0;
    long fix_field_len = positions[romfield].end - positions[romfield].beg;
    if (fix_field_len > 0) {
        err =
            clean_text(rom, fixmsg, (size_t) fix_field_len, romfield,
                       positions);
    }
    return err;
}

int def_xlate_fixrom(char *fixmsg, dart_rom_pos * positions,
                     char mtype, char *status,
                     databuf_t * rom, size_t romfield,
                     int *rejtected, dart_ex_order_obj * order,
                     pthread_mutex_t * mutex, hashmap_t ord_map)
{
    char *beg_off = fixmsg;
    size_t len = (positions[romfield].end - positions[romfield].beg);
    beg_off += positions[romfield].beg;
    set_val_in_order_struct(beg_off, len, romfield, order, &mtype);
    if (!determine_trans_type(romfield, beg_off, rom, len)) {
        databuf_memcpy(rom, beg_off, len);
    }
    return 1;
}

int def_handle_ordids(char *fixmsg, dart_rom_pos * positions,
                      char mtype, char *status,
                      databuf_t * rom, size_t romfield,
                      int *rejtected, dart_ex_order_obj * order,
                      pthread_mutex_t * mutex, hashmap_t ord_map)
{
    return handle_clordids(status, rom, fixmsg,
                           positions, mutex, ord_map, order);
}

int def_no_op(char *fixmsg, dart_rom_pos * positions,
              char mtype, char *status,
              databuf_t * rom, size_t romfield,
              int *rejtected, dart_ex_order_obj * order,
              pthread_mutex_t * mutex, hashmap_t ord_map)
{
    return 1;
}

long def_fix_to_rom_override(struct message *mess, char *fix_message,
                             databuf_t * output, long fix_len,
                             char rom_type,
                             struct dart_rom_pos positions[ROM_LEN],
                             int *rejected, orderbook * ob)
{
    int i = 1;
    int s_len = 0;
    char *status = NULL;
    dart_ex_order_obj *obj = NULL;
    size_t doa_len = 0;
    if (rom_type == 'U') {
        s_len = databuf_write(output, "S,");
    } else {
        s_len = databuf_write(output, "%c,", rom_type);
    }
    if (!get_order_object
        (fix_message, fix_len, ob->cl_ord_id_map, positions, &ob->mutex,
         rom_type, &obj, mess, output, ob)) {
        return 0;
    }
    mess->name = doa_get_sender_name(obj, &doa_len);
    mess->name_len = doa_len;
    status = fix_message;
    status += positions[14].beg;
    printf("This is the status: %c \n", status[0]);
    if (doa_get_is_spread(obj) == 1 && !ob->no_fix_43 &&
            !ob->no_spread_head) {
        update_spread_head(obj, status,
                           positions[14].end - positions[14].beg, rom_type,
                           fix_message, fix_len, positions, &ob->mutex,
                           ob->cl_ord_id_map, output);
    } else if(doa_get_is_spread(obj) == 1 &&
            ob->no_spread_head && (status[0] == '2'|| status[0] == '1' || status[0] == '4')) {
        update_spread_head(obj, status,
                           positions[14].end - positions[14].beg, rom_type,
                           fix_message, fix_len, positions, &ob->mutex,
                           ob->cl_ord_id_map, output);
    }
    for (i = 1; i < ROM_LEN; ++i) {
        if (ob->fix2rom[i] != 0) {
            if (ob->fix2rom[i] (fix_message, positions, rom_type,
                                status, output, i, rejected,
                                obj, &ob->mutex,
                                ob->cl_ord_id_map) == -1) {
                fprintf(stderr, "%s: Processing %d failed\n",
                        __FUNCTION__, i);
            }
            if (*rejected != 0) {
                /* We do not support sending a DK (Don't Know) and
                 * most FIX sessions would not handle it anyway.  The
                 * best we can do is drop the resulting message. */
                /* fprintf(stderr, */
                /*         "%s:%d Message processing failed while on field %d, " */
                /*         "not sending to ROM\n", */
                /*         __FILE__, __LINE__, i); */
                return 0;
            }
        }
        databuf_memcpy(output, ",", 1);
    }
    databuf_memcpy(output, "\n", 1);
    check_n_clean(status, ob->cl_ord_id_map, ob->rom_ord_id_map, obj,
                  &ob->mutex, rom_type, ob, fix_message, fix_len,
                  positions);
    return databuf_unread(output);
}
static void gen_add_to_map(hashmap_t map, char *buffer, long len)
{
    char *off;
    long c_offset = 0;
    off = strchr(buffer, ',');
    if (off) {
        char *tag;
        char* val;
        char *val_off;
        c_offset = off - buffer;
        val = calloc(10,1);
        tag = calloc(c_offset + 1, 1);
        memcpy(tag, buffer, c_offset);

        val_off = buffer;
        val_off += c_offset + 1;
        int val_len = len - (c_offset + 1);
        memcpy(val, val_off, val_len); 
        insert_map(map, tag, c_offset, val, val_len);
        free(val);
        free(tag);
    } else {
        printf("Could not find a comma in this mess: %s \n", buffer);
    }
}

int gen_build_clear_id_map(const char *filename, hashmap_t map)
{
    char *buffer;
    long size;
    size = 0;
    buffer = NULL;
    buffer = get_file_contents(filename, &size);
    if (size > 0) {
        long cur_pos = 0;
        char eol[2] = { '\n', '\0' };
        long end_pos = 0;
        int should_parse = 1;
        char *d_off = buffer;
        while (cur_pos < size && should_parse) {
            end_pos = find_offset(d_off, (size - cur_pos), eol, 1);
            if (end_pos > 0) {
                gen_add_to_map(map, d_off, end_pos);
                d_off += end_pos + 1;
                cur_pos += end_pos + 1;
            } else {
                should_parse = 0;
            }
        }
        free(buffer);
    } else {
        return 1;
    }
    return 0;
}
