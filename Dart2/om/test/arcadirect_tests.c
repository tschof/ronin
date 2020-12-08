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
#include "ad_parser.h"

int test_ad_price_converter(const char **testname,
                           const char **extrareporting)
{
    STARTTEST;
    //Done so that we will get closure.
    REQCOND(1 != 0);
    char* price = "123.2345,897,B";
    int a = rom_to_ad_price(price, 8);
    CHKCOND(a == 1232345);
    price = "99999.9,23445";
    a = rom_to_ad_price(price, 8);
    CHKCOND(a == 999999000);
    price = "89023,45,B";
    a = rom_to_ad_price(price, 8);
    CHKCOND(a == 890230000);
    STOPTEST;
}
