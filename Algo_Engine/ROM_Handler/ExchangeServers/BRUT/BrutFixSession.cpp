/*******************************************************************************
*
* Copyright (c) 2005 by RONIN CAPITAL, LLC
*
* All Rights Reserved. 
*******************************************************************************/
/*
*  BrutFixSession.cpp : Processes Messages to and from the Brut exchange. Translates 
*                       Ronin OM to FIX 4.0.  Maintains Brut specific state.
*
*
* History
*
* date        user	       comment
* -------     --------    -----------------------------------------------------
* 11-11-2005   Parnell     Added logic to handle replace orders ( previously Brut
*                          replace orders were handled with a cancel and a new).
*                          Added logic to add Brut specific FIX tags for routing 
*                          information.
*******************************************************************************/

#include "BrutFixSession.h"
#include "FixAppMsgs.h"
#include "Cancel.h"
#include "Order.h"
#include "Execution.h"
#include "MessageApp.h"

#include <iostream>

CBrutFixSession::CBrutFixSession()
{
    CMessageApp *pApp = (CMessageApp*)AfxGetApp();
    m_bSuperMontage = GetPrivateProfileInt("Option Information", "Do SuperMontage", 0, pApp->DatFile());
    _tcsset(m_tag, '\0');
    _tcsset(m_value, '\0');
}

long CBrutFixSession::ProcessEntry(COrder& a_Order)
{
    const MapIntToChar& l_SideMap = Translator().m_ToExchangeSide;
    const MapIntToChar& l_TypeMap = Translator().m_ToExchangeSide;
    const MapIntToChar& l_TIFMap = Translator().m_ToExchangeTIF;
    
    MapIntToChar::const_iterator l_SideIter = l_SideMap.find(a_Order.Side());
    MapIntToChar::const_iterator l_TypeIter = l_TypeMap.find(a_Order.Type());
    MapIntToChar::const_iterator l_TIFIter = l_TIFMap.find(a_Order.TIF());
    
    long l_SeqNum = OutgoingMsgs().NextMsgSeqNum();
    /* Parnell created local order type varable since I need it for BRUT Processing.*/
    EFixOrdType l_OrdType = (EFixOrdType) l_TypeIter->second;
    CFixOrder l_Order(
        l_SeqNum, 
        GetCurrentTimeStr(), 
        a_Order.OrderTag(), 
        a_Order.Symbol(), 
        (EFixSide) l_SideIter->second, 
        l_OrdType    
        );
    l_Order.TargetSubID(TargetSubId().c_str());
    l_Order.SenderSubID(a_Order.SourceId());
    l_Order.Price(a_Order.dPrice());
    l_Order.OrderQty(a_Order.Shares());
    l_Order.Account(a_Order.ClearingId());
    l_Order.TimeInForce((EFixTimeInForce) l_TIFIter->second);
    if(a_Order.Floor() != 0) {
        l_Order.MaxFloor(a_Order.Floor());
    }
    const char* shortLender = a_Order.ShortLender();
    if(shortLender && *shortLender) {
        CFixOrderedFieldSet::TFixFieldAutoPtr l_FieldPtr;
        l_FieldPtr = l_Order.CreateNewField((EFixFieldID)9305);
        l_FieldPtr->Value(shortLender);
        l_Order.InsertInBody(l_FieldPtr);
    }
    /*Locally scoped. Inserts Capacity*/
    {
        CFixOrderedFieldSet::TFixFieldAutoPtr l_FieldPtr;
        l_FieldPtr = l_Order.CreateNewField((EFixFieldID)9301);
        l_FieldPtr->Value(a_Order.Capacity());
        l_Order.InsertInBody(l_FieldPtr);
    }
    
    //l_Order.Rule80A(a_Order.Capacity());
    CString exDestination;
    if(a_Order.ExDestination() != "") {
        // l_Order.ExDestination(a_Order.ExDestination());
        exDestination = a_Order.ExDestination();
    } else {
        exDestination = a_Order.OrigExDestination();
    }
    
    CString instructions = a_Order.Instructions();
    SetBrutTags(l_Order, l_OrdType, exDestination, instructions);
    if(SendMsg(l_Order))
        return l_SeqNum;
    else
        return 0;
}
void CBrutFixSession::SetBrutTags(CFixOrderBase& l_Order, EFixOrdType& l_OrdType,
                                  CString& exDestination, CString& instructions)
{
/* 
- Parnell - 
The values seen below represent the defaults that will be applied
based on destination and Order type.  If the user has sent values in
the instruction field of the OM message then those values assuming they 
are properly formatted will be applied to the BRUT Fix message.
    */
    if(instructions == "" &&
        l_OrdType != FIX_MarketOnClose)
    {
        CFixOrderedFieldSet::TFixFieldAutoPtr l_FieldPtr;
        l_FieldPtr = l_Order.CreateNewField((EFixFieldID)9303);
        
        CFixOrderedFieldSet::TFixFieldAutoPtr l_FieldPtr1;
        l_FieldPtr1 = l_Order.CreateNewField((EFixFieldID)9307);
        if (exDestination == "N")
        {
            l_FieldPtr->Value("X");
            l_FieldPtr1->Value("ADOT");
            l_Order.InsertInBody(l_FieldPtr);
            l_Order.InsertInBody(l_FieldPtr1);
        }
        else if (exDestination == "A")
        {
            l_FieldPtr->Value("X");
            l_FieldPtr1->Value("AMEX");
            l_Order.InsertInBody(l_FieldPtr);
            l_Order.InsertInBody(l_FieldPtr1);
        }
        else if (exDestination == "O")
        {
            l_FieldPtr->Value("G");
            l_Order.InsertInBody(l_FieldPtr);
        }
        else if(exDestination == "Direct+" ||
            exDestination == "D") 
        {
            l_FieldPtr->Value("X");
            l_FieldPtr1->Value("ADOT");
            CFixOrderedFieldSet::TFixFieldAutoPtr l_FieldPtr3;
            l_FieldPtr3 = l_Order.CreateNewField((EFixFieldID)5194);
            l_FieldPtr3->Value("Y");
            l_Order.InsertInBody(l_FieldPtr);
            l_Order.InsertInBody(l_FieldPtr1);
            l_Order.InsertInBody(l_FieldPtr3);
        }
    } 
    /* In the case of a Market On Close we have no other option than
    to send the order through BRUT to the NYSE.*/
    else if(l_OrdType == FIX_MarketOnClose) 
    {
        CFixOrderedFieldSet::TFixFieldAutoPtr l_FieldPtr;
        l_FieldPtr = l_Order.CreateNewField((EFixFieldID)9303);
        
        CFixOrderedFieldSet::TFixFieldAutoPtr l_FieldPtr1;
        l_FieldPtr1 = l_Order.CreateNewField((EFixFieldID)9307);
        l_FieldPtr->Value("T");
        l_FieldPtr1->Value("NYSE");
        l_Order.InsertInBody(l_FieldPtr);
        l_Order.InsertInBody(l_FieldPtr1);
    } 
    else if(instructions != "")
    {
        ProcessInstructionField(instructions, l_Order);
    }
    
}

BOOL CBrutFixSession::ProcessReplace(CReplace& a_Replace)
{
    EFixOrdType l_OrdType = (EFixOrdType) m_Translator.m_ToExchangeType[a_Replace.Type()];
    CFixReplace l_Order(
        m_OutgoingMsgs.NextMsgSeqNum(),
        GetCurrentTimeStr(),
        a_Replace.OrderTag(),
        a_Replace.CancelTag(),
        a_Replace.Symbol(),
        (EFixSide) m_Translator.m_ToExchangeSide[a_Replace.Side()],
        l_OrdType
        );
    
    if (strcmp(TargetSubId().c_str(), "") != 0)
    {
        l_Order.TargetSubID(m_TargetSubId.c_str());
    }
    
    TString senderSubId = SenderSubId();
    if ( senderSubId.length() > 0 )
        l_Order.SenderSubID((const char*)senderSubId.c_str());
    else if (strcmp(a_Replace.SourceId(), "") != 0)
        l_Order.SenderSubID(a_Replace.SourceId());
    
    
    if (a_Replace.ExecInst() != COrder::LastExecInst)
    {
        char ExecInst[2];
        sprintf(ExecInst, "%c", a_Replace.ExecInst());
        l_Order.ExecInst(ExecInst);
    }
    
    l_Order.TimeInForce((EFixTimeInForce) m_Translator.m_ToExchangeTIF[a_Replace.TIF()]);
    
    l_Order.OrderQty(a_Replace.Shares());
    
    if (a_Replace.Type() == COrder::Stop)
    {
        l_Order.StopPx(a_Replace.StopPrice());
    }
    else if (a_Replace.Type() == COrder::StopLimit)
    {
        l_Order.StopPx(a_Replace.StopPrice());
        l_Order.Price(a_Replace.Price());
    }
    else if (a_Replace.Type() != COrder::Market &&
        a_Replace.Type() != COrder::MarketOnClose &&
        a_Replace.Type() != COrder::MarketAtOpen)
    {
        l_Order.Price(a_Replace.Price());
    }
    
    if (a_Replace.Floor() != 0)
    {
        l_Order.MaxFloor(a_Replace.Floor());
    }
    
    
    CString exDestination;
    
    exDestination = a_Replace.ExDestination();
    
    
    CString instructions = a_Replace.Instructions();
    SetBrutTags(l_Order, l_OrdType, exDestination, instructions);
    
    return SendMsg(l_Order);
}

void CBrutFixSession::ProcessInstructionField(const CString& instructions,
                                              CFixOrderBase& l_Order)
{   
    LPCTSTR localInst = (LPCTSTR)instructions;
    int length = instructions.GetLength();   
    int lastTag = 0;
    int pos = -1;
    BOOL foundTag = FALSE;
    while (lastTag < length) {
        pos = -1;
        pos = _tcscspn(localInst + lastTag, "=");
        if(pos > 0 && (pos +lastTag) < (length) && pos < BRUT_UD_TAG_SIZE) {        
            _tcsncpy(m_tag, localInst + lastTag, (pos));
            lastTag += (pos + 1);
            pos = _tcscspn(localInst + lastTag, "#");
            if(pos >0 && (pos + lastTag) <= length && pos < BRUT_UD_VALUE_SIZE) {                             
                CFixOrderedFieldSet::TFixFieldAutoPtr l_FieldPtr;
                try {
                    l_FieldPtr = l_Order.CreateNewField((EFixFieldID)_ttoi(m_tag));                
                    _tcsncpy(m_value, localInst + lastTag, pos);
                    lastTag += (pos + 1);
                    l_FieldPtr->Value(m_value);
                    l_Order.InsertInBody(l_FieldPtr);
                    _tcsnset(m_value, 0, pos);
                    
                } catch(...) {
                    CString errorString("An error occurred while processing Instruction field: ");
                    errorString += (LPCTSTR)instructions;
                    ProcessError(errorString); 
                }
            } else {
                //Error return without setting fields. 
                CString errorString("An error occurred while processing Instruction field: ");
                errorString += (LPCTSTR)instructions;
                ProcessError(errorString);
                return;
            }
            _tcsset(m_tag, '\0');
        } else {
            CString errorString("An error occurred while processing Instruction field: ");
            errorString += (LPCTSTR)instructions;
            ProcessError(errorString);
            return;
        }
    }
}

BOOL CBrutFixSession::ProcessCancel(CCancel& a_Cancel)
{
    const MapIntToChar& l_Map = Translator().m_ToExchangeSide;
    MapIntToChar::const_iterator l_Iter = l_Map.find(a_Cancel.Side());
    if(l_Iter == l_Map.end())
        return FALSE;
    CFixCancel l_Cancel(
        OutgoingMsgs().NextMsgSeqNum(),
        GetCurrentTimeStr(),
        a_Cancel.OrderTag(),
        a_Cancel.CancelTag(),
        a_Cancel.Symbol(),
        (EFixSide) l_Iter->second
        );
    l_Cancel.TargetSubID(TargetSubId().c_str());
    l_Cancel.OrderID(a_Cancel.ExchangeTag());
    l_Cancel.OrderQty(a_Cancel.Shares());
    if(CFixMsg::GetVersionNum() < 4.1)
        l_Cancel.CxlType(FIX_FullCxl);
    l_Cancel.OrigSendingTime(GetCurrentTimeStr());
    
    CFixOrderedFieldSet::TFixFieldAutoPtr l_FieldPtr;
    l_FieldPtr = l_Cancel.CreateNewField((EFixFieldID)9306);
    l_FieldPtr->Value(a_Cancel.Instructions());
    l_Cancel.InsertInBody(l_FieldPtr);
    
    return SendMsg(l_Cancel);
}

void CBrutFixSession::Process(const CFixExecution& a_Exec)
{
    if(!AddIncomingMsgToSequence(a_Exec))
        return;
    CExecution l_Exec;
    l_Exec.SetSourceId(a_Exec.TargetSubID().c_str());
    l_Exec.SetReport((char) a_Exec.ReportToExch());
    l_Exec.SetOrderTag(a_Exec.ClOrdID().c_str());
    l_Exec.SetExchangeTag(a_Exec.OrderID().c_str());
    l_Exec.SetOriginalTag(a_Exec.OrigClOrdID().c_str());
    l_Exec.SetExecBroker(a_Exec.ExecBroker().c_str());
    l_Exec.SetConfirm(a_Exec.ExecID().c_str());
    l_Exec.SetTransactionType(a_Exec.ExecTransType());
    l_Exec.SetReferenceConfirm(a_Exec.ExecRefID().c_str());
    l_Exec.SetAccount(a_Exec.Account().c_str());
    l_Exec.SetSymbol(a_Exec.Symbol().c_str());
    l_Exec.SetInstructions(a_Exec.ExecInst().c_str());
    l_Exec.SetText(a_Exec.Text().c_str());
    
    const MapCharToInt& l_StatusMap = Translator().m_ToStaffordStatus;
    MapCharToInt::const_iterator l_Iter = l_StatusMap.find(a_Exec.OrdStatus());
    if(l_Iter == l_StatusMap.end()) {
        ProcessError("An error occurred while processing an execution message.");
        return;
    }
    l_Exec.SetStatus(l_Iter->second);
    const MapCharToInt& l_SideMap = Translator().m_ToStaffordSide;
    if((l_Iter = l_SideMap.find(a_Exec.Side())) == l_SideMap.end()) {
        ProcessError("An error occurred while processing an execution message.");
        return;
    }
    l_Exec.SetSide(l_Iter->second);
    l_Exec.SetShares( (a_Exec.OrderQty() - a_Exec.CumQty()) );
    
    
    const MapCharToInt& l_TypeMap = Translator().m_ToStaffordType;
    if((l_Iter = l_TypeMap.find(a_Exec.OrdType())) == l_TypeMap.end()) {
        ProcessError("An error occurred while processing an execution message.");
        return;
    }
    l_Exec.SetOrderType(l_Iter->second);
    const MapCharToInt& l_TifMap = Translator().m_ToStaffordTIF;
    if((l_Iter = l_TifMap.find(a_Exec.TimeInForce())) == l_TifMap.end()) {
        ProcessError("An error occurred while processing an execution message.");
        return;
    }
    l_Exec.SetTimeInForce(l_Iter->second);
    l_Exec.SetLastShares(a_Exec.LastShares());
    l_Exec.SetLeaves(a_Exec.LeavesQty());
    l_Exec.SetCumulative(a_Exec.CumQty());
    l_Exec.SetAverage(a_Exec.AvgPx());
    l_Exec.SetPrice(a_Exec.Price());
    l_Exec.SetStopPrice(a_Exec.StopPx());
    l_Exec.SetLastPrice(a_Exec.LastPx());
    if((l_Iter = l_StatusMap.find(a_Exec.ExecType())) == l_StatusMap.end()) {
        ProcessError("An error occurred while processing an execution message.");
        return;
    }
    l_Exec.SetExecType(l_Iter->second);
    
    try
    {
        l_Exec.SetInstructions(a_Exec.GetField((EFixFieldID)9306)->ValueAsString().c_str());
    }
    catch(...){}
    
    if(a_Exec.OrdStatus() ==  FIX_PartiallyFilled ||
        a_Exec.OrdStatus() == FIX_Filled) {
        try {
            l_Exec.SetBillingCode(a_Exec.GetField((EFixFieldID)9373)->ValueAsString().c_str());
        } catch(...) {
            std::cerr<<" ERROR GETTING THE BILLING CODE."<<std::endl;
        }
    }
    ((CMessageApp*) AfxGetApp())->Handler().ProcessMessage(l_Exec);
}
