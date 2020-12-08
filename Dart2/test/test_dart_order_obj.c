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
#include "message_queue.h"
#include "databuf.h"
#include "dart_handler.h"
#include "rex_constants.h"
#include "dart_order_obj.h"
#include "dart_order_fact.h"
#include "parser_func.h"
#include "pillar.h"
#include "pillar_oats_parser.h"
#include "pillar_test.h"


static char *static_rom_one =
    "E,20110523-15:44:05,PARNELL,PARNELL1104231044050001,1,ADSK,100,0.950000,2,0,A,0551,RONIN,24,,,,PARNELL-50110523-2,,,,,USER0001,WAG,PARNELL,,8522820409900022,PARNELL,PARNELL-50110523-2-0,N,,,,,,E,,,1,,,24,,,,,,,,,20110523-15:44:05,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,DODO\n";

static char *static_rom_three =
    "E,20110523-15:44:05,PARNELL,PARNELL1104231044050001,1,ADSK,10000,0.950000,2,0,A,0551,RONIN,24,,,,PARNELL-50110523-2,,,,,USER0001,WAG,PARNELL,,8522820409900022,PARNELL,PARNELL-50110523-2-0,N,,,,,,E,,,1,,,24,9999,,34.5,,,,,,20110523-15:44:05,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,DODO\n";

static char *static_rom_two =
    "S,20110523-15:44:05,PARNELL,PARNELL1104231044050001,1,ADSK,100,0.950000,2,0,A,0551,RONIN,24,,,,PARNELL-50110523-2,,,,,USER0001,WAG,PARNELL,,8522820409900022,PARNELL,PARNELL-50110523-2-0,N,,,,,,E,,,1,,,24,200,,45.87,,,,400,,20110523-15:44:05,,,,,,,,,,,,,,,,,,,,,,,,,,,,,\n";


int get_int_for_val(dart_order_obj* ldoj, int rom_off)
{
    uint32_t last_shares = 0;//atoi(shar_finder);
    char shar_finder[64];
    memset(shar_finder, '\0',64);
    struct db_node *db = ldoj->head;
    if (ldoj->positions[rom_off].iov_len != 0) {
        while (db != NULL
               && !(ldoj->positions[rom_off].iov_base < db->end_ptr
                    && ldoj->positions[rom_off].iov_base > db->start_ptr)) {
            db = db->next;
        }
        if (db == NULL) {
                memcpy(shar_finder, ldoj->positions[rom_off].iov_base,
                               ldoj->positions[rom_off].iov_len);
                last_shares = atoi(shar_finder);
        } else {
                int len = ldoj->positions[rom_off].iov_base - db->start_ptr;
                char *offset = db->data->buffer + len;
                memcpy(shar_finder, offset, ldoj->positions[rom_off].iov_len);
                last_shares = atoi(shar_finder);
        }
        db = ldoj->head;
    }
    return last_shares;
}
int test_bit_grabber(const char** testname, const char** extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    REQCOND(buff != 0);

    dartfact* ofact =  create_dart_order_factory(8);
    dart_order_obj *doj = get_doj(ofact);

    dart_style_reset(doj);
    set_rom_field(doj, 0, "S", 1);
    set_rom_field(doj, ROM_TYPE, "2", 1);
    set_rom_field(doj, ROM_SIDE, "5", 1);
    set_rom_field(doj, ROM_TIF,"2" , 1);
    set_rom_field(doj, ROM_DESTID,"247" , 3);
    set_rom_field(doj, ROM_CAP, "A", 1);

    uint64_t bits =set_pillar_bitfields(doj); 
    uint8_t fside = get_bitfield_instruction(bits, 60, 64);    
    uint8_t cap = get_bitfield_instruction(bits, 22,25);
    uint8_t tif = get_bitfield_instruction(bits, 31,34);
    CHKCOND(fside == 3);
    CHKCOND(cap == 1);
    CHKCOND(tif == 3);
    dart_style_reset(doj);
    set_rom_field(doj, 0, "S", 1);
    set_rom_field(doj, ROM_TYPE, "2", 1);
    set_rom_field(doj, ROM_SIDE, "1", 1);
    set_rom_field(doj, ROM_TIF,"0" , 1);
    set_rom_field(doj, ROM_DESTID,"247" , 3);
    set_rom_field(doj, ROM_CAP, "A", 1);
    bits =set_pillar_bitfields(doj);
    printf("Dis da bits: %lu \n", bits);


    databuf_destroy(buff);
    STOPTEST;
}


int test_cp_with_itoa(const char **testname, const char** extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    queue_t q = create_queue();
    REQCOND(buff != 0);
    CHKCOND(is_empty(q));
    dartfact* ofact =  create_dart_order_factory(8);
    dart_order_obj *doj = get_doj(ofact);

    dart_style_reset(doj);
    int i = 0;
    int cum = 0;
    for(;i < 200; ++i) {
        databuf_write(doj->head->data, static_rom_two);
        ph_build_iovec(doj);
        set_rom_field(doj, 0, "S", 1);
        u64_dart_itoa(doj, 2498572389798, ROM_EX_TAG,buff);
        databuf_reset(buff);
        u64_dart_itoa(doj, 89876543456, EXEC_ID,buff);
        databuf_reset(buff);
        unsigned_dart_itoa(doj, 200 - i,
                                 ROM_LEAVES, buff);
        databuf_reset(buff);
        cum += (i * 3);
        unsigned_dart_itoa(doj, cum, ROM_CUM, buff);
        databuf_reset(buff);
        unsigned_dart_itoa(doj, i * 3, ROM_LAST_SHARES, buff);
        databuf_reset(buff);
        dart_order_obj *noj = get_doj(ofact);
        copy_dart_order_obj(doj, noj);
        enqueue(q, noj);
    }

    CHKCOND(!is_empty(q));
    cum = 0;
    i = 0;
    element_t e, temp;
    e = dequeue(q);
    CHKCOND(is_empty(q));
    for(;i < 200; ++i) {
      if(e != NULL) {
        CHKCOND(e->d != NULL);
        dart_order_obj* ldoj = (dart_order_obj*)e->d;
        int last_shares = get_int_for_val(ldoj, ROM_LAST_SHARES);
        CHKCOND(last_shares == (i * 3));
        cum += (i * 3);
        CHKCOND(cum ==  get_int_for_val(ldoj, ROM_CUM));
        CHKCOND((200 - i) ==  get_int_for_val(ldoj, ROM_LEAVES));
        temp = e;
        e = temp->next;
        free_element(q, temp);
      }
    }
    CHKCOND(is_empty(q));
    databuf_destroy(buff);

    clean_fact(ofact);
    STOPTEST;
}
int test_copy_doj(const char **testname, const char **extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    REQCOND(buff != 0);
    dartfact* ofact =  create_dart_order_factory(4);
    dart_order_obj *doj = get_doj(ofact);
    dart_order_obj *noj = get_doj(ofact);
    dart_order_obj *filler = get_doj(ofact);
    dart_style_reset(doj);
    dart_style_reset(filler);
    dart_style_reset(noj);
    int i = 0;
    int sro_len = strlen(static_rom_one);
    for (; i < 5; ++i) {
        databuf_write(doj->head->data, static_rom_one);
        ph_build_iovec(doj);
        copy_dart_order_obj(doj, noj);
        set_copied_doo_in_buf(noj, buff);
        CHKCOND(strncmp(buff->buffer, static_rom_one, sro_len) == 0);
        databuf_write(filler->head->data, static_rom_two);
        ph_build_iovec(filler);
        int cum = 400;
        int ls = 200;
        fill_in_rom_field(doj, ROM_BIN_CUM, &cum, sizeof(int));
        fill_in_rom_field(doj, ROM_BIN_LAST_SHARES, &ls, sizeof(int));
        fill_in_rom_field(doj, ROM_LAST_SHARES,
                          filler->positions[ROM_LAST_SHARES].iov_base,
                          filler->positions[ROM_LAST_SHARES].iov_len);
        fill_in_rom_field(doj, ROM_EXEC_PRICE,
                          filler->positions[ROM_EXEC_PRICE].iov_base,
                          filler->positions[ROM_EXEC_PRICE].iov_len);


        dart_style_reset(doj);
        dart_style_reset(filler);
        dart_style_reset(noj);
    }
    reset_doj(ofact, doj);
    reset_doj(ofact, filler);
    reset_doj(ofact, noj);
    databuf_destroy(buff);

    clean_fact(ofact);
    STOPTEST;
}
int test_long_copy_doj(const char **testname, const char **extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    REQCOND(buff != 0);
    dartfact* ofact =  create_dart_order_factory(4);
    dart_order_obj *doj = get_doj(ofact);
    dart_order_obj *noj = get_doj(ofact);
    dart_order_obj *filler = get_doj(ofact);
    dart_style_reset(doj);
    dart_style_reset(filler);
    dart_style_reset(noj);
    int sro_len = strlen(static_rom_one);
    databuf_memcpy(doj->head->data, static_rom_one, sro_len);
    ph_build_iovec(doj);

    fill_in_rom_field(doj, ROM_SHARES, "100000", 5);
    int i = 0;
    for(;i < 800; ++i) {
        int cum = 400;
        int ls = 200;
        databuf_write(filler->head->data, static_rom_two);
        ph_build_iovec(filler);
        fill_in_rom_field(doj, ROM_BIN_CUM, &cum, sizeof(int));
        fill_in_rom_field(doj, ROM_BIN_LAST_SHARES, &ls, sizeof(int));
        if(i % 2) {
            fill_in_rom_field(doj, ROM_LAST_SHARES,
                                   "9999", 4);
            fill_in_rom_field(doj, ROM_EXEC_PRICE,"34.5", 4);
        } else {
            fill_in_rom_field(doj, ROM_LAST_SHARES,"864", 3);
            fill_in_rom_field(doj, ROM_EXEC_PRICE,"34.599", 6);
        }
    }
    copy_dart_order_obj(doj, noj);
    CHKCOND(strncmp(getpval(noj, ROM_LAST_SHARES), getpval(doj, ROM_LAST_SHARES),
               getplen(doj, ROM_LAST_SHARES)) == 0);
    fill_in_rom_field(doj, ROM_LAST_SHARES,
                           "32", 2);
    set_copied_doo_in_buf(noj, buff);
    CHKCOND(strncmp(buff->buffer, static_rom_three, 262) == 0);
    //printf("%s\n", static_rom_three);
    //printf("%s\n", buff->buffer);
    clean_fact(ofact);
    databuf_destroy(buff);
    STOPTEST;
}

int test_deep_copy(const char **testname, const char **extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    REQCOND(buff != 0);
    dart_init_def_pos();
    dartfact* ofact =  create_dart_order_factory(4);
    dart_order_obj *doj = get_doj(ofact);
    dart_order_obj *noj = get_doj(ofact);
    dart_style_reset(doj);
    dart_style_reset(noj);
    databuf_write(doj->head->data, static_rom_one);
    ph_build_iovec(doj);
    int i = 0;
    for (; i < 200; ++i) {
        set_rom_field(doj, ROM_TAG, "PARNELL-50110523-2", 18);
    }
    copy_dart_order_obj(doj, noj);
    set_copied_doo_in_buf(noj, buff);
    CHKCOND(strncmp(buff->buffer, static_rom_one, 262) == 0);
    clean_fact(ofact);
    STOPTEST;
}

int test_send_bunches(struct iovec *miov, int iovcnt, ssize_t inc, size_t partial_amt, int *err,
                    int *vec_sent)
{
    struct iovec *iov = miov;
    size_t in_iovec = 0;
    size_t bytes_written = 0;
    ssize_t temp = inc;
    while (bytes_written < partial_amt) {
        temp = inc;
        bytes_written += (size_t) temp;
        while (iov && iovcnt > 0) {
            if (iov->iov_len <= temp) {
                temp -= iov->iov_len;
                if(in_iovec) {
                    iov->iov_base -= in_iovec;
                    iov->iov_len += in_iovec;
                    in_iovec = 0;
                }
                ++iov;
                --iovcnt;
            } else {
                iov->iov_base += temp;
                iov->iov_len -= temp;
                in_iovec += temp;
                break;
            }
        }
    }
    if(in_iovec) {
        iov->iov_base -= in_iovec;
        iov->iov_len += in_iovec;
    }
    *err = 0;
    *vec_sent = iovcnt;
    return bytes_written;
}
int test_iovec_setting(const char **testname, const char **extrareporting)
{
    STARTTEST;
    dartfact* ofact =  create_dart_order_factory(4);
    dart_order_obj *doj = get_doj(ofact);
    dart_style_reset(doj);
    databuf_write(doj->head->data, static_rom_one);
    ph_build_iovec(doj);
    CHKCOND(strncmp
            ("PARNELL-50110523-2", doj->positions[34].iov_base,
             doj->positions[34].iov_len) == 0);
    CHKCOND(strncmp
            ("PARNELL", doj->positions[4].iov_base,
             doj->positions[4].iov_len) == 0);
    int err = 0;
    int vec_sent = 0;
    int total_sent = test_send_bunches(doj->positions, DART_SEND_LEN, 10, 60, &err, &vec_sent);
    CHKCOND(total_sent == 60);
    databuf_t *buff = databuf_alloc(512);
    copy_iovecs_to_buff(doj->positions, vec_sent,
                         buff, total_sent);
    CHKCOND(buff->buffer[0] == '0');
    CHKCOND(databuf_unread(buff) == 204);


    total_sent = test_send_bunches(doj->positions, DART_SEND_LEN, 4, 264, &err, &vec_sent);
    databuf_reset(buff);
    CHKCOND(total_sent == 264);
    CHKCOND(vec_sent == 0);

    total_sent = test_send_bunches(doj->positions, DART_SEND_LEN, 9, 72, &err, &vec_sent);
    CHKCOND(total_sent == 72);
    CHKCOND(vec_sent == 146); 
    copy_iovecs_to_buff(doj->positions, vec_sent,
                         buff, total_sent);
    CHKCOND(buff->buffer[0] == '2');
    CHKCOND(databuf_unread(buff) == 192);

    total_sent = test_send_bunches(doj->positions, DART_SEND_LEN, 264, 264, &err, &vec_sent);
    CHKCOND(total_sent == 264);
    CHKCOND(vec_sent == 0);
    clean_fact(ofact);
    STOPTEST;
}

int test_doj_itoa(const char **testname, const char **extrareporting)
{
    STARTTEST;
    dartfact* ofact =  create_dart_order_factory(4);
    dart_order_obj *doj = get_doj(ofact);
    dart_style_reset(doj);
    databuf_t *buff = databuf_alloc(512);
    REQCOND(buff != 0);
    int hundred = 100;
    int eight_sf = 875;
    int two_towsand = 2245;
    int two_mill = 2245986;
    uint64_t big = 47135000005;
    dart_itoa(doj, hundred, 6, buff);
    CHKCOND(strncmp("100", doj->positions[6].iov_base, doj->positions[6].iov_len) == 0);
    unsigned_dart_itoa(doj, eight_sf,6, buff);
    CHKCOND(strncmp("875", doj->positions[6].iov_base, doj->positions[6].iov_len) == 0);
    dart_itoa(doj, two_mill, 6, buff);
    CHKCOND(strncmp("2245986", doj->positions[6].iov_base, doj->positions[6].iov_len) == 0);
    unsigned_dart_itoa(doj, hundred,6, buff);
    CHKCOND(strncmp("100", doj->positions[6].iov_base, doj->positions[6].iov_len) == 0);
    dart_itoa(doj, two_towsand, 6, buff);
    CHKCOND(strncmp("2245", doj->positions[6].iov_base, doj->positions[6].iov_len) == 0);
    unsigned_dart_itoa(doj, two_mill,6, buff);
    CHKCOND(strncmp("2245986", doj->positions[6].iov_base, doj->positions[6].iov_len) == 0);
    dart_style_reset(doj);
    databuf_reset(buff);
    u64_dart_itoa(doj, big, 16, buff);
    CHKCOND(strncmp("47135000005", doj->positions[16].iov_base, doj->positions[16].iov_len) == 0);
    databuf_reset(buff);
    unsigned_dart_itoa(doj, big, 6, buff);
    CHKCOND(strncmp("47135000005", doj->positions[6].iov_base, doj->positions[6].iov_len) != 0);
    clean_fact(ofact);
    STOPTEST;
}

int test_copy_doj_2(const char **testname, const char **extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    REQCOND(buff != 0);
    dartfact* ofact =  create_dart_order_factory(4);
    dart_order_obj *doj = get_doj(ofact);
    dart_style_reset(doj);
    databuf_write(doj->head->data, static_rom_one);
    ph_build_iovec(doj);
    dart_order_obj *noj = get_doj(ofact);
    dart_style_reset(noj);
    set_rom_field(doj, 34, "PF_DODO_BRAIN", 13);
    reset_rom_field(doj, 6, "STATIC", 6);
    copy_dart_order_obj(doj, noj);
    set_copied_doo_in_buf(noj, buff);
    int sro_len = strlen(static_rom_one);
    CHKCOND(strncmp(buff->buffer, static_rom_one, sro_len) != 0);
    CHKCOND(strstr(buff->buffer, "PF_DODO_BRAIN") != NULL);
    CHKCOND(strstr(buff->buffer, "STATIC") != NULL);
    databuf_destroy(buff);
    clean_fact(ofact);
    STOPTEST;
}
