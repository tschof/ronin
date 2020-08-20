#if !defined(AFX_FIXSEQUENCESET_H__69038F22_E4E8_11D1_8D02_006008CD4894__INCLUDED_)
#define AFX_FIXSEQUENCESET_H__69038F22_E4E8_11D1_8D02_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// FixSequenceSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFixSequenceSet recordset

class CFixSequenceSet : public CRecordset
{
public:
	CString	m_QueryHost;
	CString	m_QueryDate;
	CString	m_Exchange;
	CString m_StationID;

	const CString& StationID() { return m_StationID; }
	
public:

	BOOL	CheckForRollover();	// will reset seq num to 1 if date has changed
	long	GetNext();				// increments seq num and returns seq num - 1
	void	SetNext(long);			// set the next sequence number to recover from errors
	long	GetExpected();			// increments the incoming sequence number and returns the expected
	void	SetExpected(long);	// sets number. used after receiving a sequence reset

	int	NextIn()	{ return m_NextIn;  }
	int	NextOut()	{ return m_NextOut; }

public:
	CFixSequenceSet(CDatabase* pDatabase = NULL, CString StationID = "");
	DECLARE_DYNAMIC(CFixSequenceSet)

// Field/Param Data
	//{{AFX_FIELD(CFixSequenceSet, CRecordset)
	long	m_NextOut;
	long	m_NextIn;
	CString	m_Host;
	CString	m_Date;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFixSequenceSet)
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

#endif // !defined(AFX_FIXSEQUENCESET_H__69038F22_E4E8_11D1_8D02_006008CD4894__INCLUDED_)
