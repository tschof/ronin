#if !defined(AFX_TAGMAPSET_H__35116FE2_FBB9_11D1_8D08_006008CD4894__INCLUDED_)
#define AFX_TAGMAPSET_H__35116FE2_FBB9_11D1_8D08_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TagMapSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTagMapSet recordset

#define MAP_STAFFORD_TAG_QUERY	0
#define MAP_EXCHANGE_TAG_QUERY	1
#define MAP_CANCEL_TAG_QUERY		2
#define MAP_ORDER_TAG_QUERY		3
#define MAP_NEW_MAP					4
#define MAP_SEQUENCE_TAG_QUERY	5


class CTagMapSet : public CRecordset
{
public:
	CTagMapSet(CDatabase* pDatabase);
	~CTagMapSet()
		{
		if (IsOpen())
			Close();
		}

	DECLARE_DYNAMIC(CTagMapSet)

// Field/Param Data
	//{{AFX_FIELD(CTagMapSet, CRecordset)
	CString	m_StaffordTag;
	CString	m_OrderTag;
	CString	m_ExchangeTag;
	CString	m_CancelTag;
	long	m_SequenceTag;
	//}}AFX_FIELD

private:
	CString			m_QueryTag;
	int				m_QueryType;
	CString			m_TableName;
	CString			m_Exchange;

private:
	BOOL UpdateTags(const CString& StaffordTag, const CString& ExchangeTag, const CString& CancelTag, const CString& OrderTag, const long SeqNum);

public:
	BOOL UpdateTags(int QueryType, const CString& QueryTag, const CString& StaffordTag, const CString& ExchangeTag, const CString& CancelTag, const CString& OrderTag, const long SeqNum);
	BOOL GetTags(int QueryType, const CString& QueryTag);
	BOOL CreateNewMap(const CString& StaffordTag, const CString& OrderTag, const long SeqNum)	{ return(UpdateTags(MAP_NEW_MAP, "", StaffordTag, "", "", OrderTag, SeqNum)); }
	
	const CString& StaffordTag()	{ return m_StaffordTag; }
	const CString& CancelTag()		{ return m_CancelTag;	}
	const CString& ExchangeTag()	{ return m_ExchangeTag; }
	const CString& OrderTag()		{ return m_OrderTag;		}
	const long		Sequence()		{ return m_SequenceTag; }

	BOOL	SetStaffordTag(const CString& Tag);
	BOOL	SetCancelTag(const CString& Tag);
	BOOL	SetExchangeTag(const CString& Tag);
	BOOL	SetOrderTag(const CString& Tag);
	BOOL	SetSequenceTag(const long SeqNum);
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTagMapSet)
	public:
	virtual CString GetDefaultConnect();    // Default connection string
	virtual CString GetDefaultSQL();    // Default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);  // RFX support
	//}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TAGMAPSET_H__35116FE2_FBB9_11D1_8D08_006008CD4894__INCLUDED_)
