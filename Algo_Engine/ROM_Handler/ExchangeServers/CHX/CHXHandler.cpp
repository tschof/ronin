#include "stdafx.h"
#include "Replace.h"
#include "MessageApp.h"
#include "CHXHandler.h"
#include "SendCancelReject.h"
#include "FixSession.h"
#include "CHXMemTagSet.h"
#include "FixSession.h"
#include "CHXReject.h"

extern CString	Exchange;
extern CTime	CurrentTime;

#pragma warning(disable:4786)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCHXHandler::CCHXHandler()
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	//m_pTags	= new CTagMapSet(&pApp->Database());
	m_pTags = new CCHXMemTagSet;
	m_pTags->InitMap();

	m_orderTypeValidator = new COrderTypeValidator(std::string((LPCSTR) pApp->DatFile()));

	// get the default clearing account
	GetPrivateProfileString("Clearing Information",	"Default Clearing Code", "", m_DefaultAccount.GetBuffer(128), 128,	pApp->DatFile()); m_DefaultAccount.ReleaseBuffer();
}

CCHXHandler::~CCHXHandler()
{
	delete m_pTags;

	if (m_orderTypeValidator != NULL)
	{
		delete m_orderTypeValidator;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////
// from client to fix

void CCHXHandler::ProcessMessage(COrder& Order)
{
	long		SequenceNumber	= 0;
	CString		Error			= "";
	CString		OldOrderTag		= Order.OrderTag();
	CMessageApp *pApp			= (CMessageApp *)AfxGetApp();	

	if ( (pApp->Host()).LoggedOn() != TRUE)
	{
		CReject	Reject("Unable To Send", Order);			
		m_pTags->CreateNewMap(Order.StaffordTag(), OldOrderTag, -1, Order.SourceId());
		pApp->Status().SetStatus(Reject);
		pApp->ProcessDebug("ORDER REJECT", Order.Debug());
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
		// lookup and set traders clearing account code
		// CHX requires ClearingAccountID for the trader in Fix Tag 1
		// and FixSession assign ClearingID of COrder to Fix Tag 1
		// so we assign ClearingAccount passed from OM to ClearingID
		if (Order.ClearingAccount() == "")
		{
			Order.SetClearingId(m_DefaultAccount);
		}
		else
		{
			Order.SetClearingId(Order.ClearingAccount());
		}

		Order.SetOrderTag(((CFixSession&) pApp->Exchange()).GetNextOutgoingSeqNum());
		// create and send fix order off to CHX
		if ((SequenceNumber = pApp->Exchange().ProcessEntry(Order)) > 0)
		{
			// map tags
			m_pTags->CreateNewMap(Order.StaffordTag(), Order.OrderTag(), SequenceNumber, Order.SourceId());
			m_pTags->SetOriginalPrice(Order.dPrice());
			m_pTags->SetShares(Order.Shares());
			m_pTags->SetOriginalShares(Order.Shares());
			m_pTags->SetCancel(FALSE);

			// update status
			pApp->Status().SetStatus(Order);
			pApp->ProcessDebug("ORDER SENT", Order.Debug());
		}
		else
		{
			// generate reject message
			CReject	Reject("Unable To Send", Order);			
			// map tags
			m_pTags->CreateNewMap(Order.StaffordTag(), OldOrderTag, -1, Order.SourceId());
			// update Status
			pApp->Status().SetStatus(Reject);
			pApp->ProcessDebug("ORDER REJECT", Order.Debug());
		}
	}

	catch (CDBException *e)
	{
		Error.Format("DB EXCEPTION: Error Processing Outgoing Message (%s): %s", e->m_strError, Order.Debug());
		pApp->ProcessError("CHX Handler", Error);
		e->Delete();
	}
	catch (const char *Message)
	{
		pApp->ProcessError("CHX Handler", Message);
	}	
}

void CCHXHandler::ProcessMessage(CReplace& Replace)
{
	CMessageApp *pApp = (CMessageApp *) AfxGetApp();	
	CFixedString CancelTag;

	try
	{
		if(m_pTags->GetTags(MAP_STAFFORD_TAG_QUERY, Replace.StaffordTag()))
		{
			// set the exchange's tag an the fix order tag
			Replace.SetExchangeTag(m_pTags->ExchangeTag());
			Replace.SetOrderTag(m_pTags->OrderTag());
			CancelTag = CTime::GetCurrentTime().Format("%Y%m%d");
			CancelTag = CancelTag + (long)((CFixSession&) pApp->Exchange()).GetNextOutgoingSeqNum();
			Replace.SetCancelTag(CancelTag.GetCString());

			Replace.SetShares(m_pTags->OriginalShares() - m_pTags->Shares() + Replace.Shares());
			Replace.SetOriginalShares(m_pTags->OriginalShares());
			Replace.SetOriginalPrice(m_pTags->OriginalPrice());

			if( pApp->Exchange().ProcessReplace(Replace) )
			{
				m_pTags->SetCancelTag(Replace.CancelTag());
				
				pApp->ProcessDebug("REPLACE SENT", "");
			}
			else
			{
				// generate reject
				CReject Reject("Unable To Send", Replace);		
				// update Status
				pApp->Status().SetStatus(Reject);
				pApp->ProcessDebug("SEND REJECT", "");
			}
			//m_pTags->Close();
		}
	}
	catch (CDBException *e)
	{
		CString Error;
		Error.Format("EXCEPTION: Error Processing Outgoing Message (%s): %s", e->m_strError, "");
		pApp->ProcessError("CHX Handler", Error);
		e->Delete();
	}
	catch (const char *Message)
	{
		pApp->ProcessError("CHX Handler", Message);
	}
}

void CCHXHandler::ProcessMessage(CCancel& Cancel)
{
	CString		Error;
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();	

	try
	{
		if (m_pTags->GetTags(MAP_STAFFORD_TAG_QUERY, Cancel.StaffordTag()))
		{
			// set the exchange's tag an the fix order tag
			Cancel.SetExchangeTag(m_pTags->ExchangeTag());
			Cancel.SetOrderTag(m_pTags->OrderTag());
			Cancel.SetCancelTag(((CFixSession&) pApp->Exchange()).GetNextOutgoingSeqNum());
			if (pApp->Exchange().ProcessCancel(Cancel))
			{
				// set the cancel tag which is generated by the fix session
				m_pTags->SetCancelTag(Cancel.CancelTag());
				m_pTags->SetCancel(TRUE);
				// update Status
				pApp->Status().SetStatus(Cancel);
				pApp->ProcessDebug("CANCEL SENT", Cancel.Debug());
			}
			else
			{
				// generate reject
				CSendCancelReject	Reject(Cancel);		
				// set the cancel tag which is generated by the fix session
				m_pTags->SetCancelTag(Cancel.CancelTag());
				// update Status
				pApp->Status().SetStatus(Reject);
				pApp->ProcessDebug("SEND REJECT", Cancel.Debug());
			}
			//m_pTags->Close();
		}
	}

	catch (CDBException *e)
	{
		Error.Format("EXCEPTION: Error Processing Outgoing Message (%s): %s", e->m_strError, Cancel.Debug());
		pApp->ProcessError("CHX Handler", Error);
		e->Delete();
	}
	catch (const char *Message)
	{
		pApp->ProcessError("CHX Handler", Message);
	}
}

void CCHXHandler::ProcessMessage(CExecution& Execution)
{
	CString		Error;
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();		

	try
	{	
		if (Execution.Status() != COrder::PendingCancel)
		{
			if(Execution.Status() == COrder::Replaced) 
			{
				if (m_pTags->GetTags(MAP_ORDER_TAG_QUERY, Execution.OrderTag()) ||
					m_pTags->GetTags(MAP_CANCEL_TAG_QUERY, Execution.OrderTag()))
				{
					// set new order tag
					m_pTags->SetOrderTag(m_pTags->CancelTag());
					m_pTags->SetCancelTag(-1);

					m_pTags->SetOriginalPrice(Execution.Price());
					m_pTags->SetShares(m_pTags->Shares() - m_pTags->OriginalShares() + Execution.Shares());
					m_pTags->SetOriginalShares(Execution.Shares());

					// set exchange tag if needed
					if (Execution.ExchangeTag() != m_pTags->ExchangeTag())
						m_pTags->SetExchangeTag(Execution.ExchangeTag());
					
					// get the stafford tag based off of the order tag
					Execution.SetStaffordTag(m_pTags->StaffordTag());
					//Execution.SetSourceId(m_pTags->Client());
					
					pApp->ProcessDebug("RECV STATUS", Execution.Debug());
					
					// update Status
					Execution.SetSourceId(m_pTags->Client());
					Execution.SetStatus(COrder::Open);
					Execution.SetShares(m_pTags->Shares());
					pApp->Status().SetStatus(Execution);
					//m_pTags->Close();
				}
				else
				{
					Error.Format("Unable To Find Order Tag (%s) In Tag Map For OrderId(%s): (%s)", Execution.OrderTag(), Execution.StaffordTag(), Execution.Debug());
					pApp->ProcessError("CHX Handler", Error);
				}
			}
			else 
			{
				BOOL bFound = FALSE;

				if (m_pTags->GetTags(MAP_ORDER_TAG_QUERY, Execution.OrderTag()))
					bFound = TRUE;
				else if (m_pTags->GetTags(MAP_CANCEL_TAG_QUERY, Execution.OrderTag()))
					bFound = TRUE;

				if(bFound)
				{
					Execution.SetSourceId(m_pTags->Client());
					// set exchange tag if needed
					if (Execution.ExchangeTag() != m_pTags->ExchangeTag())
						m_pTags->SetExchangeTag(Execution.ExchangeTag());
					
					// get the stafford tag based off of the order tag
					Execution.SetStaffordTag(m_pTags->StaffordTag());
					Execution.SetSourceId(m_pTags->Client());
					pApp->ProcessDebug("RECV STATUS", Execution.Debug());

					switch(Execution.Status())
					{
					case COrder::PartiallyFilled:
						m_pTags->SetShares(m_pTags->Shares() - Execution.LastShares());
						break;
					case COrder::Stopped:
						return;
					case COrder::Canceled:
					case COrder::Filled:
					case COrder::DoneForDay:
					case COrder::Rejected:
					//case COrder::Stopped:
						m_pTags->RemoveTags();
						break;
					case COrder::Open:
						if (m_pTags->Cancel() != TRUE)
						{
							m_pTags->SetOriginalPrice(Execution.Price());
							Execution.SetShares(0);
						}
						break;
					}

					// update Status
					if (Execution.Status() == COrder::Open)
					{
						if (m_pTags->Cancel() != TRUE)
						{
							pApp->Status().SetStatus(Execution);
						}
						else
						{
							m_pTags->SetCancel(FALSE);
						}
					}
					else
					{
						pApp->Status().SetStatus(Execution);
					}
				}
				else
				{
					Error.Format("Unable To Find Order Tag (%s) In Tag Map For OrderId(%s): (%s)", Execution.OrderTag(), Execution.StaffordTag(), Execution.Debug());
					pApp->ProcessError("CHX Handler", Error);
				}
			}
		}
	}
	catch (CDBException *e)
	{
		Error.Format("EXCEPTION: Error Processing Incoming Status Message (%s): %s", e->m_strError, Execution.Debug());
		pApp->ProcessError("CHX Handler", Error);
		e->Delete();
	}
}

void CCHXHandler::ProcessMessage(CCancelReject& Reject)
{
	CString		Error;
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();
	
	try
	{
		pApp->ProcessDebug("RECV STATUS", Reject.Debug());

		if (m_pTags->GetTags(MAP_CANCEL_TAG_QUERY, Reject.CancelTag()))
		{
			// set exchange tag if needed
			// if ((Reject.ExchangeTag() != "") && (m_pTags->ExchangeTag() == ""))
			//	m_pTags->SetExchangeTag(Reject.ExchangeTag());
			// set the stafford tag based off of the order tag
			Reject.SetSourceId(m_pTags->Client());
			Reject.SetStaffordTag(m_pTags->StaffordTag());
			// update Status
			pApp->Status().SetStatus(Reject);
			m_pTags->SetCancel(FALSE);
		}
	}

	catch (CDBException *e)
	{
		Error.Format("EXCEPTION: Error Processing Incoming Cancel Reject Message (%s): %s", e->m_strError, Reject.Debug());
		pApp->ProcessError("CHX Handler", Error);
		e->Delete();
	}
}

void CCHXHandler::ProcessMessage(CReject& Reject)
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
			Reject.SetSourceId(m_pTags->Client());

			// update Status
			pApp->Status().SetStatus(Reject);
			//m_pTags->Close();
		}
	}

	catch (CDBException *e)
	{
		Error.Format("EXCEPTION: Error Processing Incoming Reject Message (%s): %s", e->m_strError, Reject.Debug());
		pApp->ProcessError("CHX Handler", Error);
		e->Delete();
	}
}

void CCHXHandler::ProcessMessage(const char *Status)
{

}