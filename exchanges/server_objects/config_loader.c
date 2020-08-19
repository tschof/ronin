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
    pthread_mutex_t config_init_mutex;
    hashmap_t header_map;
    char *filename;
};

static char *get_file_contents(const char *filename, long *size)
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
                //printf("Inserted this tag: %s and this val: %d\n", tag, *val);
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
    hashmap_t l_map = create_map(10);
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

static int build_map_from_file(const char *filename, hashmap_t map)
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

void dart_reset_maps(dart_config * df)
{
    pthread_mutex_lock(&df->config_init_mutex);
    delete_map(df->header_map);
    df->header_map = create_map(50);
    build_map_from_file(df->filename, df->header_map);
    pthread_mutex_unlock(&df->config_init_mutex);
}

void dart_load_map(dart_config * df)
{
    pthread_mutex_lock(&df->config_init_mutex);
    delete_map(df->header_map);
    df->header_map = create_map(50);
    build_map_from_file(df->filename, df->header_map);
    pthread_mutex_unlock(&df->config_init_mutex);
}

dart_config *initialize_config(const char *filename, int len)
{
    dart_config *df = malloc(sizeof(struct config_loader));
    df->filename = calloc(len + 1, 1);
    memcpy(df->filename, filename, len);
    df->header_map = create_map(50);
    if (pthread_mutex_init(&df->config_init_mutex, NULL) != 0) {
        printf("Failed to init obj manager mutex.\n");
    }
    if (build_map_from_file(df->filename, df->header_map) == -1) {
        free(df->filename);
        delete_map(df->header_map);
        free(df);
        df = 0;
    }
    return df;
}

char *get_val_for_tag(dart_config * df, const char *section, int sec_len,
                      const char *tag, int tag_len, int *val_len)
{
    char *temp;
    int ret_val;
    char *data;
    hashmap_t l_map;
    pthread_mutex_lock(&df->config_init_mutex);
    temp = NULL;
    ret_val =
        get_map_entry(df->header_map, section, sec_len, (void *) &l_map);
    if (ret_val <= 0) {
        pthread_mutex_unlock(&df->config_init_mutex);
        *val_len = 0;
        return NULL;
    }
    ret_val = get_map_entry(l_map, tag, tag_len, (void *) &temp);
    if (ret_val <= 0) {
        pthread_mutex_unlock(&df->config_init_mutex);
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
    pthread_mutex_unlock(&df->config_init_mutex);
    return data;
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

void set_val_for_tag(dart_config * df, const char *section,
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
    long eol_len;
    long size = 0;
    char eol[2] = { '\n', '\0' };
    eol_len = 1;
    pthread_mutex_lock(&df->config_init_mutex);
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
                            tag_off = strstr(tag_off +1, tag);
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
    pthread_mutex_unlock(&df->config_init_mutex);
}
