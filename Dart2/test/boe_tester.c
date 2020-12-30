#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/uio.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include "tests.h"
#include "testmacros.h"
#include "databuf.h"
#include "rom_to_boe.h"
#include "boe_master.h"
#include "boe_utils.h"


//int trans_mat_ymd(char* year_mon, int ymlen, char* day, int dlen)
int test_boe_sym_len(const char **testname, const char **extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    REQCOND(buff != 0);
    int len = find_cfe_base_sym("VX49Z7", 6);    
    CHKCOND(len == 2);
    len = find_cfe_base_sym("488803566", 9);    
    CHKCOND(len == 9);
    len = find_cfe_base_sym("VXTZ7", 5);
    CHKCOND(len == 3);
    len = find_cfe_base_sym("VX", 2);
    CHKCOND(len == 2);
    len = find_cfe_base_sym("VXT", 3);
    CHKCOND(len == 3);
    len = find_cfe_base_spread_sym("VXH8-VXZ8", 9);
    CHKCOND(len == 2);
    len = find_cfe_base_spread_sym("123456789", 9);
    CHKCOND(len == 8);
    databuf_destroy(buff);
    STOPTEST;
}
int test_boe_mat_ymd(const char **testname, const char **extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    REQCOND(buff != 0);
    int date = trans_mat_ymd("201712", 6, "3", 1); 
    CHKCOND(date == 20171203);
    date = trans_mat_ymd("201803", 6, "14", 2);
    CHKCOND(date == 20180314);
    databuf_destroy(buff);
    STOPTEST;
}

int test_boe_price_trans(const char** testname, const char** extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    REQCOND(buff != 0);

    int new_price = rom_to_boe_price("12.34", 5);
    CHKCOND(new_price == 123400);
    new_price = rom_to_boe_price("-12.34", 6);
    CHKCOND(new_price == -123400);
    databuf_destroy(buff);
    STOPTEST;
}


int test_boe_is_present(const char** testname, const char** extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    REQCOND(buff != 0);
    uint8_t cur_bit = 0x4;
    uint32_t is_present = 0x0;
    IS_BIT_SET(is_present, cur_bit, 0x2);
    CHKCOND(is_present == 4);
    databuf_destroy(buff);
    STOPTEST;
}
