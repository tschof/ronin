// StatusMsg.cpp: implementation of the CStatusMsg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "csv.h"
#include "oats.h"
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

CStatusMsg::CStatusMsg( CCsv * pCsv ) 
{
	m_szCommand		= pCsv->Get( CSV_COMMAND );;
	m_szTrader		= pCsv->Get( CSV_TRADER );
	m_szTradeFor	= pCsv->Get( CSV_TRADE_FOR );
	m_szTag			= pCsv->Get( CSV_TAG );			
	m_nSide			= pCsv->nGet( CSV_SIDE );
	m_szSymbol		= pCsv->Get( CSV_SYMBOL );		
	m_nShares		= pCsv->nGet( CSV_SHARES );
	m_fPrice		= pCsv->dGet( CSV_PRICE );
	m_nType			= pCsv->nGet( CSV_TYPE );
	m_nTIF			= pCsv->nGet( CSV_TIF );

	m_szLocalAcct	= pCsv->Get( CSV_LOCALACCOUNT );
	m_szFirm		= pCsv->Get( CSV_FIRM );
	m_szClgAcct		= pCsv->Get( CSV_CLEARINGACCOUNT );		
	m_nStatus		= pCsv->nGet( CSV_STATUS );
	m_szStaffordTag	= pCsv->Get( CSV_STAFFORD_TAG );
	m_nExchange		= pCsv->nGet( CSV_EXCHANGE );
	m_szExDest		= pCsv->Get( CSV_EXDEST );
	m_fStopPrice	= pCsv->dGet( CSV_STOPPRICE );
	m_nCumQty		= pCsv->nGet( CSV_CUM_SHARES );
	m_nLeaveQty		= pCsv->nGet( CSV_LEAVE_SHARES );
}

bool CStatusMsg::Process()
{
	return true;
}

long CStatusMsg::GetRequestID()
{
	int nPos = m_szStaffordTag.ReverseFind('-');
	if( nPos < 0 )
		return 0;

	return atol( (LPCSTR)m_szStaffordTag.Mid(nPos+1) );
}