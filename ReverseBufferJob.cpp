#include "ReverseBufferJob.hpp"


ReverseBufferJob::ReverseBufferJob(unsigned char*& buffer, size_t bufferSize) :
m_buffer(buffer),
m_size(bufferSize)
{
}


ReverseBufferJob::~ReverseBufferJob()
{

}

void ReverseBufferJob::Run()
{
	unsigned char* istart = m_buffer, *iend = istart + m_size;
	std::reverse(istart, iend);
}
