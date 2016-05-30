#pragma once
#include "Engine\Utilities\BinaryBufferParser.hpp"
class BinaryFileParser :
	public BinaryBufferParser
{
public:
	BinaryFileParser();
	BinaryFileParser(const std::string& filePath);
	bool LoadBufferFromFile(const std::string& fileName);
	~BinaryFileParser();

public:
	std::vector< unsigned char> m_bufferData;
};

