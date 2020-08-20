#ifndef INCLUDED_RVTHREAD_H
#define INCLUDED_RVTHREAD_H

#include "Thread.h"
#include "CriticalSection.h"
#include "Event.h"
#include "Queue.h"
#include "rvcpp.h"
#include <string>
#include <set>

/*
*	Handler which is notified of new messages intercepted on a threads message queue.
*/
class WinMsgPumpHandler
{
private:
	friend class WinMsgPump;

private:
	virtual void OnMessage( const MSG& );
};

/*
*	BlockingCall that waits for messages to appear on a threads queue.
*/
class WinMsgPump : public MultiThreading::Thread::BlockingCall, protected WinMsgPumpHandler
{
public:
	WinMsgPump( const DWORD a_ThreadID, const HWND a_hWnd, WinMsgPumpHandler* a_pHandler = 0 );
	virtual ~WinMsgPump();

public:
	virtual void StartBlock();
	virtual void StopBlock();

private:
	DWORD m_ThreadID;
	HWND m_hWnd;

	WinMsgPumpHandler* m_pHandler;
};

/*
*	Processes RvDataCallbacks in a seperate thread.  This thread encapsulates all
*	session, data dictionary, and listener management.
*/
class RvThread : public MultiThreading::Thread, private RvDataCallback, protected WinMsgPumpHandler
{
public:
	RvThread( 
		const HINSTANCE a_hInstance,
		const std::string& a_Service, const std::string& a_Network, const std::string& a_Daemon
	);
	virtual ~RvThread();

public:
	RvListener* AddListener( const std::string&, RvDataCallback& );
	bool RemoveListener( const RvListener* );

private:
	static const int RVTHREAD_ADDLISTENER;
	static const int RVTHREAD_REMOVELISTENER;

	typedef std::set< RvListener const* > tRvListeners;

	struct AddListenerMSG
	{
		AddListenerMSG( 
			std::string a_Subject,
			RvDataCallback* a_pCallback,
			RvListener* a_pListener = 0
		);
		~AddListenerMSG();

		std::string m_Subject;
		RvListener* m_pListener;
		RvDataCallback* m_pCallback;
		MultiThreading::Event m_Event;
	};

	struct RemoveListenerMSG
	{
		RemoveListenerMSG(
			const RvListener* a_pListener
		);
		~RemoveListenerMSG();

		const RvListener* m_pListener;
		MultiThreading::Event m_Event;
	};

private:
	void OnAddListener( AddListenerMSG* );
	void OnRemoveListener( RemoveListenerMSG* );
	virtual void OnMessage( const MSG& );

	virtual	void onData( const char*, RvSender*, const RvDatum&, RvListener* );
	
	virtual bool Initialize();
	virtual bool Run();
	virtual bool Uninitialize();

private:
	bool StartRV();
	bool GetDataDictionary();

	static void SetGotDataDictionary( const bool );
	static bool GetGotDataDictionary();

private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	RvSession* m_pSession;
	RvListener* m_pDictionaryListener;

	static bool s_bGotDataDictionary;
	static long s_Instances;
	static MultiThreading::CriticalSection s_CS;

	const std::string m_Service;
	const std::string m_Network;
	const std::string m_Daemon;

	MultiThreading::Queue<AddListenerMSG> m_AddListenerQueue;
	MultiThreading::Queue<RemoveListenerMSG> m_RemoveListenerQueue;
	tRvListeners m_RvListeners;
};

class RvThreadResource
{
public:
	class Exception { };

	RvThreadResource( 
		const HINSTANCE a_hInstance,
		const std::string& a_Service, const std::string& a_Network, const std::string& a_Daemon,
		long a_Timeout = 3000000
	)
	:	m_pThread(0), m_Timeout(a_Timeout)
	{
		m_pThread = new RvThread( a_hInstance, a_Service, a_Network, a_Daemon );
		m_pThread->Start();
		if( !m_pThread->IsBlocking(m_Timeout) )
		{
			Stop();
			throw Exception();
		}
	}

	virtual ~RvThreadResource()
	{
		Stop();
	}

	RvThread* get() const throw()
	{
		return m_pThread;
	}

	RvThread *operator->() const throw()
	{
		return m_pThread;
	}

private:
	void Stop()
	{
		if( m_pThread )
		{
			m_pThread->Stop();
			if( !m_pThread->Wait(m_Timeout) )
				m_pThread->Kill();
			delete m_pThread;
		}
		m_pThread = 0;
	}

	RvThread* m_pThread;
	long m_Timeout;
};

#endif //INCLUDED_RVTHREAD_H