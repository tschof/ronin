#if !defined I_CRITICALSECTION_H
#define I_CRITICALSECTION_H
#include <windows.h>

namespace MultiThreading
{
	/*
	*		Encapsulation of windows critical sections
	*/
	class CriticalSection
	{
	protected:
		class Exception { };

	public:
		/*
		*	Locks a CriticalSection and unlocks when jumping scope
		*/
		class Locker
		{
		public:
			Locker( CriticalSection& );
			virtual ~Locker();

		private:
			CriticalSection& m_CriticalSection;
		};

	public:
		CriticalSection();
		virtual ~CriticalSection();
		operator CRITICAL_SECTION() { return m_CriticalSection; }

	public:
		void Lock();
		#if (_WIN32_WINNT) >= 0x0400
		bool TryLock();
		#endif
		void Unlock();

	private:
		CRITICAL_SECTION m_CriticalSection;
	};
} // namespace MultiThreading

#endif