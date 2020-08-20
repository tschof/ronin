// MessageProcessor.h: interface for the CMessageProcessor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGEPROCESSOR_H__F2CB2DE8_C5AF_4413_BB3A_5F0E44410055__INCLUDED_)
#define AFX_MESSAGEPROCESSOR_H__F2CB2DE8_C5AF_4413_BB3A_5F0E44410055__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OMDSCommon.h"
#include "Connections.h"
#include "DATib.h"
#include "DataManager.h"
#include "Log.h"	
#include "Lock.h"
#include "RequestManager.h"

class CMessageProcessor//  :	public CDACTranslator
{
public:
	CMessageProcessor(CDataManager * pDM, CConnections * pC);
	virtual ~CMessageProcessor();

	CDATib *						getDataAdapterTib();

	void							processTibMessage();
	void							processClientRequest();
	
	ULONG							TibWait();
	void							TibRelease();

	ULONG							ClientWait();
	void							ClientRelease();

	RequestManager *				getRequestManager() { return m_pRequestManager; }
private:
	CLog							m_log;
	CDataManager *					m_pDataManager;
	CConnections *					m_pConnections;
	CDATib *						m_pDATib;

	HANDLE							m_ClientSemaphore;
	HANDLE							m_TibSemaphore;

	Lock							m_gLock;

	// <QS|MSFT>, <!QS|MSFT> would have the same request key <QS|MSFT>, <DELETE|ALL> is not a request key
	// CDCClientRequest is refCounted. addRef/relRef has to be done within the request lock
	RequestManager				*	m_pRequestManager;
};

#endif // !defined(AFX_MESSAGEPROCESSOR_H__F2CB2DE8_C5AF_4413_BB3A_5F0E44410055__INCLUDED_)
