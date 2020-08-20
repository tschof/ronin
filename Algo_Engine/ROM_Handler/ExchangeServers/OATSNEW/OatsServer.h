#if !defined(AFX_OATSSERVER_H__88637275_EB53_4AF9_A454_44845547A926__INCLUDED_)
#define AFX_OATSSERVER_H__88637275_EB53_4AF9_A454_44845547A926__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OatsServer.h : header file
//
#include <vector>
#include "Csv.h"

/////////////////////////////////////////////////////////////////////////////
// COatsServer command target
class COmSession;

class COatsServer : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	COatsServer();
	virtual ~COatsServer();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COatsServer)
	public:
	virtual void OnAccept(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(COatsServer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
	//bool IsConnected();
	void HeartBeat();

	void Send(CCsv& szMsg);
	int OMCount();
	void Remove( COmSession * pSession );
	void GetRomInstances(CString& roms);
	static int COatsServer::GetOMInstanceIdFromOMTag(CString omTag);

protected:
	//COmSession * m_pSession;
	typedef std::vector<COmSession*> OMSessions;
	OMSessions m_pSessions;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OATSSERVER_H__88637275_EB53_4AF9_A454_44845547A926__INCLUDED_)
