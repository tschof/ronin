#if !defined(AFX_EXECUTIONMAPSET_H__1E788F82_FF99_11D1_8D08_006008CD4894__INCLUDED_)
#define AFX_EXECUTIONMAPSET_H__1E788F82_FF99_11D1_8D08_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ExecutionMapSet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExecutionMapSet recordset

#pragma warning(disable:4786)

class CExecutionMapSet : public CRecordset
{
private:
	CString	m_Sql;
	CString	m_TableName;

private:
	CString	GetDate()	{ return CTime::GetCurrentTime().Format("%Y%m%d"); }
public:
	BOOL		AddMap(const CExecution& Exec);
	BOOL		Lookup(const CString& Confirm);

public:
	CExecutionMapSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CExecutionMapSet)

// Field/Param Data
	//{{AFX_FIELD(CExecutionMapSet, CRecordset)
	CString	m_ExchangeTag;
	CString	m_StaffordTag;
	CString	m_OrderTag;
	CString	m_ExecutionTag;
	CString	m_Date;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExecutionMapSet)
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

#endif // !defined(AFX_EXECUTIONMAPSET_H__1E788F82_FF99_11D1_8D08_006008CD4894__INCLUDED_)
