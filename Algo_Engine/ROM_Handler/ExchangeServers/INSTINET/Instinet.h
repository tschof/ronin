// Instinet.h : main header file for the INSTINET application
//

#if !defined(AFX_INSTINET_H__E0817956_68FB_11D2_8D35_00104B6DEAA9__INCLUDED_)
#define AFX_INSTINET_H__E0817956_68FB_11D2_8D35_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "Connection.h"
#include "ClientConnection.h"
#include "RecorderConnection.h"
#include "Server.h"
#include "InstinetDoc.h"
#include "InstinetView.h"
#include "TrafficDoc.h"
#include "TrafficView.h"
#include "MessageApp.h"
#include "InstinetHandler.h"
#include "InstinetStatus.h"

/////////////////////////////////////////////////////////////////////////////
// CInstinetApp:
// See Instinet.cpp for the implementation of this class
//

class CInstinetApp : public CMessageApp
{
public:
	CInstinetApp();

private:
	CInstinetView	*m_pClientView;
	CInstinetView	*m_pExchangeView;
	CTrafficView	*m_pTrafficView;
	CTrafficView	*m_pStatusView;
	CTrafficView	*m_pErrorView;
	bool			m_bDisplayTraffic;

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
	BOOL ProcessClientCancels(CBaseConnection& pC, CCsv& CSV);
	BOOL ProcessClientReplace(CBaseConnection& pC, CCsv& CSV);
	BOOL ProcessClientStatus(CBaseConnection& pC, CCsv& CSV);

	void ProcessTimer();

	void ProcessLogEntry(const CString& Name, const CString& Status, char Append = '\n');
	void ProcessErrorEx(DWORD Mode,const CString& Name, const CString& Error);
	void ProcessDebug(const CString& Name, const CString& Debug, char Append = '\n');

	// host/exchange functions
	CBaseConnection* CreateHost() { return(m_pHost = new CHostConnection(m_Name, m_Name, m_Path, NULL)); }
	//CConnection* CreateRecorder() { return(m_pRecorder	= new CRecorderConnection("Recorder", m_Name, m_Path));	}

	//CConnection	*Recorder()	{ return m_pRecorder; }

	void RecycleHost()	
	{
		DeleteHost(m_pHost);
		AddHost((CConnection*)CreateHost());
	}
	//void RecycleRecorder()	
	//{
	//	DeleteHost(m_pRecorder);
	//	AddHost((CHostConnection*)CreateRecorder());
	//}
		
	void AddHost(CConnection *pConnection);
	void DeleteHost(CConnection *pConnection);
	void UpdateHost();	
	void UpdateClients();
	void SetStatusTimer(UINT Interval);
	
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInstinetApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CInstinetApp)
	afx_msg void OnAppAbout();
	afx_msg void OnLogout();
	afx_msg void OnResetOutgoing();
	afx_msg void OnResetIncoming();
	afx_msg void OnClearOutgoing();
	afx_msg void OnClearIncoming();
	afx_msg void OnTaggleDisplay() { m_bDisplayTraffic = !m_bDisplayTraffic;}
	afx_msg void OnUpdateTaggleDisplay(CCmdUI* pCmdUI) {pCmdUI->SetCheck(m_bDisplayTraffic);}
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSTINET_H__E0817956_68FB_11D2_8D35_00104B6DEAA9__INCLUDED_)
