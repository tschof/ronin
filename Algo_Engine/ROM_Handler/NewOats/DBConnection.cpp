// DBConnection.cpp: implementation of the CDBConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DBConnection.h"
#include "OrderMsg.h"
#include "StatusMsg.h"
#include "Logger.h"
#include "Locker.h"
#include "dbo.h"

#include <string>
#include <sstream>
#include <iomanip>
#include "Config.h"
#include "RomValueDefs.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//long CDBConnection::m_nCount = 0;

CDBConnection::CDBConnection() : m_bIsOpen( false ), m_nLevel(1) //, m_pConn(NULL)
{
	m_pConn = new DboConnection((CConfig::getInstance()).DBServer(), 
								(CConfig::getInstance()).DBLogin(), 
								(CConfig::getInstance()).DBPassword(), 
								"OM" );
	m_numEmail = 0;
}

CDBConnection::~CDBConnection()
{
	//Close();
	if (m_pConn)
	{
		delete m_pConn;
	}
}


//bool CDBConnection::InsertMessage(const CRomCsv& csv)
bool CDBConnection::InsertMessage(COrderMsg * pOrder)
{
	bool bResult = false;
	std::string tBegin = pOrder->TimeStamp();
	try
	{
		bResult = InsertOrder(pOrder);
	}
	catch(...)
	{
		Logger::getInstance().LogError( "InsertOrder unknown exception" );
		Logger::getInstance().LogError(BuildInsertString( pOrder ) );
	}
	std::string tEnd = pOrder->TimeStamp();
	if(GetLogLevel() == 2 )
	{
		char buff[1024];
		sprintf( buff, "Debug: Thread %d: Insert Record %s; DB Time %s --- %s", GetCurrentThreadId(), bResult ? "Suceess" : "Failure", tBegin.c_str(), tEnd.c_str() );
		Logger::getInstance().LogError( buff );
	}
	return bResult;
}


bool CDBConnection::InsertOrder( COrderMsg * pOrder)
{
	if ((CConfig::getInstance()).OrderNumWithRomInstance())
	{
		GenerateOrderNumNew( pOrder );
	}
	else
	{
		GenerateOrderNum( pOrder );
	}

	if( m_nLevel == 2 )
	{
		std::stringstream stm;
		stm << "Debug: Thread ID = " << GetCurrentThreadId() << "; " << BuildInsertString( pOrder );
		Logger::getInstance().LogMessage( stm.str() );
	}

	DboCommand cmd( m_pConn, "rom.dbo.OATS_insMsgNew" );

	cmd.paramIn( "@command", (char*)pOrder->Command() );
	cmd.paramIn( "@status", pOrder->Status() );
	if(strcmp(pOrder->OrderNum(), "") != 0)
	{
		cmd.paramIn( "@_OrdNum_str", (char*)pOrder->OrderNum() );
	}

	if ((pOrder->Command())[0] == ROMVALUE::COMMAND_ORDER)
	{
		cmd.paramIn( "@avgPrice", pOrder->AvgPrice() );
		cmd.paramIn( "@callPut", (char*)pOrder->CallPut() );
		cmd.paramIn( "@capacity", (char*)pOrder->Cacacity() );
		if( strcmp(pOrder->CancelTag(), "") != 0)
			cmd.paramIn( "@cancelTag", (char*)pOrder->CancelTag() );
		cmd.paramIn( "@clearingAccount", (char*)pOrder->ClgAcct() );
		cmd.paramIn( "@clearingID", (char*)pOrder->ClgID() );
		if( strcmp(pOrder->ClientTime(), "") != 0)
			cmd.paramIn( "@clientTime", (char*)pOrder->ClientTime() );
		cmd.paramIn( "@CMTAID", (char*)pOrder->CMTA() );
		cmd.paramIn( "@contra", (char*)pOrder->Contra() );
		cmd.paramIn( "@cumQty", pOrder->CumQty() );

		cmd.paramIn( "@currency", (char*)pOrder->Currency() );
		//cmd.paramIn( "@dbSymbol", (char*)pOrder->DBSymbol() );
		cmd.paramIn( "@DestID", pOrder->Exchange() );
		cmd.paramIn( "@DestRouteID", pOrder->OrderDest() );
		cmd.paramIn( "@discresionary", (char*)pOrder->Discretion() );
		cmd.paramIn( "@exchangeTag", (char*)pOrder->ExchgTag() );
		cmd.paramIn( "@exConfirm", (char*)pOrder->ExecConfirm() );
		cmd.paramIn( "@exDest", (char*)pOrder->ExDest() );
		cmd.paramIn( "@expDate", (char*)pOrder->ExpDate() );
		cmd.paramIn( "@exQty", pOrder->ExecShares() );
		if( strcmp(pOrder->ExecPrice(), "") != 0)
			cmd.paramIn( "@exPrice", atof(pOrder->ExecPrice()) );

		cmd.paramIn( "@exTag", (char*)pOrder->ExecTag() );
		if( strcmp(pOrder->ExecTime(), "") != 0)
			cmd.paramIn( "@exTime", (char*)pOrder->ExecTime() );
		cmd.paramIn( "@firm", (char*)pOrder->Firm() );
		cmd.paramIn( "@GiveUp", (char*)pOrder->GiveUp() );
		cmd.paramIn( "@instructions", (char*)pOrder->Instruction() );
		cmd.paramIn( "@leavesQty", pOrder->LeaveQty() );
		cmd.paramIn( "@localAcct", (char*)pOrder->LocalAcct() );
		cmd.paramIn( "@maxFloor", pOrder->MaxFloor() );
		if( strcmp(pOrder->MinQty(), "") != 0)
			cmd.paramIn( "@minQty", atoi( pOrder->MinQty() ) );
		cmd.paramIn( "@mmAcct", (char*)pOrder->MMAccount() );
		if( strcmp(pOrder->MMExchange(), "") != 0)
			cmd.paramIn( "@mmExchange", atoi(pOrder->MMExchange()) );
		cmd.paramIn( "@multiplier", pOrder->Multipler() );
		cmd.paramIn( "@omExTag", (char*)(pOrder->OMExecTag()) );
		cmd.paramIn( "@omTag", (char*)(pOrder->StaffordTag().c_str()) );
		if( strcmp(pOrder->OMTime(), "") != 0)
			cmd.paramIn( "@omTime", (char*)pOrder->OMTime() );
		cmd.paramIn( "@openClose", pOrder->OpenClose() == 1 ? "O" : "C" ); // 1 open : 0 close
		if( strcmp(pOrder->OrderExpDate(), "") != 0)
			cmd.paramIn( "@orderExpiresDate", (char*)pOrder->OrderExpDate() );
		cmd.paramIn( "@orderTag", (char*)pOrder->OrderTag() );
		cmd.paramIn( "@orderType", pOrder->Type() );
		cmd.paramIn( "@originalShares", pOrder->OrgShares() );
		
		cmd.paramIn( "@owner", (char*)pOrder->Owner() );
		cmd.paramIn( "@price", pOrder->Price() );
		cmd.paramIn( "@pricecheck", (char*)pOrder->PriceCheck() );
		cmd.paramIn( "@qty", pOrder->Shares() );
		cmd.paramIn( "@report", (char*)pOrder->Report() );
		cmd.paramIn( "@secType", (char*)pOrder->SecType() );
		cmd.paramIn( "@side", pOrder->Side() );
		cmd.paramIn( "@solicited", (char*)pOrder->Solicited() );
		cmd.paramIn( "@stopprice", pOrder->StopPrice() );

		cmd.paramIn( "@strikePrice", pOrder->StrikePrice() );
		cmd.paramIn( "@symbol", (char*)pOrder->Symbol() );
		cmd.paramIn( "@tag", (char*)pOrder->Tag() );
		cmd.paramIn( "@text", (char*)pOrder->Text() );
		cmd.paramIn( "@tif", pOrder->m_nTIF );
		cmd.paramIn( "@tradeFor", (char*)pOrder->TradeFor() );
		cmd.paramIn( "@trader", (char*)pOrder->Trader() );
		cmd.paramIn( "@underlying", (char*)pOrder->Underlying() );
		cmd.paramIn( "@algotype", pOrder->AlgoType() );
		cmd.paramIn( "@expireDate", (char*)pOrder->ExpireDate() );

		if( strcmp(pOrder->ShortLender(), "") != 0)
			cmd.paramIn( "@shortLender", (char*)pOrder->ShortLender() );
		if( pOrder->m_nExecInst != -1 )
		{
			char c = pOrder->ExecInst();
			cmd.paramIn( "@execInst", (char *)&c );
		}

		if (strlen(pOrder->EffectiveTime()) != 0)
		{
			cmd.paramIn( "@effectiveTime", (char*)pOrder->EffectiveTime() );
		}

		if (strlen(pOrder->BillingCode()) != 0)
		{
			cmd.paramIn( "@billStr", (char*)pOrder->BillingCode() );
		}

		if (strlen(pOrder->ParentOmTag()) != 0)
		{
			cmd.paramIn("@parentOmTag", (char*)pOrder->ParentOmTag() );
		}

		if (strlen(pOrder->SecID()) != 0)
		{
			cmd.paramIn("@secid", (char*)pOrder->SecID() );
		}

		if (strlen(pOrder->SecDef()) != 0)
		{
			cmd.paramIn("@SecDef", (char*)pOrder->SecDef() );
		}

		if (pOrder->PegOffset() != 0)
		{
			cmd.paramIn("@pegoffset", pOrder->PegOffset());
		}

		cmd.paramIn("@cplxtype", pOrder->CplxType());
	}
	else if((pOrder->Command())[0] == ROMVALUE::COMMAND_STATUS)
	{
		cmd.paramIn( "@omTag", (char*)(pOrder->StaffordTag().c_str()) );
		cmd.paramIn( "@orderType", pOrder->Type() );

		cmd.paramIn( "@DestRouteID", pOrder->OrderDest() );
		cmd.paramIn("@cplxtype", pOrder->CplxType());
		cmd.paramIn( "@secid",   (char*)pOrder->SecID() );
		cmd.paramIn( "@avgPrice", pOrder->AvgPrice() );
		cmd.paramIn( "@cumQty", pOrder->CumQty() );
		cmd.paramIn( "@exchangeTag", (char*)pOrder->ExchgTag() );
		cmd.paramIn( "@DestID", pOrder->Exchange() );
		cmd.paramIn( "@leavesQty", pOrder->LeaveQty() );
		cmd.paramIn( "@omExTag", (char*)(pOrder->OMExecTag()) );
		if( strlen(pOrder->OMTime()) != 0)
		{
			cmd.paramIn( "@omTime", (char*)pOrder->OMTime() );
		}
		cmd.paramIn( "@orderTag", (char*)pOrder->OrderTag() );
		cmd.paramIn( "@exPrice", atof(pOrder->ExecPrice()));
		cmd.paramIn( "@exQty", pOrder->ExecShares() );
		cmd.paramIn( "@price", pOrder->Price() );
		cmd.paramIn( "@qty", pOrder->Shares() ); 
		cmd.paramIn( "@text", (char*)pOrder->Text() );
		cmd.paramIn( "@tif", pOrder->m_nTIF );

		if (strlen(pOrder->ParentOmTag()) != 0)
		{
			cmd.paramIn("@parentOmTag", (char*)pOrder->ParentOmTag() );
		}

		if (pOrder->Status() == ROMVALUE::STATUS_REPLACED)
		{
			cmd.paramIn( "@stopprice", pOrder->StopPrice() );
			cmd.paramIn( "@maxFloor", pOrder->MaxFloor() );

			if( pOrder->m_nExecInst != -1 )
			{
				char c = pOrder->ExecInst();
				cmd.paramIn( "@execInst", (char *)&c );
			}

			if (pOrder->PegOffset() != 0)
			{
				cmd.paramIn("@pegoffset", pOrder->PegOffset());
			}
		}

		if( strcmp(pOrder->ClientTime(), "") != 0)
		{
			cmd.paramIn( "@clientTime", (char*)pOrder->ClientTime() );
		}

		if (strlen(pOrder->BillingCode()) != 0)
		{
			cmd.paramIn( "@billStr", (char*)pOrder->BillingCode() );
		}

		if (pOrder->Status() == ROMVALUE::STATUS_FILLED ||
			pOrder->Status() == ROMVALUE::STATUS_PARTIALLYFILLED)
		{
			cmd.paramIn( "@callPut", (char*)pOrder->CallPut() );
			cmd.paramIn( "@strikePrice", pOrder->StrikePrice() );
			cmd.paramIn( "@symbol", (char*)pOrder->Symbol() );
			cmd.paramIn( "@side", pOrder->Side() );
			cmd.paramIn( "@expireDate", (char*)pOrder->ExpireDate() );
			cmd.paramIn( "@secType", (char*)pOrder->SecType() );
			cmd.paramIn( "@secid",   (char*)pOrder->SecID() );
			cmd.paramIn( "@exTag", (char*)pOrder->ExecTag() );
			cmd.paramIn( "@exDest", (char*)pOrder->ExDest() );
			cmd.paramIn( "@exConfirm", (char*)pOrder->ExecConfirm() );
			cmd.paramIn( "@contra", (char*)pOrder->Contra() );

			if( strcmp(pOrder->ExecTime(), "") != 0)
			{
				cmd.paramIn( "@exTime", (char*)pOrder->ExecTime() );
			}
		}
	}

	if ((CConfig::getInstance()).DbPfmLogOn() == true)
	{	LARGE_INTEGER cntBefore, cntAfter;
		QueryPerformanceCounter(&cntBefore) ;
		cmd.exec();
		QueryPerformanceCounter(&cntAfter) ;
		double difCount = 1000 * ((double)(cntAfter.QuadPart - cntBefore.QuadPart)) / (double )((CConfig::getInstance()).PFFrequency().QuadPart);
		std::stringstream stm;
		stm << difCount;
		Logger::getInstance().LogPerformence(stm.str());
	}
	else
	{
		cmd.exec();
	}

	m_ProcessedCnt.Increment(1);

	if( cmd.status() != DBO_SUCCESS )
	{
		switch(cmd.statusCode())
		{
		case DBO_CONNECT_LOST:
		case DBO_CTLIB_ERR:
			{
				delete m_pConn;
				m_pConn = new DboConnection((CConfig::getInstance()).DBServer(), 
											(CConfig::getInstance()).DBLogin(), 
											(CConfig::getInstance()).DBPassword(), 
											"OM" );
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
		//static int nError = 0;
		if( m_numEmail < 5 )
		{
			m_numEmail++;
			std::string subject = "Oats DB Error";
			std::string message = cmd.statusText();
			(CConfig::getInstance()).SendEmail(subject, message);
		}
		return false;
	}
	else
	{
		m_InsertedCnt.Increment(1);
	}

	return true;
}

std::string CDBConnection::BuildInsertString( COrderMsg * pOrder )
{
	std::stringstream stm;
	stm << "rom.dbo.OATS_insMsgNew "
		<< "'" << pOrder->Command()			<< "', "
		<< pOrder->Status()							<< ", ";
	if( strcmp(pOrder->OrderNum(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->OrderNum()		<< "', ";
	stm << pOrder->AvgPrice()							<< ", ";

	if( strcmp(pOrder->CallPut(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->CallPut() 		<< "', ";
	
	if( strcmp(pOrder->Cacacity(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->Cacacity()		<< "', ";
	
	if( strcmp(pOrder->CancelTag(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->CancelTag()		<< "', ";
	
	if( strcmp(pOrder->ClgAcct(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->ClgAcct()		<< "', ";

	if( strcmp(pOrder->ClgID(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->ClgID()		<< "', ";

	if( strcmp(pOrder->ClientTime(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->ClientTime()	<< "', ";
	
	if( strcmp(pOrder->CMTA(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->CMTA()					<< "', ";
	
	if( strcmp(pOrder->Contra(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->Contra() 		<< "', ";
	
	stm	<< pOrder->CumQty()							<< ", ";
	
	if( strcmp(pOrder->Currency(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->Currency() 		<< "', ";

	if( strcmp(pOrder->DBSymbol(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->DBSymbol() 		<< "', ";
	
	stm	<< pOrder->Exchange()							<< ", ";
	stm	<< pOrder->OrderDest()							<< ", ";
	
	if( strcmp(pOrder->Discretion(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->Discretion()	<< "', ";
	
	if( strcmp(pOrder->ExchgTag(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->ExchgTag() 		<< "', ";
	
	if( strcmp(pOrder->ExecConfirm(), "") == 0)
		stm << "null, ";
	else
		stm	<< "'" << pOrder->ExecConfirm()	<< "', ";
	
	if( strcmp(pOrder->ExDest(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->ExDest() 		<< "', " ;
	
	if( strcmp(pOrder->ExpDate(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->ExpDate() 		<< "', ";
	
	stm	<< pOrder->ExecShares()						<< ", ";
	
	if( strcmp(pOrder->ExecPrice(), "") == 0)
		stm << "null, ";
	else
		stm << atof(pOrder->ExecPrice()) 		<< ", ";
	
	if( strcmp(pOrder->ExecTag(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->ExecTag() 		<< "', ";
	
	if( strcmp(pOrder->ExecTime(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->ExecTime() 		<< "', ";
	
	if( strcmp(pOrder->Firm(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->Firm() 			<< "', ";
	
	if( strcmp(pOrder->GiveUp(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->GiveUp() 		<< "', ";
	
	if( strcmp(pOrder->Instruction(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->Instruction()	<< "', ";
	
	stm << pOrder->LeaveQty()							<< ", ";
	
	if( strcmp(pOrder->LocalAcct(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->LocalAcct() 	<< "', ";
	
	stm	<< pOrder->MaxFloor()						<< ", ";
	
	if( strcmp(pOrder->MinQty(), "") == 0)
		stm << "null, ";
	else
		stm << atoi(pOrder->MinQty()) 		<< ", ";
	
	if( strcmp(pOrder->MMAccount(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->MMAccount() 	<< "', ";
	
	if( strcmp(pOrder->MMExchange(), "") == 0)
		stm << "null, ";
	else		
		stm << atoi(pOrder->MMExchange()) 	<< ", ";
	
	stm	<< pOrder->Multipler()							<< ", ";
	
	if( strcmp(pOrder->OMExecTag(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->OMExecTag() 	<< "', ";
	
	//if( strcmp(pOrder->pStaffordTag(), "") == 0)
	if( pOrder->StaffordTag().empty())
		stm << "null, ";
	else
		stm << "'" << pOrder->StaffordTag()	<< "', ";
	
	if( strcmp(pOrder->OMTime(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->OMTime()		<< "', ";
	
	stm	<< "'" << (pOrder->OpenClose() == 1 ? "O":"C" )	<< "', ";
	
	if( strcmp(pOrder->OrderExpDate(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->OrderExpDate()	<< "', ";
	
	if( strcmp(pOrder->OrderTag(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->OrderTag() 		<< "', ";
	
	stm	<< pOrder->Type()								<< ", ";
	stm << pOrder->OrgShares()							<< ", ";
	
	if( strcmp(pOrder->Owner(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->Owner() 		<< "', ";
	
	stm	<< pOrder->Price()								<< ", ";
	
	if( strcmp(pOrder->PriceCheck(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->PriceCheck() 	<< "', ";
	
	stm	<< pOrder->Shares()							<< ", ";
	
	if( strcmp(pOrder->Report(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->Report()		<< "', ";
	
	if( strcmp(pOrder->SecType(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->SecType() 		<< "', ";
	
	stm	<< pOrder->m_nSide								<< ", ";
	
	if( strcmp(pOrder->Solicited(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->Solicited() 	<< "', ";
	
	stm	<< pOrder->StopPrice()							<< ", ";

	stm	<< pOrder->StrikePrice()						<< ", ";
	
	if( strcmp(pOrder->Symbol(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->Symbol() 		<< "', ";
	
	if( strcmp(pOrder->Tag(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->Tag() 			<< "', ";
	
	if( strcmp(pOrder->Text(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->Text() 			<< "', ";
	
	stm	<< pOrder->TIF()								<< ", ";
	
	if( strcmp(pOrder->TradeFor(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->TradeFor() 		<< "', ";
	
	if( strcmp(pOrder->Trader(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->Trader() 		<< "', ";
	
	if( strcmp(pOrder->Underlying(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->Underlying() 	<< "', ";

	if( strcmp(pOrder->ShortLender(), "") == 0)
		stm << "null, ";
	else
		stm << "'" << pOrder->ShortLender() << "', ";
	
	if( pOrder->ExecInst() == -1 )
		stm << "null, ";
	else
		stm << "'" << (char)(pOrder->ExecInst()) << "', ";

	if( strcmp(pOrder->EffectiveTime(), "") == 0)
		stm << "null ";
	else
		stm << "'" << pOrder->EffectiveTime() << "' ";

	if( strlen(pOrder->BillingCode()) == 0)
		stm << "null ";
	else
		stm << "'" << pOrder->BillingCode() << "' ";

	if( strlen(pOrder->ParentOmTag()) == 0)
		stm << "null ";
	else
		stm << "'" << pOrder->ParentOmTag() << "' ";

	if( strlen(pOrder->SecID()) == 0)
		stm << "null ";
	else
		stm << "'" << pOrder->SecID() << "' ";

	if( pOrder->CplxType() == -1)
		stm << "null ";
	else
		stm << "'" << pOrder->CplxType() << "' ";

	if( pOrder->PegOffset() == 0)
		stm << "null ";
	else
		stm << "'" << pOrder->PegOffset() << "' ";

	if( strlen(pOrder->SecDef()) == 0)
		stm << "null ";
	else
		stm << "'" << pOrder->SecDef() << "' ";

	if( pOrder->AlgoType() == 0)
		stm << "null ";
	else
		stm << "'" << pOrder->AlgoType() << "' ";

	if( strlen(pOrder->ExpireDate()) == 0)
		stm << "null ";
	else
		stm << "'" << pOrder->ExpireDate() << "' ";

	return stm.str();
}

void CDBConnection::GenerateOrderNum( COrderMsg * pOrder )
{
	if(pOrder->StaffordTag().empty())
	{
		return;
	}
	
	std::string tag(pOrder->StaffordTag());
	int nPos = tag.rfind( '-' );
	std::stringstream stm;
	std::string szNum = tag.substr( nPos+1, -1 );
	tag = tag.substr( 0, nPos );
	std::string szDate = tag.substr( tag.length()-6, -1 );
	stm << szDate << std::setw(8) << std::setfill('0') << szNum;
//	LONG64 num = _atoi64( stm.str().c_str() );
	pOrder->m_szOrderNum = stm.str().c_str();
}


void CDBConnection::GenerateOrderNumNew(COrderMsg* pOrder)
{
	//if( strcmp(pOrder->StaffordTag(), "") == 0)
	if (pOrder->StaffordTag().empty())
	{
		return;
	}

	int index1 = pOrder->StaffordTag().rfind('-');
	std::string szNum = pOrder->StaffordTag().substr(index1 + 1);
	std::string szDate = pOrder->StaffordTag().substr(index1 - 6, 6);
	int index2 = pOrder->StaffordTag().find('-');
	short romInstanceOffset = (CConfig::getInstance()).romInstanceOffset();

	std::string romInstance = pOrder->StaffordTag().substr(index2 + 1, index1 - index2 - romInstanceOffset);
	std::stringstream stm;
	/*std::string tag(pOrder->StaffordTag());
	int nPos = tag.find('-');
	char omInstance = tag[nPos + 1];
	nPos = tag.rfind( '-' );
	std::stringstream stm;
	std::string szNum = tag.substr( nPos+1, -1 );
	tag = tag.substr( 0, nPos );
	std::string szDate = tag.substr( tag.length()-6, -1 );*/

	stm << romInstance << szDate << std::setw(11) << std::setfill('0') << szNum;;

	pOrder->m_szOrderNum = stm.str();
}
