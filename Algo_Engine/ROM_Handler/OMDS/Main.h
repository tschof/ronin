// Main.h: interface for the CMain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAIN_H__3EEA92DA_8E56_45AC_BD9A_C7A08511DB11__INCLUDED_)
#define AFX_MAIN_H__3EEA92DA_8E56_45AC_BD9A_C7A08511DB11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OMDSCommon.h"
#include <time.h>
#include <stdio.h>
#include <string>
#include "Lock.h"
#include "Entitlement.h"

#define MESSAGE_PROCESS_THREAD_COUNT	10
#define	STATE_CHECK_TIME				15
#define CLEAN_REQUESTS_TIME             (60*15)
#define	INIT_FILE						"symbolCache.txt"

#define WM_CLIENT_UPDATE			WM_USER+1
#define WM_REQUEST_UPDATE			WM_USER+2
#define WM_RESPONSE_UPDATE			WM_USER+3
#define WM_IN_TRANS					WM_USER+4
#define WM_OUT_TRANS				WM_USER+5
#define WM_ERROR_UPDATE				WM_USER+6
#define WM_WARNING_UPDATE			WM_USER+7
#define WM_CLIENT_CLOSE				WM_USER+8

class CConnections;
class CDataManager;
class CMessageProcessor;
class CThreadClientProcessor;
class CThreadTibProcessor;
class CUserData;
typedef std::list<CThreadClientProcessor *>	TCPList;
typedef std::list<CThreadTibProcessor *>	TTPList;

class EntitlementException : public exception
{
public:
	EntitlementException( const char * reason ) : exception( reason ) {}
	virtual ~EntitlementException() {}
};

class CMain
{
	CMain();
public:
	static CMain * getInstance()
	{
		if( !m_pMain )
			m_pMain = new CMain();
		return m_pMain;
	}
	

	CConnections *		getConnections		();
	CMessageProcessor * getMessageProcessor	();
	CDataManager *		getDataManager		() { return m_pDataManager; }
	void initializeCache(std::string fileName);
	void shutDown();
	void run();
	virtual ~CMain();

	void SetHwnd( HWND hWnd ) { m_hWnd = hWnd; }
	void ClearConnection();
	void AddConnection();
	void CloseConnection( int cid );
	void RemoveConnection();
	void UpdateMsgIn( long nSize );
	void UpdateMsgOut( long nSize );
	void UpdateInTransaction();
	void UpdateOutTransaction();
	void UpdateWarning();
	void UpdateError();

	void AddError( const std::string  & szError );
	void AddWarning( const std::string & szWarning );
	bool GetNextError( std::string & szError );
	bool GetNextWarning( std::string & szWarning );

	bool IsValidLogin( const std::string & szMsg, std::string & szUser );
private:
	bool	ConnectDB	();
	void	CloseDB		();

	CConnections *		m_pConnections;
	CDataManager *		m_pDataManager;
	CMessageProcessor *	m_pMessageProcessor;
	TCPList				m_clientProcList;
	TTPList				m_tibProcList;
	long				m_nClients;
	long				m_nMsgIn;
	long				m_nMsgOut;
	long				m_nTibSubject;
	long				m_nClientSubject;
	static CMain		* m_pMain;
public:
	HWND				m_hWnd;
	CLog				m_log;
	CConfiguration		m_config;
	bool				m_bEnableSecurity;
	std::list<std::string>	m_lstErrors;
	std::list<std::string>	m_lstWarnings;
	Lock				m_Lock;

	CEntitlement		m_Entitlement;
};

#endif // !defined(AFX_MAIN_H__3EEA92DA_8E56_45AC_BD9A_C7A08511DB11__INCLUDED_)
