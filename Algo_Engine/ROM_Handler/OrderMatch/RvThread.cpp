#pragma warning(disable:4786)
#include "RvThread.h"
#include "rvcpp.h"
#include "rvevm.h"
#include "rvevmwin.h"
#include "tibmsg.h"

const int RvThread::RVTHREAD_ADDLISTENER = WM_USER + 1;
const int RvThread::RVTHREAD_REMOVELISTENER = WM_USER + 2;
bool RvThread::s_bGotDataDictionary = false;
MultiThreading::CriticalSection RvThread::s_CS;
long RvThread::s_Instances = 0;

void WinMsgPumpHandler::OnMessage( const MSG& a_Message )
{
}

WinMsgPump::WinMsgPump( const DWORD a_ThreadID, HWND a_hWnd, WinMsgPumpHandler* a_pHandler )
:	m_ThreadID( a_ThreadID ),
	m_hWnd( a_hWnd ),
	m_pHandler( a_pHandler )
{
	if( !m_pHandler )
		m_pHandler = this;
}

WinMsgPump::~WinMsgPump()
{
}

void WinMsgPump::StartBlock()
{
	MSG l_Message;

	// Block for windows messages on thread
	while( ::GetMessage(&l_Message, NULL, NULL, NULL) )
	{
		m_pHandler->OnMessage( l_Message );

		::TranslateMessage( &l_Message );
		::DispatchMessage( &l_Message );
	};
}

void WinMsgPump::StopBlock()
{
	// Stop the blocking call from processing messages
	::PostThreadMessage( m_ThreadID, WM_QUIT, 0, 0 );
}

RvThread::AddListenerMSG::AddListenerMSG(
	std::string a_Subject,
	RvDataCallback* a_pCallback,
	RvListener* a_pListener
)
:	m_Subject( a_Subject ),
	m_pCallback( a_pCallback ),
	m_pListener( a_pListener ),
	m_Event( TRUE, FALSE )
{
}

RvThread::AddListenerMSG::~AddListenerMSG()
{
}

RvThread::RemoveListenerMSG::RemoveListenerMSG( const RvListener* a_pListener )
:	m_pListener( a_pListener ),
	m_Event( TRUE, FALSE )
{
}

RvThread::RemoveListenerMSG::~RemoveListenerMSG()
{
}

RvThread::RvThread( 
	const HINSTANCE a_hInstance,
	const std::string& a_Service, const std::string& a_Network, const std::string& a_Daemon
)
:	m_hInstance( a_hInstance ),
	m_hWnd( 0 ),
	m_pSession( 0 ),
	m_Service( a_Service ),
	m_Network( a_Network ),
	m_Daemon( a_Daemon ),
	m_pDictionaryListener( 0 )
{
}

RvThread::~RvThread()
{
}

RvListener* RvThread::AddListener( const std::string& a_Subject, RvDataCallback& a_Cb )
{
	if( !GetThreadID() )
		return 0;

	RvListener* r_pValue = 0;

	AddListenerMSG* l_pMSG 
		= new AddListenerMSG( a_Subject, &a_Cb );

	if( GetThreadID() != ::GetCurrentThreadId() )
	{
		m_AddListenerQueue.Push( *l_pMSG );

		// Request received outside thread
		if( ::PostThreadMessage( GetThreadID(), RVTHREAD_ADDLISTENER, 0, 0 ) )
		{
			// Wait for thread to process message before returning
			try
			{
				if( l_pMSG->m_Event.Wait( 1000 ) )
				{
					r_pValue = l_pMSG->m_pListener;
					delete l_pMSG;
				}
			}
			catch( MultiThreading::Event::Exception ) 
			{ 
			}
		}
	}
	else
	{
		// Request recieved from inside thread
		OnAddListener( l_pMSG );
		r_pValue = l_pMSG->m_pListener;
		delete l_pMSG;
	}

	return r_pValue;
}

bool RvThread::RemoveListener( const RvListener* a_pListener )
{
	if( !GetThreadID() )
		return false;
	if( a_pListener == 0 )
		return false;

	bool r_bValue = false;

	RemoveListenerMSG* l_pMSG
		= new RemoveListenerMSG( a_pListener );

	if( GetThreadID() != ::GetCurrentThreadId() )
	{	
		m_RemoveListenerQueue.Push( *l_pMSG );

		// Request received outside thread
		if( ::PostThreadMessage( GetThreadID(), RVTHREAD_REMOVELISTENER, 0, 0 ) )
		{
			// Wait for thread to process message before returning
			try
			{
				if( l_pMSG->m_Event.Wait( 1000 ) )
				{
					r_bValue = l_pMSG->m_pListener != 0;
					delete l_pMSG;
				}
			}
			catch( MultiThreading::Event::Exception ) { }
		}
	}
	else
	{
		// Request recieved from inside thread
		OnRemoveListener( l_pMSG );
		r_bValue = l_pMSG->m_pListener != 0;
		delete l_pMSG;
	}

	return r_bValue;
}

void RvThread::OnAddListener( AddListenerMSG* a_pMSG )
{
	if( !a_pMSG )
		return;

	a_pMSG->m_pListener =
		m_pSession->newListener( a_pMSG->m_Subject.c_str(), a_pMSG->m_pCallback, 0 );
	
	m_RvListeners.insert( a_pMSG->m_pListener );
}

void RvThread::OnRemoveListener( RemoveListenerMSG* a_pMSG )
{
	if( !a_pMSG )
		return;

	tRvListeners::iterator l_iListener =
		m_RvListeners.find( a_pMSG->m_pListener );

	if( l_iListener != m_RvListeners.end() )
	{
		m_RvListeners.erase( a_pMSG->m_pListener );
		delete a_pMSG->m_pListener;
	}
}

bool RvThread::Initialize()
{
	m_hWnd = CreateWindow( "STATIC", 0, 0, 0, 0, 0, 0, 0, 0, m_hInstance, 0 );

	::InterlockedIncrement( &s_Instances );

	if( !m_hInstance && !m_hWnd )
		return false;
	
	if( StartRV() )
	{
		if( GetDataDictionary() )
		{
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

bool RvThread::Run()
{
	WinMsgPump l_BlockingCall( GetThreadID(), m_hWnd, this );
	Block( &l_BlockingCall );
	return true;
}

bool RvThread::Uninitialize()
{
	tRvListeners::iterator l_iListener = m_RvListeners.begin();
	for(;l_iListener != m_RvListeners.end(); l_iListener++ )
	{
		delete *l_iListener;
	}

	{
		AddListenerMSG* l_pMSG;
		while( m_AddListenerQueue.Pop(l_pMSG) )
			delete l_pMSG;
	}
	
	{
		RemoveListenerMSG* l_pMSG;
		while( m_RemoveListenerQueue.Pop(l_pMSG) )
			delete l_pMSG;
	}
	
	if( m_pSession )
		delete m_pSession;

	// If this is the last RvThread, extra deconstruction required
	if( ::InterlockedDecrement( &s_Instances ) == 0 )
	{		
		//TibMsg::SetDataDictionary( (TibMsg_dict *)0 , 0);
		//s_bGotDataDictionary = false;
	}

	::DestroyWindow( m_hWnd );

	return true;
}

bool RvThread::StartRV()
{
	rvevm_Context l_Context;
	rv_Session l_Session;
	rv_Error l_RvError;
	rvevm_Returns l_RvEvmReturns;

	// Create context for processing events
	l_RvEvmReturns = rvevm_MakeWin( 
		&l_Context,
		m_hWnd,
		m_hInstance
	);

	if( l_RvEvmReturns != RVEVM_OK )
		return false;

	// Initialize RV session, must only be accessed by this thread
	if( m_Daemon != "" && m_Daemon != "localhost" )
	{
		l_RvError = rv_InitSession(
			&l_Session,
			l_Context,
			const_cast<char*>(m_Service.c_str()),
			const_cast<char*>(m_Network.c_str()),
			const_cast<char*>(m_Daemon.c_str())
		);
	}
	else
	{
		l_RvError = rv_InitSession(
			&l_Session,
			l_Context,
			const_cast<char*>(m_Service.c_str()),
			const_cast<char*>(m_Network.c_str()),
			0);
	}

	if( l_RvError != RVEVM_OK )
		return false;

	m_pSession = new RvSession( l_Session );
	if( m_pSession->status() != RV_OK )
		return false;
			
	return true;
}

bool RvThread::GetDataDictionary()
{
	MultiThreading::CriticalSection::Locker l_Locker( s_CS );

	if( GetGotDataDictionary() == true )
		return true;

	rv_Error l_RvError;

	m_pDictionaryListener =
		m_pSession->newListener( "RAG.RV.DICT", this, 0 );
	
	// Synchronously retreive data dictionary
	l_RvError = rv_SendWithReply(
		m_pSession->rv_session(),
		"_TIC.REPLY.SASS.DATA.DICTIONARY",
		"RAG.RV.DICT",
		RVMSG_STRING,
		0,
		""
	);

	if( l_RvError != RV_OK )
		return false;

	// Begin processing messages until data dictionary arrives
	MSG l_Message;
	for( int l_Count = 0; l_Count < 100 && !GetGotDataDictionary(); l_Count++ )
	{
		if( ::PeekMessage(&l_Message, NULL, 0, 0, PM_REMOVE) )
        {
            ::TranslateMessage( &l_Message );
            ::DispatchMessage( &l_Message );
        }

		Sleep( 100 );
	}

	return GetGotDataDictionary();
}

void RvThread::onData(
	const char* a_pSubject, 
	RvSender* a_pReplySender, 
	const RvDatum& a_Data, 
	RvListener* a_pInvoker
)
{
	TibMsg l_Message;

	l_Message.UnPack( (char*)a_Data.data() );
	TibMsg::UnPackDataDictionary( &l_Message );

	SetGotDataDictionary( true );

	if( m_pDictionaryListener )
		delete m_pDictionaryListener;
	m_pDictionaryListener = 0;
}

void RvThread::OnMessage( const MSG& a_Message )
{
	// Messages processed by the thread
	switch( a_Message.message )
	{
	case RVTHREAD_ADDLISTENER:
		{
			AddListenerMSG* l_pMSG;
			m_AddListenerQueue.Pop( l_pMSG );
			OnAddListener( l_pMSG );
			l_pMSG->m_Event.Set();
		}
		break;
	case RVTHREAD_REMOVELISTENER:
		{
			RemoveListenerMSG* l_pMSG;
			m_RemoveListenerQueue.Pop( l_pMSG );
			OnRemoveListener( l_pMSG );
			l_pMSG->m_Event.Set();
		}
		break;
	};
}

void RvThread::SetGotDataDictionary( const bool a_bValue )
{
	MultiThreading::CriticalSection::Locker l_Locker( s_CS );
	
	s_bGotDataDictionary = a_bValue;
}

bool RvThread::GetGotDataDictionary()
{
	MultiThreading::CriticalSection::Locker l_Locker( s_CS );
	
	return s_bGotDataDictionary;
}
