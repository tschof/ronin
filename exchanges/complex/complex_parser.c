/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fix_rom_trans.h"
#include "databuf.h"
#include "dart_constants.h"
#include "complex_parser.h"
#include "rom2fix.h"
#include "ex_common.h"
#include "fix_handler.h"


void set_fix_type(char rom_type, char *fix_type, int* ft_len, int is_spread,
                  int no_fix_43)
{
    if(rom_type == 'E' && is_spread && !no_fix_43) {
        memcpy(fix_type, "AB", 2);
        *ft_len = 2;
    } else if(rom_type == 'E') {
        memcpy(fix_type, "D", 1);
        *ft_len = 1;
    } else if(rom_type == 'C') {
        memcpy(fix_type, "F", 1);
        *ft_len = 1;
    } else if(rom_type == 'R' && is_spread && !no_fix_43) {
        memcpy(fix_type, "AC", 2);
        *ft_len = 2;
    } else if(rom_type == 'R') {
        memcpy(fix_type, "G", 1);
        *ft_len = 1;
    } else {
        memcpy(fix_type, "8", 1);
        *ft_len = 1;
    }
}


struct fix_tag_def **get_tag_def_bundle_only(char rom_type, orderbook * ob)
{
    switch (rom_type) {
    case 'E':
        return ob->rom_to_fix_out;
    case 'R':
        return ob->rom_to_fix_rpl;
    case 'C':
        return ob->rom_to_fix_cnl;
    default:
        return ob->rom_to_fix_status;
    }
}


struct fix_tag_def **get_tag_def_bundle(char rom_type, orderbook * ob,
                                        char *fix_type, int *ft_len)
{
    switch (rom_type) {
    case 'E':
        if(!ob->no_fix_43) {
            memcpy(fix_type, "AB", 2);
            *ft_len = 2;
        } else {
            memcpy(fix_type, "D", 1);
            *ft_len = 1;
        }
        return ob->rom_to_fix_out;
    case 'R':
        if(!ob->no_fix_43) {
            memcpy(fix_type, "AC", 2);
            *ft_len = 2;
        } else {
            memcpy(fix_type, "R", 1);
            *ft_len = 1;
        }
        return ob->rom_to_fix_rpl;
    case 'C':
        memcpy(fix_type, "F", 1);
        *ft_len = 1;
        return ob->rom_to_fix_cnl;
    default:
        memcpy(fix_type, "8", 1);
        *ft_len = 1;
        return ob->rom_to_fix_status;
    }
}

void build_pos_array(struct dart_fix_pos *l_positions,
                            struct fix_tag_def *ftd[ROM_LEN])
{
    int i = 0;
    for (; i < ROM_LEN; ++i) {
        l_positions[i].beg = 0;
        l_positions[i].end = 0;
        if (ftd[i] != NULL) {
            l_positions[i].name = ftd[i]->name;
            l_positions[i].len = ftd[i]->len;
        } else {
            l_positions[i].name = NULL;
            l_positions[i].len = 0;
        }
    }
}

void build_pos_array2(struct dart_fix_pos* l_positions,
                             struct fix_tag_def* ftd[ROM_LEN],
                             int leg_array_size)
{
    int i = 0;
    for (; i < ROM_LEN; ++i) {
        l_positions[i].beg = 0;
        l_positions[i].end = 0;
        l_positions[i].name = NULL;
        l_positions[i].len = 0;
        int j = 0;
        for(;j < leg_array_size; ++j) {
            if (ftd[j]->rom_tag == i) {
                l_positions[i].name = ftd[j]->name;
                l_positions[i].len = ftd[j]->len;
            } 
        }
    }
}

static dart_ex_order_obj *get_order_obj(char *rom,
                                        struct dart_fix_pos
                                        positions[ROM_LEN], orderbook * ob)
{
    char *beg_off;
    long len = 0;
    dart_ex_order_obj *obj = NULL;
    beg_off = rom;
    beg_off += positions[17].beg;
    len = (positions[17].end - positions[17].beg);
    pthread_mutex_lock(&ob->mutex);
    get_map_entry(ob->rom_ord_id_map, beg_off, len, (void *) &obj);
    pthread_mutex_unlock(&ob->mutex);
    return obj;
}

static void set_leg_id(dart_ex_order_obj * obj,
                       dart_leg_obj * leg_obj,
                       int leg_pos, databuf_t * output)
{
    int len = 0;
    char *leg_id = doa_get_leg_id(obj, leg_obj, &len);
    databuf_write(output, "%c654=%s", FIX_FIELD_DELIM, leg_id);
    free(leg_id);
}
/*
static void numeric_only_leg_id(dart_ex_order_obj * obj,
                                dart_leg_obj * leg_obj,
                                int leg_pos, databuf_t * output)
{
    char leg_id[5];
    int len = sprintf(leg_id, "%d", leg_pos);
    databuf_write(output, "%c654=", FIX_FIELD_DELIM);
    databuf_memcpy(output, leg_id, (len ));
    doa_set_leg_id(obj, leg_id, (len), leg_obj);
}
*/

static void create_n_set_leg_id(dart_ex_order_obj * obj,
                                dart_leg_obj * leg_obj,
                                int leg_pos, databuf_t * output)
{
    size_t clordid_len = 0;
    char leg_id[5];
    char *leg_off = leg_id;
    char *cl_ord_id = doa_get_cl_ord_id(obj, &clordid_len);
    char* cp_point = cl_ord_id;
    int len = sprintf(leg_id, "%d", leg_pos);
    int left_over = 5 - len;
    int cp_len = 0;
    leg_off += len;
    cp_len = left_over > clordid_len ? clordid_len : left_over;
    cp_point += (clordid_len - cp_len);
    memcpy(leg_off, cp_point, cp_len);
    databuf_write(output, "%c654=", FIX_FIELD_DELIM);
    databuf_memcpy(output, leg_id, (len + cp_len));
    doa_set_leg_id(obj, leg_id, (len + cp_len), leg_obj);
    if(cl_ord_id) {
        free(cl_ord_id);
   }
}

static void set_rom_tag_in_leg(char *rom, dart_fix_pos * positions,
                               dart_leg_obj * l_obj)
{
    long len = 0;
    char *beg_off = rom;
    beg_off += positions[17].beg;
    len = (positions[17].end - positions[17].beg);
    memset(l_obj->rom_id, 0, DART_EX_TAG_LEN);
    memcpy(l_obj->rom_id, beg_off, len);
}

int def_option_rom_tag(char *fixmsg, dart_rom_pos * positions,
                       char mtype, char *status,
                       databuf_t * rom, size_t romfield,
                       int *rejtected, dart_ex_order_obj * order,
                       pthread_mutex_t * mutex, hashmap_t ord_map)
{
    if (doa_get_is_spread(order)) {
        char *beg_off = fixmsg;
        char *sec_beg_off = fixmsg;
        int len = positions[romfield].end - positions[romfield].beg;
        beg_off += positions[romfield].beg;
        int sec_len = positions[66].end - positions[66].beg;
        sec_beg_off += positions[66].beg;
        if (len > 0 && sec_len > 0 && sec_beg_off[0] != '3') {
            size_t rom_len = 0;
            char *id =
                doa_get_rom_id_for_leg(order, &rom_len, beg_off, len);
            databuf_memcpy(rom, id, rom_len);
            free(id);
        } else {
            add_rom_id_to_mess(rom, order);
        }
    } else {
        add_rom_id_to_mess(rom, order);
    }
    return 1;
}

int def_option_ord_ids(char *fixmsg, dart_rom_pos * positions,
                       char mtype, char *status,
                       databuf_t * rom, size_t romfield,
                       int *rejtected, dart_ex_order_obj * order,
                       pthread_mutex_t * mutex, hashmap_t ord_map)
{
    size_t len = positions[17].end - positions[17].beg;
    if (doa_get_is_spread(order) && len > 0) {
        char *beg_off = fixmsg;
        beg_off += positions[17].beg;
        databuf_memcpy(rom, beg_off, len);
        return 1;
    } else {
        return handle_clordids(status, rom, fixmsg, positions, mutex,
                               ord_map, order);
    }
}

int def_create_leg_tag(char *fixmsg, dart_rom_pos * positions,
                       char mtype, char *status,
                       databuf_t * rom, size_t romfield,
                       int *rejtected, dart_ex_order_obj * order,
                       pthread_mutex_t * mutex, hashmap_t ord_map)
{
    size_t len = positions[66].end - positions[66].beg;
    char* beg_off = fixmsg;
    beg_off += positions[66].beg;
    if (doa_get_is_spread(order) && len > 0 && beg_off[0] == '2') {
        databuf_memcpy(rom, "6", 1);
    } else if(doa_get_is_spread(order) && len > 0) {
        databuf_memcpy(rom, "5", 1);
    } else if (doa_get_is_spread(order)) {
        databuf_memcpy(rom, "3", 1);
    }
    return 1;
}

void set_option_fix_to_rom(orderbook * ob)
{
    ob->fix2rom[63] = def_no_op;
    ob->fix2rom[15] = def_option_ord_ids;
    ob->fix2rom[17] = def_option_rom_tag;
    ob->fix2rom[66] = def_create_leg_tag;
    ob->fix2rom[26] = def_map_fix_text_to_rom;
}

int trans_mleg_symbol(char *rom, dart_fix_pos * map,
                      char *mtype, databuf_t * fix,
                      size_t field, int *rejected,
                      dart_ex_order_obj * order, dart_leg_obj * l_obj)
{
    int clean_up = 0;
    int len = map[field].end - map[field].beg;
    char *put_call = rom + map[field].beg;
    if (len > 0) {
        doa_set_leg_symbol(order, put_call, len, l_obj);
    } else {
        put_call = doa_get_leg_symbol(order, l_obj, &len);
        clean_up = 1;
    }
    if (len > 0 && put_call != 0) {
        databuf_write(fix, "%c%s=", FIX_FIELD_DELIM, "600");
        databuf_memcpy(fix, put_call, len);
    }

    if (clean_up) {
        free(put_call);
    }

    return 1;
}

int trans_mleg_side(char *rom, dart_fix_pos * map,
                    char *mtype, databuf_t * fix,
                    size_t field, int *rejected,
                    dart_ex_order_obj * order, dart_leg_obj * l_obj)
{
    int clean_up = 0;
    int len = map[field].end - map[field].beg;
    char *put_call = rom + map[field].beg;
    if (len > 0) {
        doa_set_leg_side(order, put_call, len, l_obj);
    } else {
        put_call = doa_get_leg_side(order, l_obj, &len);
        clean_up = 1;
    }
    if (len > 0 && put_call != 0) {
        databuf_write(fix, "%c%s=", FIX_FIELD_DELIM, "624");
        databuf_memcpy(fix, put_call, len);
    }

    if (clean_up) {
        free(put_call);
    }

    return 1;
}

int trans_mleg_ratio(char *rom, dart_fix_pos * map,
                     char *mtype, databuf_t * fix,
                     size_t field, int *rejected,
                     dart_ex_order_obj * order, dart_leg_obj * l_obj)
{
    int clean_up = 0;
    int len = map[field].end - map[field].beg;
    char *put_call = rom + map[field].beg;
    if (len > 0) {
        doa_set_leg_ratio(order, put_call, len, l_obj);
        l_obj->i_ratio = atoi(put_call);
    } else {
        put_call = doa_get_leg_ratio(order, l_obj, &len);
        clean_up = 1;
    }
    if (len > 0 && put_call != 0) {
        databuf_write(fix, "%c%s=", FIX_FIELD_DELIM, "623");
        databuf_memcpy(fix, put_call, len);
    }

    if (clean_up) {
        free(put_call);
    }

    return 1;
}

int trans_mleg_stike(char *rom, dart_fix_pos * map,
                     char *mtype, databuf_t * fix,
                     size_t field, int *rejected,
                     dart_ex_order_obj * order, dart_leg_obj * l_obj)
{
    int clean_up = 0;
    int len = map[field].end - map[field].beg;
    char *put_call = rom + map[field].beg;
    if (len > 0) {
        doa_set_leg_strike(order, put_call, len, l_obj);
    } else {
        put_call = doa_get_leg_strike(order, l_obj, &len);
        if(put_call) {
            clean_up = 1;
        }
    }
    if (len > 0) {
        databuf_write(fix, "%c%s=", FIX_FIELD_DELIM, "612");
        databuf_memcpy(fix, put_call, len);
    }

    if (clean_up) {
        free(put_call);
    }

    return 1;
}

int trans_mleg_open_close(char *rom, dart_fix_pos * map,
                          char *mtype, databuf_t * fix,
                          size_t field, int *rejected,
                          dart_ex_order_obj * order, dart_leg_obj * l_obj)
{
    int clean_up = 0;
    int len = map[field].end - map[field].beg;
    char *put_call = rom + map[field].beg;
    if (len > 0) {
        if (strncmp(put_call, "1", 1) == 0) {
            put_call = "O";
        } else {
            put_call = "C";
        }
        doa_set_leg_open_close(order, put_call, len, l_obj);
    } else {
        put_call = doa_get_leg_open_close(order, l_obj, &len);
        if(len > 0 && put_call) {
            clean_up = 1;
        }
    }
    if (len > 0) {
        databuf_write(fix, "%c%s=", FIX_FIELD_DELIM, "564");
        databuf_memcpy(fix, put_call, len);
    }

    if (clean_up) {
        free(put_call);
    }

    return 1;
}
int trans_mleg_sec_type(char *rom, dart_fix_pos * map,
                        char *mtype, databuf_t * fix,
                        size_t field, int *rejected,
                        dart_ex_order_obj * order, dart_leg_obj * l_obj)
{
    int clean_up = 0;
    int len = map[field].end - map[field].beg;
    char *put_call = rom + map[field].beg;
    if (len > 0) {
        doa_set_leg_sec_type(order, put_call, len, l_obj);
    } else if( len <= 0 && rom[0] == 'E') {
        doa_set_leg_sec_type(order, "OPT", 3, l_obj);        
    } else {
        put_call = doa_get_leg_sec_type(order, l_obj, &len);
        if(put_call) {
           clean_up = 1;
        }
    }
    if (len > 0) {
        databuf_write(fix, "%c%s=", FIX_FIELD_DELIM, "609");
        if(put_call[0] == 'O') {
            databuf_memcpy(fix, "OPT", 3);
        } else if(put_call[0] == 'E') {
            databuf_memcpy(fix, "CS", 2);
        } else {
            databuf_memcpy(fix, "FUT", 3);
        }
    } else {
        databuf_write(fix, "%c%s=OPT", FIX_FIELD_DELIM, map[field].name);
    }

    if (clean_up) {
        free(put_call);
    }

    return 1;
}

int trans_mleg_put_call(char *rom, dart_fix_pos * map,
                        char *mtype, databuf_t * fix,
                        size_t field, int *rejected,
                        dart_ex_order_obj * order, dart_leg_obj * l_obj)
{
    int clean_up = 0;
    int len = map[field].end - map[field].beg;
    char *put_call = rom + map[field].beg;
    if (len > 0) {
        doa_set_leg_call_put(order, put_call, len, l_obj);
    } else {
        put_call = doa_get_leg_call_put(order, l_obj,&len);
        if(len > 0) {
           clean_up = 1;
        }
    }
    if (len > 0) {
        databuf_write(fix, "%c%s=O", FIX_FIELD_DELIM, "608");
        databuf_memcpy(fix, put_call, len);
    } else {
        databuf_write(fix, "%c608=ES", FIX_FIELD_DELIM);
    }

    if (clean_up) {
        free(put_call);
    }

    return 1;
}

int trans_leg_mat_date(char *rom, dart_fix_pos * map,
                       char *type, databuf_t * fix,
                       size_t field, int *rejected,
                       dart_ex_order_obj * order, dart_leg_obj * l_obj)
{
    int len = map[field].end - map[field].beg;
    char *mat_date = rom + map[field].beg;
    char t_date[20];
    size_t d_len = map[62].end - map[62].beg;
    if (len > 0 && d_len > 0) {
        memset(t_date, 0, 20);
        memcpy(t_date, mat_date, len);
        memcpy((t_date + len), (rom + map[62].beg), d_len);
        doa_set_leg_expire_date(order, t_date, (len + d_len), l_obj);
        databuf_write(fix, "%c%s=", FIX_FIELD_DELIM, map[field].name);
        databuf_memcpy(fix, t_date, (len + d_len));
    } else {
        mat_date = doa_get_leg_expire_date(order, l_obj, &len);
        if(len > 0) {
            databuf_write(fix, "%c%s=", FIX_FIELD_DELIM, "611");
            databuf_memcpy(fix, mat_date, len);
        }
        free(mat_date);
    }
    return 1;
}

long trans_rom_spread_leg(char *rom, databuf_t * output,
                          long fix_len, char *fix_type,
                          struct dart_fix_pos positions[ROM_LEN],
                          int *rejected, orderbook * ob,
                          dart_ex_order_obj * parent, dart_leg_obj * lobj)
{
    int i = 0;
    for (; i < ob->leg_array_size; ++i) {
        if (ob->r2fleg[i].rom_tag != 0) {
            if (ob->r2fleg[i].legfunc
                (rom, positions, fix_type, output, ob->r2fleg[i].rom_tag,
                 rejected, parent, lobj) == -1) {
                fprintf(stderr, "%s: Processing %d failed\n", __FUNCTION__,
                        i);
                return 0;
            }
        }
    }

    return 0;
}

static void spread_loop(char *rom, databuf_t * output,
                        long fix_len, char *fix_type,
                        struct dart_fix_pos positions[ROM_LEN],
                        int *rejected, orderbook * ob,
                        int num_legs, dart_ex_order_obj * obj)
{
    int leg_pos = 1;
    element_t g;
    struct dart_fix_pos l_pos[ROM_LEN];
    build_pos_array(l_pos, ob->rom_to_fix_leg);
    g = front(doa_get_children(obj));
    while (g != NULL) {
        if (g->d != NULL) {
            dart_leg_obj *leg_obj = (dart_leg_obj *) g->d;
            if(!ob->id_last) {
                set_leg_id(obj, leg_obj, leg_pos, output);
            }
            trans_rom_spread_leg(rom, output, fix_len,
                                 fix_type, l_pos, rejected,
                                 ob, obj, leg_obj);
            if(ob->id_last) {
                set_leg_id(obj, leg_obj, leg_pos, output);
            }
        }
        ++leg_pos;
        g = g->next;
    }

    if (!*rejected) {
        databuf_memcpy(output, "\x01", 1);
    }
}

static void new_spread_loop(char *rom, databuf_t * output,
                            long fix_len, char *fix_type,
                            struct dart_fix_pos positions[ROM_LEN],
                            int *rejected, orderbook * ob,
                            queue_t legs, int num_legs,
                            dart_ex_order_obj * obj, int id_last)
{
    int leg_pos = 0;
    element_t e;
    e = front(legs);
    while (e != NULL) {
        if (e->d != NULL) {
            struct dart_fix_pos *l_pos = (struct dart_fix_pos *) e->d;
            dart_leg_obj *leg_obj = calloc(1,sizeof(dart_leg_obj));
            set_rom_tag_in_leg(rom, l_pos, leg_obj);
            if(!id_last) {
                create_n_set_leg_id(obj, leg_obj, leg_pos, output);
                //numeric_only_leg_id(obj, leg_obj, leg_pos, output);
            }
            trans_rom_spread_leg(rom, output, fix_len,
                                 fix_type, l_pos, rejected,
                                 ob, obj, leg_obj);
            if(id_last && !*rejected) {
                //numeric_only_leg_id(obj, leg_obj, leg_pos, output);
                create_n_set_leg_id(obj, leg_obj, leg_pos, output);
            }
            if(leg_pos == 1) {
                leg_obj->chosen_one = 1;
                int q = (int)doa_get_order_qty(obj);
                doa_set_spread_head_qty(obj, q);
            } else {
                leg_obj->chosen_one = 0;
            }
            doa_add_child(obj, leg_obj);
            free(e->d);
        }
        e = e->next;
        ++leg_pos;
        if(*rejected) {
            return;
        }
    }
    if (!*rejected) {
        databuf_memcpy(output, "\x01", 1);
    }
}

void trans_rom_spread_body(struct message *mess,
                           char *rom, databuf_t * output,
                           long fix_len, char *fix_type,
                           struct dart_fix_pos positions[ROM_LEN],
                           int *rejected, orderbook * ob,
                           queue_t legs, int num_legs, int id_last)
{
    dart_ex_order_obj *obj = NULL;
    rom2fix_translate_fields(mess, rom,
                             output, fix_len,
                             fix_type, positions, rejected, ob, 1);
    if(num_legs > 0) {
        databuf_write(output, "555=%d", num_legs);
    }
    obj = get_order_obj(rom, positions, ob);
    doa_set_is_spread(obj, 1);
    doa_set_num_legs(obj, num_legs);
    doa_set_pending_cancel(obj, num_legs);
    new_spread_loop(rom, output, fix_len, fix_type,
                    positions, rejected, ob, legs, num_legs, obj, id_last);
}

void build_rom_pos_leg(char* rom_message, long rom_len,
                    struct fix_tag_def* ftd[ROM_LEN],
                    struct dart_fix_pos* l_positions,
                    long offset, int ftd_len)
{
    long len = rom_len;
    char* c = rom_message;
    char* val_off = c;
    int rom_tag = 0;
    for(;len > 0;++c, --len) {
        if(*c == ',' || *c == '\n') {
            l_positions[rom_tag].beg = (val_off - rom_message) + offset;
            l_positions[rom_tag].end = (c - rom_message) + offset;
            l_positions[rom_tag].name = NULL;
            l_positions[rom_tag].len = 0;
            int j =0;
            for(;j < ftd_len; ++j) { //
                if(ftd[j]->rom_tag == rom_tag) {
                    l_positions[rom_tag].name = ftd[j]->name;
                    l_positions[rom_tag].len = ftd[j]->len;
                }
            }
            val_off = c + 1;
            ++rom_tag;
            if(rom_tag > ROM_LEN) {
                break;
            }
        }
    }
}
long deconstruct_rom_spread(struct message *mess,
                            char *rom, long rom_len, char *leg_off,
                            databuf_t * output,
                            char *fix_type, int *ft_len, int *rejected,
                            orderbook * ob, int id_last)
{
    long bod_len = leg_off - rom;
    struct fix_tag_def **ftd =
        get_tag_def_bundle(rom[0], ob, fix_type, ft_len);
    struct dart_fix_pos positions[ROM_LEN];
    char *cur_off = rom;
    long len_left = rom_len;
    int num_legs = 0;
    queue_t leg_pos = create_sized_queue(10);
    build_pos_array(positions, ftd);
    build_rom_pos2(rom, bod_len, ftd, positions, 0);
    leg_off += 3;
    len_left -= (leg_off - rom);
    cur_off = leg_off;
    for(;len_left > 0;++leg_off, --len_left) {
        if(*leg_off == '!' &&  len_left > 3 && leg_off[1] == '#') {
            long l_off = cur_off - rom;
            struct dart_fix_pos *l_pos =
                malloc(sizeof(struct dart_fix_pos) * ROM_LEN);
            build_pos_array2(l_pos, ob->rom_to_fix_leg, ob->leg_array_size);
            num_legs++;
            bod_len = leg_off - cur_off;
            build_rom_pos_leg(cur_off, bod_len,//(bod_len + l_off),
                          ob->rom_to_fix_leg, l_pos, l_off, ob->leg_array_size);
            enqueue(leg_pos, l_pos);
            leg_off += 3;
            cur_off = leg_off;
            len_left -= 3;
        }
    }
    trans_rom_spread_body(mess, rom, output, rom_len, fix_type,
                          positions, rejected, ob, leg_pos, num_legs, id_last);
    destroy_queue(leg_pos);
    return databuf_unread(output);
}

static long non_new_order_breakdown(struct message *mess, char *rom,
                                    long rom_len, databuf_t * output,
                                    char *fix_type, int *ft_len,
                                    int *rejected, orderbook * ob,
                                    long (*rom_to_fix_override) (struct
                                                                 message *
                                                                 mess,
                                                                 char
                                                                 *fix_message,
                                                                 databuf_t
                                                                 * output,
                                                                 long
                                                                 fix_len,
                                                                 char
                                                                 *fix_type,
                                                                 struct
                                                                 dart_fix_pos
                                                                 positions
                                                                 [ROM_LEN],
                                                                 int
                                                                 *rejected,
                                                                 orderbook
                                                                 * ob,
                                                                 int is_spread))
{
    struct fix_tag_def **ftd =
        get_tag_def_bundle_only(rom[0], ob);
    struct dart_fix_pos positions[ROM_LEN];
    char *beg_off = NULL;
    long len = 0;
    size_t ret = 0;
    dart_ex_order_obj *obj = NULL;
    build_pos_array(positions, ftd);
    build_rom_pos2(rom, rom_len, ftd, positions, 0);
    beg_off = rom;
    beg_off += positions[17].beg;
    len = (positions[17].end - positions[17].beg);
    pthread_mutex_lock(&ob->mutex);
    ret = get_map_entry(ob->rom_ord_id_map, beg_off, len, (void *) &obj);
    pthread_mutex_unlock(&ob->mutex);
    if (ret) {
        if (doa_get_is_spread(obj) && !(rom[0] == 'C' && ob->no_fix_43)) {

            set_fix_type(rom[0], fix_type, ft_len, 1, ob->no_fix_43);
            rom2fix_translate_fields(mess, rom,
                                     output, rom_len,
                                     fix_type, positions, rejected, ob,1);
            if(doa_get_num_legs(obj) > 0) {
                databuf_write(output, "555=%d", doa_get_num_legs(obj));
                spread_loop(rom, output, rom_len, fix_type,
                            positions, rejected, ob,
                            doa_get_num_legs(obj), obj);
            }
        } else {
            set_fix_type(rom[0], fix_type, ft_len, 0, ob->no_fix_43);
            rom_to_fix_override(mess, rom, output,
                                rom_len, fix_type, positions,
                                rejected, ob, 0);
        }
    }
    return databuf_unread(output);
}

long from_rom_determine_spread(struct message *mess,
                               char *rom_message, long rom_len,
                               databuf_t * output,
                               char *fix_type, int *ft_len, int *rejected,
                               orderbook * ob,
                               long (*rom_to_fix_override) (struct message
                                                            * mess,
                                                            char
                                                            *fix_message,
                                                            databuf_t *
                                                            output,
                                                            long fix_len,
                                                            char *fix_type,
                                                            struct
                                                            dart_fix_pos
                                                            positions
                                                            [ROM_LEN],
                                                            int *rejected,
                                                            orderbook *
                                                            ob, int is_spread))
{
    int ret_code = 0;
    if (rom_message[0] == 'E') {
        char *leg_off = NULL;
        if ((leg_off = strstr(rom_message, ROM_SPREAD_SEP)) != NULL
            && (leg_off - rom_message) < rom_len) {
            ret_code =
                deconstruct_rom_spread(mess, rom_message, rom_len, leg_off,
                                       output, fix_type, ft_len, rejected,
                                       ob,ob->id_last);
        } else {
            ret_code =
                trans_rom_message(mess, rom_message, rom_len, output,
                                  fix_type, ft_len, rejected, ob,
                                  rom_to_fix_override);
        }
    } else {
        ret_code =
            non_new_order_breakdown(mess, rom_message, rom_len, output,
                                    fix_type, ft_len, rejected, ob,
                                    rom_to_fix_override);
    }
    return ret_code;
}

void complex_from_rom_func(struct message *mess, void *b)
{
    databuf_t *output = databuf_alloc(mess->len * 8);
    orderbook *ob = (orderbook *) b;
    long total_len;
    struct message m1;
    struct message log;
    char fix_type[3];
    int ft_len = 0;
    int rejected = 0;
    total_len = 0;

    log.name = mess->name;
    log.name_len = mess->name_len;
    log.data = mess->data;
    log.len = mess->len;
    dart_rolling_log_coming(&log, ob->module_name, ob->module_name_len);

    if (ob->is_connected()) {
        m1.name = mess->name;
        m1.name_len = mess->name_len;
        memset(fix_type, 0, 3);
        total_len =
            from_rom_determine_spread(&m1, mess->data, mess->len, output,
                                      fix_type, &ft_len, &rejected,
                                      (orderbook *) ob,
                                      &rom2fix_translate_fields);
        if (rejected) {
            m1.data = output->buffer;
            m1.len = databuf_unread(output);
            send_message_to_roms(&m1, ob);
        } else if (total_len > 0) {
            m1.name = ob->fix_con_name;
            m1.name_len = ob->fix_con_len;
            m1.data = output->buffer;
            m1.len = databuf_unread(output);
            send_fix_message(&m1, fix_type, ft_len);
        }
    } else {
        char *l_message = "Not connected to Exchange.";
        rejected = 1;
        m1.name = mess->name;
        m1.name_len = mess->name_len;
        total_len = create_rom_reject(mess->data, output, l_message);
        if (total_len > 0) {
            m1.data = output->buffer;
            m1.len = databuf_unread(output);
            send_message_to_roms(&m1, ob);
        } else {
            databuf_destroy(output);
            return;
        }
    }
    databuf_destroy(output);
}

int set_leg_value(char *fix_mess, size_t len, char *fix_tag,
                  size_t tag_len, databuf_t * output)
{
    char *tag_off = strstr(fix_mess, fix_tag);
    if (tag_off != NULL && (tag_off - fix_mess) < len) {
        char *val_off = tag_off + tag_len;
        char *end_off = strchr(val_off, FIX_FIELD_DELIM);
        if (end_off != NULL && (end_off - fix_mess) < len) {
            databuf_memcpy(output, val_off, (end_off - val_off));
        }
    }
    return 1;
}

/*
 *
 */
void fs_set_spread_book(orderbook * ob)
{
    ob->fix_rom_spreadx[30] = get_mat_month_year;
    ob->fix_rom_spreadx[62] = break_n_set_maturity_day;
    ob->fix_rom_spreadx[5] = set_leg_symbol;
    ob->fix_rom_spreadx[78] = set_leg_sec_id;
    ob->fix_rom_spreadx[31] = set_cfi_code;
    ob->fix_rom_spreadx[32] = set_leg_strike;
    ob->fix_rom_spreadx[38] = set_leg_pos_eff;
    ob->fix_rom_spreadx[55] = set_leg_sec_desc;
    ob->fix_rom_spreadx[4] = set_leg_side;
    ob->fix_rom_spreadx[6] = set_leg_ratio;
    ob->fix_rom_spreadx[3] = set_leg_multi;
    ob->fix_rom_spreadx[35] = set_leg_sec_type;
}
/**
 *
600,5
608,31
611,30
612,32
564,38
623,6
624,4
37,17
 */
void set_rom2fix_leg_book(orderbook* ob)
{
    ob->r2fleg[1].legfunc = trans_mleg_put_call;
    ob->r2fleg[1].rom_tag = 31;
    ob->r2fleg[4].legfunc = trans_mleg_open_close;
    ob->r2fleg[4].rom_tag = 38;
    ob->r2fleg[3].legfunc = trans_mleg_stike;
    ob->r2fleg[3].rom_tag = 32;
    ob->r2fleg[2].legfunc = trans_leg_mat_date;
    ob->r2fleg[2].rom_tag = 30;
    ob->r2fleg[0].legfunc = trans_mleg_symbol;
    ob->r2fleg[0].rom_tag = 5;
    ob->r2fleg[6].legfunc = trans_mleg_side;
    ob->r2fleg[6].rom_tag = 4;
    ob->r2fleg[5].legfunc = trans_mleg_ratio;
    ob->r2fleg[5].rom_tag = 6;
}
/*void set_rom2fix_leg_book(orderbook * ob)
{
    ob->r2fleg[31] = trans_mleg_put_call;
    ob->r2fleg[38] = trans_mleg_open_close;
    ob->r2fleg[32] = trans_mleg_stike;
    ob->r2fleg[30] = trans_leg_mat_date;
    ob->r2fleg[5] = trans_mleg_symbol;
    ob->r2fleg[4] = trans_mleg_side;
    ob->r2fleg[6] = trans_mleg_ratio;
}*/

int get_mat_month_year(char *fix_mess, size_t len, int rom_tag,
                       databuf_t * output)
{
    char *mat_date = strstr(fix_mess, FIX_LEGEXPIRE_DATE);
    if (mat_date != NULL && (mat_date - fix_mess) < len) {
        char *val_off = mat_date + 4;
        databuf_memcpy(output, val_off, 6);
    }
    return 1;
}

int break_n_set_maturity_day(char *fix_mess, size_t len, int rom_tag,
                             databuf_t * output)
{
    char *mat_date = strstr(fix_mess, FIX_LEGEXPIRE_DATE);
    if (mat_date != NULL && (mat_date - fix_mess) < len) {
        char *val_off = mat_date + 10;
        databuf_memcpy(output, val_off, 2);
    }
    return 1;
}

int set_leg_symbol(char *fix_mess, size_t len, int rom_tag,
                   databuf_t * output)
{
    return set_leg_value(fix_mess, len, "600", 4, output);
}

int set_leg_sec_id(char *fix_mess, size_t len, int rom_tag,
                   databuf_t * output)
{
    return set_leg_value(fix_mess, len, "602", 4, output);
}

int set_cfi_code(char *fix_mess, size_t len, int rom_tag,
                 databuf_t * output)
{
    char *tag_off = strstr(fix_mess, "608");
    if (tag_off != NULL && (tag_off - fix_mess) < len) {
        char *val_off = tag_off + 5;
        databuf_memcpy(output, val_off, 1);
    }
    return 1;
}

int set_leg_strike(char *fix_mess, size_t len, int rom_tag,
                   databuf_t * output)
{
    return set_leg_value(fix_mess, len, "612", 4, output);
}

int set_leg_sec_type(char *fix_mess, size_t len, int rom_tag,
                     databuf_t * output)
{
    char *tag_off = strstr(fix_mess, "608");
    if (tag_off != NULL && (tag_off - fix_mess) < len) {
        char *val_off = tag_off + 4;
        databuf_memcpy(output, val_off, 1);
    }
    return 1;
}

int set_leg_pos_eff(char *fix_mess, size_t len, int rom_tag,
                    databuf_t * output)
{
    char *tag_off = strstr(fix_mess, "564");
    if (tag_off != NULL && (tag_off - fix_mess) < len) {
        char *val_off = tag_off + 4;
        if (val_off[0] == 'O') {
            databuf_memcpy(output, "1", 1);
        } else {
            databuf_memcpy(output, "0", 1);
        }
    }
    return 1;
}

int set_leg_sec_desc(char *fix_mess, size_t len, int rom_tag,
                     databuf_t * output)
{
    return set_leg_value(fix_mess, len, "620", 4, output);
}

int set_leg_side(char *fix_mess, size_t len, int rom_tag,
                 databuf_t * output)
{
    return set_leg_value(fix_mess, len, "624", 4, output);
}

int set_leg_ratio(char *fix_mess, size_t len, int rom_tag,
                  databuf_t * output)
{
    return set_leg_value(fix_mess, len, "623", 4, output);
}

int set_leg_multi(char *fix_mess, size_t len, int rom_tag,
                  databuf_t * output)
{
    return set_leg_value(fix_mess, len, "654", 4, output);
}

long check_for_spreads(struct message *mess, char *fix_message,
                       databuf_t * output, long fix_len,
                       char rom_type,
                       struct dart_rom_pos positions[ROM_LEN],
                       int *rejected, orderbook * ob)
{
    size_t len = fix_len;
    char *next_off = NULL;
    char *leg_finder = fix_message;
    int i = 0;
    int found_spread = 0;
    char *leg_off = strstr(leg_finder, SEARCH_FIX_LEG_SYM);
    while (leg_off != NULL && (leg_off - leg_finder) < fix_len) {
        databuf_write(output, "!#!%c", rom_type);
        found_spread = 1;
        len = fix_len - (leg_off - fix_message);
        next_off = strstr((leg_off + 3), FIX_LEGSYMB);
        if (next_off != NULL) {
            len = next_off - leg_off;
        }
        for (i = 0; i < ROM_LEN; ++i) {
            if (ob->fix_rom_spreadx[i] != 0) {
                ob->fix_rom_spreadx[i] (leg_off, len, i, output);
            }
            databuf_memcpy(output, ",", 1);
        }
        leg_finder = leg_off + 3;
        leg_off = strstr(leg_finder, SEARCH_FIX_LEG_SYM);
    }
    if (found_spread) {
        databuf_memcpy(output, "!#!", 3);
    }
    return 1;
}

long fix_rom_spread_override(struct message *mess, char *fix_message,
                             databuf_t * output, long fix_len,
                             char rom_type,
                             struct dart_rom_pos positions[ROM_LEN],
                             int *rejected, orderbook * ob)
{
    default_fix_to_rom_override(mess, fix_message,
                                output, fix_len,
                                rom_type, positions, rejected, ob);

    output->wr_ptr--;
    check_for_spreads(mess, fix_message, output, fix_len, rom_type,
                      positions, rejected, ob);
    databuf_memcpy(output, "\n", 1);
    return 1;
}
