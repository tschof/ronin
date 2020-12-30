/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _OTTO_H__
#define _OTTO_H__

#if defined(__cplusplus)
extern "C" {
#endif


#include "dart_order_obj.h"
/*System event codes*/
#define START_OF_MESSAGES 0x4f
#define START_OF_SYSTEM_HOURS 0x53
#define START_OF_OPENING 0x51
#define END_OF_NORMAL_HOURS 0x4E
#define END_OF_LATE_HOURS 0x4C
#define END_OF_SYSTEM_HOURS 0x45
#define END_OF_MESSAGES 0x43

#define ntohll(x) ( ( (uint64_t)(ntohl( (uint32_t)((x << 32) >> 32) )) << 32) | ntohl( ((uint32_t)(x >> 32)) ) )
#define htonll(x) ntohll(x)


#define Swap8Bytes(val) \
    ((((val) >> 56) & 0x00000000000000FF) | (((val) >> 40) & 0x000000000000FF00) | \
     (((val) >> 24) & 0x0000000000FF0000) | (((val) >>  8) & 0x00000000FF000000) | \
     (((val) <<  8) & 0x000000FF00000000) | (((val) << 24) & 0x0000FF0000000000) | \
     (((val) << 40) & 0x00FF000000000000) | (((val) << 56) & 0xFF00000000000000) )

#define ouch_find_token_len(p, l)        \
    do {                                \
        --l;                            \
    } while(l > 0 && *(p+l) == ' ');       \
    if(*(p+l) != ' ') ++l;                      \

#pragma pack(push, 1)

typedef struct otto_new_ord {
    char type;
    char firmid[4];
    uint32_t instrumentid;
    char clordid[16];
    uint32_t cmta;
    char clearingaccount[4];
    uint32_t occaccount;
    char custacc[10];
    char preferredparty[3];
    char aloinst;
    char iso;
    char side;
    char ordtype;
    int64_t price;
    uint32_t qty;
    uint32_t minqty;
    char tif;
    char capacity;
    char auctiontype;
    uint32_t auctionid;
    char flashinst;
    uint8_t disclosuremask;
    char priceprotection;
    uint16_t displayqty;
    char displaywhen;
    char displaymethod;
    uint16_t displaylowqty;
    uint16_t displayhighqty;
    uint16_t positioneffectmask;
    char stocklegshortsale;
    char stocklegMpid[4];
    char persistinst; 
}ottono;

typedef struct otto_new_ord_short {
    char type;
    char firmid[4];
    uint32_t instrumentid;
    char clordid[16];
    char aloinst;
    char iso;
    char side;
    char ordtype;
    int64_t price;
    uint32_t qty;
    uint32_t minqty;
    char tif;
    char capacity;
    char auctiontype;
    uint32_t auctionid;
    char priceprotection;
    uint16_t positioneffectmask;
} ottonos;

typedef struct otto_replace {
    char type;
    char firmid[4];
    char origclordid[16];
    char clordid[16];
    uint32_t qty;
    char ordtype;
    int64_t price;
    char tif;
    char custacc[10];
    char priceprotection;
} ottorep;

typedef struct otto_cancel {
    char type;
    char firmid[4];
    char clordid[16];
} ottocnl;

typedef struct otto_mass_cancel {
    char type;
    char firmid[4];
    char clrequestid[16];
    char instrumenttype;
    char scope;
    uint16_t productid;
    uint32_t instrumentid;
    char underlyingsym[13];
} ottomcnl;

typedef struct otto_new_order_cross {
    char type;
    char firmid[4];
    char crosstype;
    char auctiontype;
    char auctionallocpct;
    char side;
    char iso;
    char priceprotection;
    uint64_t effectivetime;
    uint8_t disclosuremask;
    char clordid[16];
    uint32_t cmta;
    char clraccount[4];
    uint32_t occaccount;
    char custacc[10];
    int64_t price;
    uint32_t qty;
    char capacity;
    uint16_t positioneffectmask;
    char stocklegshortsale;
    char stocklegmpid[4];
    char contraclordid[16];
    uint32_t contracmta;
    char contraclraccount[4];
    uint32_t contraoccaccount;
    char contracustacc[10];
    int64_t contraprice;
    uint32_t contraqty;
    char contracapacity;
    uint16_t contrapositioneffectmask;
    char contrastocklegshortsale;
    char contrastocklegmpid[4];
} ottonoc;

typedef struct otto_leg_definitions {
    char legtype;
    uint32_t leginstrumentid;
    char legside;
    uint16_t legratio;
} ottolegdef;

typedef struct otto_add_cmplx {
    char type;
    char firmid[4];
    char clrequestid[16];
    uint16_t productid;
    char productname[13];
    uint8_t numlegs;
} ottocmplx;

typedef struct otto_kill_switch {
    char type;
    char firmid[4];
    char clrequestid[16];
    char targetfirmid[4];
    char killaction;
} ottokill;

typedef struct otto_subscription_request {
    char type;
    char firmid[4];
    char clrequestid[16];
    char subscriptions[16];
} ottosub;

typedef struct otto_system_event_message {
    char type; /*z*/
    uint64_t timestamp;
    char eventcode;
} ottosysevent;

typedef struct otto_simple_instrument_directory {
    char type; /*o*/
    uint64_t timestamp;
    uint16_t productid;
    char productname[13];
    uint32_t instrumentid;
    uint8_t expiryear;
    uint8_t expirmon;
    uint8_t expirday;
    int64_t strikeprice;
    char optiontype;
    char closingtype;
    char tradable;
    char closingonly;
    uint16_t contract_size;
    char mpv;
    uint8_t systempartition;
    char securitysym[5];
} ottosid;

typedef struct otto_complex_instrument_directory {
    char type; /*s*/
    uint64_t timestamp;
    uint16_t productid;
    char productname[13];
    uint32_t instrumentid;
    uint8_t numlegs;
} ottocid;

typedef struct otto_instrument_trading_action_message {
    char type; /*i*/
    uint64_t timestamp;
    uint16_t productid;
    uint32_t instrumentid;
    char tradingstate;
} ottointam;

typedef struct otto_order_accepted {
    char type; /*a*/
    uint64_t timestamp;
    char firmid[4];
    uint32_t instrumentid;
    uint64_t orderid;
    char clordid[16];
    uint32_t cmta;
    char clraccount[4];
    uint32_t occaccount;
    char custacct[10];
    char preferredparty[3];
    char aloinst;
    char iso;
    char side;
    char ordertype;
    int64_t price;
    uint32_t qty;
    uint32_t minqty;
    char tif;
    char capacity;
    char auctiontype;
    uint32_t auctionid;
    char flashinst;
    uint8_t disclosuremask;
    char priceprotection;
    uint16_t displayqty;
    char displaywhen;
    char displaymethod;
    uint16_t displaylowqty;
    uint16_t displayhighqty;
    uint16_t positioneffectmask;
    char stocklegshortsale;
    char socklegmpid[4];
    char persistinst;
} ottooa;

typedef struct otto_order_accepted_short {
    char type; /*b*/
    uint64_t timestamp;
    char firmid[4];
    uint32_t instrumentid;
    uint64_t orderid;
    char clordid[16];
    char aloinst;
    char iso;
    char side;
    char ordertype;
    int64_t price;
    uint32_t qty;
    char tif;
    char capacity;
    char auctiontype;
    uint32_t auctionid;
    char priceprotection;
    uint16_t positioneffectmask;
}ottooashorty;

typedef struct otto_order_replaced {
    char type; /*b*/
    uint64_t timestamp;
    char firmid[4];
    uint32_t instrumentid;
    uint64_t origorderid;
    uint64_t orderid;
    char origclordid[16];
    char clordid[16];
    char aloinst;
    char iso;
    char side;
    char ordertype;
    int64_t price;
    uint32_t qty;
    char tif;
    char custacct[10];
    char capacity;
    char auctiontype;
    uint32_t auctionid;
    uint16_t positioneffectmask;
    char priceprotection;
}ottorepl;

typedef struct otto_order_canceled {
    char type;
    uint64_t timestamp;
    char firmid[4];
    uint32_t instrumentid;
    uint64_t orderid;
    char clordid[16];
    char cancelreason;
}ottocnled;

typedef struct otto_order_executed {
    char type; /*b*/
    uint64_t timestamp;
    char firmid[4];
    uint16_t productid;
    char ordexectype;
    uint32_t instrumentid;
    uint32_t legistrumentid;
    uint8_t legid;
    char autiontype;
    uint64_t orderid;
    char clordid[16];
    uint32_t crossid;
    uint32_t matchid;
    char side;
    char stocklegshortsale;
    int64_t exeprice;
    uint32_t lastshares;
    uint8_t liquidityind;
}ottoexe;

typedef struct otto_trade_details {
    char type; /*t*/
    uint64_t timestamp;
    char firmid[4];
    uint16_t productid;
    char ordexectype;
    uint32_t instrumentid;
    uint32_t legistrumentid;
    uint8_t legid;
    char transtype;
    char eventsource;
    char auctiontype;
    uint64_t orderid;
    char clordid[16];
    uint32_t crossid;
    uint32_t matchid;
    uint32_t refmatchid;
    char side;
    char stocklegshortsale;
    int64_t exeprice;
    uint32_t lastshares;
    uint8_t liquidityind;
    uint32_t cmta;
    char clraccount[4];
    uint32_t occaccount;
    char custacct[10];
    char stockvenue;
    char stocklegmpid[4];
    char capacity;
    char openclose;
}ottotd;

typedef struct otto_cross_order_accpeted {
    char type; /*t*/
    uint64_t timestamp;
    char firmid[4];
    uint32_t instrumentid;
    char crosstype;
    uint32_t auctionid;
    char auctiontype;
    uint8_t auctionallocpct;
    char side;
    char iso;
    char priceprotection;
    uint64_t effectivetime;
    uint8_t disclosuremask;
    uint64_t orderid;
    char clordid[16];
    uint32_t cmta;
    char clraccount[4];
    uint32_t occaccount;
    char custacct[10];
    int64_t price;
    uint32_t qty;
    char capacity;
    uint16_t positioneffectmask;
    char stocklegshortsale;
    char socklegmpid;
    uint64_t contraorderid;
    char contraclordid[16];
    uint32_t contracmta;
    char contraclraccount[4];
    uint32_t contraoccaccount;
    char contracustacct[10];
    int64_t contraprice;
    uint32_t contraqty;
    char ccontraapacity;
    uint16_t contrapositioneffectmask;
    char contrastocklegshortsale;
    char contrasocklegmpid;
}ottocross;

typedef struct otto_member_kill_switch_notification {
    char type;
    uint64_t timestamp;
    char firmid[4];
    char clrequestid[16];
    char targetfirmid[4];
    char killaction;
} ottokillnote;

typedef struct otto_mass_cancel_reponse {
    char type;
    uint64_t timestamp;
    char firmid[4];
    char clrequestid[16];
    uint32_t numcanceled;
    uint32_t numpending;
}ottomasscnlresp;

typedef struct otto_add_cmplx_instrument_response {
    char type;
    uint64_t timestamp;
    char firmid[4];
    char clrequestid[16];
    uint32_t instrumentid;
} ottocmplxresp;

typedef struct otto_modify_trade_response {
    char type;
    uint64_t timestamp;
    char firmid[4];
    char clrequestid[16];
    char clordid[16];
    uint32_t crossid;
    uint32_t matchid;
}ottomodresp;

typedef struct otto_subscription_response {
    char type; /*f*/
    uint64_t timestamp;
    char firmid[4];
    char clrequestid[16];
} ottosubresp;

typedef struct otto_reject {
    char type; /*j*/
    uint64_t timestamp;
    char rejectmsgtype;
    char clordid[16];
    uint16_t rejectcode;
}ottorej;

typedef struct otto_pending_response {
    char type; /*p*/
    uint64_t timestamp;
    char firmid[4];
    uint32_t instrumentid;
    char pendingmsgtype;
    char clordid[16];
    char pendingreason;
}ottopending;
#pragma pack(pop)
typedef struct otto_parser ottop;

ottop* create_empty_otto_parser();
void create_instrument_sym(ottop* otp, ottosid* osid);
uint32_t get_instrument_with_sec_def(ottop* otp, char* sec_def, int len);
uint32_t get_instrumentid(char* sym, int slen,
                          char* expire, int elen,
                          char* strike, int tlen,
                          char* exday, int edlen,
                          char put_call, ottop* otp);
void set_rom_symbol(ottop* otp, uint32_t ottosym, dart_order_obj* doj);
#if defined(__cplusplus)
}
#endif
#endif/*End of _OTTO_H__*/
