// FixSequenceSet.cpp : implementation file
//

#include "stdafx.h"
#include "MessageApp.h"
#include "FixSequenceSet.h"

#pragma warning(disable:4786)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CTime	CurrentTime;

/////////////////////////////////////////////////////////////////////////////
// CFixSequenceSet

IMPLEMENT_DYNAMIC(CFixSequenceSet, CRecordset)

CFixSequenceSet::CFixSequenceSet(CDatabase* pdb, CString StationID)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CFixSequenceSet)
	m_NextOut = 1;
	m_NextIn = 1;
	m_Host = _T("");
	m_Date = _T("");
	m_nFields = 4;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
	if(StationID.GetLength())
		m_QueryHost = ((CMessageApp *)AfxGetApp())->Name() + "_" + StationID;
	else
		m_QueryHost = ((CMessageApp *)AfxGetApp())->Name();

	m_StationID = StationID;
}


CString CFixSequenceSet::GetDefaultConnect()
{
	return _T("");
}

CString CFixSequenceSet::GetDefaultSQL()
{
	if (m_QueryDate != "")
		return _T("SELECT [NextOut],[NextIn],[Host],[Date] FROM [FixSequenceNumbers] WHERE [Host] = '" + m_QueryHost + "' AND [Date] = '" + m_QueryDate + "'");
	else
		return _T("SELECT [NextOut],[NextIn],[Host],[Date] FROM [FixSequenceNumbers] WHERE [Host] = '" + m_QueryHost + "'");
}

void CFixSequenceSet::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CFixSequenceSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Long(pFX, _T("[NextOut]"), m_NextOut);
	RFX_Long(pFX, _T("[NextIn]"), m_NextIn);
	RFX_Text(pFX, _T("[Host]"), m_Host);
	RFX_Text(pFX, _T("[Date]"), m_Date);
	//}}AFX_FIELD_MAP
}


BOOL CFixSequenceSet::CheckForRollover()
{
	m_QueryDate = CTime::GetCurrentTime().Format("%Y%m%d");

	Requery();

	if (!(GetRecordCount()))
	{
		m_QueryDate = "";

		// reopen so new query takes effect
		Close();
		Open(CRecordset::snapshot);

		if (!(GetRecordCount()))
		{
			AddNew();
			m_Host = m_QueryHost;
		}
		else
			Edit();

		m_QueryDate	= CTime::GetCurrentTime().Format("%Y%m%d");
		m_NextOut	= 1;
		m_NextIn	= 1;
		m_Date		= m_QueryDate;

		Update();

		Requery();

		return(TRUE);
	}

	return(FALSE);
}

long CFixSequenceSet::GetNext()
{
	// prepare for change
	Edit();
	// increment counter
	m_NextOut++;
	// udpate table
	Update();

	return(m_NextOut-1);
}

void CFixSequenceSet::SetNext(long SequenceNumber)
{
	// prepare for change
	Edit();
	// increment counter
	m_NextOut = SequenceNumber;
	// udpate table
	Update();
}

long CFixSequenceSet::GetExpected()
{
	// prepare for change
	Edit();
	// increment counter
	m_NextIn++;
	// udpate table
	Update();

	return(m_NextIn - 1);
}

void CFixSequenceSet::SetExpected(long SequenceNumber)
{
	if (SequenceNumber < 1)
		SequenceNumber = 1;

	// prepare for change
	Edit();
	// increment counter
	m_NextIn = SequenceNumber;
	// udpate table
	Update();
}


////////////////////////////////////////////////////////////////////////////
// CFixSequenceSet diagnostics

#ifdef _DEBUG
void CFixSequenceSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CFixSequenceSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
