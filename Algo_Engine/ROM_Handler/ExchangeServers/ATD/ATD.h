// ATD.h : main header file for the ATD application
//

#if !defined(AFX_ATD_H__E78F621D_3CA1_465A_B333_42DE4D536BE7__INCLUDED_)
#define AFX_ATD_H__E78F621D_3CA1_465A_B333_42DE4D536BE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "exchangeapp.h"
#include "ATDdoc.h"
#include "ATDview.h"

/////////////////////////////////////////////////////////////////////////////
// CATDApp:
// See ATD.cpp for the implementation of this class
//

class CATDApp : public CExchangeApp
{
private:
	CATDView	*m_pClientView;

public:
	CATDApp();
	virtual void UpdateClients();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CATDApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CATDApp)
	afx_msg void OnAppAbout();
	afx_msg void OnReloadTags();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATD_H__E78F621D_3CA1_465A_B333_42DE4D536BE7__INCLUDED_)
