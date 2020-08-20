// CBOE.h : main header file for the CBOE application
//

#if !defined(AFX_CBOE_H__E78F621D_3CA1_465A_B333_42DE4D536BE7__INCLUDED_)
#define AFX_CBOE_H__E78F621D_3CA1_465A_B333_42DE4D536BE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "exchangeapp.h"
#include "cboedoc.h"
#include "cboeview.h"

/////////////////////////////////////////////////////////////////////////////
// CCBOEApp:
// See CBOE.cpp for the implementation of this class
//

class CCBOEApp : public CExchangeApp
{
private:
	CCBOEView	*m_pClientView;

public:
	CCBOEApp();
	virtual void UpdateClients();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCBOEApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CCBOEApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CBOE_H__E78F621D_3CA1_465A_B333_42DE4D536BE7__INCLUDED_)
