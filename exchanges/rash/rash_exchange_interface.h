#ifndef __OUCH_EXCHANGE_INTERFACE_H__
#define __OUCH_EXCHANGE_INTERFACE_H__

#include "exchange_interface.h"
#include "rash_util.h"

struct rash_exchange_interface;
struct parse_con;
typedef int (*rash_state_function) (struct rash_exchange_interface *,
                                    const char *, size_t, int *,
                                    struct parse_con *);

struct rash_exchange_interface {
    struct exchange_interface ei;
    rash_state_function evaluate;
    struct rash_timestamp_converter *tsconverter;
};

#endif
