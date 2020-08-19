#ifndef __MODIFY_ORDER_H__
#define __MODIFY_ORDER_H_

#include "Header.h"
#include "OptionalFields.h"
#include "Macros.h"

#include <sys/uio.h>

#pragma pack(push, 1)

struct ModifyOrderFixedFields {
    struct Header header;
    char clOrdID[20];
    char origClOrdID[20];
    uint8_t bitfields[2];
};

struct ModifyOrder {
    struct ModifyOrderFixedFields fixedFields;
    struct OptionalFields optionalFields;
    struct iovec
     iov[member_sizeof(struct ModifyOrderFixedFields, bitfields) * 8 + 1];
};

#pragma pack(pop)

enum ModifyOrderOptionalFieldID {
    ModifyOrderClearingFirm = 0,
    ModifyOrderReserved_Byte1_Bit2,
    ModifyOrderQty,
    ModifyOrderPrice,
    ModifyOrderType,
    ModifyOrderCancelOrigOnReject,
    ModifyOrderExecInst,
    ModifyOrderSide,
};

void initModifyOrder(struct ModifyOrder *, dof_t *);
void prepModifyOrderForSending(struct ModifyOrder *modifyOrder,
                               uint32_t nextSequenceNumber);
void destroyModifyOrder(struct ModifyOrder *);

#endif
