/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 * TestDB.cpp: implementation of the CTestDB class.
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
#include "TestDB.h"
#include "csv.h"
#include "RequestFactory.h"
#include "OrderMsg.h"
#include "DBConnection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTestDB::CTestDB( CString szMsgFile )
{
	char szDSN[32], szUser[32], szPass[32], szDatabase[32];
	GetPrivateProfileString("Database Information", "DSN", "RON_SYB_DEV1", szDSN, 32, "\\OrderManager\\Oats.dat");
	GetPrivateProfileString("Database Information", "User",	"rom_user",	szUser,	32, "\\OrderManager\\Oats.dat");
	GetPrivateProfileString("Database Information", "Password", "resu_mor", szPass, 32, "\\OrderManager\\Oats.dat");
	GetPrivateProfileString("Database Information", "Database", "rom", szDatabase, 32, "\\OrderManager\\Oats.dat");
	
	CDBConnection::getInstance().Open( szDSN, szUser, szPass, szDatabase );
	
	CFileException		fe;


	BOOL bContinue = TRUE;
	CString	Message;
	DWORD	BytesRead = 0;
	DWORD	FileSize = 0;
	
	CQueueRequest *pRequest = NULL;
	if ( MsgFile.Open( szMsgFile, CFile::modeRead|CFile::shareDenyNone, &fe ) )
	{
		while (1)
		{
			bContinue = MsgFile.ReadString(Message);
			BytesRead += Message.GetLength();
			pRequest = CRequestFactory::getInstance().createRequest(&CCsv(Message));
			if( !bContinue )
			{
				delete pRequest;
				pRequest = NULL;
				break;
			}
			else
			{
				if( pRequest )
					ProcessMessage(pRequest);
				delete pRequest;
			}
		}

		MsgFile.Close();
	}

}

CTestDB::~CTestDB()
{

}

bool CTestDB::ProcessMessage( CQueueRequest *pRequest )
{
	return pRequest->Process();
}



