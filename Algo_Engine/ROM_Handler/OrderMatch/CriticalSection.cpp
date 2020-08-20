#include "CriticalSection.h"

namespace MultiThreading
{
	CriticalSection::Locker::Locker( CriticalSection& a_CriticalSection )
	:	m_CriticalSection( a_CriticalSection )
	{
		m_CriticalSection.Lock();
	}

	CriticalSection::Locker::~Locker()
	{
		m_CriticalSection.Unlock();
	}

	CriticalSection::CriticalSection()
	{
		::InitializeCriticalSection( &m_CriticalSection );
	}

	CriticalSection::~CriticalSection()
	{
		::DeleteCriticalSection( &m_CriticalSection );
	}

	void CriticalSection::Lock()
	{
		::EnterCriticalSection( &m_CriticalSection );
	}

	#if (_WIN32_WINNT) >= 0x0400
	bool CriticalSection::TryLock()
	{
		return ::TryEnterCriticalSection( &m_CriticalSection ) == TRUE;
	}
	#endif

	void CriticalSection::Unlock()
	{
		::LeaveCriticalSection( &m_CriticalSection );
	}
} // namespace MultiThreading