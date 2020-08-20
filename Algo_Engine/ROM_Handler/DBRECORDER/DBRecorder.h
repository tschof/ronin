// DBRecorder.h : main header file for the DBRECORDER application
//

#if !defined(AFX_DBRECORDER_H__E2E8B3C6_3B63_11D2_8D1C_00104B6DEAA9__INCLUDED_)
#define AFX_DBRECORDER_H__E2E8B3C6_3B63_11D2_8D1C_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "TradeTable.h"
#include "resource.h"       // main symbols
#include "RecorderView.h"
#include "TrafficView.h"
#include "Clients.h"
#include "Server.h"
#include "CommonDatabase.h"
#include "Message.h"
#include "OrderSet.h"
#include "TradeTableSet.h"
#include "Connection.h"
#include "ClientConnection.h"
#include "RvOrderHandler.h"
#include "LockList.h"
#include "MessageApp.h"
#include "MemoryMap.h"

/////////////////////////////////////////////////////////////////////////////
// CRecorderApp:
// See DBRecorder.cpp for the implementation of this class
//

UINT RecordingProc(LPVOID pParam);

class CThreadObject : public CObject
{
public:
	CMessage *pMsg;
	CBaseConnection *client;

	CThreadObject(CMessage* initMsg, CBaseConnection* initClient)
	{
		pMsg = initMsg;
		client = initClient;
	}
	~CThreadObject()
	{
		//delete pMsg;
	}
};


//static CEvent g_KillEvent;

class CRecorderApp : public CMessageApp
{
private:
	CStdioFile			m_Log;

	CRecorderView		*m_pClientView;
	CTrafficView		*m_pTrafficView;
	CTrafficView		*m_pStatusView;

	CWinThread			*m_pRecordingThread;

	//CLockList			*m_pMessageQueue;
	//CSemaphore			m_locker;
	
	CTrades* m_pTRADES;
	// database
	//CCommonDatabase	*m_pDb;
	CCommonDatabase		*m_pTradeDb;
	CCommonDatabase		*m_pPrimaryOrderDb;
	CCommonDatabase		*m_pBackupOrderDb;
	BOOL				m_UseODBC;

	// rv
	CString				m_BroadcastSubject;
	CString				m_XcalBroadcastSubject;
	
	COrderSet			*m_pPrimaryOrders;
	COrderSet			*m_pBackupOrders;
	CTradeTableSet		*m_pTrades;
	CTradeTable			*m_pTradeTable;
	//CMemoryMap<COrder>	*m_pMemoryMap;
	BOOL				bConnected;

	void reConnect();

	//void BuildParmString1(CCsv& Exec, CString &ParmString);
	//void BuildParmString2(CCsv& Exec, CString &ParmString);
	void BuildParmString1(TRADE* pTrade, CString &ParmString);
	void BuildParmString2(TRADE* pTrade, CString &ParmString);
	void FormatFailureMsg(CString &ParmString, int ErrorCode, CString &MsgOut);
	void BuildTradeMessage(CString& Msg, TRADE* pTrade);

	CStdioFile				m_DBRecordLog;

	virtual void ProcessDBRecordLog(const CString& Msg)
	{
		ProcessLog(m_DBRecordLog, "DBRecord", Msg, "");
	}


public:

	enum ERROR_CODE
	{
		DB_DISCONNECT = -100
	};

	CDatabase			*TradeDatabase()		{ return m_pTradeDb->Database();		}
	CDatabase			*PrimaryDatabase()		{ return m_pPrimaryOrderDb->Database();	}
	CDatabase			*BackupDatabase()		{ return m_pBackupOrderDb->Database();	}
	//CMemoryMap<COrder>	*MemoryMap()			{ return m_pMemoryMap;					}
	//CLockList			*MessageQueue()			{ return m_pMessageQueue;				}

	//void Lock()	 { m_locker.Lock();   }
	//void Unlock() { m_locker.Unlock(); }
	CTrades* Trades() { return m_pTRADES; }
	int  AddTrade(TRADE* pTrade);

public:
	void ProcessClientMessage(CBaseConnection &pC, CMessage &Message)
	{
		// For more efficient multi-threaded processing
		StoreMessage(&Message, &pC); 
		if (&Message != NULL)
		{
			delete &Message;
		}
	}
	BOOL ProcessClientLogon(CBaseConnection& pC, CCsv& CSV);
	BOOL ProcessClientLogout(CBaseConnection &pC, CCsv& CSV);
	//BOOL ProcessClientOrder(CBaseConnection& pC, CCsv& CSV);
	//BOOL ProcessClientStatus(CBaseConnection& pC, CCsv& CSV);
	//BOOL ProcessClientZap(CBaseConnection& pC, CCsv& CSV);
	//BOOL ProcessClientTradeReport(CBaseConnection& pC, CCsv& CSV);

	void ProcMessage(CBaseConnection &pC, CMessage &Message)
	{
		// For more efficient multi-threaded processing
		CMessageApp::ProcessClientMessage(pC, Message);
	}

	void ProcessStatus(const CString& Name, const CString& Message, char Append = '\n');
	void ProcessTraffic(const CString& Name, const CString& Traffic, char Append = '\n');
	void ProcessLogEntry(const CString& Name, const CString& Entry, char Append = '\n');
	void StoreMessage(CMessage*, CBaseConnection*);

public:
	//void PublishStatus(long TradeId, const CString& Error, const CCsv& CSV);
	void PublishStatus(long TradeId, const CString& Error, const TRADE* pTrade);
	void SendFailureReport(CString& Msg);

	
public:
	CRecorderApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecorderApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRecorderApp)
	afx_msg void OnAppAbout();
	afx_msg void OnGotDictionary(WPARAM wParam, LPARAM lParam);
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBRECORDER_H__E2E8B3C6_3B63_11D2_8D1C_00104B6DEAA9__INCLUDED_)
