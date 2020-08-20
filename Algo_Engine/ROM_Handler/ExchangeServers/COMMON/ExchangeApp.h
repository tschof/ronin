// ExchangeApp.h : main header file for the exchange application
//

#if !defined(AFX_ExchangeApp_H__E0817956_68FB_11D2_8D35_00104B6DEAA9__INCLUDED_)
#define AFX_ExchangeApp_H__E0817956_68FB_11D2_8D35_00104B6DEAA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "Connection.h"
#include "ClientConnection.h"
#include "RecorderConnection.h"
#include "Server.h"
#include "MessageApp.h"
//#include "ExchangeHandler.h"
//#include "ExchangeStatus.h"

/////////////////////////////////////////////////////////////////////////////
// CExchangeApp:
// See Exchange.cpp for the implementation of this class
//
class CTrafficView;
class CExchangeView: public CFormView
{
public:
	
	CExchangeView(UINT nIDTemplate) : CFormView(nIDTemplate)
	{
		m_TimerId = 0;
	}

	void CreateTimer(UINT Interval);

protected:
	virtual void OnDestroy();
	virtual void OnTimer(UINT nIDEvent);
	virtual void OnSize(UINT nType, int cx, int cy);

public:
	CConnectionList	m_List;
	int				m_TimerId;

};

class CExchangeApp : public CMessageApp
{
public:
	CExchangeApp()
	{
		m_bDisplayTraffic = false;
	}
	virtual ~CExchangeApp() {;}

protected:
	CExchangeView	*m_pExchangeView;
	CTrafficView	*m_pTrafficView;
	CTrafficView	*m_pStatusView;
	CTrafficView	*m_pErrorView;
	bool			m_bDisplayTraffic;
	CString			m_strPCName;

public:
	virtual void OnHostConnect(CBaseConnection &pC, int nErrorCode);
	virtual void ProcessHostMessage(CBaseConnection &pC, CString& String)
	{
		pC.SetLeftOver((Exchange().ProcessMessage(String, &pC)));
	}

	virtual BOOL ProcessClientLogon(CBaseConnection &pC, CCsv& CSV);
	virtual BOOL ProcessClientLogout(CBaseConnection &pC, CCsv& CSV);
	virtual BOOL ProcessClientOrder(CBaseConnection& pC, CCsv& CSV);
	virtual BOOL ProcessClientCancel(CBaseConnection& pC, CCsv& CSV);
	virtual BOOL ProcessClientReplace(CBaseConnection& pC, CCsv& CSV);
	virtual BOOL ProcessClientStatus(CBaseConnection& pC, CCsv& CSV);
	virtual BOOL ProcessClientSpreadOrder(CBaseConnection& pC, CString& String);
	
	virtual void ProcessLogEntry(const CString& Name, const CString& Status, char Append = '\n');
	virtual void ProcessErrorEx(DWORD Mode,const CString& Name, const CString& Error);
	virtual void ProcessDebug(const CString& Name, const CString& Debug, char Append = '\n');

	// host/exchange functions
	virtual CBaseConnection* CreateHost() { return(m_pHost = new CHostConnection(m_Name, m_Name, m_Path, NULL)); }
	//CConnection* CreateRecorder() { return(m_pRecorder	= new CRecorderConnection("Recorder", m_Name, m_Path));	}

	//CConnection	*Recorder()	{ return m_pRecorder; }

	virtual void RecycleHost()	
	{
		DeleteHost(m_pHost);
		AddHost((CConnection*)CreateHost());
	}
		
	virtual void AddHost(CConnection *pConnection);
	virtual void DeleteHost(CConnection *pConnection);
	virtual void UpdateHost();	
	virtual void SetStatusTimer(UINT Interval);
	
	afx_msg void OnLogout();
	afx_msg void OnResetOutgoing();
	afx_msg void OnResetIncoming();
	afx_msg void OnClearOutgoing();
	afx_msg void OnClearIncoming();
	afx_msg void OnTaggleDisplay() { m_bDisplayTraffic = !m_bDisplayTraffic;}
	afx_msg void OnUpdateTaggleDisplay(CCmdUI* pCmdUI) {pCmdUI->SetCheck(m_bDisplayTraffic);}

	const CString& GetPCName()	const { return m_strPCName;}
protected:
	virtual void SetPCName(); //This functions is used to initialize m_strPCName to the first client in order to send back GTC orders

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_Exchange_H__E0817956_68FB_11D2_8D35_00104B6DEAA9__INCLUDED_)
