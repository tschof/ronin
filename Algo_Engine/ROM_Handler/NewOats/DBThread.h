#ifndef _DBTHREAD_H__
#define _DBTHREAD_H__

//#include "ThreadPoolManager.h"
#include "OatsThreadManager.h"
#include "MessageBuffer.h"
#include "OrderMsg.h"
#include "Count.h"
#include <list>

#pragma once

class CDBThread : public Thread
{
public:
	CDBThread(void);
	~CDBThread(void);

	virtual void	Run();
	
	//void AddMessages(std::list<CRomCsv*>& msgs);
	void CheckDumpRequests();
	void AddMessages(const std::string& msgs);
	long GetReceivedCnt() { return m_rvdOatsMsgCount.Value(); }
	long GetQueuedCnt();
	long GetProcessedCnt();
	long GetInsertedCnt();
	void StartRecovering();
	bool FinishedRecovering();

private:
	void ProcessSMessages(); //string messages
	//void ProcessOMessages(); //object messages
	void ProcessRecover(CRomCsv& csv);
	bool IsWithinTimeRange(std::string& timeStr, long beginTime, long endTime);

	bool AddRequest(std::string* pRq);
	bool DumpRequests();

	//CMessageBuffer<CRomCsv> m_MsgBuffer;
	CStringBuffer m_sMsgBUffer;
	std::list<std::string*> m_rqList;

	HANDLE m_Events[3];
	COatsThreadManager* m_pMgr;
	CCount m_rvdOatsMsgCount;
	CCount m_recoverFlag;
};

#endif //_DBTHREAD_H__
