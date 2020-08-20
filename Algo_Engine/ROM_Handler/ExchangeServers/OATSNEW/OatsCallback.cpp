/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 * OatsCallback.cpp: implementation of the COatsCallback class.
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 * 09/28/05    joel        Change DB default values
 *
 *******************************************************************************/

#include "stdafx.h"
#include "oats.h"
#include "OatsCallback.h"
#include "RequestFactory.h"
#include "DBConnection.h"
#include "TestDB.h"
#include "Logger.h"
#include "OMRequest.h"

#include <sstream>
#include <string>
#include <iomanip>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include "Csv.h"
#include "Order.h"
#include "OatsDlg.h"
#include "Logger.h"
#include "ThreadPoolManager.h"
#include "OrderMsg.h"

//extern COatsDlg *theDlg;

int COatsCallback::m_nThreads = 1;

COatsCallback::COatsCallback() 
: m_pMgr(NULL)
, m_eLog(NULL)
, m_oLog(NULL)
, m_fLog(NULL)
, m_cLog(NULL)
, m_nCount(0)
, m_nSize(1000)
{
	try
	{
		//{ WXU, clean unused dir
		m_eLog = new ZLog("c:\\OrderManager\\Oats.dat", ZLog::EQUITY);
		m_oLog = new ZLog("c:\\OrderManager\\Oats.dat", ZLog::OPTION);
		m_fLog = new ZLog("c:\\OrderManager\\Oats.dat", ZLog::FUTURE);
		m_cLog = new ZLog("c:\\OrderManager\\Oats.dat", ZLog::CURRENCY);
		UpdateRunDate("c:\\OrderManager\\Oats.dat");
	}
	catch(exception &ex)
	{
		Logger::getInstance().LogError( ex.what() );
		AfxMessageBox(ex.what());
		PostQuitMessage(0);
		return;
	}

	GetPrivateProfileString("Database Information", "Enable", "False", m_szEnable, 32, "\\OrderManager\\Oats.dat");
	GetPrivateProfileString("Database Information", "DSN", "RON_SYB_DEV1", m_szServer, 32, "\\OrderManager\\Oats.dat");
	GetPrivateProfileString("Database Information", "DSN", "RON_SYB_DEV1", m_szServer, 32, "\\OrderManager\\Oats.dat");
	GetPrivateProfileString("Database Information", "User",	"rom_user",	m_szUser,	32, "\\OrderManager\\Oats.dat");
	GetPrivateProfileString("Database Information", "Password", "resu_mor", m_szPass, 32, "\\OrderManager\\Oats.dat");
	GetPrivateProfileString("Database Information", "Database", "rom", m_szDB, 32, "\\OrderManager\\Oats.dat");
	m_nLevel = GetPrivateProfileInt("Database Information", "DebugLevel", 1, "\\OrderManager\\Oats.dat");
	m_nThreads = GetPrivateProfileInt("Database Information", "ThreadNum", 1, "\\OrderManager\\Oats.dat");

	char szSpecial[256];
	GetPrivateProfileString("Database Information", "Special Users", "", szSpecial, sizeof(szSpecial), "\\OrderManager\\Oats.dat");
	m_Users.clear();
	if( strlen(szSpecial) != 0 )
	{
		std::string special = (LPCSTR)szSpecial;
		std::stringstream stm(special);
		std::string szUser;
		while( !stm.fail() )
		{
			stm >> szUser;
			if( szUser == "" )
				continue;
			m_Users.insert( szUser );
		}
	}

	
	m_pMgr = new ThreadPoolManager( m_nThreads, m_nThreads, m_nThreads );
	m_pSlaveMgr = new ThreadPoolManager( 1, 1 );

	if( !CDBConnection::getInstance().Open( m_szServer, m_szUser, m_szPass, m_szDB ) )
	{
		AfxMessageBox("Database Connection Failure!");
		AfxGetMainWnd()->PostMessage( WM_QUIT, 0, 0 );
		return;
	}
	CDBConnection::getInstance().SetLogLevel( m_nLevel );
	Logger::getInstance().LogMessage( "Database Connectioned" );

#ifdef USE_MEM_MAP
	LoadMap();
#endif
}

COatsCallback::~COatsCallback()
{
	if( m_pMgr )
	{
		m_pMgr->ShutDown(3000);
		delete m_pMgr;
	}

	if (m_pAdminMgr)
	{
		//m_pAdminMgr->ShutDown(3000);
		m_pAdminMgr->PostThreadMessage(WM_QUIT, 0, 0);
		//Sleep(500);
		//delete m_pAdminMgr;
	}

	if (m_pSlaveMgr)
	{
		m_pSlaveMgr->ShutDown(3000);
		delete m_pSlaveMgr;
	}
	
	CDBConnection::getInstance().Close();

	if (m_eLog)
	{
		delete m_eLog;
	}

	if (m_oLog)
	{
		delete m_oLog;
	}

	if (m_fLog)
	{
		delete m_fLog;
	}

	if (m_cLog)
	{
		delete m_cLog;
	}
}

void COatsCallback::onData( CString & szMsg )
{
	if (szMsg[0] == 'L' ||
		szMsg[0] == 'K')
	{
		return;
	}
//	CCsv csv;
//	CString msg = szMsg+"\n";
//	csv.Bust( szMsg );
/*	switch( csv.cGet( CSV_COMMAND ) )
	{
	case 'S':
		//if (csv.nGet(CSV_CPLXORDER_TYPE) == COrder::CBOELEG)
		//{
		//	return;
		//}
		break;
	case 'E':
		break;
	case 'J':
		m_AdminServer.Send(csv);
		return;
	case 'L':
	case 'K':
	default:
		{
			Logger::getInstance().LogError( (LPCSTR)szMsg );
		}
		return;
	}*/

	//m_nCount++;

	//CQueueRequest * pAdminMsg = CRequestFactory::getInstance().createRequest( szMsg );
	//if (pAdminMsg)
	//{
		QueueAdminMessage(szMsg);
	//}

	if( stricmp( m_szEnable, "False" ) == 0 )
	{
		return;
	}

	CQueueRequest * pDBMsg = CRequestFactory::getInstance().createRequest( szMsg, false );
	if (m_pSlaveMgr)
	{
		QueueDBMessage(pDBMsg);
	}

	//m_AdminServer.Send( msg );
	//WriteCSV( &csv );
	
	//if( stricmp( m_szEnable, "False" ) == 0 )
	//{
	//	return;
	//}
/*
#ifdef USE_MEM_MAP
	COMRequest * pMsg = new COMRequest();
	if( !pMsg )
		ReloadMap();
	pMsg = new COMRequest();
	if( !pMsg )
		return;
	pMsg->Bust( msg );
#else
	CQueueRequest * pMsg = CRequestFactory::getInstance().createRequest( &csv );
#endif
	if( pMsg )
		QueueMessage( pMsg );*/

}


void COatsCallback::QueueDBMessage(CCsv& msg)
{
	CQueueRequest * pMsg = CRequestFactory::getInstance().createRequest( &msg);

	if( pMsg )
	{
		IncrementReceived();
		QueueMessage( pMsg );
	}
}


void COatsCallback::IncrementReceived()
{
	m_Count.Increment(1);
}


/*
void COatsCallback::AdminSend(CString& msg)
{
	m_AdminServer.Send( msg );

	CCsv csv;
	csv.Bust(msg);

	WriteCSV( &csv );
}*/

bool COatsCallback::QueueMessage( CQueueRequest * pMsg )
{
	if( !m_pMgr )
		return false;
	m_pMgr->AddRequest( pMsg );
	return true;
}

/*
bool COatsCallback::QueueAdminMessage(CQueueRequest* pMsg)
{
	if (!m_pAdminMgr)
	{
		return false;
	}
	else
	{
		m_pAdminMgr->AddRequest(pMsg);
		return true;
	}
}*/
bool COatsCallback::QueueAdminMessage(CString& msg)
{
	if (!m_pAdminMgr)
	{
		return false;
	}
	else
	{
		m_pAdminMgr->Send(msg);
		return true;
	}
}

bool COatsCallback::QueueDBMessage(CQueueRequest* pMsg)
{
	if (!m_pSlaveMgr)
	{
		return false;
	}
	else
	{
		m_pSlaveMgr->AddRequest(pMsg);
		return true;
	}
}

void COatsCallback::PauseDB()
{
	m_pMgr->Suspend();
}

void COatsCallback::ResumeDB()
{
	m_pMgr->Resume();
}

void COatsCallback::WriteCSV(CCsv* pCsv)
{
	CString strPass = pCsv->Build();
	char secType = pCsv->cGet(CSV_SECURITY_TYPE);

	if (COrder::IsEquityType(secType))
	{
		m_eLog->Log(strPass.SpanExcluding("\n\r").GetBuffer(0));
	}
	else if (COrder::IsOptionType(secType))
	{
		m_oLog->Log(strPass.SpanExcluding("\n\r").GetBuffer(0));
	}
	else if (COrder::IsFutureType(secType))
	{
		m_fLog->Log(strPass.SpanExcluding("\n\r").GetBuffer(0));
	}
	else if (COrder::IsForexType(secType))
	{
		m_cLog->Log(strPass.SpanExcluding("\n\r").GetBuffer(0));
	}
}

bool COatsCallback::StartServer()
{
	BOOL yes = true;
	
	CString	szDatFile = "c:\\Ordermanager\\Oats.dat";
	int nPort = GetPrivateProfileInt("OM Server Information", "Port", 4000, szDatFile );
	int nBufSize = GetPrivateProfileInt("OM Server Information", "Socket Buffer Size", 0x6000, szDatFile );

	if( FALSE == m_OMServer.Create(nPort) )
	{
		return false;
	}

	m_OMServer.SetSockOpt(SO_SNDBUF, &nBufSize, sizeof(nBufSize));
	m_OMServer.SetSockOpt(SO_RCVBUF, &nBufSize, sizeof(nBufSize));

	if( FALSE == m_OMServer.Listen() )
		return false;

	m_pAdminMgr = new CAdminThread();
	m_pAdminMgr->CreateThread();

//	nPort = GetPrivateProfileInt("Admin Server Information", "Port", 4100, szDatFile );
//	if( FALSE == m_AdminServer.Create(nPort) )
//		return false;
//	if( FALSE == m_AdminServer.Listen() )
//		return false;
	return true;
}

long COatsCallback::GetReceived()
{
	//return m_nCount;
	return m_Count.Value();
}

long COatsCallback::GetDatabased()
{
	return CDBConnection::GetCount();
}

/*long COatsCallback::GetAdminCount()
{
	return m_AdminServer.Count();
}
*/
void COatsCallback::LoadMap()
{
	m_nSize = 2;
	DataStore<COMRequest>::getInstance().Initialize( "Oats.map", m_nSize );
	COMRequest * pRequest = DataStore<COMRequest>::getInstance().FirstUsed();
	while( pRequest )
	{
		pRequest = DataStore<COMRequest>::getInstance().NextUsed();
		if( !pRequest )
		{
			ReloadMap();
			break;
		}
		QueueMessage( pRequest );
	}

}

void COatsCallback::ReloadMap()
{
	m_nSize = m_nSize*2;
	DataStore<COMRequest>::getInstance().Grow( m_nSize );
}


void COatsCallback::SendToOM(CCsv & szMessage )
{
	Logger::getInstance().LogDebug( (LPCSTR)szMessage.Message() );
	m_OMServer.Send(szMessage);
}

/*void COatsCallback::SendToAdmin(CString& szMessage )
{
	m_AdminServer.Send(szMessage);
	CCsv csv;
	csv.Bust(szMessage);
	WriteCSV( &csv );
}*/




void COatsCallback::UpdateRunDate(string datFile)
{
	CString CurrentDate = CTime::GetCurrentTime().Format("%Y%m%d");
	WritePrivateProfileString("Runtime Information", "Last Run Date", CurrentDate, datFile.c_str());
}


void COatsCallback::ProcessAdminRequest(CCsv* pRequest)
{
	if (pRequest->cGet(CSV_COMMAND) == (char)(CMessage::Admin))
	{
		if (pRequest->nGet(CSV_COMMAND_FLEAK) == QUERY_OATS)
		{
			if (theDlg)
			{
				CString sRequest = pRequest->Build();
				CCsv csv(sRequest);
				CString text;
				text.Format("Oats:Received %s messages and Recorded %s messages", theDlg->Received(), theDlg->Recorded());
				csv.SetAt(CSV_COMMAND, (char)CMessage::ResponseToAdmin);
				csv.SetAt(CSV_TEXT, text);
				sRequest = csv.Build();
				QueueAdminMessage(sRequest);
			}
		}
		else
		{
			SendToOM(*pRequest);
		}
	}
	else 
	{
		SendToOM(*pRequest);
	}
}


void COatsCallback::RomInstanceUpdate(int adminID)
{
	CString roms;
	m_OMServer.GetRomInstances(roms);
	CCsv csv((char)(CMessage::ResponseToAdmin));
	csv.SetAt(CSV_COMMAND_FLEAK, ROM_INSTANCES_UPDATE);
	if (adminID != -1)
	{
		csv.SetAt(CSV_CLIENT_FLEAK, adminID);
	}
	csv.SetAt(CSV_ROM_INSTANCES, roms);
	CString sRequest = csv.Build();
	QueueAdminMessage(sRequest);
}

