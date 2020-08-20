// ThreadClientProcessor.cpp: implementation of the CThreadClientProcessor class.
//
//////////////////////////////////////////////////////////////////////
//
//	This object extends runnable and exists
//	simply starts a worker client processor thread
//

#include "stdafx.h"

#pragma warning(disable:4786)
#pragma warning(disable:4503)

#include "ThreadClientProcessor.h"
#include <process.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThreadClientProcessor::CThreadClientProcessor(CMessageProcessor * pMP)
{
	m_pMessageProcessor = pMP;
	m_log.setObjectName("CThreadClientProcessor");
}

CThreadClientProcessor::~CThreadClientProcessor()
{
}

void CThreadClientProcessor::run()
{
	try
	{
		while( WaitForSingleObject( m_hKill, 0 ) != WAIT_OBJECT_0 )
		{
			if( m_pMessageProcessor->ClientWait() != WAIT_TIMEOUT )
				m_pMessageProcessor->processClientRequest();
		}
		SetEvent( m_hKilled );
		_endthread();
	}
	catch (...)
	{
		m_log.write("CThreadClientProcessor::run() failed");
	}
}
