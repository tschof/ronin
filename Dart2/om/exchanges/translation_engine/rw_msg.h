#ifndef __RW_MSG_H__
#define __RW_MSG_H__

#include "rdonly_msg.h"

struct rw_msg;
typedef struct rw_msg rw_msg_t;

struct rw_msg {
    struct iovec (*get) (rw_msg_t *, size_t);
    void (*free) (void *);
    int (*dump) (rw_msg_t *, FILE *);
    int (*set) (rw_msg_t *, size_t, struct iovec);
    void (*clear) (rw_msg_t *);
};

#endif
