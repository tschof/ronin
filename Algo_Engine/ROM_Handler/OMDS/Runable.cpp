// Runable.cpp: implementation of the CRunable class.
//
//////////////////////////////////////////////////////////////////////
//
//	CRunnable is a template class
//	the primary method is start() which will create a thread and
//	then execute the run() method.
//	objects that require a single thread extend this object
//

#include "stdafx.h"
#pragma warning(disable:4786)
#pragma warning(disable:4503)

#include "Runable.h"
#include "Log.h"
#include  <process.h>

//
//	This is the constant global method that will execute the run method of the child object
unsigned WINAPI Runit(CRunable * pRunable)
{
	try
	{
		pRunable->run();
	}
	catch(...)
	{
		CLog log;
		log.write(LOG_ERROR,"FAILURE CRunnable WINAPI Runit pRunable->run() failed.");
	}
	return(1L);
}


unsigned WINAPI CRunable::ThreadProc( LPVOID a_pParameter )
{
	CRunable* pRunable = reinterpret_cast<CRunable*>( a_pParameter );

	if( pRunable )
	{
		try
		{
			pRunable->run();
		}
		catch(...)
		{
			CLog log;
			log.write(LOG_ERROR, (std::string)"FAILURE CRunable::ThreadProc()... pRunable->run() failed.");
		}
	}

	return 0;
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRunable::CRunable()
{
	m_hThread = NULL;
	m_threadId = 0;
	m_hKill = CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hKilled = CreateEvent( NULL, FALSE, FALSE, NULL );
}

CRunable::~CRunable()
{
	CloseHandle( m_hKill );
	CloseHandle( m_hKilled );
}

HANDLE CRunable::start()
{
	LPDWORD	exitCode = 0;

	if (m_hThread != NULL)
	{
		try
		{
			GetExitCodeThread(m_hThread, exitCode);
			if (*exitCode == STILL_ACTIVE)
			{
				// The thread is running already
				return(m_hThread);
			}
		}
		catch(...)
		{
			throw exception("Unkown Error"); // added throw
		}
	}

	try
	{
		SECURITY_ATTRIBUTES l_SecurityAttributes;
		l_SecurityAttributes.nLength = sizeof( SECURITY_ATTRIBUTES );
		l_SecurityAttributes.lpSecurityDescriptor = NULL;
		l_SecurityAttributes.bInheritHandle = FALSE;

		unsigned threadId;

		m_hThread = (void*) ::_beginthreadex(
			&l_SecurityAttributes,
			0,
			&ThreadProc,
			this,
			CREATE_SUSPENDED,
			&threadId
			);
		if( !m_hThread )
			throw exception("Unkown Error");
		// I copy the threadId from a unsigned to a DWORD to get rid of
		// the compiler error and avoid a reinterpret_cast -- deviant
		m_threadId = threadId;
		if( -1 == ResumeThread( m_hThread ) )
			throw exception("Unkown Error");
	}
	catch(...)
	{
		throw exception("Unkown Error");
	}

	// sleep 200 miliseconds to allow the child thread to run
	::Sleep(200);
	return m_hThread;
}

DWORD CRunable::getThreadId()
{
	return(m_threadId);
}

void CRunable::shutDown()
{
	BOOL bShutdown = SetEvent( m_hKill );
	WaitForSingleObject( m_hKilled, 5000 );
	CloseHandle( m_hThread );
	m_hThread = NULL;
}

bool CRunable::isRunning()
{
	// LPDWORD	exitCode;
	unsigned long	exitCode;
	bool			running = false;

	if (m_hThread != NULL)
	{
		try
		{
			GetExitCodeThread(m_hThread, &exitCode);
			running = (exitCode == STILL_ACTIVE);
		}
		catch(...)
		{
			throw exception("Unkown Error"); // added throw
		}
	}

	return(running);
}

void CRunable::run()
{

}

bool CRunable::isCorrectThread()
{
	DWORD dwCurrThreadId = GetCurrentThreadId();
	return(getThreadId() == dwCurrThreadId);
}

HANDLE CRunable::getThread()
{
	return(m_hThread);
}

void CRunable::onShutdown(HANDLE thead)
{

}
