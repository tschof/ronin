// ArcaDirect.h : main header file for the ArcaDirect application
//

#if !defined(AFX_ArcaDirect_H__CB666CF6_26ED_11D2_8D11_006008CD4894__INCLUDED_)
#define AFX_ArcaDirect_H__CB666CF6_26ED_11D2_8D11_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include <sstream>
#include "resource.h"       // main symbols
#include "Connection.h"
#include "ClientConnection.h"
#include "DirectConnection.h"
#include "Server.h"
#include "ArcaDirectDoc.h"
#include "ArcaDirectView.h"
#include "TrafficDoc.h"
#include "TrafficView.h"
#include "MessageApp.h"
#include "ArcaDirectHandler.h"
#include "ArcaDirectSession.h"
#include "ArcaDirectStatus.h"

/////////////////////////////////////////////////////////////////////////////
// CArcaDirectApp:
// See ArcaDirect.cpp for the implementation of this class
//

class CArcaDirectApp : public CMessageApp
{
public:
    CArcaDirectApp();

private:
    CArcaDirectView		*m_pClientView;
    CArcaDirectView		*m_pExchangeView;
    CTrafficView	*m_pTrafficView;
    CTrafficView	*m_pStatusView;
    CTrafficView	*m_pErrorView;
    DirectConnection* m_pDirect;

public:
    void OnHostConnect(CBaseConnection &pC, int nErrorCode);
    void ProcessHostMessage(CBaseConnection &pC, CString& String) {
        pC.SetLeftOver((Exchange().ProcessMessage(String, &pC)));//DEBUG!!!
    }

    int ProcessHostMessage(CBaseConnection &dC, std::stringstream& message, int length) {
        return ((ArcaDirectSession&)Exchange()).ProcessMessage(message, length, &dC);
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
    CBaseConnection* CreateHost() { return(m_pDirect = new DirectConnection(m_Name, m_Name, m_Path)); }

    DirectConnection* GetDirect() { return m_pDirect; }
    void RecycleHost()	{
        DeleteHost(m_pDirect);
        CreateHost();
        AddHost(m_pDirect);
    }
		
    void AddHost(CBaseConnection *pConnection);
    void DeleteHost(DirectConnection *pConnection);
    //void DeleteHost(DirectConnection *pConnection);
    void UpdateHost();	
    void UpdateClients();
    void SetStatusTimer(UINT Interval);

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CArcaDirectApp)
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    //}}AFX_VIRTUAL

// Implementation

    //{{AFX_MSG(CArcaDirectApp)
    afx_msg void OnAppAbout();
    afx_msg void OnLogout();
    afx_msg void OnResetOutgoing();
    afx_msg void OnResetIncoming();
    afx_msg void OnClearOutgoing();
    afx_msg void OnClearIncoming();
	afx_msg void OnResetOrderId();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
        };


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ArcaDirect_H__CB666CF6_26ED_11D2_8D11_006008CD4894__INCLUDED_)
