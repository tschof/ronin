/**************************************************************************
 * Copyright (c) 2008 by DART Executions, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>

#include "fix_trans.h"
#include "hashmap.h"
#include "parser_func.h"
#include "dart_constants.h"
#include "databuf.h"
#include "tm_to_str.h"
#include "rb_tree.h"
#include "sti_tree.h"
#include "dart_allocator.h"
#include "order_obj_fact.h"
#include "debug_logger.h"


static char dig[] = "0123456789abcdefghijklmnopqrstuvwxyz";
#define NUM_ORDS 8
#define RADIX 10
#define CHAR_BIT 8
void t_destroy(void *a)
{
    free((short *) a);
}

int t_compare(const void *a, const void *b)
{
    if (*(short *) a > *(short *) b) {
        return 1;
    }
    if (*(short *) a < *(short *) b) {
        return -1;
    }
    return 0;
}

void ft_print(const void *a)
{
    printf("%d", *(int *) a);
}

void ft_print_val(const void *a)
{
    printf("Printing the object");
}

/**
 * This saves the length of the fix tag
 */
typedef struct dart_fix_pos {
    char *name;
    int len;
    unsigned long val_start;
    int vlen;
} dart_fix_pos;

struct out_fix_parser {
    sti_tree_t* map;
    char* raw;
    dart_fix_pos* pos;
    databuf_t* buff;
    int header_len;
    int type;
};

void t_info_destroy(void *a)
{
    printf("t destroy called. \n");
    dallocator* d = (dallocator*)a;
    int i = 0;
    for(; i< NUM_ORDS;++i) {
        ofp* pa = (ofp*)get_stuff(d);
        pa->map = 0;
        return_stuff(d, pa);
    }
    destroy_mem_glob(d);
}
void i_info_destroy(void *a)
{
    printf("t destroy called. \n");
    hashmap_t pa = (hashmap_t)a;
    delete_map(pa);
}
struct fix_translator {
    rb_tree_t* out_types;
    rb_tree_t* in_types;
    int* field;
};

static char* get_file_contents(const char* filename, long* size)
{
    FILE* mfile = 0;
    long res = 0;
    char* buff = 0;
    int ret = 0;
    mfile = fopen(filename, "r");
    if(mfile != 0) {
        fseek(mfile, 0, SEEK_END);
        *size = ftell(mfile);
        rewind(mfile);
        DART_ALLOC(buff, 8, *size + 1, ret);
        res = fread(buff, 1, *size, mfile);
        if(res != *size) {
            free(buff);
            buff = 0;
        }
        fclose(mfile);
    } 
    return buff;
}

static void build_map_from_file(const char* filename, hashmap_t map)
{
    char* buffer = 0;
    long size = 0;
    buffer = get_file_contents(filename, &size);
    if(size > 0) {
        char* start = buffer;
        char* comma = buffer;
        char* end = buffer;
        int tag_len = 0;
        int val = 0;
        do {
            if(*end == ',') {
                tag_len = end - start;
                comma = end;
            } else if(*end == '\n') {
                val = atoi(comma + 1);
                insert_map(map, start, tag_len, &val, sizeof(int));
                start = end + 1;
            }
            ++end;
            --size;
        } while(size > 0);
    }
}

static void build_fix_pos_array(struct dart_fix_pos* dfp,
        char* buff, long size,
        sti_tree_t* tree)
{
    if(size > 0) {
        char* start = buff;
        char* d_off = buff;
        int is_def_val = 0;
        int skip = 0;
        int i = 0;
        do {
            if(*d_off == ',' || *d_off == '^' || *d_off == '\n') {
                if(!is_def_val && !skip) {
                    dfp[i].name = buff;
                    dfp[i].len = d_off - buff;
                    int itag = atoi(buff);
                    int ival = i; 
                    sti_tree_insert(tree, itag, ival);
                }
                if(*d_off == ',' && is_def_val) {
                    goto increment;
                }
                if(*d_off == ',') {
                    is_def_val = 1;
                } else if(*d_off == '^') {
                    dfp[i].val_start = 0;
                    dfp[i].vlen = atoi(d_off + 1);
                    skip = 1;
                } else if(*d_off == '\n') {
                    if(is_def_val) {
                        dfp[i].val_start = buff - start;
                        dfp[i].vlen = d_off - buff;
                        ++i;
                    } else if(skip) {
                        skip = 0;
                        ++i; 
                    } else {
                        dfp[i].val_start = 0;
                        dfp[i].vlen = 0;
                        ++i;
                    }
                    is_def_val = 0;
                }
                buff = d_off + 1;
            }
increment:
            ++d_off;
            --size;
        } while(size > 0);
    }
}

void reset_object(trans_t* t, ofp* doj)
{
    dallocator* p = rb_tree_get_val(t->out_types, &doj->type);
    if(p) {
        doj->buff->wr_ptr = doj->buff->rd_ptr + doj->header_len;
        int offset = 9;
        int off = sti_tree_get_val(doj->map, offset);
        if(off > 0) {
            memset(doj->buff->rd_ptr + doj->pos[off].val_start,
                    '0', doj->pos[off].vlen);
        }
        offset = 34;
        off = sti_tree_get_val(doj->map, offset);
        if(off > 0) {
            memset(doj->buff->rd_ptr + doj->pos[off].val_start,
                    '0', doj->pos[off].vlen);
        }
    }
}

static short get_type(sti_tree_t* map, struct dart_fix_pos* dfp, 
                      char* buff)
{
    int offset = 35;
    unsigned short stype = 0;
    int off = sti_tree_get_val(map, offset);
    if(off > 0) {
        char* type = buff + dfp[off].val_start;
        if(dfp[off].vlen > 1) {
            stype = ((type[1] << CHAR_BIT) + type[0]);
        } else {
            stype = ((0x0 << CHAR_BIT) +  type[0]);
        }
    }
    return stype;
}

static short build_order_obj(ofp* out, char* buff, 
        long size, int num_headers)
{
    int i =0;
    build_fix_pos_array(out->pos,buff, size,out->map);
    out->type = get_type(out->map, out->pos, buff);

    for(; i < num_headers; ++i) {
        databuf_memcpy(out->buff, out->pos[i].name, out->pos[i].len);
        if(out->pos[i].val_start == 0) {
            memset(out->buff->wr_ptr, '0', out->pos[i].vlen);
        } else {
            memcpy(out->buff->wr_ptr, buff + out->pos[i].val_start, out->pos[i].vlen);
        }
        out->pos[i].val_start = out->buff->wr_ptr - out->buff->rd_ptr;
        out->buff->wr_ptr += out->pos[i].vlen;
        *out->buff->wr_ptr++ = '\x01';
    }
    out->header_len = databuf_unread(out->buff);
    return out->type;
}

static char*  read_n_set_outgoing(const char* filename, 
        int* fields, long* ret_size)
{
    char* buff = get_file_contents(filename, ret_size);
    long size = *ret_size;
    char* off = buff;
    do {
        if(*off == '\n') {
            *fields += 1;
        } 
        ++off;
        --size;
    } while(size);
    return buff;
}
static void clear_pg(void* v)
{
    memset(v, '\0', 4096);
}
/**
 *
 */
static void create_fix_outgoing_trans(trans_t* t, char* filename)
{
    dallocator* objs = create_mem_glob(NUM_ORDS * 4096, clear_pg);
    long size = 0;
    short* type = calloc(1, sizeof(short));
    int num_tags = 0;
    char* buff = read_n_set_outgoing(filename, &num_tags, &size);
    int i = 0;
    ofp* out_array[NUM_ORDS];
    for(; i< NUM_ORDS; ++i) {
        void* mem = get_stuff(objs);
        ofp* out = mem;
        out->header_len = 0;
        size_t map_size = 24 + ((num_tags + 2) * 40);
        int pos_len = (num_tags * sizeof(struct dart_fix_pos));
        out->pos = (mem + sizeof(ofp));
        out->raw = mem + sizeof(ofp) + pos_len;
        memcpy(out->raw, buff, size);
        out->map = 
            create_sti_tree((mem + sizeof(ofp) + pos_len + size), map_size);
        out->buff = mem + sizeof(ofp) + pos_len + size + map_size;
        out->buff->buffer = mem + sizeof(ofp) + pos_len 
            + size + map_size + sizeof(databuf_t);
        out->buff->rd_ptr = out->buff->wr_ptr = out->buff->buffer;
        out->buff->buflen = 4096 - (sizeof(ofp) + pos_len + 
                size + map_size + sizeof(databuf_t));
        out->buff->flags = 0x2;
        *type = build_order_obj(out, out->raw, size,num_tags);
        out_array[i] = out;
    }
    for(i = 0; i < NUM_ORDS; ++i) {
        return_stuff(objs, out_array[i]);
    }
    rb_tree_insert(t->out_types, type, objs);
}
static void create_fix_incoming_trans(trans_t* t, char* filename, short type)
{
    hashmap_t l = create_map(64);
    build_map_from_file(filename, l);
    short* itype = calloc(1, sizeof(short));
    *itype = type;
    rb_tree_insert(t->in_types, itype, l);
}

trans_t* create_translator(ff_desc* out_types, int out_len,
        ff_desc* in_types, int in_len)
{
    trans_t* t = calloc(1, sizeof(struct fix_translator));
    t->out_types = create_rb_tree(t_compare, t_destroy,
            t_info_destroy, ft_print,
            ft_print);
    t->in_types = create_rb_tree(t_compare, t_destroy,
            i_info_destroy, ft_print,
            ft_print);
    t->field = calloc(1, sizeof(int));
    do {
        create_fix_outgoing_trans(t, out_types[--out_len].filename);
    } while(out_len);
    do {
        --in_len;
        create_fix_incoming_trans(t, in_types[in_len].filename,
                in_types[in_len].fix_type);
    } while(in_len);
    return t;
}
void translate_fix_msg(trans_t* t, char* fix_msg, int len, 
        dart_order_obj* obj, short type)
{
    char* tag = fix_msg;
    char* val = fix_msg;
    char* end = fix_msg;
    hashmap_t transy = 0;
    transy = rb_tree_get_val(t->in_types, &type);
    if(transy) {
        do {
            if(*end == '=') {
                val = end;
                NEXT_ONE(end, len);
                if(find_n_fill(transy, tag, (val - tag),
                            (void **)&t->field) > 0) {
                    ++val;
                    reset_rom_field(obj, *t->field, val, end - val);
		    //send_debug_message("Tag len: %d\n", end - val);
                }
                tag = end + 1;
            }
            ++end;
            --len;
        } while(len > 0);
    } else {
        send_debug_message("No translation for type:%d\n", type);
    }
}

ofp* get_fixy_message(trans_t* t, short type)
{
    ofp* doj = 0;
    dallocator* p = rb_tree_get_val(t->out_types, &type);
    if(p) {
        doj = get_stuff(p);
    }
    return doj;
}
void destroy_fixy_message(trans_t* t, ofp* doj)
{
    dallocator* p = rb_tree_get_val(t->out_types, &doj->type);
    if(p) {
        reset_object(t, doj);
        return_stuff(p, doj);
    }
}
static unsigned int local_unsigned_itoa(char* str, unsigned long value)
{
    long n = 0;
    unsigned long v = 0;
    char *p, *q;
    char c;
    v = value;
    do {
        str[n++] = dig[v % RADIX];
        v /= RADIX;
    } while (v);
    for (p = str, q = p + (n - 1); p < q; ++p, --q) {
        c = *p;
        *p = *q;
        *q = c;
    }
    return n;
}

void set_fix_val(trans_t*t, ofp* obj, int offset, char* val, int val_len)
{
    unsigned int len = local_unsigned_itoa(obj->buff->wr_ptr, offset);
    obj->buff->wr_ptr += len;
    *obj->buff->wr_ptr++ = '=';
    databuf_memcpy(obj->buff, val, val_len);
    *obj->buff->wr_ptr++ = '\x01';
}
void set_char_fix_val(trans_t*t, ofp* obj, char* fix_tag, int f_len, char* val, int val_len)
{
    databuf_memcpy(obj->buff, fix_tag, f_len);
    *obj->buff->wr_ptr++ = '=';
    databuf_memcpy(obj->buff, val, val_len);
    *obj->buff->wr_ptr++ = '\x01';
}

void add_single_char_val(trans_t* t, ofp* obj, char* fix_tag, int f_len, char val)
{
    databuf_memcpy(obj->buff, fix_tag, f_len);
    *obj->buff->wr_ptr++ = '=';
    *obj->buff->wr_ptr++ = val;
    *obj->buff->wr_ptr++ = '\x01';
}

int overwrite_fix_val(trans_t*t, ofp* p, int offset, char* val, int val_len,
                      int indent)
{
    int ret_val = 0;
    int off = sti_tree_get_val(p->map, offset);
    if(off >= 0) {
        if(p->pos[off].vlen >= (indent +val_len)) {
            ret_val = 1;
            memcpy((p->buff->rd_ptr + p->pos[off].val_start) + indent, val, val_len);
        } else {
            ret_val = -1;
        }
    } else {
        set_fix_val(t,p, offset, val, val_len);
    }
    return ret_val;
}
void set_tag_nine(trans_t* t, ofp* obj)
{
    char* off = (obj->buff->rd_ptr + obj->pos[1].val_start) + (obj->pos[1].vlen);
    unsigned int value = obj->buff->wr_ptr - off;
    value -= 1;
    long n = 0;
    unsigned long v = 0;
    char str[64];
    char *p;
    v = value;
    do {
        str[n++] = dig[v % RADIX];
        v /= RADIX;
    } while (v);
    p = ((obj->buff->rd_ptr + obj->pos[1].val_start) + (obj->pos[1].vlen - n));
    do {
        *p++ = str[n - 1];
        --n;
    } while(n);
}
static void set_the_value(ofp* p, int tag, databuf_t* buff)
{
    int off = sti_tree_get_val(p->map, tag);
    if(off >= 0) {
        databuf_memcpy(buff, (p->buff->rd_ptr + p->pos[off].val_start),
                p->pos[off].vlen);
    } 
}
void set_cannonical_str(trans_t* t, ofp* message, databuf_t* cmessage)
{
    set_the_value(message, 34, cmessage);
    databuf_memcpy(cmessage, "\n", 1);
    set_the_value(message, 49, cmessage);
    databuf_memcpy(cmessage, "\n", 1);
    set_the_value(message, 50, cmessage);
    databuf_memcpy(cmessage, "\n", 1);
    set_the_value(message, 52, cmessage);
    databuf_memcpy(cmessage, "\n", 1);
    set_the_value(message, 57, cmessage);
    databuf_memcpy(cmessage, "\n", 1);
    set_the_value(message, 108, cmessage);
    databuf_memcpy(cmessage, "\n", 1);
    set_the_value(message, 142, cmessage);
    databuf_memcpy(cmessage, "\n", 1);
    set_the_value(message, 369, cmessage);
    databuf_memcpy(cmessage, "\n", 1);
    set_the_value(message, 1603, cmessage);
    databuf_memcpy(cmessage, "\n", 1);
    set_the_value(message, 1604, cmessage);
    databuf_memcpy(cmessage, "\n", 1);
    set_the_value(message, 1605, cmessage);
}
void set_time_tag(trans_t* t, ofp* p, 
        struct tm *timeval, int offset)
{
    int off = sti_tree_get_val(p->map, offset);
    if(off > 0) {
#ifdef _MICRO_STAMPS_
        tm_to_str_micro(p->buff->rd_ptr + p->pos[off].val_start, p->pos[off].vlen);
#else
        struct timeb rtime;
        ftime(&rtime);
        tm_to_str_millis(timeval, rtime.millitm,
                         p->buff->rd_ptr + p->pos[off].val_start,
                         p->pos[off].vlen);
#endif
    } else {
        unsigned int len = local_unsigned_itoa(p->buff->wr_ptr, offset);
        p->buff->wr_ptr += len;
        *p->buff->wr_ptr++ = '=';
#ifdef _MICRO_STAMPS_
        len =  tm_to_str_micro(p->buff->wr_ptr, 24);
        p->buff->wr_ptr += 24;
#else
        struct timeb rtime;
        ftime(&rtime);
        tm_to_str_millis(timeval, rtime.millitm, p->buff->wr_ptr,
                21);
        p->buff->wr_ptr += 21;
#endif
        *p->buff->wr_ptr++ = '\x01';
    } 
}

void add_time_tag(trans_t* t, ofp* p,
                  struct tm* timeval, int offset)
{
    struct timeb rtime;
    ftime(&rtime);
    unsigned int len = local_unsigned_itoa(p->buff->wr_ptr, offset);
    p->buff->wr_ptr += len;
    *p->buff->wr_ptr++ = '=';
#ifdef _MICRO_STAMPS_
        len =  tm_to_str_micro(p->buff->wr_ptr, 24);
        p->buff->wr_ptr += 24;
#else
    tm_to_str_millis(timeval, rtime.millitm, p->buff->wr_ptr,
            21);
    p->buff->wr_ptr += 21;
#endif
    *p->buff->wr_ptr++ = '\x01';
}
static int calc_check_sum(const char* begin, int len)
{
    int sum = 0;
    int i;
    const char *c;
    c = begin;
    for (i = 0; i < len; ++i) {
        sum = (sum + *c);
        ++c;
    }
    sum = sum % 256;
    return sum;
}
void set_check_sum(trans_t* t, ofp* obj)
{
    int sum = calc_check_sum(obj->buff->rd_ptr, databuf_unread(obj->buff));
    unsigned int len = local_unsigned_itoa(obj->buff->wr_ptr, 10);
    obj->buff->wr_ptr += len;
    *obj->buff->wr_ptr++ = '=';
    databuf_memcpy(obj->buff, "000", 3);
    long n = 0;
    unsigned long v = 0;
    char str[4];
    char *p;
    v = sum;
    do {
        str[n++] = dig[v % RADIX];
        v /= RADIX;
    } while (v);
    p = obj->buff->wr_ptr - n;
    do {
        *p++ = str[n - 1];
        --n;
    } while(n);
    *obj->buff->wr_ptr++ = '\x01';
}
void set_num_val(trans_t* t, ofp* obj, int fix_tag, unsigned int value)
{
    int off = sti_tree_get_val(obj->map, fix_tag);
    if(off > 0) {
        long n = 0;
        unsigned long v = 0;
        char str[64];
        char *p;
        v = value;
        do {
            str[n++] = dig[v % RADIX];
            v /= RADIX;
        } while (v);
        p = ((obj->buff->rd_ptr + obj->pos[off].val_start) + (obj->pos[off].vlen - n));
        do {
            *p++ = str[n - 1 ];
            --n;
        } while(n);
    } else {
        unsigned int len = local_unsigned_itoa(obj->buff->wr_ptr, fix_tag);
        obj->buff->wr_ptr += len;
        *obj->buff->wr_ptr++ = '=';
        len = local_unsigned_itoa(obj->buff->wr_ptr, value);
        obj->buff->wr_ptr += len;
        *obj->buff->wr_ptr++ = '\x01';
    }
}

void add_num_val(trans_t* t, ofp* obj, int fix_tag, unsigned int value)
{
    unsigned int len = local_unsigned_itoa(obj->buff->wr_ptr, fix_tag);
    obj->buff->wr_ptr += len;
    *obj->buff->wr_ptr++ = '=';
    len = local_unsigned_itoa(obj->buff->wr_ptr, value);
    obj->buff->wr_ptr += len;
    *obj->buff->wr_ptr++ = '\x01';
}

void add_value(trans_t* t, ofp* obj, char* fix_tag, int len, unsigned int val)
{
    databuf_memcpy(obj->buff, fix_tag, len);
    *obj->buff->wr_ptr++ = '=';
    len = local_unsigned_itoa(obj->buff->wr_ptr, val);
    obj->buff->wr_ptr += len;
    *obj->buff->wr_ptr++ = '\x01';
}

void safe_num_val(trans_t* t, ofp* obj, int fix_tag, unsigned int value)
{
    databuf_write(obj->buff, "%d=%ld%c", fix_tag, value, '\x01');
}
void safe_val_add(trans_t* t, ofp* obj, const char* fix_tag, int flen,
                  char* val, int len)
{
    databuf_memcpy(obj->buff, fix_tag, flen);
    databuf_safe_inc(obj->buff, '=');
    databuf_memcpy(obj->buff, val, len);
    databuf_safe_inc(obj->buff, '\x01');
}
databuf_t* get_message(ofp* o)
{
    return o->buff;
}
