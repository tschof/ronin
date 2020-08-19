/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "dart_order_obj.h"

dart_order_obj *explicit_doo_create()
{
    dart_order_obj *doj = calloc(1, sizeof(struct dart_order_obj));
    doj->positions = calloc(ROM_LEN, sizeof(struct dart_fix_pos));
    doj->data = databuf_alloc(500);
    return doj;
}

/**
 * This function is needed if these objects are to be used with the
 * dart_obj_factory.
 */
void create_dart_obj_func(queue_t glob, queue_t q, int num)
{
    int offset = 0;
    while (offset < num) {
        dart_order_obj *doj = explicit_doo_create();
        enqueue(q, doj);
        ++offset;
    }
}

/**
 * Clean function used with the object factory.
 */
void clean_doo(void *u)
{
    dart_order_obj *doj = (dart_order_obj *) u;
    memset(doj->positions, '\0', sizeof(struct dart_fix_pos) * ROM_LEN);
    doj->rep = NULL;
    databuf_reset(doj->data);
}

/**
 * Generic destroy function used with object factory
 */
void destroy_doo(void *d)
{
    dart_order_obj *doj = (dart_order_obj *) d;
    databuf_destroy(doj->data);
    if (doj->rep) {
        destroy_doo(doj->rep);
    }
    free(doj->positions);
    free(doj);
}

/**
 * Takes the rom field number and returns the offset to the begining
 * of the field and the length of that field in the len pointer.  If
 * the field does not exist then the return will be null and len will
 * be set to 0.
 */
char *get_rom_field(dart_order_obj * doj, int rom_num, int *len)
{
    char *off = doj->data->buffer + doj->positions[rom_num].beg;
    *len = doj->positions[rom_num].end - doj->positions[rom_num].beg;
    return off;
}

/**
 * This will append this new value to the end of the databuf and store
 * the offsets in the positions array at rom_num.  It will return 0 if
 * the operation fails.
 */
int set_rom_field(dart_order_obj * doj, int rom_num, const void *val,
                  int len)
{
    int ret = 1;
    doj->positions[rom_num].beg = (doj->data->wr_ptr - doj->data->buffer);
    doj->positions[rom_num].end = doj->positions[rom_num].beg + len;
    doj->positions[rom_num].val_len = len;
    databuf_memcpy(doj->data, val, len);
    return ret;
}

/**
 * This will change the values saved in the positions array for the
 * rom field (rom_num) to beg and len.
 */
int reset_rom_field(dart_order_obj * doj, int rom_num, int beg, int len)
{
    int ret = 1;
    doj->positions[rom_num].beg = beg;
    doj->positions[rom_num].end = doj->positions[rom_num].beg + len;
    doj->positions[rom_num].val_len = len;
    return ret;
}

int fill_in_rom_field(dart_order_obj * doj, int rom_num, const void *val,
                      int len)
{
    int ret = 1;
    if (doj->positions[rom_num].val_len >= len) {
        doj->positions[rom_num].end = doj->positions[rom_num].beg + len;
        doj->positions[rom_num].val_len = len;
        memcpy((doj->data->buffer + doj->positions[rom_num].beg), val,
               len);
    } else {
        ret = 0;
    }
    return ret;
}
