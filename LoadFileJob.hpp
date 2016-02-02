#pragma once
#include "JobManager.hpp"

class LoadFileJob :
	public Job
{
public:
	LoadFileJob(const std::string& filePath, unsigned char*& out_binaryBuffer);
	virtual ~LoadFileJob();

	virtual void Run();

public:
	std::string m_filePath;
	unsigned char* m_binaryBuffer;
	size_t m_dataSize;

private:
	LoadFileJob& operator=(const LoadFileJob &tmp);

};