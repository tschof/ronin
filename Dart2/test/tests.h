#ifndef __TESTS_H__
#define __TESTS_H__

typedef int (*testfunc) (const char **testname,
                         const char **extrareporting);

#define DECLARE_TEST(testname) \
    int testname(const char **testname, const char **extrareporting);

/* Each of these tests will return the number of failures for easy reporting */
int testsuffixmap(const char **testname, const char **extrareporting);
int testdatabuf(const char **testname, const char **extrareporting);
int test_hash(const char **testname, const char **extrareporting);
int test_comma_remove(const char **testname, const char **extrareporting);
int test_db(const char **testname, const char **extrareporting);
int test_db2(const char **testname, const char **extrareporting);
int test_tpos_sym_trans(const char **testname,
                        const char **extrareporting);
int test_spread_parse(const char **testname, const char **extrareporting);
int test_full_spread_parse(const char **testname,
                           const char **extrareporting);
int test_full_spread_cancel(const char **testname,
                            const char **extrareporting);
int test_spread_fr_open_mess(const char **testname,
                             const char **extrareporting);
int test_ftr_trans_new(const char **testname, const char **extrareporting);
int test_ftr_trans_replace(const char **testname,
                           const char **extrareporting);
int test_ftr_trans_cnl(const char **testname, const char **extrareporting);
int test_rtf_trans_fill(const char **testname,
                        const char **extrareporting);
int test_rtf_trans_open(const char **testname,
                        const char **extrareporting);
int test_rtf_trans_out(const char **testname, const char **extrareporting);
int test_rtf_trans_rpl(const char **testname, const char **extrareporting);
int test_rtf_trans_rep_rej(const char **testname,
                           const char **extrareporting);
int test_rtf_trans_cnl_rej(const char **testname,
                           const char **extrareporting);
int test_small_bucket(const char **testname, const char **extrareporting);
int test_contains_key(const char **testname, const char **extrareporting);
int test_r2f_spread_parse(const char **testname,
                          const char **extrareporting);
int test_r2f_spread_cancel(const char **testname,
                           const char **extrareporting);
int test_r2f_reg_option(const char **testname,
                        const char **extrareporting);
int test_r2f_reg_opt_cancel(const char **testname,
                            const char **extrareporting);
int test_option_life_cycle(const char **testname,
                           const char **extrareporting);
int test_rb_insert(const char **testname, const char **extrareporting);
int test_rb_multi_orders(const char **testname,
                         const char **extrareporting);
DECLARE_TEST(test_reset_calculator)
    DECLARE_TEST(test_r2f_spread_life_cycle)
    DECLARE_TEST(test_cx_frame)
    DECLARE_TEST(test_ntohll_htonll)
    DECLARE_TEST(test_amount_trans)
    DECLARE_TEST(test_price_trans)
    DECLARE_TEST(test_rb_remove)
    DECLARE_TEST(test_cx_trade)
    DECLARE_TEST(test_cx_ii_reply)
    DECLARE_TEST(test_cx_order_ack)
    DECLARE_TEST(test_cx_order_ack_rej)
    DECLARE_TEST(test_cancel_rej)
    DECLARE_TEST(test_cx_order_replaced)
    DECLARE_TEST(test_cx_order_replace_rej)
    DECLARE_TEST(test_cx_order_replace_cnl)
    DECLARE_TEST(test_cx_order_canclled)
    DECLARE_TEST(test_date_roll_check)
    DECLARE_TEST(test_date_roll_check2)
    DECLARE_TEST(test_date_roll_check3)
    DECLARE_TEST(test_date_roll_check4)
    DECLARE_TEST(test_rb_tree_crush)
    DECLARE_TEST(test_rb_masher)
    DECLARE_TEST(test_cx_cross_hash)
    DECLARE_TEST(test_mult_thread_access)
    DECLARE_TEST(test_rom_pad_one)
    DECLARE_TEST(test_rom_pad_twenty_five)
    DECLARE_TEST(test_mail_message)
    DECLARE_TEST(test_options_db)
    DECLARE_TEST(test_resend_request_handler)
    DECLARE_TEST(test_build_rom_pos)
    DECLARE_TEST(test_tm_to_str)
    DECLARE_TEST(test_queue)
    DECLARE_TEST(test_create_fix_header)
    DECLARE_TEST(test_modulus)
    DECLARE_TEST(test_next_pow_two)
    DECLARE_TEST(test_month_codes)
    DECLARE_TEST(test_symbol_mod)
    DECLARE_TEST(test_config_loader)
    DECLARE_TEST(test_pop_queue)
    DECLARE_TEST(testdatabuf_realloc)
    DECLARE_TEST(test_ph_parser)
    DECLARE_TEST(test_trans_rom_symbol)
    DECLARE_TEST(test_utp_price_trans)
    DECLARE_TEST(test_utp_time)
    DECLARE_TEST(test_utp_order_ack)
    DECLARE_TEST(test_copy_doj)
    DECLARE_TEST(test_copy_doj_2)
    DECLARE_TEST(test_jiggly_update_new)
    DECLARE_TEST(test_jiggly_update_fill)
    DECLARE_TEST(test_jiggly_update_replace)
    DECLARE_TEST(test_find_rom_offset)
    DECLARE_TEST(test_jiggly_check_rom)
    DECLARE_TEST(test_doj_itoa)
    DECLARE_TEST(test_iovec_setting)
    DECLARE_TEST(test_deep_copy)
    DECLARE_TEST(test_ad_price_converter)
    DECLARE_TEST(test_timer)
    DECLARE_TEST(test_time_t_convert)
    DECLARE_TEST(test_last_logon_logic)
    DECLARE_TEST(test_make_long)
    DECLARE_TEST(testshort_creation)
    DECLARE_TEST(test_fix_tag_parsin)
    DECLARE_TEST(test_build_fix_offsets)
    DECLARE_TEST(test_find_fix_offsets)
    DECLARE_TEST(test_build_fix_trans)
    DECLARE_TEST(test_fix_rom)
    DECLARE_TEST(test_rom_pad_one)
    DECLARE_TEST(test_rom_pad_twenty_five)
    DECLARE_TEST(test_build_rom_pos)
    DECLARE_TEST(test_ph_parser)
    DECLARE_TEST(test_ice_sym_trans)
    DECLARE_TEST(test_ice_opt_sym_trans)
    DECLARE_TEST(test_ice_order_gen)
    DECLARE_TEST(test_resend_req)
    DECLARE_TEST(test_time_checksum)
    DECLARE_TEST(test_utp_trade)
    DECLARE_TEST(test_gatekeeper_databufs)
    DECLARE_TEST(test_sti_masher)
    DECLARE_TEST(test_product_code)
    DECLARE_TEST(test_rom_to_cme_price)
    DECLARE_TEST(test_cme_to_rom_price)
    DECLARE_TEST(test_check_sum)
    DECLARE_TEST(test_realloc_ofp)
    DECLARE_TEST(test_long_copy_doj)
    DECLARE_TEST(test_boe_bit_count)
    DECLARE_TEST(test_build_n_hold)
    DECLARE_TEST(test_boe_new_order)
    DECLARE_TEST(test_boe_cancel)
    DECLARE_TEST(test_boe_replace)
    DECLARE_TEST(test_boe_logon)
    DECLARE_TEST(test_boe_time_parse)
    DECLARE_TEST(test_boe_parse_ack)
    DECLARE_TEST(test_boe_parse_rej)
    DECLARE_TEST(test_boe_parse_exe)
    DECLARE_TEST(test_boe_killer)
    DECLARE_TEST(test_utp_server_parse)
    DECLARE_TEST(test_otto_sym_len)
    DECLARE_TEST(test_otto_instrument_parsing)
    DECLARE_TEST(test_otto_byte_swap)
    DECLARE_TEST(test_boe_sym_len)
    DECLARE_TEST(test_boe_mat_ymd)
    DECLARE_TEST(test_boe_price_trans)
    DECLARE_TEST(test_boe_is_present)
    DECLARE_TEST(test_cp_with_itoa)
    DECLARE_TEST(test_bit_grabber)
    DECLARE_TEST(test_rb_key_glob)
    DECLARE_TEST(test_integer_hashes)
    DECLARE_TEST(test_tree_vs_hash)
#ifdef _SPREAD_N_READY_
    DECLARE_TEST(test_spread_parsing)
#endif

#endif                          //__TESTS_H__
