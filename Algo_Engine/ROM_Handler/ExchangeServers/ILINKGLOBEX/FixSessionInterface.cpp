//**************************************************************************
// 
//	File:
//		FixSessionInterface.cpp
//
//  Summary:
//  CILinkGlobexApp::InitializeFixSession
//  CILinkGlobexApp::AddHost
//  CILinkGlobexApp::CreateHost
//  CILinkGlobexApp::DeleteHost
//  CILinkGlobexApp::OnHostConnect
//  CILinkGlobexApp::ProcessHostMessage
//  CILinkGlobexApp::RecycleHost
//  CILinkGlobexApp::UpdateHost
//
//	History
//
//	Name							Reason				Date
//	Randy Fredrick				Created				20-NOV-02
//****************************************************************************

#include "stdafx.h"
#include "iLinkGlobex.h"
#include "GlobexFixSession.h"

//**************************************************************************
// 
//	Method:
//		CILinkGlobexApp::InitializeFixSession
//
//	Parameters:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//****************************************************************************
void CILinkGlobexApp::InitializeFixSession(void)
{
	CMessageApp::m_pExchange = new CGlobexFixSession();
	AddHost((CHostConnection*) CreateHost());
}

//**************************************************************************
// 
//	Method:
//		CILinkGlobexApp::AddHost
//
//	Parameters:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//****************************************************************************
void CILinkGlobexApp::AddHost(CConnection *pConnection)
{
	if (pConnection)
	{
		m_pExchangeView->m_List.Add(pConnection);
	}
}

//**************************************************************************
// 
//	Method:
//		CILinkGlobexApp::CreateHost
//
//	Parameters:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//****************************************************************************
CBaseConnection* CILinkGlobexApp::CreateHost()
{
	CString connectionName = m_Name;

	if (m_usePrimaryConnection)
	{
		connectionName += "-P";

		return(m_pHost = new CHostConnection(connectionName, m_Name, m_Path, NULL)); 
	}
	else
	{
		connectionName += "-S";

		return(m_pHost = new CHostConnection(connectionName, m_Name, m_Path, NULL));
	}
}

//**************************************************************************
// 
//	Method:
//		CILinkGlobexApp::DeleteHost
//
//	Parameters:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//****************************************************************************
void CILinkGlobexApp::DeleteHost(CConnection *pConnection)
{
	if (pConnection)
	{
		if (pConnection->m_hSocket != INVALID_SOCKET)
		{
			pConnection->AsyncSelect(0);
			pConnection->Close();
		}
	}
	else
	{
		return;
	}

	// Check failover scenario
	if (true == m_useFailOver)
	{
		m_retryAttempts++;

		if (m_retryAttempts == m_maxConnectionAttempts)
		{
			CString connectionStatus = "Switching connection";

			// toggle the 
			m_usePrimaryConnection = (m_usePrimaryConnection == true) ? false:true;
			m_retryAttempts = 0;

			// Just post the event. Leave the error view out of it.
			CMessageApp::ProcessErrorEx(M_ERR_NOTIFY_EL_WARNING, pConnection->Name(), connectionStatus);
		}
	}

	m_pExchangeView->m_List.Delete(pConnection);
	delete pConnection;
	pConnection = NULL;
}

//**************************************************************************
// 
//	Method:
// 
//		CILinkGlobexApp::OnHostConnect
//
//	Parameters:
//
//		CBaseConnection &pC
//		int nErrorCode
//
//	Synopsis:
//
//		Called when the CASyncSocket notifies the Connection object that a connection
//		to the socket has been made.
//
//	Derived From:
//
//		CMessageApp
//
//	Side Effects:
//
//		Invalidates the CListBox on the Exchange View
//
//****************************************************************************
void CILinkGlobexApp::OnHostConnect(CBaseConnection &pC, int nErrorCode)
{
	if (nErrorCode)
	{
		return;
	}

	UpdateHost();

	if (Exchange().StartLogon())
	{
		if (!(Exchange().Logon()))
		{
			ProcessErrorEx(MODE_ERR_NOTIFY,pC.Name(), "Error Logging On");
		}

		m_ForceRecycle = false;
	}
}

//**************************************************************************
// 
//	Method:
//		CILinkGlobexApp::ProcessHostMessage
//
//	Parameters:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//****************************************************************************
void CILinkGlobexApp::ProcessHostMessage(CBaseConnection &pC, CString& String)
{
	pC.SetLeftOver((Exchange().ProcessMessage(String, &pC)));
}

//**************************************************************************
// 
//	Method:
//		CILinkGlobexApp::RecycleHost
//
//	Parameters:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//****************************************************************************

void CILinkGlobexApp::RecycleHost()	
{
	DeleteHost(m_pHost);
	AddHost((CConnection*)CreateHost());
}

//**************************************************************************
// 
//	Method:
//		CILinkGlobexApp::UpdateHost
//
//	Parameters:
//
//
//	Synopsis:
//
//
//	Derived From:
//
//
//	Side Effects:
//
//
//****************************************************************************
void CILinkGlobexApp::UpdateHost()
{
	if (m_pExchangeView)
	{
		m_pExchangeView->m_List.Update();
	}
}