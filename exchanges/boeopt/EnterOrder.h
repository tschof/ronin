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
    /* Byte 0 */
    EnterOrderClearingFirm = 0,
    EnterOrderClearingAcct,
    EnterOrderPrice,
    EnterOrderExecInst,
    EnterOrderOrdType,
    EnterOrderTimeInForce,
    EnterOrderMinQty,
    EnterOrderMaxFloor,

    /* Byte 1 */
    EnterOrderSymbol,
    EnterOrderReserved_Byte2_Bit2,
    EnterOrderReserved_Byte2_Bit4,
    EnterOrderReserved_Byte2_Bit8,
    EnterOrderReserved_Byte2_Bit16,
    EnterOrderReserved_Byte2_Bit32,
    EnterOrderCapacity,
    EnterOrderRoutingInst,

    /* Byte 2 */
    EnterOrderAccount,
    EnterOrderDisplayIndicator,
    EnterOrderMaxRemovePct,
    EnterOrderDiscretionAmount,
    EnterOrderReserved_Byte3_Bit5,
    EnterOrderPreventMemberMatch,
    EnterOrderReserved_Byte3_Bit7,
    EnterOrderExpireTime,

    /* Byte 3 */
    EnterOrderMaturityDate,
    EnterOrderStrikePrice,
    EnterOrderPutOrCall,
    EnterOrderRiskReset,
    EnterOrderOpenClose,
    EnterOrderCMTANumber,
    EnterOrderReserved_Byte4_Bit64,
    EnterOrderReserved_Byte4_Bit128,

    /* Byte 4 */
    EnterOrderReserved_Byte5_Bit1,
    EnterOrderAttributedQuote,
    EnterOrderReserved_Byte5_Bit4,
    EnterOrderReserved_Byte5_Bit8,
    EnterOrderReserved_Byte5_Bit16,
    EnterOrderReserved_Byte5_Bit32,
    EnterOrderReserved_Byte5_Bit64,
    EnterOrderReserved_Byte5_Bit128,
};

void initEnterOrder(struct EnterOrder *, dof_t *);
int setSide(struct EnterOrder *, const char *, size_t);
void prepEnterOrderForSending(struct EnterOrder *,
                              uint32_t nextSequenceNumber);
void destroyEnterOrder(struct EnterOrder *);

#endif
