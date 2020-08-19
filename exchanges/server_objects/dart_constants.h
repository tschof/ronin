/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _DART_CONSTANTS_H__
#define _DART_CONSTANTS_H__

#if defined(__cplusplus)
extern "C" {
#endif

#define ROM_SIDE 4
#define ROM_SYM 5
#define ROM_SHARES 6
#define ROM_PRICE 7
#define ROM_TYPE 8
#define ROM_TIF 9
#define ROM_CAP 10
#define ROM_CLR_ID 11
#define ROM_CLR_ACC 12
#define ROM_STATUS 14
#define ROM_INSTR 15
#define ROM_TAG 17
#define ROM_MIN_QTY 18
#define ROM_STOP 19
#define ROM_EXPIRE_DATE 30
#define ROM_P_C 31
#define ROM_CALL_PUT 31
#define ROM_STRIKE 32
#define ROM_MAX_FLOOR 34
#define ROM_SEC_TYPE 35
#define ROM_MM_ACC 36
#define ROM_MM_HOME 37
#define ROM_O_C 38
#define ROM_OPEN_CLOSE 38
#define ROM_LAST_SHARES 42
#define ROM_ORIG_SHARES 43
#define ROM_EXEC_PRICE 44
#define ROM_CUM 48
#define ROM_CMTA 46
#define ROM_SEC_DEF 55
#define ROM_EXEC_INST 57
#define ROM_SHORT_LEND 58
#define ROM_MATURITY_DAY 62
#define ROM_DISPLAY_INSTRUCTIONS 63
#define ROM_PEGPRICE_OFFSET 64
#define ROM_BEG_TIME 76
#define ROM_ALGO_TYPE 73
#define ROM_END_TIME 77
#define ROM_CPLX_TYPE 66
#define ROM_ON_BEHALF 85
#define ROM_SENDER 86
#define ROM_CLORDID 87
#define ROM_ORIG_ID 88
#define ROM_BIN_QTY 89
#define ROM_BIN_CUM 90
#define ROM_BIN_MAXFLOOR 91
#define ROM_BIN_PRICE 92
#define ROM_EX_ID 93

#define FIX_HEARTBEAT '0'
#define FIX_TEST_REQ '1'
#define FIX_RESEND '2'
#define FIX_REJECT '3'
#define FIX_SEQ_RESET '4'
#define FIX_LOGOUT '5'
#define FIX_EXEC  '8'
#define FIX_CANCEL_REJECT '9'
#define FIX_LOGON  'A'
#define FIX_ORDER_S 'D'
#define FIX_REPLACE_REQ  'G'
#define FIX_CANCEL_REQ 'F'
#define FIX_FIELD_DELIM 1
#define ETX 3
#define FIX_REJECT '3'
#define FIX_BIZ_REJECT 'j'
#define FIX_FIELD_VALUE_DELIM  '='

#define FIX_EOM_OFFSET 8
#define FIX_EOM_TAG_LEN 7
#define FIX_BEG_MESSAGE_LEN 12
#define FIX_CHECKSUM_LEN 7
#define FIX_LEGSYMB "600"
#define SEARCH_FIX_LEG_SYM  "600="
#define FIX_LEG_SEC_ID "602"
#define FIX_LEG_CFI_CODE "608"
#define FIX_LEGMATURITY "610"
#define FIX_LEGEXPIRE_DATE "611"
#define FIX_LEGSTRIKE "612"
#define FIX_OPT_ATT "613"
#define FIX_LEG_POS_EFF "564"
#define FIX_LEG_SEC_DESC "620"
#define FIX_LEG_SIDE "624"
#define FIX_LEG_RATIO "623"
#define FIX_LEG_MULTI "654"

#define ROM_SPREAD_SEP "!#!"

#define SAFE_STRLEN_ (s, l) (                   \
        int r = 0;                              \
        while(*s && l)                          \
            ++s, ++l, ++r;                      \
        return r)                               \

#if defined(__cplusplus)
}
#endif
#endif                          //_DART_CONSTANTS_H__
