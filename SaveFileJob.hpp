#pragma once
#include "JobManager.hpp"
class SaveFileJob : public Job
{
public:
	SaveFileJob(unsigned char* buffer, size_t bufferSize, const std::string& filePath);
	~SaveFileJob();

	virtual void Run();

	unsigned char* m_buffer;
	size_t m_bufferSize;
	std::string m_filePath;
};

