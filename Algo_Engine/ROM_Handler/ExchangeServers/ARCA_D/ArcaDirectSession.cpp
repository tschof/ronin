/*******************************************************************************
*
* Copyright (c) 2005 by RONIN CAPITAL, LLC
*
* All Rights Reserved. 
*******************************************************************************/
/*
*  : Defines the class behaviors for the application.
*
*
* History
*
* date        user	       comment
* -------     --------    -----------------------------------------------------
*     Parnell     
*
*******************************************************************************/
#include "stdafx.h"
#include <sstream>

#include "ArcaDirect.h"

#include "ArcaDirectSession.h"
#include "DirectConnection.h"
#include "ClientMessages.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ArcaDirectSession::~ArcaDirectSession()
{
}

ArcaDirectSession::ArcaDirectSession()
:CExchangeSession(),m_sequenceSet(), m_rejLogonSeq(false)
{
    CMessageApp *pApp = (CMessageApp *)AfxGetApp();
    CString	FileName = pApp->Path() + pApp->Name() + ".DAT";
    // read values from exchange's DAT file
    m_InitiateLogon		= TRUE;//GetPrivateProfileInt("Fix Information",
    //  "Initiate Logon",	1,		FileName);
    GetPrivateProfileString("Session Information", "UserName",
        "RON01", m_Account.GetBuffer(128), 128, FileName);	m_Account.ReleaseBuffer();
    GetPrivateProfileString("Session Information", "Password", "",
        m_Password.GetBuffer(128), 128, FileName); m_Password.ReleaseBuffer();
    
    m_IncomingHeartBeatInterval = 15;
    m_firstRun = pApp->IsFirstRun();
    if (m_firstRun != TRUE) {
        m_loginSeqNum = m_sequenceSet.NextIn();
    } else {     
        //Reset the login seq to zero at the beginning of the day.
        m_loginSeqNum = 0;
    }
    m_dwLastMess = GetTickCount();
    // initialize
    ArcaDirectMessage::Init();
}

///////////////////////////////////////////////////////////////b///////////////////////////////////////////////
// outgoing event processing

BOOL ArcaDirectSession::Logon()
{
    
    if (m_firstRun != TRUE && m_rejLogonSeq == false) {
        m_loginSeqNum = m_sequenceSet.NextIn();
    } else if (m_firstRun != TRUE && m_rejLogonSeq == true){
        m_loginSeqNum = -1;
    }else {     
        //Reset the login seq to zero at the beginning of the day.
        m_loginSeqNum = 0;
    }
    ClientMessages::CreateLogonMessage(m_Account, m_loginSeqNum);
    return(SendMessage(ClientMessages::GetLogonMessage(), ArcaDirect::LOGON_LEN));
}



long ArcaDirectSession::ProcessEntry(COrder& Order)
{
    CMessageApp *pApp = (CMessageApp *)AfxGetApp();		
    /* Set the sequence in the order.*/
    Order.SetSequence(m_sequenceSet.GetNextOut());
    // generate a binary message
    ClientMessages::CreateOrderMessage(Order);
    if (SendMessage(ClientMessages::GetOrderMessage(), ArcaDirect::ORDER_LEN)) {
        return m_sequenceSet.NextOut();
    } else {
        return(FALSE);
    }
}

BOOL ArcaDirectSession::ProcessCancel(CCancel& Cancel)
{
    /* Set the sequence in the order.*/
    Cancel.SetSequence(m_sequenceSet.GetNextOut());
    //ArcaDirectCancel arcaCancel(Cancel);
    ClientMessages::CreateCancelMessage(Cancel);
    if (SendMessage(ClientMessages::GetCancelMessage(), ArcaDirect::CANCEL_LEN)) {     
        return(TRUE);
    }
    
    return(FALSE);
}

BOOL ArcaDirectSession::ProcessReplace(CReplace& Order)
{
    BOOL sucess = FALSE;
    /* Set the sequence in the order.*/
    Order.SetSequence(m_sequenceSet.GetNextOut());
    
    //ArcaDirectReplace replace(Order);
    ClientMessages::CreateReplaceMessage(Order);
    if(SendMessage(ClientMessages::GetReplaceMessage(), ArcaDirect::CANCEL_REP_LEN)) {     
        sucess = TRUE;
    }
    return sucess;
}

BOOL ArcaDirectSession::ProcessStatusRequest(CRequest& Order)
{
    return(FALSE);
}

//////////////////////////////////////////////////////
// incoming event processing

int ArcaDirectSession::ProcessMessage(std::stringstream& data,
                                      int length,
                                      CBaseConnection *pC)
{
    bool done = false;
    int usedLen = 0;
    int totalUsed =0;
    m_dwLastMess = GetTickCount();
    while(!done) {
        //char type = data.peek();
        if((totalUsed + 4) >= length) {
            return totalUsed;
        }

        char type;
        data.get(type);
        data.ignore(1);
        
        short len;
        data.read((char*)&len, sizeof(len));
        len = ntohs(len);

        if(len <= 0 || len > 150) {
            data.seekg(totalUsed, ios::beg);
            ((CMessageApp *)AfxGetApp())->ProcessTraffic("Garbage Data in the Length spot ", "!!!!!!!!");
        }
        
        switch(type) {
        case ArcaDirectMessage::Login:
            usedLen += len;
            if(usedLen <= length) {
                totalUsed = usedLen;
                //data.ignore(len - 4);
                ProcessLogon(data);
            } else {
                data.seekg(totalUsed, ios::beg);
            }
            break;
        case ArcaDirectMessage::LoginReject:
            usedLen += len;
            if(usedLen <= length) {
                totalUsed = usedLen;                
                ProcessLogonReject(data);
            } else {
                std::stringstream rejMes;
                rejMes <<"Message says it is this long "<<len<<" used len "<<usedLen<<" length passed in "<<length<<" ";
                ((CMessageApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,"Reject len problem. ", rejMes.str().c_str());
                data.seekg(totalUsed, ios::beg);
            }
            break;
        case ArcaDirectMessage::TestReq:
            usedLen += len;
            if(usedLen <= length) {
                totalUsed = usedLen;                
                data.ignore(len -4);
                ProcessTestRequest();
            }else {
                data.seekg(totalUsed, ios::beg);
            }
            break;
        case ArcaDirectMessage::HeartBeat:
            usedLen += len;
            if(usedLen <= length) {
                totalUsed = usedLen;                
                data.ignore(len -4);
                ProcessHeartBeat();
            } else {
                data.seekg(totalUsed, ios::beg);
            }
            break;
        case ArcaDirectMessage::OrderAck:
            usedLen += len;
            if(usedLen <= length) {
                totalUsed = usedLen;                
                ProcessAckMessage(data, COrder::Open);
            } else {
                data.seekg(totalUsed, ios::beg);
            }
            break;
        case ArcaDirectMessage::OrderFill:
            usedLen += len;
            if(usedLen <= length) {
                totalUsed = usedLen;                
                ProcessExecutionReport(data);
            } else {
                data.seekg(totalUsed, ios::beg);
            }
            break;
        case ArcaDirectMessage::CancelAck:
            usedLen += len;
            if(usedLen <= length) {
                totalUsed = usedLen;                
                ProcessAckMessage(data, COrder::PendingCancel);
            } else {
                data.seekg(totalUsed, ios::beg);
            }
            break;
        case ArcaDirectMessage::OrderKilled:
            usedLen += len;
            if(usedLen <= length) {
                totalUsed = usedLen;                
                ProcessOrderCancelled(data);
            } else {
                data.seekg(totalUsed, ios::beg);
            }
            break;
        case ArcaDirectMessage::ReplaceAck:
            usedLen += len;
            if(usedLen <= length) {
                totalUsed = usedLen;                
                ProcessAckMessage(data, COrder::ReplacePending);
            } else {
                data.seekg(totalUsed, ios::beg);
            }
            break;
        case ArcaDirectMessage::ReplacedMess:
            usedLen += len;
            if(usedLen <= length) {
                totalUsed = usedLen;                
                ProcessOrderReplaced(data);
            } else {
                data.seekg(totalUsed, ios::beg);
            }
            break;
        case ArcaDirectMessage::Reject:
            usedLen += len;
            if(usedLen <= length) {
                totalUsed = usedLen;                
                ProcessReject(data);
            } else {
                data.seekg(totalUsed, ios::beg);
            }
            break;
        case ArcaDirectMessage::BustOrCorrect:
            usedLen += len;
            if(usedLen <= length) {
                totalUsed = usedLen;                
                ProcessBust(data);
            } else {
                data.seekg(totalUsed, ios::beg);
            }
            break;
        default:
            {
                totalUsed = length;
                done = true;
                std::string message("Type: ");
                message.append(1,type);
                message.append(" ");
               // ((CMessageApp *)AfxGetApp())->ProcessErrorEx(MODE_ERR_DEVELOPER,"UNABLE TO PARSE MESSAGE LOGGING OFF. ",
                //    message.c_str());
                  ((CMessageApp *)AfxGetApp())->ProcessTraffic("UNABLE TO PARSE MESSAGE LOGGING OFF. ",
                    message.c_str());


            }
            
        }
        if(usedLen >= length) {
            done = true;
        }
    }
    return totalUsed;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// order related SEQUENCED message processing


void ArcaDirectSession::ProcessExecutionReport(std::stringstream& message)
{
    CMessageApp	*pApp = (CMessageApp *)AfxGetApp();	
    CExecution exe;
    exe.SetStatus(COrder::Filled);
    memset(m_clientID, '\0',ArcaDirect::ID_STRING_LEN);
    memset(m_arcaID, '\0',ArcaDirect::ID_STRING_LEN);
    int seq = ArcaDirect::ProcessOrderFilledMessage(message,
        exe, m_clientID,
        m_arcaID);
    SetNextIncomingSeqNum(seq);
    pApp->Handler().ProcessMessage(exe);
}

void ArcaDirectSession::ProcessOrderCancelled(std::stringstream& message) 
{
    CMessageApp	*pApp = (CMessageApp *)AfxGetApp();	
    CExecution exe;
    exe.SetStatus(COrder::Canceled);
    memset(m_clientID, '\0',ArcaDirect::ID_STRING_LEN);
    memset(m_arcaID, '\0',ArcaDirect::ID_STRING_LEN);
    int seq = ArcaDirect::ProcessMessage(message,
        exe, m_clientID, m_arcaID);
    SetNextIncomingSeqNum(seq);    
    pApp->Handler().ProcessMessage(exe);
}

void ArcaDirectSession::ProcessReject(std::stringstream& message)
{
    CMessageApp	*pApp = (CMessageApp *)AfxGetApp();	
    memset(m_reason, '\0', ArcaDirect::REJECT_REASON_LEN);
    memset(m_clientID, '\0',ArcaDirect::ID_STRING_LEN);
    memset(m_arcaID, '\0',ArcaDirect::ID_STRING_LEN);
    int loc = message.tellg();
    //message.seekg(loc + 20);
    message.seekg(loc + 16);
    char type = message.peek();
    message.seekg(loc);
    if(type == '1') {
        CReject reject;
        ArcaDirect::ProcessRejectMessage(message,
            reject, m_clientID,
            m_arcaID, m_reason, type);
        reject.SetReason(m_reason);
        pApp->Handler().ProcessMessage(reject);
    } else if('2' == type) {
        CCancelReject reject;
        ArcaDirect::ProcessRejectMessage(message,
            reject, m_clientID,
            m_arcaID, m_reason, type);
        reject.SetReason(m_reason);
        pApp->Handler().ProcessMessage(reject);
    } else if('3' == type) {
        CReplaceReject reject;
        ArcaDirect::ProcessRejectMessage(message,
            reject, m_clientID,
            m_arcaID, m_reason, type);
        reject.SetReason(m_reason);
        pApp->Handler().ProcessMessage(reject);
    } 
}

void ArcaDirectSession::ProcessOrderReplaced(std::stringstream& message)
{
    CMessageApp	*pApp = (CMessageApp *)AfxGetApp();	
    CExecution exe;
    exe.SetStatus(COrder::Replaced);
    memset(m_clientID, '\0',ArcaDirect::ID_STRING_LEN);
    memset(m_arcaID, '\0',ArcaDirect::ID_STRING_LEN);
    
    int seq = ArcaDirect::ProcessMessage(message,
        exe, m_clientID, m_arcaID);
    SetNextIncomingSeqNum(seq);    
    pApp->Handler().ProcessMessage(exe);
}

void ArcaDirectSession::ProcessAckMessage(std::stringstream& message,
                                          int status) 
{
    CMessageApp	*pApp = (CMessageApp *)AfxGetApp();	
    CExecution exe;
    exe.SetStatus(status);
    memset(m_clientID, '\0',ArcaDirect::ID_STRING_LEN);
    memset(m_arcaID, '\0',ArcaDirect::ID_STRING_LEN);
    
    int seq = ArcaDirect::ProcessMessage(message, exe,
        m_clientID, m_arcaID);
    SetNextIncomingSeqNum(seq);    
    pApp->Handler().ProcessMessage(exe);
}

void ArcaDirectSession::ProcessBust(std::stringstream& stream)
{
    CString l_MsgStr;
    ArcaDirect::ProcessBustOrCorrect(stream, l_MsgStr);    
    ((CMessageApp*)AfxGetApp())->ProcessNotifyEx(MODE_ERR_BUSINESS,"Busted Trade", l_MsgStr);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// admin message processing

void ArcaDirectSession::ProcessHeartBeat()
{
    //ArcaDirectHeartbeat Beat;
   // ClientMessages::CreateHeartbeatMessage();
   // SendMessage(ClientMessages::GetHBMessage(), ArcaDirect::HEARTB_LEN);
}

void ArcaDirectSession::ProcessLogonReject(std::stringstream& stream)
{
    m_firstRun = false;
    int lastSeq;
    stream.read((char*)&lastSeq, sizeof(lastSeq));
    lastSeq = ntohl(lastSeq);
    //SetNextOutgoingSeqNum(lastSeq);
    int lastSeqSent;
    stream.read((char*)&lastSeqSent, sizeof(lastSeqSent));
    lastSeqSent = ntohl(lastSeqSent);
    SetNextIncomingSeqNum(lastSeqSent);
    short rejectCode;
    stream.read((char*)&rejectCode, sizeof(rejectCode));
    rejectCode = ntohs(rejectCode);
    CString errorMessage("Login rejected by ArcaDirect: ");
    CString seqNums;
    switch(rejectCode) {
    case 1:
        errorMessage += "System unavailable: ";
        break;
    case 2:
        errorMessage += "Invalid Sequence Number: Last Number Server Recieved = ";
        itoa(lastSeq, seqNums.GetBuffer(100), 10);
        seqNums.ReleaseBuffer();
        errorMessage += seqNums;
        seqNums.Empty();
        itoa(lastSeqSent, seqNums.GetBuffer(100), 10);
        errorMessage += " The last number the server sent was ";
        seqNums.ReleaseBuffer();
        errorMessage += seqNums;
        errorMessage += " :  ";
        m_rejLogonSeq = true;
        break;
    case 3:
        errorMessage += "Invalid Series index value: ";
        break;
    case 4:
        errorMessage += "Logon was attempted more than onece. ";
        break;
    case 5:
        errorMessage += "Session has been disabled during the day. ";
        break;
    case 6:
        errorMessage += "The configured Connection type of the session does not match the port the client connects to. ";
        break;    
    }
    char buffer[41];
    memset(buffer, '\0', 41);
    stream.read(buffer, 40);
    errorMessage += buffer;
    stream.ignore(2);
    CMessageApp	*pApp = (CMessageApp *)AfxGetApp();
    pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,pApp->Name(), errorMessage);
    
}

void ArcaDirectSession::ProcessTestRequest()
{
    //    ArcaDirectHeartbeat Beat;
    //	SendMessage(Beat);
    ClientMessages::CreateHeartbeatMessage();
    SendMessage(ClientMessages::GetHBMessage(), ArcaDirect::HEARTB_LEN);
}
BOOL ArcaDirectSession::Logout()
{
    CArcaDirectApp	*pApp = (CArcaDirectApp *)AfxGetApp();	
    
    pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,pApp->Name(),
        "Logged Out Of ARCA DIRECT.");
    
    pApp->GetDirect()->SetLogonStatus(FALSE);//pApp->Host().SetLogonStatus(FALSE);//
    pApp->GetDirect()->Close();//Host().Close();//
    pApp->UpdateHost();
    
    pApp->SetStatusTimer(15000);
    return TRUE;
}

void ArcaDirectSession::ProcessLogon(std::stringstream& stream)
{
    CArcaDirectApp	*pApp = (CArcaDirectApp *)AfxGetApp();	
    
    pApp->ProcessLogEntry(pApp->Name(), "Welcome To ARCADIRECT");


    pApp->SetStatusTimer(15000);
    m_rejLogonSeq = false;
    m_firstRun = false;

    int lastSeq;
    stream.read((char*)&lastSeq, sizeof(lastSeq));
    lastSeq = ntohl(lastSeq);

    int ourLast = NextOutgoingSeqNum();
    if(lastSeq < ourLast) {
        std::stringstream debug;
        debug <<"This is the last seq number ArcaReceived: "<<lastSeq<<" This was the last Seq number we sent: "<<ourLast<<"  ";
        
        pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,pApp->Name(),
        debug.str().c_str());
    }
    
    stream.ignore(8);
    
    if (m_InitiateLogon)
    {
        pApp->GetDirect()->SetLogonStatus(TRUE);//.SetLogonStatus(TRUE);//
        pApp->UpdateHost();
    }
}

BOOL ArcaDirectSession::SendMessage(const char* message, int length)
{
    CArcaDirectApp	*pApp = (CArcaDirectApp *)AfxGetApp();
    return pApp->GetDirect()->SendDirectMessage(
        message, length);
}

void ArcaDirectSession::ProcessTimer()
{
    CTimeSpan	TimeDiff;
    CTime		CurrentTime = CTime::GetCurrentTime();
    CArcaDirectApp	*pApp = (CArcaDirectApp *)AfxGetApp();
    DirectConnection* pHost = pApp->GetDirect();
    //CHostConnection* pHost = (CHostConnection*)&pApp->Host();
    int StartTime, EndTime;
    
    StartTime = GetPrivateProfileInt("Runtime Information",
        "Start Time",6, pApp->Path() + pApp->Name() + ".DAT");
    EndTime = GetPrivateProfileInt("Runtime Information",
        "End Time", 6, pApp->Path() + pApp->Name() + ".DAT");
    
    int HM = 100 * CurrentTime.GetHour() + CurrentTime.GetMinute();
    
    if(StartTime == EndTime)
    {
        if(HM == StartTime)
        {
            pApp->SetCloseDisplayMsg(FALSE);
            Sleep(1);
            pHost->Close();
            AfxGetMainWnd()->PostMessage(WM_CLOSE);
            return;
        }
    }
    else if(StartTime > EndTime)
    {
        if ((HM >= EndTime) && (HM < StartTime))
        {
            pApp->SetCloseDisplayMsg(FALSE);
            Sleep(1);
            pHost->Close();
            AfxGetMainWnd()->PostMessage(WM_CLOSE);
            return;
        }
    }
    else
    {
        if ((HM < StartTime) || (HM >= EndTime))
        {
            pApp->SetCloseDisplayMsg(FALSE);
            Sleep(1);
            pHost->Close();
            AfxGetMainWnd()->PostMessage(WM_CLOSE);
            return;
        }
    }
    
    if ((pHost->Reconnect()) && (!(pHost->m_Connected)))	// not connected
    {
        pApp->SetStatusTimer(15000);
       // pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,pApp->Name(),
            //"ArcaDirect Connection To Host Lost");
        //	pApp->Handler().ProcessMessage("DEAD");      
        pApp->RecycleHost();				
    }
    else if ((pHost->Reconnect()) && (!(pHost->LoggedOn())))	// not logged on
    {
        pApp->SetStatusTimer(200);
        //pApp->Handler().ProcessMessage("DEAD");
       // pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,pApp->Name(),
       //     "ArcaDirect Connected To Host But Not Logged On");
        Logon();
    }
    else	// connection is good so just check for heartbeats and test requests
    {
        DWORD interval = GetTickCount() - m_dwLastMess;
        if(interval <= 30000) {
            ClientMessages::CreateTestRequestMessage();
            SendMessage(ClientMessages::GetTestReqMessage(), ArcaDirect::TEST_LEN);
            
        } else {
            Logout();
        }
    }
}

/*!
This will have to be mutex protected if
we ever change to a multithreaded design.
The other design desicsion that this assumes is that
this application will not run for more than 24 hours at a 
streach. We would have to add date checking that would reset 
the m_clientOrderID after the close of Arca (3:30 CST) 
if we wanted to support this.
*/
int ArcaDirectSession::GetClientOrderId(char idStr[12]) 
{
    int id = m_sequenceSet.GetNextMsgNum();
    itoa(id, idStr, 10);  
    return id;
}

void ArcaDirectSession::SetClientOrderID(int id)
{
    m_sequenceSet.SetNextMsgNum(id);
}

void ArcaDirectSession::SetNextIncomingSeqNum(int seq)
{
    m_sequenceSet.SetNextIn(seq);
}

void ArcaDirectSession::SetNextOutgoingSeqNum(int seq)
{
    m_sequenceSet.SetNextOut(seq);
}

int ArcaDirectSession::GetNextIncomingSeqNum()
{
    return m_sequenceSet.GetNextIn();
}


int ArcaDirectSession::GetNextOutgoingSeqNum()
{
    return m_sequenceSet.GetNextOut();
}

int ArcaDirectSession::NextIncomingSeqNum()
{
    return m_sequenceSet.NextIn();
}

int ArcaDirectSession::NextOutgoingSeqNum()
{
    return m_sequenceSet.NextOut();
}
