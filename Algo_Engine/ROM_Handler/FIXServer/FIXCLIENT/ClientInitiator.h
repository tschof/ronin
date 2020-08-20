// ClientInitiator.h: interface for the CClientInitiator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENTINITIATOR_H__F3C46399_4AEF_4285_810D_55A31D3A1ACF__INCLUDED_)
#define AFX_CLIENTINITIATOR_H__F3C46399_4AEF_4285_810D_55A31D3A1ACF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Initiator.h"
#include "SocketConnector.h"

class CClientInitiator : public FIX::Initiator, FIX::SocketConnector::Strategy  
{
public:
	CClientInitiator( FIX::Application& app, FIX::MessageStoreFactory& factory, const FIX::SessionSettings& settings);
	virtual ~CClientInitiator();
   
	virtual bool start();
	/// Implemented to start connecting to targets.
    virtual bool onStart( const FIX::SessionSettings& );
    /// Implemented to stop a running initiator.
    virtual void onStop();
    /// Implemented to connect a session to its target.
    virtual bool doConnect( const FIX::SessionID&, const FIX::Dictionary& );
   
	void onConnect( FIX::SocketConnector&, int );
    void onData( FIX::SocketConnector&, int );
    void onDisconnect( FIX::SocketConnector&, int );
    void onError( FIX::SocketConnector& );
    void onTimeout( FIX::SocketConnector& );
  
private:
	void getHost( const FIX::SessionID&, const FIX::Dictionary&, std::string&, short&);

private:
	FIX::SessionSettings m_settings;
	FIX::SocketConnector m_connector;
    
	int m_elapsedTimeouts;
    int m_reconnectInterval;
    bool m_stop;

};

#endif // !defined(AFX_CLIENTINITIATOR_H__F3C46399_4AEF_4285_810D_55A31D3A1ACF__INCLUDED_)
