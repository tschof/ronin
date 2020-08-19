#include "tests.h"
#include "testmacros.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "fix_rom_trans.h"
#include "fix_handler.h"
#include "dart_orderbook.h"
#include "rom2fix.h"
#include "parser_func.h"
#include "client_manager.h"
#include "transaction_recorder.h"
#include "ex_common.h"
#include "complex_parser.h"
#include "dart_constants.h"


static char *parse_me_2 =
    "8=FIX.4.29=26935=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=6147857=7YSC11=344029965041=344029965037=2305217=-5935920=0150=439=4109=DEGS55=WOOD54=138=10059=332=031=0.0151=014=06=0.018=N1=7YSC120944=46.460040=247=A29=176=INET58=T10=2328=FIX.4.29=26935=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=6147957=7YSC11=344029500441=344029500437=1840617=-5936020=0150=439=4109=DEGS55=HTLF54=538=10059=332=031=0.0151=014=06=0.018=N1=7YSC120944=14.490040=247=A29=176=INET58=T10=2018=FIX.4.29=27035=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=6148057=7YSC11=344029936641=344029936637=2276817=-5936120=0150=439=4109=DEGS55=CINF54=538=300059=332=031=0.0151=014=06=0.018=N1=7YSC120944=30.750040=247=A29=176=INET58=T10=2558=FIX.4.29=09335=249=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=614877=4513916=010=0878=FIX.4.29=26935=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=6148857=7YSC11=344029277941=344029277937=1618117=-5936820=0150=439=4109=DEGS55=KNOT54=138=20059=332=031=0.0151=014=06=0.018=N1=7YSC120944=10.060040=247=A29=176=INET58=T10=2398=FIX.4.29=26935=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=6148957=7YSC11=344029290241=344029290237=1630417=-5936920=0150=439=4109=DEGS55=NLCI54=538=10059=332=031=0.0151=014=06=0.018=N1=7YSC120944=11.680040=247=A29=176=INET58=T10=2048=FIX.4.29=26835=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=6149057=7YSC11=344030093341=344030093337=2433517=-5937020=0150=439=4109=DEGS55=NEWS54=538=10059=332=031=0.0151=014=06=0.018=N1=7YSC120944=9.370040=247=A29=176=INET58=T10=1568=FIX.4.29=26935=849=INET56=DEGSR143=N52=20110531-13:30:04122=20110531-13:30:0434=6149157=7YSC11=344029630241=344029630237=1970417=-5937120=0150=439=4109=DEGS55=DXPE54=138=10059=332=031=0.0151=014";


static unsigned long test_insert_messages(char *data, unsigned long size,
                                          unsigned long byte_offset,
                                          unsigned long *seq_num)
{
    if (size > 0) {
        char eol[5] = { FIX_FIELD_DELIM, '1', '0',
            FIX_FIELD_VALUE_DELIM, '\0'
        };
        char *d_off;
        unsigned long read_bytes;
        long ret_code;
        unsigned long l_seq;
        read_bytes = 0;
        ret_code = 0;
        l_seq = *seq_num;
        d_off = data;
        ret_code = find_offset(d_off, size, eol, 5);
        while (ret_code >= 0) {
            size_t len = 0;
            struct message m;
            if ((read_bytes + ret_code + FIX_EOM_OFFSET) > size) {
                *seq_num = l_seq;
                return read_bytes;
            }
            len = (ret_code + FIX_EOM_OFFSET);
            read_bytes += (unsigned long) ret_code;
            read_bytes += FIX_EOM_OFFSET;
            m.name = "INET";
            m.name_len = 4;
            m.data = d_off;
            m.len = len;
            record_sent_message(&m);
            d_off = d_off + ret_code + FIX_EOM_OFFSET;
            ++l_seq;
            ret_code = find_offset(d_off, (size - read_bytes), eol, 5);
        }
        *seq_num = l_seq;
        return read_bytes;
    }
    return size;
}

static int test_set_outgoing(size_t * seq_num, FILE * file)
{
    //unsigned long seq_num;
    unsigned long bytes_read;
    unsigned long bytes_used;
    unsigned long total_bytes_read;
    unsigned long cum_bytes_used;
    unsigned long total_size;
    unsigned long prev_total_size;
    char *file_reader;
    char *wr_ptr;
    char *rd_ptr;
    unsigned long read_offset;
    unsigned long write_offset;
    file_reader = calloc(FILE_BUFFER_SIZE, 1);
    wr_ptr = file_reader;
    rd_ptr = file_reader;
    read_offset = 0;
    write_offset = 0;
    bytes_used = 0;
    bytes_read = 0;
    total_bytes_read = 0;
    cum_bytes_used = 0;
    // seq_num = 1;
    total_size = FILE_BUFFER_SIZE;
    while (!(feof(file))) {
        if ((total_size - write_offset) <= FILE_READ_CHUNK) {
            char *temp;
            prev_total_size = (write_offset - read_offset) + 1;
            temp = calloc(prev_total_size, 1);
            memcpy(temp, rd_ptr, (write_offset - read_offset));
            free(file_reader);
            file_reader = NULL;
            total_size += FILE_BUFFER_SIZE;
            file_reader = calloc(total_size, 1);
            memcpy(file_reader, temp, prev_total_size);
            free(temp);
            temp = NULL;
            rd_ptr = file_reader;
            write_offset = (write_offset - read_offset);
            read_offset = 0;
            wr_ptr = file_reader + write_offset;
        }
        bytes_read = fread(wr_ptr, 1, FILE_READ_CHUNK, file);
        total_bytes_read = bytes_read + (write_offset - read_offset);
        bytes_used =
            test_insert_messages(rd_ptr, total_bytes_read, cum_bytes_used,
                                 seq_num);
        cum_bytes_used += bytes_used;
        if (bytes_used >= total_bytes_read) {
            write_offset = 0;
            read_offset = 0;
            wr_ptr = file_reader;
            rd_ptr = file_reader;
        } else {
            rd_ptr = rd_ptr + bytes_used;
            read_offset += bytes_used;
            write_offset += bytes_read;
            wr_ptr = wr_ptr + bytes_read;
        }
    }
    free(file_reader);
    return 0;
}

static void test_client_callback(struct message *m, void *ob)
{
}

static struct init_args *create_test_args()
{
    struct init_args *in = malloc(sizeof(struct init_args));
    in->name = calloc(5, 1);
    memcpy(in->name, "INET", 4);
    in->name_len = 4;
    in->sender_comp = calloc(10, 1);
    memcpy(in->sender_comp, "DART1", 5);
    in->sc_len = 5;
    in->target_comp = calloc(5, 1);
    memcpy(in->target_comp, "INET", 4);
    in->tc_len = 4;
    in->target_sub = 0;
    in->ts_len = 0;
    in->sender_sub = 0;
    in->ss_len = 0;
    in->fix_version = calloc(10, 1);
    memcpy(in->fix_version, "4.2", 3);
    in->fv_len = 3;
    in->reset_time = 0;
    in->log_outgoing = 1;
    in->cancel_only = 0;
    in->create_mess_header = create_fix_header;
    return in;
}

void *run_inserter(void *args)
{
    size_t seq_num = 1;
    FILE *file;
    file = fopen("TEST_outgoing", "rb+");
    test_set_outgoing(&seq_num, file);
    return NULL;
}

void *run_parser(void *args)
{
    struct parse_con *pcon = (struct parse_con *) args;
    int cut_con = 0;

    parse_fix_message(parse_me_2, strlen(parse_me_2), &cut_con, pcon);
    return NULL;
}

static void connection_callback(con_obj * co, int is_connected, char *log,
                                int len)
{
}

int test_create_fix_header(const char **testname,
                           const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    time_t rawtime;
    time_t now;
    struct tm *ti;
    struct tm *to;
    time(&rawtime);
    create_fix_header(test_b, "DODO", 4, "BRAIN", 5, "SCHMO", 5, "JOE", 3,
                      1500, &now);
    struct tm lm;
    ti = gmtime_r(&rawtime, &lm);
    ti = &lm;
    to = gmtime_r(&now, &lm);
    to = &lm;
    CHKCOND(to->tm_hour == ti->tm_hour);
    CHKCOND(to->tm_year == ti->tm_year);
STOPTEST}

int test_resend_request_handler(const char **testname,
                                const char **extrareporting)
{
    STARTTEST databuf_t *test_b = databuf_alloc(500);
    REQCOND(test_b != 0);
    con_obj *co = create_con_obj("INET", 4, -1, 0, 1, 1);
    int status = 0;
    void *result;
    struct parse_con *pcon = malloc(sizeof(struct parse_con));
    if (pcon == NULL) {
        return -1;
    }
    pcon->send_ptr = send_message;
    pcon->callback = test_client_callback;
    pcon->record_mess = record_inbound_message;
    pcon->init_con = add_connection;
    pcon->remove_con = remove_connection;
    pcon->connection_notice = connection_callback;
    pcon->timeout_val = 30;
    pcon->is_logged_on = 1;
    pcon->book = NULL;
    pcon->t = NULL;
    pcon->co = co;
    pcon->in_args = create_test_args();
    pcon->mod_name = calloc(6, 1);
    memcpy(pcon->mod_name, "TJAM", 4);
    pcon->mod_len = 4;

    app_end_time *t = malloc(sizeof(struct app_end_time));
    pcon->t = t;
    struct rec_init *rec = malloc(sizeof(struct rec_init));
    if (rec == NULL) {
        return -1;
    }
    t->end_hour = 23;
    t->end_min = 23;
    t->end_day = 7;
    rec->create_new = 0;
    rec->timeout_value = 30;
    rec->t = t;
    start_recorder(rec);
    struct init_args* in_args = calloc(1,sizeof(struct init_args));
    struct seq_reset_time* rt = calloc(1, sizeof(struct seq_reset_time));
    rt->create_new = 0;
    rt->new_incoming = 0;
    rt->new_outgoing = 0;
    in_args->fix_version = "FIX.4.2";
    in_args->reset_time = rt;
    sequence_req *sr =
        create_sequence_request("INET", 4, in_args,
                                build_fix_offsets,
                                create_fix_resend_pad);
    if (sr == NULL) {
        return -1;
    }
    get_sequence_numbers(sr);
    destroy_sequence_req(sr);
    pthread_t num1;
    pthread_t num2;
    status = pthread_create(&num1, NULL, &run_inserter, pcon);
    sleep(5);
    status = pthread_create(&num2, NULL, &run_parser, pcon);
    sleep(2);
    status = pthread_join(num1, &result);
    status = pthread_join(num2, &result);
    databuf_destroy(test_b);
STOPTEST}
