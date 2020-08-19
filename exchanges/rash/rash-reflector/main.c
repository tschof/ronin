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
#include <stddef.h>

int sendAll(int fd, const char *p, size_t n)
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

static uint64_t refNum = 0;

#pragma pack(push,1)

struct SoupLogin {
    char packetType;
    char username[6];
    char password[10];
    char requestedSession[10];
    char requestedSequenceNumber[10];
    char terminatingLinefeed;
};

struct SoupLoginAccepted {
    char packetType;
    char session[10];
    char sequenceNumber[10];
    char terminatingLinefeed;
};

struct RashOrder {
    char soupPacketType;
    char rashType;
    char orderToken[14];
    char side;
    char shares[6];
    char symbol[8];
    char price[10];
    char timeInForce[5];
    char firm[4];
    char display;
    char minQty[6];
    char maxFloor[6];
    char pegType;
    char pegDifferenceSign;
    char pegDifference[10];
    char discretionPrice[10];
    char discretionPegType;
    char discretionPegDifferenceSign;
    char discretionPegDifference[10];
    char capacity;
    char randomReserve[6];
    char routeDest[4];
    char cust[32];
    char terminatingLinefeed;
};

struct RashOrderExt {
    char soupPacketType;
    char rashType;
    char orderToken[14];
    char side;
    char shares[6];
    char symbol[8];
    char price[10];
    char timeInForce[5];
    char firm[4];
    char display;
    char minQty[6];
    char maxFloor[6];
    char pegType;
    char pegDifferenceSign;
    char pegDifference[10];
    char discretionPrice[10];
    char discretionPegType;
    char discretionPegDifferenceSign;
    char discretionPegDifference[10];
    char capacity;
    char randomReserve[6];
    char routeDest[4];
    char cust[32];
    char isoEligible;
    char crossType;
    char terminatingLinefeed;
};

struct RashAcceptOrder {
    char soupPacketType;
    char timestamp[8];
    char rashType;
    char orderToken[14];
    char side;
    char shares[6];
    char symbol[8];
    char price[10];
    char timeInForce[5];
    char firm[4];
    char display;
    char orderReferenceNumber[9];
    char minQty[6];
    char maxFloor[6];
    char pegType;
    char pegDifferenceSign;
    char pegDifference[10];
    char discretionPrice[10];
    char discretionPegType;
    char discretionPegDifferenceSign;
    char discretionPegDifference[10];
    char capacity;
    char randomReserve[6];
    char routeDest[4];
    char cust[32];
    char terminatingLinefeed;
};

struct RashAcceptOrderExt {
    char soupPacketType;
    char timestamp[8];
    char rashType;
    char orderToken[14];
    char side;
    char shares[6];
    char symbol[8];
    char price[10];
    char timeInForce[5];
    char firm[4];
    char display;
    char orderReferenceNumber[9];
    char minQty[6];
    char maxFloor[6];
    char pegType;
    char pegDifferenceSign;
    char pegDifference[10];
    char discretionPrice[10];
    char discretionPegType;
    char discretionPegDifferenceSign;
    char discretionPegDifference[10];
    char capacity;
    char randomReserve[6];
    char routeDest[4];
    char cust[32];
    char isoEligible;
    char crossType;
    char terminatingLinefeed;
};

struct RashCancelOrder {
    char soupPacketType;
    char rashMessageType;
    char orderToken[14];
    char shares[6];
    char terminatingNewline;
};

struct RashOrderCancelled {
    char soupPacketType;
    char timeStamp[8];
    char rashMessageType;
    char orderToken[14];
    char shares[6];
    char reason;
    char terminatingNewline;
};

int setTimeStamp(char *p, size_t n, struct timeval *tv)
{
    static time_t midnightToday = (time_t) - 1;
    if (midnightToday == -1) {
        /* Initialize midnight */
        midnightToday = time(0);
        struct tm now;
        if (localtime_r(&midnightToday, &now) == 0) {
            return -1;
        }
        now.tm_hour = 0;
        now.tm_min = 0;
        now.tm_sec = 0;
        if ((midnightToday = mktime(&now)) == -1) {
            return -1;
        }
        if (timezone == 21600) {
            midnightToday -= 3600;
        }
    }
    uint64_t millisecondsSinceMidnight = tv->tv_sec - midnightToday;
    millisecondsSinceMidnight *= 1000;
    millisecondsSinceMidnight += tv->tv_usec / 1000;
    char tmp[n + 1];
    int nwritten = snprintf(tmp, sizeof tmp, "%0*zu", (int) n,
                            millisecondsSinceMidnight);
    if (nwritten < 0 || nwritten >= sizeof tmp) {
        return -1;
    }
    memcpy(p, tmp, n);
    return 0;
}

int setTimeStampFromNow(char *p, size_t n)
{
    struct timeval tv;
    if (gettimeofday(&tv, 0) == -1) {
        return -1;
    }
    return setTimeStamp(p, n, &tv);
}

int initCancelled(struct RashCancelOrder *order,
                  struct RashOrderCancelled *cancelled)
{
    cancelled->soupPacketType = 'S';
    if (setTimeStampFromNow
        ((char *) &cancelled->timeStamp,
         sizeof cancelled->timeStamp) == -1) {
        return -1;
    }
    cancelled->rashMessageType = 'C';
    memcpy(&cancelled->orderToken, &order->orderToken,
           sizeof cancelled->orderToken);
    memset(&cancelled->shares, '0', sizeof cancelled->shares);
    cancelled->reason = 'U';    /* User requested cancel */
    cancelled->terminatingNewline = '\n';
    return 0;
}

int setUnsignedInteger(char *p, size_t n, uint64_t integer)
{
    int result = -1;
    char tmp[n + 1];
    int nwritten =
        snprintf(tmp, sizeof tmp, "%0*zu", (int) sizeof tmp, integer);
    if (nwritten != -1 && nwritten <= sizeof tmp) {
        memcpy(p, tmp, nwritten);
        result = 0;
    }
    return result;
}

int initOrderAccept(struct RashOrder *order,
                    struct RashAcceptOrder *accept)
{
    accept->soupPacketType = 'S';
    if (setTimeStampFromNow((char *) &accept->timestamp,
                            sizeof accept->timestamp) == -1) {
        return -1;
    }
    accept->rashType = 'A';
    memcpy(&accept->orderToken, &order->orderToken,
           offsetof(struct RashAcceptOrder, orderReferenceNumber)
           -offsetof(struct RashAcceptOrder, orderToken));
    if (setUnsignedInteger
        ((char *) &accept->orderReferenceNumber,
         sizeof accept->orderReferenceNumber, refNum++) == -1) {
        return -1;
    }
    memcpy(&accept->minQty, &order->minQty,
           sizeof(struct RashAcceptOrder) -
           offsetof(struct RashAcceptOrder, minQty));
    return 0;
}

int initOrderAcceptExt(struct RashOrderExt *order,
                       struct RashAcceptOrderExt *accept)
{
    if (initOrderAccept
        ((struct RashOrder *) order,
         (struct RashAcceptOrder *) accept) == -1) {
        return -1;
    }
    accept->rashType = 'R';
    accept->isoEligible = order->isoEligible;
    accept->crossType = order->crossType;
    return 0;
}

struct rash_cancel {
    char type;
    char order_token[14];
    uint32_t shares;
};

struct rash_canceled {
    char packetType;
    char type;
    uint64_t timestamp;
    char order_token[14];
    uint32_t decrement_shares;
    char reason;                /* U: User cancel */
};

#pragma pack(pop)

struct RashReflector {
    int (*onMessage) (struct RashReflector *, int, char *, size_t);
};

int initRefNum(uint64_t * refnum)
{
    int result = -1;
    struct timeval tv;
    if (gettimeofday(&tv, 0) != -1) {
        struct tm today;
        if (localtime_r(&tv.tv_sec, &today) != 0) {
            today.tm_hour = today.tm_min = today.tm_sec = 0;
            time_t midnight = mktime(&today);
            *refnum = tv.tv_sec - midnight;
            *refnum *= 1000;
            *refnum += tv.tv_usec / 1000;
            result = 0;
        }
    }
    return result;
}

int onOrder(struct RashReflector *reflector, int fd, char *buffer,
            size_t nbytes)
{
    int result = -1;
    char soupPacketType = buffer[0];
    if (soupPacketType == 'R') {
        return 2;
    }
    if (soupPacketType == 'U') {
        char rashMessageType = buffer[1];
        if (rashMessageType == 'O') {
            fprintf(stderr, "Unsupported RASH message type 'O'");
        } else if (rashMessageType == 'Q') {
            struct RashOrderExt *order = (struct RashOrderExt *) buffer;
            struct RashAcceptOrderExt accept;
            if (initOrderAcceptExt(order, &accept) != -1) {
                accept.terminatingLinefeed = '\n';
                if (sendAll(fd, (const char *) &accept, sizeof accept) !=
                    -1) {
                    result = (int) sizeof accept;
                }
            }
        } else if (rashMessageType == 'X') {
            struct RashCancelOrder *cancel =
                (struct RashCancelOrder *) buffer;
            struct RashOrderCancelled cancelled;
            if (initCancelled(cancel, &cancelled) != -1
                && sendAll(fd, (const char *) &cancelled,
                           sizeof cancelled) != -1) {
                result = sizeof cancelled;
            }
        }
    } else {
        fprintf(stderr, "Buffer is not unsequenced SOUP packet: '%.*s'\n",
                (int) nbytes, buffer);
    }
    return result;
}

int
onLogin(struct RashReflector *reflector, int fd, char *buffer,
        size_t nbytes)
{
    if (nbytes < sizeof(struct SoupLogin)) {
        return 0;
    }
    /* We are just going to accept all logons */
    struct SoupLoginAccepted accept;
    struct SoupLogin *loginp = (struct SoupLogin *) buffer;
    if (loginp->packetType != 'L') {
        fprintf(stderr, "login packet expected\n");
        return -1;
    }
    memset(&accept, ' ', sizeof accept);
    accept.terminatingLinefeed = '\n';
    accept.packetType = 'A';
    memcpy(accept.sequenceNumber, loginp->requestedSequenceNumber,
           sizeof accept.sequenceNumber);
    reflector->onMessage = onOrder;
    if (sendAll(fd, (char *) &accept, sizeof accept) != -1) {
        return sizeof *loginp;
    } else {
        return -1;
    }
}

void rashSessionEventLoop(int fd)
{
    const size_t initial_readbuf_size = 4096;
    char *readbuf = malloc(initial_readbuf_size);
    char *readbuf_end = readbuf + initial_readbuf_size;
    char *wr_ptr = readbuf, *rd_ptr = readbuf;
    ssize_t nread;
    struct RashReflector reflector = {
        .onMessage = onLogin,
    };
    while ((nread = read(fd, wr_ptr, readbuf_end - wr_ptr)) > 0) {
        wr_ptr += nread;
        char *newline;
        while ((newline = memchr(rd_ptr, '\n', wr_ptr - rd_ptr)) != 0) {
            ++newline;
            if (reflector.onMessage(&reflector, fd, readbuf,
                                    newline - rd_ptr) == -1) {
                fprintf(stderr, "Error processing message\n");
            }
            rd_ptr = newline;
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
    if (argc < 2) {
        fprintf(stderr, "usage: rash-reflector listenport\n");
        return EXIT_FAILURE;
    }
    if (initRefNum(&refNum) == -1) {
        fprintf(stderr, "Cannot initialize reference number\n");
        exit(EXIT_FAILURE);
    }
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
        rashSessionEventLoop(clientfd);
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
