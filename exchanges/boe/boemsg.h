#ifndef __BOEMSG_H__
#define __BOEMSG_H__

#include "Header.h"

#pragma pack(push,1)

struct BoeHeader {
    uint16_t startOfMessage;
    uint16_t messageLength;
    uint8_t messageType;
    uint8_t matchingUnit;
    uint32_t sequenceNumber;
};

struct BoeEnterOrder {
    struct BoeHeader header;
    char clOrdID[20];
    char side;
    uint32_t orderQty;
    uint8_t bitfields[6];
};

struct boe_ClientHeartbeat {
    char startOfMessage[2];
    uint16_t messageLength;
    uint8_t messageType;
    uint8_t matchingUnit;
    uint32_t sequenceNumber;
};

struct ResponseHeader {
    struct Header header;
    uint64_t transactionTime;
    char clOrdID[20];
};

struct OrderReject {
    struct ResponseHeader responseHeader;
    char rejectReason;
    char text[60];
};

struct CancelReject {
    struct ResponseHeader responseHeader;
    char reason;
    char text[60];
};

struct OrderExecution {
    struct ResponseHeader responseHeader;
    uint64_t execID;
    uint32_t lastShares;
    uint64_t lastPx;
    uint32_t leavesQty;
    char baseLiquidityIndicator;
    char subLiquidityIndicator;
    uint64_t accessFee;
    char contraBroker[4];
    uint8_t bitfields[8];
};

struct OrderModified {
    struct ResponseHeader responseHeader;
    uint64_t orderID;
    uint8_t bitfields[7];
};

struct OrderModificationRejected {
    struct ResponseHeader responseHeader;
    char modifyRejectReason;
    char text[60];
    uint8_t bitfield;
};

struct OrderBustOrCorrection {
    struct ResponseHeader responseHeader;
    uint64_t orderID;
    uint64_t execRefID;
    char side;
    char baseLiquidityIndicator;
    char clearingFirm[4];
    char clearingAccount[4];
    uint32_t lastShares;
    int64_t lastPx;
    int64_t correctedPx;
    int64_t origTime;
};

struct BoeLoginResponse {
    char startOfMessage[2];
    uint16_t messageLength;
    uint8_t messageType;
    uint8_t matchingUnit;
    uint32_t sequenceNumber;
    char status;
    char text[60];
    uint8_t noUnspecifiedUnitReplace;
    /* We are going to ignore these for now, but we need to account
     * for their space because there are fields that come after that
     * we will need to read. */
    char returnBitFields[88];
    uint32_t lastReceivedSequenceNumber;
    uint8_t numberOfUnits;
    /* Variable number of matching unit sequence numbers follow
     * here. */
};

struct BoeLoginRequest {
    struct BoeHeader header;
    char sessionSubID[4];
    char username[4];
    char password[10];
    char noUnspecifiedUnitReplace;
    char orderAckFields[7];
    char reserved1;
    char orderRejFields[7];
    char reserved2;
    char orderModFields[7];
    char reserved3;
    char orderRestatedFields[7];
    char reserved4;
    char orderModRejFields[7];
    char reserved5;
    char orderCancelledFields[7];
    char reserved6;
    char cancelRejFields[7];
    char reserved7;
    char orderExecFields[7];
    char reserved8;
    char tradeCancelCorrectFields[7];
    char reserved9[17];
};

#pragma pack(pop)

#endif
