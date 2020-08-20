#include "stdafx.h"
#include "Replace.h"
#include "MessageApp.h"
#include "BrassHandler.h"
#include "SendCancelReject.h"
#include "FixSession.h"
#include "BrassMemTagSet.h"
#include "BrassFixSession.h"
#include "BrassReject.h"

// New stuff for GTC/GTD
#include "DatabaseSettingsFactory.h"
#include "DatabaseSettingsInterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBrassHandler::CBrassHandler(CConnection& host, std::string& datFile) : CExchangeHandlerBase(&host, datFile)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	InitializeMemoryMap();

	m_pBRASSInterface = NULL;

#ifdef _RECORDER
	m_pBRASSInterface = new CBRASSInterface(&pApp->Database());
#endif

	// get the default clearing account
	GetPrivateProfileString("Clearing Information",	"TDPROX Account", "", m_TDPXAccount.GetBuffer(128), 128,	pApp->DatFile()); m_TDPXAccount.ReleaseBuffer();
	GetPrivateProfileString("Clearing Information",	"Default Clearing Code", "", m_DefaultAccount.GetBuffer(128), 128,	pApp->DatFile()); m_DefaultAccount.ReleaseBuffer();
}

CBrassHandler::~CBrassHandler()
{
	delete m_pTags;

	if(m_pBRASSInterface)
	{
		delete m_pBRASSInterface;
	}
}

void CBrassHandler::InitializeMemoryMap()
{
	CString exchangeDestinationId;
	CString dbUser;
	CString dbPassword;
	CString	dbServer;
	CMessageApp *pApp;
	bool	forceReload;

	pApp = (CMessageApp *)AfxGetApp();

	GetPrivateProfileString("Brass Connection",	
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

	dbUser.ReleaseBuffer();

	GetPrivateProfileString("Order Database Information",
							"Server",	
							"",	
							dbServer.GetBuffer(128), 
							128, 
							pApp->DatFile());

	dbServer.ReleaseBuffer();

	forceReload = GetPrivateProfileInt("MemoryMapSettings", "ForceReload", 0, pApp->DatFile()) == 0 ? false : true;

	TIDatabaseSettingsAutoPtr lDatabaseSettings = CDatabaseSettingsFactory::CreateDatabaseSettings();

	lDatabaseSettings->AppName((LPCSTR) pApp->Name());
	lDatabaseSettings->DestinationId((LPCSTR) exchangeDestinationId);
	lDatabaseSettings->Server((LPCSTR) dbServer);
	lDatabaseSettings->User((LPCSTR) dbUser);
	lDatabaseSettings->Password((LPCSTR) dbPassword);

	m_pTags = new CBrassMemTagSet(lDatabaseSettings, forceReload);

	WritePrivateProfileString("MemoryMapSettings", "ForceReload", "0", pApp->DatFile());

	MemTagSet(m_pTags);
}

//////////////////////////////////////////////////////////////////////
// Overidden PreProcessing Methods
//////////////////////////////////////////////////////////////////////
void CBrassHandler::OnReceive(CCancelReject& cancelReject)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	pApp->ProcessDebug("RECV STATUS", cancelReject.Debug());
}

void CBrassHandler::OnReceive(CReject& reject)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	pApp->ProcessDebug("RECV STATUS", reject.Debug());
}

//////////////////////////////////////////////////////////////////////
// Overidden Order Processing Methods
//////////////////////////////////////////////////////////////////////
void CBrassHandler::Process(CCancel& cancel)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();	

	try
	{
		// set the exchange's tag an the fix order tag
		cancel.SetExchangeTag(m_pTags->ExchangeTag());
		cancel.SetOrderTag(m_pTags->OrderTag());
		cancel.SetCancelTag((pApp->Exchange()).GetNextOutgoingSeqNum());
		cancel.SetOriginalOrderTime(m_pTags->OriginalOrderTime());

		if(0 == strcmp(m_pTags->CancelTag(),""))
		{
			m_pTags->SetCancelTag(cancel.CancelTag());
		}
		else
		{
			cancel.SetCancelTag(m_pTags->CancelTag());
		}

		cancel.SetShares(m_pTags->RemainingShares());

		ExchangeProcess(cancel);

		// update Status
		pApp->Status().SetStatus(cancel);
		pApp->ProcessDebug("CANCEL SENT", cancel.Debug());
	}
	catch(CBrassSessionCancelException)
	{
		throw CRejectCancel();
	}
	catch (const char *Message)
	{
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Brass Handler", Message);
	}
}

void CBrassHandler::Process(CCancelReject& cancelReject)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();	

	cancelReject.SetStaffordTag(m_pTags->StaffordTag());
	
	// update Status
	pApp->Status().SetStatus(cancelReject);
}

void CBrassHandler::Process(CExecution& execution)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();		

	if (execution.Status() == COrder::PendingCancel)
	{
		return;
	}

	pApp->ProcessDebug("RECV STATUS", execution.Debug());
	execution.SetStaffordTag(m_pTags->StaffordTag());

	// set exchange tag if needed
	if (strcmp((LPCTSTR)(execution.ExchangeTag()), m_pTags->ExchangeTag()))
	{
		m_pTags->SetExchangeTag(execution.ExchangeTag());
	}

	switch(execution.Status())
	{

	case COrder::PartiallyFilled:
		m_pTags->SetRemainingShares(m_pTags->RemainingShares() - execution.LastShares());
		break;

	case COrder::Filled:
		m_pTags->SetRemainingShares(m_pTags->RemainingShares() - execution.LastShares());
		m_pTags->RemoveTags();
		break;

	case COrder::Canceled:

		//Intentionally falls through
		m_pTags->SetCancelTag("");

	case COrder::DoneForDay:
	case COrder::Stopped:
		m_pTags->RemoveTags();
		break;
		
	case COrder::Rejected:
		if (strcmp(m_pTags->CancelTag(), m_pTags->OrderTag()) != 0)
		{
			execution.SetStatus(COrder::CancelRejected);
		}
		else
		{
			m_pTags->RemoveTags();
		}
		
		break;
		
	case COrder::Replaced:
		ProcessReplacedOrder(execution);
		break;

	case COrder::Open:
		if (m_pTags->Type() == COrder::StopLimit)
		{
			execution.SetPrice(0); // Brass sets price with the value of StopPrice
		}
		break;
	
//	case COrder::Open:
//		{
//		CString s = "R,084150,TU70,MCC_2118_185002,2,NSM,80,15.78,2,1,A,0551,43834320D0,25,0,TU70,N,OTTO-20040730-2660,0,10,0,0,TU70,WAG,TU70,,,OTTO,1,,,,,,,E,,,,,,25,,,,,,,,,084150,,,,,,,,,";
//		CCsv sv(s);
//		CReplace Replace(sv);
//		Process(Replace);
//		}
//		break;

	default:
		break;
	}
	
	pApp->Status().SetStatus(execution);

	CString StaffordTag = execution.StaffordTag();

	if (StaffordTag.Find("CMSTDCAN") != -1 &&
		(execution.Status() == COrder::Filled ||
		 execution.Status() == COrder::PartiallyFilled))
	{
		CBRASSData* pBRASSData = m_pBRASSInterface->GetMemoryAt(m_TDPXAccount);

		if(pBRASSData)
		{
			execution.SetClearingAccount(pBRASSData->GetClearingAcID());
			execution.SetLocalAccount(pBRASSData->GetLocalAcAcr());
			execution.SetFirm(pBRASSData->GetFirmAcr());
			execution.SetTrader(pBRASSData->GetTraderAcr());
			execution.SetOrderTag("MANUAL");

			if (m_pTags->Side() == COrder::Buy)
			{
				execution.SetSide(COrder::Sell);
			}
			else if (m_pTags->Side() == COrder::Sell ||
					 m_pTags->Side() == COrder::Short ||
					 m_pTags->Side() == COrder::Exempt)
			{
				execution.SetSide(COrder::Buy);
			}
			pApp->Status().SetStatus(execution);
		}
	}
}

void CBrassHandler::Process(COrder& order)
{
	long		sequenceNumber	= 0;
	CMessageApp *pApp			= (CMessageApp *)AfxGetApp();	

	try
	{
		//if (order.ExecInst() != COrder::LastExecInst)
		//{
		//	throw CRejectOrder("Execution Instruction not supported");
		//}  // This is just temperary and should be removed once BRASS fixed the "Removing ExecInst by Replace".

		// lookup and set traders clearing account code
		// Brass requires ClearingAccountID for the trader in Fix Tag 1
		// and FixSession assign ClearingID of COrder to Fix Tag 1
		// so we assign ClearingAccount passed from OM to ClearingID
		if (order.ClearingAccount() == "")
		{
			order.SetClearingId(m_DefaultAccount);
		}
		else
		{
			order.SetClearingId(order.ClearingAccount());
		}

		
		order.SetOrderTag(((CFixSession&) pApp->Exchange()).GetNextOutgoingSeqNum());

		ExchangeProcess(order, sequenceNumber);

		// map tags
		m_pTags->CreateNewMap(order.StaffordTag(), order.OrderTag(), sequenceNumber, order.SourceId(), "", "");
		m_pTags->SetCancelTag("");
		m_pTags->SetShares(order.Shares());
		m_pTags->SetPrice(order.dPrice());
		m_pTags->SetNewShares(order.Shares());
		m_pTags->SetNewPrice(order.dPrice());
		m_pTags->SetRemainingShares(order.Shares());
		m_pTags->SetOriginalOrderTime(CTime::GetCurrentTime().FormatGmt("%Y%m%d-%H:%M:%S"));
		m_pTags->SetSide(order.Side());

		// update status
		pApp->Status().SetStatus(order);
		pApp->ProcessDebug("ORDER SENT", order.Debug());
	}
	catch(CBrassSessionOrderException)
	{
		throw CRejectOrder("Unable To Send");
	}

	catch (const char *Message)
	{
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Brass Handler", Message);
	}	
}

void CBrassHandler::Process(CReject& reject)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();		
	
	// set tags
	reject.SetOrderTag(m_pTags->OrderTag());
	reject.SetExchangeTag(m_pTags->ExchangeTag());
	reject.SetCancelTag(m_pTags->CancelTag());
	reject.SetStaffordTag(m_pTags->StaffordTag());

	// update Status
	pApp->Status().SetStatus(reject);
}

void CBrassHandler::Process(CReplace& replace)
{
	CMessageApp *pApp = (CMessageApp *) AfxGetApp();	
	CFixedString CancelTag;

	try
	{
		if (TRUE == m_pTags->ReplacePending())
		{
			throw CRejectReplace("Pending Replace");
		}

		CFixedString replaceTag;

		// set the exchange's tag an the fix order tag
		replace.SetExchangeTag(m_pTags->ExchangeTag());
		replace.SetOrderTag(m_pTags->OrderTag());
		char OrigTime[10];
		strcpy(OrigTime, m_pTags->OrderTag());
		OrigTime[8] = '\0';
		
		replaceTag = CTime::GetCurrentTime().Format("%Y%m%d");
		replaceTag = replaceTag + (long)((CFixSession&) pApp->Exchange()).GetNextOutgoingSeqNum();
		
		replace.SetCancelTag(replaceTag.GetCString());
		replace.SetOriginalShares(m_pTags->Shares());
		replace.SetOriginalPrice(m_pTags->Price());
		replace.SetLeaveShares(m_pTags->RemainingShares());
		replace.SetOriginalOrderTime(OrigTime);

		ExchangeProcess(replace);

		m_pTags->SetCancelTag(replace.CancelTag());
		m_pTags->SetNewShares(replace.Shares());
		m_pTags->SetNewPrice(replace.Price());
		m_pTags->SetNewTIF(replace.TIF());
		m_pTags->SetNewExecInsts(replace.ExecInst());
		m_pTags->SetNewStopPrice(replace.StopPrice());
		m_pTags->SetNewType(replace.Type());

		m_pTags->SetReplacePending(TRUE);

		pApp->ProcessDebug("REPLACE SENT", "");
	}
	catch(CBrassSessionReplaceException)
	{
		throw CRejectReplace("Unable To Send");
	}
	catch(CRejectReplace& e)
	{
		// generate reject
		CReplaceReject reject("Unable To Send", replace);
		
		reject.SetReason(e.what());
		
		// update Status
		pApp->Status().SetStatus(reject);
		pApp->ProcessDebug("SEND REJECT", "");
	}
	catch (const char *Message)
	{
		pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Brass Handler", Message);
	}
}

//////////////////////////////////////////////////////////////////////
// Overidden Exception Processing Methods
//////////////////////////////////////////////////////////////////////
void CBrassHandler::Reject(CCancel& cancel)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	// generate reject
	CSendCancelReject cancelReject(cancel);

	// set the cancel tag which is generated by the fix session
	m_pTags->SetCancelTag(cancel.CancelTag());
	
	// update Status
	pApp->Status().SetStatus(cancelReject);
	pApp->ProcessDebug("SEND REJECT", cancel.Debug());
}

void CBrassHandler::Reject(COrder& order, const char* reason)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	CReject	reject(reason, order);			
	pApp->Status().SetStatus(reject);

	pApp->ProcessDebug("ORDER REJECT", (LPCSTR) order.Debug());
}

void CBrassHandler::Reject(CReplace& replace, const char* reason)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	CReplaceReject reject(reason, replace);

	// update Status
	pApp->Status().SetStatus(reject);
	pApp->ProcessDebug("SEND REJECT", "");
}

void CBrassHandler::OrderNotFound(CExecution& execution)
{
	CString error;
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	if(execution.Status() == COrder::PartiallyFilled || execution.Status() == COrder::Filled)
	{
		if ((execution.OrderTag()).Right(6) == _T("MANUAL"))
		{
			CBRASSData* pBRASSData = m_pBRASSInterface->GetMemoryAt(execution.Account());

			if(pBRASSData)
			{
				execution.SetClearingAccount(pBRASSData->GetClearingAcID());
				execution.SetLocalAccount(pBRASSData->GetLocalAcAcr());
				execution.SetFirm(pBRASSData->GetFirmAcr());
				execution.SetTrader(pBRASSData->GetTraderAcr());
			}

			pApp->Status().SetStatus(execution);
			return;
		}
	}

	error.Format("Unable To Find Order Tag (%s) In Tag Map For OrderId(%s): (%s)", execution.OrderTag(), execution.StaffordTag(), execution.Debug());
	pApp->ProcessErrorEx(MODE_ERR_DEVELOPER,"Brass Handler", error);
}

//////////////////////////////////////////////////////////////////////
// Overidden Memory Map Methods
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Exchange Session Methods
//////////////////////////////////////////////////////////////////////

void CBrassHandler::ExchangeProcess(CCancel& cancel)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	if (FALSE == pApp->Exchange().ProcessCancel(cancel))
	{
		throw CBrassSessionCancelException();
	}
}

void CBrassHandler::ExchangeProcess(COrder& order, long& sequenceNumber)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	// create and send fix order off to terra
	if ((sequenceNumber = pApp->Exchange().ProcessEntry(order)) <= 0)
	{
		throw CBrassSessionOrderException();
	}
}

void CBrassHandler::ExchangeProcess(CReplace& replace)
{
	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	if(FALSE == pApp->Exchange().ProcessReplace(replace))
	{
		throw CBrassSessionReplaceException();
	}
}

void CBrassHandler::ProcessReplacedOrder(CExecution& execution)
{
	// set new order tag
	m_pTags->SetOrderTag(m_pTags->CancelTag());
	m_pTags->SetCancelTag("");
	m_pTags->SetReplacePending(FALSE);

	m_pTags->SetShares(m_pTags->NewShares());
	m_pTags->SetPrice(m_pTags->NewPrice());

	m_pTags->SetRemainingShares(m_pTags->NewShares());
	execution.SetShares(m_pTags->NewShares());

	m_pTags->SetOriginalOrderTime(CTime::GetCurrentTime().FormatGmt("%Y%m%d-%H:%M:%S"));

	execution.SetPrice(m_pTags->NewPrice());
	execution.SetTimeInForce(m_pTags->NewTIF());
	execution.SetInstructions(m_pTags->NewExecInst());
	execution.SetStopPrice(m_pTags->NewStopPrice());
	execution.SetOrderType(m_pTags->NewType());
	execution.SetOrderTag(m_pTags->OrderTag());

	//execution.SetStatus(COrder::Open);
	execution.SetStatus(COrder::Replaced);
}

//////////////////////////////////////////////////////////////////////
// Overidden Memory Map Methods
//////////////////////////////////////////////////////////////////////

bool CBrassHandler::LoadOrder(CCancelReject& cancelReject)
{
	if (TRUE == m_pTags->GetTags(MAP_CANCEL_TAG_QUERY, cancelReject.CancelTag()))
	{
		return true;
	}

	return false;
}

bool CBrassHandler::LoadOrder(CExecution& execution)
{
	if (TRUE == m_pTags->GetTags(MAP_ORDER_TAG_QUERY, execution.OrderTag()))
	{
		return true;
	}

	if (TRUE == m_pTags->GetTags(MAP_CANCEL_TAG_QUERY, execution.OrderTag()))
	{
		return true;
	}

	return false;
}
