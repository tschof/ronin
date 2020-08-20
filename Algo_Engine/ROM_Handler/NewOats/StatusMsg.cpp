// StatusMsg.cpp: implementation of the CStatusMsg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RomCsv.h"
#include "StatusMsg.h"
#include "DBConnection.h"
#include "Locker.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
Lock CStatusMsg::m_Lock;

CStatusMsg::CStatusMsg()
{

}

CStatusMsg::~CStatusMsg()
{

}

CStatusMsg::CStatusMsg( CRomCsv * pCsv ) 
{
	m_szCommand		= pCsv->Get( RFIX::CSV_COMMAND );;
	m_szTrader		= pCsv->Get( RFIX::CSV_TRADER );
	m_szTradeFor	= pCsv->Get( RFIX::CSV_TRADE_FOR );
	m_szTag			= pCsv->Get( RFIX::CSV_TAG );			
	m_nSide			= pCsv->nGet( RFIX::CSV_SIDE );
	m_szSymbol		= pCsv->Get( RFIX::CSV_SYMBOL );		
	m_nShares		= pCsv->nGet( RFIX::CSV_SHARES );
	m_fPrice		= pCsv->dGet( RFIX::CSV_PRICE );

	if (pCsv->IsFieldEmpty(RFIX::CSV_PEGPRICE_OFFSET))
	{
		m_fPegOffset = 0;
	}
	else
	{
		m_fPegOffset	= pCsv->dGet( RFIX::CSV_PEGPRICE_OFFSET );
	}

	m_nType			= pCsv->nGet( RFIX::CSV_TYPE );
	m_nTIF			= pCsv->nGet( RFIX::CSV_TIF );

	m_szLocalAcct	= pCsv->Get( RFIX::CSV_LOCALACCOUNT );
	m_szFirm		= pCsv->Get( RFIX::CSV_FIRM );
	m_szClgAcct		= pCsv->Get( RFIX::CSV_CLEARINGACCOUNT );		
	m_nStatus		= pCsv->nGet( RFIX::CSV_STATUS );
	m_szStaffordTag	= pCsv->Get( RFIX::CSV_STAFFORD_TAG );
	m_nExchange		= pCsv->nGet( RFIX::CSV_EXCHANGE );
	m_szExDest		= pCsv->Get( RFIX::CSV_EXDEST );
	m_fStopPrice	= pCsv->dGet( RFIX::CSV_STOPPRICE );
	m_nCumQty		= pCsv->nGet( RFIX::CSV_CUM_SHARES );
	m_nLeaveQty		= pCsv->nGet( RFIX::CSV_LEAVE_SHARES );
}

bool CStatusMsg::Process()
{
	return true;
}

long CStatusMsg::GetRequestID()
{
	int nPos = m_szStaffordTag.find_last_of('-');
	if( nPos == std::string::npos) 
	{
		return 0;
	}

	int len = m_szStaffordTag.length() - nPos;

	std::string substr = m_szStaffordTag.substr(nPos + 1, len);
	return atol( substr.c_str() );
}