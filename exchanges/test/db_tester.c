#include "tests.h"
#include "testmacros.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "db_layer.h"
#include "generic_db.h"
#include "message_queue.h"
#include "databuf.h"
#include "ex_common.h"
#include "options_common.h"
#include "dart_order_allocator.h"
#include "dart_orderbook.h"
#include "order_token_creator.h"
#include "dart_constants.h"
#include "rom2fix.h"
#include "../ex_db/database_conf.h"

static token_struct_t *idgenerator = 0;
static char datestr[9] = { '\0' };

static char *test_gen_opt_id(size_t * nlen)
{
    char *cl_ord_id = 0;
    if (nlen != 0) {
        int retval = 0;
        cl_ord_id = calloc(200, 1);
        memcpy(cl_ord_id, datestr, sizeof datestr - 1);
        if ((retval = create_order_token(idgenerator, cl_ord_id +
                                         (sizeof datestr - 1),
                                         200 - 1)) > 0) {
            *nlen = (size_t) retval + (sizeof datestr - 1);
        } else {
            *nlen = 0;
        }
    }
    return cl_ord_id;
}

int set_opt_test_def(char *rommsg, struct dart_fix_pos *positions,
                     char *mtype, databuf_t * output,
                     size_t romfield, int *rejected,
                     dart_ex_order_obj * order)
{
    databuf_write(output, "%c21=1", FIX_FIELD_DELIM);
    return 1;
}

int get_is_test_connected()
{
    return 1;
}

int test_db(const char **testname, const char **extrareporting)
{
    STARTTEST queue_t q = create_queue();
    REQCOND(q != 0);
    element_t e, temp;
    int i = 0;
    e = NULL;
    temp = NULL;
    struct database_conf dbconf = {
        .server = "DEV_DS_1",
        .database = "rom",
        .username = "rom_dba",
        .password = "abd_mor",
    };
    get_db_objects(&dbconf, q, "7");
    e = dequeue(q);
    CHKCOND(e != NULL);
    while (e != NULL) {
        struct dart_ex_result_set *ders = NULL;
        ders = (struct dart_ex_result_set *) e->d;
        CHKCOND(ders != NULL);
        if (ders != NULL) {
            CHKCOND(strlen(ders->omTag) > 0);
            ++i;
        }

        temp = e;
        e = temp->next;
        if (ders != NULL) {
            free(ders);
        }
        temp->d = NULL;
        free(temp);
        temp = NULL;
    }
    destroy_queue(q);
    CHKCOND(i > 0);
    STOPTEST;
}

int test_db2(const char **testname, const char **extrareporting)
{
    STARTTEST queue_t q = create_queue();
    REQCOND(q != 0);
    element_t e, f, g, temp;
    //int i = 0;
    char *query = NULL;
    e = NULL;
    temp = NULL;
    f = NULL;
    g = NULL;
    query =
        "SELECT callPut, capacity, clearingAccount, clearingID, CMTAID, cumQty, exchangeTag, leavesQty, maxFloor, mmAcct, omTag, orderTag, orderType, price, qty, secType, side, stopPrice, strikePrice,symbol,tag,tif,expireDate FROM rom..Orders WHERE DestID = 7 AND status <> 2 AND status <> 4 AND status <> 8 AND status <> 12";
    gendb_get_results("rom_dba", 7, "abd_mor", 7, "DEV_DS_1", 8, q, query);
    e = dequeue(q);
    CHKCOND(e != NULL);
    while (e != NULL) {
        queue_t q1 = NULL;
        q1 = (queue_t) e->d;
        CHKCOND(q1 != NULL);
        if (q1 != NULL) {
            f = dequeue(q1);
            CHKCOND(f != NULL);
            while (f != NULL) {
                gendb_v *gv = NULL;
                gv = (gendb_v *) f->d;
                g = f;
                f = g->next;
                if (gv != NULL) {
                    free(gv);
                }
                g->d = NULL;
                free(g);
                g = NULL;
            }
        }
        temp = e;
        e = temp->next;
        destroy_queue(q1);
        free(temp);
        temp = NULL;
    }
    destroy_queue(q);
STOPTEST}

char *insert_two_a =
    "0, 'P', 'B', 'RONIN', '474', NULL, 0, NULL, NULL, NULL, NULL, 77, 77, NULL, '201106', 'WAG', NULL, NULL, 1, 'USER0001', 0, 0, NULL, 0, 1,";
char *insert_two_b = " 'C', NULL, NULL, 2, 1, 'PARNELL', 1, NULL, 1, 'U', 'E', 2, NULL, 0, 0, 0, 'ALL', 'PARNELL1104251456390040.1', 'New Order', 0, 'PARNELL', 'PARNELL', NULL, NULL, NULL, NULL, 6, ";        //'PARNELL-50110525-18', NULL, NULL, NULL, 0, NULL)";
char *insert_three_a =
    "0, 'C', 'B', 'RONIN', '474', NULL, 0, NULL, NULL, NULL, NULL, 77, 77, NULL, '201106', 'WAG', NULL, NULL, 1, 'USER0001', 0, 0, NULL, 0, 1,";
char *insert_three_b = " 'C', NULL, NULL, 2, 1, 'PARNELL', 1, NULL, 1, 'U', 'E', 1, NULL, 0, 0, 0, 'ALL', 'PARNELL1104251456390040.0', 'New Order', 0, 'PARNELL', 'PARNELL', NULL, NULL, NULL, NULL, 6,";       // 'PARNELL-50110525-18', NULL, NULL, NULL, 0, NULL)";
char *insert_one_a =
    "0, NULL, 'B', 'RONIN', '474', NULL, 0, 'USD', NULL, NULL, NULL, 77, 77, NULL, '201106', 'WAG', NULL, NULL, 1, 'USER0001', 0, 0, NULL, 0, 1,";
char *insert_one_b =
    " 'O', NULL, NULL, 2, 1, 'PARNELL', 1.2, NULL, 1, 'U', 'O', 1, NULL, 0, 0, 50, NULL, 'PARNELL1104251456390040', 'New Order', 0, 'PARNELL', 'PARNELL', NULL, NULL, NULL, NULL, 3, NULL, NULL, NULL, NULL, 0, '6/18/2011 12:00:00 AM')";
static char *create_unique_ids(char *p1, char *p2, int num, int leg,
                               char *rom_tag, databuf_t * d_off)
{
    time_t rawtime;
    struct tm *ti;
    int year;
    int month;
    time(&rawtime);
    struct tm lm;
    ti = gmtime_r(&rawtime, &lm);
    ti = &lm;
    year = 1900 + ti->tm_year;
    month = 1 + ti->tm_mon;
    databuf_write(d_off,
                  "INSERT INTO Orders VALUES (77%04d%02d%02d%02d%02d%02d%02d,",
                  year, month, ti->tm_mday, ti->tm_hour, ti->tm_min,
                  ti->tm_sec, num);
    databuf_write(d_off, "'%04d-%02d-%02d %02d:%02d:%02d',", year, month,
                  ti->tm_mday, ti->tm_hour, ti->tm_min, ti->tm_sec);
    databuf_write(d_off, "%s", p1);
    databuf_write(d_off, "'PARNELL-77%04d%02d%02d-%02d%02d%02d%02d',",
                  year, month, ti->tm_mday, ti->tm_hour, ti->tm_min,
                  ti->tm_sec, num);
    databuf_write(d_off, "'%04d-%02d-%02d %02d:%02d:%02d',", year, month,
                  ti->tm_mday, ti->tm_hour, ti->tm_min, ti->tm_sec);
    databuf_write(d_off, "%s", p2);
    if (leg == 1) {
        sprintf(rom_tag, "%02d%02d%02d%02d", ti->tm_hour, ti->tm_min,
                ti->tm_sec, num);
    } else {
        databuf_write(d_off,
                      "'PARNELL-77%04d%02d%02d-%s', NULL, NULL, NULL, 0, '6/18/2011 12:00:00 AM')",
                      year, month, ti->tm_mday, rom_tag);
    }
    return d_off->buffer;
}

static void run_insertion_query(queue_t q, char *query)
{
    gendb_get_results("rom_dba", 7, "abd_mor", 7, "DEV_DS_1", 8, q, query);
}

int test_options_db(const char **testname, const char **extrareporting)
{
    STARTTEST queue_t q = create_queue();
    REQCOND(q != 0);
    char *rom_tag = calloc(32, 1);
    databuf_t *d_off = databuf_alloc(500);
    char *query =
        create_unique_ids(insert_one_a, insert_one_b, 1, 1, rom_tag,
                          d_off);
    run_insertion_query(q, query);
    databuf_reset(d_off);
    char *query2 =
        create_unique_ids(insert_two_a, insert_two_b, 2, 6, rom_tag,
                          d_off);
    run_insertion_query(q, query2);
    databuf_reset(d_off);
    char *query3 =
        create_unique_ids(insert_three_a, insert_three_b, 3, 6, rom_tag,
                          d_off);
    run_insertion_query(q, query3);
    databuf_reset(d_off);
    orderbook *ob = init_order_book(test_gen_opt_id, get_is_test_connected,
                                    def_rom2fix, set_opt_test_def,
                                    &def_xlate_fixrom, 20,
                                    "test_opt", 8, "opt_test",
                                    8, NULL, 0);
    struct database_conf dbconf = {
        .server = "DEV_DS_1",
        .database = "rom",
        .username = "rom_dba",
        .password = "abd_mor",
    };
    opt_database_init(&dbconf, "77", ob, 0);
    time_t rawtime;
    element_t e, temp;
    struct tm *ti;
    int year;
    int month;
    time(&rawtime);
    struct tm lm;
    ti = gmtime_r(&rawtime, &lm);
    ti = &lm;
    year = 1900 + ti->tm_year;
    month = 1 + ti->tm_mon;
    databuf_write(d_off, "PARNELL-77%04d%02d%02d-%s", year, month,
                  ti->tm_mday, rom_tag);
    dart_ex_order_obj *obj = NULL;
    int ret_val = 0;
    printf("%s %lu \n", d_off->buffer, databuf_unread(d_off));
    ret_val =
        get_map_entry(ob->rom_ord_id_map, d_off->buffer,
                      databuf_unread(d_off), (void *) &obj);
    CHKCOND(ret_val > 0);
    if(ret_val <= 0) {
        clean_dart_oo_pool(ob->mgr);
    } else {
        queue_t children = doa_get_children(obj);
        CHKCOND(children != NULL);
        e = dequeue(children);
        int d = 0;
        CHKCOND(e != NULL);
        while (e != NULL) {
            ++d;
            temp = e;
            e = temp->next;
            free(temp);
            temp = NULL;
        }
        CHKCOND(d == 2);
        clean_dart_oo_pool(ob->mgr);
    }
STOPTEST}
