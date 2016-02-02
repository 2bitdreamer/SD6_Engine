#include "HashBufferJob.hpp"
#include "Utilities\EngineCommon.hpp"


HashBufferJob::HashBufferJob(std::vector<unsigned char> buffer, size_t* hash) :
m_buffer(buffer),
m_hashValue(hash)
{
}


void HashBufferJob::Run()
{
	*m_hashValue = HashBytes(m_buffer);
}

HashBufferJob::~HashBufferJob()
{
}
