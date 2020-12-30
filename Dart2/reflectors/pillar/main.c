#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <endian.h>

#define PILL_CNL_REP_REJ 0x0263
#define PILL_BUST 0x0292
#define PILL_EXE 0x0290
#define PILL_PRI_UPDATE 0x0262
#define PILL_MOD_OUT_ACK 0x0271
#define PILL_ORD_ACK 0x0260
#define PILL_MASS_CNL_REQ 0x0281
#define PILL_MOD_REQ 0x0270

#pragma pack(push, 1)

typedef struct pillar_msg_header {
    uint16_t type;
    uint16_t length;
} pmh;

typedef struct pillar_stream_id {
    uint32_t sess;
    uint32_t value;
} psid;

typedef struct pillar_seq_msg_id {
    uint64_t id;
    uint64_t seq;
} psmid;

typedef struct pillar_stream_avail {
    pmh header;
    uint64_t stream_id;
    uint64_t next_seq;
    uint8_t access;
}psavail;

typedef struct pillar_heart_beat {
    pmh header;
}phb;

typedef struct pillar_open {
    pmh header;
    uint64_t stream_id;
    uint64_t start_seq;
    uint64_t end_seq;
    uint8_t access;
    uint8_t mode;
} pillopen;

typedef struct pillar_open_reponse {
    pmh header;
    uint64_t stream_id;
    uint8_t status;
    uint8_t access;
} popen_resp;

typedef struct pillar_close {
    pmh header;
    uint64_t stream_id;
} pillclose;

typedef struct pillar_close_resp {
    pmh header;
    uint64_t stream_id;
    uint8_t status;
} pclose_resp;

typedef struct pillar_login {
    pmh header;
    char username[16];
    char password[32];
    char mic[4];
    char version[20];
}pillog;

typedef struct pillar_login_resp {
    pmh header;
    char username[16];
    uint8_t status;
} plr;

typedef struct pillar_seq_msg {
    pmh header;
    psmid seq_msg_id;
    uint32_t reserve;
    uint64_t timestamp;
} psm;

typedef struct pillar_reference_data_struct {
    pmh header;
    uint64_t timestamp;
    uint32_t symbol;
    char nyse_sym[24];
    char listedmic[4];
    uint8_t round_lot_size;
    uint64_t max_ord_price;
    uint16_t mpv_class_id;
    uint8_t test_sym_ind;
}pill_ref_data;
typedef struct pillar_mpid_configuration {
    pmh header;
    uint64_t timestamp;
    uint8_t status;
    char mpid[4];
    char username[16];
} mpid_config;

typedef struct pillar_session_ack {
    pmh header;
    uint64_t timestamp;
    uint8_t user_session_type;
    uint8_t user_session_status;
    char username[16];
    char mic[4];
    uint8_t cancel_on_disconnect;
    uint8_t throttle_pref;
    uint16_t throttle_window;
    uint16_t throttle_thresh;
    uint8_t symbol_eligiblity;
    uint32_t max_order_qty;
    uint8_t self_trade_prev;
    uint8_t order_priority_update;
    uint8_t ack_status;
} pillsessack;

typedef struct pillar_session_config_req {
    pmh header;
    char username[16];
    uint8_t cancel_on_disconnect;
    uint8_t throttle_pref;
    uint8_t self_trade_prev;
    uint8_t order_priority_update;
} pillsessconfig;

typedef struct pillar_new_order_request {
    pmh header;
    uint32_t symbol;
    char mpid[4];
    uint32_t mmid;
    char mpsubid;
    uint64_t clordid;
    uint64_t orig_ord_key;
    uint64_t bitfield_instr;
    uint64_t price;
    uint32_t qty;
    uint32_t min_qty;
    char user_data[8];
    uint32_t optional;
} pillneword;

typedef struct pillar_cancel_request {
    pmh header;
    uint32_t symbol;
    char mpid[4];
    uint64_t clordid;
    uint64_t orig_ord_key;
} pillcancel;

typedef struct pillar_modify_request {
    pmh header;
    uint32_t symbol;
    uint64_t clordid;
    uint64_t orig_ord_key;
    uint32_t qty;
    uint8_t side;
    uint8_t locate;
} pillmodify;

typedef struct pillar_mass_cancel_request {
    pmh header;
    uint32_t symbol;
    char mpid[4];
    uint32_t mmid;
    uint64_t clordid;
    char deliver_to_comp[5];
    uint8_t bulk_cancel_type;
    uint8_t side;
} pillmasscancel;

typedef struct pillar_order_ack {
    pmh header;
    uint64_t timestamp;
    uint32_t symbol;
    char mpid[4];
    uint32_t mmid;
    char mpsubid;
    uint64_t clordid;
    uint64_t orig_clordid;
    uint64_t bitfield_instr;
    uint64_t price;
    uint32_t qty;
    uint32_t min_qty;
    uint64_t order_id;
    uint32_t leaves;
    uint64_t working_price;
    uint8_t wadisplay;
    char pre_liq_ind[4];
    uint16_t reason;
    uint8_t acktype;
    uint8_t bitfieldflow;
    char user_data[8];
} pillordack;

typedef struct pillar_modify_cancel_ack {
    pmh header;
    uint64_t timestamp;
    uint32_t symbol;
    char mpid[4];
    uint64_t order_id;
    uint64_t refclordid;
    uint64_t orig_clordid;
    uint64_t price;
    uint32_t qty;
    uint32_t leaves;
    uint8_t side;
    uint8_t locate;
    uint16_t reason;
    uint8_t acktype;
    uint8_t bit_indicator;
    char user_data[8];
} pillmodcnlack;

typedef struct pillar_priority_update_ack {
    pmh header;
    uint64_t timestamp;
    uint32_t symbol;
    char mpid[4];
    uint64_t order_id;
    uint64_t clordid;
    uint64_t workingprice;
    uint32_t qty;
    uint8_t wadisplay;
    char preliqind[4];
    uint8_t acktype;
    char user_data[8];
} pillpriorack;

typedef struct pillar_execution_report {
    pmh header;
    uint64_t timestamp;
    uint32_t symbol;
    char mpid[4];
    uint64_t order_id;
    uint64_t  clordid;
    uint64_t dealid;
    uint64_t last_price;
    uint32_t leaves;
    uint32_t cum;
    uint32_t lastqty;
    char liq[4];
    char displiq[4];
    uint8_t locate;
    uint8_t participanttype;
    uint16_t reason;
    char user_data[8];
} pillexerpt;


typedef struct pillar_bust_correct {
    pmh header;
    uint64_t timestamp;
    uint32_t symbol;
    char mpid[4];
    uint64_t order_id;
    uint64_t clordid;
    psmid refseqmsgid;
    uint64_t dealid;
    uint64_t last_price;
    uint32_t lastqty;
    uint16_t reason;
    char user_data[8];
} pillbust;

typedef struct pillar_cnl_rep_mod_rej {
    pmh header;
    uint64_t timestamp;
    uint32_t symbol;
    char mpid[4];
    uint64_t clordid;
    uint16_t reason;
    uint8_t rejtype;
    char user_data[8];
} pillcnlreprej;

typedef struct pillar_equity_cust_addon {
    pmh header;
    char deliver[5];
    uint32_t maxfloor;
    uint32_t minpegqty;
    uint64_t discpricerng;
    uint64_t offsetprice;
    uint64_t effectivetime;
} pillequityadd;

typedef struct pillar_floor_broker_long_addon {
    pmh header;
    char parentmpid[4];
    uint64_t parentclordid;
    char badgeid[4];
    char billto[5];
    uint32_t maxfloor;
    uint32_t minpegqty;
    uint64_t discpricerng;
    uint64_t offsetprice;
    uint8_t display;
} pillflobrolong;

typedef struct pillar_floor_broker_short_addon {
    pmh header;
    char parentmpid[4];
    uint64_t parentclordid;
    char badgeid[4];
    char billto[5];
} pillflobroshort;

#pragma pack(pop)
/* TODO
   1. Manufacture real timestamp.
 */

static char seq_msg[512];

static int message_num = 1;


uint8_t get_bitfield_instruction(uint64_t bits, uint64_t start, uint64_t end)
{
    uint64_t mask = ((1UL << (end - start)) - 1UL) << start;
    uint64_t value = mask & bits;
    uint8_t v = value >> start;
    return v;
}
int send_all(int fd, const char *p, size_t n)
{
    ssize_t nwritten;
    const char *end = p + n;
    while ((nwritten = write(fd, p, end - p)) != -1) {
        p += nwritten;
        if (p == end) {
            return n;
        }
    }
    return -1;
}

int send_seq(int fd, const char* p, size_t n)
{
	memset(seq_msg, '\0', 512);
       	psm* seq = (psm*)seq_msg;
 	memcpy(seq_msg + 32, p, n);
	seq->header.type = 0x0905;
	seq->header.length = (n + 32);
	seq->seq_msg_id.id = 1125327079135513748;
	seq->seq_msg_id.seq = message_num;
	++message_num; 	
	return send_all(fd, seq_msg, (n + 32));
}



struct pillar_reflector {
    int (*on_data_proc) (struct pillar_reflector *, int, char *, size_t);
};
/*static void fill_order(char* buffer, int fd)
{
    send_all(fd, (char *) &rej, sizeof(struct pillar_rej));
}*/


#define EMICRO_SEC 1000000
static uint64_t get_pillar_time()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((tv.tv_usec + (tv.tv_sec * EMICRO_SEC)) * 1000);
}

void respond_cancel(int fd, uint64_t clordid, uint64_t orig, uint32_t sym)
{
    pillmodcnlack* pma = calloc(sizeof(pillmodcnlack), 1);
    pma->header.type = 0x0271;
    pma->header.length =74;
    pma->symbol = sym;
    pma->order_id = get_pillar_time();
    pma->refclordid = clordid;
    pma->orig_clordid = orig;
    pma->acktype = 11;
    send_seq(fd, (char*)pma, sizeof(pillmodcnlack));
    free(pma);
}

void respond_new_ord(pillneword* pn, int fd)
{
    pillordack* pack = calloc(sizeof(pillordack), 1);
    pack->header.type = 0x0260;
    pack->header.length = 102;
    pack->timestamp = get_pillar_time();
    pack->symbol = pn->symbol;
    pack->clordid = pn->clordid;
    pack->orig_clordid = pn->orig_ord_key;
    pack->bitfield_instr = pn->bitfield_instr;
    pack->price = pn->price;
    pack->qty = pn->qty;
    pack->min_qty = pn->min_qty;
    pack->leaves = pn->qty;
    pack->working_price = pn->price;
    pack->acktype = 1;
    send_seq(fd, (char*)pack, sizeof(pillordack));
    uint8_t tif = get_bitfield_instruction(pn->bitfield_instr, 31, 34);
    if(tif == 2) {
        respond_cancel(fd, pack->clordid, pack->clordid, pack->symbol);
    }
    free(pack);
}

int parse_payload(char* d_off, size_t nbytes, int fd)
{
    int bytes_used = 0;
    uint16_t ord_type = 0;
    uint16_t len = 0;
    memcpy(&ord_type, d_off, 2);
    memcpy(&len, d_off + 2, 2);
    switch(ord_type) {
        case 0x0240:
            /*New Order*/
            respond_new_ord((pillneword*)d_off, fd);
            break;
        case 0x0280:
            /*Cancel*/
            {
                pillcancel* pcr = (pillcancel*)d_off;
                respond_cancel(fd, pcr->clordid, pcr->orig_ord_key, pcr->symbol);
            }
            break;
        default:
            printf("Unknown: %d, %d \n", ord_type, len);
            break;
    }
    return bytes_used;
}

static void send_stream_avail(int fd)
{
    /* Now we need to send the stream available messages*/
    psavail* pa = calloc(sizeof(struct pillar_stream_avail), 1);
    pa->header.type = 0x0203;
    pa->header.length = 21;
    pa->stream_id = 1125327079135513748;
    pa->next_seq = 1;
    pa->access = 0;
    if(send_all(fd, (char*)pa, sizeof(struct pillar_login_resp)) != -1) {
        pa->stream_id =981211891059657876;
        if(send_all(fd, (char*)pa, sizeof(struct pillar_login_resp)) != -1) {
            pa->stream_id =2422363771818216596;
            send_all(fd, (char*)pa, sizeof(struct pillar_login_resp));
        }
    }
    free(pa);
}
int on_order(struct pillar_reflector *reflector, int fd, char *buffer,
             size_t nbytes)
{
    uint16_t ord_type = 0;
    uint16_t mess_len = 0;
    char* d_off = buffer;
    int64_t bytes_used = 0;
    int64_t len_left = nbytes;
    do {
        memcpy(&ord_type, d_off, 2);
        memcpy(&mess_len, d_off + 2, 2);
        if(mess_len <= len_left) {
        switch(ord_type) {
            case 0x0205:
                /*Open request*/
                {
                    pillopen* po = (pillopen*)d_off;
                    popen_resp* pr = calloc(sizeof(popen_resp), 1);
                    pr->header.type = 0x0206;
                    pr->header.length = 14;
                    pr->stream_id = po->stream_id;
                    pr->status = 0;
                    pr->access = po->access;
                    send_all(fd, (char*)pr, sizeof(popen_resp));
                    free(pr);
                }
                break;
            case 0x0207:
                /**
                 *Close
                 **/
                {
                    pillclose* pc = (pillclose*)d_off;
                    pclose_resp* pcr = calloc(sizeof(pclose_resp),1);
                    pcr->header.type = 0x0208;
                    pcr->header.length = 13;
                    pcr->stream_id = pc->stream_id;
                    pcr->status = 0;
                }
                break;
            case 0x0204:
                send_stream_avail(fd);
                break;
            case 0x0905:
printf("Got a seq msg: %lu, %d \n", len_left, ord_type);
                parse_payload(d_off + 32, len_left, fd);
                break;
	    default:
		break;

        }
        bytes_used += mess_len;
        len_left -= mess_len;
        d_off += mess_len;
        } else {
	   return bytes_used;
        }
    } while(len_left > 4);
    return bytes_used;
}

int on_login(struct pillar_reflector *reflector, int fd, char *buffer,
             size_t nbytes)
{
    int ret = 0;
    if (nbytes < sizeof(struct pillar_login)) {
        return 0;
    }
    /* We are just going to accept all logons */
    plr* lg = calloc(sizeof(struct pillar_login_resp), 1);
    lg->header.type = 0x0202;
    lg->header.length = 21;
    lg->status = 0;
    reflector->on_data_proc = on_order;
    if (send_all(fd, (char*)lg, sizeof(struct pillar_login_resp)) != -1) {
        /* Now we need to send the stream available messages*/
        send_stream_avail(fd);
        ret =  sizeof(struct pillar_login);
    } else {
        ret = -1;
    }
    free(lg);
    return ret;
}

void pillar_session_event_loop(int fd)
{
    const size_t initial_readbuf_size = 4096;
    char *readbuf = malloc(initial_readbuf_size);
    char *readbuf_end = readbuf + initial_readbuf_size;
    char *wr_ptr = readbuf, *rd_ptr = readbuf;
    ssize_t nread;
    struct pillar_reflector reflector = {
        .on_data_proc = on_login,
    };
    while ((nread = read(fd, wr_ptr, readbuf_end - wr_ptr)) > 0) {
        wr_ptr += nread;
        int nprocessed;
        while ((nprocessed =
                reflector.on_data_proc(&reflector, fd, readbuf,
                                       wr_ptr - rd_ptr)) > 0) {
            rd_ptr += nprocessed;
            size_t nunread = wr_ptr - rd_ptr;
            memmove(readbuf, rd_ptr, nunread);
            rd_ptr = readbuf;
            wr_ptr = readbuf + nunread;
        }
    }
    if (nread == 0) {
        fprintf(stderr, "peer closed its connection\n");
    } else {
        perror("reading socket");
    }
    free(readbuf);
}

int main(int argc, char *argv[])
{
    srand((unsigned int) time(0));
    if (argc < 2) {
        fprintf(stderr, "usage: pillar-reflector listenport\n");
        return EXIT_FAILURE;
    }
    /* initialize_refnum(&refnum); */
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        perror("creating listen socket");
        return EXIT_FAILURE;
    }
    struct sockaddr_in listen_addr;
    memset(&listen_addr, 0, sizeof listen_addr);
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(atoi(argv[1]));
    listen_addr.sin_addr.s_addr = INADDR_ANY;
    int tr = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &tr, sizeof tr) ==
        -1) {
        perror("setting socket reuse address option");
        return EXIT_FAILURE;
    }
    if (bind
        (listenfd, (struct sockaddr *) &listen_addr,
         sizeof listen_addr) == -1) {
        perror("binding listening socket");
        return EXIT_FAILURE;
    }
    if (listen(listenfd, 1) == -1) {
        perror("listen failed");
        return EXIT_FAILURE;
    }
    for (;;) {
        int clientfd = accept(listenfd, 0, 0);
        if (clientfd == -1) {
            perror("accept failed");
            return EXIT_FAILURE;
        }
        pillar_session_event_loop(clientfd);
        if (shutdown(clientfd, SHUT_RDWR) == -1) {
            perror("shutting down client socket");
        }
        if (close(clientfd) == -1) {
            perror("closing client fd");
        }
    }
    return 0;
}
