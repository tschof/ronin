#include "tests.h"
#include "testmacros.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "fix_rom_trans.h"
#include "dart_orderbook.h"
#include "rom2fix.h"
#include "ex_common.h"
#include "complex_parser.h"

int get_is_test_con()
{
    return 1;
}

char *test_id_gen(size_t * len)
{
    return NULL;
}


int test_month_codes(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    orderbook *ob = init_order_book(test_id_gen, get_is_test_con,
                                    def_rom2fix, def_rom2fix,
                                    def_xlate_fixrom, 50000,
                                    "NYSE", 4, "NYSE",
                                    4, NULL, 0);
    initalize_maps(ob);
    int ret = is_month_code("Y");
    CHKCOND(ret == 0);
    ret = is_month_code("WY");
    CHKCOND(ret == 0);
    ret = is_month_code(NULL);
    CHKCOND(ret == 0);
    ret = is_month_code("H");
    CHKCOND(ret == 1);
STOPTEST}

int test_symbol_killer(const char **testname, const char **extrareporting)
{

    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    create_month_codes();
    int len = get_base_sym_len("ESU1", 4);
    CHKCOND(len == 2);
    len = get_base_sym_len("EW1", 3);
    CHKCOND(len == 3);
    len = get_base_sym_len("YYYYY", 5);
    CHKCOND(len == 5);
    len = get_base_sym_len("GGGGG", 5);
    CHKCOND(len == 5);
    len = get_base_sym_len("12345", 5);
    CHKCOND(len == 0);
    len = get_base_sym_len("ES", 2);
    CHKCOND(len == 2);
    len = get_base_sym_len("VXH3", 4);
    CHKCOND(len == 2);
STOPTEST}
int test_symbol_mod(const char **testname, const char **extrareporting)
{

    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    orderbook *ob = init_order_book(test_id_gen, get_is_test_con,
                                    def_rom2fix, def_rom2fix,
                                    def_xlate_fixrom, 50000,
                                    "NYSE", 4, "NYSE",
                                    4, NULL, 0);
    initalize_maps(ob);
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
    len = get_symbol_len("NGG9", 4);
    CHKCOND(len == 2);
STOPTEST}

int test_tpos_sym_trans(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    orderbook *ob = init_order_book(test_id_gen, get_is_test_con,
                                    def_rom2fix, def_rom2fix,
                                    def_xlate_fixrom, 50000,
                                    "NYSE", 4, "NYSE",
                                    4, NULL, 0);
    initalize_maps(ob);
    char *fix_mess = "E{,ESM7 PC1120,},O,,,,,,,,,,,";
    size_t len = strlen(fix_mess);
    struct dart_fix_pos p = { 3, 14 };
    struct dart_fix_pos o = { 17, 18 };
    struct dart_fix_pos pos[2] = { p, o };
    tpos_symbol_trans(fix_mess, test_b, "D", pos, 0, 1);
    CHKCOND(strstr(test_b->buffer, ",") == NULL);
    CHKCOND(strlen(test_b->buffer) == 6);
    printf("OUT: {%s}\n", test_b->buffer);
    databuf_reset(test_b);
    char *fix_mess2 = "E{,ESM7,},O,,,,,,,,,,,";
    len = strlen(fix_mess2);
    struct dart_fix_pos p2 = { 3, 7 };
    struct dart_fix_pos o2 = { 10, 11 };
    struct dart_fix_pos pos2[2] = { p2, o2 };
    tpos_symbol_trans(fix_mess2, test_b, "D", pos2, 0, 1);
    CHKCOND(strstr(test_b->buffer, ",") == NULL);
    CHKCOND(strlen(test_b->buffer) == 6);
    printf("OUT: {%s}\n", test_b->buffer);
    databuf_reset(test_b);
    char *fix_mess3 = "E{,ESMT,},O,,,,,,,,,,,";
    len = strlen(fix_mess3);
    struct dart_fix_pos p3 = { 3, 7 };
    struct dart_fix_pos o3 = { 10, 11 };
    struct dart_fix_pos pos3[2] = { p3, o3 };
    tpos_symbol_trans(fix_mess3, test_b, "D", pos3, 0, 1);
    CHKCOND(strstr(test_b->buffer, ",") == NULL);
    CHKCOND(strlen(test_b->buffer) == 8);
    databuf_reset(test_b);
    char *fix_mess4 = "E{,EW1,},O,,,,,,,,,,,";
    len = strlen(fix_mess4);
    struct dart_fix_pos p4 = { 3, 6 };
    struct dart_fix_pos o4 = { 9, 10 };
    struct dart_fix_pos pos4[2] = { p4, o4 };
    tpos_symbol_trans(fix_mess4, test_b, "D", pos4, 0, 1);
    CHKCOND(strstr(test_b->buffer, ",") == NULL);
    CHKCOND(strlen(test_b->buffer) == 7);
    databuf_reset(test_b);
    char *fix_mess5 = "E{,VIA/B,},,,,,,,,,,,,";
    len = strlen(fix_mess5);
    struct dart_fix_pos p5 = { 3, 8 };
    struct dart_fix_pos o5 = { 0, 0 };
    struct dart_fix_pos pos5[2] = { p5, o5 };
    tpos_symbol_trans(fix_mess5, test_b, "D", pos5, 0, 1);
    CHKCOND(strstr(test_b->buffer, ",") == NULL);
    CHKCOND(strlen(test_b->buffer) == 12);
    databuf_reset(test_b);
    char *fix_mess6 = "E{,VIA/PR,},,,,,,,,,,,,";
    len = strlen(fix_mess6);
    struct dart_fix_pos p6 = { 3, 9 };
    struct dart_fix_pos o6 = { 0, 0 };
    struct dart_fix_pos pos6[2] = { p6, o6 };
    tpos_symbol_trans(fix_mess6, test_b, "D", pos6, 0, 1);
    CHKCOND(strstr(test_b->buffer, ",") == NULL);
    CHKCOND(strlen(test_b->buffer) == 12);
    printf("OUT: {%s}\n", test_b->buffer);
    databuf_reset(test_b);
    char *fix_mess7 = "E{,VIA/PRA,},,,,,,,,,,,,";
    len = strlen(fix_mess7);
    struct dart_fix_pos p7 = { 3, 10 };
    struct dart_fix_pos o7 = { 0, 0 };
    struct dart_fix_pos pos7[2] = { p7, o7 };
    tpos_symbol_trans(fix_mess7, test_b, "D", pos7, 0, 1);
    CHKCOND(strstr(test_b->buffer, ",") == NULL);
    CHKCOND(strlen(test_b->buffer) == 13);
    printf("OUT: {%s}\n", test_b->buffer);
    databuf_reset(test_b);
    char *fix_mess8 = "E{,VIA/PRWS,,,,,,,,,,,,,";
    len = strlen(fix_mess8);
    struct dart_fix_pos p8 = { 3, 11 };
    struct dart_fix_pos o8 = { 0, 0 };
    struct dart_fix_pos pos8[2] = { p8, o8 };
    tpos_symbol_trans(fix_mess8, test_b, "D", pos8, 0, 1);
    CHKCOND(strstr(test_b->buffer, ",") == NULL);
    CHKCOND(strlen(test_b->buffer) == 14);
    //printf("OUT: {%s}\n", test_b->buffer);
    clean_dart_oo_pool(ob->mgr);
    databuf_destroy(test_b);
    STOPTEST
}

static void run_rtf_test_bk(char *rom_mess, databuf_t * test_b)
{
    orderbook *ob = init_order_book(test_id_gen, get_is_test_con,
                                    def_rom2fix, def_rom2fix,
                                    def_xlate_fixrom, 50000,
                                    "NYSE", 4, "NYSE",
                                    4, NULL, 0);
    long rom_len = strlen(rom_mess);
    long ret = 0;
    int rej = 0;
    char fix_type[3];
    int ft_len = 0;
    struct message mess;
    mess.name = "Jojo";
    mess.name_len = 4;
    fs_set_spread_book(ob);
    ret = trans_rom_message(&mess, rom_mess, rom_len, test_b,
                            fix_type, &ft_len, &rej, ob,
                            default_rom_to_fix_override);
    clean_dart_oo_pool(ob->mgr);
}

static void run_test_background(char *fix_mess, databuf_t * test_b)
{
    orderbook *ob = init_order_book(test_id_gen, get_is_test_con,
                                    def_rom2fix, def_rom2fix,
                                    def_xlate_fixrom, 50000,
                                    "NYSE", 4, "NYSE",
                                    4, NULL, 0);
    long fix_len = strlen(fix_mess);
    long ret = 0;
    int rej = 0;
    struct message mess;
    mess.name = "Jojo";
    mess.name_len = 4;
    fs_set_spread_book(ob);
    ret = trans_fix_message(&mess, fix_mess, fix_len, test_b,
                            &rej, ob, fix_rom_spread_override);
    clean_dart_oo_pool(ob->mgr);
}

int test_rtf_trans_fill(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    char *rom_mess =
        "S,151010,FIXWANG,091007_792_2047,1,POOL,100,21.3600,2,0,A,,,146,2,2010120132579,21786,FIXWANG-10101201-57798,,,,,,,,,Fill,,FIXWANG-10101201-57798-3,,,,,,,E,,,,,INET,,93,,21.3600,7YTC1209,,,100,0,20101201-15:10:10,21.360000,,,31391142,,,N,,,,2,,,,,,8,,A,,2787,,,,0,,,,,\n";
    run_rtf_test_bk(rom_mess, test_b);
    //printf("New: %s \n", test_b->buffer);
//    printf("Old: %s \n", rom_res);
    CHKCOND(strstr(test_b->buffer, "11=091007_792_2047") != NULL);
    CHKCOND(strstr(test_b->buffer, "167=CS") != NULL);
    CHKCOND(strstr(test_b->buffer, "31=21.3600") != NULL);
    CHKCOND(strstr(test_b->buffer, "32=93") != NULL);
    CHKCOND(strstr(test_b->buffer, "6=21.360000") != NULL);
    CHKCOND(strstr(test_b->buffer, "55=POOL") != NULL);
    CHKCOND(strstr(test_b->buffer, "37=FIXWANG-10101201-57798") != NULL);
    CHKCOND(strstr(test_b->buffer, "14=100") != NULL);
    CHKCOND(strstr(test_b->buffer, "20=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "150=2") != NULL);
    CHKCOND(strstr(test_b->buffer, "39=2") != NULL);
    CHKCOND(strstr(test_b->buffer, "40=2") != NULL);
    CHKCOND(strstr(test_b->buffer, "54=1") != NULL);
    CHKCOND(strstr(test_b->buffer, "60=20101201-15:10:10") != NULL);
    CHKCOND(strstr(test_b->buffer, "59=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "151=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "17=FIXWANG-10101201-57798-3") != NULL);

    databuf_destroy(test_b);
STOPTEST}

int test_rtf_trans_open(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    char *rom_mess =
        "S,,,091016_839_2162,1,KSU,100,48.5000,2,0,,,7YTC1209,192,13,ZXG 1793/12012010,ZXG 1793/12012010,FIXWANG-10101201-57850,,,,,,,,,,,FIXWANG-10101201-57850-1,,,,,,,E,,,,,,,0,,0,,,,0,100,20101201-15:10:16,0.000000,,,0,,,,,,,0,,,,,,,,,,2823,,,,,,,,,\n";
    run_rtf_test_bk(rom_mess, test_b);
    CHKCOND(strstr(test_b->buffer, "11=091016_839_2162") != NULL);
    CHKCOND(strstr(test_b->buffer, "167=CS") != NULL);
    CHKCOND(strstr(test_b->buffer, "31=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "32=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "6=0.000000") != NULL);
    CHKCOND(strstr(test_b->buffer, "55=KSU") != NULL);
    CHKCOND(strstr(test_b->buffer, "37=FIXWANG-10101201-57850") != NULL);
    CHKCOND(strstr(test_b->buffer, "14=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "20=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "150=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "39=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "40=2") != NULL);
    CHKCOND(strstr(test_b->buffer, "54=1") != NULL);
    CHKCOND(strstr(test_b->buffer, "60=20101201-15:10:16") != NULL);
    CHKCOND(strstr(test_b->buffer, "59=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "151=100") != NULL);
    CHKCOND(strstr(test_b->buffer, "17=FIXWANG-10101201-57850-1") != NULL);
    databuf_destroy(test_b);
STOPTEST}

int test_rtf_trans_out(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    char *rom_mess =
        "S,151009,FIXWANG,090717_538_1985,1,XOP,100,49.07,2,0,,,,140,4,201012016874,4503612514675903,FIXWANG-10101201-57357,,,,,,,,,Cancelled by User,,FIXWANG-10101201-57357-64,,,,,,,E,,,,,,,0,,0,7YTC1209,,,0,0,20101201-15:10:09,0.000000,,,4503612512300307,,,,,,090717_538_2103,4,,,,,,8,,,,2744,,,,0,,,,,\n";
    run_rtf_test_bk(rom_mess, test_b);
    CHKCOND(strstr(test_b->buffer, "41=090717_538_1985") != NULL);
    CHKCOND(strstr(test_b->buffer, "11=090717_538_2103") != NULL);
    CHKCOND(strstr(test_b->buffer, "167=CS") != NULL);
    CHKCOND(strstr(test_b->buffer, "31=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "32=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "6=0.000000") != NULL);
    CHKCOND(strstr(test_b->buffer, "55=XOP") != NULL);
    CHKCOND(strstr(test_b->buffer, "37=FIXWANG-10101201-57357") != NULL);
    CHKCOND(strstr(test_b->buffer, "14=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "20=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "150=4") != NULL);
    CHKCOND(strstr(test_b->buffer, "39=4") != NULL);
    CHKCOND(strstr(test_b->buffer, "40=2") != NULL);
    CHKCOND(strstr(test_b->buffer, "54=1") != NULL);
    CHKCOND(strstr(test_b->buffer, "60=20101201-15:10:09") != NULL);
    CHKCOND(strstr(test_b->buffer, "59=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "151=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "17=FIXWANG-10101201-57357-64") !=
            NULL);
    databuf_destroy(test_b);
STOPTEST}

int test_rtf_trans_rpl(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    char *rom_mess =
        "S,,,090507_78_1886,5,CBT,100,36.700000,2,0,,,7YTC1209,192,5,ZXG 1787/12012010,ZXG 1644/12012010,FIXWANG-10101201-56755,,,,,,,,,,,FIXWANG-10101201-56755-41,,,,,,,E,,,,,,,0,,0,,,,0,100,20101201-15:10:09,0.000000,,,0,,,,,,090507_78_2145,5,,,,,,,,,,2786,,,,,,,,,\n";
    run_rtf_test_bk(rom_mess, test_b);
    CHKCOND(strstr(test_b->buffer, "41=090507_78_1886") != NULL);
    CHKCOND(strstr(test_b->buffer, "11=090507_78_2145") != NULL);
    CHKCOND(strstr(test_b->buffer, "167=CS") != NULL);
    CHKCOND(strstr(test_b->buffer, "31=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "32=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "6=0.000000") != NULL);
    CHKCOND(strstr(test_b->buffer, "55=CBT") != NULL);
    CHKCOND(strstr(test_b->buffer, "37=FIXWANG-10101201-56755") != NULL);
    CHKCOND(strstr(test_b->buffer, "14=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "20=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "150=5") != NULL);
    CHKCOND(strstr(test_b->buffer, "39=5") != NULL);
    CHKCOND(strstr(test_b->buffer, "40=2") != NULL);
    CHKCOND(strstr(test_b->buffer, "54=5") != NULL);
    CHKCOND(strstr(test_b->buffer, "60=20101201-15:10:09") != NULL);
    CHKCOND(strstr(test_b->buffer, "59=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "151=100") != NULL);
    CHKCOND(strstr(test_b->buffer, "17=FIXWANG-10101201-56755-41") !=
            NULL);
    databuf_destroy(test_b);
STOPTEST}

int test_rtf_trans_rep_rej(const char **testname,
                           const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    char *rom_mess =
        "S,151014,FIXWANG,091007_802_2057,5,ESRX,100,54.07,2,0,A,,,146,20,Cxl_Rpl,21789,FIXWANG-10101201-57808,,,,,,,,,Order is already pending cancel or replace.,FIXWANG,FIXWANG-10101201-57808-5,,,,,,,E,,,,,,,0,100,0,WANGJ 501 7YTC1209 GSEC WANGJ,,,100,0,20101201-15:10:14.664,54.060000,,,,,,,501,501,091007_802_2156,,,,,,,,,,,2813,,,,,,,,,\n";
    run_rtf_test_bk(rom_mess, test_b);
    //printf("{%s}", test_b->buffer);
    CHKCOND(strstr(test_b->buffer, "41=091007_802_2057") != NULL);
    CHKCOND(strstr(test_b->buffer, "11=091007_802_2156") != NULL);
    CHKCOND(strstr(test_b->buffer, "167=CS") == NULL);
    CHKCOND(strstr(test_b->buffer, "31=0") == NULL);
    CHKCOND(strstr(test_b->buffer, "32=0") == NULL);
    CHKCOND(strstr(test_b->buffer, "37=FIXWANG-10101201-57808") != NULL);
    CHKCOND(strstr(test_b->buffer, "20=0") == NULL);
    CHKCOND(strstr(test_b->buffer, "150=8") == NULL);
    CHKCOND(strstr(test_b->buffer, "39=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "40=2") == NULL);
    CHKCOND(strstr(test_b->buffer, "54=5") == NULL);
    CHKCOND(strstr
            (test_b->buffer,
             "58=Order is already pending cancel or replace.") != NULL);
    CHKCOND(strstr(test_b->buffer, "59=0") == NULL);
    CHKCOND(strstr(test_b->buffer, "434=2") != NULL);
    databuf_destroy(test_b);
STOPTEST}

int test_rtf_trans_cnl_rej(const char **testname,
                           const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    char *rom_mess =
        "S,150804,FIXTHALES2,1012309,1,ANF,70,,2,,,,,187,14,20101201359,201012011008040781690916819,FIXTHALES2-10101201-56634,,,,,,,,,The selected order was not found,,FIXTHALES2-10101201-56634-4,,,,0.000000,,,O,,,0,,,,0,,0,,,,47,23,,0.000000,,,,,,,,,1012382,,,1,,,3,9,,,,1428,,,JC1:WKIM:seeker184SP,,,,,,\n";
    run_rtf_test_bk(rom_mess, test_b);
    CHKCOND(strstr(test_b->buffer, "41=1012309") != NULL);
    CHKCOND(strstr(test_b->buffer, "11=1012382") != NULL);
    CHKCOND(strstr(test_b->buffer, "32=0") == NULL);
    CHKCOND(strstr(test_b->buffer, "37=FIXTHALES2-10101201-56634") !=
            NULL);
    CHKCOND(strstr(test_b->buffer, "20=0") == NULL);
    CHKCOND(strstr(test_b->buffer, "150=8") == NULL);
    CHKCOND(strstr(test_b->buffer, "39=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "40=2") == NULL);
    CHKCOND(strstr(test_b->buffer, "54=5") == NULL);
    CHKCOND(strstr(test_b->buffer, "59=0") == NULL);
    CHKCOND(strstr(test_b->buffer, "58=The selected order was not found")
            != NULL);
    CHKCOND(strstr(test_b->buffer, "434=1") != NULL);
    databuf_destroy(test_b);
STOPTEST}

int test_ftr_trans_new(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    char *fix_mess =
        "35=D34=9549=FIXWANG50=WANGJ52=20101201-15:05:07.83556=ROMFIX1=WANGJ 501 7YTC1209 GSEC WANGJ11=090507_0_021=138=10040=244=23.0447=A54=255=KRE58=INET-O59=060=20101201-15:05:07.783167=CS10001=14610=207";
    char *rom_res =
        "E,,FIXWANG,090507_0_0,2,KRE,100,23.04,2,0,A,501,7YTC1209,146,,INET-O,,,,,,,WANGJ,GSEC,WANGJ,,,,,,,,,,,E,,,,,,,,,,,,,,,20101201-15:05:07.783,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";
    run_test_background(fix_mess, test_b);
    CHKCOND(strcmp(test_b->buffer, rom_res) == 0);
    databuf_destroy(test_b);
STOPTEST}

int test_ftr_trans_replace(const char **testname,
                           const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    char *fix_mess =
        "35=G34=34249=FIXWANG50=WANGJ52=20101201-15:05:09.09756=ROMFIX1=WANGJ 501 7YTC1209 GSEC WANGJ11=090507_98_24721=138=10040=241=090507_98_9844=86.9347=A54=155=GR58=Cxl_Rpl59=060=20101201-15:05:09.095167=CS10=040";
    char *rom_res =
        "R,,FIXWANG,090507_98_98,1,GR,100,86.93,2,0,A,,,,,Cxl_Rpl,,,,,,,,,,,,,,,,,,,,E,,,,,,,,,,,,,,-1,20101201-15:05:09.095,,,,,,,,,,090507_98_247,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";
    run_test_background(fix_mess, test_b);
    CHKCOND(strcmp(test_b->buffer, rom_res) == 0);
    databuf_destroy(test_b);
STOPTEST}

int test_ftr_trans_cnl(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    char *fix_mess =
        "35=F34=218249=FIXWANG50=WANGJ52=20101201-15:10:09.34556=ROMFIX1=WANGJ 501 7YTC1209 GSEC WANGJ11=090717_538_210338=10041=090717_538_198554=155=XOP58=Cxl60=20101201-15:10:09.34410=144";
    char *rom_res =
        "C,,FIXWANG,090717_538_1985,1,XOP,100,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,20101201-15:10:09.344,,,,,,,,,,090717_538_2103,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";
    run_test_background(fix_mess, test_b);
    CHKCOND(strcmp(test_b->buffer, rom_res) == 0);
    databuf_destroy(test_b);
STOPTEST}
