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
#include "jigglypuff.h"
#include "master_config.h"

static jp_limits_checker *jlc = 0;
static master_config *main_cfg = 0;
static int is_init = 0;

void init_jiggly()
{
    if (!is_init) {
        is_init = 1;
        main_cfg = init_mconfig("glob.dat", 8);
        jlc = create_jp_limits_checker(main_cfg, 0,0);
    }
}

dart_order_obj *create_doj()
{
    char *glob = calloc(1, DEFAULT_BUFF_SIZE);
    dart_order_obj *doj = calloc(1, sizeof(struct dart_order_obj));
    doj->positions = calloc(ROM_LEN, sizeof(struct iovec));
    doj->positions[161].iov_base = "\n";
    doj->positions[161].iov_len = 1;
    doj->head = malloc(sizeof(db_node_t));
    doj->head->data = malloc(sizeof(databuf_t));
    doj->current = doj->head;
    doj->head->next = NULL;
    databuf_attach(doj->head->data, glob, DEFAULT_BUFF_SIZE);
    doj->head->start_ptr = glob;
    doj->head->end_ptr = glob + DEFAULT_BUFF_SIZE;
    doj->rep = NULL;
    doj->dest = NULL;
    doj->sender = NULL;
    return doj;
}

void free_doj(dart_order_obj * doj)
{
    databuf_destroy(doj->head->data);
    free(doj->head);
    free(doj->positions);
    free(doj);
}

int test_jiggly_update_new(const char **testname,
                           const char **extrareporting)
{
    STARTTEST;
    init_jiggly();
    REQCOND(jlc != 0);
    REQCOND(main_cfg != 0);
    dart_order_obj *doj = create_doj();
    jp_limits *jpl = get_equity_limits_for_trader(jlc, "PARNELL", 7);
    long init_bp = jpl->cur_balance_buy;
    reset_rom_field(doj, ROM_STATUS, "0", 1);
    reset_rom_field(doj, ROM_TRADER, "PARNELL", 7);
    reset_rom_field(doj, ROM_SIDE, "1", 1);
    reset_rom_field(doj, ROM_SEC_TYPE, "E", 1);
    reset_rom_field(doj, ROM_SHARES, "100", 3);
    reset_rom_field(doj, ROM_PRICE, "871.98", 3);
    update_balance_new(jlc, doj, 1);
    int null_int = 0;
    set_rom_field(doj, ROM_BIN_CUM, &null_int, sizeof(int));
    long bp = jpl->cur_balance_buy;
    CHKCOND(bp == (init_bp + 871980000));
    reset_rom_field(doj, ROM_STATUS, "0", 1);
    update_balance(jlc, doj);
    bp = jpl->cur_balance_buy;
    CHKCOND(init_bp == bp);

    update_balance_new(jlc, doj, 1);
    bp = jpl->cur_balance_buy;
    CHKCOND(init_bp == (bp - 871980000));
    dart_order_obj *d2 = create_doj();
    init_bp = jpl->cur_balance_sell;
    reset_rom_field(d2, ROM_STATUS, "0", 1);
    reset_rom_field(d2, ROM_TRADER, "PARNELL", 7);
    reset_rom_field(d2, ROM_SIDE, "5", 1);
    reset_rom_field(d2, ROM_SEC_TYPE, "E", 1);
    reset_rom_field(d2, ROM_SHARES, "100", 3);
    reset_rom_field(d2, ROM_PRICE, "871.98", 3);
    update_balance_new(jlc, d2, 1);
    bp = jpl->cur_balance_sell;
    CHKCOND(init_bp == (bp - 871980000));
    free_doj(doj);
    free_doj(d2);
    STOPTEST;
}

int test_jiggly_check_rom(const char **testname,
                          const char **extrareporting)
{
    STARTTEST;
    init_jiggly();
    REQCOND(jlc != 0);
    REQCOND(main_cfg != 0);
    dart_order_obj *doj = create_doj();
    reset_rom_field(doj, ROM_SENDER, "PARNELL", 7);
    reset_rom_field(doj, ROM_TAG, "PARNELL-330120721-69993765", 26);
    check_rom(doj, "24", 2);
    CHKCOND(strncmp("ROM33", doj->positions[ROM_SENDER].iov_base, 5) == 0);
    CHKCOND(5 == doj->positions[ROM_SENDER].iov_len);
    reset_rom_field(doj, ROM_SENDER, "DODO", 4);
    reset_rom_field(doj, ROM_TAG, "PARNELL-2400120721-69993765", 26);
    check_rom(doj, "24", 2);
    CHKCOND(strncmp("ROM240", doj->positions[ROM_SENDER].iov_base, 5) ==
            0);
    CHKCOND(6 == doj->positions[ROM_SENDER].iov_len);
    reset_rom_field(doj, ROM_SENDER, "DODO", 4);
    reset_rom_field(doj, ROM_TAG, "PARNELL-240120721-69993765", 26);
    check_rom(doj, "24", 2);
    CHKCOND(strncmp("DODO", doj->positions[ROM_SENDER].iov_base, 4) == 0);
    CHKCOND(4 == doj->positions[ROM_SENDER].iov_len);
    free_doj(doj);
    STOPTEST;
}

int test_jiggly_update_fill(const char **testname,
                            const char **extrareporting)
{
    STARTTEST;
    init_jiggly();
    REQCOND(jlc != 0);
    REQCOND(main_cfg != 0);
    dart_order_obj *doj = create_doj();
    jp_limits *jpl = get_equity_limits_for_trader(jlc, "PARNELL", 7);
    long init_bp = jpl->cur_balance_buy;
    reset_rom_field(doj, ROM_STATUS, "0", 1);
    reset_rom_field(doj, ROM_TRADER, "PARNELL", 7);
    reset_rom_field(doj, ROM_SIDE, "1", 1);
    reset_rom_field(doj, ROM_SEC_TYPE, "E", 1);
    reset_rom_field(doj, ROM_SHARES, "100", 3);
    reset_rom_field(doj, ROM_PRICE, "871.98", 3);
    update_balance_new(jlc, doj, 1);
    int null_int = 0;
    set_rom_field(doj, ROM_BIN_CUM, &null_int, sizeof(int));
    long bp = jpl->cur_balance_buy;
    CHKCOND(init_bp == (bp - 871980000));
    reset_rom_field(doj, ROM_STATUS, "2", 1);
    int full = 100;
    set_rom_field(doj, ROM_BIN_CUM, &full, sizeof(int));
    set_rom_field(doj, ROM_BIN_LAST_SHARES, &full, sizeof(int));
    update_balance_fill(jlc, doj);
    bp = jpl->cur_balance_buy;
    CHKCOND(init_bp == bp);
    free_doj(doj);
    STOPTEST;
}

int test_jiggly_update_replace(const char **testname,
                               const char **extrareporting)
{
    STARTTEST;
    init_jiggly();
    REQCOND(jlc != 0);
    REQCOND(main_cfg != 0);
    dart_order_obj *doj = create_doj();
    jp_limits *jpl = get_equity_limits_for_trader(jlc, "PARNELL", 7);
    long init_bp = jpl->cur_balance_buy;
    reset_rom_field(doj, ROM_STATUS, "0", 1);
    reset_rom_field(doj, ROM_TRADER, "PARNELL", 7);
    reset_rom_field(doj, ROM_SIDE, "1", 1);
    reset_rom_field(doj, ROM_SEC_TYPE, "E", 1);
    reset_rom_field(doj, ROM_SHARES, "100", 3);
    reset_rom_field(doj, ROM_PRICE, "800.10", 3);
    update_balance_new(jlc, doj, 1);
    int null_int = 0;
    set_rom_field(doj, ROM_BIN_CUM, &null_int, sizeof(int));
    long bp = jpl->cur_balance_buy;
    CHKCOND(init_bp == (bp - 800100000));


    dart_order_obj *d2 = create_doj();
    reset_rom_field(d2, ROM_STATUS, "0", 1);
    reset_rom_field(d2, ROM_TRADER, "PARNELL", 7);
    reset_rom_field(d2, ROM_SIDE, "1", 1);
    reset_rom_field(d2, ROM_SEC_TYPE, "E", 1);
    reset_rom_field(d2, ROM_SHARES, "200", 3);
    doj->rep = d2;
    update_balance_replace(jlc, doj);
    bp = jpl->cur_balance_buy;
    CHKCOND(init_bp == (bp - 1600200000));
    printf("Open After: %ld before: %ld \n", bp, init_bp);
    update_balance_rep_rej(jlc, doj);
    bp = jpl->cur_balance_buy;
    CHKCOND(init_bp == (bp - 800100000));
    free_doj(d2);
    doj->rep = 0;
    free_doj(doj);
    STOPTEST;
}

int test_jiggly_update(const char **testname, const char **extrareporting)
{
    STARTTEST;
    init_jiggly();
    REQCOND(jlc != 0);
    REQCOND(main_cfg != 0);
    STOPTEST;
}

int test_jiggly_update_bad_trader(const char **testname,
                                  const char **extrareporting)
{
    STARTTEST;
    init_jiggly();
    REQCOND(jlc != 0);
    REQCOND(main_cfg != 0);
    STOPTEST;
}
