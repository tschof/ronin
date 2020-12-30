/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _REX_CONSTANTS_H__
#define _REX_CONSTANTS_H__
#if defined(__cplusplus)
extern "C" {
#endif

#define COMMA ","
#define DART_SEND_LEN 162
#define ROM_SEQ 2
#define ROM_TRADER 4
#define ROM_CLIENT_ID 6
#define ROM_SIDE 8
#define ROM_SYM 10
#define ROM_SHARES 12
#define ROM_PRICE 14
#define ROM_TYPE 16 
#define ROM_TIF 18 
#define ROM_CAP 20
#define ROM_CLR_ID 22
#define ROM_CLR_ACC 24
#define ROM_DESTID 26
#define ROM_STATUS 28
#define ROM_INSTR 30
#define ROM_EX_TAG 32
#define ROM_TAG 34
#define ROM_MIN_QTY 36
#define ROM_STOP 38
#define ROM_SELF_MATCH 40
#define ROM_MANUAL 42
#define ROM_LOCAL_ACC 44
#define ROM_FIRM 46
#define ROM_TRADE_FOR 48
#define ROM_TEXT 52
#define ROM_OWNER 54
#define ROM_EXECUTION_TAG 56
#define ROM_EXDEST 58
#define ROM_EXPIRE_DATE 60
#define ROM_P_C 62
#define ROM_STRIKE 64
#define ROM_UNDER 66
#define ROM_MAX_FLOOR 68
#define ROM_SEC_TYPE 70
#define ROM_MM_ACC 72
#define ROM_MM_HOME 74
#define ROM_O_C 76
#define ROM_OWN_CNL 78
#define ROM_DESC 80
#define ROM_DEST 82
#define ROM_LAST_SHARES 84
#define ROM_ORIG_SHARES 86
#define ROM_EXEC_PRICE 88
#define ROM_EXPIRE_TIME 94
#define ROM_CUM 96
#define ROM_LEAVES 98
#define ROM_CMTA 92
#define EXEC_TIME 100
#define ROM_AVG_PRICE 102
#define ROM_TIME 104
#define ROM_NAME 106
#define EXEC_ID 108
#define ROM_SEC_DEF 110
#define ROM_PROD_CODE 112
#define ROM_EXEC_INST 114
#define ROM_SHORT_LEND 116
#define ROM_PEG_DIFF 128
#define ROM_PARENT 130
#define ROM_SPREAD 132
#define ROM_AUCTION_ID 134
#define ROM_DISP_ROUTE 136
#define ROM_EXPIRE_DAY 124
#define ROM_DISP_INSTR 126
#define ROM_LIQ 138
#define ROM_OM_PARENT 140
#define ROM_FREQ_TRADE 142
#define ROM_LEG_MARK 144
#define ROM_ECHO 148
#define ROM_BEG_TIME 152
#define ROM_ALGO_TYPE 146
#define ROM_END_TIME 154
#define ROM_SEC_ID 156
#define ROM_STATUS_CNT 160
#define ROM_CPLX_TYPE 132
#define ROM_ON_BEHALF 170
#define ROM_EXEC_SEC_TYPE 171
#define ROM_SENDER 172
#define ROM_BIN_LAST_SHARES 174
#define ROM_BIN_LAST_PRICE 176
#define ROM_BIN_ORDID 176
#define ROM_STASH_SYM 175
#define ROM_STASH_SIDE 173
#define ROM_CLORDID 30
#define ROM_ORIG_ID 122
#define ROM_PENDING_ID 122
#define ROM_NEW_ID 120
#define ROM_EXCHANGE_SYM 177
#define ROM_BIN_QTY 178
#define ROM_BIN_CUM 180
#define ROM_BIN_MAXFLOOR 182
#define ROM_BIN_PRICE 184
#define ROM_EXCH_PRICE 185
#define ROM_EX_ID 186
#define ROM_BIN_STOP 187
#define ROM_COPY_INSTR 188
#define ROM_BIN_MULT 189
#define ROM_BIN_CLORDID 190
#define ROM_BIN_PENDING_ID 192
#define ROM_BIN_PRICE_DELTA 194
#define ROM_BIN_SHARES_DELTA 196

//Admin commands
#define BLOCK_USER 20
#define DISCONNECT_USER 3
#define USER_INFO 2
#define RELOAD_TRADERS 4
#define EXEMPT_USER 5
#define DESTROUTE_UPDATE 11
#define RELOAD_DEST_IPPORT 13
#define RESET_SEQ 23
#define QUERY_DESTS 17
#define QUERY_USERS 19
#define MOD_DEST_ACCESS 1
#define DISCONNECT_DEST 18
#define USER_STATUS 24
#define CLEARINGACT_SEC_LIMITS_STATUS 25
#define RELOAD_CLEARINGACT_LIMITS 26
#define MSG_RATE_LIMIT 35
#define EQUITY_SYM_CHECK 36
#define BILLINGGRP_LIMIT_STATUS 37
#define RELOAD_BILLINGGRP_LIMITS 38
#define EXEMPT_BILLINGGRP 53

/* These constants are for the information about user. */
#define MAX_ORDERS 13
#define CURRENT_OPEN_ORDERS 14
#define MAX_TOTAL_SHARES 15
#define CURRENT_OPEN_SHARES 16
#define MAX_TOTAL_DOLLARS 17
#define CURRENT_OPEN_DOLLARS 18
#define MAX_SHARES_PER 19
#define MAX_DOLLARS_PER 20


#define ACTION 10
#define COMMAND 4
#define EXCHANGE 8
#define TRADER 6
#define ORDER_DEST 16
#define CLIENT 14

#define ROM_OPEN "13"
#define ROM_CANCELLED "4"
#define ROM_REPLACED "5"
#define ROM_PENDING "6"
#define ROM_CNCL_REJ "14"
#define ROM_RPL_REJ "20"
#define ROM_REJ "8"
#define ROM_ZERO_STATUS "S"
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

#define CHAR_BIT 8
#define ROM_SPREAD_SEP "!#!"


#define FIX_SHORT_NEW_ORD 0x44
#define FIX_SHORT_CNL 0x46
#define FIX_SHORT_RPL 0x47
#define FIX_SHORT_TRADER_LOGON 0x4743
#define FIX_SHORT_SEC_DEF_REQ 0x63
#define FIX_SHORT_EXECUTION 0x38
#define FIX_SHORT_CNL_REP_REJ 0x39
#define FIX_SHORT_EMAIL 0x42
#define FIX_SHORT_SEC_DEF_RESP 0x64
#define FIX_SHORT_RESEND_REQ 0x32
#define FIX_SHORT_LOGON 0x41
#define FIX_SHORT_LOGOUT 0x35
#define FIX_SHORT_TEST_REQ 0x31
#define FIX_SHORT_GAP_FILL 0x34
#define FIX_SHORT_HEART_BEAT 0x30

#define FIX_CLORDID 15
#define FIX_ORIG_ID 60
#define FIX_EXCHANGE_TAG 16
#define FIX_LEG_TAG 17
#define FIX_EXEC_TAG 54
#define FIX_EXEC_TIME 50
#define FIX_TIME 50
#define FIX_LAST_SHARES 42
#define FIX_SHARES 6
#define FIX_SIDE 4
#define FIX_EXEC_PRICE 44
#define FIX_STATUS 14
#define FIX_PRICE 7
#define FIX_TEXT 26
#define FIX_LIQ_FLAG 69
#define FIX_LEG_TYPE 66
#define FIX_CNL_TYPE 62
#define FIX_CUM 48
#define FIX_LEAVES 49
#define FIX_EXE_TYPE 75
#define FIX_LEG_REPORTER 77
#define FIX_MESS_TYPE 74
#define FIX_SYM 5
#define FIX_EXPIRE_DATE 30
#define FIX_P_C 31
#define FIX_STRIKE 32
#define FIX_SEC_TYPE 35
#define FIX_SEC_DEF 55
#define SAFE_STRLEN_ (s, l) (                   \
        int r = 0;                              \
        while(*s && l)                          \
            ++s, ++l, ++r;                      \
        return r)                               \

#if defined(__cplusplus)
}
#endif
#endif
