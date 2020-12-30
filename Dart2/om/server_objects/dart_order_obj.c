/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "dart_order_obj.h"
#include "dart_constants.h"
#include "debug_logger.h"
#include "gatekeeper.h"
#include "dart_allocator.h"
#include "order_obj_fact.h"
#include "async_server.h"

static struct iovec *def_pos = 0;
static int is_init = 0;

void init_def_pos()
{
    if (!is_init) {
        is_init = 1;
        def_pos = calloc(ROM_LEN, sizeof(struct iovec));
        def_pos[161].iov_base = "\n";
        def_pos[161].iov_len = 1;
        int rom_tag = 0;
        while (rom_tag < 160) {
            ++rom_tag;
            def_pos[rom_tag].iov_base = ",";
            def_pos[rom_tag].iov_len = 1;
            ++rom_tag;
        }
    }
}
void rom_style_reset(dart_order_obj* doj)
{
    init_def_pos();
    memcpy(doj->positions, def_pos, (sizeof(struct iovec) * ROM_LEN));
    doj->in_use = 0;
    doj->ex_mess = 0;
    doj->rep = NULL;
    db_node_t *db = doj->head;
    while (db != NULL) {
        databuf_reset(db->data);
        db = db->next;
    }
    doj->current = doj->head;
    doj->positions[161].iov_base = "\n";
    doj->positions[161].iov_len = 1;
    doj->dest = NULL;
    doj->sender = 0;
    doj->childs = 0;
    doj->num_childs = 0;
}

void copy_dart_order_obj(dart_order_obj * orig, dart_order_obj * cp)
{
    if (databuf_unread(orig->head->data) > DEFAULT_BUFF_SIZE) {
        long buff_len = databuf_unread(orig->head->data);
        int i = 0;
        send_debug_message("POSITION ARRAY %d \n", buff_len);
        for (; i < ROM_LEN; ++i) {
            if (orig->positions[i].iov_len > 0
                && (orig->positions[i].iov_base < orig->head->end_ptr
                    && orig->positions[i].iov_base >
                    orig->head->start_ptr)) {
                int len =
                    orig->positions[i].iov_base - orig->head->start_ptr;
                send_debug_message("{%d,%d}", i, len);
            }
        }
        int ret = 0;
        char *buff = 0;
        DART_ALLOC(buff, 8, buff_len + 1, ret);
        memcpy(buff, orig->head->data->buffer, buff_len);
        write_debug_message(buff, buff_len);
        free(buff);
    }
    memcpy(cp->positions, orig->positions,
           (ROM_LEN * sizeof(struct iovec)));
    memcpy(cp->head->data->buffer, orig->head->data->buffer,
            orig->head->data->buflen);
           //databuf_unread(orig->head->data));
    cp->head->start_ptr = orig->head->start_ptr;
    cp->head->end_ptr = orig->head->end_ptr;
    cp->head->data->wr_ptr += databuf_unread(orig->head->data);
    cp->head->data->rd_ptr = cp->head->data->buffer;
    cp->head->data->buflen = orig->head->data->buflen;
    /* This should be the usual case so lets pop outta here */
    if (orig->head->next == NULL) {
        cp->head->next = NULL;
    } else {
        struct db_node *db = orig->head->next;
        struct db_node *prev = cp->head;
        while (db != NULL) {
            struct db_node *t = get_new_buffer((oofact*)cp->dalloc);
            prev->next = t;
            t->start_ptr = db->start_ptr;
            t->end_ptr = db->end_ptr;
            t->next = db->next;
            if(t->data->buflen < databuf_unread(db->data)) {
                send_debug_message("New buff len: %d less than unread: %d \n",
                        t->data->buflen, databuf_unread(db->data));
            }
            databuf_memcpy(t->data, db->data->buffer,
                           db->data->buflen);
                           //databuf_unread(db->data));
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
                if((offset + doj->positions[i].iov_len) < (db->data->buffer + db->data->buflen)) {
                    databuf_memcpy(d, offset, doj->positions[i].iov_len);
                } else {
                    send_debug_message("Overflow value len: %d offset at: %d field number: %d\n",
                        doj->positions[i].iov_len, len, i);
                }
            }
            db = doj->head;
        }
    }
}

char *get_sender_name_from_copy(dart_order_obj * doj)
{
    struct db_node *db = doj->head;
    if (doj->positions[ROM_SENDER].iov_len != 0) {
        while (db != NULL
               && !(doj->positions[ROM_SENDER].iov_base < db->end_ptr
                    && doj->positions[ROM_SENDER].iov_base >
                    db->start_ptr)) {
            db = db->next;
        }
        if (db == NULL && doj->positions[ROM_SENDER].iov_len > 0) {
            return doj->positions[ROM_SENDER].iov_base;
        } else if (doj->positions[ROM_SENDER].iov_len > 0) {
            int len = doj->positions[ROM_SENDER].iov_base - db->start_ptr;
            char *offset = db->data->buffer + len;
            return offset;
        } else {
            return NULL;
        }
    } else {
        return NULL;
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

static void update_buffer(dart_order_obj * doj)
{
    db_node_t* node = get_new_buffer((oofact*)doj->dalloc);
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
        ret = databuf_memcpy(doj->current->data, val, len);
        if(ret == 0) {
            doj->positions[rom_num].iov_len = len;
            doj->positions[rom_num].iov_base =
                doj->current->data->wr_ptr - len;
        }
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

void set_dom_tag(dart_order_obj * doj, void *g)
{
    int dom_len = 0;
    gatekeeper *gk = (gatekeeper *) g;
    char *dom_str = get_dom_tag(gk, &dom_len);
    databuf_t *itoa_buff = get_databuf(gk);
    databuf_memcpy(itoa_buff,
           doj->positions[ROM_SENDER].iov_base,
           doj->positions[ROM_SENDER].iov_len);
    databuf_memcpy(itoa_buff, dom_str, dom_len);
    size_t v = get_dom_num(gk);
    int len = no_set_unsigned_itoa(itoa_buff->wr_ptr, v);
    itoa_buff->wr_ptr += len;
    int dom_tag_len = databuf_unread(itoa_buff);
    databuf_memcpy(itoa_buff, "-", 1);
    len = no_set_unsigned_itoa(itoa_buff->wr_ptr, doj->ex_mess);
    itoa_buff->wr_ptr += len;
    ++doj->ex_mess;
    int space = databuf_space_left(doj->current->data);
    len = databuf_unread(itoa_buff);
    if (space < (len + 5)) {
        update_buffer(doj);
    }
    databuf_memcpy(doj->current->data, itoa_buff->rd_ptr, len);
    doj->positions[ROM_EXECUTION_TAG].iov_len = len;
    doj->positions[ROM_EXECUTION_TAG].iov_base =
        doj->current->data->wr_ptr - len;
    doj->current->data->wr_ptr += 5;
    reset_rom_field(doj, ROM_TAG, getpval(doj, ROM_EXECUTION_TAG), dom_tag_len);
    destroy_gk_db(gk, itoa_buff);
}

void set_spread_dom_tag(dart_order_obj* doj, void* g)
{
    set_dom_tag(doj, g);
    if(doj->childs != 0) {
        children* lch = doj->childs;
        while(lch != NULL) {
            dart_order_obj* ldoj = (dart_order_obj*)lch->payload;
            set_dom_tag(ldoj, g);
            ldoj->sender = doj->sender;
            ldoj->dest = doj->dest;
            set_rom_field(ldoj,ROM_OM_PARENT, getpval(doj, ROM_TAG),
                          getplen(doj, ROM_TAG));
            set_rom_field(ldoj, ROM_STATUS, "0", 1);
            lch = lch->next;
        }
    } 
}
void set_ex_tag(dart_order_obj* doj, int ex)
{
    doj->ex_mess = ex;
    char itoa_buff[64];
    int len = getplen(doj, ROM_TAG);
    memcpy(itoa_buff, getpval(doj, ROM_TAG), len);
    memcpy(itoa_buff + len, "-", 1);
    len += 1;
    len += no_set_unsigned_itoa(itoa_buff + len, doj->ex_mess);
    ++doj->ex_mess;
    set_rom_field(doj, ROM_EXECUTION_TAG, itoa_buff, len);
}

void increment_ex_tag(dart_order_obj* doj)
{
    if(getplen(doj, ROM_EXECUTION_TAG) > 0) {
        int inital_len = getplen(doj, ROM_TAG) + 1;
        char* offset = getpval(doj, ROM_EXECUTION_TAG);
        offset += inital_len;
        int len = no_set_unsigned_itoa(offset, doj->ex_mess);
        doj->positions[ROM_EXECUTION_TAG].iov_len = (inital_len + len);
        ++doj->ex_mess;
    }
}
char* get_raw_message(dart_order_obj* obj, void* a, int* len)
{
    async_parse_args *apa = (async_parse_args*) a;
    sock_recv_manager* rph = apa->con;
    *len = obj->current->data->buflen;
    return rph->rd_ptr; 
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
