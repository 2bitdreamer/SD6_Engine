#pragma once
#if !defined(__MYTHREADS__)
#define __MYTHREADS____
#include <windows.h>
class UDPSocket;
class NetPacketQueue;

typedef void(*thread_entry_cb)(void *arg);

class Thread
{
	friend void ThreadClassEntry(void *arg);
private:
	HANDLE thread_handle;

	// disable copy constructor
	Thread(const Thread& other) = delete;

protected:
	virtual void Run() = 0;

	// Question:  Why make these protected?
	void YieldExecution();
	void Sleep(unsigned int ms);

public:
	Thread() { thread_handle = NULL; }

	bool Start(char const *name);
	void Detach();
	void Join();
};

HANDLE ThreadCreate(char const *name, thread_entry_cb entry, void *arg);

void ThreadDetach(HANDLE handle);
void ThreadJoin(HANDLE handle);

void ThreadYield();
void ThreadSleep(unsigned int ms);

DWORD ThreadGetCurrentID();
DWORD ThreadGetID(HANDLE handle);

#endif