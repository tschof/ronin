// MessageProcessor.cpp: implementation of the CMessageProcessor class.
//
//////////////////////////////////////////////////////////////////////
//
//	There are two main methods
//	processClientRequest()
//	processsTibMessage()
//
//	All requests must start with a request for the option series
//	The call for the option series returns the information
//	required to construct tib requests
//	on receipt of a tib option series, the clients original request
//	must be checked to see which subject must be requested.
//
#include "stdafx.h"

#pragma warning(disable:4786)
#pragma warning(disable:4503)

#include "MessageProcessor.h"
#include "Main.h"
#include "Locker.h"
#include "ConTib.h"
#include "ConClientMessage.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMessageProcessor::CMessageProcessor(CDataManager * pDM, CConnections * pC)
{
	m_pDataManager = pDM;
	m_pConnections = pC;
	m_pDATib = new CDATib(m_pDataManager);
	m_log.setObjectName("CMessageProcessor");

	m_pRequestManager = new RequestManager(m_pConnections, pDM);
	m_TibSemaphore = ::CreateSemaphore(NULL, 0, 0x7fffffff, NULL);
	m_ClientSemaphore = ::CreateSemaphore(NULL, 0, 0x7fffffff, NULL);
}

CMessageProcessor::~CMessageProcessor()
{
	delete m_pDATib;
	m_pDATib = NULL; // set to null

	CloseHandle( m_TibSemaphore );
	m_TibSemaphore = NULL;

	CloseHandle( m_ClientSemaphore );
	m_ClientSemaphore = NULL;
	
	delete m_pRequestManager;
}

void CMessageProcessor::processClientRequest()
{
	CDClientRequest *	pCR = NULL;
	try
	{
		CConClientMessage conClientMessage = m_pConnections->getNextClientMessage();
		if (!conClientMessage.message.compare("ClientClosed"))
			return;
		m_pRequestManager->addRequest( conClientMessage );
	}
	catch(...)
	{
		m_log.write(LOG_ERROR,"CMessageProcessor::processClientRequest() Unknown exception.");
	}
}


void CMessageProcessor::processTibMessage()
{
	CDTibMessage *		pTM = NULL;
	std::string			subject;
	int					type;
	try
	{
		pTM = m_pConnections->getNextTibMessage();
	}
	catch(...)
	{
		m_log.write( LOG_ERROR, "CMessageProcessor::processTibMessage() getNextTibMessage exception");
		return;
	}
	if (pTM == NULL)
	{
		m_log.write( LOG_ERROR, "CMessageProcessor::processTibMessage() pTM = NULL");
		return;
	}
	subject = pTM->getSubject();
	type = pTM->getType();
	if (subject.size() == 0)
	{
		m_log.write(LOG_WARNING, "Empty Subject in processTibMessage");
		delete pTM;
		return;
	}
	try
	{
		if( pTM->getRetry() < 10 )
			pTM->Process();
		else
		{
			m_log.write(LOG_WARNING, "Max retry reached on " + subject);
		}
	}
	catch(CMaskNotFoundException *pEx )
	{
		delete pEx;
		// retry mechanism
		m_log.write(LOG_DEBUG, "CMaskNotFoundException on " + subject);
		pTM->setRetry( pTM->getRetry()+1 );
		m_pConnections->getConTib()->addMessage( pTM );
		return;
	}
	catch(...)
	{
		delete pTM;
		m_log.write( LOG_ERROR, "CMessageProcessor::processTibMessage() unhandled exception on " + subject );
		return;
	}

	try
	{
		delete pTM;
	}
	catch(...)
	{
		m_log.write( LOG_ERROR, "CMessageProcessor::processTibMessage() delete pTM unhandled exception on "+ subject );
	}

}

CDATib * CMessageProcessor::getDataAdapterTib()
{
	return m_pDATib;
}

ULONG CMessageProcessor::TibWait()
{
	return ::WaitForSingleObject( m_TibSemaphore, 3000 );
}


void CMessageProcessor::TibRelease()
{
	::ReleaseSemaphore( m_TibSemaphore, 1, NULL );
}


ULONG CMessageProcessor::ClientWait()
{
	return ::WaitForSingleObject( m_ClientSemaphore, 3000 );
}


void CMessageProcessor::ClientRelease()
{
	::ReleaseSemaphore( m_ClientSemaphore, 1, NULL );
}


