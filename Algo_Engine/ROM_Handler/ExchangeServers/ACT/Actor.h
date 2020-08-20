// TerraNova.h : main header file for the TERRANOVA application
//

#if !defined(AFX_TERRANOVA_H__CB666CF6_26ED_11D2_8D11_006008CD4894__INCLUDED_)
#define AFX_TERRANOVA_H__CB666CF6_26ED_11D2_8D11_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "Connection.h"
#include "ClientConnection.h"
#include "Server.h"
#include "TerraNovaDoc.h"
#include "TerraNovaView.h"
//#include "TerraNovaView2.h"
#include "TrafficDoc.h"
#include "TrafficView.h"
#include "MessageApp.h"
#include "ActSession.h"
#include "ActorConnection.h"
//#include "TerraHandler.h"
//#include "TerraStatus.h"
//#include "X25Socket.h"
#include "ActDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CTerraNovaApp:
// See TerraNova.cpp for the implementation of this class
//

class CTerraNovaApp : public CMessageApp
{
public:
	CTerraNovaApp();

private:
		//CStdioFile				m_Log;

		CTerraNovaView			*m_pClientView;
		CTerraNovaView			*m_pExchangeView;
		CTrafficView			*m_pTrafficView;
		CTrafficView			*m_pStatusView;
		CTrafficView			*m_pErrorView;

		unsigned char			m_Card;
		WORD					m_SendChannel;
		WORD					m_ReceiveChannel;

		CActDialog				*m_ActDialog;

		CMapStringToString		m_MapTagToOrderTag;
		CMapStringToString		m_MapTagToExecTag;
		CMapStringToString		m_MapTagToName;

		CActorConnection *m_pHost;
		CActSession *m_pExchange;
public:
	void	ProcessStatus(const CString& Name, const CString& Message);
	void	ProcessError(const CString& Name, const CString& Error);
	//void	ProcessTraffic(const CString& Name, const CString& Traffic);
	void	ProcessLogEntry(const CString& Name, const CString& Entry, char Append = '\n');	
	void	AddHost(CBaseConnection *pHost);
	void	DeleteHost(CConnection *pHost);
	void	UpdateHost();
	void	SetStatusTimer(UINT Interval);
	//void	OpenLogFile();
	//void ProcessHostMessage(CBaseConnection &pC, CString& String)
	//{
	//	pC.SetLeftOver((Exchange().ProcessMessage(String, &pC)));
	//}



	BOOL ProcessClientLogon(CBaseConnection& pC, CCsv& CSV);
	BOOL ProcessClientLogout(CBaseConnection& pC, CCsv& CSV);
	BOOL ProcessClientOrder(CBaseConnection& pC, CCsv& CSV);
	
	void MapConnection(CString Tag, CString OrderTag, CString ExecTag, CString Name)
	{
		m_MapTagToOrderTag.SetAt(Tag, OrderTag);
		m_MapTagToExecTag.SetAt(Tag, ExecTag);
		m_MapTagToName.SetAt(Tag, Name);
	}
	void RemoveTag(CString &Tag) 
	{ 
		m_MapTagToOrderTag.RemoveKey(Tag);
		m_MapTagToExecTag.RemoveKey(Tag);
		m_MapTagToName.RemoveKey(Tag);		 
	}
	CString	GetOrderTag(CString &Tag)
	{
		CString rValue; 
		m_MapTagToOrderTag.Lookup(Tag, rValue);  
		return rValue; 
	}
	CString GetExecTag(CString &Tag)
	{
		CString r_Value;
		m_MapTagToExecTag.Lookup(Tag, r_Value);
		return r_Value;
	}
	CString GetName(CString &Tag)
	{
		CString rValue; 
		m_MapTagToName.Lookup(Tag, rValue); 
		return rValue; 
	}

	CActorConnection&	Host()	{ return *m_pHost; }
	CActSession& Exchange() { return *m_pExchange ;}

	CBaseConnection* CreateHost() { return(m_pHost = new CActorConnection(m_Name, m_Name, m_Path)); }

	void ProcessActorMessage(const char* Msg, int Size);

	void RecycleHost()	
	{
		DeleteHost(m_pHost);
		AddHost((CConnection*)CreateHost());
	}

	//virtual void ProcessTimer();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTerraNovaApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTerraNovaApp)
	afx_msg void OnAppAbout();
	afx_msg void OnLogout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERRANOVA_H__CB666CF6_26ED_11D2_8D11_006008CD4894__INCLUDED_)
