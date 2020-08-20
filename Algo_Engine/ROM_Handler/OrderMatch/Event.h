#if !defined I_EVENT_H
#define I_EVENT_H
#include <windows.h>
#include <string>

namespace MultiThreading
{
	/*
	*	Encapsulation of windows events
	*/
	class Event
	{
	public:
		class Exception { };
		class UnknownException : public Exception {};
		class FailureException : public Exception {};

	public:
		Event( bool a_bManualReset, bool a_bInitialState, const std::string& a_Name = "" );
		virtual ~Event();
		operator HANDLE() { return m_hEvent; }

	public:
		void Pulse();
		void Reset();
		void Set();
		bool Wait( int a_Time );

	private:
		HANDLE m_hEvent;
	};
} // namespace MultiThreading

#endif