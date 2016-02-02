#pragma once
#if !defined(__THREAD_SAFE_QUEUE__)
#define __THREAD_SAFE_QUEUE__

#include <queue>
#include "Lock.hpp"

template <typename _Tx>
class ThreadSafeQueue 
{
	private:
		mutable CriticalSection cs;
		std::queue<_Tx> queue;

	public:
		void enqueue( _Tx &item ) 
		{
			cs.Enter();
			queue.push( item );
			cs.Exit();
		}

		bool dequeue( _Tx *out )
		{
			bool success = false;
			cs.Enter();
			if (queue.size() > 0) {
				*out = queue.front();
				queue.pop();
				success = true;
			}
			cs.Exit();
			return success;
		}

		size_t size() const
		{
			size_t size = 0;
			cs.Enter();
			queue.size();
			cs.Exit();

			return size;
		}
};

#endif