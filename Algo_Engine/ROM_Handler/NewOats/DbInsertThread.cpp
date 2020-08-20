#include "DbInsertThread.h"
#include <iostream>
#include "Logger.h"
#include "Config.h"
#include "DBConnection.h"

CDbInsertThread::CDbInsertThread(const std::string& qName)
{
	std::string errStr = "";

	CComQIPtr<IMSMQQueueInfo, &IID_IMSMQQueueInfo> ipQueueInfo;
	HRESULT hr = CoCreateInstance(	CLSID_MSMQQueueInfo,
									NULL,
									CLSCTX_SERVER,
									IID_IMSMQQueueInfo,
									(void**)(&ipQueueInfo.p));
	if(hr != S_OK)
	{
		errStr = "CoCreateInstance failed for CLSID_MSMQQueueInfo\r\n";
	}
	else
	{
		hr = ipQueueInfo->put_PathName(_bstr_t(qName.c_str()));
		if(hr != S_OK)
		{
			errStr = "put_PathName failed\r\n";
		}
		else
		{
			hr = ipQueueInfo->Open(MQ_RECEIVE_ACCESS, MQ_DENY_NONE, &m_ipReceiveQueue);
			if(hr != S_OK)
			{
				std::stringstream stm;
				stm << "MSMQ queue "
					<< qName
					<< " open for reading failed\r\n";
				errStr = stm.str();
			}
			else
			{
				std::stringstream stm;
				stm << "MSMQ queue "
					<< qName
					<< " open for reading succeeded\r\n";
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
	}
	else
	{
		Logger::getInstance().LogMessage(errStr);
	}
}

CDbInsertThread::~CDbInsertThread(void)
{
}

void CDbInsertThread::Run()
{
	CDBConnection* pConn = NULL;
	
	while (!pConn)
	{
		try
		{
			pConn = new CDBConnection();
		}
		catch(...)
		{
			Logger::getInstance().LogError("CDBConnection() exception");
			pConn = NULL;
			Sleep(10000);
		}
	}

	while( WaitForSingleObject( m_hKill, 0 ) != WAIT_OBJECT_0 )
	{
		CComQIPtr<IMSMQMessage, &IID_IMSMQMessage> ipMessage;
		 _variant_t       bvtWantBody((bool)true);      
	
		HRESULT hr = m_ipReceiveQueue->Receive(&vtMissing, &vtMissing, &bvtWantBody, &_variant_t(3000), &ipMessage);
		
		if (ipMessage != NULL)
		{
			_bstr_t szMessage;
			_variant_t vtValue;
			hr = ipMessage->get_Body(&vtValue);

			if(S_OK == hr &&
			   VT_BSTR == vtValue.vt)
			{
				szMessage = vtValue.bstrVal;
				std::string msg = (const char *)szMessage;
				CRomCsv csv( msg  );
				char cmd = csv.cGet(RFIX::CSV_COMMAND);
				if ('E' == cmd ||
					'S' == cmd) //skip corrupted messages
				{
					COrderMsg order(&csv);
					if (pConn->InsertMessage(&order))
					{
						m_InsertedCnt.Increment(1);
					}
					m_ProcessedCnt.Increment(1);
				}
			}
		}
	}
    m_ipReceiveQueue->Close();
	SetEvent( m_hKilled );
}

long CDbInsertThread::GetProcessedCnt()
{
	return m_ProcessedCnt.Value();
}

long CDbInsertThread::GetInsertedCnt()
{
	return m_InsertedCnt.Value();
}
