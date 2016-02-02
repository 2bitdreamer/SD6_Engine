#include "Memory.hpp"
#include <windows.h>
#include "Assert.hpp"

/*
std::map<size_t, ByteTrackerData> g_allocationTracker;
OverallAllocationData g_overallAllocationData;

void* operator new(size_t size, const char* file, size_t line){
	void* ptr = malloc(size);
	FATAL_ASSERT(ptr);
	//TrackAllocation(ptr, file, line, size);
	(void)line;
	(void)file;
	return ptr;
}

void* operator new[](size_t size, const char* file, size_t line){
	void* ptr = malloc(size);
	FATAL_ASSERT(ptr);
	(void)line;
	(void)file;
	//TrackAllocation(ptr, file, line, size);
	return ptr;
}

void* operator new(size_t size){
	void* ptr = malloc(size);
	FATAL_ASSERT(ptr);
	return ptr;
}

void* operator new[](size_t size){
	void* ptr = malloc(size);
	FATAL_ASSERT(ptr);
	return ptr;
}

void operator delete(void* ptr, const char* file, size_t line) {
	(void)file;
	(void)line;
	free(ptr);
}

void operator delete[](void* ptr, const char* file, size_t line) {
	(void)file;
	(void)line;
	free(ptr);
}


void operator delete(void* ptr) {
	//if (!g_isQuitting)
		//UntrackAllocation(ptr);
	free(ptr);
}

void operator delete[](void* ptr) {
	//if (!g_isQuitting)
		//UntrackAllocation(ptr);
	free(ptr);
}

void TrackAllocation(void* ptr, const char* file, size_t line, size_t size) {
	(void)ptr;
	(void)file;
	(void)line;
	(void)size;

	/*
	ByteTrackerData btd;
	btd.m_bytesAllocated = size;
	btd.m_file = file;
	btd.m_lineNum = line;
	g_allocationTracker[(size_t)ptr] = btd;

	OverallAllocationData& allData = g_overallAllocationData;
	allData.m_totalBytesAllocated += size;
	allData.m_totalAllocations++;

	if (size > allData.m_largestAllocation)
		allData.m_largestAllocation = size;

	allData.m_averageAllocation = (allData.m_totalBytesAllocated / allData.m_totalAllocations);
}

void UntrackAllocation(void* ptr) {
	if (g_allocationTracker.size() > 0) {
		auto it = g_allocationTracker.find((size_t)ptr);
		if (it != g_allocationTracker.end())
			g_allocationTracker.erase(it);
	}
}
*/


