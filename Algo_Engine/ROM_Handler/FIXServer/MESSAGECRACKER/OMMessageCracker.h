// MessageCracker.h : main header file for the MESSAGECRACKER application
//

#if !defined(AFX_MESSAGECRACKER_H__349F98B2_1CC6_4C2A_B2ED_64B47C85CDE9__INCLUDED_)
#define AFX_MESSAGECRACKER_H__349F98B2_1CC6_4C2A_B2ED_64B47C85CDE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CCrackerApp:
// See MessageCracker.cpp for the implementation of this class
//
#include <string>
#include <map>
#include "FixMessage.h"

class CCrackerApp : public CWinApp
{
public:
	CCrackerApp();

public:
	typedef std::map<std::string,std::string>	tSymbolMap;
	tSymbolMap									m_OptionSymbols;

	typedef std::map<std::string,FixMessage>	tMessageMap;
	tMessageMap									m_Messages;
	FixMessage									m_Header;
	FixMessage									m_Trailer;

	typedef std::map<int,std::string>			tOmFieldMap;
	tOmFieldMap									m_SendFields;
	tOmFieldMap									m_ReceiveFields;

	CSingleDocTemplate*							m_pDocTemplate;

	bool LoadOptionSymbols();
	bool LoadFixSpec	();
	bool LoadOmSpec		();
	void AddMessage		( const std::string & szMsg );
	FixTag FindTag( int nTagID, FixMessage & msg );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCrackerApp)
	public:
	virtual BOOL InitInstance();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName); // open named file
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CCrackerApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESSAGECRACKER_H__349F98B2_1CC6_4C2A_B2ED_64B47C85CDE9__INCLUDED_)
