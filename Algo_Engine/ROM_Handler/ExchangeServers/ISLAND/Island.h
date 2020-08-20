// Island.h : main header file for the ISLAND application
//

#if !defined(AFX_ISLAND_H__AF6E8EA6_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_)
#define AFX_ISLAND_H__AF6E8EA6_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "Connection.h"
#include "ClientConnection.h"
#include "HostConnection.h"
//#include "RecorderConnection.h"
#include "Server.h"
#include "IslandDoc.h"
#include "IslandView.h"
#include "TrafficDoc.h"
#include "TrafficView.h"
#include "MessageApp.h"
#include "IslandSession.h"
#include "IslandHandler.h"
#include "IslandStatus.h"

/////////////////////////////////////////////////////////////////////////////
// CInstinetApp:
// See Instinet.cpp for the implementation of this class
//

class CIslandApp : public CMessageApp
{
public:
	CIslandApp();

private:
	/*
	CStdioFile		m_Log;
	CStdioFile		m_ErrorLog;
	CStdioFile		m_TrafficLog;
	*/
	CIslandView		*m_pClientView;
	CIslandView		*m_pExchangeView;
	CTrafficView	*m_pTrafficView;
	CTrafficView	*m_pStatusView;
	CTrafficView	*m_pErrorView;
	bool			m_bDisplayTraffic;
	CString			m_MMID;

	//CRecorderConnection		*m_pRecorder;

public:
	void OnHostConnect(CBaseConnection &pC, int nErrorCode);
	void ProcessHostMessage(CBaseConnection &pC, CString& String)
	{
		pC.SetLeftOver((Exchange().ProcessMessage(String, &pC)));
	}

	BOOL ProcessClientLogon(CBaseConnection &pC, CCsv& CSV);
	BOOL ProcessClientLogout(CBaseConnection &pC, CCsv& CSV);
	BOOL ProcessClientOrder(CBaseConnection& pC, CCsv& CSV);
	BOOL ProcessClientCancel(CBaseConnection& pC, CCsv& CSV);
	BOOL ProcessClientReplace(CBaseConnection& pC, CCsv& CSV);
	BOOL ProcessClientStatus(CBaseConnection& pC, CCsv& CSV);

	void ProcessLogEntry(const CString& Name, const CString& Status, char Append = '\n');
	void ProcessErrorEx(DWORD Mode,const CString& Name, const CString& Error);
	void ProcessDebug(const CString& Name, const CString& Debug,  char Append = '\n');
	
	void ProcessTimer();

	CBaseConnection* CreateHost() { return(m_pHost = new CHostConnection(m_Name, m_Name, m_Path, NULL)); }
//	CRecorderConnection* CreateRecorder() { return(m_pRecorder = new CRecorderConnection("Recorder", m_Name, m_Path)); }

	void RecycleHost()	
	{
		DeleteHost(m_pHost);
		AddHost((CConnection*)CreateHost());
	}
		
	/*void RecycleRecorder()	
	{
		DeleteHost(m_pRecorder);
		AddHost(CreateRecorder());
	}*/
	
	void AddHost(CConnection *pConnection);
	void DeleteHost(CConnection *pConnection);
	void UpdateHost();
	void UpdateClients();
	void SetStatusTimer(UINT Interval);
	CString MMID() { return m_MMID; }

	//CRecorderConnection	*Recorder()	{ return m_pRecorder; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIslandApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CIslandApp)
	afx_msg void OnAppAbout();
	afx_msg void OnLogout();
	afx_msg void OnResetOutgoing();
	afx_msg void OnResetIncoming();
	afx_msg void OnClearOutgoing();
	afx_msg void OnClearIncoming();
	afx_msg void OnTaggleDisplay() { m_bDisplayTraffic = !m_bDisplayTraffic;}
	afx_msg void OnUpdateTaggleDisplay(CCmdUI* pCmdUI) {pCmdUI->SetCheck(m_bDisplayTraffic);}
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ISLAND_H__AF6E8EA6_8DD8_11D2_8D41_00104B6DEAA9__INCLUDED_)
