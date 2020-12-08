#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include "rom_to_boe.h"
#include "boe.h"
#include "boe_seq.h"
#include "tests.h"
#include "testmacros.h"
#include "databuf.h"
#include "dart_constants.h"
#include "transaction_recorder.h"
#include "boe_parser.h"
#include "order_obj_fact.h"
#include "dart_order_obj.h"
#include "rom_handler.h"
#include "connection_interface.h"

int test_boe_bit_count(const char **testname, const char **extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    REQCOND(buff != 0);
    databuf_destroy(buff);
    unsigned int is_set = 0x0;
    unsigned char val = 0x0;
    //unsigned char ones = 0xf;
    unsigned char flag = 1;
    unsigned char mask = 2;
    SET_BIT(flag, mask, val);
    CHKCOND(val != 0x0);
    CHKCOND(val == 0x2);
    flag = 0x0;
    SET_BIT(flag, mask, val);
    CHKCOND(val == 0x0);
    CHKCOND(val != 0x2);
    flag = 0x1;
    val = 0x3;
    SET_BIT(flag, 0xC, val);
    CHKCOND(val != 0x3);
    CHKCOND(val == 0xF);
    SET_BIT(flag, 0x20,val);
    CHKCOND(val != 0xF);
    CHKCOND(val == 0x2F);
    flag = 0x0;
    SET_BIT(flag, 0xFF, val);
    CHKCOND(val != 0x2F);
    CHKCOND(val == 0x00);
    IS_BIT_SET(is_set, val, 3);
    CHKCOND(is_set == 0x0);
    flag = 0x1;
    SET_BIT(flag, 0x8, val);
    CHKCOND(val != 0x0);
    CHKCOND(val == 0x8);
    SET_BIT(flag, 0x2, val);
    CHKCOND(val != 0x1);
    CHKCOND(val == 0xA);
    IS_BIT_SET(is_set, val, 3);
    CHKCOND(is_set == 0x8);
    IS_BIT_SET(is_set, val, 2);
    CHKCOND(is_set == 0x0);

    char replace_three = 0xD8;
    is_set = 0x0;
    IS_BIT_SET(is_set, replace_three, 0);
    CHKCOND(is_set == 0x0);
    IS_BIT_SET(is_set, replace_three, 1);
    CHKCOND(is_set == 0x0);
    IS_BIT_SET(is_set, replace_three, 2);
    CHKCOND(is_set == 0x0);
    IS_BIT_SET(is_set, replace_three, 3);
    CHKCOND(is_set > 0x0);
    IS_BIT_SET(is_set, replace_three, 4);
    CHKCOND(is_set > 0x0);
    IS_BIT_SET(is_set, replace_three, 5);
    CHKCOND(is_set == 0x0);
    IS_BIT_SET(is_set, replace_three, 6);
    CHKCOND(is_set > 0x0);
    IS_BIT_SET(is_set, replace_three, 7);
    CHKCOND(is_set > 0x0);
    replace_three = 0xb3;
    is_set = 0x0;
    IS_BIT_SET(is_set, replace_three, 0);
    CHKCOND(is_set > 0x0);
    IS_BIT_SET(is_set, replace_three, 1);
    CHKCOND(is_set > 0x0);
    IS_BIT_SET(is_set, replace_three, 2);
    CHKCOND(is_set == 0x0);
    IS_BIT_SET(is_set, replace_three, 3);
    CHKCOND(is_set == 0x0);
    IS_BIT_SET(is_set, replace_three, 4);
    CHKCOND(is_set > 0x0);
    IS_BIT_SET(is_set, replace_three, 5);
    CHKCOND(is_set > 0x0);
    IS_BIT_SET(is_set, replace_three, 6);
    CHKCOND(is_set == 0x0);
    IS_BIT_SET(is_set, replace_three, 7);
    CHKCOND(is_set > 0x0);

    STOPTEST;
}

int test_boe_time_parse(const char** testname, const char** extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    databuf_t *comper = databuf_alloc(2000);
    REQCOND(buff != 0);
    btk* b = create_boe_time_keeper("bats_suffix.map");
    time_t rawtime;
    time(&rawtime);
    struct tm* pt = gmtime(&rawtime);
    unsigned long boe_raw = rawtime * 1000000000;
    int blen = boe_to_rom_time(b, boe_raw, buff->rd_ptr);
    int len = sprintf(comper->rd_ptr, "%04d%02d%02d-%02d:%02d:%02d", (pt->tm_year + 1900), 
            (pt->tm_mon + 1), pt->tm_mday, pt->tm_hour, pt->tm_min, pt->tm_sec);
    CHKCOND(strncmp(comper->rd_ptr, buff->rd_ptr, 16) == 0);
    CHKCOND(len < blen);
    CHKCOND(blen == 24);
    boe_raw += 300000000;
    databuf_reset(buff);
    blen = boe_to_rom_time(b, boe_raw, buff->rd_ptr);
    CHKCOND(blen == 24);
    CHKCOND(strncmp(comper->rd_ptr, buff->rd_ptr, 16) == 0);
    boe_raw += 380729000;
    databuf_reset(buff);
    blen = boe_to_rom_time(b, boe_raw, buff->rd_ptr);
    CHKCOND(blen == 24);
    CHKCOND(strncmp("680729", buff->rd_ptr + 18, 6) == 0);
    STOPTEST;
}
static char *static_peg_stop_limit_gtd =
"E,20110523-15:44:05,PARNELL,PARNELL1104231044050001,1,ADSK,500,0.950000,4,6,A,0695,CRON067,24,,KLRTI-DIRC,,PARNELL-50110523-2,,0.99,,,USER0001,WAG,PARNELL,,8522820409900022,PARNELL,PARNELL-50110523-2-0,N,,,,,100,E,,,1,,TG4,24,,,,,,20180523-15:23:04,,,20110523-15:44:05,,,,,,,P,,,,,,V,-.03,,,,,,,,,,,,,,,,DODO\n";

static char* replace_mess = "R,20150424-15:18:57,PARNELL,PARNELL1104231044050001,1,ADSK,100,10.950000,,,,,,,,,,PARNELL-50110523-2,,,,,,,,,8440330350229,,,,,,,,,,,,,,,,,,,,,,,,20150424-15:18:57,,,,,,,,\n";
static char* cancel_mess = "C,20150424-15:18:57,PARNELL,PARNELL1104231044050001,1,JNJ,100,0.950000,,,,,,,,CR678H,,PARNELL-50110523-2,,,,,,,,,8440330350229,,,,,,,,,,,,,,,,,,,,,,,,20150424-15:18:57,,,,,,,,\n";

static char* bats_killer = "E,,PARNELL,REDSOXS06251426531-3,2,AVG/PRCWD,100,12.5,2,0,A,501,7YZ0,153,0,,,REDSOXS-420150625-415,,,,,DODO,GSEC,PARNELL,,,REDSOXS,REDSOXS-420150625-415-0,,,,,,,E,,,,,,153,,,,,,,,,,,20150625-19:26:53.425,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";



static dart_order_obj* create_boe_rom_mess(char* message)
{
    oofact* ofact =  create_order_obj_factory(4);
    dart_order_obj *doj = get_doj(ofact);
    rom_style_reset(doj);
    databuf_write(doj->head->data, message);
    ph_build_iovec(doj);
    return doj;
}

static con_interface* create_test_face()
{
    con_interface *con_int = create_empty_interface();
    con_int->cl_ord_id_map = create_map(8);
    con_int->sbm = create_service_bureau_matcher("IDtomnemonic.map");
    con_int->tg = generic_creation_func();
    con_int->parser = create_boe_time_keeper("bats_suffix.map");
    gatekeeper* gk = create_empty_gatekeeper(8, "BOE", 3,NULL, 1);
    con_int->gk = gk;
    return con_int;
}

int test_boe_killer(const char** testname, const char** extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    REQCOND(buff != 0);
    con_interface* con_int = create_test_face();
    dart_order_obj* doj = create_boe_rom_mess(bats_killer);
    int len = create_boe_new_order_message(buff, doj, 0,
            con_int);
    CHKCOND(len == 85);
    char* tester = buff->rd_ptr;
    CHKCOND(tester[30] == '2');
    short mlen = 0;
    memcpy(&mlen, tester + 2, 2);
    CHKCOND(mlen == 83);
    int qty = 0;
    memcpy(&qty, tester + 31, 4);
    CHKCOND(qty == 100);
    char one = 0x37;
    char two = 0x41;
    CHKCOND(tester[35] == 0x6);
    CHKCOND(tester[36] == one);
    CHKCOND(tester[37] == two);
    CHKCOND(tester[38] == 0x1);
    CHKCOND(tester[39] == 0x0);
    CHKCOND(tester[40] == 0x0);
    CHKCOND(tester[41] == 0x0);
    CHKCOND(strncmp((tester + 42), "DART", 4) == 0);
    CHKCOND(strncmp((tester + 46), "7YZ0", 4) == 0);
    long price_check = 0;
    memcpy(&price_check, (tester + 50), 8);
    CHKCOND(price_check == 125000);
    CHKCOND(tester[58] == '2');
    CHKCOND(tester[59] == '0');
    CHKCOND(strncmp((tester + 60), "AVG-C$", 6) == 0);
    CHKCOND(tester[68] == 'A');
    CHKCOND(strncmp((tester + 69), "7YZ0", 4) == 0);
    STOPTEST;
}

int test_boe_new_order(const char **testname, const char **extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    REQCOND(buff != 0);
    con_interface* con_int = create_test_face();
    dart_order_obj* doj = create_boe_rom_mess(static_peg_stop_limit_gtd);
    int len = create_boe_new_order_message(buff, doj, 0,
            con_int);
    CHKCOND(len == 131);
    char* tester = buff->rd_ptr;
    char b3 = 0xb3;
    char b2 = 0xc1;
    char b1 = 0xbf;
    CHKCOND(tester[35] == 0x06); 
    CHKCOND(tester[36] == b1); 
    CHKCOND(tester[37] == b2); 
    CHKCOND(tester[38] == b3); 
    CHKCOND(tester[39] == 0x0); 
    CHKCOND(tester[40] == 0x0); 
    CHKCOND(tester[41] == 0x0e); 
    CHKCOND(strncmp((tester + 42), "DEGS", 4) == 0);
    CHKCOND(strncmp((tester + 46), "N067", 4) == 0);
    long price_check = 0;
    memcpy(&price_check, (tester + 50), 8);
    CHKCOND(price_check == 9500);
    CHKCOND(tester[58] == 'P');
    CHKCOND(tester[59] == 'P');
    CHKCOND(tester[60] == '6');
    memcpy(&price_check, (tester + 61), 4);
    CHKCOND(price_check == 100);
    CHKCOND(strncmp((tester + 65), "ADSK", 4) == 0);
    short len_check = 0;
    memcpy(&len_check, (tester + 2), 2);
    CHKCOND(len_check == 129);
    CHKCOND(tester[73] == 'A');
    CHKCOND(strncmp((tester + 74), "KL", 2) == 0);
    CHKCOND(strncmp((tester + 78), "CRON067", 7) == 0);
    CHKCOND(tester[94] == 'V');
    memcpy(&price_check, (tester + 95), 8);
    CHKCOND(price_check == -300);
    CHKCOND(strncmp((tester + 103), "TG4", 3) == 0);
    unsigned long date = 0;
    memcpy(&date, (tester + 106), 8);
    CHKCOND(date == 1527088984000000000);
    memcpy(&price_check, (tester + 114), 8);
    CHKCOND(price_check == 9900);
    CHKCOND(strncmp((tester + 122), "DIRC", 4) == 0);
    CHKCOND(strncmp((tester + 128), "RTI", 3) == 0);
    time_t ret = date / 1000000000;
    struct tm *t = gmtime(&ret);
    CHKCOND((t->tm_year + 1900) == 2018);
    CHKCOND(t->tm_sec == 4);
    CHKCOND(t->tm_hour == 15);
    STOPTEST;
}

int test_boe_logon(const char **testname, const char **extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    REQCOND(buff != 0);
    async_parse_args* apa = calloc(1, sizeof(struct async_parse_args));
    apa->in_args = calloc(1, sizeof(struct init_args));
    apa->in_args->username = "DODO";
    apa->in_args->un_len = 4;
    apa->in_args->password = "SHIT";
    apa->in_args->p_len = 4;
    apa->in_args->sender_comp = "RATS";
    apa->in_args->sc_len = 4;
    bsm* seq_master = create_seq_master();
    create_boe_logon(buff, apa,seq_master);
    int len = databuf_unread(buff);
    char* tester = buff->rd_ptr;
    CHKCOND(len == 62);
    CHKCOND(tester[28] == 0x5);
    short mlen = 0;
    memcpy(&mlen, buff->rd_ptr + 2, 2);
    CHKCOND(mlen == 60);
    CHKCOND(strncmp("RATS", buff->rd_ptr + 10, 4) == 0);
    CHKCOND(strncmp("SHIT", buff->rd_ptr + 18, 4) == 0);
    CHKCOND(strncmp("DODO", buff->rd_ptr + 14, 4) == 0);
    unsigned char param_group_type = 0x81;
    unsigned char message_type = 0x25;
    CHKCOND(((unsigned char)tester[31]) == param_group_type);
    CHKCOND(tester[32] == message_type);
    CHKCOND(tester[33] == 0x0);
    memcpy(&mlen, buff->rd_ptr + 34, 2);
    CHKCOND(mlen == 0x5);
    CHKCOND(((unsigned char)tester[36]) == param_group_type);
    message_type = 0x26;
    CHKCOND(tester[37] == message_type);
    CHKCOND(tester[38] == 0x0);

    memcpy(&mlen, buff->rd_ptr + 39, 2);
    CHKCOND(mlen == 0xd);
    CHKCOND(((unsigned char)tester[41]) == param_group_type);
    message_type = 0x27;
    CHKCOND(tester[42] == message_type);
    CHKCOND(tester[43] == 0x8);
    message_type = 0x7f;
    CHKCOND(tester[44] == message_type);
    CHKCOND(tester[45] == 0x0);
    message_type = 0xd8;
    CHKCOND(((unsigned char)tester[46]) == message_type);
    CHKCOND(tester[47] == 0x0);
    message_type = 0xb3;
    CHKCOND(((unsigned char)tester[48]) == message_type);
    CHKCOND(tester[49] == 0x0);
    CHKCOND(tester[50] == 0x0);
    message_type = 0x3c;
    CHKCOND(((unsigned char)tester[51]) == message_type);

    memcpy(&mlen, tester + 52, 2);
    CHKCOND(mlen == 5);
    CHKCOND(((unsigned char)tester[54]) == param_group_type);
    message_type = 0x2a;
    CHKCOND(((unsigned char)tester[55]) == message_type);
    CHKCOND(tester[56] == 0x0);

    CHKCOND(((unsigned char)tester[59]) == param_group_type); 
    message_type = 0x2b;
    CHKCOND(((unsigned char)tester[60]) == message_type); 
    message_type = 0x2a;
    CHKCOND(((unsigned char)tester[55]) == message_type); 
    CHKCOND(((unsigned char)tester[54]) == param_group_type); 
    CHKCOND(tester[57] == 0x5); 
    STOPTEST;
}

int test_boe_cancel(const char **testname, const char **extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    REQCOND(buff != 0);
    con_interface* con_int = create_test_face();
    dart_order_obj* doj = create_boe_rom_mess(cancel_mess);
    int len = create_boe_cancel_message(buff, doj, 2,con_int);
    char* tester = buff->rd_ptr;
    short mlen = 0;
    memcpy(&mlen, tester + 2, 2);
    CHKCOND(mlen == 29);
    CHKCOND(len == 31);
    CHKCOND(strncmp(tester + 10, "CR678H", 6) == 0);
    STOPTEST;
}
int test_boe_replace(const char **testname, const char **extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    databuf_t *rbuff = databuf_alloc(2000);
    REQCOND(buff != 0);
    con_interface* con_int = create_test_face();
    dart_order_obj* doj = create_boe_rom_mess(static_peg_stop_limit_gtd);
    int nlen = create_boe_new_order_message(buff, doj, 0,
            con_int);
    CHKCOND(nlen == 131);
    char* tester = buff->rd_ptr;
    dart_order_obj* rep = create_boe_rom_mess(replace_mess);
    doj->rep = rep;
    int len = create_boe_replace_message(rbuff, doj, 1,con_int);
    char* rtester = rbuff->rd_ptr;
    CHKCOND(len == 69);
    char num_bits = 0x2;
    CHKCOND(rtester[50] == num_bits);
    CHKCOND(rtester[51] == 0xd);
    CHKCOND(rtester[52] == 0x0);
    CHKCOND(strncmp(rtester + 53, "N067", 4) == 0);
    CHKCOND(strncmp(rtester + 30, tester + 10, doj->positions[30].iov_len) == 0);
    CHKCOND(strncmp(rtester + 10, tester + 10, doj->positions[30].iov_len) != 0);
    long price_check = 0;
    memcpy(&price_check, (rtester + 57), 4);
    CHKCOND(price_check == 100);
    memcpy(&price_check, rtester + 61, 8);
    CHKCOND(price_check == 109500);
    STOPTEST;
}

static void create_mess_head(databuf_t* buff, char type,short mlen,
        int seq, long time, char* clordid, int len)
{
    *buff->wr_ptr++ = 0xBA;
    *buff->wr_ptr++ = 0xBA;
    databuf_memcpy(buff,&mlen, 2); 
    *buff->wr_ptr++ = 0x1;
    *buff->wr_ptr++ = type;
    databuf_memcpy(buff, &seq, 4);
    databuf_memcpy(buff, &time, 8);
    memcpy(buff->wr_ptr, clordid, len);
    buff->wr_ptr += 20;
}

int test_boe_parse_ack(const char** testname, const char** extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    databuf_t *comper = databuf_alloc(2000);
    memset(buff->rd_ptr,'\0', 1000);
    REQCOND(buff != 0);
    time_t rawtime;
    time(&rawtime);
    struct tm* pt = gmtime(&rawtime);
    unsigned long boe_raw = rawtime * 1000000000;
    boe_raw += 234235000;
    create_mess_head(buff, 0x25, 47,241, boe_raw, "RB764WE", 7);
    dart_order_obj* doj = create_boe_rom_mess(static_peg_stop_limit_gtd);
    btk* b = create_boe_time_keeper("bats_suffix.map");
    con_interface* con_int = create_test_face();
    con_int->parser = b;
    long ordid = 93456;
    databuf_memcpy(buff, &ordid, 8);
    boe_parse_order_ack(doj, buff->rd_ptr, con_int);
    CHKCOND(strncmp(doj->positions[32].iov_base, "93456", 5) == 0);
    CHKCOND(doj->positions[100].iov_len == 24);
    CHKCOND(strncmp(doj->positions[100].iov_base + 18, "234235", 6) == 0);
    int len = sprintf(comper->rd_ptr, "%04d%02d%02d-%02d:%02d:%02d", (pt->tm_year + 1900), 
            (pt->tm_mon + 1), pt->tm_mday, pt->tm_hour, pt->tm_min, pt->tm_sec);
    CHKCOND(strncmp(doj->positions[100].iov_base, comper->rd_ptr, len) == 0);
    STOPTEST;
}
int test_boe_parse_rej(const char** testname, const char** extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    databuf_t *comper = databuf_alloc(2000);
    memset(buff->rd_ptr,'\0', 1000);
    REQCOND(buff != 0);
    time_t rawtime;
    time(&rawtime);
    struct tm* pt = gmtime(&rawtime);
    unsigned long boe_raw = rawtime * 1000000000;
    boe_raw += 734289000;
    create_mess_head(buff, 0x25, 60,241, boe_raw, "TB964GE", 7);
    dart_order_obj* doj = create_boe_rom_mess(static_peg_stop_limit_gtd);
    btk* b = create_boe_time_keeper("bats_suffix.map");
    con_interface* con_int = create_test_face();
    con_int->parser = b;
    char ordid = 'G';
    char* message = "You suck so we rejected.";
    databuf_write(buff, "%c%s", ordid, message);
    boe_parse_rej(doj, buff->rd_ptr, con_int);
    CHKCOND(doj->positions[100].iov_len == 24);
    CHKCOND(strncmp(doj->positions[100].iov_base + 18, "734289", 6) == 0);
    int len = sprintf(comper->rd_ptr, "%04d%02d%02d-%02d:%02d:%02d", (pt->tm_year + 1900), 
            (pt->tm_mon + 1), pt->tm_mday, pt->tm_hour, pt->tm_min, pt->tm_sec);
    CHKCOND(strncmp(doj->positions[100].iov_base, comper->rd_ptr, len) == 0);
    CHKCOND(strncmp(doj->positions[52].iov_base,message,
                doj->positions[52].iov_len) == 0);
    STOPTEST;
}
int test_boe_parse_rep(const char** testname, const char** extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    REQCOND(buff != 0);
    STOPTEST;
}

static void set_fill_vals(databuf_t* buff,
        unsigned long exec_id, unsigned int last, long price,
        int leaves)
{
    databuf_memcpy(buff, &exec_id, 8);
    databuf_memcpy(buff, &last, 4);
    databuf_memcpy(buff, &price, 8);
    databuf_memcpy(buff, &leaves, 4);
    databuf_memcpy(buff, "AIBEX", 5);
}
int test_boe_parse_exe(const char** testname, const char** extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    databuf_t *comper = databuf_alloc(2000);
    memset(buff->rd_ptr,'\0', 1000);
    REQCOND(buff != 0);
    time_t rawtime;
    time(&rawtime);
    struct tm* pt = gmtime(&rawtime);
    unsigned long boe_raw = rawtime * 1000000000;
    boe_raw += 835271000;
    create_mess_head(buff, 0x25, 69,241, boe_raw, "XO964T2", 7);
    dart_order_obj* doj = create_boe_rom_mess(static_peg_stop_limit_gtd);
    btk* b = create_boe_time_keeper("bats_suffix.map");
    con_interface* con_int = create_test_face();
    con_int->parser = b;
    int intial_stuff = 0;
    fill_in_rom_field(doj, ROM_BIN_CUM, &intial_stuff, 4);
    intial_stuff = 3300;
    fill_in_rom_field(doj, ROM_BIN_QTY, &intial_stuff, 4);
    //set_fill_vals(buff, 800, 30, 1200, 19);
    set_fill_vals(buff, 8976534, 3281, 12450102, 19);
    boe_parse_filled(doj, buff->rd_ptr, con_int);
    CHKCOND(strncmp(doj->positions[100].iov_base + 18, "835271", 6) == 0);
    int len = sprintf(comper->rd_ptr, "%04d%02d%02d-%02d:%02d:%02d", (pt->tm_year + 1900), 
            (pt->tm_mon + 1), pt->tm_mday, pt->tm_hour, pt->tm_min, pt->tm_sec);
    CHKCOND(strncmp(doj->positions[100].iov_base, comper->rd_ptr, len) == 0);
    CHKCOND(strncmp(doj->positions[ROM_LIQ].iov_base, "AI", 2) == 0);
    CHKCOND(strncmp(doj->positions[ROM_EXEC_PRICE].iov_base, "1245.0102", 9) == 0);
    CHKCOND(strncmp(doj->positions[ROM_LAST_SHARES].iov_base, "3281", 4) == 0);
    int lscheck = 0;
    memcpy(&lscheck, doj->positions[ROM_BIN_LAST_SHARES].iov_base, 4);
    CHKCOND(lscheck == 3281);
    CHKCOND(strncmp(doj->positions[ROM_LEAVES].iov_base, "19", 2) == 0);
    CHKCOND(strncmp(doj->positions[EXEC_ID].iov_base, "8976534", 5) == 0);
    STOPTEST;
}
