#ifndef TPOS_DEFS_H
#define TPOS_DEFS_H

# include<stdlib.h>
# include <string>
# include <vector>
# include <list>

#define TCR_NONE       0
#define TCR_TRADE      (1<<1)
#define TCR_POSITION   (1<<2)
#define TCR_THEO       (1<<3)
#define TCR_ALL        0xFFFFFFFF

//#define DEBUG_FILTER    if ( getenv( "RTP_DEBUG" ) && atoi( getenv( "RTP_DEBUG" ) ) > 0 )
extern int s_debugFilter;
#define DEBUG_FILTER    if ( s_debugFilter )
#define STATUS_FILTER

//#define SM_OPTION_CHAIN_TEXT     OPTION_CHAIN_TEXT // "OPTION CHAIN"
//#define SM_OPT_CHAIN_COUNT_TEXT  OPT_CHAIN_COUNT_TEXT //"OPTION CHAIN COUNT"
//#define SM_UNDRLYING_TEXT        UNDRLYING_TEXT // "Underlying"
//#define SM_UNDERLYING_COUNT_TEXT UNDERLYING_COUNT_TEXT // "Underlying Count"
//#define SM_FUT_CHAIN_COUNT_TEXT  FUT_CHAIN_COUNT_TEXT //"Future Chain Count"
#define SM_OPTION_CHAIN_TEXT     "OPTION CHAIN"
#define SM_OPT_CHAIN_COUNT_TEXT  "OPTION CHAIN COUNT"
#define SM_UNDRLYING_TEXT        "Underlying"
#define SM_UNDERLYING_COUNT_TEXT "Underlying Count"
#define SM_FUT_CHAIN_COUNT_TEXT  "Future Chain Count"

#define SM_INSTRUMENT_ID    "Golden Copy Key"
#define SM_POS_INSTRUMENT_ID  "PARENT ID"
#define SM_CUSIP            "ID CUSIP/CINS/PPN"
#define SM_ISIN             "ID ISIN"
#define SM_SEDOL            "ID SEDOL"
#define SM_PRIMARY_EXHANGE  "PRIMARY TRADING ARENA"
#define SM_EXP_DATE         "MATURITY/EXPIRY/TENOR DATE"
#define SM_EXCHANGE         "EXCHANGE"
#define SM_TICKER_SYMBOL    "TICKER SYMBOL"
#define SM_STRIKE           "STRIKE PRICE"
#define SM_CONTRACT_SIZE    "CONTRACT SIZE"
#define SM_PUT_CALL         "CALL/PUT INDICATOR"
#define SM_FUTURES_DEL_DATE "FUTURES DELIVERY FIRST DATE"
#define SM_FUTURES_DEL_DATE "FUTURES DELIVERY FIRST DATE"


#define TPOS_CMD "TposCommand"
#define TPOS_CMD_USER "cmd_username"
#define TPOS_CMD_PASSWORD "cmd_password"
#define TPOS_CMD_HOST "cmd_host_and_pid"
#define TPOS_CMD_APPLICATION "cmd_application"
#define TPOS_CMD_VERSION "cmd_version"
#define TPOS_CMD_VERSION_NUM "001.000.000"
#define TPOS_BASKET_RETRIEVE_CMD "basket_retrieve"
#define TPOS_TRADE_RETRIEVE_CMD "trade_retrieve"
#define TPOS_EX_TRADE_RETRIEVE_CMD "ex_trade_retrieve"
#define TPOS_TRADE_FORMAT_CMD "trade_format"
#define TPOS_TRADE_REASON_RETRIEVE_CMD "trade_reason"
#define TPOS_MARKET_FEED_RETRIEVE_CMD "market_feeds"
#define TPOS_ACCOUNT_INFO_RETRIEVE_CMD "account_info"
#define TPOS_SECURITY_ID_RETRIEVE_CMD "security_id_retrieve"
#define TPOS_SECURITY_TICKER_RETRIEVE_CMD "security_ticker_retrieve"
#define TPOS_OPTION_CHAIN_RETRIEVE_CMD "option_chain_info"
#define TPOS_FUTURE_CHAIN_RETRIEVE_CMD "future_chain_info"
#define TPOS_SSFUTURE_CHAIN_RETRIEVE_CMD "ssfuture_chain_info"
#define TPOS_OPTION_RETRIEVE_CMD "option_info"
#define TPOS_MAP_RETRIEVE_CMD "map_info"
#define TPOS_SPECIAL_SETTLE_RETRIEVE_CMD "special_settle_rtrv"
#define TPOS_THEORETICAL_COMMIT_CMD "theoretical_interface_commit"
#define TPOS_THEORETICAL_RETRIEVE_CMD "theoretical_interface_rtrv"
#define TPOS_WATCHLIST_RETRIEVE_CMD "watchlist_interface_rtrv"
#define TPOS_WATCHLIST_ADD_CMD "watchlist_interface_add"
#define TPOS_WATCHLIST_DEL_CMD "watchlist_interface_del"
#define TPOS_DIVIDEND_RETRIEVE_CMD "dividend_rtrv"
#define TPOS_ACCOUNTINGPRICES_RETRIEVE_CMD "accountingprices_rtrv"
#define TPOS_MKTPRICES_RETRIEVE_CMD "mktprices_rtrv"
#define TPOS_BULK_TRADE_RTRV_CMD "bulk_trade_retrieve"
#define TPOS_BULK_POS_RTRV_CMD "bulk_pos_retrieve"
#define TPOS_EXTID_RTRV_BY_INSTID_CMD "externalid_by_InstID_retrieve"
#define TPOS_BULK_EXTID_RTRV_CMD "bulk_externalid_retrieve"
#define TPOS_INSTID_RTRV_BY_EXTID_CMD "instID_by_extID_retrieve"

// Fields for TPOS_BULK_TRADE_RTRV_CMD
#define TPOS_BULK_TRADE_DATE "date"
#define TPOS_BULK_TRADE_FILTER_TYPE "filter_type"
#define TPOS_BULK_TRADE_FILTER "filter"
#define TPOS_BULK_TRADE_SUBJECT "subject"
#define TPOS_BULK_TRADE_TRADEGROUP "trade_group"
#define TPOS_BULK_TRADE_PORTFOLIO "portfolio"

// Fields for TPOS_BULK_POS_RTRV_CMD
#define TPOS_BULK_POS_DATE "date"
#define TPOS_BULK_POS_FILTER_TYPE "filter_type"
#define TPOS_BULK_POS_FILTER "filter"
#define TPOS_BULK_POS_SUBJECT "subject"
#define TPOS_BULK_POS_TRADEGROUP "trade_group"
#define TPOS_BULK_POS_PORTFOLIO "portfolio"

#define TPOSFLD_RIC              "tposfld_ric"
#define TPOSFLD_RICGROUP         "tposfld_ricgrp"
#define TPOSFLD_INSTTYPE         "tposfld_inst_type"
#define TPOSFLD_INSTRUMENTID     "tposfld_instrumentid"
#define TPOSFLD_UNDINSTRUMENTID  "tposfld_und_instrumentid"
#define TPOSFLD_ITEM_SIZE        "tposfld_size"
#define TPOSFLD_THEO_DATE        "tposfld_theo_date"
#define TPOSFLD_THEO_GROUP       "tposfld_theo_group"
#define TPOSFLD_DIV_AMOUNT       "tposfld_div_amount"
#define TPOSFLD_DIV_STARTDATE    "tposfld_div_sdate"
#define TPOSFLD_DIV_ENDDATE      "tposfld_div_edate"
#define TPOSFLD_MKTSOURCE        "tposfld_mktsrc"

#define TPOSFLD_ITEMCOUNT        "tposfld_itemcount"
#define TPOSFLD_DATE             "tposfld_date"
#define TPOSFLD_TRDDATE          "tposfld_trddate"
#define TPOSFLD_CLEARINGFIRMID   "tposfld_clearingfirmid"
#define TPOSFLD_CLEARINGFIRM     "tposfld_clearingfirm"
#define TPOSFLD_PRICE            "tposfld_price"

#define TPOS_MANAGER_IP_NAME "ipAddress"
#define TPOS_MANAGER_PROG_NUM_NAME "programNumber"
#define TPOS_MANAGER_PROTO_TYPE "type"
#define TPOS_MANAGER_IP_NAME2 "ipAddress2"
#define TPOS_MANAGER_PROG_NUM_NAME2 "programNumber2"
#define TPOS_MANAGER_PROTO_TYPE2 "type2"

extern const char * TposSybaseServer_s;
extern const char * TposSybaseUser_s;
extern const char * TposSybasePassword_s;
extern const char * TposSybaseDBname_s;

extern int s_TposDBWrite;
extern int s_TposDBLoad;

#include <string>

struct TposAccount
{
   std::string m_validTraderID;
   std::string m_tradingGroupSymbol;
   std::string m_portfolioSymbol;
   std::string m_clearingFirmSymbol;
   std::string m_clearingAccount;
   std::string m_clearingSubAccount;
   std::string m_traderAcronym;
   std::string m_traderPassword;
};

struct securityInfo
{
 public:
   virtual ~securityInfo() {};
   std::string securityType;
   std::string baseSymbol;
   std::string fullSymbol;
   std::string exchange;
   double strike;
   std::string expirationDate;
   double contractSize;
   std::string putCall;
   std::string spcStlFlag;
   double tickSize;
   std::string curCode;
   std::string exerType;
   std::string instrumentId;
   std::string undInstrumentId;
   std::string longName;
   std::string obsoleteDateTime;
   std::string undExpirationDate;
   std::string roninSymbol;
   
   
};

struct exchangeInfo
{
   int ID;
   std::string name;
   std::string mdSymbol;
};

struct securityInfoX : public securityInfo
{
 public:
   virtual ~securityInfoX() {};
   std::string genericMDSymbol;
   std::string dataSourceInfo;
   std::vector<exchangeInfo> exchangeList;
};

struct feedData
{
	std::string feed;
	bool mpfFlag;
};

struct optionInfo
{
   std::string stockSymbol;
   std::string optionSymbol;
   std::string opraSymbol;
   std::string exchange;
   double strike;
   std::string expirationDate;
   double contractSize;
   std::string putCall;
   std::string spcStlFlag;
   double tickSize;
   std::string curCode;
   std::string exerType;
   std::string instrumentId;
   std::string undInstrumentId;
   std::string longName;
   std::string genericMDSymbol;
   std::string obsoleteDateTime;
   std::string undExpirationDate;
   std::string roninSymbol;
};

struct futureInfo
{
   std::string baseSymbol;
   std::string fullSymbol;
   std::string exchange;
   std::string expirationDate;
   std::string description;
   double contractSize;
   double tickSize;
   std::string curCode;
   std::string instrumentId;
   std::string undInstrumentId;
   std::string longName;
   std::string genericMDSymbol;
   std::string obsoleteDateTime;
   std::string undExpirationDate;
   std::string roninSymbol;
};

// DB Record Field Names
extern const char * TposBasketIdName_s;
extern const char * TposTradeIdName_s;
extern const char * TposTradeVersionName_s;
extern const char * TposTraderPortfolioID_s;
extern const char * TposTradingGroupSymbol_s;
extern const char * TposTradingGroupPortfolio_s;
extern const char * TposClearingFirmSymbol_s;
extern const char * TposValidTraderKey_s;
extern const char * TposClearingAccount_s;
extern const char * TposClearingSubAccount_s;
extern const char * TposTraderAcronym_s;
extern const char * TposTradeQuantityName_s;
extern const char * TposPosOpeningQty_s;
extern const char * TposPosCurrentQty_s;
extern const char * TposTradePrice_s;
extern const char * TposBaseCurrency_s;
extern const char * TposCrossCurrency_s;
extern const char * TposContraBroker_s;
extern const char * TposContraFirm_s;
extern const char * TposTradeDate_s;
extern const char * TposTradeTime_s;
extern const char * TposBookingDate_s;
extern const char * TposBookingTime_s;
extern const char * TposTradeSource_s;
extern const char * TposSpreadFlag_s;
extern const char * TposShortSaleFlag_s;
extern const char * TposLastModifiedReason_s;
extern const char * TposLastModifiedReasonID_s;
extern const char * TposLastModifiedBy_s;
extern const char * TposLastModifiedDate_s;
extern const char * TposLastModifiedTime_s;
extern const char * TposTradeLockedFlag_s;
extern const char * TposTradeSpcStlFlag_s;

extern const char * TposInstrumentID_s;
extern const char * TposInstrumentDesc_s;
extern const char * TposSecurityType_s;
extern const char * TposExchange_s;
extern const char * TposSymbol_s;
extern const char * TposExtendedSymbol_s;
extern const char * TposExpDate_s;
extern const char * TposExpTime_s;
extern const char * TposStrike_s;
extern const char * TposContractSize_s;
extern const char * TposPutCall_s;
extern const char * TposCUSIP_s;
extern const char * TposISIN_s;
extern const char * TposSEDOL_s;
extern const char * TposFlexFlag_s;
extern const char * TposNote_s;

extern const char * TposPositionInstrumentID_s;
extern const char * TposPositionInstrumentDesc_s;
extern const char * TposPrimaryExchange_s;

extern const char * TposUnderlyingInstrumentID_s;
extern const char * TposUnderlyingInstrumentDesc_s;
extern const char * TposUndSecurityType_s;
extern const char * TposUndExchange_s;
extern const char * TposUndSymbol_s;
extern const char * TposUndExpDate_s;
extern const char * TposUndExpTime_s;
extern const char * TposUndStrike_s;
extern const char * TposUndContractSize_s;
extern const char * TposUndPutCall_s;
extern const char * TposUndCUSIP_s;
extern const char * TposUndISIN_s;
extern const char * TposUndSEDOL_s;

extern const char * TposExternalTradeID_s;
extern const char * TposExternalTradeDesc_s;
extern const char * TposExternalBasketID_s;

extern const char * TposLastTradeDate_s;
extern const char * TposLastTradeTime_s;

class  TposPubMessage;
typedef int (*tposCallbackPtr)( TposPubMessage * msg, void *rock);

// DB Record Field Names


// Non DB Record Field Names
extern const char * TposTradeStatusName_s;
extern const char * TposBasketCountName_s;
extern const char * TposExceptionError_s;
extern const char * TposIVCcurrent_s;
extern const char * TposIVCtotal_s;


#define TPOS_ERROR(info) TposTime::stamp(cout); cout<<" - ERROR- "<<info<<", File: "<<__FILE__<<" Line: "<<__LINE__<<endl;
#define TPOS_INFO(info) DEBUG_FILTER {TposTime::stamp(cout); cout<<" - INFO- "<<info<<", File: "<<__FILE__<<" Line: "<<__LINE__<<endl;}
#define TPOS_STATUS(info) STATUS_FILTER {TposTime::stamp(cout); cout<<" - INFO- "<<info<<", File: "<<__FILE__<<" Line: "<<__LINE__<<endl;}

#define DEBUG_DUMP 0
#define LOG_DUMP 0

struct TposErrorStruct
{
   int errNumber;
   const char * errString;
};

#define TPOS_MAX_STRING_LENGTH 512

////////////////////////////
// General Errors
#define TPOS_SUCCESS 1
#define TPOS_GENERIC_ERROR -1
#define TPOS_INVALID_STRING -2
#define TPOS_FIELD_VALIDATION_FAILED -3
#define TPOS_FIELD_VALIDATION_BOGUS_CHAR -4
#define TPOS_CURENTLY_BLOCKING_TRADES -5

////////////////////////////
// Account Validation
#define TPOS_MISSING_PORTFOLIO_VALUE   -100
#define TPOS_MISSING_TRD_GROUP_VALUE   -101
#define TPOS_MISSING_EXECUTION_TRADER   -102
#define TPOS_MISSING_ENTRY_TRADER   -103
#define TPOS_MISSING_CLEARING_FIRM  -104
#define TPOS_MISSING_CLEARING_ACCOUNT  -105
#define TPOS_MISSING_CLEARING_SUBACCOUNT  -106
#define TPOS_INVALID_ACCOUNT_EX_TRADER  -107
#define TPOS_INVALID_ACCOUNT_ENT_TRADER  -108
#define TPOS_INVALID_PORTFOLIO_TRADEGROUP  -109
#define TPOS_INVALID_TRADER_AND_USER_COMBO  -110

////////////////////////////
// Client Errors
#define TPOS_ID_NOT_FOUND   -200
#define TPOS_EXTERNAL_ID_NOT_FOUND   -201
#define TPOS_INVALID_USERID   -202

////////////////////////////
// Trade Entry 
#define TPOS_VERSION_MISMATCH    -300
#define TPOS_NON_EXISTENT_TRADE  -301
#define TPOS_MISSING_EXT_ID  -302
#define TPOS_FAILED_TRADE_UPDATE  -303
#define TPOS_INVALID_REASON  -304
#define TPOS_NEW_TRADE_SUCCESS    300
#define TPOS_EXISTING_TRADE_UPDATE   301
#define TPOS_TRADE_DATE_ERROR -305
#define TPOS_UNKNOWN_TG_OR_PORT -306
#define TPOS_ATTEMPT_TO_EDIT_VTK -307
#define TPOS_DELETED_TRADE_NOT_EDITABLE -308

////////////////////////////
// Security Validation
#define TPOS_SECURITY_VALIDATION_FAILED -401
#define TPOS_SECURITY_VALIDATION_CHANGE -402

extern const TposErrorStruct TposErrorArray_s[];
const char * TposErrorString( int a_errno );

typedef std::list<std::string>  StringList;

// Cache mask for infoserver
#define CACHE_ACCT_DATA            1<<0  // INFO
#define CACHE_MAPPING              1<<1  // INFO
#define CACHE_REASON               1<<2  // INFO
#define CACHE_SPECIAL_SETTLE       1<<3  // INFO
#define CACHE_THEOS                1<<4  // INFO
#define CACHE_WATCHLIST            1<<5  // INFO
#define CACHE_DIVIDEND             1<<6  // INFO
#define CACHE_ACCTING_PRICE        1<<7  // INFO
#define CACHE_POSITION             1<<8
#define CACHE_IMCLIENT             1<<9  // SM
#define CACHE_MKT_PRICE            1<<10  // INFO

typedef unsigned int ElvinStatus;

#endif
