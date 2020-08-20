// X25ClientConnection.cpp: implementation of the CX25ClientConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "X25ClientConnection.h"
#include "MessageApp.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CX25ClientConnection::CX25ClientConnection()
{

}

CX25ClientConnection::~CX25ClientConnection()
{

}


void CX25ClientConnection::ProcessMessage(CString& WholeString)
{
	CMessageApp	*pApp		= (CMessageApp *)AfxGetApp();
	int			Location	= -1;
	CString		String;
//	CLogin		Login;
//	CLogout		Logout;
	CString		SourceId;

	while ((Location = WholeString.Find('\n')) >= 0)
	{
		String = "";
		if (Location == 0)
			WholeString = WholeString.Mid(1);
		else
		{
			String = WholeString.Left(Location);
			CMessage *pMessage = new CMessage(CMessage::Socket, String);

			switch(pMessage->Type())
			{
			case (CMessage::Logon):
				//m_Name = pMessage->Login().Trader();
				//SetLogonStatus(TRUE);
				
				//pApp->Clients().SetClient(this, m_Name);

				//Login.SetTrader(m_Name);
				//SendMessage(Login.Message());

				break;

			case(CMessage::Logout):
				//Logout.SetTrader(m_Name);
				//SendMessage(Logout.Message());

				//Close();
				//SetLogonStatus(FALSE);

				//pApp->Clients().Update();

				break;

			case (CMessage::Order):

				COrder *pOrder;
				pOrder = new COrder(pMessage->Message());
				pOrder->SetOwner(m_Name);
				SourceId.Format("%s,%s", m_Name, pOrder->Trader());
				pOrder->SetSourceId(SourceId);
				pApp->Handler()->ProcessMessage(*pOrder);
				delete pOrder;
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

void CX25ClientConnection::Close(/*int nErrorCode*/)
{
	SetLogonStatus(FALSE);
	//CX25Connection::Close(nErrorCode);
	((CMessageApp *)AfxGetApp())->Clients().Delete(this);
}

//void CX25ClientConnection::Connect(int nErrorCode)
//{
//	CX25Connection::Connect(nErrorCode);
//}

void CX25ClientConnection::ProcessStatus(const CString& Message)
{
	((CMessageApp *)AfxGetApp())->ProcessStatus(m_Name, Message);
}

void CX25ClientConnection::ProcessError(const CString& Error)
{
	((CMessageApp *)AfxGetApp())->ProcessError(m_Name, Error);
}

void CX25ClientConnection::ProcessLogEntry(const CString& Entry)
{
	((CMessageApp *)AfxGetApp())->ProcessLogEntry(m_Name, Entry);
}
