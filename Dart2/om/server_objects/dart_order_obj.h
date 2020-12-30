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
#include <sys/uio.h>
#include "databuf.h"
#include "message_queue.h"

#define DEFAULT_BUFF_SIZE 1024
#define ROM_LEN 200

#define getplen(rph, pos) (rph->positions[pos].iov_len)
#define getpval(rph, pos) (rph->positions[pos].iov_base)

typedef struct db_node db_node_t;

struct db_node {
    databuf_t *data;
    db_node_t *next;
    void *start_ptr;
    void *end_ptr;
};


typedef struct children children;

struct children {
    void* payload;
    children* next;
};

    /**
     * We will pimple this later but for now you can touch the private
     * parts.
     */
typedef struct dart_order_obj {
    struct iovec *positions;
    short num_childs;
    short ex_mess;
    short in_use;
    short type;
    db_node_t *head;
    db_node_t *current;
    void *rep;
    void *dest;
    void *sender;
    void *dalloc;
    children* childs;
    int lock;
} dart_order_obj;

void rom_style_reset(dart_order_obj* doj);
    /**
     * This function grabs the object lock for doj and 
     * then checks the 'in_use' flag.  If the in_use flag
     * is equal to zero the lock is freed and 0 is returned.
     * Otherwise the lock is held and 1 is returned.
     */
int lock_n_check(dart_order_obj * doj);

int just_lock(dart_order_obj * doj);

int unlock(dart_order_obj * doj);

int check_vals(dart_order_obj * doj, char *sender, int sender_len,
               char *clordid, int ordid_len);
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
int reset_rom_field(dart_order_obj * doj, int rom_num, char *beg, int len);

    /**
     * This function will check if the new value will "fit" in the space occupied by 
     * the old value and if so it will copy the new value into it. Otherwise it will 
     * return 0.
     */
int fill_in_rom_field(dart_order_obj * doj, int rom_num,
                      const void *val, int len);
void set_copied_doo_in_buf(dart_order_obj * doj, databuf_t * d);
void copy_dart_order_obj(dart_order_obj * orig, dart_order_obj * cp);
char *get_sender_name_from_copy(dart_order_obj * doj);
char *get_value_from_copy(dart_order_obj * doj, int rom_id, int *val_len);
long dart_itoa(dart_order_obj * doj, long value, int rom_id, databuf_t* db);
unsigned int unsigned_dart_itoa(dart_order_obj * doj, unsigned int value,
                                int rom_id, databuf_t* db);
unsigned int no_set_unsigned_itoa(char* str, unsigned long value);
void check_and_resize(dart_order_obj * doj, size_t space_needed);
void init_def_pos();

void set_dom_tag(dart_order_obj * doj, void *gk);
void set_spread_dom_tag(dart_order_obj* doj, void* g);
void increment_ex_tag(dart_order_obj* doj);
void set_ex_tag(dart_order_obj* doj, int ex);

/**
 * Returns the raw buffer associated with this order object
 */
char* get_raw_message(dart_order_obj* obj, void* a, int* len);

void add_child_obj(dart_order_obj* parent, dart_order_obj* child);

#if defined(__cplusplus)
}
#endif
#endif
