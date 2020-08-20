#include "CMSSession.h"
#include "MessageApp.h"
//#include "HostConnection.h"
//#include "CMSAdminMsgs.h"
//#include "CMSAppMsgs.h"

#pragma warning(disable:4786)

const char* CCMSSession::m_MonthArray[12] =
		{"JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"};
//-- CCMSSession ------------------------------------------------------------------------

//---- Services -------------------------------------------------------------------------

CCMSSession::CCMSSession()
	: m_RcvHeartbtInt(30), m_LastSendTime(CTime::GetCurrentTime()), m_LastRcvTime(CTime::GetCurrentTime()),
		m_AwaitingResend(false), m_LogonInitiated(false), m_LogoutInitiated(false), m_bDoResetSequence(false)
{
	m_pApp = (CMessageApp*) AfxGetApp();

	GetPrivateProfileString("CMS Information", "SenderId", "", m_SenderID.GetBuffer(32), 32, m_pApp->DatFile());
	m_SenderID.ReleaseBuffer();
	GetPrivateProfileString("CMS Information", "TargetId", "", m_TargetID.GetBuffer(32), 32, m_pApp->DatFile());
	m_TargetID.ReleaseBuffer();
	GetPrivateProfileString("CMS Information", "BranchCode", "", m_BranchCode.GetBuffer(8), 8, m_pApp->DatFile()); 
	m_BranchCode.ReleaseBuffer();
	::GetPrivateProfileString("CMS Information", "Password", "", m_Password.GetBuffer(8), 8, m_pApp->DatFile());
	m_Password.ReleaseBuffer();

	m_pApp->ProcessLogEntry("Branch Code: ",m_BranchCode);
	m_pApp->ProcessLogEntry("SenderID: ",m_SenderID);
	m_pApp->ProcessLogEntry("TargetID: ",m_TargetID);

	m_SendHeartbtInt = GetPrivateProfileInt("CMS Information",	"Heartbeat", 30, m_pApp->DatFile());
	m_bDoLogon = (bool)GetPrivateProfileInt("CMS Information",	"Do Logon", 0, m_pApp->DatFile());
	m_bDoLogout = (bool)GetPrivateProfileInt("CMS Information",	"Do Logout", 0, m_pApp->DatFile());
	m_bDoHeartBeat = (bool)GetPrivateProfileInt("CMS Information",	"Do Heartbeat", 0, m_pApp->DatFile());
	m_bDoResetSequence = (bool)GetPrivateProfileInt("CMS Information",	"Do ResetSequence", 0, m_pApp->DatFile());

	CString l_FileName;
	GetPrivateProfileString("CMS Information", "Incoming Sequence File", "IncomingSeq.txt", 
		l_FileName.GetBuffer(128), 128, m_pApp->DatFile());
	l_FileName.ReleaseBuffer();
	m_IncomingMsgs.SetFile(l_FileName);

	GetPrivateProfileString("CMS Information", "Outgoing Sequence File", "OutgoingSeq.txt", 
		l_FileName.GetBuffer(128), 128, m_pApp->DatFile());
	l_FileName.ReleaseBuffer();
	m_OutgoingMsgs.SetFile(l_FileName);

	m_StartTime = GetPrivateProfileInt("Runtime Information",	"Start Time",600, m_pApp->DatFile());
	m_EndTime = GetPrivateProfileInt("Runtime Information",	"End Time", 1800, m_pApp->DatFile());

}


bool CCMSSession::StartSession()
{
	CString l_LastDate;
	GetPrivateProfileString("Runtime Information",	"Last Run Date", "", l_LastDate.GetBuffer(128), 128,m_pApp->DatFile());
	l_LastDate.ReleaseBuffer();
	CString l_CurDate = CTime::GetCurrentTime().Format("%Y%m%d");
	WritePrivateProfileString("Runtime Information",	"Last Run Date", l_CurDate,m_pApp->DatFile());

	bool l_bDeleteOld = (l_CurDate != l_LastDate);

	return m_IncomingMsgs.LoadFile(l_bDeleteOld) && m_OutgoingMsgs.LoadFile(l_bDeleteOld);
}

BOOL CCMSSession::Logon()
{
	if(m_bDoLogon && !m_LogonInitiated) 
	{
		CCMSMsg l_Logon;
		GetLogonMsg(l_Logon);
		if(SendMsg(l_Logon)) {
			m_LogonInitiated = true;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CCMSSession::Logout()
{
	m_pApp->ProcessErrorEx(MODE_ERR_NOTIFY, m_pApp->Name()+" CCMSSession::Logout() "+m_SenderID, "logout.");
	if(m_bDoLogout)
	{
		CCMSMsg l_Logout;
		GetLogoutMsg(l_Logout);
		SendMsg(l_Logout);
		if(!m_LogoutInitiated)
		{
			m_LogoutInitiated = true;
			return FALSE;
		}
	}
	Shutdown();
	return true;
}

long CCMSSession::ProcessEntry(COrder& a_Order)
{
	CCMSMsg l_CMSMsg;

	l_CMSMsg.SetMsgType(CMS_Order);
	l_CMSMsg.SetMsgSeqNum(GetNextOutgoingSeqNum());
	TString& l_MsgBody = l_CMSMsg.MsgStrForChange();

	//lone zero, firm nmemonic
	if(a_Order.GiveUpAccount() != "")
		l_MsgBody.append(a_Order.GiveUpAccount());
	else
		l_MsgBody.append(m_SenderID); //used as default 
	
	l_MsgBody += '\n';

	l_MsgBody += a_Order.OrderTag() + '\n';

	//line 1A needed for PHLX linkage
	OnSetLineOneA(a_Order, l_MsgBody);

//	l_CMSMsg.SetPossDupOffset(l_MsgBody.length());
	// Blank Line
	l_MsgBody += '\n';
	
	// Line 2.  No poss dup
	
	if (a_Order.Side() == COrder::Buy) 
		l_MsgBody += "BUY ";
	else
		l_MsgBody += "SL ";
	
	if (a_Order.CallOrPut() == COrder::Put)
		l_MsgBody += "PUT\n";
	else 
		l_MsgBody += "CALL\n";
	
	// Line 3
	CString l_Temp;
	l_Temp.Format("%d ",a_Order.Shares());
	l_MsgBody += l_Temp;
	l_MsgBody += a_Order.Symbol() +' ';
	l_MsgBody.append(GetExpirationMonth(a_Order.ExpDate()));
	l_MsgBody.append(" ");
	l_Temp.Format("%.4f ",a_Order.StrikePrice());
	l_MsgBody += l_Temp;

	//order type
	if(!GetOrderType(a_Order, l_MsgBody))
	{
		TString l_Error("Invalid order type for this exchange: ");
		char l_Type[3];
		itoa((long)a_Order.Type(),l_Type,10);
		l_Error += l_Type;
		a_Order.SetInstructions(l_Error.c_str());
		return false;
	}
	
	// Line 3A
	if(!GetTIF(a_Order.TIF(),l_MsgBody))
	{
		TString l_Error("Invalid Time in Force for this account type/exchange: ");
		char l_TIF[3];
		itoa((long)a_Order.TIF(),l_TIF,10);
		l_Error += l_TIF;
		a_Order.SetInstructions(l_Error.c_str());
		return false;
	}
	
	
	// Line 3C (3B not used)
	if(a_Order.OpenClose() == 1)
		l_MsgBody += "OPEN ";
	else if(a_Order.OpenClose() == 0)
		l_MsgBody += "CLOSE ";
	else
		return false;

	//set CUST, FIRM, CMTA, GIVEUP Linkage, etc
	if(!OnSetAccountType(a_Order, l_MsgBody))
	{
		a_Order.SetInstructions(l_MsgBody.c_str());
		return false;
	}
		
	// Linkage, line 4B/C
/*	if(a_Order.Capacity() == COrder::Linkage)
	{
		if(!OnSetLinkage(a_Order, l_MsgBody))
			return false;
	}
*/

	BuildCMSTrailer(l_MsgBody);
	return SendMsg(l_CMSMsg);
}

BOOL CCMSSession::ProcessReplace(CReplace& a_Replace)
{
	//we are not going to do replace right now
	return FALSE;
}

BOOL CCMSSession::ProcessCancel(CCancel& a_Cancel)
{
	CCMSMsg l_CMSMsg;

	l_CMSMsg.SetMsgType(CMS_Cancel);
	l_CMSMsg.SetMsgSeqNum(GetNextOutgoingSeqNum());
	TString& l_MsgBody = l_CMSMsg.MsgStrForChange();
	//lone zero, firm nmemonic
	if(a_Cancel.GiveUpAccount() != "")
		l_MsgBody.append(a_Cancel.GiveUpAccount());
	else
		l_MsgBody.append(m_SenderID); //used as default
	l_MsgBody += '\n';

	l_MsgBody += a_Cancel.CancelTag() + '\n';
	
	// Blank Line
	l_MsgBody += '\n';
	
	// Line 2.  No poss dup
	l_MsgBody += "CXL ";

	//get the orginal order message, change quantity if necessary, then send it
	//since we can assume the sequence number is the same as the numeric part of the original order tag
	CString l_OrderID = a_Cancel.OrderTag();
	CString l_OrderSeqNumStr = l_OrderID.Mid(l_OrderID.Find(CMS_MSG_FIELD_DELIMITOR)+1);
	long l_OrderSeqNum = atoi(l_OrderSeqNumStr);

	TCMSMsgPtrList l_OrdList;
	m_OutgoingMsgs.FindRange(l_OrderSeqNum,l_OrderSeqNum,l_OrdList);

	if(l_OrdList.empty())
	{
		TString l_Error("CMSSession:Can't find Original OrdID in sequence file for cancel: ");
		l_Error += l_OrderSeqNumStr;
		a_Cancel.SetInstructions(l_Error.c_str());
		m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER, m_pApp->Name() + " CMSSession::ProcessCancel()", "Can't find ID in sequence file: " + l_OrderID);
		return FALSE;
	}
	else if(!BuildCxlStr(l_OrdList.begin()->get(), l_MsgBody, a_Cancel))
	{
		TString l_Error("CMSSession:Can't build cancel message from sequence message: ");
		l_Error += l_OrderSeqNumStr;
		a_Cancel.SetInstructions(l_Error.c_str());
		m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER, m_pApp->Name() + " CMSSession::ProcessCancel()", "Can't build cancel message from sequence message: " + l_OrderID);
		return FALSE;
	}


	BuildCMSTrailer(l_MsgBody); 
	return SendMsg(l_CMSMsg);
}

CString	CCMSSession::ProcessMessage(CString a_MsgStr, CBaseConnection* a_pConnection)
{
	int l_Offset=0;
	while(true)
	{
		IncomingCMSMsg l_CMSMsg;
		if(OnUnpackTCPMsg(l_CMSMsg, a_MsgStr,l_Offset)) 
			Process(l_CMSMsg);
		else
			break;
	}
	m_LastRcvTime = CTime::GetCurrentTime();
	
	return a_MsgStr.Right(a_MsgStr.GetLength() - l_Offset);
}

void CCMSSession::ProcessTimer()
{
	CMessageApp* l_pApp = (CMessageApp*) AfxGetApp();
	CTime l_CurrTime = CTime::GetCurrentTime();
	int l_CurrHour = l_CurrTime.GetHour();
	if(m_LogoutInitiated) {
		CTimeSpan l_TimeDiff = l_CurrTime - m_LastRcvTime;
		if(l_TimeDiff.GetTotalSeconds() > 5) {
			Shutdown();
			return;
		}
	}
	else
	{
		int HM = 100 * l_CurrTime.GetHour() + l_CurrTime.GetMinute();
		if(m_StartTime == m_EndTime) 
		{
			if(HM == m_StartTime) 
			{
				Logout();
				return;
			}
		}
		else if(m_StartTime > m_EndTime) 
		{
			if((HM >= m_EndTime) && (HM < m_StartTime)) 
			{
				Logout();
				return;
			}
		}
		else if((HM < m_StartTime) || (HM >= m_EndTime)) 
		{
			Logout();
			return;
		}
	}
	if(l_pApp->Host().Reconnect() && !l_pApp->Host().m_Connected)	
	{
		l_pApp->SetStatusTimer(15000);
 		l_pApp->ProcessErrorEx(MODE_ERR_NOTIFY,l_pApp->Name() + " CMSSession::OnTimer", "Connection To Host Lost");
		l_pApp->RecycleHost();	
		m_LogonInitiated = false;
	}
	else if(l_pApp->Host().Reconnect() && !l_pApp->Host().LoggedOn()) 
	{
		l_pApp->SetStatusTimer(2000);
		l_pApp->ProcessErrorEx(MODE_ERR_NOTIFY,m_pApp->Name() + " CMSSession::OnTimer" , "Connected To Host But Not Logged On");
		Logon();
	}
	else if(l_pApp->Host().LoggedOn() && m_bDoHeartBeat)
	{
		int l_TimeDiff = (l_CurrTime - m_LastSendTime).GetTotalSeconds();
		if(l_TimeDiff >= m_SendHeartbtInt) 
		{
			m_pApp->ProcessLogEntry(m_pApp->Name(),"Sending heartbeat.");
			CCMSMsg l_Heartbt;
			if(GetHeartBeatMsg(l_Heartbt))
				SendMsg(l_Heartbt);
		}
	}
}

BOOL CCMSSession::SendMsg(CCMSMsg& a_Msg)
{
	if(!OnPackTCPMsg(a_Msg))
		return FALSE;
	if(!a_Msg.PossDupFlag()) {
		CCMSMsgSequence::EAddMsgReturnCode l_RetCode;
		if((l_RetCode = m_OutgoingMsgs.AddMsg(a_Msg)) != CCMSMsgSequence::ADD_MSG_NO_ERROR) {
			switch(l_RetCode) {
				case CCMSMsgSequence::ADD_MSG_SEQ_NUM_GAP:
					m_pApp->ProcessErrorEx(MODE_ERR_NOTIFY,m_pApp->Name() + " CCMSSession::SendMsg","SERIOUS ERROR: Sequence number gap detected in outgoing message.");
					break;
				case CCMSMsgSequence::ADD_MSG_SEQ_NUM_SMALLER_THAN_EXPECTED:
					m_pApp->ProcessErrorEx(MODE_ERR_NOTIFY,m_pApp->Name() + " CCMSSession::SendMsg","SERIOUS ERROR: Outgoing sequence number smaller than expected.");
					break;
				case CCMSMsgSequence::ADD_MSG_CANT_WRITE_TO_FILE:
					m_pApp->ProcessErrorEx(MODE_ERR_NOTIFY,m_pApp->Name() + " CCMSSession::SendMsg","SERIOUS ERROR: Can't write to sequence file.");
					break;
				default:
					m_pApp->ProcessErrorEx(MODE_ERR_NOTIFY,m_pApp->Name() + " CCMSSession::SendMsg","Unknown error occurred while adding outgoing msg to sequence.");
					break;
			}
			return FALSE;
		}
	}
	if(((CMessageApp*) AfxGetApp())->Host().SendMessage(a_Msg.MsgStr().c_str())) {
		m_LastSendTime = CTime::GetCurrentTime();
		return TRUE;
	}
	return FALSE;
}

void CCMSSession::Process(IncomingCMSMsg& a_Msg)
{
	if(a_Msg.PossDupFlag() 
		&& m_IncomingMsgs.SeqNumPos(a_Msg.MsgSeqNum())
		&& a_Msg.MsgSeqNum() < m_IncomingMsgs.NextMsgSeqNum()) 
	{
		//Poss dump that is already processed.  ignored
		return;
	}
	switch(a_Msg.MsgType()) {

	case CMS_Status_Order_ACK:
	case CMS_Report_Partial:
	case CMS_Report_Fill:
	case CMS_Admin_UR_OUT:
	case CMS_Admin_CORRECTED_PRICE:
	case CMS_Admin_ND:
		ProcessReport(a_Msg);
		break;

	case CMS_Status_OrderRej:
		ProcessReject(a_Msg);
		break;

	case CMS_Admin_TLTC:
	case CMS_Status_CancelRej:
		//Cancel Reject should not be sent back and just logged
		ProcessCancelReject(a_Msg);
		break;

//connection related:
	case CMS_Heartbeat:
		ProcessHeartbeat(a_Msg);
		break;

	case CMS_ResendRequest:
	case CMS_Status_NumberGap:
		ProcessResendRequest(a_Msg);
		break;
	case CMS_SequenceReset:
		ProcessSeqReset(a_Msg);
		break;

//	case CMS_Status_GoodNight:
	case CMS_Logout:
		ProcessLogout(a_Msg);
		break;
	case CMS_LogonReply:
		ProcessLogonReply(a_Msg);
		break;

	//admin messages

	case CMS_Status_CMSRestart:
	case CMS_Status_Admin_ACK:
	case CMS_Status_FreeFormat:
	case CMS_Status_Rej:
	case CMS_Status_PossLoss:
	// business alert or event log entry //TEMP
		((CMessageApp*) AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER, m_pApp->Name() + " CCMSSession::Process()", a_Msg.Text().c_str());
		break;

	case CMS_Admin_UR_STPD:
	case CMS_Admin_UR_GUAR:
	//do not do anything
		break;
	
	//Process Replace Reject Messages
	case CMS_Status_ReplaceRej:
		ProcessReplaceReject(a_Msg);
		break;

	case CMS_Status:
	case CMS_Admin:
	case CMS_Report:
	case CMS_UNKNOWN:
	default:
		CString l_Err = "Unknown message received: ";
		l_Err += a_Msg.MsgStr().c_str();
		((CMessageApp*) AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER, m_pApp->Name() + " CCMSSession::Process()", l_Err);
		if(a_Msg.MsgSeqNum() >0)
			AddIncomingMsgToSequence(a_Msg);
		break;
	}
}

void CCMSSession::ProcessLogonReply(const IncomingCMSMsg& a_Msg)
{
		m_pApp->ProcessTraffic(m_pApp->Name(),"Logon acknowledgement received.");
		m_pApp->Host().SetLogonStatus(TRUE);
		m_pApp->UpdateHost();
		m_pApp->SetStatusTimer(2000);
		m_LogonInitiated = false;
}
/*
void CCMSSession::ProcessHeartbeat(const CCMSMsg& a_Msg)
{
//	if(!AddIncomingMsgToSequence(a_Heartbeat))
//		return;
//	ProcessLogEntry("Heartbeat received.  ");
}
*/
void CCMSSession::ProcessSeqReset(const IncomingCMSMsg& a_Msg)
{
	int l_NewSeqNum = a_Msg.MsgSeqNum();
	CString l_Alert;
	l_Alert.Format("In comming sequence Reset from %d to %d", GetNextIncomingSeqNum(),a_Msg.MsgSeqNum());
	m_pApp->ProcessLogEntry(m_pApp->Name(),l_Alert);
	SetNextIncomingSeqNum(l_NewSeqNum); 
}

void CCMSSession::ProcessLogout(const IncomingCMSMsg& a_Msg)
{
	m_pApp->ProcessLogEntry(m_pApp->Name(),"Received good night from exchange");
}

void CCMSSession::ProcessResendRequest(const IncomingCMSMsg& a_Msg)
{
	long l_BeginSeqNo = a_Msg.ResendStart();
	long l_EndSeqNo = a_Msg.ResendEnd();
	if(l_BeginSeqNo > l_EndSeqNo)
		return;

	CString l_Alert;
	l_Alert.Format("Resend request received -- from %d to %d", l_BeginSeqNo,l_EndSeqNo);
	m_pApp->ProcessLogEntry(m_pApp->Name(),l_Alert);

	TCMSMsgPtrList l_MsgPtrList;
	if(!m_OutgoingMsgs.FindRange(l_BeginSeqNo, l_EndSeqNo, l_MsgPtrList))
	{
		l_Alert.Format("Find range failed in resendReq from %d to %d", l_BeginSeqNo,l_EndSeqNo);
		m_pApp->ProcessErrorEx(MODE_ERR_DEVELOPER, m_pApp->Name() + " CCMSSession::ProcessResendRequest()",l_Alert);
	}


	//send rejects for cancel/replace/order
	for(TCMSMsgPtrList::iterator i=l_MsgPtrList.begin(); i!=l_MsgPtrList.end(); ++i) 
	{
		switch((*i)->MsgType()) 
		{
			//we reject to client unreceived messages
			case CMS_Cancel:
			case CMS_Replace:
			case CMS_Order:
			{
				CReject l_Reject;
				l_Reject.SetOrderTag((*i)->ParseOutOrderTag().c_str());
				l_Reject.SetReason("Sequence Reset Error");
				m_pApp->Handler().ProcessMessage(l_Reject);
				break;
			}
			default: 
				break;
		}
	}

	//if reset is accepted, do reset sequence.  We are not resend anything to exchange
	if(m_bDoResetSequence) 
	{
		//do sequence reset
		CCMSMsg l_ResetMsg;
		GetResetSeqMsg(l_EndSeqNo+1,l_ResetMsg);
		if(!SendMsg(l_ResetMsg))
			m_pApp->ProcessErrorEx(MODE_ERR_NOTIFY, m_pApp->Name() + " CCMSSession::ProcessResendRequest", "Can't send reset seq msg");
	}

	if(a_Msg.MsgSeqNum()>0)
		AddIncomingMsgToSequence(a_Msg);
}

void CCMSSession::ProcessReport(const IncomingCMSMsg& a_Msg)
{
	if(!AddIncomingMsgToSequence(a_Msg))
		return;

	CExecution l_Exec;

	ECMSMsgType l_MsgType = a_Msg.MsgType();
	if(l_MsgType == CMS_Admin_CORRECTED_PRICE)
	{
		//trade correction
		((CMessageApp*)AfxGetApp())->ProcessErrorEx(MODE_ERR_BUSINESS,m_pApp->Name() + " CCMSSession:Trade Correction. Human Intervention necessary!!!", a_Msg.CMSMsgPtr());
		return;
	}

	switch(l_MsgType)
	{
		case CMS_Status_Order_ACK:
			l_Exec.SetStatus(COrder::PendingNew);
			break;
		case CMS_Report_Partial:
			l_Exec.SetStatus(COrder::PartiallyFilled);
			break;
		case CMS_Report_Fill:
			l_Exec.SetStatus(COrder::Filled);
			break;
		case CMS_Admin_UR_OUT:
			l_Exec.SetStatus(COrder::Canceled);
			break;
		case CMS_Admin_ND:
			l_Exec.SetStatus(COrder::DoneForDay);
			break;
		default:
			ASSERT(FALSE);
	}

	l_Exec.SetSourceId(m_TargetID);
	l_Exec.SetReport('N');
	l_Exec.SetOrderTag(a_Msg.OrderID().c_str());
	char l_ExchSeqTag[16];
	itoa(a_Msg.MsgSeqNum(),l_ExchSeqTag,10);
	l_Exec.SetExchangeTag(l_ExchSeqTag);
//	l_Exec.SetOriginalTag(a_Exec.OrigClOrdID().c_str());
//	l_Exec.SetExecBroker(a_Exec.ExecBroker().c_str());
//	l_Exec.SetConfirm(a_Exec.ExecID().c_str());
	l_Exec.SetTransactionType("0");
//	l_Exec.SetReferenceConfirm(a_Exec.ExecRefID().c_str());
//	l_Exec.SetAccount(a_Exec.Account().c_str());
//	l_Exec.SetSymbol(a_Exec.Symbol().c_str());
//	l_Exec.SetInstructions(a_Exec.ExecInst().c_str());
	l_Exec.SetText(a_Msg.Text().c_str());
//	l_Exec.SetSide(m_Translator.m_ToStaffordSide[a_Exec.Side()]);
//	l_Exec.SetShares(a_Exec.OrderQty());
//	l_Exec.SetOrderType(m_Translator.m_ToStaffordType[a_Exec.OrdType()]);
//	l_Exec.SetTimeInForce(m_Translator.m_ToStaffordTIF[a_Exec.TimeInForce()]);
	l_Exec.SetLastShares(a_Msg.LastShares());
	l_Exec.SetLeaves(a_Msg.Leaves());
//	l_Exec.SetCumulative(a_Exec.CumQty());
//	l_Exec.SetAverage(a_Exec.AvgPx());
//	l_Exec.SetPrice(a_Exec.Price());
//	l_Exec.SetStopPrice(a_Exec.StopPx());
	l_Exec.SetLastPrice(a_Msg.LastPrice());
//	l_Exec.SetExecType(m_Translator.m_ToStaffordStatus[a_Exec.ExecType()]);
	
	CTime CurrentTime = CTime::GetCurrentTime();
	l_Exec.SetTransactionTime(CurrentTime.FormatGmt("%Y%m%d-%H:%M:%S"));

	ExecutionOverride(l_Exec, a_Msg);

	((CMessageApp*) AfxGetApp())->Handler().ProcessMessage(l_Exec);

}

void CCMSSession::ProcessCancelReject(const IncomingCMSMsg& a_Msg)
{
/*	if(!AddIncomingMsgToSequence(a_Reject))
		return;
	CCancelReject l_Reject;
	l_Reject.SetSourceId(a_Reject.TargetSubID().c_str());
	l_Reject.SetOrderTag(a_Reject.OrigClOrdID().c_str());
	l_Reject.SetExchangeTag(a_Reject.OrderID().c_str());
	l_Reject.SetCancelTag(a_Reject.ClOrdID().c_str());
	l_Reject.SetText(a_Reject.Text().c_str());
	l_Reject.SetStatus(m_Translator.m_ToStaffordStatus[a_Reject.OrdStatus()]);
	((CMessageApp*) AfxGetApp())->Handler().ProcessMessage(l_Reject);
*/
}


void CCMSSession::Shutdown()
{
	((CMessageApp*) AfxGetApp())->Host().Close();
	((CMessageApp*) AfxGetApp())->SetCloseDisplayMsg(FALSE);
	AfxGetMainWnd()->PostMessage(WM_CLOSE);
}
//---------------------------------------------------------------------------------------

//---- Laborers -------------------------------------------------------------------------


bool CCMSSession::AddIncomingMsgToSequence(const CCMSMsg& a_Msg)
{
	CCMSMsgSequence::EAddMsgReturnCode l_RetCode;
	if((l_RetCode = m_IncomingMsgs.AddMsg(a_Msg)) != CCMSMsgSequence::ADD_MSG_NO_ERROR) 
	{
		switch(l_RetCode) {
			case CCMSMsgSequence::ADD_MSG_SEQ_NUM_GAP:
				if(!m_AwaitingResend) {
/*					CCMSMsg l_ResendReq;
					if(GetResendReqMsg(l_ResendReq)) //pcx will accept resend request
					{
						m_AwaitingResend = true;
						SendMsg(l_ResendReq);
					} 
					else*/
					{
						//reset and give warning
						CString l_ErrMsg;
						l_ErrMsg.Format("Possible message loss!  Received: %d  Expected: %d.  Contact Exchange for the missing messages", a_Msg.MsgSeqNum(), m_IncomingMsgs.NextMsgSeqNum());
						m_pApp->ProcessErrorEx(MODE_ERR_NOTIFY, m_pApp->Name() + " CMSSession::AddIncomingMsgToSequence()", l_ErrMsg);
						m_IncomingMsgs.ResetNextMsgSeqNum(a_Msg.MsgSeqNum()+1);
						m_IncomingMsgs.AddMsg(a_Msg);
						return true;
					}
				}
				break;
			case CCMSMsgSequence::ADD_MSG_SEQ_NUM_SMALLER_THAN_EXPECTED: 
				{
					CString l_ErrMsg;
					l_ErrMsg.Format("Sequence number mismatch!  Received: %d  Expect: %d.  Remote exchange may have restarted", a_Msg.MsgSeqNum(), m_IncomingMsgs.NextMsgSeqNum());
					m_pApp->ProcessErrorEx(MODE_ERR_NOTIFY, m_pApp->Name() + " CMSSession::AddIncomingMsgToSequence()", l_ErrMsg);
					m_IncomingMsgs.SeqNumPos(a_Msg.MsgSeqNum());
					m_IncomingMsgs.AddMsg(a_Msg);
					return true;
				}
			default:
				m_pApp->ProcessErrorEx(MODE_ERR_NOTIFY,m_pApp->Name() + " CMSSession::AddIncomingMsgToSequence()","Unknown error occurred while adding incoming msg to sequence.");
		}
		return false;
	}
	if(m_AwaitingResend)
		m_AwaitingResend = false;
	return true;
}

bool CCMSSession::ParseCMSMsg(IncomingCMSMsg& a_CMSMsg)
{
	if(!ParseHeader(a_CMSMsg))
		return false;

	switch(a_CMSMsg.MsgType())
	{
	case CMS_Report:
		return ParseReportMsg(a_CMSMsg);
		break;
	case CMS_Admin:
		return ParseAdminMsg(a_CMSMsg);
		break;
	case CMS_Status:
		return ParseStatusMsg(a_CMSMsg);
		break;
	default:
		return false;
	}

	return true;
}

bool CCMSSession::ParseHeader(IncomingCMSMsg& a_CMSMsg)
{
	//it is the fourth one in the header section from exchange
	char l_MsgType = CMS_UNKNOWN;
	int l_SeqNum = 0;
	char* l_Pos;

	//firm id
	if((l_Pos = strchr(a_CMSMsg.CMSMsgPtr(),CMS_MSG_FIELD_DELIMITOR)) == NULL)
			return false;
	//exchange id
	if((l_Pos = strchr(l_Pos+1,CMS_MSG_FIELD_DELIMITOR)) == NULL)
			return false;

	//sequence number, message type
	if(sscanf(l_Pos+1, "%d %c",&l_SeqNum, &l_MsgType) <2)
		return false;

	//  Should be A,R,S,U
	switch(l_MsgType)
	{
	case CMS_Admin:
	case CMS_Status:
	case CMS_Report:
		a_CMSMsg.SetMsgType((ECMSMsgType) l_MsgType);
		break;
	default:
		return false;
	}
	if(a_CMSMsg.MsgSeqNum() <=0)
		a_CMSMsg.SetMsgSeqNum(l_SeqNum);
	return true;
}

bool CCMSSession::ParseAdminMsg(IncomingCMSMsg& a_CMSMsg)
{
	//skip header line
	const TString&  l_MsgStr = a_CMSMsg.MsgStr();

	int l_StartPos, l_EndPos;

	//Get branch code and sequence number
	//ParseOutOrderTag() can this be used instead? //TEMP
	l_StartPos = l_MsgStr.find(GetBranchCode());
	l_EndPos = l_MsgStr.find(CMS_MSG_LINE_DELIMITOR, l_StartPos);
	a_CMSMsg.SetOrderID(l_MsgStr.substr(l_StartPos, l_EndPos - l_StartPos));
	l_EndPos++;

	if(l_MsgStr.find("UR OUT",l_EndPos) != TString::npos)
	{
		//check to see if it is UR OUT
		a_CMSMsg.SetMsgType(CMS_Admin_UR_OUT);
	}
	else if(l_MsgStr.find("TLTC", l_EndPos) != TString::npos)
	{
		//check to see if it is TLTC
		a_CMSMsg.SetMsgType(CMS_Admin_TLTC);
	} 
	else if(l_MsgStr.find("CORRECTED PRICE", l_EndPos) != TString::npos)
	{
	//check to see UR GUAR 
		a_CMSMsg.SetMsgType(CMS_Admin_CORRECTED_PRICE);
	}
	else if(l_MsgStr.find("UR STPD", l_EndPos) != TString::npos)
	{
	//check to see UR STPD 
		a_CMSMsg.SetMsgType(CMS_Admin_UR_STPD);
	}
	else if(l_MsgStr.find("UR GUAR", l_EndPos) != TString::npos)
	{
	//check to see UR GUAR 
		a_CMSMsg.SetMsgType(CMS_Admin_UR_GUAR);
	}
	else if(l_MsgStr.find("\nND\n", l_EndPos) != TString::npos ||
		l_MsgStr.find("\nND ", l_EndPos) != TString::npos)
	{
	//check to see UR GUAR 
		a_CMSMsg.SetMsgType(CMS_Admin_ND);
	}
	else
	{
	//UNKNOWN type 
		a_CMSMsg.SetMsgType(CMS_UNKNOWN);
	}

	return true;
}

bool CCMSSession::ParseStatusMsg(IncomingCMSMsg& a_CMSMsg)
{
	const TString&  l_MsgStr = a_CMSMsg.MsgStr();

	int l_StartPos, l_EndPos;

	l_StartPos = l_MsgStr.find("STATUS");
	if(l_StartPos == TString::npos)
		return false;

	l_StartPos += 6;

	try
	{
		if((l_EndPos = l_MsgStr.find("ORDER ACK",l_StartPos)) != TString::npos)
		{
			//order ack.
			a_CMSMsg.SetMsgType(CMS_Status_Order_ACK);

			//Get branch code and sequence number
			l_StartPos = l_MsgStr.find(GetBranchCode(),l_EndPos+9);
			l_EndPos = l_MsgStr.find(CMS_MSG_LINE_DELIMITOR, l_StartPos);
			a_CMSMsg.SetOrderID(l_MsgStr.substr(l_StartPos, l_EndPos - l_StartPos));
		}
		else if((l_EndPos = l_MsgStr.find("ADMIN ACK",l_StartPos)) != TString::npos)
		{
			//admin ack
			a_CMSMsg.SetMsgType(CMS_Status_Admin_ACK);
		}
		else if((l_EndPos = l_MsgStr.find("REJ",l_StartPos)) != TString::npos)
		{
			// order reject
			//get reason
			a_CMSMsg.SetMsgType(CMS_Status_OrderRej);
			l_StartPos = l_EndPos; 
			l_EndPos = l_MsgStr.find(CMS_MSG_LINE_DELIMITOR,l_StartPos); //go to the end of the line
			a_CMSMsg.SetText(l_MsgStr.substr(l_StartPos, l_EndPos - l_StartPos));

			//Get branch code and sequence number
			l_StartPos = l_MsgStr.find(GetBranchCode(),l_EndPos+1);
			l_EndPos = l_MsgStr.find(CMS_MSG_LINE_DELIMITOR, l_StartPos);
			a_CMSMsg.SetOrderID(l_MsgStr.substr(l_StartPos, l_EndPos - l_StartPos));

		}
		else if((l_EndPos = l_MsgStr.find("NUMBER GAP",l_StartPos)) != TString::npos)
		{
			//Number gap
			a_CMSMsg.SetMsgType(CMS_Status_NumberGap);
			l_EndPos = l_MsgStr.find(CMS_MSG_LINE_DELIMITOR,l_EndPos+10); //skip the rest of the line
			l_EndPos = l_MsgStr.find(CMS_MSG_LINE_DELIMITOR,l_EndPos+1); //skip next line

			//get expected number and received number
			l_EndPos = l_MsgStr.find(CMS_MSG_FIELD_DELIMITOR,l_EndPos+1); 

			long l_Expected, l_Received;
			sscanf(l_MsgStr.c_str()+l_EndPos,"%d %d",&l_Expected, &l_Received);
			a_CMSMsg.SetResendStart(l_Expected);
			a_CMSMsg.SetResendEnd(l_Received-1);
		}
		else if((l_EndPos = l_MsgStr.find("GOOD MORNING",l_StartPos)) != TString::npos)
		{
			a_CMSMsg.SetMsgType(CMS_LogonReply);
		}
		else if((l_EndPos = l_MsgStr.find("GOOD NIGHT",l_StartPos)) != TString::npos)
		{
			a_CMSMsg.SetMsgType(CMS_LogonReply);
		}
		else if((l_EndPos = l_MsgStr.find("CMS RESTART",l_StartPos)) != TString::npos)
		{
			a_CMSMsg.SetMsgType(CMS_Status_CMSRestart);
		}
		else if((l_EndPos = l_MsgStr.find("POSS LOSS",l_StartPos)) != TString::npos)
		{
			a_CMSMsg.SetMsgType(CMS_Status_PossLoss);			
		}
		else 
		{
			a_CMSMsg.SetMsgType(CMS_Status_FreeFormat);			
		}
	}
	catch (...)
	{
		return false;
	}
	return true;
}

bool CCMSSession::ParseReportMsg(IncomingCMSMsg& a_CMSMsg)
{
	try
	{
		//skip header line
		const TString&  l_MsgStr = a_CMSMsg.MsgStr();

		int l_StartPos, l_EndPos;

		//Get branch code and sequence number
		l_StartPos = l_MsgStr.find(GetBranchCode());
		l_EndPos = l_MsgStr.find(CMS_MSG_LINE_DELIMITOR, l_StartPos);

		a_CMSMsg.SetOrderID(l_MsgStr.substr(l_StartPos, l_EndPos - l_StartPos));

		//find BOT/SLD ans skip that line
		 l_StartPos = l_MsgStr.find("BOT", l_EndPos+1);
		 if(l_StartPos == TString::npos)
			 l_StartPos = l_MsgStr.find("SLD", l_EndPos+1);
		 if(l_StartPos == TString::npos)
		 {
			 return false;
		 }

		 l_StartPos = l_MsgStr.find(CMS_MSG_LINE_DELIMITOR, l_StartPos+3);
		l_StartPos++; //go pass the newline char

		//get last quantity
		int l_LastQty = -1;
		if(sscanf(l_MsgStr.c_str()+l_StartPos, "%d",&l_LastQty) != 1 || l_LastQty <=0)
			return false;
		a_CMSMsg.SetLastShares(l_LastQty);
		l_StartPos = l_MsgStr.find(CMS_MSG_FIELD_DELIMITOR,l_StartPos); //pass quantity
		l_StartPos++;

		//get symbol
		l_EndPos = l_MsgStr.find(CMS_MSG_FIELD_DELIMITOR,l_StartPos); //Get symbol
		a_CMSMsg.SetSymbol(l_MsgStr.substr(l_StartPos,l_EndPos-l_StartPos));

		l_StartPos = l_MsgStr.find(CMS_MSG_FIELD_DELIMITOR,l_EndPos+1); //pass month
		l_StartPos = l_MsgStr.find(CMS_MSG_FIELD_DELIMITOR,l_StartPos+1); //pass strike price
		

		if( (l_EndPos = l_MsgStr.find("/",l_StartPos)) != TString::npos ) //skip fractional part of strike price
		{
			l_StartPos = l_MsgStr.find(CMS_MSG_FIELD_DELIMITOR,l_EndPos); //pass fractional part
		}
				
		l_StartPos++;
		//get last price
		float l_LastPrice = -1.0;
		if(sscanf(l_MsgStr.c_str()+l_StartPos, "%f",&l_LastPrice) != 1)
			return false;
		a_CMSMsg.SetLastPrice(l_LastPrice);


		//check for lvs or fills
		if( (l_EndPos = l_MsgStr.find("LVS ",l_StartPos)) != TString::npos )
		{
			int l_Leaves = -1;
			if(sscanf(l_MsgStr.c_str()+l_EndPos+4, "%d",&l_Leaves) != 1 || l_LastQty <=0)
				return false;
			a_CMSMsg.SetLeaves(l_Leaves);
			a_CMSMsg.SetMsgType(CMS_Report_Partial);
		}
		else if(l_MsgStr.find("FILLS",l_StartPos) != TString::npos)
		{
			a_CMSMsg.SetMsgType(CMS_Report_Fill);
		}
	}
	catch (...)
	{
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
bool CCMSSession::BuildCxlStr(CCMSMsg* a_Order, TString& l_MsgBody,CCancel& a_Cancel)
{
	TString l_Temp = a_Order->CMSMsgPtr();
	int l_Pos = l_Temp.find("\n\n"); //get to the line after brance code and seq num
	l_Pos +=2;

	//append lien 2
	int l_PosEnd = l_Temp.find(CMS_MSG_LINE_DELIMITOR, l_Pos);
	if(l_Pos == TString::npos || l_PosEnd == TString::npos)
		return false;
	l_PosEnd++; //to include the line separator
	l_MsgBody.append(l_Temp.substr(l_Pos, l_PosEnd - l_Pos));

	//set the quantity and append the rest for line 3
	char l_CxlQty[16];
	itoa(a_Cancel.LeavesQty(),l_CxlQty,10);
	l_MsgBody.append(l_CxlQty);

	l_Pos = l_Temp.find(CMS_MSG_FIELD_DELIMITOR, l_PosEnd);
	l_PosEnd = l_Temp.find("\n\n", l_Pos) + 2;
	//find line 4
	l_PosEnd = l_Temp.find(CMS_MSG_LINE_DELIMITOR, l_PosEnd) + 1;
	l_MsgBody.append(l_Temp.substr(l_Pos, l_PosEnd - l_Pos));

	//append re: line (4A)
	l_MsgBody.append("RE ");

	l_MsgBody += a_Cancel.OrderTag();
	l_MsgBody += "/" + CTime::GetCurrentTime().Format("%m%d%y");

	//if it is CMTA, append to the line, other wise append to a new line
//	if(l_Temp.find("CMTA", l_PosEnd) != TString::npos)
//		l_MsgBody += CMS_MSG_FIELD_DELIMITOR;
//	else
		l_MsgBody += CMS_MSG_LINE_DELIMITOR;

	//find last field separator, and append
	l_Pos = l_PosEnd;
	l_PosEnd = l_Temp.rfind(CMS_MSG_LINE_DELIMITOR);
	if(l_PosEnd > l_Pos)
		l_MsgBody.append(l_Temp.substr(l_Pos, l_PosEnd+1 - l_Pos));

	return true;
}

bool CCMSSession::OnSetAccountType(const COrder& a_Order,TString& a_OutStr)
{
	TString l_Account = a_Order.ClearingAccount();
	switch(a_Order.Capacity())
	{
	
	case COrder::Agency:
		a_OutStr += "CUST \n\n";//3c, 3D is empty
		break;
	case COrder::Principal:
		a_OutStr += "FIRM \n\n";//3c, 3D is empty
		break;
	case COrder::Linkage:
		a_OutStr += "CUST \n\n";//3c, 3D is empty	
		l_Account = "";
		if(!OnSetLinkage(a_Order, l_Account))
			return false;
		break;
	case COrder::MarketMaker:
	case COrder::PrimaryMM:
		a_OutStr += "MM\n\n";//3c, 3D is empty
		l_Account = "MM-";
		l_Account.append(a_Order.MMAccount());
		break;
	case COrder::BrokerDealer:
		a_OutStr += "CUST BD\n\n";//3c, 3D is empty
		break;
	default:
		return false;
	}

	//line 4 account info
	a_OutStr.append(l_Account);
	a_OutStr += '\n';

	//CMTA line 4A/B
  	if(a_Order.CMTAAccount() != "")
	{
		a_OutStr.append("CMTA ");
		a_OutStr.append(a_Order.CMTAAccount());
		a_OutStr += '\n';
	}
	return true;

}
