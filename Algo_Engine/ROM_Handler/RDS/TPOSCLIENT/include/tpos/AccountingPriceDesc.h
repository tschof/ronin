#ifndef __INTERFACE_ACCOUNTINGPRICE_DESC_H__
#define __INTERFACE_ACCOUNTINGPRICE_DESC_H__

#include <string>
#include <map>
#include <vector>

/*
InstrumentID                   numeric 
TradeDate                      datetime
PriceSourceID                  int
TradingSymbol
BaseSymbol
ExpirationDate
StrikePrice
Call_Put
MarkToMarket                   decimal
CUSIP
RoninSymbol
LastModifiedBy                 varchar    
LastModifiedDateAndTime        smalldatetime
*/

class AccountingPriceDesc;

typedef  std::map<std::string, double >  AccountingPriceMap;

class AccountingPriceDesc
{
 public:
   AccountingPriceDesc();
   AccountingPriceDesc(const AccountingPriceDesc & a_old );
   virtual ~AccountingPriceDesc();

   bool setDesc( const char * a_desc );
   void getDesc( std::string & r_info ) const;

   int setInstrumentID(const char* a_info);
   int setUndInstrumentID(const char* a_info);
   int setTradeDate(const char* a_info);
   int setPrice( float a_info);
   int setPriceSource( const char* a_info);

   const std::string& getInstrumentID() const;
   const std::string& getUndInstrumentID() const;
   const std::string& getTradeDate() const;
   float getPrice() const;
   const std::string& getPriceSource() const;
  
 private:
   std::string m_instrumentID;
   std::string m_undInstrumentID;
   std::string m_tradeDate;
   float       m_price;
   std::string m_priceSource;
};

/*
class AccountingPriceSet : public std::set<AccountingPriceDesc>  
{
  public:
    AccountingPriceSet();
    virtual ~AccountingPriceSet();

    void free();
};
*/
#endif
