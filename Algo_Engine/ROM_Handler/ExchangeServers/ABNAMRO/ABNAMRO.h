// ABNAMRO.h : main header file for the ABNAMRO application
//

#if !defined(AFX_ABNAMRO_H__BC7AB843_E979_4E9F_A9D5_6E66A602B252__INCLUDED_)
#define AFX_ABNAMRO_H__BC7AB843_E979_4E9F_A9D5_6E66A602B252__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "exchangeapp.h"
#include "ABNAMROdoc.h"
#include "ABNAMROview.h"
/////////////////////////////////////////////////////////////////////////////
// CABNAMROApp:
// See ABNAMRO.cpp for the implementation of this class
//

class CABNAMROApp : public CExchangeApp
{
private:
	CABNAMROView	*m_pClientView;

public:
	CABNAMROApp();
	~CABNAMROApp();
	virtual void UpdateClients();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CABNAMROApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CABNAMROApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABNAMRO_H__BC7AB843_E979_4E9F_A9D5_6E66A602B252__INCLUDED_)
