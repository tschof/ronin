/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 * BRASSRecord.cpp : implementation file
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
#include "BRASS.h"
#include "BRASSRecord.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBRASSRecord

IMPLEMENT_DYNAMIC(CBRASSRecord, CRecordset)

CBRASSRecord::CBRASSRecord(CDatabase* pdb)
	: CIRecord(pdb)
{
	//{{AFX_FIELD_INIT(CBRASSRecord)
	m_CustomerID = _T("");
	m_TraderAcr = _T("");
	m_LocalAcAcr = _T("");
	m_ClearingAcID = _T("");
	m_FirmAcr = _T("");
	m_Flags = 0;
	m_nFields = 6;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
}


CString CBRASSRecord::GetDefaultConnect()
{
	return _T("ODBC;DSN=RON_SYB_DEV1");
}

CString CBRASSRecord::GetDefaultSQL()
{
	return _T("[dbo].[BRASSInterface]");
}

void CBRASSRecord::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CBRASSRecord)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Text(pFX, _T("[CustomerID]"), m_CustomerID);
	RFX_Text(pFX, _T("[TraderAcr]"), m_TraderAcr);
	RFX_Text(pFX, _T("[LocalAcAcr]"), m_LocalAcAcr);
	RFX_Text(pFX, _T("[ClearingAcID]"), m_ClearingAcID);
	RFX_Text(pFX, _T("[FirmAcr]"), m_FirmAcr);
	RFX_Byte(pFX, _T("[Flags]"), m_Flags);
	//}}AFX_FIELD_MAP
}

BOOL CBRASSRecord::Open()
{
	TRY
	{
		m_strSort = "CustomerID";
		CRecordset::Open( CRecordset::snapshot, 
						  NULL,
						  CRecordset::useBookmarks);
		return TRUE;
	}
	CATCH(CDBException, e)
	{
		TRACE("%s\n", e->m_strError);
		return FALSE;
    }
	END_CATCH	
}

/////////////////////////////////////////////////////////////////////////////
// CBRASSRecord diagnostics

#ifdef _DEBUG
void CBRASSRecord::AssertValid() const
{
	CRecordset::AssertValid();
}

void CBRASSRecord::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
