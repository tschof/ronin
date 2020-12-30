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
#include "risk_cntr_structs.h"

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
    rcntr risk;
} dart_order_obj;

    /**
     * This function grabs the object lock for doj and 
     * then checks the 'in_use' flag.  If the in_use flag
     * is equal to zero the lock is freed and 0 is returned.
     * Otherwise the lock is held and 1 is returned.
     */
int lock_n_check(dart_order_obj * doj);

int just_lock(dart_order_obj * doj);

int unlock(dart_order_obj * doj);

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
long dart_itoa(dart_order_obj * doj, long value, int rom_id, databuf_t* db);
uint64_t u64_dart_itoa(dart_order_obj * doj, uint64_t value, int rom_tag,
                       databuf_t* db);
unsigned int unsigned_dart_itoa(dart_order_obj * doj, unsigned int value,
                                int rom_id, databuf_t* db);
unsigned int no_set_unsigned_itoa(char* str, unsigned long value);
void check_and_resize(dart_order_obj * doj, size_t space_needed);

int check_vals(dart_order_obj * doj, char *sender, int sender_len,
               char *clordid, int ordid_len);

void add_child_obj(dart_order_obj* parent, dart_order_obj* child);

int32_t dart_signed_itoa(dart_order_obj* doj, int32_t value, int rom_tag, databuf_t* db);
void set_copied_doo_in_buf(dart_order_obj * doj, databuf_t * d);
void copy_dart_order_obj(dart_order_obj * orig, dart_order_obj * cp);
char *get_value_from_copy(dart_order_obj * doj, int rom_id, int *ret_len);
void update_buffer(dart_order_obj * doj);
#if defined(__cplusplus)
}
#endif
#endif
