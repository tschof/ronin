#include "tests.h"
#include "testmacros.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "rom_handler.h"
#include "rom_fix_trans_funcs.h"
#include "databuf.h"
#include "dart_constants.h"
static char *twenty_five =
"E,20110523-15:44:05,PARNELL,PARNELL1104231044050001,1,ADSK,100,0.950000,2,0,A,0551,RONIN,24,,,,PARNELL-50110523-2,,,,,USER0001,WAG,PARNELL,,8522820409900022,PARNELL,PARNELL-50110523-2-0,N,,,,,,E,,,1,,,24,,,,,,,,,20110523-15:44:05,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\nE,20110523-15:44:06,PARNELL,PARNELL1104231044060002,1,ADSK,100,0.950000,2,0,A,0551,RONIN,24,,,,PARNELL-50110523-3,,,,,USER0001,WAG,PARNELL,,8522822659789124,PARNELL,PARNELL-50110523-3-0,N,,,,,,E,,,1,,,24,,,,,,,,,20110523-15:44:06,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\nE,20110523-15:44:06,PARNELL,PARNELL1104231044060003,1,ADSK,100,0.950000,2,0,A,0551,RONIN,24,,,,PARNELL-50110523-4,,,,,USER0001,WAG,PARNELL,,8522824167845018,PARNELL,PARNELL-50110523-4-0,N,,,,,,E,,,1,,,24,,,,,,,,,20110523-15:44:06,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\nE,20110523-15:44:07,PARNELL,PARNELL1104231044070004,1,ADSK,100,0.950000,2,0,A,0551,RONIN,24,,,,PARNELL-50110523-5,,,,,USER0001,WAG,PARNELL,,8522825005646873,PARNELL,PARNELL-50110523-5-0,N,,,,,,E,,,1,,,24,,,,,,,,,20110523-15:44:07,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\nE,20110523-15:44:07,PARNELL,PARNELL1104231044070005,1,ADSK,100,0.950000,2,0,A,0551,RONIN,24,,,,PARNELL-50110523-6,,,,,USER0001,WAG,PARNELL,,8522826418566206,PARNELL,PARNELL-50110523-6-0,N,,,,,,E,,,1,,,24,,,,,,,,,20110523-15:44:07,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\nC,20110523-15:44:08,PARNELL,PARNELL1104231044050001,1,ADSK,100,0.950000,,,,,,,,,,PARNELL-50110523-2,,,,,,,,,8522830775056184,PARNELL,PARNELL-50110523-2-2,,,,,,,,,,,,,,,,,,,,,,20110523-15:44:08,,,,,,,,,,PARNELL1104231044080006,\nC,20110523-15:44:08,PARNELL,PARNELL1104231044060002,1,ADSK,100,0.950000,,,,,,,,,,PARNELL-50110523-3,,,,,,,,,8522830775293748,PARNELL,PARNELL-50110523-3-2,,,,,,,,,,,,,,,,,,,,,,20110523-15:44:08,,,,,,,,,,PARNELL1104231044080007,\nC,20110523-15:44:08,PARNELL,PARNELL1104231044060003,1,ADSK,100,0.950000,,,,,,,,,,PARNELL-50110523-4,,,,,,,,,8522830775592107,PARNELL,PARNELL-50110523-4-2,,,,,,,,,,,,,,,,,,,,,,20110523-15:44:08,,,,,,,,,,PARNELL1104231044080008,\nC,20110523-15:44:08,PARNELL,PARNELL1104231044070004,1,ADSK,100,0.950000,,,,,,,,,,PARNELL-50110523-5,,,,,,,,,8522830775760011,PARNELL,PARNELL-50110523-5-2,,,,,,,,,,,,,,,,,,,,,,20110523-15:44:08,,,,,,,,,,PARNELL1104231044080009,\nC,20110523-15:44:08,PARNELL,PARNELL1104231044070005,1,ADSK,100,0.950000,,,,,,,,,,PARNELL-50110523-6,,,,,,,,,8522830775950307,PARNELL,PARNELL-50110523-6-2,,,,,,,,,,,,,,,,,,,,,,20110523-15:44:08,,,,,,,,,,PARNELL1104231044080010,\nE,20110523-15:44:10,PARNELL,PARNELL1104231044100011,1,ADSK,100,0.950000,2,0,A,0551,RONIN,24,,,,PARNELL-50110523-7,,,,,USER0001,WAG,PARNELL,,8522836784609597,PARNELL,PARNELL-50110523-7-0,N,,,,,,E,,,1,,,24,,,,,,,,,20110523-15:44:10,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\nR,20110523-15:44:18,PARNELL,PARNELL1104231044100011,1,ADSK,100,0.960000,2,0,A,,,,,,,PARNELL-50110523-7,,,,,,,PARNELL,,8522858404328021,PARNELL,PARNELL-50110523-7-2,,,,,,,,,,,,,,,100,,,,,0,100,20110523-15:44:18,,,,,,,,,,PARNELL1104231044180017,,,,,,,,,,,,,,,,,,,\nC,20110523-15:44:21,PARNELL,PARNELL1104231044180017,1,ADSK,100,0.950000,,,,,,,,,,PARNELL-50110523-7,,,,,,,,,8522868483176975,PARNELL,PARNELL-50110523-7-4,,,,,,,,,,,,,,,,,,,,,,20110523-15:44:21,,,,,,,,,,PARNELL1104231044210018,\nE,20110523-15:44:24,PARNELL,PARNELL1104231044240019,1,ADSK,100,0.950000,2,0,A,0551,RONIN,24,,,,PARNELL-50110523-8,,,,,USER0001,WAG,PARNELL,,8522875759394159,PARNELL,PARNELL-50110523-8-0,N,,,,,,E,,,1,,,24,,,,,,,,,20110523-15:44:24,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\nE,20110523-15:44:24,PARNELL,PARNELL1104231044240020,1,ADSK,100,0.950000,2,0,A,0551,RONIN,24,,,,PARNELL-50110523-9,,,,,USER0001,WAG,PARNELL,,8522877267819080,PARNELL,PARNELL-50110523-9-0,N,,,,,,E,,,1,,,24,,,,,,,,,20110523-15:44:24,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\nE,20110523-15:44:24,PARNELL,PARNELL1104231044240021,1,ADSK,100,0.950000,2,0,A,0551,RONIN,24,,,,PARNELL-50110523-10,,,,,USER0001,WAG,PARNELL,,8522877770493299,PARNELL,PARNELL-50110523-10-0,N,,,,,,E,,,1,,,24,,,,,,,,,20110523-15:44:24,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\nE,20110523-15:44:24,PARNELL,PARNELL1104231044240022,1,ADSK,100,0.950000,2,0,A,0551,RONIN,24,,,,PARNELL-50110523-11,,,,,USER0001,WAG,PARNELL,,8522878153592231,PARNELL,PARNELL-50110523-11-0,N,,,,,,E,,,1,,,24,,,,,,,,,20110523-15:44:24,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\nR,20110523-15:44:30,PARNELL,PARNELL1104231044240019,1,ADSK,100,0.980000,2,0,A,,,,,,,PARNELL-50110523-8,,,,,,,PARNELL,,8522896588003268,PARNELL,PARNELL-50110523-8-2,,,,,,,,,,,,,,,100,,,,,0,100,20110523-15:44:30,,,,,,,,,,PARNELL1104231044300024,,,,,,,,,,,,,,,,,,,\nR,20110523-15:44:30,PARNELL,PARNELL1104231044240020,1,ADSK,100,0.980000,2,0,A,,,,,,,PARNELL-50110523-9,,,,,,,PARNELL,,8522896588249706,PARNELL,PARNELL-50110523-9-2,,,,,,,,,,,,,,,100,,,,,0,100,20110523-15:44:30,,,,,,,,,,PARNELL1104231044300025,,,,,,,,,,,,,,,,,,,\nR,20110523-15:44:30,PARNELL,PARNELL1104231044240021,1,ADSK,100,0.980000,2,0,A,,,,,,,PARNELL-50110523-10,,,,,,,PARNELL,,8522896588548398,PARNELL,PARNELL-50110523-10-2,,,,,,,,,,,,,,,100,,,,,0,100,20110523-15:44:30,,,,,,,,,,PARNELL1104231044300026,,,,,,,,,,,,,,,,,,,\nR,20110523-15:44:30,PARNELL,PARNELL1104231044240022,1,ADSK,100,0.980000,2,0,A,,,,,,,PARNELL-50110523-11,,,,,,,PARNELL,,8522896588811513,PARNELL,PARNELL-50110523-11-2,,,,,,,,,,,,,,,100,,,,,0,100,20110523-15:44:30,,,,,,,,,,PARNELL1104231044300027,,,,,,,,,,,,,,,,,,,\nE,20110523-15:44:35,PARNELL,PARNELL1104231044350028,2,ADSK,100,0.980000,2,0,A,0551,RONIN,24,,,,PARNELL-50110523-12,,,,,USER0001,WAG,PARNELL,,8522911430636066,PARNELL,PARNELL-50110523-12-0,N,,,,,,E,,,1,,,24,,,,,,,,,20110523-15:44:35,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\nE,20110523-15:44:36,PARNELL,PARNELL1104231044360029,2,ADSK,100,0.980000,2,0,A,0551,RONIN,24,,,,PARNELL-50110523-13,,,,,USER0001,WAG,PARNELL,,8522914566766157,PARNELL,PARNELL-50110523-13-0,N,,,,,,E,,,1,,,24,,,,,,,,,20110523-15:44:36,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\nC,20110523-15:44:38,PARNELL,PARNELL1104231044300026,1,ADSK,100,0.950000,,,,,,,,,,PARNELL-50110523-10,,,,,,,,,8522920599765497,PARNELL,PARNELL-50110523-10-4,,,,,,,,,,,,,,,,,,,,,,20110523-15:44:38,,,,,,,,,,PARNELL1104231044380033,\nC,20110523-15:44:38,PARNELL,PARNELL1104231044300027,1,ADSK,100,0.950000,,,,,,,,,,PARNELL-50110523-11,,,,,,,,,8522920599980399,PARNELL,PARNELL-50110523-11-4,,,,,,,,,,,,,,,,,,,,,,20110523-15:44:38,,,,,,,,,,PARNELL1104231044380034,\n";

static char *twenty_five_checker =
    "A,,,,,,,,,,,,\nA,,,,,,,,,,,,\nA,,,,,,,,,,,,\nA,,,,,,,,,,,,\nA,,,,,,,,,,,,\nC,20110523-15:44:08,PARNELL,PARNELL1104231044050001,1,ADSK,100,0.950000,,,,,,,,,,PARNELL-50110523-2,,,,,,,,,8522830775056184,PARNELL,PARNELL-50110523-2-2,,,,,,,,,,,,,,,,,,,,,,20110523-15:44:08,,,,,,,,,,PARNELL1104231044080006,\nC,20110523-15:44:08,PARNELL,PARNELL1104231044060002,1,ADSK,100,0.950000,,,,,,,,,,PARNELL-50110523-3,,,,,,,,,8522830775293748,PARNELL,PARNELL-50110523-3-2,,,,,,,,,,,,,,,,,,,,,,20110523-15:44:08,,,,,,,,,,PARNELL1104231044080007,\nC,20110523-15:44:08,PARNELL,PARNELL1104231044060003,1,ADSK,100,0.950000,,,,,,,,,,PARNELL-50110523-4,,,,,,,,,8522830775592107,PARNELL,PARNELL-50110523-4-2,,,,,,,,,,,,,,,,,,,,,,20110523-15:44:08,,,,,,,,,,PARNELL1104231044080008,\nC,20110523-15:44:08,PARNELL,PARNELL1104231044070004,1,ADSK,100,0.950000,,,,,,,,,,PARNELL-50110523-5,,,,,,,,,8522830775760011,PARNELL,PARNELL-50110523-5-2,,,,,,,,,,,,,,,,,,,,,,20110523-15:44:08,,,,,,,,,,PARNELL1104231044080009,\nC,20110523-15:44:08,PARNELL,PARNELL1104231044070005,1,ADSK,100,0.950000,,,,,,,,,,PARNELL-50110523-6,,,,,,,,,8522830775950307,PARNELL,PARNELL-50110523-6-2,,,,,,,,,,,,,,,,,,,,,,20110523-15:44:08,,,,,,,,,,PARNELL1104231044080010,\nA,,,,,,,,,,,,\nA,,,,,,,,,,,,\nC,20110523-15:44:21,PARNELL,PARNELL1104231044180017,1,ADSK,100,0.950000,,,,,,,,,,PARNELL-50110523-7,,,,,,,,,8522868483176975,PARNELL,PARNELL-50110523-7-4,,,,,,,,,,,,,,,,,,,,,,20110523-15:44:21,,,,,,,,,,PARNELL1104231044210018,\nA,,,,,,,,,,,,\nA,,,,,,,,,,,,\nA,,,,,,,,,,,,\nA,,,,,,,,,,,,\nA,,,,,,,,,,,,\nA,,,,,,,,,,,,\nA,,,,,,,,,,,,\nA,,,,,,,,,,,,\nA,,,,,,,,,,,,\nA,,,,,,,,,,,,\nC,20110523-15:44:38,PARNELL,PARNELL1104231044300026,1,ADSK,100,0.950000,,,,,,,,,,PARNELL-50110523-10,,,,,,,,,8522920599765497,PARNELL,PARNELL-50110523-10-4,,,,,,,,,,,,,,,,,,,,,,20110523-15:44:38,,,,,,,,,,PARNELL1104231044380033,\nC,20110523-15:44:38,PARNELL,PARNELL1104231044300027,1,ADSK,100,0.950000,,,,,,,,,,PARNELL-50110523-11,,,,,,,,,8522920599980399,PARNELL,PARNELL-50110523-11-4,,,,,,,,,,,,,,,,,,,,,,20110523-15:44:38,,,,,,,,,,PARNELL1104231044380034,\n";


int r_get_is_test_con()
{
    return 1;
}

char *r_test_id_gen(size_t * len)
{
    return NULL;
}

int test_rom_pad_one(const char **testname, const char **extrareporting)
{
    STARTTEST;
    databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    databuf_write(test_b,
                  "%s\n"
                  "E,20110523-15:44:05,PARNELL,PARNELL1104231044050001,1,ADSK,100,0.950000,2,0,A,0551,RONIN,24,,,,PARNELL-50110523-2,,,,,USER0001,WAG,PARNELL,,8522820409900022,PARNELL,PARNELL-50110523-2-0,N,,,,,,E,,,1,,,24,,,,,,,,,20110523-15:44:05,,,,,,,,,,,,,,,,,,,,,,,,,,,,,");
    char *result = pad_rom_resend(test_b->buffer, databuf_unread(test_b));
    CHKCOND(strcmp(result, "A,,,,,,,,,,,,\n") == 0);
    STOPTEST;
}

int test_rom_pad_twenty_five(const char **testname,
                             const char **extrareporting)
{
    STARTTEST;
    databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    databuf_write(test_b, "%s", twenty_five);
    char *result = pad_rom_resend(test_b->buffer, databuf_unread(test_b));
    CHKCOND(result[0] == 'A');
    CHKCOND(strcmp(result, twenty_five_checker) == 0);
    STOPTEST;
}

char *rom_message =
    "E,20110523-15:44:05,PARNELL,PARNELL1104231044050001,1,ADSK,\n";
char *rom_message2 = "E,20110523-15:44:05,PARNELL,PARNELL1104231044050001,1,ADSK,,,,,,,,,,,,DODO,\n";   //17
int test_build_rom_pos(const char **testname, const char **extrareporting)
{
    STARTTEST;
    databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    STOPTEST;
}
void no_op_callback(dart_order_obj* doj, void* ob)
{
    //do nothing
}
/**
 *
 *
 */
int test_ph_parser(const char **testname, const char **extrareporting)
{
    STARTTEST;
    databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);

    gatekeeper * gate = create_gatekeeper(10,
                                          "DODO", 4,
                                          NULL, 1, "dest_route_tbl.map",NULL);
    sock_recv_manager* srm = create_sock_recv_manager();
    srm->buffer = twenty_five;
    srm->rd_ptr = twenty_five;
    srm->wr_ptr = twenty_five + 262;
    async_parse_args* apa = calloc(1, sizeof(struct async_parse_args));
    apa->gk = gate;
    apa->con = srm;
    apa->parse_v = ph_build_non_fix_rom_pos;
    apa->obj_callback = no_op_callback;
    apa->in_args = calloc(1, sizeof(struct init_args));
    apa->is_logged_on = 1;
    int cut_con = 0;
    ph_parse_rom_message(&cut_con,apa);
    CHKCOND(srm->rd_ptr[0] == 'E');
    CHKCOND(srm->rd_ptr - srm->buffer == 259);
    srm->wr_ptr += 10;
    ph_parse_rom_message(&cut_con, apa);
    CHKCOND(srm->rd_ptr - srm->buffer == 259);
    CHKCOND(srm->wr_ptr - srm->rd_ptr == 13);
    srm->wr_ptr += 10;
    ph_parse_rom_message(&cut_con, apa);
    CHKCOND(srm->rd_ptr - srm->buffer == 259);
    CHKCOND(srm->wr_ptr - srm->rd_ptr == 23);
    srm->wr_ptr += 100;
    ph_parse_rom_message(&cut_con, apa);
    CHKCOND(srm->rd_ptr - srm->buffer == 259);
    CHKCOND(srm->wr_ptr - srm->rd_ptr == 123);
    srm->wr_ptr += 100;
    ph_parse_rom_message(&cut_con, apa);
    CHKCOND(srm->rd_ptr - srm->buffer == 259);
    CHKCOND(srm->wr_ptr - srm->rd_ptr == 223);
    srm->wr_ptr += 100;
    ph_parse_rom_message(&cut_con, apa);
    CHKCOND(srm->rd_ptr - srm->buffer == 518);
    CHKCOND(srm->wr_ptr - srm->rd_ptr == 64);
    srm->rd_ptr = srm->buffer;
    ph_parse_rom_message(&cut_con, apa);
    CHKCOND(srm->rd_ptr - srm->buffer == 518);
    CHKCOND(srm->wr_ptr - srm->rd_ptr == 64);

    STOPTEST;
}

#ifdef _SPREAD_N_READY_

static char *spread_direct = "E,,JMEISNER,JMEISNER081109013588,1,,50,-0.31,2,0,B,695,CRON067,220,,,,,,,N,Y,BRACK,ABN,BRACK,,,,,,,,,NFLX,,O,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,JMEISNER081109013588,3,,,,,,,,,,,,!#!E,,JMEISNER,JMEISNER081109013588-1,1,NFLX,1,,2,0,B,695,CRON067,220,,,,,,,N,Y,BRACK,ABN,BRACK,,,,100,,201508,P,114,NFLX,,O,,,1,,,,,,,,,,,,,,,,,NFLX__150821P00114000,,,,,,,21,,,JMEISNER081109013588,6,,,,,,,,,,,,!#!E,,JMEISNER,JMEISNER081109013588-2,2,NFLX,1,,2,0,B,695,CRON067,220,,,,,,,N,Y,BRACK,ABN,BRACK,,,,100,,201508,P,112,NFLX,,O,,,1,,,,,,,,,,,,,,,,,NFLX__150828P00112000,,,,,,,28,,,JMEISNER081109013588,6,,,,,,,,,,,,!#!\n";
static char* spread_from_rom = "E,,JMEISNER,JMEISNER081109013588,1,,50,-0.31,2,0,B,695,CRON067,220,,,,JMEISNER-10150811-19779,,,N,Y,BRACK,ABN,BRACK,,,JMEISNER,JMEISNER-10150811-19779-0,,,,,NFLX,,O,,,,,,220,,,,,,,,,,,20150811-14:01:35.797,,,,,,,,,,,,,JMEISNER081109013588,3,,,,,,,,,,,,!#!E,,JMEISNER,JMEISNER081109013588-1,1,NFLX,1,,2,0,B,695,CRON067,220,,,,JMEISNER-10150811-19780,,,N,Y,BRACK,ABN,BRACK,,,JMEISNER,JMEISNER-10150811-19780-0,,201508,P,114,NFLX,,O,,,1,,,,,,,,,,,,,,20150811-14:01:35.797,,,NFLX__150821P00114000,,,,,,,21,,,JMEISNER081109013588,6,,,,JMEISNER-10150811-19779,,,,,,,,!#!E,,JMEISNER,JMEISNER081109013588-2,2,NFLX,1,,2,0,B,695,CRON067,220,,,,JMEISNER-10150811-19781,,,N,Y,BRACK,ABN,BRACK,,,JMEISNER,JMEISNER-10150811-19781-0,,201508,P,112,NFLX,,O,,,1,,,,,,,,,,,,,,20150811-14:01:35.797,,,NFLX__150828P00112000,,,,,,,28,,,JMEISNER081109013588,6,,,,JMEISNER-10150811-19779,,,,,,,,\n";

static dart_order_obj* s_doj = 0;

static void spread_callback(dart_order_obj* doj, void* ob)
{
    s_doj = doj;
}

int test_spread_parsing(const char** testname, const char** extrareporting)
{
    STARTTEST;
    databuf_t* test_h = databuf_alloc(1024);
    REQCOND(test_h != 0);
    gatekeeper * gate = create_gatekeeper(16,
                                          "DODO", 4,
                                          NULL, 1, "dest_route_tbl.map",NULL);
    sock_recv_manager* srm = create_sock_recv_manager();
    srm->buffer = spread_direct;
    srm->rd_ptr = spread_direct;
    srm->wr_ptr = spread_direct + strlen(spread_direct);
    async_parse_args* apa = calloc(1, sizeof(struct async_parse_args));
    apa->gk = gate;
    apa->con = srm;
    apa->parse_v = ph_build_non_fix_rom_pos;
    apa->obj_callback = spread_callback;
    apa->in_args = calloc(1, sizeof(struct init_args));
    apa->is_logged_on = 1;
    int cut_con = 0;
    ph_parse_rom_message(&cut_con,apa);
    CHKCOND(s_doj != 0);
    CHKCOND(s_doj->childs != 0);
    CHKCOND(s_doj->childs->next != 0);
    CHKCOND(s_doj->childs->next->next == 0);
    dart_order_obj* c1 = s_doj->childs->payload;
    dart_order_obj* c2 = s_doj->childs->next->payload;
    CHKCOND(c1 != 0);
    CHKCOND(c2 != 0);
    CHKCOND(strncmp("JMEISNER081109013588", getpval(s_doj, 6), getplen(s_doj, 6)) == 0);
    CHKCOND(strncmp("JMEISNER081109013588-1", getpval(c1, 6), getplen(c1, 6)) == 0);
    CHKCOND(strncmp("JMEISNER081109013588-2", getpval(c2, 6), getplen(c2, 6)) == 0);
    CHKCOND(strncmp("114", getpval(c1, ROM_STRIKE), getplen(c1, ROM_STRIKE)) == 0);
    CHKCOND(strncmp("112", getpval(c2, ROM_STRIKE), getplen(c2, ROM_STRIKE)) == 0);
    destroy_gk_obj(gate, s_doj);
    srm->buffer = spread_from_rom;
    srm->rd_ptr = spread_from_rom;
    srm->wr_ptr = spread_from_rom + strlen(spread_from_rom);
    ph_parse_rom_message(&cut_con,apa);
    CHKCOND(s_doj != 0);
    CHKCOND(s_doj->childs != 0);
    CHKCOND(s_doj->childs->next != 0);
    CHKCOND(s_doj->childs->next->next == 0);
    c1 = s_doj->childs->payload;
    c2 = s_doj->childs->next->payload;
    CHKCOND(c1 != 0);
    CHKCOND(c2 != 0);
    CHKCOND(strncmp("JMEISNER-10150811-19779", getpval(s_doj, ROM_TAG), getplen(s_doj, ROM_TAG)) == 0);
    CHKCOND(strncmp("JMEISNER-10150811-19780", getpval(c1, ROM_TAG), getplen(c1, ROM_TAG)) == 0);
    CHKCOND(strncmp("JMEISNER-10150811-19781", getpval(c2, ROM_TAG), getplen(c2, ROM_TAG)) == 0);
    CHKCOND(strncmp("JMEISNER081109013588", getpval(c1, ROM_PARENT), getplen(c1, ROM_PARENT)) == 0);
    CHKCOND(strncmp("JMEISNER081109013588", getpval(c2, ROM_PARENT), getplen(c2, ROM_PARENT)) == 0);
    CHKCOND(strncmp("JMEISNER081109013588", getpval(s_doj, 6), getplen(s_doj, 6)) == 0);
    CHKCOND(strncmp("JMEISNER081109013588-1", getpval(c1, 6), getplen(c1, 6)) == 0);
    CHKCOND(strncmp("JMEISNER081109013588-2", getpval(c2, 6), getplen(c2, 6)) == 0);
    CHKCOND(strncmp("114", getpval(c1, ROM_STRIKE), getplen(c1, ROM_STRIKE)) == 0);
    CHKCOND(strncmp("112", getpval(c2, ROM_STRIKE), getplen(c2, ROM_STRIKE)) == 0);
    destroy_gk_obj(gate, s_doj);
    STOPTEST;
}
#endif
static char* bad_text = "[DMA200] Client limits breached: PriceTolerance BREACH: Buy Price 7.5 is too low (passive buy threshold=1222.0 passive multiplier=50.0%) BAND ABF.L, refTime=1480351500946 buy[max=2700.0,  phase=[nonTrad,] flags=[static,lastTradedRef,]] sell[min=2444.0,  phase=[nonTrad,] flags=[static,lastTradedRef,]]";

static char* h_blowout = "E,,MM8Z,MM8Z-R2-1389,5,KSS,300,76.030000,2,2,A,501,7YVK1209,251,0,,,,,,,,MM8Z,GSEC,MM8Z,,,,,,,,,,,E,,,,,,,,,,,,,,,,,,,,,,,501,,,,,,,,,,,,,,,,0\nC,,MM8Z,MM8Z-R2-1389,5,KSS,300,76.030000,2,2,A,501,7YVK1209,251,,,,,,,,,MM8Z,GSEC,MM8Z,\nE,,MM8Z,MM8Z-R2-1391,1,PNW,200,61.960000,2,2,A,501,7YVK1209,251,0,,,,,,,,MM8Z,GSEC,MM8Z,,,,,,,,,,,E,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,0\nC,,MM8Z,MM8Z-R2-1391,1,PNW,200,61.960000,2,2,A,501,7YVK1209,251,,,,,,,,,MM8Z,GSEC,MM8Z,\nE,,MM8Z,MM8Z-R2-1393,1,OAKS,200,10.750000,2,2,A,501,7YVK1209,251,0,,,,,,,,MM8Z,GSEC,MM8Z,,,,,,,,,,,E,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,0\nE,,MM8Z,MM8Z-R2-1395,5,LYB,200,95.860000,2,2,A,501,7YVK1209,251,0,,,,,,,,MM8Z,GSEC,MM8Z,,,,,,,,,,,E,,,,,,,,,,,,,,,,,,,,,,,501,,,,,,,,,,,,,,,,0\nE,,MM8Z,MM8Z-R2-1397,1,LYB,600,94.820000,2,2,A,501,7YVK1209,251,0,,,,,,,,MM8Z,GSEC,MM8Z,,,,,,,,,,,E,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,0\nC,,MM8Z,MM8Z-R2-1395,5,LYB,200,95.860000,2,2,A,501,7YVK1209,251,,,,,,,,,MM8Z,GSEC,MM8Z,\nE,,MM8Z,MM8Z-R2-1399,5,LYB,200,95.840000,2,2,A,501,7YVK1209,251,0,,,,,,,,MM8Z,GSEC,MM8Z,,,,,,,,,,,E,,,,,,,,,,,,,,,,,,,,,,,501,,,,,,,,,,,,,,,,0\nC,,MM8Z,MM8Z-R2-1399,5,LYB,200,95.840000,2,2,A,501,7YVK1209,251,,,,,,,,,MM8Z,GSEC,MM8Z,\n";
static queue_t oats_queue;
static hashmap_t oats_mapper;
static databuf_t* test_h = 0;
void build_op_callback(dart_order_obj* doj, void* ob)
{
    async_parse_args *ap = (async_parse_args *) ob;
    char *rtype = doj->positions[0].iov_base;
    if(rtype[0] == 'C') {
        databuf_memcpy(test_h, doj->current->data->rd_ptr, databuf_unread(doj->current->data));
        dart_order_obj* orig = 0;
        int found = get_map_entry(oats_mapper,
                                  doj->positions[ROM_CLIENT_ID].iov_base,
                                  doj->positions[ROM_CLIENT_ID].iov_len,
                                  (void **) &orig);
        if(found) {
            reset_rom_field(orig, ROM_STATUS, "26", 2);
            reset_rom_field(orig, 0, "S", 1);
            dart_order_obj *ldoj = get_gk_obj(ap->gk);
            copy_dart_order_obj(orig, ldoj);
            enqueue(oats_queue, ldoj);
        }
        destroy_gk_obj(ap->gk, doj);
    } else {
        int allowed = unique_no_copy_insert(oats_mapper,
                                            doj->positions[ROM_CLIENT_ID].
                                            iov_base,
                                            doj->positions[ROM_CLIENT_ID].
                                            iov_len, doj);
        if(allowed == 0) {
            printf("PROBLEMS!!!! UNABLE TO INSERT ORDER!!!!\n");
        }
        dart_order_obj *ldoj = get_gk_obj(ap->gk);
        copy_dart_order_obj(doj, ldoj);
        enqueue(oats_queue, ldoj);
    }
}
int test_build_n_hold(const char** testname, const char** extrareporting)
{
    STARTTEST;
    test_h = databuf_alloc(1024);
    REQCOND(test_h != 0);
    oats_queue = create_sized_queue(2);
    oats_mapper = create_map(32);
    gatekeeper * gate = create_gatekeeper(2,
                                          "DODO", 4,
                                          NULL, 1, "dest_route_tbl.map",NULL);
    sock_recv_manager* srm = create_sock_recv_manager();
    srm->buffer = h_blowout;
    srm->rd_ptr = h_blowout;
    srm->wr_ptr = h_blowout + strlen(h_blowout);
    async_parse_args* apa = calloc(1, sizeof(struct async_parse_args));
    apa->gk = gate;
    apa->con = srm;
    apa->parse_v = ph_build_non_fix_rom_pos;
    apa->obj_callback = build_op_callback;
    apa->in_args = calloc(1, sizeof(struct init_args));
    apa->is_logged_on = 1;
    int cut_con = 0;
    ph_parse_rom_message(&cut_con,apa);
    element_t e, temp;
    e = dequeue(oats_queue);
    while (e != NULL) {
        struct dart_order_obj *m;
        if (e->d != NULL) {
            m = (struct dart_order_obj *) e->d;
            set_copied_doo_in_buf(m, test_h);
            CHKCOND(test_h->buffer[0] != 'C');
            databuf_reset(test_h);
        }
        temp = e;
        e = temp->next;
        m = NULL;
        temp->d = NULL;
        temp->next = 0;
        free_element(oats_queue, temp);
    }
    databuf_reset(test_h);
    dart_order_obj *ldoj = get_gk_obj(gate);
    int rlen = strlen(bad_text);
    clean_text(ldoj, test_h, 
        ROM_TEXT, bad_text,  rlen);
    char* commer = memchr(ldoj->positions[ROM_TEXT].iov_base, ',', 
            ldoj->positions[ROM_TEXT].iov_len);
    CHKCOND(commer == NULL);
    STOPTEST;
}


