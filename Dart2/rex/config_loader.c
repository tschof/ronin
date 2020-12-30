/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "config_loader.h"
#include "parser_func.h"
#include "hashmap.h"

struct config_loader {
    hashmap_t header_map;
    pthread_spinlock_t lock;
    char *filename;
};


int lock_config_loader(rex_config* doj)
{
    return pthread_spin_lock(&doj->lock);
}
int unlock_config_loader(rex_config * doj)
{
    return pthread_spin_unlock(&doj->lock);
}
char *get_file_contents(const char *filename, long *size)
{
    FILE *map_file;
    long result;
    char *buff;
    buff = NULL;
    *size = -1;
    map_file = NULL;
    map_file = fopen(filename, "r+b");
    if (map_file != NULL) {
        fseek(map_file, 0, SEEK_END);
        *size = ftell(map_file);
        rewind(map_file);
        buff = calloc(*size + 1, 1);
        if (buff != NULL) {
            result = fread(buff, 1, *size, map_file);
            if (result == *size) {
                //do nothing
            } else {
                printf("File read error on this file: %s \n", filename);
                *size = -1;
                free(buff);
                buff = NULL;
            }
        }
        fclose(map_file);
    }
    return buff;
}

static void add_to_map(hashmap_t map, char *buffer, long len)
{
    char comma[2] = { '=', '\0' };
    long c_offset = 0;
    c_offset = find_offset(buffer, len, comma, 1);
    if (c_offset > 0) {
        char *tag;
        char *val;
        char *val_off;
        long val_len;
        tag = calloc(c_offset + 1, 1);
        if (tag != NULL) {
            memcpy(tag, buffer, c_offset);

            val_off = buffer;
            val_off += c_offset + 1;
            val_len = len - (c_offset + 1);
            val = calloc(val_len, 1);
            if (val != NULL) {
                memcpy(val, val_off, val_len);
                insert_map(map, tag, c_offset, val, val_len);
                free(val);
            }
            free(tag);
        }
    }
}

static hashmap_t create_map_for_header(hashmap_t mmap, char *d_off,
                                       long end_pos)
{
    char e_bracket[2] = { ']', '\0' };
    char *name = NULL;
    long end;
    char *l_off = d_off;
    hashmap_t l_map = create_map(8);
    l_off += 1;
    end = find_offset(l_off, (end_pos - 1), e_bracket, 1);
    if (end < 0) {
        end = end_pos - 1;
    }
    name = calloc(end + 1, 1);
    if (name != NULL) {
        memcpy(name, l_off, end);
        no_copy_insert(mmap, name, end, l_map);
        free(name);
    }
    return l_map;
}

int build_map_from_file(const char *filename, hashmap_t map)
{
    int result = -1;
    long size = 0;
    char *buffer = get_file_contents(filename, &size);
    if (size > 0) {
        long cur_pos = 0;
        long eol_len;
        long end_pos = 0;
        int should_parse = 1;
        char *d_off = buffer;
        hashmap_t l_map = NULL;
        char eol[2] = { '\n', '\0' };
        eol_len = 1;
        result = 0;

        while (cur_pos < size && should_parse) {
            end_pos = find_offset(d_off, (size - cur_pos), eol, eol_len);
            if (end_pos > 0) {
                long bracket_off = 0;
                bracket_off = find_offset(d_off, (end_pos - 1), "[", 1);
                if (bracket_off >= 0) {
                    char *h_off = d_off;
                    h_off += bracket_off;
                    l_map = create_map_for_header(map, h_off, end_pos);
                } else if (l_map) {
                    add_to_map(l_map, d_off, end_pos);
                }
                d_off += end_pos + eol_len;
                cur_pos += end_pos + eol_len;
            } else if (end_pos == 0 && (cur_pos + 1) < size) {
                d_off += 1;
                cur_pos += 1;
            } else {
                should_parse = 0;
            }
        }
        free(buffer);
    }
    return result;
}

static void clean_sub_header_map(hashmap_t h_map)
{
    char*  rlf;
    char *key = 0;
    int key_len = 0;
    hashmap_iterator_t hi = hashmap_iterator_begin(h_map);
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                                     (void *) &rlf);
    while (valid) {
        free(rlf);
        rlf = 0;
        valid = hashmap_iterator_next(h_map, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &rlf);
    }
    hashmap_iterator_destroy(hi);
}
static void clean_header_map(hashmap_t h_map)
{
    hashmap_t  rlf;
    char *key = 0;
    int key_len = 0;
    hashmap_iterator_t hi = hashmap_iterator_begin(h_map);
    int valid =
        hashmap_iterator_dereference(hi, &key, &key_len,
                                     (void *) &rlf);
    while (valid) {
        clean_sub_header_map(rlf);
        delete_map(rlf);
        rlf = 0;
        valid = hashmap_iterator_next(h_map, hi);
        if (!valid) {
            break;
        }
        valid =
            hashmap_iterator_dereference(hi, &key,
                                         &key_len, (void *) &rlf);
    }
    hashmap_iterator_destroy(hi);
}

void rex_destroy_config(rex_config* df)
{
    clean_header_map(df->header_map);
    delete_map(df->header_map);
    if(df->filename) {
        free(df->filename);
    }
    free(df);
}

void rex_reset_maps(rex_config * df)
{
    clean_header_map(df->header_map);
    delete_map(df->header_map);
    df->header_map = create_map(32);
    build_map_from_file(df->filename, df->header_map);
}

rex_config *initialize_config(const char *filename, int len)
{
    rex_config *df = calloc(sizeof(struct config_loader), 1);
    df->filename = calloc(len + 1, 1);
    memcpy(df->filename, filename, len);
    df->header_map = create_map(32);
    pthread_spin_init(&df->lock, PTHREAD_PROCESS_PRIVATE);
    build_map_from_file(df->filename, df->header_map);
    return df;
}

char *get_val_for_tag(rex_config * df, const char *section, int sec_len,
                      const char *tag, int tag_len, int *val_len)
{
    char *temp;
    int ret_val;
    char *data;
    hashmap_t l_map;
    temp = NULL;
    ret_val =
        get_map_entry(df->header_map, section, sec_len, (void *) &l_map);
    if (ret_val <= 0) {
        *val_len = 0;
        return NULL;
    }
    ret_val = get_map_entry(l_map, tag, tag_len, (void *) &temp);
    if (ret_val <= 0) {
        *val_len = 0;
        return NULL;
    }
    data = calloc(ret_val + 1, 1);
    if (data != NULL) {
        memcpy(data, temp, ret_val);
        *val_len = ret_val;
    } else {
        *val_len = 0;
    }
    return data;
}

char* get_no_copy_val(rex_config* df, const char* section, int sec_len,
                      const char* tag, int tag_len, int* val_len)
{
    char* data = 0;
    hashmap_t l_map = 0;
    *val_len = get_map_entry(df->header_map, section, sec_len,
                                (void*) &l_map);
    if(*val_len <= 0) {
        return 0;
    }
    *val_len = get_map_entry(l_map, tag, tag_len, (void*) &data);
    return data;
}
int cp_to_buff(rex_config* df, const char* section, int sec_len,
                     const char* tag, int tag_len, char* output,
                     int max_len)
{
    char* data = 0;
    hashmap_t l_map = 0;
    int val_len = get_map_entry(df->header_map, section, sec_len,
                                (void*) &l_map);
    if(val_len <= 0) {
        return 0;
    }
    val_len = get_map_entry(l_map, tag, tag_len, (void*) &data);
    if(val_len <= max_len) {
        memcpy(output, data, val_len);
    } else {
        val_len = 0;
    }
    return val_len;
}

static void write_val_to_file(FILE * map_file, long bytes_to_write,
                              char *val)
{
    int written = 0;
    char *val_to_write = val;
    long b_to_w = bytes_to_write;
    while (written < b_to_w) {
        written += fwrite(val_to_write, 1, bytes_to_write, map_file);
        bytes_to_write -= written;
        val_to_write += written;
    }
}

void set_val_for_tag(rex_config * df, const char *section,
                     int sec_len, const char *tag, int tag_len, char *val,
                     int val_len)
{
    char e_bracket = '[';
    FILE *map_file;
    long result;
    char *buff;
    char *tag_off;
    char *sec_off;
    char *next_sec;
    long size = 0;
    char eol[2] = { '\n', '\0' };
    buff = NULL;
    map_file = NULL;
    map_file = fopen(df->filename, "r+b");
    if (map_file != NULL) {
        fseek(map_file, 0, SEEK_END);
        size = ftell(map_file);
        rewind(map_file);
        buff = calloc(size + 1, 1);
        if (buff != NULL) {
            result = fread(buff, 1, size, map_file);
            if (result == size) {
                int keep_looking = 1;
                sec_off = strstr(buff, section);
                while (sec_off != NULL && keep_looking) {
                    if ((sec_off - 1) != NULL && *(sec_off - 1) != '[') {
                        sec_off = strstr((sec_off + sec_len), section);
                    } else {
                        keep_looking = 0;
                    }
                }

                if (sec_off != NULL) {
                    tag_off = strstr(sec_off, tag);
                    while(tag_off) {
                        if((tag_off - 1)[0] == '\n') {
                            char* next_eq = strchr(tag_off, '=');
                            if(next_eq && next_eq - tag_off == tag_len) {
                                break;
                            } else if(next_eq) {
                                tag_off = strstr(tag_off + (next_eq - tag_off), tag);
                            }
                        } else {
                            tag_off = strstr(tag_off + 1, tag);
                        }
                    }
                    next_sec = strchr(sec_off, e_bracket);
                    if ((tag_off != NULL && next_sec == NULL) ||
                        (next_sec != NULL && tag_off != NULL &&
                         ((next_sec - sec_off) > (tag_off - sec_off)))) {
                        int offset;
                        int written;
                        char *val_to_write = val;
                        int bytes_to_write = val_len;
                        char *old_eol = 0;
                        tag_off += tag_len + 1;
                        offset = tag_off - buff;
                        fseek(map_file, offset, SEEK_SET);
                        written = 0;
                        while (written < val_len) {
                            written +=
                                fwrite(val_to_write, 1, bytes_to_write,
                                       map_file);
                            bytes_to_write -= written;
                            val_to_write += written;
                        }
                        old_eol = strstr(tag_off, eol);
                        if (old_eol != NULL) {
                            long b_to_w = size - ((old_eol) - buff);
                            fseek(map_file, (offset + val_len), SEEK_SET);
                            write_val_to_file(map_file, b_to_w, old_eol);
                        }
                    } else {
                        int offset = 0;
                        if (next_sec != NULL) {
                            offset = ((next_sec - 1) - buff);
                        } else {
                            offset = size - 1;
                        }
                        fseek(map_file, offset, SEEK_SET);
                        if (next_sec == NULL) {
                            write_val_to_file(map_file, 1, "\n");
                        }
                        write_val_to_file(map_file, tag_len, (char *) tag);
                        write_val_to_file(map_file, 1, "=");
                        write_val_to_file(map_file, val_len, val);
                        write_val_to_file(map_file, 1, "\n");
                        if (next_sec != NULL) {
                            long b_to_w = size - ((next_sec - 1) - buff);
                            fseek(map_file,
                                  (offset + (2 + tag_len + val_len)),
                                  SEEK_SET);
                            write_val_to_file(map_file, b_to_w,
                                              (next_sec - 1));
                        }

                    }
                }
                free(buff);
            } else {
                printf("File read error on this file: %s \n",
                       df->filename);
                free(buff);
                buff = NULL;
            }
            fclose(map_file);
        }
    }
}
