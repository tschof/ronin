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

#pragma pack(push,1)

struct login {
    uint16_t length;
    char type;
    char username[6];
    char password[10];
    char requested_session[10];
    char requested_sequence_number[20];
};

struct login_accepted {
    uint16_t length;
    char type;
    char session[10];
    char sequence_number[20];
};

struct soupbin_header {
    uint16_t length;
    char type;
};

struct ouch_order {
    char type;
    char order_token[14];
    char side;
    uint32_t shares;
    char symbol[8];
    uint32_t price;
    uint32_t tif;
    char firm[4];
    char display;
    char capacity;
    char iso_eligible;
    uint32_t minqty;
    char crosstype;
    char customer;
};

struct ouch_accept {
    struct soupbin_header header;
    char type;
    uint64_t timestamp;
    char order_token[14];
    char side;
    uint32_t shares;
    char symbol[8];
    uint32_t price;
    uint32_t tif;
    char firm[4];
    char display;
    uint64_t refnumber;
    char capacity;
    char iso_eligible;
    uint32_t minqty;
    char crosstype;
    char orderstate;
};

struct ouch_cancel {
    char type;
    char order_token[14];
    uint32_t shares;
};

struct ouch_canceled {
    struct soupbin_header header;
    char type;
    uint64_t timestamp;
    char order_token[14];
    uint32_t decrement_shares;
    char reason;                /* U: User cancel */
};

struct ouch_replace {
    char type;
    char existing_order_token[14];
    char replacement_order_token[14];
    uint32_t shares;
    uint32_t price;
    uint32_t time_in_force;
    char display;
    char intermarket_sweep_eligibility;
    uint32_t minimum_quantity;
};

struct ouch_replaced {
    struct soupbin_header header;
    char type;
    uint64_t timestamp;
    char replacement_order_token[14];
    char buy_sell_indicator;
    uint32_t shares;
    char stock[8];
    uint32_t price;
    uint32_t time_in_force;
    char firm[4];
    char display;
    uint64_t order_reference_number;
    char capacity;
    char intermarket_sweep_eligibility;
    uint32_t minimum_quantity;
    char cross_type;
    char order_state;
    char previous_order_token[14];
};

struct ouch_executed {
    struct soupbin_header header;
    char message_type;
    uint64_t timestamp;
    char order_token[14];
    uint32_t executed_shares;
    uint32_t executed_price;
    char liquidity_flag;
    uint64_t match_number;
};

typedef struct ouch_executed execution_t;

#pragma pack(pop)

static uint64_t refnum = 1, matchnum = 1;

void initialize_accept_from_order(struct ouch_order *order,
                                  struct ouch_accept *accept)
{
    memset(accept, ' ', sizeof *accept);
    accept->header.length = htons(sizeof *accept - sizeof(uint16_t));
    accept->header.type = 'S';
    accept->type = 'A';
    accept->timestamp = 0;
    memcpy(accept->order_token, order->order_token,
           sizeof accept->order_token);
    accept->side = order->side;
    accept->shares = order->shares;
    memcpy(accept->symbol, order->symbol, sizeof accept->symbol);
    accept->price = order->price;
    accept->tif = order->tif;
    accept->display = order->display;
    uint64_t tmprefnum = refnum++;
    accept->refnumber = htobe64(tmprefnum);
    accept->iso_eligible = order->iso_eligible;
    accept->minqty = order->minqty;
    accept->crosstype = order->crosstype;
    accept->orderstate = 'L';
}

void initialize_replaced_from_order(struct ouch_replace *replace,
                                    struct ouch_replaced *replaced) {
    memset(replaced, ' ', sizeof *replaced);
    replaced->header.length = htons(sizeof *replaced - sizeof(uint16_t));
    replaced->header.type = 'S';
    replaced->type = 'U';
    replaced->timestamp = 0;
    memcpy(replaced->replacement_order_token, replace->replacement_order_token,
           sizeof replaced->replacement_order_token);
    memcpy(replaced->previous_order_token, replace->existing_order_token,
           sizeof replaced->previous_order_token);
    replaced->shares = replace->shares;
    replaced->price = replace->price;
    replaced->time_in_force = replace->time_in_force;
    replaced->display = replace->display;
    replaced->intermarket_sweep_eligibility = replace->intermarket_sweep_eligibility;
    replaced->minimum_quantity = replace->minimum_quantity;
    uint64_t tmprefnum = refnum++;
    replaced->order_reference_number = htobe64(tmprefnum);
}

void initialize_execution_from_order(struct ouch_order *order,
                                     struct ouch_executed *execution)
{
    memset(execution, ' ', sizeof *execution);
    execution->header.length =
        htons(sizeof *execution - sizeof execution->header.length);
    execution->header.type = 'S';
    execution->message_type = 'E';
    execution->timestamp = 0;
    memcpy(execution->order_token, order->order_token,
           sizeof execution->order_token);
    execution->executed_shares = order->shares;
    execution->executed_price = order->price;
    execution->liquidity_flag = 'R';    /* For now all executions remove
                                         * liquidity by trading against
                                         * standing orders on the
                                         * book. */
    execution->match_number = htobe64(matchnum++);
}

void initialize_canceled_from_cancel(struct ouch_cancel *cancel,
                                     struct ouch_canceled *canceled)
{
    canceled->header.length =
        htons(sizeof *canceled - sizeof canceled->header.length);
    canceled->header.type = 'S';
    canceled->type = 'C';
    canceled->timestamp = 0;
    memcpy(canceled->order_token, cancel->order_token,
           sizeof canceled->order_token);
    canceled->decrement_shares = 0;
    canceled->reason = 'U';
}

struct ouch_reflector {
    int (*on_data_proc) (struct ouch_reflector *, int, char *, size_t);
};

void initialize_refnum(uint64_t * refnum)
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    struct tm today;
    localtime_r(&tv.tv_sec, &today);
    today.tm_hour = today.tm_min = today.tm_sec = 0;
    *refnum = mktime(&today) * 1000000;
    *refnum += tv.tv_usec;
}

int on_order(struct ouch_reflector *reflector, int fd, char *buffer,
             size_t nbytes)
{
    if (nbytes < sizeof(struct soupbin_header)) {
        return 0;
    }
    struct soupbin_header *header = (struct soupbin_header *) buffer;
    /* fprintf (stderr, "%c\n", header->type); */
    uint16_t nmsgbytes = ntohs(header->length);
    size_t ntotalbytes = nmsgbytes + sizeof header->length;
    if (nbytes < ntotalbytes) {
        return 0;
    }
    if (header->type != 'U') {
        return (int) ntotalbytes;
    }
    char ouchtype = buffer[sizeof(struct soupbin_header)];
    if (ouchtype == 'O') {
        struct ouch_order *order =
            (struct ouch_order *) &buffer[sizeof(struct soupbin_header)];
        struct ouch_accept accept;
        initialize_accept_from_order(order, &accept);
        if (send_all(fd, (const char *) &accept, sizeof accept) != -1) {
            /* For now we are going to also send an execution on the
             * heels of the accept. */
            if (1) {
                execution_t exec;
                initialize_execution_from_order(order, &exec);
                if (send_all(fd, (const char *) &exec, sizeof exec) != -1) {
                    return (int) ntotalbytes;
                } else {
                    return -1;
                }
            }  else {
                return (int) ntotalbytes;
            }
            return (int) ntotalbytes;
        } else {
            return -1;
        }
    } else if (ouchtype == 'X') {
        struct ouch_cancel *cancel =
            (struct ouch_cancel *) &buffer[sizeof(struct soupbin_header)];
        struct ouch_canceled canceled;
        initialize_canceled_from_cancel(cancel, &canceled);
        if (send_all(fd, (const char *) &canceled, sizeof canceled) != -1) {
            return (int) ntotalbytes;
        } else {
            return -1;
        }
    } else if (ouchtype == 'U') {
        struct ouch_replace *replace = (struct ouch_replace *) &buffer[sizeof(struct soupbin_header)];
        struct ouch_replaced replaced;
        initialize_replaced_from_order(replace, &replaced);
        if (send_all(fd, (const char *) &replaced, sizeof replaced) != -1) {
            return (int) ntotalbytes;
        } else {
            return -1;
        }
    }
    return ntotalbytes;
}

int on_login(struct ouch_reflector *reflector, int fd, char *buffer,
             size_t nbytes)
{
    if (nbytes < sizeof(struct login)) {
        return 0;
    }
    /* We are just going to accept all logons */
    struct login_accepted accept;
    struct login *loginp = (struct login *) buffer;
    if (loginp->type != 'L') {
        fprintf(stderr, "login packet expected\n");
        return -1;
    }
    memset(&accept, ' ', sizeof accept);
    accept.length = htons(sizeof accept - sizeof accept.length);
    accept.type = 'A';
    memcpy(accept.sequence_number, loginp->requested_sequence_number,
           sizeof accept.sequence_number);
    reflector->on_data_proc = on_order;
    if (send_all(fd, (char *) &accept, sizeof accept) != -1) {
        return sizeof *loginp;
    } else {
        return -1;
    }
}

void ouch_session_event_loop(int fd)
{
    const size_t initial_readbuf_size = 4096;
    char *readbuf = malloc(initial_readbuf_size);
    char *readbuf_end = readbuf + initial_readbuf_size;
    char *wr_ptr = readbuf, *rd_ptr = readbuf;
    ssize_t nread;
    struct ouch_reflector reflector = {
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
    refnum = rand();
    matchnum = rand();
    if (argc < 2) {
        fprintf(stderr, "usage: ouch-reflector listenport\n");
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
        ouch_session_event_loop(clientfd);
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
