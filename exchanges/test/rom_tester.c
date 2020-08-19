#include "tests.h"
#include "testmacros.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "dart_orderbook.h"
#include "rom_handler.h"
#include "rom2fix.h"
#include "ex_common.h"
#include "databuf.h"
#include "sock_recv_manager.h"
#include "dart_resend_request.h"

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
static char* first = "S,,MM8R,MM8R-2815,1,NNVC,200,2.6300000,2,2,A,501,7YVK1209,246,13,DSE 3483/01152015,";
static char* threeSevenFiveThreeOhTwo = "S,,MM8M,MM8M-2979,1,FPO,200,13.5000000,2,0,A,501,7YVK1209,271,2,EMI 0144/01152015,";
static char* one = "E,,ROMTEST,RD-8351-1,1,IBM,1,1.0000000,2,0,A,0501,7YKS1209,270,0,,,ROMTEST-160150115-2,";
static char* twelve = "S,,ROMTEST,RD-8351-3,1,IBM,1,1.0000000,2,0,A,0501,7YKS1209,247,4,JQA 0001/01152015,";
int test_resend(const char** testname, const char** extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    FILE *in_file = fopen("Oats_test_resend_log", "r+"); 
    int f_len = strlen(first);
    int t_len = strlen(threeSevenFiveThreeOhTwo);
    size_t out_len = 0;
    if(in_file) {
        resend_req *rr = create_resend_request("Oats",
                                               4,
                                               32699, 0,
                                               find_rom_offsets);
        drr_create_resend_out(rr, in_file);
        char* output = (char *) drr_get_resend_output(rr, &out_len);
        CHKCOND(strncmp(output, first, f_len) == 0);
        destroy_resend_req(rr);
        rr = create_resend_request("Oats",
                                               4,
                                               375302, 0,
                                               find_rom_offsets);
        drr_create_resend_out(rr, in_file);
        output = (char *) drr_get_resend_output(rr, &out_len);
        CHKCOND(strncmp(output, threeSevenFiveThreeOhTwo, t_len) == 0);
        destroy_resend_req(rr);
        fclose(in_file);
    }  
    in_file = fopen("Oats_tester_two", "r+");
    if(in_file) {
        int o_len = strlen(one);
        resend_req *rr = create_resend_request("Oats",
                                               4,
                                               1, 0,
                                               find_rom_offsets);
        drr_create_resend_out(rr, in_file);
        char* output = (char *) drr_get_resend_output(rr, &out_len);
        CHKCOND(strncmp(output, one, o_len) == 0);
        destroy_resend_req(rr);
        t_len = strlen(twelve);
        rr = create_resend_request("Oats",
                                               4,
                                               12, 0,
                                               find_rom_offsets);
        drr_create_resend_out(rr, in_file);
        output = (char *) drr_get_resend_output(rr, &out_len);
        CHKCOND(strncmp(output, twelve, t_len) == 0);
        destroy_resend_req(rr);
        fclose(in_file);
    }
    STOPTEST
}

int test_realloc_receive(const char** testname, const char** extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    char* orig = calloc(SOCK_READ_SIZE,1);
    char* end = orig + SOCK_READ_SIZE;
    size_t tf_len = strlen(twenty_five);
    memcpy(end - tf_len, twenty_five,tf_len);
    char* wr_ptr = orig + SOCK_READ_SIZE;
    CHKCOND(strncmp(wr_ptr - 27,",,PARNELL1104231044380034,\n" ,26) == 0);
    char* rd_ptr = end - tf_len;
    CHKCOND(strncmp(rd_ptr,"E,20110523-15:44:05,PARNELL," ,28) == 0);
    unsigned long write_offset = SOCK_READ_SIZE;
    unsigned long read_offset = SOCK_READ_SIZE - tf_len;
    unsigned long total_size = SOCK_READ_SIZE;
    CHKCOND(orig[0] == '\0');
    CHKCOND(strncmp(rd_ptr,"E,20110523-15:44:05,PARNELL," ,28) == 0);
    orig = realloc_receive(&total_size, &write_offset, &read_offset,
                                wr_ptr, rd_ptr, orig);
    wr_ptr = orig;
    wr_ptr += write_offset;
    rd_ptr = orig;
    CHKCOND(strncmp(orig,"E,20110523-15:44:05,PARNELL," ,28) == 0);
    CHKCOND(strncmp(rd_ptr,"E,20110523-15:44:05,PARNELL," ,28) == 0);
    CHKCOND(strncmp(wr_ptr - 27,",,PARNELL1104231044380034,\n" ,26) == 0);
    STOPTEST
}

int test_rom_pad_one(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    databuf_write(test_b,
                  "%s\n"
                  "E,20110523-15:44:05,PARNELL,PARNELL1104231044050001,1,ADSK,100,0.950000,2,0,A,0551,RONIN,24,,,,PARNELL-50110523-2,,,,,USER0001,WAG,PARNELL,,8522820409900022,PARNELL,PARNELL-50110523-2-0,N,,,,,,E,,,1,,,24,,,,,,,,,20110523-15:44:05,,,,,,,,,,,,,,,,,,,,,,,,,,,,,");
    char *result = pad_rom_resend(test_b->buffer, databuf_unread(test_b));
    CHKCOND(strcmp(result, "A,,,,,,,,,,,,\n") == 0);
STOPTEST}

int test_rom_pad_twenty_five(const char **testname,
                             const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    databuf_write(test_b, "%s", twenty_five);
    char *result = pad_rom_resend(test_b->buffer, databuf_unread(test_b));
    CHKCOND(result[0] == 'A');
    CHKCOND(strcmp(result, twenty_five_checker) == 0);
STOPTEST}

char *rom_message =
    "E,20110523-15:44:05,PARNELL,PARNELL1104231044050001,1,ADSK,\n";
char *rom_message2 = "E,20110523-15:44:05,PARNELL,PARNELL1104231044050001,1,ADSK,,,,,,,,,,,,DODO,\n";   //17
int test_build_rom_pos(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);

    struct dart_fix_pos l_positions[ROM_LEN];
    int i = 0;

    for (; i < ROM_LEN; ++i) {
        l_positions[i].beg = 0;
        l_positions[i].end = 0;
        l_positions[i].name = NULL;
        l_positions[i].len = 0;
    }
    orderbook *ob = init_order_book(r_test_id_gen, r_get_is_test_con,
                                    def_rom2fix, def_rom2fix,
                                    def_xlate_fixrom, 50000,
                                    "NYSE", 4, "NYSE",
                                    4, NULL, 0);
    initalize_maps(ob);
    build_rom_pos2(rom_message, strlen(rom_message),
                  ob->rom_to_fix_status, l_positions, 0);
    CHKCOND(l_positions[5].beg > 0);
    char *test = calloc(10, 1);
    memcpy(test, rom_message + l_positions[5].beg,
           (l_positions[5].end - l_positions[5].beg));
    CHKCOND(strcmp(test, "ADSK") == 0);
    CHKCOND(l_positions[6].beg == 59);

    for (; i < ROM_LEN; ++i) {
        l_positions[i].beg = 0;
        l_positions[i].end = 0;
        l_positions[i].name = NULL;
        l_positions[i].len = 0;
    }
    build_rom_pos2(rom_message2, strlen(rom_message2),
                  ob->rom_to_fix_status, l_positions, 0);
    CHKCOND(l_positions[16].beg == 69);

    char *test22 = calloc(10, 1);
    memcpy(test22, rom_message2 + l_positions[17].beg,
           (l_positions[17].end - l_positions[17].beg));
    CHKCOND(l_positions[17].beg > 0);
    CHKCOND(strcmp(test22, "DODO") == 0);
    clean_dart_oo_pool(ob->mgr);
STOPTEST}


static int call_back_att = 0;
static void create_databuffs(queue_t glob, queue_t q, int num)
{
    int i = 0;
    for (; i < num; ++i) {
        databuf_t *buff = databuf_alloc(500);
        enqueue(q, buff);
    }
}

static void clear_db(void *db)
{
    databuf_t *buff = (databuf_t *) db;
    databuf_reset(buff);
}


static void destroy_db(void *d)
{
    databuf_t *buff = (databuf_t *) d;
    databuf_destroy(buff);
}

static char *fake_client_id(size_t * i)
{
    return NULL;
}
static int get_fake_connected() {return 1;}
static void test_ph_callback(void *ph, struct message * m, void *ob)
{
    ++call_back_att;
}
int test_ph_parser(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    orderbook *ob = no_order_mng_book(fake_client_id, get_fake_connected,
                                      def_rom2fix, def_rom2fix,
                                      def_xlate_fixrom,
                                      50,
                                      "PHTESTER", 8,
                                      "PH",
                                      2, NULL, 0);
    dof_t *objects = create_dart_obj_fact(10,
                                          destroy_doo,
                                          create_dart_obj_func,
                                          clean_doo);
    dof_t *buffs = create_dart_obj_fact(100, destroy_db,
                                        create_databuffs,
                                        clear_db);
    ob->ord_obj = objects;
    ob->databufs = buffs;
    struct parse_con* pc = malloc(sizeof(struct parse_con)); 
    struct rom_parser_helper* rph = malloc(sizeof(struct rom_parser_helper)); 
    con_obj* doc = create_con_obj("NAME", 4, -1,0,0, 0);
    pc->book = ob;
    pc->parser_helper = rph;
    pc->p_callback = test_ph_callback;
    pc->co = doc;
    char *l_twenty_five ="E,20110523-15:44:05,PARNELL,PARNELL1104231044050001,1,rrrr,100,0.950000,2,0,A,0551,RONIN,24,,,,PARNELL-50110523-2,,,,,USER0001,WAG,PARNELL,,8522820409900022,PARNELL,PARNELL-50110523-2-0,N,,,,,,E,,,1,,,24,,,,,,,,,20110523-15:44:05,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\nE,20110523-15:44:06,PARNELL,PARNELL1104231044060002,1,aaaa,100,0.950000,2,0,A,0551,RONIN,24,,,,PARNELL-50110523-3,,,,,USER0001,WAG,PARNELL,,8522822659789124,PARNELL,PARNELL-50110523-3-0,N,,,,,,E,,,1,,,24,,,,,,,,,20110523-15:44:06,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\nE,20110523-15:44:06,PARNELL,PARNELL1104231044060003,1,ADSK,100,0.950000,2,0,A,0551,RONIN,24,,,,PARNELL-50110523-4,,,,,USER0001,WAG,PARNELL,,8522824167845018,PARNELL,PARNELL-50110523-4-0,N,,,,,,E,,,1,,,24,,,,,,,,,20110523-15:44:06,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";
    char* big_gulp = "E,20110523-15:44:05,PARNELL,PARNELL1104231044050001,1,ADSK,100,0.950000,2,0,A,0551,RONIN,24,,,,PARNELL-50110523-2,,,,,USER0001,WAG,PARNELL,,8522820409900022,PARNELL,PARNELL-50110523-2-0,N,,,,,,E,,,1,,,24,,,,,,,,,20110523-15:44:05,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\nE,20110523-15:44:06,PARNELL,PARNELL1104231044060002,1,ADSK,100,0.950000,2,0,A,0551,RONIN,24,,,,PARNELL-50110523-3,,,,,USER0001,WAG,PARNELL,,8522822659789124,PARNELL,PARNELL-50110523-3-0,N,,,,,,E,,,1,,,24,,,,,,,,,20110523-15:44:06,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\nE,20110523-15:44:06,PARNELL,PARNELL1104231044060003,1,ADSK,100,0.950000,2,0,A,0551,RONIN,24,,,,PARNELL-50110523-4,,,,,USER0001,WAG,PARNELL,,8522824167845018,PARNELL,PARNELL-50110523-4-0,N,,,,,,E,,,1,,,24,,,,,,,,,20110523-15:44:06,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";
    rph->cur_doj = explicit_doo_create();
    rph->read_bytes = 0;
    rph->mess_bytes = 0;
    rph->d_off = l_twenty_five;
    rph->message_beg = l_twenty_five;
    rph->rom_tag = 0;
    rph->parse_v = ph_build_dart_obj;
    CHKCOND(rph->read_bytes == 0);
    CHKCOND(rph->mess_bytes == 0);
    
printf("\n\nLength of the big boi: %lu \n\n", strlen(twenty_five));
    int after = 0;
    unsigned long  len = strlen(l_twenty_five);
    ph_build_dart_obj(len, rph, pc);
    CHKCOND(call_back_att == 3);
    CHKCOND((rph->read_bytes - rph->mess_bytes) == (len - after));
    CHKCOND(rph->rom_tag == 0);
    call_back_att = 0;
    char* partial = "E,20110523-15:44:05,PARNELL,PARNELL1104231044050001,1,ADSK,100,0.950000,2,0,A,0551,RONIN,24,,,,PARNELL-50110523-2,,,,,USER0001,WAG,PARNELL,,8522820409900022,PARNELL,PARNELL-50110523-2-0,N,,,,,,E,,,1,,,24,,,,,,,,,20110523-15:44:05,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\nE,20110523-15:44:06,PARNELL,PARNELL1104231044060002,1,ADSK,100,0.950000,2,0,A,05";
    len = strlen(partial);
    rph->read_bytes = 0;
    rph->mess_bytes = 0;
    rph->d_off = partial;
    rph->message_beg = partial;
    rph->rom_tag = 0;
    ph_build_dart_obj(len, rph, pc);
    
    CHKCOND(call_back_att == 1); 
    CHKCOND(rph->rom_tag == 11);
    CHKCOND((rph->message_beg - partial) == 259);
    rph->read_bytes = 0;
    rph->mess_bytes = 0;
    rph->d_off = 0;
    rph->message_beg = 0;
    rph->rom_tag = 0;
    call_back_att = 0;
    CHKCOND(rph->read_bytes == 0);
    CHKCOND(rph->mess_bytes == 0);
    unsigned long parsed_len = ph_parse_rom_message(partial,
                                   len, &after,pc);
    CHKCOND(parsed_len == 259);
    CHKCOND(rph->rom_tag == 11);
    CHKCOND(call_back_att == 1); 
    len = strlen(big_gulp);
    parsed_len = ph_parse_rom_message((big_gulp + parsed_len), (len - parsed_len), 
                                      &after, pc);

    CHKCOND(call_back_att == 3); 
    CHKCOND(parsed_len == (len - 259));
    free(rph);
    STOPTEST}
