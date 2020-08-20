#include "Event.h"

namespace MultiThreading
{
	Event::Event( bool a_bManualReset, bool a_bInitialState, const std::string& a_Name )
	{
		m_hEvent = ::CreateEvent(
			NULL,
			a_bManualReset ? TRUE : FALSE,
			a_bInitialState ? TRUE : FALSE,
			a_Name.size() ? a_Name.c_str() : 0
		);
	}

	Event::~Event()
	{
		::CloseHandle( m_hEvent );
	}

	void Event::Pulse()
	{
		::PulseEvent( m_hEvent );
	}

	void Event::Reset()
	{
		::ResetEvent( m_hEvent );
	}

	void Event::Set()
	{
		::SetEvent( m_hEvent );
	}

	bool Event::Wait( int a_Time )
	{
		switch( ::WaitForSingleObject( m_hEvent, a_Time ) )
		{
		case WAIT_OBJECT_0:
			return true;
		case WAIT_TIMEOUT:
			return false;
		case WAIT_FAILED:
			throw FailureException();
		default:
			throw UnknownException();
		};
	}
}