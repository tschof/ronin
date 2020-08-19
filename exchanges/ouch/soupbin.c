#include "soupbin.h"
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

int soupbin_serialize(struct databuf *buffer, void *p, uint16_t n)
{
    int retval = -1;
    struct soupbin_header header = {
        .packet_length = htons(n + 1),
        .packet_type = 'U',
    };
    if ((retval = databuf_memcpy(buffer, &header, sizeof header)) != -1)
        retval = databuf_memcpy(buffer, p, n);
    return retval;
}

uint16_t soupbin_frame_message(databuf_t * dbuff)
{
    uint16_t retval = 0;
    size_t nbytes = databuf_unread(dbuff);
    if (nbytes >= sizeof(struct soupbin_header)) {
        struct soupbin_header *header =
            (struct soupbin_header *) dbuff->rd_ptr;
        retval =
            ntohs(header->packet_length) + sizeof header->packet_length;
    }
    return retval;
}

size_t soupbin__frame(const char *p, size_t n)
{
    size_t retval = 0;
    if (n >= sizeof(struct soupbin_header)) {
        struct soupbin_header *header = (struct soupbin_header *) p;
        size_t packet_length = ntohs(header->packet_length);
        if (n >= packet_length)
            retval = packet_length;
    }
    return retval;
}

int soupbin_heartbeat_init(struct soupbin_header *heartbeat)
{
    heartbeat->packet_length = sizeof heartbeat->packet_type;
    /* Since this value never changes we are simply converting the
     * byte order here once so it does not need to be done in the
     * serialize function each time. */
    heartbeat->packet_length = htons(heartbeat->packet_length);
    heartbeat->packet_type = SOUPBIN_PACKET_CLIENT_HEARTBEAT;
    return 0;
}

int soupbin_heartbeat_serialize(struct soupbin_header *heartbeat,
                                databuf_t * buffer)
{
    return databuf_memcpy(buffer, heartbeat, sizeof *heartbeat);
}

int soupbin_login_init(struct soupbin_login *login, const char *username,
                       const char *password)
{
    int retval = -1;
    memset(login, ' ', sizeof *login);
    size_t len = strlen(username);
    if (len <= sizeof login->username) {
        memcpy(login->username, username, len);
        len = strlen(password);
        if (len <= sizeof login->password) {
            memcpy(login->password, password, len);
            login->header.packet_length =
                sizeof *login - sizeof login->header.packet_length;
            login->header.packet_length =
                htons(login->header.packet_length);
            login->header.packet_type = SOUPBIN_PACKET_TYPE_LOGIN;
            retval = 0;
        }
    }
    return retval;
}

int soupbin__login__set_sequence_number(struct soupbin_login *login,
                                        unsigned long int seqno)
{
    int retval = -1;
    retval = snprintf(login->requested_sequence_number,
                      sizeof login->requested_sequence_number - 1,
                      "%lu", seqno);
    if (retval != -1
        && retval < sizeof login->requested_sequence_number - 1) {
        /* Field is space-padded */
        login->requested_sequence_number[retval] = ' ';
        retval = 0;
    }
    return retval;
}

int soupbin_packet_to_str(struct soupbin_header *header,
                          struct databuf *str,
                          int (*print_payload) (void *, size_t,
                                                struct databuf *))
{
    int retval = -1;
    uint16_t packet_length = ntohs(header->packet_length);
    if ((retval =
         databuf_write(str, "SoupBinTCP Packet Length = '%d' ",
                       (int) packet_length)) == -1)
        goto done;
    switch (header->packet_type) {
        /* Server Packets */
    case '+':
        {
            const char *p = (const char *) header;
            p += sizeof *header;
            databuf_write(str, "DEBUG: %.*s", (int) (packet_length - 1),
                          p);
        }
        break;
    case 'A':
        {
            const struct soupbin_login_accepted *login_accepted =
                (struct soupbin_login_accepted *) header;
            databuf_write(str,
                          "LOGIN ACCEPTED: Session='%.*s' "
                          "Sequence Number='%.*s'",
                          (int) sizeof login_accepted->session,
                          login_accepted->session,
                          (int) sizeof login_accepted->sequence_number,
                          login_accepted->sequence_number);
        }
        break;
    case 'J':
        {
            const struct soupbin_login_rejected *login_rejected =
                (struct soupbin_login_rejected *) header;
            retval =
                databuf_write(str,
                              "LOGIN REJECTED: Reject Reason Code='%c'",
                              login_rejected->reject_reason_code);
        }
    case 'S':
        retval = databuf_write(str, "SEQUENCED DATA: ");
        break;
    case 'H':
        retval = databuf_write(str, "SERVER HEARTBEAT");
        break;
    case 'Z':
        retval = databuf_write(str, "END OF SESSION");
        break;

        /* Client Packets */
    case 'L':
        {
            struct soupbin_login *login = (struct soupbin_login *) header;
            retval = databuf_write(str, "LOGIN REQUEST: "
                                   "Username = '%.*s'; "
                                   "Password = '%.*s'; "
                                   "Requested Session = '%.*s'; "
                                   "Requested Sequence Number = '%.*s'",
                                   (int) sizeof login->username,
                                   login->username,
                                   (int) sizeof login->password,
                                   login->password,
                                   (int) sizeof login->requested_session,
                                   login->requested_session,
                                   (int) sizeof
                                   login->requested_sequence_number,
                                   login->requested_sequence_number);
        }
        break;
    case 'U':
        retval = databuf_write(str, "UNSEQUENCED DATA: ");
        break;
    case 'R':
        retval = databuf_write(str, "CLIENT HEARTBEAT");
        break;
    case 'O':
        retval = databuf_write(str, "LOGOUT REQUEST");
        break;
    }
  done:
    return retval;
}

int soupbin__print_packet(void *p, size_t n, struct databuf *logbuf,
                          void *print_payload_function)
{
    int retval = -1;
    int (*print_payload) (void *, size_t, struct databuf *, void *) =
        print_payload_function;
    struct soupbin_header *header = p;
    uint16_t packet_length = ntohs(header->packet_length);
    if ((retval =
         databuf_write(logbuf, "SoupBinTCP Packet Length = '%d' ",
                       (int) packet_length)) == -1)
        goto done;
    switch (header->packet_type) {
        /* Server Packets */
    case '+':
        {
            const char *p = (const char *) header;
            p += sizeof *header;
            databuf_write(logbuf, "DEBUG: %.*s", (int) (packet_length - 1),
                          p);
        }
        break;
    case 'A':
        {
            const struct soupbin_login_accepted *login_accepted =
                (struct soupbin_login_accepted *) header;
            databuf_write(logbuf,
                          "LOGIN ACCEPTED: Session='%.*s' "
                          "Sequence Number='%.*s'",
                          (int) sizeof login_accepted->session,
                          login_accepted->session,
                          (int) sizeof login_accepted->sequence_number,
                          login_accepted->sequence_number);
        }
        break;
    case 'J':
        {
            const struct soupbin_login_rejected *login_rejected =
                (struct soupbin_login_rejected *) header;
            retval =
                databuf_write(logbuf,
                              "LOGIN REJECTED: Reject Reason Code='%c'",
                              login_rejected->reject_reason_code);
        }
    case 'S':
        if ((retval = databuf_write(logbuf, "SEQUENCED DATA: ")) != -1)
            retval = print_payload(p + sizeof *header, n - sizeof *header,
                                   logbuf, 0);
        break;
    case 'H':
        retval = databuf_write(logbuf, "SERVER HEARTBEAT");
        break;
    case 'Z':
        retval = databuf_write(logbuf, "END OF SESSION");
        break;

        /* Client Packets */
    case 'L':
        {
            struct soupbin_login *login = (struct soupbin_login *) header;
            retval =
                databuf_write(logbuf,
                              "LOGIN REQUEST: " "Username = '%.*s'; "
                              "Password = '%.*s'; "
                              "Requested Session = '%.*s'; "
                              "Requested Sequence Number = '%.*s'",
                              (int) sizeof login->username,
                              login->username,
                              (int) sizeof login->password,
                              login->password,
                              (int) sizeof login->requested_session,
                              login->requested_session,
                              (int) sizeof
                              login->requested_sequence_number,
                              login->requested_sequence_number);
        }
        break;
    case 'U':
        if ((retval = databuf_write(logbuf, "UNSEQUENCED DATA: ")) != -1)
            return print_payload(p + sizeof *header, n - sizeof *header,
                                 logbuf, 0);
        break;
    case 'R':
        retval = databuf_write(logbuf, "CLIENT HEARTBEAT");
        break;
    case 'O':
        retval = databuf_write(logbuf, "LOGOUT REQUEST");
        break;
    }
  done:
    return retval;
}

int soupbin__dump_data(void *p, size_t n, struct databuf *buffer,
                       dump_inner_data dump_message)
{
    int retval = -1;
    struct soupbin_header *header = p;
    if (n < sizeof *header)
        return -1;
    uint16_t packet_length = ntohs(header->packet_length);
    if ((retval =
         databuf_write(buffer, "soupbin packet %02d bytes ",
                       (int) packet_length)) == -1)
        goto done;
    switch (header->packet_type) {
        /* Server Packets */
    case '+':
        {
            const char *p = (const char *) header;
            p += sizeof *header;
            databuf_write(buffer, "DEBUG: %.*s", (int) (packet_length - 1),
                          p);
        }
        break;
    case 'A':
        {
            const struct soupbin_login_accepted *login_accepted =
                (struct soupbin_login_accepted *) header;
            databuf_write(buffer,
                          "LOGIN ACCEPTED: Session='%.*s' "
                          "Sequence Number='%.*s'",
                          (int) sizeof login_accepted->session,
                          login_accepted->session,
                          (int) sizeof login_accepted->sequence_number,
                          login_accepted->sequence_number);
        }
        break;
    case 'J':
        {
            const struct soupbin_login_rejected *login_rejected =
                (struct soupbin_login_rejected *) header;
            retval =
                databuf_write(buffer,
                              "LOGIN REJECTED: Reject Reason Code='%c'",
                              login_rejected->reject_reason_code);
        }
    case 'S':
        if ((retval = databuf_write(buffer, "SEQUENCED DATA: ")) != -1)
            retval = dump_message(p + sizeof *header, n - sizeof *header,
                                  buffer);
        break;
    case 'H':
        /* retval = databuf_write(buffer, "SERVER HEARTBEAT"); */
        retval = 0;
        break;
    case 'Z':
        retval = databuf_write(buffer, "END OF SESSION");
        break;

        /* Client Packets */
    case 'L':
        {
            struct soupbin_login *login = (struct soupbin_login *) header;
            retval =
                databuf_write(buffer,
                              "LOGIN REQUEST: " "Username = '%.*s'; "
                              "Password = '%.*s'; "
                              "Requested Session = '%.*s'; "
                              "Requested Sequence Number = '%.*s'",
                              (int) sizeof login->username,
                              login->username,
                              (int) sizeof login->password,
                              login->password,
                              (int) sizeof login->requested_session,
                              login->requested_session,
                              (int) sizeof
                              login->requested_sequence_number,
                              login->requested_sequence_number);
        }
        break;
    case 'U':
        if ((retval = databuf_write(buffer, "UNSEQUENCED DATA: ")) != -1)
            return dump_message(p + sizeof *header, n - sizeof *header,
                                buffer);
        break;
    case 'R':
        /* retval = databuf_write(buffer, "CLIENT HEARTBEAT"); */
        retval = 0;
        break;
    case 'O':
        retval = databuf_write(buffer, "LOGOUT REQUEST");
        break;
    }
  done:
    return retval;
}
