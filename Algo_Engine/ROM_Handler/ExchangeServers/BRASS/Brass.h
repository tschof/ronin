// Brass.h : main header file for the Brass application
//

#if !defined(AFX_Brass_H__CB666CF6_26ED_11D2_8D11_006008CD4894__INCLUDED_)
#define AFX_Brass_H__CB666CF6_26ED_11D2_8D11_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "Connection.h"
#include "HostConnection.h"
#include "ClientConnection.h"
#include "Server.h"
#include "BrassDoc.h"
#include "BrassView.h"
#include "TrafficDoc.h"
#include "TrafficView.h"
#include "MessageApp.h"
#include "BrassHandler.h"
#include "BrassStatus.h"

/////////////////////////////////////////////////////////////////////////////
// CBrassApp:
// See Brass.cpp for the implementation of this class
//

class CBrassApp : public CMessageApp
{
public:
	CBrassApp();

private:
	CBrassView		*m_pClientView;
	CBrassView		*m_pExchangeView;
	CTrafficView	*m_pTrafficView;
	CTrafficView	*m_pStatusView;
	CTrafficView	*m_pErrorView;

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
	void ProcessDebug(const CString& Name, const CString& Debug, char Append = '\n');

	// host/exchange functions
	CBaseConnection* CreateHost() { return(m_pHost = new CHostConnection(m_Name, m_Name, m_Path, NULL)); }
	CConnection* CreateRecorder() { return(m_pRecorder	= new CRecorderConnection("Recorder", m_Name, m_Path));	}

	void RecycleHost()	
	{
		DeleteHost(m_pHost);
		AddHost((CBaseConnection*)CreateHost());
		((CBrassHandler*)m_pHandler)->UpdateHost(m_pHost);
	}
	void RecycleRecorder()	
	{
		DeleteHost(m_pRecorder);
		AddHost((CHostConnection*)CreateRecorder());
	}
		
	void AddHost(CBaseConnection *pConnection);
	void DeleteHost(CHostConnection *pConnection);
	void UpdateHost();	
	void UpdateClients();
	void SetStatusTimer(UINT Interval);

	void ProcessTimer();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrassApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CBrassApp)
	afx_msg void OnAppAbout();
	afx_msg void OnLogout();
	afx_msg void OnResetOutgoing();
	afx_msg void OnResetIncoming();
	afx_msg void OnClearOutgoing();
	afx_msg void OnClearIncoming();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_Brass_H__CB666CF6_26ED_11D2_8D11_006008CD4894__INCLUDED_)
