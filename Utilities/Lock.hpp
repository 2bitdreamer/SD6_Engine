#pragma once
//#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#if !defined(__LOCK__)
#define __LOCK__

class CriticalSection
{
	private:
		CRITICAL_SECTION cs;

	public: 
		CriticalSection()
		{
			::InitializeCriticalSection( &cs );
		}

		void Enter()
		{
			::EnterCriticalSection( &cs );
		}

		void Exit()
		{
			::LeaveCriticalSection( &cs );
		}

		void lock() {
			Enter();
		}

		void unlock() {
			Exit();
		}
};

#endif