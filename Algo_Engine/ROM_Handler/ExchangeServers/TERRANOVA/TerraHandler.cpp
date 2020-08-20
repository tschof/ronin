#include "stdafx.h"
#include <sstream>
#include "MessageApp.h"
#include "TerraHandler.h"
#include "SendCancelReject.h"
#include "FixSession.h"
#include "FixSessionException.h"
#include "TerraMemTagSet.h"

// New stuff for GTC/GTD
#include "DatabaseSettingsFactory.h"
#include "DatabaseSettingsInterface.h"

#pragma warning(disable:4786)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CArcaHandler::CArcaHandler(CArcaStatus& statusHandler, CConnection& host, CTerraNovaSession& exchangeSession, IErrorHandler& errorHandler, std::string& datFile) 
: CExchangeHandlerBase(&host, datFile), m_statusHandler(statusHandler), m_exchangeSession(exchangeSession), m_errorHandler(errorHandler)
{
	Initialize(datFile);
}

CArcaHandler::~CArcaHandler()
{
	delete m_pTags;
}

//////////////////////////////////////////////////////////////////////
// Initialization
//////////////////////////////////////////////////////////////////////

void CArcaHandler::Initialize(std::string& datFile)
{
	InitializeMemoryMap();

	// get the default clearing account
	GetPrivateProfileString("Clearing Information",	"Default Clearing Code", "", m_DefaultAccount.GetBuffer(128), 128,	datFile.c_str());
	m_DefaultAccount.ReleaseBuffer();
}

void CArcaHandler::InitializeMemoryMap()
{
	CString exchangeDestinationId;
	CString dbUser;
	CString dbPassword;
	CString	dbServer;
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();
	bool forceReload;

	try
	{
		
		GetPrivateProfileString("TNTO Connection",	
			"Exchange Destination Id",	
			"",	
			exchangeDestinationId.GetBuffer(128), 
			128, 
			DatFile().c_str());
		
		exchangeDestinationId.ReleaseBuffer();
		
		GetPrivateProfileString("Order Database Information",
			"User",	
			"",	
			dbUser.GetBuffer(128), 
			128, 
			DatFile().c_str());
		
		dbUser.ReleaseBuffer();
		
		GetPrivateProfileString("Order Database Information",
			"Password",	
			"",	
			dbPassword.GetBuffer(128), 
			128, 
			DatFile().c_str());
		
		dbUser.ReleaseBuffer();
		
		GetPrivateProfileString("Order Database Information",
			"Server",	
			"",	
			dbServer.GetBuffer(128), 
			128, 
			DatFile().c_str());
		
		dbServer.ReleaseBuffer();
		
		forceReload = (GetPrivateProfileInt("MemoryMapSettings", "ForceReload", 0, pApp->DatFile()) == 0) ? false : true;
		
		TIDatabaseSettingsAutoPtr lDatabaseSettings = CDatabaseSettingsFactory::CreateDatabaseSettings();
		
		lDatabaseSettings->AppName((LPCSTR) pApp->Name());
		lDatabaseSettings->DestinationId((LPCSTR) exchangeDestinationId);
		lDatabaseSettings->Server((LPCSTR) dbServer);
		lDatabaseSettings->User((LPCSTR) dbUser);
		lDatabaseSettings->Password((LPCSTR) dbPassword);

		m_pTags = new CTerraMemTagSet(lDatabaseSettings, forceReload);
		
		MemTagSet(m_pTags);
		
		WritePrivateProfileString("MemoryMapSettings", "ForceReload", "0", pApp->DatFile());
	}
	catch(MMIErrorCodes errorCode)
	{
		CString error = "Unable to intialize memory map";

		switch(errorCode)
		{
		case MMI_DB_CONNECTION_ERROR:
			error += " (database connection failure)";
			break;
		};

		pApp->ProcessErrorEx(MODE_ERR_BUSINESS, "Arca Exchange", error);

		AfxMessageBox(error, MB_OK | MB_ICONERROR);
		PostQuitMessage(-1);
	}
}

//////////////////////////////////////////////////////////////////////
// Overidden Order Processing Methods
//////////////////////////////////////////////////////////////////////

void CArcaHandler::Process(CCancel& cancel)
{
	try
	{
		CString cancelTag = m_pTags->CancelTag();

		if (TRUE == m_pTags->Replace())
		{
			throw CRejectCancel("Replace Pending");
		}

		// set the exchange's tag an the fix order tag
		cancel.SetExchangeTag(m_pTags->ExchangeTag());
		cancel.SetOrderTag(m_pTags->CancelTag());

		cancel.SetShares(cancel.LeavesQty() + cancel.CumulativeQty());
	
		CreateCancelTag(cancelTag,m_pTags->CancelReplaceCnt() + 1);
		//CreateCancelTag(cancelTag, m_exchangeSession.GetNextOutgoingSeqNum());
		cancel.SetCancelTag(cancelTag);
		m_pTags->SetCancelReplaceCnt(m_pTags->CancelReplaceCnt() + 1);
		
		ExchangeProcess(cancel);
		
		m_pTags->SetCancel(TRUE);


		// update Status
		m_statusHandler.SetStatus(cancel);
		m_errorHandler.Debug("CANCEL SENT", (LPCSTR)cancel.Debug());
	}
	catch(CFixSessionCancelException)
	{
		throw CRejectCancel("Unable To Send");
	}
	catch (const char* message)
	{
		m_errorHandler.Error(MODE_ERR_DEVELOPER, "Arca Handler", message);
	}
}

void CArcaHandler::Process(CCancelReject& cancelReject)
{	
	// set the stafford tag based off of the order tag
	cancelReject.SetStaffordTag(m_pTags->StaffordTag());

	// update Status
	if (m_pTags->Replace() == TRUE)
	{
		cancelReject.SetStatus(COrder::ReplaceRejected);
		m_pTags->SetReplace(FALSE);
	}
	else
	{
		cancelReject.SetStatus(COrder::CancelRejected);
		m_pTags->SetCancel(FALSE);
	}

	m_statusHandler.SetStatus(cancelReject);
}

void CArcaHandler::Process(CExecution& execution)
{
	if (execution.Status() == COrder::PendingCancel)
	{
		if (m_pTags->Replace() == TRUE)
		{
			// This is actually pending cancel/replace
			execution.SetStatus(COrder::ReplacePending);
		}

		if (TRUE == m_pTags->Cancel())
		{
			// See if ARCA sent shares in the
			// pending cancel message.
			if (0 == execution.LastShares())
			{
				return;
			}
		}
	}

	execution.SetStaffordTag(m_pTags->StaffordTag());

	// Arca sends back an Open for orders that have been
	// replaced.  Assuming that the order was actually 
	// being replaced process the replaced order.
	if((COrder::Open == execution.Status()) && (TRUE == m_pTags->Replace()))
	{
		ProcessReplacedOrder(execution);
		return;
	}

	ProcessOrderExecution(execution);
}

void CArcaHandler::Process(COrder& order)
{
	long sequenceNumber	= 0;

	try
	{
		if (COrder::Market == order.Type())
		{
			order.SetPrice(0);
		}

		if (COrder::GoodTillDate == order.TIF())
		{
			if (order.ExpTime() == "")
			{
				throw CInvalidOrderData();
			}
		}

		// lookup and set traders clearing account code
		if (order.ClearingId() == "")
		{
			order.SetClearingId(m_DefaultAccount);	
		}

		CString orderTag;
		CreateOrderTag(orderTag, m_exchangeSession.GetNextOutgoingSeqNum());
		order.SetOrderTag(orderTag);
		//order.SetOrderTag(m_exchangeSession.GetNextOutgoingSeqNum());

		ExchangeProcess(order, sequenceNumber);
		
		m_pTags->CreateNewMap(order.StaffordTag(), order.OrderTag(), sequenceNumber, order.SourceId());
		m_pTags->SetCancel(FALSE);
		m_pTags->SetReplace(FALSE);
		m_pTags->SetCancelReplaceCnt(0);
        m_pTags->SetClient(order.TradeFor());

		// update status
		m_statusHandler.SetStatus(order);
		m_errorHandler.Debug("ORDER SENT", (LPCSTR)order.Debug());
	}
	catch(CFixSessionOrderException)
	{
		throw CRejectOrder("Unable to Send");
	}
	catch(CInvalidOrderData)
	{
		throw CRejectOrder("Invalid Order Data - missing expire time");
	}
	catch (const char* message)
	{
		m_errorHandler.Error(MODE_ERR_DEVELOPER, "Arca Handler", message);
	}
}

void CArcaHandler::Process(CReplace& replace)
{
	CFixedString CancelTag;
	bool sendReject = false;
	CMessageApp *pApp = (CMessageApp *) AfxGetApp();

	try
	{
		if (TRUE == m_pTags->Replace())
		{
			throw CRejectReplace("Replace Pending");
		}

		if (TRUE == m_pTags->Cancel())
		{
			throw CRejectReplace("Cancel Pending");
		}

		// set the exchange's tag an the fix order tag
		replace.SetExchangeTag(m_pTags->ExchangeTag());

		// Always take the cancel tag as the order to replace.
		// The cancel tag is defaulted to the order tag when
		// the order is created but the cancel tag is updated
		// in replace scenarios when the order is successfully
		// replaced.  Therefore the current working order id at
		// the exchange is always in the cancel tag.
		replace.SetOrderTag(m_pTags->CancelTag());
		CString replaceTag = m_pTags->CancelTag();
		CreateReplaceTag(replaceTag,m_pTags->CancelReplaceCnt() + 1);
		//CreateReplaceTag(replaceTag, m_exchangeSession.GetNextOutgoingSeqNum());

		replace.SetReplaceTag(replaceTag);

		replace.SetShares(replace.Shares() + replace.CumShares());
		replace.SetOriginalShares(replace.OriginalShares());
		replace.SetOriginalPrice(replace.OriginalPrice());
		
		m_pTags->SetCancelReplaceCnt(m_pTags->CancelReplaceCnt() + 1);		
		
		ExchangeProcess(replace);
			
		m_errorHandler.Debug("REPLACE SENT", "");
	}
	catch(CFixSessionOrderException)
	{
		throw CRejectReplace("Unable to Send");
	}
	catch (const char* message)
	{
		m_errorHandler.Error(MODE_ERR_DEVELOPER, "Arca Handler", message);
	}
}

void CArcaHandler::Process(CReject& reject)
{
	// set tags
	reject.SetOrderTag(m_pTags->OrderTag());
	reject.SetExchangeTag(m_pTags->ExchangeTag());
	reject.SetCancelTag(m_pTags->CancelTag());
	reject.SetStaffordTag(m_pTags->StaffordTag());
	
	// update Status
	m_statusHandler.SetStatus(reject);
}

//////////////////////////////////////////////////////////////////////
// Overidden Preprocessing Methods
//////////////////////////////////////////////////////////////////////

void CArcaHandler::OnReceive(CCancelReject& cancelReject)
{
	m_errorHandler.Debug("RECV STATUS", (LPCSTR)cancelReject.Debug());
}

void CArcaHandler::OnReceive(CExecution& execution)
{
	m_errorHandler.Debug("RECV STATUS", (LPCSTR) execution.Debug());
}

void CArcaHandler::OnReceive(CReject& reject)
{
	m_errorHandler.Debug("RECV STATUS", (LPCSTR)reject.Debug());
}

//////////////////////////////////////////////////////////////////////
// Overidden Exception Processing
//////////////////////////////////////////////////////////////////////

void CArcaHandler::Reject(CCancel& cancel, const char* reason)
{
	// generate reject
	CSendCancelReject cancelReject(cancel);

	cancelReject.SetReason(reason);
		
	// update Status
	m_statusHandler.SetStatus(cancelReject);
	m_errorHandler.Debug("SEND REJECT", (LPCSTR)cancel.Debug());
}

void CArcaHandler::Reject(COrder& order, const char* reason)
{
	CReject	reject(reason, order);			
	m_statusHandler.SetStatus(reject);
	m_errorHandler.Debug("ORDER REJECT", (LPCSTR) order.Debug());
}

void CArcaHandler::Reject(CReplace& replace, const char* reason)
{
	// generate reject
	CReplaceReject reject(reason, replace);

	m_statusHandler.SetStatus(reject);
	m_errorHandler.Debug("SEND REJECT", "");
}

void CArcaHandler::OrderNotFound(const char* orderTag, const char* staffordTag, const char* debug)
{
	m_errorHandler.TagNotFound("Arca Handler", orderTag, staffordTag, debug);
}

//////////////////////////////////////////////////////////////////////
// Overidden Memory Map Methods
//////////////////////////////////////////////////////////////////////

bool CArcaHandler::LoadOrder(CCancelReject& cancelReject)
{
	CString origOrderTag;

	GetOrigOrderId(origOrderTag, const_cast<CString&>(cancelReject.CancelTag()));

	if (FALSE == m_pTags->GetTags(MAP_ORDER_TAG_QUERY, origOrderTag))
	{
		return false;
	}

	return true;
}

bool CArcaHandler::LoadOrder(CExecution& execution)
{
	CString origOrderTag;

	GetOrigOrderId(origOrderTag, const_cast<CString&>(execution.OrderTag()));

	if (FALSE == m_pTags->GetTags(MAP_ORDER_TAG_QUERY, origOrderTag))
	{
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// Processing Methods
//////////////////////////////////////////////////////////////////////

void CArcaHandler::ProcessReplacedOrder(CExecution& execution)
{
	// set new order tag
	m_pTags->SetCancelTag(execution.OrderTag());

	// Update the replace pending flag
	m_pTags->SetReplace(FALSE);

	// set exchange tag if needed
	if (execution.ExchangeTag() != m_pTags->ExchangeTag())
	{
		m_pTags->SetExchangeTag(execution.ExchangeTag());
	}

	execution.SetStaffordTag(m_pTags->StaffordTag());

	// Default status to replaced
	execution.SetStatus(COrder::Replaced);

	// Reset state if necessary
	if (execution.Cumulative() == execution.Shares())
	{
		execution.SetStatus(COrder::Filled);
	}

	m_statusHandler.SetStatus(execution);
}

void CArcaHandler::ProcessOrderExecution(CExecution& execution)
{
	// Default to always update the status.
	bool updateStatus = true;

	switch(execution.Status())
	{
	case COrder::Canceled:

		// Intentionally fall through
		m_pTags->SetCancel(FALSE);

	case COrder::Filled:
	case COrder::DoneForDay:
	case COrder::Rejected:
	case COrder::Stopped:
		m_pTags->RemoveTags();
		break;
	}
	
	// update Status
	if (execution.Status() == COrder::Open)
	{
		m_pTags->SetExchangeTag(execution.ExchangeTag());

		if (TRUE == m_pTags->Cancel())
		{
			// This case is a simple acknowledgement
			// from arca that it received the cancel
			// request.  Therefore there is no need
			// to update the client.
			updateStatus = false;
		}
	}

	if (true == updateStatus)
	{
		m_statusHandler.SetStatus(execution);
	}
}

//////////////////////////////////////////////////////////////////////
// Exchange Session Methods
//////////////////////////////////////////////////////////////////////
void CArcaHandler::ExchangeProcess(CCancel& cancel)
{
	if (FALSE == m_exchangeSession.ProcessCancel(cancel))
	{
		throw CFixSessionCancelException();
	}
}

void CArcaHandler::ExchangeProcess(COrder& order, long& sequenceNumber)
{
	// create and send fix order off to terra
	if ((sequenceNumber = m_exchangeSession.ProcessEntry(order)) <= 0)
	{
		throw CFixSessionOrderException();
	}
}

void CArcaHandler::ExchangeProcess(CReplace& replace)
{
	if(FALSE == m_exchangeSession.ProcessReplace(replace))
	{
		throw CFixSessionReplaceException();
	}
	else
	{
		m_pTags->SetReplace(TRUE);
	}
}

void CArcaHandler::CreateCancelTag(CString& orderTag, int sequenceNumber) 
{
	
	CString s;
	
	s.Format("%s%ld", "C", sequenceNumber);
	
	orderTag += s;
/////////////////////////////////////
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();
	
	int index;
	
	if ((-1 != (index = orderTag.Find('R'))) || (-1 != (index = orderTag.Find('C'))))
	{
		// This order has been replaced or cancelled before, add the new sequence number
		CString str;
		CString newOrderTag;
		
		str = orderTag.Mid(0, index);
		
		// Now just add the new sequence number
		newOrderTag.Format("%s%s%ld", str,"C", sequenceNumber);
		
		orderTag = newOrderTag;
	}
	else
	{
		// This order has not been replaced.
		// Create a replaced order tag
		CString str;
		
		str.Format("%s%ld", "C", sequenceNumber);
		
		orderTag += str;
	}

}
void CArcaHandler::LoadTradeFor(CString& tradeFor, const char* staffordTag) 
{
        if(m_pTags->GetTags(MAP_STAFFORD_TAG_QUERY, staffordTag)) {
            tradeFor = m_pTags->Client();
        }
}
