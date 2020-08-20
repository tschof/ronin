//**************************************************************************
// 
//	File:
//	
//	iLinkGlobex.h
//
//  Summary:
//
//	History
//
//	Name						Reason								Date
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************

#if !defined(AFX_ILINKGLOBEX_H__10010249_CED9_4CFD_92FF_0DC4A97A0A96__INCLUDED_)
#define AFX_ILINKGLOBEX_H__10010249_CED9_4CFD_92FF_0DC4A97A0A96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "Resource.h"
#include "MainFrm.h"
#include "MessageApp.h"
#include "TrafficDoc.h"
#include "TrafficView.h"
#include "ILinkGlobexDoc.h"
#include "ILinkGlobexView.h"
#include "OrderHandler.h"
#include "ZLog.h"

//**************************************************************************
// 
//	Class:
//	CILinkGlobexApp
//
//	Purpose:
//
//
//
//
//	Base Class:
//
//	History
//
//	
//****************************************************************************
class CILinkGlobexApp : public CMessageApp
{
public:
	CILinkGlobexApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CILinkGlobexApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CILinkGlobexApp)
	afx_msg void OnAppAbout();
	afx_msg void OnLogout();
	afx_msg void OnResetOutgoing();
	afx_msg void OnResetIncoming();
	afx_msg void OnClearOutgoing();
	afx_msg void OnClearIncoming();
	afx_msg void OnToggleDisplay();
	afx_msg void OnUpdateToggleDisplay(CCmdUI* pCmdUI);
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:

///////////////////////////////////////////////////////////////
//
//	Host interface methods (Better known as the fix connection)
//
///////////////////////////////////////////////////////////////

	virtual CString& GetTitleText()
	{ 
		return m_AppSpecificName;
	}

	void ProcessHostMessage(CBaseConnection &pC, CString& String);

	// Host connection maintenance
	CBaseConnection* CreateHost();
	void AddHost(CConnection *pConnection);						// Common
	void DeleteHost(CConnection *pConnection);					// Probably Common
	void OnHostConnect(CBaseConnection &pC, int nErrorCode);	// Common
	void RecycleHost();											// Probably Common
	void UpdateHost();											// Probably Common

// Processing Methods

	// CME specific
	void Audit(const CCsv& csv, char *source, char *direction,
        bool bTime, const CString& orderTag);
    CILinkGlobexOrderHandler& GetHandler() {
        return ((CILinkGlobexOrderHandler&)Handler());
    }

	// Error View
	void ProcessErrorEx(DWORD Mode,const CString& Name, const CString& Error);

	// Status View
	void ProcessLogEntry(const CString& Name, const CString& Status, char Append = '\n');

	// Order View
	void ProcessDebug(const CString& Name, const CString& Debug, char Append = '\n');

	// Not exactly sure what this piece is for
	void SetStatusTimer(UINT Interval);

// Client Message Processing
	BOOL ProcessClientLogon	(CBaseConnection &pC, CCsv& CSV);
	BOOL ProcessClientLogout(CBaseConnection &pC, CCsv& CSV);
	BOOL ProcessClientOrder(CBaseConnection& pC, CCsv& CSV);
	BOOL ProcessClientCancel(CBaseConnection& pC, CCsv& CSV);
	BOOL ProcessClientReplace(CBaseConnection& pC, CCsv& CSV);
	BOOL ProcessClientStatus(CBaseConnection& pC, CCsv& CSV);

	bool ForceRecycle();

private:

	// Private Data
	CILinkGlobexView*	m_pClientView;
	CILinkGlobexView*	m_pExchangeView;
	CTrafficView*		m_pOrderView;
	CTrafficView*		m_pStatusView;
	CTrafficView*		m_pErrorView;

	CMultiDocTemplate*	m_pILinkGlobexDocTemplate;
	CMultiDocTemplate*	m_pTrafficDocTemplate;

	bool				m_bDisplayTraffic;

	ZLog				*m_auditTrail;

	// Failover members
	bool				m_usePrimaryConnection;
	bool				m_useFailOver;
	int					m_retryAttempts;
	int					m_maxConnectionAttempts;

	// Sequence Number members
	bool				m_ForceRecycle;

	static int			m_messageNumber;

	// Private Methods
	void CreateDocumentTemplates();
	BOOL CreateMainFrame(CMainFrame** ppMainFrame);
	void InitializeNameAndPath(void);
	void InitializeDATFilePath(void);
	void InitializeLogFilePath(void);
	void InitializeFailoverParameters(void);
	void InitializeSequenceNumbers(void);

	void CreateDocumentWindow(CMultiDocTemplate* pTrafficDocTemplate, CString& title, CFormView** ppTrafficView);
	void CreateTrafficWindows();
	void CreateExchangeWindows();
	void CreateClientWindows();

	void InitializeAuditTrail(void);
    void InitalizeFirmNameForAudit();
	void InitializeOrderManager(void);
	void InitializeOrderStatus(void);
	void InitializeOrderHandler(void);
	void InitializeFixSession(void);

	CString ParseProduct(const CString&);
	CString ParseDate(const CString&);
	CString m_AppSpecificName;
    CString m_AuditFirm;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ILINKGLOBEX_H__10010249_CED9_4CFD_92FF_0DC4A97A0A96__INCLUDED_)
