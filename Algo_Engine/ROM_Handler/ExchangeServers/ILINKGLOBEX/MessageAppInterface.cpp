//**************************************************************************
// 
//	File:
//	
//  Summary:
//
//	CILinkGlobexApp::CILinkGlobexApp()
//	CILinkGlobexApp::InitInstance()
//	CILinkGlobexApp::CreateDocumentTemplates()
//	CILinkGlobexApp::OnAppAbout()
//
//	History
//
//	Name						Reason								Date
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************

#include "stdafx.h"
#include "iLinkGlobex.h"

//**************************************************************************
// 
//	Method:
//		CILinkGlobexApp::ProcessErrorEx
//
//	Parameters:
//
//	Synopsis:
//
//	Derived From:
//		N/A
//
//	Side Effects:
//		None
//
//	History
//
//	Name							Reason				Date
//	Randy Fredrick				Created				02-DEC-02
//****************************************************************************
void CILinkGlobexApp::ProcessErrorEx(DWORD Mode, const CString& Name,
                                     const CString& Error)
{
	CMessageApp::ProcessErrorEx(Mode, Name, Error);
	
	// Send the Error to the Error window
	if (m_pErrorView) 
	{
		m_pErrorView->Add(Name + '\t' + Error);
	}
}

//**************************************************************************
// 
//	Method:
//		CILinkGlobexApp::ProcessLogEntry
//
//	Parameters:
//
//	Synopsis:
//	Called from CFixSession::Process fix specific message
//	Derived From:
//		N/A
//
//	Side Effects:
//		None
//
//	History
//
//	Name						Reason				Date
//	Randy Fredrick				Created				02-DEC-02
//****************************************************************************
void CILinkGlobexApp::ProcessLogEntry(const CString& Name, const CString& Message,
                                      char Append)
{
	CMessageApp::ProcessLogEntry(Name, Message, Append);
	
	if (m_pStatusView)
	{
		m_pStatusView->Add(Name + '\t' + Message);
	}
}

//**************************************************************************
// 
//	Method:
//		CILinkGlobexApp::ProcessDebug
//
//	Parameters:
//		None
//
//	Synopsis:
//		Cleans up the Globex specific FIX session
//
//	Derived From:
//		N/A
//
//	Side Effects:
//		None
//
//	History
//
//	Name							Reason				Date
//	Randy Fredrick				Created				02-DEC-02
//****************************************************************************
void CILinkGlobexApp::ProcessDebug(const CString& Name, const CString& Debug,
                                   char Append)
{
	if (!m_bDisplayTraffic) 
	{
		return;
	}
	
	CMessageApp::ProcessDebug(Name, Debug, Append);
	
	if( m_pOrderView)
	{
		m_pOrderView->Add(Name + '\t' + Debug);
	}
}

//**************************************************************************
// 
//	Method:
//		CILinkGlobexApp::ProcessLogEntry
//
//	Parameters:
//		None
//
//	Synopsis:
//		Cleans up the Globex specific FIX session
//
//	Derived From:
//		N/A
//
//	Side Effects:
//		None
//
//	History
//
//	Name							Reason				Date
//	Randy Fredrick				Created				02-DEC-02
//****************************************************************************
void CILinkGlobexApp::SetStatusTimer(UINT Interval)
{
	m_pExchangeView->CreateTimer(Interval);
}