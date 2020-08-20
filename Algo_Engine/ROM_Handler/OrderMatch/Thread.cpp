#include "Thread.h"
#include "atlbase.h"
#include <process.h>

namespace MultiThreading
{
	Thread::BlockingCall::BlockingCall()
	{
	}

	Thread::BlockingCall::~BlockingCall()
	{
	}

	Thread::Thread( bool autoDelete )
	:	m_pBlockingCall( 0 ),
		m_ThreadID( 0 ),
		m_hThread( 0 ),
		m_IsRunningEvent( true, false ),
		m_IsBlockingEvent( true, false ),
		m_autoDelete(autoDelete)
	{
	}

	Thread::~Thread()
	{
		if( m_hThread )
			::CloseHandle( m_hThread );
	}

	unsigned WINAPI Thread::ThreadProc( LPVOID a_pParameter )
	{
		Thread* l_pThread =
			reinterpret_cast<Thread*>( a_pParameter );

		if( l_pThread )
		{
			l_pThread->PreInitialize();
			if( l_pThread->Initialize() )
			{
				try
				{
					l_pThread->PreRun();
					l_pThread->Run();
					l_pThread->PostRun();
				}
				catch(...){}
			}
			l_pThread->Uninitialize();
			l_pThread->PostUninitialize();
			if( l_pThread->IsAutoDelete() )
				delete l_pThread;
		}

		return 0;
	}

	bool Thread::Start()
	{
		SECURITY_ATTRIBUTES l_SecurityAttributes;
		l_SecurityAttributes.nLength = sizeof( SECURITY_ATTRIBUTES );
		l_SecurityAttributes.lpSecurityDescriptor = NULL;
		l_SecurityAttributes.bInheritHandle = FALSE;

		unsigned ThreadID = 0;
		m_hThread = (void*) 
			::_beginthreadex(
				&l_SecurityAttributes,
				0,
				&ThreadProc,
				(LPVOID)this,
				0,
				&ThreadID
			);

		if( m_hThread )
		{
			m_ThreadID = ThreadID;
			return true;
		}
		else
			return false;
	}

	bool Thread::Stop()
	{
		// Stop a blocking thread
		Unblock();

		return true;
	}

	bool Thread::Suspend()
	{
		// Suspend a threads execution
		DWORD r_Value =
			::SuspendThread( m_hThread );

		if( r_Value == 0xFFFFFFFF )
			return false;
		else
			return true;
	}

	bool Thread::Resume()
	{
		// Resume a threads execution
		DWORD r_Value =
			::ResumeThread( m_hThread );

		if( r_Value == 0xFFFFFFFF )
			return false;
		else
			return true;
	}

	bool Thread::Wait( const long a_Timeout )
	{
		// Wait for a thread to  be cancelled ( must be called from outside thread )
		if( ::WaitForSingleObject( m_hThread, a_Timeout ) == WAIT_OBJECT_0 )
			return true;
		else
			return false;
	}

	void Thread::Kill()
	{
		::TerminateThread( m_hThread, 0 );
	}

	bool Thread::IsRunning( const long a_Timeout )
	{
		bool l_bIsRunning;
		try
		{
			l_bIsRunning = m_IsRunningEvent.Wait( a_Timeout );
		}
		catch( Event::Exception )
		{
			l_bIsRunning = false;
		}
		return l_bIsRunning;
	}

	bool Thread::IsBlocking( const long a_Timeout )
	{
		bool l_bIsBlocking;
		try
		{
			l_bIsBlocking = m_IsBlockingEvent.Wait( a_Timeout );
		}
		catch( Event::Exception )
		{
			l_bIsBlocking = false;
		}
		return l_bIsBlocking;
	}

	class BlockResource
	{
		public:
			BlockResource( Thread::BlockingCall* a_pNewCall, Thread::BlockingCall*& a_pThreadsCall, Event& a_ThreadsEvent )
				: m_pThreadsCall(a_pThreadsCall), m_ThreadsEvent(a_ThreadsEvent)
			{
				m_pThreadsCall = a_pNewCall;
				m_ThreadsEvent.Set();
			}

			~BlockResource()
			{
				m_pThreadsCall = 0;
				m_ThreadsEvent.Reset();
			}

		private:
			Thread::BlockingCall*& m_pThreadsCall;
			Event& m_ThreadsEvent;
	};
	
	void Thread::Block( BlockingCall* a_pBlockingCall )
	{
		BlockResource  l_Resource( a_pBlockingCall, m_pBlockingCall, m_IsBlockingEvent );
		
		// Begin blocking
		m_pBlockingCall->StartBlock();
	}

	void Thread::Unblock()
	{
		if( !IsBlocking(0) )
			return;

		// Cancel registered blocking call
		if( m_pBlockingCall )
			m_pBlockingCall->StopBlock();
	}

	void Thread::PreInitialize()
	{
	}

	void Thread::PostUninitialize()
	{
	}

	void Thread::PreRun()
	{
		m_IsRunningEvent.Set();
	}

	void Thread::PostRun()
	{
		m_IsRunningEvent.Reset();
	}

	DWORD Thread::GetThreadID()
	{
		return m_ThreadID;
	}
} // namespace MultiThreading
