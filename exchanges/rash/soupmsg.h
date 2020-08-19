#ifndef __SOUP_H__
#define __SOUP_H__

struct soup_login_accepted {
    char type;
    char session[10];
    char sequence_number[10];
    char terminator;
};

struct rash_soup_header {
    char packet_type;
    char timestamp[8];
    char message_type;
    char order_token[14];
};

#endif
