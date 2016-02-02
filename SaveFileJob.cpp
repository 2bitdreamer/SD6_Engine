#include "SaveFileJob.hpp"
#include "Utilities\EngineCommon.hpp"


SaveFileJob::SaveFileJob(unsigned char* buffer, size_t bufferSize, const std::string& filePath) :
m_buffer(buffer),
m_bufferSize(bufferSize),
m_filePath(filePath)
{
}


SaveFileJob::~SaveFileJob()
{
}

void SaveFileJob::Run()
{
	FlushByteBufferToDisk(m_filePath, m_buffer, m_bufferSize);
}
