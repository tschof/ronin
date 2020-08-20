// X25HostConnection.cpp: implementation of the CX25HostConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "X25HostConnection.h"
#include "MessageApp.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CX25HostConnection::CX25HostConnection(const CString& Name, const CString& AppName, const CString& Path, unsigned char Card, WORD SendChannel, WORD ReceiveChannel)
	: CX25Connection(Name, AppName, Path, Card, SendChannel, ReceiveChannel)
{
	m_LastReception		= CTime::GetCurrentTime();
	m_LastTransmission	= m_LastReception;

	//SetLogonStatus(FALSE);
	m_Reconnect = TRUE;

	ProcessStatus("Attempting To Make Connection...");
	Disconnect();
	ConnectToHost();
	if(!Connected())
		ProcessStatus("Connection Failed... Retrying...");
}

CX25HostConnection::~CX25HostConnection()
{

}

/*BEGIN_MESSAGE_MAP(CX25HostConnection, CWinThread)
	ON_THREAD_MESSAGE(X25_STARTLISTENING, Listen)
END_MESSAGE_MAP()*/

void CX25HostConnection::ProcessError(const CString& Error)
{
	((CMessageApp *)AfxGetApp())->ProcessError(m_Name, Error);
}

void CX25HostConnection::ProcessLogEntry(const CString& Entry)
{
	((CMessageApp *)AfxGetApp())->ProcessLogEntry(m_Name, Entry);
}

void CX25HostConnection::ProcessStatus(const CString& Entry)
{
	((CMessageApp *)AfxGetApp())->ProcessStatus(m_Name, Entry);
}

void CX25HostConnection::ProcessMessage(CString& Message)
{
	// save the time
		m_LastReception = CTime::GetCurrentTime();

	// bust incoming message
		SetLeftOver(((CMessageApp *)AfxGetApp())->Exchange()->ProcessMessage(Message));
}

BOOL CX25HostConnection::SendMessage(CString Message)
{
	BOOL r_Value = FALSE;

	TRACE("SEQ NUM: %d\n", CActTrailer::LastSent());
	
	for(int i = 0; i < 5; i++)
	{
		if ((r_Value = CX25Connection::SendMessage(Message)))
		{
			m_LastTransmission = CTime::GetCurrentTime();
			return TRUE;
		}

		Sleep(100);
	}

	m_SendQueue.AddTail((CObject*)new CString(Message));
	
	return(FALSE);
}

BOOL CX25HostConnection::InitInstance()
{
	BOOL rValue = CWinThread::InitInstance();
//	PostThreadMessage(X25_STARTLISTENING, NULL, NULL);
	return TRUE;
}

BOOL CX25HostConnection::ExitInstance()
{
	return CX25Connection::ExitInstance();
}

BOOL CX25HostConnection::OnIdle( LONG lCount )
{
	int rValue;
	char data[MDATALEN];
   	CMessageApp* pApp = (CMessageApp*)AfxGetApp();
	
	if(Connected())
	{
		if(rValue = !Recieve(data))
		{
			pApp->Exchange()->ProcessMessage((CString)data);
			ProcessLogEntry("Received: " + CString(data));
		}
		else
		{
			rValue = CheckLink();
			if((rValue == 0x02))
			{
				ProcessStatus("Connection Lost... Attempting To Reconnect");
				ConnectToHost();
			}
			if((rValue == 0x08))
			{
				//AfxMessageBox("Fatal Error, Modem Failure!");
				Disconnect();
				ProcessStatus("Modem Failure... Attempting To Reconnect");
				ConnectToHost();
			}
		}
		/*if(int queueSize = m_SendQueue.GetCount())
		{
			CString *pStr;
			for(int i = 0; i < queueSize; i++)
			{
				pStr = (CString*)m_SendQueue.RemoveHead();
				SendMessage(*pStr);
				delete pStr;
			}
		}*/
	}
	else
		ConnectToHost(FALSE);

	return 1;
}

/*void CX25HostConnection::Listen(WPARAM wParam, LPARAM lParam)
{
	MSG msg;
	
	BOOL bLOOP = TRUE;

	/*while(bLOOP)
	{
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{
				bLOOP = FALSE;
				break;
			}
			if (!AfxGetApp()->PreTranslateMessage(&msg))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
		
	}
	PostThreadMessage(msg.message, msg.wParam, msg.lParam);
}*/
