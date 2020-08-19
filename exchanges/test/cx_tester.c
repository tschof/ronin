#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "tests.h"
#include "testmacros.h"
#include "cx_object.h"
#include "databuf.h"
#include "dart_constants.h"
#include "transaction_recorder.h"

static time_t trade_time_test = 0;
static databuf_t *rom_trade_checker = 0;
static databuf_t *rom_ack_checker = 0;
static char ack_message[300];
static char *trade_message = 0;

static void create_time_str(databuf_t * output)
{
    struct tm lm;
    struct tm *t;
    t = gmtime_r(&trade_time_test, &lm);
    t = &lm;
    databuf_write(output, ",%04d%02d%02d-%02d:%02d:%02d,,,,",
                  (t->tm_year + 1900), (t->tm_mon + 1), t->tm_mday,
                  t->tm_hour, t->tm_min, t->tm_sec);
}

void test_send_to_rom(char *message, int len, char *rom, int rom_len)
{
}

void test_send_cancelled(char *message, int len, char *rom, int rom_len)
{
    databuf_write(rom_ack_checker,
                  "S,,,,,,,,,,,,,,4,22,33,ROM-ORDER-TAG,,,,,,,,,System cancel,,,,,,,,,,,,,,,,,,,,\n");
    //printf("%s%s", message, rom_ack_checker->buffer);
    memset(ack_message, 0, 300);
    memcpy(ack_message, message, len);
}

void test_send_replace_cnl(char *message, int len, char *rom, int rom_len)
{
    databuf_write(rom_ack_checker,
                  "S,,,,,,,,,,,,,,4,22,,ROM-ORDER-TAG,,,,,,,,,Order Not Active,,,,,,,,,,,,,,,,,,,,\n");
    //printf("%s%s", message, rom_ack_checker->buffer);
    memset(ack_message, 0, 300);
    memcpy(ack_message, message, len);
}

void test_send_replace_rej(char *message, int len, char *rom, int rom_len)
{
    databuf_write(rom_ack_checker,
                  "S,,,,,,,,,,,,,,20,33,,ROM-ORDER-TAG,,,,,,,,,Invalid Client Order Id,,,,,,,,,,,,,,,,,,,,\n");
//    printf("%s%s", message, rom_ack_checker->buffer);
    memset(ack_message, 0, 300);
    memcpy(ack_message, message, len);
}

void test_send_replaced(char *message, int len, char *rom, int rom_len)
{
    databuf_write(rom_ack_checker,
                  "S,,,,,,,,,,,,,,5,22,,ROM-ORDER-TAG,,,,,,,,,Confirmed,,,,,,,,,,,,,,,,,,,,\n");
//    printf("%s%s", message, rom_ack_checker->buffer);
    memset(ack_message, 0, 300);
    memcpy(ack_message, message, len);
}

void test_send_cancel_rej(char *message, int len, char *rom, int rom_len)
{
    databuf_write(rom_ack_checker,
                  "S,,,,,,,,,,,,,,14,33,,ROM-ORDER-TAG,,,,,,,,,Invalid Credit,,,,,,,,,,,,,,,,,,,,\n");
    //   printf("%s%s", message, rom_ack_checker->buffer);
    memset(ack_message, 0, 300);
    memcpy(ack_message, message, len);
}

void test_send_ack_rej(char *message, int len, char *rom, int rom_len)
{
    databuf_write(rom_ack_checker,
                  "S,,,,,,,,,,,,,,8,22,33,ROM-ORDER-TAG,,,,,,,,,Invalid Credit,,,,,,,,,,,,,,,,,,,,\n");
    //  printf("%s%s", message, rom_ack_checker->buffer);
    memset(ack_message, 0, 300);
    memcpy(ack_message, message, len);
}

void test_send_ack_to_rom(char *message, int len, char *rom, int rom_len)
{
    databuf_write(rom_ack_checker,
                  "S,,,,,,,,,,,,,,13,22,33,ROM-ORDER-TAG,,,,,,,,,Confirmed,,,,,,,,,,,,,,,,,,,,\n");
//    printf("%s%s", message, rom_ack_checker->buffer);
    memset(ack_message, 0, 300);
    memcpy(ack_message, message, len);
}

void test_send_trade_to_rom(char *message, int len, char *rom, int rom_len)
{
    databuf_write(rom_trade_checker,
                  "S,,,,,,,,,,,,,,2,22,33,ROM-ORDER-TAG,,,,,,,,,,,,,,,,,,,,,,,,,1000000.00,,1.411190,,,,,0.00");
    create_time_str(rom_trade_checker);
    databuf_write(rom_trade_checker,
                  "{thekpdksaltdkprnte},,,,,,,,,,,,,,,1,,,,,,,,,\n");
    trade_message = message;
    //printf("%s%s\n", trade_message, rom_trade_checker->buffer);
}

int test_cx_ii_reply(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *buff = databuf_alloc(2000);
    REQCOND(buff != 0);
    cx_obj *obj = create_cx_obj("", 4, "", 4, 42, 19, 19, 0, "DO", 2, NULL,
                                test_send_to_rom);
    databuf_write(buff, "%c", FIX_FIELD_DELIM);
    int seq = 1;
    int cut_con = 0;
    int t_stamp = 1;
    char m_type = 'D';
    int session = 3;
    short ii = 3;
    char i_type = '1';
    char *i_id = "EUR/USD             ";
    long settle_date = 30000L;
    struct rec_init *rec = malloc(sizeof(struct rec_init));
    app_end_time *t = malloc(sizeof(struct app_end_time));
    if (rec == NULL) {
        return -1;
    }
    t->end_hour = 23;
    t->end_min = 23;
    t->end_day = 7;
    rec->t = t;
    rec->create_new = 0;
    rec->timeout_value = 30;
    start_recorder(rec);
    free(rec);
    databuf_memcpy(buff, &seq, 4);
    databuf_memcpy(buff, &t_stamp, 4);
    databuf_memcpy(buff, &m_type, 1);
    databuf_memcpy(buff, &session, 4);
    databuf_memcpy(buff, &ii, 2);
    databuf_memcpy(buff, &i_type, 1);
    databuf_memcpy(buff, i_id, 20);
    databuf_memcpy(buff, &settle_date, 8);
    databuf_write(buff, "%c", ETX);
    int len = databuf_unread(buff);
    unsigned long ret_val =
        frame_cx_mess(obj, buff->buffer, len, &cut_con, NULL);
    CHKCOND(len == ret_val);
    destroy_cx_obj(obj);
    databuf_destroy(buff);
STOPTEST}

int test_cancel_rej(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *buff = databuf_alloc(2000);
    rom_ack_checker = databuf_alloc(300);
    REQCOND(buff != 0);
    databuf_write(buff, "%c", FIX_FIELD_DELIM);
    dart_obj_manager *dom = startup_allocator(10);
    dart_ex_order_obj *doj = get_empty_dart_order_obj(dom);
    cx_obj *obj = create_cx_obj("", 4, "", 4, 42, 19, 19, 0, "DO", 2, NULL,
                                test_send_cancel_rej);
    doa_set_rom_id(doj, "ROM-ORDER-TAG", 13);
    doa_set_sender_name(doj, "ROM1", 4);
    int seq = 1;
    int cut_con = 0;
    int t_stamp = 1;
    char m_type = 'O';
    int cl_ord = 22;
    int ord_id = 33;
    short ec = 11;
    add_order_to_maps(obj, doj, ord_id);
    seq = htonl(seq);
    t_stamp = htonl(t_stamp);
    databuf_memcpy(buff, &seq, 4);
    databuf_memcpy(buff, &t_stamp, 4);
    databuf_memcpy(buff, &m_type, 1);
    cl_ord = htonl(cl_ord);
    ord_id = htonl(ord_id);
    databuf_memcpy(buff, &cl_ord, 4);
    databuf_memcpy(buff, &ord_id, 4);
    ec = htons(ec);
    databuf_memcpy(buff, &ec, 2);
    databuf_write(buff, "%c", ETX);
    int len = databuf_unread(buff);
    unsigned long ret_val =
        frame_cx_mess(obj, buff->buffer, len, &cut_con, NULL);
    CHKCOND(len == ret_val);
    CHKCOND(strcmp(ack_message, rom_ack_checker->buffer) == 0);
    destroy_cx_obj(obj);
    clean_dart_oo_pool(dom);
    databuf_destroy(buff);
    databuf_destroy(rom_ack_checker);
    rom_ack_checker = 0;
STOPTEST}

int test_cx_order_replace_cnl(const char **testname,
                              const char **extrareporting)
{
    STARTTEST databuf_t *buff = databuf_alloc(2000);
    rom_ack_checker = databuf_alloc(300);
    REQCOND(buff != 0);
    databuf_write(buff, "%c", FIX_FIELD_DELIM);
    dart_obj_manager *dom = startup_allocator(10);
    dart_ex_order_obj *doj = get_empty_dart_order_obj(dom);
    cx_obj *obj = create_cx_obj("", 4, "", 4, 42, 19, 19, 0, "DO", 2, NULL,
                                test_send_replace_cnl);
    doa_set_rom_id(doj, "ROM-ORDER-TAG", 13);
    doa_set_sender_name(doj, "ROM1", 4);
    int seq = 1;
    int cut_con = 0;
    int t_stamp = 1;
    char m_type = 'Q';
    int cl_ord = 22;
    int ord_id = 33;
    short ec = 14;
    add_order_to_maps(obj, doj, ord_id);
    seq = htonl(seq);
    t_stamp = htonl(t_stamp);
    databuf_memcpy(buff, &seq, 4);
    databuf_memcpy(buff, &t_stamp, 4);
    databuf_memcpy(buff, &m_type, 1);
    cl_ord = htonl(cl_ord);
    ord_id = htonl(ord_id);
    databuf_memcpy(buff, &cl_ord, 4);
    databuf_memcpy(buff, &ord_id, 4);
    databuf_memcpy(buff, "C", 1);
    ec = htons(ec);
    databuf_memcpy(buff, &ec, 2);
    databuf_write(buff, "%c", ETX);
    int len = databuf_unread(buff);
    unsigned long ret_val =
        frame_cx_mess(obj, buff->buffer, len, &cut_con, NULL);
    CHKCOND(len == ret_val);
    CHKCOND(strcmp(ack_message, rom_ack_checker->buffer) == 0);
    destroy_cx_obj(obj);
    clean_dart_oo_pool(dom);
    databuf_destroy(buff);
    databuf_destroy(rom_ack_checker);
    rom_ack_checker = 0;
STOPTEST}

int test_cx_order_replace_rej(const char **testname,
                              const char **extrareporting)
{
    STARTTEST databuf_t *buff = databuf_alloc(2000);
    rom_ack_checker = databuf_alloc(300);
    REQCOND(buff != 0);
    databuf_write(buff, "%c", FIX_FIELD_DELIM);
    dart_obj_manager *dom = startup_allocator(10);
    dart_ex_order_obj *doj = get_empty_dart_order_obj(dom);
    cx_obj *obj = create_cx_obj("", 4, "", 4, 42, 19, 19, 0, "DO", 2, NULL,
                                test_send_replace_rej);
    doa_set_rom_id(doj, "ROM-ORDER-TAG", 13);
    doa_set_sender_name(doj, "ROM1", 4);
    int seq = 1;
    int cut_con = 0;
    int t_stamp = 1;
    char m_type = 'Q';
    int cl_ord = 22;
    int ord_id = 33;
    short ec = 10;
    doa_set_cl_ord_id(doj, &ord_id, 4);
    doa_set_rep_ord_id(doj, &cl_ord, 4);
    add_new_id_to_tree(obj, doj, ord_id);
    add_order_to_maps(obj, doj, ord_id);
    seq = htonl(seq);
    t_stamp = htonl(t_stamp);
    databuf_memcpy(buff, &seq, 4);
    databuf_memcpy(buff, &t_stamp, 4);
    databuf_memcpy(buff, &m_type, 1);
    cl_ord = htonl(cl_ord);
    ord_id = htonl(ord_id);
    databuf_memcpy(buff, &cl_ord, 4);
    databuf_memcpy(buff, &ord_id, 4);
    databuf_memcpy(buff, "R", 1);
    ec = htons(ec);
    databuf_memcpy(buff, &ec, 2);
    databuf_write(buff, "%c", ETX);
    int len = databuf_unread(buff);
    unsigned long ret_val =
        frame_cx_mess(obj, buff->buffer, len, &cut_con, NULL);
    CHKCOND(len == ret_val);
    CHKCOND(strcmp(ack_message, rom_ack_checker->buffer) == 0);
    cl_ord = ntohl(cl_ord);
    ord_id = ntohl(ord_id);
    CHKCOND(get_ord_from_tree(obj, cl_ord) == 0);
    CHKCOND(get_ord_from_tree(obj, ord_id) != 0);
    destroy_cx_obj(obj);
    clean_dart_oo_pool(dom);
    databuf_destroy(buff);
    databuf_destroy(rom_ack_checker);
    rom_ack_checker = 0;
STOPTEST}

int test_rb_tree_crush(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *buff = databuf_alloc(2000);
    rom_ack_checker = databuf_alloc(300);
    REQCOND(buff != 0);
    dart_obj_manager *dom = startup_allocator(10);
    dart_ex_order_obj *doj = get_empty_dart_order_obj(dom);
    cx_obj *obj = create_cx_obj("", 4, "", 4, 42, 19, 19, 0, "DO", 2, NULL,
                                test_send_replace_rej);
    doa_set_rom_id(doj, "ROM-ORDER-TAG", 13);
    doa_set_sender_name(doj, "ROM1", 4);
    int i = 1;
    for (i = 1; i < 100000; ++i) {
        add_new_id_to_tree(obj, doj, i);
        CHKCOND(get_ord_from_tree(obj, i) != 0);
    }
    for (i = 100001; i < 200001; ++i) {
        add_new_id_to_tree(obj, doj, i);
        remove_from_tree(obj, (i - 100000));
        CHKCOND(get_ord_from_tree(obj, i) != 0);
        CHKCOND(get_ord_from_tree(obj, (i - 100000)) == 0);
    }
    for (i = 100001; i < 200001; ++i) {
        remove_from_tree(obj, i);
        CHKCOND(get_ord_from_tree(obj, i) == 0);
        if (i < 199990) {
            int j = i + 1;
            CHKCOND(get_ord_from_tree(obj, j) != 0);
        }
    }


    destroy_cx_obj(obj);
    clean_dart_oo_pool(dom);
    databuf_destroy(buff);
    databuf_destroy(rom_ack_checker);
    rom_ack_checker = 0;
STOPTEST}

int test_cx_order_replaced(const char **testname,
                           const char **extrareporting)
{
    STARTTEST databuf_t *buff = databuf_alloc(2000);
    rom_ack_checker = databuf_alloc(300);
    REQCOND(buff != 0);
    databuf_write(buff, "%c", FIX_FIELD_DELIM);
    dart_obj_manager *dom = startup_allocator(10);
    dart_ex_order_obj *doj = get_empty_dart_order_obj(dom);
    cx_obj *obj = create_cx_obj("", 4, "", 4, 42, 19, 19, 0, "DO", 2, NULL,
                                test_send_replaced);
    doa_set_rom_id(doj, "ROM-ORDER-TAG", 13);
    doa_set_sender_name(doj, "ROM1", 4);
    int seq = 1;
    int cut_con = 0;
    int t_stamp = 1;
    char m_type = 'Q';
    int cl_ord = 22;
    int ord_id = 33;
    short ec = 0;
    doa_set_cl_ord_id(doj, &ord_id, 4);
    doa_set_rep_ord_id(doj, &cl_ord, 4);
    add_new_id_to_tree(obj, doj, cl_ord);
    add_order_to_maps(obj, doj, ord_id);
    seq = htonl(seq);
    t_stamp = htonl(t_stamp);
    databuf_memcpy(buff, &seq, 4);
    databuf_memcpy(buff, &t_stamp, 4);
    databuf_memcpy(buff, &m_type, 1);
    cl_ord = htonl(cl_ord);
    ord_id = htonl(ord_id);
    databuf_memcpy(buff, &cl_ord, 4);
    databuf_memcpy(buff, &ord_id, 4);
    databuf_memcpy(buff, "P", 1);
    ec = htons(ec);
    databuf_memcpy(buff, &ec, 2);
    databuf_write(buff, "%c", ETX);
    int len = databuf_unread(buff);
    unsigned long ret_val =
        frame_cx_mess(obj, buff->buffer, len, &cut_con, NULL);
    CHKCOND(len == ret_val);
    CHKCOND(strcmp(ack_message, rom_ack_checker->buffer) == 0);
    cl_ord = ntohl(cl_ord);
    ord_id = ntohl(ord_id);
    dart_ex_order_obj *toj = get_ord_from_tree(obj, ord_id);
    CHKCOND(get_ord_from_tree(obj, cl_ord) != 0);
    CHKCOND(toj == 0);
    destroy_cx_obj(obj);
    clean_dart_oo_pool(dom);
    databuf_destroy(buff);
    databuf_destroy(rom_ack_checker);
    rom_ack_checker = 0;
STOPTEST}

int test_cx_order_canclled(const char **testname,
                           const char **extrareporting)
{
    STARTTEST databuf_t *buff = databuf_alloc(2000);
    rom_ack_checker = databuf_alloc(300);
    REQCOND(buff != 0);
    databuf_write(buff, "%c", FIX_FIELD_DELIM);
    dart_obj_manager *dom = startup_allocator(10);
    dart_ex_order_obj *doj = get_empty_dart_order_obj(dom);
    cx_obj *obj = create_cx_obj("", 4, "", 4, 42, 19, 19, 0, "DO", 2, NULL,
                                test_send_cancelled);
    doa_set_rom_id(doj, "ROM-ORDER-TAG", 13);
    doa_set_sender_name(doj, "ROM1", 4);
    int seq = 1;
    int cut_con = 0;
    int t_stamp = 1;
    char m_type = 'R';
    int cl_ord = 22;
    long ord_id = 33;
    short ec = 1;
    doa_set_cl_ord_id(doj, &ord_id, 4);
    doa_set_rep_ord_id(doj, &cl_ord, 4);
    add_order_to_maps(obj, doj, cl_ord);
    add_new_id_to_tree(obj, doj, ord_id);
    seq = htonl(seq);
    t_stamp = htonl(t_stamp);
    databuf_memcpy(buff, &seq, 4);
    databuf_memcpy(buff, &t_stamp, 4);
    databuf_memcpy(buff, &m_type, 1);
    cl_ord = htonl(cl_ord);
    ord_id = htonll(ord_id);
    databuf_memcpy(buff, &cl_ord, 4);
    databuf_memcpy(buff, &ord_id, 8);
    databuf_memcpy(buff, "C", 1);
    ec = htons(ec);
    databuf_memcpy(buff, &ec, 2);
    databuf_write(buff, "%c", ETX);
    int len = databuf_unread(buff);
    unsigned long ret_val =
        frame_cx_mess(obj, buff->buffer, len, &cut_con, NULL);
    CHKCOND(len == ret_val);
    CHKCOND(strcmp(ack_message, rom_ack_checker->buffer) == 0);
    cl_ord = ntohl(cl_ord);
    CHKCOND(get_ord_from_tree(obj, cl_ord) == 0);
    destroy_cx_obj(obj);
    clean_dart_oo_pool(dom);
    databuf_destroy(buff);
    databuf_destroy(rom_ack_checker);
    rom_ack_checker = 0;
STOPTEST}

int test_cx_order_ack_rej(const char **testname,
                          const char **extrareporting)
{
    STARTTEST databuf_t *buff = databuf_alloc(2000);
    rom_ack_checker = databuf_alloc(300);
    REQCOND(buff != 0);
    databuf_write(buff, "%c", FIX_FIELD_DELIM);
    dart_obj_manager *dom = startup_allocator(10);
    dart_ex_order_obj *doj = get_empty_dart_order_obj(dom);
    cx_obj *obj = create_cx_obj("", 4, "", 4, 42, 19, 19, 0, "DO", 2, NULL,
                                test_send_ack_rej);
    doa_set_rom_id(doj, "ROM-ORDER-TAG", 13);
    doa_set_sender_name(doj, "ROM1", 4);
    int seq = 1;
    int cut_con = 0;
    int t_stamp = 1;
    char m_type = 'M';
    int cl_ord = 22;
    long ord_id = 33;
    short ec = 11;
    doa_set_cl_ord_id(doj, &ord_id, 4);
    doa_set_rep_ord_id(doj, &cl_ord, 4);
    add_order_to_maps(obj, doj, cl_ord);
    add_new_id_to_tree(obj, doj, ord_id);
    seq = htonl(seq);
    t_stamp = htonl(t_stamp);
    databuf_memcpy(buff, &seq, 4);
    databuf_memcpy(buff, &t_stamp, 4);
    databuf_memcpy(buff, &m_type, 1);
    cl_ord = htonl(cl_ord);
    ord_id = htonll(ord_id);
    databuf_memcpy(buff, &cl_ord, 4);
    databuf_memcpy(buff, &ord_id, 8);
    databuf_memcpy(buff, "R", 1);
    ec = htons(ec);
    databuf_memcpy(buff, &ec, 2);
    databuf_write(buff, "%c", ETX);
    int len = databuf_unread(buff);
    unsigned long ret_val =
        frame_cx_mess(obj, buff->buffer, len, &cut_con, NULL);
    CHKCOND(len == ret_val);
    CHKCOND(strcmp(ack_message, rom_ack_checker->buffer) == 0);
    destroy_cx_obj(obj);
    clean_dart_oo_pool(dom);
    databuf_destroy(buff);
    databuf_destroy(rom_ack_checker);
    rom_ack_checker = 0;
STOPTEST}

int test_cx_order_ack(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *buff = databuf_alloc(2000);
    rom_ack_checker = databuf_alloc(300);
    REQCOND(buff != 0);
    databuf_write(buff, "%c", FIX_FIELD_DELIM);
    dart_obj_manager *dom = startup_allocator(10);
    dart_ex_order_obj *doj = get_empty_dart_order_obj(dom);
    cx_obj *obj = create_cx_obj("", 4, "", 4, 42, 19, 19, 0, "DO", 2, NULL,
                                test_send_ack_to_rom);
    doa_set_rom_id(doj, "ROM-ORDER-TAG", 13);
    doa_set_sender_name(doj, "ROM1", 4);
    int seq = 1;
    int cut_con = 0;
    int t_stamp = 1;
    char m_type = 'M';
    int cl_ord = 22;
    long ord_id = 33;
    short ec = 0;
    add_order_to_maps(obj, doj, cl_ord);
    seq = htonl(seq);
    t_stamp = htonl(t_stamp);
    databuf_memcpy(buff, &seq, 4);
    databuf_memcpy(buff, &t_stamp, 4);
    databuf_memcpy(buff, &m_type, 1);
    cl_ord = htonl(cl_ord);
    ord_id = htonll(ord_id);
    databuf_memcpy(buff, &cl_ord, 4);
    databuf_memcpy(buff, &ord_id, 8);
    databuf_memcpy(buff, "C", 1);
    ec = htons(ec);
    databuf_memcpy(buff, &ec, 2);
    databuf_write(buff, "%c", ETX);
    int len = databuf_unread(buff);
    unsigned long ret_val =
        frame_cx_mess(obj, buff->buffer, len, &cut_con, NULL);
    CHKCOND(len == ret_val);
    CHKCOND(strcmp(ack_message, rom_ack_checker->buffer) == 0);
    destroy_cx_obj(obj);
    clean_dart_oo_pool(dom);
    databuf_destroy(buff);
    databuf_destroy(rom_ack_checker);
    rom_ack_checker = 0;
STOPTEST}

int test_cx_trade(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *buff = databuf_alloc(2000);
    rom_trade_checker = databuf_alloc(300);
    REQCOND(buff != 0);
    databuf_write(buff, "%c", FIX_FIELD_DELIM);
    dart_obj_manager *dom = startup_allocator(10);
    dart_ex_order_obj *doj = get_empty_dart_order_obj(dom);
    cx_obj *obj = create_cx_obj("", 4, "", 4, 42, 19, 19, 0, "DO", 2, NULL,
                                test_send_trade_to_rom);
    doa_set_rom_id(doj, "ROM-ORDER-TAG", 13);
    doa_set_sender_name(doj, "ROM1", 4);
    int seq = 1;
    int cut_con = 0;
    int t_stamp = 1;
    char m_type = 'T';
    int cl_ord = 22;
    long ord_id = 33;
    short ii = 3;
    long fill_amt = 100000000;
    int fill_rate = 141119;
    char *exec_broke = "DUH!";
    char *exec_id = "{thekpdksaltdkprnte}";
    long settle_date = 4;
    long trade_date = 5;
    time_t now;
    long leaves_amt = 0;
    add_order_to_maps(obj, doj, cl_ord);
    seq = htonl(seq);
    t_stamp = htonl(t_stamp);
    databuf_memcpy(buff, &seq, 4);
    databuf_memcpy(buff, &t_stamp, 4);
    databuf_memcpy(buff, &m_type, 1);
    cl_ord = htonl(cl_ord);
    ord_id = htonll(ord_id);
    ii = htons(ii);
    databuf_memcpy(buff, &cl_ord, 4);
    databuf_memcpy(buff, &ord_id, 8);
    databuf_memcpy(buff, &ii, 2);
    databuf_memcpy(buff, "B", 1);
    fill_amt = htonll(fill_amt);
    databuf_memcpy(buff, &fill_amt, 8);
    fill_rate = htonl(fill_rate);
    databuf_memcpy(buff, &fill_rate, 4);
    databuf_memcpy(buff, exec_broke, 4);
    databuf_memcpy(buff, exec_id, 20);
    databuf_memcpy(buff, "1", 1);
    settle_date = htonll(settle_date);
    databuf_memcpy(buff, &settle_date, 8);
    trade_date = htonll(trade_date);
    databuf_memcpy(buff, &trade_date, 8);
    time(&now);
    trade_time_test = now;
    now = now * 1000;
    now = htonll(now);
    databuf_memcpy(buff, &now, 8);
    leaves_amt = htonll(leaves_amt);
    databuf_memcpy(buff, &leaves_amt, 8);
    databuf_memcpy(buff, "1", 1);
    databuf_write(buff, "%c", ETX);
    int len = databuf_unread(buff);
    unsigned long ret_val =
        frame_cx_mess(obj, buff->buffer, len, &cut_con, NULL);
    CHKCOND(len == ret_val);
    CHKCOND(strcmp(trade_message, rom_trade_checker->buffer) == 0);
    destroy_cx_obj(obj);
    clean_dart_oo_pool(dom);
    databuf_destroy(buff);
    databuf_destroy(rom_ack_checker);
    rom_ack_checker = 0;
STOPTEST}

int test_cx_frame(const char **testname, const char **extrareporting)
{
    int cut_con = 0;
    STARTTEST databuf_t *buff = databuf_alloc(1000);
    databuf_write(buff, "%c%s%c", FIX_FIELD_DELIM, "{THIS IS -THE MIDDLE}",
                  ETX);
    REQCOND(buff != 0);
    unsigned long len = databuf_unread(buff);
    cx_obj *obj = create_cx_obj("", 4, "", 4, 42, 19, 19, 0, "DO", 2, NULL,
                                test_send_to_rom);
    unsigned long ret_val = frame_cx_mess(obj, buff->buffer, len,
                                          &cut_con, NULL);
    CHKCOND(0 == ret_val);
    databuf_reset(buff);
    databuf_write(buff, "%c%s%c%c%s%c%c%s%c%c%s%c", FIX_FIELD_DELIM,
                  "{VAL1---C---}", ETX, FIX_FIELD_DELIM, "{VAL2---C---}",
                  ETX, FIX_FIELD_DELIM, "{VAL3---C---}", ETX,
                  FIX_FIELD_DELIM, "{VAL4---C---}", ETX);
    len = databuf_unread(buff);
    ret_val = frame_cx_mess(obj, buff->buffer, len, &cut_con, NULL);
    CHKCOND(len == ret_val);
    databuf_destroy(buff);
STOPTEST}

int test_ntohll_htonll(const char **testname, const char **extrareporting)
{
    STARTTEST long null_show = 0x7FFFFFFF;
    long fake = 666039381293;
    REQCOND(fake == 666039381293);
    REQCOND(null_show > 0);
    long n_fake = htonll(fake);
    CHKCOND(n_fake != fake);
    long h_fake = ntohll(n_fake);
    CHKCOND(h_fake = fake);
//    printf("%ld,%ld,%ld,%ld", fake, n_fake, h_fake, null_show);
STOPTEST}

/*
 *
    void rom_to_cx_amount(databuf_t* buff, long qty);
    long cx_to_rom_amount(char* data);
    void rom_to_cx_price(databuf_t* buff, char* price, size_t len);
    double cx_to_rom_price(char* data);
 */
int test_amount_trans(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *buff = databuf_alloc(100);
    REQCOND(buff != 0);
    rom_to_cx_amount(buff, 1000000.00);
    long tester = htonll(100000000L);
    long a = 0;
    memcpy(&a, buff->buffer, 8);
    CHKCOND(a == tester);
    cx_to_rom_amount(buff->buffer);

    databuf_reset(buff);
    rom_to_cx_amount(buff, .01);
    tester = htonll(1L);
    a = 0;
    memcpy(&a, buff->buffer, 8);
    CHKCOND(a == tester);

    databuf_reset(buff);
    rom_to_cx_amount(buff, .1);
    tester = htonll(10L);
    a = 0;
    memcpy(&a, buff->buffer, 8);
    CHKCOND(a == tester);
    double ret = cx_to_rom_amount(buff->buffer);
    databuf_reset(buff);
    cx_double_to_char_amount(buff, ret);
    CHKCOND(strcmp(buff->buffer, "0.10") == 0);
    databuf_destroy(buff);
STOPTEST}

int test_price_trans(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *buff = databuf_alloc(100);
    REQCOND(buff != 0);
    rom_to_cx_price(buff, ".1", 2);
    int tester = htonl(10000);
    int a = 0;
    memcpy(&a, buff->buffer, 4);
    CHKCOND(a == tester);
    double d = cx_to_rom_price(buff->buffer);
    databuf_reset(buff);
    cx_double_to_char_price(buff, d);
    CHKCOND(strcmp(buff->buffer, "0.100000") == 0);
    databuf_destroy(buff);
STOPTEST}
