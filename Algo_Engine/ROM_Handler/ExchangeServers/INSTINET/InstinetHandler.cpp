// InstinetHandler.cpp: implementation of the CInstinetHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MessageApp.h"
#include "SendCancelReject.h"
#include "Instinet.h"
#include "FixSession.h"
#include "IncaFixSession.h"
#include "InstinetHandler.h"

#pragma warning(disable:4786)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInstinetHandler::CInstinetHandler()
{
	CString		Error;
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	m_pTags = new CInstinetMemTagSet;
	m_pTags->InitMap();

	m_orderTypeValidator = new COrderTypeValidator(std::string((LPCSTR) pApp->DatFile()));

	//m_pTags	= new CTagSet(&pApp->Database());
	
	// get the default clearing account
	GetPrivateProfileString("Clearing Information",	"Default Clearing Code",	"", m_DefaultAccount.GetBuffer(128), 128,	pApp->DatFile()); m_DefaultAccount.ReleaseBuffer();

	// load terminal/trader information
	char User[128];
	char Password[128];
	char Source[128];

	GetPrivateProfileString("External Database Information",	"User",	"", User, 128,	pApp->DatFile());
	GetPrivateProfileString("External Database Information",	"Password",	"", Password, 128,	pApp->DatFile());
	GetPrivateProfileString("External Database Information",	"DataSource",	"", Source, 128,	pApp->DatFile());

	try	
	{
		//m_pTerminals = new CInstiInterface(User, Password, Source);	
	}
	catch (CDBException *e)
	{
		Error.Format("EXCEPTION: Error Opening Instinet Interface (%s)", e->m_strError);
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"INCA Handler", Error);
		e->Delete();
	}
}

CInstinetHandler::~CInstinetHandler()
{
	delete m_pTags;

	if (m_orderTypeValidator != NULL)
	{
		delete m_orderTypeValidator;
	}

	//delete m_pTerminals;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// from client to fix

void CInstinetHandler::ProcessMessage(COrder& Order)
{
	long		SequenceNumber	= 0;
	CString		Error			= "";
	CString		Account;
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
		if (Order.ClearingId() == "")
			Order.SetClearingId(m_DefaultAccount);
		
		Order.SetOrderTag(((CFixSession&) pApp->Exchange()).GetNextOutgoingSeqNum());
		 // create and send fix order off to terra

		char TargetSubId[15];

		if (Order.ExDestination() == "DOT")
		{
			strcpy(TargetSubId, "_DOT");
		}
		else if(Order.ExDestination() != "ARC")
		{
			strcpy(TargetSubId, "INET");
		}

		CIncaFixSession *pSession = (CIncaFixSession*)&(pApp->Exchange());
		SequenceNumber = pSession->ProcessEntry(Order, TargetSubId);
		//SequenceNumber = pSession->ProcessEntry(Order, "ARC");
		if (SequenceNumber > 0)
		{
			// map tags
			m_pTags->CreateNewMap(Order.StaffordTag(), Order.OrderTag(), SequenceNumber, Order.SourceId());
			m_pTags->SetTargetSubId(TargetSubId);
			m_pTags->InitReplace();

			if (Order.Type() == COrder::Market)
			{
				m_pTags->SetOriginalPrice(0);
			}
			else
			{
				m_pTags->SetOriginalPrice(Order.dPrice());
			}

			m_pTags->SetShares(Order.Shares());
			m_pTags->SetOriginalShares(Order.Shares());

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
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Instinet Handler", Error);
		e->Delete();
	}
	catch (const char *Message)
	{
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Instinet Handler", Message);
	}	
}

void CInstinetHandler::ProcessMessage(CRequest& Order)
{
}

void CInstinetHandler::ProcessMessage(CCancel& Cancel)
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

			CIncaFixSession *pSession = (CIncaFixSession*)&(pApp->Exchange());
			
			if (pSession->ProcessCancel(Cancel, m_pTags->TargetSubId()))
			//if (pSession->ProcessCancel(Cancel, "ARC"))
			{
				// set the cancel tag which is generated by the fix session
				m_pTags->SetCancelTag(Cancel.CancelTag());
				// update Status
				pApp->Status().SetStatus(Cancel);
				pApp->ProcessDebug("CANCEL SENT", Cancel.Debug());
			}
			else
			{
				// generate reject
				CSendCancelReject Reject(Cancel);		
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
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"INCA Handler", Error);
		e->Delete();
	}
	catch (const char *Message)
	{
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Instinet Handler", Message);
	}
}

void CInstinetHandler::ProcessMessage(CExecution& Execution)
{
	CString		 Error;
	CInstinetApp *pApp = (CInstinetApp *)AfxGetApp();		

	if (Execution.TransactionType() == "3")
	{
		return;
	}

	try
	{	
		if (Execution.OrderTag() != "")
		{
			if (Execution.Text().Find("CXL") < 0)		// don't care about a cancel ack (pending cancel)
			{
				if (m_pTags->GetTags(MAP_ORDER_TAG_QUERY, Execution.OrderTag()) ||
					m_pTags->GetTags(MAP_CANCEL_TAG_QUERY, Execution.OrderTag()))
				{
					// set exchange tag if needed
					if ((Execution.ExchangeTag() != "") && (*(m_pTags->ExchangeTag()) == '\0'))
						m_pTags->SetExchangeTag(Execution.ExchangeTag());
					// get the stafford tag based off of the order tag
					Execution.SetStaffordTag(m_pTags->StaffordTag());

					pApp->ProcessDebug("RECV STATUS", Execution.Debug());

					switch(Execution.Status())
					{
					case COrder::PartiallyFilled:
						m_pTags->SetShares(m_pTags->Shares() - Execution.LastShares());
						break;
					case COrder::Canceled:
					case COrder::Filled:
					case COrder::DoneForDay:
					case COrder::Rejected:
					case COrder::Stopped:
						m_pTags->RemoveTags();
						break;
					case COrder::Open:
						if (m_pTags->Replace() != TRUE)
						{
							m_pTags->SetOriginalPrice(Execution.Price());
						}
						else
						{
							m_pTags->SetOriginalPrice(Execution.Price());
							m_pTags->SetShares(m_pTags->Shares() - m_pTags->OriginalShares() + Execution.Shares());
							m_pTags->DeleteReplace();
							m_pTags->SetOriginalShares(Execution.Shares());
							m_pTags->SetOrderTag(m_pTags->CancelTag());

							Execution.SetStatus(COrder::Open);
							Execution.SetShares(m_pTags->Shares());
						}
					}

					// update Status
					pApp->Status().SetStatus(Execution);
					
					//m_pTags->Close();
				}
				else
				{
					Error.Format("Unable To Find Order Tag (%s) In Tag Map For OrderId(%s): (%s)", Execution.OrderTag(), Execution.StaffordTag(), Execution.Debug());
					pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Instinet Handler", Error);
				}
			}
		}
		else
		{
			/*if (pApp->Status().ProcessTrade(Execution))
			{
				pApp->ProcessTraffic("TERMINAL", Execution.Debug());

				CFromInstinetExecution *pExecution = (CFromInstinetExecution *)&Execution;				

				CInstiData	*pTerminal = m_pTerminals->GetMemoryInsti(pExecution->TerminalId());

				if (pTerminal)
				{
					CCsv *pCsv = BuildTerminalTrade(*pExecution, *pTerminal);

					if (pCsv)
						pApp->Recorder()->SendMessage(pCsv->Message());					
				}
			}*/
		}		
	}

	catch (CDBException *e)
	{
		Error.Format("EXCEPTION: Error Processing Incoming Status Message (%s): %s", e->m_strError, Execution.Debug());
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Instinet Handler", Error);
		e->Delete();
	}
}

/*CCsv * CInstinetHandler::BuildTerminalTrade(const CFromInstinetExecution& Exec, CInstiData& Terminal)
{
	int			ExecType	= Exec.ExecType();
	int			Status		= Exec.Status();
	double		LastPrice	= Exec.LastPrice();
	long		LastShares	= Exec.LastShares();
	CMessageApp *pApp		= (CMessageApp *)AfxGetApp();
	
	CCsv *Execution = new CCsv((char)CMessage::TradeReport);
	
	if (ExecType == 0)		// new, busted, corrected
	{
		if (LastShares)	
		{
			Execution->SetAt(CSV_TRADER,			Terminal.GetTrader());
			Execution->SetAt(CSV_SIDE,				Exec.Side());
			Execution->SetAt(CSV_SYMBOL,			Exec.Symbol());
			Execution->SetAt(CSV_SHARES,			Exec.LastShares());
			Execution->SetAt(CSV_PRICE,				Exec.LastPrice());
			Execution->SetAt(CSV_TIF,				Exec.TimeInForce());
			Execution->SetAt(CSV_CAPACITY,			Terminal.GetTType());
			Execution->SetAt(CSV_CLEARINGACCOUNT,	Terminal.GetClearingAccount());
			Execution->SetAt(CSV_EXCHANGE,			COrder::INSTINET);
			Execution->SetAt(CSV_STATUS,			COrder::Filled);
			Execution->SetAt(CSV_LOCALACCOUNT,		Terminal.GetLocalAccount());
			Execution->SetAt(CSV_FIRM,				Terminal.GetFirm());
			Execution->SetAt(CSV_EXEC_TAG,			Exec.TradeTag());
			Execution->SetAt(CSV_EXEC_CONFIRM,		Exec.Confirm());
			Execution->SetAt(CSV_REFERENCE,			Exec.ReferenceConfirm());
			Execution->SetAt(CSV_REPORT,			Exec.Report());
			Execution->SetAt(CSV_CONTRA,			Exec.ExecBroker());
		}
	}

	return (Execution);
}*/

void CInstinetHandler::ProcessMessage(CCancelReject& Reject)
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
			Reject.SetSourceId(m_pTags->Client());
			// update Status
			pApp->Status().SetStatus(Reject);
			//m_pTags->Close();
		}
	}

	catch (CDBException *e)
	{
		Error.Format("EXCEPTION: Error Processing Incoming Cancel Reject Message (%s): %s", e->m_strError, Reject.Debug());
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Instinet Handler", Error);
		e->Delete();
	}
}

void CInstinetHandler::ProcessMessage(CReject& Reject)
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
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Instinet Handler", Error);
		e->Delete();
	}
}

void CInstinetHandler::ProcessMessage(const char *Status)
{

}


void CInstinetHandler::ProcessMessage(CReplace& Replace)
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
			Replace.SetOriginalShares(m_pTags->OriginalShares());
			Replace.SetOriginalPrice(m_pTags->OriginalPrice());
			Replace.SetShares(m_pTags->OriginalShares() - m_pTags->Shares() + Replace.Shares());

			CIncaFixSession *pSession = (CIncaFixSession*)&(pApp->Exchange());
			if(pSession->ProcessReplace(Replace, m_pTags->TargetSubId()) )
			//if(pSession->ProcessReplace(Replace, "ARC") )
			{
				m_pTags->SetCancelTag(Replace.CancelTag());
				m_pTags->AddReplace();

				pApp->ProcessDebug("REPLACE SENT", "");
			}
			else
			{
				// generate reject
				//CBrassReject Reject("Unable To Send", Replace);
				CReplaceReject Reject("Unable To Send", Replace);
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
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"INCA Handler", Error);
		e->Delete();
	}
	catch (const char *Message)
	{
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"INCA Handler", Message);
	}
}