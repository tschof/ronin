#ifndef __TRANSLATOR_H__
#define __TRANSLATOR_H__

#include "rdonly_msg.h"
#include "rw_msg.h"

struct op;
typedef struct op op_t;

int translate(rdonlymsg_t *, rw_msg_t *, op_t *);

#endif
