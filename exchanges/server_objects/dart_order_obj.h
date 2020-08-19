/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _DART_ORDER_OBJ_H__
#define _DART_ORDER_OBJ_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "databuf.h"
#include "message_queue.h"

#define ROM_LEN 100

    /**
     * Relocated here for now.
     */
    typedef struct dart_fix_pos {
        long beg;
        long end;
        long val_len;
        char *name;
        int len;
        int rom_tag;
    } dart_fix_pos;

    /**
     * We will pimple this later but for now you can touch the private
     * parts.
     */
    typedef struct dart_order_obj {
        struct dart_fix_pos *positions;
        databuf_t *data;
        void *rep;
    } dart_order_obj;

    dart_order_obj *explicit_doo_create();

    /**
     * This function is needed if these objects are to be used with
     * the dart_obj_factory.
     */
    void create_dart_obj_func(queue_t glob, queue_t q, int num);

    /**
     * Clean function used with the object factory.
     */
    void clean_doo(void *u);

    /**
     * Generic destroy function used with object factory
     */
    void destroy_doo(void *d);

    /**
     * Takes the rom field number and returns the offset to the
     * begining of the field and the length of that field in the len
     * pointer.  If the field does not exist then the return will be
     * null and len will be set to 0.
     */
    char *get_rom_field(dart_order_obj * doj, int rom_num, int *len);

    /**
     * This will append this new value to the end of the databuf and
     * store the offsets in the positions array at rom_num.  It will
     * return 0 if the operation fails.
     */
    int set_rom_field(dart_order_obj * doj, int rom_num, const void *val,
                      int len);

    /**
     * This will change the values saved in the positions array for
     * the rom field (rom_num) to beg and len.
     */
    int reset_rom_field(dart_order_obj * doj, int rom_num, int beg,
                        int len);

    /**
     * This function will check if the new value will "fit" in the
     * space occupied by the old value and if so it will copy the new
     * value into it. Otherwise it will return 0.
     */
    int fill_in_rom_field(dart_order_obj * doj, int rom_num,
                          const void *val, int len);

#if defined(__cplusplus)
}
#endif
#endif                          //_CLIENT_MANAGER_H__
