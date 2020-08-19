#include "tests.h"
#include "testmacros.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "fix_rom_trans.h"
#include "dart_orderbook.h"
#include "config_loader.h"

int test_config_loader(const char **testname, const char **extrareporting)
{
    STARTTEST dart_config *ret_val = 0;
    char *sender_comp;
    char *send_tag = "Sender Comp ID";
    int send_len = 14;
    char *target_comp;
    char *target_tag = "Target Comp ID";
    int targ_len = 14;
    int ret_len;
    char *fix_port;
    char *rom_user_name;
    char *rom_con_name;
    char *rom_port;
    char *rom_host;
    char *end_hour;
    char *end_min;
    char *fix_port1;
    char *rom_user_name1;
    char *rom_con_name1;
    char *rom_port1;
    char *rom_host1;
    char *end_hour1;
    char *end_min1;
    char *date = 0;
    ret_val = initialize_config("TestFixConfig.dat", 17);
    REQCOND(ret_val != 0);
    sender_comp =
        get_val_for_tag(ret_val, "FIX SERVER", 10, send_tag, send_len,
                        &ret_len);
    target_comp =
        get_val_for_tag(ret_val, "FIX SERVER", 10, target_tag, targ_len,
                        &ret_len);

    fix_port =
        get_val_for_tag(ret_val, "FIX SERVER", 10, "Fix Port", 8,
                        &ret_len);
    rom_user_name =
        get_val_for_tag(ret_val, "ROM SERVER", 10, "Rom User Name", 13,
                        &ret_len);
    rom_con_name =
        get_val_for_tag(ret_val, "ROM SERVER", 10, "Rom Connection Name",
                        19, &ret_len);
    rom_port =
        get_val_for_tag(ret_val, "ROM SERVER", 10, "Rom Port", 8,
                        &ret_len);
    rom_host =
        get_val_for_tag(ret_val, "ROM SERVER", 10, "Rom Host", 8,
                        &ret_len);
    end_hour =
        get_val_for_tag(ret_val, "TIMES", 5, "End Hour", 8, &ret_len);
    end_min = get_val_for_tag(ret_val, "TIMES", 5, "End Min", 7, &ret_len);

    date =
        get_val_for_tag(ret_val, "Server", 5, "Last Run Day", 12,
                        &ret_len);
    if (date) {
        CHKCOND(strcmp(date, "20104699") == 0);

    }
    set_val_for_tag(ret_val, "Server", 6, "Last Run Day", 12,
                    "20100322", 8);

    dart_load_map(ret_val);
    date =
        get_val_for_tag(ret_val, "Server", 6, "Last Run Day", 12,
                        &ret_len);
    CHKCOND(strcmp(date, "20100322") == 0);
    dart_reset_maps(ret_val);



    sender_comp =
        get_val_for_tag(ret_val, "FIX SERVER", 10, send_tag, send_len,
                        &ret_len);
    target_comp =
        get_val_for_tag(ret_val, "FIX SERVER", 10, target_tag, targ_len,
                        &ret_len);


    fix_port1 =
        get_val_for_tag(ret_val, "FIX SERVER", 10, "Fix Port", 8,
                        &ret_len);
    rom_user_name1 =
        get_val_for_tag(ret_val, "ROM SERVER", 10, "Rom User Name", 13,
                        &ret_len);
    rom_con_name1 =
        get_val_for_tag(ret_val, "ROM SERVER", 10, "Rom Connection Name",
                        19, &ret_len);
    rom_port1 =
        get_val_for_tag(ret_val, "ROM SERVER", 10, "Rom Port", 8,
                        &ret_len);
    rom_host1 =
        get_val_for_tag(ret_val, "ROM SERVER", 10, "Rom Host", 8,
                        &ret_len);
    end_hour1 =
        get_val_for_tag(ret_val, "TIMES", 5, "End Hour", 8, &ret_len);
    end_min1 =
        get_val_for_tag(ret_val, "TIMES", 5, "End Min", 7, &ret_len);

    set_val_for_tag(ret_val, "Server", 6, "Last Run Day", 12,
                    "20104699", 8);
    set_val_for_tag(ret_val, "ARCA", 4, "Dodo Gone Day", 13,
                    "Dis is da dodo", 14);

    dart_reset_maps(ret_val);
    date =
        get_val_for_tag(ret_val, "Server", 6, "Last Run Day", 12,
                        &ret_len);
    CHKCOND(strcmp(date, "20104699") == 0);

    set_val_for_tag(ret_val, "ROM SERVER", 10, "KILLDAY", 7, "YEST", 4);
    set_val_for_tag(ret_val, "MAIL", 4, "BOZO", 4, "PUGY", 4);
    set_val_for_tag(ret_val, "Special Option Dests", 20, "222", 3, "789", 3);
    set_val_for_tag(ret_val, "Special Option Dests", 20, "11", 2, "567", 3);
    set_val_for_tag(ret_val, "Special Option Dests", 20, "3", 1, "8", 1);

    dart_reset_maps(ret_val);
    char* sod222 = get_val_for_tag(ret_val, "Special Option Dests", 20, "222", 3, &ret_len);
    CHKCOND(strcmp("789", sod222) == 0);
    char* sod11 = get_val_for_tag(ret_val, "Special Option Dests", 20, "11", 2, &ret_len);
    CHKCOND(strcmp("567", sod11) == 0);
    char* sod3 = get_val_for_tag(ret_val, "Special Option Dests", 20, "3", 1, &ret_len);
    CHKCOND(strcmp("8", sod3) == 0);
    STOPTEST
}
