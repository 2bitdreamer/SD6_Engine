#pragma once
#include "JobManager.hpp"
class ReverseBufferJob :
	public Job
{
public:
	ReverseBufferJob(unsigned char*& buffer, size_t bufferSize);

	~ReverseBufferJob();

	virtual void Run();
	unsigned char*& m_buffer;
	size_t m_size;

private:
	void operator=(const ReverseBufferJob &D);
};

