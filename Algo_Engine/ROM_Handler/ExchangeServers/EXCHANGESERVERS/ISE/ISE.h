// ISE.h : main header file for the ISE application
//

#if !defined(AFX_ISE_H__BC7AB843_E979_4E9F_A9D5_6E66A602B252__INCLUDED_)
#define AFX_ISE_H__BC7AB843_E979_4E9F_A9D5_6E66A602B252__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "exchangeapp.h"
#include "isedoc.h"
#include "iseview.h"
/////////////////////////////////////////////////////////////////////////////
// CISEApp:
// See ISE.cpp for the implementation of this class
//

class CISEApp : public CExchangeApp
{
private:
	CISEView	*m_pClientView;

public:
	CISEApp();
	~CISEApp();
	virtual void UpdateClients();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CISEApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CISEApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ISE_H__BC7AB843_E979_4E9F_A9D5_6E66A602B252__INCLUDED_)
