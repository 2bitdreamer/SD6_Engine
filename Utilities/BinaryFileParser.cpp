#include "BinaryFileParser.hpp"
#include <windows.h>


BinaryFileParser::BinaryFileParser()
{
}

BinaryFileParser::~BinaryFileParser()
{
	m_byteBuffer = nullptr;
	m_currentReadPosition = nullptr;
}

BinaryFileParser::BinaryFileParser(const std::string& filePath) {
	LoadBufferFromFile(filePath);
}

bool BinaryFileParser::LoadBufferFromFile(const std::string& fileName)
{
	FILE * pFile;
	size_t fileSize;
	size_t numBytesRead;

	fopen_s(&pFile, fileName.c_str(), "rb");
	if (pFile == NULL) {
		return false;
	}

	fseek(pFile, 0, SEEK_END);
	fileSize = ftell(pFile);
	rewind(pFile);

	m_bufferSize = fileSize;
	m_bufferData.resize(fileSize);
	numBytesRead = fread(&m_bufferData.front(), 1, fileSize, pFile);
	m_byteBuffer = &m_bufferData.front();

	m_currentReadPosition = m_byteBuffer;
	if (numBytesRead != fileSize) {
		fclose(pFile);
		return false;
	}

	fclose(pFile);
	return true;
}
