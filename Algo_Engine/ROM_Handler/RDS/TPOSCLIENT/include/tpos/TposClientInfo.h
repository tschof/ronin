#ifndef TPOS_CLIENT_INFO_H
#define TPOS_CLIENT_INFO_H

#include <set>
#include <vector>
#include <tpos/TposDefs.h>

class BasicTposClientStatusCallback;
class TposClientConnectionImp;

class TposClientInfo
{
public:
   TposClientInfo( const char * a_traderAcronym, const char * a_password, const char * a_elvinHost, int a_elvinPort, const char * a_interfaceName );
   ~TposClientInfo();

   int connect();

   const char * getTposMapValue( const char * a_mapType, const char * a_interfaceMapValue );
   const TposAccount * getTposAccount( const char * a_validTraderID );
   const TposAccount * getTposAccountInfo( const char * a_interfaceID );
   const TposAccount * getTposAccountInfoPartialMatch( const char * a_interfaceID );
   void  addElvinStatusCallback(BasicTposClientStatusCallback&, int delay = 0);
   void refreshMap();
   void refreshAccountData();
   void closeConnection(std::string a_connection);

private:
   TposClientConnectionImp * m_client;
};

#endif
