#ifndef LimitSingleInstance_H
#define LimitSingleInstance_H
#pragma once

#include <windows.h> 

class LimitSingleInstance
{
public:
	DWORD  m_dwLastError;
	HANDLE m_hMutex;

public:
	LimitSingleInstance(TCHAR *strMutexName)
	{
		m_hMutex = CreateMutex(NULL, FALSE, strMutexName);
		m_dwLastError = GetLastError();
	}

	~LimitSingleInstance()
	{
		if (m_hMutex)
		{
			CloseHandle(m_hMutex);
			m_hMutex = NULL;
		}
	}

	BOOL IsAnotherInstanceRunning()
	{
		return (ERROR_ALREADY_EXISTS == m_dwLastError);
	}
};
#endif