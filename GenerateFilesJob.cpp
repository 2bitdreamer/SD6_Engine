#include "GenerateFilesJob.hpp"
#include "Utilities\EngineCommon.hpp"


GenerateFilesJob::GenerateFilesJob(int numFiles, size_t fileSize, const std::string& basePath) :
m_basePath(basePath),
m_numFiles(numFiles),
m_fileSize(fileSize)
{
}

void GenerateFilesJob::Run() {
	GenerateFiles(m_numFiles, m_fileSize, m_basePath);
}


GenerateFilesJob::~GenerateFilesJob()
{
}
