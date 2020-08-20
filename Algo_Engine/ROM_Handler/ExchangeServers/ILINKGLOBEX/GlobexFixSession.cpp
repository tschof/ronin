/*******************************************************************************
*
* Copyright (c) 2005 by RONIN CAPITAL, LLC
*
* All Rights Reserved. 
*******************************************************************************/
/*
*  
*
*
* History
*
* date        user	       comment
* -------     --------    -----------------------------------------------------
*
*******************************************************************************/


#include "stdafx.h"
#include "iLinkGlobex.h"
#include "OrderHandler.h"
#include "GlobexFixSession.h"
#include "FixAdminMsgs.h"
#include "FixAppMsgs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Fix 4.2 Tags
const EFixFieldID CGlobexFixSession::FIX_42_ExpireDate			= (EFixFieldID) 432;

// CME Custom Tags
const EFixFieldID CGlobexFixSession::FIX_CME_CustomerTypeCode = (EFixFieldID) 9702;
const EFixFieldID CGlobexFixSession::FIX_CME_FeeBilling	= (EFixFieldID) 9706;
const EFixFieldID CGlobexFixSession::FIX_CME_LoginRouteId = (EFixFieldID) 9716;
const EFixFieldID CGlobexFixSession::FIX_CME_CorrelationOrdId = (EFixFieldID) 9717;
const EFixFieldID CGlobexFixSession::FIX_CME_NextExpectedMsgSeqNum = (EFixFieldID)789;
const EFixFieldID CGlobexFixSession::FIX_CME_OrderQtyFlag = (EFixFieldID)9768;

//**************************************************************************
// 
//****************************************************************************
CGlobexFixSession::CGlobexFixSession()
{
    //	int senderCount;
    
    CMessageApp* pApp = (CMessageApp*) AfxGetApp();
    
    CString fileName = pApp->DatFile();
    
    //TString buffer;
    //buffer.resize(129);
    
    
    GetPrivateProfileString("Fix Information", "Firm Name", "", m_firmName.GetBuffer(128),
        128, (LPCTSTR)fileName);
    m_firmName.ReleaseBuffer();
    
    GetPrivateProfileString("Fix Information", "Firm ID", "", m_firmId.GetBuffer(128),
        128, (LPCTSTR)fileName);
    m_firmId.ReleaseBuffer();
    
    GetPrivateProfileString("Fix Information", "FTI", "", m_fti.GetBuffer(128),
        128, (LPCTSTR)fileName);
    m_fti.ReleaseBuffer();
    
    GetPrivateProfileString("Fix Information", "TargetId", "", m_targetId.GetBuffer(128),
        128, (LPCTSTR)fileName);
    m_targetId.ReleaseBuffer();
    
    GetPrivateProfileString("Fix Information", "TargetSubId", "", m_targetSubId.GetBuffer(128),
        128, (LPCTSTR)fileName);
    m_targetSubId.ReleaseBuffer();
    
    GetPrivateProfileString("Fix Information", "Session ID", "", m_sessionId.GetBuffer(128),
        128, (LPCTSTR)fileName);
    m_sessionId.ReleaseBuffer();
    
    GetPrivateProfileString("Fix Information", "Password", "", m_password.GetBuffer(128),
        128, (LPCTSTR)fileName);
    m_password.ReleaseBuffer();
    
    GetPrivateProfileString("Fix Information", "SenderSubId", "", m_SenderSubId.GetBuffer(128),
        128, (LPCTSTR)fileName);
    m_SenderSubId.ReleaseBuffer();
    
    GetPrivateProfileString("Fix Information", "SenderLocationID", "", m_SenderLocationId.GetBuffer(128),
        128, (LPCTSTR)fileName);
    m_SenderLocationId.ReleaseBuffer();
    
    
    
    m_SenderCompId += m_sessionId;
   // m_SenderCompId += m_firmId;
   // m_SenderCompId += m_fti;
    
    
    //////////////////////////////////////////////////////////////////////////////////////////////
    
    
    
    GetPrivateProfileString("Clearing Information", "CustomerTypeCode", "",
        m_CustomerTypeCode.GetBuffer(128),
        128, (LPCTSTR)fileName);
    m_CustomerTypeCode.ReleaseBuffer();
    
    GetPrivateProfileString("Clearing Information", "FeeBilling", "", m_FeeBilling.GetBuffer(128),
        128, (LPCTSTR)fileName);
    m_FeeBilling.ReleaseBuffer();
    
    GetPrivateProfileString("Clearing Information", "CustomerOrFirm", "", m_CustomerOrFirm.GetBuffer(128),
        128, (LPCTSTR)fileName);
    m_CustomerOrFirm.ReleaseBuffer();
    
}

BOOL CGlobexFixSession::Logout()
{
    ProcessLogEntry("Sending logout.");
    CFixLogout l_Logout(m_OutgoingMsgs.NextMsgSeqNum(), GetCurrentTimeStr());
    l_Logout.TargetSubID(FIX_ADMIN_TARGET_SUB_ID);
    // This is only for a GTD order
    CFixField* pNextExpectedMsgSeqNum = l_Logout.GuaranteedGetCustomField(FIX_CME_NextExpectedMsgSeqNum);
    
    if (pNextExpectedMsgSeqNum) {
        pNextExpectedMsgSeqNum->Value(m_IncomingMsgs.NextMsgSeqNum());
    }
    
    LogoutOverride(l_Logout);
    
    if(SendMsg(l_Logout))
    {
        //m_LogoutInitiated = true;
        return TRUE;
    }
    
    //Shutdown();
    return FALSE;
}


void CGlobexFixSession::Process(const CFixLogout& a_Logout)
{
    if(!AddIncomingMsgToSequence(a_Logout))
    {
        return;
    }
    
    TOStrStream l_LogMsg;
    l_LogMsg << "Logout received -- Text: " << a_Logout.Text();
    ProcessLogEntry(l_LogMsg.str().c_str());
    int a_MsgSeqNum = a_Logout.GetField((EFixFieldID)FIX_CME_NextExpectedMsgSeqNum)->ValueAsInt();
    
    m_IncomingMsgs.NextMsgSeqNum(a_MsgSeqNum);
    
    if(!m_LogoutInitiated)
    {
        ProcessLogEntry("Sending logout acknowledgement.");
        CFixLogout l_Logout(m_OutgoingMsgs.NextMsgSeqNum(), GetCurrentTimeStr());
        l_Logout.TargetSubID(FIX_ADMIN_TARGET_SUB_ID);
        SendMsg(l_Logout);
    }
    else
    {
        m_LogoutInitiated = false;
        Shutdown();
    }
}

//**************************************************************************
// 
//****************************************************************************
CGlobexFixSession::~CGlobexFixSession()
{
    m_SenderToSubMap.RemoveAll();
}

//**************************************************************************
// 
//****************************************************************************
void CGlobexFixSession::SetSenderSubId(CFixMsg& fixMsg, CString& senderCompId)
{
    CString senderSubId;
    
    m_SenderToSubMap.Lookup(senderCompId, senderSubId);
    
    fixMsg.SenderSubID(senderSubId);
}

//**************************************************************************
// 
//****************************************************************************
BOOL CGlobexFixSession::AddFix42HeaderFields(CFixMsg& fixMsg, const char* trader,
                                             const CString& clearingId)
{
    CString tempSenderComp;
    tempSenderComp += m_SenderCompId;
    tempSenderComp += clearingId;
    tempSenderComp += m_fti;
    fixMsg.SenderCompID(tempSenderComp);
    fixMsg.SenderSubID(trader);
    fixMsg.TargetSubID(m_targetSubId);
    fixMsg.SetTargetCompID(m_targetId);
    fixMsg.SenderLocationID(m_SenderLocationId);
    
    //SetSenderSubId(fixMsg, senderCompId);
    
    return TRUE;
}

//**************************************************************************
// 
//****************************************************************************
BOOL CGlobexFixSession::AddFixAdminMsgHeader(CFixMsg& fixMsg, const char* trader,
                                             const CString& clearingId)
{
    return AddFix42HeaderFields(fixMsg, trader, clearingId);//, senderCompId);
}

//**************************************************************************
// 
//****************************************************************************
BOOL CGlobexFixSession::AddFixApplicationMsgHeader(CFixMsg& fixMsg, const char* trader,
                                                   const CString& clearingId)
{
    return AddFix42HeaderFields(fixMsg, trader, clearingId);
}

//**************************************************************************
// 
//****************************************************************************
void CGlobexFixSession::AddFix42OrderFields(const COrder& order, CFixOrder& fixOrder)
{
    int orderType = order.Type();
    
    fixOrder.Account(order.ClearingAccount());
    
    if (orderType == COrder::Limit)
    {
        fixOrder.Price(fixOrder.Price());
    }
    fixOrder.SenderSubID(order.TradeFor());
    fixOrder.TransactTime(fixOrder.SendingTime().c_str());
    
    fixOrder.SecurityType(_T("FUT"));
    
    fixOrder.CustomerOrFirm(atoi(m_CustomerOrFirm));
    
    fixOrder.SecurityExchange(_T("2"));
    
    fixOrder.SecurityDesc(order.Symbol());
    
    CFixField* pCorealationOrdID = fixOrder.GuaranteedGetCustomField(FIX_CME_CorrelationOrdId);
    
    if (pCorealationOrdID)
    {
        pCorealationOrdID->Value(order.OrderTag());
    }
    
    
    if (order.TIF() == COrder::GoodTillDate)
    {
        // This is only for a GTD order
        CFixField* pExpireDate = fixOrder.GuaranteedGetCustomField(FIX_42_ExpireDate);
        
        if (pExpireDate)
        {
            CTime currentTime = CTime::GetCurrentTime();
            CString expireDate;
            expireDate.Format("%s", currentTime.Format("%Y%m%d"));	
            pExpireDate->Value(expireDate);
        }
    } else if(order.TIF() == COrder::IC) {
        CFixField* pMinQty = fixOrder.GuaranteedGetCustomField(FIX_MinQty);
        if(pMinQty) {
            pMinQty->Value(fixOrder.OrderQty());
        }
        
    }
    
    // This is only for a GTD order
    CFixField* pCustomerType = fixOrder.GuaranteedGetCustomField(FIX_CME_CustomerTypeCode);
    
    if (pCustomerType)
    {
        pCustomerType->Value(m_CustomerTypeCode);
    }
    
    CFixField* pFeeBilling = fixOrder.GuaranteedGetCustomField(FIX_CME_FeeBilling);
    
    if (pFeeBilling)
    {
        pFeeBilling->Value(m_FeeBilling);
    }
}




long CGlobexFixSession::ProcessEntry(COrder& a_Order)
{
    if(m_AwaitingResend)
    {
        ((CMessageApp*) AfxGetApp())->ProcessErrorEx(MODE_ERR_NOTIFY | MODE_ERR_DEVELOPER, ((CMessageApp*) AfxGetApp())->Name() + ":FIX Session", "Can't process outgoing orders until fix session is corrected.");
        a_Order.SetInstructions("Fix session out of sequence:Can't send order.");
        return 0;
    }
    
    long l_SeqNum = m_OutgoingMsgs.NextMsgSeqNum();
    
    CString clientOrderId = a_Order.OrderTag();
    
    CFixOrder l_Order(
        l_SeqNum,
        GetCurrentTimeStr(),
        clientOrderId,
        FIX_AutoPublic, //Handling Instructions (Automatic or Manual hndling of order)
        a_Order.Symbol(),
        (EFixSide) m_Translator.m_ToExchangeSide[a_Order.Side()],
        (EFixOrdType) m_Translator.m_ToExchangeType[a_Order.Type()]
        );
    
    if (a_Order.Type() == COrder::Stop)
    {
        l_Order.StopPx(a_Order.StopPrice());
        //	l_Order.Price(0);
    }
    else if (a_Order.Type() == COrder::StopLimit)
    {
        l_Order.StopPx(a_Order.StopPrice());
        l_Order.Price(a_Order.dPrice());
    }
    else if (a_Order.Type() != COrder::Market 
        && a_Order.Type() != COrder::MarketOnClose 
        && a_Order.Type() != COrder::MarketAtOpen
        && a_Order.Type() != COrder::MarketLimit)
    {
        l_Order.Price(a_Order.dPrice());
    }
    
    if (strcmp(TargetSubId().c_str(), "") != 0)
    {
        l_Order.TargetSubID(m_TargetSubId.c_str());
    }
    
    TString senderSubId = SenderSubId();
    if ( senderSubId.length() > 0 )
        l_Order.SenderSubID((const char*)senderSubId.c_str());
    else
        l_Order.SenderSubID(a_Order.SourceId());
    // }
    l_Order.OrderQty(a_Order.Shares());
    
    l_Order.TimeInForce((EFixTimeInForce) m_Translator.m_ToExchangeTIF[a_Order.TIF()]);
    
    
    if (a_Order.ExecInst() != COrder::LastExecInst)
    {
        char ExecInst[2];
        sprintf(ExecInst, "%c", a_Order.ExecInst());
        l_Order.ExecInst(ExecInst);
    }
    
    if ((a_Order.ExpTime()).IsEmpty() != TRUE)
    {
        l_Order.ExpireTime((LPCTSTR)a_Order.ExpTime());
    }
    
    if(a_Order.SecurityType() != COrder::Option)
    {
        l_Order.Rule80A(a_Order.Capacity());
    }
    
    if (a_Order.Floor() != 0)
    {
        l_Order.MaxFloor(a_Order.Floor());
    }
    
    if(a_Order.ExDestination() != "")
        l_Order.ExDestination(a_Order.ExDestination());
    
    try
    {
        if(OrderEntryOverride(a_Order, l_Order) )
            return SendMsg(l_Order);
        else
        {
            a_Order.SetInstructions("FixSession:Account type / Order type not correct.");
            return 0;
        }
    }
    catch (const char *Message)
    {
        a_Order.SetInstructions(Message);
        return FALSE;
    }
}
//**************************************************************************
// 
//****************************************************************************
void CGlobexFixSession::AddFix42CancelRequestFields(const CCancel& cancelRequest,
                                                    CFixCancel& fixCancelRequest)
{
    fixCancelRequest.TransactTime(fixCancelRequest.SendingTime().c_str());
    fixCancelRequest.Account(cancelRequest.ClearingAccount());
    fixCancelRequest.SecurityType(_T("FUT"));
    fixCancelRequest.SenderSubID(cancelRequest.Trader());
    CFixField* pSecurityeExchange = fixCancelRequest.GuaranteedGetCustomField(FIX_SecurityExchange);
    
    if (pSecurityeExchange)
    {
        pSecurityeExchange->Value(_T("2"));
    }
    
    CFixField* pCorealationOrdID = fixCancelRequest.GuaranteedGetCustomField(FIX_CME_CorrelationOrdId);
    
    if (pCorealationOrdID)
    {
        pCorealationOrdID->Value(cancelRequest.OrderTag());
    }
    
    CFixField* pSecDesc = fixCancelRequest.GuaranteedGetCustomField(FIX_SecurityDesc);
    
    if (pSecDesc)
    {
        pSecDesc->Value(cancelRequest.Symbol());
    }
}

//**************************************************************************
// 
//****************************************************************************
BOOL CGlobexFixSession::CancelOverride(const CCancel& cancelRequest,
                                       CFixCancel& fixCancelRequest)
{
    CString tradeFor;
   ((CILinkGlobexApp *)AfxGetApp())->GetHandler().LoadTradeFor(
        tradeFor, cancelRequest.StaffordTag());
    AddFixApplicationMsgHeader(fixCancelRequest, cancelRequest.Trader(),
        cancelRequest.ClearingId());
    
    AddFix42CancelRequestFields(cancelRequest, fixCancelRequest);
    
    return TRUE;
}

BOOL CGlobexFixSession::ReplaceOverride(const CReplace &a_Replace,
                                        CFixReplace& a_FixReplace)
{

    CString tradeFor;
   ((CILinkGlobexApp *)AfxGetApp())->GetHandler().LoadTradeFor(
        tradeFor, a_Replace.StaffordTag());
    AddFixApplicationMsgHeader(a_FixReplace, a_Replace.Trader(),
        a_Replace.ClearingId());
    
    AddFix42ReplaceRequestFields(a_Replace, a_FixReplace);
    
    return TRUE;
}


void CGlobexFixSession::AddFix42ReplaceRequestFields(const CReplace& replace,
                                                     CFixReplace& fixReplace)
{
    fixReplace.TransactTime(fixReplace.SendingTime().c_str());
    
    fixReplace.SecurityType(_T("FUT"));
    fixReplace.SenderSubID(replace.Trader());
    CFixField* pSecurityeExchange = fixReplace.GuaranteedGetCustomField(FIX_SecurityExchange);
    
    if (pSecurityeExchange)
    {
        pSecurityeExchange->Value(_T("2"));
    }
    
    
    CFixField* pOrderQtyFlag = fixReplace.GuaranteedGetCustomField(FIX_CME_OrderQtyFlag);
    
    if (pOrderQtyFlag)
    {
        pOrderQtyFlag->Value(_T("Y"));
    }
    // This is only for a GTD order
    CFixField* pCustomerType = fixReplace.GuaranteedGetCustomField(FIX_CME_CustomerTypeCode);
    
    if (pCustomerType)
    {
        pCustomerType->Value(m_CustomerTypeCode);
    }
    
    CFixField* pCorealationOrdID = fixReplace.GuaranteedGetCustomField(FIX_CME_CorrelationOrdId);
    
    if (pCorealationOrdID)
    {
        pCorealationOrdID->Value(replace.OrderTag());
    }
    fixReplace.CustomerOrFirm(atoi(m_CustomerOrFirm)); 
    
    fixReplace.SecurityDesc(replace.Symbol());
    
    CFixField* pExchangeOrdID = fixReplace.GuaranteedGetCustomField(FIX_OrderID);
    
    if (pExchangeOrdID)
    {
        pExchangeOrdID->Value(replace.ExchangeTag());
    }
    fixReplace.Account(replace.ClearingAccount());
    
}

BOOL CGlobexFixSession::ProcessReplace(CReplace& a_Replace)
{
    if(m_AwaitingResend)
    {
        ((CMessageApp*) AfxGetApp())->ProcessErrorEx(MODE_ERR_NOTIFY | MODE_ERR_DEVELOPER, ((CMessageApp*) AfxGetApp())->Name() + ":FIX Session", "Can't process outgoing replace until fix session is corrected.");
        a_Replace.SetInstructions("Fix session out of sequence:Can't replace order.");
        return FALSE;
    }
    
    CFixReplace l_Replace(
        m_OutgoingMsgs.NextMsgSeqNum(),
        GetCurrentTimeStr(),
        a_Replace.OrderTag(),
        a_Replace.CancelTag(),
        FIX_AutoPrivate,
        a_Replace.Symbol(),
        (EFixSide) m_Translator.m_ToExchangeSide[a_Replace.Side()],
        (EFixOrdType) m_Translator.m_ToExchangeType[a_Replace.Type()]
        );
    
    if (strcmp(TargetSubId().c_str(), "") != 0)
    {
        l_Replace.TargetSubID(m_TargetSubId.c_str());
    }
    
    TString senderSubId = SenderSubId();
    if ( senderSubId.length() > 0 )
        l_Replace.SenderSubID((const char*)senderSubId.c_str());
    else if (strcmp(a_Replace.SourceId(), "") != 0)
        l_Replace.SenderSubID(a_Replace.SourceId());
    // }
    
    
    if (a_Replace.ExecInst() != COrder::LastExecInst)
    {
        char ExecInst[2];
        sprintf(ExecInst, "%c", a_Replace.ExecInst());
        l_Replace.ExecInst(ExecInst);
    }
    
    l_Replace.TimeInForce((EFixTimeInForce) m_Translator.m_ToExchangeTIF[a_Replace.TIF()]);
    
    l_Replace.OrderQty(a_Replace.Shares());
    
    if (a_Replace.Type() != COrder::Stop &&
        a_Replace.Type() != COrder::MarketLimit)
    {
        l_Replace.Price(a_Replace.Price());
    }
    
    if (a_Replace.Type() == COrder::StopLimit ||
        a_Replace.Type() == COrder::Stop)
    {
        l_Replace.StopPx(a_Replace.StopPrice());
    }
    
    if (a_Replace.Floor() != 0)
    {
        l_Replace.MaxFloor(a_Replace.Floor());
    }
    if(a_Replace.TIF() == COrder::IC) {
        CFixField* pMinQty = l_Replace.GuaranteedGetCustomField(FIX_MinQty);
        if(pMinQty) {
            pMinQty->Value(l_Replace.OrderQty());
        }
    }
    try
    {
        if(ReplaceOverride(a_Replace, l_Replace))
            return SendMsg(l_Replace);
        else
        {
            a_Replace.SetInstructions("FixSession:Account type / Order type not correct.");
            return FALSE;
        }
    }
    catch (const char *Message)
    {
        a_Replace.SetInstructions(Message);
        return FALSE;
    }
}

void CGlobexFixSession::Process(const CFixCancelReject& a_Reject)
{
    if(!AddIncomingMsgToSequence(a_Reject)) {
        return;
    }  
    if(1 == atoi(const_cast<CFixCancelReject&>(a_Reject).GetCustomFieldValue(434).c_str())){
        CCancelReject l_Reject;
        l_Reject.SetSourceId(a_Reject.TargetSubID().c_str());
        l_Reject.SetOrderTag(a_Reject.OrigClOrdID().c_str());
        l_Reject.SetExchangeTag(a_Reject.OrderID().c_str());
        l_Reject.SetCancelTag(a_Reject.ClOrdID().c_str());
        l_Reject.SetText(a_Reject.Text().c_str());
        l_Reject.SetStatus(m_Translator.m_ToStaffordStatus[a_Reject.OrdStatus()]);
        l_Reject.SetReason(const_cast<CFixCancelReject&>(a_Reject).GetCustomFieldValue(58).c_str());
        ((CMessageApp*) AfxGetApp())->Handler().ProcessMessage(l_Reject);
    } else if(2 == atoi(const_cast<CFixCancelReject&>(a_Reject).GetCustomFieldValue(434).c_str())) {
        CReplaceReject l_Reject;
        l_Reject.SetSourceId(a_Reject.TargetSubID().c_str());
        l_Reject.SetOrderTag(a_Reject.OrigClOrdID().c_str());
        l_Reject.SetExchangeTag(a_Reject.OrderID().c_str());
        l_Reject.SetCancelTag(a_Reject.ClOrdID().c_str());
        l_Reject.SetText(a_Reject.Text().c_str());
        l_Reject.SetReason(const_cast<CFixCancelReject&>(a_Reject).GetCustomFieldValue(58).c_str());
        ((CMessageApp*) AfxGetApp())->Handler().ProcessMessage(l_Reject);
    }
}

//**************************************************************************
// 
//****************************************************************************
void CGlobexFixSession::HeartbeatOverride(CFixHeartbeat& heartbeat)
{
    AddFixAdminMsgHeader(heartbeat, m_SenderSubId, m_firmId);
}

//**************************************************************************
//
//****************************************************************************
void CGlobexFixSession::LogonOverride(CFixLogon& logon)
{
    CILinkGlobexApp* pApp = (CILinkGlobexApp*) AfxGetApp();
    
    AddFixAdminMsgHeader(logon, m_SenderSubId, m_firmId);
    CFixField* pResetSeqNum = logon.GuaranteedGetCustomField(FIX_ResetSeqNumFlag);
    pResetSeqNum->Value(_T("N"));
    
    if (pApp->ForceRecycle()) {
        SetNextIncomingSeqNum(1);
        SetNextOutgoingSeqNum(1);
    } 
    
}

//**************************************************************************

//****************************************************************************
void CGlobexFixSession::LogoutOverride(CFixLogout& logout)
{
    AddFixAdminMsgHeader(logout, m_SenderSubId, m_firmId);
}

//**************************************************************************
// 
//****************************************************************************
BOOL CGlobexFixSession::OrderEntryOverride(const COrder& order, CFixOrder& fixOrder)
{
    // Handling instructions = FIX_AutoPrivate
    // This call can fail that's why it is here instead
    // of AddFix42OrderFields.  If it fails there is no
    // sense of continuing.
    CFixField* pField = fixOrder.GetField(FIX_HandlInst);
    
    if (!pField)
    {
        return FALSE;
    }
    
    pField->Value(FIX_AutoPrivate);
    
    AddFixApplicationMsgHeader(fixOrder, order.TradeFor(), order.ClearingId());//, order.Instructions());
    
    AddFix42OrderFields(order, fixOrder);
    
    return TRUE;
}

//**************************************************************************
// 
//****************************************************************************
void CGlobexFixSession::ResendOverride(CFixResendRequest& resendRequest)
{
    AddFixAdminMsgHeader(resendRequest, m_SenderSubId, m_firmId);
}

//**************************************************************************
//
//****************************************************************************
void CGlobexFixSession::RejectOverride(CFixReject& rejectRequest)
{
    AddFixAdminMsgHeader(rejectRequest, m_SenderSubId, m_firmId);
}

//**************************************************************************
// 
//****************************************************************************
void CGlobexFixSession::SequenceResetOverride(CFixSequenceReset& resetRequest)
{
    AddFixAdminMsgHeader(resetRequest, m_SenderSubId, m_firmId);
}

//**************************************************************************
// 
//****************************************************************************
void CGlobexFixSession::TestRequestOverride(CFixTestRequest& testRequest)
{
    AddFixAdminMsgHeader(testRequest, m_SenderSubId, m_firmId);
}



