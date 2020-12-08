#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include "tests.h"
#include "testmacros.h"
#include "globex_trans.h"
#include "globex.h"
#include "databuf.h"
#include "dart_order_obj.h"
#include "order_obj_fact.h"
#include "dart_constants.h"

int test_product_code(const char **testname, const char **extrareporting)
{
    STARTTEST;
    REQCOND(1 != 0);
    size_t len = get_symbol_len("ESU1", 4);
    CHKCOND(len == 2);
    len = get_symbol_len("ESU1-ESH1", 9);
    CHKCOND(len == 2);
    len = get_symbol_len("EW1", 3);
    CHKCOND(len == 3);
    len = get_symbol_len("OZBU1 C1290", 11);
    CHKCOND(len == 3);
    len = get_symbol_len("YYYYY", 5);
    CHKCOND(len == 5);
    len = get_symbol_len("12345", 5);
    CHKCOND(len == 5);
    len = get_symbol_len("UUUU1 C1290", 11);
    CHKCOND(len == 3);
    len = get_symbol_len("ABCC1 C1290", 11);
    CHKCOND(len == 11);
    len = get_symbol_len("UUUUP C1290", 11);
    CHKCOND(len == 5);
    len = get_symbol_len("12345 C1290", 11);
    CHKCOND(len == 5);
    len = get_symbol_len("ES", 2);
    CHKCOND(len == 2);
    len = get_symbol_len("ESU1,asfaskdjkj kj;kj;", 4);
    CHKCOND(len == 2);
    len = get_symbol_len("UD:1N: VT 1102942397", 20);
    CHKCOND(len == 6);
    len = get_symbol_len("UD:L1:CRR VT 1102942397", 20);
    CHKCOND(len == 9);
    STOPTEST;
}
int test_rom_to_cme_price(const char **testname, const char **extrareporting)
{
    STARTTEST;
    databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    oofact* ofact =  create_order_obj_factory(4);
    dart_order_obj *doj = get_doj(ofact);
    set_rom_field(doj, ROM_PRICE, "-0.061", 6);
    set_rom_to_cme_price(doj, ROM_PRICE, 1000,test_b);
    CHKCOND(strncmp("-0061", test_b->buffer, 5) == 0);
    databuf_reset(test_b);
    set_rom_field(doj, ROM_PRICE, "0.009325", 8);
    set_rom_to_cme_price(doj, ROM_PRICE, 1000000,test_b);
    CHKCOND(strncmp("0009325", test_b->buffer, 7) == 0);
    databuf_reset(test_b);
    set_rom_field(doj, ROM_PRICE, "-0.65", 5);
    set_rom_to_cme_price(doj, ROM_PRICE, 1000,test_b);
    CHKCOND(strncmp("-0650", test_b->buffer, 5) == 0);
    STOPTEST;
}
int test_cme_to_rom_price(const char **testname, const char **extrareporting)
{
    STARTTEST;
    databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    oofact* ofact =  create_order_obj_factory(4);
    dart_order_obj *doj = get_doj(ofact);
    set_rom_field(doj, ROM_PRICE, "-61", 3);
    set_cme_to_rom(doj, ROM_PRICE, 1000,test_b);
    CHKCOND(strncmp("-.061", test_b->buffer, 6) == 0);
    databuf_reset(test_b);
    set_rom_field(doj, ROM_EXEC_PRICE, "3248.97", 7);
    set_cme_to_rom(doj, ROM_EXEC_PRICE, 10,test_b);
    CHKCOND(strncmp("324.897", test_b->buffer, 7) == 0);
    databuf_reset(test_b);
    set_rom_field(doj, ROM_EXEC_PRICE, "9325", 4);
    set_cme_to_rom(doj, ROM_EXEC_PRICE, 1000000,test_b);
    CHKCOND(strncmp(".009325", test_b->buffer, 7) == 0);
    STOPTEST;
}
