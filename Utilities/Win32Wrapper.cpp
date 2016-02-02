#include "Engine/Utilities/Win32Wrapper.hpp"
#include <windows.h>


Win32Wrapper::Win32Wrapper(void)
{
}


Win32Wrapper::~Win32Wrapper(void)
{
}

void Win32Wrapper::MyGetCursorPos(Vec2i& lpPoint) {
	POINT resultPoint;
	GetCursorPos(&resultPoint);
	lpPoint.x() = resultPoint.x;
	lpPoint.y() = resultPoint.y;
	
}

double InitializeTimeSystem( LARGE_INTEGER& out_initialTime )
{
	LARGE_INTEGER countsPerSecond;
	QueryPerformanceFrequency( &countsPerSecond );
	QueryPerformanceCounter( &out_initialTime );
	return( 1.0 / static_cast< double >( countsPerSecond.QuadPart ) );
}

//---------------------------------------------------------------------------
double Win32Wrapper::GetAbsoluteTimeSeconds()
{
	static LARGE_INTEGER initialTime;
	static double secondsPerCount = InitializeTimeSystem( initialTime );
	LARGE_INTEGER currentCount;
	QueryPerformanceCounter( &currentCount );
	LONGLONG elapsedCountsSinceInitialTime = currentCount.QuadPart - initialTime.QuadPart;

	double currentSeconds = static_cast< double >( elapsedCountsSinceInitialTime ) * secondsPerCount;
	return currentSeconds;
}


void Win32Wrapper::MySetCursorPos(int x, int y) {
	SetCursorPos(x, y);
}