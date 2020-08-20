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
*     Parnell     
*
*******************************************************************************/
#include "stdafx.h"
#include "ArcaDirect.h"
#include "ArcaDirectHandler.h"
#include "SendCancelReject.h"
#include <SSTREAM>


#include "DatabaseSettingsFactory.h"
#include "DatabaseSettingsInterface.h"


/*!
Creates a new ArcaDirectHandler which really means
initializing the memory map. 
*/
ArcaDirectHandler::ArcaDirectHandler()
{
    CString		Error;
    CMessageApp *pApp = (CMessageApp *)AfxGetApp();
    InitializeArcaDirectMemoryMap();
    // get the default clearing account
    GetPrivateProfileString("Clearing Information",
        "Default Clearing Code",	"",
        m_DefaultAccount.GetBuffer(128), 128,
        pApp->Path() + pApp->Name() + ".DAT");
    
    m_DefaultAccount.ReleaseBuffer();
    
    m_orderTypeValidator = new COrderTypeValidator(
        std::string((LPCSTR) pApp->DatFile()));
    
    
}

ArcaDirectHandler::~ArcaDirectHandler()
{
    /*Free the memory mapped file.*/
    delete m_pTags;
    
    if (m_orderTypeValidator != NULL) {
        delete m_orderTypeValidator;
    }
}
/*!
Reads the information about the database connection from the application Dat file.
Then it creates the Memory mapped file (either from the database or using what is 
in memory).  
*/
void ArcaDirectHandler::InitializeArcaDirectMemoryMap()
{
    CString exchangeDestinationId;
    
    CString dbUser;
    CString dbPassword;
    CString	dbServer;
    CMessageApp *pApp;
    
    pApp = (CMessageApp *)AfxGetApp();
    
    GetPrivateProfileString(pApp->Name() + " Server",
        "Exchange Destination Id",
        "",
        exchangeDestinationId.GetBuffer(128),
        128,
        pApp->DatFile());
    
    exchangeDestinationId.ReleaseBuffer();
    
    GetPrivateProfileString("Order Database Information",
        "User",
        "",
        dbUser.GetBuffer(128),
        128,
        pApp->DatFile());
    
    dbUser.ReleaseBuffer();
    
    GetPrivateProfileString("Order Database Information",
        "Password",
        "",
        dbPassword.GetBuffer(128),
        128,
        pApp->DatFile());
    
    dbPassword.ReleaseBuffer();
    
    GetPrivateProfileString("Order Database Information",
        "Server",
        "",
        dbServer.GetBuffer(128),
        128,
        pApp->DatFile());
    
    dbServer.ReleaseBuffer();
    
    bool forceReload = GetPrivateProfileInt("MemoryMapSettings", "ForceReload", 0, pApp->DatFile()) == 0 ? false : true;
    
    DatabaseSettings::TIDatabaseSettingsAutoPtr lDatabaseSettings =
        DatabaseSettings::CDatabaseSettingsFactory::CreateDatabaseSettings();
    
    lDatabaseSettings->AppName((LPCSTR) pApp->Name());
    lDatabaseSettings->DestinationId(
        (LPCSTR) exchangeDestinationId);
    lDatabaseSettings->Server((LPCSTR) dbServer);
    lDatabaseSettings->User((LPCSTR) dbUser);
    lDatabaseSettings->Password((LPCSTR) dbPassword);
    
    m_pTags = new ArcaDirectMemTagSet(lDatabaseSettings,
        forceReload);   
}


/////////////////////////////////////////////////////////////////////////////////////////////
// from client to ArcaDirect
/*!
Process orders received from OM and send them on to ArcaDirect.   
*/
void ArcaDirectHandler::ProcessMessage(COrder& Order)
{
    long		SequenceNumber	= 0;
    CString		Error			= "";
    CString		Account;
    CArcaDirectApp *pApp= (CArcaDirectApp*)AfxGetApp();	
    
    if ((pApp->GetDirect())->LoggedOn() != TRUE) {
        // generate reject message
        CReject	Reject("Unable To Send", Order);			
        // map tags
        m_pTags->CreateNewMap(Order.StaffordTag(),
            Order.OrderTag(), -1, Order.SourceId());
        // update Status
        pApp->Status().SetStatus(Reject);
        pApp->ProcessDebug("ORDER REJECT", Order.Debug());
        return;
    }
    
    try {
        /*Test to see if we have a valid time in force and type.*/
        m_orderTypeValidator->IsSupportedTIF(
            (COrder::TimeInForce) (Order.TIF()));
        m_orderTypeValidator->IsSupportedOrderType(
            (COrder::OrderType) (Order.Type()));
    } catch(CTIFException e) {
        std::stringstream reason;   
        reason << "Unsupported Time In Force: " << e.TimeInForce();       
        CReject	Reject(reason.str().c_str(), Order);
        pApp->Status().SetStatus(Reject);
        pApp->ProcessDebug("ORDER REJECT", Order.Debug());
        return;
    } catch(COrderTypeException e) {
        std::stringstream reason;       
        reason << "Unsupported Order Type: " << e.OrderType();       
        CReject	Reject(reason.str().c_str(), Order);
        pApp->Status().SetStatus(Reject);
        pApp->ProcessDebug("ORDER REJECT", Order.Debug());
        return;
    } catch(...) {
    /*TODO Do we need this generic catch all??  And if so what should we
        do in it?*/
    }
    
    try {
        char orderIDTag[12];
        /*Set this as the unique tag for this cancel.*/
        ((ArcaDirectSession&)pApp->Exchange()).GetClientOrderId(orderIDTag);       
        /*The working order id at the exchange is always in the cancel tag.*/
        Order.SetCancelTag(orderIDTag);
        Order.SetOrderTag(orderIDTag);
        // lookup and set traders clearing account code
        if (Order.ClearingId() == "") {
            Order.SetClearingId(m_DefaultAccount);
        }
        
        // create and send binary order off to ArcaDirect.
        if ((SequenceNumber = 
            pApp->Exchange().ProcessEntry(Order)) > 0) {
            // map tags
            m_pTags->CreateNewMap(
                Order.StaffordTag(),
                Order.OrderTag(),
                SequenceNumber,
                Order.SourceId(),
                " ",
                /*Setting the cancel tag to the order tag.*/
                Order.OrderTag());
            m_pTags->SetSource(Order.Owner());
            m_pTags->SetShares(Order.Shares());
            m_pTags->SetExecutedShares(0);
            m_pTags->SetDate(
                CTime::GetCurrentTime().Format("%B%d%Y"));
            m_pTags->SetOrderTag(Order.OrderTag());
            m_pTags->SetCancelTag(Order.CancelTag());
            m_pTags->SetPrice(Order.dPrice());
            // update status
            pApp->Status().SetStatus(Order);
            pApp->ProcessDebug("ORDER SENT", Order.Debug());
        } else {
            // generate reject message
            CReject	Reject("Unable To Send", Order);			
            // map tags
            m_pTags->CreateNewMap(
                Order.StaffordTag(),
                Order.OrderTag(), -1, Order.SourceId());
            
            // update Status
            Reject.SetOwner(Order.Owner());
            Reject.SetSourceId(Order.SourceId());
            pApp->Status().SetStatus(Reject);
            pApp->ProcessDebug("ORDER REJECT",
                Order.Debug());
        }
    } catch (CDBException *e) {
        Error.Format(
            "DB EXCEPTION: Error Processing Outgoing Message (%s): %s", e->m_strError, Order.Debug());
        pApp->ProcessErrorEx(
            MODE_ERR_DEVELOPER,"ArcaDirect Handler", Error);
        e->Delete();
    } catch (const char *Message) {
        pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"ArcaDirect Handler",
            Message);
    }	
}

void ArcaDirectHandler::ProcessMessage(CReplace& Replace)
{
    CMessageApp *pApp = (CMessageApp *) AfxGetApp();	
    try {
        if(m_pTags->GetTags(MAP_STAFFORD_TAG_QUERY,
            Replace.StaffordTag())) {
            // set the exchange's tag an the arcaDirect order tag
            Replace.SetExchangeTag(m_pTags->ExchangeTag());
            // Always take the cancel tag as the order to replace.
            // The cancel tag is defaulted to the order tag when
            // the order is created but the cancel tag is updated
            // in replace scenarios when the order is successfully
            // replaced.  Therefore the current working order id at
            // the exchange is always in the cancel tag.
            Replace.SetOrderTag(m_pTags->CancelTag());
            Replace.SetShares(Replace.Shares() + Replace.CumShares());
            Replace.SetOriginalShares(Replace.OriginalShares());
            Replace.SetOriginalPrice(Replace.OriginalPrice());
            
            m_pTags->SetDate(
                CTime::GetCurrentTime().Format("%B%d%Y"));
            
            char orderIDTag[12];
            /*Set this as the unique tag for this cancel.*/
            ((ArcaDirectSession&)pApp->Exchange()).GetClientOrderId(orderIDTag);
            /*The working order id at the exchange is always in the cancel tag.*/
            Replace.SetCancelTag(orderIDTag);
            m_pTags->SetSource(Replace.Owner());
            if(pApp->Exchange().ProcessReplace(Replace)) {
                m_pTags->SetCancelTag(Replace.CancelTag());
                long repShares = Replace.Shares();
                long tagShares = m_pTags->ExecutedShares();
                if(tagShares < repShares) {
                    m_pTags->SetShares(repShares - tagShares);
                }
                m_pTags->SetPrice(Replace.Price());
                pApp->ProcessDebug("REPLACE SENT", "");
                pApp->Status().SetStatus(Replace);
            } else {
                // generate reject
                CReject	Reject("Unable To Send", Replace);
                m_pTags->SetCancelTag(Replace.CancelTag());
                
                // update Status
                pApp->Status().SetStatus(Reject);
                pApp->ProcessDebug("SEND REJECT", "");
            }
        }
    } catch (CDBException *e) {
        CString Error;
        Error.Format(
            "EXCEPTION: Error Processing Outgoing Message (%s): %s", e->m_strError, "");
        pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"BRUT Handler", Error);
        e->Delete();
    } catch (const char *Message) {
        pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"BRUT Handler", Message);
    }
}



void ArcaDirectHandler::ProcessMessage(CCancel& Cancel)
{
    CString		Error;
    CMessageApp *pApp = (CMessageApp *)AfxGetApp();	
    
    try {
        if (m_pTags->GetTags(MAP_STAFFORD_TAG_QUERY, Cancel.StaffordTag())) {
            // set the exchange's tag an the fix order tag
            Cancel.SetExchangeTag(m_pTags->ExchangeTag());
            Cancel.SetOrderTag(m_pTags->OrderTag());
            char orderIDTag[12];
            /*Set this as the unique tag for this cancel.*/
            ((ArcaDirectSession&)pApp->Exchange()).GetClientOrderId(orderIDTag);
            /*The working order id at the exchange is always in the cancel tag.*/
            Cancel.SetCancelTag(orderIDTag);
            if (pApp->Exchange().ProcessCancel(Cancel)) {
                // set the cancel tag which is generated by the arcaDirect session
                m_pTags->SetCancelTag(Cancel.CancelTag());
                m_pTags->SetSource(Cancel.Owner());
                
                // update Status
                pApp->Status().SetStatus(Cancel);
                pApp->ProcessDebug("CANCEL SENT", Cancel.Debug());
            } else {
                // generate reject
                CSendCancelReject Reject(Cancel);		
                // set the cancel tag which is generated by the fix session
                m_pTags->SetCancelTag(Cancel.CancelTag());
                
                // update Status
                pApp->Status().SetStatus(Reject);
                pApp->ProcessDebug("SEND REJECT", Cancel.Debug());
            }
        }
    } catch (CDBException *e) {
        Error.Format("EXCEPTION: Error Processing Outgoing Message (%s): %s", e->m_strError,
            Cancel.Debug());
        pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"ArcaDirect Handler",
            Error);
        e->Delete();
    } catch (const char *Message) {
        pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"ArcaDirect Handler",
            Message);
    }
}

void ArcaDirectHandler::ProcessMessage(CExecution& Execution)
{
    CString		Error;
    CArcaDirectApp	*pApp = (CArcaDirectApp *)AfxGetApp();		
    
    try {	
        if (Execution.OrderTag() != "") {
            if (m_pTags->GetTags(MAP_ORDER_TAG_QUERY, Execution.OrderTag())) {
                Execution.SetCancelTag(m_pTags->CancelTag());
                SetExecutionStatus(Execution);
            } else if(m_pTags->GetTags(MAP_CANCEL_TAG_QUERY, Execution.CancelTag())) {
                Execution.SetOrderTag(m_pTags->OrderTag());
                SetExecutionStatus(Execution);		
            } else {
                Error.Format("Unable To Find Order Tag (%s) In Tag Map For OrderId(%s): (%s)",
                    Execution.OrderTag(), Execution.StaffordTag(), Execution.Debug());
                pApp->ProcessErrorEx(MODE_ERR_BASIC,"ArcaDirect Handler", Error);
            }
        } else if(Execution.CancelTag() != "") {
            if(m_pTags->GetTags(MAP_CANCEL_TAG_QUERY, Execution.CancelTag())) {
                Execution.SetOrderTag(m_pTags->OrderTag());
                SetExecutionStatus(Execution);
            }
        }
    } catch (CDBException *e) {
        Error.Format("EXCEPTION: Error Processing Incoming Status Message (%s): %s", e->m_strError, Execution.Debug());
        pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"ArcaDirect Handler", Error);
        e->Delete();
    }
}

void ArcaDirectHandler::SetExecutionStatus(CExecution& Execution)
{
    CArcaDirectApp	*pApp = (CArcaDirectApp *)AfxGetApp();		
    // set exchange tag if needed
    if(Execution.ExchangeTag() != "") {
        m_pTags->SetExchangeTag(Execution.ExchangeTag());
    }
    if(Execution.Status() == COrder::Filled) {
        int total = m_pTags->Shares() - Execution.LastShares();
        m_pTags->AddToExecutedShares(Execution.LastShares());
        if(total > 0) {
            Execution.SetStatus(COrder::PartiallyFilled);
            m_pTags->SetShares(total);
            Execution.SetLeaves((total));
        }
        Execution.SetShares(m_pTags->ExecutedShares());
        Execution.SetCumulative(m_pTags->ExecutedShares());
    }
    // get the stafford tag based off of the order tag
    Execution.SetStaffordTag(m_pTags->StaffordTag());
    Execution.SetSourceId(m_pTags->Client());
    
    if(Execution.Status() == COrder::Replaced) {
        Execution.SetShares(m_pTags->Shares());
        Execution.SetPrice(m_pTags->Price());
        m_pTags->SetOrderTag(Execution.CancelTag());
    }
    
    Execution.SetOwner(m_pTags->Source());
    pApp->ProcessDebug("RECV STATUS", Execution.Debug());
    
    switch(Execution.Status())
    {
    case COrder::Canceled:
    case COrder::Filled:
    case COrder::DoneForDay:
    case COrder::Rejected:
    case COrder::Stopped:
        m_pTags->RemoveTags();
        break;
    }
    
    // update Status
    pApp->Status().SetStatus(Execution);
}

void ArcaDirectHandler::ProcessMessage(CCancelReject& Reject)
{
    CString		Error;
    CMessageApp *pApp = (CMessageApp *)AfxGetApp();		
    
    try {
        pApp->ProcessDebug("RECV STATUS", Reject.Debug());
        
        if (m_pTags->GetTags(MAP_ORDER_TAG_QUERY, Reject.OrderTag())) {
            // set the stafford tag based off of the order tag
            Reject.SetStaffordTag(m_pTags->StaffordTag());
            Reject.SetCancelTag(m_pTags->CancelTag());
            if((*(m_pTags->Source()) == '\0')) {
                Reject.SetOwner("OM");
            } else {
                Reject.SetOwner(m_pTags->Source());
            }
            Reject.SetSourceId(m_pTags->Client());
            // update Status
            pApp->Status().SetStatus(Reject);
            //m_pTags->Close();
        }
    } catch (CDBException *e) {
        Error.Format("EXCEPTION: Error Processing Incoming Cancel Reject Message (%s): %s", e->m_strError, Reject.Debug());
        pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"ArcaDirect Handler", Error);
        e->Delete();
    }
}


void ArcaDirectHandler::ProcessMessage(CReplaceReject& Reject)
{
    CString		Error;
    CMessageApp *pApp = (CMessageApp *)AfxGetApp();		
    
    try {
        pApp->ProcessDebug("RECV STATUS", Reject.Debug());
        
        if (m_pTags->GetTags(MAP_ORDER_TAG_QUERY, Reject.OrderTag())) {
            // set the stafford tag based off of the order tag
            Reject.SetStaffordTag(m_pTags->StaffordTag());
            Reject.SetCancelTag(m_pTags->CancelTag());
            if(!(m_pTags->Source())) {
                Reject.SetOwner("OM");
            } else {
                Reject.SetOwner(m_pTags->Source());
            }
            Reject.SetSourceId(m_pTags->Client());
            // update Status
            pApp->Status().SetStatus(Reject);
            //m_pTags->Close();
        }
    } catch (CDBException *e) {
        Error.Format("EXCEPTION: Error Processing Incoming Replace Reject Message (%s): %s", e->m_strError, Reject.Debug());
        pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"ArcaDirect Handler", Error);
        e->Delete();
    }
}

void ArcaDirectHandler::ProcessMessage(CReject& Reject)
{
    CString	Error;
    CMessageApp *pApp = (CMessageApp *)AfxGetApp();		
    
    try {
        pApp->ProcessDebug("RECV STATUS", Reject.Debug());
        
        if (m_pTags->GetTags(MAP_ORDER_TAG_QUERY, Reject.OrderTag())) {
            // set tags
            //            Reject.SetExchangeTag(m_pTags->ExchangeTag());
            Reject.SetCancelTag(m_pTags->CancelTag());
            Reject.SetStaffordTag(m_pTags->StaffordTag());
            if((*(m_pTags->Source()) == '\0')) {
                Reject.SetOwner("OM");
            } else {
                Reject.SetOwner(m_pTags->Source());
            }
            Reject.SetSourceId(m_pTags->Client());
            // update Status
            pApp->Status().SetStatus(Reject);
            //m_pTags->Close();
        }
    } catch (CDBException *e) {
        Error.Format("EXCEPTION: Error Processing Incoming Reject Message (%s): %s", e->m_strError, Reject.Debug());
        pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"ArcaDirect Handler", Error);
        e->Delete();
    }
}


void ArcaDirectHandler::ProcessMessage(const char *Status)
{
}

/*
void ArcaDirectHandler::SetNextClientOrderID(int seq)
{
HKEY regHandle;
CString regSubKey = "Software\\Ronin\\ArcaDirect";
CString sSequence;
int len = regSubKey.GetLength();

  sSequence.Format("%d", 
  seq);
  
    if (RegCreateKey( HKEY_LOCAL_MACHINE, 
    (const char*)(regSubKey.GetBuffer(len)), 
    &regHandle) == ERROR_SUCCESS) {
    len = sSequence.GetLength();
    
      RegSetValueEx( regHandle, 
      "NextClientOrderID",
      0,
      REG_SZ,
      (const unsigned char*)(sSequence.GetBuffer(len)),
      sSequence.GetLength() + 1);
      
        RegCloseKey(regHandle);
        }
        m_clientOrderID = seq;
        }
        
          int ArcaDirectHandler::GetNextClientOrderID()
          {
          HKEY regHandle;
          CString regSubKey = "Software\\Ronin\\ArcaDirect";
          int len = regSubKey.GetLength();
          
            int rc = 1;
            
              if (RegCreateKey( HKEY_LOCAL_MACHINE, 
              (const char*)(regSubKey.GetBuffer(len)), 
              &regHandle) == ERROR_SUCCESS) {
              DWORD type = REG_SZ;
              char sBuff[64];
              DWORD size = sizeof(sBuff);
              
                long k = RegQueryValueEx(regHandle,
                "NextClientOrderID",
                0,
                &type,
                (unsigned char*)sBuff,
                &size);
                
                  if (k != ERROR_SUCCESS) {
                  CString sSequence = "1";
                  
                    RegSetValueEx(	regHandle, 
                    "NextIncomingSequence",
                    0,
                    REG_SZ,
                    (const unsigned char*)(sSequence.GetBuffer(1)),
                    2);
                    } else {
                    rc = atoi(sBuff);
                    }
                    
                      RegCloseKey(regHandle);
                      }
                      
                        return rc;
                        }
                        
*/