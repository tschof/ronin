// Connections.h: interface for the CConnections class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNECTIONS_H__C59C274C_7BA7_47D0_90DA_21CC9618D588__INCLUDED_)
#define AFX_CONNECTIONS_H__C59C274C_7BA7_47D0_90DA_21CC9618D588__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OMDSCommon.h"
#include "Log.h"	// Added by ClassView

#define	CONNECTION_STARTING		0x0001
#define CONNECTION_RUNNING		0x0000
#define	CONNECTION_SHUTDOWN		0x0002
#define CONNECTION_DOWN			0x1000
#define CONNECTION_CONFIGERROR	0x2000

class CConClient;
class CConClientMessage;
class CMessageProcessor;
class CDTibMessage;
class CMain;
class CConTib;

class CConnections // : 	public CConClientCallback
{
public:
	CConnections(CMain * pMP);
	virtual ~CConnections();

	int						getStatus();
	void					shutDown();
	void					onTibShutdown();
	void					sendHeartbeat();
	void					onClientShutdown(HANDLE thread);
	void					sendToClient(int clientId, const std::string &message);
	void					start();
	void					disconnectClient(int cid);
	CDTibMessage *			getNextTibMessage();
	CConClientMessage 		getNextClientMessage();
	CMessageProcessor *		getMessageProcessor();
	CConTib *				getConTib() { return m_pConTib; }
	CConClient *			getConClient() { return m_pConClient; }

	int		m_status;
private:
	// CMessageProcessor * m_pMessageProcessor;
	CLog			m_log;
	void			startClient();
	void			startTib();
	CConfiguration	m_config;
	CConTib *		m_pConTib;
	CConClient *	m_pConClient;
};

#endif // !defined(AFX_CONNECTIONS_H__C59C274C_7BA7_47D0_90DA_21CC9618D588__INCLUDED_)
