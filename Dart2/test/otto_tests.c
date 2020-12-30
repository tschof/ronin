#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/uio.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include "tests.h"
#include "testmacros.h"
#include "databuf.h"
#include "otto.h"
#include "otto_parser.h"

int test_otto_sym_len(const char **testname, const char **extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    REQCOND(buff != 0);
    databuf_memcpy(buff, "APL     ", 8);
    int len = 8;
    ouch_find_token_len(buff->rd_ptr, len);
    CHKCOND(len == 3);
    databuf_reset(buff);
    databuf_memcpy(buff, "    ", 4);
    len = 4;
    ouch_find_token_len(buff->rd_ptr, len);
    CHKCOND(len == 0);
    databuf_reset(buff);
    databuf_memcpy(buff, "YGNT     UGH,", 13);
    len = 9;
    ouch_find_token_len(buff->rd_ptr, len);
    CHKCOND(len == 4);
    databuf_reset(buff);
    databuf_memcpy(buff, "C     ", 6);
    len = 6;
    ouch_find_token_len(buff->rd_ptr, len);
    CHKCOND(len == 1);
    databuf_reset(buff);
    databuf_memcpy(buff, " HGH  T", 7);
    len = 7;
    ouch_find_token_len(buff->rd_ptr, len);
    CHKCOND(len == 7);
    databuf_reset(buff);
    databuf_memcpy(buff, " HG      ", 9);
    len = 9;
    ouch_find_token_len(buff->rd_ptr, len);
    CHKCOND(len == 3);
    databuf_destroy(buff);
    STOPTEST;
}

int test_otto_byte_swap(const char **testname, const char** extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    REQCOND(buff != 0);
    int64_t swapper = 984567500000;
    int64_t bestrike = Swap8Bytes(swapper);
    int64_t strike = Swap8Bytes(bestrike);
    CHKCOND(strike == swapper);
    swapper = 0x1;
    bestrike = Swap8Bytes(swapper);
    CHKCOND(bestrike == 0x100000000000000);
    swapper = -0x1;
    bestrike = Swap8Bytes(swapper);
    CHKCOND(bestrike == -0x1);
    strike = Swap8Bytes(bestrike);
    CHKCOND(strike == -1);

    swapper = -1;
    bestrike = ntohll(swapper);
    CHKCOND(bestrike == -1);

    swapper = -45760000;
    bestrike = Swap8Bytes(swapper);
    CHKCOND(bestrike == 54683102705876991);
    strike = Swap8Bytes(bestrike);
    CHKCOND(strike == -45760000);
    char* d = "-45.76";
    int len = 6;

    int64_t tester = rom_to_otto_price(d, len);
    CHKCOND(tester == 0xc245fdffffffff);
    len = otto_to_rom_price(tester, buff);
    CHKCOND(strncmp(buff->rd_ptr, "-45.76", 6) == 0);
    databuf_destroy(buff);
    STOPTEST;
}

int test_otto_instrument_parsing(const char **testname, const char **extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    REQCOND(buff != 0);
    ottop* otp = create_empty_otto_parser();
    ottosid osid;
    osid.productid = 12;
    memcpy(&osid.productname, "AAPL         ", 13);
    memcpy(&osid.securitysym, "AAPL ", 5);
    osid.instrumentid = 45;
    osid.expiryear = 17;
    osid.expirday = 12;
    osid.expirmon = 5;
    osid.strikeprice = -9164279849571844096;//150000000;
    osid.optiontype = 'P';
    create_instrument_sym(otp, &osid);
    uint32_t id = get_instrument_with_sec_def(otp, "AAPL__170512P00150000", 21);
    CHKCOND(id == 45);
    char* num = "00157500";
    int first = atoi(num);
    double second = first/1000;
    int third = atoi(num);
    double div = 1000.0;
    double fourth = ((double)third)/div;
    printf("with no cast: %f, with cast: %f \n", second, fourth);
    CHKCOND(fourth > second);

    num = "00097500";
    first = atoi(num);
    second = first/1000;
    third = atoi(num);
    fourth = ((double)third)/div;
    printf("with no cast: %f, with cast: %f \n", second, fourth);
    CHKCOND(fourth > second);

    databuf_destroy(buff);
    STOPTEST;
}
