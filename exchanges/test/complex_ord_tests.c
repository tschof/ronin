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
#include "order_token_creator.h"


//static char* ise_bastard = "35=j49=ISE56=RONIN234=65852=20121114-18:40:1345=662372=D379=11142012LRXXDAA380=9958=tag 207=securityExchange cannot be empty10=246";

int complex_con()
{
    return 1;
}

char *complex_gen(size_t * len)
{
    return NULL;
}

token_struct_t *test_token_gen = 0;
char *test_complex_gen_id(size_t * len)
{
    char *idbuf = 0;
    if (len != 0) {
        *len = 0;
        idbuf = malloc(64);
        if (idbuf != 0) {
            int nwritten = create_order_token(test_token_gen, idbuf, 64);
            if (nwritten > 0) {
                *len = (size_t) nwritten;
            } else {
                free(idbuf);
                idbuf = 0;
            }
        }
    }
    return idbuf;
}

int test_spread_parse(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    orderbook *ob = init_order_book(complex_gen, complex_con,
                                    def_rom2fix, def_rom2fix,
                                    def_xlate_fixrom, 50000,
                                    "NYSE", 4, "NYSE",
                                    4, NULL, 0);
    char *fix_mess =
        "35=AB34=6449=FIXTHALES252=20101116-14:31:12.46156=ROMFIX1=THLS 501 7YPX1209 GSEC THLEQ11=1011121=138=2240=244=-6.9247=B54=155=ANF59=060=20101116-14:31:12109=JD1:JKHOURY:seeker1SPI_SpreadServerSRON-CHI-SKR12167=MLEG204=0311=ANF386=1440=690555=2600=ANF610=201101624=2612=43613=1623=1654=L1564=O608=OC611=20110122600=ANF610=201101624=2612=42613=0623=1654=L2564=O608=OP611=2011012210001=18710=035";
    char *rom_res =
        "!#!E,,,L1,2,ANF,1,,,,,,,,,,,,,,,,,,,,,,,,201101,C,43,,,O,,,1,,,,,,,,,,,,,,,,,,,,,,,,22,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,!#!E,,,L2,2,ANF,1,,,,,,,,,,,,,,,,,,,,,,,,201101,P,42,,,O,,,1,,,,,,,,,,,,,,,,,,,,,,,,22,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,!#!";
    long fix_len = strlen(fix_mess);
    long ret = 0;
    struct dart_rom_pos *positions = NULL;
    int rej = 0;
    char rom_type = 'E';
    struct message *mess = NULL;
    fs_set_spread_book(ob);
    ret = check_for_spreads(mess, fix_mess, test_b, fix_len,
                            rom_type, positions, &rej, ob);
    CHKCOND(strcmp(test_b->buffer, rom_res) == 0);
    clean_dart_oo_pool(ob->mgr);
    STOPTEST
}

int test_full_spread_parse(const char **testname,
                           const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    orderbook *ob = init_order_book(complex_gen, complex_con,
                                    def_rom2fix, def_rom2fix,
                                    def_xlate_fixrom, 50000,
                                    "NYSE", 4, "NYSE",
                                    4, NULL, 0);
    char *fix_mess =
        "35=AB34=6449=FIXTHALES252=20101116-14:31:12.46156=ROMFIX1=THLS 501 7YPX1209 GSEC THLEQ11=1011121=138=2240=244=-6.9247=B54=155=ANF59=060=20101116-14:31:12109=JD1:JKHOURY:seeker1SPI_SpreadServerSRON-CHI-SKR12167=MLEG204=0311=ANF386=1440=690555=2600=ANF610=201101624=2612=43613=1623=1654=L1564=O608=OC611=20110122600=ANF610=201101624=2612=42613=0623=1654=L2564=O608=OP611=2011012210001=18710=035";
    char *rom_res =
        "E,,FIXTHALES2,10111,1,ANF,22,-6.92,2,0,B,501,7YPX1209,187,,,,,,,,,THLEQ,GSEC,THLS,,,,,,,,,ANF,,O,690,,,,,,,,,,,,,,20101116-14:31:12,,,,,,,,,,,,,,,,3,,,,,,,,JD1:JKHOURY:seeker1SPI_SpreadServerSRON-CHI-SKR12,,,,,,,,,,,,,,,,,,,,,,,,,,!#!E,,,L1,2,ANF,1,,,,,,,,,,,,,,,,,,,,,,,,201101,C,43,,,O,,,1,,,,,,,,,,,,,,,,,,,,,,,,22,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,!#!E,,,L2,2,ANF,1,,,,,,,,,,,,,,,,,,,,,,,,201101,P,42,,,O,,,1,,,,,,,,,,,,,,,,,,,,,,,,22,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,!#!\n";
    long fix_len = strlen(fix_mess);
    long ret = 0;
    int rej = 0;
    struct message mess;
    mess.name = "Jojo";
    mess.name_len = 4;
    fs_set_spread_book(ob);
    ret = trans_fix_message(&mess, fix_mess, fix_len, test_b,
                            &rej, ob, fix_rom_spread_override);
    CHKCOND(strcmp(test_b->buffer, rom_res) == 0);
    clean_dart_oo_pool(ob->mgr);
    STOPTEST
}

int test_r2f_reg_option(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    test_token_gen = init_order_token_creator();
    orderbook *ob = init_order_book(test_complex_gen_id, complex_con,
                                    def_rom2fix, def_rom2fix,
                                    def_xlate_fixrom, 50000,
                                    "NYSE", 4, "NYSE",
                                    4, NULL, 0);
    char *rom_mess =
        "E,20110121-08:30:42.761550,KNV,MWALSH2Qpah-20110121,2,SPY,50,0.01,2,0,S,501,34CX1209,182,0,,,MWALSH-10110121-2001,0,,,,QFR,GS,KNV,,,MWALSH,MWALSH-10110121-2001-0,,201101,C,131,SPY,,O,QFR,22,1,,,182,,,,,824,,,,,,,,,SPY___110122C00131000,,,,,,,22,,,,,,,,,1,,,,,,,\n";
    long rom_len = strlen(rom_mess);
    long ret = 0;
    int rej = 0;
    char fix_type[10];
    int ft_len = 0;
    struct message mess;
    mess.name = "Jojo";
    mess.name_len = 4;
    memset(fix_type, 0, 10);
    set_option_rom2fix(ob);
    fs_set_spread_book(ob);
    set_rom2fix_leg_book(ob);
    ret = from_rom_determine_spread(&mess, rom_mess, rom_len,
                                    test_b,
                                    fix_type, &ft_len, &rej,
                                    ob, rom2fix_translate_fields);
    CHKCOND(strstr(test_b->buffer, "1=34CX") != NULL);
    CHKCOND(strstr(test_b->buffer, "1209") == NULL);
    CHKCOND(strstr(test_b->buffer, "167=OPT") != NULL);
    CHKCOND(strstr(test_b->buffer, "440=QFR") != NULL);
    CHKCOND(strstr(test_b->buffer, "55=SPY") != NULL);
    CHKCOND(strstr(test_b->buffer, "59=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "40=2") != NULL);
    CHKCOND(strstr(test_b->buffer, "38=50") != NULL);
    CHKCOND(strstr(test_b->buffer, "54=2") != NULL);
    CHKCOND(strstr(test_b->buffer, "202=131") != NULL);
    CHKCOND(strstr(test_b->buffer, "200=201101") != NULL);
    CHKCOND(strstr(test_b->buffer, "201=1") != NULL);
    CHKCOND(strstr(test_b->buffer, "204=5") != NULL);
    CHKCOND(strstr(test_b->buffer, "439=824") != NULL);
    clean_dart_oo_pool(ob->mgr);
    STOPTEST
}

int test_r2f_reg_opt_cancel(const char **testname,
                            const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    databuf_t *test_b2 = databuf_alloc(500);
    REQCOND(test_b != 0);
    test_token_gen = init_order_token_creator();
    orderbook *ob = init_order_book(test_complex_gen_id, complex_con,
                                    def_rom2fix, def_rom2fix,
                                    def_xlate_fixrom, 50000,
                                    "NYSE", 4, "NYSE",
                                    4, NULL, 0);
    char *rom_mess =
        "E,143816,FIXNGT5,18744210180000,2,ACI,10,2.65,2,0,S,501,32FY1209,182,,,,FIXNGT5-10110128-2227,,,,,NGT,GSEC,NGT,,ACI,FIXNGT5,FIXNGT5-10110128-2227-0,,201104,P,32,,,O,NGT,21,O,,,182,,,,NGT,,,,,20110128-14:38:15,,,,,,,,,,,,16,,,,,,,,,7,,,,,,,,,\n";
    char *rom_mess2 =
        "C,143821,FIXNGT5,18744210180000,2,ACI,10,,,,,,,,,,,FIXNGT5-10110128-2227,,,,,,,,,ACI,FIXNGT5,1,,,,32,,,,,,,,,,,,,NGT 501 32FY1209 GSEC NGT,,,,,20110128-14:38:21,,,,,,,,,,18744210180001,,,,,,,,,,,8,,,,,,,,,\n";
    long rom_len = strlen(rom_mess);
    long rom_len2 = strlen(rom_mess2);
    long ret = 0;
    int rej = 0;
    char fix_type[10];
    int ft_len = 0;
    struct message mess;
    mess.name = "Jojo";
    mess.name_len = 4;
    memset(fix_type, 0, 10);
    set_option_rom2fix(ob);
    fs_set_spread_book(ob);
    set_rom2fix_leg_book(ob);
    ret = from_rom_determine_spread(&mess, rom_mess, rom_len,
                                    test_b,
                                    fix_type, &ft_len, &rej,
                                    ob, rom2fix_translate_fields);
    CHKCOND(strstr(test_b->buffer, "1=32FY") != NULL);
    CHKCOND(strstr(test_b->buffer, "1209") == NULL);
    CHKCOND(strstr(test_b->buffer, "167=OPT") != NULL);
    CHKCOND(strstr(test_b->buffer, "440=NGT") != NULL);
    CHKCOND(strstr(test_b->buffer, "55=ACI") != NULL);
    CHKCOND(strstr(test_b->buffer, "59=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "40=2") != NULL);
    CHKCOND(strstr(test_b->buffer, "38=10") != NULL);
    CHKCOND(strstr(test_b->buffer, "54=2") != NULL);
    CHKCOND(strstr(test_b->buffer, "202=32") != NULL);
    CHKCOND(strstr(test_b->buffer, "205=16") != NULL);
    CHKCOND(strstr(test_b->buffer, "200=201104") != NULL);
    CHKCOND(strstr(test_b->buffer, "201=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "204=4") != NULL);
    ret = from_rom_determine_spread(&mess, rom_mess2, rom_len2,
                                    test_b2, fix_type, &ft_len, &rej,
                                    ob, rom2fix_translate_fields);
    //              printf("Cancel: %s \n", test_b2->buffer);
    CHKCOND(strstr(test_b2->buffer, "202=32") != NULL);
    CHKCOND(strstr(test_b2->buffer, "204=4") != NULL);
    CHKCOND(strstr(test_b2->buffer, "167=OPT") != NULL);
    CHKCOND(strstr(test_b2->buffer, "201=0") != NULL);
    CHKCOND(strstr(test_b2->buffer, "200=201104") != NULL);
    CHKCOND(strstr(test_b2->buffer, "205=16") != NULL);
    clean_dart_oo_pool(ob->mgr);

    STOPTEST
}

int test_r2f_spread_cancel(const char **testname,
                           const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    databuf_t *test_b2 = databuf_alloc(500);
    REQCOND(test_b != 0);
    test_token_gen = init_order_token_creator();
    orderbook *ob = init_order_book(test_complex_gen_id, complex_con,
                                    def_rom2fix, def_rom2fix,
                                    def_xlate_fixrom, 50000,
                                    "NYSE", 4, "NYSE",
                                    4, NULL, 0);
    char *rom_mess =
        "E,144030,FIXNGT4,18574228010000,1,IYT,11,-7,2,0,S,501,32FY1209,187,,,,FIXNGT4-40110121-59198,,,,,NGT,GSEC,NGT,,,FIXNGT4,FIXNGT4-40110121-59198-0,,,,,,,OPT,NGT,21,,,,187,,,,NGT,,,,,,,,,,,,,,,,,,,,,3,,,,,1,,,,,,,,,!#!E,144030,,1857422801L0,2,IYT,1,,,,,,,,,,,FIXNGT4-40110121-59199,,,,,,,,,,,,,201103,C,92,,,OC,,,1,,,,,,,,,,,,,,,,,,,,,,,,19,,,,,,,,,,,,,,,,,,!#!E,144030,,1857422801L1,2,IYT,1,,,,,,,,,,,FIXNGT4-40110121-59200,,,,,,,,,,,,,201103,P,92,,,OP,,,1,,,,,,,,,,,,,,,,,,,,,,,,19,,,,,,,,,,,,,,,,,,!#!\n";
    char *rom_mess2 =
        "C,145347,FIXNGT4,18574228010000,1,IYT,11,,,,,,,,,,,FIXNGT4-40110121-59198,,,,,,,,,,FIXNGT4,1,,,,,,,,,,,,,,,,,NGT 501 32FY1209 GSEC NGT,,,,,20110121-14:53:47,,,,,,,,,,18574228010001,,,,,,,,,,,78,,,,,,,,,\n";
    long rom_len2 = strlen(rom_mess2);
    long rom_len = strlen(rom_mess);
    long ret = 0;
    int rej = 0;
    char fix_type[10];
    int ft_len = 0;
    struct message mess;
    mess.name = "Jojo";
    mess.name_len = 4;
    memset(fix_type, 0, 10);
    set_option_rom2fix(ob);
    fs_set_spread_book(ob);
    set_rom2fix_leg_book(ob);
    ret = from_rom_determine_spread(&mess, rom_mess, rom_len,
                                    test_b,
                                    fix_type, &ft_len, &rej,
                                    ob, rom2fix_translate_fields);
    CHKCOND(strstr(test_b->buffer, "555=2") != NULL);
    CHKCOND(strstr(test_b->buffer, "59=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "167=MLEG") != NULL);
    CHKCOND(strstr(test_b->buffer, "440=NGT") != NULL);
    CHKCOND(strstr(test_b->buffer, "1=32FY") != NULL);
    CHKCOND(strstr(test_b->buffer, "1209") == NULL);
    CHKCOND(strstr(test_b->buffer, "600=IYT") != NULL);
    CHKCOND(strstr(test_b->buffer, "623=1") != NULL);
    CHKCOND(strstr(test_b->buffer, "608=OP") != NULL);
    CHKCOND(strstr(test_b->buffer, "608=OC") != NULL);
    //        printf("FIX TRANS: %s \n", test_b->buffer);
    ret = from_rom_determine_spread(&mess, rom_mess2, rom_len2,
                                    test_b2, fix_type, &ft_len, &rej,
                                    ob, rom2fix_translate_fields);
    //        printf("Cancel: %s \n", test_b2->buffer);
    CHKCOND(strstr(test_b2->buffer, "555=2") != NULL);
    CHKCOND(strstr(test_b2->buffer, "600=IYT") != NULL);
    CHKCOND(strstr(test_b2->buffer, "167=MLEG") != NULL);
    CHKCOND(strstr(test_b2->buffer, "623=1") != NULL);
    CHKCOND(strstr(test_b2->buffer, "608=OP") != NULL);
    CHKCOND(strstr(test_b2->buffer, "608=OC") != NULL);

    clean_dart_oo_pool(ob->mgr);
    STOPTEST
}

int test_r2f_spread_parse(const char **testname,
                          const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    test_token_gen = init_order_token_creator();
    orderbook *ob = init_order_book(test_complex_gen_id, complex_con,
                                    def_rom2fix, def_rom2fix,
                                    def_xlate_fixrom, 50000,
                                    "NYSE", 4, "NYSE",
                                    4, NULL, 0);
    char *rom_mess =
        "E,143923,FIXNGT4,18190873140000,1,ABV,25,-6.81,2,0,S,501,32FY1209,180,,,,FIXNGT4-40110105-59023,,,,,NGT,GSEC,NGT,,,FIXNGT4,FIXNGT4-40110105-59023-0,,,,,,,OPT,NGT,,,,,180,,,,NGT,,,,,,,,,,,,,,,,,,,,,3,,,,,15,,,,,,,,,!#!E,143923,,1819087314L0,2,ABV,1,,,,,,,,,,,FIXNGT4-40110105-59024,,,,,,,,,,,,,201107,C,31,,,OC,,,1,,,,,,,,,,,,,,,,,,,,,,,,16,,,,,,,,,,,,,,,,,,!#!E,143923,,1819087314L1,2,ABV,1,,,,,,,,,,,FIXNGT4-40110105-59025,,,,,,,,,,,,,201107,P,31,,,OP,,,1,,,,,,,,,,,,,,,,,,,,,,,,16,,,,,,,,,,,,,,,,,,!#!\n";
    long rom_len = strlen(rom_mess);
    long ret = 0;
    int rej = 0;
    char fix_type[10];
    int ft_len = 0;
    struct message mess;
    mess.name = "Jojo";
    mess.name_len = 4;
    memset(fix_type, 0, 10);
    set_option_rom2fix(ob);
    fs_set_spread_book(ob);
    ob->r2fleg[1].legfunc = trans_mleg_put_call;
    ob->r2fleg[1].rom_tag = 31;
    ob->r2fleg[4].legfunc = trans_mleg_open_close;
    ob->r2fleg[4].rom_tag = 38;
    ob->r2fleg[3].legfunc = trans_mleg_stike;
    ob->r2fleg[3].rom_tag = 32;
    ob->r2fleg[2].legfunc = trans_leg_mat_date;
    ob->r2fleg[2].rom_tag = 30;
    ob->r2fleg[0].legfunc = trans_mleg_symbol;
    ob->r2fleg[0].rom_tag = 5;
    ob->r2fleg[6].legfunc = trans_mleg_side;
    ob->r2fleg[6].rom_tag = 4;
    ob->r2fleg[5].legfunc = trans_mleg_ratio;
    ob->r2fleg[5].rom_tag = 6;
    /*ob->r2fleg[31] = trans_mleg_put_call;
    ob->r2fleg[38] = trans_mleg_open_close;
    ob->r2fleg[32] = trans_mleg_stike;
    ob->r2fleg[30] = trans_leg_mat_date;
    ob->r2fleg[5] = trans_mleg_symbol;
    ob->r2fleg[4] = trans_mleg_side;
    ob->r2fleg[6] = trans_mleg_ratio;*/
    ret = from_rom_determine_spread(&mess, rom_mess, rom_len,
                                    test_b,
                                    fix_type, &ft_len, &rej,
                                    ob, rom2fix_translate_fields);
    //printf("\n%s\n", test_b->buffer);
    CHKCOND(strstr(test_b->buffer, "555=2") != NULL);
    CHKCOND(strstr(test_b->buffer, "600=ABV") != NULL);
    CHKCOND(strstr(test_b->buffer, "623=1") != NULL);
    CHKCOND(strstr(test_b->buffer, "608=OP") != NULL);
    CHKCOND(strstr(test_b->buffer, "608=OC") != NULL);
    CHKCOND(strstr(test_b->buffer, "1=32FY") != NULL);
    CHKCOND(strstr(test_b->buffer, "1209") == NULL);

    CHKCOND(strstr(test_b->buffer, "167=MLEG") != NULL);
    CHKCOND(strstr(test_b->buffer, "440=NGT") != NULL);
    clean_dart_oo_pool(ob->mgr);
    STOPTEST
}

int test_full_spread_cancel(const char **testname,
                            const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    orderbook *ob = init_order_book(complex_gen, complex_con,
                                    def_rom2fix, def_rom2fix,
                                    def_xlate_fixrom, 50000,
                                    "NYSE", 4, "NYSE",
                                    4, NULL, 0);
    char *fix_mess =
        "35=F34=6549=FIXTHALES252=20101116-14:31:14.46156=ROMFIX1=THLS 501 7YPX1209 GSEC THLEQ11=1011238=2241=1011144=-6.9247=B54=155=ANF59=060=20101116-14:31:14109=DF1:seeker1SPI_SpreadServerSRON-CHI-SKR12167=MLEG204=0386=1440=690555=2600=ANF610=201101624=2612=43613=1623=1654=L1564=O608=OC611=20110122600=ANF610=201101624=2612=42613=0623=1654=L2564=O608=OP611=2011012210=250";
    char *rom_res =
        "C,,FIXTHALES2,10111,1,ANF,22,-6.92,,0,B,,,,,,,,,,,,,,,,,,,,,,,,,O,,,,,,,,,,,,,,,20101116-14:31:14,,,,,,,,,,10112,,,,,,3,,,,,,,,DF1:seeker1SPI_SpreadServerSRON-CHI-SKR12,,,,,,,,,,,,,,,,,,,,,,,,,,!#!C,,,L1,2,ANF,1,,,,,,,,,,,,,,,,,,,,,,,,201101,C,43,,,O,,,1,,,,,,,,,,,,,,,,,,,,,,,,22,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,!#!C,,,L2,2,ANF,1,,,,,,,,,,,,,,,,,,,,,,,,201101,P,42,,,O,,,1,,,,,,,,,,,,,,,,,,,,,,,,22,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,!#!\n";

    long fix_len = strlen(fix_mess);
    long ret = 0;
    int rej = 0;
    struct message mess;
    mess.name = "Jojo";
    mess.name_len = 4;
    fs_set_spread_book(ob);
    ret = trans_fix_message(&mess, fix_mess, fix_len, test_b,
                            &rej, ob, fix_rom_spread_override);
    CHKCOND(strcmp(test_b->buffer, rom_res) == 0);
    clean_dart_oo_pool(ob->mgr);
    STOPTEST
}

int test_spread_fr_open_mess(const char **testname,
                             const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    orderbook *ob = init_order_book(complex_gen, complex_con,
                                    def_rom2fix, def_rom2fix,
                                    def_xlate_fixrom, 50000,
                                    "NYSE", 4, "NYSE",
                                    4, NULL, 0);
    char *rom_mess =
        "S,143112,,L1,2,ANF,22,-6.92,2,0,,,,187,13,I000154A,9A6D7747:83CC338C_2_ANF1JAN43.0C,FIXTHALES2-10101116-2226,,,,,,,,,7YPX,,FIXTHALES2-10101116-2226-1,,201101,C,43.000000,,,O,,,1,,,,0,,0,7YPX,,,0,22,20101116-14:31:13,0.000000,,,178000070,,,,,,,0,22,,,10111,6,8,,,,1,,,JD1:JKHOURY:seeker1S,0,,,,,\n";
    long rom_len = strlen(rom_mess);
    long ret = 0;
    int rej = 0;
    char fix_type = 'S';
    int ft_len = 0;
    struct message mess;
    mess.name = "Jojo";
    mess.name_len = 4;
    fs_set_spread_book(ob);
    ret = trans_rom_message(&mess, rom_mess, rom_len, test_b,
                            &fix_type, &ft_len, &rej, ob,
                            default_rom_to_fix_override);
    //printf("New: %s \n", test_b->buffer);
    //printf("Old: %s \n", fix_mess);
    CHKCOND(strstr(test_b->buffer, "11=L1") != NULL);
    CHKCOND(strstr(test_b->buffer, "167=OPT") != NULL);
    CHKCOND(strstr(test_b->buffer, "200=201101") != NULL);
    CHKCOND(strstr(test_b->buffer, "201=1") != NULL);
    CHKCOND(strstr(test_b->buffer, "202=43.000000") != NULL);
    CHKCOND(strstr(test_b->buffer, "205=22") != NULL);
    CHKCOND(strstr(test_b->buffer, "37=FIXTHALES2-10101116-2226") != NULL);
    CHKCOND(strstr(test_b->buffer, "150=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "39=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "151=22") != NULL);
    CHKCOND(strstr(test_b->buffer, "17=FIXTHALES2-10101116-2226-1") !=
            NULL);

    databuf_reset(test_b);
    char *rom_mess2 =
        "S,143112,FIXTHALES2,10111,1,ANF,22,,2,,,,7YPX,187,13,,,FIXTHALES2-10101116-2225,,,,,,GSEC,,,,,FIXTHALES2-10101116-2225-1,,,,0.000000,,,O,,,0,,,,0,,0,,,,0,22,,0.000000,,,,,,,,,,,,,,,3,,,,,2,,,JD1:JKHOURY:seeker1S,,,,,,\n";
    rom_len = strlen(rom_mess2);
    ret = trans_rom_message(&mess, rom_mess2, rom_len, test_b,
                            &fix_type, &ft_len, &rej, ob,
                            default_rom_to_fix_override);
    //printf("New: %s \n", test_b->buffer);
    //printf("Old: %s \n", fix_mess);
    CHKCOND(strstr(test_b->buffer, "11=10111") != NULL);
    CHKCOND(strstr(test_b->buffer, "167=OPT") != NULL);
    CHKCOND(strstr(test_b->buffer, "202=0.000000") != NULL);
    CHKCOND(strstr(test_b->buffer, "37=FIXTHALES2-10101116-2225") != NULL);
    CHKCOND(strstr(test_b->buffer, "150=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "39=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "151=22") != NULL);
    CHKCOND(strstr(test_b->buffer, "17=FIXTHALES2-10101116-2225-1") !=
            NULL);

    databuf_reset(test_b);
    char *rom_mess3 =
        "S,143112,,L2,2,ANF,22,-6.92,2,0,,,,187,13,I000154B,9A6D7747:83CC338C_2_ANF1JAN42.0P,FIXTHALES2-10101116-2227,,,,,,,,,7YPX,,FIXTHALES2-10101116-2227-1,,201101,P,42.000000,,,O,,,1,,,,0,,0,7YPX,,,0,22,20101116-14:31:13,0.000000,,,178000071,,,,,,,0,22,,,10111,6,8,,,,3,,,JD1:JKHOURY:seeker1S,0,,,,,\n";
    rom_len = strlen(rom_mess3);
    ret = trans_rom_message(&mess, rom_mess3, rom_len, test_b,
                            &fix_type, &ft_len, &rej, ob,
                            default_rom_to_fix_override);
    //printf("New: %s \n", test_b->buffer);
    //printf("Old: %s \n", fix_mess);
    CHKCOND(strstr(test_b->buffer, "11=L2") != NULL);
    CHKCOND(strstr(test_b->buffer, "167=OPT") != NULL);
    CHKCOND(strstr(test_b->buffer, "200=201101") != NULL);
    CHKCOND(strstr(test_b->buffer, "201=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "202=42.000000") != NULL);
    CHKCOND(strstr(test_b->buffer, "205=22") != NULL);
    CHKCOND(strstr(test_b->buffer, "37=FIXTHALES2-10101116-2227") != NULL);
    CHKCOND(strstr(test_b->buffer, "150=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "39=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "151=22") != NULL);
    CHKCOND(strstr(test_b->buffer, "17=FIXTHALES2-10101116-2227-1") !=
            NULL);

    databuf_reset(test_b);
    clean_dart_oo_pool(ob->mgr);
    STOPTEST
}

static void create_fix_test_message(char *seed_msg_1, char *seed_msg_2,
                                    dart_ex_order_obj * obj, long *size,
                                    int leg_pos, databuf_t * ret_val,
                                    char *id_val)
{
    char *leg_off = id_val;
    size_t clordid_len = 0;
    char *cl_ord_id = doa_get_cl_ord_id(obj, &clordid_len);
    int len = sprintf(id_val, "%d", leg_pos);
    char* cp_point = cl_ord_id;
    int left_over = 5 - len;
    int cp_len = 0;
    leg_off += len;
    cp_len = left_over > clordid_len ? clordid_len : left_over;
    cp_point += (clordid_len - cp_len);
    memcpy(leg_off, cp_point, cp_len);
    //cp_len = left_over > clordid_len ? clordid_len : left_over;
    //memcpy(leg_off, cl_ord_id, cp_len);
    databuf_write(ret_val, "%s%s%s", seed_msg_1, id_val, seed_msg_2);
    *size = databuf_unread(ret_val);
}

int test_option_life_cycle(const char **testname,
                           const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    test_token_gen = init_order_token_creator();
    orderbook *ob = init_order_book(test_complex_gen_id, complex_con,
                                    def_rom2fix, def_rom2fix,
                                    def_xlate_fixrom, 50000,
                                    "NYSE", 4, "NYSE",
                                    4, NULL, 0);

    char *fix_mess =
        "35=D34=9549=FIXWANG50=WANGJ52=20101201-15:05:07.83556=ROMFIX1=WANGJ 501 7YTC1209 GSEC WANGJ11=090507_0_021=138=10040=244=23.0447=A54=255=KRE58=INET-O59=060=20101201-15:05:07.783167=CS10001=14610=207600=3";
    int fix_len = (strlen(fix_mess) - 5);
    long ret = 0;
    int rej = 0;
    char fix_type[10];
    struct message mess;
    mess.name = "Jojo";
    mess.name_len = 4;
    memset(fix_type, 0, 10);
    set_option_rom2fix(ob);
    fs_set_spread_book(ob);
    set_rom2fix_leg_book(ob);
    set_option_fix_to_rom(ob);
    ret = trans_fix_message(&mess, fix_mess, fix_len, test_b,
                            &rej, ob, fix_rom_spread_override);
    printf("mess:{%s} \n", test_b->buffer);
    CHKCOND(strstr(test_b->buffer, "!#!") == 0);
    clean_dart_oo_pool(ob->mgr);
STOPTEST}

int test_r2f_spread_life_cycle(const char **testname,
                               const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    databuf_t *test_b2 = databuf_alloc(500);
    REQCOND(test_b != 0);
    test_token_gen = init_order_token_creator();
    orderbook *ob = init_order_book(test_complex_gen_id, complex_con,
                                    def_rom2fix, def_rom2fix,
                                    def_xlate_fixrom, 50000,
                                    "NYSE", 4, "NYSE",
                                    4, NULL, 0);
    char *rom_mess =
        "E,143223,FIXNGT4,18700032010000,1,UAL,150,-1.9,2,0,S,501,32FY1209,187,,,,FIXNGT4-40110127-55691,,,,,NGT,GSEC,NGT,,,FIXNGT4,FIXNGT4-40110127-55691-0,,,,,,,OPT,NGT,21,,,,187,,,,NGT,,,,,,,,,,,,,,,,,,,,,3,,,,,1,,,,,,,,,!#!E,143223,,1870003201L0,2,UAL,1,,,,,,,,,,,FIXNGT4-40110127-55692,,,,,,,,,,,,,201102,C,26,,,OC,,,1,,,,,,,,,,,,,,,,,,,,,,,,19,,,,,,,,,,,,,,,,,,!#!E,143223,,1870003201L1,2,UAL,1,,,,,,,,,,,FIXNGT4-40110127-55693,,,,,,,,,,,,,201102,P,26,,,OP,,,1,,,,,,,,,,,,,,,,,,,,,,,,19,,,,,,,,,,,,,,,,,,!#!\nE,23456,KJLKJI,134,!#!";
    char *rom_mess2 =
        "C,143245,FIXNGT4,18700032010000,1,UAL,150,,,,,,,,,,,FIXNGT4-40110127-55691,,,,,,,,,,FIXNGT4,1,,,,,,,,,,,,,,,,,NGT 501 32FY1209 GSEC NGT,,,,,20110127-14:32:45,,,,,,,,,,18700032010001,,,,,,,,,,,2,,,,,,,,,\n";
    long rom_len2 = strlen(rom_mess2);
    long rom_len = strlen(rom_mess);
    char *fix_open1 =
        "35=849=ISE56=DART234=15652=20110127-14:32:2257=FOC37=236FB1ED:66CA168A_2_UAL1FEB26.0C11=2011012715617=185200620150=039=01=32FY55=UAL167=MLEG200=201102202=26442=254=238=15040=244=-1.9059=0204=432=031=0151=15014=06=060=20110127-14:32:2377=O58=32FY654=";
    char *fix_open1_p2 = "201=1205=1920=010=093";
    char *fix_open2 =
        "35=849=ISE56=DART234=15752=20110127-14:32:2257=FOC37=236FB1ED:66CA168A_2_UAL1FEB26.0P11=2011012715617=185200621150=039=01=32FY55=UAL167=MLEG200=201102202=26442=254=238=15040=244=-1.9059=0204=432=031=0151=15014=06=060=20110127-14:32:2377=O58=32FY654=";
    char *fix_open2_p2 = "201=0205=1920=010=108";
    char *fix_pfill1_1 =
        "35=849=ISE56=DART234=15852=20110127-14:32:2257=FOC37=236FB1ED:66CA168A_2_UAL1FEB26.0C11=2011012715617=185200622150=139=11=32FY55=UAL167=MLEG200=201102202=26442=254=238=15040=244=-1.9059=0204=432=1431=1.04151=13614=146=1.0460=20110127-14:32:2377=O58=32FY654=";
    char *fix_pfill1_2 =
        "9730=M9205=105440=NGT201=1205=1920=010=189";


    char *fix_pfill2_1 =
        "35=849=ISE56=DART234=15952=20110127-14:32:2257=FOC37=236FB1ED:66CA168A_2_UAL1FEB26.0P11=2011012715617=185200623150=139=11=32FY55=UAL167=MLEG200=201102202=26442=254=238=15040=244=-1.9059=0204=432=1431=0.86151=13614=146=0.8660=20110127-14:32:2377=O58=32FY654=";
    char *fix_pfill2_2 =
        "9730=M9205=105440=NGT201=0205=1920=010=222";
    char *fix_out1_1 =
        "35=849=ISE56=DART234=16252=20110127-14:32:4557=FOC37=236FB1ED:66CA168A_2_UAL1FEB26.0C11=2011012715741=2011012715617=185200626150=439=41=32FY55=UAL167=MLEG200=201102202=26442=254=238=15040=244=-1.9059=0204=432=031=0151=014=146=1.0460=20110127-14:32:4577=O58=32FY654=";
    char *fix_out1_2 = "201=1205=1920=010=160";

    char *fix_out2_1 =
        "35=849=ISE56=DART234=16352=20110127-14:32:4557=FOC37=236FB1ED:66CA168A_2_UAL1FEB26.0P11=2011012715741=2011012715617=185200627150=439=41=32FY55=UAL167=MLEG200=201102202=26442=254=238=15040=244=-1.9059=0204=432=031=0151=014=146=0.8660=20110127-14:32:4577=O58=32FY654=";
    char *fix_out2_2 = "201=0205=1920=010=184";
    long ret = 0;
    int rej = 0;
    char fix_type[10];
    int ft_len = 0;
    struct message mess;
    mess.name = "Jojo";
    mess.name_len = 4;
    memset(fix_type, 0, 10);
    set_option_rom2fix(ob);
    fs_set_spread_book(ob);
    set_rom2fix_leg_book(ob);
    set_option_fix_to_rom(ob);
    ret = from_rom_determine_spread(&mess, rom_mess, rom_len,
                                    test_b,
                                    fix_type, &ft_len, &rej,
                                    ob, rom2fix_translate_fields);
    CHKCOND(strstr(test_b->buffer, "555=2") != NULL);
    CHKCOND(strstr(test_b->buffer, "59=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "167=MLEG") != NULL);
    CHKCOND(strstr(test_b->buffer, "440=NGT") != NULL);
    CHKCOND(strstr(test_b->buffer, "1=32FY") != NULL);
    CHKCOND(strstr(test_b->buffer, "1209") == NULL);
    CHKCOND(strstr(test_b->buffer, "600=UAL") != NULL);
    CHKCOND(strstr(test_b->buffer, "623=1") != NULL);
    CHKCOND(strstr(test_b->buffer, "608=OP") != NULL);
    CHKCOND(strstr(test_b->buffer, "608=OC") != NULL);
    databuf_reset(test_b);
    dart_ex_order_obj *obj = NULL;
    get_map_entry(ob->rom_ord_id_map, "FIXNGT4-40110127-55691", 22,
                  (void *) &obj);
    no_copy_insert(ob->cl_ord_id_map, "20110127156", 11, obj);
    long fix_open1_len = 0;
    databuf_t *ret_val = databuf_alloc(500);
    char *id_val_one = calloc(11, 1);
    create_fix_test_message(fix_open1, fix_open1_p2,
                            obj, &fix_open1_len, 1, ret_val, id_val_one);
    ret = trans_fix_message(&mess, ret_val->buffer, fix_open1_len, test_b,
                            &rej, ob, def_fix_to_rom_override);
    databuf_t *rom_mess_one = databuf_alloc(500);
    char *p1 = "S,,,,2,UAL,150,-1.90,2,0,,,,,13,";
    char *p2 =
        ",236FB1ED:66CA168A_2_UAL1FEB26.0C,FIXNGT4-40110127-55692,,,,,,,,,,,,,201102,C,26,,,,,,1,,,,0,,0,,,,0,150,20110127-14:32:23,0,,,185200620,,,,,,,,19,,,,6,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";
    databuf_write(rom_mess_one, "S,,,,,,,,,,,,,,13,20110127156,,FIXNGT4-40110127-55691,,,,,,,,,,,,,,,,,,,,,,,,,,,\n%s%s%s", p1, id_val_one, p2);
    CHKCOND(strcmp(rom_mess_one->buffer, test_b->buffer) == 0);
    //printf("\n%s\n%s\n", test_b->buffer, rom_mess_one->buffer);
    databuf_reset(test_b);
    databuf_reset(ret_val);
    long fix_open2_len = 0;
    char *id_val_two = calloc(11, 1);
    databuf_t *rom_mess_two = databuf_alloc(500);
    char *pp1 = "S,,,,2,UAL,150,-1.90,2,0,,,,,13,";
    char *pp2 =
        ",236FB1ED:66CA168A_2_UAL1FEB26.0P,FIXNGT4-40110127-55693,,,,,,,,,,,,,201102,P,26,,,,,,1,,,,0,,0,,,,0,150,20110127-14:32:23,0,,,185200621,,,,,,,,19,,,,6,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";
    create_fix_test_message(fix_open2, fix_open2_p2, obj, &fix_open2_len,
                            2, ret_val, id_val_two);
    databuf_write(rom_mess_two, "%s%s%s", pp1, id_val_two, pp2);

    ret = trans_fix_message(&mess, ret_val->buffer, fix_open2_len, test_b,
                            &rej, ob, def_fix_to_rom_override);
    CHKCOND(strcmp(rom_mess_two->buffer, test_b->buffer) == 0);
    databuf_reset(test_b);
    databuf_reset(ret_val);
    databuf_reset(rom_mess_one);
    long fix_open3_len = 0;
    char *id_val_three = calloc(11, 1);
    char *fp1 = "S,,,,2,UAL,150,-1.90,2,0,,,,,1,";
    char *fp2 =
        ",236FB1ED:66CA168A_2_UAL1FEB26.0C,FIXNGT4-40110127-55692,,,,,,,,,,,,,201102,C,26,,,,NGT,,1,,,,14,,1.04,,,,14,136,20110127-14:32:23,1.04,,,185200622,,,,,,,,19,,,,6,,,M,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";

    create_fix_test_message(fix_pfill1_1, fix_pfill1_2,
                            obj, &fix_open3_len, 1, ret_val, id_val_three);
    databuf_write(rom_mess_one, "S,,,,,,,,,,,,,,1,20110127156,,FIXNGT4-40110127-55691,,,,,,,,,,,,,,,,,,,,,,,,,,,\n%s%s%s", fp1, id_val_three, fp2);
    ret = trans_fix_message(&mess, ret_val->buffer, fix_open3_len, test_b,
                            &rej, ob, def_fix_to_rom_override);
    CHKCOND(strcmp(rom_mess_one->buffer, test_b->buffer) == 0);
    //printf("\n%s\n%s\n", test_b->buffer, rom_mess_one->buffer);
    databuf_reset(test_b);
    databuf_reset(ret_val);
    databuf_reset(rom_mess_one);
    long fix_open4_len = 0;
    char *id_val_four = calloc(11, 1);
    char *fp21 = "S,,,,2,UAL,150,-1.90,2,0,,,,,1,";
    char *fp22 =
        ",236FB1ED:66CA168A_2_UAL1FEB26.0P,FIXNGT4-40110127-55693,,,,,,,,,,,,,201102,P,26,,,,NGT,,1,,,,14,,0.86,,,,14,136,20110127-14:32:23,0.86,,,185200623,,,,,,,,19,,,,6,,,M,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";

    create_fix_test_message(fix_pfill2_1, fix_pfill2_2,
                            obj, &fix_open4_len, 2, ret_val, id_val_four);
    databuf_write(rom_mess_one, "%s%s%s", fp21, id_val_four, fp22);
    ret = trans_fix_message(&mess, ret_val->buffer, fix_open4_len, test_b,
                            &rej, ob, def_fix_to_rom_override);
    CHKCOND(strcmp(rom_mess_one->buffer, test_b->buffer) == 0);
    ret = from_rom_determine_spread(&mess, rom_mess2, rom_len2,
                                    test_b2, fix_type, &ft_len, &rej,
                                    ob, rom2fix_translate_fields);
    CHKCOND(strstr(test_b2->buffer, "555=2") != NULL);
    CHKCOND(strstr(test_b2->buffer, "600=UAL") != NULL);
    CHKCOND(strstr(test_b2->buffer, "167=MLEG") != NULL);
    CHKCOND(strstr(test_b2->buffer, "623=1") != NULL);
    CHKCOND(strstr(test_b2->buffer, "608=OP") != NULL);
    CHKCOND(strstr(test_b2->buffer, "608=OC") != NULL);
    databuf_reset(test_b);
    databuf_reset(ret_val);
    databuf_reset(rom_mess_one);
    long fix_open5_len = 0;
    char *id_val_five = calloc(11, 1);
    char *fout1 = "S,,,,2,UAL,150,-1.90,2,0,,,,,4,";
    char *fout2 =
        ",236FB1ED:66CA168A_2_UAL1FEB26.0C,FIXNGT4-40110127-55692,,,,,,,,,,,,,201102,C,26,,,,,,1,,,,0,,0,,,,14,0,20110127-14:32:45,1.04,,,185200626,,,,,,20110127156,,19,,,,6,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";


    create_fix_test_message(fix_out1_1, fix_out1_2,
                            obj, &fix_open5_len, 1, ret_val, id_val_five);
    databuf_write(rom_mess_one, "S,,,,,,,,,,,,,,4,20110127157,,FIXNGT4-40110127-55691,,,,,,,,,,,,,,,,,,,,,,,,,,,\n%s%s%s", fout1, id_val_five, fout2);
    ret = trans_fix_message(&mess, ret_val->buffer, fix_open5_len, test_b,
                            &rej, ob, def_fix_to_rom_override);
    CHKCOND(strcmp(rom_mess_one->buffer, test_b->buffer) == 0);
    //printf("\n%s\n%s\n", test_b->buffer, rom_mess_one->buffer);
    databuf_reset(test_b);
    databuf_reset(ret_val);
    databuf_reset(rom_mess_one);
    long fix_open6_len = 0;
    char *id_val_six = calloc(11, 1);
    char *fout21 = "S,,,,2,UAL,150,-1.90,2,0,,,,,4,";
    char *fout22 =
        ",236FB1ED:66CA168A_2_UAL1FEB26.0P,FIXNGT4-40110127-55693,,,,,,,,,,,,,201102,P,26,,,,,,1,,,,0,,0,,,,14,0,20110127-14:32:45,0.86,,,185200627,,,,,,20110127156,,19,,,,6,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";



    create_fix_test_message(fix_out2_1, fix_out2_2,
                            obj, &fix_open6_len, 2, ret_val, id_val_six);
    databuf_write(rom_mess_one, "%s%s%s", fout21, id_val_six, fout22);
    ret = trans_fix_message(&mess, ret_val->buffer, fix_open6_len, test_b,
                            &rej, ob, def_fix_to_rom_override);
    CHKCOND(strcmp(rom_mess_one->buffer, test_b->buffer) == 0);
    clean_dart_oo_pool(ob->mgr);
    STOPTEST
}
int test_r2f_spread_life_cycle2(const char **testname,
                               const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    databuf_t *test_b2 = databuf_alloc(500);
    REQCOND(test_b != 0);
    test_token_gen = init_order_token_creator();
    orderbook *ob = init_order_book(test_complex_gen_id, complex_con,
                                    def_rom2fix, def_rom2fix,
                                    def_xlate_fixrom, 50000,
                                    "NYSE", 4, "NYSE",
                                    4, NULL, 0);
    char *rom_mess =
        "E,143223,FIXNGT4,18700032010000,1,UAL,150,-1.9,2,0,S,501,32FY1209,187,,,,FIXNGT4-40110127-55691,,,,,NGT,GSEC,NGT,,,FIXNGT4,FIXNGT4-40110127-55691-0,,,,,,,OPT,NGT,21,,,,187,,,,NGT,,,,,,,,,,,,,,,,,,,,,3,,,,,1,,,,,,,,,!#!E,143223,,1870003201L0,2,UAL,1,,,,,,,,,,,FIXNGT4-40110127-55692,,,,,,,,,,,,,201102,C,26,,,OC,,,1,,,,,,,,,,,,,,,,,,,,,,,,19,,,,,,,,,,,,,,,,,,!#!E,143223,,1870003201L1,2,UAL,1,,,,,,,,,,,FIXNGT4-40110127-55693,,,,,,,,,,,,,201102,P,26,,,OP,,,1,,,,,,,,,,,,,,,,,,,,,,,,19,,,,,,,,,,,,,,,,,,!#!\n!#!";
    char *rom_mess2 =
        "C,143245,FIXNGT4,18700032010000,1,UAL,150,,,,,,,,,,,FIXNGT4-40110127-55691,,,,,,,,,,FIXNGT4,1,,,,,,,,,,,,,,,,,NGT 501 32FY1209 GSEC NGT,,,,,20110127-14:32:45,,,,,,,,,,18700032010001,,,,,,,,,,,2,,,,,,,,,\n";
    long rom_len2 = strlen(rom_mess2);
    long rom_len = strlen(rom_mess);
    char *fix_open1 =
        "35=849=ISE56=DART234=15652=20110127-14:32:2257=FOC37=236FB1ED:66CA168A_2_UAL1FEB26.0C11=2011012715617=185200620150=039=01=32FY55=UAL167=MLEG200=201102202=26442=254=238=15040=244=-1.9059=0204=432=031=0151=15014=06=060=20110127-14:32:2377=O58=32FY654=";
    char *fix_open1_p2 = "201=1205=1920=010=093";
    char *fix_open2 =
        "35=849=ISE56=DART234=15752=20110127-14:32:2257=FOC37=236FB1ED:66CA168A_2_UAL1FEB26.0P11=2011012715617=185200621150=039=01=32FY55=UAL167=MLEG200=201102202=26442=254=238=15040=244=-1.9059=0204=432=031=0151=15014=06=060=20110127-14:32:2377=O58=32FY654=";
    char *fix_open2_p2 = "201=0205=1920=010=108";
    char *fix_pfill1_1 =
        "35=849=ISE56=DART234=15852=20110127-14:32:2257=FOC37=236FB1ED:66CA168A_2_UAL1FEB26.0C11=2011012715617=185200622150=139=11=32FY55=UAL167=MLEG200=201102202=26442=254=238=15040=244=-1.9059=0204=432=1431=1.04151=13614=146=1.0460=20110127-14:32:2377=O58=32FY654=";
    char *fix_pfill1_2 =
        "9730=M9205=105440=NGT201=1205=1920=010=189";


    char *fix_pfill2_1 =
        "35=849=ISE56=DART234=15952=20110127-14:32:2257=FOC37=236FB1ED:66CA168A_2_UAL1FEB26.0P11=2011012715617=185200623150=139=11=32FY55=UAL167=MLEG200=201102202=26442=254=238=15040=244=-1.9059=0204=432=1431=0.86151=13614=146=0.8660=20110127-14:32:2377=O58=32FY654=";
    char *fix_pfill2_2 =
        "9730=M9205=105440=NGT201=0205=1920=010=222";
    char *fix_out1_1 =
        "35=849=ISE56=DART234=16252=20110127-14:32:4557=FOC37=236FB1ED:66CA168A_2_UAL1FEB26.0C11=2011012715741=2011012715617=185200626150=439=41=32FY55=UAL167=MLEG200=201102202=26442=254=238=15040=244=-1.9059=0204=432=031=0151=014=146=1.0460=20110127-14:32:4577=O58=32FY654=";
    char *fix_out1_2 = "201=1205=1920=010=160";

    char *fix_out2_1 =
        "35=849=ISE56=DART234=16352=20110127-14:32:4557=FOC37=236FB1ED:66CA168A_2_UAL1FEB26.0P11=2011012715741=2011012715617=185200627150=439=41=32FY55=UAL167=MLEG200=201102202=26442=254=238=15040=244=-1.9059=0204=432=031=0151=014=146=0.8660=20110127-14:32:4577=O58=32FY654=";
    char *fix_out2_2 = "201=0205=1920=010=184";
    long ret = 0;
    int rej = 0;
    char fix_type[10];
    int ft_len = 0;
    struct message mess;
    mess.name = "Jojo";
    mess.name_len = 4;
    memset(fix_type, 0, 10);
    set_option_rom2fix(ob);
    fs_set_spread_book(ob);
    set_rom2fix_leg_book(ob);
    set_option_fix_to_rom(ob);
    ret = from_rom_determine_spread(&mess, rom_mess, rom_len,
                                    test_b,
                                    fix_type, &ft_len, &rej,
                                    ob, rom2fix_translate_fields);
    CHKCOND(strstr(test_b->buffer, "555=2") != NULL);
    CHKCOND(strstr(test_b->buffer, "59=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "167=MLEG") != NULL);
    CHKCOND(strstr(test_b->buffer, "440=NGT") != NULL);
    CHKCOND(strstr(test_b->buffer, "1=32FY") != NULL);
    CHKCOND(strstr(test_b->buffer, "1209") == NULL);
    CHKCOND(strstr(test_b->buffer, "600=UAL") != NULL);
    CHKCOND(strstr(test_b->buffer, "623=1") != NULL);
    CHKCOND(strstr(test_b->buffer, "608=OP") != NULL);
    CHKCOND(strstr(test_b->buffer, "608=OC") != NULL);
    databuf_reset(test_b);
    dart_ex_order_obj *obj = NULL;
    get_map_entry(ob->rom_ord_id_map, "FIXNGT4-40110127-55691", 22,
                  (void *) &obj);
    no_copy_insert(ob->cl_ord_id_map, "20110127156", 11, obj);
    long fix_open1_len = 0;
    databuf_t *ret_val = databuf_alloc(500);
    char *id_val_one = calloc(11, 1);
    create_fix_test_message(fix_open1, fix_open1_p2,
                            obj, &fix_open1_len, 1, ret_val, id_val_one);
    ret = trans_fix_message(&mess, ret_val->buffer, fix_open1_len, test_b,
                            &rej, ob, def_fix_to_rom_override);
    databuf_t *rom_mess_one = databuf_alloc(500);
    char *p1 = "S,,,,2,UAL,150,-1.90,2,0,,,,,13,";
    char *p2 =
        ",236FB1ED:66CA168A_2_UAL1FEB26.0C,FIXNGT4-40110127-55692,,,,,,,,,,,,,201102,C,26,,,,,,1,,,,0,,0,,,,0,150,20110127-14:32:23,0,,,185200620,,,,,,,,19,,,,6,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";
    databuf_write(rom_mess_one, "S,,,,,,,,,,,,,,13,20110127156,,FIXNGT4-40110127-55691,,,,,,,,,,,,,,,,,,,,,,,,,,,\n%s%s%s", p1, id_val_one, p2);
    CHKCOND(strcmp(rom_mess_one->buffer, test_b->buffer) == 0);
    //printf("\n%s\n%s\n", test_b->buffer, rom_mess_one->buffer);
    databuf_reset(test_b);
    databuf_reset(ret_val);
    long fix_open2_len = 0;
    char *id_val_two = calloc(11, 1);
    databuf_t *rom_mess_two = databuf_alloc(500);
    char *pp1 = "S,,,,2,UAL,150,-1.90,2,0,,,,,13,";
    char *pp2 =
        ",236FB1ED:66CA168A_2_UAL1FEB26.0P,FIXNGT4-40110127-55693,,,,,,,,,,,,,201102,P,26,,,,,,1,,,,0,,0,,,,0,150,20110127-14:32:23,0,,,185200621,,,,,,,,19,,,,6,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";
    create_fix_test_message(fix_open2, fix_open2_p2, obj, &fix_open2_len,
                            2, ret_val, id_val_two);
    databuf_write(rom_mess_two, "%s%s%s", pp1, id_val_two, pp2);

    ret = trans_fix_message(&mess, ret_val->buffer, fix_open2_len, test_b,
                            &rej, ob, def_fix_to_rom_override);
    CHKCOND(strcmp(rom_mess_two->buffer, test_b->buffer) == 0);
    databuf_reset(test_b);
    databuf_reset(ret_val);
    databuf_reset(rom_mess_one);
    long fix_open3_len = 0;
    char *id_val_three = calloc(11, 1);
    char *fp1 = "S,,,,2,UAL,150,-1.90,2,0,,,,,1,";
    char *fp2 =
        ",236FB1ED:66CA168A_2_UAL1FEB26.0C,FIXNGT4-40110127-55692,,,,,,,,,,,,,201102,C,26,,,,NGT,,1,,,,14,,1.04,,,,14,136,20110127-14:32:23,1.04,,,185200622,,,,,,,,19,,,,6,,,M,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";

    create_fix_test_message(fix_pfill1_1, fix_pfill1_2,
                            obj, &fix_open3_len, 1, ret_val, id_val_three);
    databuf_write(rom_mess_one, "S,,,,,,,,,,,,,,1,20110127156,,FIXNGT4-40110127-55691,,,,,,,,,,,,,,,,,,,,,,,,,,,\n%s%s%s", fp1, id_val_three, fp2);
    ret = trans_fix_message(&mess, ret_val->buffer, fix_open3_len, test_b,
                            &rej, ob, def_fix_to_rom_override);
    CHKCOND(strcmp(rom_mess_one->buffer, test_b->buffer) == 0);
    //printf("\n%s\n%s\n", test_b->buffer, rom_mess_one->buffer);
    databuf_reset(test_b);
    databuf_reset(ret_val);
    databuf_reset(rom_mess_one);
    long fix_open4_len = 0;
    char *id_val_four = calloc(11, 1);
    char *fp21 = "S,,,,2,UAL,150,-1.90,2,0,,,,,1,";
    char *fp22 =
        ",236FB1ED:66CA168A_2_UAL1FEB26.0P,FIXNGT4-40110127-55693,,,,,,,,,,,,,201102,P,26,,,,NGT,,1,,,,14,,0.86,,,,14,136,20110127-14:32:23,0.86,,,185200623,,,,,,,,19,,,,6,,,M,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";

    create_fix_test_message(fix_pfill2_1, fix_pfill2_2,
                            obj, &fix_open4_len, 2, ret_val, id_val_four);
    databuf_write(rom_mess_one, "%s%s%s", fp21, id_val_four, fp22);
    ret = trans_fix_message(&mess, ret_val->buffer, fix_open4_len, test_b,
                            &rej, ob, def_fix_to_rom_override);
    CHKCOND(strcmp(rom_mess_one->buffer, test_b->buffer) == 0);
    ret = from_rom_determine_spread(&mess, rom_mess2, rom_len2,
                                    test_b2, fix_type, &ft_len, &rej,
                                    ob, rom2fix_translate_fields);
    CHKCOND(strstr(test_b2->buffer, "555=2") != NULL);
    CHKCOND(strstr(test_b2->buffer, "600=UAL") != NULL);
    CHKCOND(strstr(test_b2->buffer, "167=MLEG") != NULL);
    CHKCOND(strstr(test_b2->buffer, "623=1") != NULL);
    CHKCOND(strstr(test_b2->buffer, "608=OP") != NULL);
    CHKCOND(strstr(test_b2->buffer, "608=OC") != NULL);
    databuf_reset(test_b);
    databuf_reset(ret_val);
    databuf_reset(rom_mess_one);
    long fix_open5_len = 0;
    char *id_val_five = calloc(11, 1);
    char *fout1 = "S,,,,2,UAL,150,-1.90,2,0,,,,,4,";
    char *fout2 =
        ",236FB1ED:66CA168A_2_UAL1FEB26.0C,FIXNGT4-40110127-55692,,,,,,,,,,,,,201102,C,26,,,,,,1,,,,0,,0,,,,14,0,20110127-14:32:45,1.04,,,185200626,,,,,,20110127156,,19,,,,6,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";


    create_fix_test_message(fix_out1_1, fix_out1_2,
                            obj, &fix_open5_len, 1, ret_val, id_val_five);
    databuf_write(rom_mess_one, "S,,,,,,,,,,,,,,4,20110127157,,FIXNGT4-40110127-55691,,,,,,,,,,,,,,,,,,,,,,,,,,,\n%s%s%s", fout1, id_val_five, fout2);
    ret = trans_fix_message(&mess, ret_val->buffer, fix_open5_len, test_b,
                            &rej, ob, def_fix_to_rom_override);
    CHKCOND(strcmp(rom_mess_one->buffer, test_b->buffer) == 0);
    //printf("\n%s\n%s\n", test_b->buffer, rom_mess_one->buffer);
    databuf_reset(test_b);
    databuf_reset(ret_val);
    databuf_reset(rom_mess_one);
    long fix_open6_len = 0;
    char *id_val_six = calloc(11, 1);
    char *fout21 = "S,,,,2,UAL,150,-1.90,2,0,,,,,4,";
    char *fout22 =
        ",236FB1ED:66CA168A_2_UAL1FEB26.0P,FIXNGT4-40110127-55693,,,,,,,,,,,,,201102,P,26,,,,,,1,,,,0,,0,,,,14,0,20110127-14:32:45,0.86,,,185200627,,,,,,20110127156,,19,,,,6,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";



    create_fix_test_message(fix_out2_1, fix_out2_2,
                            obj, &fix_open6_len, 2, ret_val, id_val_six);
    databuf_write(rom_mess_one, "%s%s%s", fout21, id_val_six, fout22);
    ret = trans_fix_message(&mess, ret_val->buffer, fix_open6_len, test_b,
                            &rej, ob, def_fix_to_rom_override);
    CHKCOND(strcmp(rom_mess_one->buffer, test_b->buffer) == 0);
    clean_dart_oo_pool(ob->mgr);
    STOPTEST
}
int test_r2f_spread_life_cycle3(const char **testname,
                               const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    databuf_t *test_b2 = databuf_alloc(500);
    REQCOND(test_b != 0);
    test_token_gen = init_order_token_creator();
    orderbook *ob = init_order_book(test_complex_gen_id, complex_con,
                                    def_rom2fix, def_rom2fix,
                                    def_xlate_fixrom, 50000,
                                    "NYSE", 4, "NYSE",
                                    4, NULL, 0);
    char *rom_mess =
        "E,143223,FIXNGT4,18700032010000,1,UAL,150,-1.9,2,0,S,501,32FY1209,187,,,,FIXNGT4-40110127-55691,,,,,NGT,GSEC,NGT,,,FIXNGT4,FIXNGT4-40110127-55691-0,,,,,,,OPT,NGT,21,,,,187,,,,NGT,,,,,,,,,,,,,,,,,,,,,3,,,,,1,,,,,,,,,!#!E,143223,,1870003201L0,2,UAL,1,,,,,,,,,,,FIXNGT4-40110127-55692,,,,,,,,,,,,,201102,C,26,,,OC,,,1,,,,,,,,,,,,,,,,,,,,,,,,19,,,,,,,,,,,,,,,,,,!#!E,143223,,1870003201L1,2,UAL,1,,,,,,,,,,,FIXNGT4-40110127-55693,,,,,,,,,,,,,201102,P,26,,,OP,,,1,,,,,,,,,,,,,,,,,,,,,,,,19,,,,,,,,,,,,,,,,,,!#!\nE,23456,KJLKJI,134,!#!\n";
    char *rom_mess2 =
        "C,143245,FIXNGT4,18700032010000,1,UAL,150,,,,,,,,,,,FIXNGT4-40110127-55691,,,,,,,,,,FIXNGT4,1,,,,,,,,,,,,,,,,,NGT 501 32FY1209 GSEC NGT,,,,,20110127-14:32:45,,,,,,,,,,18700032010001,,,,,,,,,,,2,,,,,,,,,\n";
    long rom_len2 = strlen(rom_mess2);
    long temp_len = strlen(rom_mess);
    char* end = memchr(rom_mess, '\n', temp_len);
    long rom_len = (end - rom_mess) + 1;
    char *fix_open1 =
        "35=849=ISE56=DART234=15652=20110127-14:32:2257=FOC37=236FB1ED:66CA168A_2_UAL1FEB26.0C11=2011012715617=185200620150=039=01=32FY55=UAL167=MLEG200=201102202=26442=254=238=15040=244=-1.9059=0204=432=031=0151=15014=06=060=20110127-14:32:2377=O58=32FY654=";
    char *fix_open1_p2 = "201=1205=1920=010=093";
    char *fix_open2 =
        "35=849=ISE56=DART234=15752=20110127-14:32:2257=FOC37=236FB1ED:66CA168A_2_UAL1FEB26.0P11=2011012715617=185200621150=039=01=32FY55=UAL167=MLEG200=201102202=26442=254=238=15040=244=-1.9059=0204=432=031=0151=15014=06=060=20110127-14:32:2377=O58=32FY654=";
    char *fix_open2_p2 = "201=0205=1920=010=108";
    char *fix_pfill1_1 =
        "35=849=ISE56=DART234=15852=20110127-14:32:2257=FOC37=236FB1ED:66CA168A_2_UAL1FEB26.0C11=2011012715617=185200622150=139=11=32FY55=UAL167=MLEG200=201102202=26442=254=238=15040=244=-1.9059=0204=432=1431=1.04151=13614=146=1.0460=20110127-14:32:2377=O58=32FY654=";
    char *fix_pfill1_2 =
        "9730=M9205=105440=NGT201=1205=1920=010=189";


    char *fix_pfill2_1 =
        "35=849=ISE56=DART234=15952=20110127-14:32:2257=FOC37=236FB1ED:66CA168A_2_UAL1FEB26.0P11=2011012715617=185200623150=139=11=32FY55=UAL167=MLEG200=201102202=26442=254=238=15040=244=-1.9059=0204=432=1431=0.86151=13614=146=0.8660=20110127-14:32:2377=O58=32FY654=";
    char *fix_pfill2_2 =
        "9730=M9205=105440=NGT201=0205=1920=010=222";
    char *fix_out1_1 =
        "35=849=ISE56=DART234=16252=20110127-14:32:4557=FOC37=236FB1ED:66CA168A_2_UAL1FEB26.0C11=2011012715741=2011012715617=185200626150=439=41=32FY55=UAL167=MLEG200=201102202=26442=254=238=15040=244=-1.9059=0204=432=031=0151=014=146=1.0460=20110127-14:32:4577=O58=32FY654=";
    char *fix_out1_2 = "201=1205=1920=010=160";

    char *fix_out2_1 =
        "35=849=ISE56=DART234=16352=20110127-14:32:4557=FOC37=236FB1ED:66CA168A_2_UAL1FEB26.0P11=2011012715741=2011012715617=185200627150=439=41=32FY55=UAL167=MLEG200=201102202=26442=254=238=15040=244=-1.9059=0204=432=031=0151=014=146=0.8660=20110127-14:32:4577=O58=32FY654=";
    char *fix_out2_2 = "201=0205=1920=010=184";
    long ret = 0;
    int rej = 0;
    char fix_type[10];
    int ft_len = 0;
    struct message mess;
    mess.name = "Jojo";
    mess.name_len = 4;
    memset(fix_type, 0, 10);
    set_option_rom2fix(ob);
    fs_set_spread_book(ob);
    set_rom2fix_leg_book(ob);
    set_option_fix_to_rom(ob);
    ret = from_rom_determine_spread(&mess, rom_mess, rom_len,
                                    test_b,
                                    fix_type, &ft_len, &rej,
                                    ob, rom2fix_translate_fields);
    CHKCOND(strstr(test_b->buffer, "555=2") != NULL);
    CHKCOND(strstr(test_b->buffer, "59=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "167=MLEG") != NULL);
    CHKCOND(strstr(test_b->buffer, "440=NGT") != NULL);
    CHKCOND(strstr(test_b->buffer, "1=32FY") != NULL);
    CHKCOND(strstr(test_b->buffer, "1209") == NULL);
    CHKCOND(strstr(test_b->buffer, "600=UAL") != NULL);
    CHKCOND(strstr(test_b->buffer, "623=1") != NULL);
    CHKCOND(strstr(test_b->buffer, "608=OP") != NULL);
    CHKCOND(strstr(test_b->buffer, "608=OC") != NULL);
    databuf_reset(test_b);
    dart_ex_order_obj *obj = NULL;
    get_map_entry(ob->rom_ord_id_map, "FIXNGT4-40110127-55691", 22,
                  (void *) &obj);
    no_copy_insert(ob->cl_ord_id_map, "20110127156", 11, obj);
    long fix_open1_len = 0;
    databuf_t *ret_val = databuf_alloc(500);
    char *id_val_one = calloc(11, 1);
    create_fix_test_message(fix_open1, fix_open1_p2,
                            obj, &fix_open1_len, 1, ret_val, id_val_one);
    ret = trans_fix_message(&mess, ret_val->buffer, fix_open1_len, test_b,
                            &rej, ob, def_fix_to_rom_override);
    databuf_t *rom_mess_one = databuf_alloc(500);
    char *p1 = "S,,,,2,UAL,150,-1.90,2,0,,,,,13,";
    char *p2 =
        ",236FB1ED:66CA168A_2_UAL1FEB26.0C,FIXNGT4-40110127-55692,,,,,,,,,,,,,201102,C,26,,,,,,1,,,,0,,0,,,,0,150,20110127-14:32:23,0,,,185200620,,,,,,,,19,,,,6,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";
    databuf_write(rom_mess_one, "S,,,,,,,,,,,,,,13,20110127156,,FIXNGT4-40110127-55691,,,,,,,,,,,,,,,,,,,,,,,,,,,\n%s%s%s", p1, id_val_one, p2);
    CHKCOND(strcmp(rom_mess_one->buffer, test_b->buffer) == 0);
    //printf("\n%s\n%s\n", test_b->buffer, rom_mess_one->buffer);
    databuf_reset(test_b);
    databuf_reset(ret_val);
    long fix_open2_len = 0;
    char *id_val_two = calloc(11, 1);
    databuf_t *rom_mess_two = databuf_alloc(500);
    char *pp1 = "S,,,,2,UAL,150,-1.90,2,0,,,,,13,";
    char *pp2 =
        ",236FB1ED:66CA168A_2_UAL1FEB26.0P,FIXNGT4-40110127-55693,,,,,,,,,,,,,201102,P,26,,,,,,1,,,,0,,0,,,,0,150,20110127-14:32:23,0,,,185200621,,,,,,,,19,,,,6,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";
    create_fix_test_message(fix_open2, fix_open2_p2, obj, &fix_open2_len,
                            2, ret_val, id_val_two);
    databuf_write(rom_mess_two, "%s%s%s", pp1, id_val_two, pp2);

    ret = trans_fix_message(&mess, ret_val->buffer, fix_open2_len, test_b,
                            &rej, ob, def_fix_to_rom_override);
    CHKCOND(strcmp(rom_mess_two->buffer, test_b->buffer) == 0);
    databuf_reset(test_b);
    databuf_reset(ret_val);
    databuf_reset(rom_mess_one);
    long fix_open3_len = 0;
    char *id_val_three = calloc(11, 1);
    char *fp1 = "S,,,,2,UAL,150,-1.90,2,0,,,,,1,";
    char *fp2 =
        ",236FB1ED:66CA168A_2_UAL1FEB26.0C,FIXNGT4-40110127-55692,,,,,,,,,,,,,201102,C,26,,,,NGT,,1,,,,14,,1.04,,,,14,136,20110127-14:32:23,1.04,,,185200622,,,,,,,,19,,,,6,,,M,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";

    create_fix_test_message(fix_pfill1_1, fix_pfill1_2,
                            obj, &fix_open3_len, 1, ret_val, id_val_three);
    databuf_write(rom_mess_one, "S,,,,,,,,,,,,,,1,20110127156,,FIXNGT4-40110127-55691,,,,,,,,,,,,,,,,,,,,,,,,,,,\n%s%s%s", fp1, id_val_three, fp2);
    ret = trans_fix_message(&mess, ret_val->buffer, fix_open3_len, test_b,
                            &rej, ob, def_fix_to_rom_override);
    CHKCOND(strcmp(rom_mess_one->buffer, test_b->buffer) == 0);
    //printf("\n%s\n%s\n", test_b->buffer, rom_mess_one->buffer);
    databuf_reset(test_b);
    databuf_reset(ret_val);
    databuf_reset(rom_mess_one);
    long fix_open4_len = 0;
    char *id_val_four = calloc(11, 1);
    char *fp21 = "S,,,,2,UAL,150,-1.90,2,0,,,,,1,";
    char *fp22 =
        ",236FB1ED:66CA168A_2_UAL1FEB26.0P,FIXNGT4-40110127-55693,,,,,,,,,,,,,201102,P,26,,,,NGT,,1,,,,14,,0.86,,,,14,136,20110127-14:32:23,0.86,,,185200623,,,,,,,,19,,,,6,,,M,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";

    create_fix_test_message(fix_pfill2_1, fix_pfill2_2,
                            obj, &fix_open4_len, 2, ret_val, id_val_four);
    databuf_write(rom_mess_one, "%s%s%s", fp21, id_val_four, fp22);
    ret = trans_fix_message(&mess, ret_val->buffer, fix_open4_len, test_b,
                            &rej, ob, def_fix_to_rom_override);
    CHKCOND(strcmp(rom_mess_one->buffer, test_b->buffer) == 0);
    ret = from_rom_determine_spread(&mess, rom_mess2, rom_len2,
                                    test_b2, fix_type, &ft_len, &rej,
                                    ob, rom2fix_translate_fields);
    CHKCOND(strstr(test_b2->buffer, "555=2") != NULL);
    CHKCOND(strstr(test_b2->buffer, "600=UAL") != NULL);
    CHKCOND(strstr(test_b2->buffer, "167=MLEG") != NULL);
    CHKCOND(strstr(test_b2->buffer, "623=1") != NULL);
    CHKCOND(strstr(test_b2->buffer, "608=OP") != NULL);
    CHKCOND(strstr(test_b2->buffer, "608=OC") != NULL);
    databuf_reset(test_b);
    databuf_reset(ret_val);
    databuf_reset(rom_mess_one);
    long fix_open5_len = 0;
    char *id_val_five = calloc(11, 1);
    char *fout1 = "S,,,,2,UAL,150,-1.90,2,0,,,,,4,";
    char *fout2 =
        ",236FB1ED:66CA168A_2_UAL1FEB26.0C,FIXNGT4-40110127-55692,,,,,,,,,,,,,201102,C,26,,,,,,1,,,,0,,0,,,,14,0,20110127-14:32:45,1.04,,,185200626,,,,,,20110127156,,19,,,,6,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";


    create_fix_test_message(fix_out1_1, fix_out1_2,
                            obj, &fix_open5_len, 1, ret_val, id_val_five);
    databuf_write(rom_mess_one, "S,,,,,,,,,,,,,,4,20110127157,,FIXNGT4-40110127-55691,,,,,,,,,,,,,,,,,,,,,,,,,,,\n%s%s%s", fout1, id_val_five, fout2);
    ret = trans_fix_message(&mess, ret_val->buffer, fix_open5_len, test_b,
                            &rej, ob, def_fix_to_rom_override);
    CHKCOND(strcmp(rom_mess_one->buffer, test_b->buffer) == 0);
    //printf("\n%s\n%s\n", test_b->buffer, rom_mess_one->buffer);
    databuf_reset(test_b);
    databuf_reset(ret_val);
    databuf_reset(rom_mess_one);
    long fix_open6_len = 0;
    char *id_val_six = calloc(11, 1);
    char *fout21 = "S,,,,2,UAL,150,-1.90,2,0,,,,,4,";
    char *fout22 =
        ",236FB1ED:66CA168A_2_UAL1FEB26.0P,FIXNGT4-40110127-55693,,,,,,,,,,,,,201102,P,26,,,,,,1,,,,0,,0,,,,14,0,20110127-14:32:45,0.86,,,185200627,,,,,,20110127156,,19,,,,6,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";



    create_fix_test_message(fix_out2_1, fix_out2_2,
                            obj, &fix_open6_len, 2, ret_val, id_val_six);
    databuf_write(rom_mess_one, "%s%s%s", fout21, id_val_six, fout22);
    ret = trans_fix_message(&mess, ret_val->buffer, fix_open6_len, test_b,
                            &rej, ob, def_fix_to_rom_override);
    CHKCOND(strcmp(rom_mess_one->buffer, test_b->buffer) == 0);
    clean_dart_oo_pool(ob->mgr);
    STOPTEST
}
int test_r2f_spread_life_cycle4(const char **testname,
                               const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    databuf_t *test_b2 = databuf_alloc(500);
    REQCOND(test_b != 0);
    test_token_gen = init_order_token_creator();
    orderbook *ob = init_order_book(test_complex_gen_id, complex_con,
                                    def_rom2fix, def_rom2fix,
                                    def_xlate_fixrom, 50000,
                                    "NYSE", 4, "NYSE",
                                    4, NULL, 0);
    char *rom_mess =
        "E,143223,FIXNGT4,18700032010000,1,UAL,150,-1.9,2,0,S,501,32FY1209,187,,,,FIXNGT4-40110127-55691,,,,,NGT,GSEC,NGT,,,FIXNGT4,FIXNGT4-40110127-55691-0,,,,,,,OPT,NGT,21,,,,187,,,,NGT,,,,,,,,,,,,,,,,,,,,,3,,,,,1,,,,,,,,,!#!E,143223,,1870003201L0,2,UAL,1,,,,,,,,,,,FIXNGT4-40110127-55692,,,,,,,,,,,,,201102,C,26,,,OC,,,1,,,,,,,,,,,,,,,,,,,,,,,,19,,,,,,,,,,,,,,,,,,!#!E,143223,,1870003201L1,2,UAL,1,,,,,,,,,,,FIXNGT4-40110127-55693,,,,,,,,,,,,,201102,P,26,,,OP,,,1,,,,,,,,,,,,,,,,,,,,,,,,19,,,,,,,,,,,,,,,,,,!#!\n!#!";
    char *rom_mess2 =
        "C,143245,FIXNGT4,18700032010000,1,UAL,150,,,,,,,,,,,FIXNGT4-40110127-55691,,,,,,,,,,FIXNGT4,1,,,,,,,,,,,,,,,,,NGT 501 32FY1209 GSEC NGT,,,,,20110127-14:32:45,,,,,,,,,,18700032010001,,,,,,,,,,,2,,,,,,,,,\n";
    long rom_len2 = strlen(rom_mess2);
    long rom_len = strlen(rom_mess);
    rom_len -= 3; 
    char *fix_open1 =
        "35=849=ISE56=DART234=15652=20110127-14:32:2257=FOC37=236FB1ED:66CA168A_2_UAL1FEB26.0C11=2011012715617=185200620150=039=01=32FY55=UAL167=MLEG200=201102202=26442=254=238=15040=244=-1.9059=0204=432=031=0151=15014=06=060=20110127-14:32:2377=O58=32FY654=";
    char *fix_open1_p2 = "201=1205=1920=010=093";
    char *fix_open2 =
        "35=849=ISE56=DART234=15752=20110127-14:32:2257=FOC37=236FB1ED:66CA168A_2_UAL1FEB26.0P11=2011012715617=185200621150=039=01=32FY55=UAL167=MLEG200=201102202=26442=254=238=15040=244=-1.9059=0204=432=031=0151=15014=06=060=20110127-14:32:2377=O58=32FY654=";
    char *fix_open2_p2 = "201=0205=1920=010=108";
    char *fix_pfill1_1 =
        "35=849=ISE56=DART234=15852=20110127-14:32:2257=FOC37=236FB1ED:66CA168A_2_UAL1FEB26.0C11=2011012715617=185200622150=139=11=32FY55=UAL167=MLEG200=201102202=26442=254=238=15040=244=-1.9059=0204=432=1431=1.04151=13614=146=1.0460=20110127-14:32:2377=O58=32FY654=";
    char *fix_pfill1_2 =
        "9730=M9205=105440=NGT201=1205=1920=010=189";


    char *fix_pfill2_1 =
        "35=849=ISE56=DART234=15952=20110127-14:32:2257=FOC37=236FB1ED:66CA168A_2_UAL1FEB26.0P11=2011012715617=185200623150=139=11=32FY55=UAL167=MLEG200=201102202=26442=254=238=15040=244=-1.9059=0204=432=1431=0.86151=13614=146=0.8660=20110127-14:32:2377=O58=32FY654=";
    char *fix_pfill2_2 =
        "9730=M9205=105440=NGT201=0205=1920=010=222";
    char *fix_out1_1 =
        "35=849=ISE56=DART234=16252=20110127-14:32:4557=FOC37=236FB1ED:66CA168A_2_UAL1FEB26.0C11=2011012715741=2011012715617=185200626150=439=41=32FY55=UAL167=MLEG200=201102202=26442=254=238=15040=244=-1.9059=0204=432=031=0151=014=146=1.0460=20110127-14:32:4577=O58=32FY654=";
    char *fix_out1_2 = "201=1205=1920=010=160";

    char *fix_out2_1 =
        "35=849=ISE56=DART234=16352=20110127-14:32:4557=FOC37=236FB1ED:66CA168A_2_UAL1FEB26.0P11=2011012715741=2011012715617=185200627150=439=41=32FY55=UAL167=MLEG200=201102202=26442=254=238=15040=244=-1.9059=0204=432=031=0151=014=146=0.8660=20110127-14:32:4577=O58=32FY654=";
    char *fix_out2_2 = "201=0205=1920=010=184";
    long ret = 0;
    int rej = 0;
    char fix_type[10];
    int ft_len = 0;
    struct message mess;
    mess.name = "Jojo";
    mess.name_len = 4;
    memset(fix_type, 0, 10);
    set_option_rom2fix(ob);
    fs_set_spread_book(ob);
    set_rom2fix_leg_book(ob);
    set_option_fix_to_rom(ob);
    ret = from_rom_determine_spread(&mess, rom_mess, rom_len,
                                    test_b,
                                    fix_type, &ft_len, &rej,
                                    ob, rom2fix_translate_fields);
    CHKCOND(strstr(test_b->buffer, "555=2") != NULL);
    CHKCOND(strstr(test_b->buffer, "59=0") != NULL);
    CHKCOND(strstr(test_b->buffer, "167=MLEG") != NULL);
    CHKCOND(strstr(test_b->buffer, "440=NGT") != NULL);
    CHKCOND(strstr(test_b->buffer, "1=32FY") != NULL);
    CHKCOND(strstr(test_b->buffer, "1209") == NULL);
    CHKCOND(strstr(test_b->buffer, "600=UAL") != NULL);
    CHKCOND(strstr(test_b->buffer, "623=1") != NULL);
    CHKCOND(strstr(test_b->buffer, "608=OP") != NULL);
    CHKCOND(strstr(test_b->buffer, "608=OC") != NULL);
    databuf_reset(test_b);
    dart_ex_order_obj *obj = NULL;
    get_map_entry(ob->rom_ord_id_map, "FIXNGT4-40110127-55691", 22,
                  (void *) &obj);
    no_copy_insert(ob->cl_ord_id_map, "20110127156", 11, obj);
    long fix_open1_len = 0;
    databuf_t *ret_val = databuf_alloc(500);
    char *id_val_one = calloc(11, 1);
    create_fix_test_message(fix_open1, fix_open1_p2,
                            obj, &fix_open1_len, 1, ret_val, id_val_one);
    ret = trans_fix_message(&mess, ret_val->buffer, fix_open1_len, test_b,
                            &rej, ob, def_fix_to_rom_override);
    databuf_t *rom_mess_one = databuf_alloc(500);
    char *p1 = "S,,,,2,UAL,150,-1.90,2,0,,,,,13,";
    char *p2 =
        ",236FB1ED:66CA168A_2_UAL1FEB26.0C,FIXNGT4-40110127-55692,,,,,,,,,,,,,201102,C,26,,,,,,1,,,,0,,0,,,,0,150,20110127-14:32:23,0,,,185200620,,,,,,,,19,,,,6,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";
    databuf_write(rom_mess_one, "S,,,,,,,,,,,,,,13,20110127156,,FIXNGT4-40110127-55691,,,,,,,,,,,,,,,,,,,,,,,,,,,\n%s%s%s", p1, id_val_one, p2);
    CHKCOND(strcmp(rom_mess_one->buffer, test_b->buffer) == 0);
    //printf("\n%s\n%s\n", test_b->buffer, rom_mess_one->buffer);
    databuf_reset(test_b);
    databuf_reset(ret_val);
    long fix_open2_len = 0;
    char *id_val_two = calloc(11, 1);
    databuf_t *rom_mess_two = databuf_alloc(500);
    char *pp1 = "S,,,,2,UAL,150,-1.90,2,0,,,,,13,";
    char *pp2 =
        ",236FB1ED:66CA168A_2_UAL1FEB26.0P,FIXNGT4-40110127-55693,,,,,,,,,,,,,201102,P,26,,,,,,1,,,,0,,0,,,,0,150,20110127-14:32:23,0,,,185200621,,,,,,,,19,,,,6,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";
    create_fix_test_message(fix_open2, fix_open2_p2, obj, &fix_open2_len,
                            2, ret_val, id_val_two);
    databuf_write(rom_mess_two, "%s%s%s", pp1, id_val_two, pp2);

    ret = trans_fix_message(&mess, ret_val->buffer, fix_open2_len, test_b,
                            &rej, ob, def_fix_to_rom_override);
    CHKCOND(strcmp(rom_mess_two->buffer, test_b->buffer) == 0);
    databuf_reset(test_b);
    databuf_reset(ret_val);
    databuf_reset(rom_mess_one);
    long fix_open3_len = 0;
    char *id_val_three = calloc(11, 1);
    char *fp1 = "S,,,,2,UAL,150,-1.90,2,0,,,,,1,";
    char *fp2 =
        ",236FB1ED:66CA168A_2_UAL1FEB26.0C,FIXNGT4-40110127-55692,,,,,,,,,,,,,201102,C,26,,,,NGT,,1,,,,14,,1.04,,,,14,136,20110127-14:32:23,1.04,,,185200622,,,,,,,,19,,,,6,,,M,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";

    create_fix_test_message(fix_pfill1_1, fix_pfill1_2,
                            obj, &fix_open3_len, 1, ret_val, id_val_three);
    databuf_write(rom_mess_one, "S,,,,,,,,,,,,,,1,20110127156,,FIXNGT4-40110127-55691,,,,,,,,,,,,,,,,,,,,,,,,,,,\n%s%s%s", fp1, id_val_three, fp2);
    ret = trans_fix_message(&mess, ret_val->buffer, fix_open3_len, test_b,
                            &rej, ob, def_fix_to_rom_override);
    CHKCOND(strcmp(rom_mess_one->buffer, test_b->buffer) == 0);
    //printf("\n%s\n%s\n", test_b->buffer, rom_mess_one->buffer);
    databuf_reset(test_b);
    databuf_reset(ret_val);
    databuf_reset(rom_mess_one);
    long fix_open4_len = 0;
    char *id_val_four = calloc(11, 1);
    char *fp21 = "S,,,,2,UAL,150,-1.90,2,0,,,,,1,";
    char *fp22 =
        ",236FB1ED:66CA168A_2_UAL1FEB26.0P,FIXNGT4-40110127-55693,,,,,,,,,,,,,201102,P,26,,,,NGT,,1,,,,14,,0.86,,,,14,136,20110127-14:32:23,0.86,,,185200623,,,,,,,,19,,,,6,,,M,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";

    create_fix_test_message(fix_pfill2_1, fix_pfill2_2,
                            obj, &fix_open4_len, 2, ret_val, id_val_four);
    databuf_write(rom_mess_one, "%s%s%s", fp21, id_val_four, fp22);
    ret = trans_fix_message(&mess, ret_val->buffer, fix_open4_len, test_b,
                            &rej, ob, def_fix_to_rom_override);
    CHKCOND(strcmp(rom_mess_one->buffer, test_b->buffer) == 0);
    ret = from_rom_determine_spread(&mess, rom_mess2, rom_len2,
                                    test_b2, fix_type, &ft_len, &rej,
                                    ob, rom2fix_translate_fields);
    CHKCOND(strstr(test_b2->buffer, "555=2") != NULL);
    CHKCOND(strstr(test_b2->buffer, "600=UAL") != NULL);
    CHKCOND(strstr(test_b2->buffer, "167=MLEG") != NULL);
    CHKCOND(strstr(test_b2->buffer, "623=1") != NULL);
    CHKCOND(strstr(test_b2->buffer, "608=OP") != NULL);
    CHKCOND(strstr(test_b2->buffer, "608=OC") != NULL);
    databuf_reset(test_b);
    databuf_reset(ret_val);
    databuf_reset(rom_mess_one);
    long fix_open5_len = 0;
    char *id_val_five = calloc(11, 1);
    char *fout1 = "S,,,,2,UAL,150,-1.90,2,0,,,,,4,";
    char *fout2 =
        ",236FB1ED:66CA168A_2_UAL1FEB26.0C,FIXNGT4-40110127-55692,,,,,,,,,,,,,201102,C,26,,,,,,1,,,,0,,0,,,,14,0,20110127-14:32:45,1.04,,,185200626,,,,,,20110127156,,19,,,,6,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";


    create_fix_test_message(fix_out1_1, fix_out1_2,
                            obj, &fix_open5_len, 1, ret_val, id_val_five);
    databuf_write(rom_mess_one, "S,,,,,,,,,,,,,,4,20110127157,,FIXNGT4-40110127-55691,,,,,,,,,,,,,,,,,,,,,,,,,,,\n%s%s%s", fout1, id_val_five, fout2);
    ret = trans_fix_message(&mess, ret_val->buffer, fix_open5_len, test_b,
                            &rej, ob, def_fix_to_rom_override);
    CHKCOND(strcmp(rom_mess_one->buffer, test_b->buffer) == 0);
    //printf("\n%s\n%s\n", test_b->buffer, rom_mess_one->buffer);
    databuf_reset(test_b);
    databuf_reset(ret_val);
    databuf_reset(rom_mess_one);
    long fix_open6_len = 0;
    char *id_val_six = calloc(11, 1);
    char *fout21 = "S,,,,2,UAL,150,-1.90,2,0,,,,,4,";
    char *fout22 =
        ",236FB1ED:66CA168A_2_UAL1FEB26.0P,FIXNGT4-40110127-55693,,,,,,,,,,,,,201102,P,26,,,,,,1,,,,0,,0,,,,14,0,20110127-14:32:45,0.86,,,185200627,,,,,,20110127156,,19,,,,6,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";



    create_fix_test_message(fix_out2_1, fix_out2_2,
                            obj, &fix_open6_len, 2, ret_val, id_val_six);
    databuf_write(rom_mess_one, "%s%s%s", fout21, id_val_six, fout22);
    ret = trans_fix_message(&mess, ret_val->buffer, fix_open6_len, test_b,
                            &rej, ob, def_fix_to_rom_override);
    CHKCOND(strcmp(rom_mess_one->buffer, test_b->buffer) == 0);
    clean_dart_oo_pool(ob->mgr);
    STOPTEST
}
