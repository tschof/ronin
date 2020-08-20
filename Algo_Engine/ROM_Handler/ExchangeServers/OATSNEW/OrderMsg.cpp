// OrderMsg.cpp: implementation of the COrderMsg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "oats.h"
#include "OrderMsg.h"
#include "Csv.h"
#include "DBConnection.h"
#include "Locker.h"
#include "Logger.h"
#include "OatsCallback.h"
#include "time.h"
#include <sstream>
#include <iomanip>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COrderMsg::COrderMsg()
{

}

COrderMsg::COrderMsg( CCsv * pCsv ) : CStatusMsg( pCsv )
{
	m_szOrderNum	= "";
	m_szCacacity	= pCsv->Get( CSV_CAPACITY );
    m_szClgID		= pCsv->Get( CSV_CLEARINGID );
	m_szInstruction	= pCsv->Get( CSV_INSTRUCTIONS );
    m_szPriceCheck	= pCsv->Get( CSV_PRICECHECK );
    m_szMinQty		= pCsv->Get( CSV_MINQTY );
    m_szDiscretion	= pCsv->Get( CSV_DISCRETIONARY );
    m_szSolicited	= pCsv->Get( CSV_SOLICITED );
    m_fMultipler	= pCsv->dGet( CSV_MULTIPLIER );
	m_szExpDate		= pCsv->Get( CSV_EXPDATE );
	//m_szCallPut		= pCsv->nGet( CSV_CALLPUT );
	m_szCallPut		= pCsv->Get( CSV_CALLPUT );
    m_fStrikePrice	= pCsv->dGet( CSV_STRIKEPRICE );
    m_szUnderlying	= pCsv->Get( CSV_UNDERLYING );
    m_szOrderTag	= pCsv->Get( CSV_ORDER_TAG );
    m_szExchgTag	= pCsv->Get( CSV_EXCHANGE_TAG );
    m_szCancelTag	= pCsv->Get( CSV_CANCEL_TAG );
    m_szExecTag		= pCsv->Get( CSV_EXEC_TAG );
    m_szExecConfirm = pCsv->Get( CSV_EXEC_CONFIRM );
    m_szReference	= pCsv->Get( CSV_REFERENCE );
    m_szReport		= pCsv->Get( CSV_REPORT );

    m_szContra		= pCsv->Get( CSV_CONTRA );
	if( m_szCommand != "E" )
	{
		m_szText		= pCsv->Get( CSV_TEXT );
		m_szDBSymbol	= "";
	}
	else
	{
	    m_szDBSymbol	= pCsv->Get( CSV_DBSYMBOL );
		m_szText		= "";
	}
    m_szOwner		= pCsv->Get( CSV_OWNER );
    m_szOMExecTag	= pCsv->Get( CSV_OMEXECUTION_TAG );
	TRACE( "OmExecTag: %s\n", m_szOMExecTag );
    m_nExecShares	= pCsv->nGet( CSV_EXEC_SHARES );
    m_nOrgShares	= pCsv->nGet( CSV_ORIGINAL_SHARES );
    m_szExecPrice	= pCsv->Get( CSV_EXEC_PRICE );
    m_szOwnerCancel	= pCsv->Get( CSV_OWNERCANCEL );
	m_szCMTA		= pCsv->Get( CSV_CMTAACCOUNT );

	m_szGiveUp		= pCsv->Get( CSV_GIVEUPACCOUNT );
	m_szMMAccount	= pCsv->Get( CSV_MM_ACCOUNT );
	m_szMMExchange	= pCsv->Get( CSV_MM_EXCHANGE );
	m_nOpenClose		= pCsv->nGet( CSV_OPENCLOSE );
	m_szSecType			= pCsv->Get( CSV_SECURITY_TYPE );
	if( m_szSecType.IsEmpty() )
	{
		m_szSecType = "E";
		Logger::getInstance().LogError( "No SecType specified, default to equity." );
	}
	m_nMaxFloor			= pCsv->nGet( CSV_FLOOR );
	m_dAvgPrice			= pCsv->dGet( CSV_AVG_PRICE );

	m_nOrderDest		= pCsv->dGet( CSV_ORDERDEST );

	m_szClientTime	= pCsv->Get( CSV_CLIENT_TIME );
	if( m_szClientTime.Find("-") != -1 )
		m_szClientTime.Replace("-"," ");
	else
		m_szClientTime	= "";
	
	m_szExecTime	= pCsv->Get( CSV_EXEC_TIME );
	if( m_szExecTime.Find("-") != -1 )
		m_szExecTime.Replace("-"," ");
	else
		m_szExecTime	= "";
	
	m_szOMTime		= pCsv->Get( CSV_OM_TIME );
	if( m_szOMTime.Find("-") != -1 )
		m_szOMTime.Replace("-"," ");
	else
		m_szOMTime	= "";

	m_szOrderExpDate	= pCsv->Get( CSV_GTD_DATE );
	if( m_szOrderExpDate.Find("-") != -1 )
		m_szOrderExpDate.Replace("-"," ");
	else
		m_szOrderExpDate	= "";

	m_szCurrency	= pCsv->Get(CSV_CURRENCY);
	m_szShortLender = pCsv->Get(CSV_SHORT_LENDER);
	if( !pCsv->Get(CSV_EXEC_INSTRUCTION).IsEmpty() )
		m_nExecInst	= pCsv->cGet(CSV_EXEC_INSTRUCTION);
	else
		m_nExecInst	= -1;

	m_szEffectiveTime = pCsv->Get(CSV_EFFECTIVE_TIME);
	if( m_szEffectiveTime.Find("-") != -1 )
		m_szEffectiveTime.Replace("-"," ");
	else
		m_szEffectiveTime	= "";

	m_szBillingCode = pCsv->Get(CSV_BILLING_CODE);
	m_szParentOmTag = pCsv->Get(CSV_PARENT_STAFFORD_TAG);
	m_szSecID		= pCsv->Get(CSV_SECID);
	m_nCplxType		= pCsv->nGet(CSV_CPLXORDER_TYPE);
}

COrderMsg::~COrderMsg()
{

}

std::string COrderMsg::TimeStamp()
{
	std::stringstream stm;
	SYSTEMTIME stime;
	GetSystemTime( &stime );
	stm << std::setw(2) << std::setfill('0') << stime.wHour << ":"
		<< std::setw(2) << std::setfill('0') << stime.wMinute << ":"
		<< std::setw(2) << std::setfill('0') << stime.wSecond << "."
		<< std::setw(3) << std::setfill('0') << stime.wMilliseconds;
	return stm.str();
}

bool COrderMsg::Process()
{
#ifdef _DEBUG
	Locker locker( &m_Lock );
#endif
	bool bResult = false;
	std::string tBegin = TimeStamp();
	try
	{
		bResult = CDBConnection::getInstance().InsertOrder( this, GetRequestID()%COatsCallback::m_nThreads );
	}
	catch(...)
	{
		Logger::getInstance().LogError( "InsertOrder unknown exception" );
		Logger::getInstance().LogError( CDBConnection::getInstance().BuildInsertString( this ) );
	}
	std::string tEnd = TimeStamp();
	if( CDBConnection::getInstance().GetLogLevel() == 2 )
	{
		char buff[1024];
		sprintf( buff, "Debug: Thread %d: Insert Record %s; Order Time: %s DB Time %s --- %s", GetCurrentThreadId(), bResult ? "Suceess" : "Failure", (LPCSTR)m_szOMTime, tBegin.c_str(), tEnd.c_str() );
		Logger::getInstance().LogError( buff );
	}
	return bResult;
}

