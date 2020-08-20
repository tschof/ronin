// MessageApp.h: interface for the CMessageApp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGEAPP_H__CB666D07_26ED_11D2_8D11_006008CD4894__INCLUDED_)
#define AFX_MESSAGEAPP_H__CB666D07_26ED_11D2_8D11_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Clients.h"
//#include "CommonDatabase.h"
#include "Message.h"
#include "Order.h"
//#include "Cancel.h"
//#include "Replace.h"
//#include "Request.h"
#include "Execution.h"
//#include "CancelReject.h"
//#include "Reject.h"
#include "ExchangeSession.h"
#include "OrderHandler.h"
#include "Connection.h"
#include "X25HostConnection.h"
#include "ClientConnection.h"
#include "StatusHandler.h"
#include "Server.h"

class CMessageApp : public CWinApp  
{
protected:
	// client management
		CClients					m_Clients;					// list of currently connected clients
		CServer					*m_pServer;					// accepts client connections
	
	// database
		//CCommonDatabase		*m_pDb;

	// exchange handling
		CExchangeSession		*m_pExchange;

	// order handling
		COrderHandler			*m_pHandler;

	// host
		CX25HostConnection		*m_pHost;

	// status handling
		CStatusHandler			*m_pStatus;

	// map of trader to 

	CString						m_Name;
	CString						m_Path;

public:

	//CDatabase			*Database()				{ return m_pDb->Database();	}
	CServer				*Server()				{ return m_pServer;				}
	CExchangeSession	*Exchange()				{ return m_pExchange;			}
	COrderHandler		*Handler()				{ return m_pHandler;				}
	CX25HostConnection	*Host()					{ return m_pHost;					}
	CStatusHandler&	Status()					{ return *m_pStatus;				}
	CClients&			Clients()				{ return m_Clients;				}
	const CString&		Name()		const		{ return m_Name;					}
	const CString&		Path()		const		{ return m_Path;					}


	virtual void	ProcessStatus(const CString& Name, const CString& Message)	= 0;
	virtual void	ProcessError(const CString& Name, const CString& Error)		= 0;
	virtual void	ProcessTraffic(const CString& Name, const CString& Traffic)	= 0;
	virtual void	ProcessLogEntry(const CString& Name, const CString& Entry)	= 0;

	virtual void	AddHost()							{	}
	virtual void	DeleteHost()						{	}
	virtual void	UpdateHost()						{	}
	virtual void	SetStatusTimer(UINT Interval)	{	}

	void	MapConnection(CString &Tag, CString &StafTag, CString &Name)
	{
		m_MapTagToTag.SetAt(Tag, StafTag);
		m_MapTagToName.SetAt(Tag, Name);
	}
	void	RemoveTag(CString &Tag) { m_MapTagToTag.RemoveKey(Tag); m_MapTagToName.RemoveKey(Tag);		 }
	CString	GetTag(CString &Tag)	{ CString rValue; 
									  m_MapTagToTag.Lookup(Tag, rValue);  
									  return rValue; 
	}
	CString GetName(CString &Tag)	{ CString rValue; 
									  m_MapTagToName.Lookup(Tag, rValue); 
									  return rValue; 
	}

public:
	CMessageApp() : CWinApp()	{ }
	virtual ~CMessageApp()		{ }

private:
	CMapStringToString		m_MapTagToTag;
	CMapStringToString		m_MapTagToName;
};

#endif // !defined(AFX_MESSAGEAPP_H__CB666D07_26ED_11D2_8D11_006008CD4894__INCLUDED_)
