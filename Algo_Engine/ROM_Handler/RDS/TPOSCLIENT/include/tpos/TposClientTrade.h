#ifndef TPOS_CLIENT_TRADE_H
#define TPOS_CLIENT_TRADE_H

#include <string>
#include <iostream>
class TposClientTradeImp;
class TposClientBasketImp;
class TposClientBasket;
class TposClientConnection;

class TposClientTrade
{ 
public:
   TposClientTrade( );
   ~TposClientTrade();

   int getVersion() const;
   const char * getTradeID() const;
   void markDeleted();
   void markTestTrade();
   bool isMarkedDeleted();
   void copyTrade( TposClientTrade & a_fromTrade );

   int setInt( const char * a_name, int a_value );
   int setFloat( const char * a_name, float a_value );
   int setDouble( const char * a_name, double a_value );
   int setString( const char * a_name, const char * a_value );

   int getInt( const char * a_name, int & r_value ) const;
   int getFloat( const char * a_name, float & r_value ) const;
   int getDouble( const char * a_name, double & r_value ) const;
   int getString( const char * a_name, const char *& r_value ) const;

   void dump( std::ostream & a_os );
   friend class TposClientConnection;
   friend class TposClientBasket;
   TposClientTradeImp * getImpTrade( ){return( m_trade );};

private:
   TposClientTradeImp * m_trade;
};

class TposClientBasket
{
public:
   TposClientBasket( const char * a_tradeGroupSymbol, const char * a_portfolioSymbol );
   ~TposClientBasket();

   int addTrade( TposClientTrade & a_trade );
   const char * getBasketID() const;
   int getTradeCount() const;
   int getTrade( int a_index, TposClientTrade & r_trade );
   int getTrade( const std::string & a_uniqueID, TposClientTrade & r_trade );

   int deleteTrade(const char * a_uniqueID );
   void dump( std::ostream & a_os );
   friend class TposClientConnection;
   
private:
   TposClientBasketImp * m_basket;
   
};

#endif //TPOS_NV_RECORD_H
