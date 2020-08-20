// ExecutionMapSet.cpp : implementation file
//

#pragma warning(disable:4786)

#include "stdafx.h"
#include "MessageApp.h"
#include "Message.h"
#include "ExecutionMapSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExecutionMapSet

IMPLEMENT_DYNAMIC(CExecutionMapSet, CRecordset)

CExecutionMapSet::CExecutionMapSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CExecutionMapSet)
	m_ExchangeTag = _T("");
	m_StaffordTag = _T("");
	m_OrderTag = _T("");
	m_ExecutionTag = _T("");
	m_Date = _T("");
	m_nFields = 5;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;


	m_TableName = "[" + ((CMessageApp *)AfxGetApp())->Name() + "ExecMap]";
}


CString CExecutionMapSet::GetDefaultConnect()
{
	return _T("");
}

CString CExecutionMapSet::GetDefaultSQL()
{
	return _T(m_Sql);
}

void CExecutionMapSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CExecutionMapSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Text(pFX, _T("[ExchangeTag]"), m_ExchangeTag);
	RFX_Text(pFX, _T("[StaffordTag]"), m_StaffordTag);
	RFX_Text(pFX, _T("[OrderTag]"), m_OrderTag);
	RFX_Text(pFX, _T("[ExecutionTag]"), m_ExecutionTag);
	RFX_Text(pFX, _T("[Date]"), m_Date);
	//}}AFX_FIELD_MAP
}
/////////////////////////////////////////////////////////////////////////////
// operations

BOOL CExecutionMapSet::AddMap(const CExecution& Exec)
{
	m_Sql = m_TableName;

	if (IsOpen())
		Close();

	if (Open(CRecordset::snapshot))
		{
		AddNew();

		m_ExchangeTag		= Exec.ExchangeTag();
		m_StaffordTag		= Exec.StaffordTag();
		m_OrderTag			= Exec.OrderTag();
		m_ExecutionTag		= Exec.Confirm();
		m_Date				= GetDate();

		Update();

		Close();

		return(TRUE);
		}

return(FALSE);
}

BOOL CExecutionMapSet::Lookup(const CString& Confirm, const CString& ExchangeTag)
{
	BOOL Found = FALSE;

	m_Sql = "select [ExchangeTag],[StaffordTag],[OrderTag],[ExecutionTag],[Date] from " + m_TableName + " where [ExecutionTag] = '" + Confirm + "' AND [ExchangeTag] = '" + ExchangeTag + "' AND [Date]='" + GetDate() + "'";

	if (IsOpen())
		Close();

	if (Open(CRecordset::snapshot))
		{
		if (GetRecordCount())
			Found = TRUE;
		
		Close();
		}

return(Found);
}

/////////////////////////////////////////////////////////////////////////////
// CExecutionMapSet diagnostics

#ifdef _DEBUG
void CExecutionMapSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CExecutionMapSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
