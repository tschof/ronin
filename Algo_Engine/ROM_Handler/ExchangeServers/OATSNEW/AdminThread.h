#if !defined(AFX_ADMINTHREAD_H__0137401B_BE14_43F5_A23B_E22A682F4D01__INCLUDED_)
#define AFX_ADMINTHREAD_H__0137401B_BE14_43F5_A23B_E22A682F4D01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SlaveThread.h"
#include "AdminServer.h"
/////////////////////////////////////////////////////////////////////////////
// CAdminThread


class CAdminThread : public SlaveThread
{
	DECLARE_DYNCREATE(CAdminThread)
public:
	CAdminThread();           // protected constructor used by dynamic creation
	virtual ~CAdminThread();
// Attributes
public:

// Operations
public:
	virtual void ProcessMessage(CString& msg);
	virtual void ProcessThreadInstruction(const MSG& msg);
	virtual void ProcessMessages(StringList& msgs);
	virtual void PreRun();
	
//	int GetAdminCount() { return m_AdminServer.Count();}
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdminThread)
	//public:
	
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	CAdminServer m_AdminServer;

	// Generated message map functions
	//{{AFX_MSG(OatsConnectThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	
	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADMINTHREAD_H__0137401B_BE14_43F5_A23B_E22A682F4D01__INCLUDED_)
