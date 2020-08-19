#ifndef __RDONLY_MSG_H__
#define __RDONLY_MSG_H__

#include <stdlib.h>
#include <stdio.h>

struct rdonly_msg;
typedef struct rdonly_msg rdonlymsg_t;

struct rdonly_msg {
    struct iovec (*get) (rdonlymsg_t *, size_t);
    void (*free) (void *);
    int (*dump) (rdonlymsg_t *, FILE *);
};

#endif
