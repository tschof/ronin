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
 * 11-11-2005   Parnell     Added the InitializeMemoryMap function and the overloaded
 *                          Process(CReplace).
 *******************************************************************************/

#include "stdafx.h"
#include "MessageApp.h"
#include "BRUTHandler.h"
#include "FixSession.h"
#include "SendCancelReject.h"
#include <sstream>


#include "DatabaseSettingsFactory.h"
#include "DatabaseSettingsInterface.h"


extern CString	Exchange;
extern CTime	CurrentTime;

#pragma warning(disable:4786)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBRUTHandler::CBRUTHandler()
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();
    InitializeBrutMemoryMap();

	m_orderTypeValidator = new COrderTypeValidator(std::string((LPCSTR) pApp->DatFile()));
	// get the default clearing account
	GetPrivateProfileString("Clearing Information",	"Default Clearing Code",
        "", m_DefaultAccount.GetBuffer(128), 128,	pApp->Path() + pApp->Name() + ".DAT");
    m_DefaultAccount.ReleaseBuffer();
}

CBRUTHandler::~CBRUTHandler()
{
	delete m_pTags;

	if (m_orderTypeValidator != NULL)
	{
		delete m_orderTypeValidator;
	}
}


void CBRUTHandler::InitializeBrutMemoryMap()
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

	bool forceReload = GetPrivateProfileInt("MemoryMapSettings", "ForceReload",
        0, pApp->DatFile()) == 0 ? false : true;

    DatabaseSettings::TIDatabaseSettingsAutoPtr lDatabaseSettings =
        DatabaseSettings::CDatabaseSettingsFactory::CreateDatabaseSettings();

	lDatabaseSettings->AppName((LPCSTR) pApp->Name());
	lDatabaseSettings->DestinationId((LPCSTR) exchangeDestinationId);
	lDatabaseSettings->Server((LPCSTR) dbServer);
	lDatabaseSettings->User((LPCSTR) dbUser);
	lDatabaseSettings->Password((LPCSTR) dbPassword);

	m_pTags = new CBrutMemTagSet(lDatabaseSettings, forceReload);   
}


/////////////////////////////////////////////////////////////////////////////////////////////
// from client to fix

void CBRUTHandler::ProcessMessage(COrder& Order)
{
	long		SequenceNumber	= 0;
	CString		Error			= "";
	CString		Account;
	CMessageApp *pApp			= (CMessageApp *)AfxGetApp();	

	if ( (pApp->Host()).LoggedOn() != TRUE)
	{
		// generate reject message
		CReject	Reject("Unable To Send", Order);			
		// map tags
		m_pTags->CreateNewMap(Order.StaffordTag(), Order.OrderTag(), -1, Order.SourceId());
		// update Status
		pApp->Status().SetStatus(Reject);
		pApp->ProcessDebug("ORDER REJECT", Order.Debug());
		return;
	}
    if(IsNASDSym(Order.lpszSymbol())) {
        CReject	Reject("Unable To Send NASD Symbols.", Order);
        pApp->Status().SetStatus(Reject);
        return;
    }
	try
	{

		m_orderTypeValidator->IsSupportedTIF((COrder::TimeInForce) (Order.TIF()));
		m_orderTypeValidator->IsSupportedOrderType((COrder::OrderType) (Order.Type()));
	}
	catch(CTIFException e)
	{
		std::stringstream reason;

		reason << "Unsupported Time In Force: " << e.TimeInForce();

		CReject	Reject(reason.str().c_str(), Order);
		pApp->Status().SetStatus(Reject);
		pApp->ProcessDebug("ORDER REJECT", Order.Debug());
		return;
	}
	catch(COrderTypeException e)
	{
		std::stringstream reason;

		reason << "Unsupported Order Type: " << e.OrderType();

		CReject	Reject(reason.str().c_str(), Order);
		pApp->Status().SetStatus(Reject);
		pApp->ProcessDebug("ORDER REJECT", Order.Debug());
		return;
	}
	catch(...)
	{
	}

	try
	{
		if (*Order.lpszExDestination() == 'O')
			Order.SetExDestination("");
		// lookup and set traders clearing account code
		if (Order.ClearingId() == "")
			Order.SetClearingId(m_DefaultAccount);	
		
		// No Date For BRUT Sequence, must be less than 8 characters.
		char buffer[9];
		sprintf(buffer, "%d", ((CFixSession&) pApp->Exchange()).GetNextOutgoingSeqNum() + 100000);
		Order.SetOrderTag(buffer);
		
		// create and send fix order off to BRUT
		if ((SequenceNumber = pApp->Exchange().ProcessEntry(Order)) > 0)
		{
			// map tags
			m_pTags->CreateNewMap(Order.StaffordTag(), Order.OrderTag(), SequenceNumber, Order.SourceId());
			m_pTags->SetShares(Order.Shares());
		    m_pTags->SetCancelReplaceCnt(0);
            m_pTags->SetDate(CTime::GetCurrentTime().Format("%B%d%Y"));
			// update status
			pApp->Status().SetStatus(Order);
			pApp->ProcessDebug("ORDER SENT", Order.Debug());
		}
		else
		{
			// generate reject message
			CReject	Reject("Unable To Send", Order);			
			// map tags
			m_pTags->CreateNewMap(Order.StaffordTag(),
                Order.OrderTag(), -1, Order.SourceId());
            m_pTags->SetDate(CTime::GetCurrentTime().Format("%B%d%Y"));
			// update Status
			pApp->Status().SetStatus(Reject);
			pApp->ProcessDebug("ORDER REJECT", Order.Debug());
		}
	}

	catch (CDBException *e)
	{
		Error.Format("DB EXCEPTION: Error Processing Outgoing Message (%s): %s", e->m_strError, Order.Debug());
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"BRUT Handler", Error);
		CReject	Reject("Unable To Send", Order);			
		// update Status
		pApp->Status().SetStatus(Reject);
		pApp->ProcessDebug("ORDER REJECT", Order.Debug());
		e->Delete();
	}
	catch (const char *Message)
	{
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"BRUT Handler", Message);
	}

}

void CBRUTHandler::ProcessMessage(CReplace& Replace)
{
	CMessageApp *pApp = (CMessageApp *) AfxGetApp();	
	try
	{
        if(m_pTags->GetTags(MAP_STAFFORD_TAG_QUERY, Replace.StaffordTag()))
        {
            // set the exchange's tag an the fix order tag
            Replace.SetExchangeTag(m_pTags->ExchangeTag());
            CString formerTag = m_pTags->CancelTag();
            Replace.SetOrderTag(formerTag);
            CreateReplaceTag(formerTag,m_pTags->CancelReplaceCnt() + 1);
            Replace.SetReplaceTag(formerTag);
            Replace.SetShares(Replace.Shares() + Replace.CumShares());
            Replace.SetOriginalShares(Replace.OriginalShares());
            Replace.SetOriginalPrice(Replace.OriginalPrice());
            
            m_pTags->SetCancelReplaceCnt(m_pTags->CancelReplaceCnt() + 1);	
            m_pTags->SetDate(CTime::GetCurrentTime().Format("%B%d%Y"));
            if(pApp->Exchange().ProcessReplace(Replace))
            {
                m_pTags->SetCancelTag(Replace.CancelTag());
                
                pApp->ProcessDebug("REPLACE SENT", "");
                pApp->Status().SetStatus(Replace);
            }
			else
			{
				// generate reject
				CReject	Reject("Unable To Send", Replace);		
				// update Status
				pApp->Status().SetStatus(Reject);
				pApp->ProcessDebug("SEND REJECT", "");
			}
		}
	}
	catch (CDBException *e)
	{
		CString Error;
		Error.Format("EXCEPTION: Error Processing Outgoing Message (%s): %s", e->m_strError, "");
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"BRUT Handler", Error);
		e->Delete();
	}
	catch (const char *Message)
	{
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"BRUT Handler", Message);
	}
}

void CBRUTHandler::CreateReplaceTag(CString& orderTag, int sequenceNumber) 
{	
	int index;
	
	if (-1 == (index = orderTag.Find('R')))
	{
		// This order has not been replaced.
		// Create a replaced order tag
		CString str;
		
		str.Format("%s%ld", "R", sequenceNumber);
		
		orderTag += str;
	}
	else
	{
		// This order has been replaced add the new sequence number
		CString str;
		CString newOrderTag;
		
		// This includes the "R"
		str = orderTag.Mid(0, index + 1);
		
		// Now just add the new sequence number
		newOrderTag.Format("%s%ld", str, sequenceNumber);
		
		orderTag = newOrderTag;
	}
}


void CBRUTHandler::ProcessMessage(CCancel& Cancel)
{
	CString		Error;
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();		
	try
	{
		if (m_pTags->GetTags(MAP_STAFFORD_TAG_QUERY, Cancel.StaffordTag()))
		{
            m_pTags->SetDate(CTime::GetCurrentTime().Format("%B%d%Y"));
			// set the exchange's tag an the fix order tag
			Cancel.SetExchangeTag(m_pTags->ExchangeTag());
			Cancel.SetOrderTag(m_pTags->OrderTag());
            int canTag = ((CFixSession&) pApp->Exchange()).GetNextOutgoingSeqNum() + 100000;
			const CString stringCanTag = Cancel.SetBrutCancelTag(canTag);
            
			Cancel.SetInstructions(m_pTags->Date());

			if (pApp->Exchange().ProcessCancel(Cancel))
			{
			    m_pTags->SetCancelTag(stringCanTag);
                // update Status
				pApp->Status().SetStatus(Cancel);
				pApp->ProcessDebug("CANCEL SENT", Cancel.Debug());
			}
			else
			{
				// generate reject
				CSendCancelReject	Reject(Cancel);		
				
				// update Status
				pApp->Status().SetStatus(Reject);
				pApp->ProcessDebug("SEND REJECT", Cancel.Debug());
			}
		}
	}

	catch (CDBException *e)
	{
		Error.Format("EXCEPTION: Error Processing Outgoing Message (%s): %s", e->m_strError, Cancel.Debug());
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"BRUT Handler", Error);
		e->Delete();
	}
	catch (const char *Message)
	{
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"BRUT Handler", Message);
	}
}

void CBRUTHandler::ProcessMessage(CExecution& Execution)
{
	CString		Error;
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();		
	
	try
	{	
		if (Execution.Text().Find("CXL") >= 0)
			return;		// don't care about a cancel ack (pending cancel)
		
		if(Execution.Status() == COrder::Replaced) 
		{
			if (m_pTags->GetTags(MAP_ORDER_TAG_QUERY, Execution.OriginalTag()))
			{
				// set new order tag
				m_pTags->SetOrderTag(Execution.OrderTag());

				// set exchange tag if needed
				if (strcmp((LPCTSTR)(Execution.ExchangeTag()), m_pTags->ExchangeTag()))
					m_pTags->SetExchangeTag(Execution.ExchangeTag());
				
				// get the stafford tag based off of the order tag
				Execution.SetStaffordTag(m_pTags->StaffordTag());
				
				pApp->ProcessDebug("RECV STATUS", Execution.Debug());
				
				// update Status
				pApp->Status().SetStatus(Execution);
			}
			else
			{
				Error.Format("Unable To Find Order Tag (%s) In Tag Map For OrderId(%s): (%s)", Execution.OrderTag(), Execution.StaffordTag(), Execution.Debug());
				pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"BRUT Handler", Error);
			}
		}
		else 
		{
			if (m_pTags->GetTags(MAP_ORDER_TAG_QUERY, Execution.OrderTag()))
			{
				// set exchange tag if needed
				if (strcmp((LPCTSTR)(Execution.ExchangeTag()), m_pTags->ExchangeTag()))
					m_pTags->SetExchangeTag(Execution.ExchangeTag());

				// get the stafford tag based off of the order tag
				Execution.SetStaffordTag(m_pTags->StaffordTag());
				pApp->ProcessDebug("RECV STATUS", Execution.Debug());

				// set order date
				switch (Execution.Status())
				{
				case COrder::Open:
					((CMemTagSet*)m_pTags)->SetDate(Execution.Instructions());
					m_pTags->SetShares(Execution.Shares());
                    m_pTags->SetStatus(COrder::Open);
					break;
				case COrder::Filled:
					m_pTags->RemoveTags();
					break;
				case COrder::PartiallyFilled:
					m_pTags->SetShares(m_pTags->Shares() - Execution.LastShares());
                    m_pTags->SetStatus(COrder::PartiallyFilled);
					break;
				case COrder::Canceled:
                    m_pTags->RemoveTags();
                    break;
				case COrder::Rejected:
					if (Execution.Shares() >= m_pTags->Shares() ||
						Execution.Shares() == 0)
					{
                        int l_status = m_pTags->Status();
                        if(l_status == COrder::Open ||
                            l_status == COrder::PartiallyFilled ||
                            l_status == COrder::Replaced) {
                            Execution.SetStatus(COrder::Canceled);
                        }
                            
						m_pTags->RemoveTags();
					}
					else
					{
						m_pTags->SetShares(m_pTags->Shares() - Execution.Shares());
						Execution.SetStatus(COrder::Replaced);
                        m_pTags->SetStatus(COrder::Replaced);
						Execution.SetCancelTag(Execution.OrderTag());
					}
					break;
				default:
					break;
				}

				// update Status
				pApp->Status().SetStatus(Execution);
			}
			else
			{
				Error.Format("Unable To Find Order Tag (%s) In Tag Map For OrderId(%s): (%s)", Execution.OrderTag(), Execution.StaffordTag(), Execution.Debug());
				pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"BRUT Handler", Error);
			}
		}
	}
	catch (CDBException *e)
	{
		Error.Format("EXCEPTION: Error Processing Incoming Status Message (%s): %s", e->m_strError, Execution.Debug());
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"BRUT Handler", Error);
		e->Delete();
	}
}

void CBRUTHandler::ProcessMessage(CCancelReject& Reject)
{
	CString		Error;
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();		
	
	try
	{
		pApp->ProcessDebug("RECV STATUS", Reject.Debug());

		if (m_pTags->GetTags(MAP_CANCEL_TAG_QUERY, Reject.CancelTag()))
		{
			// set exchange tag if needed
			if ((Reject.ExchangeTag() != "") && (*(m_pTags->ExchangeTag()) == '\0'))
				m_pTags->SetExchangeTag(Reject.ExchangeTag());
			// set the stafford tag based off of the order tag
			Reject.SetStaffordTag(m_pTags->StaffordTag());
            //Reset the cancel tag since we were rejected.
            m_pTags->SetCancelTag(m_pTags->OrderTag());
			// update Status
			pApp->Status().SetStatus(Reject);
        } //else {
		    //pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"COULD NOT FIND TAG CX REJ ", Reject.CancelTag());
        //}
	}

	catch (CDBException *e)
	{
		Error.Format("EXCEPTION: Error Processing Incoming Cancel Reject Message (%s): %s", e->m_strError, Reject.Debug());
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"BRUT Handler", Error);
		e->Delete();
	}
}

void CBRUTHandler::ProcessMessage(CReject& Reject)
{
	CString	Error;
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();		
	
	try
	{
		pApp->ProcessDebug("RECV STATUS", Reject.Debug());

		if (m_pTags->GetTags(MAP_SEQUENCE_TAG_QUERY, Reject.OrderTag()))
		{
			// set tags
			Reject.SetOrderTag(m_pTags->OrderTag());
			Reject.SetExchangeTag(m_pTags->ExchangeTag());
			Reject.SetCancelTag(m_pTags->CancelTag());
			Reject.SetStaffordTag(m_pTags->StaffordTag());

			// update Status
			pApp->Status().SetStatus(Reject);
		}
	}

	catch (CDBException *e)
	{
		Error.Format("EXCEPTION: Error Processing Incoming Reject Message (%s): %s", e->m_strError, Reject.Debug());
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"BRUT Handler", Error);
		e->Delete();
	}
}

void CBRUTHandler::ProcessMessage(const char *Status)
{

}

bool CBRUTHandler::IsNASDSym(const char* symbol) 
{
   return (strlen(symbol) >= 4);
}