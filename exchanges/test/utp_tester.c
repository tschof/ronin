#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include "tests.h"
#include "testmacros.h"
#include "databuf.h"
#include "dart_constants.h"
#include "transaction_recorder.h"
#include "utp_object.h"
#include "utpdirect.h"

static databuf_t *rom_trade_checker = 0;
static databuf_t *rom_ack_checker = 0;

int test_utp_order_ack(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *buff = databuf_alloc(2000);
    REQCOND(buff != 0);
    char* dodo = calloc(18, 1);
    memcpy(dodo, "RT 9999/20120403", 16);
    int len = find_utp_field_len( dodo, 17);
    CHKCOND(len == 16);
    char* dodo2 = calloc(18, 1);
    memcpy(dodo2, "RTO 9999/20120403", 17);
    len = find_utp_field_len(dodo2, 17);
    CHKCOND(len == 17);
    char* dodo3 = calloc(19, 1);
    memcpy(dodo3, "RTO 9999/20120403H", 18);
    len = find_utp_field_len(dodo3, 17);
    CHKCOND(len == 17);
    databuf_destroy(buff);
STOPTEST}

int test_utp_cancel_rej(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *buff = databuf_alloc(2000);
    rom_ack_checker = databuf_alloc(300);
    REQCOND(buff != 0);
    databuf_destroy(buff);
    databuf_destroy(rom_ack_checker);
    rom_ack_checker = 0;
STOPTEST}

int test_utp_order_replace_cnl(const char **testname,
                              const char **extrareporting)
{
    STARTTEST databuf_t *buff = databuf_alloc(2000);
    rom_ack_checker = databuf_alloc(300);
    REQCOND(buff != 0);
    databuf_destroy(buff);
    databuf_destroy(rom_ack_checker);
    rom_ack_checker = 0;
STOPTEST}

int test_utp_order_replace_rej(const char **testname,
                              const char **extrareporting)
{
    STARTTEST databuf_t *buff = databuf_alloc(2000);
    rom_ack_checker = databuf_alloc(300);
    REQCOND(buff != 0);
    databuf_destroy(buff);
    databuf_destroy(rom_ack_checker);
    rom_ack_checker = 0;
STOPTEST}

int test_utp_order_replaced(const char **testname,
                           const char **extrareporting)
{
    STARTTEST databuf_t *buff = databuf_alloc(2000);
    rom_ack_checker = databuf_alloc(300);
    REQCOND(buff != 0);
    databuf_destroy(buff);
    databuf_destroy(rom_ack_checker);
    rom_ack_checker = 0;
STOPTEST}

int test_utp_order_canclled(const char **testname,
                           const char **extrareporting)
{
    STARTTEST databuf_t *buff = databuf_alloc(2000);
    rom_ack_checker = databuf_alloc(300);
    REQCOND(buff != 0);
    databuf_destroy(buff);
    databuf_destroy(rom_ack_checker);
    rom_ack_checker = 0;
STOPTEST}

int test_utp_order_ack_rej(const char **testname,
                          const char **extrareporting)
{
    STARTTEST databuf_t *buff = databuf_alloc(2000);
    rom_ack_checker = databuf_alloc(300);
    REQCOND(buff != 0);
    databuf_destroy(buff);
    databuf_destroy(rom_ack_checker);
    rom_ack_checker = 0;
STOPTEST}
int test_utp_trade(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *buff = databuf_alloc(2000);
    rom_trade_checker = databuf_alloc(300);
    REQCOND(buff != 0);
    databuf_destroy(buff);
    databuf_destroy(rom_ack_checker);
    rom_ack_checker = 0;
STOPTEST}


int test_utp_time(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *buff = databuf_alloc(1000);
    databuf_t *fbuff = databuf_alloc(1000);
    char* mlbk = calloc(50, 1);
    char* fiver = calloc(50, 1);
    int one_oclock = 3600000;
    int five_oclock = 18000000;
    time_t rawtime;
    struct tm *ti;
    time(&rawtime);
    struct tm lm;
    ti = gmtime_r(&rawtime, &lm);
    ti = &lm;
    set_date_str(buff, one_oclock);
    set_date_str(fbuff, five_oclock);
    sprintf(mlbk, "%04d%02d%02d-05:00:00", ti->tm_year + 1900, ti->tm_mon + 1,
                   ti->tm_mday);
    sprintf(fiver, "%04d%02d%02d-09:00:00", ti->tm_year + 1900, ti->tm_mon + 1,
                   ti->tm_mday);
    CHKCOND(strcmp(buff->buffer, mlbk) == 0);
    CHKCOND(strcmp(fbuff->buffer, fiver) == 0);
    databuf_destroy(buff);
STOPTEST}
/*
 *
 */

int test_utp_price_trans(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *buff = databuf_alloc(100);
    REQCOND(buff != 0);
    int a = rom_to_utp_price(".1", 2);
    a = htonl(a);
    int tester = htonl(1000);
    CHKCOND(a == tester);
    double d = utp_to_rom_price(a, 4);
    databuf_write(buff,"%f", d);
    CHKCOND(strcmp(buff->buffer, "0.100000") == 0);
    databuf_reset(buff);
    int i = 100;
    double j = .01;
    int t = 0;
    int len;
    char flake[30];
    memset(flake, '\0', 30);
    for(; t < 80000; ++t) {
       len = sprintf(flake, "%f", j);
       a = rom_to_utp_price(flake, len);
       a = htonl(a);
       tester = htonl(i);
       CHKCOND(a == tester);
       i += 100;
       j += .01;
    }
    
    databuf_destroy(buff);
STOPTEST}

