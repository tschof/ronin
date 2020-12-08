#include "tests.h"
#include "testmacros.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#include "fix_handler.h"
#include "dart_thr_pool.h"
#include "parser_func.h"
#include "transaction_recorder.h"
#include "dart_constants.h"
#include "dart_resend_request.h"


static char *parse_me_1 = "8=FIX.4.29=26935=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=157=7YSC147811=344029965041=344029965037=2305217=-5935920=0150=439=4109=DEGS55=WOOD54=138=10059=332=031=0.0151=014=06=0.018=N1=7YSC120944=46.460040=247=A29=176=INET58=T10=2328=FIX.4.29=26935=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=257=7YSC147911=344029500441=344029500437=1840617=-5936020=0150=439=4109=DEGS55=HTLF54=538=10059=332=031=0.0151=014=06=0.018=N1=7YSC120944=14.490040=247=A29=176=INET58=T10=2018=FIX.4.29=27035=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=357=7YSC148011=344029936641=344029936637=2276817=-5936120=0150=439=4109=DEGS55=CINF54=538=300059=332=031=0.0151=014=06=0.018=N1=7YSC120944=30.750040=247=A29=176=INET58=T10=2558=FIX.4.29=09335=249=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=47=45139148716=010=0878=FIX.4.29=26935=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=557=7YSC148811=344029277941=344029277937=1618117=-5936820=0150=439=4109=DEGS55=KNOT54=138=20059=332=031=0.0151=014=06=0.018=N1=7YSC120944=10.060040=247=A29=176=INET58=T10=2398=FIX.4.29=26935=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=657=7YSC148911=344029290241=344029290237=1630417=-5936920=0150=439=4109=DEGS55=NLCI54=538=10059=332=031=0.0151=014=06=0.018=N1=7YSC120944=11.680040=247=A29=176=INET58=T10=2048=FIX.4.29=26835=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=757=7YSC149011=344030093341=344030093337=2433517=-5937020=0150=439=4109=DEGS55=NEWS54=538=10059=332=031=0.0151=014=06=0.018=N1=7YSC120944=9.370040=247=A29=176=INET58=T10=156";
static char *parse_me_3 = "8=FIX.4.29=26935=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=157=7YSC147811=344029965041=344029965037=2305217=-5935920=0150=439=4109=DEGS55=WOOD54=138=10059=332=031=0.0151=014=06=0.018=N1=7YSC120944=46.460040=247=A29=176=INET58=T10=2328=FIX.4.29=26935=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=257=7YSC147911=344029500441=344029500437=1840617=-5936020=0150=439=4109=DEGS55=HTLF54=538=10059=332=031=0.0151=014=06=0.018=N1=7YSC120944=14.490040=247=A29=176=INET58=T10=2018=FIX.4.29=27035=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=357=7YSC148011=344029936641=344029936637=2276817=-5936120=0150=439=4109=DEGS55=CINF54=538=300059=332=031=0.0151=014=06=0.018=N1=7YSC120944=30.750040=247=A29=176=INET58=T10=2558=FIX.4.29=09335=249=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=47=45139148716=010=0878=FIX.4.29=26935=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=557=7YSC148811=344029277941=344029277937=1618117=-5936820=0150=439=4109=DEGS55=KNOT54=138=20059=332=031=0.0151=014=06=0.018=N1=7YSC120944=10.060040=247=A29=176=INET58=T10=2398=FIX.4.29=26935=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=657=7YSC148911=344029290241=344029290237=1630417=-5936920=0150=439=4109=DEGS55=NLCI54=538=10059=332=031=0.0151=014=06=0.018=N1=7YSC120944=11.680040=247=A29=176=INET58=T10=2048=FIX.4.29=26835=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=757=7YSC149011=344030093341=344030093337=2433517=-5937020=0150=439=4109=DEGS55=NEWS54=538=10059=332=031=0.0151=014=06=0.018=N1=7YSC120944=9.370040=247=A29=176=INET58=T10=1568=FIX.4.29=26935=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=857=7YSC149111=344029630241=344029630237=1970417=-5937120=0150=439=4109=DEGS55=DXPE54=138=10059=332=031=0.0151=014";
static char *parse_me_2 =
    "8=FIX.4.29=26935=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=6147857=7YSC11=344029965041=344029965037=2305217=-5935920=0150=439=4109=DEGS55=WOOD54=138=10059=332=031=0.0151=014=06=0.018=N1=7YSC120944=46.460040=247=A29=176=INET58=T10=2328=FIX.4.29=26935=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=6147957=7YSC11=344029500441=344029500437=1840617=-5936020=0150=439=4109=DEGS55=HTLF54=538=10059=332=031=0.0151=014=06=0.018=N1=7YSC120944=14.490040=247=A29=176=INET58=T10=2018=FIX.4.29=27035=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=6148057=7YSC11=344029936641=344029936637=2276817=-5936120=0150=439=4109=DEGS55=CINF54=538=300059=332=031=0.0151=014=06=0.018=N1=7YSC120944=30.750040=247=A29=176=INET58=T10=2558=FIX.4.29=09335=249=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=614877=4513916=010=0878=FIX.4.29=26935=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=6148857=7YSC11=344029277941=344029277937=1618117=-5936820=0150=439=4109=DEGS55=KNOT54=138=20059=332=031=0.0151=014=06=0.018=N1=7YSC120944=10.060040=247=A29=176=INET58=T10=2398=FIX.4.29=26935=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=6148957=7YSC11=344029290241=344029290237=1630417=-5936920=0150=439=4109=DEGS55=NLCI54=538=10059=332=031=0.0151=014=06=0.018=N1=7YSC120944=11.680040=247=A29=176=INET58=T10=2048=FIX.4.29=26835=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=6149057=7YSC11=344030093341=344030093337=2433517=-5937020=0150=439=4109=DEGS55=NEWS54=538=10059=332=031=0.0151=014=06=0.018=N1=7YSC120944=9.370040=247=A29=176=INET58=T10=1568=FIX.4.29=26935=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=6149157=7YSC11=344029630241=344029630237=1970417=-5937120=0150=439=4109=DEGS55=DXPE54=138=10059=332=031=0.0151=014";

#define CHAR_BIT 8
unsigned short combo_mess_types(unsigned char a, unsigned char b)
{
    return (a << CHAR_BIT) + b;
}

int test_build_fix_offsets(const char** testname, const char** extrareporting)
{
    STARTTEST;
    unsigned long  pm2_len = (unsigned long)strlen(parse_me_2);
    REQCOND(pm2_len != 0);
    unsigned long l_seq = 0;
    unsigned long read_bytes = build_fix_offsets(parse_me_2, pm2_len,0,&l_seq, NULL);
    CHKCOND(read_bytes == 1868); 
    CHKCOND(l_seq == 7);
    STOPTEST;
}

int test_find_fix_offsets(const char** testname, const char** extrareporting)
{
    STARTTEST;
    unsigned long pm1_len = (unsigned long)strlen(parse_me_1);
    REQCOND(pm1_len != 0);
    size_t  seq_off = 0;
    size_t beg_off = 0;
    size_t end_off = 0;
    size_t res = find_fix_offsets(parse_me_1, &seq_off, pm1_len,
                                  0, 1, 5, &beg_off,&end_off);
    CHKCOND(res == 1285);
    CHKCOND(beg_off == 0);
    CHKCOND(end_off == 1285);
    CHKCOND(res == end_off);
    CHKCOND(seq_off == 5);

    seq_off = 0;
    beg_off = 0;
    end_off = 0;
    res = find_fix_offsets(parse_me_1, &seq_off, pm1_len,
                           0, 1, 9, &beg_off, &end_off);
    CHKCOND(beg_off == 0);
    CHKCOND(end_off == 0);
    CHKCOND(seq_off == 7);
    CHKCOND(res == 1868);

    unsigned long pm3_len = (unsigned long)strlen(parse_me_3);
    seq_off = 0;
    end_off = 0;
    beg_off = 0;
    res = find_fix_offsets(parse_me_3, &seq_off, pm3_len,
                           0, 1, 9, &beg_off, &end_off);
    CHKCOND(beg_off == 0);
    CHKCOND(end_off == 0);
    CHKCOND(seq_off == 7);
    CHKCOND(res == 1868);


    seq_off = 0;
    end_off = 0;
    beg_off = 0;
    res = find_fix_offsets(parse_me_3, &seq_off, pm3_len,
                           0, 7, 9, &beg_off, &end_off);
    CHKCOND(beg_off == 1577);
    CHKCOND(end_off == 0);
    CHKCOND(seq_off == 7);
    CHKCOND(res == 1868);


    seq_off = 0;
    end_off = 0;
    beg_off = 0;
    res = find_fix_offsets(parse_me_3, &seq_off, pm3_len,
                           0, 7, 7, &beg_off, &end_off);
    CHKCOND(beg_off == 1577);
    CHKCOND(end_off == 1868);
    CHKCOND(seq_off == 7);
    CHKCOND(res == 1868);
    STOPTEST;
}

int test_fix_tag_parsin(const char** testname, const char** extrareporting)
{
    STARTTEST;
    databuf_t* dbuff = databuf_alloc(60);
    REQCOND(dbuff != 0);
    databuf_memcpy(dbuff, parse_me_2, 10); 
    char* one = "8=FIX.4.29=26935=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=6147857=7YSC11=344029965041=344029965037=2305217=-5935920=0150=439=4109=DEGS55=WOOD54=138=10059=332=031=0.0151=014=06=0.018=N1=7YSC120944=46.460040=247=A29=176=INET58=T10=232";
    int one_len = strlen(one);
    char* d_off = one;
    int found = 0;
    char* tag = "76";
    GET_OFF(d_off, one_len, tag, 2, found);
    char* val_end = d_off + 3;
    CHKCOND(found == 1);
    CHKCOND(strncmp(d_off, "76", 2) == 0);
    NEXT_ONE(val_end, one_len);
    CHKCOND(val_end[0] == 0x01);
    CHKCOND(val_end - (d_off + 3) == 4);

    {
        one_len = strlen(one);
        d_off = one;
        GET_OFF(d_off, one_len, "99=", 3, found);
        CHKCOND(found == 0);
        CHKCOND(one_len == 3);
    }
    {
        char* partial = "8=FIX.4.29=26935=849=IN";
        int p_len = strlen(partial);
        d_off = partial;
        found = 0;
        GET_OFF(d_off, p_len, "49=", 3, found);
        CHKCOND(found == 1);
        CHKCOND(strncmp(d_off, "49", 2) == 0);
        val_end = d_off + 3;
        p_len -= 3;
        NEXT_ONE(val_end, p_len);
        CHKCOND(p_len == 0);
        CHKCOND(*val_end != 0x01);
    }
    {
        char* partial = "8=FIX.4.29=26935=849=INET56=DEGSR1";
        int p_len = 26;
        d_off = partial;
        found = 0;
        GET_OFF(d_off, p_len, "49=", 3, found);
        CHKCOND(found == 1);
        CHKCOND(strncmp(d_off, "49", 2) == 0);
        val_end = d_off + 3;
        p_len -= 3;
        NEXT_ONE(val_end, p_len);
        CHKCOND(p_len == 0);
        CHKCOND(*val_end != 0x01);
    }
    {
        char* partial = "8=FIX.4.29=26935=849=INET56=DEGSR1";
        int p_len = 29;
        d_off = partial;
        found = 0;
        GET_OFF(d_off, p_len, "49=", 3, found);
        CHKCOND(found == 1);
        CHKCOND(strncmp(d_off, "49", 2) == 0);
        val_end = d_off + 3;
        p_len -= 3;
        NEXT_ONE(val_end, p_len);
        CHKCOND(p_len == 1);
        CHKCOND(*val_end == 0x01);
    }
    databuf_destroy(dbuff);
    STOPTEST;
}
static char* r_t_2 ="8=FIX.4.29=0027935=D49=116856=ICE34=000237752=20150601-14:56:49.028109=1168115=DART|DART144=DAU|ROMCHI9139=dt_fix_trader21=1116=8KG|8KG50=8KG11=3800601225255=800895167=FUT54=160=20150601-14:56:49.02838=140=244=-1.759=0440=TEST1=TEST439=6379195=C9208=29207=12345678901210=168";
static char* r_tester = "8=FIX.4.29=008635=c49=116856=ICE34=000000350=552=20150601-00:35:01.845320=048=0321=3167=FUT10=1748=FIX.4.29=008835=c49=116856=ICE34=000000450=552=20150601-00:35:01.845320=048=0321=100167=OPT10=0198=FIX.4.29=008635=c49=116856=ICE34=000000550=552=20150601-00:35:01.845320=148=1321=3167=FUT10=178";

static long test_get_sequence(char *mblk, size_t len)
{
    char* d_off = mblk;
    int l_len = (int) len;
    long found = 0;
    GET_OFF(d_off, l_len, "\00134=", 4, found);
    if(found) {
        found = atol(d_off + 4);
    }
    return found;
}
int test_resend_req(const char** testname, const char** extrareporting)
{
    STARTTEST;
    size_t out_len = 0;
    REQCOND(out_len == 0);
    int file = open("ICE_outgoing.test", O_APPEND | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    resend_req* rr = create_resend_request("1168", 4,
                                           2254, 2254, find_fix_offsets);
    drr_create_resend_out(rr, file);
    char* output = (char *) drr_get_resend_output(rr, &out_len);
    CHKCOND(strncmp(output,"8=FIX.4.29=013735=F49=116856=ICE34=0002254",43) == 0);
    CHKCOND(out_len == 161);
    CHKCOND(strstr(output, "35=F") != NULL);
    CHKCOND(strstr(output, "34=0002254") != NULL);
    CHKCOND(strstr(output, "167=FUT") != NULL);
    CHKCOND(strstr(output, "10=107") != NULL);
    destroy_resend_req(rr);
    resend_req* r2 = create_resend_request("1168", 4, 1795, 1810, find_fix_offsets);
    drr_create_resend_out(r2, file);
    output = (char *) drr_get_resend_output(r2, &out_len);
    CHKCOND(out_len == 1673);
    CHKCOND(strncmp((output + 37), "34=0001795", 6) == 0);
    CHKCOND(strstr(output, "34=0001796") != NULL);
    CHKCOND(strstr(output, "34=0001800") != NULL);
    CHKCOND(strstr(output, "34=0001810") != NULL);
    CHKCOND(strncmp((output + 1666), "10=188", 6) == 0);
    CHKCOND(output[1672] == 0x01);
    {
        int beg_seq = 1795;
        int end = 1810;
        int found = 0;
        char* d_off = output;
        long len = out_len;
        long seq = beg_seq;
        do {
            GET_OFF(d_off, len, "\00135=", 4, found);
            if(found) {
                char* val = d_off + 4;
                seq = test_get_sequence(d_off, len);
                switch(val[0]) {
                    default:
                        //do nothing we will gap fill later.
                        break;
                    case 'C':
                        if(val[1] == 'G') {
                            if(beg_seq != seq) {
                                CHKCOND(beg_seq <= seq);
                            }
                            beg_seq = seq + 1;
                        }
                        break;
                    case 'c':
                    case '8':
                    case 'F':
                        if(beg_seq != seq) {
                            CHKCOND(beg_seq <= seq);
                        }
                        beg_seq = seq + 1;
                        CHKCOND(beg_seq >= seq);
                        break;
                };
            }
            d_off += 4;
            len -= 4;
        } while(found);
        if(beg_seq != seq) {
            long end_val = end == 0 ? -1: end;
            CHKCOND(end_val == end);
        }
    }
    destroy_resend_req(r2);
    r2 = create_resend_request("1168", 4, 3, 5, find_fix_offsets);
    drr_create_resend_out(r2, file);
    output = (char *) drr_get_resend_output(r2, &out_len);
    int r_len = strlen(r_tester);
    CHKCOND(strncmp(r_tester, output, r_len) == 0);
    destroy_resend_req(r2);

    r2 = create_resend_request("1168", 4,2377, 2377,find_fix_offsets);
    drr_create_resend_out(r2, file);
    output = (char *) drr_get_resend_output(r2, &out_len);
    r_len = strlen(r_t_2);
    CHKCOND(strncmp(r_t_2, output, r_len) == 0);
    destroy_resend_req(r2);

    out_len = 0;
    output = 0;
    r2 = create_resend_request("1168", 4,2378, 2378,find_fix_offsets);
    drr_create_resend_out(r2, file);
    output = (char *) drr_get_resend_output(r2, &out_len);
    CHKCOND(out_len == 0);
    destroy_resend_req(r2);
    close(file);

    file = open("2JB_outgoing.test", O_APPEND | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    r2 = create_resend_request("2JB",3 , 215341,215341,find_fix_offsets);
    drr_create_resend_out(r2, file);
    output = (char *) drr_get_resend_output(r2, &out_len);
    CHKCOND(out_len == 0);
    destroy_resend_req(r2);
    close(file);






    rec_args*  rc = init_recorder(128, 128, "./",2);
    thr_pool_t* thr = thr_pool_create(2, 2, 0, NULL);
    //Start the recorder first, if we are in the 2 thread mode
    //we can get into a state where one thread records messages and
    //epolls otherwise.
    if(rc) {
        create_thr(thr, record_messages, rc);
    }
    seq_reset_time srt;
    srt.reset_hour = 1;
    srt.reset_min = 1;
    srt.reset_day = 0;
    srt.create_new = 1;
    srt.last_logon = 0;
    srt.new_incoming = 0;
    srt.new_outgoing = 8888888;

    struct init_args iarg;
    memset(&iarg, '\0', sizeof(iarg));
    iarg.reset_time = &srt;
    iarg.fix_version = "FIX.4.2";
    iarg.fv_len = 7;
    sequence_req* sr = create_sequence_request("CME55", 5,
                                 &iarg,
                                 build_fix_offsets,
                                 create_fix_resend_pad, NULL);
    get_sequence_numbers(sr);
    CHKCOND(dsr_get_outgoing(sr) == 8888888);
    stop_logging();
    thr_pool_destroy(thr);
    thr_pool_wait_n_join(thr);
    STOPTEST;
}

/*
 *
 *
 */
int testshort_creation(const char **testname, const char **extrareporting)
{
    STARTTEST;
    databuf_t *dbuff = 0;
    REQCOND(dbuff == 0);
    char* type = "8";
    unsigned short output = 0;
    int out_len = 1; 
    output = out_len > 1 ? combo_mess_types(type[1],type[0]) : combo_mess_types(0x0, type[0]);
    CHKCOND(output == 56);
    CHKCOND(output == 0x38);
    type = "G";
    output = out_len > 1 ? combo_mess_types(type[1],type[0]) : combo_mess_types(0x0, type[0]);
    CHKCOND(output == 71);
    CHKCOND(output == 0x47);
    type = "F";
    output = out_len > 1 ? combo_mess_types(type[1],type[0]) : combo_mess_types(0x0, type[0]);
    CHKCOND(output == 70);
    CHKCOND(output == 0x46);
    type = "D";
    output = out_len > 1 ? combo_mess_types(type[1],type[0]) : combo_mess_types(0x0, type[0]);
    CHKCOND(output == 68);
    CHKCOND(output == 0x44);

    type = "AB";
    out_len = 2;
    output = out_len > 1 ? combo_mess_types(type[1],type[0]) : combo_mess_types(0x0, type[0]);
    CHKCOND(output == 16961);
    CHKCOND(output == 0x4241);

    type = "AC";
    output = out_len > 1 ? combo_mess_types(type[1],type[0]) : combo_mess_types(0x0, type[0]);
    CHKCOND(output == 17217);
    CHKCOND(output == 0x4341);

    type = "CG";
    output = out_len > 1 ? combo_mess_types(type[1],type[0]) : combo_mess_types(0x0, type[0]);
    CHKCOND(output == 0x4743);
    
    type = "CH";
    output = out_len > 1 ? combo_mess_types(type[1],type[0]) : combo_mess_types(0x0, type[0]);
    CHKCOND(output == 0x4843);
   
    type = "CI";
    output = out_len > 1 ? combo_mess_types(type[1],type[0]) : combo_mess_types(0x0, type[0]);
    CHKCOND(output == 0x4943);

    type = "CJ";
    output = out_len > 1 ? combo_mess_types(type[1],type[0]) : combo_mess_types(0x0, type[0]);
    CHKCOND(output == 0x4A43);

    type = "BF";
    output = out_len > 1 ? combo_mess_types(type[1],type[0]) : combo_mess_types(0x0, type[0]);
    CHKCOND(output == 17986);
    CHKCOND(output == 0x4642);

    type = "BE";
    output = out_len > 1 ? combo_mess_types(type[1],type[0]) : combo_mess_types(0x0, type[0]);
    CHKCOND(output == 17730);
    CHKCOND(output == 0x4542);

    type = "AR";
    output = out_len > 1 ? combo_mess_types(type[1],type[0]) : combo_mess_types(0x0, type[0]);
    CHKCOND(output == 21057);
    CHKCOND(output == 0x5241);

    type = "AE";
    output = out_len > 1 ? combo_mess_types(type[1],type[0]) : combo_mess_types(0x0, type[0]);
    CHKCOND(output == 17729);
    CHKCOND(output == 0x4541);

    type = "b";
    out_len = 1;
    output = out_len > 1 ? combo_mess_types(type[1],type[0]) : combo_mess_types(0x0, type[0]);
    CHKCOND(output == 98);
    CHKCOND(output == 0x62);

    type = "Q";
    output = out_len > 1 ? combo_mess_types(type[1],type[0]) : combo_mess_types(0x0, type[0]);
    CHKCOND(output == 81);
    CHKCOND(output == 0x51);

    type = "r";
    output = out_len > 1 ? combo_mess_types(type[1],type[0]) : combo_mess_types(0x0, type[0]);
    CHKCOND(output == 114);
    CHKCOND(output == 0x72);

    STOPTEST;
}
