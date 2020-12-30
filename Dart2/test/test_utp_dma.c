#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/uio.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include "tests.h"
#include "testmacros.h"
#include "databuf.h"
#include "utp_dma.h"
#include "connection_interface.h"
#include "hashmap.h"
#include "order_token_generator.h"
#include "polling_server.h"
#include "dart_order_obj.h"
#include "dart_order_fact.h"
#include "parser_func.h"
#include "rex_constants.h"
#include "eurex.h"


rex_con* test_create_rc()
{
/*    rex_con* rc = 0;
    int r = 0;
    REX_ALLOC(rc, 8, sizeof(struct rex_connection), r);
    if(linfo == 0) {
        rc->logon = 0;
    } else {
        rc->logon = linfo;
    }
    rc->write_callback = 0;
    rc->connection_notice = 0;
    rc->parse = parse;
    rc->is_valid = 0;
    rc->p = p;
    rc->o = p->o;
    rc->conf = ex;
    rc->parser = parse_pass_back;
    rc->con_pass_back = con_pass_back;
    rc->con = create_sock_recv_manager();
    rc->read_wrapper = 0;
    rc->write_wrapper = 0;*/
    return NULL;
}

/**
 * @param f is the flag that determines if we set or clear bit.
 * @param m the bit mask
 * @param w the value to modify
 * if (f) w |= m else w &= ~m
 */
#define TSET_BIT(f, m, w)            \
    w ^= (-f ^ w) & m;              \

int test_utp_server_parse(const char **testname, const char **extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    REQCOND(buff != 0);
    long time2 = convert_to_nanosec_since_1970("20110113-09:02:53");
    CHKCOND(time2 == 1294909373000000000);
    
     
    int len = eur_to_rom_time(time2, buff);
    CHKCOND(len > 15);
    CHKCOND(strncmp("20110113-09:02:53", buff->rd_ptr, 17) == 0);
    databuf_reset(buff);
    time2 = convert_to_nanosec_since_1970("20161026-10:26:23");
    time2 += 12345600;
    len = eur_to_rom_time(time2, buff);
    CHKCOND(strncmp("20161026-10:26:23.12345", buff->rd_ptr, 17) == 0);

    databuf_reset(buff);
    dartfact* ofact =  create_dart_order_factory(4);
    dart_order_obj *doj = get_doj(ofact);
    uint64_t b = 0xdeadbeef;
    dart_itoa(doj, b, ROM_SEC_DEF, buff);
    uint32_t c = get_eurex_secuity_id(ROM_SEC_DEF,  doj);
    CHKCOND(c == 0xdeadbeef);
    databuf_reset(buff);
    b = 0xcafedeadbeef;
    dart_itoa(doj, b, ROM_SEC_DEF, buff);
    c = get_eurex_secuity_id(ROM_SEC_DEF,  doj);
    CHKCOND(c == 0xdeadbeef);

    uint8_t one = 1;
    uint8_t two = 2;
    uint8_t three = 3;
    uint8_t four = 4;
    uint8_t five = 5;
    uint8_t six = 6;
    uint8_t seven = 7;
    uint8_t eight = 8;
    CHKCOND(one != two);
    CHKCOND(three != four);
    CHKCOND(five != six);
    CHKCOND(seven != eight);
    uint64_t on = 0x1;
   // uint64_t tf = 24;
    uint64_t ord_instructions = 0;
    ord_instructions |= on << 0ULL;
    CHKCOND(ord_instructions == 1 );
    ord_instructions |= on << 1ULL;
    CHKCOND(ord_instructions == 3 );
    ord_instructions |= on << 2ULL;
    CHKCOND(ord_instructions == 7 );
    char* test_id = malloc(16);
    memcpy(test_id, &ord_instructions, 8);
    memcpy(test_id + 8, &on, 8);
    char* bucket_test = malloc(16);
    int bucket = hashfunc((unsigned char*)test_id, 16, 2048);
    int has_one = 0;
    int num_dupes = 0;
   
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t seed = (tv.tv_sec * 1000000) + tv.tv_usec;
    uint64_t i = 0;
    uint64_t j = 20170216;
    for(; j < 20170236; ++j) {
        memcpy(test_id, &j, 8);
        i = 0;
        seed += j;
        hashmap_t map = create_map(2048);
        for(;i < 65000; ++i) {
            seed += i;
            memcpy(test_id, &seed, 8);
            bucket = hashfuncOAT((unsigned char*)test_id, 8, 65536);
            len = sprintf(bucket_test, "%d", bucket);
            has_one = unique_no_copy_insert(map, bucket_test,
                                    len, (void *)test_id);
            if(has_one == 0) {
                ++num_dupes;
            }
        }
        delete_map(map);
    }
    CHKCOND(i == 65000);
    printf("Total dupes Numeric: %d \n", num_dupes);


    token_gen* tg = generic_creation_func();
    i = 0;
    num_dupes = 0;
    hashmap_t mapp = create_map(2048);
    memset(bucket_test, '\0', 16);
    for(;i < 16000; ++ i) {
        len = create_multiday_all_num_token(tg, i, bucket_test, 16, 6);
        bucket = hashfuncOAT((unsigned char*)bucket_test, len, 16384);
        len = sprintf(bucket_test, "%d", bucket);
        has_one = unique_no_copy_insert(mapp, bucket_test, len, (void*)bucket_test);
        if(has_one == 0) {
            ++num_dupes;
        }
    }
    CHKCOND(i == 16000);
    printf("Total dupes: %d \n", num_dupes);
    STOPTEST;
}
