#ifndef __SOUPBIN_H__
#define __SOUPBIN_H__

#include "server_objects/databuf.h"
#include "dump_data.h"

#include <stdint.h>

#pragma pack(push, 1)

/* This is the header of all SoupBin TCP packets.  There are several
 * messages that are composed entirely of a struct soupbin_header with
 * no additional fields.  Those messages are: Server Heartbeat, End of
 * Session, Client Heartbeat, and Logout Request packets. */
struct soupbin_header {
    uint16_t packet_length;
    char packet_type;
};

/* SoupBin TCP Server Packets */
struct soupbin_login_accepted {
    struct soupbin_header header;
    char session[10];
    char sequence_number[20];
};

struct soupbin_login_rejected {
    struct soupbin_header header;
    char reject_reason_code;
};

/* SoupBin TCP Client Packets */
struct soupbin_login {
    struct soupbin_header header;
    char username[6];
    char password[10];
    char requested_session[10];
    char requested_sequence_number[20];
};

#pragma pack(pop)

#define SOUPBIN_PACKET_TYPE_HEARTBEAT 'H'
#define SOUPBIN_PACKET_TYPE_LOGIN 'L'
#define SOUPBIN_PACKET_LOGIN_ACCEPTED 'A'
#define SOUPBIN_PACKET_LOGIN_REJECTED 'J'
#define SOUPBIN_PACKET_SEQUENCED_DATA 'S'
#define SOUPBIN_PACKET_SERVER_HEARTBEAT 'H'
#define SOUPBIN_PACKET_CLIENT_HEARTBEAT 'R'
#define SOUPBIN_PACKET_END_OF_SESSION 'Z'
#define SOUPBIN_PACKET_UNSEQUENCED_DATA 'U'

size_t soupbin__frame(const char *, size_t n);
uint16_t soupbin_frame_message(databuf_t * dbuff);
int soupbin_heartbeat_init(struct soupbin_header *heartbeat);
int soupbin_heartbeat_serialize(struct soupbin_header *heartbeat,
                                databuf_t * buffer);
int soupbin_login_init(struct soupbin_login *login, const char *username,
                       const char *password);
int soupbin__login__set_sequence_number(struct soupbin_login *login,
                                        unsigned long int seqno);
int soupbin_serialize(struct databuf *, void *, uint16_t);

/* The struct soupbin_header pointer must point to a complete packet,
 * i.e. the rest of the soupbin packet must follow.  The print_payload
 * function pointer is meant to dump the contents of SoupBin packets
 * that contain another protocol's payload, e.g. OUCH, RASH, etc. */
int soupbin_packet_to_str(struct soupbin_header *header,
                          struct databuf *str,
                          int (*print_payload) (void *, size_t,
                                                struct databuf *));
int soupbin__print_packet(void *, size_t, struct databuf *, void *);
int soupbin__dump_data(void *, size_t, struct databuf *, dump_inner_data);

#endif
