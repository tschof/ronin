#include "../baz/libbaz.h"
#include "../server_objects/databuf.h"
#include "DartToBoe.h"
#include "DataTypes.h"
#include "EnterOrder.h"
#include "Iovec.h"
#include "Macros.h"
#include "OptionalFields.h"

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static void create_databuffs(queue_t glob, queue_t q, int num)
{
    int i = 0;
    for (; i < num; ++i) {
        databuf_t *buff = databuf_alloc(1);
        enqueue(q, buff);
    }
}

void runBoeOrderTest(void)
{
    struct EnterOrder enterOrder;
    dof_t *objfact =
        create_dart_obj_fact(1, databuf_clear, create_databuffs,
                             databuf_clear);
    initEnterOrder(&enterOrder, objfact);
    /* Test setting the price */
    CHKCOND(setOptionalField(0, EnterOrderPreventMemberMatch, 0, 0) == -1);
    char *preventMemberMatch = "ABC";
    CHKCOND(setOptionalField
            (&enterOrder.optionalFields, EnterOrderPreventMemberMatch,
             preventMemberMatch, strlen(preventMemberMatch)) == 0);
    int fd = creat("messages", S_IRUSR | S_IWUSR);
    if (fd != -1) {
        iovec_writeall(fd, enterOrder.iov,
                       sizeof enterOrder.iov / sizeof enterOrder.iov[0]);
    } else {
        perror("cannot create message file");
    }
    if (close(fd) == -1) {
        perror("closing message file");
    }
}

void runBoeOptionalFieldsTest(void)
{
    uint8_t bits[16];
    struct iovec fields[16];
    dof_t *objfact =
        create_dart_obj_fact(1, databuf_clear, create_databuffs,
                             databuf_clear);
    struct OptionalFields optfields;
    initOptionalFields(&optfields, bits, narray(bits), fields,
                       narray(fields), objfact);
    const char *str = "foobar";
    CHKCOND(setOptionalAlphaNumeric
            (&optfields, EnterOrderSymbol, str, 6, 8) == 0);
    /* CHKCOND(setOptionalField(&optfields, 0x00000002, (void *) str, strlen(str)) == 0); */
    /* CHKCOND(setOptionalAlphaNumeric */
    /*         (&optfields, EnterOrderSymbolSfx, "PRB", 3, 8) == 0); */
    CHKCOND(optfields.bitsp[1] & 0x01);
    CHKCOND(optfields.bitsp[1] & 0x02);
    CHKCOND((optfields.bitsp[1] & 0x04) == 0);
    {
        char tgtval[] = "foobar\0\0";
        CHKCOND(optfields.iovp[EnterOrderSymbol].iov_len ==
                (sizeof tgtval - 1));
        CHKCOND(memcmp
                (optfields.iovp[EnterOrderSymbol].iov_base, tgtval,
                 sizeof tgtval - 1) == 0);
    }
    {
        /* char tgtval[] = "PRB\0\0\0\0\0"; */
        /* CHKCOND(optfields.iovp[EnterOrderSymbolSfx].iov_len == */
        /*         (sizeof tgtval - 1)); */
        /* CHKCOND(memcmp */
        /*         (optfields.iovp[EnterOrderSymbolSfx].iov_base, tgtval, */
        /*          sizeof tgtval - 1) == 0); */
    }
    CHKCOND(optfields.iovp[EnterOrderExecInst].iov_len == 0);
    CHKCOND(optfields.iovp[EnterOrderExecInst].iov_base == 0);
}

void testOptionalFields2(void)
{
    uint8_t bits[16];
    struct iovec fields[16];
    dof_t *objfact =
        create_dart_obj_fact(1, databuf_clear, create_databuffs,
                             databuf_clear);
    struct OptionalFields optfields;
    initOptionalFields(&optfields, bits, narray(bits), fields,
                       narray(fields), objfact);
    struct databuf *dbg = databuf_alloc(256);
    dumpOptionalFieldBits(&optfields, dbg);
    printf("Before\n%s\n", dbg->rd_ptr);
    databuf_reset(dbg);
    const char *tif = "0";
    CHKCOND(setOptionalAlphaNumeric(&optfields, EnterOrderTimeInForce,
                                    tif, 1, 1) == 0);
    dumpOptionalFieldBits(&optfields, dbg);
    printf("After\n%s\n", dbg->rd_ptr);
    databuf_reset(dbg);
    const char *symbol = "ZVZZT";
    CHKCOND(setOptionalAlphaNumeric
            (&optfields, EnterOrderSymbol, symbol, 5, 8) == 0);
    dumpOptionalFieldBits(&optfields, dbg);
    printf("Setting Symbol\n%s\n", dbg->rd_ptr);
    databuf_destroy(dbg);
}

void testBoePriceConversions(void)
{
    const char *p = "1.23";
    int64_t n = 0;
    CHKCOND(setSignedBinaryPrice(&n, p, strlen(p)) == 0);
    CHKCOND(n == 12300);
    p = "-1.23";
    CHKCOND(setSignedBinaryPrice(&n, p, strlen(p)) == 0);
    CHKCOND(n == -12300);
    p = "1.";
    CHKCOND(setSignedBinaryPrice(&n, p, strlen(p)) == 0);
    CHKCOND(n == 10000);
    p = "0.12345";
    CHKCOND(setSignedBinaryPrice(&n, p, strlen(p)) == -1);
    p = "12345.9999";
    CHKCOND(setSignedBinaryPrice(&n, p, strlen(p)) == 0);
    CHKCOND(n == 123459999);
}

void roundtripPriceTests(void)
{
    struct databuf *input = databuf_alloc(64), *output = databuf_alloc(64);
    int whole, dec;
    for (whole = 0; whole < 9999; ++whole) {
        for (dec = 0; dec < 100; ++dec) {
            databuf_write(input, "%d.%02d", whole, dec);
            price_t price;
            CHKCOND(setSignedBinaryPrice(&price, input->rd_ptr,
                                         databuf_unread(input)) != -1);
            CHKCOND(binaryPriceToString(price, output) != -1);
            CHKCOND(strncmp
                    (output->rd_ptr, input->rd_ptr,
                     databuf_unread(output) - 2) == 0);
            CHKCOND(strcmp
                    (output->rd_ptr + databuf_unread(output) - 2,
                     "00") == 0);
            databuf_reset(output);
            databuf_reset(input);
        }
    }

    for (whole = 0; whole < 9999; ++whole) {
        for (dec = 0; dec < 10000; ++dec) {
            databuf_write(input, "%d.%04d", whole, dec);
            price_t price;
            CHKCOND(setSignedBinaryPrice(&price, input->rd_ptr,
                                         databuf_unread(input)) != -1);
            CHKCOND(binaryPriceToString(price, output) != -1);
            CHKCOND(strcmp(output->rd_ptr, input->rd_ptr) == 0);
            databuf_reset(output);
            databuf_reset(input);
        }
    }

    databuf_destroy(output);
    databuf_destroy(input);
}

void test_romcollection(void);

void baz_run_tests(void)
{
    /* printBytesAndBits(); */
    /* runBoeOrderTest(); */
    /* runBoeOptionalFieldsTest(); */
    /* testBoePriceConversions(); */
    /* testOptionalFields2(); */
    /* roundtripPriceTests(); */
    test_romcollection();
}
