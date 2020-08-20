#if !defined(AFX_OMSESSION_H__5F05EFA4_6103_442B_B4DE_F09D34943981__INCLUDED_)
#define AFX_OMSESSION_H__5F05EFA4_6103_442B_B4DE_F09D34943981__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OmSession.h : header file
//

class IOMApplication;


/////////////////////////////////////////////////////////////////////////////
// COmSession command target

class COmSession : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	COmSession( IOMApplication * pApp );
	virtual ~COmSession();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COmSession)
	public:
	virtual void OnReceive(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(COmSession)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
	IOMApplication *m_pApp;
	CString			m_szLeft;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OMSESSION_H__5F05EFA4_6103_442B_B4DE_F09D34943981__INCLUDED_)
