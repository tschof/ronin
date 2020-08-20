#include "stdafx.h"
#include "DBThread.h"
#include "Config.h"
#include "OatsServer.h"
#include "RomValueDefs.h"
#include "RomCsv.h"
#include "OrderMsg.h"

CDBThread::CDBThread(void)
{
	m_Events[0] = Thread::Kill();
	m_Events[1] = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_Events[2] = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_pMgr = NULL;
}

CDBThread::~CDBThread(void)
{
	if (m_pMgr)
	{
		delete m_pMgr;
	}
}


void CDBThread::Run()
{
	int nThreads = (CConfig::getInstance()).numDBThreads();

	m_pMgr = new COatsThreadManager(nThreads);

	while (1)
	{
		DWORD waitResult = WaitForMultipleObjects(3, m_Events, FALSE, INFINITE);

		switch (waitResult)
		{
		case WSA_WAIT_TIMEOUT:
			break;
		default:
			{
				int index = waitResult - - WSA_WAIT_EVENT_0;
				switch(index)
				{
				case 0:
					{
						//shut down those db slaves
						delete m_pMgr;
						break;
					}
				case 1:
					{
						ProcessSMessages();
						break;
					}
				case 2:
					{
						DumpRequests();
						break;
					}
				}
			}
		}
	}
}

void CDBThread::CheckDumpRequests()
{
	::SetEvent(m_Events[2]);
}

void CDBThread::AddMessages(const std::string& msgs)
{
	m_sMsgBUffer.AddBuffer(msgs);
	::SetEvent(m_Events[1]);
}

bool CDBThread::IsWithinTimeRange(std::string& timeStr, long beginTime, long endTime)
{
	std::string h = timeStr.substr(0, 2);
	std::string m = timeStr.substr(3, 2);
	std::string s = timeStr.substr(6, 2);
	
	long time = 3600 * atol(h.c_str()) + 60 * atol(m.c_str()) + atol(s.c_str());

	if (time <= endTime &&
		time >= beginTime)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CDBThread::ProcessRecover(CRomCsv& csv)
{
	long beginTime = csv.nGet(RFIX::CSV_START_TIME);
	long endTime = csv.nGet(RFIX::CSV_END_TIME);
	std::string fileName = csv.Get(RFIX::CSV_TEXT);

	FILE* fp = fopen(fileName.c_str(), "r");
	if (fp != NULL)
	{
		char buffer[2048];
		std::string l_msg, l_timeStr;
		std::string* pMsg = NULL;
		
		while (fgets(buffer, 2048, fp) != NULL)
		{
			l_msg = buffer;
			
			if (l_msg.size() == 1 ||
				l_msg.size() == 0) //in case recover file has empty lines
			{
				continue;
			}

			if (buffer[0] == ROMVALUE::COMMAND_ORDER ||
				buffer[0] == ROMVALUE::COMMAND_STATUS)
			{
				l_timeStr = l_msg.substr(11, 8);
			}
			else if (buffer[0] == ROMVALUE::COMMAND_LOGON)
			{
				continue;
			}
			else
			{
				l_timeStr = l_msg.substr(11, 8);
				l_msg = l_msg.substr(20);
			}

			if (IsWithinTimeRange(l_timeStr, beginTime, endTime) == true)
			{
				m_rvdOatsMsgCount.Increment(1);
				pMsg = new std::string(l_msg);
				if (AddRequest(pMsg) == false &&
					m_rqList.size() > 500000)
				{
					Sleep(10000);
				}
			}
		}

		std::cerr << "Finished loading recover file" << std::endl;
		fclose(fp);

		while (m_rvdOatsMsgCount.Value() > m_pMgr->GetProcessedCnt())
		{
			Sleep(500);
			DumpRequests();
		}

		m_recoverFlag.Decrement(1);
	}
}

bool CDBThread::AddRequest(std::string* pRq)
{
	m_rqList.push_back(pRq);
	return DumpRequests();
}

bool CDBThread::DumpRequests()
{
	bool rc = true;

	while (m_rqList.empty() == false)
	{
		std::string* pRq = m_rqList.front();
		m_rqList.pop_front();

		CRomCsv msg(*pRq);
		delete pRq;
		if (m_pMgr->AddRequest(&msg) == false)
		{
			std::string* pMsg = new std::string(msg.Message());
			m_rqList.push_front(pMsg);
			rc = false;
			break;
		}
	}

	return rc;
}

void CDBThread::ProcessSMessages()
{
	std::string msgs;
	m_sMsgBUffer.RetriveBuffer(msgs);

	int index = msgs.find(MESSAGE_END);
	int currOff = 0, len = 0;
	std::string msg;
	std::string* pMsg = NULL;

	while (index != std::string::npos)
	{
		len = index - currOff;
		msg = msgs.substr(currOff, len);
		currOff = index + 1;

		if (ROMVALUE::COMMAND_ORDER == msg[0] ||
			ROMVALUE::COMMAND_STATUS == msg[0])
		{
			m_rvdOatsMsgCount.Increment(1);
			pMsg = new std::string(msg);
			AddRequest(pMsg);
		}
		else if (ROMVALUE::COMMAND_ADMIN == msg[0])
		{
			CRomCsv csv(msg);
			if (csv.nGet(RFIX::CSV_COMMAND_FREAK) == ROMVALUE::OATS_REVOVER)
			{
				ProcessRecover(csv);
			}
		}

		index = msgs.find(MESSAGE_END, currOff);
	}
}

long CDBThread::GetQueuedCnt()
{
	long rc = 0;
	if (m_pMgr != NULL)
	{
		rc = GetReceivedCnt() - m_pMgr->GetProcessedCnt();
	}
	return rc;
}

long CDBThread::GetProcessedCnt()
{
	if (m_pMgr)
	{
		return m_pMgr->GetProcessedCnt();
	}
	else 
	{
		return 0;
	}
}

long CDBThread::GetInsertedCnt()
{
	if (m_pMgr)
	{
		return m_pMgr->GetInsertedCnt();
	}
	else 
	{
		return 0;
	}
}


void CDBThread::StartRecovering()
{
	m_recoverFlag.Increment(1);
}

bool CDBThread::FinishedRecovering()
{
	bool rc = false;

	if (m_recoverFlag.Value() == 0)
	{
		rc = true;
	}
	
	return rc;
}

