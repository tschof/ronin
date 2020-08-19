/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "config_loader.h"
#include "ex_common.h"
#include "cx_ouch_helper.h"
#include "dart_orderbook.h"
#include "cx_object.h"

unsigned long build_cx_offsets(char *data,
                               unsigned long size,
                               unsigned long byte_offset,
                               unsigned long *seq_num)
{
    return size;
}

static void cx_build_rom_pos(char *rom_message, long rom_len,
                             struct dart_fix_pos *l_positions, long offset)
{
    long cur_off;
    long len_left;
    long val_end;
    char *vend_ptr = 0;
    char *d_off;
    int should_parse = 1;
    int rom_tag = 0;
    d_off = rom_message;
    cur_off = offset;
    val_end = 0;
    len_left = rom_len;
    while (should_parse && rom_tag < ROM_LEN && len_left > 0) {
        vend_ptr = strchr(d_off, ',');
        if (vend_ptr != NULL) {
            val_end = (vend_ptr - d_off);
            l_positions[rom_tag].beg = cur_off;
            l_positions[rom_tag].end = (cur_off + val_end);
            l_positions[rom_tag].name = 0;
            l_positions[rom_tag].len = 0;
            d_off += val_end + 1;
            ++rom_tag;
            cur_off += val_end + 1;
        } else if (vend_ptr == NULL) {
            should_parse = 0;
        } else {
            val_end = (vend_ptr - d_off);
            d_off += val_end + 1;
            ++rom_tag;
            cur_off += val_end + 1;
            len_left -= (val_end + 1);
        }
    }
}

char *create_cx_resend_pad(unsigned long seq, char *rom, size_t * len)
{
    return 0;
}


size_t find_cx_offsets(char *data, size_t * seq,
                       size_t size, size_t byte_offset,
                       size_t beg_seq, size_t end_seq,
                       size_t * beg_off, size_t * end_off)
{
    return size;
}

static char get_cx_type(char *rom_mess)
{
    switch (rom_mess[0]) {
    case 'E':
        return 'L';
    case 'R':
        return 'P';
    case 'C':
        return 'N';
    default:
        return 'Z';
    }
}

void cx_send_message_to_roms(struct message *mess)
{
    long len_sent = 0;
    long ret_val = 0;
    int failed_attempts = 0;
    long t_len = mess->len;
    char *orig_data = mess->data;
    char *mess_off = mess->data;
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
    mess->len = t_len;
    mess->data = orig_data;
    record_sent_message(mess);
}

static void cx_rom_reject(cx_obj * c, char *rom, struct message *mess,
                          const char *error_mess)
{
    int len = 0;
    struct message m1;
    m1.name = mess->name;
    m1.name_len = mess->name_len;
    databuf_t *output = databuf_alloc(300);
    len = create_rom_reject(rom, output, error_mess);
    if (len > 0) {
        m1.data = output->buffer;
        m1.len = databuf_unread(output);
        cx_send_message_to_roms(&m1);
    }
    databuf_destroy(output);
}

static void cx_build_replace(cx_obj * c, char *rom,
                             struct dart_fix_pos *map,
                             char type, struct message *mess)
{
    size_t len = map[17].end - map[17].beg;
    int ret = 0;
    dart_ex_order_obj *obj = 0;
    char *rom_tag = rom + map[17].beg;
    //ret = get_map_entry(c->rom_map, rom_tag, len, (void*) &obj);
    ret = cx_get_obj_w_rom(c, rom_tag, len, (void *) &obj);
    if (ret > 0) {
        rom_tag = rom + map[6].beg;
        len = map[6].end - map[6].beg;
        if (len > 0) {
            double qty = atof(rom_tag);
            doa_set_order_qty(obj, qty);
        }
        rom_tag = rom + map[7].beg;
        len = map[7].end - map[7].beg;
        if (len > 0) {
            doa_set_price(obj, rom_tag, len);
        }
        send_seq_cx_message(c, type, obj);
    } else {
        cx_rom_reject(c, rom, mess, "Unable to find ROM TAG.");
    }
}

static void cx_build_order(cx_obj * c, char *rom,
                           struct dart_fix_pos *map,
                           char type, char *rom_name, int rom_name_len)
{
    size_t len = map[17].end - map[17].beg;
    char *rom_tag = rom + map[17].beg;
    dart_ex_order_obj *obj = (dart_ex_order_obj *) cx_get_dart_ord_obj(c);      //get_empty_dart_order_obj(c->mgr);
    doa_set_sender_name(obj, rom_name, rom_name_len);
    doa_set_rom_id(obj, rom_tag, len);
    rom_tag = rom + map[5].beg;
    len = map[5].end - map[5].beg;
    if (len > 0) {
        doa_set_symbol(obj, rom_tag, len);
    }
    rom_tag = rom + map[4].beg;
    len = map[4].end - map[4].beg;
    if (len > 0) {
        doa_set_side(obj, rom_tag, len);
    }
    rom_tag = rom + map[8].beg;
    len = map[8].end - map[8].beg;
    if (len > 0) {
        doa_set_type(obj, rom_tag, len);
    }
    rom_tag = rom + map[9].beg;
    len = map[9].end - map[9].beg;
    if (len > 0) {
        doa_set_tif(obj, rom_tag, len);
    }
    rom_tag = rom + map[18].beg;
    len = map[18].end - map[18].beg;
    if (len > 0) {
        double dmin = atof(rom_tag);
        doa_set_min_qty(obj, dmin);
    }
    rom_tag = rom + map[6].beg;
    len = map[6].end - map[6].beg;
    if (len > 0) {
        double qty = atof(rom_tag);
        doa_set_order_qty(obj, qty);
    }
    rom_tag = rom + map[7].beg;
    len = map[7].end - map[7].beg;
    if (len > 0) {
        doa_set_price(obj, rom_tag, len);
    }
    rom_tag = rom + map[34].beg;
    len = map[34].end - map[34].beg;
    if (len > 0) {
        double show = atof(rom_tag);
        doa_set_show_amt(obj, show);
    }
    send_seq_cx_message(c, type, obj);
}

void to_cx_func(struct message *mess, void *b)
{
    cx_obj *c = (cx_obj *) b;
    int mod_len = 0;
    char *mod = cx_get_mod_name(c, &mod_len);
    dart_rolling_log_coming(mess, mod, mod_len);
    if (!is_cx_connected(c)) {
        cx_rom_reject(c, mess->data, mess, "Not connected to CXOUCH.");
        return;
    }
    struct dart_fix_pos map[ROM_LEN];
    char type = get_cx_type(mess->data);
    cx_build_rom_pos(mess->data, mess->len, map, 0);
    if (type == 'L') {
        cx_build_order(c, mess->data, map, type, mess->name,
                       mess->name_len);
    } else if (type == 'P') {
        cx_build_replace(c, mess->data, map, type, mess);
    } else {
        size_t len = map[17].end - map[17].beg;
        if (len > 0) {
            dart_ex_order_obj *obj = 0;
            int ret = 0;
            char *rom_tag = mess->data + map[17].beg;
            ret = cx_get_obj_w_rom(c, rom_tag, len, (void *) &obj);
            if (ret > 0) {
                send_seq_cx_message(c, type, obj);
            } else {
                cx_rom_reject(c, mess->data, mess,
                              "Unable to find rom tag.");
            }
        }
    }
}

struct init_args *load_cx_config(const char *filename, int file_len)
{
    struct init_args *in_args = malloc(sizeof(struct init_args));
    int ret_len = 0;
    char *reset_h;
    char *reset_m;
    char *last_run_date;
    time_t rawtime;
    struct tm *ti;
    char *new_incoming;
    char *new_outgoing;
    char *now;
    int year;
    int month;
    int len;
    struct seq_reset_time *rt;
    in_args->dc = initialize_config(filename, file_len);
    in_args->name =
        get_val_for_tag(in_args->dc, "OUCH SERVER", 11, "UserID", 6,
                        &ret_len);
    in_args->name_len = ret_len;
    in_args->username =
        get_val_for_tag(in_args->dc, "OUCH SERVER", 11, "UserID", 6,
                        &ret_len);
    in_args->un_len = ret_len;
    in_args->password =
        get_val_for_tag(in_args->dc, "OUCH SERVER", 11, "password", 8,
                        &ret_len);
    in_args->p_len = ret_len;
    in_args->target_comp =
        get_val_for_tag(in_args->dc, "OUCH SERVER", 11, "sessionid", 9,
                        &ret_len);
    in_args->tc_len = ret_len;
    in_args->sender_comp = 0;
    in_args->sc_len = 0;
    in_args->target_sub = 0;
    in_args->ts_len = 0;
    in_args->sender_sub = 0;
    in_args->ss_len = 0;
    in_args->fix_version = 0;
    in_args->fv_len = 0;
    in_args->log_outgoing = 0;
    rt = malloc(sizeof(struct seq_reset_time));

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

    new_incoming =
        get_val_for_tag(in_args->dc, "OUCH SERVER", 10, "Reset Incoming",
                        14, &ret_len);
    if (new_incoming != NULL) {
        rt->new_incoming = atoi(new_incoming);
        free(new_incoming);
    } else {
        rt->new_incoming = 0;
    }
    new_outgoing =
        get_val_for_tag(in_args->dc, "OUCH SERVER", 10, "Reset Outgoing",
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
    now = calloc(9, 1);
    year = 1900 + ti->tm_year;
    month = 1 + ti->tm_mon;
    len = sprintf(now, "%04d%02d%02d", year, month, ti->tm_mday);
    if (last_run_date && strncmp(now, last_run_date, 8) == 0 && len <= 8) {
        rt->create_new = 0;
        if (ti->tm_hour == rt->reset_hour &&
            (ti->tm_min >= rt->reset_min &&
             (rt->reset_min + 5) >= ti->tm_min)) {
            rt->create_new = 1;
        }
    } else {
        rt->create_new = 1;
    }
    in_args->reset_time = rt;
    set_val_for_tag(in_args->dc, "TIMES", 5, "Last Run Day", 12, now, 8);
    free(now);
    free(last_run_date);
    return in_args;
}
