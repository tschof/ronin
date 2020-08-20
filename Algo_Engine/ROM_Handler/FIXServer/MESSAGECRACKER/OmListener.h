#if !defined(AFX_OMLISTENER_H__27044FFE_CEA4_41EC_B1A3_E686B4583131__INCLUDED_)
#define AFX_OMLISTENER_H__27044FFE_CEA4_41EC_B1A3_E686B4583131__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OmListener.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// COmListener command target
class IOMApplication;

class COmListener : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	COmListener( IOMApplication * pApp );
	virtual ~COmListener();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COmListener)
	public:
	virtual void OnAccept(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(COmListener)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
	IOMApplication * m_pApp;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OMLISTENER_H__27044FFE_CEA4_41EC_B1A3_E686B4583131__INCLUDED_)
