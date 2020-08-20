// ActSession.cpp: implementation of the CActSession class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MessageApp.h"
#include "Actor.h"
#include "Act.h"
#include "ActFields.h"
#include "ActSession.h"
#include "Msg.h"

//#include "TerraHandler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#pragma warning(disable:4786)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CActSession::~CActSession()
{
	CMsg* pMsg;
	
	while (m_ActMsgList.IsEmpty())
	{
		pMsg = (CMsg*)(m_ActMsgList.RemoveHead());
		if (pMsg != NULL)
		{
			delete pMsg;
		}
	}
}

CActSession::CActSession()
{
	// Get version from dat file;

	// Get Act login
	
	char LoginID[11];
	char Version[3];

	m_HeartbtInt = 8;

	CTerraNovaApp *pApp = (CTerraNovaApp *)AfxGetApp();
	GetPrivateProfileString("Session Information",	"Logon ID", "", LoginID, 11, pApp->Path() + pApp->Name() + ".DAT");
	GetPrivateProfileString("Session Information",	"Version", "", Version, 3, pApp->Path() + pApp->Name() + ".DAT");

	CMsg::Init(Version, LoginID);
	COleDateTime time = COleDateTime::GetCurrentTime();
		
	char *buffer = new char[10];
	
	_itoa( time.GetMonth(), buffer, 10 );
	m_Date += buffer;
	_itoa( time.GetDay(), buffer, 10 );
	m_Date += buffer;
	_itoa( time.GetYear(), buffer, 10 );
	m_Date += buffer;

	char Date[10];
	GetPrivateProfileString("Session Information", "Date", "", Date, 10, "c:\\OrderManager\\Act.dat");
	//CString Date = pApp->GetProfileString("Session", "Date", m_Date);
	if(m_Date != Date)
	{
		CActTrailer::LastSent(0);
		WritePrivateProfileString("Session Information", "Date", (LPCTSTR)m_Date, "c:\\OrderManager\\Act.dat");
		WritePrivateProfileString("Session Information", "Sequence", "0", "c:\\OrderManager\\Act.dat");
	}
	else
	{
		char SeqString[10];
		GetPrivateProfileString("Session Information", "Sequence", "0", SeqString, 10, "c:\\OrderManager\\Act.dat");
		CActTrailer::LastSent(atoi(SeqString));
	}
	
	delete buffer;
}

void CActSession::ProcessTimer()
{
	SendActMsgs();

	COleDateTime time = COleDateTime::GetCurrentTime();
	CString Date;

	CTimeSpan TimeDiff = CTime::GetCurrentTime() - m_LastSendTime;

	CTerraNovaApp	*pApp = (CTerraNovaApp *)AfxGetApp();	

	CTime CurrentTime = CTime::GetCurrentTime();
	int StartTime, EndTime;

	StartTime = GetPrivateProfileInt("Runtime Information",	"Start Time",6, pApp->Path() + pApp->Name() + ".DAT");
	EndTime = GetPrivateProfileInt("Runtime Information",	"End Time", 6, pApp->Path() + pApp->Name() + ".DAT");

	if(StartTime == EndTime)
	{
		if((CurrentTime.GetHour() == StartTime))
		{
			((CMessageApp*) AfxGetApp())->SetCloseDisplayMsg(FALSE);
			AfxGetMainWnd()->PostMessage(WM_CLOSE);
			return;
		}
	}
	else if(StartTime > EndTime)
	{
		if ((CurrentTime.GetHour() >= EndTime) && (CurrentTime.GetHour() < StartTime))
		{
			((CMessageApp*) AfxGetApp())->SetCloseDisplayMsg(FALSE);
			AfxGetMainWnd()->PostMessage(WM_CLOSE);
			return;
		}
	}
	else
	{
		if ((CurrentTime.GetHour() < StartTime) || (CurrentTime.GetHour() >= EndTime))
		{
			((CMessageApp*) AfxGetApp())->SetCloseDisplayMsg(FALSE);
			AfxGetMainWnd()->PostMessage(WM_CLOSE);
			return;
		}
	}

	char *buffer = new char[10];
	
	_itoa( time.GetMonth(), buffer, 10 );
	Date += buffer;
	_itoa( time.GetDay(), buffer, 10 );
	Date += buffer;
	_itoa( time.GetYear(), buffer, 10 );
	Date += buffer;

	delete buffer;

	if(Date != m_Date)
	{
		CTerraNovaApp *pApp = (CTerraNovaApp *)AfxGetApp();
		//pApp->WriteProfileString("Session", "Date", Date );
		//pApp->OpenLogFile();
		m_Date = Date;
		CActTrailer::LastSent(0);
	}

	if (pApp->Host().LoggedOn() == TRUE &&
		TimeDiff.GetTotalSeconds() >= m_HeartbtInt)
	{
		if (SendHeartBeat() == TRUE)
		{
			m_LastSendTime = CTime::GetCurrentTime();
			pApp->ProcessTraffic(pApp->Name(), "Heartbeat Sent");
		}
	}

	ConnectionCheck();
}

void CActSession::ConnectionCheck()
{
	CTerraNovaApp* pApp = (CTerraNovaApp*)AfxGetApp();

	if(pApp->Host().m_Connected != TRUE)	
	{
 		pApp->ProcessErrorEx(MODE_ERR_NOTIFY, pApp->Name(), "Connection To Host Lost");
		pApp->RecycleHost();	
		//m_LogonInitiated = false;
	}
	else if(pApp->Host().LoggedOn() != TRUE) 
	{
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY, pApp->Name(), "Connected To Host But Not Logged On");
		if (SendLogin() == TRUE)
		{
			m_LastSendTime = CTime::GetCurrentTime();
			pApp->ProcessTraffic(pApp->Name(), "Login Sent");
		}
	}

	pApp->SetStatusTimer(5000);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// incoming event processing

/*void CActSession::DissembleMessage(const char *Msg, int Size, char *Cmd, char *Body)
{
	memcpy(Body, Msg + 16, Size - 18);
	Body[Size - 18] = '\0';
	memcpy(Cmd, Msg + 13, 3);
	Cmd[3] = '\0';
}*/


void CActSession::ProcessMessage(const char *Msg, int Size)
{
	CTerraNovaApp* pApp = (CTerraNovaApp*)AfxGetApp();
	CString Text;

	switch(CMsg::GetType(Msg))
	{
	case CMsg::LOGONRESPONSE:
		ProcessLogonResponse();
		break;
	case CMsg::HEARTBEAT:
		//probably do nothing
		pApp->ProcessTraffic(pApp->Name(), "Heartbeat Received");
		break;
	case CMsg::HEARTBEATRESPONSE:
		//probably do nothing
		pApp->ProcessTraffic(pApp->Name(), "Heartbeat Response Received");
		break;
	case CMsg::FLOWCONTROL:
		//ProcessFlowControl(Msg);
		CMsg::FlowControlMsg(Msg, Text);
		pApp->ProcessTraffic(pApp->Name(), "Flow Control Received:" + Text);
		break;
	case CMsg::CHANNELSTATE:
		ProcessChannelStateQuery(Msg);
		break;
	case CMsg::ACT:
		ProcessActResponse(Msg, Size);
		break;
	default:
		break;
	}
}


void CActSession::ProcessLogonResponse()
{
	//AddIncomingMsgToSequence(a_Logon);
	
	CTerraNovaApp	*pApp = (CTerraNovaApp *)AfxGetApp();	

	//pApp->ProcessLogEntry("ACT Session", "Logon acknowledgement received.");
	pApp->ProcessTraffic("ACT Session", "Logon acknowledgement received.");
	//pApp->SetStatusTimer(2000);

	//if (m_InitiateLogon)
	//{
	pApp->Host().SetLogonStatus(TRUE);
	pApp->UpdateHost();
	//}
}


/*void CActSession::ProcessHeartBeat()
{
	SendHeartBeat();
}*/


void CActSession::ProcessHeartBeatResponse()
{
}

//void CActSession::ProcessTimer()
//{
//}

/*void CActSession::ProcessFlowControl(const char *Msg)
{
	BYTE channel;
	
	memcpy(&channel, Msg, 1);
	memcpy(&(m_ActChannelStatus[channel]), Msg + 1, 1);
}*/


void CActSession::ProcessChannelStateQuery(const char *Msg)
{
	BYTE Channel = CMsg::GetChannel(Msg);

	if (SendChannelState(Channel) == TRUE)
	{
		m_LastSendTime = CTime::GetCurrentTime();
	}
}


/*void CActSession::ProcessChannelStateResponse(const char *Msg)
{
	//BYTE channel = CMSG::GetChannel(Msg);
	//CMSG Msg(CMSG::
	memcpy(&channel, Msg, 1);
	memcpy(&m_ActChannelStatus[channel], Msg + 1, 1);
}*/



/*void CActSession::AssembleMessage(char * MsgBuffer, char *Body, BYTE Channel, u_short BodySize)
{
	u_short MsgSize = 15 + BodySize;
	char TimeStamp[9];
	char *p = MsgBuffer;
	char Tail[3] ;
	strcpy(Tail, "UU");

	CString t = (CTime::GetCurrentTime()).Format("%H:%M:%S") + "00";
	strcpy(TimeStamp, (LPCTSTR)t);
	
	MsgSize = htons(MsgSize);
	memcpy(p, &MsgSize, 2); p += 2;
	memcpy(p, &m_version, 2); p += 2;
	memcpy(p, TimeStamp, 8); p += 8;
	memcpy(p, &Channel, 1); p += 1;
	memcpy(p, Body, BodySize); p += BodySize;
	memcpy(p, Tail, 2);
}*/


BOOL CActSession::SendLogin()
{
	CTerraNovaApp	*pApp = (CTerraNovaApp *)AfxGetApp();
	CMsg Logon(CMsg::LOGON);

	return (pApp->Host()).SendMessage(Logon.Message(), Logon.Size());
}


BOOL CActSession::SendHeartBeat()
{
	CTerraNovaApp	*pApp = (CTerraNovaApp *)AfxGetApp();
	CMsg HeartBeat(CMsg::HEARTBEAT);

	return (pApp->Host()).SendMessage(HeartBeat.Message(), HeartBeat.Size());
}


BOOL CActSession::SendAct(const char *Act)
{

	CMsg *pMsg = new CMsg(Act);

	m_ActMsgList.AddTail(pMsg);
	return SendActMsgs();

	// Save the CMsg object to the end of the list
	// Going through the list to send the message
	// Make sure FIFS
	//CTerraNovaApp	*pApp = (CTerraNovaApp *)AfxGetApp();
	//rc = (pApp->Host()).SendMessage(pMsg->Message(), pMsg->Size());

	//if (rc == TRUE)
	//{
	//	delete pMsg;
	//}
	//else 
	//{
		// Save the CMsg object
	//}
	
	//return rc;
}


BOOL CActSession::SendActMsgs()
{
	CTerraNovaApp	*pApp = (CTerraNovaApp *)AfxGetApp();
	BOOL rc = FALSE;
	CString sActBody;

	while ( (pApp->Host()).Connected() &&
			(pApp->Host()).LoggedOn() &&
			m_ActMsgList.IsEmpty() != TRUE)
	{
		if (CMsg::GetActiveOutChannel() != 0)
		{
			CMsg* pMsg = (CMsg*)(m_ActMsgList.GetHead());
			
			pMsg->GetActBody(&sActBody);

			pApp->ProcessTraffic(pApp->Name(), "Sending: " +  sActBody);

			rc = (pApp->Host()).SendMessage(pMsg->Message(), pMsg->Size());
			if (rc == TRUE)
			{
				pApp->ProcessTraffic(pApp->Name(), "Sent: " +  sActBody);

				m_ActMsgList.RemoveHead();
				delete pMsg;
			}
		}
		else
		{
			break;
		}
	}

	return rc;
}

BOOL CActSession::SendChannelState(BYTE Channel)
{
	CMsg Msg(CMsg::CHANNELSTATERESPONSE, Channel);
	CTerraNovaApp	*pApp = (CTerraNovaApp *)AfxGetApp();

	return (pApp->Host()).SendMessage(Msg.Message(), Msg.Size());
}



void CActSession::ProcessActResponse(const char* Msg, u_short Size)
{
	CTerraNovaApp	*pApp = (CTerraNovaApp *)AfxGetApp();	
	CCsv csv;
	CString Message;
	char ActBody[1024];
	
	CMsg::GetAct(ActBody, Msg, Size);
	Message = ActBody;

	pApp->ProcessTraffic(pApp->Name(), "Received: " +  Message);
	if(CAct::ValidAct(Message))
	{
		CAct Act(Message);
		CString tag = Act.GetAt(ACT_REFERENCENUMBER);

		TrimLeadingZeros(tag);

		CString name = pApp->GetName(tag);
		Act.Reference(&csv);
		csv.SetAt(CSV_ACT_ORDER_TAG, pApp->GetOrderTag(tag));
		csv.SetAt(CSV_ACT_EXECUTION_TAG, pApp->GetExecTag(tag));
		csv.SetAt(CSV_ACT_COMMAND, CSV_ACT_COMMAND_STATUS);
		csv.SetAt(CSV_ACT_TIME, Act.GetAt(ACT_EXECUTIONTIME));
		csv.SetAt(CSV_ACT_REFERENCE_NUM, Act.GetControlNum());
		
		if(Act.GetAt(ACT_BSX) == "B")
			csv.SetAt(CSV_ACT_SIDE, "1");
		else
			csv.SetAt(CSV_ACT_SIDE, "2");

		switch(Act.GetType())
		{
		case ACT_T_TCEN:
			csv.SetAt(CSV_ACT_TYPE, CSV_ACT_TCEN_TYPE);
			pApp->ProcessTraffic(pApp->Name(), "MSG RECV TCEN: " + Act.Debug());
			break;
		case ACT_T_TTEN:
			csv.SetAt(CSV_ACT_TYPE, CSV_ACT_TTEN_TYPE);
			pApp->ProcessTraffic(pApp->Name(), "MSG RECV TTEN: " + Act.Debug());
			break;
		case ACT_T_CTEN:
			csv.SetAt(CSV_ACT_TYPE, CSV_ACT_TCEN_TYPE);
			pApp->ProcessTraffic(pApp->Name(), "MSG RECV CTEN: " + Act.Debug());
			break;
		case ACT_T_TCAL:
			csv.SetAt(CSV_ACT_TYPE, CSV_ACT_TCAL_TYPE);
			pApp->ProcessTraffic(pApp->Name(), "MSG RECV TCAL: " + Act.Debug());
			break;
		case ACT_T_TTAL:
			csv.SetAt(CSV_ACT_TYPE, CSV_ACT_TTAL_TYPE);
			pApp->ProcessTraffic(pApp->Name(), "MSG RECV TTAL: " + Act.Debug());
			break;
		case ACT_T_CTAL:
			csv.SetAt(CSV_ACT_TYPE, CSV_ACT_CTAL_TYPE);
			pApp->ProcessTraffic(pApp->Name(), "MSG RECV CTAL: " + Act.Debug());
			break;
		case ACT_T_TCLK:
			csv.SetAt(CSV_ACT_TYPE, CSV_ACT_TCLK_TYPE);
			pApp->ProcessTraffic(pApp->Name(), "MSG RECV TCLK: See Log");
			pApp->RemoveTag(tag);
			break;
		case ACT_T_REJ:
			csv.SetAt(CSV_ACT_TYPE, CSV_ACT_REJ_TYPE);
			pApp->ProcessTraffic(pApp->Name(), "MSG RECV REJ: " + Act.Debug());
			pApp->RemoveTag(tag);
			break;
		default:
			//ProcessEntry(&csv);
			break;
		};

		pApp->SendToClient(name, csv.Message());
	}
	//return(Message);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// order related message processing

long CActSession::ProcessActEntry(CCsv *csv)
{
	long rValue = 0;
	CTerraNovaApp	*pApp = (CTerraNovaApp *)AfxGetApp();	
	
	CAct Act(csv);
	Act.SetAt(ACT_OEID, "INCA");

	Act.Increment();
	char sSeqNumber[8];
	CString sSeqN;
	_itoa(CActTrailer::LastSent(), sSeqNumber, 10);
	sSeqN = sSeqNumber;

	Act.SetAt(CSV_ACT_TAG, sSeqN);
	CString msg = Act.Send();

	//pApp->ProcessTraffic(pApp->Name(), "Sending: " +  msg);

	pApp->MapConnection(sSeqN, csv->Get(CSV_ACT_ORDER_TAG), csv->Get(CSV_ACT_EXECUTION_TAG), csv->Get(CSV_OWNER));

	if(rValue = SendAct((LPCTSTR)msg))
	{
		//pApp->ProcessTraffic(pApp->Name(), "Sent: " +  msg);
		m_LastSendTime = CTime::GetCurrentTime();
	}
	else
	{
		/*Act.Decrement();
		CString tag = Act.GetAt(ACT_REFERENCENUMBER);
		CString name = pApp->GetName(tag);

		csv->SetAt(CSV_ACT_TYPE, CSV_ACT_REJ_TYPE);
		csv->SetAt(CSV_ACT_REJECT_REASON, "SEND FAILED");
		pApp->RemoveTag(tag);
		pApp->SendToClient(name, csv->Message());*/
	}

	//char SeqString[10];
	//_itoa(CActTrailer::LastSent(), SeqString, 10);
	//WritePrivateProfileString("Session Information", "Sequence", SeqString, "c:\\OrderManager\\Act.dat");

	//AfxGetApp()->WriteProfileInt( "Session", "Seq", CActTrailer::LastSent());
	return rValue;
}


void CActSession::TrimLeadingZeros(CString &s)
{
	int l = s.GetLength();
	for (int i = 0; i < l; i++)
	{
		if (s.GetAt(i) != '0')
		{
			break;
		}
	}

	s = s.Right(l - i);
}
