#ifndef TPOS_TRADE_DESC_H
#define TPOS_TRADE_DESC_H

#include <string>
#include <vector>

using namespace std;

static const char * s_TpBasketID = "BasketID";
static const char * s_TpTradeID = "TradeID";
static const char * s_TpTradeVersion = "Version";
static const char * s_TpAccountTraderID = "AcctTraderID";

static const char * s_TpTradeGroup = "TradeGroup";
static const char * s_TpPortfolio = "Portfolio";
static const char * s_TpClearingFirm = "ClringFirm";
static const char * s_TpClearingAccount = "ClringAcct";
static const char * s_TpClearingSubAccount = "ClringSubAcct";
static const char * s_TpTrader = "Trader";

static const char * s_TpTradeDate = "TradeDate";
static const char * s_TpTradeTime = "TradeTime";
static const char * s_TpLastModDate = "LastModDate";
static const char * s_TpLastModTime = "LastModTime";
static const char * s_TpBookDate = "BookDate";
static const char * s_TpBookTime = "BookTime";
static const char * s_TpLastModBy = "LastModBy";
static const char * s_TpLastModReason = "LastModReason";
static const char * s_TpLastModReasonID = "LastModReasonID";

static const char * s_TpInstID = "InstID";
static const char * s_TpPosInstID = "PosInstID";
static const char * s_TpSecType = "SecType";
static const char * s_TpExch = "Exch";
static const char * s_TpSymbol = "Symb";
static const char * s_TpTicker = "Ticker";
static const char * s_TpExpDate = "ExpDate";  // Format: YYYY-MM-DD
static const char * s_TpExpTime = "ExpTime";  // Format: HH:MM:SS
static const char * s_TpStrike = "Strike";
static const char * s_TpContractSize = "ContSize";
static const char * s_TpPutCall = "P/C";
static const char * s_TpCUSIP = "CUSIP";
static const char * s_TpISIN = "ISIN";
static const char * s_TpSEDOL = "SEDOL";
static const char * s_TpUndInstID = "UndInstID";
static const char * s_TpUndSymbol = "UndSymb";
static const char * s_TpUndType = "UndType";
static const char * s_TpUndExchange = "UndExch";
static const char * s_TpUndExpDate = "UndExpDate";
static const char * s_TpBaseCurr = "BaseCurr";
static const char * s_TpCrossCurr = "CrossCurr";
static const char * s_TpTradeSource = "TrdSrc";

static const char * s_TpQty = "Qty";
static const char * s_TpPrice = "Price";
static const char * s_TpSSFlag = "SSFlag";
static const char * s_TpSpreadFlag = "SpreadFlag";
static const char * s_TpFlexFlag = "FlexFlag";
static const char * s_TpContraBroker = "ContraBroker";
static const char * s_TpContraFirm = "ContraFirm";
static const char * s_TpExtTradeID = "ExtTradeID";
static const char * s_TpExtTradeDesc = "ExtTradeDesc";

static const char * s_TpNote = "Note";
static const char * s_TpLockFlag = "LockFlag";
static const char * s_TpSpcStlFlag = "SpcStlFlag";
////////////////////////////
// These are for Positions only
static const char * s_TpOpenQty = "openQty";
static const char * s_TpCurrQty = "currQty";
static const char * s_TpAsOfDate = "asOfDate";
////////////////////////////
static const char * TpHeader[] =
{
   s_TpBasketID,
   s_TpTradeID,
   s_TpTradeVersion,
   s_TpAccountTraderID,

   s_TpTradeGroup,
   s_TpPortfolio,
   s_TpClearingFirm,
   s_TpClearingAccount,
   s_TpClearingSubAccount,
   s_TpTrader,

   s_TpTradeDate,
   s_TpTradeTime,
   s_TpLastModDate,
   s_TpLastModTime,
   s_TpBookDate,
   s_TpBookTime,
   s_TpLastModBy,
   s_TpLastModReason,
   s_TpLastModReasonID,

   s_TpInstID,
   s_TpPosInstID,
   s_TpSecType,
   s_TpExch,
   s_TpSymbol,
   s_TpTicker,
   s_TpExpDate,
   s_TpExpTime,
   s_TpStrike,
   s_TpContractSize,
   s_TpPutCall,
   s_TpCUSIP,
   s_TpISIN,
   s_TpSEDOL,
   s_TpUndInstID,
   s_TpUndSymbol,
   s_TpUndType,
   s_TpUndExchange,
   s_TpUndExpDate,
   s_TpBaseCurr,
   s_TpCrossCurr,
   s_TpTradeSource,

   s_TpQty,
   s_TpPrice,
   s_TpSSFlag,
   s_TpSpreadFlag,
   s_TpFlexFlag,
   s_TpContraBroker,
   s_TpContraFirm,
   s_TpExtTradeID,
   s_TpExtTradeDesc,

   s_TpNote,
   s_TpLockFlag,
   s_TpSpcStlFlag,
   "END_OF_LIST"
};
////////////////////////////
#define TDESC_BASKET_ID         (long long)1<<0
#define TDESC_TRADE_ID          (long long)1<<1
#define TDESC_VERSION           (long long)1<<2

#define TDESC_TRADE_GROUP       (long long)1<<3
#define TDESC_PORTFOLIO         (long long)1<<4
#define TDESC_CLEARING_FIRM     (long long)1<<5
#define TDESC_CLEARING_ACCT     (long long)1<<6
#define TDESC_CLEARING_SACCT    (long long)1<<7
#define TDESC_TRADER            (long long)1<<8

#define TDESC_TRADE_DATE        (long long)1<<9
#define TDESC_TRADE_TIME        (long long)1<<10
#define TDESC_BOOK_DATE         (long long)1<<11
#define TDESC_BOOK_TIME         (long long)1<<12
#define TDESC_LASTMOD_DATE      (long long)1<<13
#define TDESC_LASTMOD_TIME      (long long)1<<14
#define TDESC_LASTMOD_BY        (long long)1<<15
#define TDESC_LASTMOD_REASON    (long long)1<<16

#define TDESC_INST_ID           (long long)1<<17
#define TDESC_POS_INST_ID       (long long)1<<18
#define TDESC_SEC_TYPE          (long long)1<<19
#define TDESC_EXCHANGE          (long long)1<<20
#define TDESC_SYMBOL            (long long)1<<21
#define TDESC_TICKER            (long long)1<<22
#define TDESC_EXP_DATE          (long long)1<<23
#define TDESC_EXP_TIME          (long long)1<<24
#define TDESC_STRIKE            (long long)1<<25
#define TDESC_CONT_SIZE         (long long)1<<26
#define TDESC_PUT_CALL          (long long)1<<27
#define TDESC_CUSIP             (long long)1<<28
#define TDESC_ISIN              (long long)1<<29
#define TDESC_SEDOL             (long long)1<<30
#define TDESC_UND_INST_ID       (long long)1<<31
#define TDESC_UND_SYMBOL        (long long)1<<32
#define TDESC_UND_TYPE          (long long)1<<33
#define TDESC_UND_EXCHANGE      (long long)1<<34
#define TDESC_UND_EXP_DATE      (long long)1<<35
#define TDESC_BASE_CURR         (long long)1<<36
#define TDESC_CROSS_CURR        (long long)1<<37
#define TDESC_TRADE_SOURCE      (long long)1<<38

#define TDESC_QUANTITY          (long long)1<<39
#define TDESC_PRICE             (long long)1<<40
#define TDESC_SS_FLAG           (long long)1<<41
#define TDESC_SPREAD_FLAG       (long long)1<<42
#define TDESC_FLEX_FLAG         (long long)1<<43
#define TDESC_CONTRA_BROKER     (long long)1<<44
#define TDESC_CONTRA_FIRM       (long long)1<<45
#define TDESC_EXT_TRADE_ID      (long long)1<<46
#define TDESC_EXT_TRADE_DESC    (long long)1<<47

#define TDESC_NOTE              (long long)1<<48
#define TDESC_LOCK_FLAG         (long long)1<<49
#define TDESC_ACCT_TRADER_ID    (long long)1<<50
#define TDESC_LASTMOD_REASON_ID (long long)1<<51
#define TDESC_SPEC_SETTLE_FLAG  (long long)1<<52

class TradeDesc
{
public:
   TradeDesc();
   ~TradeDesc();
   static void getHeader( vector<std::string> & a_list );

   bool setDesc( const char * a_desc );
   void getDesc( std::string & r_info, long long a_mask = 0xffffffffffffffff) const;
   static void getDesc( char * r_trade, int argc, char **argv, long long a_mask = 0xffffffffffffffff );

   void setBasketID( const char * a_info );
   void setTradeID( const char * a_info );
   void setVersion( int a_info );

   void setAccountTraderID( const char * a_info );
   void setTradeGroup( const char * a_info );
   void setPortfolio( const char * a_info );
   void setClearingFirm( const char * a_info );
   void setClearingAccount( const char * a_info );
   void setClearingSubAccount( const char * a_info );
   void setTrader( const char * a_info );

   void setTradeDate( const char * a_info );
   void setTradeTime( const char * a_info );
   void setLastModDate( const char * a_info );
   void setLastModTime( const char * a_info );
   void setBookDate( const char * a_info );
   void setBookTime( const char * a_info );
   void setLastModBy( const char * a_info );
   void setLastModReason( const char * a_info );
   void setLastModReasonID( int a_info );

   void setInstID( const char * a_info );
   void setPosInstID( const char * a_info );
   void setSecType( const char * a_info );
   void setExch( const char * a_info );
   void setSymbol( const char * a_info );
   void setTicker( const char * a_info );
   void setExpDate( const char * a_info );
   void setExpTime( const char * a_info );
   void setStrike( double a_info );
   void setContractSize( double a_info );
   void setPutCall( const char * a_info );
   void setCUSIP( const char * a_info );
   void setISIN( const char * a_info );
   void setSEDOL( const char * a_info );
   void setUndInstID( const char * a_info );
   void setUndSymbol( const char * a_info );
   void setUndType( const char * a_info );
   void setUndExchange( const char * a_info );
   void setUndExpDate( const char * a_info );
   void setBaseCurr( const char * a_info );
   void setCrossCurr( const char * a_info );
   void setTradeSource( const char * a_info );

   void setQty( int a_info );
   void setPrice( double a_info );
   void setSSFlag( const char * a_info );
   void setSpreadFlag( const char * a_info );
   void setFlexFlag( const char * a_info );
   void setContraBroker( const char * a_info );
   void setContraFirm( const char * a_info );
   void setExtTradeID( const char * a_info );
   void setExtTradeDesc( const char * a_info );

   void setNote( const char * a_info );
   void setLockFlag( const char * a_info );
   void setSpcStlFlag( const char * a_info );

   const std::string & getBasketID() const;
   const std::string & getTradeID() const;
   int getVersion() const;

   const std::string & getAccountTraderID() const;
   const std::string & getTradeGroup() const;
   const std::string & getPortfolio() const;
   const std::string & getClearingFirm() const;
   const std::string & getClearingAccount() const;
   const std::string & getClearingSubAccount() const;
   const std::string & getTrader() const;

   const std::string & getTradeDate() const;
   const std::string & getTradeTime() const;
   const std::string & getLastModDate() const;
   const std::string & getLastModTime() const;
   const std::string & getBookDate() const;
   const std::string & getBookTime() const;
   const std::string & getLastModBy() const;
   const std::string & getLastModReason() const;
   int getLastModReasonID() const;

   const std::string & getInstID() const;
   const std::string & getPosInstID() const;
   const std::string & getSecType() const;
   const std::string & getExch() const;
   const std::string & getSymbol() const;
   const std::string & getTicker() const;
   const std::string & getExpDate() const;
   const std::string & getExpTime() const;
   double getStrike() const;
   double getContractSize() const;
   const std::string & getPutCall() const;
   const std::string & getCUSIP() const;
   const std::string & getISIN() const;
   const std::string & getSEDOL() const;
   const std::string & getUndInstID() const;
   const std::string & getUndSymbol() const;
   const std::string & getUndType() const;
   const std::string & getUndExchange() const;
   const std::string & getUndExpDate() const;
   const std::string & getBaseCurr() const;
   const std::string & getCrossCurr() const;
   const std::string & getTradeSource() const;

   int  getQty() const;
   double getPrice() const;
   const std::string & getSSFlag() const;
   const std::string & getSpreadFlag() const;
   const std::string & getFlexFlag() const;
   const std::string & getContraBroker() const;
   const std::string & getContraFirm() const;
   const std::string & getExtTradeID() const;
   const std::string & getExtTradeDesc() const;

   const std::string & getNote() const;
   const std::string & getLockFlag() const;
   const std::string & getSpcStlFlag() const;

private:
	std::string m_BasketID;
	std::string m_TradeID;
	int m_Version;

	std::string m_AccountTraderID;
	std::string m_TradeGroup;
	std::string m_Portfolio;
	std::string m_ClearingFirm;
	std::string m_ClearingAccount;
	std::string m_ClearingSubAccount;
	std::string m_Trader;

	std::string m_TradeDate;
	std::string m_TradeTime;
	std::string m_LastModDate;
	std::string m_LastModTime;
	std::string m_BookDate;
	std::string m_BookTime;
	std::string m_LastModBy;
	std::string m_LastModReason;
	int m_LastModReasonID;

	std::string m_InstID;
	std::string m_PosInstID;
	std::string m_SecType;
	std::string m_Exch;
	std::string m_Symbol;
	std::string m_Ticker;
	std::string m_ExpDate;  // Format: YYYY-MM-DD
	std::string m_ExpTime;  // Format: HH:MM:SS
	double m_Strike;
	double m_ContractSize;
	std::string m_PutCall;
	std::string m_CUSIP;
	std::string m_ISIN;
	std::string m_SEDOL;
	std::string m_UndInstID;
	std::string m_UndSymbol;
	std::string m_UndType;
	std::string m_UndExchange;
	std::string m_UndExpDate;
	std::string m_BaseCurr;
	std::string m_CrossCurr;
	std::string m_TradeSource;

	int m_Qty;
	double m_Price;
	std::string m_SSFlag;
	std::string m_SpreadFlag;
	std::string m_FlexFlag;
	std::string m_ContraBroker;
	std::string m_ContraFirm;
	std::string m_ExtTradeID;
	std::string m_ExtTradeDesc;

	std::string m_Note;
	std::string m_LockFlag;
	std::string m_SpcStlFlag;
};

class TradeVector : public std::vector<TradeDesc *>
{
public:
   virtual ~TradeVector() { free(); }
   void    free();
};

#endif
