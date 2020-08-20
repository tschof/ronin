// Bloomberg.h : main header file for the BLOOMBERG application
//

#if !defined(AFX_BLOOMBERG_H__F46917B5_DFCC_11D3_A600_00500401F87A__INCLUDED_)
#define AFX_BLOOMBERG_H__F46917B5_DFCC_11D3_A600_00500401F87A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "Connection.h"
#include "ClientConnection.h"
#include "HostConnection.h"
#include "Server.h"
#include "MessageApp.h"
#include "FixSession.h"
#include "resource.h"       // main symbols
#include "MessageApp.h"
#include "BaseConnection.h"

class CBloombergView;
class CTrafficView;

/////////////////////////////////////////////////////////////////////////////
// CBloombergApp:
// See Bloomberg.cpp for the implementation of this class
//


class CBloombergApp : public CMessageApp
{
public:
	CBloombergApp();


private:
	CBloombergView	*m_pClientView;
	CBloombergView	*m_pExchangeView;
	CTrafficView	*m_pTrafficView;
	CTrafficView	*m_pStatusView;
	CTrafficView	*m_pErrorView;
	int				m_nReConnection;
	int				m_nMaxReConnection;
	CString			m_SenderSubId;
	bool			m_bDisplayTraffic;

	void SwitchHostIP();

public:
	void OnHostConnect(CBaseConnection &pC, int nErrorCode);
	void ProcessHostMessage(CBaseConnection &pC, CString& String)
	{
		pC.SetLeftOver((Exchange().ProcessMessage(String, &pC)));
	}

	CString	SenderSubId() { return m_SenderSubId; }

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
	//CConnection* CreateRecorder() { return(m_pRecorder	= new CRecorderConnection("Recorder", m_Name, m_Path));	}

	//CConnection	*Recorder()	{ return m_pRecorder; }

	void RecycleHost()	
	{
		DeleteHost(m_pHost);
		AddHost((CConnection*)CreateHost());
	}
	/*void RecycleRecorder()	
	{
		DeleteHost(m_pRecorder);
		AddHost((CHostConnection*)CreateRecorder());
	}*/
		
	void AddHost(CConnection *pConnection);
	void DeleteHost(CConnection *pConnection);
	void UpdateHost();	
	void UpdateClients();
	void SetStatusTimer(UINT Interval);
	void ProcessConnectionCheck();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBloombergApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CBloombergApp)
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
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BLOOMBERG_H__F46917B5_DFCC_11D3_A600_00500401F87A__INCLUDED_)
