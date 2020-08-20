#if !defined(AFX_CLIENTCONNECTION_H__84ECCD23_1C10_11D2_8D11_006008CD4894__INCLUDED_)
#define AFX_CLIENTCONNECTION_H__84ECCD23_1C10_11D2_8D11_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ClientConnection.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CClientConnection command target

class CClientConnection : public CConnection
{
// Attributes
private:
	void ProcessMessage(CString& Message);

// Operations
public:

	CClientConnection()	:
		CConnection()
		{ 
		}
	virtual ~CClientConnection()	{ }

public:
	void	ProcessStatus(const CString& Message);
	void	ProcessError(const CString& Error);
	void	ProcessLogEntry(const CString& Entry);

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientConnection)
	public:
	void OnClose(int nErrorCode);
	void OnConnect(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CClientConnection)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTCONNECTION_H__84ECCD23_1C10_11D2_8D11_006008CD4894__INCLUDED_)
