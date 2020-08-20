// oats.h : main header file for the OATS application
//

#if !defined(AFX_OATS_H__8A627924_2B35_11D3_A8D7_006008CD4894__INCLUDED_)
#define AFX_OATS_H__8A627924_2B35_11D3_A8D7_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "OatsDlg.h"
/////////////////////////////////////////////////////////////////////////////
// COatsApp:
// See oats.cpp for the implementation of this class
//
const UINT TF_ADMIN_REQUEST = RegisterWindowMessage("TF_ADMIN_REQUEST");
const UINT TF_ADMINCOUNT_UPDATE = RegisterWindowMessage("TF_ADMINCOUNT_UPDATE");
const UINT TF_ADMIN_DISCONNECT = RegisterWindowMessage("TF_ADMIN_DISCONNECT");

class COrderMsg;

class COatsApp : public CWinApp
{
public:
	COatsApp();

	static void SendMail( LPCSTR szMsg );


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COatsApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(COatsApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	afx_msg void OnProcessAdminRequest( UINT wParam, LONG lParam );
	afx_msg void OnProcessAdminCountUpdate( UINT wParam, LONG lParam );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OATS_H__8A627924_2B35_11D3_A8D7_006008CD4894__INCLUDED_)
