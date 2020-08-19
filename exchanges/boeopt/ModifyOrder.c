#include "ModifyOrder.h"

#include <string.h>

void initModifyOrder(struct ModifyOrder *modifyOrder,
                     dof_t * databufFactory)
{
    memset(modifyOrder, 0, sizeof *modifyOrder);
    struct iovec *iovp = modifyOrder->iov;
    size_t iovn = narray(modifyOrder->iov);
    iovp[0].iov_base = &modifyOrder->fixedFields;
    iovp[0].iov_len = sizeof modifyOrder->fixedFields;
    ++iovp;
    --iovn;
    initOptionalFields(&modifyOrder->optionalFields,
                       modifyOrder->fixedFields.bitfields,
                       narray(modifyOrder->fixedFields.bitfields), iovp,
                       iovn, databufFactory);
}

static size_t getModifyOrderSize(struct ModifyOrder *modifyOrder)
{
    size_t nbytes = sizeof modifyOrder->fixedFields;
    nbytes += sumOptionalFieldLength(&modifyOrder->optionalFields);
    nbytes -= sizeof modifyOrder->fixedFields.header.startOfMessage;
    return nbytes;
}

void prepModifyOrderForSending(struct ModifyOrder *modifyOrder,
                               uint32_t nextSequenceNumber)
{
    struct Header *hdr = &modifyOrder->fixedFields.header;
    hdr->startOfMessage[0] = 0xBA;
    hdr->startOfMessage[1] = 0xBA;
    hdr->messageLength = getModifyOrderSize(modifyOrder);
    hdr->messageType = 0x06;
    hdr->matchingUnit = 0x00;
    hdr->sequenceNumber = nextSequenceNumber;
}

void destroyModifyOrder(struct ModifyOrder *modifyOrder)
{
    destroyOptionalFields(&modifyOrder->optionalFields);
}
