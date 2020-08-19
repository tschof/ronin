#include "tests.h"
#include "testmacros.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "ex_common.h"
#include "dart_constants.h"

int test_comma_remove(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    char *fix_mess =
        "35=834=2249=SIMULATORi,DODO52=20101013-13:06:17.92956=DART6=0.9511=BB 0001/1013201014=017=120=031=032=037=38=1000039=044=0.9554=155=WTF150=0151=1000010=012";
    size_t len = strlen(fix_mess);
    struct dart_rom_pos p = { 14, 30 };
    struct dart_rom_pos pos[1] = { p };
    size_t cp = clean_text(test_b, fix_mess, len, 0, pos);
    CHKCOND(strstr(test_b->buffer, ",") == NULL);
    CHKCOND(strlen(test_b->buffer) == 16);
    char *fix_mess2 =
        "35=834=2249=SIMULATORi,DODO,D23,go here52=20101013-13:06:17.92956=DART6=0.9511=BB 0001/1013201014=017=120=031=032=037=38=1000039=044=0.9554=155=WTF150=0151=1000010=012";
    databuf_reset(test_b);
    len = strlen(fix_mess2);
    struct dart_rom_pos p2 = { 14, 42 };
    struct dart_rom_pos pos2[1] = { p2 };
    cp = clean_text(test_b, fix_mess2, len, 0, pos2);
    CHKCOND(strstr(test_b->buffer, ",") == NULL);
    CHKCOND(strlen(test_b->buffer) == 28);


    char *fix_mess3 =
        "35=834=2249=SIMULATORi,DODO,D23,go here,52=20101013-13:06:17.92956=DART6=0.9511=BB 0001/1013201014=017=120=031=032=037=38=1000039=044=0.9554=155=WTF150=0151=1000010=012";
    databuf_reset(test_b);
    len = strlen(fix_mess3);
    struct dart_rom_pos p3 = { 14, 43 };
    struct dart_rom_pos pos3[1] = { p3 };
    cp = clean_text(test_b, fix_mess3, len, 0, pos3);
    CHKCOND(strstr(test_b->buffer, ",") == NULL);
    CHKCOND(strlen(test_b->buffer) == 29);
    char *fix_mess4 =
        "35=834=2249=SIMULATORi,DODO,D23,go her,e52=20101013-13:06:17.92956=DART6=0.9511=BB 0001/1013201014=017=120=031=032=037=38=1000039=044=0.9554=155=WTF150=0151=1000010=012";
    databuf_reset(test_b);
    len = strlen(fix_mess4);
    struct dart_rom_pos p4 = { 14, 43 };
    struct dart_rom_pos pos4[1] = { p4 };
    cp = clean_text(test_b, fix_mess4, len, 0, pos4);
    CHKCOND(strstr(test_b->buffer, ",") == NULL);
    CHKCOND(strlen(test_b->buffer) == 29);
    char *fix_mess5 =
        "35=834=2249=SIMULATORi#DODO#D23#go her#e52=20101013-13:06:17.92956=DART6=0.9511=BB 0001/1013201014=017=120=031=032=037=38=1000039=044=0.9554=155=WTF150=0151=1000010=012";
    databuf_reset(test_b);
    len = strlen(fix_mess5);
    struct dart_rom_pos p5 = { 14, 43 };
    struct dart_rom_pos pos5[1] = { p5 };
    cp = clean_text(test_b, fix_mess5, len, 0, pos5);
    CHKCOND(strstr(test_b->buffer, ",") == NULL);
    CHKCOND(strlen(test_b->buffer) == 29);
    databuf_destroy(test_b);
    STOPTEST
}

int test_trans_rom_symbol(const char **testname, const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    databuf_t* d_off = databuf_alloc(500);
    REQCOND(test_b != 0);
    translate_rom_symbol(test_b, "VIA/B",
                            5);
    databuf_write(d_off, "%c55=VIA%c65=B", FIX_FIELD_DELIM, FIX_FIELD_DELIM);    
    CHKCOND(strncmp(test_b->buffer, d_off->buffer, databuf_unread(test_b)) == 0);
    databuf_reset(test_b);
    databuf_reset(d_off);
    translate_rom_symbol(test_b, "ADSK/WSA",8);
    databuf_write(d_off, "%c55=ADSK%c65=WSA", FIX_FIELD_DELIM, FIX_FIELD_DELIM);    
    CHKCOND(strncmp(test_b->buffer, d_off->buffer, databuf_unread(test_b)) == 0);
    databuf_destroy(test_b);
    databuf_destroy(d_off);
    STOPTEST
}
