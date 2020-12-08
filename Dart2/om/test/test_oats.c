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
#include "dart_order_obj.h"
#include "transaction_recorder.h"
#include "oats.h"

int test_find_rom_offset(const char **testname,
                         const char **extrareporting)
{
    STARTTEST;
    databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    char *rom_message =
        "E,20110523-15:44:05,PARNELL,PARNELL1104231044050001,1,ADSK\n";
    int val_len = 0;
    char *ret_val =
        get_rom_val(rom_message, strlen(rom_message), &val_len, 2);
    CHKCOND(strncmp(ret_val, "PARNELL", 7) == 0);
    CHKCOND(val_len == 7);
    ret_val = get_rom_val(rom_message, strlen(rom_message), &val_len, 5);
    CHKCOND(strncmp(ret_val, "ADSK", 4) == 0);
    CHKCOND(val_len == 4);
    STOPTEST;
}
