#include "stdafx.h"
#include "FIXServer.h"
#include "OMMessage.h"
#include "OMSocketInitiator.h"
#include "OMSocketAcceptor.h"
#include "SessionSettings.h"
#include "SocketInitiator.h"
#include "MessageStore.h"
#include "FileStore.h"
#include "SocketAcceptor.h"
#include "TranslationMessages.h"
#include "FIX42_Messages.h"
#include <iostream>
#include <time.h>
#include "Log.h"
#include "dbo.h"


#pragma comment( lib, "libcs.lib")
#pragma comment( lib, "libct.lib")


//static FIX::Mutex s_coutMutex;

FIXServer * FIXServer::m_pServer = NULL;
std::string FIXServer::m_omFile = "";
std::string FIXServer::m_fixFile = "";

FIX::SessionID FIXServer::m_clientID(FIX::BeginString(BeginString_FIX42),FIX::SenderCompID(""),FIX::TargetCompID("") );

FIXServer::FIXServer( const std::string& omFile, const std::string& fixFile )
	: m_omConfigFile(omFile), m_fixConfigFile(fixFile), m_isAccepting(false), m_pMsgStoreFactory(NULL), m_pFixSetting(NULL), m_pOmSetting(NULL), m_bLoadSymbol(false)
{
	m_pFixSetting = new FIX::SessionSettings(fixFile);
	m_pOmSetting = new FIX::SessionSettings(omFile);

	FIX::Dictionary dict = m_pFixSetting->get();

	std::string szPath = "";
	try
	{
		szPath = dict.getString("LogPath");
	}
	catch(...)
	{
		char pszPath[MAX_PATH];
		GetModuleFileName(NULL,pszPath,sizeof(pszPath));
		szPath = pszPath;
		int nPos = szPath.find_last_of('\\');
		szPath = szPath.substr(0, nPos );
	}
	try
	{
		std::string szDest = dict.getString("DefaultDest");
		m_nDefaultDest = atoi(szDest.c_str());
	}
	catch(...)
	{
		m_nDefaultDest = -1;
	}
	Log::Initialize(szPath,"FIXServer");
	try
	{
		char szBuf[256];
		char pszPath[MAX_PATH];
		GetCurrentDirectory( sizeof(pszPath), pszPath );
		strcat( pszPath, "\\" );
		strcat( pszPath, fixFile.c_str() );
		GetPrivateProfileString( "DEFAULT", "DbServer", "", szBuf, sizeof(szBuf), pszPath );
		szDbServer = szBuf;
		GetPrivateProfileString( "DEFAULT", "DbUser", "", szBuf, sizeof(szBuf), pszPath );
		szDbUser = szBuf;
		GetPrivateProfileString( "DEFAULT", "DbPass", "", szBuf, sizeof(szBuf), pszPath );
		szDbPass = szBuf;
		GetPrivateProfileString( "SESSION", "TargetCompID", "", szBuf, sizeof(szBuf), pszPath );
		szOmUser = szBuf;
        GetPrivateProfileString( "SESSION", "SenderCompID", "", szBuf, sizeof(szBuf), pszPath );
		szSenderID = szBuf;
		if( szOmUser.empty() || szDbServer.empty() || szDbUser.empty() || szDbPass.empty() )
		{
//			std::cout << "missing database setting in fix.cfg\n";
			return;
		}
        
		// Engine Mode: 0 for drop copy, 1 for order taking, default to drop copy mode
		m_bSmartRoute = GetPrivateProfileInt( "DEFAULT", "SmartRoute", 0, pszPath ) == 0 ? false : true;

		m_bLoadSymbol = GetPrivateProfileInt( "DEFAULT", "LoadSymbol", 0, pszPath ) == 0 ? false : true;
		if( m_bLoadSymbol )
			LoadSymbols();
		if( !LoadAccount() )
			throw exception("Fail to load account from database");
	}
	catch(...)
	{
		Log::Write( LOG_ERROR, "Initialization Failure" );
		throw exception("Initialization Failure");
	}
//	std::cout << "Fix Server 1.0.0 ( Log path: " << szPath << " )\n";
	m_pMsgStoreFactory = new FIX::FileStoreFactory(szPath);
	/*
	HRESULT hr = m_spQuotes.CoCreateInstance( __uuidof(QuoteManager) );
	if( FAILED( hr ) )
		Log::Write( LOG_ERROR, "Fail to create quote manager" );
	*/
}



FIXServer::~FIXServer()
{
	delete m_pFixSetting;
	delete m_pOmSetting;
	delete m_pMsgStoreFactory;
	m_Accounts.clear();
}

void FIXServer::Start()
{
    thread_spawn(&startThread, this);
}

void* FIXServer::startThread( void* p )
{
	FIXServer * pServer = (FIXServer *)p;
	pServer->Connect();	
	return NULL;
}

void FIXServer::onCreate( const FIX::SessionID& sessionID )
{

}

void FIXServer::onLogon( const FIX::SessionID& sessionID )
{
//	FIX::Locker lock(s_coutMutex);
	std::cout << "Logon - " << sessionID << std::endl;
	m_clientID = sessionID;
	std::stringstream stm;
	stm << "FIXServer::onLogon, SessionID = " << sessionID;
	Log::Write( LOG_DEBUG, stm.str() );
	SendQueuedMessage( sessionID );
}

void FIXServer::onLogout( const FIX::SessionID& sessionID )
{
//	FIX::Locker lock(s_coutMutex);
	std::cout << "Logout - " << sessionID << std::endl;
	std::stringstream stm;
	stm << "FIXServer::onLogout, SessionID = " << sessionID;
	Log::Write( LOG_DEBUG, stm.str() );

    OM::Message mess('c', 3);
    time_t t;
    time(&t);
    std::stringstream stream;
    
    tm* pTimeStruct = localtime(&t);
    
    stream 
        << std::setw(2) << std::setfill('0') << pTimeStruct->tm_hour 
        << std::setw(2) << std::setfill('0') << pTimeStruct->tm_min 
        << std::setw(2) << std::setfill('0') << pTimeStruct->tm_sec;
    mess.SetField(1, stream.str());
    mess.SetField(2, szSenderID);
    OM::Session::sendToTarget(mess);

}

void FIXServer::toApp( FIX::Message& message, const FIX::SessionID& sessionId ) throw(FIX::DoNotSend&)
{
//	FIX::Locker lock(s_coutMutex);
//	std::cout << "Sent - " << sessionId << " - " << message << std::endl;
}

void FIXServer::fromApp( const FIX::Message& message, const FIX::SessionID& sessionID ) 
	throw(FIX::FieldNotFound&, FIX::UnsupportedMessageType&, FIX::IncorrectTagValue&)
{
	{
//		FIX::Locker lock(s_coutMutex);
//		std::cout << "Received - " << sessionID << " - " << message << std::endl;
		Log::Write( LOG_MESSAGE, "Received " + message.getString() );
	}
	static_cast<FIX::MessageCracker*>(this)->crack(message, sessionID);
}

void FIXServer::onRun()
{
	FIX::Locker lock(m_mutex);
	if(!m_isAccepting)
	{
		m_isAccepting = true;
		m_mutex.unlock();

		Accept();

		m_mutex.lock();
		m_isAccepting = false;
		m_mutex.unlock();
	}
	else
	{
		m_mutex.unlock();
		while(true)
			Sleep(1000);
	}
}

void FIXServer::onMessage( const FIX42::NewOrderSingle& single, const FIX::SessionID& sessionId )
{
	try
	{
		OM::Message message = m_manager.Translate( single );
		OM::Session::sendToTarget(message);
	}
	catch(FIX::RequiredTagMissing ex)
	{
		int nSeq = atoi(single.getHeader().getField(FIELD::MsgSeqNum).c_str());
		FIX42::Reject reject;
		reject.setSessionID( FIXServer::clientSessionID() );
		reject.set( FIX::RefSeqNum(nSeq) );
		reject.set( FIX::Text(ex.what()) );
		FIX::Session::sendToTarget(reject);
	}
	catch( exception & ex )
	{
		int nSeq = atoi(single.getHeader().getField(FIELD::MsgSeqNum).c_str());
		FIX42::Reject reject;
		reject.setSessionID( FIXServer::clientSessionID() );
		reject.set( FIX::RefSeqNum(nSeq) );
		reject.set( FIX::Text(ex.what()) );
		FIX::Session::sendToTarget(reject);
	}
	catch(...)
	{
		int nSeq = atoi(single.getHeader().getField(FIELD::MsgSeqNum).c_str());
		FIX42::Reject reject;
		reject.setSessionID( FIXServer::clientSessionID() );
		reject.set( FIX::RefSeqNum(nSeq) );
		reject.set( FIX::Text("Unknown exception") );
		FIX::Session::sendToTarget(reject);
	}
}

void FIXServer::onMessage( const FIX42::OrderCancelRequest& cancel, const FIX::SessionID& sessionId )
{
	try
	{
		OM::Message message = m_manager.Translate( cancel );
		OM::Session::sendToTarget(message);
	}
	catch( ... )
	{
		int nSeq = atoi(cancel.getHeader().getField(FIELD::MsgSeqNum).c_str());
		FIX42::Reject reject;
		reject.setSessionID( FIXServer::clientSessionID() );
		reject.set( FIX::RefSeqNum(nSeq) );
		reject.set( FIX::Text("Unknown exception") );
		FIX::Session::sendToTarget(reject);
	}
}

void FIXServer::onMessage( const FIX42::OrderCancelReplaceRequest& replace, const FIX::SessionID& sessionId )
{
	try
	{
		OM::Message message = m_manager.Translate( replace );
		OM::Session::sendToTarget(message);
	}
	catch( ... )
	{
		int nSeq = atoi(replace.getHeader().getField(FIELD::MsgSeqNum).c_str());
		FIX42::Reject reject;
		reject.setSessionID( FIXServer::clientSessionID() );
		reject.set( FIX::RefSeqNum(nSeq) );
		reject.set( FIX::Text("Unknown exception") );
		FIX::Session::sendToTarget(reject);
	}
}

void FIXServer::toApp( const OM::Message& message, const FIX::SessionID& ) throw(FIX::DoNotSend&) 
{
//	FIX::Locker lock(s_coutMutex);
//	std::cout << "Sent - OM - " << message << std::endl;
}

void FIXServer::fromApp( const OM::Message& message, const FIX::SessionID& sessId ) throw(FIX::FieldNotFound&, FIX::UnsupportedMessageType&, FIX::IncorrectTagValue&) 
{
	{
//		FIX::Locker lock(s_coutMutex);
//		std::cout << "Recevied - OM - " << message << std::endl;
		Log::Write( LOG_MESSAGE, "Recevied " + message.getString() );
	}
	try
	{
		static_cast<OM::MessageCracker*>(this)->crack(message);
	}
	catch( exception & ex )
	{
		Log::Write( LOG_ERROR, ex.what() );
	}
	catch(...)
	{
		Log::Write( LOG_ERROR, "Unknown exception when cracking OM messages" );
	}
}

FIX::SessionID FIXServer::GetSessionID( const OM::EnterOrder & order )
{
	std::string beginString,senderCompID,targetCompID;
	std::string omTag = order.GetTag();
	int nPos = omTag.find('-');
	if( nPos == -1 )
		throw exception("Order tag not created by fix server");
	omTag = omTag.substr( 0, nPos );
	nPos = omTag.find('|');
	if( nPos == -1 )
		throw exception("Order tag not created by fix server");
	beginString = omTag.substr( 0, nPos );
	omTag = omTag.substr( nPos+1, -1 );
	nPos = omTag.find('|');
	if( nPos == -1 )
		throw exception("Order tag not created by fix server");
	senderCompID = omTag.substr( 0, nPos );
	targetCompID = omTag.substr( nPos+1, -1 );
	return FIX::SessionID(beginString,senderCompID,targetCompID);
}

void FIXServer::onMessage( const OM::EnterOrder& order )
{
}

void FIXServer::onMessage( const OM::Status& status )
{
	try
	{
		FIX42::Message message = m_manager.Translate(status);
		Log::Write( LOG_MESSAGE, "Sending " + message.getString() );
		FIX::Session::sendToTarget(message);
	}
	catch( TranslationError& )
	{
		Log::Write( LOG_ERROR, "FIXServer::onMessage( const OM::Status& status ) TranlationError: " + status.getString() );
	}
	catch( SessionNotFound & )
	{
		FIX::Locker lock(m_mutex);
		m_outQueue.push( status );
//		std::cout << "SessionNotFound\n";
		Log::Write( LOG_ERROR, "FIXServer::onMessage( const OM::Status& status ) SessionNotFound: " + status.getString() );
	}
	catch(...)
	{
		Log::Write( LOG_ERROR, "FIXServer::onMessage( const OM::Status& status ) Unknown Exception: " + status.getString() );
	}
}

void FIXServer::SendQueuedMessage( const FIX::SessionID& sessionID )
{
	try
	{
		FIX::Locker lock(m_mutex);
		while( m_outQueue.size() )
		{
			FIX42::Message message = m_manager.Translate(m_outQueue.front());
			m_outQueue.pop();
			Log::Write( LOG_MESSAGE, "Sending " + message.getString() );
			FIX::Session::sendToTarget(message);
		}
	}
	catch( ... )
	{
		Log::Write( LOG_ERROR, "FIXServer::SendQueuedMessage() exception" );
	}
}

void FIXServer::onMessage( const OM::Query& query )
{

}

void FIXServer::onMessage( const OM::Invalid& invalid )
{

}

void FIXServer::onMessage( const OM::Logout& logout )
{

}

void FIXServer::Connect()
{
	try
	{
		//FIX::SocketInitiator initiator(*this, *m_pMsgStoreFactory, *m_pOmSetting);
		OM::SocketInitiator initiator(*this, *m_pOmSetting);
		initiator.start();
	}
	catch(FIX::ConfigError& e)
	{
//		FIX::Locker lock(s_coutMutex);
//		std::cout << e.what();
	}
}


void FIXServer::Accept()
{
	try
	{
		FIX::SocketAcceptor acceptor(*this, *m_pMsgStoreFactory, *m_pFixSetting);
		//OM::SocketAcceptor acceptor(*this, *m_pMsgStoreFactory, *m_pFixSetting);
		acceptor.start();
	}
	catch(...)
	{
	}
}

std::string FIXServer::GetRetailAccount( const AccountID & id )
{
    m_mutex.lock();
	tAccountMap::iterator it = m_Accounts.find(id);
	if( it == m_Accounts.end() )
	{
	    m_mutex.unlock();
		throw exception("Account not found");
	}
	std::string retail = (*it).second.m_szRetailAcct;
	m_mutex.unlock();
	return retail;
}

std::string FIXServer::GetOMAccount( const AccountID & id )
{
    m_mutex.lock();
	tAccountMap::iterator it = m_Accounts.find(id);
	if( it == m_Accounts.end() )
	{
	    m_mutex.unlock();
		throw exception("Account not found");
	}
	std::string omAcct = (*it).second.m_szOMAcct;
	m_mutex.unlock();
	return omAcct;
}

bool FIXServer::LoadAccount()
{
	DboConnection db( szDbServer.c_str(), szDbUser.c_str(), szDbPass.c_str(), "OMFIX" );
	DboCommand cmd( &db, "rom.dbo.OMFIX_getAcctInfo" );
	cmd.paramIn( "@firmFixLogin", (char *)szOmUser.c_str() );

	cmd.exec();
	if( cmd.status() != DBO_SUCCESS )
	{
		Log::Write( LOG_ERROR, "rom.dbo.OMFIX_getAcctInfo failed" );
		return false;
	}
    m_mutex.lock();
	m_Accounts.clear();
	DboResults * pRes = NULL;
	for( int i = 0; i < cmd.resultsCount(); i++ )
	{
		pRes = cmd.results(i);
		for( int j = 0; j < pRes->rowCount(); j++ )
		{
			try
			{
				pRes->rowSet( j );
				char szBuff[128];
				pRes->colValue( 0, szBuff, sizeof(szBuff) );
				std::string szTraderAcr = szBuff;
				pRes->colValue( 1, szBuff, sizeof(szBuff) );
				std::string szLocalAcAcr = szBuff;
				pRes->colValue( 2, szBuff, sizeof(szBuff) );
				std::string szClearingAcID = szBuff;
				AccountID::TrimRight( szClearingAcID );
				pRes->colValue( 3, szBuff, sizeof(szBuff) );
				std::string szFirmAcr = szBuff;
				pRes->colValue( 4, szBuff, sizeof(szBuff) );
				std::string szRetailAccount = szBuff;
				pRes->colValue( 5, szBuff, sizeof(szBuff) );
				std::string szOMAccount = szBuff;
				AccountID::TrimRight( szOMAccount );
				pRes->colValue( 6, szBuff, sizeof(szBuff) );
				std::string szType = szBuff;
				AccountID::TrimRight( szType );
				AccountID id( szTraderAcr, szLocalAcAcr, szClearingAcID, szFirmAcr, szType );
				OMAccount acct( id, szRetailAccount, szOMAccount );
				m_Accounts.insert( std::make_pair<AccountID,OMAccount>(id,acct) );
			}
			catch(...)
			{
				continue;
			}
		}
	}
    m_mutex.unlock();
//	std::cout << "Account Loaded\n";
	return true;
}

bool FIXServer::LoadSymbols()
{
	if( !LoadOptionSymbols() )
		return false;
	if( !LoadFutureSymbols() )
		return false;
	return true;
}

bool FIXServer::LoadOptionSymbols()
{
	DboConnection db( szDbServer.c_str(), szDbUser.c_str(), szDbPass.c_str(), "OMFIX" );
	DboCommand cmd( &db, "rom.dbo.OM_FIX_selOptClass" );
	cmd.exec();
	if( cmd.status() != DBO_SUCCESS )
	{
		Log::Write( LOG_ERROR, "rom.dbo.OM_FIX_selOptClass failed" );
//		std::cout << "\nFail to load symbol\n";
		return false;
	}
    m_mutex.lock();
	m_OptionSymbols.clear();
	DboResults * pRes = NULL;
	for( int i = 0; i < cmd.resultsCount(); i++ )
	{
		pRes = cmd.results(i);
		for( int j = 0; j < pRes->rowCount(); j++ )
		{
			try
			{
				pRes->rowSet( j );
				char szBuff[128];
				pRes->colValue( 0, szBuff, sizeof(szBuff) );
				std::string symbol = szBuff;
				int index = symbol.find( ' ' );
				if( index != -1 )
					symbol = symbol.substr( 0, index );
				pRes->colValue( 1, szBuff, sizeof(szBuff) );
				std::string under = szBuff;
				index = under.find( ' ' );
				if( index != -1 )
					under = under.substr( 0, index );
				m_OptionSymbols.insert( std::make_pair<std::string,std::string>(symbol,under) );
			}
			catch(...)
			{
				continue;
			}
		}
	}
    m_mutex.unlock();
//	std::cout << "\nOption Symbol Loaded\n";
	return true;
}

bool FIXServer::LoadFutureSymbols()
{
	DboConnection db( szDbServer.c_str(), szDbUser.c_str(), szDbPass.c_str(), "OMFIX" );
	DboCommand cmd( &db, "rom.dbo.OM_FIX_selFutureBase" );
	cmd.exec();
	if( cmd.status() != DBO_SUCCESS )
	{
		Log::Write( LOG_ERROR, "rom.dbo.OM_FIX_selFutureBase failed" );
//		std::cout << "\nFail to load symbol\n";
		return false;
	}
    m_mutex.lock();
	m_FutureSymbols.clear();
	DboResults * pRes = NULL;
	for( int i = 0; i < cmd.resultsCount(); i++ )
	{
		pRes = cmd.results(i);
		for( int j = 0; j < pRes->rowCount(); j++ )
		{
			try
			{
				pRes->rowSet( j );
				char szBuff[128];
				pRes->colValue( 0, szBuff, sizeof(szBuff) );
				std::string symbol = szBuff;
				int index = symbol.find( ' ' );
				if( index != -1 )
					symbol = symbol.substr( 0, index );
				pRes->colValue( 1, szBuff, sizeof(szBuff) );
				std::string under = szBuff;
				index = under.find( ' ' );
				if( index != -1 )
					under = under.substr( 0, index );
				m_FutureSymbols.insert( std::make_pair<std::string,std::string>(symbol,under) );
			}
			catch(...)
			{
				continue;
			}
		}
	}
    m_mutex.unlock();
//	std::cout << "\nFuture Symbol Loaded\n";
	return true;
}

std::string FIXServer::FindOptionUnderlying( const std::string & szSymbol )
{
    m_mutex.lock();
	tSymbolMap::iterator it = m_OptionSymbols.find(szSymbol);
	if( it == m_OptionSymbols.end() )
	{
		DboConnection db( szDbServer.c_str(), szDbUser.c_str(), szDbPass.c_str(), "OMFIX" );
		DboCommand cmd( &db, "rom.dbo.OM_FIX_selOptClass" );
		cmd.paramIn( "@class", (char *)szSymbol.c_str() );
		cmd.exec();
		if( cmd.status() != DBO_SUCCESS )
		{
			m_mutex.unlock();
			Log::Write( LOG_ERROR, "rom.dbo.OM_FIX_selOptClass failed" );
//			std::cout << "\nFail to load symbol for " << szSymbol << std::endl;
			throw exception("Unable to find underlying");
		}
		DboResults * pRes = NULL;
		for( int i = 0; i < cmd.resultsCount(); i++ )
		{
			pRes = cmd.results(i);
			for( int j = 0; j < pRes->rowCount(); j++ )
			{
				try
				{
					pRes->rowSet( j );
					char szBuff[128];
					pRes->colValue( 0, szBuff, sizeof(szBuff) );
					std::string symbol = szBuff;
					int index = symbol.find( ' ' );
					if( index != -1 )
						symbol = symbol.substr( 0, index );
					pRes->colValue( 1, szBuff, sizeof(szBuff) );
					std::string under = szBuff;
					index = under.find( ' ' );
					if( index != -1 )
						under = under.substr( 0, index );
					m_OptionSymbols.insert( std::make_pair<std::string,std::string>(symbol,under) );
				}
				catch(...)
				{
					continue;
				}
			}
		}
		it = m_OptionSymbols.find(szSymbol);
		if( it == m_OptionSymbols.end() )
		{
			m_mutex.unlock();
			throw exception("Unable to find underlying");
		}
	}
	std::string szUnder = (*it).second;
	m_mutex.unlock();
	return szUnder;
}

std::string FIXServer::FindFutureUnderlying( const std::string & szSymbol )
{
    m_mutex.lock();
	tSymbolMap::iterator it = m_FutureSymbols.find(szSymbol);
	if( it == m_FutureSymbols.end() )
	{
		DboConnection db( szDbServer.c_str(), szDbUser.c_str(), szDbPass.c_str(), "OMFIX" );
		DboCommand cmd( &db, "rom.dbo.OM_FIX_selOptClass" );
		cmd.paramIn( "@class", (char *)szSymbol.c_str() );
		cmd.exec();
		if( cmd.status() != DBO_SUCCESS )
		{
			m_mutex.unlock();
			Log::Write( LOG_ERROR, "rom.dbo.OM_FIX_selOptClass failed" );
//			std::cout << "\nFail to load symbol for " << szSymbol << std::endl;
			throw exception("Unable to find underlying");
		}
		DboResults * pRes = NULL;
		for( int i = 0; i < cmd.resultsCount(); i++ )
		{
			pRes = cmd.results(i);
			for( int j = 0; j < pRes->rowCount(); j++ )
			{
				try
				{
					pRes->rowSet( j );
					char szBuff[128];
					pRes->colValue( 0, szBuff, sizeof(szBuff) );
					std::string symbol = szBuff;
					int index = symbol.find( ' ' );
					if( index != -1 )
						symbol = symbol.substr( 0, index );
					pRes->colValue( 1, szBuff, sizeof(szBuff) );
					std::string under = szBuff;
					index = under.find( ' ' );
					if( index != -1 )
						under = under.substr( 0, index );
					m_FutureSymbols.insert( std::make_pair<std::string,std::string>(symbol,under) );
				}
				catch(...)
				{
					continue;
				}
			}
		}
		it = m_FutureSymbols.find(szSymbol);
		if( it == m_FutureSymbols.end() )
		{
			m_mutex.unlock();
			throw exception("Unable to find underlying");
		}
	}
	std::string szUnder = (*it).second;
	m_mutex.unlock();
	return szUnder;
}
// msxsl OMSchema.xml xml2h.xsl -o OMMessages.h

int FIXServer::GetOrderRoute( const FIX42::NewOrderSingle& single )
{
	if( !m_bSmartRoute )
	{
		if( m_nDefaultDest == -1 )
			throw exception("no route set");
		return m_nDefaultDest;
	}

	/*
	try
	{
		std::string szSymbol;
		szSymbol = single.get(FIX::Symbol());
		FIX::Side side;
		single.get(side);
		CComPtr<IQuote> spQuote;
		spQuote = m_spQuotes->GetStockQuote( _bstr_t( szSymbol.c_str() ), _bstr_t("BBO") );
		if( side == FIX::Side_BUY )
		{
			_bstr_t szVendor = spQuote->GetAskSource();
		}
		else
		{
			_bstr_t szVendor = spQuote->GetBidSource();
		}
	}
	catch(...)
	{
		Log::Write( LOG_ERROR, "Fail to get BBO Quote" );
		return m_nDefaultDest;
	}
	*/
	return m_nDefaultDest;
}
