// TagMapSet.cpp : implementation file
//

#include "stdafx.h"
#include "MessageApp.h"
#include "TagMapSet.h"

#pragma warning(disable:4786)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTagMapSet

IMPLEMENT_DYNAMIC(CTagMapSet, CRecordset)

CTagMapSet::CTagMapSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CTagMapSet)
	m_StaffordTag = _T("");
	m_OrderTag = _T("");
	m_ExchangeTag = _T("");
	m_CancelTag = _T("");
	m_SequenceTag = 0;
	m_nFields = 5;
	//}}AFX_FIELD_INIT
	m_nDefaultType = snapshot;
	
	m_QueryType = 0;
	m_QueryTag	= "";
	m_Exchange = ((CMessageApp *)AfxGetApp())->Name();
	m_TableName = "[" + m_Exchange + "TagMap]";
}


CString CTagMapSet::GetDefaultConnect()
{
	return _T("");
}

CString CTagMapSet::GetDefaultSQL()
{
	switch (m_QueryType)
		{
		case MAP_NEW_MAP:
			return(m_TableName);
			break;
		case MAP_STAFFORD_TAG_QUERY:
			return("SELECT [StaffordTag],[OrderTag],[ExchangeTag],[CancelTag],[SequenceTag] FROM " + m_TableName + " WHERE StaffordTag='" + m_QueryTag + "'");
			break;
		case MAP_EXCHANGE_TAG_QUERY:
			return("SELECT [StaffordTag],[OrderTag],[ExchangeTag],[CancelTag],[SequenceTag] FROM " + m_TableName + " WHERE ExchangeTag='" + m_QueryTag + "'");
			break;
		case MAP_CANCEL_TAG_QUERY:
			return("SELECT [StaffordTag],[OrderTag],[ExchangeTag],[CancelTag],[SequenceTag] FROM " + m_TableName + " WHERE CancelTag='" + m_QueryTag + "'");
			break;
		case MAP_ORDER_TAG_QUERY:
			return("SELECT [StaffordTag],[OrderTag],[ExchangeTag],[CancelTag],[SequenceTag] FROM " + m_TableName + " WHERE OrderTag='" + m_QueryTag + "'");
			break;
		case MAP_SEQUENCE_TAG_QUERY:
			return("SELECT [StaffordTag],[OrderTag],[ExchangeTag],[CancelTag],[SequenceTag] FROM " + m_TableName + " WHERE SequenceTag=" + m_QueryTag );
			break;
		default:
			return(m_TableName);
		}
}

void CTagMapSet::DoFieldExchange(CFieldExchange* pFX)
{
	
	//{{AFX_FIELD_MAP(CTagMapSet)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Text(pFX, _T("[StaffordTag]"), m_StaffordTag);
	RFX_Text(pFX, _T("[OrderTag]"), m_OrderTag);
	RFX_Text(pFX, _T("[ExchangeTag]"), m_ExchangeTag);
	RFX_Text(pFX, _T("[CancelTag]"), m_CancelTag);
	RFX_Long(pFX, _T("[SequenceTag]"), m_SequenceTag);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CTagMapSet diagnostics

#ifdef _DEBUG
void CTagMapSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CTagMapSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG

BOOL CTagMapSet::UpdateTags(const CString& StaffordTag, const CString& ExchangeTag, const CString& CancelTag, const CString& OrderTag, const long SequenceTag)
{
	if (IsOpen())
		Close();

	if (Open(CRecordset::snapshot))
		{
		if ((m_QueryType == MAP_NEW_MAP) || (!(GetRecordCount())))
			AddNew();
		else
			Edit();

		if (StaffordTag != "")	
			m_StaffordTag	= StaffordTag;
		if (CancelTag != "")		
			m_CancelTag		= CancelTag;
		if (OrderTag != "")		
			m_OrderTag		= OrderTag;
		if (ExchangeTag != "")	
			m_ExchangeTag	= ExchangeTag;
		if (SequenceTag > 0)		
			m_SequenceTag	= SequenceTag;

		Update();

		Close();

		return(TRUE);
		}

return(FALSE);
}

//////////////////////////////////////////////////////////////////////////
// tag management

BOOL CTagMapSet::UpdateTags(int QueryType, const CString& QueryTag, const CString& StaffordTag, const CString& ExchangeTag, const CString& CancelTag, const CString& OrderTag, const long SequenceTag)
{
	m_QueryType = QueryType;
	m_QueryTag = QueryTag;

return(UpdateTags(StaffordTag, ExchangeTag, CancelTag, OrderTag, SequenceTag));
}

BOOL CTagMapSet::GetTags(int QueryType, const CString& QueryTag)
{
	m_QueryType = QueryType;
	m_QueryTag = QueryTag;

	if (IsOpen())
		Close();

	if (!(Open(CRecordset::snapshot)))
		return(FALSE);
	else if (GetRecordCount())
		return(TRUE);
		
return(FALSE);
}

BOOL	CTagMapSet::SetStaffordTag(const CString& Tag)
{
	if ((!(IsOpen())) || (!(GetRecordCount())))
		return(FALSE);
	
	Edit();

	m_StaffordTag = Tag;

	Update();

return(TRUE);
}

BOOL	CTagMapSet::SetCancelTag(const CString& Tag)
{
	if ((!(IsOpen())) || (!(GetRecordCount())))
		return(FALSE);
	
	Edit();

	m_CancelTag = Tag;

	Update();

return(TRUE);
}

BOOL	CTagMapSet::SetExchangeTag(const CString& Tag)
{
	if ((!(IsOpen())) || (!(GetRecordCount())))
		return(FALSE);
	
	Edit();

	m_ExchangeTag = Tag;

	Update();

return(TRUE);
}

BOOL	CTagMapSet::SetOrderTag(const CString& Tag)
{
	if ((!(IsOpen())) || (!(GetRecordCount())))
		return(FALSE);
	
	Edit();

	m_OrderTag = Tag;

	Update();

return(TRUE);
}

BOOL	CTagMapSet::SetSequenceTag(const long Tag)
{
	if ((!(IsOpen())) || (!(GetRecordCount())))
		return(FALSE);
	
	Edit();

	m_SequenceTag = Tag;

	Update();

return(TRUE);
}
