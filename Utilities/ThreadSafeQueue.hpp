#pragma once
#if !defined(__THREAD_SAFE_QUEUE__)
#define __THREAD_SAFE_QUEUE__

#include <queue>
#include "Lock.hpp"

template <typename _Tx>
class ThreadSafeQueue 
{
	private:
		mutable CriticalSection m_cs;
		std::queue<_Tx> m_queue;

	public:
		void enqueue( _Tx &item ) 
		{
			m_cs.Enter();
			m_queue.push( item );
			m_cs.Exit();
		}

		bool dequeue( _Tx *out )
		{
			bool success = false;
			m_cs.Enter();
			if (m_queue.size() > 0) {
				*out = m_queue.front();
				m_queue.pop();
				success = true;
			}
			m_cs.Exit();
			return success;
		}

		size_t size() const
		{
			size_t size = 0;
			m_cs.Enter();
			m_queue.size();
			m_cs.Exit();

			return size;
		}
};

#endif