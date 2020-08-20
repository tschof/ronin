// TradeTableSet.cpp : implementation file
//

#include "stdafx.h"
#include "Message.h"
#include "Order.h"
#include "Connection.h"
#include "ClientConnection.h"
#include "OrderSet.h"
#include "TradeTableSet.h"

#pragma warning(disable:4786)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTradeTableSet

IMPLEMENT_DYNAMIC(CTradeTableSet, CRecordset)

CTradeTableSet::CTradeTableSet(CDatabase* pdb)
	: CRecordset(pdb)
{

	CTime CurrentTime = CTime::GetCurrentTime();

	//{{AFX_FIELD_INIT(CTradeTableSet)
	m_trade_id					= 0;
	m_trader						= _T("");
	m_local_ac					= _T("");
	m_clearing_ac				= _T("");
	m_clearing_firm			= _T("");
	m_contra						= _T("");
	m_contra_firm				= _T("");
	m_exchange					= _T("");
	m_base_cusip				= _T("");
	m_symbol						= _T("");
	m_sec_type					= _T("");
	m_cp							= _T("");
	m_strike						= 0.0;
	m_qty							= 0;
	m_price						= 0.0;
	m_spread_id					= 0;
	m_spread_type				= _T("");
	m_source						= "D";
	m_clearing_ac_acr			= _T("");
	m_clearing_firm_id		= _T("");
	m_trade_date				= CurrentTime;
	m_entry_date				= CurrentTime;
	m_change_date				= CurrentTime;
	m_exp_date					= CurrentTime;
	m_uid							= _T("");
	m_trade_status				= 0;
	m_active						= TRUE;
	m_transno					= _T("");
	m_transdt					= _T("");
	m_msgseq						= 0;
	m_trade_stat				= _T("");
	m_purpose					= "FTP TNTO";

	SetType(TRADE_ADD_QUERY);
	m_param_trade_status		= 0;
	m_param_uid					= _T("");
	m_param_trader				= _T("");
	m_param_local_ac			= _T("");
	m_param_clearing_ac_acr = _T("");
	m_param_clearing_firm	= _T("");
	m_param_contra				= _T("");
	m_param_contra_firm		= _T("");
	m_param_base				= _T("");
	m_param_sec_types			= _T("");
	m_param_cp					= _T("");
	m_param_low_strike		= 0.0;
	m_param_high_strike		= 0.0;
	m_param_bs					= _T("");
	m_param_low_qty			= 0;
	m_param_high_qty			= 0;
	m_param_low_price			= 0.0;
	m_param_high_price		= 0.0;
	m_param_purpose			= "FTP TNTO";
	m_param_start_exp_date	= _T("");
	m_param_end_exp_date		= _T("");
	m_param_start_trade_date= _T("");
	m_param_end_trade_date	= _T("");
	m_param_start_entry_date= _T("");
	m_param_end_entry_date	= _T("");
	//}}AFX_FIELD_INIT

	m_nDefaultType = snapshot;
}


void CTradeTableSet::SetType(int Type)
{
	m_Type = Type;

	switch( m_Type )
		{
		case TRADE_ADD_QUERY:
			m_nParams = 28;
			m_nFields = 1;
			m_TypeStr = "DA";
			break;
		case TRADE_UPDATE_QUERY:
			m_nParams = 26;
			m_nFields = 1;
			m_TypeStr = "DC";
			break;
		case TRADE_REMOVE_QUERY:
			m_nParams = 7;
			m_nFields = 1;
			m_TypeStr = "DD";
			break;
		default:
			m_nParams = 0;
			m_nFields = 0;
		};

	
}


CString CTradeTableSet::GetDefaultConnect()
{
	return _T("");
}

CString CTradeTableSet::GetDefaultSQL()
{
	CString sql;

	switch( m_Type )
		{
		case TRADE_ADD_QUERY:
			sql = "{CALL add_trade_ex_V2 ?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?}";
			break;
		case TRADE_UPDATE_QUERY:
			sql = "{CALL update_trade_qp ?,?,?}";
			break;
		case TRADE_REMOVE_QUERY:
			sql = "{CALL transaction_delete ?,?,?,?,?,?,?,?}";
			break;
		default: 
			sql = "";
			break;
		};

return _T(sql);
}

void CTradeTableSet::DoFieldExchange(CFieldExchange* pFX)
{
	CString contra;

	if (m_nParams > 0)
		{

		pFX->SetFieldType(CFieldExchange::param);

		switch( m_Type )
			{
			case TRADE_ADD_QUERY:

				RFX_Text(pFX,	_T("[trader]"),				m_trader);
				RFX_Text(pFX,	_T("[local_ac]"),				m_local_ac);
				RFX_Text(pFX,	_T("[clearing_ac]"),			m_clearing_ac);
				RFX_Text(pFX,	_T("[firm]"),					m_clearing_firm);
				RFX_Text(pFX,	_T("[contra]"),				m_purpose);
				RFX_Text(pFX,	_T("[base_cusip]"),			m_base_cusip);
				RFX_Text(pFX,	_T("[type]"),					m_sec_type);
				RFX_Date(pFX,	_T("[expdate]"),				m_exp_date);
				RFX_Double(pFX,_T("[strike]"),				m_strike);
				RFX_Text(pFX,	_T("[cp]"),						m_cp);
				RFX_Long(pFX,	_T("[quant]"),					m_qty);
				RFX_Double(pFX,_T("[price]"),					m_price);
				RFX_Date(pFX,	_T("[time]"),					m_trade_date);
				RFX_Date(pFX,	_T("[timeofentry]"),			m_entry_date);
				RFX_Int(pFX,	_T("[active]"),				m_active );
				RFX_Long(pFX,	_T("[spread_id]"),			m_spread_id);
				RFX_Long(pFX,	_T("[tradeId]"),				m_trade_id);
				RFX_Text(pFX,	_T("[trade_type]"),			m_TypeStr );
				RFX_Text(pFX,	_T("[spread_type]"),			m_spread_type );
				RFX_Text(pFX,	_T("[exchange]"),				m_exchange);
				RFX_Text(pFX,	_T("[firm_id]"),				m_clearing_firm_id);
				RFX_Text(pFX,	_T("[uid]"),					m_uid);
				RFX_Text(pFX,	_T("[transNo]"),				m_transno );
				RFX_Text(pFX,	_T("[transDt]"),				m_transdt );
				RFX_Int(pFX,	_T("[msgSeq]"),				m_msgseq );
				RFX_Text(pFX,	_T("[source]"),				m_source);
				RFX_Text(pFX,	_T("[contra_trader_acr]"), m_contra);
				RFX_Text(pFX,	_T("[contra_firm_acr]"),	m_contra_firm);

				break;

			case TRADE_UPDATE_QUERY:

				RFX_Long(pFX,	_T("[id]"),						m_trade_id);
				RFX_Text(pFX,	_T("[trader]"),				m_trader);
				RFX_Text(pFX,	_T("[local_ac]"),				m_local_ac);
				RFX_Text(pFX,	_T("[clearing_ac]"),			m_clearing_ac);
				RFX_Text(pFX,	_T("[clearing_firm]"),		m_clearing_firm);
				RFX_Text(pFX,	_T("[contra]"),				m_purpose);
				RFX_Text(pFX,	_T("[base_cusip]"),			m_base_cusip);
				RFX_Text(pFX,	_T("[type]"),					m_sec_type);
				RFX_Date(pFX,	_T("[expdate]"),				m_exp_date);
				RFX_Double(pFX,_T("[strike]"),				m_strike);
				RFX_Text(pFX,	_T("[cp]"),						m_cp);
				RFX_Long(pFX,	_T("[quant]"),					m_qty);
				RFX_Double(pFX,_T("[price]"),					m_price);
				RFX_Date(pFX,	_T("[timeoftrade]"),			m_trade_date);
				RFX_Date(pFX,	_T("[timeofentry]"),			m_entry_date);
				RFX_Int(pFX,	_T("[active]"),				m_active );
				RFX_Long(pFX,	_T("[spread_id]"),			m_spread_id);
				RFX_Text(pFX,	_T("[transType]"),			m_TypeStr);
				RFX_Text(pFX,	_T("[exchange]"),				m_exchange);
				RFX_Text(pFX,	_T("[trade_stat]"),			m_trade_stat);
				RFX_Text(pFX,	_T("[firm_id]"),				m_clearing_firm_id);
				RFX_Text(pFX,	_T("[uid]"),					m_uid);
				RFX_Text(pFX,	_T("[transNo]"),				m_transno );
				RFX_Text(pFX,	_T("[transDt]"),				m_transdt );
				RFX_Text(pFX,	_T("[contra_trader_acr]"), m_contra);
				RFX_Text(pFX,	_T("[contra_firm_acr]"),	m_contra_firm);

				break;

			case TRADE_REMOVE_QUERY:

				RFX_Long(pFX,	_T("[id]"),						m_trade_id);
				RFX_Text(pFX,	_T("[transType]"),			m_TypeStr);
				RFX_Text(pFX,	_T("[trade_stat]"),			m_trade_stat);
				RFX_Text(pFX,	_T("[transNo]"),				m_transno );
				RFX_Text(pFX,	_T("[transDt]"),				m_transdt );
				RFX_Text(pFX,	_T("[uid]"),					m_uid);
				RFX_Text(pFX,	_T("[firm_id]"),				m_clearing_firm_id);

				break;

			default:
				break;
			};
		}

	pFX->SetFieldType(CFieldExchange::outputColumn);

	switch( m_Type )
		{
		case TRADE_ADD_QUERY:
		case TRADE_UPDATE_QUERY:
		case TRADE_REMOVE_QUERY:

			RFX_Long(pFX,	_T("[status]"),					m_param_trade_id );
			break;

		default:
			break;
		};

	m_cp.TrimRight();
}

long CTradeTableSet::AddTrade(const CCsv& Exec, const COrderSet& Order)
{
	SetType(TRADE_ADD_QUERY);
	SetProperties(Exec, Order);

	m_param_trade_id = -1;

	if (Open(CRecordset::forwardOnly))
		Close();

return(m_param_trade_id);

}

long CTradeTableSet::RemoveTrade(const CCsv& Exec, const COrderSet& Order)
{
	SetType(TRADE_REMOVE_QUERY);
	SetProperties(Exec, Order);

	m_param_trade_id = -1;

	if (Open(CRecordset::forwardOnly))
		Close();

return(m_param_trade_id);

}

long CTradeTableSet::UpdateTrade(const CCsv& Exec, const COrderSet& Order)
{
	SetType(TRADE_UPDATE_QUERY);
	SetProperties(Exec, Order);

	m_param_trade_id = -1;

	if (Open(CRecordset::forwardOnly))
		Close();

return(m_param_trade_id);

}

long CTradeTableSet::AddTrade(const CCsv& Exec)
{
	SetType(TRADE_ADD_QUERY);
	SetProperties(Exec);

	m_param_trade_id = -1;

	if (Open(CRecordset::forwardOnly))
		Close();

return(m_param_trade_id);

}


CString CTradeTableSet::GetTimeString( CTime& time, CString format )
{
	if ( time == CTime(0) )
		return "";
	else
		return time.Format( format );
}

void CTradeTableSet::SetProperties(const CCsv& Exec, const COrderSet& Order)
{
	CTime CurrentTime = CTime::GetCurrentTime();
	CTime	EntryTime = Order.m_TimeStamp;

	m_param_trade_id			= 0;
	m_param_uid					= "";
	m_param_trader				= Order.m_TradeFor;
	m_param_local_ac			= Order.m_LocalAccount;
	m_param_clearing_ac_acr = Order.m_ClearingAccount;
	m_param_clearing_firm	= Order.m_Firm;
	m_param_contra				= "";
	m_param_contra_firm		= Exec.Get(CSV_CONTRA);
	m_param_base				= "";
	m_param_sec_types			= "E";
	m_param_cp					= "";
	m_param_low_strike		= 0.0;
	m_param_high_strike		= 0.0;
	m_param_bs					= COrder::SideString(Order.m_Side);

	switch (Order.m_Side)
		{
		case COrder::Sell:
		case COrder::SellPlus:
		case COrder::Short:
		case COrder::Exempt:
		
			m_param_low_qty			= (-1 * Exec.nGet(CSV_SHARES));
			m_param_high_qty			= (-1 * Exec.nGet(CSV_SHARES));
			break;

		default:

			m_param_low_qty			= Exec.nGet(CSV_SHARES);
			m_param_high_qty			= Exec.nGet(CSV_SHARES);
			break;
		};

	m_param_low_price			= Exec.dGet(CSV_PRICE);
	m_param_high_price		= Exec.dGet(CSV_PRICE);
	m_param_start_trade_date= GetTimeString(CurrentTime);
	m_param_end_trade_date	= GetTimeString(CurrentTime);
	m_param_start_entry_date= GetTimeString(EntryTime);
	m_param_end_entry_date	= GetTimeString(EntryTime);
	m_param_start_exp_date	= GetTimeString(CurrentTime);
	m_param_end_exp_date		= GetTimeString(CurrentTime);

	if (COrder::Destination(Order.m_Exchange) == COrder::BRASS)
	{
		m_param_purpose = "FTP STAF";
	}
	else if (COrder::Destination(Order.m_Exchange) == COrder::XCAL)
	{
		m_param_purpose	= "FTP " + CString(COrder::Destination(Order.m_Exchange)) + " O";
	}
	else
	{
		m_param_purpose			= "FTP " + CString(COrder::Destination(Order.m_Exchange));
	}

	m_trader						= Order.m_TradeFor;
	m_local_ac					= Order.m_LocalAccount;
	m_clearing_ac				= Order.m_ClearingAccount;
	m_clearing_firm			= Order.m_Firm;

	if (COrder::Destination(Order.m_Exchange) == COrder::BRASS)
	{
		m_purpose = "FTP STAF";
	}
	else if (COrder::Destination(Order.m_Exchange) == COrder::XCAL)
	{
		m_purpose = "FTP " + CString(COrder::Destination(Order.m_Exchange)) + " O";
	}
	else
	{
		m_purpose			= "FTP " + CString(COrder::Destination(Order.m_Exchange));
	}

	CString	Symbol = Order.m_Symbol;
	int		Index;

	if ((Index = Symbol.Find('.')) > 0)
		{
		if ((Symbol.Mid(Index+1)).FindOneOf("1234567890") < 0)
			Symbol = Symbol.Left(Index);
		}

	m_base_cusip				= Symbol;
	m_sec_type					= "E";
	m_exp_date					= CurrentTime;
	m_strike						= Exec.dGet(CSV_PRICE);;
	m_cp							= "";

	switch (Order.m_Side)
		{
		case COrder::Sell:
		case COrder::SellPlus:
		case COrder::Short:
		case COrder::Exempt:
		
			m_qty			= (-1 * Exec.nGet(CSV_SHARES));
			break;

		default:

			m_qty			= Exec.nGet(CSV_SHARES);
			break;
		};

	m_price						= Exec.dGet(CSV_PRICE);
	m_trade_date				= CurrentTime;
	m_entry_date				= EntryTime;
	m_active						= 1; 
	m_spread_id					= 1;
	m_TypeStr					= "DA";
	m_spread_type				= "";			
	m_exchange					= "O";
	m_clearing_firm_id		= "";
	m_uid							= "";
	m_transno					= "";
	m_transdt					= GetTimeString(CurrentTime);
	m_msgseq						= 0;
	m_source						= " ";
	m_contra						= Exec.Get(CSV_CONTRA);
	m_contra_firm				= Exec.Get(CSV_CONTRA);

}

void CTradeTableSet::SetProperties(const CCsv& Exec)
{
	CTime CurrentTime = CTime::GetCurrentTime();
	CTime	EntryTime = CurrentTime;

	m_param_trade_id			= 0;
	m_param_uid					= "";
	m_param_trader				= Exec.Get(CSV_TRADER);
	m_param_local_ac			= Exec.Get(CSV_LOCALACCOUNT);
	m_param_clearing_ac_acr = Exec.Get(CSV_CLEARINGACCOUNT);
	m_param_clearing_firm	= Exec.Get(CSV_FIRM);
	m_param_contra				= "";
	m_param_contra_firm		= Exec.Get(CSV_CONTRA);
	m_param_base				= "";
	m_param_sec_types			= "E";
	m_param_cp					= "";
	m_param_low_strike		= 0.0;
	m_param_high_strike		= 0.0;
	m_param_bs					= COrder::SideString(Exec.nGet(CSV_SIDE));

	switch (Exec.nGet(CSV_SIDE))
		{
		case COrder::Sell:
		case COrder::SellPlus:
		case COrder::Short:
		case COrder::Exempt:
		
			m_param_low_qty			= (-1 * Exec.nGet(CSV_SHARES));
			m_param_high_qty			= (-1 * Exec.nGet(CSV_SHARES));
			break;

		default:

			m_param_low_qty			= Exec.nGet(CSV_SHARES);
			m_param_high_qty			= Exec.nGet(CSV_SHARES);
			break;
		};

	m_param_low_price			= Exec.dGet(CSV_PRICE);
	m_param_high_price		= Exec.dGet(CSV_PRICE);
	m_param_start_trade_date= GetTimeString(CurrentTime);
	m_param_end_trade_date	= GetTimeString(CurrentTime);
	m_param_start_entry_date= GetTimeString(EntryTime);
	m_param_end_entry_date	= GetTimeString(EntryTime);
	m_param_start_exp_date	= GetTimeString(CurrentTime);
	m_param_end_exp_date		= GetTimeString(CurrentTime);

	if (COrder::Destination(Exec.nGet(CSV_EXCHANGE)) == COrder::BRASS)
	{
		m_param_purpose = "FTP STAF";
	}
	else if (COrder::Destination(Exec.nGet(CSV_EXCHANGE)) == COrder::XCAL)
	{
		m_param_purpose	= "FTP " + CString(COrder::Destination(Exec.nGet(CSV_EXCHANGE))) + " O";
	}
	else
	{
		m_param_purpose			= "FTP " + CString(COrder::Destination(Exec.nGet(CSV_EXCHANGE)));
	}

	m_trader						= Exec.Get(CSV_TRADER);
	m_local_ac					= Exec.Get(CSV_LOCALACCOUNT);
	m_clearing_ac				= Exec.Get(CSV_CLEARINGACCOUNT);
	m_clearing_firm			= Exec.Get(CSV_FIRM);

	if (COrder::Destination(Exec.nGet(CSV_EXCHANGE)) == COrder::BRASS)
	{
		m_purpose = "FTP STAF";
	}
	else if (COrder::Destination(Exec.nGet(CSV_EXCHANGE)) == COrder::XCAL)
	{
		m_purpose = "FTP " + CString(COrder::Destination(Exec.nGet(CSV_EXCHANGE))) + " O";
	}
	else
	{
		m_purpose			= "FTP " + CString(COrder::Destination(Exec.nGet(CSV_EXCHANGE)));
	}

	CString	Symbol = Exec.Get(CSV_SYMBOL);
	int		Index;

	if ((Index = Symbol.Find('.')) > 0)
		{
		if ((Symbol.Mid(Index+1)).FindOneOf("1234567890") < 0)
			Symbol = Symbol.Left(Index);
		}

	m_base_cusip				= Symbol;
	m_sec_type					= "E";
	m_exp_date					= CurrentTime;
	m_strike						= Exec.dGet(CSV_PRICE);;
	m_cp							= "";

	switch (Exec.nGet(CSV_SIDE))
		{
		case COrder::Sell:
		case COrder::SellPlus:
		case COrder::Short:
		case COrder::Exempt:
		
			m_qty			= (-1 * Exec.nGet(CSV_SHARES));
			break;

		default:

			m_qty			= Exec.nGet(CSV_SHARES);
			break;
		};

	m_price						= Exec.dGet(CSV_PRICE);
	m_trade_date				= CurrentTime;
	m_entry_date				= EntryTime;
	m_active						= 1; 
	m_spread_id					= 1;
	m_TypeStr					= "DA";
	m_spread_type				= "";			
	m_exchange					= "O";
	m_clearing_firm_id		= "";
	m_uid							= "";
	m_transno					= "";
	m_transdt					= GetTimeString(CurrentTime);
	m_msgseq						= 0;
	m_source						= " ";
	m_contra						= Exec.Get(CSV_CONTRA);
	m_contra_firm				= Exec.Get(CSV_CONTRA);

}


/////////////////////////////////////////////////////////////////////////////
// CTradeTableSet diagnostics

#ifdef _DEBUG
void CTradeTableSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CTradeTableSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}

#endif //_DEBUG
