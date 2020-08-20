// DBConnection.cpp: implementation of the CDBConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "oats.h"
#include "DBConnection.h"
#include "OrderMsg.h"
#include "StatusMsg.h"
#include "Logger.h"
#include "Locker.h"
#include "dbo.h"

#include <string>
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
long CDBConnection::m_nCount = 0;

CDBConnection::CDBConnection() : m_bIsOpen( false ), m_nLevel(1) //, m_pConn(NULL)
{
	m_nThreads = GetPrivateProfileInt("Database Information", "ThreadNum", 1, "\\OrderManager\\Oats.dat");
	m_Conn.reserve(m_nThreads);
	m_bUsingOmInstance = GetPrivateProfileInt("Settings", "Using OM Instance", 0, "\\OrderManager\\Oats.dat");
}

CDBConnection::~CDBConnection()
{
	Close();
}

bool CDBConnection::Open( TCHAR *tszDSNName, TCHAR *tszUserName, TCHAR *tszPassword, TCHAR * tszDatabase )
{
	if (m_bIsOpen)
	{
		return false;
	}
	m_szUser = tszUserName;
	m_szPass = tszPassword;
	m_szDSN	 = tszDSNName;
	m_bIsOpen = true;
	for( int i = 0; i < m_nThreads; i++ )
		m_Conn[i] = NULL;
	return true;
}

bool CDBConnection::Close()
{
	if (m_bIsOpen)
	{
		// Release the SQL handles
		m_bIsOpen = false;
		//delete m_pConn;
		for( int i = 0; i < m_nThreads; i++ )
		{
			delete m_Conn[i];
			m_Conn[i] = NULL;
		}

		//m_pConn = NULL;
	}
	else
		return false;

	return true;
}

bool CDBConnection::InsertOrder( COrderMsg * pOrder, int nConnection )
{
	InterlockedIncrement( &m_nCount );
//	if( pOrder->m_szCommand == "E" )

	if (m_bUsingOmInstance)
	{
		GenerateOrderNum1( pOrder );
	}
	else
	{
		GenerateOrderNum( pOrder );
	}
//	Locker locker(&m_Lock);
	if( nConnection >= m_nThreads )
	{
		Logger::getInstance().LogError( "Wrong Thread" );
		return false;
	}
	if( m_nLevel == 2 )
	{
		std::stringstream stm;
		stm << "Debug: Thread ID = " << GetCurrentThreadId() << "; " << "Connection ID = " << nConnection << "; " << BuildInsertString( pOrder );
		Logger::getInstance().LogMessage( stm.str() );
	}
	if( !m_Conn[nConnection] )
		m_Conn[nConnection] = new DboConnection( m_szDSN, m_szUser, m_szPass, "OM" );
	DboCommand cmd( m_Conn[nConnection], "rom.dbo.OATS_insMsgNew" );

	cmd.paramIn( "@command", (char*)(LPCTSTR)pOrder->m_szCommand );
	cmd.paramIn( "@status", pOrder->m_nStatus );
	if( !pOrder->m_szOrderNum.IsEmpty() )
		cmd.paramIn( "@_OrdNum_str", (char*)(LPCTSTR)pOrder->m_szOrderNum );
	cmd.paramIn( "@avgPrice", pOrder->m_dAvgPrice );
	cmd.paramIn( "@callPut", (char*)(LPCTSTR)pOrder->m_szCallPut );
	cmd.paramIn( "@capacity", (char*)(LPCTSTR)pOrder->m_szCacacity );
	if( !pOrder->m_szCancelTag.IsEmpty() )
		cmd.paramIn( "@cancelTag", (char*)(LPCTSTR)pOrder->m_szCancelTag );
	cmd.paramIn( "@clearingAccount", (char*)(LPCTSTR)pOrder->m_szClgAcct );
	cmd.paramIn( "@clearingID", (char*)(LPCTSTR)pOrder->m_szClgID );
	if( !pOrder->m_szClientTime.IsEmpty() )
		cmd.paramIn( "@clientTime", (char*)(LPCTSTR)pOrder->m_szClientTime );
	cmd.paramIn( "@CMTAID", (char*)(LPCTSTR)pOrder->m_szCMTA );
	cmd.paramIn( "@contra", (char*)(LPCTSTR)pOrder->m_szContra );
	cmd.paramIn( "@cumQty", pOrder->m_nCumQty );

	cmd.paramIn( "@currency", (char*)(LPCTSTR)pOrder->m_szCurrency );
	cmd.paramIn( "@dbSymbol", (char*)(LPCTSTR)pOrder->m_szDBSymbol );
	cmd.paramIn( "@DestID", pOrder->m_nExchange );
	cmd.paramIn( "@DestRouteID", pOrder->m_nOrderDest );
	cmd.paramIn( "@discresionary", (char*)(LPCTSTR)pOrder->m_szDiscretion );
	cmd.paramIn( "@exchangeTag", (char*)(LPCTSTR)pOrder->m_szExchgTag );
	cmd.paramIn( "@exConfirm", (char*)(LPCTSTR)pOrder->m_szExecConfirm );
	cmd.paramIn( "@exDest", (char*)(LPCTSTR)pOrder->m_szExDest );
	cmd.paramIn( "@expDate", (char*)(LPCTSTR)pOrder->m_szExpDate );
	cmd.paramIn( "@exQty", pOrder->m_nExecShares );
	if( !pOrder->m_szExecPrice.IsEmpty() )
		cmd.paramIn( "@exPrice", atof((LPCSTR)pOrder->m_szExecPrice) );

	cmd.paramIn( "@exTag", (char*)(LPCTSTR)pOrder->m_szExecTag );
	if( !pOrder->m_szExecTime.IsEmpty() )
		cmd.paramIn( "@exTime", (char*)(LPCTSTR)pOrder->m_szExecTime );
	cmd.paramIn( "@firm", (char*)(LPCTSTR)pOrder->m_szFirm );
	cmd.paramIn( "@GiveUp", (char*)(LPCTSTR)pOrder->m_szGiveUp );
	cmd.paramIn( "@instructions", (char*)(LPCTSTR)pOrder->m_szInstruction );
	cmd.paramIn( "@leavesQty", pOrder->m_nLeaveQty );
	cmd.paramIn( "@localAcct", (char*)(LPCTSTR)pOrder->m_szLocalAcct );
	cmd.paramIn( "@maxFloor", pOrder->m_nMaxFloor );
	if( !pOrder->m_szMinQty.IsEmpty() )
		cmd.paramIn( "@minQty", atoi( (LPCSTR)pOrder->m_szMinQty ) );
	cmd.paramIn( "@mmAcct", (char*)(LPCTSTR)pOrder->m_szMMAccount );
	if( !pOrder->m_szMMExchange.IsEmpty() )
		cmd.paramIn( "@mmExchange", atoi((LPCSTR)pOrder->m_szMMExchange) );
	cmd.paramIn( "@multiplier", pOrder->m_fMultipler );
	cmd.paramIn( "@omExTag", (char*)(LPCTSTR)pOrder->m_szOMExecTag );
	cmd.paramIn( "@omTag", (char*)(LPCTSTR)pOrder->m_szStaffordTag );
	if( !pOrder->m_szOMTime.IsEmpty() )
		cmd.paramIn( "@omTime", (char*)(LPCTSTR)pOrder->m_szOMTime );
	cmd.paramIn( "@openClose", pOrder->m_nOpenClose == 1 ? "O" : "C" ); // 1 open : 0 close
	if( !pOrder->m_szOrderExpDate.IsEmpty() )
		cmd.paramIn( "@orderExpiresDate", (char*)(LPCTSTR)pOrder->m_szOrderExpDate );
	cmd.paramIn( "@orderTag", (char*)(LPCTSTR)pOrder->m_szOrderTag );
	cmd.paramIn( "@orderType", pOrder->m_nType );
	cmd.paramIn( "@originalShares", pOrder->m_nOrgShares );
	
	cmd.paramIn( "@owner", (char*)(LPCTSTR)pOrder->m_szOwner );
	cmd.paramIn( "@price", pOrder->m_fPrice );
	cmd.paramIn( "@pricecheck", (char*)(LPCTSTR)pOrder->m_szPriceCheck );
	cmd.paramIn( "@qty", pOrder->m_nShares );
	cmd.paramIn( "@report", (char*)(LPCTSTR)pOrder->m_szReport );
	cmd.paramIn( "@secType", (char*)(LPCTSTR)pOrder->m_szSecType );
	cmd.paramIn( "@side", pOrder->m_nSide );
	cmd.paramIn( "@solicited", (char*)(LPCTSTR)pOrder->m_szSolicited );
	cmd.paramIn( "@stopprice", pOrder->m_fStopPrice );

	cmd.paramIn( "@strikePrice", pOrder->m_fStrikePrice );
	cmd.paramIn( "@symbol", (char*)(LPCTSTR)pOrder->m_szSymbol );
	cmd.paramIn( "@tag", (char*)(LPCTSTR)pOrder->m_szTag );
	cmd.paramIn( "@text", (char*)(LPCTSTR)pOrder->m_szText );
	cmd.paramIn( "@tif", pOrder->m_nTIF );
	cmd.paramIn( "@tradeFor", (char*)(LPCTSTR)pOrder->m_szTradeFor );
	cmd.paramIn( "@trader", (char*)(LPCTSTR)pOrder->m_szTrader );
	cmd.paramIn( "@underlying", (char*)(LPCTSTR)pOrder->m_szUnderlying );

	if( !pOrder->m_szShortLender.IsEmpty() )
		cmd.paramIn( "@shortLender", (char*)(LPCTSTR)pOrder->m_szShortLender );
	if( pOrder->m_nExecInst != -1 )
	{
		char c = pOrder->m_nExecInst;
		cmd.paramIn( "@execInst", (char *)&c );
	}

	if ((pOrder->m_szEffectiveTime).IsEmpty() == 0)
	{
		cmd.paramIn( "@effectiveTime", (char*)(LPCTSTR)pOrder->m_szEffectiveTime );
	}

	if ((pOrder->m_szBillingCode).IsEmpty() == 0)
	{
		cmd.paramIn( "@billStr", (char*)(LPCTSTR)pOrder->m_szBillingCode );
	}

	if ((pOrder->m_szParentOmTag).IsEmpty() == 0)
	{
		cmd.paramIn("@parentOmTag", (char*)(LPCTSTR)pOrder->m_szParentOmTag );
	}

	if ((pOrder->m_szSecID).IsEmpty() == 0)
	{
		cmd.paramIn("@secid", (char*)(LPCTSTR)pOrder->m_szSecID );
	}

	cmd.paramIn("@cplxtype", pOrder->m_nCplxType);

	cmd.exec();
 	if( cmd.status() != DBO_SUCCESS )
	{
		switch(cmd.statusCode())
		{
		case DBO_CONNECT_LOST:
		case DBO_CTLIB_ERR:
			{
				delete m_Conn[nConnection];
				m_Conn[nConnection] = new DboConnection( m_szDSN, m_szUser, m_szPass, "OM" );
			}
			break;
		case DBO_DEADLOCK:
			{
				Logger::getInstance().LogError( "DBO_DEADLOCK, retry in 10 ms" );
				Sleep( 10 );
				for( int i = 0; i < 3; i++ )
				{
					cmd.exec();
					if( cmd.status() == DBO_SUCCESS )
						return true;
					Logger::getInstance().LogError( "DBO_DEADLOCK, retry again in 10 ms" );
					Sleep( 10 );
				}
				
			}
			break;
		default:
			break;
		}
		Logger::getInstance().LogError( cmd.statusText() );
		Logger::getInstance().LogMessage( "Error: " + BuildInsertString( pOrder ) );
		static int nError = 0;
		if( nError < 5 )
		{
			nError++;
			COatsApp * pApp = (COatsApp *)AfxGetApp();
			pApp->SendMail( CString("DB Error: ") + cmd.statusText() );
		}
		return false;
	}
	
	return true;
}

std::string CDBConnection::BuildInsertString( COrderMsg * pOrder )
{
	std::stringstream stm;
	stm << "OATS_insMsg "
		<< "'" << (LPCSTR)pOrder->m_szCommand			<< "', "
		<< pOrder->m_nStatus							<< ", ";
	if( pOrder->m_szOrderNum.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szOrderNum				<< "', ";
	stm << pOrder->m_dAvgPrice							<< ", ";

	if( pOrder->m_szCallPut.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szCallPut 		<< "', ";
	
	if( pOrder->m_szCacacity.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szCacacity		<< "', ";
	
	if( pOrder->m_szCancelTag.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szCancelTag		<< "', ";
	
	if( pOrder->m_szClgAcct.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szClgAcct		<< "', ";

	if( pOrder->m_szClgID.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szClgID		<< "', ";

	if( pOrder->m_szClientTime.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szClientTime	<< "', ";
	
	if( pOrder->m_szCMTA.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << pOrder->m_szCMTA					<< "', ";
	
	if( pOrder->m_szContra.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szContra 		<< "', ";
	
	stm	<< pOrder->m_nCumQty							<< ", ";
	
	if( pOrder->m_szCurrency.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szCurrency 		<< "', ";

	if( pOrder->m_szDBSymbol.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szDBSymbol 		<< "', ";
	
	stm	<< pOrder->m_nExchange							<< ", ";
	stm	<< pOrder->m_nOrderDest							<< ", ";
	
	if( pOrder->m_szDiscretion.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szDiscretion	<< "', ";
	
	if( pOrder->m_szExchgTag.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szExchgTag 		<< "', ";
	
	if( pOrder->m_szExecConfirm.IsEmpty() )
		stm << "null, ";
	else
		stm	<< "'" << (LPCSTR)pOrder->m_szExecConfirm	<< "', ";
	
	if( pOrder->m_szExDest.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szExDest 		<< "', " ;
	
	if( pOrder->m_szExpDate.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szExpDate 		<< "', ";
	
	stm	<< pOrder->m_nExecShares						<< ", ";
	
	if( pOrder->m_szExecPrice.IsEmpty() )
		stm << "null, ";
	else
		stm << atof((LPCSTR)pOrder->m_szExecPrice) 		<< ", ";
	
	if( pOrder->m_szExecTag.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szExecTag 		<< "', ";
	
	if( pOrder->m_szExecTime.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szExecTime 		<< "', ";
	
	if( pOrder->m_szFirm.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szFirm 			<< "', ";
	
	if( pOrder->m_szGiveUp.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szGiveUp 		<< "', ";
	
	if( pOrder->m_szInstruction.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szInstruction	<< "', ";
	
	stm << pOrder->m_nLeaveQty							<< ", ";
	
	if( pOrder->m_szLocalAcct.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szLocalAcct 	<< "', ";
	
	stm	<< pOrder->m_nMaxFloor							<< ", ";
	
	if( pOrder->m_szMinQty.IsEmpty() )
		stm << "null, ";
	else
		stm << atoi((LPCSTR)pOrder->m_szMinQty ) 		<< ", ";
	
	if( pOrder->m_szMMAccount.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szMMAccount 	<< "', ";
	
	if( pOrder->m_szMMExchange.IsEmpty() )
		stm << "null, ";
	else		
		stm << atoi((LPCSTR)pOrder->m_szMMExchange) 	<< ", ";
	
	stm	<< pOrder->m_fMultipler							<< ", ";
	
	if( pOrder->m_szOMExecTag.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szOMExecTag 	<< "', ";
	
	if( pOrder->m_szStaffordTag.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szStaffordTag	<< "', ";
	
	if( pOrder->m_szOMTime.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szOMTime		<< "', ";
	
	stm	<< "'" << (pOrder->m_nOpenClose==1 ? "O":"C" )	<< "', ";
	
	if( pOrder->m_szOrderExpDate.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szOrderExpDate	<< "', ";
	
	if( pOrder->m_szOrderTag.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szOrderTag 		<< "', ";
	
	stm	<< pOrder->m_nType								<< ", ";
	stm << pOrder->m_nOrgShares							<< ", ";
	
	if( pOrder->m_szOwner.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szOwner 		<< "', ";
	
	stm	<< pOrder->m_fPrice								<< ", ";
	
	if( pOrder->m_szPriceCheck.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szPriceCheck 	<< "', ";
	
	stm	<< pOrder->m_nShares							<< ", ";
	
	if( pOrder->m_szReport.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szReport		<< "', ";
	
	if( pOrder->m_szSecType.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szSecType 		<< "', ";
	
	stm	<< pOrder->m_nSide								<< ", ";
	
	if( pOrder->m_szSolicited.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szSolicited 	<< "', ";
	
	stm	<< pOrder->m_fStopPrice							<< ", ";

	stm	<< pOrder->m_fStrikePrice						<< ", ";
	
	if( pOrder->m_szSymbol.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szSymbol 		<< "', ";
	
	if( pOrder->m_szTag.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szTag 			<< "', ";
	
	if( pOrder->m_szText.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szText 			<< "', ";
	
	stm	<< pOrder->m_nTIF								<< ", ";
	
	if( pOrder->m_szTradeFor.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szTradeFor 		<< "', ";
	
	if( pOrder->m_szTrader.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szTrader 		<< "', ";
	
	if( pOrder->m_szUnderlying.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szUnderlying 	<< "', ";

	if( pOrder->m_szShortLender.IsEmpty() )
		stm << "null, ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szShortLender << "', ";
	
	if( pOrder->m_nExecInst == -1 )
		stm << "null, ";
	else
		stm << "'" << (char)pOrder->m_nExecInst << "', ";

	if( pOrder->m_szEffectiveTime.IsEmpty() )
		stm << "null ";
	else
		stm << "'" << (LPCSTR)pOrder->m_szEffectiveTime << "' ";

	return stm.str();
}

void CDBConnection::GenerateOrderNum( COrderMsg * pOrder )
{
	if( pOrder->m_szStaffordTag.IsEmpty() )
		return;
	std::string tag((LPCSTR)pOrder->m_szStaffordTag);
	int nPos = tag.rfind( '-' );
	std::stringstream stm;
	std::string szNum = tag.substr( nPos+1, -1 );
	tag = tag.substr( 0, nPos );
	std::string szDate = tag.substr( tag.length()-6, -1 );
	stm << szDate << std::setw(8) << std::setfill('0') << szNum;
//	LONG64 num = _atoi64( stm.str().c_str() );
	pOrder->m_szOrderNum = stm.str().c_str();
}


void CDBConnection::GenerateOrderNum1(COrderMsg* pOrder)
{
	if( pOrder->m_szStaffordTag.IsEmpty() )
		return;
	std::string tag((LPCSTR)pOrder->m_szStaffordTag);
	int nPos = tag.find('-');
	char omInstance = tag[nPos + 1];
	nPos = tag.rfind( '-' );
	std::stringstream stm;
	std::string szNum = tag.substr( nPos+1, -1 );
	tag = tag.substr( 0, nPos );
	std::string szDate = tag.substr( tag.length()-6, -1 );

	stm << szDate << omInstance << std::setw(7) << std::setfill('0') << szNum;;

	pOrder->m_szOrderNum = stm.str().c_str();
}
