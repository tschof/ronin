/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "dart_order_obj.h"
#include "rex_allocator.h"
#include "dart_order_fact.h"
#include "rex_constants.h"

int lock_n_check(dart_order_obj * doj)
{
    while (__sync_val_compare_and_swap(&doj->lock, 0, 1) == 1);
    if (doj->in_use) {
        __sync_fetch_and_sub(&doj->lock, 1);
        return 0;
    } else {
        __sync_fetch_and_sub(&doj->lock, 1);
    }
    return 1;
}

int just_lock(dart_order_obj * doj)
{
    while (__sync_val_compare_and_swap(&doj->lock, 0, 1) == 1);
    return 1;
}

int unlock(dart_order_obj * doj)
{
    return __sync_fetch_and_sub(&doj->lock, 1);
}

int check_vals(dart_order_obj * doj, char *sender, int sender_len,
               char *clordid, int ordid_len)
{
    if(sender != 0 && doj != 0 && doj->positions != 0 &&
            getpval(doj,ROM_SENDER) != 0) {
        int is_same =
            strncmp(sender, getpval(doj, ROM_SENDER), getplen(doj, ROM_SENDER));
        is_same +=
            strncmp(clordid, doj->positions[ROM_CLIENT_ID].iov_base,
                    ordid_len);
        if (is_same == 0) {
            return 1;
        } else {
            return 0;
        }
    }
    return 0;
}

/* Takes the rom field number and returns the offset to the begining
 * of the field and the length of that field in the len pointer.  If
 * the field does not exist then the return will be null and len will
 * be set to 0. */
char *get_rom_field(dart_order_obj * doj, int rom_num, int *len)
{
    *len = doj->positions[rom_num].iov_len;
    return doj->positions[rom_num].iov_base;
}

void update_buffer(dart_order_obj * doj)
{
    db_node_t* node = get_new_buffer((dartfact*)doj->dalloc);
    doj->current->next = node;
    node->next = NULL;
    doj->current = node;
}

/* This will append this new value to the end of the databuf and store
 * the offsets in the positions array at rom_num.  It will return 0 if
 * the operation fails. */
int set_rom_field(dart_order_obj * doj, int rom_num, const void *val,
                  int len)
{
    int ret = 1;
    ret = databuf_memcpy(doj->current->data, val, len);
    if (ret == 0) {
        doj->positions[rom_num].iov_len = len;
        doj->positions[rom_num].iov_base =
            doj->current->data->wr_ptr - len;
    } else {
        update_buffer(doj);
        databuf_memcpy(doj->current->data, val, len);
        doj->positions[rom_num].iov_len = len;
        doj->positions[rom_num].iov_base =
            doj->current->data->wr_ptr - len;
    }
    return ret;
}

/* This will change the values saved in the positions array for the
 * rom field (rom_num) to beg and len. */
int reset_rom_field(dart_order_obj * doj, int rom_num, char *beg, int len)
{
    int ret = 1;
    doj->positions[rom_num].iov_len = len;
    doj->positions[rom_num].iov_base = beg;
    return ret;
}

int fill_in_rom_field(dart_order_obj * doj, int rom_num, const void *val,
                      int len)
{
    int ret = 1;
    if (doj->positions[rom_num].iov_len >= len) {
        doj->positions[rom_num].iov_len = len;
        memcpy(doj->positions[rom_num].iov_base, val, len);
    } else {
        set_rom_field(doj, rom_num, val, len);
    }
    return ret;
}

/* This routine will enure that there is enough space in the databuff
 * to write space_needed bytes.  If there is not enough space in the
 * curent buffer a new buffer will be added and it will be set to
 * current. */
void check_and_resize(dart_order_obj * doj, size_t space_needed)
{
    if (databuf_space_left(doj->current->data) >= space_needed) {
    } else {
        update_buffer(doj);
    }
}

int length_of_a(long value)
{
    size_t n = 0;
    long v = value;
    do {
        ++n;
        v /= 10;
    } while (v);
    return n;
}

uint64_t u64_dart_itoa(dart_order_obj * doj, uint64_t value, int rom_tag, databuf_t* db)
{
    char *str = db->buffer;
    int radix = 10;
    static char dig[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    uint64_t n = 0, neg = 0;
    unsigned long v = 0;
    char *p, *q;
    char c;
    if (radix == 10 && value < 0) {
        value = -value;
        neg = 1;
    }
    v = value;
    do {
        str[n++] = dig[v % radix];
        v /= radix;
    } while (v);
    if (neg) {
        str[n++] = '-';
    }
    for (p = str, q = p + (n - 1); p < q; ++p, --q) {
        c = *p;
        *p = *q;
        *q = c;
    }
    fill_in_rom_field(doj, rom_tag, db->buffer,n);
    return n;
}
long dart_itoa(dart_order_obj * doj, long value, int rom_tag, databuf_t* db)
{
    char *str = db->buffer;
    int radix = 10;
    static char dig[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    long n = 0, neg = 0;
    unsigned long v = 0;
    char *p, *q;
    char c;
    if (radix == 10 && value < 0) {
        value = -value;
        neg = 1;
    }
    v = value;
    do {
        str[n++] = dig[v % radix];
        v /= radix;
    } while (v);
    if (neg) {
        str[n++] = '-';
    }
    for (p = str, q = p + (n - 1); p < q; ++p, --q) {
        c = *p;
        *p = *q;
        *q = c;
    }
    fill_in_rom_field(doj, rom_tag, db->buffer,n);
    return n;
}

int32_t dart_signed_itoa(dart_order_obj* doj, int32_t value, int rom_tag, databuf_t* db)
{
    char *str = db->buffer;
    int radix = 10;
    static char dig[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    int32_t n = 0, neg = 0;
    int32_t v = 0;
    char *p, *q;
    char c;
    if (radix == 10 && value < 0) {
        value = -value;
        neg = 1;
    }
    v = value;
    do {
        str[n++] = dig[v % radix];
        v /= radix;
    } while (v);
    if (neg) {
        str[n++] = '-';
    }
    for (p = str, q = p + (n - 1); p < q; ++p, --q) {
        c = *p;
        *p = *q;
        *q = c;
    }
    fill_in_rom_field(doj, rom_tag, db->buffer,n);
    return n;
}

unsigned int unsigned_dart_itoa(dart_order_obj * doj, unsigned int value,
                                int rom_tag,  databuf_t* db)
{
    char *str = db->buffer;
    int radix = 10;
    static char dig[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    unsigned int n = 0;
    unsigned long v = 0;
    char *p, *q;
    char c;
    v = value;
    do {
        str[n++] = dig[v % radix];
        v /= radix;
    } while (v);
    for (p = str, q = p + (n - 1); p < q; ++p, --q) {
        c = *p;
        *p = *q;
        *q = c;
    }
    fill_in_rom_field(doj, rom_tag, db->buffer,n);
    return n;
}

unsigned int no_set_unsigned_itoa(char* str, unsigned long value)
{
    int radix = 10;
    static char dig[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    long n = 0;
    unsigned long v = 0;
    char *p, *q;
    char c;
    v = value;
    do {
        str[n++] = dig[v % radix];
        v /= radix;
    } while (v);
    for (p = str, q = p + (n - 1); p < q; ++p, --q) {
        c = *p;
        *p = *q;
        *q = c;
    }
    return n;
}
void add_child_obj(dart_order_obj* parent, dart_order_obj* child)
{
    children* lch = calloc(1, sizeof(struct children));
    lch->payload = child;
    lch->next = 0;
    if(parent->childs == 0) {
       parent->childs = lch; 
    } else {
        children* front = parent->childs;
        while (front->next != NULL) {
            front = front->next;
        }
        front->next = lch;
    }
    parent->num_childs++;
}
void copy_dart_order_obj(dart_order_obj * orig, dart_order_obj * cp)
{
    memcpy(cp->positions, orig->positions,
           (200 * sizeof(struct iovec)));
    memcpy(cp->head->data->buffer, orig->head->data->buffer,
            orig->head->data->buflen);
    cp->head->start_ptr = orig->head->start_ptr;
    cp->head->end_ptr = orig->head->end_ptr;
    cp->head->data->wr_ptr += databuf_unread(orig->head->data);
    cp->head->data->rd_ptr = cp->head->data->buffer;
    cp->head->data->buflen = orig->head->data->buflen;
    if (orig->head->next == NULL) {
        cp->head->next = NULL;
    } else {
        struct db_node *db = orig->head->next;
        struct db_node *prev = cp->head;
        while (db != NULL) {
            struct db_node *t = get_new_buffer((dartfact*)cp->dalloc);
            prev->next = t;
            t->start_ptr = db->start_ptr;
            t->end_ptr = db->end_ptr;
            t->next = db->next;
            databuf_memcpy(t->data, db->data->buffer,
                           db->data->buflen);
            db = db->next;
            prev = t;
        }
    }
}

void set_copied_doo_in_buf(dart_order_obj * doj, databuf_t * d)
{
    struct db_node *db = doj->head;
    int i = 0;
    for (; i < 162; ++i) {
        if (doj->positions[i].iov_len != 0) {
            while (db != NULL
                   && !(doj->positions[i].iov_base < db->end_ptr
                        && doj->positions[i].iov_base >= db->start_ptr)) {
                db = db->next;
            }
            if (db == NULL) {
                databuf_memcpy(d, doj->positions[i].iov_base,
                               doj->positions[i].iov_len);
            } else {
                    int len = doj->positions[i].iov_base - db->start_ptr;
                    char *offset = db->data->buffer + len;
                    databuf_memcpy(d, offset, doj->positions[i].iov_len);
            }
            db = doj->head;
        }
    }
}

char *get_value_from_copy(dart_order_obj * doj, int rom_id, int *ret_len)
{
    struct db_node *db = doj->head;
    if (doj->positions[rom_id].iov_len != 0) {
        while (db != NULL
               && !(doj->positions[rom_id].iov_base < db->end_ptr
                    && doj->positions[rom_id].iov_base > db->start_ptr)) {
            db = db->next;
        }
        if (db == NULL && doj->positions[rom_id].iov_len > 0) {
            *ret_len = doj->positions[rom_id].iov_len;
            return doj->positions[rom_id].iov_base;
        } else if (doj->positions[rom_id].iov_len > 0) {
            int len = doj->positions[rom_id].iov_base - db->start_ptr;
            char *offset = db->data->buffer + len;
            *ret_len = doj->positions[rom_id].iov_len;
            return offset;
        } else {
            *ret_len = 0;
            return NULL;
        }
    } else {
        *ret_len = 0;
        return NULL;
    }
}
