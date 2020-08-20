#ifndef _DBINSERTTHREAD_H__
#define _DBINSERTTHREAD_H__

#include "stdafx.h"
#include <atlbase.h>
#include <comutil.h>
#include <mqoai.h>
#include <string>
#include "Thread.h"
#include "RomCsv.h"
#include "OrderMsg.h"
#include "Count.h"

class CDbInsertThread : public Thread
{
public:
	CDbInsertThread(const std::string& qName);
	~CDbInsertThread(void);

	virtual void Run();
	long GetProcessedCnt();
	long GetInsertedCnt();

private:
	CComQIPtr<IMSMQQueue, &IID_IMSMQQueue> m_ipReceiveQueue;
	CCount m_ProcessedCnt;
	CCount m_InsertedCnt;
};

#endif //_DBINSERTTHREAD_H__
