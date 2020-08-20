#if !defined(AFX_ADMINSERVER_H__C721BFD8_CD8A_4164_BB9A_E8A6E89D97C7__INCLUDED_)
#define AFX_ADMINSERVER_H__C721BFD8_CD8A_4164_BB9A_E8A6E89D97C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdminServer.h : header file
//
#include <list>
#include "Csv.h"
/////////////////////////////////////////////////////////////////////////////
// CAdminServer command target
class CAdminSession;

class CAdminServer : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	CAdminServer();
	virtual ~CAdminServer();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdminServer)
	public:
	virtual void OnAccept(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CAdminServer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	void Send( CString szMsg );
	void Send(CCsv& szMsg);
	void Remove( CAdminSession * pSession );
	long Count() { return m_Admins.size(); }
// Implementation
protected:
	typedef std::list<CAdminSession *> tAdmins;
	tAdmins	m_Admins;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADMINSERVER_H__C721BFD8_CD8A_4164_BB9A_E8A6E89D97C7__INCLUDED_)
