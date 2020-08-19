/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <time.h>
#include <stdio.h>
#include <string.h>

#include "common_structs.h"
#include "message_queue.h"


long make_long(const char* num, int size)
{
    const char* end;
    long ret = 0;
    end = num + size;
    while(num < end) {
        ret = ret * 10 + *num - '0';
        ++num;
    }
    return ret;
}
time_t convert_to_sec_since_1970(char* yyyymmddhhmmss)
{
    struct tm t;
    memset(&t, 0, sizeof(struct tm));
    t.tm_year = make_long(yyyymmddhhmmss, 4) - 1900;
    t.tm_mon = make_long(yyyymmddhhmmss + 4, 2) -1;
    t.tm_mday = make_long(yyyymmddhhmmss + 6, 2);
    t.tm_hour = make_long(yyyymmddhhmmss + 8, 2);
    t.tm_min = make_long(yyyymmddhhmmss + 10, 2);
    t.tm_sec = make_long(yyyymmddhhmmss +12, 2);
    return mktime(&t);
}
/*
 * Clean up the init args.
 */
void destroy_init_args(struct init_args *in)
{
    if (in->name_len > 0 && in->name != NULL) {
        free(in->name);
        in->name = 0;
        in->name_len = 0;
    }
    if (in->sc_len > 0 && in->sender_comp != NULL) {
        free(in->sender_comp);
        in->sender_comp = 0;
        in->sc_len = 0;
    }
    if (in->tc_len > 0 && in->target_comp != NULL) {
        free(in->target_comp);
        in->target_comp = 0;
        in->tc_len = 0;
    }
    if (in->ts_len > 0 && in->target_sub != NULL) {
        free(in->target_sub);
        in->target_sub = 0;
        in->ts_len = 0;
    }
    if (in->ss_len > 0 && in->sender_sub != NULL) {
        free(in->sender_sub);
        in->sender_sub = 0;
        in->ss_len = 0;
    }
    if (in->un_len > 0 && in->username != NULL) {
        free(in->username);
        in->username = 0;
        in->un_len = 0;
    }
    if (in->p_len > 0 && in->password != NULL) {
        free(in->password);
        in->password = 0;
        in->p_len = 0;
    }
    if (in->fv_len > 0 && in->fix_version != 0) {
        free(in->fix_version);
        in->fix_version = 0;
        in->fv_len = 0;
    }
    if (in->reset_time != NULL) {
        free(in->reset_time);
    }
    free(in);
    in = 0;
}

void destroy_init_args_list(queue_t list)
{
    element_t e, temp;
    e = dequeue(list);
    while (e != NULL) {
        struct init_args *rr = (struct init_args *) e->d;
        destroy_init_args(rr);
        temp = e;
        e = temp->next;
        free(temp);
    }
}

void destroy_parse_con(parse_con * pc)
{
    if (pc) {
        if (pc->is_server) {
            //do_nothing
            free(pc);
            pc = 0;
        } else {
            if (pc->in_args_list) {
                destroy_init_args_list(pc->in_args_list);
                destroy_queue(pc->in_args_list);
            }
            free(pc);
            pc = 0;
        }
    }
}

int dart_itoa(int value, char *str, int radix)
{
    static char dig[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    int n = 0, neg = 0;
    unsigned int v = 0;
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
    return n;
}
