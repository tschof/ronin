// OMDSServer.h : main header file for the OMDSSERVER application
//

#if !defined(AFX_OMDSSERVER_H__B103EDCF_B615_4DF5_B3A7_A2E5FC277001__INCLUDED_)
#define AFX_OMDSSERVER_H__B103EDCF_B615_4DF5_B3A7_A2E5FC277001__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// COMDSServerApp:
// See OMDSServer.cpp for the implementation of this class
//

class COMDSServerApp : public CWinApp
{
public:
	COMDSServerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COMDSServerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(COMDSServerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OMDSSERVER_H__B103EDCF_B615_4DF5_B3A7_A2E5FC277001__INCLUDED_)
