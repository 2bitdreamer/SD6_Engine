#pragma once
#include "Engine/Math/raw_vector.hpp"
//#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define WVK_SPACE 0x20
#define WVK_SHIFT 0x10

struct ProfTimer {

	LARGE_INTEGER m_timeStart;
	LARGE_INTEGER m_timeStop;

	void Start(void) {
		QueryPerformanceCounter(&m_timeStart);
	};

	void Stop(void) {
		QueryPerformanceCounter(&m_timeStop);
	};

	ProfTimer::ProfTimer() {
		Start();
	}

	double GetDurationInSecs(void)
	{
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		double duration = (double)(m_timeStop.QuadPart - m_timeStart.QuadPart) / (double)freq.QuadPart;
		return duration;
	}
};

class Win32Wrapper
{
public:
	Win32Wrapper(void);
	~Win32Wrapper(void);
	void MyGetCursorPos(Vec2i& lpPoint);
	void MySetCursorPos(int x, int y);
	void MyQueryPerformanceCounter(long& currentCount);
	double GetAbsoluteTimeSeconds();
};

