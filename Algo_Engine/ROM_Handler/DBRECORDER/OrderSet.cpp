// OrderSet.cpp : implementation file
//

#include "stdafx.h"
#include "DBRecorder.h"
#include "OrderSet.h"
#include "Message.h"
#include "Order.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COrderSet

IMPLEMENT_DYNAMIC(COrderSet, CRecordset)

COrderSet::COrderSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(COrderSet)
	m_Exchange = 0;
	m_Status = 0;
	m_Type = 0;
	m_Capacity = 0;
	m_TimeInForce = 0;
	m_Side = 0;
	m_ClearingAccount = _T("");
	m_ClearingId = _T("");
	m_LocalAccount = _T("");
	m_Firm = _T("");
	m_Instructions = _T("");
	m_Symbol = _T("");
	m_Trader = _T("");
	m_Price = 0.0;
	m_Owner = _T("");
	m_SourceId = _T("");
	m_Floor = 0;
	m_MinQty = 0;
	m_PriceCheck = 0;
	m_Shares = 0;
	m_OriginalShares = 0;
	m_ReplacementPrice = 0.0;
	m_PendingReplace = 0;
	m_StopPrice = 0.0;
	m_Discretionary = 0;
	m_Solicited = 0;
	m_ClientTag = _T("");
	m_OMTag = _T("");
	m_Client = _T("");
	m_TradeFor = _T("");
	m_ExDestination = _T("");
	m_ExpDate = _T("");
	m_CallOrPut = _T("");
	m_StrikePrice = 0.0;
	m_nFields = 35;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;

	m_SQLStatement = "[RecorderOrders]"; //"[dbo].[RecorderOrders]";
}


CString COrderSet::GetDefaultConnect()
{
	return _T("");
}

CString COrderSet::GetDefaultSQL()
{
	return _T(m_SQLStatement);
}

void COrderSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(COrderSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Int(pFX, _T("[Exchange]"), m_Exchange);
	RFX_Int(pFX, _T("[Status]"), m_Status);
	RFX_Int(pFX, _T("[Type]"), m_Type);
	RFX_Int(pFX, _T("[Capacity]"), m_Capacity);
	RFX_Int(pFX, _T("[TimeInForce]"), m_TimeInForce);
	RFX_Int(pFX, _T("[Side]"), m_Side);
	RFX_Text(pFX, _T("[ClearingAccount]"), m_ClearingAccount);
	RFX_Text(pFX, _T("[ClearingId]"), m_ClearingId);
	RFX_Text(pFX, _T("[LocalAccount]"), m_LocalAccount);
	RFX_Text(pFX, _T("[Firm]"), m_Firm);
	RFX_Text(pFX, _T("[Instructions]"), m_Instructions);
	RFX_Text(pFX, _T("[Symbol]"), m_Symbol);
	RFX_Text(pFX, _T("[Trader]"), m_Trader);
	RFX_Double(pFX, _T("[Price]"), m_Price);
	RFX_Text(pFX, _T("[Owner]"), m_Owner);
	RFX_Text(pFX, _T("[SourceId]"), m_SourceId);
	RFX_Long(pFX, _T("[Floor]"), m_Floor);
	RFX_Long(pFX, _T("[MinQty]"), m_MinQty);
	RFX_Long(pFX, _T("[PriceCheck]"), m_PriceCheck);
	RFX_Long(pFX, _T("[Shares]"), m_Shares);
	RFX_Long(pFX, _T("[OriginalShares]"), m_OriginalShares);
	RFX_Double(pFX, _T("[ReplacementPrice]"), m_ReplacementPrice);
	RFX_Long(pFX, _T("[PendingReplace]"), m_PendingReplace);
	RFX_Double(pFX, _T("[StopPrice]"), m_StopPrice);
	RFX_Byte(pFX, _T("[Discretionary]"), m_Discretionary);
	RFX_Byte(pFX, _T("[Solicited]"), m_Solicited);
	RFX_Text(pFX, _T("[ClientTag]"), m_ClientTag);
	RFX_Text(pFX, _T("[OMTag]"), m_OMTag);
	RFX_Date(pFX, _T("[TimeStamp]"), m_TimeStamp);
	RFX_Text(pFX, _T("[Client]"), m_Client);
	RFX_Text(pFX, _T("[TradeFor]"), m_TradeFor);
	RFX_Text(pFX, _T("[ExDestination]"), m_ExDestination);
	RFX_Text(pFX, _T("[ExpDate]"), m_ExpDate);
	RFX_Text(pFX, _T("[CallOrPut]"), m_CallOrPut);
	RFX_Double(pFX, _T("[StrikePrice]"), m_StrikePrice);
	//}}AFX_FIELD_MAP

	//RFX_Int(pFX, _T("[TradeID]"), m_TradeID);
}

/////////////////////////////////////////////////////////////////////////////
// COrderSet diagnostics

#ifdef _DEBUG
void COrderSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void COrderSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG

//////////////////////////////////////////////////////////////////////////////
// commands

BOOL	COrderSet::New(const CCsv& Order, const CString& Client)
{
	CRecorderApp *pApp = (CRecorderApp *)AfxGetApp();

	m_SQLStatement = "[RecorderOrders]"; //"[dbo].[RecorderOrders]";

	try
	{	
		if (Open(CRecordset::snapshot, m_SQLStatement, CRecordset::appendOnly|CRecordset::executeDirect)) // Open(CRecordset::snapshot)
		{
			AddNew();

			m_Exchange				= Order.nGet(CSV_EXCHANGE);
			m_Status				= Order.nGet(CSV_STATUS);
			m_Type					= Order.nGet(CSV_TYPE);
			m_Capacity				= Order.nGet(CSV_CAPACITY);
			m_TimeInForce			= Order.nGet(CSV_TIF);
			m_Side					= Order.nGet(CSV_SIDE);
			m_ClearingAccount		= Order.Get(CSV_CLEARINGACCOUNT);
			m_ClearingId			= Order.Get(CSV_CLEARINGID);
			m_LocalAccount			= Order.Get(CSV_LOCALACCOUNT);
			m_Firm					= Order.Get(CSV_FIRM);
			m_Instructions			= Order.Get(CSV_INSTRUCTIONS);
			m_Symbol				= Order.Get(CSV_DBSYMBOL);
			if(!m_Symbol.GetLength())
				m_Symbol			= Order.Get(CSV_SYMBOL);
			m_Trader				= Order.Get(CSV_TRADER);
			m_Price					= Order.dGet(CSV_PRICE);
			m_Owner					= Order.Get(CSV_OWNER);
			m_SourceId				= "";
			m_Floor					= Order.nGet(CSV_FLOOR);
			m_MinQty				= Order.nGet(CSV_MINQTY);
			m_PriceCheck			= Order.nGet(CSV_PRICECHECK);
			m_Shares				= Order.nGet(CSV_SHARES);
			m_OriginalShares		= Order.nGet(CSV_SHARES);
			m_ReplacementPrice		= 0.0;
			m_PendingReplace		= 0;
			m_StopPrice				= Order.dGet(CSV_STOPPRICE);
			m_Discretionary			= Order.nGet(CSV_DISCRETIONARY);
			m_Solicited				= Order.nGet(CSV_SOLICITED);
			m_ClientTag				= Order.Get(CSV_TAG);
			m_OMTag					= Order.Get(CSV_STAFFORD_TAG);
			m_Client				= Client;
			m_TradeFor				= Order.Get(CSV_TRADE_FOR);
			m_TimeStamp				= CTime::GetCurrentTime();
			m_TradeID				= 0;
			m_ExDestination			= Order.Get(CSV_EXDEST);
			m_ExpDate				= Order.Get(CSV_EXPDATE);
			m_CallOrPut				= (char)Order.cGet(CSV_CALLPUT);
			m_StrikePrice			= Order.dGet(CSV_STRIKEPRICE);
			
			Update();

			Close();

			return(TRUE);
		}
	}

	catch (CDBException *e)
	{
		CString Error;
		Error.Format("EXCEPTION: Error Adding To Order Table (%s)", e->m_strError);
		pApp->ProcessStatus("Status", Error);
		e->Delete();
		if (IsOpen())
			Close();	
	}

	return (FALSE);
}


BOOL COrderSet::Status(const CCsv& Status)
{
	CRecorderApp *pApp = (CRecorderApp *)AfxGetApp();

	if (Status.nGet(CSV_STATUS) == COrder::CancelRejected)		// ignore cancel rejects
		return TRUE;

	//m_SQLStatement = "select [Exchange],[Status],[Type],[Capacity],[TimeInForce],[Side],[ClearingAccount],[ClearingId],[LocalAccount],[Firm],[Instructions],[Symbol],[Trader],[Price],[Owner],[SourceId],[Floor],[MinQty],[PriceCheck],[Shares],[OriginalShares],[ReplacementPrice],[PendingReplace],[StopPrice],[Discretionary],[Solicited],[ClientTag],[OMTag],[TimeStamp],[Client],[TradeFor] from [RecorderOrders] where [OMTag]='" + Status.Get(CSV_STAFFORD_TAG) + "'";
	m_SQLStatement = "select * from [RecorderOrders] where [OMTag]='" + Status.Get(CSV_STAFFORD_TAG) + "'";

	try
	{
		if (Open(CRecordset::snapshot, m_SQLStatement, CRecordset::executeDirect))
		{
			if (GetRecordCount())
			{
				Edit();
			
				m_Status = 	Status.nGet(CSV_STATUS);

				// if execution then reduce the order's outstanding shares
					if ((m_Status == COrder::Filled) || (m_Status == COrder::PartiallyFilled))
						m_Shares = m_Shares - Status.nGet(CSV_SHARES);
			
				Update();
				Close();
				return TRUE;
			}
			Close();
		}
	}

	catch (CDBException *e)
	{
		CString Error;
		Error.Format("EXCEPTION: Error Updating Order Table (%s)", e->m_strError);
		pApp->ProcessStatus("Status", Error);
		e->Delete();
		if (IsOpen())
			Close();
	}

	return FALSE;
}

BOOL COrderSet::Zap(const CCsv& Status)
{
	CRecorderApp *pApp = (CRecorderApp *)AfxGetApp();

	//m_SQLStatement = "select [Exchange],[Status],[Type],[Capacity],[TimeInForce],[Side],[ClearingAccount],[ClearingId],[LocalAccount],[Firm],[Instructions],[Symbol],[Trader],[Price],[Owner],[SourceId],[Floor],[MinQty],[PriceCheck],[Shares],[OriginalShares],[ReplacementPrice],[PendingReplace],[StopPrice],[Discretionary],[Solicited],[ClientTag],[OMTag],[TimeStamp],[Client],[TradeFor] from [RecorderOrders] where [OMTag]='" + Status.Get(CSV_QUERY_TAG) + "'";
	m_SQLStatement = "select * from [RecorderOrders] where [OMTag]='" + Status.Get(CSV_STAFFORD_TAG) + "'";

	try
	{
		if (Open(CRecordset::snapshot))
		{
			if (GetRecordCount())
			{
				Edit();
			
				m_Status = 	-1; 
				m_Shares =	0;
			
				Update();
				Close();
				return TRUE;
			}
			Close();	
		}
	}

	catch (CDBException *e)
	{
		CString Error;
		Error.Format("EXCEPTION: Error Zapping Order (%s)", e->m_strError);
		pApp->ProcessStatus("Status", Error);
		e->Delete();
		if (IsOpen())
			Close();	
	}

	return FALSE;
}

BOOL COrderSet::TradeID(const CCsv& Exec, int TradeID)
{
	CRecorderApp *pApp = (CRecorderApp *)AfxGetApp();

	//m_SQLStatement = "select [Exchange],[Status],[Type],[Capacity],[TimeInForce],[Side],[ClearingAccount],[ClearingId],[LocalAccount],[Firm],[Instructions],[Symbol],[Trader],[Price],[Owner],[SourceId],[Floor],[MinQty],[PriceCheck],[Shares],[OriginalShares],[ReplacementPrice],[PendingReplace],[StopPrice],[Discretionary],[Solicited],[ClientTag],[OMTag],[TimeStamp],[Client],[TradeFor] from [RecorderOrders] where [OMTag]='" + Exec.Get(CSV_QUERY_TAG) + "'";
	m_SQLStatement = "select * from [RecorderOrders] where [OMTag]='" + Exec.Get(CSV_QUERY_TAG) + "'";

	try
	{
		if (Open(CRecordset::snapshot))
		{
			if (GetRecordCount())
			{
				Edit();
			
				m_TradeID = TradeID;

				Update();
				Close();
				return TRUE;
			}
			Close();	
		}
	}

	catch (CDBException *e)
	{
		CString Error;
		Error.Format("EXCEPTION: Error Adding TradeID Order (%s)", e->m_strError);
		pApp->ProcessStatus("Status", Error);
		e->Delete();
		if (IsOpen())
			Close();	
	}

	return FALSE;
}

BOOL COrderSet::GetOrder(const CCsv& Order)
{
	CRecorderApp *pApp = (CRecorderApp *)AfxGetApp();

	//m_SQLStatement = "select [Exchange],[Status],[Type],[Capacity],[TimeInForce],[Side],[ClearingAccount],[ClearingId],[LocalAccount],[Firm],[Instructions],[Symbol],[Trader],[Price],[Owner],[SourceId],[Floor],[MinQty],[PriceCheck],[Shares],[OriginalShares],[ReplacementPrice],[PendingReplace],[StopPrice],[Discretionary],[Solicited],[ClientTag],[OMTag],[TimeStamp],[Client],[TradeFor] from [RecorderOrders] where [OMTag]='" + Order.Get(CSV_STAFFORD_TAG) + "'";
	m_SQLStatement = "select * from [RecorderOrders] where [OMTag]='" + Order.Get(CSV_STAFFORD_TAG) + "'";

	try
	{
		if (Open(CRecordset::snapshot, m_SQLStatement, CRecordset::executeDirect|CRecordset::readOnly))
		{
			if (GetRecordCount())
			{
				Close();
				return TRUE;
			}
			Close();
		}
	}

	catch (CDBException *e)
	{
		CString Error;
		Error.Format("EXCEPTION: Error Getting Order (%s)", e->m_strError);
		pApp->ProcessStatus("Status", Error);
		e->Delete();
		if (IsOpen())
			Close();
	}

	return FALSE;
}

BOOL COrderSet::Query(const CString& Client, CClients& ClientList)
{
	CRecorderApp	*pApp = (CRecorderApp *)AfxGetApp();
	CTime			Today = CTime::GetCurrentTime();
	CCsv			CSV((char)CMessage::Query);
	
	//m_SQLStatement = "select [Exchange],[Status],[Type],[Capacity],[TimeInForce],[Side],[ClearingAccount],[ClearingId],[LocalAccount],[Firm],[Instructions],[Symbol],[Trader],[Price],[Owner],[SourceId],[Floor],[MinQty],[PriceCheck],[Shares],[OriginalShares],[ReplacementPrice],[PendingReplace],[StopPrice],[Discretionary],[Solicited],[ClientTag],[OMTag],[TimeStamp],[Client],[TradeFor],[TradeID] from [RecorderOrders] where [Client]='" + Client + "' AND [TimeStamp] >= '" + Today.Format("%Y/%m/%d")  + "'";
	m_SQLStatement = "select * from [RecorderOrders] where [Client]='" + Client + "' AND [TimeStamp] >= '" + Today.Format("%Y/%m/%d")  + "'";

	try
	{
		if (Open(CRecordset::snapshot))
		{
			if (GetRecordCount())
			{
				while (!IsEOF())
				{
					switch (m_Status)
					{
						case COrder::Open:
						case COrder::PartiallyFilled:
						case COrder::PendingCancel:
						case COrder::PendingNew:
						case COrder::CancelRejected:	
							CSV.SetAt(CSV_TRADER,			m_Trader);
							CSV.SetAt(CSV_TRADE_FOR,		m_TradeFor);
							CSV.SetAt(CSV_TAG,				m_ClientTag);
							CSV.SetAt(CSV_SIDE,				m_Side);
							CSV.SetAt(CSV_SYMBOL,			m_Symbol);
							CSV.SetAt(CSV_OWNER,			m_Owner);
							CSV.SetAt(CSV_SHARES,			m_Shares);
							CSV.SetAt(CSV_PRICE,			m_Price);
							CSV.SetAt(CSV_TYPE,				m_Type);
							CSV.SetAt(CSV_TIF,				m_TimeInForce);
							CSV.SetAt(CSV_CAPACITY,			m_Capacity);
							CSV.SetAt(CSV_CLEARINGID,		m_ClearingId);
							CSV.SetAt(CSV_CLEARINGACCOUNT,	m_ClearingAccount);
							CSV.SetAt(CSV_EXCHANGE,			m_Exchange);
							CSV.SetAt(CSV_STATUS,			m_Status);
							CSV.SetAt(CSV_INSTRUCTIONS,		m_Instructions);
							CSV.SetAt(CSV_PRICECHECK,		m_PriceCheck);
							CSV.SetAt(CSV_FLOOR,			m_Floor);
							CSV.SetAt(CSV_MINQTY,			m_MinQty);
							CSV.SetAt(CSV_STOPPRICE,		m_StopPrice);
							CSV.SetAt(CSV_DISCRETIONARY,	m_Discretionary);
							CSV.SetAt(CSV_SOLICITED,		m_Solicited);
							CSV.SetAt(CSV_LOCALACCOUNT,		m_LocalAccount);
							CSV.SetAt(CSV_FIRM,				m_Firm);
							CSV.SetAt(CSV_QUERY_TAG,		m_OMTag);
							CSV.SetAt(CSV_EXDEST,			m_ExDestination);
							CSV.SetAt(CSV_EXPDATE,			m_ExpDate);
							CSV.SetAt(CSV_CALLPUT,			m_CallOrPut);
							CSV.SetAt(CSV_STRIKEPRICE,		m_StrikePrice);
							
							ClientList.Send(Client, CSV.Message());

							break;
					};

					MoveNext();
				}
			}

		Close();
			
		return TRUE;
		}
	}

	catch (CDBException *e)
	{
		CString Error;
		Error.Format("EXCEPTION: Error Zapping Order (%s)", e->m_strError);
		pApp->ProcessStatus("Status", Error);
		e->Delete();
		if (IsOpen())
			Close();		
	}

	return FALSE;
} 