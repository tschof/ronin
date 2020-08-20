#if !defined(AFX_USERDATA_H__5AB455D2_426F_11D2_A836_006008CD4894__INCLUDED_)
#define AFX_USERDATA_H__5AB455D2_426F_11D2_A836_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// UserData.h : header file

/////////////////////////////////////////////////////////////////////////////
// CUSERDATA recordset
#include <string>

class CUserData : public CRecordset
{
public:
	bool IsValidLogin( const std::string & szUser, const std::string & szToken );
	
	CUserData(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CUserData)

// Field/Param Data
	//{{AFX_FIELD(CUserData, CRecordset)
	CString	m_UserName;
	CString	m_Password;
	CString m_Token;
	//}}AFX_FIELD

	CString m_UserParam;
	CString m_TokenParam;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserData)
	public:
	virtual CString GetDefaultConnect();    // Default connection string
	virtual CString GetDefaultSQL();    // Default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);  // RFX support
	//}}AFX_VIRTUAL

private:

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERDATA_H__5AB455D2_426F_11D2_A836_006008CD4894__INCLUDED_)
