#include "OatsThreadManager.h"
#include "Config.h"

HRESULT COatsThreadManager::OpenMSQueues(std::vector<std::string>& qNames)
{
	int size = qNames.size();
	std::string errStr = "";
	m_ipSendQueues.resize(size);

	std::vector<CComQIPtr<IMSMQQueueInfo, &IID_IMSMQQueueInfo>> ipQueueInfos;
	ipQueueInfos.resize(size);
	HRESULT hr = S_OK;

	for (int i = 0; i < size; i++)
	{
		hr = CoCreateInstance( CLSID_MSMQQueueInfo,
							   NULL,
							   CLSCTX_SERVER,
							   IID_IMSMQQueueInfo,
							   (void**)(&(ipQueueInfos[i].p)));
		if(hr != S_OK)
		{
			errStr = "CoCreateInstance failed for CLSID_MSMQQueueInfo\r\n";
		}
		else
		{
			hr = ipQueueInfos[i]->put_PathName(_bstr_t(qNames[i].c_str()));
			if(hr != S_OK)
			{
				errStr = "put_PathName failed\r\n";
			}
			else
			{
				hr = ipQueueInfos[i]->Open(MQ_SEND_ACCESS, MQ_DENY_NONE, &m_ipSendQueues[i]);
				
				if(hr != S_OK)
				{
					VARIANT vtFalse;
					VariantInit(&vtFalse);
					vtFalse.vt = VT_BOOL;
					vtFalse.boolVal = FALSE;
					
					hr = ipQueueInfos[i]->Create(&vtFalse, &vtFalse);
					if( hr != S_OK )
					{
						std::stringstream stm;
						stm << "MSMQ Create queue "
							<< qNames[i]
							<< " failed\r\n";
						errStr = stm.str();
					}
					else
					{
						hr = ipQueueInfos[i]->Open(MQ_SEND_ACCESS, MQ_DENY_NONE, &m_ipSendQueues[i]);
						if (hr != S_OK)
						{
							std::stringstream stm;
							stm << "MSMQ queue "
								<< qNames[i]
								<< " Open for writting failed\r\n";
							errStr = stm.str();
						}
						else
						{
							std::stringstream stm;
							stm << "MSMQ queue "
								<< qNames[i]
								<< " Open for writting Succeeded\r\n";
							errStr = stm.str();
						}
					}
				}
				else
				{
					std::stringstream stm;
					stm << "MSMQ queue "
						<< qNames[i]
						<< " Open for writting Succeeded\r\n";
					errStr = stm.str();
				}
			}
		}

		std::cerr << errStr;
		if (hr != S_OK)
		{
			Logger::getInstance().LogError(errStr);
			std::string subject = "NewOats start failed";
			(CConfig::getInstance()).SendEmail(subject, errStr);
			break;
		}
		else
		{
			Logger::getInstance().LogMessage(errStr);
		}
	}

	return hr;
}


HRESULT COatsThreadManager::DeleteMSQueues(std::vector<std::string>& qNames)
{
	int size = qNames.size();

	m_ipSendQueues.resize(size);

	std::vector<CComQIPtr<IMSMQQueueInfo, &IID_IMSMQQueueInfo>> ipQueueInfos;
	ipQueueInfos.resize(size);
	HRESULT hr = S_OK;

	for (int i = 0; i < size; i++)
	{
		hr = CoCreateInstance( CLSID_MSMQQueueInfo,
							   NULL,
							   CLSCTX_SERVER,
							   IID_IMSMQQueueInfo,
							   (void**)(&(ipQueueInfos[i].p)));
		if(hr != S_OK)
		{
			return hr;
		}

		hr = ipQueueInfos[i]->put_PathName(_bstr_t(qNames[i].c_str()));
		if(hr != S_OK)
		{
			return hr;
		}

		try
		{
			hr = ipQueueInfos[i]->Delete();
		}
		catch(...)
		{
			Logger::getInstance().LogError("Delete Queues error\r\n");
			hr = S_FALSE;
			break;
		}
	}

	return hr;
}

COatsThreadManager::COatsThreadManager(int nThreads)
{
	m_nThreads = nThreads;
	m_Threads.resize(m_nThreads);
	m_bShutdown = false;
	std::vector<std::string> qNames;
	qNames.resize(nThreads);
	
	int i = 0;
	
	for (i = 0 ; i < m_nThreads; i++)
	{
		std::stringstream stm;
		//stm << "DIRECT=TCP:10.103.102.28\\private$\\OatsQ" 
		stm << ".\\private$\\" 
			<< (CConfig::getInstance()).MSMSQBaseName()
			<< '_'
			<< i;
		qNames[i] = stm.str();
	}

	HRESULT hr = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);

	hr = OpenMSQueues(qNames);
	if (S_OK == hr)
	{
		for( i = 0; i < m_nThreads; i++ )
		{
			m_Threads[i] = new CDbInsertThread(qNames[i]);
			m_Threads[i]->Start();
		}
	}
}


COatsThreadManager::~COatsThreadManager(void)
{
	for (int i = 0; i < m_nThreads; i++)
	{
		m_ipSendQueues[i]->Close();
	}

	ShutDown(3000);
	CoUninitialize();
}

void COatsThreadManager::ShutDown( int nTimeOut )
{
	for( int i = 0; i < m_Threads.size(); i++ )
	{
		m_Threads[i]->Stop(nTimeOut);
		delete m_Threads[i];
	}
	m_Threads.clear();
	m_bShutdown = true;
}

void COatsThreadManager::Suspend()
{
	if( m_bShutdown )
	{
		return;
	}

	Locker locker( &m_Lock );
	for( int i = 0; i < m_Threads.size(); i++ )
	{
		m_Threads[i]->Suspend();
	}
}

void COatsThreadManager::Resume()
{
	if( m_bShutdown )
	{
		return;
	}

	Locker locker( &m_Lock );
	for( int i = 0; i < m_Threads.size(); i++ )
	{
		m_Threads[i]->Resume();
	}
}

int COatsThreadManager::GetQueueIndexFromRomTag(const std::string& romTag)
{
	int nPos = romTag.find_last_of('-');
	if( nPos == std::string::npos) 
	{
		return 0;
	}

	int len = romTag.length() - nPos - 1;

	std::string substr = romTag.substr(nPos + 1, len);
	__int64 stmp = _atoi64(substr.c_str());

	return stmp % m_nThreads;
}

bool COatsThreadManager::AddRequest(CRomCsv* pMsg)
{
	bool rc = true;
	HRESULT hr = S_OK;
	int index = GetQueueIndexFromRomTag(pMsg->GetAt(RFIX::CSV_STAFFORD_TAG));

	CComQIPtr<IMSMQMessage, &IID_IMSMQMessage> ipMessage;
	hr =	CoCreateInstance( CLSID_MSMQMessage,
							  NULL,
							  CLSCTX_SERVER,
							  IID_IMSMQMessage,
							  (void**)(&ipMessage.p));
	if(hr != S_OK)
	{
		//m_ipSendQueues[index]->Close();
		rc = false;
	}
	else
	{
	   // compose message to send
		VARIANT vtSendText;
		VariantInit(&vtSendText);
		vtSendText.bstrVal = SysAllocString(_bstr_t((pMsg->Message()).c_str()));
		vtSendText.vt = VT_BSTR;
		//ipMessage->put_Delivery(MQMSG_DELIVERY_RECOVERABLE);

		if ((hr=ipMessage->put_Body(vtSendText) != S_OK) ||
			((hr=ipMessage->Send(m_ipSendQueues[index], NULL)) != S_OK))
		{
			//m_ipSendQueues[index]->Close();
			static int cnt = 0;
			SysFreeString(vtSendText.bstrVal);
			std::string errStr = "MSMQ send failed\r\n";

			Logger::getInstance().LogError(errStr.c_str());

			if (cnt < 5)
			{
				cnt++;
				std::string subject = "Oats Warning";
				(CConfig::getInstance()).SendEmail(subject, errStr);
			}

			rc = false;
		}

		SysFreeString(vtSendText.bstrVal);
	}

	return rc;
}
/*
void COatsThreadManager::AddRequests(std::list<CRomCsv*>& requests)
{
	CRomCsv* pMsg = NULL;
	while (requests.empty() == false)
	{
		pMsg = requests.front();
		AddRequest(pMsg);
		delete pMsg;
		requests.pop_front();
	}
}
*/

long COatsThreadManager::GetProcessedCnt()
{
	long rc = 0;

	for (int i = 0; i < m_nThreads; i++)
	{
		rc += m_Threads[i]->GetProcessedCnt();
	}

	return rc;
}

long COatsThreadManager::GetInsertedCnt()
{
	long rc = 0;

	for (int i = 0; i < m_nThreads; i++)
	{
		rc += m_Threads[i]->GetInsertedCnt();
	}

	return rc;
}