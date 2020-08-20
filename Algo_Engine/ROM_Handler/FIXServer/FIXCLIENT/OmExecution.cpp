// OmExecution.cpp: implementation of the COmExecution class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fixclient.h"
#include "OmExecution.h"
#include <time.h>
#include "FIX42_Messages.h"
#include "Values.h"

#include "string_util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COmExecution::COmExecution( const FIX42::ExecutionReport & report )
	:m_szTraderAcr("")
	,m_szClearingID("")
	,m_szExeTime("")
	,m_nQty(0)
	,m_fPrice(0.0f)
	,m_szOrderTag("")
	,m_szExchange("")
	,m_szSymbol("")
	,m_szSecType("")
	,m_szBase("")
	,m_fStrike(0.0f)
	,m_szCallPut("")
	,m_szExpire("")
	,m_nSide(0)
	,m_nOpenClose(-1)
	,m_szLocalAcct("")
	,m_szRetailAcct("")
	,m_szFirm("")
{
	std::string acct = report.get(FIX::Account());
	std::vector<std::string> vFields;
	string_util::parse( acct, vFields, ' ' );
	m_szTraderAcr = vFields[0];
	m_szClearingID = vFields[1];
	m_szFirm = vFields[2];
	m_szLocalAcct = vFields[3];
	m_szRetailAcct = vFields[4];
	try
	{
		FIX::TransactTime tran = report.get(FIX::TransactTime());
		m_szExeTime = tran.getString();
	}
	catch(...)
	{
		m_szExeTime = "";
	}
	m_nQty = report.get(FIX::LastShares());
	m_fPrice = report.get(FIX::LastPx());
	m_szOrderTag = report.get(FIX::OrderID());
	m_szSymbol = report.get(FIX::Symbol());
//	m_szExchange = report.get(FIX::SecurityExchange());
	m_szExchange = report.get(FIX::LastMkt());
	std::string szSecType = report.get(FIX::SecurityType());
	
	if( szSecType == FIX::SecurityType_PREFERRED_STOCK )
	{
		m_szSecType = "E";
	}
	else if( szSecType == FIX::SecurityType_OPTION )
	{
		m_szSecType = "OE";
		char openclose = report.get(FIX::OpenClose());
		if( openclose == FIX::OpenClose_OPEN )
		{
			m_nOpenClose = 1;
		}
		else if( openclose == FIX::OpenClose_CLOSE )
		{
			m_nOpenClose = 0;
		}
		char callput = report.get(FIX::PutOrCall());
		if( callput == FIX::PutOrCall_CALL )
		{
			m_szCallPut = "C";
		}
		else if( callput == FIX::PutOrCall_PUT )
		{
			m_szCallPut = "P";
		}
		m_szExpire = report.get( FIX::MaturityMonthYear() );
		m_fStrike =	report.get( FIX::StrikePrice() );
		m_szBase = report.get(FIX::SymbolSfx());

	}
	else if( szSecType == FIX::SecurityType_FUTURE )
	{
		m_szSecType = "F";
	}
	char side = report.get(FIX::Side());
	switch( side )
	{
	case '1':
		m_nSide = 1;
		break;
	case '2':
		m_nSide = 2;
		break;
	case '5':
		m_nSide = 5;
		break;
	default:
		m_nSide = -1;
		break;
	}
	float fAvgPrice = report.get(FIX::AvgPx());
	int nCumQty = report.get(FIX::CumQty());
	int nLeaveQty = report.get(FIX::LeavesQty());
	m_szExecID = report.get(FIX::ExecID());
}

COmExecution::~COmExecution()
{

}
