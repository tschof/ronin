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

int send_all(int fd, const char *p, size_t n) {
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

#pragma pack(pop)

static uint64_t refnum = 0;

void initialize_accept_from_order(struct ouch_order *order,
                                  struct ouch_accept *accept) {
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
    accept->refnumber = refnum++;
    accept->iso_eligible = order->iso_eligible;
    accept->minqty = order->minqty;
    accept->crosstype = order->crosstype;
    accept->orderstate = 'L';
}

void initialize_canceled_from_cancel(struct ouch_cancel *cancel,
                                     struct ouch_canceled *canceled) {
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

typedef struct reflector {
    int (*frame) (struct reflector *, void *, size_t);
    int (*on_message) (struct reflector *, int, void *, size_t);
    void *data;
} reflector_t;

int on_order(struct reflector *reflector, int fd, void *buf, size_t nbytes) {
    char *buffer = buf;
    if (nbytes < sizeof(struct soupbin_header)) {
        return 0;
    }
    struct soupbin_header *header = (struct soupbin_header *) buffer;
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
    }
    return ntotalbytes;
}

#pragma pack(push, 1)

typedef struct BATS_Binary_Header {
    uint16_t start_of_message;
    uint16_t message_length;
    uint8_t message_type;
} bats_header_t;

typedef struct BATS_Binary_Login_Response {
    bats_header_t header;
    uint8_t matching_unit;
    uint32_t sequence_number;
    char login_response_status;
    char login_response_text[60];
    uint8_t no_unspecified_unit_replay;
    uint8_t empty[88];
    uint32_t last_received_sequence_number;
    uint8_t number_of_units;
} login_response_t;

typedef struct BATS_Binary_Replay_Complete {
    bats_header_t header;
    uint8_t matching_unit;
    uint32_t sequence_number;
} replay_complete_t;

#pragma pack(pop)

void login_response__init(login_response_t * lr) {
    memset(lr, 0, sizeof *lr);
    lr->header.start_of_message = 0xbaba;
    lr->header.message_length =
        sizeof *lr - sizeof lr->header.start_of_message;
    lr->header.message_type = 0x07;
    lr->login_response_status = 'A';
    strcpy(lr->login_response_text,
           "Hello from the BATS Binary Reflector");
}

void replay_complete__init(replay_complete_t * rc) {
    memset(rc, 0, sizeof *rc);
    rc->header.start_of_message = 0xbaba;
    rc->header.message_length =
        sizeof *rc - sizeof rc->header.start_of_message;
    rc->header.message_type = 0x13;
}

int send_from_file(reflector_t * reflector, int fd, void *p, size_t n) {
    return -1;
}

int on_login(struct reflector *reflector, int fd, void *p, size_t n) {
    bats_header_t *header = p;
    if (header->message_type != 0x01) {
        return -1;
    }
    login_response_t lr;
    login_response__init(&lr);
    if (send_all(fd, (char *) &lr, sizeof lr) == -1) {
        return -1;
    }
    replay_complete_t rc;
    replay_complete__init(&rc);
    if (send_all(fd, (char *) &rc, sizeof rc) == -1) {
        return -1;
    }
    reflector->on_message = send_from_file;
    return 0;
}

typedef struct buffer {
    void *buffer, *rd_ptr, *wr_ptr;
    size_t buffer_size, low_water_mark, growby;
} buffer_t;

buffer_t *make_buffer(size_t initial_buffer_size, size_t low_water_mark,
                      size_t growby) {
    buffer_t *buf = malloc(sizeof *buf);
    buf->buffer_size = initial_buffer_size;
    buf->buffer = buf->rd_ptr = buf->wr_ptr = malloc(buf->buffer_size);
    buf->low_water_mark = low_water_mark;
    buf->growby = growby;
    return buf;
}

size_t get_writeable_space(buffer_t *buf) {
    return buf->buffer + buf->buffer_size - buf->wr_ptr;
}

size_t get_readable_space(buffer_t *buf) {
    return buf->wr_ptr - buf->rd_ptr;
}

int grow_buffer(buffer_t *buf) {
    int retval = -1;
    size_t space_left = get_writeable_space(buf);
    if (space_left < buf->low_water_mark) {
        buffer_t tmp = *buf;
        tmp.buffer_size += tmp.growby;
        tmp.buffer = realloc(tmp.buffer, tmp.buffer_size);
        if (tmp.buffer != buf->buffer) {
            tmp.rd_ptr = tmp.buffer + (buf->rd_ptr - buf->buffer);
            tmp.wr_ptr = tmp.buffer + (buf->wr_ptr - buf->buffer);
        }
        *buf = tmp;
        retval = 0;
    }
    return retval;
}

void prep_writeable_space(buffer_t *buf) {
    ssize_t writeable_space = get_writeable_space(buf);
    if (writeable_space < buf->low_water_mark) {
        size_t readable_space = get_readable_space(buf);
        memmove(buf->buffer, buf->rd_ptr, readable_space);
        buf->wr_ptr = buf->rd_ptr + readable_space;
        writeable_space = get_writeable_space(buf);
        if (writeable_space < buf->low_water_mark) {
            writeable_space = grow_buffer(buf);
        }
    }
}

void increment_write_ptr(buffer_t *buf, size_t n) {
    buf->wr_ptr += n;
}

/* Returns 0 for an incomplete message indicating the framework should
 * wait for more data and then call again; -1 upon error indicating
 * the data does not match expectations and the connection should be
 * dropped; or n > 0 to indicate a valid message of n bytes is
 * present, as counted from the first byte pointed to by p. */
int frame(reflector_t * r, void *p, size_t n) {
    if (n < sizeof(bats_header_t)) {
        return 0;
    }
    bats_header_t *header = p;
    if (header->start_of_message != 0xbaba) {
        return -1;
    }
    size_t total_length = header->message_length + sizeof header->start_of_message;
    if (n < total_length) {
        return 0;
    }
    return (int) total_length;
}

void session_event_loop2(int fd, reflector_t *reflector) {
    buffer_t *buf = make_buffer(4096, 512, 1024);
    ssize_t nread;
    while ((nread = read(fd, buf->wr_ptr, get_writeable_space(buf))) > 0) {
        
        prep_writeable_space(buf);
    }
}

void session_event_loop(int fd) {
    const size_t initial_readbuf_size = 4096;
    char *readbuf = malloc(initial_readbuf_size);
    char *readbuf_end = readbuf + initial_readbuf_size;
    char *wr_ptr = readbuf, *rd_ptr = readbuf;
    ssize_t nread;
    struct reflector reflector = {
        .frame = frame,
        .on_message = on_login,
        .data = 0,
    };
    while ((nread = read(fd, wr_ptr, readbuf_end - wr_ptr)) > 0) {
        wr_ptr += nread;
        int nframed, nprocessed = -1;
        while ((nframed =
                reflector.frame(&reflector, readbuf,
                                wr_ptr - rd_ptr)) > 0) {
            if ((nprocessed =
                 reflector.on_message(&reflector, fd, readbuf,
                                      nframed)) == -1) {
                break;
            }
            rd_ptr += nframed;
            size_t nunread = wr_ptr - rd_ptr;
            memmove(readbuf, rd_ptr, nunread);
            rd_ptr = readbuf;
            wr_ptr = readbuf + nunread;
            if ((wr_ptr - rd_ptr) == 0) {
                break;
            }
        }
        if (nprocessed == -1) {
            printf("Detected error, killing the connection\n");
            goto cleanup;
        }
    }
    if (nread == 0) {
        fprintf(stderr, "peer closed its connection\n");
    } else {
        perror("reading socket");
    }
  cleanup:
    free(readbuf);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage: ouch-reflector listenport\n");
        return EXIT_FAILURE;
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
        session_event_loop(clientfd);
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
