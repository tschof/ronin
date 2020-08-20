// OneChicago.h : main header file for the OneChicago application
//

#if !defined(AFX_OneChicago_H__E78F621D_3CA1_465A_B333_42DE4D536BE7__INCLUDED_)
#define AFX_OneChicago_H__E78F621D_3CA1_465A_B333_42DE4D536BE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "exchangeapp.h"
#include "OneChicagoDoc.h"
#include "OneChicagoView.h"

/////////////////////////////////////////////////////////////////////////////
// COneChicagoApp:
// See OneChicago.cpp for the implementation of this class
//

class COneChicagoApp : public CExchangeApp
{
private:
	COneChicagoView	*m_pClientView;

public:
	COneChicagoApp();
	virtual void UpdateClients();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COneChicagoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(COneChicagoApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OneChicago_H__E78F621D_3CA1_465A_B333_42DE4D536BE7__INCLUDED_)
