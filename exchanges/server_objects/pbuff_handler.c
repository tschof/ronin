/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include "pbuff_handler.h"
#include "rom_handler.h"
#include "client_manager.h"
#include "transaction_recorder.h"

unsigned long build_pbuff_offsets(queue_t q, char *data,
                                  unsigned long size,
                                  unsigned long byte_offset,
                                  unsigned long *seq_num)
{
    if (size > 0) {
        char *d_off = data;
        char *l_off = d_off;
        unsigned long l_seq;
        unsigned long read_bytes = 0;
        unsigned long temp_read = 0;
        unsigned short len = 0;

        l_seq = *seq_num;
        while (read_bytes < (size - (1 + sizeof(len)))) {
            unsigned short len;
            l_off += 1;

            memcpy(&len, l_off, sizeof(len));
            len = ntohs(len);
            temp_read = len + sizeof(len) + 1;
            if ((read_bytes + temp_read) <= size) {
                struct seq_offset *off;
                off = malloc(sizeof(struct seq_offset));
                off->seq = l_seq;
                off->beg_off = read_bytes + byte_offset;
                read_bytes += temp_read;
                off->end_off = read_bytes + byte_offset;
                enqueue(q, off);
                d_off = d_off + temp_read;
                ++l_seq;
            } else {
                break;
            }
        }
        *seq_num = l_seq;
        return read_bytes;
    }
    return size;
}


unsigned long parse_pbuff_message(const char *message, unsigned long len,
                                  int *cut_con, struct parse_con *pc)
{

    unsigned long read_bytes = 0;
    if (len > 0) {
        char *d_off;
        long ret_code;
        char *ret_val;
        read_bytes = 0;
        ret_code = 0;
        d_off = (char *) message;
        if (d_off[0] == 'L') {
            ret_val = strchr(d_off, '\n');
            while (ret_val) {
                ret_code = ret_val - d_off;
                read_bytes += 1 + (unsigned long) ret_code;
                if (read_bytes > len) {
                    unsigned long ret_bytes = read_bytes - (ret_code + 1);
                    return (ret_bytes < len ? ret_bytes : len);
                }
                if (pc->is_server) {
                    handle_csv_logon(d_off[0], d_off, ret_code + 1,
                                     cut_con, pc);
                }
                d_off = d_off + ret_code + 1;
                ret_val = strchr(d_off, '\n');
            }
        } else {
            char *l_off = d_off;
            unsigned short len;
            struct message m;
            struct incoming_message *m1;
            char *name = (char *) get_name(pc->co);
            l_off += 1;
            memcpy(&len, l_off, sizeof(len));
            len = ntohs(len);
            ret_code = 1 + sizeof(len) + len;
            m.name = name;
            m.name_len = get_name_len(pc->co);
            m.len = ret_code;
            m.data = d_off;
            pc->callback(&m, pc->book);
            m1 = create_incoming_message(name, get_name_len(pc->co), 1);
            record_inbound_message(m1);
        }

    } else {
        return 0;
    }
    return read_bytes;
}
