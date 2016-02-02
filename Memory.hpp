#pragma once

#include <malloc.h>
#include <map>
extern bool g_isQuitting;
#undef new


void TrackAllocation(void* ptr, const char* file, size_t line, size_t size);
void UntrackAllocation(void* ptr);

//void* operator new(size_t size, const char* file, size_t line);
//void operator delete(void* ptr, const char* file, size_t line);
//void* operator new(size_t size);
//void operator delete(void* ptr);
//void* operator new[](size_t size, const char* file, size_t line);
//void operator delete[](void* ptr);
//void operator delete[](void* ptr, const char* file, size_t line);

struct ByteTrackerData {
	size_t m_bytesAllocated = 0;
	size_t m_lineNum = 0;
	const char* m_file = nullptr;
};

struct OverallAllocationData {
	int m_totalAllocations = 0;
	size_t m_totalBytesAllocated = 0;
	size_t m_largestAllocation = 0;
	size_t m_averageAllocation = 0;
};

extern std::map<size_t, ByteTrackerData> g_allocationTracker;
extern OverallAllocationData g_overallAllocationData;


