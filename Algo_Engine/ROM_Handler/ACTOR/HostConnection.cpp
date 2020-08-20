// HostConnection.cpp: implementation of the CHostConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MessageApp.h"

#pragma warning(disable:4786)

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHostConnection::CHostConnection(const CString& Name, const CString& AppName, const CString& Path) :
	CConnection(Name, AppName, Path)
{
	m_LastReception		= CTime::GetCurrentTime();
	m_LastTransmission	= m_LastReception;

	SetLogonStatus(FALSE);
	m_Reconnect = TRUE;

	ConnectToHost();
}

CHostConnection::~CHostConnection()
{
}

void	CHostConnection::ProcessError(const CString& Error)
{
	((CMessageApp *)AfxGetApp())->ProcessError(m_Name, Error);
}
void	CHostConnection::ProcessLogEntry(const CString& Entry)
{
	((CMessageApp *)AfxGetApp())->ProcessLogEntry(m_Name, Entry);
}
void  CHostConnection::ProcessStatus(const CString& Entry)
{
	((CMessageApp *)AfxGetApp())->ProcessStatus(m_Name, Entry);
}

void CHostConnection::ProcessMessage(CString& Message)
{
	// save the time
		m_LastReception = CTime::GetCurrentTime();

	// bust incoming message
		SetLeftOver(((CMessageApp *)AfxGetApp())->Exchange()->ProcessMessage(Message));
}

void CHostConnection::OnConnect(int nErrorCode)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	CConnection::OnConnect(nErrorCode);

	if (!(nErrorCode))
		{

		SetLogonStatus(TRUE);
		pApp->UpdateHost();

		if (pApp->Exchange()->StartLogon())
			{
			if (!(pApp->Exchange()->Logon())) 
				ProcessError("Error Logging On");
			}
		}
	else
		PostError(nErrorCode);
}

void CHostConnection::OnClose(int nErrorCode)
{
	m_Connected =	FALSE;
	SetLogonStatus(FALSE); 

	if (nErrorCode)
		PostError(nErrorCode);

	CConnection::OnClose(nErrorCode);

}

BOOL CHostConnection::SendMessage(CString Message)
{
	int Bytes = 0;
	
	if ((Bytes = CConnection::SendMessage(Message)))
		m_LastTransmission = CTime::GetCurrentTime();
	
return(Bytes);
}

