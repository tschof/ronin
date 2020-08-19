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

#include "client_manager.h"
#include "message_queue.h"
#include "parser_func.h"
#include "rom_handler.h"
#include "fix_handler.h"
#include "fix_rom_trans.h"
#include "dart_constants.h"
#include "config_loader.h"
#include "fix_admin_master.h"
#include "dart_order_allocator.h"
//#include "nyse_order_tracker.h"
#include "db_layer.h"
#include "mailer.h"
#include "hashmap.h"
#include "ex_common.h"
#include "databuf.h"
#include "order_token_creator.h"
#include "testmacros.h"
#define NUM_CONNECTIONS 200

//static char *rom_client_name = "ROM3";
//static int rom_client_len = 4;
//static int fix_client_len = 7;
//static char connections[NUM_CONNECTIONS];
//static int current_con = 0;

//void from_fix_func(struct message *mess);
void test_smtp();
void test_parse_fix();
void test_parse_functions_directly();
void str_str_test();
void test_parse_func();
void client_from_fix(struct message *mess)
{
}

void server_from_rom_func(struct message *mess);
//struct init_args* load_rom_config();
//struct init_args* load_fix_config();
void con_callBack(con_obj * co, int is_connected);
void fix_connection_callBack(con_obj * co, int is_connected);
void test_lac(con_obj * co);
void test_db_code();
void test_order_token_creator();

#include "tests.h"              /* Declarations of tests to be run */

testfunc tests[] = {
    test_resend,
    test_realloc_receive,
    test_token_up,
    test_last_logon_logic,
    test_make_long,
    test_time_t_convert,
    test_allocate_master,
    test_itoa,
    test_utp_order_ack,
    test_utp_time,
    test_utp_price_trans,
    test_trans_rom_symbol,
    test_ph_parser,
    testdatabuf_realloc,
    test_pop_queue,
    test_config_loader,
    test_modulus,
    test_month_codes,
    test_symbol_killer,
    test_symbol_mod,
    test_tm_to_str,
    test_create_fix_header,
    testsuffixmap,
    testdatabuf,
    testfixdatabuf,
    test_hash,
    test_comma_remove,
    test_db,
    test_db2,
    test_tpos_sym_trans,
    test_spread_parse,
    test_full_spread_parse,
    test_full_spread_cancel,
    test_spread_fr_open_mess,
    test_ftr_trans_new,
    test_ftr_trans_replace,
    test_ftr_trans_cnl,
    test_rtf_trans_fill,
    test_rtf_trans_open,
    test_rtf_trans_out,
    test_rtf_trans_rpl,
    test_rtf_trans_rep_rej,
    test_rtf_trans_cnl_rej,
    test_small_bucket,
    test_contains_key,
    test_r2f_spread_parse,
    test_r2f_spread_cancel,
    test_reset_calculator,
    test_r2f_reg_option,
    test_r2f_reg_opt_cancel,
    test_r2f_spread_life_cycle,
    test_r2f_spread_life_cycle2,
    test_r2f_spread_life_cycle3,
    test_r2f_spread_life_cycle4,
    test_option_life_cycle,
    test_rb_insert,
    test_rb_multi_orders,
    test_cx_frame,
    test_ntohll_htonll,
    test_amount_trans,
    test_price_trans,
    test_rb_remove,
    test_cx_trade,
    test_cx_ii_reply,
    test_cx_order_ack,
    test_cx_order_ack_rej,
    test_cancel_rej,
    test_cx_order_replaced,
    test_cx_order_replace_rej,
    test_cx_order_replace_cnl,
    test_cx_order_canclled,
    test_date_roll_check,
    test_date_roll_check2,
    test_date_roll_check3,
    test_date_roll_check4,
    test_rb_tree_crush,
    test_rb_masher,
    test_cx_cross_hash,
    test_mult_thread_access,
    test_rom_pad_one,
    test_rom_pad_twenty_five,
    test_mail_message,
    test_options_db,
    test_queue,
    test_resend_request_handler,
    test_build_rom_pos,
    test_fixmsg_addstr,
    test_fixmsg_attach,
    test_next_pow_two
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

    //  test_db_code();
    //test_parse_functions_directly();
    //test_config_loader();
    //str_str_test();  
}



void fix_connection_callBack(con_obj * co, int is_connected)
{
    printf("Info about this FIX Connection: %s, Is it connected {%d} ? \n",
           get_name(co), is_connected);
    if (is_connected) {
    } else {
        //remove_fix_con(co);
    }
    //

}


void test_lac(con_obj * co)
{
    printf("Low activity reported on this connection: %s \n",
           get_name(co));
}

void con_callBack(con_obj * co, int is_connected)
{
    printf("Info about this Connection: %s, Is it connected {%d} ? \n",
           get_name(co), is_connected);
    //is_con_now = is_connected;
}

void test_order_token_creator()
{
    token_struct_t *t1 = init_order_token_creator();
    token_struct_t *t2;
    int ret_len = 0;
    int buffer_len = 500;
    char *buffer = calloc(buffer_len + 1, 1);
    char *buffer2 = calloc(buffer_len + 1, 1);
    int i = 0;
    for (; i < 1000000; ++i) {
        ret_len = create_order_token(t1, buffer, buffer_len);
        if (ret_len > buffer_len) {
            printf("SPRINTF OVERRAN BUFFER!!!!! %d, %d \n", ret_len,
                   buffer_len);
        } else {
            if (strcmp(buffer, buffer2) == 0) {
                printf("Failed to increment Counter!!!!! %s, %s \n",
                       buffer, buffer2);
            }
        }
        memcpy(buffer2, buffer, buffer_len);
    }
    t2 = init_order_token_creator();
    ret_len = create_order_token(t2, buffer, buffer_len);
    if (ret_len < buffer_len) {
        printf("Restart buffer = %s, last buff: %s \n", buffer, buffer2);
    } else {
        printf("OVERRAN SECOND BUFFER!!!! %d \n", ret_len);
    }
}

/*static hashmap_t create_fix_parser()
{
    hashmap_t test = create_map(20);
    insert_map(test, "11", 2, "3", 1);
    insert_map(test, "150", 3, "61", 2);
    insert_map(test, "44", 2, "7", 1);
    insert_map(test, "38", 2, "6", 1);
    return test;
} */


void test_parse_func()
{
    struct message m;
    //char *output = calloc(600, 1);
    //int out_len = 600;
    //int rejected = 1;
    //int total_len = 0;
    char *fix_m =
        "35=8128=DVG34=14823849=CCG56=DRT_DART05M52=20100909-13:32:2855=ITG37=XRU 8134/0909201011=XRU 8134/0909201017=XRU 8134/09092010 00100100120=039=1150=154=238=20040=244=14.110059=031=14.160032=10014=06=0151=10060=20100909-13:32:2858=Partial Fill1=7YVK120930=N76=0055207=N47=P9426=T9483=0000389570=T382=1375=LOC337=0000437=100438=09329579=00001000019433=005529=463=09440=00100100110=212";
    m.name = "Dodo";
    m.name_len = 4;
    m.data = fix_m;
    m.len = strlen(fix_m);

    //initalize_maps();
    startup_allocator(50000);
}

/*
void test_smtp()
{
    char *mail_server = NULL;
    char *e_mail = NULL;
    int m_server_len = 0;
    int e_mail_len = 0;
    int ret_val = 0;
    struct message m;
    char *message = "Hey Fatso it's time to get to work!";
    databuf_t *output = databuf_alloc(500);
    char rom_type = 8;
    int i = 0;
    hashmap_t trans_map;
    struct dart_rom_pos l_positions[ROM_LEN];
    char *title = "Message for you loser!";
    char *fix_m =
        "8128=DIO34=4450149=CCG56=DRT_DART0152=20100831-19:58:381=RONIN44=0.95000011=BI 2971/0831201038=10040=254=155=MOS37=BI 2971/0831201032=031=014=06=020=039=858=Unable to determine target connection60=20100831-19:58:38150=8151=059=047=P207=N17=010=118";
    m.name = "Dodo";
    m.name_len = 4;
    m.data = fix_m;
    m.len = strlen(fix_m);

    mail_server = get_val_for_tag("MAIL", 4, "Server", 6, &m_server_len);
    e_mail = get_val_for_tag("MAIL", 4, "Recipients", 10, &e_mail_len);

    ret_val =
        start_dart_mail_service(mail_server, m_server_len, e_mail,
                                e_mail_len, e_mail, e_mail_len, 23, 23,7);
    ret_val =
        send_tech_message(message, strlen(message), title, strlen(title));
    for (; i < ROM_LEN; ++i) {
        l_positions[i].beg = 0;
        l_positions[i].end = 0;
    }

    trans_map = create_fix_parser();    //get_map_for_type('8', &rom_type);
    build_dart_rom_positions(l_positions, fix_m, m.len, trans_map, 0);
    ret_val =
        send_alert_for_message(&m, fix_m, output, m.len,  rom_type,
                               l_positions);
    sleep(500);
}
*/

void parse_fix_callback(struct message *mess)
{
    printf("Got da message: Name = {%s} message = {%s}\n", mess->name,
           mess->data);
}

void parse_fix_record_mess(struct incoming_message *mess)
{
}

int parse_fix_send_ptr(struct message *mess)
{
    return 0;
}

int parse_fix_init_con(con_obj * co)
{
    return 0;
}

void parse_fix_remove_con(con_obj * co)
{
}

/*
void test_config_loader()
{
    int ret_val;
    char *sender_comp;
    char *send_tag = "Sender Comp ID";
    int send_len = 14;
    char *target_comp;
    char *target_tag = "Target Comp ID";
    int targ_len = 14;
    int ret_len;
    char *fix_port;
    char *rom_user_name;
    char *rom_con_name;
    char *rom_port;
    char *rom_host;
    char *end_hour;
    char *end_min;
    char *fix_port1;
    char *rom_user_name1;
    char *rom_con_name1;
    char *rom_port1;
    char *rom_host1;
    char *end_hour1;
    char *end_min1;
    ret_val = initialize_config("TestFixConfig");
    sender_comp =
        get_val_for_tag("FIX SERVER", 10, send_tag, send_len, &ret_len);
    target_comp =
        get_val_for_tag("FIX SERVER", 10, target_tag, targ_len, &ret_len);

    printf("Sender comp: {%s} target comp: {%s}\n", sender_comp,
           target_comp);

    fix_port = get_val_for_tag("FIX SERVER", 10, "Fix Port", 8, &ret_len);
    rom_user_name =
        get_val_for_tag("ROM SERVER", 10, "Rom User Name", 13, &ret_len);
    rom_con_name =
        get_val_for_tag("ROM SERVER", 10, "Rom Connection Name", 19,
                        &ret_len);
    rom_port = get_val_for_tag("ROM SERVER", 10, "Rom Port", 8, &ret_len);
    rom_host = get_val_for_tag("ROM SERVER", 10, "Rom Host", 8, &ret_len);
    printf
        ("Fix_port: {%s} RomUserName: {%s} \n RomConName: {%s} Rom Port: {%s} Rom Host: {%s}\n",
         fix_port, rom_user_name, rom_con_name, rom_port, rom_host);
    end_hour = get_val_for_tag("TIMES", 5, "End Hour", 8, &ret_len);
    end_min = get_val_for_tag("TIMES", 5, "End Min", 7, &ret_len);
    printf("End Hour: {%s} End Min: {%s}\n", end_hour, end_min);

    set_val_for_tag("TestFixConfig", "TIMES", 5, "Last Run Day", 12,
                    "20100322", 8);

    dart_reset_maps();

    dart_load_map("TestFixConfig");

    printf
        ("*******************************AFTER THE RESET*********************************************\n");

    sender_comp =
        get_val_for_tag("FIX SERVER", 10, send_tag, send_len, &ret_len);
    target_comp =
        get_val_for_tag("FIX SERVER", 10, target_tag, targ_len, &ret_len);

    printf("Sender comp: {%s} target comp: {%s}\n", sender_comp,
           target_comp);

    fix_port1 = get_val_for_tag("FIX SERVER", 10, "Fix Port", 8, &ret_len);
    rom_user_name1 =
        get_val_for_tag("ROM SERVER", 10, "Rom User Name", 13, &ret_len);
    rom_con_name1 =
        get_val_for_tag("ROM SERVER", 10, "Rom Connection Name", 19,
                        &ret_len);
    rom_port1 = get_val_for_tag("ROM SERVER", 10, "Rom Port", 8, &ret_len);
    rom_host1 = get_val_for_tag("ROM SERVER", 10, "Rom Host", 8, &ret_len);
    printf
        ("Fix_port: {%s} RomUserName: {%s} \n RomConName: {%s} Rom Port: {%s} Rom Host: {%s}\n",
         fix_port1, rom_user_name1, rom_con_name1, rom_port1, rom_host1);
    end_hour1 = get_val_for_tag("TIMES", 5, "End Hour", 8, &ret_len);
    end_min1 = get_val_for_tag("TIMES", 5, "End Min", 7, &ret_len);
    printf("End Hour: {%s} End Min: {%s}\n", end_hour1, end_min1);

    set_val_for_tag("TestFixConfig", "TIMES", 5, "Last Run Day", 12,
                    "20100322", 8);


}
*/

/*
void test_parse_fix() {
  struct rec_init* rec;
  rec = malloc(sizeof(struct rec_init));
  //rec->read_messages = (*read_fn);
  rec->end_hour = 23;
  rec->end_min = 22;
  rec->create_new = 0;
  start_recorder(rec);
  struct init_args* in_args;
  in_args = malloc(sizeof(struct init_args));
  in_args->name = "PARNELL";
  in_args->name_len = 7;
  in_args->sender_comp = "ROMFIX";
  in_args->sc_len = 6;
  in_args->target_comp = "PARNELL";
  in_args->tc_len = 7;
  in_args->target_sub = NULL;
  in_args->ts_len = 0;
  in_args->sender_sub = NULL;
  in_args->ss_len = 0;
  in_args->username = "PARNELL";
  in_args->un_len = 7;
  in_args->password = "PARNELL";
  in_args->p_len = 7;
  struct seq_reset_time* rt;
  rt = malloc(sizeof(struct seq_reset_time));
  rt->reset_hour = 10;
  rt->reset_min = 10;
  rt->create_new = 1;
  in_args->reset_time = rt;

  struct parse_con* pcon;
  pcon = malloc(sizeof(struct parse_con));
  pcon->send_ptr = parse_fix_send_ptr;
  pcon->callback = (*parse_fix_callback);
  pcon->record_mess = &parse_fix_record_mess;
  pcon->init_con = &parse_fix_init_con;
  pcon->remove_con = &parse_fix_remove_con;
  //  pcon->reset_time->create_new = 1;
  pcon->in_args = in_args;
  con_obj* co;
  co = malloc(sizeof (con_obj));
  co->name = "PARNELL";
  co->name_len = 7;
  co->in_seq = 1;
  co->out_seq = 1;
  pcon->co = co;


  int cut_con = 0;
  unsigned long ret_val = 0;
  char* fix_mess = "8=FIX.4.29=20235=D34=8549=FIXWANG50=WANGJ52=20100226-15:00:10.57456=ROMFIX1=WANGJ 501 7YTC1209 GSEC WANGJ11=090010_0_021=138=10040=244=42.4847=A54=155=BOH59=060=20100226-15:00:10.509167=CS10001=14410=0908=FIX.4.29=20135=D34=8649=FIXWANG50=WANGJ52=20100226-15:00:10.57956=ROMFIX1=WANGJ 501 7YTC1209 GSEC WANGJ11=090010_1_121=138=10040=244=19.547=A54=155=BXS59=060=20100226-15:00:10.579167=CS10001=14810=0778=FIX.4.29=20235=D34=8749=FIXWANG50=WANGJ52=20100226-15:00:10.58256=ROMFIX1=WANGJ 501 7YTC1209 GSEC WANGJ11=090010_2_221=138=10040=244=21.8547=A54=155=CBU59=060=20100226-15:00:10.582167=CS10001=14810=0998=FIX.4.29=21735=D34=8849=FIXWANG50=WANGJ52=20100226-15:00:10.58456=ROMFIX1=WANGJ 501 7YTC1209 GSEC WANGJ11=090010_3_321=138=10040=244=53.9647=A54=555=CFR59=060=20100226-15:00:10.584114=N167=CS5700=50110001=14810=0628=FIX.4.29=22835=D34=8949=FIXWANG50=WANGJ52=20100226-15:00:10.58656=ROMFIX1=WANGJ 501 7YTC1209 GSEC WANGJ11=090010_4_421=138=10040=244=20.6847=A54=555=COLB58=INET-O59=060=20100226-15:00:10.586114=N167=CS5700=50110001=14610=2188=FIX.4.29=21335=D34=9049=FIXWANG50=WANGJ52=20100226-15:00:10.58956=ROMFIX1=WANGJ 501 7YTC1209 GSEC WANGJ11=090010_5_521=138=10040=244=20.3947=A54=155=PACW58=INET-O59=060=20100226-15:00:10.588167=CS10001=14610=0228=FIX.4.29=22835=D34=9149=FIXWANG50=WANGJ52=20100226-15:00:10.59156=ROMFIX1=WANGJ 501 7YTC1209 GSEC WANGJ11=090010_6_621=138=10040=244=21.1647=A54=555=FMER58=INET-O59=060=20100226-15:00:10.591114=N167=CS5700=50110001=14610=2118=FIX.4.29=21235=D34=9249=FIXWANG50=WANGJ52=20100226-15:00:10.59356=ROMFIX1=WANGJ 501 7YTC1209 GSEC WANGJ11=090010_7_721=138=10040=244=14.247=A54=155=GBCI58=INET-O59=060=20100226-15:00:10.593167=CS10001=14610=1978=FIX.4.29=22835=D34=9349=FIXWANG50=WANGJ52=20100226-15:00:10.59556=ROMFIX1=WANGJ 501 7YTC1209 GSEC WANGJ11=090010_8_821=138=10040=244=13.3947=A54=555=HCBK58=INET-O59=060=20100226-15:00:10.595114=N167=CS5700=50110001=14610=2138=FIX.4.29=22835=D34=9449=FIXWANG50=WANGJ52=20100226-15:00:10.59756=ROMFIX1=WANGJ 501 7YTC1209 GSEC WANGJ11=090010_9_921=138=10040=244=57.8647=A54=555=IBKC58=INET-O59=060=20100226-15:00:10.596114=N167=CS5700=50110001=14610=2308=FIX.4.29=21535=D34=9549=FIXWANG50=WANGJ52=20100226-15:00:10.59856=ROMFIX1=WANGJ 501 7YTC1209 GSEC WANGJ11=090010_10_1021=138=10040=244=20.5647=A54=155=MBFI58=INET-O59=060=20100226-15:00:10.598167=CS10001=14610=1048=FIX.4.29=20435=D34=9649=FIXWANG50=WANGJ52=20100226-15:00:10.60056=ROMFIX1=WANGJ 501 7YTC1209 GSEC WANGJ11=090010_11_1121=138=10040=244=15.4147=A54=155=NYB59=060=20100226-15:00:10.600167=CS10001=14410=185";
  unsigned long len;
  char* len_tester = "35=D34=8549=FIXWANG50=WANGJ52=20100226-15:00:10.57456=ROMFIX1=WANGJ 501 7YTC1209 GSEC WANGJ11=090010_0_021=138=10040=244=42.4847=A54=155=BOH59=060=20100226-15:00:10.509167=CS10001=144";
  int len_len = strlen(len_tester);
  printf("This should be 202: {%d}", len_len);
  len = strlen(fix_mess);
  ret_val = parse_fix_message(fix_mess, len, &cut_con, pcon);
  printf("How many bytes did we use? %lu Of this many total: %lu \n", ret_val, len);

  char* fix_mess2 = "8=FIX.4.29=23635=G34=36349=FIXWANG50=WANGJ52=20100226-15:00:11.00856=ROMFIX1=WANGJ 501 7YTC1209 GSEC WANGJ11=090010_9_27821=138=10040=241=090010_9_944=57.8347=A54=555=IBKC58=Cxl_Rpl59=060=20100226-15:00:11.006114=N167=CS5700=50110=1578=FIX.4.29=22635=G34=36549=FIXWANG50=WANGJ52=20100226-15:00:11.41456=ROMFIX1=WANGJ 501 7YTC1209 GSEC WANGJ11=090010_137_28021=138=10040=241=090010_137_13744=34.2647=A54=155=MHP58=Cxl_Rpl59=060=20100226-15:00:11.414167=CS10=1918=FIX.4.29=24135=G34=37249=FIXWANG50=WANGJ52=20100226-15:00:11.68856=ROMFIX1=WANGJ 501 7YTC1209 GSEC WANGJ11=090010_171_28721=138=10040=241=090010_171_17144=60.5847=A54=555=OII58=Cxl_Rpl59=060=20100226-15:00:11.688114=N167=CS5700=50110=1558=FIX.4.29=18635=F34=731849=FIXWANG50=WANGJ52=20100226-15:15:09.30156=ROMFIX1=WANGJ 501 7YTC1209 GSEC WANGJ11=090010_8_722038=10041=090010_8_607754=555=HCBK58=Cxl60=20100226-15:15:09.30010=2128=FIX.4.29=18635=F34=731949=FIXWANG50=WANGJ52=20100226-15:15:09.30256=ROMFIX1=WANGJ 501 7YTC1209 GSEC WANGJ11=090010_13_72213";
  unsigned long len2;
  len2 = strlen(fix_mess2);
  ret_val = parse_fix_message(fix_mess2, len2, &cut_con, pcon);
  char* off;
  off = fix_mess2;
  off += ret_val;
  printf("Should be less How many bytes did we use? %lu Of this many total: %lu offset {%s}\n", ret_val, len2, off);

  char* fix_mess2_extra = "8=FIX.4.29=18635=F34=731949=FIXWANG50=WANGJ52=20100226-15:15:09.30256=ROMFIX1=WANGJ 501 7YTC1209 GSEC WANGJ11=090010_13_722138=10041=090010_13_711254=555=RY58=Cxl60=20100226-15:15:09.30210=187";
  len2 = strlen(fix_mess2_extra);
  ret_val = parse_fix_message(fix_mess2_extra, len2, &cut_con, pcon);
  printf("Last value: How many bytes did we use? %lu Of this many total: %lu \n", ret_val, len2);

  char* fix_mess3 = "8=FIX.4.29=20935=849=ROMFIX56=PARNELL34=1252=20100303-15:08:5211=IBM2010020309085200000154=155=IBM38=10044=1040=259=047=A439=05511=USER0001100=039=837=PARNELL-30100303-100000958=Can not find dest:0167=CS10=102";
  len2 = strlen(fix_mess3);
  char* test_fix_1 = "35=849=ROMFIX56=PARNELL34=1252=20100303-15:08:5211=IBM2010020309085200000154=155=IBM38=10044=1040=259=047=A439=05511=USER0001100=039=837=PARNELL-30100303-100000958=Can not find dest:0167=CS";
  int len3 = strlen(test_fix_1);
  printf("This should be 209: {%d}", len3);
  ret_val = parse_fix_message(fix_mess3, len2, &cut_con, pcon);
  printf("Our Fix Message: How many bytes did we use? %lu Of this many total: %lu \n", ret_val, len2);

  char* fix_mess4 = "8=FIX.4.29=31435=849=ROMFIX56=PARNELL34=352=20100304-16:03:3643=Y97=Y122=20100304-16:00:4711=IBM2010020410004700000154=155=IBM38=10044=1040=259=047=A439=05511=USER000139=8150=837=PARNELL-30100304-100000058=Trader:PARNELL is not allowed to trader for JOEL17=PARNELL-30100304-1000000-1167=CS14=0151=06=010=2398=FIX.4.29=9535=449=ROMFIX56=PARNELL34=452=20100304-16:03:3643=Y97=Y122=20100304-16:03:36123=Y36=510=048";
  len2 = strlen(fix_mess4);
  ret_val = parse_fix_message(fix_mess4, len2, &cut_con, pcon);
  printf("Our resend message How many bytes did we use? %lu out of %lu \n", ret_val, len2);
} 

struct init_args* test_load_rom_config() {
  struct init_args* in_args;
  in_args = malloc(sizeof(struct init_args));
  int ret_val;
  int ret_len;
  ret_len = 0;
  ret_val = initialize_config("TestFixConfig");
  in_args->name = get_val_for_tag("ROM SERVER", 10,"Rom Connection Name", 19, &ret_len);
  in_args->name_len = ret_len;
  in_args->sender_comp = get_val_for_tag("FIX SERVER", 10,"Sender Comp ID", 14, &ret_len);
  in_args->sc_len = ret_len;
  in_args->target_comp = get_val_for_tag("FIX SERVER", 10,"Target Comp ID", 14, &ret_len);
  in_args->tc_len = ret_len;
  in_args->target_sub = NULL;
  in_args->ts_len = 0;
  in_args->sender_sub = NULL;
  in_args->ss_len = 0;
  in_args->username = get_val_for_tag("ROM SERVER", 10,"Rom User Name", 13, &ret_len);
  in_args->un_len = ret_len;
  in_args->password = in_args->username;
  in_args-> p_len = in_args->un_len;

  struct seq_reset_time* rt;
  rt = malloc(sizeof(struct seq_reset_time));

  char* reset_h = get_val_for_tag("TIMES", 5,"Reset Hour", 10, &ret_len);
  if(reset_h != NULL) {
    rt->reset_hour = atoi(reset_h);
  } else {
    rt->reset_hour = 0;
  }
  char* reset_m = get_val_for_tag("TIMES", 5,"Reset Min", 9, &ret_len);
  if(reset_m != NULL) {
    rt->reset_min = atoi(reset_m);
  } else {
    rt->reset_min = 0;
  }
  char* last_run_date = get_val_for_tag("TIMES", 5,"Last Run Day", 12, & ret_len);

  time_t rawtime;
  struct tm* ti;
  time(&rawtime);
  ti = localtime(&rawtime);
  char* now = calloc(9,1);
  int year = 1900 + ti->tm_year;
  int month = 1 + ti->tm_mon;
  int len = sprintf(now, "%04d%02d%02d", year, month, ti->tm_mday);
  if(strncmp(now, last_run_date, 8) == 0 && len <= 8) {
    rt->create_new = 0;
  } else {
    rt->create_new = 1;
  }
  in_args->reset_time = rt;
  set_val_for_tag("TestFixConfig","TIMES", 5, "Last Run Day", 12, now, 8);
  return in_args;
}
void test_config_loader() {
  int ret_val;
  ret_val = initialize_config("TestFixConfig");
  char* sender_comp;
  char* send_tag = "Sender Comp ID";
  int send_len = 14;
  char* target_comp;
  char* target_tag = "Target Comp ID";
  int targ_len = 14;
  int ret_len;
  sender_comp = get_val_for_tag("FIX SERVER", 10, send_tag, send_len, &ret_len);
  target_comp = get_val_for_tag("FIX SERVER", 10, target_tag, targ_len, &ret_len);

  printf("Sender comp: {%s} target comp: {%s}\n", sender_comp, target_comp);

  char* fix_port;
  fix_port = get_val_for_tag("FIX SERVER", 10, "Fix Port", 8, &ret_len);
  char* rom_user_name;
  rom_user_name = get_val_for_tag("ROM SERVER",10, "Rom User Name",13, &ret_len);
  char* rom_con_name;
  rom_con_name = get_val_for_tag("ROM SERVER", 10,"Rom Connection Name", 19, &ret_len);
  char* rom_port;
  rom_port = get_val_for_tag("ROM SERVER", 10, "Rom Port", 8, &ret_len);
  char* rom_host;
  rom_host = get_val_for_tag("ROM SERVER", 10, "Rom Host", 8, &ret_len);
  printf("Fix_port: {%s} RomUserName: {%s} \n RomConName: {%s} Rom Port: {%s} Rom Host: {%s}\n", fix_port, rom_user_name, rom_con_name, rom_port, rom_host);
  char* end_hour;
  end_hour = get_val_for_tag("TIMES",5,"End Hour", 8, &ret_len);
  char* end_min;
  end_min = get_val_for_tag("TIMES", 5,"End Min", 7, &ret_len);
  printf("End Hour: {%s} End Min: {%s}\n", end_hour, end_min);

  set_val_for_tag("TestFixConfig","TIMES", 5, "Last Run Day", 12, "20100322", 8);
}



int test_parse_rom() {
    struct rec_init* rec;
  rec = malloc(sizeof(struct rec_init));
  //rec->read_messages = (*read_fn);
  rec->end_hour = 23;
  rec->end_min = 22;
  rec->create_new = 0;
  start_recorder(rec);
  struct init_args* in_args;
  in_args = malloc(sizeof(struct init_args));
  in_args->name = "PARNELL";
  in_args->name_len = 7;
  in_args->sender_comp = "ROMFIX";
  in_args->sc_len = 6;
  in_args->target_comp = "PARNELL";
  in_args->tc_len = 7;
  in_args->target_sub = NULL;
  in_args->ts_len = 0;
  in_args->sender_sub = NULL;
  in_args->ss_len = 0;
  in_args->username = "PARNELL";
  in_args->un_len = 7;
  in_args->password = "PARNELL";
  in_args->p_len = 7;
  struct seq_reset_time* rt;
  rt = malloc(sizeof(struct seq_reset_time));
  rt->reset_hour = 10;
  rt->reset_min = 10;
  rt->create_new = 1;
  in_args->reset_time = rt;
  
  struct parse_con* pcon;
  pcon = malloc(sizeof(struct parse_con));
  pcon->send_ptr = parse_fix_send_ptr;
  pcon->callback = (*parse_fix_callback);
  pcon->record_mess = &parse_fix_record_mess;
  pcon->init_con = &parse_fix_init_con;
  pcon->remove_con = &parse_fix_remove_con;
  //  pcon->reset_time->create_new = 1;
  pcon->in_args = in_args;
  con_obj* co;
  co = malloc(sizeof (con_obj));
  co->name = "PARNELL";
  co->name_len = 7;
  co->in_seq = 1;
  co->out_seq = 1;
  pcon->co = co;

  //int cut_con = 0;
  //unsigned long ret_val = 0;

  //  char* execution = "S,151509,FIXWANG,091509_1849_7194,1,O,100,28.12000000,2,0,A,,7YTC1209,144,13,201002262568,L40000723251,FIXWANG-10100226-18121,,,,,,,,,,,FIXWANG-10100226-18121-1,,,,,,,E,,,,,,,0,,0,7YTC1209,,,0,100,20100226-15:15:09.279,0.000000,,,None,,,,,,,0,,,,,,8,,,,8896,,,,0,,,,,";
  // char* new_order = "\n";
  //char* cancel = "\n";
  //char* replace = "\n";
  return 0;
}
*/
