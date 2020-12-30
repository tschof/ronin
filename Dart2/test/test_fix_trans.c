#include "tests.h"
#include "testmacros.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "fix_trans.h"
#include "parser_func.h"
#include "rex_constants.h"



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
static char* sec_req = "8=FIX.4.29=0008835=c49=116856=ICE57=534=077777752=000000000000000000000320=048=0321=100167=OPT10=056";
int test_build_fix_trans(const char** testname, const char** extrareporting)
{
    STARTTEST;
    REQCOND(1 != 0);
    int in_len = 0;
    int out_len = 0;
    char* dir = "./test_config/";//"/home/fparnell/git_repo/Dart2/om/test/";
    int d_len = strlen(dir);
    ff_desc* in_arr = create_ff_in_array(&in_len, dir, d_len);
    ff_desc* out_arr = create_ff_out_array(&out_len, dir, d_len);
    trans_t* trans = create_translator(out_arr, out_len, in_arr, in_len);
    CHKCOND(trans != NULL); 
    ofp* doj = get_fixy_message(trans, 0x44);
    CHKCOND(doj != NULL);
    set_fix_val(doj, 50, "SCHMUCK", 7);
    
    set_num_val(doj, 34, 9);
    char* lcomper = "8=FIX.4.29=000035=D49=ICE56=116857=534=000000952=000000000000000000000115=DART|DART116=DART144=DAU|ROMCHI50=SCHMUCK";
    CHKCOND(strncmp(lcomper, get_message(doj)->buffer, 126) == 0);
    reset_object(trans, doj);
    char* lcomper2 = "8=FIX.4.29=000035=D49=ICE56=116857=534=000000052=000000000000000000000115=DART|DART116=DART144=DAU|ROMCHI";
    CHKCOND(strncmp(lcomper2, get_message(doj)->buffer, 116) == 0);

    ofp* local_hb = get_fixy_message(trans, 0x63);
    set_num_val(local_hb, 320, 0);
    set_num_val(local_hb, 48, 0);
    set_fix_val(local_hb, 321, "100", 3);
    set_fix_val(local_hb, 167, "OPT", 3);
    set_num_val(local_hb, 34, 777777);
    set_tag_nine(local_hb);                      
    set_check_sum(local_hb);
    char output[500];
    memset(output, '\0', 500);
    databuf_t* b = get_message(local_hb);
    memcpy(output, b->rd_ptr, databuf_unread(b));
    
    CHKCOND(strncmp(sec_req, output, strlen(sec_req)) == 0);
    printf("Message: %s \n", output);
    STOPTEST;
}
static char* exe  = "8=FIX.4.29=26935=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=157=7YSC147811=344029965041=344029965037=2305217=-5935920=0150=439=4109=DEGS55=WOOD54=138=10059=332=031=0.0151=014=06=0.018=N1=7YSC120944=46.460040=247=A29=176=INET58=T10=232";

int test_fix_rom(const char** testname, const char** extrareporting)
{
    STARTTEST;
    dart_order_obj* doj = create_empty_doj();
    int in_len = 0;
    int out_len = 0;
    char* dir =  "./test_config/";//"/home/fparnell/git_repo/Dart2/om/test/";
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
        safe_num_val(doj, 320, i);
        if(i <= 146) {
            CHKCOND(get_message(doj)->buflen == 3278);
        }
        safe_num_val(doj, 321, i+3);
        if(i == 146) {
            CHKCOND(get_message(doj)->buflen == 6557);
        }
        sbuff_len = sprintf(sbuff, "%d", i +5);
        safe_val_add(doj, "322", 3, sbuff, sbuff_len);
    }
    CHKCOND(get_message(doj)->buflen == 13114);
    set_num_val(doj, 34, 2234);
    CHKCOND(strncmp("8=FIX.4.29=0000035=c49=116856=ICE57=534=0002234", get_message(doj)->rd_ptr,54) == 0);
    set_tag_nine(doj);                      
    CHKCOND(strncmp("8=FIX.4.29=1174735", get_message(doj)->rd_ptr,19) == 0);
    set_check_sum(doj);
    CHKCOND(strncmp("10=184",get_message(doj)->wr_ptr- 7 , 7) == 0);
    reset_object(trans, doj);
    CHKCOND(strncmp("8=FIX.4.29=0000035=c49=116856=ICE57=534=0000000", get_message(doj)->rd_ptr,54) == 0);
    STOPTEST;
}

int test_check_sum(const char** testname, const char** extrareporting)
{
    STARTTEST;
    REQCOND(1 != 0);
    char* dir =  "./test_config/";//"/home/fparnell/git_repo/Dart2/om/test/";
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
    set_check_sum(doj);

    CHKCOND(strncmp(sec_one, b->rd_ptr, databuf_unread(b)) == 0);
    reset_object(trans, doj);
    databuf_reset(b);     
    char* hb_one = "8=FIX.4.29=6135=049=RONIN56=ARCA57=ARCA34=2452=20141209-13:16:02.00010=084";
    sec_len = strlen(hb_one) - 7;
    databuf_memcpy(get_message(doj), hb_one, sec_len);
    set_check_sum(doj);
    CHKCOND(strncmp(hb_one, b->rd_ptr, databuf_unread(b)) == 0);
    reset_object(trans, doj);
    databuf_reset(b);     
    char* trade_log = "8=FIX.4.29=010235=CG49=116856=ICE50=534=000000252=20141209-01:35:01.773109=1168553=dt_fix_trader96=Starts12310=197";
    sec_len = strlen(trade_log) - 7;
    databuf_memcpy(get_message(doj), trade_log, sec_len);
    set_check_sum(doj);
    CHKCOND(strncmp(trade_log, b->rd_ptr, databuf_unread(b)) == 0);
    //printf("Message: %s \n", b->buffer);
    STOPTEST;
}
char* comper = "8=FIX.4.29=020035=D49=ICE56=116857=534=0002234";
char* comper2 = "8=FIX.4.29=020035=D49=ICE56=116857=534=0002235";
char* comper3 = "8=FIX.4.29=020735=D49=TPF56=116857=534=9342235";



int test_time_checksum(const char** testname, const char** extrareporting)
{
    STARTTEST
    int in_len = 0;
    REQCOND(in_len == 0);
    int out_len = 0;
    char* dir =  "./test_config/";//"/home/fparnell/git_repo/Dart2/om/test/";
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
    set_num_val(doj, 11, seqnum);
    set_fix_val(doj, 1, "RONIN", 5); 
    set_fix_val(doj, 44, "3.1415926", 9); 
    set_fix_val(doj, 40, "2", 1); 
    set_fix_val(doj, 54, "2", 1); 
    set_fix_val(doj, 59, "0", 1); 
    char symbol[3][5] = { "ESZ3", "ESH4", "ESM4" };
    set_fix_val(doj, 107, symbol[seqnum % 3], 4);
    set_fix_val(doj, 55, "ES", 2);
    set_fix_val(doj, 167, "FUT", 3);
    set_time_tag(doj, &tmval, 60);
    set_num_val(doj, 38, (1+seqnum%10));
    set_time_tag(doj,&tmval, 52); 
    set_num_val(doj, 34, 2234);
    set_tag_nine(doj);                      
    set_check_sum(doj);
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
            set_num_val(doj2, 34, 2235);
            set_num_val(doj2, 11, seqnum);
            set_fix_val(doj2, 1, "RONIN", 5); 
            set_fix_val(doj2, 44, "3.1415926", 9); 
            set_fix_val(doj2, 40, "2", 1); 
            set_fix_val(doj2, 54, "2", 1); 
            set_fix_val(doj2, 59, "0", 1); 
            set_fix_val(doj2, 107, symbol[seqnum % 3], 4);
            set_fix_val(doj2, 55, "ES", 2);
            set_fix_val(doj2, 167, "FUT", 3);
            set_time_tag(doj2, &tmval, 60);
            set_num_val(doj2, 38, (1+seqnum%10));
            set_time_tag(doj2,&tmval, 52); 
            set_tag_nine(doj2);                      
            set_check_sum(doj2);
            memset(output, '\0', 500);
            databuf_t* b = get_message(doj2);
            memcpy(output, b->rd_ptr, databuf_unread(b));
            CHKCOND(strncmp(comper2, output, strlen(comper2)) == 0);
            reset_object(t, doj2);
        } else {
            set_num_val(doj3, 11, seqnum);
            set_fix_val(doj3, 1, "RONIN", 5); 
            set_fix_val(doj3, 44, "3.1415926", 9); 
            set_fix_val(doj3, 40, "2", 1); 
            set_fix_val(doj3, 54, "2", 1); 
            set_fix_val(doj3, 59, "0", 1); 
            set_fix_val(doj3, 107, symbol[seqnum % 3], 4);
            set_fix_val(doj3, 55, "ES", 2);
            set_fix_val(doj3, 167, "FUT", 3);
            set_time_tag(doj3, &tmval, 60);
            set_num_val(doj3, 38, (1+seqnum%10));
            set_time_tag(doj3,&tmval, 52); 
            set_num_val(doj3, 34, 2234);
            set_tag_nine(doj3);                      
            set_check_sum(doj3);
            memset(output, '\0', 500);
            databuf_t* b = get_message(doj3);
            memcpy(output, b->rd_ptr, databuf_unread(b));
            CHKCOND(strncmp(comper, output, strlen(comper)) == 0);
            reset_object(t, doj3);
        }
    }


    set_num_val(doj, 11, seqnum);
    //set_fix_val(t, doj, 58, i_and_a, len);
    set_fix_val(doj, 1, "RONIN", 5); 
    set_fix_val(doj, 44, "3.1415926", 9); 
    set_fix_val(doj, 40, "2", 1); 
    set_fix_val(doj, 54, "2", 1); 
    set_fix_val(doj, 59, "0", 1); 
    set_fix_val(doj, 107, symbol[seqnum % 3], 4);
    set_fix_val(doj, 55, "ES", 2);
    set_fix_val(doj, 167, "FUT", 3);
    int worked = overwrite_fix_val(doj, 49, "TPF", 3, 0);
    CHKCOND(worked == 1);
    worked = overwrite_fix_val(doj, 49, "TPF", 6, 0);
    CHKCOND(worked == -1);
    worked = overwrite_fix_val(doj, 58, "TPF", 3, 0);
    CHKCOND(worked == 0);
    set_time_tag(doj, &tmval, 60);
    set_num_val(doj, 38, (1+seqnum%10));
    set_time_tag(doj,&tmval, 52); 
    set_num_val(doj, 34, 9342235);
    set_tag_nine(doj);                      
    b = get_message(doj);
    CHKCOND(strncmp(comper3, b->rd_ptr, strlen(comper3)) == 0);

    STOPTEST;
}
