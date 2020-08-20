/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 * CommonDatabase.cpp: implementation of the CCommonDatabase class.
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 * 09/28/05    joel        Change DB default value
 *
 *******************************************************************************/

#include "stdafx.h"
#include "CommonDatabase.h"

#pragma warning(disable:4786)

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommonDatabase::CCommonDatabase(const CString& FileName)
{
	// read values from exchange's DAT file
		GetPrivateProfileString("Database Information", "User",			"rom_user",					m_User.GetBuffer(128),			128, FileName); m_User.ReleaseBuffer();
		GetPrivateProfileString("Database Information", "Password",		"resu_mor",				m_Password.GetBuffer(128),		128, FileName); m_Password.ReleaseBuffer();
		GetPrivateProfileString("Database Information", "Database",		"rom",					m_Database.GetBuffer(128),		128, FileName); m_Database.ReleaseBuffer();
		GetPrivateProfileString("Database Information", "DataSource",	"RON_SYB_DEV1",				m_DataSource.GetBuffer(128),	128, FileName); m_DataSource.ReleaseBuffer();
		GetPrivateProfileString("Database Information", "Server",		"RON_SYB_DEV1",	m_Server.GetBuffer(128),		128, FileName); m_Server.ReleaseBuffer();
		
	// get workstation name
		gethostname(m_WorkStation.GetBuffer(128), 128); m_WorkStation.ReleaseBuffer();

	// init
		m_pDatabase		= NULL;

}

CCommonDatabase::~CCommonDatabase()
{

	if (m_pDatabase)
		{
		if (m_pDatabase->IsOpen())
			m_pDatabase->Close();
		delete m_pDatabase;
		}			
}


BOOL CCommonDatabase::Connect()
{
	CString	SQLStatement;
	BOOL		Result = TRUE;
			
	try
		{
		if (m_pDatabase = new CDatabase())
			{
			m_Connect.Format("DSN=%s;DATABASE=%s;UID=%s;PWD=%s;WKID=%s", m_DataSource, m_Database, m_User, m_Password, m_WorkStation);
			if (m_pDatabase->OpenEx(m_Connect, CDatabase::useCursorLib | CDatabase::noOdbcDialog))
				{

				
				}
			else
				Result = FALSE;
			}
		else
			Result = FALSE;
		}

	catch (CDBException *e)
		{
		CString Error;
		Error.Format("EXCEPTION: Error Opening Database: %s", e->m_strError);
		AfxMessageBox(Error);
		return(FALSE);
		}
	catch (CMemoryException *e)
		{
		return(FALSE);
		}

return(Result);
}

