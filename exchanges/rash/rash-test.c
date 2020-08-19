#include "../baz/libbaz.h"
#include "rash_util.h"
#include <stdio.h>
#include <string.h>
#include "server_objects/databuf.h"

void convert_timestamps(void)
{
    const char *millis = "86399123";
    char buffer[TIMESTAMP_LEN_WITH_MILLIS + 1];
    memset(buffer, 0, sizeof buffer);
    CHKCOND(rash_milliseconds_from_midnight_to_timestamp
            (millis, strlen(millis), buffer, sizeof buffer) != -1);
    /* printf("input=%s, output=%s\n", millis, buffer); */
}

void convert_timestamps2(void)
{
    struct rash_timestamp_converter *tsconverter =
        create_rash_timestamp_converter();
    CHKCOND(tsconverter != 0);
    const char *millis = "86399123";
    char buffer[TIMESTAMP_LEN_WITH_MILLIS + 1];
    memset(buffer, 0, sizeof buffer);
    CHKCOND(rash_convert_timestamp_to_dartcsv
            (tsconverter, millis, strlen(millis), buffer,
             sizeof buffer) != -1);
    /* printf("%s => %.*s\n", millis, (int) TIMESTAMP_LEN_WITH_MILLIS, */
    /*        buffer); */
    CHKCOND(strncmp
            (buffer, "20120724-23:59:59.123",
             TIMESTAMP_LEN_WITH_MILLIS) == 0);
}

void priceConversionTestFromRash(void)
{
    const char *rashPrice = "0000030000";
    struct databuf *priceBuffer = databuf_alloc(256);
    CHKCOND(rash_get_price(rashPrice, priceBuffer) != -1);
    CHKCOND(databuf_unread(priceBuffer) == 4);
    if (databuf_unread(priceBuffer) == 4)
        CHKCOND(strncmp(priceBuffer->rd_ptr, "3.00", 4) == 0);
    rashPrice = "0000030001";
    databuf_reset(priceBuffer);
    CHKCOND(rash_get_price(rashPrice, priceBuffer) != -1);
    CHKCOND(databuf_unread(priceBuffer) == 6);
    if (databuf_unread(priceBuffer) == 6)
        CHKCOND(strncmp(priceBuffer->rd_ptr, "3.0001", 6) == 0);
    rashPrice = "9999999999";
    databuf_reset(priceBuffer);
    CHKCOND(rash_get_price(rashPrice, priceBuffer) != -1);
    CHKCOND(databuf_unread(priceBuffer) == 11);
    if (databuf_unread(priceBuffer) == 11)
        CHKCOND(strncmp(priceBuffer->rd_ptr, "999999.9999", 11) == 0);
    rashPrice = "0000991230";
    databuf_reset(priceBuffer);
    CHKCOND(rash_get_price(rashPrice, priceBuffer) != -1);
    CHKCOND(databuf_unread(priceBuffer) == 6);
    if (databuf_unread(priceBuffer) == 6)
        CHKCOND(strncmp(priceBuffer->rd_ptr, "99.123", 6) == 0);
    rashPrice = "0000121000";
    databuf_reset(priceBuffer);
    CHKCOND(rash_get_price(rashPrice, priceBuffer) != -1);
    CHKCOND(databuf_unread(priceBuffer) == 5);
    if (databuf_unread(priceBuffer) == 5)
        CHKCOND(strncmp(priceBuffer->rd_ptr, "12.10", 5) == 0);
    databuf_destroy(priceBuffer);
}

void priceConversionTestToRash(void)
{
    const char *dartPrice = "1.23";
    char rashPrice[11];
    memset(rashPrice, '0', sizeof rashPrice);
    CHKCOND(rash_put_price(dartPrice, strlen(dartPrice), rashPrice) != -1);
    /* printf("'%s'\n", rashPrice); */
    CHKCOND(strncmp(rashPrice, "0000012300", 10) == 0);
}

void baz_run_tests(void)
{
    /* convert_timestamps(); */
    /* convert_timestamps2(); */
    priceConversionTestFromRash();
    priceConversionTestToRash();
}
