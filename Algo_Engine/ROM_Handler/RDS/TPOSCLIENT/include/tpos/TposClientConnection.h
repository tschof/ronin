#ifndef TPOS_CLIENT_CONNECTION_H
#define TPOS_CLIENT_CONNECTION_H

#include <time.h>
#include <set>
#include <vector>
#include <tpos/TposDefs.h>
#include <tpos/TposUtils.h>
#include <tpos/TposClientTrade.h>
#include <tpos/TposDefs.h>
#include <tpos/SpecialSettleDesc.h>
#include <tpos/TheoreticalDesc.h>
#include <tpos/DividendDesc.h>
#include <tpos/AccountingPriceDesc.h>
#include <tpos/MktPriceDesc.h>
#include <tpos/TradeDesc.h>
#include <tpos/PositionDesc.h>

class MelvinStatus;
class TposClientConnection;
class TposClientConnectionImp;
class TposPubMessage;
typedef int (*tposCallbackPtr)( TposPubMessage * msg, void *rock);

class BasicTposClientStatusCallback
{
  public:
    virtual ~BasicTposClientStatusCallback() {}
    virtual void onStatusChange(TposStatus&);
};

class TposClientStatusCallback : public BasicTposClientStatusCallback
{
  public:
    TposClientStatusCallback(int subscribeFlag = TCR_NONE, int interval =0);
    TposClientStatusCallback(TposClientConnection& t, int subscribeFlag = TCR_NONE, int interval =0);
    virtual ~TposClientStatusCallback() {}
    virtual void onStatusChange(TposStatus&);

  protected:
    TposClientConnection*    m_tpos;
    int                      m_subscribeFlag;
    int                      m_interval;
    time_t                   m_downTime;
    TposStatus               m_prevStatus;
};

class TposClientConnection
{
public:
   TposClientConnection( const char * a_traderAcronym, const char * a_password, const char * a_elvinHost, int a_elvinPort, const char * a_interfaceName = "GUI" );
   ~TposClientConnection();

   const char * getTrader();
   int connect();
   void  recover(int subscribeFlag);
   int enterBasket( TposClientBasket & a_basket );
   int retrieveWithBasketID( const char * a_basketID, TposClientBasket & a_basket );
   int retrieveWithTradeID( const char * a_tradeID, TposClientBasket & a_basket );
   int retrieveWithExTradeID( const char * a_exTradeID, TposClientBasket & a_basket );

   int getExchangeList( std::vector<std::string> & r_list  );
   int getReasonList( std::vector<std::string> & r_list  );
   int getTraderList( std::set<std::string> & r_list  );
   int getPortfolioList( const char * a_trader,
                        std::set<std::string> & r_list  );
   int getAccountList( const char * a_trader,
                       const char * a_tgPortfolio,
                       std::set<std::string> & r_list  );
   int getAccountList( std::vector<TposAccount> & r_accountList );

   int getSpecialSettleTable( std::vector<SpecialSettleDesc> & r_chain  );

   int getTheoreticals( TheoreticalDesc& r_greek, const char* r_date, const char* r_groupAcrn, const char* r_inst);
   int getTheoreticals( TheoreticalMap& r_greeks, const char* r_date, const char* r_groupAcrn, const char* r_undInst = 0);
   int setTheoreticals( const TheoreticalDesc& r_greek);
   int setTheoreticals( const TheoreticalVector & r_greeks);

   int getDividends( DividendSet& r_divs, const char* r_inst, const char* r_edate=0);

   int getWatchListInstrumentIDs(StringList& r_list, const char* groupAcrn);
   int addWatchListInstrumentID( const char* r_inst, const char* groupAcrn);
   int addWatchListInstrumentIDs(const StringList& r_list, const char* groupAcrn);
   int removeWatchListInstrumentID( const char* r_inst, const char* groupAcrn);
   int removeWatchListInstrumentIDs( const StringList& r_list, const char* groupAcrn);

   int getMktPrices(MktPriceMap&, const char* r_ricgrp, const char* r_date, const char* mktsrc="REUTERS");
   int getAccountingPrices(AccountingPriceMap&, const char* r_undInst, const char* r_date, const char* clearingFirm="FOC");
   int getAccountingPrice( double & r_price, const char* a_Inst, const char* a_date, const char* clearingFirm="FOC");

   int getOptionChain( const char * a_symbol,
                       const char * a_exchange,
                       std::vector<optionInfo> & r_chain  );
   int getFlexOptionChain( const char * a_symbol,
                       const char * a_exchange,
                       std::vector<optionInfo> & r_chain  );
   int getFutureChain( const char * a_symbol,
                       const char * a_exchange,
                       std::vector<futureInfo> & r_chain  );
   int getFutureChainUnd( const char * a_undsymbol,
                       const char * a_exchange,
                       std::vector<futureInfo> & r_chain  );
   int getSSFutureChain( const char * a_symbol,
                       const char * a_exchange,
                       std::vector<futureInfo> & r_chain  );
   int getSecurityByID(  const char * a_id, securityInfo & r_sec  );
   int getSecurityXByID(  const char * a_id, securityInfoX & r_sec  );

   int getExternalIDList      (const char * a_id, std::vector<std::string> &r_extIDList  );
   int getExternalIDByInstID  (const char * a_externalIDType, const char *   a_instId,     std::vector<std::string> & r_extIDList);
   int getInstIDByExternalID  ( const char * a_externalIDType, const char *  a_extId, std::vector<std::string> & r_instIDList);
   int getSecuritybyTicker(  const char * a_ticker, std::vector<securityInfoX> & r_sec );
   int getMarketFeeds(std::vector<feedData> & r_feed );
   ////////////////////////////////////////////////////////
   // OLD POSITION subscription methods
   int subscribePosition( const char * a_tradeGroup,
                          const char * a_portfolio,
                          tposCallbackPtr a_funcPtr,
                          void * a_rock );

   int subscribePosition( const char * a_tradeGroup,
                          const char * a_portfolio,
                          const char * a_trader,
                          tposCallbackPtr a_funcPtr,
                          void * a_rock );

   int unSubscribePosition( const char * a_tradeGroup,
                            const char * a_portfolio );
   int unSubscribePosition( const char * a_tradeGroup,
                            const char * a_portfolio,
                            const char * a_trader);

   int getPositionsAsOf( vector<PositionDesc *> & r_positions, 
                          const char * a_tradeGroup,
                          const char * a_portfolio,
                          const char * a_asOfDate ); 

   int getPositionsAsOf( vector<PositionDesc *> & r_positions, 
                          const char * a_tradeGroup,
                          const char * a_portfolio,
                          const char * a_trader,
                          const char * a_asOfDate );

   int getPositionsCurrent( vector<PositionDesc *> & r_positions, 
                          const char * a_tradeGroup,
                          const char * a_portfolio );

   int getPositionsCurrent( vector<PositionDesc *> & r_positions, 
                          const char * a_tradeGroup,
                          const char * a_portfolio,
                          const char * a_trader );
   // OLD POSITION subscription methods
   ////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////
   // NEW POSITION subscription methods
   int getPositionsPort( vector<PositionDesc *> & r_positions, 
                          const char * a_tradeGroup,
                          const char * a_portfolio,
                          const char * a_asOfDate = 0 );

   int getPositionsAccount( vector<PositionDesc *> & r_positions, 
                          const char * a_tradeGroup,
                          const char * a_portfolio,
                          const char * a_account,
                          const char * a_asOfDate = 0 );

   int getPositionsTrader( vector<PositionDesc *> & r_positions, 
                          const char * a_tradeGroup,
                          const char * a_portfolio,
                          const char * a_trader,
                          const char * a_asOfDate = 0 );

   int getPositionsPort(  const char * a_tradeGroup,
                          const char * a_portfolio,
                          tposCallbackPtr a_funcPtr,
                          void * a_rock,
                          const char * a_asOfDate = 0 );

   int getPositionsAccount(  const char * a_tradeGroup,
                          const char * a_portfolio,
                          const char * a_account,
                          tposCallbackPtr a_funcPtr,
                          void * a_rock,
                          const char * a_asOfDate = 0 );

   int getPositionsTrader(  const char * a_tradeGroup,
                          const char * a_portfolio,
                          const char * a_trader,
                          tposCallbackPtr a_funcPtr,
                          void * a_rock,
                          const char * a_asOfDate = 0 );

   int getPositionsBase(  const char * a_tradeGroup,
                          const char * a_portfolio,
                          const char * a_trader,
                          const char * a_account,
                          tposCallbackPtr a_funcPtr,
                          void * a_rock,
                          const char * a_asOfDate = 0 );

   int subscribePositionsPort( const char * a_tradeGroup,
                          const char * a_portfolio,
                          tposCallbackPtr a_funcPtr,
                          void * a_rock,
                          const char * a_asOfDate = 0 );

   int unsubscribePositionsPort( const char * a_tradeGroup,
                                 const char * a_portfolio,
                                 const char * a_asOfDate = 0 );

   int subscribePositionsAccount( const char * a_tradeGroup,
                          const char * a_portfolio,
                          const char * a_account,
                          tposCallbackPtr a_funcPtr,
                          void * a_rock,
                          const char * a_asOfDate = 0 );

   int unsubscribePositionsAccount( const char * a_tradeGroup,
                                 const char * a_portfolio,
                                 const char * a_account,
                                 const char * a_asOfDate = 0 );
   
   int subscribePositionsTrader( const char * a_tradeGroup,
                          const char * a_portfolio,
                          const char * a_trader,
                          tposCallbackPtr a_funcPtr,
                          void * a_rock,
                          const char * a_asOfDate = 0 );

   int unsubscribePositionsTrader( const char * a_tradeGroup,
                                 const char * a_portfolio,
                                 const char * a_trader,
                                 const char * a_asOfDate = 0 );

   // NEW POSITION subscription methods
   ////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////
   // OLD TRADE subscription methods
   int subscribeTrade( const char * a_tradeGroup,
                       const char * a_portfolio,
                       tposCallbackPtr a_funcPtr,
                       void * a_rock,
                       bool a_allVersions = false );
   
   int subscribeTrade( const char * a_tradeGroup,
                       const char * a_portfolio,
                       const char * a_trader,
                       tposCallbackPtr a_funcPtr,
                       void * a_rock,
                       bool a_allVersions = false );

   int unSubscribeTrade( const char * a_tradeGroup,
                         const char * a_portfolio );
   int unSubscribeTrade( const char * a_tradeGroup,
                         const char * a_portfolio,
                         const char * a_trader);

   //  OLD TRADE subscription methods
   ////////////////////////////////////////////////////////

   
   ////////////////////////////////////////////////////////
   // NEW TRADE subscription methods
   int getTrades( vector<TradeDesc *> & r_trades, 
                          const char * a_tradeGroup,
                          const char * a_portfolio,
                          const char * a_trader,
                          const char * a_beginLastModDate,
                          const char * a_beginLastModTime,
                          const char * a_endLastModDate,
                          const char * a_endLastModTime,
                          const char * a_beginTradeDate,
                          const char * a_beginTradeTime,
                          const char * a_endTradeDate,
                          const char * a_endTradeTime,
                          bool a_allVersions = false );

   int getTrades( const char * a_tradeGroup, 
                  const char * a_portfolio,
                  const char * a_trader,
                  const char * a_beginLastModDate,
                  const char * a_beginLastModTime,
                  const char * a_endLastModDate,
                  const char * a_endLastModTime,
                  const char * a_beginTradeDate,
                  const char * a_beginTradeTime,
                  const char * a_endTradeDate,
                  const char * a_endTradeTime,
                  tposCallbackPtr a_funcPtr,
                  void * a_rock,
                  bool a_allVersions = false);

   int subscribeTrades( const char * a_tradeGroup, 
                        const char * a_portfolio,
                        const char * a_trader,
                        const char * a_account,
                        const char * a_beginLastModTime,
                        const char * a_endLastModTime,
                        const char * a_beginTradeTime,
                        const char * a_endTradeTime,
                        tposCallbackPtr a_funcPtr,
                        void * a_rock,
                        bool a_allVersions = false);

   int unsubscribeTrades( const char * a_tradeGroup, 
                        const char * a_portfolio,
                        const char * a_trader,
                        const char * a_security );
   // NEW TRADE subscription methods
   ////////////////////////////////////////////////////////

   int subscribeTheo( TheoMsgCallback*, const char* r_groupAcrn, const char* r_inst, const char* r_date = 0);
   int subscribeTheo( TheoreticalDesc & r_greek, TheoMsgCallback*, 
                      const char* r_groupAcrn, const char* r_inst, const char* r_idate, const char* r_date = 0);

   int subscribeTheos( TheoMsgCallback*, const char* r_groupAcrn, const char* r_undInst, const char* r_date = 0);
   int subscribeTheos( TheoreticalMap & r_greeks, TheoMsgCallback*, 
                      const char* r_groupAcrn, const char* r_undinst, const char* r_idate, const char* r_date = 0);

   // int subscribeTheo( const char * a_groupAcrn, tposCallbackPtr a_funcPtr, void * a_rock);

   int unSubscribeTheo( const char * a_groupAcrn, const char* a_inst, const char* a_sdate=0);
   int unSubscribeTheos(const char * a_groupAcrn, const char* a_undinst=0, const char* a_sdate=0);


   static void parseAccountStr( std::string & r_clearingFirm,
                                std::string & r_clearingAccount,
                                std::string & r_clearingSubAccount,
                                const char * a_account );

   static void parseTgPort( std::string & r_tradeGroup,
                            std::string & r_portfolio,
                            const char * a_tgPortfolio );

   static void generateAccountStr( const char * a_clearingFirm,
                                const char * a_clearingAccount,
                                const char * a_clearingSubAccount,
                                std::string & r_account );

   static void generateTgPort( const char * a_tradeGroup,
                               const char * a_portfolio,
                               std::string & r_tgPortfolio );

   ////////////////////////////////////////////////////////
   // Enters or Replaces trades based on external trade ID.
   // External trade ID must be unique per clearing firm/account.
   int enterOrUpdateTrade( TposClientTrade  & a_trade );

   ////////////////////////////////////////////////////////
   // Deletes trade based on external trade ID.
   // External trade ID must be unique per clearing firm/account.
   int deleteTradeWithExTradeID( TposClientTrade  & a_trade );

   const char * getTposMapValue( const char * a_mapType, const char * a_interfaceMapValue );
   const TposAccount * getTposAccount( const char * a_validTraderID );
   int getTposAccountID( std::string & a_interfaceID, std::string & r_tposID );
   int getTposAccountInfo( std::string & a_interfaceID, std::string & a_tposID );

   ElvinStatus& getConnectionStatus();
   void  addElvinStatusCallback(BasicTposClientStatusCallback&, int delay = 0);
   void refreshMap();
   void refreshAccountData();
   void closeConnection(std::string a_connection);

private:
   TposClientConnectionImp * m_client;

   void setConnectionStatus(ElvinStatus&);
};

#endif
