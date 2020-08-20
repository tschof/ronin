// OrderMsg.cpp: implementation of the COrderMsg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OrderMsg.h"
#include "RomCsv.h"
#include "DBConnection.h"
#include "Locker.h"
#include "Logger.h"
#include "time.h"
#include "Config.h"
#include <sstream>
#include <iomanip>
#include "RomValueDefs.h"

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

COrderMsg::COrderMsg( CRomCsv * pCsv ) : CStatusMsg( pCsv )
{
	m_szOrderNum	= "";
	m_szCacacity	= pCsv->Get( RFIX::CSV_CAPACITY );
    m_szClgID		= pCsv->Get( RFIX::CSV_CLEARINGID );
	m_szInstruction	= pCsv->Get( RFIX::CSV_INSTRUCTIONS );
    m_szPriceCheck	= pCsv->Get( RFIX::CSV_PRICECHECK );
    m_szMinQty		= pCsv->Get( RFIX::CSV_MINQTY );
    m_szDiscretion	= pCsv->Get( RFIX::CSV_DISCRETIONARY );
    m_szSolicited	= pCsv->Get( RFIX::CSV_SOLICITED );
    m_fMultipler	= pCsv->dGet( RFIX::CSV_OATSMULTIPLIER );
	m_szExpDate		= pCsv->Get( RFIX::CSV_EXPDATE );
	//m_szCallPut		= pCsv->nGet( CSV_CALLPUT );
	m_szCallPut		= pCsv->Get( RFIX::CSV_CALLPUT );
    m_fStrikePrice	= pCsv->dGet( RFIX::CSV_STRIKEPRICE );
    m_szUnderlying	= pCsv->Get( RFIX::CSV_UNDERLYING );
    m_szOrderTag	= pCsv->Get( RFIX::CSV_ORDER_TAG );
    m_szExchgTag	= pCsv->Get( RFIX::CSV_EXCHANGE_TAG );
    m_szCancelTag	= pCsv->Get( RFIX::CSV_CANCEL_TAG );
    m_szExecTag		= pCsv->Get( RFIX::CSV_EXEC_TAG );
    m_szExecConfirm = pCsv->Get( RFIX::CSV_EXEC_CONFIRM );
    m_szReference	= pCsv->Get( RFIX::CSV_REFERENCE );
    m_szReport		= pCsv->Get( RFIX::CSV_REPORT );

    m_szContra		= pCsv->Get( RFIX::CSV_CONTRA );
	if( m_szCommand != "E" )
	{
		m_szText		= pCsv->Get( RFIX::CSV_TEXT );
		m_szDBSymbol	= "";
	}
	else
	{
	    m_szDBSymbol	= pCsv->Get( RFIX::CSV_DBSYMBOL );
		m_szText		= "New Order";
	}

	m_szOMExecTag	= pCsv->Get( RFIX::CSV_OMEXECUTION_TAG );
    m_szOwner		= pCsv->Get( RFIX::CSV_OWNER );
    m_nExecShares	= pCsv->nGet( RFIX::CSV_EXEC_SHARES );
    m_nOrgShares	= pCsv->nGet(RFIX::CSV_ORIGINAL_SHARES );
    m_szExecPrice	= pCsv->Get( RFIX::CSV_EXEC_PRICE );
    m_szOwnerCancel	= pCsv->Get( RFIX::CSV_OWNERCANCEL );
	m_szCMTA		= pCsv->Get( RFIX::CSV_CMTAACCOUNT );

	m_szGiveUp		= pCsv->Get( RFIX::CSV_GIVEUPACCOUNT );
	m_szMMAccount	= pCsv->Get( RFIX::CSV_MM_ACCOUNT );
	m_szMMExchange	= pCsv->Get( RFIX::CSV_MM_EXCHANGE );
	m_nOpenClose	= pCsv->nGet( RFIX::CSV_OPENCLOSE );
	m_szSecType		= pCsv->Get( RFIX::CSV_SECURITY_TYPE );
	if( m_szSecType.empty() )
	{
		m_szSecType = "E";
		Logger::getInstance().LogError( "No SecType specified, default to equity." );
	}
	m_nMaxFloor			= pCsv->nGet( RFIX::CSV_FLOOR );
	m_dAvgPrice			= pCsv->dGet( RFIX::CSV_AVG_PRICE );

	m_nOrderDest		= pCsv->dGet( RFIX::CSV_ORDERDEST );

	m_szClientTime	= pCsv->Get( RFIX::CSV_CLIENT_TIME );
	int index = m_szClientTime.find('-');
	if (index != std::string::npos)
	{
		m_szClientTime[index] = ' ';
	}
	else
	{
		m_szClientTime = "";
	}
	//Probably need to create a ReplaceDashWithSpace()

	//if( m_szClientTime.find("-") != -1 )
	//	m_szClientTime.Replace("-"," ");
	//else
	//	m_szClientTime	= "";
	
	m_szExecTime	= pCsv->Get( RFIX::CSV_EXEC_TIME );
	index = m_szExecTime.find('-');
	if (index != std::string::npos)
	{
		m_szExecTime[index] = ' ';
	}
	else
	{
		m_szExecTime = "";
	}
	//if( m_szExecTime.Find("-") != -1 )
	//	m_szExecTime.Replace("-"," ");
	//else
	//	m_szExecTime	= "";
	
	m_szOMTime		= pCsv->Get( RFIX::CSV_OM_TIME );
	index = m_szOMTime.find('-');
	if (index != std::string::npos)
	{
		m_szOMTime[index] = ' ';
	}
	else
	{
		m_szOMTime = "";
	}
	//if( m_szOMTime.Find("-") != -1 )
	//	m_szOMTime.Replace("-"," ");
	//else
	//	m_szOMTime	= "";

	m_szOrderExpDate	= pCsv->Get( RFIX::CSV_GTD_DATE );
	index = m_szOrderExpDate.find('-');
	if (index != std::string::npos)
	{
		m_szOrderExpDate[index] = ' ';
	}
	else
	{
		m_szOrderExpDate = "";
	}
	//if( m_szOrderExpDate.Find("-") != -1 )
	//	m_szOrderExpDate.Replace("-"," ");
	//else
	//	m_szOrderExpDate	= "";

	m_szCurrency	= pCsv->Get(RFIX::CSV_CURRENCY);
	m_szShortLender = pCsv->Get(RFIX::CSV_SHORT_LENDER);
	if( pCsv->IsFieldEmpty(RFIX::CSV_EXEC_INSTRUCTION) == false )
		m_nExecInst	= pCsv->cGet(RFIX::CSV_EXEC_INSTRUCTION);
	else
		m_nExecInst	= -1;

	m_szEffectiveTime = pCsv->Get(RFIX::CSV_EFFECTIVE_TIME);
	index = m_szEffectiveTime.find('-');
	if (index != std::string::npos)
	{
		m_szEffectiveTime[index] = ' ';
	}
	else
	{
		m_szEffectiveTime = "";
	}
	//if( m_szEffectiveTime.Find("-") != -1 )
	//	m_szEffectiveTime.Replace("-"," ");
	//else
	//	m_szEffectiveTime	= "";

	m_szBillingCode = pCsv->Get(RFIX::CSV_BILLING_CODE);
	m_szParentOmTag = pCsv->Get(RFIX::CSV_PARENT_STAFFORD_TAG);
	m_szSecID		= pCsv->Get(RFIX::CSV_SEC_ID);
	m_nCplxType		= pCsv->nGet(RFIX::CSV_CPLXORDER_TYPE);
	m_szSecDef		= pCsv->Get(RFIX::CSV_SEC_DEF);
	m_nAlgoType		= pCsv->nGet(RFIX::CSV_ALGOTYPE);
	m_szExpireDate  = "";

	if (COrderMsg::IsOptionType(m_szSecType[0]) &&
		pCsv->IsFieldEmpty(RFIX::CSV_MATURITYDAY) == false &&
		m_szExpDate.size() >= 6)
	{
		char expireDate[9];
		sprintf(expireDate, "%s%02u", m_szExpDate.substr(0, 6).c_str(), pCsv->nGet(RFIX::CSV_MATURITYDAY));
		m_szExpireDate = expireDate;
	}
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
/*
bool COrderMsg::Process()
{
#ifdef _DEBUG
	Locker locker( &m_Lock );
#endif
	bool bResult = false;
	std::string tBegin = TimeStamp();
	try
	{
		bResult = CDBConnection::getInstance().InsertOrder( this, GetRequestID()%(CConfig::getInstance()).numDBThreads() );
		//bResult = CDBConnection::getInstance().NewInsertOrder( this, GetRequestID()%(CConfig::getInstance()).numDBThreads() );
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
		sprintf( buff, "Debug: Thread %d: Insert Record %s; Order Time: %s DB Time %s --- %s", GetCurrentThreadId(), bResult ? "Suceess" : "Failure", m_szOMTime, tBegin.c_str(), tEnd.c_str() );
		Logger::getInstance().LogError( buff );
	}
	return bResult;
}*/

bool COrderMsg::IsOptionType(char secType)
{
	if (secType == ROMVALUE::SECTYPE_OPTION ||
		secType == ROMVALUE::SECTYPE_OPTIONINDEX ||
		secType == ROMVALUE::SECTYPE_OPTIONFUTURE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool COrderMsg::IsFutureType(char secType)
{
	if (secType == ROMVALUE::SECTYPE_FUTURE ||
		secType == ROMVALUE::SECTYPE_FUTUREEQUITY ||
		secType == ROMVALUE::SECTYPE_FUTURECURRENCY)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool COrderMsg::IsCurrencyType(char secType)
{
	if (secType == ROMVALUE::SECTYPE_CURRENCY)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool COrderMsg::IsEquityType(char secType)
{
	if (secType == ROMVALUE::SECTYPE_EQUITY)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool COrderMsg::IsComplexType(char secType)
{
	if (secType == ROMVALUE::SECTYPE_COMPLEX)
	{
		return true;
	}
	else
	{
		return false;
	}
}