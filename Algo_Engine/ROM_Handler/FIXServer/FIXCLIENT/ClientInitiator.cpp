// ClientInitiator.cpp: implementation of the CClientInitiator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FixClient.h"
#include "ClientInitiator.h"
#include "FIX42_Messages.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClientInitiator::CClientInitiator( FIX::Application& app, FIX::MessageStoreFactory& factory, const FIX::SessionSettings& settings)
: FIX::Initiator( app, factory, settings )
{

}

CClientInitiator::~CClientInitiator()
{

}
 
bool CClientInitiator::start()
{
    return onStart(m_settings);
}

bool CClientInitiator::onStart( const FIX::SessionSettings& s )
{
    try { m_reconnectInterval = s.get().getLong("ReconnectInterval"); }
    catch(std::exception&) {}

    connect();
	return true;
}

void CClientInitiator::onStop()
{

}

bool CClientInitiator::doConnect( const FIX::SessionID& s, const FIX::Dictionary& d)
{
    try
      {
        std::string address;
        short port = 0;

        getHost(s, d, address, port);

        int result = m_connector.connect(address, port,*this);
        if( !result ) return false;

        return true;
      }
    catch(std::exception&) { return false; }
	return true;
}

void CClientInitiator::onError( FIX::SocketConnector& connector)
{
}

void CClientInitiator::onTimeout( FIX::SocketConnector& connector)
{
}

void CClientInitiator::onData( FIX::SocketConnector& connector, int s )
{
}

void CClientInitiator::onDisconnect( FIX::SocketConnector&, int s )
{
}

void CClientInitiator::getHost( const FIX::SessionID& s, const FIX::Dictionary& d,
                             std::string& address, short& port)
{
	address = d.getString("SocketConnectHost");
	port = d.getLong("SocketConnectPort");
}

void CClientInitiator::onConnect( FIX::SocketConnector& connector, int s )
{
	FIX42::Logon logon;
	
}
