/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "message_queue.h"
#include "parser_func.h"
#include "dart_handler.h"
//#include "fix_handler.h"
#include "rex_constants.h"
#include "config_loader.h"
#include "hashmap.h"
#include "databuf.h"
#include "testmacros.h"
#define NUM_CONNECTIONS 200

void test_smtp();
void test_parse_fix();
void test_parse_functions_directly();
void str_str_test();
void test_parse_func();

void test_db_code();
void test_order_token_creator();

#include "tests.h"              /* Declarations of tests to be run */

testfunc tests[] = {
#ifdef _SPREAD_N_READY_
     test_spread_parsing,   
#endif
/*    test_boe_killer,
    test_boe_parse_exe,
    test_boe_parse_rej,
    test_boe_parse_ack,
    test_boe_time_parse,
    test_boe_logon,
    test_boe_replace,
    test_boe_cancel,
    test_boe_new_order,
    test_build_n_hold,
    test_boe_bit_count,
    test_long_copy_doj,
    test_rom_to_cme_price,
    test_cme_to_rom_price,
    test_product_code,
    test_utp_trade,
    test_ice_sym_trans,
    test_ice_order_gen,
    test_ice_opt_sym_trans,
    test_ad_price_converter,
    test_jiggly_check_rom,
    test_find_rom_offset,
    test_jiggly_update_replace,
    test_jiggly_update_new,
    test_jiggly_update_fill,
    test_utp_order_ack,
    test_utp_time,
    test_utp_price_trans,*/
    test_tree_vs_hash,
    test_integer_hashes,
    test_rb_key_glob,
    test_bit_grabber,
    test_cp_with_itoa,
    test_boe_is_present,
    test_boe_price_trans,
    test_boe_mat_ymd,
    test_boe_sym_len,
    test_otto_byte_swap,
    test_otto_instrument_parsing,
    test_otto_sym_len,
    test_utp_server_parse,
    test_timer,
    test_time_t_convert,
    test_last_logon_logic,
    test_make_long,
    test_find_fix_offsets,
    test_resend_req,
    test_time_checksum,
    test_check_sum,
    test_realloc_ofp,
    test_build_fix_trans,
    test_fix_rom,
    test_build_fix_offsets,
    test_fix_tag_parsin,
    testshort_creation, 
    test_rom_pad_one,
    test_rom_pad_twenty_five,
    test_build_rom_pos,
    test_ph_parser,
    test_copy_doj,
    test_copy_doj_2,
    test_doj_itoa,
    test_gatekeeper_databufs,
    test_iovec_setting,
    test_deep_copy,
    test_sti_masher,
    testdatabuf_realloc,
    test_pop_queue,
    test_config_loader,
    test_modulus,
    test_tm_to_str,
    testsuffixmap,
    testdatabuf,
    test_hash,
    test_small_bucket,
    test_contains_key,
    test_reset_calculator,
    test_rb_insert,
    test_rb_multi_orders,
    test_rb_remove,
    /*test_date_roll_check,
    test_date_roll_check2,
    test_date_roll_check3,
    test_date_roll_check4,
    test_mail_message,*/
    test_mult_thread_access,
    test_queue,
    test_next_pow_two,
    test_cx_cross_hash
};

int main(int argc, char **argv)
{
    unsigned int i;
    const unsigned int ntests = sizeof tests / sizeof tests[0];
    for (i = 0; i < ntests; ++i) {
        const char *name = 0;
        const char *extrainfo = 0;
        const int nfailures = tests[i] (&name, &extrainfo);
        if (name == 0) {
            name = "Unknown test";
        }
        printf("(%d) %-30s\t%d failures", i, name, nfailures);
        if (extrainfo == 0) {
            printf("\n");
        } else {
            printf(": %s\n", extrainfo);
        }
    }
    return 0;
}
