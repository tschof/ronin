/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <ctype.h>

#include "fix_rom_trans.h"
#include "fix_handler.h"
#include "hashmap.h"
#include "parser_func.h"
#include "dart_constants.h"

static pthread_mutex_t trans_init_mutex = PTHREAD_MUTEX_INITIALIZER;
static hashmap_t month_codes = 0;
static int m_codes[26] = {
    0, 0, 0, 0, 0,
    1, 1, 1, 0, 1,
    1, 0, 1, 1, 0,
    0, 1, 0, 0, 0,
    1, 1, 0, 1, 0, 1
};

static int is_init = 0;

char *get_file_contents(const char *filename, long *size)
{
    FILE *map_file;
    long result;
    char *buff;
    *size = -1;
    buff = NULL;
    map_file = NULL;
    map_file = fopen(filename, "r");
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
            fclose(map_file);
        }
    }
    return buff;
}

static void add_to_rom_array(struct fix_tag_def *rom_to_fix[],
                             char *buffer, long len)
{
    char *c_offset = 0;
    long c_len = 0;
    c_offset = strchr(buffer, ',');
    if (c_offset) {
        char *tag;
        int val;
        char *val_off;
        c_len = c_offset - buffer;
        val = 0;
        tag = calloc(c_len + 1, 1);
        memcpy(tag, buffer, c_len);

        val_off = buffer;
        val_off += c_len + 1;
        val = atoi(val_off);
        if (val >= 0 && val < ROM_LEN) {
            struct fix_tag_def *ftd;
            ftd = malloc(sizeof(struct fix_tag_def));
            if (ftd != NULL) {
                ftd->name = tag;
                ftd->len = c_len;
                ftd->rom_tag = val;
                rom_to_fix[val] = ftd;
            }
        } else {
            printf("Val value is outside the accepted range: %d \n", val);
        }
    } else {
        printf("Could not find a comma in this mess: %s \n", buffer);
    }
}

static void add_to_rom_array2(fix_tag_def * rom_to_fix[],
                              char *buffer, long len)
{
    char *c_offset = 0;
    long c_len = 0;
    c_offset = strchr(buffer, ',');
    if (c_offset) {
        char *tag;
        int val;
        char *val_off;
        c_len = c_offset - buffer;
        val = 0;
        tag = calloc(c_len + 1, 1);
        memcpy(tag, buffer, c_len);

        val_off = buffer;
        val_off += c_len + 1;
        val = atoi(val_off);
        if (val >= 0 && val < ROM_LEN) {
            struct fix_tag_def *ftd;
            ftd = malloc(sizeof(struct fix_tag_def));
            if (ftd != NULL) {
                ftd->name = tag;
                ftd->len = c_len;
                ftd->rom_tag = val;
                rom_to_fix[len] = ftd;
            }
        } else {
            printf("Val value is outside the accepted range: %d \n", val);
        }
    } else {
        printf("Could not find a comma in this mess: %s \n", buffer);
    }
}

static void add_to_map(hashmap_t map, char *buffer, long len)
{
    char *off;
    long c_offset = 0;
    off = strchr(buffer, ',');
    if (off) {
        char *tag;
        int *val;
        char *val_off;
        c_offset = off - buffer;
        val = malloc(sizeof(int));
        tag = calloc(c_offset + 1, 1);
        memcpy(tag, buffer, c_offset);

        val_off = buffer;
        val_off += c_offset + 1;
        *val = atoi(val_off);
        insert_map(map, tag, c_offset, val, sizeof(int));
        free(val);
        free(tag);
    } else {
        printf("Could not find a comma in this mess: %s \n", buffer);
    }
}

int build_map_from_file(const char *filename, hashmap_t map)
{
    char *buffer;
    long size;
    size = 0;
    buffer = NULL;
    buffer = get_file_contents(filename, &size);
    if (size > 0) {
        long cur_pos = 0;
        char eol[2] = { '\n', '\0' };
        long end_pos = 0;
        int should_parse = 1;
        char *d_off = buffer;
        while (cur_pos < size && should_parse) {
            end_pos = find_offset(d_off, (size - cur_pos), eol, 1);
            if (end_pos > 0) {
                add_to_map(map, d_off, end_pos);
                d_off += end_pos + 1;
                cur_pos += end_pos + 1;
            } else {
                should_parse = 0;
            }
        }
        free(buffer);
    } else {
        return 1;
    }
    return 0;
}

static int build_rom_array(const char *filename,
                           struct fix_tag_def *rom_to_fix[])
{
    char *buffer;
    long size;
    size = 0;
    buffer = NULL;
    buffer = get_file_contents(filename, &size);
    if (size > 0) {
        long cur_pos = 0;
        char eol[2] = { '\n', '\0' };
        long end_pos = 0;
        int should_parse = 1;
        char *d_off = buffer;
        while (cur_pos < size && should_parse) {
            end_pos = find_offset(d_off, size - cur_pos, eol, 1);
            if (end_pos > 0) {
                add_to_rom_array(rom_to_fix, d_off, end_pos);
                d_off += end_pos + 1;
                cur_pos += end_pos + 1;
            } else if (end_pos == -1 && ((size - cur_pos) != 0)) {
                /* There's more to read here, probably a
                   text file w/o a newline at the end */
                add_to_rom_array(rom_to_fix, d_off, size - cur_pos);
                should_parse = 0;
            } else {
                should_parse = 0;
            }
        }
        free(buffer);
    } else {
        printf("Could not get anything from this file: %s \n", filename);
    }
    return size;
}

static int build_rom_leg_array(const char *filename,
                               struct fix_tag_def *rom_to_fix[])
{
    char *buffer;
    long size;
    size = 0;
    int array_size = 0;
    buffer = NULL;
    buffer = get_file_contents(filename, &size);
    if (size > 0) {
        long cur_pos = 0;
        char eol[2] = { '\n', '\0' };
        long end_pos = 0;
        int should_parse = 1;
        char *d_off = buffer;
        while (cur_pos < size && should_parse) {
            end_pos = find_offset(d_off, size - cur_pos, eol, 1);
            if (end_pos > 0) {
                add_to_rom_array2(rom_to_fix, d_off, array_size);
                d_off += end_pos + 1;
                cur_pos += end_pos + 1;
            } else if (end_pos == -1 && ((size - cur_pos) != 0)) {
                /* There's more to read here, probably a
                   text file w/o a newline at the end */
                add_to_rom_array2(rom_to_fix, d_off, array_size);
                should_parse = 0;
            } else {
                should_parse = 0;
            }
            array_size++;
        }
        free(buffer);
    } else {
        printf("Could not get anything from this file: %s \n", filename);
    }
    return array_size;
}

int is_month_code(char *code)
{
    if (month_codes && code) {
        char *temp = 0;
        return find_n_fill(month_codes, code, 1, (void *) &temp);
    }
    return 0;
}

size_t get_symbol_len(char *sym, size_t s_len)
{
    size_t len = s_len;
    char *d_off = sym;
    d_off += (s_len - 1);
    int i = 0;
    for (; i < s_len; ++i) {
        if (sym[i] == 0x20 || sym[i] == 0x2D) {
            d_off = ((sym + i) - 1);
            len = i;
            break;
        }
    }
    i = 0;
    if (*d_off >= 0x30 && *d_off <= 0x39) {
        for (; i < len; ++i, --d_off) {
            if (*d_off < 0x30 || *d_off > 0x39) {
                if (*d_off > 0x5A || *d_off < 0x41
                    || !(m_codes[(*d_off) - 65])) {
                    return s_len;
                }
                len = d_off - sym;
                break;
            }
        }
    }
    return len;
}

void create_month_codes()
{
    if (month_codes == 0) {
        month_codes = create_map(12);
    }
    insert_map(month_codes, "F", 1, "F", 1);
    insert_map(month_codes, "G", 1, "G", 1);
    insert_map(month_codes, "H", 1, "H", 1);
    insert_map(month_codes, "J", 1, "J", 1);
    insert_map(month_codes, "K", 1, "K", 1);
    insert_map(month_codes, "M", 1, "M", 1);
    insert_map(month_codes, "N", 1, "N", 1);
    insert_map(month_codes, "Q", 1, "Q", 1);
    insert_map(month_codes, "U", 1, "U", 1);
    insert_map(month_codes, "V", 1, "V", 1);
    insert_map(month_codes, "X", 1, "X", 1);
    insert_map(month_codes, "Z", 1, "Z", 1);
}

static void init_map(orderbook * b, const char *fix_rom_new,
                     const char *fix_rom_cnl, const char *fix_rom_rpl,
                     const char *fix_rom_exe, const char *fix_rom_cnl_rej,
                     const char *rom_fix_stat, const char *rom_fix_out,
                     const char *rom_fix_cnl, const char *rom_fix_rpl,
                     const char *rom_fix_leg)
{
    int ret = 0;
    b->fix_to_rom_map_new = create_map(95);
    b->fix_to_rom_map_cnl = create_map(95);
    b->fix_to_rom_map_rpl = create_map(95);
    b->fix_to_rom_map_exe = create_map(95);
    b->fix_to_rom_map_cnlrej = create_map(20);
    b->rom_to_fix_status = calloc(ROM_LEN, sizeof(struct fix_tag_def));
    b->rom_to_fix_out = calloc(ROM_LEN, sizeof(struct fix_tag_def));

    b->rom_to_fix_cnl = calloc(ROM_LEN, sizeof(struct fix_tag_def));
    b->rom_to_fix_rpl = calloc(ROM_LEN, sizeof(struct fix_tag_def));

    b->rom_to_fix_leg = calloc(ROM_LEN, sizeof(struct fix_tag_def));

    ret = build_map_from_file(fix_rom_new, b->fix_to_rom_map_new);
    if (ret) {
        printf("Failed to build map");
    }
    ret = build_map_from_file(fix_rom_cnl, b->fix_to_rom_map_cnl);
    if (ret) {
        printf("Failed to build map");
    }
    ret = build_map_from_file(fix_rom_rpl, b->fix_to_rom_map_rpl);
    if (ret) {
        printf("Failed to build map");
    }
    ret = build_map_from_file(fix_rom_exe, b->fix_to_rom_map_exe);
    if (ret) {
        printf("Failed to build map");
    }
    ret = build_map_from_file(fix_rom_cnl_rej, b->fix_to_rom_map_cnlrej);
    if (ret) {
        int l_val = 0;
        int *val = &l_val;
        printf("Failed to build cancel reject map.");
        *val = 60;
        insert_map(b->fix_to_rom_map_cnlrej, "11", 2, val, sizeof(int));
        *val = 26;
        insert_map(b->fix_to_rom_map_cnlrej, "58", 2, val, sizeof(int));
        *val = 14;
        insert_map(b->fix_to_rom_map_cnlrej, "39", 2, val, sizeof(int));
        *val = 50;
        insert_map(b->fix_to_rom_map_cnlrej, "60", 2, val, sizeof(int));
        *val = 3;
        insert_map(b->fix_to_rom_map_cnlrej, "41", 2, val, sizeof(int));
        *val = 63;
        insert_map(b->fix_to_rom_map_cnlrej, "434", 3, val, sizeof(int));
        *val = 16;
        insert_map(b->fix_to_rom_map_cnlrej, "37", 2, val, sizeof(int));
    }
    build_rom_array(rom_fix_stat, b->rom_to_fix_status);
    build_rom_array(rom_fix_out, b->rom_to_fix_out);
    build_rom_array(rom_fix_cnl, b->rom_to_fix_cnl);
    build_rom_array(rom_fix_rpl, b->rom_to_fix_rpl);
    b->leg_array_size =
        build_rom_leg_array(rom_fix_leg, b->rom_to_fix_leg);
}

void initalize_maps(orderbook * b)
{
    const char *fix_rom_new = NULL;
#ifdef _BOGUS_FIX_
    fix_rom_new = "BOGUSFixToRomNew";
#else
    fix_rom_new = "FixToRomNew";
#endif
    pthread_mutex_lock(&trans_init_mutex);
    if (is_init) {
        //do nothing
    } else {
        is_init = 1;
        month_codes = create_map(12);
        create_month_codes();
    }
    pthread_mutex_unlock(&trans_init_mutex);
    init_map(b, fix_rom_new, "FixToRomCnl", "FixToRomRpl", "FixToRomExe",
             "FixToRomCnlRej", "RomToFixStat", "RomToFixOut",
             "RomToFixCnl", "RomToFixRpl", "RomToFixLeg");
}

hashmap_t get_map_for_type(char type, char *rom_type, orderbook * ob)
{
    switch (type) {
    case '8':
        *rom_type = 'S';
        return ob->fix_to_rom_map_exe;
    case 'D':
        *rom_type = 'E';
        return ob->fix_to_rom_map_new;
    case 'F':
        *rom_type = 'C';
        return ob->fix_to_rom_map_cnl;
    case 'G':
        *rom_type = 'R';
        return ob->fix_to_rom_map_rpl;
    case '9':
        *rom_type = 'U';
        return ob->fix_to_rom_map_cnlrej;
    default:
        *rom_type = 'S';
        return ob->fix_to_rom_map_exe;
    }
}

static hashmap_t get_fix_trans_map(char *type, char *rom_type,
                                   orderbook * ob)
{
    char *end = strchr(type, FIX_FIELD_DELIM);
    if (end != NULL && (end - type) > 1) {
        if (strncmp(type, "AB", 2) == 0) {
            *rom_type = 'E';
            return ob->fix_to_rom_map_new;
        } else if (strncmp(type, "AC", 2) == 0) {
            *rom_type = 'R';
            return ob->fix_to_rom_map_rpl;
        } else {
            *rom_type = 'S';
            return ob->fix_to_rom_map_exe;
        }
    } else {
        return get_map_for_type(type[0], rom_type, ob);
    }

}

size_t rom_to_split_fix_sym(databuf_t * d_off, const char *symbol,
                            const size_t sym_len)
{
    char *slash_off = 0;
    size_t s_len;
    size_t ret_len = 0;
    slash_off = memchr(symbol, '/', sym_len);
    if (slash_off == 0) {
        s_len = databuf_write(d_off, "%c55=", FIX_FIELD_DELIM);
        ret_len += s_len;
        databuf_memcpy(d_off, symbol, sym_len);
        ret_len += sym_len;
    } else {
        const char *e_off = slash_off + 1;
        size_t ex_len = (sym_len - ((slash_off + 1) - symbol));
        s_len = databuf_write(d_off, "%c55=", FIX_FIELD_DELIM);
        ret_len += s_len;
        databuf_memcpy(d_off, symbol, (slash_off - symbol));
        ret_len += (slash_off - symbol);
        s_len = databuf_write(d_off, "%c65=", FIX_FIELD_DELIM);
        ret_len += s_len;
        databuf_memcpy(d_off, e_off, ex_len);
        ret_len += ex_len;
    }
    return ret_len;
}

long translate_put_call(char val, databuf_t * output)
{
    switch (val) {
    case 'P':
        databuf_memcpy(output, "0", 1);
        break;
    case '0':
        databuf_memcpy(output, "P", 1);
        break;
    case '1':
        databuf_memcpy(output, "C", 1);
        break;
    case 'C':
        databuf_memcpy(output, "1", 1);
        break;
    default:
        return 0;
    }
    return 1;
}

long translate_open_close(char val, databuf_t * output)
{
    switch (val) {
    case '1':
        databuf_memcpy(output, "O", 1);
        break;
    case 'O':
        databuf_memcpy(output, "1", 1);
        break;
    case '0':
        databuf_memcpy(output, "C", 1);
        break;
    case 'C':
        databuf_memcpy(output, "0", 1);
        break;
    default:
        return 0;
    }
    return 1;
}

long translate_sec_type(char *value, databuf_t * output, long val_len)
{
    long len = 0;
    if (strncmp("E", value, 1) == 0) {
        databuf_memcpy(output, "CS", 2);
        len = 2;
    } else if (strncmp("CS", value, 2) == 0) {
        databuf_memcpy(output, "E", 1);
        len = 1;
    } else if (strncmp("FUT", value, 3) == 0) {
        databuf_memcpy(output, "F", 1);
        len = 1;
    } else if (strncmp("OPT", value, 3) == 0) {
        databuf_memcpy(output, "O", 1);
        len = 1;
    } else if (strncmp("FOR", value, 3) == 0) {
        databuf_memcpy(output, "X", 1);
        len = 1;
    } else if (strncmp("F", value, 1) == 0) {
        databuf_memcpy(output, "FUT", 3);
        len = 3;
    } else if (strncmp("O", value, 1) == 0) {
        databuf_memcpy(output, "OPT", 3);
        len = 3;
    } else if (strncmp("X", value, 1) == 0) {
        databuf_memcpy(output, "FOR", 3);
        len = 3;
    } else if (strncmp("MLEG", value, 4) == 0) {
        databuf_memcpy(output, "O", 1);
        len = 1;
    }
    return len;
}

long translate_status(char *value, databuf_t * output, long value_len)
{
    long len = value_len;
    if (strncmp("C", value, 1) == 0) {
        databuf_memcpy(output, "12", 2);
        len = 2;
    } else if (strncmp("13", value, 2) == 0) {
        databuf_memcpy(output, "0", 1);
        len = 1;
    } else if (strncmp("0", value, 1) == 0) {
        databuf_memcpy(output, "13", 2);
        len = 2;
    } else if (strncmp("12", value, 2) == 0) {
        databuf_memcpy(output, "C", 1);
        len = 1;
    } else if (strncmp("21", value, 2) == 0) {
        databuf_memcpy(output, "E", 1);
        len = 1;
    } else if (strncmp("E", value, 1) == 0) {
        databuf_memcpy(output, "21", 2);
        len = 2;
    } else {
        databuf_memcpy(output, value, value_len);
    }
    return len;
}

long determine_trans_type(int rom_tag, char *value, databuf_t * output,
                          long value_len)
{
    switch (rom_tag) {
    case 14:
        return translate_status(value, output, value_len);
    case 31:
        return translate_put_call(value[0], output);
    case 38:
        return translate_open_close(value[0], output);
    case 35:
        return translate_sec_type(value, output, value_len);
    default:
        return 0;
    }
}

void set_tag_one_vals(char *fix_message, long len,
                      char **trade_for, int *tf_len,
                      char **local_acc, int *la_len, char **firm,
                      int *firm_len, char **trade_acc,
                      int *tacc_len, struct dart_rom_pos *positions,
                      databuf_t * output)
{
    long end = 0;
    long bogus_len = 0;
    char bogus_separator = ' ';
    char *clearingid = NULL;
    *trade_for = fix_message;
    *trade_for += positions[11].beg;
    bogus_len = positions[11].end - positions[11].beg;
    clearingid = strchr(*trade_for, bogus_separator);
    if (clearingid) {
        *tf_len = (clearingid - *trade_for);
        clearingid += 1;
        *trade_acc = strchr(clearingid, bogus_separator);
        if (*trade_acc) {
            end = *trade_acc - clearingid;
            databuf_memcpy(output, clearingid, end);
            *trade_acc += 1;
            *firm = strchr(*trade_acc, bogus_separator);
            if (*firm) {
                *tacc_len = (*firm - *trade_acc);
                *firm += 1;
                *local_acc = strchr(*firm, bogus_separator);
                if (*local_acc) {
                    *firm_len = (*local_acc - *firm);
                    *local_acc += 1;
                    *la_len = bogus_len - (*local_acc - *trade_for);
                }
            }
        }
    }
}

static void set_cplx_type(char *fix_message,
                          struct dart_rom_pos *positions,
                          databuf_t * output)
{
    char *type = fix_message;
    type += positions[35].beg;
    if (strncmp(type, "MLEG", 4) == 0) {
        if(positions[55].end - positions[55].beg > 0) {
            databuf_memcpy(output, "5", 1);
        } else {
            databuf_memcpy(output, "3", 1);
        }
    }
}

long default_fix_to_rom_override(struct message *mess, char *fix_message,
                                 databuf_t * output, long fix_len,
                                 char rom_type,
                                 struct dart_rom_pos positions[ROM_LEN],
                                 int *rejected, orderbook * ob)
{
    long cur_off;
    int i = 1;
    int should_parse = 1;
    int s_len;
    char *firm = NULL;
    int firm_len = 0;
#ifdef _BOGUS_FIX_
    char *trade_for = NULL;
    int tf_len = 0;
    char *local_acc = NULL;
    int la_len = 0;
    char *trade_acc = NULL;
    int trade_acc_len = 0;
#endif
    cur_off = 0;
    s_len = databuf_write(output, "%c,", rom_type);
    cur_off += s_len;
    while (i < ROM_LEN && should_parse) {
#ifdef _BOGUS_FIX_
        if (positions[i].beg == positions[i].end && i != 12 && i != 24
            && i != 49 && i != 22 && i != 23) {
            if (i == 66) {
                set_cplx_type(fix_message, positions, output);
            }
        } else if (i != 11 && i != 12 && i != 24 && i != 49
                   && i != 22 && i != 23) {
            char *beg_off;
            long len;
            beg_off = fix_message;
            beg_off += positions[i].beg;
            len = (positions[i].end - positions[i].beg);
            if (i != 14 && i != 31 && i != 38 && i != 35) {
                databuf_memcpy(output, beg_off, len);
            } else {
                len = determine_trans_type(i, beg_off, output, len);
            }
            cur_off += len;
        } else if (i == 11) {
            set_tag_one_vals(fix_message, fix_len, &trade_for, &tf_len,
                             &local_acc, &la_len, &firm, &firm_len,
                             &trade_acc, &trade_acc_len, positions,
                             output);
        } else if (i == 12) {
            databuf_memcpy(output, trade_acc, trade_acc_len);
        } else if (i == 22) {
            databuf_memcpy(output, local_acc, la_len);
        } else if (i == 23) {
            databuf_memcpy(output, firm, firm_len);
        } else if (i == 24) {
            databuf_memcpy(output, trade_for, tf_len);
        } else if (i == 49 && rom_type == 'R') {
            databuf_memcpy(output, "-1", 2);
            cur_off += 2;
        }
#else
        if (positions[i].beg == positions[i].end && i != 49) {
            // do nothing
            if (i == 66) {
                set_cplx_type(fix_message, positions, output);
            }
            if (i == 23 && firm != NULL) {
                databuf_memcpy(output, firm, firm_len);
            }
        } else if (i == 49 && rom_type == 'R') {
            databuf_memcpy(output, "-1", 2);
            cur_off += 2;
        } else if (i == 11) {
            char *beg_off;
            long len;
            beg_off = fix_message;
            beg_off += positions[i].beg;
            len = (positions[i].end - positions[i].beg);
            databuf_memcpy(output, beg_off, len);
            firm = beg_off;
            firm_len = len;
        } else if (i == 23 && firm != NULL) {
            databuf_memcpy(output, firm, firm_len);
        } else {
            char *beg_off;
            long len;
            beg_off = fix_message;
            beg_off += positions[i].beg;
            len = (positions[i].end - positions[i].beg);
            if (i != 14 && i != 31 && i != 38 && i != 35) {
                databuf_memcpy(output, beg_off, len);
            } else {
                len = determine_trans_type(i, beg_off, output, len);
            }
            cur_off += len;
        }
#endif
        databuf_memcpy(output, ",", 1);
        cur_off += 1;
        ++i;
    }
    databuf_memcpy(output, "\n", 1);
    cur_off += 1;
    return cur_off;
}

void build_dart_positions(struct dart_rom_pos l_positions[],
                          char *fix_message, long fix_len,
                          hashmap_t trans_map)
{
    long len = fix_len;
    char *c = fix_message;
    char *tag_off = c;
    int tag_len = 0;
    char *val_off = c;
    int rval = 0;
    int *rom_val = &rval;
    for (; len > 0; ++c, --len) {
        if (*c == '=') {
            val_off = c + 1;
            tag_len = c - tag_off;
        } else if (*c == FIX_FIELD_DELIM) {
            if (find_n_fill
                (trans_map, tag_off, tag_len, (void **) &rom_val) > 0) {
                l_positions[*rom_val].beg = (val_off - fix_message);
                l_positions[*rom_val].end = (c - fix_message);
            }
            tag_off = c + 1;
        }
    }
}

/**
 * This function assumes that all fix messages passed in start with
 * 35=.
 */
long trans_fix_message(struct message *mess, char *fix_message,
                       long fix_len, databuf_t * output,
                       int *rejected, orderbook * ob,
                       long (*fix_to_rom_override) (struct message * mess,
                                                    char *fix_message,
                                                    databuf_t * output,
                                                    long fix_len,
                                                    char rom_type,
                                                    struct dart_rom_pos
                                                    positions[],
                                                    int *rejected,
                                                    orderbook * b))
{
    struct dart_rom_pos l_positions[ROM_LEN];
    int i = 0;
    char rom_type = 'S';
    hashmap_t trans_map;
    for (i = 0; i < ROM_LEN; ++i) {
        l_positions[i].beg = 0;
        l_positions[i].end = 0;
    }
    trans_map = get_fix_trans_map((fix_message + 3), &rom_type, ob);
    build_dart_positions(l_positions, fix_message, fix_len, trans_map);
    return fix_to_rom_override(mess, fix_message, output, fix_len,
                               rom_type, l_positions, rejected, ob);
}

static void create_tag_sixty(databuf_t * output)
{
    time_t rawtime;
    struct tm *ti;
    struct tm lm;
    int year;
    int month;
    databuf_write(output, "%c60=", FIX_FIELD_DELIM);
    time(&rawtime);
    ti = gmtime_r(&rawtime, &lm);
    ti = &lm;
    year = 1900 + ti->tm_year;
    month = 1 + ti->tm_mon;
    databuf_write(output,
                  "%04d%02d%02d-%02d:%02d:%02d", year,
                  month, ti->tm_mday, ti->tm_hour, ti->tm_min, ti->tm_sec);
}

static void create_cnl_rep_rej(char *fix_message, databuf_t * output,
                               char *fix_type,
                               struct dart_fix_pos positions[],
                               char *status_offset, int status_len)
{

    char *beg_off;
    long len;
    int found_cl = 0;
    *fix_type = '9';
    positions[3].name = "41";
    positions[60].name = "11";

    beg_off = fix_message;
    beg_off += positions[17].beg;
    len = (positions[17].end - positions[17].beg);
    databuf_write(output, "%c%s=", FIX_FIELD_DELIM, positions[17].name);
    if (len <= 0) {
        databuf_memcpy(output, "UNKNOWN", 7);
    } else {
        databuf_memcpy(output, beg_off, len);
    }
    beg_off = fix_message;
    beg_off += positions[60].beg;
    len = (positions[60].end - positions[60].beg);
    if (len > 0) {
        databuf_write(output, "%c%s=", FIX_FIELD_DELIM,
                      positions[60].name);
        databuf_memcpy(output, beg_off, len);
        found_cl = 1;
    }
    beg_off = fix_message;
    beg_off += positions[3].beg;
    len = (positions[3].end - positions[3].beg);
    databuf_write(output, "%c%s=", FIX_FIELD_DELIM, positions[3].name);
    databuf_memcpy(output, beg_off, len);
    if (!found_cl) {
        databuf_write(output, "%c%s=", FIX_FIELD_DELIM,
                      positions[60].name);
        databuf_memcpy(output, beg_off, len);
    }
    databuf_write(output, "%c39=0", FIX_FIELD_DELIM);
    if (strncmp("14", status_offset, status_len) == 0 ||
            strncmp("29", status_offset, status_len) == 0) {
        databuf_write(output, "%c434=1", FIX_FIELD_DELIM);
    } else {
        databuf_write(output, "%c434=2", FIX_FIELD_DELIM);
    }
    beg_off = fix_message;
    beg_off += positions[26].beg;
    len = (positions[26].end - positions[26].beg);
    if (len > 0) {
        databuf_write(output, "%c%s=", FIX_FIELD_DELIM,
                      positions[26].name);
        databuf_memcpy(output, beg_off, len);
    }
}

void cleave_tpos_time(const char *fix_message, databuf_t * output,
                      char *fix_type,
                      const struct dart_fix_pos positions[], int sym_off)
{
    size_t len = positions[sym_off].end - positions[sym_off].beg;
    const char *d_off = fix_message + positions[sym_off].beg;
    if (len >= 17) {
        databuf_memcpy(output, d_off, 17);
    }
}

/**
 * This function will strip the year and month code off a symbol.
 * This cannot be used for option on future symbols that include
 * spaces, strikes and so forth.
 */
int get_base_sym_len(const char *symbol, int sym_len)
{
    int end = sym_len - 1;
    if (symbol[end] > 0x30 && symbol[end] < 0x39) {
        --end;
        for (; end > 0; --end) {
            if (symbol[end] < 0x30 || symbol[end] > 0x39) {
                if (!(m_codes[symbol[end] - 65])) {
                    return sym_len;
                }
                break;
            }
        }
    } else {
        return sym_len;
    }
    return end;
}

void tpos_symbol_trans(const char *fix_message, databuf_t * output,
                       char *fix_type,
                       const struct dart_fix_pos positions[], int sym_off,
                       int st_off)
{
    size_t len = positions[sym_off].end - positions[sym_off].beg;
    const char *d_off = fix_message + positions[sym_off].beg;
    char *comma = strchr(d_off, '/');
    //size_t st_len = positions[st_off].end - positions[st_off].beg;
    const char *t_off = fix_message + positions[st_off].beg;
    if (comma != NULL && (comma - d_off) < len) {
        char *suff_off = comma;
        suff_off += 1;
        if (strncmp(suff_off, "PR", 2) == 0) {
            int suf_len = 0;
            databuf_write(output, "%c55=", FIX_FIELD_DELIM);
            databuf_memcpy(output, d_off, (comma - d_off));
            suff_off += 2;
            suf_len = len - (suff_off - d_off);
            if (suf_len <= 0) {
                databuf_write(output, "%c65=P", FIX_FIELD_DELIM);
            } else {
                databuf_write(output, "%c65=P", FIX_FIELD_DELIM);
                databuf_memcpy(output, suff_off, suf_len);
            }
        } else if (strncmp(t_off, "E", 1) == 0) {
            rom_to_split_fix_sym(output, d_off, len);
        } else {
            len = positions[sym_off].end - positions[sym_off].beg;
            d_off = fix_message + positions[sym_off].beg;
            databuf_write(output, "%c55=", FIX_FIELD_DELIM);
            databuf_memcpy(output, d_off, len);
        }

    } else {
        len = positions[sym_off].end - positions[sym_off].beg;
        d_off = fix_message + positions[sym_off].beg;
        databuf_write(output, "%c55=", FIX_FIELD_DELIM);
        databuf_memcpy(output, d_off, len);
        //rom_to_split_fix_sym(output, d_off, len);
    }
}

long default_rom_to_fix_override(struct message *mess, char *fix_message,
                                 databuf_t * output, long fix_len,
                                 char *fix_type,
                                 struct dart_fix_pos positions[],
                                 int *rejected, orderbook * ob, int is_spread)
{

    char end_ascii_one[2] = { FIX_FIELD_DELIM, '\0' };
    int i = 1;
    int should_parse = 1;
    int s_len;
    char *status_offset;
    int status_len = 0;
    status_offset = NULL;
    status_offset = fix_message;
    status_offset += positions[14].beg;
    status_len = (positions[14].end - positions[14].beg);
    if (strncmp("14", status_offset, 2) != 0 &&
        strncmp("20", status_offset, 2) != 0 &&
        strncmp("29", status_offset, 2) != 0 &&
        strncmp("30", status_offset, 2) != 0) {
        if (*fix_type == '8') {
            char *s_off;
            s_off = fix_message;
            s_off += positions[14].beg;
            if (strncmp("5", s_off, 1) == 0 || strncmp("4", s_off, 1) == 0) {
                positions[3].name = "41";
                positions[60].name = "11";
            }
        }
        while (i < ROM_LEN && should_parse) {
            if (positions[i].beg == positions[i].end
                && positions[i].name != NULL) {
                if (*fix_type == '8' && (i == 44 || i == 42)) {
                    char *tag_type = NULL;
                    tag_type = i == 42 ? "32=0" : "31=0";
                    s_len =
                        databuf_write(output,
                                      "%c%s", FIX_FIELD_DELIM, tag_type);
                } else if (i == 50) {
                    create_tag_sixty(output);
                } else if (i == 60) {
                    char *l_beg = fix_message;
                    long len = 0;
                    l_beg += positions[3].beg;
                    len = (positions[3].end - positions[3].beg);
                    s_len =
                        databuf_write(output, "%c%s=", FIX_FIELD_DELIM,
                                      positions[i].name);
                    databuf_memcpy(output, l_beg, len);
                }
            } else if (positions[i].name != NULL) {

                char *beg_off;
                long len;
                beg_off = fix_message;
                beg_off += positions[i].beg;
                len = (positions[i].end - positions[i].beg);
#ifdef _TPOS_
                if (i != 14 && i != 31 && i != 38 && i != 35 && i != 5 &&i != 50) {
                    s_len =
                        databuf_write(output, "%c%s=", FIX_FIELD_DELIM,
                                      positions[i].name);
                    databuf_memcpy(output, beg_off, len);
                } else if (i == 31 || i == 38 || i == 35) {
                    s_len =
                        databuf_write(output, "%c%s=", FIX_FIELD_DELIM,
                                      positions[i].name);
                    len = determine_trans_type(i, beg_off, output, len);
                } else if (i == 14) {
                    s_len =
                        databuf_write(output, "%c%s=", FIX_FIELD_DELIM,
                                      positions[i].name);
                    s_len = determine_trans_type(14, beg_off, output, len);
                    s_len =
                        databuf_write(output, "%c150=", FIX_FIELD_DELIM);
                    s_len = determine_trans_type(14, beg_off, output, len);
                    s_len =
                        databuf_write(output, "%c20=0", FIX_FIELD_DELIM);
                } else if (i == 50) {
                    s_len =
                        databuf_write(output, "%c%s=", FIX_FIELD_DELIM,
                                      positions[i].name);
                    cleave_tpos_time(fix_message, output, fix_type, positions, 50);    
                }  else if( i == 5) {
                    tpos_symbol_trans(fix_message, output, fix_type, 
                                      positions, 5, 35);
                }
#else
                s_len =
                    databuf_write(output, "%c%s=", FIX_FIELD_DELIM,
                                  positions[i].name);
                if (i != 14 && i != 31 && i != 38 && i != 35 && i != 50 && i != 66) {
                    databuf_memcpy(output, beg_off, len);
                } else if( i == 35) {
                    char* cplx = fix_message;
                    cplx += positions[66].beg;
                    int cplx_len = positions[66].end - positions[66].beg;
                    if(cplx_len > 0 && (cplx[0] == '5' || cplx[0] == '3')) {
                        databuf_memcpy(output, "MLEG", 4);
                    } else {
                        len = determine_trans_type(i, beg_off, output, len);
                    }
                } else if (i == 31 || i == 38 || i == 35) {
                    len = determine_trans_type(i, beg_off, output, len);
                } else if (i == 50) {
                    cleave_tpos_time(fix_message, output, fix_type,
                                     positions, 50);
                } else if(i == 66 && len > 0) {
                    if(beg_off[0] == '6') {
                        databuf_memcpy(output, "2", 1);
                    } else if(beg_off[0] == '5' || beg_off[0] == '3') {
                        databuf_memcpy(output, "3", 1);
                    } else {
                        databuf_memcpy(output, "1", 1);
                    }
                } else {
                    s_len = determine_trans_type(14, beg_off, output, len);
                    s_len =
                        databuf_write(output, "%c150=", FIX_FIELD_DELIM);
                    s_len = determine_trans_type(14, beg_off, output, len);
                    s_len =
                        databuf_write(output, "%c20=0", FIX_FIELD_DELIM);
                }
#endif                          //_TPOS_
            }
            ++i;
        }
    } else {
        create_cnl_rep_rej(fix_message, output, fix_type,
                           positions, status_offset, status_len);
    }
    databuf_memcpy(output, end_ascii_one, 1);
    return databuf_unread(output);
}

void build_rom_pos2(char *rom_message, long rom_len,
                    struct fix_tag_def *ftd[ROM_LEN],
                    struct dart_fix_pos *l_positions, long offset)
{
    long len = rom_len;
    char *c = rom_message;
    char *val_off = c;
    int rom_tag = 0;
    for (; len > 0; ++c, --len) {
        if (*c == ',' || *c == '\n') {
            l_positions[rom_tag].beg = (val_off - rom_message) + offset;
            l_positions[rom_tag].end = (c - rom_message) + offset;
            if (ftd[rom_tag] != NULL) {
                l_positions[rom_tag].name = ftd[rom_tag]->name;
                l_positions[rom_tag].len = ftd[rom_tag]->len;
            } else {
                l_positions[rom_tag].name = NULL;
                l_positions[rom_tag].len = 0;
            }
            val_off = c + 1;
            ++rom_tag;
            if (rom_tag >= ROM_LEN) {
                break;
            }
        }
    }
}

long rom_breakdown(struct message *mess, char *rom_message,
                   long rom_len, databuf_t * output,
                   struct fix_tag_def *ftd[ROM_LEN],
                   struct dart_fix_pos l_positions[ROM_LEN],
                   char *fix_type, int *rejected,
                   orderbook * ob,
                   long (*rom_to_fix_override) (struct message *
                                                mess,
                                                char *fix_message,
                                                databuf_t * output,
                                                long fix_len,
                                                char *fix_type,
                                                struct dart_fix_pos
                                                positions[ROM_LEN],
                                                int *rejected,
                                                orderbook * b, int is_spread))
{
    build_rom_pos2(rom_message, rom_len, ftd, l_positions, 0);
    return rom_to_fix_override(mess, rom_message, output, rom_len,
                               fix_type, l_positions, rejected, ob, 0);
}

static const struct dart_fix_pos df_initializer = { 0, 0, 0, 0 };

static void field_pos_init(struct dart_fix_pos dfpos[ROM_LEN],
                           struct fix_tag_def *ftdefs[ROM_LEN])
{
    size_t i;
    for (i = 0; i < ROM_LEN; ++i) {
        dfpos[i] = df_initializer;
        if (ftdefs[i] != 0) {
            dfpos[i].name = ftdefs[i]->name;
            dfpos[i].len = ftdefs[i]->len;
        }
    }
}

long trans_rom_message(struct message *mess, char *rom_message,
                       long rom_len, databuf_t * output,
                       char *ret_fix_type, int *ft_len, int *rejected,
                       orderbook * ob,
                       long (*rom_to_fix_override) (struct message * mess,
                                                    char *fix_message,
                                                    databuf_t * output,
                                                    long fix_len,
                                                    char *fix_type,
                                                    struct dart_fix_pos
                                                    positions[ROM_LEN],
                                                    int *rejected,
                                                    orderbook * b, int is_spread))
{

    long ret_val;
    struct dart_fix_pos l_positions[ROM_LEN];
    char fix_type;
    ret_val = 0;

    fix_type = rom_message[0];
    if (fix_type == 'S') {
        fix_type = '8';
        field_pos_init(l_positions, ob->rom_to_fix_status);
        ret_val =
            rom_breakdown(mess, rom_message, rom_len, output,
                          ob->rom_to_fix_status, l_positions, &fix_type,
                          rejected, ob, rom_to_fix_override);
    } else {
        switch (fix_type) {
        case 'E':
            fix_type = 'D';
            field_pos_init(l_positions, ob->rom_to_fix_out);
            ret_val =
                rom_breakdown(mess, rom_message, rom_len, output,
                              ob->rom_to_fix_out, l_positions, &fix_type,
                              rejected, ob, rom_to_fix_override);
            break;
        case 'R':
            fix_type = 'G';
            field_pos_init(l_positions, ob->rom_to_fix_rpl);
            ret_val =
                rom_breakdown(mess, rom_message, rom_len, output,
                              ob->rom_to_fix_rpl, l_positions, &fix_type,
                              rejected, ob, rom_to_fix_override);
            break;
        case 'C':
            fix_type = 'F';
            field_pos_init(l_positions, ob->rom_to_fix_cnl);
            ret_val =
                rom_breakdown(mess, rom_message, rom_len, output,
                              ob->rom_to_fix_cnl, l_positions, &fix_type,
                              rejected, ob, rom_to_fix_override);
            break;
        default:
            *rejected = 1;
            break;
        }
    }
    ret_fix_type[0] = fix_type;
    ret_fix_type[1] = '\0';
    *ft_len = 1;
    return ret_val;
}
