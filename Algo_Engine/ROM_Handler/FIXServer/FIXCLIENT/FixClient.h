// FixClient.h : main header file for the FIXCLIENT application
//

#if !defined(AFX_FIXCLIENT_H__83C8FB2C_D079_4207_983E_F59FA8D43D0D__INCLUDED_)
#define AFX_FIXCLIENT_H__83C8FB2C_D079_4207_983E_F59FA8D43D0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFixClientApp:
// See FixClient.cpp for the implementation of this class
//

class CFixClientApp : public CWinApp
{
public:
	CFixClientApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFixClientApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFixClientApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIXCLIENT_H__83C8FB2C_D079_4207_983E_F59FA8D43D0D__INCLUDED_)
