#ifndef __ENTER_ORDER_H__
#define __ENTER_ORDER_H__

#include "Header.h"
#include "OptionalFields.h"
#include "Macros.h"
#include "ErrorReport.h"

#include <stdint.h>
#include <sys/uio.h>

#pragma pack(push,1)

struct EnterOrderFixedFields {
    struct Header header;
    char clOrdID[20];
    char side;
    uint32_t orderQty;
    uint8_t bitfields[6];
};

struct EnterOrder {
    struct EnterOrderFixedFields fixedFields;
    struct OptionalFields optionalFields;
    /* One iovec for every bit in the bitfields plus one for the fixed
     * field at the beginning of the message. */
    struct iovec iov[member_sizeof(struct EnterOrderFixedFields, bitfields)
                     * 8 + 1];
};

#pragma pack(pop)

enum EnterOrderOptionalFieldID {
    EnterOrderClearingFirm = 0,
    EnterOrderClearingAcct,
    EnterOrderPrice,
    EnterOrderExecInst,
    EnterOrderOrdType,
    EnterOrderTimeInForce,
    EnterOrderMinQty,
    EnterOrderMaxFloor,
    EnterOrderSymbol,
    EnterOrderSymbolSfx,
    EnterOrderReserved_Byte2_Bit3,
    EnterOrderReserved_Byte2_Bit4,
    EnterOrderReserved_Byte2_Bit5,
    EnterOrderReserved_Byte2_Bit6,
    EnterOrderCapacity,
    EnterOrderRoutingInst,
    EnterOrderAccount,
    EnterOrderDisplayIndicator,
    EnterOrderMaxRemovePct,
    EnterOrderDiscretionAmount,
    EnterOrderPegDifference,
    EnterOrderPreventMemberMatch,
    EnterOrderLocateReqd,
    EnterOrderExpireTime,
    EnterOrderReserved_Byte4_Bit1,
    EnterOrderReserved_Byte4_Bit2,
    EnterOrderReserved_Byte4_Bit3,
    EnterOrderReserved_Byte4_Bit4,
    EnterOrderReserved_Byte4_Bit5,
    EnterOrderReserved_Byte4_Bit6,
    EnterOrderReserved_Byte4_Bit7,
    EnterOrderReserved_Byte4_Bit8,
    EnterOrderReserved_Byte5_Bit1,
    EnterOrderAttributedQuote,
    EnterOrderReserved_Byte5_Bit2,
    EnterOrderReserved_Byte5_Bit3,
    EnterOrderReserved_Byte5_Bit4,
    EnterOrderReserved_Byte5_Bit5,
    EnterOrderReserved_Byte5_Bit6,
    EnterOrderReserved_Byte5_Bit7,
    EnterOrderReserved_Byte5_Bit8,
};

void initEnterOrder(struct EnterOrder *, dof_t *);
int setSide(struct EnterOrder *, const char *, size_t);
void prepEnterOrderForSending(struct EnterOrder *enterOrder,
                              uint32_t nextSequenceNumber);
void destroyEnterOrder(struct EnterOrder *);

#endif
