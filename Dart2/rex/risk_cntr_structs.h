#ifndef _RISK_CNTR_STRUCTS_H__
#define _RISK_CNTR_STRUCTS_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#define NYBIN 1
#define CSV 2
#define BOE 3


#pragma pack(push, 1)
typedef struct risk_cntr_struct {
    void* raw;
    uint16_t rlen;
    uint16_t rtype;
    int64_t price;
    int32_t size;
    int32_t cum;
    int32_t last;
    int32_t delta;
    char owner[14];
    char* sym;
    char* clr_acc;
    char errStr[56];
    uint8_t olen;
    uint8_t slen;
    uint8_t clr_len;
    uint8_t err_len;
    char sec_type;
    char side;
}rcntr;
#pragma pack(pop)


#if defined(__cplusplus)
}
#endif
#endif
