#ifndef _OATSTHREADMANAGER_H__
#define _OATSTHREADMANAGER_H__

#include "DbInsertThread.h"
#include "RomCsv.h"
#include <list>
#include <vector>
#include <iomanip>
#include <time.h>
#include "Logger.h"
#include "Locker.h"

class COatsThreadManager
{
public:
	COatsThreadManager(int nThreads);
	~COatsThreadManager(void);

	bool				AddRequest(CRomCsv* pMsg );
	//void				AddRequests(std::list<CRomCsv*>& requests);
	
	void				Suspend();
	void				Resume();
	void				ShutDown( int nTimeOut );
	long				GetProcessedCnt();
	long				GetInsertedCnt();

private:
	HRESULT DeleteMSQueues(std::vector<std::string>& qNames);
	HRESULT OpenMSQueues(std::vector<std::string>& qNames);
	int COatsThreadManager::GetQueueIndexFromRomTag(const std::string& romTag);

	std::vector<CDbInsertThread*> m_Threads;
	int							  m_nThreads;

	Lock						  m_Lock;
	bool						  m_bShutdown;

	std::vector<CComQIPtr<IMSMQQueue, &IID_IMSMQQueue>> m_ipSendQueues;
};
#endif //_OATSTHREADMANAGER_H__