// IslandSession.cpp: implementation of the IslandSession class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Island.h"
#include "IslandSession.h"

#include "IslandOrder.h"
#include "IslandCancel.h"
#include "IslandRewind.h"
#include "FromIslandExecution.h"
#include "FromIslandReject.h"
#include "FromIslandCancelReject.h"

#include "HostConnection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIslandSession::~CIslandSession()
{
}

CIslandSession::CIslandSession()
: m_sequenceSet()
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();
	CString	FileName = pApp->Path() + pApp->Name() + ".DAT";

	// read values from exchange's DAT file
	m_InitiateLogon		= GetPrivateProfileInt("Fix Information",	"Initiate Logon",	1,		FileName);
	GetPrivateProfileString("Island Information", "Account", "", m_Account.GetBuffer(128), 128, FileName);	m_Account.ReleaseBuffer();
	GetPrivateProfileString("Island Information", "Password", "", m_Password.GetBuffer(128), 128, FileName); m_Password.ReleaseBuffer();

	m_IncomingHeartBeatInterval = 15;

	// initialize
	CIslandHeader::Init();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// outgoing event processing

BOOL CIslandSession::Logon()
{
	CIslandLogin Login(m_Account, m_Password);

	return(SendMessage(Login));
}

BOOL CIslandSession::Logout()
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	pApp->Host().SetReconnect(FALSE);
	CIslandLogout Logout;

	return(SendMessage(Logout));
}


long CIslandSession::ProcessEntry(COrder& Order)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();
	CString	Tag;
	
	int seq = NextOutgoingSeqNum();
	// set the order's tag based off of the assigned sequence number and date
	Tag.Format("%s%ld", CTime::GetCurrentTime().Format("%m%d"), seq);
	Order.SetOrderTag(Tag);

	// generate fix message
	CIslandOrder IslandOrder(Order);

	if (SendMessage(IslandOrder))
	{
		SetNextOutgoingSeqNum(seq + 1);
		return(seq);
	}
	else
	{
		return(FALSE);
	}
}

BOOL CIslandSession::ProcessCancel(CCancel& Cancel)
{
	CIslandCancel IslandCancel(Cancel);

	if (SendMessage(IslandCancel))
		return(TRUE);

	return(FALSE);
}

BOOL CIslandSession::ProcessReplace(CReplace& Order)
{
	return(FALSE);
}

BOOL CIslandSession::ProcessStatusRequest(CRequest& Order)
{
	return(FALSE);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// incoming event processing

CString CIslandSession::ProcessMessage(CString Message, CBaseConnection *pC)
{
	CString			CurrentMessage;
	CMessageApp		*pApp = (CMessageApp *)AfxGetApp();
	BOOL			done = FALSE;
	int				Index;
	char			Type;

	while (!done)
	{
		if ((Index = Message.Find((char)10)) > 0)
		{
			CurrentMessage = Message.Left(Index-2);
			Message = Message.Mid(Index + 1);
			CIslandHeader Header(CurrentMessage);

			switch (Header.Type())
			{
				case CIslandHeader::Login:	
					ProcessLogon();
					break;
				case CIslandHeader::Goodbye:
					ProcessLogout(Header);
					break;
				case CIslandHeader::Heartbeat:
					ProcessHeartBeat(Header, CurrentMessage);
					break;
				case CIslandHeader::Sequenced:
					if (ProcessSequence(Header.Sequence(), NextIncomingSeqNum()))
					{
						Type = CurrentMessage[16];

						switch (Type)
						{
							case CIslandMessage::Text:				
								ProcessText(CurrentMessage);
								break;
							case CIslandMessage::Status:
								ProcessExecutionReport(Header, CurrentMessage);
								break;
							case CIslandMessage::Event:
								ProcessEvent(CurrentMessage);
								break;
							default:
								break;
						};
					}
					break;
				case CIslandHeader::Reject:
					Type = CurrentMessage[1];

					switch (Type)
					{
						case CIslandMessage::Reject:				// order reject
							ProcessReject(CurrentMessage);
							break;
						case CIslandMessage::CancelReject:		// cancel reject
							ProcessCancelReject(CurrentMessage);
							break;
						default:
							break;
					};
					break;
				default:
					break;
			};
		}
		else
			done = TRUE;
	}
	
	return(Message);		// return leftover
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// order related SEQUENCED message processing


void CIslandSession::ProcessExecutionReport(const CIslandHeader& Header, const CString& Message)
{
	CMessageApp	*pApp = (CMessageApp *)AfxGetApp();			
	CFromIslandExecution Execution(Message);
	pApp->Handler().ProcessMessage(Execution);
}

void CIslandSession::ProcessText(const CString& Message)
{
	CMessageApp	*pApp = (CMessageApp *)AfxGetApp();	
	pApp->ProcessErrorEx(MODE_ERR_BASIC,pApp->Name(), Message.Mid(17, 80));
}

void CIslandSession::ProcessEvent(const CString& Message)
{
	CMessageApp	*pApp = (CMessageApp *)AfxGetApp();	

	char Type = Message[16];

	switch (Type)
	{
		case 'S':
			pApp->ProcessLogEntry(pApp->Name(), "Island Start Of Day");
			break;
		case 'E':
			pApp->ProcessLogEntry(pApp->Name(), "Island End Of Day");
			break;
		default:
			break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// order relact NON-SEQUENCED message processing

void CIslandSession::ProcessCancelReject(const CString& Message)
{
	CMessageApp	*pApp = (CMessageApp *)AfxGetApp();	
	CFromIslandCancelReject	Reject(Message);
	pApp->Handler().ProcessMessage(Reject);
}

void CIslandSession::ProcessReject(const CString& Message)
{
	CMessageApp	*pApp = (CMessageApp *)AfxGetApp();	
	CFromIslandReject Reject(Message);
	pApp->Handler().ProcessMessage(Reject);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// admin message processing

void CIslandSession::ProcessHeartBeat(const CIslandHeader& Header, const CString& Message)
{
	int Received = atoi(Message.Mid(1 , 10));

	CIslandHeartbeat Beat;
	SendMessage(Beat);

	ProcessSequence(Received, NextIncomingSeqNum());
}

void CIslandSession::ProcessLogout(const CIslandHeader& Header)
{
	CMessageApp	*pApp = (CMessageApp *)AfxGetApp();	

	pApp->ProcessErrorEx(MODE_ERR_NOTIFY,pApp->Name(), "Logged Out Of Island. Reason: "+ Header.Text());
	
	pApp->Host().SetLogonStatus(FALSE);
	pApp->Host().Close();
	pApp->UpdateHost();

	pApp->SetStatusTimer(15000);
}

void CIslandSession::ProcessLogon()
{
	CMessageApp	*pApp = (CMessageApp *)AfxGetApp();	

	pApp->ProcessLogEntry(pApp->Name(), "Welcome To Island");

	pApp->SetStatusTimer(2000);

	if (m_InitiateLogon)
	{
		pApp->Host().SetLogonStatus(TRUE);
		pApp->UpdateHost();
	}
}
			
BOOL CIslandSession::ProcessSequence(int Received, int Expected)
{
	if (Received > Expected)
	{
		// create resend message
		CIslandRewind	Rewind(Expected);
		SendMessage(Rewind);
		return(FALSE);
	}
	else
	{
		SetNextIncomingSeqNum(Received + 1);
		return(TRUE);
	}

	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// utilities

BOOL CIslandSession::SendMessage(CIslandMessage& Island)
{
	CMessageApp	*pApp = (CMessageApp *)AfxGetApp();	

	return(pApp->Host().SendMessage(Island.Message()+(char)13+(char)10));
}

void CIslandSession::ProcessTimer()
{
	CTimeSpan	TimeDiff;
	CTime		CurrentTime = CTime::GetCurrentTime();
	CMessageApp	*pApp = (CMessageApp *)AfxGetApp();
	CHostConnection *pHost = (CHostConnection *)&pApp->Host();

	int StartTime, EndTime;

	StartTime = GetPrivateProfileInt("Runtime Information",	"Start Time",6, pApp->Path() + pApp->Name() + ".DAT");
	EndTime = GetPrivateProfileInt("Runtime Information", "End Time", 6, pApp->Path() + pApp->Name() + ".DAT");

	int HM = 100 * CurrentTime.GetHour() + CurrentTime.GetMinute();

	if(StartTime == EndTime)
	{
		if(HM == StartTime)
		{
			pApp->SetCloseDisplayMsg(FALSE);
			CIslandLogout Logout;
			SendMessage(Logout);
			Sleep(1);
			pApp->Host().Close();
			AfxGetMainWnd()->PostMessage(WM_CLOSE);
			return;
		}
	}
	else if(StartTime > EndTime)
	{
		if ((HM >= EndTime) && (HM < StartTime))
		{
			pApp->SetCloseDisplayMsg(FALSE);
			CIslandLogout Logout;
			SendMessage(Logout);
			Sleep(1);
			pApp->Host().Close();
			AfxGetMainWnd()->PostMessage(WM_CLOSE);
			return;
		}
	}
	else
	{
		if ((HM < StartTime) || (HM >= EndTime))
		{
			pApp->SetCloseDisplayMsg(FALSE);
			CIslandLogout Logout;
			SendMessage(Logout);
			Sleep(1);
			pApp->Host().Close();
			AfxGetMainWnd()->PostMessage(WM_CLOSE);
			return;
		}
	}
	
	if ((pHost->Reconnect()) && (!(pHost->m_Connected)))	// not connected
	{
		pApp->SetStatusTimer(15000);
 		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,pApp->Name(), "Connection To Host Lost");
		pApp->Handler().ProcessMessage("DEAD");

		pApp->RecycleHost();				
	}
	else if ((pHost->Reconnect()) && (!(pHost->LoggedOn())))	// not logged on
	{
		pApp->SetStatusTimer(2000);
		pApp->Handler().ProcessMessage("DEAD");
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,pApp->Name(), "Connected To Host But Not Logged On");
		CIslandLogin Login(m_Account, m_Password);
		SendMessage(Login);
	}
	else	// connection is good so just check for heartbeats and test requests
	{
	}
}

int CIslandSession::GetNextIncomingSeqNum()
{
    return m_sequenceSet.GetExpected();
}


int CIslandSession::GetNextOutgoingSeqNum()
{
    return m_sequenceSet.GetNext();
}


void CIslandSession::SetNextIncomingSeqNum(int seq)
{
	m_sequenceSet.SetExpected(seq);
}

void CIslandSession::SetNextOutgoingSeqNum(int seq)
{
	m_sequenceSet.SetNext(seq);
}
