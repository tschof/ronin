#include "tests.h"
#include "testmacros.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "fix_trans.h"
#include "parser_func.h"
#include "transaction_recorder.h"
#include "dart_constants.h"
#include "hmac_helper.h"


char* data_to_encode_go = "abc123!?$*&()'-=@~";

char* data_encoded_go = "YWJjMTIzIT8kKiYoKSctPUB+";

static dart_order_obj *create_empty_doj()
{
    dart_order_obj *doj = calloc(1, sizeof(struct dart_order_obj));
    doj->positions = calloc(ROM_LEN, sizeof(struct iovec));
    doj->positions[161].iov_base = "\n";
    doj->positions[161].iov_len = 1;
    int rom_tag = 0;
        while (rom_tag < 160) {
            ++rom_tag;
            doj->positions[rom_tag].iov_base = ",";
            doj->positions[rom_tag].iov_len = 1;
            ++rom_tag;
        }
    doj->head = malloc(sizeof(db_node_t));
    char *block = calloc(800, 1);
    doj->head->data = malloc(sizeof(databuf_t));
    databuf_attach(doj->head->data, block, 800);
    doj->current = doj->head;
    doj->head->next = NULL;
    doj->head->start_ptr = doj->head->data->buffer;
    doj->head->end_ptr = doj->head->data->buffer + 800;
    doj->rep = NULL;
    doj->dest = NULL;
    return doj;
}
/**
 * Cancel, Replace, NewOrder, Heartbeat, TestReq, ResendReq, GapFill, Logon
typedef struct fix_file_desc {
    char* filename;
    int f_len;
    short fix_type;
} ff_desc;
 */
static ff_desc* create_ff_out_array(int* len, char* dir, int d_len)
{
    char* full_path = calloc(1, d_len + 9);
    ff_desc* ffd = calloc(16, sizeof(struct fix_file_desc));
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "NewOrder", 8);
    ffd[0].filename = full_path;
    full_path = calloc(1, d_len + 7);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "Cancel", 6);
    ffd[1].filename = full_path;
    full_path = calloc(1, d_len + 8);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "Replace", 7);
    ffd[2].filename = full_path;
    full_path = calloc(1, d_len + 8);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "GapFill", 7);
    ffd[3].filename = full_path;
    full_path = calloc(1, d_len + 6);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "Logon", 5);
    ffd[4].filename = full_path;
    full_path = calloc(1, d_len + 8);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "Logout", 6);
    ffd[5].filename = full_path;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "ResendReq", 9);
    ffd[6].filename = full_path;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "SecDefReq", 9);
    ffd[7].filename = full_path;
    full_path = calloc(1, d_len + 8);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "TestReq", 7);
    ffd[8].filename = full_path;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "TraderLogon", 11);
    ffd[9].filename = full_path;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "Heartbeat", 9);
    ffd[10].filename = full_path;
    *len = 11;
    return ffd;
}

static ff_desc* create_ff_in_array(int* len, char* dir, int d_len)
{
    char* full_path = calloc(1, d_len + 16);
    ff_desc* ffd = calloc(10, sizeof(struct fix_file_desc));
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomExe", 11);
    ffd[0].filename = full_path;
    ffd[0].fix_type = 0x38;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomCnlRej", 14);
    ffd[1].filename = full_path;
    ffd[1].fix_type = 0x39;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomNORej", 13);
    ffd[2].filename = full_path;
    ffd[2].fix_type = 0x44;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomLogon", 13);
    ffd[3].filename = full_path;
    ffd[3].fix_type = 0x41;
    *len = 4;
    return ffd;
}

int test_cme_logon(const char** testname, const char** extrareporting)
{
    STARTTEST;
    REQCOND(1 != 0);
    int in_len = 0;
    int out_len = 0;
    char* dir = "./";//"/home/fparnell/git_repo/Dart2/om/test/";
    int d_len = strlen(dir);
    ff_desc* in_arr = create_ff_in_array(&in_len, dir, d_len);
    ff_desc* out_arr = create_ff_out_array(&out_len, dir, d_len);
    trans_t* trans = create_translator(out_arr, out_len, in_arr, in_len);
    CHKCOND(trans != NULL); 
    char* sample = "0000001\nS61474N\n5\n20180312-18:48:39.802\nG\n30\nUS,IL\n0000000\nROM\n201\nRonin";
    int sample_len = strlen(sample);
    char* encode_output;
    encode_base64((uint8_t*)sample, sample_len, &encode_output);
    printf("Encoded FIX: %s \n", encode_output);
    int e2len = strlen(encode_output);
    char* e2_output;
    decode_base64((const uint8_t*)encode_output, e2len, &e2_output);
    printf("Decoded Out: %s \n", e2_output); 
    CHKCOND(strncmp(sample, e2_output, sample_len) == 0);

    /**/ 
    char* cmesample = "WjDLJekvdvfWMBV0g0Pu-V0sa-4kNtiEn8rbYopIIEI";
    int dlen = strlen(cmesample);
    char* decode_output;
    decode_base64((const uint8_t*)cmesample, dlen, &decode_output);
    printf("Decoded Out: %s \n", decode_output); 
    char* dsample = "SGVsbG8gV29ybGQ=";
    dlen = strlen(dsample);
    char* dsample_out;
    decode_base64((const uint8_t*)dsample, dlen, &dsample_out);
    printf("2nd Decoded Out: %s \n", dsample_out); 

    char* rival_msg_secret = "psrzN_vC06vPcn2iPV10xAu-1y4nQ9-ti38jgnxf_9o";
    int rms_len = strlen(rival_msg_secret);
    char* rival_con_str = "0014170\nLVM8NWN\nrvaleroso2\n20180413-21:02:10.805\n52\n30\nUS,IL\n0000000\nRival\n8.0\nRival";

    char* dkey;
    int keylen = decode_base64((const uint8_t*)rival_msg_secret,
            rms_len, &dkey);
    unsigned char md[2048];
    uint32_t md_len = 0;
    unsigned char* result = dart_hmac_sha256(dkey, keylen,
        (uint8_t*)rival_con_str, strlen(rival_con_str), md,
        &md_len);
    if(result > 0) {
    char* dd = "gDEqUvVnaVrww9pu-0ds-7Z8-MfACqCl4LMDOQqFzKY";
    int ddlen = strlen(dd);
    char* buffer;
    int elen = encode_base64(md, md_len, &buffer); 
    CHKCOND(strncmp("gDEqUvVnaVrww9pu-0ds-7Z8-MfACqCl4LMDOQqFzKY", buffer, elen) == 0);
    printf("RIVAL TEST: Our val: {%s}\n len: %d, %d \n", buffer, elen, ddlen);
    }
    STOPTEST;
}
static char* sec_req = "8=FIX.4.29=008835=c49=ICE56=116857=534=777777752=000000000000000000000320=048=0321=100167=OPT10=015";




static char dig1[] = "0123456789abcdefghijklmnopqrstuvwxyz";
static int calc_check_sumtest(const char* begin, int len)
{
    int sum = 0;
    int i;
    const char *c;
    c = begin;
    for (i = 0; i < len; ++i) {
        sum = (sum + *c);
        ++c;
    }
    sum = sum % 256;
    return sum;
}
int test_build_fix_trans(const char** testname, const char** extrareporting)
{
    STARTTEST;
    REQCOND(1 != 0);
    int in_len = 0;
    int out_len = 0;
    char* dir = "./";//"/home/fparnell/git_repo/Dart2/om/test/";
    int d_len = strlen(dir);
    ff_desc* in_arr = create_ff_in_array(&in_len, dir, d_len);
    ff_desc* out_arr = create_ff_out_array(&out_len, dir, d_len);
    trans_t* trans = create_translator(out_arr, out_len, in_arr, in_len);
    CHKCOND(trans != NULL); 
    ofp* doj = get_fixy_message(trans, 0x44);
    CHKCOND(doj != NULL);
    set_fix_val(trans, doj, 50, "SCHMUCK", 7);
    
    set_num_val(trans, doj, 34, 9);
    char* lcomper = "8=FIX.4.29=000035=D49=ICE56=116857=534=000000952=000000000000000000000142=US,IL115=DART|DART116=DART144=DAU|ROMCHI50=SCHMUCK";
    CHKCOND(strncmp(lcomper, get_message(doj)->buffer, 126) == 0);
    reset_object(trans, doj);
    char* lcomper2 = "8=FIX.4.29=000035=D49=ICE56=116857=534=000000052=000000000000000000000142=US,IL115=DART|DART116=DART144=DAU|ROMCHI";
    CHKCOND(strncmp(lcomper2, get_message(doj)->buffer, 116) == 0);

    ofp* local_hb = get_fixy_message(trans, 0x63);
    set_num_val(trans, local_hb, 320, 0);
    set_num_val(trans, local_hb, 48, 0);
    set_fix_val(trans, local_hb, 321, "100", 3);
    set_fix_val(trans, local_hb, 167, "OPT", 3);
    set_num_val(trans, local_hb, 34, 7777777);
    set_tag_nine(trans, local_hb);                      
    set_check_sum(trans, local_hb);
    char output[500];
    memset(output, '\0', 500);
    databuf_t* b = get_message(local_hb);
    memcpy(output, b->rd_ptr, databuf_unread(b));
    
    CHKCOND(strncmp(sec_req, output, strlen(sec_req)) == 0);
    printf("Message: %s \n", output);

    char* bad_message = "8=FIX.4.29=78335=i34=226196949=4ND7Y0N52=20180725-19:34:14.090369=000000056=CME57=54142=US,IL50=FZL11028=N204=07928=81173169702=2296=1302=1304=6295=6299=153248603255=1V107=UD:1V: VT 0724949082167=MLEG132=25.0000000134=100133=50.0000000135=100299=153248603355=1V107=UD:1V: VT 0724949085167=MLEG132=5.0000000134=100133=25.0000000135=100299=153248603455=1V107=UD:1V: VT 0724949086167=MLEG132=5.0000000134=100133=25.0000000135=100299=153248603555=1V107=UD:1V: VT 0724949091167=MLEG132=60.0000000134=100133=85.0000000135=100299=153248603655=1V107=UD:1V: VT 0724949092167=MLEG132=5.0000000134=100133=25.0000000135=100299=153248603755=1V107=UD:1V: VT 0724949093167=MLEG132=305.0000000134=100133=325.0000000135=1009771=36372117=3065690737";
    int sum = calc_check_sumtest(bad_message, strlen(bad_message));
    char* checker = calloc(1, 6);
    long n = 0;
    unsigned long v = 0;
    char str[4];
    char *p;
    v = sum;
    do {
        str[n++] = dig1[v % 10];
        v /= 10;
    } while (v);
    p = checker - n;
    do {
        *p++ = str[n - 1];
        --n;
    } while(n);
  printf("BAD CHECKSUM: %s \n\n", checker); 

    STOPTEST;
}
static char* exe  = "8=FIX.4.29=26935=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=157=7YSC147811=344029965041=344029965037=2305217=-5935920=0150=439=4109=DEGS55=WOOD54=138=10059=332=031=0.0151=014=06=0.018=N1=7YSC120944=46.460040=247=A29=176=INET58=T10=232";

int test_fix_rom(const char** testname, const char** extrareporting)
{
    STARTTEST;
    dart_order_obj* doj = create_empty_doj();
    int in_len = 0;
    int out_len = 0;
    char* dir =  "./";//"/home/fparnell/git_repo/Dart2/om/test/";
    int d_len = strlen(dir);
    ff_desc* in_arr = create_ff_in_array(&in_len, dir, d_len);
    ff_desc* out_arr = create_ff_out_array(&out_len, dir, d_len);
    trans_t* trans = create_translator(out_arr, out_len, in_arr, in_len);
    REQCOND(doj != 0);
    int len = strlen(exe);
    translate_fix_msg(trans, exe, len, 
                       doj, 0x38);
    CHKCOND(doj->positions[8].iov_len == 1);
    CHKCOND(strncmp(doj->positions[8].iov_base, "2", 1) == 0);
    CHKCOND(doj->positions[15].iov_len == 10);
    CHKCOND(strncmp(doj->positions[15].iov_base, "3440299650", 10) == 0);
    dart_order_obj* obj = create_empty_doj();
    translate_fix_msg(trans, exe, len, 
                       obj, 0x41);
    CHKCOND(obj->positions[4].iov_len == 4);

    STOPTEST;
}

int test_realloc_ofp(const char** testname, const char** extrareporting)
{
    STARTTEST;
    REQCOND(1 != 0);
    char* dir =  "./test_config/";//"/home/fparnell/git_repo/Dart2/om/test/";
    int d_len = strlen(dir);
    int in_len = 0;
    int out_len = 0;
    char sbuff [20];
    int sbuff_len = 0;
    ff_desc* in_arr = create_ff_in_array(&in_len, dir, d_len);
    ff_desc* out_arr = create_ff_out_array(&out_len, dir, d_len);
    trans_t* trans = create_translator(out_arr, out_len, in_arr, in_len);
    ofp* doj = get_fixy_message(trans, 0x63);
    unsigned int i = 0;
    for(;i < 500; ++i) {
        safe_num_val(trans, doj, 320, i);
        if(i <= 146) {
            CHKCOND(get_message(doj)->buflen == 3278);
        }
        safe_num_val(trans, doj, 321, i+3);
        if(i == 146) {
            CHKCOND(get_message(doj)->buflen == 6557);
        }
        sbuff_len = sprintf(sbuff, "%d", i +5);
        safe_val_add(trans, doj, "322", 3, sbuff, sbuff_len);
    }
    CHKCOND(get_message(doj)->buflen == 13114);
    set_num_val(trans, doj, 34, 2234);
    CHKCOND(strncmp("8=FIX.4.29=0000035=c49=116856=ICE57=534=0002234", get_message(doj)->rd_ptr,54) == 0);
    set_tag_nine(trans, doj);                      
    CHKCOND(strncmp("8=FIX.4.29=1174735", get_message(doj)->rd_ptr,19) == 0);
    set_check_sum(trans, doj);
    CHKCOND(strncmp("10=184",get_message(doj)->wr_ptr- 7 , 7) == 0);
    reset_object(trans, doj);
    CHKCOND(strncmp("8=FIX.4.29=0000035=c49=116856=ICE57=534=0000000", get_message(doj)->rd_ptr,54) == 0);
    STOPTEST;
}

int test_check_sum(const char** testname, const char** extrareporting)
{
    STARTTEST;
    REQCOND(1 != 0);
    char* dir =  "./";//"/home/fparnell/git_repo/Dart2/om/test/";
    int d_len = strlen(dir);
    int in_len = 0;
    int out_len = 0;
    ff_desc* in_arr = create_ff_in_array(&in_len, dir, d_len);
    ff_desc* out_arr = create_ff_out_array(&out_len, dir, d_len);
    trans_t* trans = create_translator(out_arr, out_len, in_arr, in_len);
    ofp* doj = get_fixy_message(trans, 0x63);
    databuf_t* b = get_message(doj);
    databuf_reset(b);     
    char* sec_one = "8=FIX.4.29=008835=c49=116856=ICE34=000002250=552=20141209-01:35:01.773320=948=9321=100167=OPT10=042";
    int sec_len = strlen(sec_one) - 7;
    databuf_memcpy(get_message(doj), sec_one, sec_len);
    set_check_sum(trans, doj);

    CHKCOND(strncmp(sec_one, b->rd_ptr, databuf_unread(b)) == 0);
    reset_object(trans, doj);
    databuf_reset(b);     
    char* hb_one = "8=FIX.4.29=6135=049=RONIN56=ARCA57=ARCA34=2452=20141209-13:16:02.00010=084";
    sec_len = strlen(hb_one) - 7;
    databuf_memcpy(get_message(doj), hb_one, sec_len);
    set_check_sum(trans, doj);
    CHKCOND(strncmp(hb_one, b->rd_ptr, databuf_unread(b)) == 0);
    reset_object(trans, doj);
    databuf_reset(b);     
    char* trade_log = "8=FIX.4.29=010235=CG49=116856=ICE50=534=000000252=20141209-01:35:01.773109=1168553=dt_fix_trader96=Starts12310=197";
    sec_len = strlen(trade_log) - 7;
    databuf_memcpy(get_message(doj), trade_log, sec_len);
    set_check_sum(trans, doj);
    CHKCOND(strncmp(trade_log, b->rd_ptr, databuf_unread(b)) == 0);
    //printf("Message: %s \n", b->buffer);
    STOPTEST;
}
char* comper = "8=FIX.4.29=021035=D49=ICE56=116857=534=0002234";
char* comper2 = "8=FIX.4.29=021035=D49=ICE56=116857=534=0002235";
char* comper3 = "8=FIX.4.29=021735=D49=TPF56=116857=534=9342235";



int test_time_checksum(const char** testname, const char** extrareporting)
{
    STARTTEST
    int in_len = 0;
    REQCOND(in_len == 0);
    int out_len = 0;
    char* dir =  "./";//"/home/fparnell/git_repo/Dart2/om/test/";
    int d_len = strlen(dir);
    ff_desc* in_arr = create_ff_in_array(&in_len, dir, d_len);
    ff_desc* out_arr = create_ff_out_array(&out_len, dir, d_len);
    trans_t* t = create_translator(out_arr, out_len, in_arr, in_len);
    ofp* doj = get_fixy_message(t, 0x44);
	time_t fix_now;
    struct tm tmval;
    time(&fix_now);
    gmtime_r(&fix_now, &tmval);
    unsigned int seqnum = 3456789;
    set_num_val(t, doj, 11, seqnum);
    set_fix_val(t, doj, 1, "RONIN", 5); 
    set_fix_val(t, doj, 44, "3.1415926", 9); 
    set_fix_val(t, doj, 40, "2", 1); 
    set_fix_val(t, doj, 54, "2", 1); 
    set_fix_val(t, doj, 59, "0", 1); 
    char symbol[3][5] = { "ESZ3", "ESH4", "ESM4" };
    set_fix_val(t, doj, 107, symbol[seqnum % 3], 4);
    set_fix_val(t, doj, 55, "ES", 2);
    set_fix_val(t, doj, 167, "FUT", 3);
    set_time_tag(t, doj, &tmval, 60);
    set_num_val(t, doj, 38, (1+seqnum%10));
    set_time_tag(t, doj,&tmval, 52); 
    set_num_val(t, doj, 34, 2234);
    set_tag_nine(t, doj);                      
    set_check_sum(t, doj);
    char output[500];
    memset(output, '\0', 500);
    databuf_t* b = get_message(doj);
    memcpy(output, b->rd_ptr, databuf_unread(b));
    
    CHKCOND(strncmp(comper, output, strlen(comper)) == 0);
    reset_object(t, doj);

    /*
     * TEST for multiple messages.
     *
     *
     */
    ofp* doj2 = get_fixy_message(t, 0x44);
    ofp* doj3 = get_fixy_message(t, 0x44);
    /*
     *
     *
     *
     * DONE with multi message test.
     */
    int i = 0;
    for(; i < 200; ++i) {
        if(i%2) {
            set_num_val(t, doj2, 34, 2235);
            set_num_val(t, doj2, 11, seqnum);
            set_fix_val(t, doj2, 1, "RONIN", 5); 
            set_fix_val(t, doj2, 44, "3.1415926", 9); 
            set_fix_val(t, doj2, 40, "2", 1); 
            set_fix_val(t, doj2, 54, "2", 1); 
            set_fix_val(t, doj2, 59, "0", 1); 
            set_fix_val(t, doj2, 107, symbol[seqnum % 3], 4);
            set_fix_val(t, doj2, 55, "ES", 2);
            set_fix_val(t, doj2, 167, "FUT", 3);
            set_time_tag(t, doj2, &tmval, 60);
            set_num_val(t, doj2, 38, (1+seqnum%10));
            set_time_tag(t, doj2,&tmval, 52); 
            set_tag_nine(t, doj2);                      
            set_check_sum(t, doj2);
            memset(output, '\0', 500);
            databuf_t* b = get_message(doj2);
            memcpy(output, b->rd_ptr, databuf_unread(b));
            CHKCOND(strncmp(comper2, output, strlen(comper2)) == 0);
            reset_object(t, doj2);
        } else {
            set_num_val(t, doj3, 11, seqnum);
            set_fix_val(t, doj3, 1, "RONIN", 5); 
            set_fix_val(t, doj3, 44, "3.1415926", 9); 
            set_fix_val(t, doj3, 40, "2", 1); 
            set_fix_val(t, doj3, 54, "2", 1); 
            set_fix_val(t, doj3, 59, "0", 1); 
            set_fix_val(t, doj3, 107, symbol[seqnum % 3], 4);
            set_fix_val(t, doj3, 55, "ES", 2);
            set_fix_val(t, doj3, 167, "FUT", 3);
            set_time_tag(t, doj3, &tmval, 60);
            set_num_val(t, doj3, 38, (1+seqnum%10));
            set_time_tag(t, doj3,&tmval, 52); 
            set_num_val(t, doj3, 34, 2234);
            set_tag_nine(t, doj3);                      
            set_check_sum(t, doj3);
            memset(output, '\0', 500);
            databuf_t* b = get_message(doj3);
            memcpy(output, b->rd_ptr, databuf_unread(b));
            CHKCOND(strncmp(comper, output, strlen(comper)) == 0);
            reset_object(t, doj3);
        }
    }


    set_num_val(t, doj, 11, seqnum);
    //set_fix_val(t, doj, 58, i_and_a, len);
    set_fix_val(t, doj, 1, "RONIN", 5); 
    set_fix_val(t, doj, 44, "3.1415926", 9); 
    set_fix_val(t, doj, 40, "2", 1); 
    set_fix_val(t, doj, 54, "2", 1); 
    set_fix_val(t, doj, 59, "0", 1); 
    set_fix_val(t, doj, 107, symbol[seqnum % 3], 4);
    set_fix_val(t, doj, 55, "ES", 2);
    set_fix_val(t, doj, 167, "FUT", 3);
    int worked = overwrite_fix_val(t, doj, 49, "TPF", 3, 0);
    CHKCOND(worked == 1);
    worked = overwrite_fix_val(t, doj, 49, "TPF", 6, 0);
    CHKCOND(worked == -1);
    worked = overwrite_fix_val(t, doj, 58, "TPF", 3, 0);
    CHKCOND(worked == 0);
    set_time_tag(t, doj, &tmval, 60);
    set_num_val(t, doj, 38, (1+seqnum%10));
    set_time_tag(t, doj,&tmval, 52); 
    set_num_val(t, doj, 34, 9342235);
    set_tag_nine(t, doj);                      
    b = get_message(doj);
    CHKCOND(strncmp(comper3, b->rd_ptr, strlen(comper3)) == 0);

    STOPTEST;
}
