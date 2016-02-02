#include "LoadFileJob.hpp"
#include "Utilities\EngineCommon.hpp"


LoadFileJob::LoadFileJob(const std::string& filePath, unsigned char*& out_binaryBuffer) :
m_binaryBuffer(out_binaryBuffer),
m_filePath(filePath)
{
	
}


LoadFileJob::~LoadFileJob()
{
}

void LoadFileJob::Run()
{
	LoadFileToNewBinaryBuffer(m_filePath, m_binaryBuffer, m_dataSize);
}
