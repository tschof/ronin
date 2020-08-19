#include "soupbin.h"
#include "../baz/libbaz.h"
#include "server_objects/config_loader.h"
#include "ex_common/order_token_creator.h"
#include "conf_util.h"
#include "ouch41.h"

#include <arpa/inet.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

struct databuf *buffer = 0;

void test_soupbin_heartbeat(void)
{
    struct soupbin_header heartbeat;
    CHKCOND(soupbin_heartbeat_init(&heartbeat) != -1);
    CHKCOND(heartbeat.packet_type == 'R');
    CHKCOND(heartbeat.packet_length == htons(1u));
    CHKCOND(soupbin_heartbeat_serialize(&heartbeat, buffer) == 0);
    CHKCOND(databuf_unread(buffer) == 3);
    CHKCOND(soupbin_frame_message(buffer) == 3);
}

void test_soupbin_login(void)
{
    struct soupbin_login login;
    CHKCOND(soupbin_login_init(&login, "foo", "bar") == 0);
    CHKCOND(memcmp(login.username, "foo   ", sizeof login.username) == 0);
    CHKCOND(memcmp(login.password, "bar       ", sizeof login.password) ==
            0);
    CHKCOND(memcmp
            (login.requested_session, "          ",
             sizeof login.requested_session) == 0);
    CHKCOND(login.header.packet_type == 'L');
}

void test_make_current_date_string(void)
{
    char buffer[12];
    CHKCOND(make_current_date_string(buffer, 1, 0) == -1);
    CHKCOND(make_current_date_string(buffer, 8, 0) == 0);
    struct tm localtm = { 0 };
    CHKCOND(make_current_date_string(buffer, 8, &localtm) == 0);
    buffer[8] = '\0';
    struct tm scantime;
    CHKCOND(sscanf
            (buffer, "%4d%2d%2d", &scantime.tm_year, &scantime.tm_mon,
             &scantime.tm_sec) == 3);
    CHKCOND(make_current_date_string(buffer, 7, &localtm) == -1);
}

void test_should_reset(void)
{
    struct tm input = {
        .tm_sec = 0,
        .tm_min = 15,
        .tm_hour = 9,
        .tm_mday = -1,
        .tm_mon = -1,
        .tm_year = -1,
        .tm_wday = -1,
        .tm_yday = -1,
        .tm_isdst = -1,
    };
    CHKCOND(should_reset(&input, 9, 15, 0) == 1);
    CHKCOND(should_reset(&input, 9, 15, 1) == 1);
    CHKCOND(should_reset(&input, 9, 14, 0) == 0);
    CHKCOND(should_reset(&input, 9, 16, 0) == 0);
    CHKCOND(should_reset(&input, 9, 15, 1) == 1);
}

void test_conf_map_to_vars(void)
{
    FILE *test_conf_file;
    CHKCOND((test_conf_file = fopen("generated.conf", "w")) != 0);
    const char *conf_file_contents =
        "[Foo]\nkey1\nKey2=val2\n=val3\nkey4=val4\n[bar]\nkey1=val1\n";
    size_t conf_file_length = strlen(conf_file_contents), nwritten;
    CHKCOND((nwritten =
             fwrite(conf_file_contents, 1, conf_file_length,
                    test_conf_file)) == conf_file_length);
    if (nwritten == -1) {
        perror("fwrite");
        return;
    }
    int retval;
    CHKCOND((retval = fflush(test_conf_file)) == 0);
    CHKCOND((retval = fclose(test_conf_file)) == 0);
    dart_config *conf;
    CHKCOND((conf = initialize_config("generated.conf", 14)) != 0);
    char *var;
    size_t varlen;
    struct conf_map cmap = {
        .section = "Foo",
        .key = "Key2",
        .value = &var,
        .value_length = &varlen,
    };
    conf_map_to_vars(conf, &cmap, 1);
    CHKCOND(cmap.value != 0);
    CHKCOND(cmap.value_length != 0);
    CHKCOND((retval = unlink("generated.conf")) == 0);
}

void test_price_conversions(void)
{
    static const char *test_cases[] = {
        "0.01", "0.10", "1.00", "1.01", "0.0001", "52.43",
    };
    struct databuf *buffer = databuf_alloc(64);
    size_t i, ncases = sizeof test_cases / sizeof test_cases[0];
    for (i = 0; i < ncases; ++i) {
        unsigned int priceint;
        CHKCOND(ouch_price_string_to_int(test_cases[i], &priceint) == 0);
        printf("%s => %u", test_cases[i], priceint);
        databuf_reset(buffer);
        CHKCOND(ouch_price_int_to_string(priceint, buffer) != -1);
        size_t nlen = strlen(test_cases[i]);
        CHKCOND(databuf_unread(buffer) == nlen);
        printf(" => %.*s\n", (int) databuf_unread(buffer), buffer->rd_ptr);
        if (databuf_unread(buffer) == nlen) {
            CHKCOND(strncmp(buffer->rd_ptr, test_cases[i], nlen) == 0);
        }
    }
}

void test_price_conversions2(void)
{
    struct databuf *pricestr = databuf_alloc(512);
    CHKCOND(pricestr != 0);
    uint32_t priceint;
    for (priceint = 1; priceint < 10000000; ++priceint) {
        CHKCOND(ouch_price_int_to_string(priceint, pricestr) != -1);
        uint32_t newprice;
        CHKCOND(ouch_price_string_to_int(pricestr->rd_ptr, &newprice) ==
                0);
        CHKCOND(newprice == priceint);
        databuf_reset(pricestr);
    }
    databuf_destroy(pricestr);
}

void baz_run_tests(void)
{
    buffer = databuf_alloc(512);
    test_soupbin_heartbeat();
    test_soupbin_login();
    test_make_current_date_string();
    test_should_reset();
    test_conf_map_to_vars();
    /* test_price_conversions(); */
    test_price_conversions2();
    /* test_ouch_enter_order(); */
    databuf_destroy(buffer);
}
