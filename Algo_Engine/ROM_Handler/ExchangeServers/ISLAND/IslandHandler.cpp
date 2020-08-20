// IslandHandler.cpp: implementation of the CIslandHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Island.h"
#include "IslandHandler.h"
#include "SendCancelReject.h"
#include <SSTREAM>


#include "DatabaseSettingsFactory.h"
#include "DatabaseSettingsInterface.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIslandHandler::CIslandHandler()
{
	CString		Error;
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	//m_pTags	= new CTagMapSet(&pApp->Database());


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
	m_pTags = new CIslandMemTagSet(lDatabaseSettings, forceReload);
	// get the default clearing account
	GetPrivateProfileString("Clearing Information",	"Default Clearing Code",	"", m_DefaultAccount.GetBuffer(128), 128,	pApp->Path() + pApp->Name() + ".DAT"); m_DefaultAccount.ReleaseBuffer();

	m_orderTypeValidator = new COrderTypeValidator(std::string((LPCSTR) pApp->DatFile()));
}

CIslandHandler::~CIslandHandler()
{
	delete m_pTags;

	if (m_orderTypeValidator != NULL)
	{
		delete m_orderTypeValidator;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////
// from client to fix

void CIslandHandler::ProcessMessage(COrder& Order)
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
		m_pTags->CreateNewMap(Order.StaffordTag(), Order.OrderTag(), -1, Order.Owner());
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
		// lookup and set traders clearing account code
		if (Order.ClearingId() == "")
			Order.SetClearingId(m_DefaultAccount);
		
		// create and send fix order off to terra
		if ((SequenceNumber = pApp->Exchange().ProcessEntry(Order)) > 0)
		{
			// map tags
			m_pTags->CreateNewMap(Order.StaffordTag(), Order.OrderTag(), SequenceNumber, Order.Owner());
            CString tradeFor = Order.TradeFor();
            m_pTags->SetTradeFor(tradeFor, tradeFor.GetLength());
			// update status
			pApp->Status().SetStatus(Order);
			pApp->ProcessDebug("ORDER SENT", Order.Debug());
		}
		else
		{
			// generate reject message
			CReject	Reject("Unable To Send", Order);			
			// map tags
			m_pTags->CreateNewMap(Order.StaffordTag(), Order.OrderTag(), -1, Order.Owner());
			// update Status
			pApp->Status().SetStatus(Reject);
			pApp->ProcessDebug("ORDER REJECT", Order.Debug());
		}
	}

	catch (CDBException *e)
	{
		Error.Format("DB EXCEPTION: Error Processing Outgoing Message (%s): %s", e->m_strError, Order.Debug());
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Island Handler", Error);
		e->Delete();
	}
	catch (const char *Message)
	{
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Island Handler", Message);
	}	
}

void CIslandHandler::ProcessMessage(CRequest& Order)
{
}

void CIslandHandler::ProcessMessage(CCancel& Cancel)
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
            CString tradeFor(m_pTags->GetTradeFor());
            CString origOwner(Cancel.Owner());
            Cancel.SetOwner(tradeFor);

			if (pApp->Exchange().ProcessCancel(Cancel))
			{
				// set the cancel tag which is generated by the fix session
				m_pTags->SetCancelTag(Cancel.CancelTag());
				// update Status
                Cancel.SetOwner(origOwner);//m_pTags->Client());
                m_pTags->SetClient(origOwner);
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
                Cancel.SetOwner(origOwner);//m_pTags->Client());
				pApp->Status().SetStatus(Reject);
				pApp->ProcessDebug("SEND REJECT", Cancel.Debug());
			}

			//m_pTags->Close();
		}
	}

	catch (CDBException *e)
	{
		Error.Format("EXCEPTION: Error Processing Outgoing Message (%s): %s", e->m_strError, Cancel.Debug());
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Island Handler", Error);
		e->Delete();
	}
	catch (const char *Message)
	{
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Island Handler", Message);
	}
}

void CIslandHandler::ProcessMessage(CExecution& Execution)
{
	CString		Error;
	CIslandApp	*pApp = (CIslandApp *)AfxGetApp();		

	try
	{	
		if (Execution.OrderTag() != "")
		{
			if(Execution.Status() == COrder::Busted)
			{
				CString l_MsgStr;
				l_MsgStr.Format("Exchange order ID: %s\nBust Time (Local): %s\nCustomer: %s\nReason: %s",
						Execution.OrderTag(),
						CTime::GetCurrentTime().Format("%Y%m%d-%H:%M:%S"),
						Execution.SourceId(),
						Execution.Text());
				((CMessageApp*)AfxGetApp())->ProcessErrorEx(MODE_ERR_BUSINESS,"Busted Trade", l_MsgStr);
				return;
			}

			if (m_pTags->GetTags(MAP_ORDER_TAG_QUERY, Execution.OrderTag()))
			{
				// set exchange tag if needed
				if ((Execution.ExchangeTag() != "") && (*(m_pTags->ExchangeTag()) == '\0'))
					m_pTags->SetExchangeTag(Execution.ExchangeTag());
				// get the stafford tag based off of the order tag
				Execution.SetStaffordTag(m_pTags->StaffordTag());
                Execution.SetOwner(m_pTags->Client());
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
				//m_pTags->Close();
			}
			else
			{
				Error.Format("Unable To Find Order Tag (%s) In Tag Map For OrderId(%s): (%s)", Execution.OrderTag(), Execution.StaffordTag(), Execution.Debug());
				pApp->ProcessErrorEx(MODE_ERR_BASIC,"Island Handler", Error);
			}
		}
	}

	catch (CDBException *e)
	{
		Error.Format("EXCEPTION: Error Processing Incoming Status Message (%s): %s", e->m_strError, Execution.Debug());
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Island Handler", Error);
		e->Delete();
	}
}

void CIslandHandler::ProcessMessage(CCancelReject& Reject)
{
	CString		Error;
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();		
	
	try
	{
		pApp->ProcessDebug("RECV STATUS", Reject.Debug());

		if (m_pTags->GetTags(MAP_ORDER_TAG_QUERY, Reject.OrderTag()))
		{
			// set exchange tag if needed
			if ((Reject.ExchangeTag() != "") && (*(m_pTags->ExchangeTag()) == '\0'))
				m_pTags->SetExchangeTag(Reject.ExchangeTag());
			// set the stafford tag based off of the order tag
			Reject.SetStaffordTag(m_pTags->StaffordTag());
            Reject.SetOwner(m_pTags->Client());
			// update Status
			pApp->Status().SetStatus(Reject);
			//m_pTags->Close();
		}
	}

	catch (CDBException *e)
	{
		Error.Format("EXCEPTION: Error Processing Incoming Cancel Reject Message (%s): %s", e->m_strError, Reject.Debug());
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Island Handler", Error);
		e->Delete();
	}
}

void CIslandHandler::ProcessMessage(CReject& Reject)
{
	CString	Error;
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();		
	
	try
	{
		pApp->ProcessDebug("RECV STATUS", Reject.Debug());

		if (m_pTags->GetTags(MAP_ORDER_TAG_QUERY, Reject.OrderTag()))
		{
			// set tags
			Reject.SetExchangeTag(m_pTags->ExchangeTag());
			Reject.SetCancelTag(m_pTags->CancelTag());
			Reject.SetStaffordTag(m_pTags->StaffordTag());
            Reject.SetOwner(m_pTags->Client());
			// update Status
			pApp->Status().SetStatus(Reject);
			//m_pTags->Close();
		}
	}

	catch (CDBException *e)
	{
		Error.Format("EXCEPTION: Error Processing Incoming Reject Message (%s): %s", e->m_strError, Reject.Debug());
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Island Handler", Error);
		e->Delete();
	}
}

void CIslandHandler::ProcessMessage(const char *Status)
{
}

bool CIslandHandler::IsNASDSym(const char* symbol) 
{
   return (strlen(symbol) >= 4);
}
