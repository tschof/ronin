#ifndef __HEADER_H__
#define __HEADER_H__

#include <stdint.h>

#pragma pack(push,1)

struct Header {
    char startOfMessage[2];
    uint16_t messageLength;
    uint8_t messageType;
    uint8_t matchingUnit;
    uint32_t sequenceNumber;
};

#pragma pack(pop)

#endif
