#if !defined I_QUEUE_H
#define I_QUEUE_H

#include <queue>
#include "CriticalSection.h"
#include "Event.h"

namespace MultiThreading
{
	/*
	*	Queue used to transfer object pointers between threads in a process.
	*/
	template< typename TMsgType >
	class Queue : protected std::queue< TMsgType* >
	{
	public:
		Queue() : m_Event( false, false )
		{
		}

		virtual ~Queue()
		{
		}

		operator HANDLE() { return m_Event; }

	public:
		void Push( TMsgType& a_Msg )
		{
			MultiThreading::CriticalSection::Locker l_Locker( m_CriticalSection );

			push( &a_Msg );

			m_Event.Pulse();
		}

		bool Pop( TMsgType*& a_pMsg )
		{
			MultiThreading::CriticalSection::Locker l_Locker( m_CriticalSection );

			if( size() )
			{
				a_pMsg = front();
				pop();
				return true;
			}
			else
				a_pMsg = 0;

			return false;
		}

		const long Size()
		{
			MultiThreading::CriticalSection::Locker l_Locker( m_CriticalSection );

			return size();

			return 0;
		}

		void Wait( long a_Timeout )
		{
			m_Event.Wait( a_Timeout );
		}

	private:
		MultiThreading::CriticalSection m_CriticalSection;
		MultiThreading::Event m_Event;
	};
} // namespace MultiThreading

#endif