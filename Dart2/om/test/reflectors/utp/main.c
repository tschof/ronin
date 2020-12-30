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

/* TODO
   1. Manufacture real timestamp.
 */

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

static int message_num = 0;

#pragma pack(push,1)

struct login {
    uint16_t type;
    uint16_t length;
    uint32_t seq;
    uint32_t last_recv_seq;
    char senderCompid[12];
    uint32_t mvp;
    char cancel_on_disconnect;
    char filler[3];
};

struct utp_order {
    uint16_t type;
    uint16_t len;
    uint32_t seq;
    uint32_t qty;
    uint32_t max_floor;
    uint32_t price;
    char p_scale;
    char symbol[11];
    char ex_inst;
    char side;
    char otype;
    char tif; 
    char rule80A;
    char route;
    char dot_resv;
    char on_behalf[5];
    char sender_sub[5];
    char clearing[5];
    char account[10];
    char clordid[17];
    char filler[3];
};

struct utp_accept {
    uint16_t type;
    uint16_t len;
    uint32_t seq;
    uint32_t me_ord_id;
    uint32_t tt;
    char deliver_comp[5];
    char target_sub[5];
    char account[10];
    char clordid[17];
    char filler[3];
};

struct utp_cancel {
    uint16_t type;
    uint16_t len;
    uint32_t seq;
    uint32_t me_ord_id;
    uint32_t orig_qty;
    uint32_t cancel_qty;
    uint32_t leaves_qty;
    char symbol[11];
    char side;
    char on_behalf[5];
    char sender_sub[5];
    char account[10];
    char clordid[17];
    char orig_clordid[17];
    char filler[2];
};

struct utp_out {
    uint16_t type;
    uint16_t len;
    uint32_t seq;
    uint32_t me_ord_id;
    uint32_t tt;
    char ic;
    char deliver_comp[5];
    char target_sub[5];
    char account[10];
    char orig_clordid[17];
    char filler[2];
};

struct utp_replace {
    uint16_t type;
    uint16_t len;
    uint32_t seq;
    uint32_t me_ord_id;
    uint32_t qty;
    uint32_t max_floor;
    uint32_t price;
    char p_scale;
    char symbol[11];
    char ex_inst;
    char route;
    char side;
    char otype;
    char tif;
    char rule80A;
    char dot_reserve;
    char on_behalf[5];
    char sender_sub[5];
    char clearing[5];
    char account[10];
    char clordid[17];
    char orig_clordid[17];
    char filler[2];
};

struct utp_rep_ack {
    uint16_t type;
    uint16_t len;
    uint32_t seq;
    uint32_t me_ord_id;
    uint32_t tt;
    char deliver_comp[5];
    char target_sub[5];
    char account[10];
    char orig_clordid[17];
    char filler[3];
};

struct utp_replaced {
    uint16_t type;
    uint16_t len;
    uint32_t seq;
    uint32_t me_ord_id;
    uint32_t tt;
    uint32_t leavesqty;
    char ic;
    char deliver_comp[5];
    char target_sub[5];
    char account[10];
    char clordid[17];
    char filler[2];
};

struct utp_executed {
    uint16_t type;
    uint16_t len;
    uint32_t seq;
    uint32_t me_ord_id;
    uint32_t tt;
    uint32_t leavesqty;
    uint32_t last_shares;
    uint32_t last_price;
    char price_scale;
    char side;
    char billing;
    char last_market;
    char deliver_comp[5];
    char target_sub[5];
    char exec_broker[5];
    char contra_broker[5];
    char contra_trader[5];
    char exec_away_mktid[6];
    char billing_rate[6];
    char exec_id[10];
    char account[10];
    char db_exec_id[10];
    char clordid[17];
};

struct utp_rej {
    uint16_t type;
    uint16_t len;
    uint32_t seq;
    uint32_t me_ord_id;
    uint32_t tt;
    uint16_t rej_rea;
    char rej_msg_type;
    char deliver_comp[5];
    char target_sub[5];
    char account[10];
    char clordid[17];
    char orig_clordid[17];
    char text[40];
    char filler[3];
};

typedef struct utp_executed execution_t;

#pragma pack(pop)
struct utp_reflector {
    int (*on_data_proc) (struct utp_reflector *, int, char *, size_t);
};
static void partial_fill(char* buffer, int fd)
{
    struct utp_order *ord =(struct utp_order *) buffer;
    struct utp_executed exe;
    memset(&exe, '\0', sizeof(struct utp_executed)); 
    exe.type = 0x0081;
    exe.type = htons(exe.type);
    exe.len = 116;
    exe.len = htons(exe.len);
    exe.leavesqty = ord->qty / 2;
    exe.last_shares = ord->qty / 2;
    exe.last_price = ord->price;
    exe.price_scale = '4';
    exe.side = ord->side;
    exe.billing = '2';
    exe.last_market = 'U';
    memcpy(&exe.target_sub , ord->sender_sub, 5);
    memcpy(&exe.account, ord->account, 10);
    memcpy(&exe.clordid , ord->clordid, 17);
    send_all(fd, (char *) &exe, sizeof(struct utp_executed));


    struct utp_out cancel;
    memset(&cancel, '\0', sizeof(struct utp_out));
    cancel.type = 0x00D1;
    cancel.type = htons(cancel.type);
    cancel.len = 56;
    cancel.len = htons(cancel.len);
    memcpy(&cancel.target_sub , ord->sender_sub, 5);
    memcpy(&cancel.account, ord->account, 10);
    memcpy(&cancel.orig_clordid , ord->clordid, 17);
    send_all(fd, (char *) &cancel, sizeof(struct utp_out));
}
static void fill_order(char* buffer, int fd)
{
    struct utp_cancel *ord =(struct utp_cancel *) buffer;
    struct utp_executed exe;
    memset(&exe, '\0', sizeof(struct utp_executed)); 
    exe.type = 0x0081;
    exe.type = htons(exe.type);
    exe.len = 116;
    exe.len = htons(exe.len);
    exe.leavesqty = 0;
    exe.last_shares = 100;
    exe.last_shares = htonl(exe.last_shares);
    exe.last_price = 4435;
    exe.last_price = htonl(exe.last_price);
    exe.price_scale = '4';
    exe.side = '1';
    exe.billing = '2';
    exe.last_market = 'U';
    memcpy(&exe.target_sub , ord->sender_sub, 5);
    memcpy(&exe.account, ord->account, 10);
    memcpy(&exe.clordid , ord->orig_clordid, 17);
    send_all(fd, (char *) &exe, sizeof(struct utp_executed));
    struct utp_rej rej;
    memset(&rej, '\0', sizeof(struct utp_rej));
    rej.type = 0x00F1;
    rej.type = htons(rej.type);
    rej.len = 116;
    rej.len = htons(rej.len);
    rej.rej_msg_type = 0x32;
    rej.rej_rea = 4004;
    memcpy(rej.target_sub , ord->sender_sub, 5);
    memcpy(rej.account , ord->account, 10);
    memcpy(rej.clordid , ord->clordid, 17);
    memcpy(rej.orig_clordid , ord->orig_clordid, 17);
    send_all(fd, (char *) &rej, sizeof(struct utp_rej));
}

static void ack_order(char* buffer, int fd)
{
    struct utp_order* ord = (struct utp_order*)buffer;
    struct utp_accept acc;
    memset(&acc, '\0', sizeof(struct utp_accept));
    acc.type = htons(0x0091);
    acc.len = htons(56);
    memcpy(&acc.target_sub , ord->sender_sub, 5);
    memcpy(&acc.account, ord->account, 10);
    memcpy(&acc.clordid , ord->clordid, 17);
    send_all(fd, (char *) &acc, sizeof(struct utp_accept));
    if(ord->tif == '3') {
        struct utp_out cancel;
        memset(&cancel, '\0', sizeof(struct utp_out));
        cancel.type = 0x00D1;
        cancel.type = htons(cancel.type);
        cancel.len = 56;
        cancel.len = htons(cancel.len);
        memcpy(&cancel.target_sub , ord->sender_sub, 5);
        memcpy(&cancel.account, ord->account, 10);
        memcpy(&cancel.orig_clordid , ord->clordid, 17);
        send_all(fd, (char *) &cancel, sizeof(struct utp_out));
    }
}

static void cancel_order(char* buffer, int fd)
{
    struct utp_cancel *ord =(struct utp_cancel *) buffer;
    struct utp_out cancel;
    memset(&cancel, '\0', sizeof(struct utp_out));
    cancel.type = 0x00D1;
    cancel.type = htons(cancel.type);
    cancel.len = 56;
    cancel.len = htons(cancel.len);
    memcpy(&cancel.target_sub , ord->sender_sub, 5);
    memcpy(&cancel.account, ord->account, 10);
    memcpy(&cancel.orig_clordid , ord->orig_clordid, 17);
    send_all(fd, (char *) &cancel, sizeof(struct utp_out));
}
void replace_order(char* buffer , int fd)
{
    struct utp_replace *rep =(struct utp_replace *) buffer;
    struct utp_rep_ack rep_ack;
    memset(&rep_ack, '\0', sizeof(struct utp_rep_ack));
    rep_ack.type = 0x00B1;
    rep_ack.type = htons(rep_ack.type);
    rep_ack.len = 56;
    rep_ack.len = htons(rep_ack.len);
    memcpy(rep_ack.target_sub , rep->sender_sub, 5);
    memcpy(rep_ack.account , rep->account, 10);
    memcpy(rep_ack.orig_clordid , rep->orig_clordid, 17);
    send_all(fd, (char *) &rep_ack, sizeof(struct utp_rep_ack));
    struct utp_replaced replace;
    memset(&replace, '\0', sizeof(struct utp_replaced));
    replace.type = 0x00E1;
    replace.type = htons(replace.type);
    replace.len = 60;
    replace.len = htons(replace.len);
    replace.leavesqty = rep->qty;
    memcpy(replace.target_sub , rep->sender_sub, 5);
    memcpy(replace.account , rep->account, 10);
    memcpy(replace.clordid , rep->clordid, 17);
    send_all(fd, (char *) &replace, sizeof(struct utp_replaced));
}
int on_order(struct utp_reflector *reflector, int fd, char *buffer,
             size_t nbytes)
{
    long total = 0;
    do {
        uint16_t type = 0;
        memcpy(&type, buffer, 2);
        type = ntohs(type);
        uint16_t nmsgbytes = 0;
        memcpy(&nmsgbytes, buffer + 2, 2);
        nmsgbytes = ntohs(nmsgbytes);
        if (nbytes < nmsgbytes) {
            return total;
        }
        ++message_num;
        switch(type) {
            case 0x0041:
                ack_order(buffer, fd);
                partial_fill(buffer, fd);
                break;
            case 0x0061:
                if(message_num % 2) {
                    cancel_order(buffer, fd);
                } //else {
                   // fill_order(buffer, fd);
               // }
                break;
            case 0x0071:
                replace_order(buffer, fd);
                break;
        };
        total += nmsgbytes;
        buffer += nmsgbytes;
        nbytes -= nmsgbytes;
    } while(nbytes > 0);
    return total;
}

int on_login(struct utp_reflector *reflector, int fd, char *buffer,
             size_t nbytes)
{
    if (nbytes < sizeof(struct login)) {
        return 0;
    }
    /* We are just going to accept all logons */
    //struct login_accepted accept;
    uint16_t type = 0;
    memcpy(&type, buffer, 2);
    type = ntohs(type);
    if (type != 0x0021) {
        fprintf(stderr, "login packet expected\n");
        return -1;
    }
    reflector->on_data_proc = on_order;
    if (send_all(fd, buffer, 60) != -1) {
        return 60;
    } else {
        return -1;
    }
}

void utp_session_event_loop(int fd)
{
    const size_t initial_readbuf_size = 4096;
    char *readbuf = malloc(initial_readbuf_size);
    char *readbuf_end = readbuf + initial_readbuf_size;
    char *wr_ptr = readbuf, *rd_ptr = readbuf;
    ssize_t nread;
    struct utp_reflector reflector = {
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
        fprintf(stderr, "usage: utp-reflector listenport\n");
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
        utp_session_event_loop(clientfd);
        if (shutdown(clientfd, SHUT_RDWR) == -1) {
            perror("shutting down client socket");
            return EXIT_FAILURE;
        }
        if (close(clientfd) == -1) {
            perror("closing client fd");
            return EXIT_FAILURE;
        }
    }
    return 0;
}
