#if !defined(AFX_OMSESSION_H__79576FDF_713F_46D8_B112_0763A5138621__INCLUDED_)
#define AFX_OMSESSION_H__79576FDF_713F_46D8_B112_0763A5138621__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OmSession.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// COmSession command target
#include "SessionBase.h"

class COmSession : public CSessionBase
{
// Attributes
public:

// Operations
public:
	COmSession( COatsCallback * pDoc );
	virtual ~COmSession();

// Overrides
public:
	virtual void OnMessage( CString & szMsg );
	virtual void ProcessTimer();
	bool IsConnected() { return m_bConnected; }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COmSession)
	public:
	virtual void OnClose(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(COmSession)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OMSESSION_H__79576FDF_713F_46D8_B112_0763A5138621__INCLUDED_)
