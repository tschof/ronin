/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 * MERRILL.h : main header file for the MERRILL application
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 * 10/18/05    joel        Initial version of MERRILL exchange interface (stocks)
 *
 *******************************************************************************/

#if !defined(AFX_MERRILL_H__E78F621D_3CA1_465A_B333_42DE4D536BE7__INCLUDED_)
#define AFX_MERRILL_H__E78F621D_3CA1_465A_B333_42DE4D536BE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "exchangeapp.h"
#include "MERRILLdoc.h"
#include "MERRILLview.h"

/////////////////////////////////////////////////////////////////////////////
// CMERRILLApp:
// See MERRILL.cpp for the implementation of this class
//

class CMERRILLApp : public CExchangeApp
{
private:
	CMERRILLView	*m_pClientView;

public:
	CMERRILLApp();
	virtual void UpdateClients();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMERRILLApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CMERRILLApp)
	afx_msg void OnAppAbout();
	afx_msg void OnReloadTags();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERRILL_H__E78F621D_3CA1_465A_B333_42DE4D536BE7__INCLUDED_)
