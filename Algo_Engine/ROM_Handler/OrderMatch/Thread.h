#if !defined I_THREAD_H
#define I_THREAD_H

#include "Event.h"

namespace MultiThreading
{
	/*
	*	Encapsulation of windows threads.
	*/
	class Thread
	{
		friend class MultiThreadingTestCase;
	public:
		class BlockingCall
		{
		public:
			BlockingCall();
			virtual ~BlockingCall();

		public:
			virtual void StartBlock() = 0;
			virtual void StopBlock() = 0;
		};

	public:
		Thread( bool autoDelete = false );
		virtual ~Thread();
		operator HANDLE() { return m_hThread; }

	public:
		bool Start();
		bool Stop();
		bool Suspend();
		bool Resume();
		bool Wait( const long = 0 );
		void Kill();
		bool IsRunning( const long = 0 );
		bool IsBlocking( const long = 0 );
		bool IsAutoDelete() { return m_autoDelete; }

	protected:
		void Block( BlockingCall* );
		void Unblock();

		DWORD GetThreadID();

	private:
		static unsigned WINAPI ThreadProc( LPVOID a_pParameter );

		void PreInitialize();
		void PostUninitialize();
		void PreRun();
		void PostRun();

		virtual bool Initialize() = 0;
		virtual bool Run() = 0;
		virtual bool Uninitialize() = 0;

	private:
		HANDLE m_hThread;
		DWORD m_ThreadID;
		Event m_IsRunningEvent;
		Event m_IsBlockingEvent;
		bool m_autoDelete;

		BlockingCall* m_pBlockingCall;
	};
} // namespace MultiThreading

#endif