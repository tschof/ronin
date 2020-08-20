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
#include "OrderHandler.h"
#include "GlobexMemTagSet.h"
#include "FixSession.h"
#include "SendCancelReject.h"

#include "DatabaseSettingsFactory.h"
#include "DatabaseSettingsInterface.h"

//**************************************************************************
// 
//****************************************************************************
CILinkGlobexOrderHandler::CILinkGlobexOrderHandler()
: m_orderIdGenerator()
{
    CMessageApp *pApp = (CMessageApp *)AfxGetApp();
    

CString exchangeDestinationId;

	CString dbUser;
	CString dbPassword;
	CString	dbServer;

	GetPrivateProfileString(pApp->Name() + " Server",
							"Exchange Destination Id",
							"",
							exchangeDestinationId.GetBuffer(128),
							128,
							pApp->DatFile());

	exchangeDestinationId.ReleaseBuffer();

	GetPrivateProfileString("Database Information",
							"User",
							"",
							dbUser.GetBuffer(128),
							128,
							pApp->DatFile());

	dbUser.ReleaseBuffer();

	GetPrivateProfileString("Database Information",
							"Password",
							"",
							dbPassword.GetBuffer(128),
							128,
							pApp->DatFile());

	dbPassword.ReleaseBuffer();

	GetPrivateProfileString("Database Information",
							"Server",
							"",
							dbServer.GetBuffer(128),
							128,
							pApp->DatFile());

	dbServer.ReleaseBuffer();

	bool forceReload = GetPrivateProfileInt("MemoryMapSettings", "ForceReload",
        0, pApp->DatFile()) == 0 ? false : true;

    DatabaseSettings::TIDatabaseSettingsAutoPtr lDatabaseSettings =
        DatabaseSettings::CDatabaseSettingsFactory::CreateDatabaseSettings();

	lDatabaseSettings->AppName((LPCSTR) pApp->Name());
	lDatabaseSettings->DestinationId((LPCSTR) exchangeDestinationId);
	lDatabaseSettings->Server((LPCSTR) dbServer);
	lDatabaseSettings->User((LPCSTR) dbUser);
	lDatabaseSettings->Password((LPCSTR) dbPassword);


    m_pTags = new CGlobexMemTagSet(lDatabaseSettings, forceReload);
    
    //m_pTags->InitMap();
    
    m_orderTypeValidator = new COrderTypeValidator(std::string((LPCSTR) pApp->DatFile()));
    
    GetPrivateProfileString("Fix Information",	"Firm ID",
        "", m_DefaultAccount.GetBuffer(128), 128,	pApp->DatFile()); 
    m_DefaultAccount.ReleaseBuffer();    
    
    
    ///////////////////////////////////////////
    GetPrivateProfileString("Clearing Information", "Allowed FirmID","",
        m_AllowedIDs.GetBuffer(128),
        128, pApp->DatFile());
    m_AllowedIDs.ReleaseBuffer();
    pApp->ProcessError("THIS IS ALL OF EM ", m_AllowedIDs);
    int Index;
    for (int FieldCnt = 0; ((Index = m_AllowedIDs.Find(',')) >= 0); FieldCnt++) {
        
        if (Index != 0) {
            CString local = m_AllowedIDs.Left(Index);
            m_AllowedFirmIDs[local] =FieldCnt;
            pApp->ProcessError("THIS IS the local", local);
        }
        
        m_AllowedIDs = m_AllowedIDs.Mid(Index+1);
        pApp->ProcessError("THIS IS WHAT IS LEFT ", m_AllowedIDs);
    }
    if(m_AllowedIDs.GetLength() > 0) {
        m_AllowedFirmIDs[m_AllowedIDs] =FieldCnt;
    }
    
    ///////////////////////////////////////
}


//**************************************************************************
//
//****************************************************************************
CILinkGlobexOrderHandler::~CILinkGlobexOrderHandler()
{
    delete m_pTags;
    
    if (m_orderTypeValidator != NULL)
    {
        delete m_orderTypeValidator;
    }
}

//**************************************************************************
//
//****************************************************************************
void CILinkGlobexOrderHandler::ProcessMessage(CCancel& cancel)
{
    CString		error;
    CMessageApp *pApp = (CMessageApp *)AfxGetApp();	
    
    try
    {
        if (m_pTags->GetTags(MAP_STAFFORD_TAG_QUERY, cancel.StaffordTag()))
        {
            
            if(m_pTags->IsPendingCancel()) {
                return;
            } 
            if(m_pTags->GetClearingId()) {
                cancel.SetClearingId(m_pTags->GetClearingId());
                
            } else if(m_AllowedFirmIDs.find(cancel.ClearingId()) == m_AllowedFirmIDs.end()) {
                CString rejectMessage = "Unrecognized Clearing ID: ";
                rejectMessage += cancel.ClearingId();
                
                // Generate Reject
                CSendCancelReject Reject(cancel);		
                Reject.SetText(rejectMessage);
                Reject.SetReason(rejectMessage);
                Reject.SetSourceId(m_pTags->Client());
                // Send to client
                pApp->Status().SetStatus(Reject);
                pApp->ProcessDebug("SEND REJECT", cancel.Debug());
                
                return;
                
            } 
            cancel.SetClearingAccount(m_pTags->GetClearingAccount());
            // set the exchange's tag an the fix order tag
            cancel.SetExchangeTag(m_pTags->ExchangeTag());
            cancel.SetOrderTag(m_pTags->OrderTag());
            
            CString cancelTag;
            m_orderIdGenerator.NextOrderId(cancelTag);
            // cancelTag = cancel.OrderTag();
            
            //CreateCancelTag(cancelTag, pApp->Exchange().GetNextOutgoingSeqNum());
            
            cancel.SetCancelTag(cancelTag);
            
            // Send to the exchange
            if (pApp->Exchange().ProcessCancel(cancel)) {
                // set the cancel tag which is generated by the fix session
                m_pTags->SetCancelTag(cancel.CancelTag());
                m_pTags->SetCancel(TRUE);
                m_pTags->SetPendingCancel(true);
                
                // Send to client
                pApp->Status().SetStatus(cancel);
                pApp->ProcessDebug("CANCEL SENT", cancel.Debug());
            } else {
                // Generate Reject
                CSendCancelReject	Reject(cancel);		
                
                // Set the cancel tag which is generated by the fix session
                m_pTags->SetCancelTag(cancel.CancelTag());
                Reject.SetSourceId(m_pTags->Client());
                
                // Send to client
                pApp->Status().SetStatus(Reject);
                pApp->ProcessDebug("SEND REJECT", cancel.Debug());
            }
        }
    }
    catch (CDBException *e)
    {
        error.Format("EXCEPTION: Error Processing Outgoing Message (%s): %s", e->m_strError, cancel.Debug());
        pApp->ProcessError("ILINKGLOBEX Handler", error);
        e->Delete();
    }
    catch (const char *Message)
    {
        pApp->ProcessError("ILINKGLOBEX Handler", Message);
    }
}



//**************************************************************************
// 
//****************************************************************************
void CILinkGlobexOrderHandler::ProcessMessage(CExecution& execution)
{
    CString		Error;
    CMessageApp *pApp = (CMessageApp *)AfxGetApp();		
    CString origOrderTag;
    if(execution.Status() == COrder::PendingCancel ||
        execution.Status() == COrder::Canceled) {
        GetOrigOrderId(origOrderTag, const_cast<CString&>(execution.OrderTag()));
    } else {
        origOrderTag = execution.OrderTag();
    }
    
    try
    {	
        if (execution.Status() != COrder::PendingCancel)
        {
            if(execution.Status() == COrder::Replaced) 
            {
                if (m_pTags->GetTags(MAP_ORDER_TAG_QUERY, origOrderTag) ||
                    m_pTags->GetTags(MAP_CANCEL_TAG_QUERY, origOrderTag))
                {
                    // set new order tag
                    m_pTags->SetOrderTag(m_pTags->CancelTag());
                    m_pTags->SetCancelTag(-1);
                    
                    m_pTags->SetOriginalPrice(execution.Price());
                    m_pTags->SetShares(m_pTags->Shares() - m_pTags->OriginalShares() + execution.Shares());
                    m_pTags->SetOriginalShares(execution.Shares());
                    
                    // set exchange tag if needed
                    if (execution.ExchangeTag() != m_pTags->ExchangeTag())
                    {
                        m_pTags->SetExchangeTag(execution.ExchangeTag());
                    }
                    
                    // get the stafford tag based off of the order tag
                    execution.SetStaffordTag(m_pTags->StaffordTag());
                    //Execution.SetSourceId(m_pTags->Client());
                    
                    pApp->ProcessDebug("RECV STATUS", execution.Debug());
                    
                    // update Status
                    if (m_pTags->GetSecurityType() == COrder::Future) {
                        double price = execution.LastPrice() / 100.00;
                        execution.SetLastPrice(price);
                        price = execution.Price() / 100.00;
                        execution.SetPrice(price);
                        
                        double stopPrice = execution.StopPrice() /100;
                        execution.SetStopPrice(stopPrice);
                    }
                    execution.SetSourceId(m_pTags->Client());
                    execution.SetStatus(COrder::Open);
                    execution.SetShares(m_pTags->Shares());
                    pApp->Status().SetStatus(execution);
                    //m_pTags->Close();
                }
                else
                {
                    Error.Format("Unable To Find Order Tag (%s) In Tag Map For OrderId(%s): (%s)", execution.OrderTag(), execution.StaffordTag(), execution.Debug());
                    pApp->ProcessError("ILINKGLOBEX Handler", Error);
                }
            }
            else 
            {
                BOOL bFound = FALSE;
                
                if (m_pTags->GetTags(MAP_ORDER_TAG_QUERY, origOrderTag))
                {
                    bFound = TRUE;
                }
                else if (m_pTags->GetTags(MAP_CANCEL_TAG_QUERY, origOrderTag))
                {
                    bFound = TRUE;
                }
                
                if(bFound)
                {
                    execution.SetSourceId(m_pTags->Client());
                    
                    // set exchange tag if needed
                    if (execution.ExchangeTag() != m_pTags->ExchangeTag())
                    {
                        m_pTags->SetExchangeTag(execution.ExchangeTag());
                    }
                    
                    // get the stafford tag based off of the order tag
                    execution.SetStaffordTag(m_pTags->StaffordTag());
                    execution.SetSourceId(m_pTags->Client());
                    
                    pApp->ProcessDebug("RECV STATUS", execution.Debug());
                    
                    switch(execution.Status())
                    {
                    case COrder::PartiallyFilled:
                    case COrder::Filled:
                        
                        if (m_pTags->GetSecurityType() == COrder::Future)
                        {
                            double price = execution.LastPrice() / 100.00;
                            execution.SetLastPrice(price);
                            double stopPrice = execution.StopPrice() /100;
                            execution.SetStopPrice(stopPrice);
                        }
                        
                        if (execution.Status() == COrder::PartiallyFilled)
                        {
                            m_pTags->SetShares(m_pTags->Shares() - execution.LastShares());
                        }
                        else
                        {
                            m_pTags->RemoveTags();
                        }
                        
                        break;
                        
                    case COrder::Canceled:
                    case COrder::DoneForDay:
                    case COrder::Rejected:
                    case COrder::Stopped:
                        
                        if (m_pTags->GetSecurityType() == COrder::Future)
                        {
                            double price = execution.Price() / 100.00;
                            execution.SetPrice(price);
                            double stopPrice = execution.StopPrice() /100;
                            execution.SetStopPrice(stopPrice);
                        }
                        
                        m_pTags->RemoveTags();
                        
                        break;
                        
                    case COrder::Open:
                        
                        double price = execution.Price();
                        
                        if (m_pTags->GetSecurityType() == COrder::Future)
                        {
                            price = price / 100.00;
                            double stopPrice = execution.StopPrice() /100;
                            execution.SetStopPrice(stopPrice);
                        }
                        
                        if (m_pTags->Cancel() != TRUE)
                        {
                            m_pTags->SetOriginalPrice(price);
                            execution.SetPrice(price);
                            execution.SetShares(0);
                        }
                        
                        break;
                    }
                    
                    // update Status
                    if (execution.Status() == COrder::Open)
                    {
                        if (m_pTags->Cancel() != TRUE)
                        {
                            pApp->Status().SetStatus(execution);
                        }
                        else
                        {
                            m_pTags->SetCancel(FALSE);
                        }
                    }
                    else
                    {
                        pApp->Status().SetStatus(execution);
                    }
                }
                else
                {
                    Error.Format("Unable To Find Order Tag (%s) In Tag Map For OrderId(%s): (%s)", execution.OrderTag(), execution.StaffordTag(), execution.Debug());
                    pApp->ProcessErrorEx(MODE_ERR_BUSINESS, "GLOBEX Handler", Error);
                }
            }
        }
    }
    catch (CDBException *e)
    {
        Error.Format("EXCEPTION: Error Processing Incoming Status Message (%s): %s", e->m_strError, execution.Debug());
        pApp->ProcessError("GLOBEX Handler", Error);
        e->Delete();
    }
}

//**************************************************************************
//
//****************************************************************************
void CILinkGlobexOrderHandler::ProcessMessage(COrder& order)
{
    long		sequenceNumber	= 0;
    CString		error			= "";
    CString		oldOrderTag		= order.OrderTag();
    CMessageApp *pApp			= (CMessageApp *)AfxGetApp();	
    
    if ((pApp->Host()).LoggedOn() != TRUE)
    {
        CReject	reject("Unable To Send", order);			
        m_pTags->CreateNewMap(order.StaffordTag(), oldOrderTag, -1, order.SourceId());
        reject.SetSourceId(order.SourceId());
        pApp->Status().SetStatus(reject);
        pApp->ProcessDebug("ORDER REJECT", order.Debug());
        return;
    }
    
    if (order.SecurityType() != COrder::Future && order.SecurityType() != COrder::FutureEquity)
    {
        CString rejectMessage = "Unrecognized order type: ";
        rejectMessage += order.SecurityType();
        
        CReject	reject(rejectMessage.GetBuffer(0), order);			
        m_pTags->CreateNewMap(order.StaffordTag(), oldOrderTag, -1, order.SourceId());
        reject.SetSourceId(order.SourceId());
        pApp->Status().SetStatus(reject);
        pApp->ProcessDebug("ORDER REJECT", order.Debug());
        return;
    }
    if(m_AllowedFirmIDs.find(order.ClearingId()) == m_AllowedFirmIDs.end()) {
        CString rejectMessage = "Unrecognized Clearing ID: ";
        rejectMessage += order.ClearingId();
        
        CReject	reject(rejectMessage.GetBuffer(0), order);			
        m_pTags->CreateNewMap(order.StaffordTag(), oldOrderTag, -1, order.SourceId());
        reject.SetSourceId(order.SourceId());
        pApp->Status().SetStatus(reject);
        pApp->ProcessDebug("ORDER REJECT", order.Debug());
        return;
        
    }
    
    try
    {
        m_orderTypeValidator->IsSupportedTIF((COrder::TimeInForce) (order.TIF()));
        m_orderTypeValidator->IsSupportedOrderType((COrder::OrderType) (order.Type()));
    }
    catch(CTIFException e)
    {
        std::stringstream reason;
        
        reason << "Unsupported Time In Force: " << e.TimeInForce();
        
        CReject	Reject(reason.str().c_str(), order);
        Reject.SetSourceId(order.SourceId());
        pApp->Status().SetStatus(Reject);
        pApp->ProcessDebug("ORDER REJECT", order.Debug());
        return;
    }
    catch(COrderTypeException e)
    {
        std::stringstream reason;
        
        reason << "Unsupported Order Type: " << e.OrderType();
        
        CReject	Reject(reason.str().c_str(), order);
        Reject.SetSourceId(order.SourceId());
        pApp->Status().SetStatus(Reject);
        pApp->ProcessDebug("ORDER REJECT", order.Debug());
        return;
    }
    catch(...)
    {
    }
    
    try
    {
        
        if (order.SecurityType() == COrder::Future)
        {
            double price = order.dPrice() * 100;
            double stopPrice = order.StopPrice() * 100;
            order.SetPrice(price);
            order.SetStopPrice(stopPrice);
        }
        
        if (order.ClearingId() == "")
        {
            order.SetClearingId(m_DefaultAccount);
        }
        /* else
        {
        order.SetClearingId(order.ClearingId());
    }*/
        CString orderId;
        m_orderIdGenerator.NextOrderId(orderId);
        order.SetOrderTag(orderId);
        
        // Send to exchange
        if ((sequenceNumber = pApp->Exchange().ProcessEntry(order)) > 0)
        {
            m_pTags->CreateNewMap(order.StaffordTag(), order.OrderTag(), sequenceNumber, order.SourceId());
            m_pTags->SetOriginalPrice(order.dPrice());
            m_pTags->SetShares(order.Shares());
            m_pTags->SetOriginalShares(order.Shares());
            m_pTags->SetCancel(FALSE);
            m_pTags->SetPendingCancel(false);
            m_pTags->SetSecurityType(order.SecurityType());
            m_pTags->SetTradeFor(order.lpszTradeFor());
            m_pTags->SetClearingId(order.ClearingId());
            m_pTags->SetClearingAccount(order.ClearingAccount());
            // Send status to client
            pApp->Status().SetStatus(order);
            
            pApp->ProcessDebug("ORDER SENT", order.Debug());
        }
        else
        {
            // generate reject message
            CReject	reject("Unable To Send", order);
            
            // Create New Map Entry
            m_pTags->CreateNewMap(order.StaffordTag(), oldOrderTag, -1, order.SourceId());
            reject.SetSourceId(m_pTags->Client());
            // Update Status
            pApp->Status().SetStatus(reject);
            pApp->ProcessDebug("ORDER REJECT", order.Debug());
        }
    }
    catch (CDBException *e)
    {
        error.Format("DB EXCEPTION: Error Processing Outgoing Message (%s): %s", e->m_strError, order.Debug());
        pApp->ProcessError("ILINKGLOBEX Handler", error);
        e->Delete();
    }
    catch (const char *Message)
    {
        pApp->ProcessError("ILINKGLOBEX Handler", Message);
    }	
}

//**************************************************************************
// 
//****************************************************************************
void CILinkGlobexOrderHandler::ProcessMessage(CReject& reject)
{
    CString	Error;
    CMessageApp *pApp = (CMessageApp *)AfxGetApp();		
    CString origOrderId;
    
    GetOrigOrderId(origOrderId, reject.OrderTag());
    
    try {
        pApp->ProcessDebug("RECV STATUS", reject.Debug());
        
        if (m_pTags->GetTags(MAP_SEQUENCE_TAG_QUERY, origOrderId)) {
            // set tags
            reject.SetOrderTag(m_pTags->OrderTag());
            reject.SetExchangeTag(m_pTags->ExchangeTag());
            reject.SetCancelTag(m_pTags->CancelTag());
            reject.SetStaffordTag(m_pTags->StaffordTag());
            reject.SetSourceId(m_pTags->Client());
            
            // update Status
            pApp->Status().SetStatus(reject);
        } else {
            CString totalMessage = reject.Reason();
            totalMessage += reject.Text();
            pApp->ProcessErrorEx(MODE_ERR_BUSINESS, "Unable to find this order",
                totalMessage );
        }
    } catch (CDBException *e) {
        Error.Format("EXCEPTION: Error Processing Incoming Reject Message (%s): %s", e->m_strError, reject.Debug());
        pApp->ProcessError("ILinkGlobex Handler", Error);
        e->Delete();
    }
}

//**************************************************************************
// 
//****************************************************************************
void CILinkGlobexOrderHandler::ProcessMessage(CReplace& replace)
{
    CMessageApp *pApp = (CMessageApp *) AfxGetApp();	
    CString cancelTag;
   /* if(m_AllowedFirmIDs.find(replace.ClearingId()) == m_AllowedFirmIDs.end()) {
        CString rejectMessage = "Unrecognized Clearing ID: ";
        rejectMessage += replace.ClearingId();
        
        // Generate Reject
        CReplaceReject Reject(rejectMessage, replace);		
        
        // Update Status
        pApp->Status().SetStatus(Reject);
        pApp->ProcessDebug("SEND REJECT", "");
        return;
        
    }*/
    
    try {
        if(m_pTags->GetTags(MAP_STAFFORD_TAG_QUERY, replace.StaffordTag())) {
            m_orderIdGenerator.NextOrderId(cancelTag);
            replace.SetClearingId(m_pTags->GetClearingId());
            replace.SetClearingAccount(m_pTags->GetClearingAccount());
            replace.SetExchangeTag(m_pTags->ExchangeTag());
            replace.SetOrderTag(m_pTags->OrderTag());
            replace.SetCancelTag(cancelTag);
            replace.SetShares(m_pTags->OriginalShares() - m_pTags->Shares() + replace.Shares());
            replace.SetOriginalShares(m_pTags->OriginalShares());
            replace.SetOriginalPrice(m_pTags->OriginalPrice());
            double price = replace.Price() * 100;
            replace.SetPrice(price);
            double stopPrice = replace.StopPrice() * 100;
            replace.SetStopPrice(stopPrice);
            
            if( pApp->Exchange().ProcessReplace(replace) ) {
                m_pTags->SetCancelTag(replace.CancelTag());
                
                pApp->ProcessDebug("REPLACE SENT", "");
            } else {
                // Generate Reject
                CReplaceReject Reject("Unable To Send", replace);		
                Reject.SetSourceId(m_pTags->Client());
                // Update Status
                pApp->Status().SetStatus(Reject);
                pApp->ProcessDebug("SEND REJECT", "");
            }
        }
    } catch (CDBException *e) {
        CString Error;
        Error.Format("EXCEPTION: Error Processing Outgoing Message (%s): %s", e->m_strError, "");
        pApp->ProcessError("ILINKGLOBEX Handler", Error);
        e->Delete();
    } catch (const char *Message) {
        pApp->ProcessError("ILINKGLOBEX Handler", Message);
    }
}

//**************************************************************************
// 
//****************************************************************************
void CILinkGlobexOrderHandler::ProcessMessage(CReplaceReject& reject)
{
    CMessageApp *pApp = (CMessageApp *)AfxGetApp();
    CString origOrderTag = reject.CancelTag();
    
    pApp->ProcessDebug("RECV STATUS", reject.Debug());
    
    if (FALSE == m_pTags->GetTags(MAP_CANCEL_TAG_QUERY, origOrderTag))
    {
        /*std::stringstream error;
        
        error << "Unable To Find Order Tag (" << (LPCSTR) origOrderTag << ") ";
        error << "In Tag Map For OrderId (" << (LPCSTR) reject.StaffordTag() << ") :" ;
        error << "(" << (LPCSTR) reject.Debug() << ")";
        
        pApp->ProcessErrorEx(MODE_ERR_BUSINESS, "ILINKGLOBEX Handler", error.str().c_str());*/
        return;
    }
    reject.SetStaffordTag(m_pTags->StaffordTag());
    reject.SetSourceId(m_pTags->Client());
    
    // update Status
    pApp->Status().SetStatus(reject);
}
//**************************************************************************
// 
//****************************************************************************
void CILinkGlobexOrderHandler::ProcessMessage(CCancelReject& cancelReject)
{
    CMessageApp *pApp = (CMessageApp *)AfxGetApp();
    CString origOrderTag;
    
    GetOrigOrderId(origOrderTag, const_cast<CString&>(cancelReject.CancelTag()));
    
    pApp->ProcessDebug("RECV STATUS", cancelReject.Debug());
    
    if (FALSE == m_pTags->GetTags(MAP_CANCEL_TAG_QUERY, origOrderTag))
    {
       /* std::stringstream error;
        
        error << "Unable To Find Order Tag (" << (LPCSTR) origOrderTag << ") ";
        error << "In Tag Map For OrderId (" << (LPCSTR) cancelReject.StaffordTag() << ") :" ;
        error << "(" << (LPCSTR) cancelReject.Debug() << ")";
        
        pApp->ProcessErrorEx(MODE_ERR_BUSINESS, "ILINKGLOBEX Handler", error.str().c_str());*/
        return;
    }
    m_pTags->SetPendingCancel(false);
    
    cancelReject.SetStaffordTag(m_pTags->StaffordTag());
    cancelReject.SetSourceId(m_pTags->Client());
    
    // update Status
    pApp->Status().SetStatus(cancelReject);
}
//**************************************************************************
// 
//****************************************************************************
void CILinkGlobexOrderHandler::ProcessMessage(const char *Status)
{
}

//**************************************************************************
// We need this function because the CME only sends back the last 5 digits
// of an CIOrderID (FIX tag 11) on a cancel or cancel replace. We "know" that 
// we only have orders open for today so we add on the the current date.
//****************************************************************************
void CILinkGlobexOrderHandler::GetOrigOrderId(CString& origOrderId, const CString& orderTag)
{
    origOrderId += CTime::GetCurrentTime().Format("%m%d");
    if(orderTag.GetLength() >= 4) {
        origOrderId += orderTag.Right(4);
    }
}

void CILinkGlobexOrderHandler::LoadTradeFor(CString& tradeFor, const char* staffordTag) 
{
    if(m_pTags->GetTags(MAP_STAFFORD_TAG_QUERY, staffordTag)) {
        tradeFor = m_pTags->GetTradeFor();
    }
}