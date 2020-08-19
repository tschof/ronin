#include "CancelOrder.h"

#include <string.h>

int initCancelOrder(struct CancelOrder *cancelOrder,
                    const char *origClOrdIDp, size_t origClOrdIDn)
{
    memset(cancelOrder, 0, sizeof *cancelOrder);
    cancelOrder->header.startOfMessage[0] = 0xBA;
    cancelOrder->header.startOfMessage[1] = 0xBA;
    cancelOrder->header.messageLength =
        sizeof *cancelOrder - sizeof cancelOrder->header.startOfMessage;
    cancelOrder->header.messageType = 0x05;
    cancelOrder->header.matchingUnit = 0x00;
    if (origClOrdIDn > sizeof cancelOrder->origClOrdID)
        return -1;
    memcpy(cancelOrder->origClOrdID, origClOrdIDp, origClOrdIDn);
    return 0;
}
