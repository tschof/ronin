// InstinetSession.cpp: implementation of the CInstinetSession class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MessageApp.h"
#include "Fix.h"
#include "FixExecution.h"
#include "FixCancelReject.h"
#include "FixReject.h"
#include "FixResend.h"
#include "FixOrder.h"
#include "FixCancel.h"
#include "FixReset.h"
#include "FixLogon.h"
#include "FixLogout.h"
#include "FixIOI.h"
#include "FixTestRequest.h"
#include "FixHeartbeat.h"
#include "FromFixExecution.h"
#include "FromFixCancelReject.h"
#include "FromFixReject.h"
#include "FromInstinetExecution.h"
#include "FixInstinetLogon.h"
#include "FixInstinetOrder.h"

#include "InstinetSession.h"
#include "InstinetHandler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CString			SenderId;
CString			TargetId;
CString			TargetSubId;
CString			BeginString;
int				HeartBeatInterval;
int				Encryption;
int				InitiateLogon;

#pragma warning(disable:4786)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInstinetSession::~CInstinetSession()
{
	if (m_pSequenceSet)
	{
		m_pSequenceSet->Close();
		delete m_pSequenceSet;
	}

	if (m_pIncomingSet)
	{
		m_pIncomingSet->Close();
		delete m_pIncomingSet;
	}

	if (m_pOutgoingSet)
	{
		m_pOutgoingSet->Close();
		delete m_pOutgoingSet;
	}
}

CInstinetSession::CInstinetSession()
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	// read values from exchange's DAT file
	GetPrivateProfileString("Fix Information",	"SenderId",		"STAF",	 SenderId.GetBuffer(128),		128,	pApp->Path() + pApp->Name() + ".DAT"); SenderId.ReleaseBuffer();
	GetPrivateProfileString("Fix Information",	"TargetId",		"TAL",	 TargetId.GetBuffer(128),		128,	pApp->Path() + pApp->Name() + ".DAT"); TargetId.ReleaseBuffer();
	GetPrivateProfileString("Fix Information",	"TargetSubId",	"TALX",	 TargetSubId.GetBuffer(128),	128,	pApp->Path() + pApp->Name() + ".DAT"); TargetSubId.ReleaseBuffer();
	GetPrivateProfileString("Fix Information",	"BeginString",	"FIX.4.0",BeginString.GetBuffer(128),	128,	pApp->Path() + pApp->Name() + ".DAT"); BeginString.ReleaseBuffer();
	HeartBeatInterval	= GetPrivateProfileInt("Fix Information",	"Heartbeat",		10,		pApp->Path() + pApp->Name() + ".DAT");
	Encryption			= GetPrivateProfileInt("Fix Information",	"Encryption",		0,		pApp->Path() + pApp->Name() + ".DAT");
	InitiateLogon		= GetPrivateProfileInt("Fix Information",	"Initiate Logon",	1,		pApp->Path() + pApp->Name() + ".DAT");
	m_InitiateLogon	= InitiateLogon;

	m_IncomingHeartBeatInterval = 20;
	
	// open datasets
	Connect();
}

BOOL CInstinetSession::Connect()
{
	CString		SQLStatement;
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();
	CString		Date	= CTime::GetCurrentTime().Format("%Y%m%d");
	try
	{
		// open sequence number data set
		pApp->ProcessLogEntry("Fix Session (DB)", "Opening Sequence Set");
		if (m_pSequenceSet = new CFixSequenceSet(&pApp->Database()))
		{
			m_pSequenceSet->m_QueryHost =pApp->Name();
			m_pSequenceSet->m_QueryDate = Date;

			if (m_pSequenceSet->Open(CRecordset::snapshot))
				m_pSequenceSet->CheckForRollover();
			else
				throw "Error Opening SequenceSet";				
		}
		else
			throw "Error Creating SequenceSet";

		// open incoming traffic
		pApp->ProcessLogEntry("Fix Session (DB)", "Opening Incoming Set");
		SQLStatement = "SELECT [MessageType],[SequenceNum],[TargetSubId],[Body],[Host],[Date],[Time] FROM [FixIncomingTraffic] WHERE [Host] = '" +pApp->Name() + "' AND [Date] = '" + Date + "'";
		if (m_pIncomingSet = new CIncomingMessageSet(&pApp->Database(), SQLStatement))
		{
			if (!(m_pIncomingSet->Open(CRecordset::snapshot)))
				throw "Error Opening Incoming Message Set";
		}
		else
			throw "Error Creating Incoming Message Set";

		// open outgoing traffic
		pApp->ProcessLogEntry("Fix Session (DB)", "Opening Outgoing Set");
		SQLStatement = "SELECT [MessageType],[SequenceNum],[TargetSubId],[Body],[Host],[Date],[Time] FROM [FixOutgoingTraffic] WHERE [Host] = '" +pApp->Name() + "' AND [Date] = '" + Date + "'";
		if (m_pOutgoingSet = new COutgoingMessageSet(&pApp->Database(), SQLStatement))
		{
			if (!(m_pOutgoingSet->Open(CRecordset::snapshot)))
				throw "Error Opening Outgoing Message Set";
		}
		else
			throw "Error Creating Outgoing Message Set";
	}

	catch (CDBException *e)
	{
		CString Error;
		Error.Format("EXCEPTION: Error Opening Fix Tables (%s)", e->m_strError);
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Fix Session", Error);
		e->Delete();
	}
	catch (CMemoryException *e)
	{
		CString Error;
		Error.Format("MEMORY EXCEPTION: Error Opening Fix Tables");
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Fix Session", Error);
		e->Delete();
	}
	catch (const char *Message)
	{
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Fix Session", Message);
		return(FALSE);
	}

	return(TRUE);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// outgoing event processing

BOOL CInstinetSession::Logon()
{
	CInstinetLogon	Logon(GetSequenceSet()->GetNext());

	return(SendMessage(Logon));
}

BOOL CInstinetSession::Logout()
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();
		
	pApp->Host().SetReconnect(FALSE);
	CFixLogout Logout(GetSequenceSet()->GetNext(), "USER EXIT");

	return(SendMessage(Logout));
}


long CInstinetSession::ProcessEntry(COrder& Order)
{
	long Sequence = GetSequenceSet()->GetNext();
	
	// set the order's tag based off of the assigned sequence number and date
	Order.SetOrderTag(Sequence);

	// generate fix message
	CInstinetOrder	FixOrder(Sequence, 'N', 'N', Order);

	if (SendMessage(FixOrder))
		return(Sequence);
	else
		return(0);
}

BOOL CInstinetSession::ProcessCancel(CCancel& Cancel)
{	
	long Sequence = GetSequenceSet()->GetNext();
	
	// set the cancel tag based off of the sequence number and date
	Cancel.SetCancelTag(Sequence);

	// generate fix message
	CFixCancel	FixCancel(Sequence, 'N', 'N', Cancel);

	return(SendMessage(FixCancel));
}

BOOL CInstinetSession::ProcessReplace(CReplace& Order)
{
	return(FALSE);
}

BOOL CInstinetSession::ProcessStatusRequest(CRequest& Order)
{
	return(FALSE);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// incoming event processing

CString CInstinetSession::ProcessMessage(CString Message, CConnection* pC)
{
	CString		CurrentMessage;
	CFixHeader	Header;
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	try
	{		
		while ((CurrentMessage = StripMessage(&Message)) != "")
		{
			pApp->ProcessLogEntry("Processing", CurrentMessage);
			Header.Fill(CurrentMessage);
			if (((Header.GetPossibleDuplicate() == 'Y') || (Header.GetPossibleResend() == 'Y')) && (Header.GetType() != FXMT_RESET))
			{
				if (IsDuplicate(Header.GetSequenceNumber()))
				{
					pApp->ProcessLogEntry("Fix Session", "DUPLICATE: " + CurrentMessage);
					// increment expected
					GetSequenceSet()->GetExpected();
					continue;
				}
			}

			if (Header.GetType() != FXMT_RESET)
				AddIncomingMessage(Header, CurrentMessage);

			switch (Header.GetType())
			{
				case FXMT_LOGON:
					ProcessLogon(Header, CurrentMessage);
					break;
				case FXMT_HEARTBEAT:
					ProcessHeartBeat(Header, CurrentMessage);
					break;
				case FXMT_TEST:
					ProcessTestRequest(Header, CurrentMessage);
					break;
				case FXMT_RESEND:
					ProcessResendRequest(Header, CurrentMessage);
					break;
				case FXMT_REJECT:
					ProcessReject(Header, CurrentMessage);
					break;
				case FXMT_RESET:
					ProcessSequenceReset(Header, CurrentMessage);
					break;
				case FXMT_LOGOUT:
					ProcessLogout(Header, CurrentMessage);
					break;
				case FXMT_EXECREPORT:
					ProcessExecutionReport(Header, CurrentMessage);
					break;
				case FXMT_CANCELREJECT:
					ProcessCancelReject(Header, CurrentMessage);
					break;
				case FXMT_IOI:
					ProcessIOI(Header, CurrentMessage);
					break;
				default:
					break;

			};
		}
	}

	catch (CDBException *e)
	{
		CString Error;
		Error.Format("EXCEPTION: Error Processing Incoming Message (%s): %s", e->m_strError, CurrentMessage);
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Fix Session", Error);
		e->Delete();
	}
	catch (CMemoryException *e)
	{
		CString Error;
		Error.Format("MEMORY EXCEPTION: Error Processing Incoming Message: %s", CurrentMessage);
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Fix Session", Error);
		e->Delete();
	}

	return(Message);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// order related message processing

void CInstinetSession::ProcessIOI(const CFixHeader& Header, CString& Message)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();	
	CFixIOI		IOI(Message);

	if (ProcessSequence(Header.GetSequenceNumber()))
		pApp->ProcessDebug("IOI", IOI.Debug());

}

void CInstinetSession::ProcessExecutionReport(const CFixHeader& Header, CString& Message)
{
	CMessageApp		*pApp = (CMessageApp *)AfxGetApp();	
	CFixExecution	FixReport(Message);

	if (ProcessSequence(Header.GetSequenceNumber()))
	{
		CFromInstinetExecution	Execution(FixReport);
		// set terminal id
		Execution.SetTerminalId(Header.GetSenderSubId());
		// set the owner
		Execution.SetSourceId(Header.GetTargetSubId());
		// set the contra if one wasn't provided by exchange
		if (Execution.ExecBroker() == "")
			Execution.SetExecBroker(pApp->Name());

		pApp->Handler().ProcessMessage(Execution);
	}
}

void CInstinetSession::ProcessCancelReject(const CFixHeader& Header, CString& Message)
{
	CFixCancelReject	FixReject(Message);
	CMessageApp			*pApp = (CMessageApp *)AfxGetApp();	

	if (ProcessSequence(Header.GetSequenceNumber()))
	{
		CFromFixCancelReject Reject(FixReject);
		Reject.SetSourceId(Header.GetTargetSubId());
		pApp->Handler().ProcessMessage(Reject);
	}
}

void CInstinetSession::ProcessReject(const CFixHeader& Header, CString& Message)
{
	CFixReject	FixReject(Message);
	CMessageApp	*pApp = (CMessageApp *)AfxGetApp();	
	
	if (ProcessSequence(Header.GetSequenceNumber()))
	{
		CFromFixReject	Reject(FixReject);
		Reject.SetSourceId(Header.GetTargetSubId());
		pApp->Handler().ProcessMessage(Reject);
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,pApp->Name(), Reject.Debug());
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// admin message processing

void CInstinetSession::ProcessResendRequest(const CFixHeader& Header, CString& Message)
{
	CFixResendRequest	Resend(Message);
	COutgoingMessageSet	*pSet;
	CString				SQLStatement;
	CFixSequenceReset	*pReset;
	CFixCancel			*pCancel;
	COrderBody			*pOrderBody;
	long				NextNum;
	CMessageApp			*pApp = (CMessageApp *)AfxGetApp();	

	CResendRequestBody&	Body = (CResendRequestBody &)Resend.GetBody();

	if ((Header.GetPossibleDuplicate() == 'Y') || (Header.GetPossibleResend() == 'Y'))
		return;

	GetSequenceSet()->GetExpected();

	pApp->ProcessLogEntry(pApp->Name(), Resend.m_Text);

	SQLStatement.Format("SELECT [MessageType],[SequenceNum],[TargetSubId],[Body],[Host],[Date],[Time] FROM [FixOutgoingTraffic] WHERE [Host]='%s' AND [Date]='%s' AND [SequenceNum] >= %ld AND [SequenceNum] <= %ld", 
		pApp->Name(), 
		CTime::GetCurrentTime().Format("%Y%m%d"),
		Body.GetBegin(),
		Body.GetEnd());

	NextNum = Body.GetBegin();
	
	if ((pSet = new COutgoingMessageSet(&pApp->Database(), SQLStatement)))
	{
		if (pSet->Open(CRecordset::snapshot))
		{
			if (!(pSet->GetRecordCount()))
			{
				while (!(pSet->IsEOF()))
				{
					if (pSet->m_TargetSubId == "ADMIN")
					{
						while (pSet->m_SequenceNum >= NextNum)
						{
							pReset = new CFixSequenceReset(NextNum++, 'Y', pSet->m_SequenceNum+1, 'Y', 'N');
							pApp->Host().SendMessage(pReset->GetMessage());
							delete pReset;
						}
					}
					else
					{
						// if it's a cancel then retransmit other wise send reject to client and sequence reset to fix host
						// so that an order isn't transmitted late						
						if (pSet->m_MessageType == "F")	// cancel so retransmit
						{								
							// generate fix message
							pCancel = new CFixCancel(pSet->m_SequenceNum, 'Y', 'Y', pSet->m_Body);
							// transmit
							pApp->Host().SendMessage(pCancel->GetMessage());					
							delete pCancel;
						}
						else
						{
							pReset = new CFixSequenceReset(NextNum++, 'Y', pSet->m_SequenceNum+1, 'Y', 'N');
							pApp->Host().SendMessage(pReset->GetMessage());
							delete pReset;
							// reject the order back to client
							pOrderBody = new COrderBody(pSet->m_Body);
							CFromFixReject	Reject(pOrderBody);
							pApp->Handler().ProcessMessage(Reject);
							delete pOrderBody;
						}
					}
					NextNum++;
					pSet->MoveNext();
				}
			}
			else
			{ 
				pReset = new CFixSequenceReset(Body.GetBegin(), 'Y', (GetSequenceSet())->NextOut(), 'Y', 'N');
				pApp->Host().SendMessage(pReset->GetMessage());
				delete pReset;
			}
		
			pSet->Close();					
		}
		delete pSet;
	}
		
}

void CInstinetSession::ProcessSequenceReset(const CFixHeader& pHeader, CString& Message)
{
	CFixSequenceReset SequenceReset(Message);
	CMessageApp	*pApp = (CMessageApp *)AfxGetApp();	

	pApp->ProcessLogEntry(pApp->Name(), SequenceReset.m_Text);
	GetSequenceSet()->SetExpected(((CSequenceResetBody &)SequenceReset.GetBody()).GetSequenceNumber());
}

void CInstinetSession::ProcessTestRequest(const CFixHeader& pHeader, CString& Message)
{
	CFixTestRequest	TestRequest(Message);
	CMessageApp	*pApp = (CMessageApp *)AfxGetApp();	
	
	pApp->ProcessLogEntry(pApp->Name(), TestRequest.m_Text);

	// check for gaps
	if (ProcessSequence(pHeader.GetSequenceNumber()))
	{
		// create response heartbeat message
		CFixHeartBeat HeartBeat(GetSequenceSet()->GetNext(), ((CTestRequestBody&)TestRequest.GetBody()).GetRequestId());	
		// send the heartbeat
		SendMessage(HeartBeat);
	}
}

void CInstinetSession::ProcessHeartBeat(const CFixHeader& Header, CString& Message)
{
	ProcessSequence(Header.GetSequenceNumber());
}

void CInstinetSession::ProcessLogout(const CFixHeader& Header, CString& Message)
{
	CFixLogout	Logout(Message);
	CMessageApp	*pApp = (CMessageApp *)AfxGetApp();	

	pApp->ProcessErrorEx(MODE_ERR_NOTIFY,pApp->Name(), Logout.m_Text);
	
	if ((Header.GetPossibleDuplicate() == 'Y') || (Header.GetPossibleResend() == 'Y'))
		return;

	if (ProcessSequence(Header.GetSequenceNumber()))
	{
		pApp->Host().SetLogonStatus(FALSE);
		pApp->Host().Close();
		pApp->UpdateHost();
	}

	pApp->SetStatusTimer(15000);
}

void CInstinetSession::ProcessLogon(const CFixHeader& Header, CString& Message)
{
	CMessageApp	*pApp = (CMessageApp *)AfxGetApp();	
	CFixLogon	Logon(Message);
	CLogonBody&	Body = (CLogonBody &)Logon.GetBody();

	pApp->ProcessErrorEx(MODE_ERR_BASIC,pApp->Name(), Logon.m_Text);

	pApp->SetStatusTimer(2000);

	if (InitiateLogon)
	{
		pApp->Host().SetLogonStatus(TRUE);
		pApp->UpdateHost();

		m_IncomingHeartBeatInterval = Body.GetInterval();
	}
	else
	{
		// set globals to received values
		HeartBeatInterval	= Body.GetInterval();
		Encryption			= Body.GetEncryption();

		// set the logon flag to logged on
		pApp->Host().SetLogonStatus(TRUE);
		// create reply
		CFixLogon LogonReply(GetSequenceSet()->GetNext());
		// send logon reply
		SendMessage(LogonReply);
	}

	// check for and handle any sequence gaps
	ProcessSequence(Header.GetSequenceNumber());
}
			
BOOL CInstinetSession::ProcessSequence(int Received)
{
	int Expected = GetSequenceSet()->GetExpected();
	
	if (Received > Expected)
	{
		// create resend message
		CFixResendRequest Resend(GetSequenceSet()->GetNext(), Expected, 999999);
		// decrement expected
		GetSequenceSet()->SetExpected(Expected);
		// send 
		SendMessage(Resend);	
		return(FALSE);
	}
	return(TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// utilities

CString CInstinetSession::StripMessage(CString *WholeMessage)
{	
	char	Delim = FIX_DELIM;
	int		ChecksumIndex;
	CString	TmpString;

	if ((ChecksumIndex = WholeMessage->Find(Delim + CString("10="))) > 0)
	{
		TmpString = WholeMessage->Left(ChecksumIndex+8);
		*WholeMessage = WholeMessage->Mid(ChecksumIndex + 8);
		return(TmpString);
	}

	return("");
}

BOOL CInstinetSession::SendMessage(const CFix& Fix)
{
	CMessageApp	*pApp = (CMessageApp *)AfxGetApp();	

	if ((Fix.GetHeader().GetPossibleDuplicate() != 'Y') && (Fix.GetHeader().GetPossibleResend() != 'Y'))
	{
		// log message
		AddOutgoingMessage(Fix.GetHeader(), (Fix.GetBody()).GetMessage());
	}

	// display it if it is an admin message
	switch (Fix.GetHeader().GetType())
	{
		case 'A':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':	
			pApp->ProcessDebug(pApp->Name(), Fix.m_Text);
			break;
		case '0':
			pApp->ProcessDebug(pApp->Name(), Fix.m_Text);
			break;
		default:
			break;
	};

	return(pApp->Host().SendMessage(Fix.GetMessage()));
}

void CInstinetSession::ProcessTimer()
{
	CTimeSpan TimeDiff;
	CMessageApp	*pApp = (CMessageApp *)AfxGetApp();	

	CTime CurrentTime = CTime::GetCurrentTime();
	int StartTime, EndTime;

	StartTime = GetPrivateProfileInt("Runtime Information",	"Start Time",6, pApp->Path() + pApp->Name() + ".DAT");
	EndTime = GetPrivateProfileInt("Runtime Information",	"End Time", 6, pApp->Path() + pApp->Name() + ".DAT");

	if(StartTime == EndTime)
	{
		if((CurrentTime.GetHour() == StartTime))
		{
			((CMessageApp*) AfxGetApp())->SetCloseDisplayMsg(FALSE);
			CFixLogout Logout(GetSequenceSet()->GetNext(), "END OF DAY");
			SendMessage(Logout);
			Sleep(1);
			pApp->Host().Close();
			AfxGetMainWnd()->PostMessage(WM_CLOSE);
			return;
		}
	}
	else if(StartTime > EndTime)
	{
		if ((CurrentTime.GetHour() >= EndTime) && (CurrentTime.GetHour() < StartTime))
		{
			((CMessageApp*) AfxGetApp())->SetCloseDisplayMsg(FALSE);
			CFixLogout Logout(GetSequenceSet()->GetNext(), "END OF DAY");
			SendMessage(Logout);
			Sleep(1);
			pApp->Host().Close();
			AfxGetMainWnd()->PostMessage(WM_CLOSE);
			return;
		}
	}
	else
	{
		if ((CurrentTime.GetHour() < StartTime) || (CurrentTime.GetHour() >= EndTime))
		{
			((CMessageApp*) AfxGetApp())->SetCloseDisplayMsg(FALSE);
			CFixLogout Logout(GetSequenceSet()->GetNext(), "END OF DAY");
			SendMessage(Logout);
			Sleep(1);
			pApp->Host().Close();
			AfxGetMainWnd()->PostMessage(WM_CLOSE);
			return;
		}
	}

	if ((pApp->Host().Reconnect()) && (!(pApp->Host().m_Connected)))	// not connected
	{
		pApp->SetStatusTimer(15000);
 		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,pApp->Name(), "Connection To Host Lost");
		pApp->Handler().ProcessMessage("DEAD");

		pApp->RecycleHost();			
	}
	else if ((pApp->Host().Reconnect()) && (!(pApp->Host().LoggedOn())))	// not logged on
	{
		pApp->SetStatusTimer(2000);
		pApp->Handler().ProcessMessage("DEAD");
		pApp->ProcessErrorEx(MODE_ERR_NOTIFY,pApp->Name(), "Connected To Host But Not Logged On");
		CFixLogon Logon(GetSequenceSet()->GetNext());
		SendMessage(Logon);
	}
	else	// connection is good so just check for heartbeats and test requests
	{		
		// calc the time since the last message was sent
		TimeDiff = CurrentTime - pApp->Host().GetLastTransmission();
		// generate a heartbeat
		if (TimeDiff.GetTotalSeconds() >= HeartBeatInterval)	
		{
			CFixHeartBeat Beat(GetSequenceSet()->GetNext(), CFix::GetFixTime());
			if (SendMessage(Beat))
				pApp->Handler().ProcessMessage("ALIVE");
		}

		// calc the time since the last message was received
		TimeDiff = CurrentTime - pApp->Host().GetLastReception();

		// generate a test request or logout	
		if (TimeDiff.GetTotalSeconds() >= (m_IncomingHeartBeatInterval*3))	// delay is too long so close down and try a restart
		{
			pApp->Handler().ProcessMessage("DEAD");
			// send one last logout
			CFixLogout Logout(GetSequenceSet()->GetNext(), "TIMEOUT");
			SendMessage(Logout);
			// close down and delete the host connection
			pApp->RecycleHost();
		}
		else if (TimeDiff.GetTotalSeconds() >= (m_IncomingHeartBeatInterval*2)+4)	// delay is too long so generate a logout
		{
			CFixLogout Logout(GetSequenceSet()->GetNext(), "TIMEOUT");
			SendMessage(Logout);
		}
		else if (TimeDiff.GetTotalSeconds() >= m_IncomingHeartBeatInterval+2)	// generate a test request to see if there is a problem
		{
			TimeDiff = CurrentTime - m_LastTestRequest;

			if ((TimeDiff.GetTotalSeconds() >= m_IncomingHeartBeatInterval) &&  (pApp->Host().LoggedOn()))
			{
				CFixTestRequest Request(GetSequenceSet()->GetNext(), CFix::GetFixTime());
				SendMessage(Request);
				m_LastTestRequest = CurrentTime;
			}
		}
	}
}

void CInstinetSession::ResetSequenceNumber()
{
	CFixSequenceReset Reset(GetSequenceSet()->GetNext(), 'Y', GetSequenceSet()->NextOut()+1, 'N', 'N');

	SendMessage(Reset);

	GetSequenceSet()->SetNext(GetSequenceSet()->NextOut()+1);
}	

BOOL CInstinetSession::IsDuplicate(long SequenceNumber)
{
	CString SQLStatement;
	BOOL Found = FALSE;
	CIncomingMessageSet	*pSet;
	CMessageApp	*pApp = (CMessageApp *)AfxGetApp();	
	
	SQLStatement.Format("SELECT [MessageType],[SequenceNum],[TargetSubId],[Body],[Host],[Date],[Time] FROM [FixIncomingTraffic] WHERE [Host]='%s' AND [Date]='%s' AND [SequenceNum] = %ld", 
		pApp->Name(), 
		CTime::GetCurrentTime().Format("%Y%m%d"),
		SequenceNumber);
		
	if ((pSet = new CIncomingMessageSet(&pApp->Database(), SQLStatement)))
	{
		if (pSet->Open(CRecordset::snapshot))
		{
			if (pSet->GetRecordCount())
				Found = TRUE;
				
			pSet->Close();
		}
		delete pSet;
	}

	return(Found);
}