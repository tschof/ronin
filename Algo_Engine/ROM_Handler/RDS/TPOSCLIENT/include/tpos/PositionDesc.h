#ifndef TPOS_POSITION_DESC_H
#define TPOS_POSITION_DESC_H

#include <string>
#include <vector>
#include <iostream>
#include <tpos/TradeDesc.h>

using namespace std;

static const char * TpHeaderPos[] =
{
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

   s_TpOpenQty,
   s_TpCurrQty,
   s_TpAsOfDate,
   s_TpSpcStlFlag,

   "END_OF_LIST"
};

#define PDESC_ACCT_TRADER_ID    (long long)1<<0
#define PDESC_TRADE_GROUP       (long long)1<<1
#define PDESC_PORTFOLIO         (long long)1<<2
#define PDESC_CLEARING_FIRM     (long long)1<<3
#define PDESC_CLEARING_ACCT     (long long)1<<4
#define PDESC_CLEARING_SACCT    (long long)1<<5
#define PDESC_TRADER            (long long)1<<6

#define PDESC_TRADE_DATE        (long long)1<<7
#define PDESC_TRADE_TIME        (long long)1<<8
#define PDESC_LASTMOD_DATE      (long long)1<<9
#define PDESC_LASTMOD_TIME      (long long)1<<10

#define PDESC_POS_INST_ID       (long long)1<<11
#define PDESC_SEC_TYPE          (long long)1<<12
#define PDESC_EXCHANGE          (long long)1<<13
#define PDESC_SYMBOL            (long long)1<<14
#define PDESC_TICKER            (long long)1<<15
#define PDESC_EXP_DATE          (long long)1<<16
#define PDESC_EXP_TIME          (long long)1<<17
#define PDESC_STRIKE            (long long)1<<19
#define PDESC_CONT_SIZE         (long long)1<<20
#define PDESC_PUT_CALL          (long long)1<<21
#define PDESC_CUSIP             (long long)1<<22
#define PDESC_ISIN              (long long)1<<23
#define PDESC_SEDOL             (long long)1<<24
#define PDESC_UND_INST_ID       (long long)1<<25
#define PDESC_UND_SYMBOL        (long long)1<<26
#define PDESC_UND_TYPE          (long long)1<<27
#define PDESC_UND_EXCHANGE      (long long)1<<28
#define PDESC_UND_EXP_DATE      (long long)1<<29

#define PDESC_OPEN_QUANTITY     (long long)1<<30
#define PDESC_CURR_QUANTITY     (long long)1<<31
#define PDESC_ASOFDATE          (long long)1<<32
#define PDESC_SPEC_SETTLE_FLAG  (long long)1<<33

////////////////////////////
class PositionDesc
{
public:
   PositionDesc();
   ~PositionDesc();

   static void getHeader( vector<std::string> & a_list );
   bool setDesc( const char * a_desc );
   static void PositionDescAppend( std::string & a_out, const std::string a_value, const char * a_name, long long a_mask, long long a_bit );
   static void PositionDescAppend( std::string & a_out, const char * a_value, const char * a_name, long long a_mask, long long a_bit );
   void getDesc( std::string & r_info, long long a_mask = 0xffffffffffffffff) const;

   void setAccountTraderID( const char * a_info );
   void setTradeGroup( const char * a_info );
   void setPortfolio( const char * a_info );
   void setClearingFirm( const char * a_info );
   void setClearingAccount( const char * a_info );
   void setClearingSubAccount( const char * a_info );
   void setTrader( const char * a_info );

   void setLastTradeDate( const char * a_info );
   void setLastTradeTime( const char * a_info );
   void setLastModDate( const char * a_info );
   void setLastModTime( const char * a_info );

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
   void setSpcStlFlag( const char * a_info );
   void setCUSIP( const char * a_info );
   void setISIN( const char * a_info );
   void setSEDOL( const char * a_info );
   void setUndInstID( const char * a_info );
   void setUndSymbol( const char * a_info );
   void setUndType( const char * a_info );
   void setUndExchange( const char * a_info );
   void setUndExpDate( const char * a_info );

   void setOpenQty( int a_info );
   void setCurrentQty( int a_info );
   void setAsOfDate( const char * a_info );

   const std::string & getAccountTraderID() const;
   const std::string & getTradeGroup() const;
   const std::string & getPortfolio() const;
   const std::string & getClearingFirm() const;
   const std::string & getClearingAccount() const;
   const std::string & getClearingSubAccount() const;
   const std::string & getTrader() const;

   const std::string & getLastTradeDate() const;
   const std::string & getLastTradeTime() const;
   const std::string & getLastModDate() const;
   const std::string & getLastModTime() const;

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
   const std::string & getSpcStlFlag() const;
   const std::string & getCUSIP() const;
   const std::string & getISIN() const;
   const std::string & getSEDOL() const;
   const std::string & getUndInstID() const;
   const std::string & getUndSymbol() const;
   const std::string & getUndType() const;
   const std::string & getUndExchange() const;
   const std::string & getUndExpDate() const;

   int  getOpenQty() const;
   int  getCurrentQty() const;
   const std::string & getAsOfDate() const;

   void dump( std::ostream & a_os );

private:
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
	std::string m_SpcStlFlag;
	std::string m_CUSIP;
	std::string m_ISIN;
	std::string m_SEDOL;
	std::string m_UndInstID;
	std::string m_UndSymbol;
	std::string m_UndType;
	std::string m_UndExchange;
	std::string m_UndExpDate;

	int m_OpenQty;
	int m_CurrentQty;
	std::string m_AsOfDate;
};

#endif
