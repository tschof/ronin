#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/timeb.h>

#include "debug_logger.h"
#include "tm_to_str.h"
#include "transaction_recorder.h"
#include "dbg_helper.h"

static int set_time_stamp(char *mblk)
{
    int retval = -1;
    struct timeb rawtime;
    ftime(&rawtime);
    struct tm lm;
    if (gmtime_r(&rawtime.time, &lm) != 0
        && tm_to_str_millis(&lm, rawtime.millitm, mblk,
                            TIMESTAMP_LEN_WITH_MILLIS) == 0) {
        retval = TIMESTAMP_LEN_WITH_MILLIS;
    }
    return retval;
}

static int prepend_time(const char *wr_ptr)
{
    char *d_off = (char *) wr_ptr;
    int s_len = 0;
    int t_len = 0;
    s_len = set_time_stamp(d_off);
    if (s_len > 0) {
        d_off += s_len;
        t_len += s_len;
    }
    memcpy(d_off, "  ", 2);
    d_off += 2;
    t_len += 2;
    return t_len;

}

int send_debug_message(const char *formatstr, ...)
{
    int result = -1;
    va_list argp;
    char *wr_ptr = calloc(1000, 1);
    char *d_off = wr_ptr;
    int t_len = 0;
    if (wr_ptr != 0) {
        t_len = prepend_time(wr_ptr);
        d_off += t_len;
        int nspace = 1000 - t_len;
        va_start(argp, formatstr);
        result = vsnprintf(d_off, nspace, formatstr, argp);
        va_end(argp);

        if (result >= nspace) {
            // Need to reallocate 
            free(wr_ptr);
            wr_ptr = 0;
            wr_ptr = calloc(result + t_len + 2, 1);
            d_off = wr_ptr;
            t_len = prepend_time(wr_ptr);
            d_off += t_len;
            va_start(argp, formatstr);
            result = vsnprintf(d_off, nspace, formatstr, argp);
            va_end(argp);
        }
        struct rolling_log_message *m1 =
            malloc(sizeof(struct rolling_log_message));
        m1->data = wr_ptr;
        m1->len = t_len + result;
        m1->name = calloc(6, 1);
        memcpy(m1->name, "DEBUG", 5);
        m1->name_len = 5;
        m1->file_name = calloc(6, 1);
        memcpy(m1->file_name, "DEBUG", 5);
        m1->file_name_len = 5;
        if (dart_rolling_log(m1) == -1) {
            result = -1;
            free(m1->data);
            free(m1->name);
            free(m1->file_name);
            free(m1);
        }
    }
    return result;
}

int write_debug_message(const void *source, size_t nbytes)
{
    int ret_code = -1;
    return ret_code;
}
