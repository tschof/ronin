#ifndef FIXSERVER_H
#define FIXSERVER_H

#include <Mutex.h>
#include "Application.h"
#include "OMApplication.h"
#include "MessageCracker.h"
#include "OMMessageCracker.h"
#include "TranslationManager.h"
#include <string>
#include "AccountID.h"
#include "Account.h"
#include <map>
#include <queue>

/*
#include <atlbase.h>
#import "..\CMSRouter\QuoteEngine\QuoteEngine.tlb" no_namespace
#import "..\CMSRouter\DecisionEngine\DecisionEngine.tlb" no_namespace
*/
namespace FIX
{
	class MessageStoreFactory;
	class SessionSettings;
};

class FIXServer : public FIX::Application, public OM::Application, public FIX::MessageCracker, public OM::MessageCracker
{
		FIXServer(const std::string& omFile, const std::string& fixFile);
	public:
		virtual ~FIXServer();

		static FIXServer * getInst()
		{
			if( !m_pServer )
			{
				m_pServer = new FIXServer(FIXServer::m_omFile,FIXServer::m_fixFile);
			}
			return m_pServer;
		}

		void Start();

		void onCreate( const FIX::SessionID& );
		void onLogon( const FIX::SessionID& sessionID );
		void onLogout( const FIX::SessionID& sessionID );
		void toAdmin( FIX::Message&, const FIX::SessionID& ) {}
		void toApp( FIX::Message&, const FIX::SessionID& ) throw(FIX::DoNotSend&);
		void fromAdmin( const FIX::Message&, const FIX::SessionID& ) throw(FIX::FieldNotFound&, FIX::RejectLogon&) {}
		void fromApp( const FIX::Message& message, const FIX::SessionID& sessionID ) 
			throw(FIX::FieldNotFound&, FIX::UnsupportedMessageType&, FIX::IncorrectTagValue&);
		
		void onRun();

		void onMessage( const FIX42::NewOrderSingle&, const FIX::SessionID& );
		void onMessage( const FIX42::OrderCancelRequest&, const FIX::SessionID& );
		void onMessage( const FIX42::OrderCancelReplaceRequest&, const FIX::SessionID& );
  
		void toAdmin( const OM::Message&, const FIX::SessionID& ) {}
		void toApp( const OM::Message&, const FIX::SessionID& ) throw(FIX::DoNotSend&);
		void fromAdmin( const OM::Message&, const FIX::SessionID& ) throw(FIX::FieldNotFound&, FIX::RejectLogon&) {}
		void fromApp( const OM::Message&, const FIX::SessionID& ) throw(FIX::FieldNotFound&, FIX::UnsupportedMessageType&, FIX::IncorrectTagValue&);
	
		void onMessage( const OM::Login& ) {}
		void onMessage( const OM::EnterOrder& );
		void onMessage( const OM::Status& );
		void onMessage( const OM::Query& );
		void onMessage( const OM::Invalid& );
		void onMessage( const OM::Logout& );
	
		static FIX::SessionID & clientSessionID() { return m_clientID; }

		std::string GetRetailAccount( const AccountID & id );
		std::string GetOMAccount( const AccountID & id );

		std::string FindOptionUnderlying( const std::string & szSymbol );
		std::string FindFutureUnderlying( const std::string & szSymbol );
		bool	LoadAccount();
		bool	LoadSymbols();
		int		GetOrderRoute( const FIX42::NewOrderSingle& single  );

	private:
		bool LoadOptionSymbols();
		bool LoadFutureSymbols();
		void Connect();
		void Accept();
		void SendQueuedMessage( const FIX::SessionID& sessionID );
		FIX::SessionID GetSessionID( const OM::EnterOrder & order );

		std::string m_omConfigFile;
		std::string m_fixConfigFile;
		bool m_isAccepting;
		FIX::Mutex m_mutex;

		TranslationManager m_manager;
		FIX::MessageStoreFactory * m_pMsgStoreFactory;
		FIX::SessionSettings *	m_pFixSetting;
		FIX::SessionSettings *	m_pOmSetting;

		typedef std::map<AccountID,OMAccount>	tAccountMap;
		tAccountMap				m_Accounts;

		typedef std::map<std::string,std::string>	tSymbolMap;
		tSymbolMap				m_OptionSymbols;
		tSymbolMap				m_FutureSymbols;

		typedef std::queue<OM::Status> tOutQueue;
		tOutQueue				m_outQueue;

		static FIX::SessionID	m_clientID;
		static FIXServer *		m_pServer;
		static void* startThread( void* p );

		std::string szDbServer, szDbUser, szDbPass, szOmUser, szSenderID;

	public:
		int						m_nDefaultDest;
		bool					m_bSmartRoute;
		bool					m_bLoadSymbol;
//		CComPtr<IQuoteManager>	m_spQuotes;

		static std::string		m_omFile;
		static std::string		m_fixFile;
};

#endif
