#pragma once
#include "JobManager.hpp"
class GenerateFilesJob :
	public Job
{
public:
	GenerateFilesJob(int numFiles, size_t fileSize, const std::string& basePath);
	~GenerateFilesJob();
	virtual void Run();

public:
	size_t m_fileSize;
	int m_numFiles;
	std::string m_basePath;
};

