// OatsCallback.h: interface for the COatsCallback class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OATSCALLBACK_H__8A627930_2B35_11D3_A8D7_006008CD4894__INCLUDED_)
#define AFX_OATSCALLBACK_H__8A627930_2B35_11D3_A8D7_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#pragma warning(disable:4786)

#include "Csv.h"
#include "ZLog.h"
#include "OatsServer.h"
#include "AdminServer.h"
#include "Count.h"
#include "AdminThread.h"

#include <string>
#include <map>
#include <deque>
#include <set>



class ThreadPoolManager;
class CQueueRequest;
class COatsDlg;

extern COatsDlg *theDlg;

class COatsCallback  
{
public:
	COatsCallback();

	virtual ~COatsCallback();

	bool StartServer();
	virtual void onData( CString & szData );
	void WriteCSV(CCsv* pCsv);
	void PauseDB();
	void ResumeDB();

	long GetReceived();
	void IncrementReceived();
	long GetDatabased();
//	long GetAdminCount();
//	void AdminSend(CString& msg);

	void LoadMap();
	void ReloadMap();

//	string FileName() { return m_Log->FileName(); }

	void SendToOM(CCsv & szMessage );
	//void SendToAdmin(CString & szMessage );
	void UpdateRunDate(string datFile);

	void QueueDBMessage(CCsv& msg);
	//bool QueueAdminMessage(CQueueRequest* pMsg);
	bool QueueAdminMessage(CString& msg);
	void ProcessAdminRequest(CCsv* pRequest);
	void RomInstanceUpdate(int adminID = -1);

private:
	std::map < std::string, std::deque<CCsv*>, std::less<std::string> > m_StatusMap;
	ZLog *m_eLog;
	ZLog *m_fLog;
	ZLog *m_oLog;
	ZLog *m_cLog;

	char m_szEnable[32], m_szServer[32], m_szUser[32], m_szPass[32], m_szDB[32];
	int	m_nLevel;
	long m_nCount;
	CCount m_Count;

	ThreadPoolManager *		m_pMgr;
	ThreadPoolManager *		m_pSlaveMgr;
	CAdminThread *			m_pAdminMgr;
	long m_nSize;
	std::set<std::string>	m_Users;	

	bool QueueMessage( CQueueRequest * pMsg );
	bool QueueDBMessage(CQueueRequest* pMsg);
	//bool QueueAdminMessage(CQueueRequest* pMsg);
	
public:
	COatsServer				m_OMServer;
//	CAdminServer			m_AdminServer;
	static int m_nThreads;
};

#endif // !defined(AFX_OATSCALLBACK_H__8A627930_2B35_11D3_A8D7_006008CD4894__INCLUDED_)
