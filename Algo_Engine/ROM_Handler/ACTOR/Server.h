#if !defined(AFX_SERVER_H__84ECCD22_1C10_11D2_8D11_006008CD4894__INCLUDED_)
#define AFX_SERVER_H__84ECCD22_1C10_11D2_8D11_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Server.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CServer command target

class CServer : public CAsyncSocket
{
// Attributes
private:
	int		m_Port;	// port to listen on

// Operations
public:
	void StartListening();
	CServer();
	virtual ~CServer();

protected:	
	virtual  CString	GetErrorText(int);
	virtual	void		ReadParameters();
	virtual	void		PostError(int Error)															{ }
	virtual	void		ProcessError(const CString& Source, const CString& Error)		{ }
	virtual	void		ProcessLogEntry(const CString& Source, const CString& Entry)	{ }
	
// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServer)
	public:
	virtual void OnAccept(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CServer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVER_H__84ECCD22_1C10_11D2_8D11_006008CD4894__INCLUDED_)
