#pragma once
#include "JobManager.hpp"
#include <vector>

class HashBufferJob : public Job
{
public:
	HashBufferJob(std::vector<unsigned char> buffer, size_t* hash);
	~HashBufferJob();

	virtual void Run();

public:
	std::vector<unsigned char> m_buffer;
	size_t* m_hashValue;

private:
	HashBufferJob& operator=(const HashBufferJob &tmp);
};

