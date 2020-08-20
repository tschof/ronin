#if !defined(AFX_SESSIONBASE_H__E8D72ECA_67FC_428E_A889_70356908D42E__INCLUDED_)
#define AFX_SESSIONBASE_H__E8D72ECA_67FC_428E_A889_70356908D42E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SessionBase.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSessionBase command target
#include <list>

using namespace std;

typedef list<CString*> msgList;
typedef msgList::iterator msgListIT;

class COatsCallback;
static const int RECV_BUFF_SIZE = 4096;

class CSessionBase : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	CSessionBase( COatsCallback * pDoc, char cTerm );
	virtual ~CSessionBase();

// Overrides
public:
	virtual int		Send		( const CString & szMsg);
	virtual int		Send		( const void* lpBuf, int nBufLen, int nFlags = 0 );
	virtual void	OnSend		( int nErrorCode ); 
	virtual void	OnReceive	( int nErrorCode ); 

	virtual void	ProcessTimer() {}

	virtual void	OnMessage	( CString & szMsg ) = 0;
	virtual void	OnMessage	( byte * msg, int nSize ) {}

	COatsCallback *	GetDoc		() { return m_pDoc; }

	void			SetConnected( bool bConnect ) { m_bConnected = bConnect; }
	void			SetID(int value) { m_ID = value; }
	int				ID() { return m_ID; }

	void SendHeartBeat();
	int				Send1(const CString& szMsg);


	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSessionBase)
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CSessionBase)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
	char					m_cTerm;

	COatsCallback *			m_pDoc;
	CString					m_szLeft;
	//CList<CString,CString>	m_SendQueue;
	msgList					m_SendQueue;
	
	bool					m_bConnected;
	CTime					m_LastSendTime;	
	CTime					m_LastRcvTime;
	int						m_ID;

	bool					DumpSendQueue();
	bool					m_ReadyToSend;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SESSIONBASE_H__E8D72ECA_67FC_428E_A889_70356908D42E__INCLUDED_)
