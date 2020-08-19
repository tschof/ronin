#ifndef __CANCEL_ORDER_H__
#define __CANCEL_ORDER_H__

#include "Header.h"

#include <stdlib.h>
#include <stdint.h>

#pragma pack(push,1)

struct CancelOrder {
    struct Header header;
    char origClOrdID[20];
    uint8_t bitfields[2];
};

#pragma pack(pop)

int initCancelOrder(struct CancelOrder *cancelOrder,
                    const char *origClOrdIDp, size_t origClOrdIDn);

#endif
