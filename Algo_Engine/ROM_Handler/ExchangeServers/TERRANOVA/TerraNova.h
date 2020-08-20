// TerraNova.h : main header file for the TERRANOVA application
//

#if !defined(AFX_TERRANOVA_H__CB666CF6_26ED_11D2_8D11_006008CD4894__INCLUDED_)
#define AFX_TERRANOVA_H__CB666CF6_26ED_11D2_8D11_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "stdafx.h"

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "Connection.h"
#include "HostConnection.h"
#include "ClientConnection.h"
#include "Server.h"
#include "TerraNovaDoc.h"
#include "TerraNovaView.h"
#include "TrafficDoc.h"
#include "TrafficView.h"
#include "MessageApp.h"
#include "TerraHandler.h"
#include "TerraStatus.h"
#include "ErrorHandler.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

/////////////////////////////////////////////////////////////////////////////
// CArcaApp:
// See TerraNova.cpp for the implementation of this class
//

class CArcaApp : public CMessageApp
{
public:
	CArcaApp();
	~CArcaApp();

private:
	CArcaView		*m_pClientView;
	CArcaView		*m_pExchangeView;
	CTrafficView	*m_pTrafficView;
	CTrafficView	*m_pStatusView;
	CTrafficView	*m_pErrorView;
	bool			m_bDisplayTraffic;
	CErrorHandler   m_ErrorHandler;

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

	void RecycleHost()	
	{
		DeleteHost(m_pHost);
		AddHost((CConnection*)CreateHost());
		((CArcaHandler*)m_pHandler)->UpdateHost(m_pHost);
	}

    CArcaHandler* GetHandler() {
        return ((CArcaHandler*)m_pHandler);
    }
		
	void AddHost(CConnection *pConnection);
	void DeleteHost(CConnection *pConnection);
	void UpdateHost();	
	void UpdateClients();
	void SetStatusTimer(UINT Interval);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArcaApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CArcaApp)
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

#endif // !defined(AFX_TERRANOVA_H__CB666CF6_26ED_11D2_8D11_006008CD4894__INCLUDED_)
