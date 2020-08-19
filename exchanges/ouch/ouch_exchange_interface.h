#ifndef __OUCH_EXCHANGE_INTERFACE_H__
#define __OUCH_EXCHANGE_INTERFACE_H__

#include "exchange_interface.h"
#include "soupbin.h"

struct ouch_exchange_interface;
struct parse_con;
typedef int (*ouch_state_function) (struct ouch_exchange_interface * oei,
                                    const struct soupbin_header * header,
                                    int *cut_connection,
                                    struct parse_con * parse_info);

struct ouch_exchange_interface {
    struct exchange_interface ei;
    ouch_state_function evaluate;
    struct soupbin_login login;
    struct soupbin_header heartbeat;
};

#endif
