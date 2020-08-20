// ThreadTibProcessor.cpp: implementation of the CThreadTibProcessor class.
//
//////////////////////////////////////////////////////////////////////
//
//	this object extends CRunnable
//	simply to start tib worker threads
//

#include "stdafx.h"

#pragma warning(disable:4786)
#pragma warning(disable:4503)

#include "ThreadTibProcessor.h"
#include <process.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThreadTibProcessor::CThreadTibProcessor(CMessageProcessor * pMP)
{
	m_pMessageProcessor = pMP;
	m_log.setObjectName("CThreadTibProcessor");
}

CThreadTibProcessor::~CThreadTibProcessor()
{
}

void CThreadTibProcessor::run()
{
	try
	{
		while( ::WaitForSingleObject( m_hKill, 0 ) != WAIT_OBJECT_0 )
		{
			if( m_pMessageProcessor->TibWait() != WAIT_TIMEOUT )
				m_pMessageProcessor->processTibMessage();
		}
		SetEvent( m_hKilled );
		_endthread();
	}
	catch (...)
	{
		m_log.write("CThreadTibProcessor::run() failed");
	}
}
