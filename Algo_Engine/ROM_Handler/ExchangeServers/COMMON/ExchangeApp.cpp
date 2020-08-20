// ExchangeApp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ExchangeApp.h"
#include "resource.h"
#include "TrafficView.h"
#include "SeqDlg.h"
#include "Exchangesession.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void CExchangeView::CreateTimer(UINT Interval)
{
	if (m_TimerId >= 0)
		KillTimer(m_TimerId);

	m_TimerId = SetTimer(1, Interval, NULL);
}

void CExchangeView::OnDestroy() 
{
	if (m_TimerId >= 0)
	{
		KillTimer(m_TimerId);
		m_TimerId = -1;
	}

	CFormView::OnDestroy();
}

void CExchangeView::OnTimer(UINT nIDEvent) 
{
	if (m_TimerId >= 0)
	{
		CExchangeApp *pApp = (CExchangeApp *)AfxGetApp();
		pApp->Exchange().ProcessTimer();
		pApp->ProcessTimer();
		pApp->UpdateHost();
		pApp->UpdateClients();
	}
	
	CFormView::OnTimer(nIDEvent);
}

void CExchangeView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);

	CRect ListRect;
	
	CFormView::OnSize(nType, cx, cy);
	
	if (m_List.m_hWnd > 0)
	{	
		m_List.GetWindowRect(&ListRect);
		ScreenToClient(&ListRect);
		ListRect.bottom = cy-3;
		ListRect.right = cx-3;
		m_List.MoveWindow(ListRect, TRUE);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CExchangeApp

/////////////////////////////////////////////////////////////////////////////
// CExchangeApp construction

BOOL CExchangeApp::ProcessClientOrder(CBaseConnection& pC, CCsv& CSV)
{
	CString SourceId;

	COrder *pOrder;
	pOrder = new COrder(CSV);
/*	pOrder->SetOwner(pC.m_Name);
	SourceId.Format("%s,%s", pC.m_Name, pOrder->Trader());
	pOrder->SetSourceId(SourceId);*/
	pOrder->SetSourceId(pOrder->Trader());//Client Tag
	Handler().ProcessMessage(*pOrder);
	delete pOrder;

	return TRUE;
}

BOOL CExchangeApp::ProcessClientCancel(CBaseConnection& pC, CCsv& CSV)
{
	CString SourceId;

	CCancel *pCancel;
	pCancel = new CCancel(CSV);
/*	pCancel->SetOwner(pC.m_Name);
	SourceId.Format("%s,%s", pC.m_Name, pCancel->Trader());
	pCancel->SetSourceId(SourceId);*/
	pCancel->SetSourceId(pCancel->Trader());
	Handler().ProcessMessage(*pCancel);
	delete pCancel;

	return TRUE;
}

BOOL CExchangeApp::ProcessClientReplace(CBaseConnection& pC, CCsv& CSV)
{
	CString SourceId;

	CReplace *pReplace;
	pReplace = new CReplace(CSV);
/*	pReplace->SetOwner(pC.m_Name);
	SourceId.Format("%s,%s", pC.m_Name, pReplace->Trader());
	pReplace->SetSourceId(SourceId);*/
	pReplace->SetSourceId(pReplace->Trader());
	Handler().ProcessMessage(*pReplace);
	delete pReplace;

	return TRUE;
}

BOOL CExchangeApp::ProcessClientStatus(CBaseConnection& pC, CCsv& CSV)
{
	CString SourceId;

	CRequest *pRequest;
	pRequest = new CRequest(CSV);
/*	pRequest->SetOwner(pC.m_Name);
	SourceId.Format("%s,%s", pC.m_Name, pRequest->Trader());
	pRequest->SetSourceId(SourceId);*/
	pRequest->SetSourceId(pRequest->Trader());
	Handler().ProcessMessage(*pRequest);
	delete pRequest;

	return TRUE;
}

void CExchangeApp::OnHostConnect(CBaseConnection &pC, int nErrorCode)
{
	if (!(nErrorCode))
	{
		//pC.SetLogonStatus(TRUE);
		UpdateHost();

		if (Exchange().StartLogon())
		{
			if (!(Exchange().Logon())) 
				ProcessErrorEx(MODE_ERR_NOTIFY,pC.Name(), "Error Logging On");
		}
	}
	else
	{}
		//pC.PostError(nErrorCode);
}

BOOL CExchangeApp::ProcessClientSpreadOrder(CBaseConnection& pC, CString& String)
{
	Handler().ProcessMessage(String);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CExchangeApp commands

void CExchangeApp::AddHost(CConnection *pHost)
{
	if (pHost)
		m_pExchangeView->m_List.Add(pHost);
}

void CExchangeApp::DeleteHost(CConnection *pHost)
{
	if (pHost)
	{
		if (pHost->m_hSocket != INVALID_SOCKET)
		{
			pHost->AsyncSelect(0);
			pHost->Close();
		}

		m_pExchangeView->m_List.Delete(pHost);
		delete pHost;
		pHost = NULL;
	}
}

void CExchangeApp::UpdateHost()
{
	if (m_pExchangeView)
		m_pExchangeView->m_List.Update();
}

void CExchangeApp::ProcessLogEntry(const CString& Name, const CString& Message, char Append)
{
	CMessageApp::ProcessLogEntry(Name, Message, Append);
	if (m_pStatusView) m_pStatusView->Add(Name + '\t' + Message);
}

void CExchangeApp::ProcessErrorEx(DWORD Mode, const CString& Name, const CString& Error)
{
	CMessageApp::ProcessErrorEx(Mode, Name, Error);
	
	if (m_pErrorView) 
	{
		m_pErrorView->Add(Name + '\t' + Error);
	}
}

void CExchangeApp::ProcessDebug(const CString& Name, const CString& Debug, char Append)
{
	if(m_bDisplayTraffic)
	{
		CMessageApp::ProcessDebug(Name, Debug, Append);

		if (m_pTrafficView) 
		{
			m_pTrafficView->Add(Name + '\t' + Debug);
		}
	}
}

void CExchangeApp::SetStatusTimer(UINT Interval)
{
	m_pExchangeView->CreateTimer(Interval);
}


void CExchangeApp::OnLogout() 
{
	Exchange().Logout();
}

void CExchangeApp::OnResetOutgoing()
{
	CSeqDlg dlg;
	if(dlg.DoModal() == IDOK)
		Exchange().SetNextOutgoingSeqNum(dlg.SeqNum());
}

void CExchangeApp::OnResetIncoming()
{
	CSeqDlg dlg;
	if(dlg.DoModal() == IDOK)
		Exchange().SetNextIncomingSeqNum(dlg.SeqNum());
}

void CExchangeApp::OnClearOutgoing()
{
	Exchange().SetNextOutgoingSeqNum(1);
}

void CExchangeApp::OnClearIncoming()
{
	Exchange().SetNextIncomingSeqNum(1);
}
	
BOOL CExchangeApp::ProcessClientLogout(CBaseConnection &pC, CCsv& CSV)
{
	CLogout Logout;

	Logout.SetTrader(pC.m_Name);
 	pC.SendMessage(Logout.Message());

	pC.SetLogonStatus(FALSE);
	pC.Close();
	Clients().Update();

	return TRUE;
}

BOOL CExchangeApp::ProcessClientLogon(CBaseConnection &pC, CCsv& CSV)
{
	CLogin Login;

	pC.m_Name = CSV.Get(CSV_TRADER);

	//if the connect is already there, kick it out
	CBaseConnection* pExist = Clients().FindClient((LPCSTR) (pC.m_Name));
	if(pExist)
	{
		CString msg = pExist->GetConnector() + " replaced by " + pC.GetConnector();
		ProcessLogEntry(pC.m_Name,msg);
		ProcessClientLogout(*pExist, CSV);
	}

	pC.SetLogonStatus(TRUE);

	CString IP;
	
	Clients().SetClient(&pC, pC.m_Name, IP);

	Login.SetTrader(m_Name);
	pC.SendMessage(Login.Message());
	SetPCName();
	Clients().Dump(pC.Name());

	return TRUE;
}

void  CExchangeApp::SetPCName()
{
	POSITION pos;
	CClient	*pCurClient;

	pos = m_Clients.GetHeadPosition();
	if(pos)		
	{
		if (pCurClient = (CClient *)m_Clients.GetAt(pos))
		{
			m_strPCName = pCurClient->Client();
		}
	}
}