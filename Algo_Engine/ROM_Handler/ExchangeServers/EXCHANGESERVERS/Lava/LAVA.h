/*******************************************************************************
 *
 * Copyright (c) 2006 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 * LAVA.h : main header file for the LAVA application
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 *
 *******************************************************************************/

#if !defined(AFX_LAVA_H__E78F621D_3CA1_465A_B333_42DE4D536BE7__INCLUDED_)
#define AFX_LAVA_H__E78F621D_3CA1_465A_B333_42DE4D536BE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "exchangeapp.h"
#include "LAVAdoc.h"
#include "LAVAview.h"

/////////////////////////////////////////////////////////////////////////////
// CLAVAApp:
// See LAVA.cpp for the implementation of this class
//

class CLAVAApp : public CExchangeApp
{
private:
	CLAVAView	*m_pClientView;

public:
	CLAVAApp();
	virtual void UpdateClients();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLAVAApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CLAVAApp)
	afx_msg void OnAppAbout();
	afx_msg void OnReloadTags();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAVA_H__E78F621D_3CA1_465A_B333_42DE4D536BE7__INCLUDED_)
