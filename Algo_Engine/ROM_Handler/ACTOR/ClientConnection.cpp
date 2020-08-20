// ClientConnection.cpp : implementation file
//

#include "stdafx.h"
#include "MessageApp.h"
#include "Act.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientConnection


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CClientConnection, CAsyncSocket)
	//{{AFX_MSG_MAP(CClientConnection)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CClientConnection member functions

void CClientConnection::ProcessMessage(CString& WholeString)
{
	CMessageApp		*pApp			= (CMessageApp *)AfxGetApp();
	int				Location		= -1;
	CString			String;
	CLogin			Login;
	CLogout			Logout;
	CString			SourceId;
	CCsv*			Msg;
	CString refnum, staftag;

	
	while ((Location = WholeString.Find('\n')) >= 0)
		{
		String = "";
		if (Location == 0)
			WholeString = WholeString.Mid(1);
		else
			{
			String = WholeString.Left(Location);
			CMessage	*pMessage	= new CMessage(CMessage::Socket, String);
			Msg = &pMessage->Message();

			switch (pMessage->Type())
				{
				case CMessage::Logon:
					// grab traders acronym
					m_Name = pMessage->Login().Trader();

					SetLogonStatus(TRUE);

					// associate trader with client connection
					pApp->Clients().SetClient(this, m_Name);

					// send login reply back to client
					Login.SetTrader(m_Name);
					SendMessage(Login.Message());

					break;

				case CMessage::Logout:
					// send logout reply
					Logout.SetTrader(m_Name);
					SendMessage(Logout.Message());

					// close
					Close();

					SetLogonStatus(FALSE);

					pApp->Clients().Update();

					break;

				case CMessage::Order:

					refnum = Msg->Get(CSV_ACT_TAG);
					refnum = CAct::Validate(ACT_REFERENCENUMBER, refnum);

					pApp->MapConnection(refnum, Msg->Get(CSV_ACT_ORDER_TAG), m_Name);
					pApp->Exchange()->ProcessEntry(Msg);
					break;

				default:
					break;
				}
		
			WholeString = WholeString.Mid(Location+1);
			m_Received++;
			delete pMessage;
			}
		}

	if (WholeString != "")
		SetLeftOver(WholeString);
}

void CClientConnection::OnClose(int nErrorCode) 
{
	SetLogonStatus(FALSE);
	CConnection::OnClose(nErrorCode);
	((CMessageApp *)AfxGetApp())->Clients().Delete(this);
}

void CClientConnection::OnConnect(int nErrorCode) 
{
	CConnection::OnConnect(nErrorCode);
}

void CClientConnection::ProcessStatus(const CString& Message)
{
	((CMessageApp *)AfxGetApp())->ProcessStatus(m_Name, Message);
}

void	CClientConnection::ProcessError(const CString& Error)
{
	((CMessageApp *)AfxGetApp())->ProcessError(m_Name, Error);
}

void	CClientConnection::ProcessLogEntry(const CString& Entry)
{
	((CMessageApp *)AfxGetApp())->ProcessLogEntry(m_Name, Entry);
}
