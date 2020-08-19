#include "EnterOrder.h"
#include "Macros.h"
#include "TradingInterface.h"
#include "server_objects/dart_order_obj.h"
#include "server_objects/dart_constants.h"
#include "server_objects/dart_obj_factory.h"
#include <string.h>
#include <stdio.h>

void initEnterOrder(struct EnterOrder *enterOrder, dof_t * databufFactory)
{
    memset(enterOrder, 0, sizeof *enterOrder);
    struct iovec *iovp = enterOrder->iov;
    size_t iovn = narray(enterOrder->iov);
    iovp[0].iov_base = &enterOrder->fixedFields;
    iovp[0].iov_len = sizeof enterOrder->fixedFields;
    ++iovp;
    --iovn;
    initOptionalFields(&enterOrder->optionalFields,
                       enterOrder->fixedFields.bitfields,
                       narray(enterOrder->fixedFields.bitfields), iovp,
                       iovn, databufFactory);
}

static size_t getEnterOrderSize(struct EnterOrder *enterOrder)
{
    size_t nbytes = sizeof enterOrder->fixedFields;
    size_t noptfields =
        sumOptionalFieldLength(&enterOrder->optionalFields);
    nbytes += noptfields;
    size_t nstart = sizeof enterOrder->fixedFields.header.startOfMessage;
    nbytes -= nstart;
    return nbytes;
}

void prepEnterOrderForSending(struct EnterOrder *enterOrder,
                              uint32_t nextSequenceNumber)
{
    struct Header *hdr = &enterOrder->fixedFields.header;
    hdr->startOfMessage[0] = 0xBA;
    hdr->startOfMessage[1] = 0xBA;
    hdr->messageLength = getEnterOrderSize(enterOrder);
    hdr->messageType = 0x04;
    hdr->matchingUnit = 0x00;
    hdr->sequenceNumber = nextSequenceNumber;
}

int setSide(struct EnterOrder *enterOrder, const char *p, size_t n)
{
    if (n == 1) {
        switch (p[0]) {
        case '1':
        case '2':
        case '5':
        case '6':
            enterOrder->fixedFields.side = p[0];
            return 0;
        }
    }
    return -1;
}

void destroyEnterOrder(struct EnterOrder *enterOrder)
{
    destroyOptionalFields(&enterOrder->optionalFields);
}
